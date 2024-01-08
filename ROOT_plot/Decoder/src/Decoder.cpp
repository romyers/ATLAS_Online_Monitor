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

bool hasNewData(istream &in) {

	return hasSignals(in);

}

DecodeData Decoder::decodeStream(istream &in) {

	// TODO: I'd like to find a way to handle this inside the signal validation
	//       logic
	// Tracks the count of TDC error signals in each event.
	int errorWords = 0;
	ErrorLogger &logger = ErrorLogger::getInstance();

	vector<Event> eventBuffer;

	DecodeData result;

	// As long as unread signals exist in the input stream,
	while(hasSignals(in)) {

		// extract a signal,
		Signal sig = extractSignal(in);

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

		// If it's a TDC error, increment errorWords
		if(sig.isTDCError()) ++errorWords;

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

			// Validate the event hit count.
			// TODO: I'd really like to put this in validateEventWarnings
			if(e.Trailer().HitCount() != e.Signals().size() + errorWords) {

				logger.logError(
					string("WARNING -- Hit count in trailer = ")
					+ to_string(e.Trailer().HitCount())
					+ ", real hit count = "
					+ to_string(e.Signals().size())
					+ ", error word count = "
					+ to_string(errorWords),
					"event",
					WARNING
				);

			}

			// Reset the error word count
			errorWords = 0;

		}

	}

	// Once all signals have been extracted,

	// count the events,
	result.eventCount = eventBuffer.size();

	// and for each event
	for(Event &e : eventBuffer) {

		// if it's nonempty
		if(e.Trailer().HitCount() != 0) {

			// process it
			processEvent(e);

			// and store it in the return vector.
			result.nonemptyEvents.push_back(e);

		}

		// TODO: Try to move this validation elsewhere -- e.g. validate the 
		//       event buffer in a function. Perhaps make an 'eventBufferValidator'
		//       class that stores the latest event ID in place of the class member.
		// Validate the event ID to check for lost or repeated events.
		if(latestEventID != -1) {

			if(e.ID() == latestEventID) {

				logger.logError(
					string("WARNING -- Repeated event! Event ID=")
					+ to_string(e.ID()),
					"event",
					WARNING
				);

			} else if(e.ID() != (latestEventID + 1) % 4096) {

				logger.logError(
					string("WARNING -- Event lost! Current=")
					+ to_string(e.ID())
					+ ", Previous="
					+ to_string(latestEventID),
					"event",
					WARNING
				);

			}

		}

		latestEventID = e.ID();

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