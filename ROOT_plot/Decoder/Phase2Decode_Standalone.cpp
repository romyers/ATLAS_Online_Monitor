/**
 * @file Phase2Decode_Standalone.cpp
 *
 * @brief Provides a standalone application for decoding raw data into events.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

// TODO: Optarg, e.g. "--help"

#include <iostream>
#include <fstream>

#include "src/Decoder.h"

#include "src/Geometry.h"

using namespace std;
using namespace Muon;

void printUsage();

int main(int argc, char **argv) {

	if(argc != 2) {

		cerr << "ERROR: Incorrect argument count." << endl;
		printUsage();

		exit(1);

	}

	fstream in(argv[1]);
	if(!in.is_open()) {

		cerr << "Couldn't open input file: " << argv[1] << endl;

		exit(1);

	}

	// This must happen to properly initialize Geometry
	Geometry::getInstance().SetRunN(0);

	Decoder decoder;

	DecodeData aggregateData;

	cout << "Decoding data from input file: " << argv[1] << endl;

	while(hasNewData(in)) {

		DecodeData loopData = decoder.decodeStream(in);

		aggregateData.eventCount     += loopData.eventCount;
		aggregateData.droppedSignals += loopData.droppedSignals;
		aggregateData.droppedEvents  += loopData.droppedEvents;

		aggregateData.nonemptyEvents.insert(
			aggregateData.nonemptyEvents.end(), 
			loopData.nonemptyEvents.cbegin  (), 
			loopData.nonemptyEvents.cend    ()
		);

	}

	// TODO: Store the events

	cout << "Finished decoding" << endl;

	cout << "Processed " << aggregateData.eventCount            << " events"          << endl;
	cout << "Found "     << aggregateData.nonemptyEvents.size() << " nonempty events" << endl;
	cout << "Dropped "   << aggregateData.droppedSignals        << " signals"         << endl;
	cout << "Dropped "   << aggregateData.droppedEvents         << " events"          << endl;

	return 0;

}

void printUsage() {

	cout << "Usage:" << endl;
	cout << "\tDecode [input file path]" << endl;

}