#include "LockableStream.h"

#include "FileManagement/FileManager.h"

using namespace std;

// TODO: Something goes wrong when the buffer fills up. Figure it out.

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
	this->filename = filename;
	this->filename.erase(this->filename.size() - 4);  //remove .dat
	// Attempt to create the directory
    if (mkdir(this->filename.c_str(), 0777) != 0) {
        if (errno == EEXIST) {
            // Directory already exists, no issue
            std::cout << "Directory already exists: " << this->filename << std::endl;
        } else {
            // Other errors (e.g., permission issues)
            std::cerr << "Error: Failed to create directory " << this->filename << ". Reason: " << strerror(errno) << std::endl;
            throw std::runtime_error("Failed to create directory");
        }
    } else {
        std::cout << "Directory created successfully: " << this->filename << std::endl;
    }
	subfileindex = 0;
}

bool LockableStream::is_open() {

	return in.is_open() && out.is_open();

}

vector<unsigned char> LockableStream::get(size_t size) {

	vector<unsigned char> buffer(size);

	buffer.resize(read((char*)buffer.data(), size));

	return buffer;

}

size_t LockableStream::read(char *buffer, size_t size) {

	size_t bytesFromCache = cache.get((unsigned char*)buffer, size);
	cache.erase(bytesFromCache);

	// Problem to address -- sometimes in.cur is already at the end of the cache
	// (if we've repopulated the cache from the file) and sometimes it isn't 
	// (if we populated the cache directly)
	// in.seekg(bytesFromCache, in.cur);
	in.read(buffer + bytesFromCache,  size - bytesFromCache);

	size_t bytesRead = in.gcount() + bytesFromCache;

	in.clear();

	if(cache.empty()) {
		
		// This will repopulate the cache from whatever is left in the file,
		// and behaves nicely with respect to cache size and file size.
		in >> cache;
		// in.seekg(-cache.size(), in.cur);

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

	size_t bytesCached = cache.insert((unsigned char*)buffer, size);

	out.write(buffer, size);
	out.flush();

	// Jump input cursor past the bytes we cached, since they should not
	// be read again.
	in.seekg(bytesCached, in.cur);


    std::ofstream temp;
    temp.open(this->filename + "/" + std::to_string(subfileindex), std::ios::binary | std::ios::app);
    if (!temp.is_open()) { // Check if the file was opened successfully
        std::cerr << "Error: Unable to open temp file"<<this->filename<<"/"<< std::to_string(subfileindex)<<" for writing." << std::endl;
        return false;
    }
    subfileindex += 1;

    temp.write(buffer, size);
    temp.close();
    
	return out.good();

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