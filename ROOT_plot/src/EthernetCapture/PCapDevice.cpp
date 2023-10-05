/**
 * @file PCapDevice.cpp
 *
 * @brief Wrapper for a PCap device.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

R__LOAD_LIBRARY(libpcap); // Loads pcap library into Root

#include <pcap.h> 

class PCapDevice {

public:

    PCapDevice(pcap_if_t *d);

    char *name       ();
    char *description();

private:

    pcap_if_t *dev;

};



PCapDevice::PCapDevice(pcap_if_t *d) : dev(d) {}

char *PCapDevice::name() {

    return dev->name;

}

char *PCapDevice::description() {

    return dev->description;

}