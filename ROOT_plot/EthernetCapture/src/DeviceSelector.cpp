#include "DeviceSelector.h"

#include <iostream>

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