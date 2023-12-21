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
#include <string>
#include <iostream>

#include "DAQMonitor/LockableStream.h"

#include "PCapDevice.h"

struct PacketData {

	int lostPackets     = 0;
	int bufferedPackets = 0;

	std::vector<unsigned char> packetBuffer;

};

// TODO: Consider pulling static members out
class PCapSessionHandler {

public:

	PCapSessionHandler ();
	~PCapSessionHandler();

	void setCheckPackets         (      bool         val       );
	void initializeSession       (      PCapDevice  &device    );
	void initializeSession       (const std::string &deviceName);

	PacketData  bufferPackets    (                             );
	void        writePackets     (LockableStream    &out       );
	void        writePackets     (std::ostream      &out       );
	void        clearBuffer      (                             );

	bool isReady                 (                             );

	static void reset            (                             );

private:

	pcap_t *handler;

	static bool checkPackets;

	static int lastPacket;

	static void packetListener(
		u_char *useless, 
		const struct pcap_pkthdr *pkthdr, 
		const u_char *packet_data
	);

	static PacketData data;

};