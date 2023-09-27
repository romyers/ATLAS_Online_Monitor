/**
 * @file monitorFile.cpp
 *
 * @brief Defines a Root macro that polls an input file for modifications and
 * calls a specified handler whenever the input file is appended to.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

// TODO: Make sure an integral unit of data is written before the monitor reads 
//       the new data. Perhaps we can do this by controlling how EthernetCap 
//       flushes its output? Or perhaps this is fine out of the box?
//         -- Examine: https://stackoverflow.com/a/30133368
//              >> Describes a way to lock files to prevent data races

#include <fstream>

#include "FileAppendMonitor.h"

using namespace std;

/**
 * Returns true if 'in' is currently at the end of its file, and false 
 * otherwise.
 * 
 * @param in The ifstream to check.
 */
bool hasUnreadContent(ifstream &in) {

	// NOTE: in.peek() sets the fail bit if we hit EOF. We will need to clear
	//       it every time.
	return in.peek() != EOF;

}

/**
 * Sets the seeker head of an ifstream to the end of its file.
 * 
 * @param in The ifstream to modify.
 */
void jumpToEndOfFile(ifstream &in) {

    in.seekg(0, in.end);

}


FileAppendMonitor::FileAppendMonitor(const string &filename) : in(filename) {

    clearAppend();

}

bool FileAppendMonitor::hasAppend() {

    if(hasUnreadContent(in)) {
        return true;
    } else {
        in.clear();
        return false;
    }

}

void FileAppendMonitor::clearAppend() {

    jumpToEndOfFile(in);

}