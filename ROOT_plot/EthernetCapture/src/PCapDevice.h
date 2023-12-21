/**
 * @file PCapDevice.h
 *
 * @brief Wrapper for a PCap device.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <pcap.h> 

class PCapDevice {

public:

    PCapDevice(pcap_if_t *d);

    char *name       ();
    char *description();

private:

    pcap_if_t *dev;

};