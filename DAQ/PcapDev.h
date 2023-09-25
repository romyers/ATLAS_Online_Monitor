#pragma once
//global variable


class PcapDev
{
public:
    PcapDev();
    ~PcapDev();
    pcap_t *pcapHandle_;
    char *dev_;  //device name
    int packetReceiver();    
};


