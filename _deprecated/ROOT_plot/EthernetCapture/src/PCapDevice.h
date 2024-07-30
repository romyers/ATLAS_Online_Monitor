/**
 * @file PCapDevice.h
 *
 * @brief Wrapper for a PCap device.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>

#include <pcap.h> 

struct PCapDevice {

    PCapDevice(pcap_if_t *d);

    const std::string name;
    const std::string description;

};