#include "DataCaptureOperations.h"

#include <fstream>

#include "macros/ErrorLogger.h"
#include "macros/UIFramework/UIException.h"
#include "macros/DAQState.h"

#include "DAQMonitor/EthernetCapture/src/PCapSessionHandler.h"
#include "DAQMonitor/EthernetCapture/src/NetworkDeviceException.h"

#include "src/ProgramControl/Terminator.h"

using namespace std;
using namespace Muon;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void   initializePCapSessionHandler(PCapSessionHandler &sessionHandler);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Muon::DataCapture::runDataCapture(
    LockableStream &dataStream, 
    DAQData &data, 
    string runLabel
) {

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

    // TODO: Main shouldn't need to care about packet counting
    // TODO: If the data stream is a stringstream, it needs to be reset
    //       each loop, or else it will act like a memory leak
    // TODO: Place the termination condition with the thread
    int packets   = 0;
    while(!Terminator::getInstance().isTerminated("RUN_FLAG")) {
 
        // TODO: Remove the sessionHandler writePackets stuff and just write directly
        //       from packetData's packet buffer
        PacketData packetData = sessionHandler.bufferPackets(); // Retrieves and buffers packets from device 
        sessionHandler.writePackets               (dataStream); // Writes buffered packets to dataStream
        sessionHandler.writePackets               (fileWriter); // Writes buffered packets to the .dat file
        sessionHandler.clearBuffer                (          ); // Clears the packet buffer

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