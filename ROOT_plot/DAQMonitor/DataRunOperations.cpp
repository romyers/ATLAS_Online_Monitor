#include "DataRunOperations.h"

#include <string>
#include <thread>
#include <fstream>
#include <sstream>

#include "TObjArray.h"
#include "TObjString.h"

#include "DataCaptureOperations.h"
#include "PacketDecodingOperations.h"
#include "DAQState.h"
#include "LockableStream.h"

#include "ProgramControl/Threads.h"
#include "DataModel/DAQData.h"

#include "Logging/ErrorLogger.h"

#include "GUI/Core/UIException.h"
#include "GUI/Core/UISignals.h"
#include "GUI/Core/UILock.h"

#include "analysis/MonitorHooks.h"

#include "MuonReco/Geometry.h"
#include "MuonReco/ConfigParser.h"

#include "FileManagement/FileManager.h"

using namespace std;
using namespace State;
using namespace MonitorHooks;
using namespace MuonReco;

size_t MAX_CACHE_SIZE = 100000000; // 100 MB

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const string CONF_PATH = "../conf/";

bool runStarted = false;

bool isCaptureRunning = false;

string getCurrentTimestamp (const string   &format       );



size_t getFileSize(ifstream &in);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool DataRun::isRunning() { return runStarted; }

void DataRun::stopRun() {

    if(!isRunning()) {

        throw UIException(
            "Please start a run."
        );

    }

    DataCapture::stopDataCapture();

	if(DAQState::getState().persistentState.dataSource != NETWORK_DEVICE_SOURCE) {

		// Stop immediately if we're reading from a file
		Decode::stopImmediately();

		// The network device source case is handled by stopping data capture.
		// The main thread will wait for data capture to finish then stop
		// the decode thread

	}

	isCaptureRunning = false;

}

void DataRun::startRun() {

    // Reset the error logger
    // TODO: This relegates the error logger to a per-run role, so it
    //       should not be used at the level of the wider program.
    //         -- Also, semantically, this sorta makes us want to 
    //            let the error logger not be a singleton anymore.
    ErrorLogger::getInstance().clear();

    DAQState state = DAQState::getState();

    string runLabel = "";

    // Abort if a run is already in progress
    if(isRunning()) {

        throw UIException(
            "Please finish the current run before starting a new run."
        );

    }

    // Set up the input source
    if(state.persistentState.dataSource == DAT_FILE_SOURCE) {

        // Check that the file name is nonempty
        if(state.persistentState.inputFilename == "") {

            throw UIException("Please select a DAT file.");

        }

        // Check that the file exists
        if(!fstream(state.persistentState.inputFilename).is_open()) {

            throw UIException(
                string("File \"") 
                + state.persistentState.inputFilename 
                + "\" could not be opened."
            );

        }

        string filename = state.persistentState.inputFilename;

        cout << endl << "File source selected" << endl;
        cout << "Filename: " << filename << endl;

        size_t filePos      = filename.find_last_of("/");
        size_t extensionPos = filename.find_last_of(".");

        // Check if it's a Windows-style path
        if(filePos == string::npos) {

            filePos = filename.find_last_of("\\");

        }

        // If still nothing, set to beginning of string
        if(filePos == string::npos) {

            filePos = 0;

        } else {

            // Otherwise increment to skip the slash
            ++filePos;

        }

        if(extensionPos == string::npos) {

            extensionPos = filename.size();

        }

        runLabel = filename.substr(filePos, extensionPos - filePos);

    } else if (state.persistentState.dataSource == NETWORK_DEVICE_SOURCE) {

        if(state.persistentState.inputDevicename == "") {

            throw UIException("Please select a network device.");

        }

        cout << endl << "Network source selected" << endl;
        cout << "Ethernet device: " << state.persistentState.inputDevicename << endl;

        runLabel = string("run") 
			+ to_string(state.persistentState.runNumber) 
			+ string("_") 
			+ getCurrentTimestamp("%Y%m%d_%H%M%S");

		++state.persistentState.runNumber;

    }

    cout << endl;

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    DAQData &data = DAQData::getInstance();

    // Configure the geometry and time correction table.

    // NOTE: Write operations on data.geo MUST NOT be performed concurrently
    //       with the decode loop in the current implementation, or thread
    //       safety is not guaranteed. So if data.geo needs to be reconfigured
    //       during a run, the configuration and decoding operations must be
    //       synchronized.
    //         -- TL;DR the decode loop does not lock 'data' before reading 
    //            from geo, so be careful not to change geo while the decode 
    //            loop is running.

    data.lock();

    try{

        data.initialize(CONF_PATH + state.persistentState.confFilename);

    } catch(int e) {

        data.unlock();

        throw UIException(
            string("Failed to configure from ") 
                + state.persistentState.confFilename
                + string(".\n Does the file exist?")
        );


    }

    data.unlock();

    cout << "Conf file: " << state.persistentState.confFilename << endl;

    // NOTE: Following the legacy code, runN is in YYYYMMDD format and does
    //       not include hours/minutes/seconds
    // NOTE: This assumes the DAT filenames are formatted as 
    //       "run_YYYYMMDD_HHMMSS.dat"
    // NOTE: DecodeRawData from the sMDT reco library never sets runN, so I'm 
    //       leaving it out here for now.
    /*
    int runN = (
        (TObjString*)(TString(
            runLabel.substr(3, runLabel.size()).data()
        ).Tokenize("_")->At(0))
    )->String().Atoi();
    data.geo.SetRunN(runN);
    */

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    runStarted = true;
    state.tempState.runLabel = runLabel;
    state.commit(); // TODO: This shouldn't fail, but better if it's robust
	state.save();

    // TODO: Hook up error handling on a per-thread basis. Threads should
    //       report to a threadsafe error handler that does the error handling
    // TODO: It's not obvious why we make a parent thread. It's because we want
    //       a few things to happen after the capture and decode threads are
    //       joined. Make that more obvious.
    ProgramFlow::threadLock.lock();
    ProgramFlow::threads.emplace_back(thread([&data, runLabel]() {

		State::DAQState state = State::DAQState::getState();

		ofstream logWriter;
			
		LockableStream dataStream(MAX_CACHE_SIZE);

		if(state.persistentState.dataSource == NETWORK_DEVICE_SOURCE) {

			string dataFile("../data/");
			dataFile += runLabel;

			string logFile(dataFile + ".log");

			dataFile += ".dat";

			// It's important to open the dataStream first. If a directory in
			// the dataFile path does not exist, the dataStream can create it,
			// but the logWriter cannot.
			dataStream.lock();
			dataStream.open(dataFile);
			if(!dataStream.is_open()) {

				ErrorLogger::getInstance().logError(
					string("Failed to open .dat file: ") + dataFile,
					"dataCapture",
					CRITICAL
				);
				cout << "Aborted run!" << endl;

				dataStream.unlock();

				throw logic_error("Data capture could not open .dat file");

			}
			dataStream.unlock();

			logWriter.open(logFile);
			if(!logWriter.is_open()) {

				ErrorLogger::getInstance().logError(
					string("Failed to open log file: ") + logFile,
					"errorLogging",
					CRITICAL
				);
				cout << "Aborted run!" << endl;

				// TODO: This won't handle stopping the run properly.
				throw logic_error("Data capture could not open logging .log file");

			}
			ErrorLogger::getInstance().addOutputStream(logWriter);

			cout << "Saving packet data to: " << dataFile << endl;

		} else {

			dataStream.lock();
			dataStream.open(state.persistentState.inputFilename);
			if(!dataStream.is_open()) {

				ErrorLogger::getInstance().logError(
					string("Failed to open .dat file: ") + state.persistentState.inputFilename,
					"dataCapture",
					CRITICAL
				);
				cout << "Aborted run!" << endl;

				dataStream.unlock();

				throw logic_error("Data capture could not open .dat file");

			}
			dataStream.unlock();

		}

        MonitorHooks::beforeStartRun(data);

        cout << endl << "Starting run: " << runLabel << endl; 

        UI::UILock.lock();
        UISignalBus::getInstance().onRunStart();
        UI::UILock.unlock();

        // DATA CAPTURE LOOP
        thread dataCaptureThread([&dataStream, &data, runLabel]() {

            if(DAQState::getState().persistentState.dataSource == NETWORK_DEVICE_SOURCE) {

                DataCapture::startDataCapture(dataStream, data, runLabel);

            } 

        });

        // DECODE LOOP
		Decode::markStart();
        thread decodeThread([&dataStream, &data, runLabel](){

			// Regarding the last parameter, we want to make sure the decoder
			// stops when told to if we're reading from a file, but we want
			// it to keep going until it's out of data if we're reading from
			// a live capture.
            Decode::startDecoding(
				dataStream, 
				data, 
				runLabel
			);

        });

        MonitorHooks::startedRun(data);

        dataCaptureThread.join();

		cout << "Data capture finished!" << endl;

		// Make the decoder stop once the data capture thread is done
		Decode::stopSoftly();

        decodeThread     .join();

		ErrorLogger::getInstance().disconnectStreams();

		if(logWriter.is_open()) logWriter.close ();

        runStarted = false;
        state.commit();

        UI::UILock.lock();
        UISignalBus::getInstance().onRunStop();
        UI::UILock.unlock();

        MonitorHooks::finishedRun(data);

        cout << endl;
        cout << "Run finished!" << endl;

        // TODO: Yet another hard-to-find place we do this.
		// This one makes sure the last group of data is displayed.
        UI::UILock.lock();
        UISignalBus::getInstance().onUpdate();
        UI::UILock.unlock();

    }));

    ProgramFlow::threadLock.unlock();

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

size_t getFileSize(ifstream &in) {

	streampos pos = in.tellg();

	in.seekg(0, in.end);
	size_t size = static_cast<size_t>(in.tellg());

	in.clear();
	in.seekg(pos, in.beg);

	return size;

}