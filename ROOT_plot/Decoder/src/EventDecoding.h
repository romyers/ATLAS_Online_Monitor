/**
 * @file EventDecoding.h
 *
 * @brief Provides functionality for assembling, validating, and processing
 * event objects.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>

#include "MuonReco/Signal.h"
#include "MuonReco/Event.h"
#include "MuonReco/Geometry.h"
#include "MuonReco/TimeCorrection.h"
#include "MuonReco/RecoUtility.h"

/**
 * Creates an event from the given signals.
 * 
 * REQUIRES: signals must have at least two elements.
 * 
 * @param signals A vector of Signals representing exactly one well-formed
 * event. A vector of Signals representing a well-formed event must respect
 * the following invariants:
 *   -- The first signal in the vector must be an event header
 *   -- The last signal in the vector must be an event trailer
 *   -- There must be no other event headers or trailers
 * Any vector with size at least 2 that does not adhere to these invariants 
 * will fail validation, so as long as any event produced with assembleEvent 
 * is validated using validateEventErrors() and validateEventWarnings(), 
 * results should be well-defined.
 * 
 * CAUTION: Results are undefined for signals.size() < 2.
 * 
 * @return The assembled event.
 */
MuonReco::Event assembleEvent(const std::vector<MuonReco::Signal> &signals);

/**
 * Checks if a signal buffer meant to represent an event has any errors
 * that would require the event to be dropped.
 * 
 * @param signals The signals to check.
 * 
 * @return True if signals had no errors, and false otherwise.
 */
bool validateEventErrors(const std::vector<MuonReco::Signal> &signals);

/**
 * Checks if a signal buffer meant to represent an event has any warnings
 * that would not require the event to be dropped.
 * 
 * @param signals The signals to check.
 * 
 * @return True if signals had no warnings, and false otherwise.
 */
void validateEventWarnings(const std::vector<MuonReco::Signal> &signals);

/**
 * Performs hit finding and other processing on the event.
 * 
 * @param e The event to process.
 */
void processEvent(
	MuonReco::Event          &e       , 
	MuonReco::Geometry       &geo     , 
	MuonReco::TimeCorrection &tc      ,
	MuonReco::RecoUtility    &recoUtil
);

/**
 * Removes TDC Header, Trailer, Overflow, and Error signals from signals.
 * 
 * REQUIRES: signals must have at least two elements.
 * 
 * @param signals A pre-validated vector of signals with at least two elements.
 */
void removeTDCSignals(
    std::vector<MuonReco::Signal> &signals
);