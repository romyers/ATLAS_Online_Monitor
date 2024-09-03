#include "Decoder.h"

#include "MuonReco/ConfigParser.h"
#include "MuonReco/Geometry.h"
#include "MuonReco/TimeCorrection.h"
#include "MuonReco/RecoUtility.h"

using namespace DAQDecode;

using MuonReco::Signal;
using MuonReco::Event;
using MuonReco::TDCErrorData;

using MuonReco::ConfigParser;
using MuonReco::Geometry;
using MuonReco::TimeCorrection;
using MuonReco::RecoUtility;

// TODO: Consider this alternative scheme for handling logging:
//       https://softwareengineering.stackexchange.com/a/350401

// TODO: Consider extracting validators to another file for easier testing

/*
 * Certain signals are good, well-defined signals, but
 * don't belong in an event. This function identifies such signals so that
 * we can remove them from the signal buffer before we pack it into an event.
 */
bool isNotEventSignal(const Signal &signal);

// TODO: This is the only place we need to know about the geometry. Can we
//       get around that dependency?
/*
 * We need to identify bad signals so that we can count them, log their issues,
 * and drop them from the signal buffer. This function returns a string message
 * telling us what errors a signal has. We can also use it to distinguish 
 * between good and bad signals; good signals return an empty string, while bad
 * signals do not.
 */
std::string getSignalErrors(
	const Signal &signal,
	const Geometry &geo
);

/*
 * Some signals are good signals, but are telling us about an issue that we
 * should log. This function translates those signals into user-friendly
 * string messages.
 * 
 * An empty return string means there were no warnings.
 */
std::string getSignalWarnings(const Signal &signal);

/*
 * Once we have a signal buffer representing a complete event, we need to
 * validate it. This function lets us know if 'signals' represents a good
 * event, i.e. the return string is empty. If it doesn't, we can log the 
 * issues in the string and drop the event.
 */
std::string getEventErrors(
	const std::vector<Signal> &signals
);

/*
 * We'll also need to check for any survivable issues with an event. This
 * function identifies such issues so that we can log them.
 * 
 * An empty return string means there were no warnings.
 */
std::string getEventWarnings(
	const std::vector<Signal> &signals
);

Decoder::Decoder() 
	: geo(new Geometry), 
	  tc(new TimeCorrection),
	  recoUtil(new RecoUtility), 
	  latestEventID(-1) {}

Decoder::Decoder(
	const std::string &configFile
) : Decoder() {

	ConfigParser cp(configFile.data());

	geo->Configure(cp.items("Geometry"));

	*tc = TimeCorrection(cp);

	recoUtil->Configure(cp.items("RecoUtility"));

}

Decoder::~Decoder() {}

DecodeResult Decoder::decode(
	const std::vector<uint64_t> &words
) {

	DecodeResult result;

	// We'll turn each word into a signal, validate it, and put it in the
	// signal buffer. If a word completes an event, we'll turn the signal
	// buffer into an event, validate it, and put it in the result.
	for(uint64_t word : words) {

		// Put every signal on the signal buffer. We'll remove bad ones later.
		signalBuffer.emplace_back(Signal(word));
		
		// Log signal errors and drop bad signals
		std::string signalErrors = getSignalErrors(
			signalBuffer.back(),
			*geo
		);
		if(!signalErrors.empty()) {

			signalErrors += "\nDropped signal";

			result.log.push_back({
				LogLevel::ERROR,
				signalErrors
			});
			
			// It's a bad signal -- remove it
			signalBuffer.pop_back();
			result.droppedSignals++;

		}

		// Log signal warnings
		std::string signalWarnings = getSignalWarnings(
			signalBuffer.back()
		);
		if(!signalWarnings.empty()) {

			result.log.push_back({
				LogLevel::WARNING,
				signalWarnings
			});

		}

		// If the signalBuffer contains a complete event, validate it.
		// If it's valid, clean it, pack it into the result, and clear the
		// buffer.
		if(
			signalBuffer.size() > 1 && 
			signalBuffer.back().isEventTrailer()
		) {

			// Grab the errors
			std::string eventErrors = getEventErrors(
				signalBuffer
			);

			// If there are any errors, we'll skip this event and move on to
			// the next word.
			if(!eventErrors.empty()) {

				eventErrors += "\nDropped event";

				result.log.push_back(
					LogLevel::ERROR,
					eventErrors
				);

				++result.droppedEvents;

			} else {

				// Grab the warnings
				std::string eventWarnings = getEventWarnings(
					signalBuffer
				);
				
				if(!eventWarnings.empty()) {

					result.log({
						LogLevel::WARNING,
						eventWarnings
					})

				}

				// Remove valid signals that don't belong in events
				signalBuffer.erase(
					std::remove_if(
						signalBuffer.begin(),
						signalBuffer.end(),
						isNotEventSignal
					),
					signalBuffer.end()
				);

				// Pack the signalBuffer into result as an event
				result.events.emplace_back(
					signalBuffer.front(),
					signalBuffer.back(),
					std::vector<Signal>(
						signalBuffer.begin() + 1,
						signalBuffer.end() - 1
					),
					signalBuffer.back().TrailerEID()
				);

				// TODO: Can we package this into getEventWarnings?
				// Check for missing or repeated events.
				// If latestEventID is -1, then this is the first event, and
				// there's nothing to check.
				if(latestEventID != -1) {

					if(result.events.back().ID() == latestEventID) {

						std::string msg = "Repeated event! Event ID=";
						msg += std::to_string(latestEventID);

						result.log.push_back({
							LogLevel::WARNING,
							msg
						});

					}

					// Event IDs are 12-bit unsigned integers, so they wrap
					// around at 4096.
					if(result.events.back().ID() != (latestEventID + 1) % 4096) {

						std::string msg = "Event lost! Current=";
						msg += std::to_string(result.events.back().ID());
						msg += ", Previous=";
						msg += std::to_string(latestEventID);

						result.log.push_back({
							LogLevel::WARNING,
							msg
						});

					}

				}

			}

			signalBuffer.clear();

		}

	}

	// TODO: Don't forget to remove empty events before we try to process them.
	//       We'll want to remove-erase empties as soon as we get result

	return result;

}

void Decoder::processEvent(Event &e) const {

	int status = 0;

	recoUtil->DoHitFinding(&e, tc.get(), *geo);
	recoUtil->DoHitClustering(&e);

	e.SetPassCheck(
		recoUtil->CheckEvent(e, &status)
	);

	e.CheckClusterTime();

}

bool isNotEventSignal(const Signal &signal) {

	// When the miniDAQ is run in triggered mode, it produces 'TDC signals'
	// that don't belong in events.
	if(signal.isTDCHeader   ()) { return true; }
	if(signal.isTDCTrailer  ()) { return true; }
	if(signal.isTDCOverflow ()) { return true; }
	if(signal.isTDCDecodeErr()) { return true; }

	return false;

}

std::string getSignalErrors(
	const Signal &signal,
	const Geometry &geo
) {

	std::string result;

	// If the signal is a header, trailer, or TDCDecodeErr, then it's a good
	// signal no matter what.
	if(signal.isEventHeader ()) return result;
	if(signal.isEventTrailer()) return result;
	if(signal.isTDCDecodeErr()) return result;

	// If the signal doesn't represent an active TDC, then it's a bad signal.
	// Note that it's important to bounds-check signal.TDC() before we call
	// IsActiveTDC(), since IsActiveTDC() may try an out-of-bounds array
	// access if we don't.
	if(signal.TDC() >= Geometry::MAX_TDC || !geo.IsActiveTDC(signal.TDC())) {

		if(!result.empty()) result += '\n';

		result += "Unexpected data TDCID = ";
		result += std::to_string(signal.TDC());
		result += ", Channel = ";
		result += std::to_string(signal.Channel());

		// We're done now
		return result;

	}

	// In any of these cases, we have a valid signal, but we'll find a channel
	// ID that is not considered active. So let's short-circuit the rest of
	// this function to make sure we don't misclassify them as bad signals
	// when we check the channel ID.
	if(signal.isTDCHeader  ()) return result;
	if(signal.isTDCTrailer ()) return result;
	if(signal.isTDCOverflow()) return result;

	// If the signal doesn't represent an active channel, then it's a bad signal.
	// Note that it's important to bounds-check signal.Channel() before we call
	// IsActiveTDCChannel(), since IsActiveTDCChannel() may try an 
	// out-of-bounds array access if we don't.
	if(
		signal.Channel() >= Geometry::MAX_TDC_CHANNEL || 
		!geo.IsActiveTDCChannel(signal.TDC(), signal.Channel())
	) {

		if(!result.empty()) result += '\n';

		result += "Unexpected data TDCID = ";
		result += std::to_string(signal.TDC());
		result += ", Channel = ";
		result += std::to_string(signal.Channel());

		// We're done now
		return result;

	}

	return result;

}

std::string getSignalWarnings(const Signal &signal) {

	std::string result;
	
	if(signal.isTDCDecodeErr()) { 

		if(!result.empty()) result += '\n';

		result += "Received TDC Error Signal: Decoding error occurred!";

	}

	if(signal.isTDCOverflow()) {

		TDCErrorData errorData = signal.getTDCError();

		if(!result.empty()) result += '\n';

		result = "WARNING -- Received TDC Error Signal:";

		if(errorData.LSBFlag1 > 0) {

			result += "\n         \tTDCID = ";
			result += std::to_string(errorData.TDC);
			result += ", Channel = ";
			result += std::to_string(errorData.LSBChannel1);
			result += " overflowed!";

		} if(errorData.LSBFlag2 > 0) {

			result += "\n         \tTDCID = ";
			result += std::to_string(errorData.TDC);
			result += ", Channel = ";
			result += std::to_string(errorData.LSBChannel2);
			result += " overflowed!";

		} 

	}

	return result;

}

std::string getEventErrors(
	const std::vector<Signal> &signals
) {

	std::string result;

	// Every event must have a header and a trailer, so at least two signals.
	if(signals.size() < 2) {

		if(!result.empty()) result += '\n';

		result += "Found event with fewer than 2 signals";

		return result;
	
	}

	const Signal &header = signals.front();
	const Signal &trailer = signals.back();

	if(!header.isEventHeader()) {

		if(!result.empty()) result += '\n';

		result += "Found event with no header";

		return result;

	}

	if(!trailer.isEventTrailer()) {

		if(!result.empty()) result += '\n';

		result += "Found event with no trailer";

		return result;

	}

	// Make sure the header and trailer event IDs match
	if(header.HeaderEID() != trailer.TrailerEID()) {

		if(!result.empty()) result += '\n';

		result += "Found event with mismatched event IDs";

		return result;

	}

	// Make sure there aren't any extraneous event headers or trailers
	// hiding in the interior of the signals buffer
	for(auto it = signals.cbegin() + 1; it != signals.cend() - 1; ++it) {

		if(it->isEventHeader()) {

			if(!result.empty()) result += '\n';

			result += "Found event with multiple headers";

			return result;

		}

		if(it->isEventTrailer()) {

			if(!result.empty()) result += '\n';

			result += "Found event with multiple trailers";

			return result;

		}

	}

	return result;

}

std::string getEventWarnings(
	const std::vector<Signal> &signals
) {

	std::string result;

	// We do this check again to make sure signals.front() and signals.back()
	// are well-defined.
	if(signals.size() < 2) {

		if(!result.empty()) result += '\n';

		result += "Found event with fewer than 2 signals";

	}

	const Signal &header = signals.front();
	const Signal &trailer = signals.back();

	// Check the event trailer for the header count error flag
	if(trailer.HeaderCountErr()) {

		if(!result.empty()) result += '\n';

		result += "Header count error flag. Got ";
		result += std::to_string(trailer.TDCHdrCount());
		result += " header(s)!";

	}

	// Check the event trailer for the trailer count error flag
	if(trailer.TrailerCountErr()) {

		if(!result.empty()) result += '\n';

		result += "Trailer count error flag. Got ";
		result += std::to_string(trailer.TDCTlrCount());
		result += " trailer(s)!";

	}

	// Check that all TDC headers have matching event IDs
	int lastTDCHeaderID = -1;
	for(const Signal &sig : signals) {

		if(sig.isTDCHeader()) {

			if(lastTDCHeaderID != -1) {

				if(sig.TDCHeaderEID() != lastTDCHeaderID) {

					if(!result.empty()) result += '\n';

					result += "TDC ";
					result += std::to_string(sig.TDC());
					result += " EventID mismatch! Current = ";
					result += std::to_string(sig.TDCHeaderEID());
					result += ", previous = ";
					result += std::to_string(lastTDCHeaderID);

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

	// Check that the real hit count matches the count reported in the 
	// event trailer, minus the event header and trailer
	if(trailer.HitCount() != hitCount) {

		if(!result.empty()) result += '\n';

		result += "Hit count in trailer = ";
		result += std::to_string(trailer.HitCount());
		result += ", real hit count = ";
		result += std::to_string(hitCount);

	}

	// Check that the real TDC header count matches the count reported in the
	// event trailer
	if(TDCHeaderCount != trailer.TDCHdrCount()) {

		if(!result.empty()) result += '\n';

		result += "TDC header count in trailer = ";
		result += std::to_string(trailer.TDCHdrCount());
		result += ", real TDC header count = ";
		result += std::to_string(TDCHeaderCount);

	}

	// Check that the real TDC trailer count matches the count reported in the
	// event trailer
	if(TDCTrailerCount != trailer.TDCTlrCount()) {

		if(!result.empty()) result += '\n';

		result += "TDC trailer count in trailer = ";
		result += std::to_string(trailer.TDCTlrCount());
		result += ", real TDC trailer count = ";
		result += std::to_string(TDCTrailerCount);

	}

	return result;

}