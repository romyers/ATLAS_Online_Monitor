/**
 * @file SignalDecoding.cpp
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

// TODO: This does three different things. Break it up.

const string SIGNAL_ERROR = "signalErr" ;
const string SIGNAL_WARN  = "signalWarn";

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
 * and returning false should sig produce an error.
 * 
 * Signals that fail error validation should be dropped.
 * 
 * @param sig The signal to validate.
 */
bool validateSignalErrors(const Signal &sig);

/**
 * Provides validation of 'sig', logging any warnings to the error logger.
 * 
 * Signals that fail warning validation should not be dropped.
 * 
 * @param sig The signal to validate.
 */
void validateSignalWarnings(const Signal &sig);

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
bool validateSignalErrors(const Signal &sig) {

	if(sig.isEventHeader ()) return true;
	if(sig.isEventTrailer()) return true;

	ErrorLogger &logger = *ErrorLogger::getInstance();
	Geometry    &geo    = *Geometry::getInstance   ();

	if(!geo.IsActiveTDC(sig.TDC())) {

		string msg = "ERROR -- Unexpected data TDCID = ";
		msg += to_string(sig.TDC());
		msg += ", Channel = ";
		msg += to_string(sig.Channel());

		logger.logError(msg, SIGNAL_ERROR);

		return false;

	}

	if(sig.isTDCError()) {

		string msg = "";

		TDCErrorData errorData = sig.getTDCError();

		if(errorData.LSBFlag1 > 0) {

			msg += "ERROR -- TDCID = ";
			msg += errorData.TDC;
			msg += ", Channel = ";
			msg += errorData.LSBChannel1;
			msg += " overflowed!";

		}

		if(errorData.LSBFlag2 > 0) {

			if(!msg.empty()) msg += "\n";

			msg += "ERROR -- TDCID = ";
			msg += errorData.TDC;
			msg += ", Channel = ";
			msg += errorData.LSBChannel2;
			msg += " overflowed!";

		}

		logger.logError(msg, SIGNAL_ERROR);

		return false;

	}

	if(sig.isTDCHeader()) {

		// TODO: Validation

		return true;

	}

	if(sig.isTDCTrailer()) {

		// TODO: Validation

		return true;

	}

	if(sig.Channel() < Geometry::MAX_TDC_CHANNEL) return true;


	string msg = "ERROR -- Unexpected data TDCID = ";
	msg += to_string(sig.TDC());
	msg += ", Channel = ";
	msg += to_string(sig.Channel());

	logger.logError(msg, SIGNAL_ERROR);

	return false;

}

void validateSignalWarnings(const Signal &sig) {



}

uint64_t byteSwap(char *data, uint8_t dataSize) {

	uint64_t result = 0;
	for(uint8_t byte = 0; byte < dataSize; ++byte) {
		result += (((uint64_t) data[dataSize - byte - 1]) & 0xff) << byte * 8;
	}

	return result;

}