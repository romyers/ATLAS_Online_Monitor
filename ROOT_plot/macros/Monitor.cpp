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

	}

	for(Event &e : eventBuffer) {

		processEvent(e);
		processedEvents.push_back(e);

	}
	eventBuffer.clear();

}

bool isEvent(const vector<Signal> &signals) {

	if(signals.empty()) return false;

	return signals.back().isEventTrailer();

}