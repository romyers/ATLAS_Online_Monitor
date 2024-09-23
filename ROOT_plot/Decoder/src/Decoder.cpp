#include "Decoder.h"

#include "EventDecoding.h"
#include "SignalDecoding.h"

using namespace std;
using namespace MuonReco;

// NOTE: It's really important that the signal buffer is guaranteed to have
//       at least two signals in it during the call to removeTDCSignals() and
//       assembleEvent(). If you see errors relating to vector bounds accesses
//       or vectors overrunning max_size, this is the first thing you should
//       check.

///////////////////////////////////////////////////////////////////////////////
//////////////////////// HELPER FUNCTIONS /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 * Returns true if signals represents a complete event (i.e. if signals ends
 * with a trailer).
 */
bool isEvent(const vector<Signal> &signals);

///////////////////////////////////////////////////////////////////////////////
/////////////////////// IMPLEMENTATION ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Decoder::Decoder(int maxSignalCount) : maxSignalCount(maxSignalCount) {}

DecodeData Decoder::decodeStream(
	vector<unsigned char> &in      , 
	Geometry       &geo     , 
	TimeCorrection &tc      ,
	RecoUtility    &recoUtil
) {

	vector<Event> eventBuffer;
	eventBuffer.reserve(maxSignalCount); // An upper bound to the number of events

	DecodeData result;

	// Total number of whole signals in the data, excluding any trailing
	// partial signal
	int signalCount = in.size() / Signal::WORD_SIZE;

	// Limit the total number of signals read at once.  Limits memory footprint
	// for applications where all 
	// processed events do not need to be preserved.
	if(maxSignalCount > 0) {
		signalCount = min(signalCount, maxSignalCount);
	}
	
	// Iterate across the words in the data
	for(
		unsigned char *wordStart = in.data(); 
		wordStart < in.data() + signalCount * Signal::WORD_SIZE; 
		wordStart += Signal::WORD_SIZE
	) {

		// extract a signal,
		Signal sig = extractSignal(wordStart, Signal::WORD_SIZE);

		// validate it
		if(validateSignalErrors(sig, geo)) {

		    validateSignalWarnings(sig, geo);

			// TODO: We should keep some metadata if it's a TDC header or
			//       trailer. See DecodeOffline.cpp

			signalBuffer.push_back(sig);

		} else {

			cerr << "Dropped signal" << endl;
			++result.droppedSignals;

		}

		// and, if it completes an event,
		if(isEvent(signalBuffer)) {

            if(validateEventErrors(signalBuffer)) {

			    validateEventWarnings(signalBuffer);

                // Weed out TDC signals that we don't want to include in the
                // event
				// NOTE: We MUST be sure that signalBuffer.size() >= 2 before
				//       calling removeTDCSignals()
                removeTDCSignals(signalBuffer);

				// NOTE: We MUST be sure that signalBuffer.size() >= 2 before
				//       trying to access its strict interior.
				eventBuffer.emplace_back(
					signalBuffer.front(),
					signalBuffer.back(),
					vector<Signal>(
						signalBuffer.begin() + 1, // Copy inefficient?
						signalBuffer.end  () - 1
					),
					signalBuffer.back().TrailerEID()
				);

            } else {

                cerr << "Dropped event" << endl;

                ++result.droppedEvents;

            }

			signalBuffer.clear();

		}

	}

	// Once all signals have been extracted,

	// count the events,
	result.eventCount = eventBuffer.size();

	// validate the buffer,
	eventBufferValidator.validateWarnings(eventBuffer);

	// and for each event
	result.nonemptyEvents.reserve(eventBuffer.size());
	for(Event &e : eventBuffer) {

		// if it's nonempty
		if(e.TrigSignals().back().HitCount() != 0) {

			// process it
			processEvent(e, geo, tc, recoUtil);

			// and store it in the return vector.
			result.nonemptyEvents.push_back(e);

		}

	}

	// Finally, clear out the processed signals from the input buffer
	in.erase(in.begin(), in.begin() + signalCount * Signal::WORD_SIZE);
	
	// and return all processed events.
	return result;

}

bool isEvent(const vector<Signal> &signals) {
	
	if(signals.size() > 1) {

		// As long as the data is well-formed, any signal vector that ends in an
		// event trailer represents an event.
		return signals.back().isEventTrailer();

	}

	return false;

}

bool hasNewData(vector<unsigned char> &in) {

	return hasSignals(in);

}