/**
 * @file NetworkDeviceException.h
 *
 * @brief Provides an exception class for network device errors.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <exception>
#include <string>

class NetworkDeviceException : public std::exception {

public:

    NetworkDeviceException(const std::string &msg);

    const char *what();

private:

    std::string message;

};