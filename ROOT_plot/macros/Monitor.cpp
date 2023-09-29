/**
 * @file Monitor.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

///////////////////////////////////////////////////////////////////////////////
//////////////////////////// DEPENDENCIES /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <fstream>

#include "macros/SignalProcessing.cpp"
#include "macros/EventProcessing.cpp"
#include "macros/ErrorLogger.cpp"

#include "src/Signal.cpp"
#include "src/Event.cpp"

///////////////////////////////////////////////////////////////////////////////
//////////////////////// INTERFACE ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: All config parameters should be in a centralized place
/**
 * The maximum number of signals that an event 
 * can include without being dropped.
 */	
const size_t  EVENT_SIZE_CUTOFF = 1000;

/**
 * Provides state and functionality for the online monitor.
 */
class Monitor {

public:

	Monitor(const string &filename);

	bool isStale();
	void refresh();

private:

	SignalReader reader;

	vector<Signal> signalBuffer;
	vector<Event > eventBuffer ;

	vector<Event > processedEvents;

};

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

Monitor::Monitor(const string &filename) : reader(filename) {}

bool Monitor::isStale() {

	return reader.isReady();

}

void Monitor::refresh() {

	while(isStale()) {

		Signal sig = reader.extractSignal();

		if(validateSignal(sig)) {

			signalBuffer.push_back(sig);

		}

		if(isEvent(signalBuffer)) {

			Event e = assembleEvent(signalBuffer);

			if(validateEvent(e)) {

				eventBuffer.push_back(e);

			}

			signalBuffer.clear();

		}

		// TODO: Try to move this somewhere else. It clutters my loop.
		// TODO: Do I really need to do this validation? We'll catch it when
		//       we miss trailers because the next trailer won't match the
		//       header
		if(signalBuffer.size() > EVENT_SIZE_CUTOFF) {

			string msg = "WARNING -- Dropping large event -- cutoff is ";
			msg += to_string(EVENT_SIZE_CUTOFF);
			msg += " signals";

			ErrorLogger::getInstance()->logError(msg);

			// TODO: We need to skip to the next header
			//         -- Or just let event validation take care of it.
			//            This will produce two errors though...
			signalBuffer.clear();

		}

	}

	for(Event &e : eventBuffer) {

		processEvent(e);
		processedEvents.push_back(e);

	}
	eventBuffer.clear();

}

bool isEvent(const vector<Signal> &signals) {

	if(signals.empty()) return false;

	return signals.back().Type() == Signal::TRAILER;

}