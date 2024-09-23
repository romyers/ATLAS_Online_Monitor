/**
 * @file LockableStream.h
 *
 * @brief A container for an iostream that can be locked and unlocked.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <mutex>
#include <vector>

class LockableData {

public:

	LockableData();

	// We don't want copying or we'll lose the mutex
	LockableData  (      LockableData &other) = delete;
	void operator=(const LockableData &other) = delete;

	void lock  ();
	void unlock();

	std::vector<unsigned char> data;

private:

	std::mutex m;

};