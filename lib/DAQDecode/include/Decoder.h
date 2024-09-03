/**
 * @file Decoder.h
 * 
 * @brief Entry point for the DAQDecoder library.
 * 
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "MuonReco/Event.h"

#include <vector>
#include <stdint.h>
#include <string>

// TODO: Documentation

// TODO: Pack function from DAQCap should be moved here
// TODO: Removing idle words should be moved here
//         -- We can just make DAQCap rely on this library. Anyone with DAQCap
//            will want to have this library too.

// TODO: How do we set it up so that MuonReco ends up at the top level in the
//       'external' directory in DAQMonitor?

namespace MuonReco {

	class Geometry;
	class TimeCorrection;
	class RecoUtility;

}

namespace DAQDecode {

	/**
	 * @brief The level of a log message.
	 * 
	 * Used to indicate the severity of a log message.
	 */
	enum class LogLevel {

		/**
		 * @brief Informational message.
		 * 
		 * Used to indicate that the message is purely informational and does
		 * not represent an error.
		 */
		INFO,

		/**
		 * @brief Warning message.
		 * 
		 * Used to indicate that the message is a warning, but does not
		 * represent an error that would cause a signal or event to be dropped.
		 */
		WARNING,

		/**
		 * @brief Error message.
		 * 
		 * Indicates that an error occurred that caused a signal or event to be
		 * dropped.
		 */
		ERROR

	};

	/**
	 * @brief A log entry.
	 * 
	 * Stores the content and severity of a log message.
	 */
	struct LogEntry {

		/**
		 * The severity of the log entry.
		 */
		LogLevel level;

		/**
		 * The content of the log entry.
		 */
		std::string message;
		
	};

	/**
	 * @brief The result of a decoding operation.
	 * 
	 * Contains any successfully decoded events, together with the number of
	 * signals or events that had to be ignored and a log of any errors that
	 * occurred.
	 */
	struct DecodeResult {

		/**
		 * @brief The number of signals that were dropped during decoding.
		 * 
		 * When a signal is dropped, it is usually because it does not conform
		 * to the chamber geometry the decoder is expecting, e.g. if the signal
		 * is associated with a TDC or channel that is not marked active in the
		 * geometry configuration file.
		 * 
		 * Signals that were never passed into the decoder in the first place
		 * are not counted.
		 */
		unsigned int droppedSignals = 0;

		/**
		 * @brief The number of events that were dropped during decoding.
		 * 
		 * When an event is dropped, it is usually because the event header
		 * and trailer are not present or mismatched, indicating that the event
		 * is corrupted.
		 * 
		 * Events that are erroneous in some way but were well-formed enough
		 * to construct are not counted. Such events will produce a 'warning'
		 * log entry with a description of the error.
		 */
		unsigned int droppedEvents  = 0;

		/**
		 * @brief The events that were successfully decoded.
		 * 
		 * Contains any event that was not dropped during decoding, including
		 * empty events.
		 */
		std::vector<MuonReco::Event> events;

		/**
		 * @brief A log of messages generated during the decoding process.
		 * 
		 * Log entries usually indicate errors that occurred during decoding.
		 * They will describe what error occurred, and can be used to determine
		 * why a signal or event was dropped.
		 * 
		 * Signals and events that are anomalous but not dropped will produce
		 * a 'warning' log entry describing the issue.
		 */
		std::vector<LogEntry> log;

	};

	/**
	 * @brief Decodes raw data words into signals and events.
	 */
	class Decoder {

	public:

		/**
		 * @brief Constructs a decoder from a geometry.
		 * 
		 * @param configFile A string file path of a configuration file
		 * containing details on the run and chamber geometry.
		 */
		Decoder(const std::string &configFile);
		Decoder();

		~Decoder();

		/**
		 * @brief TODO: Write
		 */
		DecodeResult decode(const std::vector<uint64_t> &words);

		/**
		 * @brief TODO: Write
		 * 
		 * @param[in][out] e The event to process. This event will be modified
         * in-place with the processed data.
		 */
		void processEvent(MuonReco::Event &e) const;

		Decoder(const Decoder&) = delete;
		Decoder& operator=(const Decoder&) = delete;

	private:

		// We'll use this to store decoded signals across decode calls until
		// we've collected enough to form an event.
		std::vector<MuonReco::Signal> signalBuffer;

		// This will store the most recent event ID we've seen, so we can 
		// check it against the next event ID to make sure they're contiguous.
		int latestEventID;

		// MuonReco objects required for validation and processing.
		// We store them as unique pointers so we can make them opaque
		// and avoid exposing them to client code.
		const std::unique_ptr<MuonReco::Geometry> geo;
		const std::unique_ptr<MuonReco::TimeCorrection> tc;
		const std::unique_ptr<MuonReco::RecoUtility> recoUtil;

	};

}