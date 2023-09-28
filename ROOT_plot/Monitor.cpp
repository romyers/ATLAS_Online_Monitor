/**
 * @file Monitor.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 * 
 * NOTE: This assumes triggerless mode.
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
#include "src/HitFinder.cpp"
#include "src/TimeCorrection.cpp"
#include "src/Geometry.cpp"

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

// TODO: This is just a hard-coded computation. 
//       It would be cleaner as a singleton.
const TimeCorrection timeCorrection = TimeCorrection();



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

// TODO: Clean this up
Geometry geo = Geometry();

void Monitor(const string &filename) {

	geo.SetRunN(0);

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

// TODO: Pull out validators
// TODO: Test validators
// TODO: We don't have event numbers, which we need to have for error reporting
// TODO: Combine this with signal buffer validation. Either this can be done
//       with access to the buffer or buffer validation can be done here.
//       If it happens in buffer validation, we can still keep track of signal
//       count.
bool validateSignal(const Signal &sig) {

	if(sig.Type() == Signal::HEADER)  return true;
	if(sig.Type() == Signal::TRAILER) return true;

	if(!geo.IsActiveTDC(sig.TDC())) {

		string msg = "ERROR -- Unexpected data TDCID = ";
		msg += to_string(sig.TDC());
		msg += ", Channel = ";
		msg += to_string(sig.Channel());

		ErrorLogger::getInstance()->logError(
			msg
		);

		return false;

	}

	if(sig.Channel() == Signal::TDC_HEADER_CHNL) {

		return true;

	}

	if(sig.Channel() == Signal::TDC_TRAILER_CHNL) {

		return true;

	}

	if(sig.Channel() == Signal::TDC_ERROR_CHNL) {

		// TODO: Extract error info

		return false;

	}

	if(sig.Channel() >= Geometry::MAX_TDC_CHANNEL) {

		string msg = "ERROR -- Unexpected data TDCID = ";
		msg += to_string(sig.TDC());
		msg += ", Channel = ";
		msg += to_string(sig.Channel());

		ErrorLogger::getInstance()->logError(
			msg
		);

		return false;

	}

	return true;

}

void extractSignals(deque<Signal> &buffer, istream &dataStream) {

	char readBuffer[Signal::WORD_SIZE];

	while(remainingChars(dataStream) >= Signal::WORD_SIZE) {

		dataStream.read(readBuffer, Signal::WORD_SIZE);

		Signal sig = ParseSignal(readBuffer, Signal::WORD_SIZE);

		if(validateSignal(sig)) {

			buffer.push_back(sig);

		}

		// TODO: Even invalid signals should contribute to the total event signals

	}

}

void extractEvents(deque<Event> &buffer, deque<Signal> &signals) {

	for(auto iter = signals.begin() + 1; iter != signals.end(); ++iter) {

		if(signals.empty()) return;

		// TODO: Put this dropping of invalid events in a separate function and
		//       call it at the top level
		if(!validateSignalBuffer(signals, iter)) {

			dropFirstEvent(signals);

			iter = signals.begin();

			// TODO: Move logic from validateSignalBuffer to event validation
			//       logic that runs after an event has been assembled, matching
			//       what's done for signal validation

		} else if(iter->Type() == Signal::TRAILER) {

			buffer.emplace_back(signals.front(), *iter,
				vector<Signal>(signals.begin() + 1, iter)
			);
			signals.erase(signals.begin(), iter + 1);

			iter = signals.begin();

		}

	}

}

// TODO: Better name, better structure
void handleEvent(Event &e) {

	// Ignore empty events
	if(e.Trailer().HitCount() == 0) return;

	DoHitFinding(&e, timeCorrection, 0, 0);
	// TODO: No hit clustering?

	e.SetPassCheck(true);
	e.CheckClusterTime();

	debug_print(e);

}

void processEvents(deque<Event> &events) {

	while(!events.empty()) {

		Event e = events.front();
		events.pop_front();

		handleEvent(e);

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
	cout << "\tHit Count: " << e.Trailer().HitCount() << endl;

	for(size_t i = 0; i < e.Signals().size(); ++i) {

		cout << "SIGNAL " << i + 1 << endl;
		cout << "\tType: " << e.Signals()[i].Type() << endl;
		cout << "\tTDC: " << e.Signals()[i].TDC() << endl;
		cout << "\tChannel: " << e.Signals()[i].Channel() << endl;
		cout << endl;

	}

	for(size_t i = 0; i < e.Hits().size(); ++i) {

		cout << "HIT " << i + 1 << endl;
		cout << "\tDrift Time: " << e.Hits()[i].DriftTime() << endl;
		cout << endl;

	}

	for(size_t i = 0; i < e.Clusters().size(); ++i) {

		cout << "CLUSTER " << i + 1 << endl;
		cout << "\tSize: " << e.Clusters()[i].Size() << endl;
		cout << endl;

	}

}