/**
 * @file UIException.h
 *
 * @brief An exception class for the UI framework.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>
#include <exception>

class UIException : public std::exception {

public:

    UIException(const std::string &msg);

    const char *what();

private:

    std::string message;

};