/**
 * @file LockableStream.h
 *
 * @brief A container for an iostream that can be locked and unlocked.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "RingBuffer.h"

#include <mutex>
#include <fstream>
#include <vector>

class LockableStream {

public:

	LockableStream(size_t cacheSize = 0);
	~LockableStream();

	// Creates the file if it doesn't exist and opens it for IO
	void open(const std::string &filename);
	void close();

	// We don't want copying or we'll lose the mutex
	LockableStream  (      LockableStream &other) = delete;
	void operator=(const LockableStream &other) = delete;

	void lock  ();
	void unlock();

	size_t fileSize();

	size_t unreadBytes();

	bool is_open();

	// Will read as many bytes as are available up to size, and
	// return the number of bytes read.
	size_t read(char *buffer, size_t size);
	
	bool write(const char *buffer, size_t size);

	std::vector<unsigned char> get(size_t size);

	size_t getCacheCapacity() const;

	size_t getCacheUsage() const;

private:

	std::ifstream in;
	std::ofstream out;

	std::mutex m;

	RingBuffer<unsigned char> cache;

};