#include "Decoder.h"

#include "DAQMonitor/PacketDecoding/src/EventDecoding.h"

using namespace std;
using namespace Muon;

///////////////////////////////////////////////////////////////////////////////
//////////////////////// HELPER FUNCTIONS /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 * Returns true if signals represents a complete event (i.e. if signals ends
 * with a trailer).
 */
bool isEvent(const vector<Signal> &signals);

///////////////////////////////////////////////////////////////////////////////
/////////////////////// IMPLEMENTATION ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Decoder::Decoder(LockableStream &in) 
	: reader(in) {}

bool Decoder::isStale() {

	return reader.isReady();

}

DecodeData Decoder::refresh() {

	DecodeData result;

	while(isStale()) {

		Signal sig = reader.extractSignal();

		if(validateSignalErrors(sig)) {

			// TODO: We should keep some metadata if it's a TDC header or
			//       trailer. See DecodeOffline.cpp

			signalBuffer.push_back(sig);

		} else {

			cerr << "Dropped signal" << endl;

			++result.droppedSignals;

		}

		validateSignalWarnings(sig);

		if(isEvent(signalBuffer)) {

			Event e = assembleEvent(signalBuffer);

			if(validateEventErrors(e)) {

				eventBuffer.push_back(e);

			} else {

				cerr << "Dropped event" << endl;

				++result.droppedEvents;

			}

			signalBuffer.clear();

			validateEventWarnings(e);

		}

	}

	result.eventCount = eventBuffer.size();

	while(!eventBuffer.empty()) {

		Event &e = eventBuffer.back();

		if(e.Trailer().HitCount() != 0) {

			processEvent(e);

			result.nonemptyEvents.push_back(e);

		}

		eventBuffer.pop_back();

	}

	return result;

}

bool isEvent(const vector<Signal> &signals) {

	if(signals.empty()) return false;

	return signals.back().isEventTrailer();

}