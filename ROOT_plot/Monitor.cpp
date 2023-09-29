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
#include <exception>

#include "macros/SignalReader.cpp"
#include "macros/Display.cpp"
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

// TODO: If we hit the event size cutoff, we need to keep dropping signals
//       until we get to a header

// TODO: Runtime is slower since I moved away from the buffering logic I used in
//       the commit from 9-28-2023

const size_t  EVENT_SIZE_CUTOFF = 1000;
const double  POLL_INTERVAL     = 0.1;

// TODO: This is just a hard-coded computation. 
//       It would be cleaner as a singleton.
const TimeCorrection timeCorrection = TimeCorrection();



void Monitor(const string &filename);

void processEvent(Event &e);

bool validateSignal(const Signal &sig);
bool validateEvent (const Event  &e  );

int getRunNumber();

void debug_print(const Event &e);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: Clean this up -- make Geometry a singleton and just get the instance
//       when we need it

void Monitor(const string &filename) {

	Geometry::getInstance()->SetRunN(getRunNumber());

	SignalReader reader(filename);

	vector<Signal> signalBuffer;
	deque<Event > eventBuffer ;

	while(true) {

		// TODO: I don't like forcing exception handling on ALMOST EVERY loop...
		//       I'd really like to return a bool instead, but I don't like
		//       modifying a signal or signal buffer reference here
		try {

			// TODO: Skip this signal if the signal buffer doesn't start with
			//       an event header

			Signal sig = reader.readSignal();

			if(validateSignal(sig)) {

				signalBuffer.push_back(sig);

			}

			if(signalBuffer.back().Type() == Signal::TRAILER) {

				Event e(signalBuffer.front(), signalBuffer.back(),
					vector<Signal>(
						signalBuffer.begin() + 1, 
						signalBuffer.end() - 1
					)
				);

				if(validateEvent(e)) {

					processEvent(e);

					eventBuffer.push_back(e);

				}

				signalBuffer.clear();

			}

			if(signalBuffer.size() > EVENT_SIZE_CUTOFF) {

				string msg = "WARNING -- Dropping large event -- cutoff is ";
				msg += to_string(EVENT_SIZE_CUTOFF);
				msg += " signals";

				ErrorLogger::getInstance()->logError(msg);

				// TODO: We need to skip to the next header...
				signalBuffer.clear();

			}

		} catch (const ExtractionException &e) {

			sleep(POLL_INTERVAL);

		}

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

	ErrorLogger &logger = *ErrorLogger::getInstance();
	Geometry    &geo    = *Geometry::getInstance   ();

	if(!geo.IsActiveTDC(sig.TDC())) {

		string msg = "ERROR -- Unexpected data TDCID = ";
		msg += to_string(sig.TDC());
		msg += ", Channel = ";
		msg += to_string(sig.Channel());

		logger.logError(msg);

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

		logger.logError(msg);

		return false;

	}

	return true;

}

bool validateEvent(const Event &e) {

	ErrorLogger &logger = *ErrorLogger::getInstance();

	if(e.Header().Type() != Signal::HEADER) {

		logger.logError(
			"WARNING -- Dropping headerless event"
		);

		return false;

	}

	if(e.Trailer().Type() != Signal::TRAILER) {

		logger.logError(
			"WARNING -- Dropping trailerless event"
		);

		return false;

	}

	for(const Signal &sig : e.Signals()) {

		if(sig.Type() == Signal::HEADER) {

			logger.logError(
				"WARNING -- Dropping event with multiple headers"
			);

			return false;

		}

		if(sig.Type() == Signal::TRAILER) {

			logger.logError(
				"WARNING -- Dropping event with multiple trailers"
			);

			return false;

		}

	}

	return true;

}

void processEvent(Event &e) {

	// Ignore empty events
	// TODO: Should empty events go on the event buffer?
	if(e.Trailer().HitCount() == 0) return;

	DoHitFinding(&e, timeCorrection, 0, 0);
	// TODO: No hit clustering?

	e.SetPassCheck(true);
	e.CheckClusterTime();

	debug_print(e);

}


int getRunNumber() {

	return 0;

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
		cout << "\tTDC: " << e.Hits()[i].TDC() << endl;
		cout << "\tChannel: " << e.Hits()[i].Channel() << endl;
		cout << "\tTDCTime: " << e.Hits()[i].TDCTime() << endl;
		cout << "\tADCTime: " << e.Hits()[i].ADCTime() << endl;
		cout << "\tDrift Time: " << e.Hits()[i].DriftTime() << endl;
		cout << "\tCorrected Time: " << e.Hits()[i].CorrTime() << endl;
		cout << endl;

	}

	for(size_t i = 0; i < e.Clusters().size(); ++i) {

		cout << "CLUSTER " << i + 1 << endl;
		cout << "\tSize: " << e.Clusters()[i].Size() << endl;
		cout << endl;

	}

}