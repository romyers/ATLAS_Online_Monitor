/**
 * @file EntryOperations.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "DAQMonitor/DataRunOperations.cpp"

#include "src/ProgramControl/Terminator.cpp"

using namespace std;

namespace Muon {
namespace EntryOperations {

	void exitAll() {

		Terminator::getInstance().terminate();

		// TODO: Close UI windows

	}

	void stopRun() {

		DataRun::stopRun();

	}
	
	void startRun() {

		DataRun::startRun();
		
	}

}
}