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

#include "MuonReco/Signal.h"
#include "MuonReco/Event.h"
#include "MuonReco/Geometry.h"
#include "MuonReco/TimeCorrection.h"
#include "MuonReco/RecoUtility.h"

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
	std::vector<MuonReco::Event> nonemptyEvents;

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
	Decoder(int maxSignalCount = 0);

	/**
	 * Reads all integral unread data from the input stream, validates it,
	 * processes it, and packs it into a DecodeData object. This should
	 * not be run concurrently with write operations to the geo parameter.
	 * 
	 * @param in The data stream to extract signals from.
	 * 
	 * @param geo The chamber geometry that signals and events should conform
	 * to. Used to validate decoded data and process events.
	 * 
	 * @return A DecodeData containing the decoded data from this call to
	 * decodeStream().
	 */
	DecodeData decodeStream(
		std::istream             &in      , 
		MuonReco::Geometry       &geo     ,
		MuonReco::TimeCorrection &tc      ,
		MuonReco::RecoUtility    &recoUtil
	);

private:

	int maxSignalCount;

	EventBufferValidator eventBufferValidator;

	std::vector<MuonReco::Signal> signalBuffer;

};