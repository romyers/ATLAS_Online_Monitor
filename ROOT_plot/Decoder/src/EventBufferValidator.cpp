#include "EventBufferValidator.h"

#include <string>

#include "Logging/ErrorLogger.h"

using namespace std;
using namespace Muon;

///////////////////////////////////////////////////////////////////////////////

const string EVENT_BUFFER_ERROR = "eventBuffer";

///////////////////////////////////////////////////////////////////////////////

EventBufferValidator::EventBufferValidator() : latestEventID(-1) {}

void EventBufferValidator::validate(const vector<Event> &buffer) {

	ErrorLogger &logger = ErrorLogger::getInstance();

	for(const Event &e : buffer) {

		if(latestEventID != -1) {

			if(e.ID() == latestEventID) {

				logger.logError(
					string("WARNING -- Repeated event! Event ID=")
					+ to_string(e.ID()),
					EVENT_BUFFER_ERROR,
					WARNING
				);

			} else if(e.ID() != (latestEventID + 1) % 4096) {

				logger.logError(
					string("WARNING -- Event lost! Current=")
					+ to_string(e.ID())
					+ ", Previous="
					+ to_string(latestEventID),
					EVENT_BUFFER_ERROR,
					WARNING
				);

			}

		}

		latestEventID = e.ID();

	}

}