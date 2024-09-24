#include "DataCaptureOperations.h"

#include <fstream>
#include <iostream>

#include "Logging/ErrorLogger.h"

#include "DAQState.h"

#include "FileManagement/FileManager.h"

#include "GUI/Core/UIException.h"

#include "EthernetCapture/src/PCapSessionHandler.h"
#include "EthernetCapture/src/NetworkDeviceException.h"
#include "EthernetCapture/src/DeviceManager.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool isEcapRunning = false;

void initializePCapSessionHandler(PCapSessionHandler &sessionHandler);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
    
vector<PCapDevice> DataCapture::getNetworkDevices() {

    DeviceManager devices;

    try {
        
        devices.initialize();

    } catch (NetworkDeviceException &e) {

        cout << e.what() << endl;

    }

    return devices.getAllDevices();

}

void DataCapture::stopDataCapture() {

    isEcapRunning = false;

}

void DataCapture::startDataCapture(
    LockableStream &dataStream, 
    DAQData &data, 
    const string &runLabel
) {

    if(isEcapRunning) return;

    // Resets all the static data stored in session handlers
    PCapSessionHandler::reset();

    PCapSessionHandler sessionHandler;

    try{

        initializePCapSessionHandler(sessionHandler);

    } catch(UIException &e) {

        ErrorLogger::getInstance().logError(e.what(), "dataCapture", CRITICAL);

        // TODO: Abort run and popup an error message in this case

        throw e;

    }

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////

    isEcapRunning = true;

    int packets   = 0;
    while(isEcapRunning) {
 
        // Retrieve buffered packets and metadata
        PacketData packetData = sessionHandler.bufferPackets(); // Retrieves and buffers packets from device 

        // Write buffered packets to output
		// This will write them to a DAT file with caching for the decoder
        dataStream.lock();
		dataStream.write(
			(char*)packetData.packetBuffer.data(), 
			packetData.packetBuffer.size()
		);
        dataStream.unlock();

        packets += packetData.bufferedPackets;

        // Store metadata
        data.lock();
        data.packetCount = packets;
        data.lostPackets += packetData.lostPackets;
		data.markDirty();
        data.unlock();

    }

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////

    cout << "Suspended data capture." << endl;
    cout << data.packetCount << " packets recorded." << endl;

}

void initializePCapSessionHandler(
    PCapSessionHandler &sessionHandler
) {

    State::DAQState state = State::DAQState::getState();

    try {

        sessionHandler.initializeSession(
            state.persistentState.inputDevicename,
			state.tempState.pcapBufferSize * 1000000
        );

        sessionHandler.setCheckPackets(true);

    } catch(NetworkDeviceException &e) {

        ErrorLogger::getInstance().logError(e.what(), "dataCapture", CRITICAL);
        cout << "Aborted run!" << endl;

        throw UIException(
            string("Unable to listen on network device: \"" 
                + state.persistentState.inputDevicename 
                + "\". Does the device exist?")
        );

    }

}