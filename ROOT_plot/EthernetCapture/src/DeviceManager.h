/**
 * @file DeviceManager.h
 *
 * @brief Manages network devices.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <iostream>

#include "PCapDevice.h"

class DeviceManager {

public:

    DeviceManager();
    ~DeviceManager();

    DeviceManager (      DeviceManager &other) = delete;
    bool operator=(const DeviceManager &other) = delete;

    void initialize();
    PCapDevice getDevice(int deviceNumber);
    std::vector<PCapDevice> getAllDevices();

    void printDevices(std::ostream &out) const;

private:

    pcap_if_t *devices;

};