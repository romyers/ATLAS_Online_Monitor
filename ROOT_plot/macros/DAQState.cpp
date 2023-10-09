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

using namespace std;

namespace Muon {
namespace State {

	// TODO: Optional data persistence so e.g. it will remember the last network
	//       device that was selected

	// TODO: Operations on DAQState that a gui element can connect to.

	// DAQState::dataSource values
	const int DAT_FILE_SOURCE       = 1;
	const int NETWORK_DEVICE_SOURCE = 2;

	const string EMPTY = "";

	// NOTE: Lock the DAQState before reading/writing data, and unlock after. 
	//       Don't lock anything else while the stateLock is locked 
	//       unless you're familiar with and know how to avoid deadlocks.
	//       Use a pattern like:
	//
	//           DAQState &state = DAQState::getState();
	//
	//           state.lock();
	//           int dataSource = DAQState.dataSource;
	//           state.unlock();
	//
	//           // Do something with dataSource
	//
	//       If you're sure a state member will never be read and written
	//       by two threads at once, you can skip locking it. But be careful.

	struct DAQState {

		int    dataSource      = NETWORK_DEVICE_SOURCE;

		string inputFilename   = EMPTY                ;
		string inputDevicename = EMPTY                ;

		// Locks and unlocks an internal mutex.
		void lock  ();
		void unlock();

		DAQState      (      DAQState &other) = delete;
		void operator=(const DAQState &other) = delete;

		static DAQState &getState();

	private:

		DAQState();

		mutex stateLock;

	};

	DAQState::DAQState() {}

	DAQState &DAQState::getState() {

		// TODO: Thread safety? Do we need to protect this declaration with a
		//       mutex?
		static DAQState state; 

		return state;

	}

	void DAQState::lock() {

		stateLock.lock();

	}

	void DAQState::unlock() {

		stateLock.unlock();

	}

}
}