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
MuonReco::Event assembleEvent(std::vector<MuonReco::Signal> signals);

/**
 * Checks if an event has no errors that require the event to be discarded.
 * 
 * @param e The event to check.
 * 
 * @return True if e had no errors, and false otherwise.
 */
bool validateEventErrors(const MuonReco::Event &e);

/**
 * Checks if an event has no errors that do not require the event to be 
 * discarded.
 * 
 * NOTE: TDC Error signals are dropped before event assembly, but are
 * counted in the event trailer's hit count. In this case, the event will
 * trigger a hit count mismatch warning.
 * 
 * As long as the difference in hit counts matches the number of TDC Error
 * signals dropped, this warning should be discarded.
 * 
 * If a TDC error signal is dropped but an event mismatch warning is not
 * triggered, then the event actually does exhibit a true hit count
 * mismatch.
 * 
 * @param e The event to check.
 * 
 * @return True if e had no errors, and false otherwise.
 */
void validateEventWarnings(const MuonReco::Event &e);

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