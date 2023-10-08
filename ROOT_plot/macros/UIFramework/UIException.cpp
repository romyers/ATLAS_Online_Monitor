/**
 * @file UIException.cpp
 *
 * @brief An exception class for the UI framework.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <exception>

using namespace std;

class UIException : public exception {

public:

    UIException(const string &msg) : message(msg) {}

    const char *what() {

        return message.data();

    }

private:

    string message;

};