/**
 * @file EventDecoding.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#include "EventDecoding.h"

#include <string>
#include <algorithm>

#include "Logging/ErrorLogger.h"

#include "src/HitFinder.cpp"
#include "src/TimeCorrection.cpp"

using namespace std;
using namespace Muon;

const string EVENT_ERROR = "event" ;

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

void processEvent(Event &e) {

	DoHitFinding(&e, *TimeCorrection::getInstance(), 0, 0);
	// TODO: No hit clustering?

	e.SetPassCheck(true);
	e.CheckClusterTime();

}

// Validation that, when failed, drops the event
bool validateEventErrors(const Event &e) {

	ErrorLogger &logger = ErrorLogger::getInstance();

	if(!e.Header().isEventHeader()) {

		logger.logError(
			"ERROR -- Found event with no header",
			EVENT_ERROR,
			ERROR
		);

		return false;

	}

	if(!e.Trailer().isEventTrailer()) {

		logger.logError(
			"ERROR -- Found event with no trailer",
			EVENT_ERROR,
			ERROR
		);

		return false;

	}

	// Make sure the header and trailer event IDs match
	if(e.Header().HeaderEID() != e.Trailer().TrailerEID()) {

		logger.logError(
			"ERROR -- Found event with mismatched event IDs",
			EVENT_ERROR,
			ERROR
		);

	}

	// Make sure there aren't any extraneous event headers or trailers
	for(const Signal &sig : e.Signals_All()) {

		if(sig.isEventHeader()) {

			logger.logError(
				"ERROR -- Found event with multiple headers",
				EVENT_ERROR,
				ERROR
			);

			return false;

		}

		if(sig.isEventTrailer()) {

			logger.logError(
				"ERROR -- Found event with multiple trailers",
				EVENT_ERROR,
				ERROR
			);

			return false;

		}

	}

	return true;

}

// Validation that, when failed, keeps the event but warns the user.
void validateEventWarnings(const Event &e) {

	ErrorLogger &logger = ErrorLogger::getInstance();

	// Check the event trailer for the header count error flag
	if(e.Trailer().HeaderCountErr()) {

		logger.logError(
			string("WARNING -- Header count error flag. Got ")
			+ to_string(e.Trailer().TDCHdrCount())
			+ " header(s)!",
			EVENT_ERROR,
			WARNING
		);

	}

	// Check the event trailer for the trailer count error flag
	if(e.Trailer().TrailerCountErr()) {

		logger.logError(
			string("WARNING -- Trailer count error flag. Got ")
			+ to_string(e.Trailer().TDCTlrCount())
			+ " trailer(s)!",
			EVENT_ERROR,
			WARNING
		);

	}

	// Check that all TDC headers have matching event IDs
	int lastTDCHeaderID = -1;
	for(const Signal &sig : e.Signals_All()) {

		if(sig.isTDCHeader()) {

			if(lastTDCHeaderID != -1) {

				if(sig.TDCHeaderEID() != lastTDCHeaderID) {

					logger.logError(
						string("WARNING -- TDC ")
						+ to_string(sig.TDC())
						+ " EventID mismatch! Current = "
						+ to_string(sig.TDCHeaderEID())
						+ ", previous = "
						+ to_string(lastTDCHeaderID),
						EVENT_ERROR,
						WARNING
					);

				}

			}

			lastTDCHeaderID = sig.TDCHeaderEID();

		}

	}

	int TDCHeaderCount  = 0;
	int TDCTrailerCount = 0;
	int TDCErrorCount   = 0;

	// Event::Signals() now filters out TDC headers/trailers/errors to ensure
	// backwards compatibility with other code that expects these signals not
	// to be included in the first place. Event::Signals_All() includes these
	// signals.

	// Count all TDC headers, trailers, and errors in the event.
	for(const Signal &sig : e.Signals_All()) {

		if(sig.isTDCHeader ()) ++TDCHeaderCount ;
		if(sig.isTDCTrailer()) ++TDCTrailerCount;
		if(sig.isTDCError  ()) ++TDCErrorCount  ;

	}

	// Check that the real hit count matches the count reported in the 
	// event trailer
	if(e.Trailer().HitCount() != e.Signals().size() + TDCErrorCount) {

		logger.logError(
			string("WARNING -- Hit count in trailer = ")
			+ to_string(e.Trailer().HitCount())
			+ ", real hit count = "
			+ to_string(e.Signals().size())
			+ ", error word count = "
			+ to_string(TDCErrorCount),
			EVENT_ERROR,
			WARNING
		);

	}

	// Check that the real TDC header count matches the count reported in the
	// event trailer
	if(TDCHeaderCount != e.Trailer().TDCHdrCount()) {

		logger.logError(
			string("WARNING -- ")
			+ to_string(TDCHeaderCount)
			+ " TDC header(s) found in data, event trailer indicates "
			+ to_string(e.Trailer().TDCHdrCount())
			+ "!",
			EVENT_ERROR,
			WARNING
		);

	}

	// Check that the real TDC trailer count matches the count reported in the
	// event trailer
	if(TDCTrailerCount != e.Trailer().TDCTlrCount()) {

		logger.logError(
			string("WARNING -- ")
			+ to_string(TDCTrailerCount)
			+ " TDC trailer(s) found in data, event trailer indicates "
			+ to_string(e.Trailer().TDCTlrCount())
			+ "!",
			EVENT_ERROR,
			WARNING
		);

	}

}