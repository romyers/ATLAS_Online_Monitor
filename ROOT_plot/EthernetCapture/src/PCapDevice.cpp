#include "PCapDevice.h"

using namespace std;

PCapDevice::PCapDevice(pcap_if_t *d) : dev(d) {}

char *PCapDevice::name() {

    return dev->name;

}

char *PCapDevice::description() {

    return dev->description;

}