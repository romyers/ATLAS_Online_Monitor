/**
 * @file DAQState.h
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

#include <iostream>
#include <string>
#include <mutex>

namespace Muon {
namespace State {

	// DAQState::dataSource values
	const int DAT_FILE_SOURCE       = 1;
	const int NETWORK_DEVICE_SOURCE = 2;

	struct PersistentState {

		int    dataSource           = NETWORK_DEVICE_SOURCE;

		std::string inputFilename   = ""                   ;
		std::string inputDevicename = ""                   ;

	};

	struct TemporaryState {

		std::string runLabel   = ""   ;

	};

	class DAQState {

	public:

		PersistentState persistentState;
		TemporaryState  tempState      ;

		void update(); // Sets this state to match the current committed state
		bool commit(bool force = false);

		bool load(const std::string &filename);
		bool save();

		void setSaveFile(const std::string &filename);

		bool readPersistentState (std::istream &in );
		void writePersistentState(std::ostream &out);

		static DAQState getState();

	private:

		unsigned int commitNumber = 0;

		static DAQState masterState;

		static std::mutex stateLock;

		DAQState();

		bool isOutdated();

		std::string saveFile;

	};

}
}