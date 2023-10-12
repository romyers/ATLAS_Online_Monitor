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

#include <sys/stat.h>

#include "monitorConfig.cpp"

#include "macros/Monitor.cpp"
#include "macros/DAQState.cpp"
#include "macros/ErrorLogger.cpp"
#include "macros/LockableStream.cpp"

#include "macros/UIFramework/UIException.cpp"

#include "src/EthernetCapture/PCapSessionHandler.cpp"
#include "src/ProgramControl/Terminator.cpp"
#include "src/ProgramControl/Threads.cpp"
#include "src/Geometry.cpp"

using namespace std;
using namespace Muon;
using namespace State;

namespace Muon {
namespace EntryOperations {

	void exitAll() {

		Terminator::getInstance().terminate();

		// TODO: Close UI windows

	}

	bool directoryExists(const string &path) {

		struct stat sb;

		if(stat(path.data(), &sb) == 0) {

			return true;

		}

		return false;

	}

	bool createDirectory(const string &path) {

		if(mkdir(path.data(), 0777) == 0) return true;

		return false;

	}

	string getCurrentTimestamp(const string &format) {

		char formatBuffer[40];
		time_t sys_time;
		struct tm *timeinfo;
		sys_time = time(0);
		timeinfo = localtime(&sys_time);
		memset(formatBuffer, 0, sizeof(formatBuffer));
		strftime(formatBuffer, 40, format.data(), timeinfo);

		return string(formatBuffer);

	}

	void createIfMissing(const string &directoryName) {

		if(!directoryExists(directoryName)) {

			createDirectory(directoryName);

			cout << "Created output directory: " << directoryName << endl;

		}

	}

	// TODO: startRun should be coupled with monitor functionality, not the
	//       entry view. Move the logic.
	void startRun() {

		DAQState state = DAQState::getState();

		if(state.persistentState.dataSource == DAT_FILE_SOURCE) {

			if(state.persistentState.inputFilename == "") {

				throw UIException("Please select a DAT file.");

				return;

			}

			cout << "File source selected" << endl;

		} else if (state.persistentState.dataSource == NETWORK_DEVICE_SOURCE) {

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

		// TODO: Hook up error handling on a per-thread basis. Threads should
		//       report to a threadsafe error handler that does the error handling
		ProgramFlow::threadLock.lock();
		ProgramFlow::threads.emplace_back(
			thread([]() {

			    DAQState state = DAQState::getState();

			    ///////////////////////////////////////////////////////////////
				/////////////////// DATA STREAM SETUP /////////////////////////
				///////////////////////////////////////////////////////////////

				LockableStream dataStream;
				dataStream.stream = nullptr;

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

					PCapSessionHandler sessionHandler;

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

				///////////////////////////////////////////////////////////////
				////////////////////// DATA CAPTURE ///////////////////////////
				///////////////////////////////////////////////////////////////

				PCapSessionHandler sessionHandler;

				if(state.persistentState.dataSource == NETWORK_DEVICE_SOURCE) {

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

			    thread dataCaptureThread([&sessionHandler, &dataStream](){

			    	// If this condition is true, we're not using ethernet 
			    	// capture.
			    	if(!sessionHandler.isReady()) return;

			    	string runTimestamp = getCurrentTimestamp("%Y%m%d_%H%M%S");

			    	// TODO: Add the run number to this
					cout << endl << "Starting run: " << runTimestamp << endl; 

					/////////////////////////////////////////////////////////
					/////////////////////////////////////////////////////////
					/////////////////////////////////////////////////////////

					createIfMissing("./data");

					string outputFile("data/run_");
					outputFile += runTimestamp;
					outputFile += ".dat";

					ofstream fileWriter(outputFile);
					if(!fileWriter.is_open()) {

						ErrorLogger::getInstance().logError(
							string("Failed to open output file: ") + outputFile
						);
						cout << "Aborted run!" << endl;

						throw logic_error("Data capture could not open output .DAT file");

					}

					/////////////////////////////////////////////////////////
					/////////////////////////////////////////////////////////
					/////////////////////////////////////////////////////////

					cout << "Saving packet data to: " << outputFile << endl;

					// TODO: Main shouldn't need to care about packet counting
					// TODO: If the data stream is a stringstream, it needs to be reset
					//       each loop, or else it will act like a memory leak
					int i = 0;
					while(!Terminator::getInstance().isTerminated()) {

						sessionHandler.bufferPackets(          ); // Retrieves and buffers packets from device
						sessionHandler.writePackets (dataStream); // Writes buffered packets to dataStream
						sessionHandler.writePackets (fileWriter); // Writes buffered packets to the .dat file
						sessionHandler.clearBuffer  (          ); // Clears the packet buffer

						++i;

						if(i % 1000 == 0) {
							cout << "Recorded " << i << " packets" << endl; // TODO: mutex
						}

					}

					/////////////////////////////////////////////////////////
					/////////////////////////////////////////////////////////
					/////////////////////////////////////////////////////////

					// FIXME: This thread fails to exit nicely

					fileWriter.close();

					cout << "Run finished!" << endl;
					cout << i << " packets recorded." << endl;

					cout << "Suspended data capture." << endl; // TODO: mutex

			    });

			    thread decodeThread([&dataStream](){

			    	Geometry::getInstance().SetRunN(getRunNumber());

					Monitor monitor(dataStream);

					while(!Terminator::getInstance().isTerminated()) {

						// TODO: Performance analysis. I'd like this loop to run faster
						//         -- I think binning and drawing is our weak point. Let's
						//            bin every event before drawing

						// FIXME: In file reading mode, this will read the whole file before
						//        terminating on ctrl+c

						monitor.refresh();

						// TODO: This is hacky; fix it. The idea here is to clear processed
						//       data from the dataStream so we don't produce a de facto
						//       memory leak. But we'd rather the code not have to care what
						//       kind of stream dataStream is. Perhaps we make our own kind
						//       of iostream that clears after read?
						//       https://stackoverflow.com/questions/63034484/how-to-create-stream-which-handles-both-input-and-output-in-c
						//       https://stackoverflow.com/questions/12410961/c-connect-output-stream-to-input-stream
						//       https://stackoverflow.com/questions/26346320/how-to-redirect-input-stream-to-output-stream-in-one-line
						// TODO: We might be able to hook our file and data output streams
						//       together so we only have to write to one of them:
						//       https://stackoverflow.com/questions/1760726/how-can-i-compose-output-streams-so-output-goes-multiple-places-at-once
						// TODO: Might it make sense to make the data stream unbuffered? See:
						//       https://stackoverflow.com/questions/52581080/usage-of-output-stream-buffer-in-context-to-stdcout-and-stdendl
						// TODO: Anyway, we can revisit how we want to handle the data streams
						//       later. For now, this will suffice.
						dataStream.lock();
						stringstream *temp = dynamic_cast<stringstream*>(dataStream.stream);
						if(temp) {
							string unread = temp->eof() ?
								"" : temp->str().substr(temp->tellg());
							temp->str(unread);
						}
						dataStream.unlock();

						this_thread::sleep_for(chrono::milliseconds((int)(1000 / DATA_REFRESH_RATE)));

						}

						cout << "Suspended data decoding." << endl; // TODO: mutex

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
				    			
			})
		);
		ProgramFlow::threadLock.unlock();

	}

}
}