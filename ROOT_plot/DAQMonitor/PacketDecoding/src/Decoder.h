/**
 * @file Decoder.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

// TODO: Rename Decoder methods to something semantically more appropriate

#pragma once

#include <vector>

#include "DAQMonitor/PacketDecoding/src/SignalDecoding.cpp"
#include "DAQMonitor/LockableStream.h"

#include "src/Signal.h"
#include "src/Event.h"

struct DecodeData {

	int eventCount     = 0;

	int droppedSignals = 0;
	int droppedEvents  = 0;

	std::vector<Muon::Event> nonemptyEvents;

};

class Decoder {

public:

	Decoder(LockableStream &in);

	bool isStale();
	DecodeData refresh();

private:

	SignalReader reader;

	std::vector<Muon::Signal> signalBuffer;
	std::vector<Muon::Event > eventBuffer ;

};