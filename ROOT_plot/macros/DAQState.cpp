#include "DAQState.h"

#include <algorithm>
#include <map>
#include <fstream>

using namespace std;
using namespace Muon;
using namespace State;



string trim(const string &str);

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

bool DAQState::load(const string &filename) {

	ifstream stateSource(filename);
	if(!stateSource.is_open()) {

		return false;

	}

	if(!readPersistentState(stateSource)) {

		stateSource.close();
		return false;

	}

	stateSource.close();
	return true;

}

bool DAQState::save(const string &filename) {

	ofstream stateDest(filename);
	if(!stateDest.is_open()) {

		return false;

	}

	writePersistentState(stateDest);

	stateDest.close();

	return true;

}


void DAQState::writePersistentState(ostream &out) {

	out << "Data Source: "           << persistentState.dataSource         << endl;
	out << "Input Device Name: "     << persistentState.inputDevicename    << endl;
	out << "Input File Name: "       << persistentState.inputFilename      << endl;
	out << "Warn Slow Frames: "      << persistentState.warnSlowFrames     << endl;
	out << "Slow Frame Tolerance: "  << persistentState.slowFrameTolerance << endl;
	out << "Slow Frame Decay Rate: " << persistentState.slowFrameDecayRate << endl;

}

bool DAQState::readPersistentState(istream &in) {

	map<string, string> tokens;

	string key;
	while(getline(in, key, ':')) {

		string value;
		getline(in, value);

		key   = trim(key  );
		value = trim(value);

		tokens[key] = value;

	}

	if(tokens["Data Source"] == "") {

		persistentState.dataSource = 0;

	} else {

		persistentState.dataSource = stoi(tokens["Data Source"]);

	}

	if(tokens["Warn Slow Frames"] == "") {

		persistentState.warnSlowFrames = true;

	} else {

		persistentState.warnSlowFrames = stoi(tokens["Warn Slow Frames"]);

	}

	if(tokens["Slow Frame Tolerance"] == "") {

		persistentState.slowFrameTolerance = 5;

	} else {

		persistentState.slowFrameTolerance = stoi(tokens["Slow Frame Tolerance"]);

	}

	if(tokens["Slow Frame Decay Rate"] == "") {

		persistentState.slowFrameDecayRate = 0.2;

	} else {

		persistentState.slowFrameDecayRate = stod(tokens["Slow Frame Decay Rate"]);

	}

	persistentState.inputDevicename = tokens["Input Device Name"];
	persistentState.inputFilename   = tokens["Input File Name"  ];

	return true;

}

DAQState DAQState::masterState;

mutex DAQState::stateLock;

DAQState::DAQState() {}

bool DAQState::commit(bool force) {

	stateLock.lock();

	if(!force) {

		if(isOutdated()) {

			// TODO: Better to throw an exception here?

			stateLock.unlock();
			return false;

		}

	}

	++commitNumber;
	masterState = *this;

	stateLock.unlock();

	return true;

}

void DAQState::update() {

	stateLock.lock();
	*this = masterState;
	stateLock.unlock();

}

DAQState DAQState::getState() {

	stateLock.lock();
	DAQState state = masterState;
	stateLock.unlock();

	return state;

}

bool DAQState::isOutdated() {

	return commitNumber < masterState.commitNumber;

}

string trim(const string &str) {

	string result = str;

	// Trim left whitespace
	result.erase(result.begin(), find_if(result.begin(), result.end(), [](unsigned char c) {

		return !isspace(c);

	}));

	// Trim right whitespace
	result.erase(find_if(result.rbegin(), result.rend(), [](unsigned char c) {

		return !isspace(c);

	}).base(), result.end());

	return result;

}