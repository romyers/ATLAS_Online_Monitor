#include "LockableStream.h"

#include "FileManagement/FileManager.h"

using namespace std;

string dir(const string &filename);

LockableStream::LockableStream(size_t cacheSize) : cache(cacheSize) {}

void LockableStream::lock  () { m.lock  (); }
void LockableStream::unlock() { m.unlock(); }

LockableStream::~LockableStream() {

	close();

}

void LockableStream::close() {

	if(out.is_open()) out.close();
	if(in.is_open()) in.close();

}

void LockableStream::open(const string &filename) {

	createIfMissing(dir(filename));

	// Important to open the output file first so the file will be
	// created if it doesn't exist.
	out.open(filename, ios::binary | ios::app);
	in.open(filename, ios::binary);
}

bool LockableStream::is_open() {

	return in.is_open() && out.is_open();

}

size_t LockableStream::read(char *buffer, size_t size) {

	size_t bytesFromCache = cache.get((unsigned char*)buffer, size);
	cache.erase(bytesFromCache);

	in.seekg(bytesFromCache, in.cur);
	in.read(buffer,  size - bytesFromCache);

	size_t bytesRead = in.gcount() + bytesFromCache;

	in.clear();

	if(cache.empty()) {
		
		// This will repopulate the cache from whatever is left in the file,
		// and behaves nicely with respect to cache size and file size.
		in >> cache;

	}

	return bytesRead;

}

size_t LockableStream::getCacheCapacity() const {

	return cache.capacity();

}

size_t LockableStream::getCacheUsage() const {

	return cache.size();

}

bool LockableStream::write(const char *buffer, size_t size) {

	cache.insert((unsigned char*)buffer, size);

	out.write(buffer, size);

	return out.good();

}

void LockableStream::flush() {

	out.flush();

}

size_t LockableStream::fileSize() {

	streampos pos = in.tellg();

	in.seekg(0, in.end);
	size_t size = static_cast<size_t>(in.tellg());

	in.clear();
	in.seekg(pos, in.beg);

	return size;

}

size_t LockableStream::unreadBytes() {

	streampos pos = in.tellg();

	in.seekg(0, in.end);
	size_t size = static_cast<size_t>(in.tellg());

	in.clear();
	in.seekg(pos, in.beg);

	return size - static_cast<size_t>(pos) + cache.size();

}

string dir(const string &filename) {

	size_t pos = filename.find_last_of("/");

	if(pos == string::npos) return "";

	return filename.substr(0, pos);

}