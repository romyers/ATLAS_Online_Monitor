/**
 * @file DAQState.cpp
 *
 * @brief Global state for the DAQ interface.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
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

	class DAQState {

	public:

		int    dataSource      = NETWORK_DEVICE_SOURCE;

		string inputFilename   = "";
		string inputDevicename = "";

		bool commit(bool force = false);
		void update(); // Sets this state to match the current committed state

		// TODO: Semantically, it should be clear that we're only printing 
		//       persistent state data. Perhaps we have separate classes for
		//       persistent and nonpersistent data united through a common
		//       interface that implements all the control logic.

		friend ostream &operator<<(ostream &out, const DAQState &state);
		friend istream &operator>>(istream &in ,       DAQState &state);

		static DAQState getState();

	private:

		unsigned int commitNumber = 0;

		static DAQState masterState;

		static mutex stateLock;

		DAQState();

		bool isOutdated();

	};

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

	// TODO: Consider a more standard format, e.g. JSON
	ostream &operator<<(ostream &out, const DAQState &state) {

		out << "Data Source: "       << state.dataSource      << endl;
		out << "Input Device Name: " << state.inputDevicename << endl;
		out << "Input File Name: "   << state.inputFilename   << endl;

		return out;

	}

	// NOTE: A templated JSON parser would be nice. It can store data
	//       in a map tree structure.
	//       Consider:
	//       https://kishoreganesh.com/post/writing-a-json-parser-in-cplusplus/
	// TODO: Add a 'serialize' function that turns the DAQState into a string
	istream &operator>>(istream &in, DAQState &state) {

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

			state.dataSource = 0;

		} else {

			state.dataSource = stoi(tokens["Data Source"]);

		}

		state.inputDevicename = tokens["Input Device Name"];
		state.inputFilename = tokens["Input File Name"];

		return in;

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