/**
 * @file EventDecoding.cpp
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

const string EVENT_ERROR = "eventErr" ;
const string EVENT_WARN  = "eventWarn";

Event assembleEvent        (      vector<Signal>  signals);
bool  validateEventErrors  (const Event          &e      );
void  validateEventWarnings(const Event          &e      );
void  processEvent         (      Event          &e      );

// TODO: This does three different things. Break it up.

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: We also want some metadata, as described in DecodeOffline
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

// TODO: Separate errors that we drop the event for and errors that we 
//       report without dropping the event
//         -- validateEventErrors and validateEventWarnings
// TODO: Use exceptions?
bool validateEventErrors(const Event &e) {

	ErrorLogger &logger = ErrorLogger::getInstance();

	if(!e.Header().isEventHeader()) {

		logger.logError(
			"ERROR -- Found event with no header",
			EVENT_ERROR
		);

		return false;

	}

	if(!e.Trailer().isEventTrailer()) {

		logger.logError(
			"ERROR -- Found event with no trailer",
			EVENT_ERROR
		);

		return false;

	}

	if(e.Header().HeaderEID() != e.Trailer().TrailerEID()) {

		logger.logError(
			"ERROR -- Found event with mismatched event IDs",
			EVENT_ERROR
		);

	}

	for(const Signal &sig : e.Signals()) {

		if(sig.isEventHeader()) {

			logger.logError(
				"ERROR -- Found event with multiple headers",
				EVENT_ERROR
			);

			return false;

		}

		if(sig.isEventTrailer()) {

			logger.logError(
				"ERROR -- Found event with multiple trailers",
				EVENT_ERROR
			);

			return false;

		}

	}

	return true;

}

void validateEventWarnings(const Event &e) {



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
	// cout << e << endl;

}