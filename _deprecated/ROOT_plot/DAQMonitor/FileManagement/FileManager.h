/**
 * @file FileManager.h
 *
 * @brief Helper functions for managing files.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>
#include <fstream>

/**
 * Checks if a directory with the given path exists.
 * 
 * @param path The path to check.
 * 
 * @return True if the directory exists, false otherwise.
 */
bool directoryExists(const std::string &path);

/**
 * Creates a directory at the given path.
 * 
 * @param path The path of the new directory.
 * 
 * @return Whether the directory was created.
 */
bool createDirectory(const std::string &path);

/**
 * Creates a directory at the given path, provided that a directory with that
 * path does not already exist. Should a directory with that path already
 * exist, this function does nothing.
 * 
 * @param path The path of the new directory.
 * 
 * @return Whether the directory was created (i.e. whether it was missing).
 */
bool createIfMissing(const std::string &path);