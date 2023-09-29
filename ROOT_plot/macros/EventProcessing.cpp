/**
 * @file EventAssembler.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <string>

#include "macros/ErrorLogger.cpp"

#include "src/Signal.cpp"
#include "src/Event.cpp"
#include "src/HitFinder.cpp"
#include "src/TimeCorrection.cpp"

const string EVENT_ERROR = "event";

Event assembleEvent(      vector<Signal>  signals);
bool  validateEvent(const Event          &e      );
void  processEvent (      Event          &e      );

// TODO: This does three different things. Break it up.

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Event assembleEvent(vector<Signal> signals) {

	return Event(
		signals.front(), 
		signals.back(),
		vector<Signal>(
			signals.begin() + 1, 
			signals.end  () - 1
		)
	);

}

bool validateEvent(const Event &e) {

	ErrorLogger &logger = *ErrorLogger::getInstance();

	if(!e.Header().isEventHeader()) {

		logger.logError(
			"WARNING -- Dropping headerless event",
			EVENT_ERROR
		);

		return false;

	}

	if(!e.Trailer().isEventTrailer()) {

		logger.logError(
			"WARNING -- Dropping trailerless event",
			EVENT_ERROR
		);

		return false;

	}

	for(const Signal &sig : e.Signals()) {

		if(sig.isEventHeader()) {

			logger.logError(
				"WARNING -- Dropping event with multiple headers",
				EVENT_ERROR
			);

			return false;

		}

		if(sig.isEventTrailer()) {

			logger.logError(
				"WARNING -- Dropping event with multiple trailers",
				EVENT_ERROR
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

	DoHitFinding(&e, *TimeCorrection::getInstance(), 0, 0);
	// TODO: No hit clustering?

	e.SetPassCheck(true);
	e.CheckClusterTime();

	// DEBUG
	cout << e << endl;

}