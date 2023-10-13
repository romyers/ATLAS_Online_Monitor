/**
 * @file RunDataCapture.cpp
 *
 * @brief Top-level logic flow for the data capture module.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>
#include <fstream>

#include <sys/stat.h>

#include "macros/ErrorLogger.cpp"

#include "src/EthernetCapture/PCapSessionHandler.cpp"
#include "src/ProgramControl/Terminator.cpp"

using namespace std;

namespace Muon {
namespace DataCapture {

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

    void runDataCapture(
        PCapSessionHandler &sessionHandler, 
        LockableStream &dataStream
    ) {

        // If this condition is true, we're not using ethernet 
        // capture.

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
        // TODO: Place the termination condition with the thread
        int i = 0;
        while(!Terminator::getInstance().isTerminated("RUN_FLAG")) {

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

        fileWriter.close();

        cout << "Run finished!" << endl;
        cout << i << " packets recorded." << endl;

    }

}
}