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

#include "macros/UIFramework/UIException.cpp"

#include "macros/UIWindows/LogWindow/LogOperations.cpp"

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

		if(state.persistentState.dataSource == State::DAT_FILE_SOURCE) {

			if(state.persistentState.inputFilename == "") {

				throw UIException("Please select a DAT file.");

				return;

			}

			cout << "File source selected" << endl;

		} else if (state.persistentState.dataSource == State::NETWORK_DEVICE_SOURCE) {

			if(state.persistentState.inputDevicename == "") {

				throw UIException("Please select a network device.");

				return;

			}

			cout << "Network source selected" << endl;

		}

		cout << "Ethernet device: " << state.persistentState.inputDevicename << endl;
		cout << "Filename: " << state.persistentState.inputFilename << endl;

		cout << endl << "ERROR: NOT YET IMPLEMENTED" << endl;
		cout << "\tTo start a data run, try running testDecode.sh instead" << endl;

		state.writePersistentState(cout);

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

		if(!state.readPersistentState(stateSource)) {

			stateSource.close();
			return false;

		}

		if(!state.commit()) {

			stateSource.close();
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

		state.writePersistentState(stateDest);

		stateDest.close();

		return true;

	}

}
}