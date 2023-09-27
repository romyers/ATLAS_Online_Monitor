/**
 * @file Monitor.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <queue>

#include "macros/ParseSignal.cpp"
#include "macros/Display.cpp"
#include "macros/FileAppendMonitor/FileAppendMonitor.cpp"
#include "macros/Event.cpp"
#include "macros/EventBuffer.cpp"

#include "src/Signal.cpp"

#include <unistd.h>

using namespace std;

const uint8_t WORD_SIZE = 5;
const double  POLL_INTERVAL = 0.1;

vector<Signal> readSignals  (istream &dataStream);
void           processEvents(EventBuffer &events);

void Monitor(const string &filename) {

	ifstream          dataStream  (filename);
	FileAppendMonitor inputWatcher(filename, true);
	EventBuffer       events;

	while(true) {

		if(inputWatcher.hasAppend()) {

			// TODO: Encapsulate everything up to processEvents within the
			//       Decode module

			dataStream.clear();

			vector<Signal> signals = readSignals(dataStream);

			for(const Signal &signal : signals) {

				events.registerSignal(signal);

			}

			processEvents(events);

			inputWatcher.clearAppend();

		} else {
			sleep(POLL_INTERVAL);
		}

	}

}

vector<Signal> readSignals(istream &dataStream) {


	char readBuffer[WORD_SIZE];
	vector<Signal> signals;

	// CAUTION: This will not handle appends of partial words well.
	//          We need to make sure the next word is written entirely
	//          before we try to read it. Either lock the file during
	//          write in ethernetCap.cpp or check that there are 5
	//          chars left to read before reading.
	while(dataStream.read(readBuffer, WORD_SIZE)) {

		signals.push_back(ParseSignal(readBuffer, WORD_SIZE));

	}

	return signals;

}

void processEvents(EventBuffer &events) {

	while(!events.empty() && !events.front().isOpen()) {

		Event e = events.pop();

		e.debug_print();

		// TODO: Extract integrated data from e

	}

}