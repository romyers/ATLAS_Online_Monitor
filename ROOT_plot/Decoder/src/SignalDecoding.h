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

#include <deque>

#include "MuonReco/Signal.h"
#include "MuonReco/Geometry.h"

/**
 * Checks whether the input data has unread signals.
 * 
 * @param in The data to be checked.
 * 
 * @return True if there are complete unread signals in the data, and false
 * otherwise.
 */
bool hasSignals(std::deque<unsigned char> &in);

/**
 * Extracts one signal from the input data.
 * 
 * @param in The data from which a signal should be extracted.
 * 
 * @return The extracted signal.
 */
MuonReco::Signal extractSignal(std::deque<unsigned char> &in);

/**
 * Provides validation of 'sig', logging an error to the error logger
 * and returning false should sig produce an error.
 * 
 * Signals that fail error validation should be dropped.
 * 
 * @param sig The signal to validate.
 * 
 * @param geo The chamber geometry the signal must conform to. Geo should be
 * configured prior to signal validation.
 * 
 * @return False if the signal is invalid and should be discarded, and true
 * otherwise.
 */
bool validateSignalErrors(
	const MuonReco::Signal   &sig, 
	const MuonReco::Geometry &geo
);

/**
 * Provides validation of 'sig', logging any warnings to the error logger.
 * 
 * Signals that fail warning validation should not be dropped.
 * 
 * @param sig The signal to validate.
 * 
 * @param geo The chamber geometry the signal must conform to. Geo should be
 * configured prior to signal validation.
 * 
 */
void validateSignalWarnings(
	const MuonReco::Signal   &sig, 
	const MuonReco::Geometry &geo
);