#include "DAQState.h"

#include <algorithm>
#include <map>
#include <fstream>

using namespace std;
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

void DAQState::setSaveFile(const string &filename) {

	saveFile = filename;

}

bool DAQState::save() {

	ofstream stateDest(saveFile);
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
	out << "Conf File Name: "        << persistentState.confFilename       << endl;

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

	persistentState.inputDevicename = tokens["Input Device Name"];
	persistentState.inputFilename   = tokens["Input File Name"  ];
	persistentState.confFilename    = tokens["Conf File Name"   ];

	return true;

}

DAQState DAQState::masterState;

mutex DAQState::stateLock;

DAQState::DAQState() {}

bool DAQState::commit(bool force) {

    std::lock_guard<std::mutex> lock(stateLock);

	if(!force) {

		if(isOutdated()) {

			// TODO: Better to throw an exception here?

			return false;

		}

	}

	++commitNumber;
	masterState = *this;

	return true;

}

void DAQState::update() {

    std::lock_guard<std::mutex> lock(stateLock);

	*this = masterState;

}

DAQState DAQState::getState() {

    std::lock_guard<std::mutex> lock(stateLock);

	DAQState state = masterState;

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