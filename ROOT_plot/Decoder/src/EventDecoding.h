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

#include "src/Signal.h"
#include "src/Event.h"

/**
 * Creates an event from the given signals.
 * 
 * @param signals A vector of Signals representing exactly one well-formed
 * event.
 * 
 * @return The assembled event.
 */
Muon::Event assembleEvent(std::vector<Muon::Signal>  signals);

/**
 * Checks if an event has no errors that require the event to be discarded.
 * 
 * @param e The event to check.
 * 
 * @return True if e had no errors, and false otherwise.
 */
bool validateEventErrors(const Muon::Event &e);

/**
 * Checks if an event has no errors that do not require the event to be 
 * discarded.
 * 
 * @param e The event to check.
 * 
 * @return True if e had no errors, and false otherwise.
 */
void validateEventWarnings(const Muon::Event &e);

/**
 * Performs hit finding and other processing on the event.
 * 
 * @param e The event to process.
 */
void processEvent(Muon::Event &e);