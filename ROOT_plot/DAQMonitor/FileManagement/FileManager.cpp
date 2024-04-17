/**
 * @file FileManager.cpp
 *
 * @brief Helper functions for managing directories.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

// TODO: Portability. Right now this only works on Linux.

#include "FileManager.h"

#include <vector>
#include <iostream>
#include <algorithm>

#include <sys/stat.h>

using namespace std;

bool directoryExists(const string &path) {

    struct stat sb;

    if(stat(path.data(), &sb) == 0) {

        return true;

    }

    return false;

}

bool createDirectory(const string &path) {

    if(mkdir(path.data(), 0777) == 0) return true;

    return false;

}

bool createIfMissing(const string &path) {

    if(!directoryExists(path)) {

        createDirectory(path);

        return true;

    }

    return false;

}