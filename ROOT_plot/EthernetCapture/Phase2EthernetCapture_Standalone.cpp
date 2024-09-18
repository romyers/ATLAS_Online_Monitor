/**
 * @file Phase2EthernetCapture_Standalone.cpp
 *
 * @brief Entry point for the standalone ethernet capture application.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#include <string>
#include <iostream>
#include <fstream>
#include <cstring>

#include <sys/stat.h>

#include "src/PCapSessionHandler.h"
#include "src/DeviceManager.h"
#include "src/DeviceSelector.h"
#include "src/PCapDevice.h"
#include "src/NetworkDeviceException.h"

using namespace std;

bool   directoryExists    (const string &path         );
bool   createDirectory    (const string &path         );
string getCurrentTimestamp(const string &format       );
void   createIfMissing    (const string &directoryName);

int main(int argc, char **argv) {

	PCapSessionHandler::reset();

	DeviceManager      devices;
	PCapSessionHandler handler;

	try {

		devices.initialize();

		PCapDevice networkDevice = runDeviceSelector(devices);

		handler.initializeSession(networkDevice, 100000000);

		// Sets the handler to notify the user when a packet is lost
		handler.setCheckPackets(true);

	} catch(NetworkDeviceException &e) {

    	cerr << e.what() << endl;
        cout << "Aborted run!" << endl;

        exit(1);

    }

    string runTimestamp = getCurrentTimestamp("%Y%m%d_%H%M%S");

	cout << endl << "Starting run: " << runTimestamp << endl; 

	createIfMissing("../data");

	string outputFile("../data/run_");
	outputFile += runTimestamp;
	outputFile += ".dat";

	ofstream fileWriter(outputFile);
	if(!fileWriter.is_open()) {

		cerr << "Failed to open output file: " << outputFile << endl;
		cout << "Aborted run!" << endl;

		exit(1);

	}

	cout << "Saving packet data to: " << outputFile << endl;

	int packets   = 0;
	int thousands = 0;

	while(true) {

		PacketData data = handler.bufferPackets();
		fileWriter.write((char*)data.packetBuffer.data(), data.packetBuffer.size());
		fileWriter.flush();

		packets += data.bufferedPackets;

		while(packets / 1000 > thousands) {

			++thousands;
			cout << "Recorded " << thousands * 1000 << " packets";

		}

	}

	fileWriter.close();
	cout << endl << "Data capture finished!" << endl;
	cout << packets << " packets recorded." << endl;

	return 0;

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