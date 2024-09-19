#include "SignalDecoding.h"

#include <stdio.h>
#include <string>
#include <istream>

#include <iostream>

#include "Logging/ErrorLogger.h"

using namespace MuonReco;
using namespace std;

const string SIGNAL_ERROR     = "signal"      ;
const string TDC_DECODE_ERROR = "tdcDecodeErr";

// Byte swap from big-endian to little-endian or vice versa
uint64_t byteSwap(uint64_t data, uint8_t dataSize);

// Check if this system is little-endian
bool systemIsLittleEndian();

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool hasSignals(deque<unsigned char> &in) {

	return in.size() >= Signal::WORD_SIZE;

}

// TODO: It's faster if we read all available signals at once and parse them
//       into signals later
// TODO: Remember the power of memcpy and static casting for parsing things
//         -- build the right struct and you can memcpy right into it very fast
Signal extractSignal(deque<unsigned char> &in) {

	// Set aside some space to store the data word
	char readBuffer[Signal::WORD_SIZE];

	// Pop the word off the deque
	for(int i = 0; i < Signal::WORD_SIZE; ++i) {

		readBuffer[i] = in.front();
		in.pop_front();

	}

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

bool validateSignalErrors(const Signal &sig, const Geometry &geo) {

	if(sig.isEventHeader ()) return true;
	if(sig.isEventTrailer()) return true;

	if(sig.isTDCDecodeErr())  return true;

	ErrorLogger &logger = ErrorLogger::getInstance();

	// NOTE: It's important that we bounds-check the TDC number before we
	//       pass it to geo.IsActiveTDC(), since IsActiveTDC() will not
	//       bounds-check the TDC number itself. If we don't, we should
	//       expect an out of bounds array access for bad TDC numbers.
	
	if(sig.TDC() >= Geometry::MAX_TDC || !geo.IsActiveTDC(sig.TDC())) {

		string msg = "Unexpected data TDCID = ";
		msg += to_string(sig.TDC());
		msg += ", Channel = ";
		msg += to_string(sig.Channel());

		logger.logError(msg, SIGNAL_ERROR, ERROR);

		return false;

	}

	// In any of these cases, we have a valid signal, but we'll find a channel
	// ID that is not considered active.
	if(sig.isTDCHeader  ()) return true;
	if(sig.isTDCTrailer ()) return true;
    if(sig.isTDCOverflow()) return true;

	if(
		sig.Channel() >= Geometry::MAX_TDC_CHANNEL || 
		!geo.IsActiveTDCChannel(sig.TDC(), sig.Channel())
	) {

		string msg = "Unexpected data TDCID = ";
		msg += to_string(sig.TDC());
		msg += ", Channel = ";
		msg += to_string(sig.Channel());

		logger.logError(msg, SIGNAL_ERROR, ERROR);

		return false;

	}

	return true;

}

void validateSignalWarnings(const Signal &sig, const Geometry &geo) {

	ErrorLogger &logger = ErrorLogger::getInstance();
    
    if(sig.isTDCDecodeErr()) { 

		string msg = "Decoding error occured!";
		logger.logError(msg, TDC_DECODE_ERROR, ERROR);

	}

	if(sig.isTDCOverflow()) {

		string msg = "";

		TDCErrorData errorData = sig.getTDCError();

		msg = "Received TDC Error Signal:";

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

		logger.logError(msg, SIGNAL_ERROR, ERROR);

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