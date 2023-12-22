#include "DataCaptureOperations.h"

#include <fstream>
#include <iostream>

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

    string outputFile("data/");
    outputFile += runLabel;
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

        data.lock();
        data.packetCount = packets;
        data.lostPackets += packetData.lostPackets;
        data.unlock();

    }

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////

    fileWriter.close();

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