/**
 * @file PCapSessionHandler.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 * 
 * Adapted from the old EthernetCap.cpp.
 * 
 * NOTE: It's reallllllly nontrivial to get a member function into
 *       pcap_dispatch, so I've had to stick with static functions.
 *       As a result, there is a lot of static data that is shared
 *       between session handlers and needs to be reset every run
 *       with the static reset method. This is far from optimal.
 */

// TODO: Separate data source/format-specific logic from data source/format
//       independent logic for easy switching of data source later.
//         -- e.g. pcap stuff, word size stuff, etc should be separate and
//            collocated

#pragma once

#include <vector>
#include <sys/socket.h>
#include <signal.h>
#include <functional>

#include "macros/ErrorLogger.cpp"

#include "DAQMonitor/LockableStream.cpp"

#include "DAQMonitor/EthernetCapture/src/PCapDevice.cpp"
#include "DAQMonitor/EthernetCapture/src/NetworkDeviceException.cpp"

// TODO: This is a little bit of a messy dependency to have at this level
#include "src/DataModel/DAQData.cpp"

using namespace std;

const string PCAP_WARN = "dataCapture";

const unsigned int WORD_SIZE = 5; // TODO: Make sure this and Signal's word size
                                  //       come from the same place

// TODO: Do we use the header and trailer chars?
unsigned char EVENT_HEADER         = 0b1010                          ;
unsigned char EVENT_TRAILER        = 0b1100                          ;
unsigned char IDLE_WORD[WORD_SIZE] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
unsigned int  DATA_START           = 14                              ;


// TODO: Consider pulling static members out to a singleton
class PCapSessionHandler {

public:

	PCapSessionHandler ();
	~PCapSessionHandler();

	void setCheckPackets  (      bool        val       );
	void initializeSession(      PCapDevice &device    );
	void initializeSession(const string     &deviceName);

	int  bufferPackets    (                            );
	void writePackets     (LockableStream   &out       );
	void writePackets     (ostream          &out       );
	void clearBuffer      (                            );

	bool isReady          (                            );

	static void reset     (                            );

private:

	pcap_t *handler;

	static bool checkPackets;

	static int lastPacket;

	static void packetListener(
		u_char *useless, 
		const struct pcap_pkthdr *pkthdr, 
		const u_char *packet_data
	);

	static vector<unsigned char> packetBuffer;

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool PCapSessionHandler::checkPackets(true);
int  PCapSessionHandler::lastPacket(-1);

vector<unsigned char> PCapSessionHandler::packetBuffer;

PCapSessionHandler::PCapSessionHandler() : handler(nullptr) {}

void PCapSessionHandler::reset() {

	lastPacket = -1;
	packetBuffer.clear();

}

void PCapSessionHandler::setCheckPackets(bool val) {

	checkPackets = val;

}

void PCapSessionHandler::initializeSession(const string &deviceName) {

	char errorBuffer[PCAP_ERRBUF_SIZE];

    // Arguments: device name, snap length, promiscuous mode, to_ms, error_buffer
	handler = pcap_open_live(deviceName.data(), 65536, 1, 10000, errorBuffer);
	if(!handler) {

		handler = nullptr;

		throw NetworkDeviceException(
			string("Could not open device ") + deviceName + " : " + errorBuffer
		);

	}

	// Compile the filter
	struct bpf_program fcode;
	bpf_u_int32 netmask = 0xffffff;
	char packetFilter[] = "ether src ff:ff:ff:c7:05:01";
	if(pcap_compile(handler, &fcode, packetFilter, 1, netmask) < 0) {

		pcap_freecode(&fcode);
		if(handler) pcap_close(handler);
		handler = nullptr;

		throw NetworkDeviceException(
			"Unable to compile the packet filter. Check the syntax!"
		);

	}

	if(pcap_setfilter(handler, &fcode) < 0) {

		pcap_freecode(&fcode);
		if(handler) pcap_close(handler);
		handler = nullptr;

		throw NetworkDeviceException(
			"Filter address error. Cannot apply filter!"
		);

	}

  	// pcap_freecode() is used to free up allocated memory pointed to by a
    // bpf_program struct generated by pcap_compile(3PCAP) when that BPF
    // program is no longer needed, for example after it has been made the
    // filter program for a pcap structure by a call to
    // pcap_setfilter(3PCAP).
	pcap_freecode(&fcode);



}

void PCapSessionHandler::initializeSession(PCapDevice &device) {

	initializeSession(device.name());

}

void PCapSessionHandler::packetListener(
	u_char *useless, 
	const struct pcap_pkthdr *pkthdr, 
	const u_char *packet_data
) {

	int length = pkthdr->len - 18; // bytes (preload = 14, postload = 4)
	for(int iter = DATA_START; iter < DATA_START + length; iter += 5) {

		if(memcmp(packet_data + iter, IDLE_WORD, WORD_SIZE)) {

			packetBuffer.insert(
				packetBuffer.end(), 
				packet_data + iter, 
				packet_data + iter + WORD_SIZE
			);

		}

	}


	int packetNum = (int)(*(packet_data + length + 18 - 2) * 256);
	packetNum += (int)(*(packet_data + length + 18 - 1));

	if(checkPackets) {

		if(lastPacket != -1) {

			if(packetNum != (lastPacket + 1) % 65536) {

				int missingPackets = (packetNum - (lastPacket + 1)) % 65536;

				ErrorLogger::getInstance().logError(
					string("WARNING -- ") + to_string(missingPackets) + " packets lost! Packet = "
						+ to_string(packetNum) + ", Last = " + to_string(lastPacket),
					PCAP_WARN,
					ERROR
				);

				DAQData &data = DAQData::getInstance();

				data.lock();
				data.lostPackets += missingPackets;
				data.unlock();

			}

		}

	}

	lastPacket = packetNum;

}


// TODO: Can we rewrite this to be non-blocking so we can run it in the main
//       thread?
// TODO: Use this to catch sigint and abort packet write:
//       https://stackoverflow.com/questions/27204444/not-able-to-catch-sigint-signal-while-using-select
// TODO: Examine this regarding making this non-blocking:
//       https://stackoverflow.com/questions/6715736/using-select-for-non-blocking-sockets
int PCapSessionHandler::bufferPackets() {

	if(!isReady()) {

		throw logic_error(
			"PCapSessionHandler::writePackets -- PCapSessionHandler must be initialized with initializeSession() before use."
		);

	}

	// Get a file descriptor for the packet capture device
	int fd = pcap_fileno(handler); 

	// File descriptor sets for "select" function (it's a bit array)
	fd_set rfds;

	FD_ZERO(&rfds); // re-clears (empty) file descriptor set
	FD_SET(fd, &rfds); // rebuild file descriptor set

	struct timespec tv;
	tv.tv_sec  = 10000;
	tv.tv_nsec = 0    ;

	// Set up pselect to handle SIGINT/SIGTERM gracefully
	// TODO: I'd rather this be handled at the level of program control
	//       and not be hidden away here. Try to replace pselect with some sort
	//       of condition we can check and wait for explicitly at a higher
	//       level
	// FIXME: This doesn't work yet -- pselect ignores signals
	//        Also, this will only ever catch SIGINT -- not termination from
	//        the terminator
	sigset_t signalSet;
	sigemptyset(&signalSet);
	sigaddset(&signalSet, SIGINT );
	sigaddset(&signalSet, SIGTERM);
	sigaddset(&signalSet, SIGQUIT);

	int selectSuccess = pselect(fd + 1, &rfds, NULL, NULL, &tv, &signalSet);
	// See: https://man7.org/linux/man-pages/man2/select.2.html

	int packetsBuffered = 0;
	if(-1 == selectSuccess) {

		cout << "Select failed" << endl;

	} else if(selectSuccess) {

		packetsBuffered = pcap_dispatch(
			handler, 
			-1, 
			packetListener,
			NULL
		);


	} else {

		cout << "Select timeout on fd: " << fd << " Return code: " << selectSuccess << endl;

	}

	return packetsBuffered;

}

// For streams that belong to just one thread
void PCapSessionHandler::writePackets(ostream &out) {

	if(!isReady()) {

		throw logic_error(
			"PCapSessionHandler::writePackets -- PCapSessionHandler must be initialized with initializeSession() before use."
		);

	}

	out.write((char*)packetBuffer.data(), packetBuffer.size());
	out.flush(); // Since we're not calling endl, it might not flush


}

// For streams that need to be shared among threads
void PCapSessionHandler::writePackets(LockableStream &out) {

	if(!isReady()) {

		throw logic_error(
			"PCapSessionHandler::writePackets -- PCapSessionHandler must be initialized with initializeSession() before use."
		);

	}

	out.lock();
	writePackets(*out.stream);
	out.unlock();

}

void PCapSessionHandler::clearBuffer() {

	if(!isReady()) {

		throw logic_error(
			"PCapSessionHandler::writePackets -- PCapSessionHandler must be initialized with initializeSession() before use."
		);

	}

	packetBuffer.clear();

}

bool PCapSessionHandler::isReady() {

	return handler != nullptr;

}

PCapSessionHandler::~PCapSessionHandler() {

	if(handler) pcap_close(handler);
	handler = nullptr;

}