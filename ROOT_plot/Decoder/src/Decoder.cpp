#include "Decoder.h"

#include "EventDecoding.h"
#include "SignalDecoding.h"

// TODO: Break this dependency
#include "Logging/ErrorLogger.h"

using namespace std;
using namespace Muon;

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

DecodeData Decoder::decodeStream(istream &in) {

	vector<Event> eventBuffer;

	DecodeData result;

	int signalCount = 0;

	// As long as unread signals exist in the input stream,
	while(hasSignals(in)) {

		// Abort the loop if we have at least one finished event, but have too
		// many signals. Limits memory footprint for applications where all 
		// processed events do not need to be preserved.
		if(!eventBuffer.empty() && signalCount > maxSignalCount) {

			break;

		}

		// extract a signal,
		Signal sig = extractSignal(in);
		++signalCount;

		// validate it
		if(validateSignalErrors(sig)) {

			// TODO: We should keep some metadata if it's a TDC header or
			//       trailer. See DecodeOffline.cpp

			signalBuffer.push_back(sig);

		} else {

			cerr << "Dropped signal" << endl;

			++result.droppedSignals;

		}

		validateSignalWarnings(sig);

		// and, if it completes an event,
		if(isEvent(signalBuffer)) {

			// make the event
			Event e = assembleEvent(signalBuffer);

			// and validate the event.
			if(validateEventErrors(e)) {

				eventBuffer.push_back(e);

			} else {

				cerr << "Dropped event" << endl;

				++result.droppedEvents;

			}

			signalBuffer.clear();

			validateEventWarnings(e);

		}

	}

	// Once all signals have been extracted,

	// count the events,
	result.eventCount = eventBuffer.size();

	// validate the buffer,
	eventBufferValidator.validate(eventBuffer);

	// and for each event
	for(Event &e : eventBuffer) {

		// if it's nonempty
		if(e.Trailer().HitCount() != 0) {

			// process it
			processEvent(e);

			// and store it in the return vector.
			result.nonemptyEvents.push_back(e);

		}

	}

	// Finally, return all processed events.
	return result;

}

bool isEvent(const vector<Signal> &signals) {

	if(signals.empty()) return false;

	// As long as the data is well-formed, any signal vector that ends in an
	// event trailer represents an event.
	return signals.back().isEventTrailer();

}

bool hasNewData(istream &in) {

	return hasSignals(in);

}