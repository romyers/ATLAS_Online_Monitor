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
#include <istream>

#include "macros/SignalDecoding.cpp"
#include "macros/EventDecoding.cpp"
#include "macros/ErrorLogger.cpp"
#include "macros/PlotMaker.cpp"
#include "macros/LockableStream.cpp"

#include "src/Signal.cpp"
#include "src/Event.cpp"

///////////////////////////////////////////////////////////////////////////////
//////////////////////// INTERFACE ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: Preserve ability to read from a file source

// TODO: Let the monitor just take in a data stream, and choose between an
//       ifstream and another kind of stream at a higher level.

/**
 * Provides state and functionality for the online monitor.
 */
class Monitor {

public:

	Monitor(LockableStream &in);

	bool isStale();
	void refresh();

private:

	SignalReader reader;

	vector<Signal> signalBuffer   ;
	vector<Event > eventBuffer    ;
	vector<Event > processedEvents;

	PlotMaker plotter;

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

Monitor::Monitor(LockableStream &in) 
	: reader(in) {}

bool Monitor::isStale() {

	return reader.isReady();

}

void Monitor::refresh() {

	while(isStale()) {

		Signal sig = reader.extractSignal();

		if(validateSignalErrors(sig)) {

			signalBuffer.push_back(sig);

		} else {

			cerr << "Dropped signal" << endl;

		}

		validateSignalWarnings(sig);

		if(isEvent(signalBuffer)) {

			Event e = assembleEvent(signalBuffer);

			if(validateEventErrors(e)) {

				eventBuffer.push_back(e);

			} else {

				cerr << "Dropped event" << endl;

			}

			signalBuffer.clear();

			validateEventWarnings(e);

		}

	}

	for(Event &e : eventBuffer) {

		processEvent(e);
		processedEvents.push_back(e);

		plotter.binEvent(e);

		// TODO: Display the event

	}
	eventBuffer.clear();

}

bool isEvent(const vector<Signal> &signals) {

	if(signals.empty()) return false;

	return signals.back().isEventTrailer();

}