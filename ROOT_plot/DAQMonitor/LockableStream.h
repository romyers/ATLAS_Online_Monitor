/**
 * @file LockableStream.h
 *
 * @brief A container for an iostream that can be locked and unlocked.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <iostream>
#include <mutex>

class LockableStream {

public:

	LockableStream();

	// We don't want copying or we'll lose the mutex
	LockableStream(      LockableStream &other) = delete;
	void operator=(const LockableStream &other) = delete;

	void lock  ();
	void unlock();

	std::iostream *stream;

private:

	std::mutex m;

};