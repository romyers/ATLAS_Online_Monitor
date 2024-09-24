#include "EventBufferValidator.h"

#include <string>

#include "Logging/ErrorLogger.h"

using namespace std;
using namespace MuonReco;

///////////////////////////////////////////////////////////////////////////////

const string EVENT_BUFFER_ERROR = "eventBuffer";

///////////////////////////////////////////////////////////////////////////////

EventBufferValidator::EventBufferValidator() : latestEventID(-1) {}

void EventBufferValidator::validateWarnings(const vector<Event> &buffer) {

	ErrorLogger &logger = ErrorLogger::getInstance();

	for(const Event &e : buffer) {
				
		if(e.TrigSignals().at(0).TrigOverflow()) {
			
			logger.logError(
				string("Trigger buffer overflow before event ")
				+ to_string(e.TrigSignals().at(0).HeaderEID()),
				EVENT_BUFFER_ERROR,
				WARNING
			);

		} 

		if(latestEventID != -1) {

			if(e.ID() == latestEventID) {

				logger.logError(
					string("Repeated event! Event ID=")
					+ to_string(e.ID()),
					EVENT_BUFFER_ERROR,
					WARNING
				);

			} else {

				if(e.ID() != (latestEventID + 1) % 4096) {

					logger.logError(
						string("Event lost! Current=")
						+ to_string(e.ID())
						+ ", Previous="
						+ to_string(latestEventID),
						EVENT_BUFFER_ERROR,
						WARNING
					);

				}
			}

		}

		latestEventID = e.ID();

	}

}