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
#include <deque>
#include <algorithm>

#include "macros/ParseSignal.cpp"
#include "macros/Display.cpp"
#include "macros/FileAppendMonitor/FileAppendMonitor.cpp"
#include "macros/ErrorLogger.cpp"

#include "src/Signal.cpp"
#include "src/Event.cpp"

#include <unistd.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: Catch the erroneous case where we have no header and no trailer.
//       The logic we have now won't catch any issues with the stream until
//       a trailer shows up.

const size_t  EVENT_SIZE_CUTOFF = 1000;
const double  POLL_INTERVAL     = 0.1;

void Monitor(const string &filename);

void extractSignals(deque<Signal> &buffer, istream       &dataStream);
void extractEvents (deque<Event > &buffer, deque<Signal> &signals   );
void processEvents (deque<Event > &events                           );

void dropFirstEvent(deque<Signal> &signals);

bool validateSignalBuffer(const deque<Signal> &signals, deque<Signal>::iterator it);

unsigned long remainingChars(istream &dataStream);

void debug_print(const Event &e);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Monitor(const string &filename) {

	FileAppendMonitor inputWatcher(filename, true);
	ifstream          dataStream  (filename      );

	deque<Signal> signalBuffer;
	deque<Event > eventBuffer ;

	while(true) {

		if(inputWatcher.hasAppend()) {

			inputWatcher.clearAppend();
			dataStream  .clear      ();

			extractSignals(signalBuffer, dataStream  );
			extractEvents (eventBuffer , signalBuffer);

			processEvents(eventBuffer);

		} 

		sleep(POLL_INTERVAL);

	}

}

void extractSignals(deque<Signal> &buffer, istream &dataStream) {

	char readBuffer[Signal::DATA_SIZE];

	while(remainingChars(dataStream) >= Signal::DATA_SIZE) {

		dataStream.read(readBuffer, Signal::DATA_SIZE);

		buffer.push_back(ParseSignal(readBuffer, Signal::DATA_SIZE));

	}

}

void extractEvents(deque<Event> &buffer, deque<Signal> &signals) {

	for(auto iter = signals.begin() + 1; iter != signals.end(); ++iter) {

		if(signals.empty()) return;

		if(!validateSignalBuffer(signals, iter)) {

			dropFirstEvent(signals);

			iter = signals.begin();

		} else if(iter->Type() == Signal::TRAILER) {

			buffer.emplace_back(signals.front(), *iter,
				vector<Signal>(signals.begin() + 1, iter)
			);
			signals.erase(signals.begin(), iter + 1);

			iter = signals.begin();

		}

	}

}

void processEvents(deque<Event> &events) {

	while(!events.empty()) {

		Event e = events.front();
		events.pop_front();

		debug_print(e);

		// TODO: Extract integrated data from e
		//         -- review DecodeOffline and see what data it's getting

	}

}

void dropFirstEvent(deque<Signal> &signals) {

	auto nextHeader = find_if(
		signals.begin() + 1, signals.end(), [](const Signal &arg){
			return arg.Type() == Signal::HEADER;
		}
	);

	signals.erase(signals.begin(), nextHeader);

}

bool validateSignalBuffer(
	const deque<Signal> &signals, 
	deque<Signal>::iterator it
) {

	if(it - signals.begin() > EVENT_SIZE_CUTOFF) {

		string msg = "WARNING -- Dropping large event -- cutoff is ";
		msg += to_string(EVENT_SIZE_CUTOFF);
		msg += " signals";

		ErrorLogger::getInstance()->logError(msg);

		return false;

	}

	if(signals.front().Type() != Signal::HEADER) {

		ErrorLogger::getInstance()->logError(
				"WARNING -- Dropping headerless event"
		);

		return false;

	}

	if(it->Type() == Signal::HEADER) {

		ErrorLogger::getInstance()->logError(
			"WARNING -- Dropping event with multiple headers"
		);

		return false;

	}

	return true;

}

// TODO: Find a faster way to do this. rdbuf->in_avail()?
unsigned long remainingChars(istream &dataStream) {

	streampos pos = dataStream.tellg();
	dataStream.seekg(0, dataStream.end);
	streampos end = dataStream.tellg();

	dataStream.seekg(pos, dataStream.beg);

	return (end - pos);

}

void debug_print(const Event &e) {

	cout << "------------------------------------" << endl;
	cout << "EVENT SIGNAL COUNT: " << e.Signals().size() << endl;
	cout << "PRINTING EVENT:" << endl << endl;

	cout << "EVENT HEADER:" << endl;
	cout << "\tType: " << e.Header().Type() << endl;

	cout << "EVENT TRAILER:" << endl;
	cout << "\tType: " << e.Trailer().Type() << endl;

	for(size_t i = 0; i < e.Signals().size(); ++i) {

		cout << "SIGNAL " << i + 1 << endl;
		cout << "\tType: " << e.Signals()[i].Type() << endl;
		cout << endl;

	}

}