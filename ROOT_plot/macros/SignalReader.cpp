/**
 * @file SignalReader.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <stdio.h>

#include "src/Signal.cpp"

using namespace Muon;
using namespace std;

// Byte swap from big-endian to little-endian or vice versa
uint64_t byteSwap(char *data, uint8_t dataSize);

// An exception thrown when a signal extraction fails
class ExtractionException : public exception {};

/**
 * 
 */
class SignalReader {

public:

	SignalReader(const string &filename);

	Signal readSignal();

private:

	ifstream dataStream;

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SignalReader::SignalReader(const string &filename) :
	dataStream(filename) {}

Signal SignalReader::readSignal() {

	streampos initialPos = dataStream.tellg();
	char readBuffer[Signal::WORD_SIZE];

	if(!dataStream.read(readBuffer, Signal::WORD_SIZE)) {

		dataStream.clear();
		dataStream.seekg(initialPos, dataStream.beg);

		throw ExtractionException();

	}

	uint64_t word = byteSwap(readBuffer, Signal::WORD_SIZE);

	return Signal(word);

}

uint64_t byteSwap(char *data, uint8_t dataSize) {

	uint64_t result = 0;
	for(uint8_t byte = 0; byte < dataSize; ++byte) {
		result += (((uint64_t) data[dataSize - byte - 1]) & 0xff) << byte * 8;
	}

	return result;

}