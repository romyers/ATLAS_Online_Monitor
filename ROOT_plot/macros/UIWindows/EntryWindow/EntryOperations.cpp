/**
 * @file EntryOperations.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "macros/DAQState.cpp"

#include "src/ProgramControl/Terminator.cpp"

#include <fstream>

using namespace std;
using namespace Muon;

// TODO: Consider pairing operations with data model instead of with view. So
//       instead of EntryOperations, we would have DAQState operations.
//         -- Well, startRun would be paired with the view. exitAll would be
//            paired with program control.

namespace Muon {
namespace EntryOperations {

	void startRun() {

		State::DAQState state = State::DAQState::getState();

		cout << "Ethernet device: " << state.inputDevicename << endl;
		cout << "Filename: " << state.inputFilename << endl;

		if(state.dataSource == State::DAT_FILE_SOURCE) {

			cout << "File source selected" << endl;

		} else if (state.dataSource == State::NETWORK_DEVICE_SOURCE) {

			cout << "Network source selected" << endl;

		}

		cout << endl << "ERROR: NOT YET IMPLEMENTED" << endl;
		cout << "\tTo start a data run, try running testDecode.sh instead" << endl;

		cout << state << endl;

	}

	// TODO: Try to find a way to pull this logic out of the device selector
	//       panel and do it here. 
	/*
	void commitSettings(const State::DAQState &state) {

		state.commit();

	}

	void revertSettings(State::DAQState &state) {

		state.update();

	}
	*/

	void exitAll() {

		Terminator::getInstance().terminate();

	}

	bool readState(const string &filename) {

		ifstream stateSource(filename);
		if(!stateSource.is_open()) {

			return false;

		}

		State::DAQState state = State::DAQState::getState();

		stateSource >> state;

		if(!state.commit()) {

			return false;

		}

		stateSource.close();

		return true;

	}

	// TODO: Consider pairing this with DAQState::commit()?
	bool saveState(const string &filename) {

		ofstream stateDest(filename);
		if(!stateDest.is_open()) {

			return false;

		}

		State::DAQState state = State::DAQState::getState();

		stateDest << state;

		stateDest.close();

		return true;

	}

}
}