/**
 * @file SignalDecoding.h
 *
 * @brief Provides functionality for decoding raw data from an istream into
 * signal objects and for validating signal objects.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <iostream>

#include "src/Signal.h"

/**
 * Checks whether the input stream has unread signals.
 * 
 * @param in The stream to be checked.
 */
bool hasSignals(std::istream &in);

/**
 * Extracts one signal from the input stream.
 * 
 * @param in The stream from which a signal should be extracted.
 */
Muon::Signal extractSignal(std::istream &in);

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