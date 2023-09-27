/**
 * @file FileAppendMonitor.h
 *
 * @brief Defines a simple class that can be used to check whether a file has
 * been appended to without C++17's filesystem library.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */


#pragma once

#include <fstream>

/**
 * Monitors a file for append operations. Append state may be queried with
 * hasAppend and cleared with clearAppend.
 * 
 * Behavior undefined for overwrites of existing data in the monitored file.
 */
class FileAppendMonitor {

public:

	/**
	 * Constructs a FileAppendMonitor. Files are considered not to have been
	 * appended to at initialization, and will not be considered appended to
	 * until a subsequent append has been performed on the file.
	 * 
	 * @param filename A relative path to the file to be monitored.
     * @param initialAppend Whether the monitor should consider a nonempty file
     * to have been appended to upon initialization.
	 */
    FileAppendMonitor(const std::string &filename, bool initialAppend = false);

    /**
     * Queries the monitor and returns true if (and only if) the file has been
     * appended to since initialization or the last call to clearAppend.
     */
    bool hasAppend();

    /**
     * Clears the append state of the file. Querying the monitor with hasAppend
     * will return false until a subsequent append has been performed on the file.
     */
    void clearAppend();

private:

    std::ifstream in;

};