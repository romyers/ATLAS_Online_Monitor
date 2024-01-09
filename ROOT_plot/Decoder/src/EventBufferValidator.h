/**
 * @file EventBufferValidator.h
 *
 * @brief Provides validation for a list of events.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>

#include "src/Event.h"

/**
 * A validator for event buffers that keeps track of data that must persist
 * between decode steps for accurate validation.
 */
class EventBufferValidator {

public:

	EventBufferValidator();

	/**
	 * Validates the buffer and logs any errors.
	 * 
	 * @param buffer The event buffer to validate.
	 */
	void validate(const std::vector<Muon::Event> &buffer);

private:

	int latestEventID;

};