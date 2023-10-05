/**
 * @file NetworkDeviceException.cpp
 *
 * @brief Provides an exception class for network device errors.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <exception>
#include <string>

using namespace std;

class NetworkDeviceException : public exception {

public:

    NetworkDeviceException(const string &msg) : message(msg) {}

    const char *what() {

        return message.data();

    }

private:

    string message;

};