#include "DataRunOperations.h"

#include <string>
#include <fstream>
#include <thread>
#include <sstream>

#include <sys/stat.h>

#include "TObjArray.h"
#include "TObjString.h"

#include "DataCaptureOperations.h"
#include "PacketDecodingOperations.h"

#include "macros/DAQState.h"
#include "macros/ErrorLogger.h"

#include "DAQMonitor/LockableStream.h"

#include "macros/UIFramework/UIException.h"
#include "macros/UIFramework/UISignals.h"
#include "macros/UIFramework/UILock.h"

#include "src/Geometry.h"
#include "src/ProgramControl/Terminator.h"
#include "src/ProgramControl/Threads.h"
#include "src/DataModel/DAQData.h"

using namespace std;
using namespace Muon;
using namespace State;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void   initializeDataStream(LockableStream &dataStream   );
string getCurrentTimestamp (const string   &format       );

bool   directoryExists     (const string   &path         );
bool   createDirectory     (const string   &path         );
void   createIfMissing     (const string   &directoryName);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

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
                string("Couldn't open file ") + state.persistentState.inputFilename,
                "runOperations",
                FATAL
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

void DataRun::stopRun() {

    if(!State::DAQState::getState().tempState.runStarted) {

        throw UIException(
            "Please start a run."
        );

    }

    Terminator::getInstance().terminate("RUN_FLAG");

}

// TODO: startRun should be coupled with monitor functionality, not the
//       entry view. Move the logic.
// TODO: Add controls for stopping a run without terminating GUI. We'll
//       want another kind of termination signal. Perhaps we can expand
//       the Terminator to have multiple kinds of signals our threads
//       can condition on.
void DataRun::startRun() {

    // Reset the error logger
    // TODO: This relegates the error logger to a per-run role, so it
    //       should not be used at the level of the wider program.
    //         -- Also, semantically, this sorta makes us want to 
    //            let the error logger not be a singleton anymore.
    ErrorLogger::getInstance().clear();
    ErrorLogger::getInstance().disconnectStreams();

    DAQState state = DAQState::getState();

    string runLabel = "";

    // Abort if a run is already in progress
    if(state.tempState.runStarted) {

        throw UIException(
            "Please finish the current run before starting a new run."
        );

    }

    // Set up the input source
    if(state.persistentState.dataSource == DAT_FILE_SOURCE) {

        if(state.persistentState.inputFilename == "") {

            throw UIException("Please select a DAT file.");

        }

        string filename = state.persistentState.inputFilename;

        cout << endl << "File source selected" << endl;
        cout << "Filename: " << filename << endl;

        size_t extensionPos = filename.find_last_of(".");

        if(extensionPos == string::npos) {

            runLabel = filename;

        } else {

            runLabel = filename.substr(0, extensionPos);

        }

    } else if (state.persistentState.dataSource == NETWORK_DEVICE_SOURCE) {

        if(state.persistentState.inputDevicename == "") {

            throw UIException("Please select a network device.");

        }

        cout << endl << "Network source selected" << endl;
        cout << "Ethernet device: " << state.persistentState.inputDevicename << endl;

        runLabel = string("run_") + getCurrentTimestamp("%Y%m%d_%H%M%S");

    }

    state.tempState.runStarted = true;
    state.tempState.runLabel = runLabel;
    state.commit(); // TODO: This shouldn't fail, but better if it's robust

    // Clear the DAQData of any data from a previous run
    DAQData &data = DAQData::getInstance();

    data.lock  ();
    data.clear ();
    data.unlock();

    // TODO: Hook up error handling on a per-thread basis. Threads should
    //       report to a threadsafe error handler that does the error handling
    // TODO: It's not obvious why we make a parent thread. It's because we want
    //       a few things to happen after the capture and decode threads are
    //       joined. Make that more obvious.
    ProgramFlow::threadLock.lock();
    ProgramFlow::threads.emplace_back(thread([&data, runLabel]() {

        cout << endl << "Starting run: " << runLabel << endl; 

        // NOTE: Following the legacy code, runN is in YYYYMMDD format and does
        //       not include hours/minutes/seconds
        // NOTE: This assumes the DAT filenames are formatted as "run_YYYYMMDD_HHMMSS.dat"
        int runN = (
            (TObjString*)(TString(
                runLabel.substr(3, runLabel.size()).data()
            ).Tokenize("_")->At(0))
        )->String().Atoi();
        Geometry::getInstance().SetRunN(runN);

        createIfMissing("./data");

        string logFile("data/");
        logFile += runLabel;
        logFile += ".log";

        ofstream logWriter(logFile);
        if(!logWriter.is_open()) {

            ErrorLogger::getInstance().logError(
                string("Failed to open log file: ") + logFile,
                "errorLogging",
                FATAL
            );
            cout << "Aborted run!" << endl;

            throw logic_error("Data capture could not open logging .log file");

        }
        ErrorLogger::getInstance().addOutputStream(logWriter);

        LockableStream dataStream;
        initializeDataStream(dataStream);

        // TODO: Put the thread termination conditions here
        thread dataCaptureThread([&dataStream, &data, runLabel]() {

            if(DAQState::getState().persistentState.dataSource == NETWORK_DEVICE_SOURCE) {

                DataCapture::runDataCapture(dataStream, data, runLabel);

            }

        });

        thread decodeThread([&dataStream, &data](){

            Decode::runDecoding(dataStream, data);

        });

        dataCaptureThread.join();
        decodeThread     .join();

        // TODO: Again, I would rather avoid caring about the type of stream.
        dataStream.lock();
        fstream *temp = dynamic_cast<fstream*>(dataStream.stream);
        if(temp) {
            temp->close();
        }
        dataStream.unlock();

        if(dataStream.stream) delete dataStream.stream;
        dataStream.stream = nullptr;

        DAQState state = DAQState::getState();
        state.tempState.runStarted = false;
        state.commit();

        logWriter.close();

        // Once the run is shut down, we can clear the
        // run flag if it exists
        Terminator::getInstance().clearFlag("RUN_FLAG");

        cout << "Run finished!" << endl;

        // TODO: Yet another hard-to-find place we do this.
        Muon::UI::UILock.lock();
        UISignalBus::getInstance().onUpdate();
        Muon::UI::UILock.unlock();

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

void createIfMissing(const string &directoryName) {

    if(!directoryExists(directoryName)) {

        createDirectory(directoryName);

        cout << "Created output directory: " << directoryName << endl;

    }

}