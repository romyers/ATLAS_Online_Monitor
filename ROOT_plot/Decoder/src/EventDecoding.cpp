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

using namespace std;
using namespace MuonReco;

const string EVENT_ERROR = "event" ;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: We also want some metadata, as described in DecodeOffline
Event assembleEvent(const vector<Signal> &signals) {

	return Event(
		signals.front(), 
		signals.back(),
		vector<Signal>(
			signals.begin() + 1, 
			signals.end  () - 1
		),
		signals.back().TrailerEID()
	);

}

void processEvent(
	Event          &e       , 
	Geometry       &geo     , 
	TimeCorrection &tc      , 
	RecoUtility    &recoUtil
) {

    int status = 0;

	recoUtil.DoHitFinding(&e, &tc, geo);
    recoUtil.DoHitClustering(&e);

	bool pass_event_check = recoUtil.CheckEvent(e, &status);
	e.SetPassCheck(pass_event_check);
	e.CheckClusterTime();

}

// Validation that, when failed, drops the event
bool validateEventErrors(const vector<Signal> &signals) {

	ErrorLogger &logger = ErrorLogger::getInstance();

    // TODO: This is a logic error not a runtime error, so we should
    //       really handle it separately.
    if(signals.size() < 2) {

        logger.logError(
            "Found event with fewer than 2 signals",
            EVENT_ERROR,
            ERROR
        );

        return false;
    
    }

    const Signal &header = signals.front();
    const Signal &trailer = signals.back();

	if(!header.isEventHeader()) {

		logger.logError(
			"Found event with no header",
			EVENT_ERROR,
			ERROR
		);

		return false;

	}

	if(!trailer.isEventTrailer()) {

		logger.logError(
			"Found event with no trailer",
			EVENT_ERROR,
			ERROR
		);

		return false;

	}

	// Make sure the header and trailer event IDs match
	if(header.HeaderEID() != trailer.TrailerEID()) {

		logger.logError(
			"Found event with mismatched event IDs",
			EVENT_ERROR,
			ERROR
		);

	}

	// Make sure there aren't any extraneous event headers or trailers
    // hiding in the interior of the signals buffer
    for(auto it = signals.cbegin() + 1; it != signals.cend() - 1; ++it) {

        if(it->isEventHeader()) {

            logger.logError(
                "Found event with multiple headers",
                EVENT_ERROR,
                ERROR
            );

            return false;

        }

        if(it->isEventTrailer()) {

            logger.logError(
                "Found event with multiple trailers",
                EVENT_ERROR,
                ERROR
            );

            return false;

        }

    }

	return true;

}

// Validation that, when failed, keeps the event but warns the user.
void validateEventWarnings(const vector<Signal> &signals) {

	ErrorLogger &logger = ErrorLogger::getInstance();

    // TODO: Again, this is a logic error not a runtime error
    if(signals.size() < 2) {

        logger.logError(
            "Found event with fewer than 2 signals",
            EVENT_ERROR,
            WARNING
        );

    }

    const Signal &header = signals.front();
    const Signal &trailer = signals.back();

	// Check the event trailer for the header count error flag
	if(trailer.HeaderCountErr()) {

		logger.logError(
			string("Header count error flag. Got ")
			+ to_string(trailer.TDCHdrCount())
			+ " header(s)!",
			EVENT_ERROR,
			WARNING
		);

	}

	// Check the event trailer for the trailer count error flag
	if(trailer.TrailerCountErr()) {

		logger.logError(
			string("Trailer count error flag. Got ")
			+ to_string(trailer.TDCTlrCount())
			+ " trailer(s)!",
			EVENT_ERROR,
			WARNING
		);

	}

	// Check that all TDC headers have matching event IDs
	int lastTDCHeaderID = -1;
	for(const Signal &sig : signals) {

		if(sig.isTDCHeader()) {

			if(lastTDCHeaderID != -1) {

				if(sig.TDCHeaderEID() != lastTDCHeaderID) {

					logger.logError(
						string("TDC ")
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
    int hitCount        = 0;

	// Event::Signals() now filters out TDC headers/trailers to ensure
	// backwards compatibility with other code that expects these signals not
	// to be included in the first place. Event::Signals_All() includes these
	// signals.

	// Count all TDC headers, trailers, and errors in the interior of the
    // buffer.
	for(auto it = signals.cbegin() + 1; it != signals.cend() - 1; ++it) {

		if(it->isTDCHeader()) {

            ++TDCHeaderCount;

        } else if(it->isTDCTrailer()) {

            ++TDCTrailerCount;

        } else if(!(it->isTDCOverflow() || it->isTDCDecodeErr())) {

            ++hitCount;

        }

	}

    // TODO: Do TDC headers/trailers count for the hit count?
	// Check that the real hit count matches the count reported in the 
	// event trailer, minus the event header and trailer
	if(trailer.HitCount() != hitCount) {

        // TODO: We need to count TDC errors that were dropped from the event

		logger.logError(
			string("Hit count in trailer = ")
			+ to_string(trailer.HitCount())
			+ ", real hit count = "
			+ to_string(signals.size()),
			EVENT_ERROR,
			WARNING
		);

	}

	// Check that the real TDC header count matches the count reported in the
	// event trailer
	if(TDCHeaderCount != trailer.TDCHdrCount()) {

		logger.logError(
			to_string(TDCHeaderCount)
			+ " TDC header(s) found in data, event trailer indicates "
			+ to_string(trailer.TDCHdrCount())
			+ "!",
			EVENT_ERROR,
			WARNING
		);

	}

	// Check that the real TDC trailer count matches the count reported in the
	// event trailer
	if(TDCTrailerCount != trailer.TDCTlrCount()) {

		logger.logError(
			to_string(TDCTrailerCount)
			+ " TDC trailer(s) found in data, event trailer indicates "
			+ to_string(trailer.TDCTlrCount())
			+ "!",
			EVENT_ERROR,
			WARNING
		);

	}

}

void removeTDCSignals(
    std::vector<MuonReco::Signal> &signals
) {

    signals.erase(
        std::remove_if(
            signals.begin(), 
            signals.end(),
            [](const Signal &signal) {
                if(signal.isTDCHeader   ()) { return true; }
                if(signal.isTDCTrailer  ()) { return true; }
                if(signal.isTDCOverflow ()) { return true; }
                if(signal.isTDCDecodeErr()) { return true; }
                return false;
            }
        ),
        signals.end()
    );

}