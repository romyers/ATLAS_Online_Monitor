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
#include <string>

#include "macros/ErrorLogger.cpp"

#include "src/Signal.cpp"
#include "src/Geometry.cpp"

using namespace Muon;
using namespace std;

// Byte swap from big-endian to little-endian or vice versa
uint64_t byteSwap(char *data, uint8_t dataSize);

// An exception thrown when a signal extraction fails
class ExtractionException : public exception {};

/**
 * Reads signals on command from a filestream.
 */
class SignalReader {

public:

	SignalReader(const string &filename);

	bool   isReady      ();
	Signal extractSignal();

private:

	ifstream dataStream;

};

/**
 * Provides validation of 'sig', logging an error to the error logger
 * and returning false should sig be an invalid signal.
 * 
 * @param sig The signal to validate.
 */
bool validateSignal(const Signal &sig);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SignalReader::SignalReader(const string &filename) :
	dataStream(filename) {}

bool SignalReader::isReady() {

	// Get initial and end positions for the stream
	streampos initialPos = dataStream.tellg();
	dataStream.seekg(0, dataStream.end);
	streampos endPos = dataStream.tellg();

	// Reset the stream
	dataStream.clear();
	dataStream.seekg(initialPos, dataStream.beg);

	// Return true if there are at least WORD_SIZE characters left on the
	// stream
	return (endPos - initialPos) >= Signal::WORD_SIZE;

}

Signal SignalReader::extractSignal() {

	char readBuffer[Signal::WORD_SIZE];

	dataStream.read(readBuffer, Signal::WORD_SIZE);

	uint64_t word = byteSwap(readBuffer, Signal::WORD_SIZE);

	return Signal(word);

}

// TODO: Pull out validators
// TODO: Test validators
// TODO: We don't have event numbers, which we need to have for error reporting
// TODO: Combine this with signal buffer validation. Either this can be done
//       with access to the buffer or buffer validation can be done here.
//       If it happens in buffer validation, we can still keep track of signal
//       count.
bool validateSignal(const Signal &sig) {

	if(sig.Type() == Signal::HEADER)  return true;
	if(sig.Type() == Signal::TRAILER) return true;

	ErrorLogger &logger = *ErrorLogger::getInstance();
	Geometry    &geo    = *Geometry::getInstance   ();

	if(!geo.IsActiveTDC(sig.TDC())) {

		string msg = "ERROR -- Unexpected data TDCID = ";
		msg += to_string(sig.TDC());
		msg += ", Channel = ";
		msg += to_string(sig.Channel());

		logger.logError(msg);

		return false;

	}

	if(sig.Channel() == Signal::TDC_HEADER_CHNL) {

		return true;

	}

	if(sig.Channel() == Signal::TDC_TRAILER_CHNL) {

		return true;

	}

	if(sig.Channel() == Signal::TDC_ERROR_CHNL) {

		// TODO: Extract error info

		return false;

	}

	if(sig.Channel() >= Geometry::MAX_TDC_CHANNEL) {

		string msg = "ERROR -- Unexpected data TDCID = ";
		msg += to_string(sig.TDC());
		msg += ", Channel = ";
		msg += to_string(sig.Channel());

		logger.logError(msg);

		return false;

	}

	return true;

}

uint64_t byteSwap(char *data, uint8_t dataSize) {

	uint64_t result = 0;
	for(uint8_t byte = 0; byte < dataSize; ++byte) {
		result += (((uint64_t) data[dataSize - byte - 1]) & 0xff) << byte * 8;
	}

	return result;

}