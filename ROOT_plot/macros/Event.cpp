/**
 * @file Event.cpp
 *
 * @brief Wrapper for an sMDT chamber event. Not to be confused with the event
 * class in src/Event.cpp authored by Kevin Nelson.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <stdio.h>

#include "src/Signal.cpp"

struct EventData {

	unsigned int eventID                  = 0;
	unsigned int eventSignals             = 0;
	unsigned int errorWordCount           = 0;
	unsigned int tdcHeaderEventIdCurrent  = 0;
	unsigned int tdcHeaderEventIdPrevious = -1;
	unsigned int headerCountTdcData       = 0;
	unsigned int trailerCountTdcData      = 0;

};

/**
 * Container for the set of Signal objects defining an sMDT event, starting
 * with a header signal and ending with a trailer signal.
 * 
 * Events are expected to begin with a header signal and end with a trailer 
 * signal.
 */
class Event {

public:

	Event();

	void open     (Signal header);
	void close    (Signal trailer);
	void addSignal(Signal sig);

	bool isOpen() const;

	unsigned int ID() const;

	void debug_print();

private:

	EventData data;

	vector<Signal> signals;
	bool           inProgress;

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Event::Event() : inProgress(false) {}

void Event::open(Signal header) {

	if(isOpen()) {

		cerr << "ERROR: Event received multiple headers" << endl;
		exit(1);

	}

	if(header.Type() != Signal::HEADER) {

		cerr << "ERROR: Events must be opened with a header signal" << endl;
		exit(1);

	}

	data.eventID = header.HeaderEID();
	// TODO: Compare this event ID with the one before it -- probably in
	//       EventBuffer.cpp

	inProgress = true;

	signals.push_back(header);


}

void Event::close(Signal trailer) {

	if(!isOpen()) {

		cerr << "ERROR: Event was closed twice" << endl;
		exit(1);

	}

	if(trailer.Type() != Signal::TRAILER) {

		cerr << "ERROR: Events must be closed with a trailer signal" << endl;
		exit(1);

	}

	signals.push_back(trailer);

	inProgress = false;

}

void Event::addSignal(Signal sig) {

	if(sig.Type() == Signal::HEADER) {

		cerr << "ERROR: Tried to add a header signal without opening the event." << endl;
		exit(1);

	}

	if(sig.Type() == Signal::TRAILER) {

		cerr << "ERROR: Tried to add a trailer signal without closing the event." << endl;
		exit(1);

	}

	if(!isOpen()) {

		cerr << "ERROR: Event received body before header" << endl;
		exit(1);

	}

	signals.push_back(sig);

}

bool Event::isOpen() const {

	return inProgress;

}

unsigned int Event::ID() const {

	return data.eventID;

}

void Event::debug_print() {

	cout << "------------------------------------" << endl;
	cout << "PRINTING EVENT:" << endl << endl;

	for(Signal &sig : signals) {

		cout << "PRINTING SIGNAL:" << endl;
		cout << "Header:" << endl;
		cout << "\t" << sig.Type() << endl;
		cout << endl;

	}

}