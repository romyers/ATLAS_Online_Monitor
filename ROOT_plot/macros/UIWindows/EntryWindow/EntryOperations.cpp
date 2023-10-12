/**
 * @file EntryOperations.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>
#include <fstream>
#include <thread>
#include <sstream>

#include "macros/DAQState.cpp"
#include "macros/ErrorLogger.cpp"
#include "macros/LockableStream.cpp"
#include "macros/RunDataCapture.cpp"
#include "macros/RunDecoding.cpp"

#include "macros/UIFramework/UIException.cpp"

#include "src/EthernetCapture/PCapSessionHandler.cpp"
#include "src/ProgramControl/Terminator.cpp"
#include "src/ProgramControl/Threads.cpp"
#include "src/Geometry.cpp"

// TEMP
#include "macros/DataModel/DAQData.cpp"

using namespace std;
using namespace Muon;
using namespace State;

// TEMP
TRootEmbeddedCanvas *c;

namespace Muon {
namespace EntryOperations {

	void exitAll() {

		Terminator::getInstance().terminate();

		// TODO: Close UI windows

	}

	

	void initializeDataStream(LockableStream &dataStream) {

		dataStream.stream = nullptr;

		State::DAQState state = State::DAQState::getState();

		if(state.persistentState.dataSource == DAT_FILE_SOURCE) {

			cout << "Reading data from file: " << state.persistentState.inputFilename << endl;

			fstream *fileStream = new fstream(state.persistentState.inputFilename);
			if(!fileStream->is_open()) {
							
				delete fileStream;
				fileStream = nullptr;

				ErrorLogger::getInstance().logError(
					string("Couldn't open file ") + state.persistentState.inputFilename
				);

				cout << "Aborted run!" << endl;

				// TODO: We need to link this exception up to an alert. But we want
				//       that alert to be created in the UI thread.
				throw UIException(
					string("File \"") 
					+ state.persistentState.inputFilename 
					+ "\" could not be opened."
				);

			}

			dataStream.stream = fileStream;

		} else if(state.persistentState.dataSource == NETWORK_DEVICE_SOURCE) {

			dataStream.stream = new stringstream();

		}

	}

	void initializePCapSessionHandler(PCapSessionHandler &sessionHandler) {

		State::DAQState state = State::DAQState::getState();

		try {

			sessionHandler.initializeSession(
				state.persistentState.inputDevicename
			);

			sessionHandler.setCheckPackets(true);

		} catch(NetworkDeviceException &e) {

			ErrorLogger::getInstance().logError(e.what());
			cout << "Aborted run!" << endl;

			throw UIException(
				string("Unable to listen on network device: \"" 
					+ state.persistentState.inputDevicename 
					+ "\". Does the device exist?")
			);

		}

	}

	// TODO: startRun should be coupled with monitor functionality, not the
	//       entry view. Move the logic.
	// TODO: Add controls for stopping a run without terminating GUI. We'll
	//       want another kind of termination signal. Perhaps we can expand
	//       the Terminator to have multiple kinds of signals our threads
	//       can condition on.
	void startRun() {

		DAQState state = DAQState::getState();

		if(state.tempState.runStarted) {

			throw UIException(
				"The current run must be terminated before\n"
				"a new run may be initiated."
			);

		}

		if(state.persistentState.dataSource == DAT_FILE_SOURCE) {

			if(state.persistentState.inputFilename == "") {

				throw UIException("Please select a DAT file.");

			}

			cout << endl << "File source selected" << endl;
			cout << "Filename: " << state.persistentState.inputFilename << endl;

		} else if (state.persistentState.dataSource == NETWORK_DEVICE_SOURCE) {

			if(state.persistentState.inputDevicename == "") {

				throw UIException("Please select a network device.");

			}

			cout << endl << "Network source selected" << endl;
			cout << "Ethernet device: " << state.persistentState.inputDevicename << endl;

		}

		// TODO: Hook up error handling on a per-thread basis. Threads should
		//       report to a threadsafe error handler that does the error handling
		ProgramFlow::threadLock.lock();
		ProgramFlow::threads.emplace_back(thread([]() {

			DAQState state = DAQState::getState();

			LockableStream dataStream;
			initializeDataStream(dataStream);

			PCapSessionHandler sessionHandler;

			if(state.persistentState.dataSource == NETWORK_DEVICE_SOURCE) {

				initializePCapSessionHandler(sessionHandler);

			}

			// TODO: Put the thread termination conditions here
			thread dataCaptureThread([&sessionHandler, &dataStream]() {

	        	if(!sessionHandler.isReady()) return;

				DataCapture::runDataCapture(sessionHandler, dataStream);

			});

			thread decodeThread([&dataStream](){

				Decoder::runDecoding(dataStream);

			});

			dataCaptureThread.join();
			decodeThread     .join();

			// TODO: Again, I would rather avoid caring about the type of stream.
			// TODO: We don't really need to lock here
			dataStream.lock();
			fstream *temp = dynamic_cast<fstream*>(dataStream.stream);
			if(temp) {
				temp->close();
			}
			dataStream.unlock();

			if(dataStream.stream) delete dataStream.stream;
			dataStream.stream = nullptr;

		}));
		ProgramFlow::threadLock.unlock();

		state.tempState.runStarted = true;
		state.commit(); // NOTE: This shouldn't fail

		// TEMP
		// Quick prototyping
		TGMainFrame *histFrame = new TGMainFrame(gClient->GetRoot(), 500, 500);

		c = new TRootEmbeddedCanvas("Histograms", histFrame, 450, 450);
		histFrame->AddFrame(c, new TGLayoutHints(kLHintsCenterX));

		// Set up the main window now that it has all its components
        histFrame->SetWindowName("Histograms");
        histFrame->MapSubwindows();
        histFrame->Resize(histFrame->GetDefaultSize());
        histFrame->MapWindow();

	}

}
}