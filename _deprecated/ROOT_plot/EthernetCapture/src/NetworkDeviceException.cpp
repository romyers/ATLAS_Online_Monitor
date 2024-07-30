#include "NetworkDeviceException.h"

using namespace std;

NetworkDeviceException::NetworkDeviceException(const string &msg) : message(msg) {}

const char *NetworkDeviceException::what() {

    return message.data();

}