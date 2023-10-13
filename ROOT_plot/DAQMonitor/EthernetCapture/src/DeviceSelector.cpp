/**
 * @file DeviceSelector.cpp
 *
 * @brief Provides a user interface for selecting a network device.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

// NOTE: Currently this is a console interface. We may eventually want to move
//       to a GUI interface here.

#pragma once

#include <iostream>

#include "DAQMonitor/EthernetCapture/src/DeviceManager.cpp"

using namespace std;

PCapDevice runDeviceSelector(DeviceManager &devices) {

    cout << endl << "Found connected network devices: " << endl;

	devices.printDevices(cout);

    int device_number;

    cout << endl << "Please select your ethernet device: ";
    cin >> device_number;

    PCapDevice d = devices.getDevice(device_number);

    cout << "Selected device " << device_number << ": " << d.name() << endl;

    return d;

}