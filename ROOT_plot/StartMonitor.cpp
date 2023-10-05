/**
 * @file StartMonitor.cpp
 *
 * @brief Entry point for the online monitor.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 * 
 * NOTE: This assumes triggerless mode.
 */

///////////////////////////////////////////////////////////////////////////////
//////////////////////////// DEPENDENCIES /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <stdio.h>
#include <thread>
#include <sstream>
#include <fstream>

#include <sys/stat.h>

#include "macros/Monitor.cpp"
#include "macros/Mutexes.cpp"
#include "macros/ErrorLogger.cpp"

#include "src/Geometry.cpp"
#include "src/EthernetCapture/DeviceSelector.cpp"
#include "src/EthernetCapture/PCapSessionHandler.cpp"
#include "src/ProgramControl/Terminator.cpp"
#include "src/ProgramControl/SigHandlers.cpp"

#include "monitorConfig.cpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
////////////////////////////// INTERFACE //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: Replace singletons with globals where it makes sense to do so

// TODO: We will have to multithread; otherwise waiting for monitor refreshes
//       blocks the UI.

// FIXME: Ctrl+c only kills the main thread, and if we try to join the UIThread
//        the terminal will block, forcing us to restart the terminal

// TODO: Try to clean up the multithreading. Ideally all display logic, 
//       including the threading, should be collocated so that we don't need
//       the ugly global mutex. Then we can just call 'startDisplay' or 
//       similar, which will build the UI and start the UI thread. It can
//       also be responsible for pulling events from the monitor and binning
//       them into histograms so that the monitor doesn't have to know about
//       the binning and plotting.
//         -- Also make sure anyone working with this in the future doesn't
//            need to know how to multithread unless they're messing with the
//            GUI framework itself.

// TODO: Rethink the semantics of Monitor. We've made our labels as if it's a
//       display element when really it does data decoding. It should be the
//       UI element that does the 'refreshing', for example.

// TODO: Examine this for ROOT tips:
//       https://mightynotes.wordpress.com/2020/02/15/cern-root-tips-and-tricks/

// TODO: Can I get rid of the multithreading if I put a call to processEvents 
//       in the right spot?

// TODO: Call getpid() for the run? Do we need to give it a pid?

// TODO: Config GUI for options like whether to show lost packets

// TODO: Threadsafe stream wrapper?

// TODO: Cout calls and thread safety

// TODO: Intercept quit root to close down nicely
// TODO: Once we ctrl+c once, we should be able to force kill by ctrl+c again

// TODO: Split out all cout calls to a console logger object. Easy to make it
//       threadsafe or switch the stream we're logging to

// TODO: Make the error logger threadsafe

// TODO: Set Geometry::runN as in DecodeOffline.cpp

/**
 * Macro defining the entry command for the monitor.
 */
void StartMonitor(const string &filename = "");

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

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// IMPLEMENTATION ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void StartMonitor(const string &filename = "") {

	ErrorLogger::getInstance().setOutputStream(cerr);

	///////////////////////////////////////////////////////////////////////////
	///////////////////////// DATA STREAM SETUP ///////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	LockableStream dataStream;
	dataStream.stream = nullptr;

	if(filename == "") {

		dataStream.stream = new stringstream();

	} else {

		cout << "Reading data from file: " << filename << endl;

		fstream *fileStream = new fstream(filename);
		if(!fileStream->is_open()) {
			
			delete fileStream;
			fileStream = nullptr;

			ErrorLogger::getInstance().logError(
				string("Couldn't open file ") + filename
			);

			cout << "Aborted run!" << endl;
			exit(1);

		}

		dataStream.stream = fileStream;

	}

	///////////////////////////////////////////////////////////////////////////
	//////////////////////////// DATA CAPTURE /////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	DeviceManager      devices       ;
	PCapSessionHandler sessionHandler;

	if(filename == "") {

		try {

			devices.initialize();

			PCapDevice networkDevice = runDeviceSelector(devices);

			sessionHandler.initializeSession(networkDevice);

			 // Sets the handler to notify the user when a packet is lost
			sessionHandler.setCheckPackets(true);

		} catch(NetworkDeviceException &e) {

			ErrorLogger::getInstance().logError(e.what());
			cout << "Aborted run!" << endl;
			return 1; // NOTE: We don't call the terminator here because we
			          //       don't need to close down any threads and we
			          //       want the program to exit immediately.

		}

	}

	// THIS MUST BE CALLED BEFORE STARTING ANY THREADS.
	// It intercepts SIGINT/SIGTERM/SIGQUIT to cleanly call threads.
	// It must also be called after we're done getting user input or we'll
	// get weird behavior where code will run expecting data that does not
	// exist, since it does not interrupt anything
	setTerminationHandlers(flagForTermination);

	// TODO: Make session handler non-blocking
	//         -- thread it and implement a thread-safe data stream object
	// TODO: Make data capture run on the main thread to avoid lots of problems
	// TODO: Make sure we make sessionHandler stop waiting for a packet if we
	//       ctrl+c
	// TODO: This lambda is a bit cluttered.
	thread dataCaptureThread([&sessionHandler, &dataStream]() {

		// End the thread if the session handler isn't ready, which means
		// we're reading from a file.
		if(!sessionHandler.isReady()) return;

		string runTimestamp = getCurrentTimestamp("%Y%m%d_%H%M%S");

		cout << endl << "Starting run: " << runTimestamp << endl; // TODO: Add the run number to this

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
			Terminator::getInstance().terminate();
			return;

		}

		/////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////

		cout << "Saving packet data to: " << outputFile << endl;

		// TODO: Main shouldn't need to care about packet counting
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

		cout << "Run finished!" << endl;
		cout << i << " packets recorded." << endl;

		cout << "Shut down data capture." << endl; // TODO: mutex

	});

	///////////////////////////////////////////////////////////////////////////
	/////////////////////////// USER INTERFACE ////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	thread UIThread([]() {

		// TODO: Proper way to terminate the GUI? It hangs sometimes if I
		//       ctrl+c out. This is behavior we want to avoid.
		//         -- Now ctrl+c will simply not close the GUI thread, and the
		//            user will have to close it manually. Still not quite what
		//            I want.
		while(true) {

			UILock.lock();
			gSystem->ProcessEvents();
			UILock.unlock();

			this_thread::sleep_for(chrono::milliseconds((int)(1000 / GUI_REFRESH_RATE)));

		}

		cout << "Shut down UI." << endl; // TODO: mutex

	});

	///////////////////////////////////////////////////////////////////////////
	/////////////////////////// DATA PROCESSING ///////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	Geometry::getInstance().SetRunN(getRunNumber());

	Monitor monitor(dataStream);

	while(!Terminator::getInstance().isTerminated()) {

		monitor.refresh();

		this_thread::sleep_for(chrono::milliseconds((int)(1000 / DATA_REFRESH_RATE)));

	}

	cout << "Shut down decoder." << endl; // TODO: mutex

	dataCaptureThread.join();
	UIThread.join();

	if(dataStream.stream) delete dataStream.stream;
	dataStream.stream = nullptr;

	cout << "Shut down complete!" << endl;

}