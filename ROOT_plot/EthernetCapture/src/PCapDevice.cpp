#include "PCapDevice.h"

using namespace std;

PCapDevice::PCapDevice(pcap_if_t *d) 
    : 
    name(d->name? d->name : "(Unknown device)"), 
    description(d->description? d->description : "(No description available)") {}