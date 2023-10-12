/**
 * @file DAQState.cpp
 *
 * @brief Global state for the DAQ interface.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 * 
 * To add new temporary state, simply add a member to the TemporaryState
 * struct. To add new persistent state, add a member to the PersistantState 
 * struct, then update DAQState::readPersistentState() and 
 * DAQState::writePersistentState with readers and writers for the new member.
 */

#pragma once

#include <string>
#include <mutex>
#include <iostream>
#include <algorithm>
#include <map>

using namespace std;

namespace Muon {
namespace State {

	string trim(const string &str);

	// TODO: Operations on DAQState that a gui element can connect to.

	// TODO: Split out a POD struct, and let the logic exist in a controller
	//       for that struct

	// DAQState::dataSource values
	const int DAT_FILE_SOURCE       = 1;
	const int NETWORK_DEVICE_SOURCE = 2;

	struct PersistentState {

		int dataSource         = NETWORK_DEVICE_SOURCE;

		string inputFilename   = ""                   ;
		string inputDevicename = ""                   ;

	};

	struct TemporaryState {



	};

	class DAQState {

	public:

		PersistentState persistentState;
		TemporaryState  tempState      ;

		bool commit(bool force = false);
		void update(); // Sets this state to match the current committed state

		bool readPersistentState (istream &in );
		void writePersistentState(ostream &out);

		static DAQState getState();

	private:

		unsigned int commitNumber = 0;

		static DAQState masterState;

		static mutex stateLock;

		DAQState();

		bool isOutdated();

	};

	void DAQState::writePersistentState(ostream &out) {

		out << "Data Source: "       << persistentState.dataSource      << endl;
		out << "Input Device Name: " << persistentState.inputDevicename << endl;
		out << "Input File Name: "   << persistentState.inputFilename   << endl;

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

		return true;

	}

	DAQState DAQState::masterState;

	mutex DAQState::stateLock;

	DAQState::DAQState() {}

	bool DAQState::commit(bool force = false) {

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

}
}