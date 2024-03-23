#include "SignalDecoding.h"

#include <stdio.h>
#include <string>
#include <istream>

#include <iostream>
#include <chrono>
#include <ctime>  

#include "Logging/ErrorLogger.h"

#include "src/Geometry.h"

using namespace Muon;
using namespace std;

const string SIGNAL_ERROR = "signal" ;

// Byte swap from big-endian to little-endian or vice versa
uint64_t byteSwap(uint64_t data, uint8_t dataSize);

// Check if this system is little-endian
bool systemIsLittleEndian();

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool hasSignals(istream &in) {

	// TODO: What if no in.end?

	// Get initial and end positions for the stream
	streampos initialPos = in.tellg();
	in.seekg(0, in.end);
	streampos endPos = in.tellg();

	// Reset the stream
	in.clear();
	in.seekg(initialPos, in.beg);

	// Return true if there are at least WORD_SIZE characters left on the
	// stream
	return (endPos - initialPos) >= Signal::WORD_SIZE;

}

// TODO: It's faster if we read all available signals at once and parse them
//       into signals later
// TODO: Remember the power of memcpy and static casting for parsing things
//         -- build the right struct and you can memcpy right into it very fast
Signal extractSignal(istream &in) {

	// Set aside some space to store the data word
	char readBuffer[Signal::WORD_SIZE];

	// Read the data word
	in.read(readBuffer, Signal::WORD_SIZE);

	// Copy the data word into an int
	uint64_t word = 0;
	memcpy(&word, readBuffer, Signal::WORD_SIZE);

	// Convert the data word to the correct endianness
	if(systemIsLittleEndian()) {

		word = byteSwap(word, Signal::WORD_SIZE);

	}

	// Return a new signal constructed from the data word.
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

	ErrorLogger &logger = ErrorLogger::getInstance();
	Geometry    &geo    = Geometry::getInstance   ();

	if(sig.isTDCDecodeErr()) {  //should be handled first
		string msg = "ERROR -- Decoding error occured!";
		logger.logError(msg, SIGNAL_ERROR, FATAL);

		return false;
	}

	if(!geo.IsActiveTDC(sig.TDC())) {
		auto end = std::chrono::system_clock::now();
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);
		string msg;
		msg += std::ctime(&end_time);
		msg += "ERROR -- Unexpected data TDCID = ";
		msg += to_string(sig.TDC());
		msg += ", Channel = ";
		msg += to_string(sig.Channel());

		logger.logError(msg, SIGNAL_ERROR, FATAL);

		return false;

	}

	// In any of these cases, we have a valid signal, but we'll find a channel
	// ID that is not considered active.
	if(sig.isTDCHeader ()) { return true; }
	if(sig.isTDCTrailer()) { return true; }
	if(sig.isTDCOverflow()) { return true; }



	if(!geo.IsActiveTDCChannel(sig.TDC(), sig.Channel())) {

		auto end = std::chrono::system_clock::now();
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);
		string msg;
		msg += std::ctime(&end_time);
		msg += "ERROR -- Unexpected data TDCID = ";
		msg += to_string(sig.TDC());
		msg += ", Channel = ";
		msg += to_string(sig.Channel());

		logger.logError(msg, SIGNAL_ERROR, FATAL);

		return false;

	}

	

	return true;

}

void validateSignalWarnings(const Signal &sig) {

	// Skip cases that are handled by error validation to avoid double-errors
	if(sig.isEventHeader ()) return;
	if(sig.isEventTrailer()) return;

	if(!Geometry::getInstance().IsActiveTDC(sig.TDC())) return;

	ErrorLogger &logger = ErrorLogger::getInstance();

	if(sig.isTDCOverflow()) {

		string msg = "";

		TDCErrorData errorData = sig.getTDCError();

		msg = "WARNING -- Received TDC Error Signal:";

		if(errorData.LSBFlag1 > 0) {

			msg += "\n         \tTDCID = ";
			msg += to_string(errorData.TDC);
			msg += ", Channel = ";
			msg += to_string(errorData.LSBChannel1);
			msg += " overflowed!";

		} if(errorData.LSBFlag2 > 0) {

			msg += "\n         \tTDCID = ";
			msg += to_string(errorData.TDC);
			msg += ", Channel = ";
			msg += to_string(errorData.LSBChannel2);
			msg += " overflowed!";

		} 

		logger.logError(msg, SIGNAL_ERROR, WARNING);

	}

}

uint64_t byteSwap(uint64_t data, uint8_t dataSize) {

	uint64_t result = 0;
	for(uint8_t byte = 0; byte < dataSize; ++byte) {

		uint64_t temp = data >> 8 * (dataSize - byte - 1);
		temp = temp & 0xff;

		result += temp << 8 * byte;

	}

	return result;

}

// SOURCE: https://stackoverflow.com/a/1001373
bool systemIsLittleEndian() {

	static union {

		uint16_t i;
		char c[2];

	} bint = { 0x0201 };

	return bint.c[0] == 1;

}