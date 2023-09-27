/**
 * @file ParseSignal.cpp
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

/**
 * Parses a single word of data into a signal object.
 * 
 * @param rawDataWord A word to parse, assumed to have opposite endianness
 * from the local processor.
 * @param wordSize The length of the word to parse.
 */
Signal ParseSignal(char *rawDataWord, uint8_t wordSize);

// Byte swap from big-endian to little-endian or vice versa
uint64_t byteSwap(char *data, uint8_t dataSize);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Parses a single word of data
Signal ParseSignal(char *rawDataWord, uint8_t wordSize) {

	// Network is big-endian. Local processor is little-endian. Let's byteswap
	// the input to get an endianness our processor can understand
	uint64_t word = byteSwap(rawDataWord, wordSize); 

	// Parse the word using Signal as a wrapper
	Signal sig(word);

	return sig;

}

uint64_t byteSwap(char *data, uint8_t dataSize) {

	uint64_t result = 0;
	for(uint8_t byte = 0; byte < dataSize; ++byte) {
		result += (((uint64_t) data[dataSize - byte - 1]) & 0xff) << byte * 8;
	}

	return result;

}