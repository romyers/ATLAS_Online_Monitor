/**
 * @file LockableStream.cpp
 *
 * @brief A container for an iostream that can be locked and unlocked.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <iostream>
#include <mutex>
#include <functional>

using namespace std;

class LockableStream {

public:

	LockableStream();

	// We don't want copying or we'll lose the mutex
	LockableStream(      LockableStream &other) = delete;
	void operator=(const LockableStream &other) = delete;

	void lock  ();
	void unlock();

	iostream *stream;

private:

	mutex m;

};

LockableStream::LockableStream() : stream(nullptr) {}

void LockableStream::lock  () { m.lock  (); }
void LockableStream::unlock() { m.unlock(); }