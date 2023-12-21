/**
 * @file SignalDecoding.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "DAQMonitor/LockableStream.h"

#include "src/Signal.h"

/**
 * Reads signals on command from a filestream.
 */
class SignalReader {

public:

	SignalReader(LockableStream &in);

	bool   isReady      ();
	Muon::Signal extractSignal();

private:

	LockableStream &dataStream;

};

/**
 * Provides validation of 'sig', logging an error to the error logger
 * and returning false should sig produce an error.
 * 
 * Signals that fail error validation should be dropped.
 * 
 * @param sig The signal to validate.
 */
bool validateSignalErrors(const Muon::Signal &sig);

/**
 * Provides validation of 'sig', logging any warnings to the error logger.
 * 
 * Signals that fail warning validation should not be dropped.
 * 
 * @param sig The signal to validate.
 */
void validateSignalWarnings(const Muon::Signal &sig);