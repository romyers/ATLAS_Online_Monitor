#pragma once
#include <pcap.h> 
#include <sstream>





class PcapDev
{
public:
    PcapDev();
    ~PcapDev();
    pcap_t *pcapHandle_;
    char *dev_;  //device name
    int packetReceiver();    
};


class EthernetCap
{
public:
    EthernetCap(std::ostream &out);
    ~EthernetCap();
    static const u_char EVENT_HEADER  = 0b1010;
    static const u_char EVENT_TRAILER = 0b1100;
    static const u_char IDLE_WORD     = 0b11111111;
    static const int    DATA_START    = 14;

    void binary_write(const struct pcap_pkthdr* pkthdr, const u_char* packet_data);
    void setCheckPacketFlagTrue();

private:
    // FILE *fp_binary_;
    std::ostream &outputStream;
    bool checkPacketFlag_;
    int packetNum_, packetNumLast_;
};


extern EthernetCap *p_ecap;
void gotPacket(u_char *useless, 
					const struct pcap_pkthdr* pkthdr, const u_char* packet_data);


