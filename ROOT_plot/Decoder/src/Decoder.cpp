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
	istream        &in      , 
	Geometry       &geo     , 
	TimeCorrection &tc      ,
	RecoUtility    &recoUtil
) {

	vector<Event> eventBuffer;

	DecodeData result;

	int signalCount = 0;

	// As long as unread signals exist in the input stream,
	while(hasSignals(in)) {

		// Abort the loop if we have at least one finished event, but have too
		// many signals. Limits memory footprint for applications where all 
		// processed events do not need to be preserved.
		if(maxSignalCount > 0) {
			if(!eventBuffer.empty() && signalCount > maxSignalCount) {
				break;
			}
		}

		// extract a signal,
		Signal sig = extractSignal(in);
		++signalCount;

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
				//       calling assembleEvent()
                eventBuffer.push_back(assembleEvent(signalBuffer));

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
	for(Event &e : eventBuffer) {

		// if it's nonempty
		if(e.TrigSignals().back().HitCount() != 0) {

			// process it
			processEvent(e, geo, tc, recoUtil);

			// and store it in the return vector.
			result.nonemptyEvents.push_back(e);

		}

	}

	// Finally, return all processed events.
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

bool hasNewData(istream &in) {

	return hasSignals(in);

}