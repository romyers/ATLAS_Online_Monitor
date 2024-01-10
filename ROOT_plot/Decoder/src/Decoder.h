/**
 * @file Decoder.h
 *
 * @brief Provides top-level flow for decoding raw data.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <iostream>

#include "EventBufferValidator.h"

#include "src/Signal.h"
#include "src/Event.h"

/**
 * Checks if the input stream contains integral unread data.
 * 
 * @param in The data stream to check.
 * 
 * @return True if the stream contains unread data, and false otherwise.
 */
bool hasNewData(std::istream &in);

/**
 * Struct packaging newly decoded events along with associated metadata.
 */
struct DecodeData {

	/**
	 * The number of events processed while packing this DecodeData.
	 */
	int eventCount     = 0;

	/**
	 * The number of signals that were discarded while packing this DecodeData.
	 */
	int droppedSignals = 0;

	/**
	 * The number of events that were discarded while packing this DecodeData.
	 */
	int droppedEvents  = 0;

	/**
	 * A vector of nonempty events decoded while packing this DecodeData.
	 */
	std::vector<Muon::Event> nonemptyEvents;

};

/**
 * Class providing functionality for decoding a raw data stream.
 */
class Decoder {

public:

	/**
	 * Constructs a Decoder.
	 * 
	 * @param maxSignalCount The maximum number of signals that may be
	 * processed during one call to Decoder::decodeStream. If 
	 * maxSignalCount is 0 or negative, no limit will be imposed, and
	 * Decoder::decodeStream will process all available signals.
	 */
	Decoder(int maxSignalCount);

	/**
	 * Reads all integral unread data from the input stream and packs it into a
	 * DecodeData.
	 * 
	 * @param in The data stream to extract signals from.
	 * 
	 * @return A DecodeData containing the decoded data from this call to
	 * decodeStream().
	 */
	DecodeData decodeStream(std::istream &in);

private:

	int maxSignalCount;

	EventBufferValidator eventBufferValidator;

	std::vector<Muon::Signal> signalBuffer;

};