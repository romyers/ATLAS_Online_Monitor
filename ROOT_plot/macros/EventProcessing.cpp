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

#include "macros/ErrorLogger.cpp"

#include "src/Signal.cpp"
#include "src/Event.cpp"
#include "src/HitFinder.cpp"
#include "src/TimeCorrection.cpp"

Event assembleEvent(      vector<Signal>  signals);
bool  validateEvent(const Event          &e      );
void  processEvent (      Event          &e      );

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

	DoHitFinding(&e, *TimeCorrection::getInstance(), 0, 0);
	// TODO: No hit clustering?

	e.SetPassCheck(true);
	e.CheckClusterTime();

	// DEBUG
	cout << e << endl;

}