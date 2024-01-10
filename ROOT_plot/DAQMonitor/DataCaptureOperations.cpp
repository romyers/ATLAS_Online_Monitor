#include "DataCaptureOperations.h"

#include <fstream>
#include <iostream>

#include <sys/stat.h>

#include "Logging/ErrorLogger.h"

#include "DAQState.h"

#include "GUI/Core/UIException.h"

#include "EthernetCapture/src/PCapSessionHandler.h"
#include "EthernetCapture/src/NetworkDeviceException.h"
#include "EthernetCapture/src/DeviceManager.h"

using namespace std;
using namespace Muon;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool isEcapRunning = false;

void initializePCapSessionHandler(PCapSessionHandler &sessionHandler);

bool directoryExists(const string   &path         );
bool createDirectory(const string   &path         );
void createIfMissing(const string   &directoryName);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
    
vector<PCapDevice> Muon::DataCapture::getNetworkDevices() {

    DeviceManager devices;

    try {
        
        devices.initialize();

    } catch (NetworkDeviceException &e) {

        cout << e.what() << endl;

    }

    return devices.getAllDevices();

}

void Muon::DataCapture::stopDataCapture() {

    isEcapRunning = false;

}

void Muon::DataCapture::startDataCapture(
    LockableStream &dataStream, 
    DAQData &data, 
    string runLabel
) {

    if(isEcapRunning) return;

    // Resets all the static data stored in session handlers
    PCapSessionHandler::reset();

    PCapSessionHandler sessionHandler;
    initializePCapSessionHandler(sessionHandler);

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////

    createIfMissing("../data");

    string outputFile("../data/");
    outputFile += runLabel;

    string logFile(outputFile + ".log");

    outputFile += ".dat";

    ofstream fileWriter(outputFile);
    if(!fileWriter.is_open()) {

        ErrorLogger::getInstance().logError(
            string("Failed to open output file: ") + outputFile,
            "dataCapture",
            FATAL
        );
        cout << "Aborted run!" << endl;

        throw logic_error("Data capture could not open output .dat file");

    }

    ofstream logWriter(logFile);
    if(!logWriter.is_open()) {

        ErrorLogger::getInstance().logError(
            string("Failed to open log file: ") + logFile,
            "errorLogging",
            FATAL
        );
        cout << "Aborted run!" << endl;

        // TODO: This won't handle stopping the run properly.
        throw logic_error("Data capture could not open logging .log file");

    }
    ErrorLogger::getInstance().addOutputStream(logWriter);

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////

    cout << "Saving packet data to: " << outputFile << endl;

    isEcapRunning = true;

    // TODO: Main shouldn't need to care about packet counting
    // TODO: If the data stream is a stringstream, it needs to be reset
    //       each loop, or else it will act like a memory leak
    int packets   = 0;
    while(isEcapRunning) {
 
        // Retrieve buffered packets and metadata
        PacketData packetData = sessionHandler.bufferPackets(); // Retrieves and buffers packets from device 

        // Write buffered packets to dataStream
        dataStream.lock();
        dataStream.stream->write(
            (char*)packetData.packetBuffer.data(), 
            packetData.packetBuffer.size()
        );
        dataStream.stream->flush();
        dataStream.unlock();

        // Write buffered packets to the .dat file
        fileWriter.write(
            (char*)packetData.packetBuffer.data(), 
            packetData.packetBuffer.size()
        );
        fileWriter.flush();

        packets += packetData.bufferedPackets;

        // Store metadata
        data.lock();
        data.packetCount = packets;
        data.lostPackets += packetData.lostPackets;
        data.unlock();

    }

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////

    ErrorLogger::getInstance().disconnectStreams();

    fileWriter.close();
    logWriter.close ();

    cout << "Suspended data capture." << endl;
    cout << data.packetCount << " packets recorded." << endl;

}

void initializePCapSessionHandler(
    PCapSessionHandler &sessionHandler
) {

    State::DAQState state = State::DAQState::getState();

    try {

        sessionHandler.initializeSession(
            state.persistentState.inputDevicename
        );

        sessionHandler.setCheckPackets(true);

    } catch(NetworkDeviceException &e) {

        ErrorLogger::getInstance().logError(e.what(), "dataCapture", FATAL);
        cout << "Aborted run!" << endl;

        throw UIException(
            string("Unable to listen on network device: \"" 
                + state.persistentState.inputDevicename 
                + "\". Does the device exist?")
        );

    }

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