/**
 * @file EventBuffer.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <queue>
#include <stdio.h>

#include "Event.cpp"

#include "src/Signal.cpp"

class EventBuffer {

public:

    EventBuffer();

    Event front();
    Event pop();

    void registerSignal(Signal sig);

    bool empty() const;

private:

    queue<Event> events;

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

EventBuffer::EventBuffer() {};

Event EventBuffer::front() {

    if(empty()) {

        cerr << "Cannot read an empty EventBuffer." << endl;
        exit(1);

    }

    return events.front();

}

Event EventBuffer::pop() {

    if(empty()) {

        cerr << "Cannot pop from an empty EventBuffer." << endl;
        exit(1);

    }

    Event front = events.front();
    events.pop();

    return front;

}

void EventBuffer::registerSignal(Signal sig) {


    if(sig.Type() == Signal::HEADER) {

        Event e;
        e.open(sig);

        // Check for repeated or lost events
        if(!empty()) {

            unsigned int currentID  = e.ID();
            unsigned int previousID = events.back().ID();

            if(currentID == previousID) {
                cout << "WARNING -- Repeating event ID: " << currentID << endl;
            } else if(currentID != (previousID + 1) % 4096) {
                cout << "WARNING -- Events lost! Curr=" << currentID 
                     << ", Pre=" << previousID << endl;
            }
            
        }

        events.push(Event());
        events.back().open(sig);

    } else if(sig.Type() == Signal::TRAILER) {

        events.back().close(sig);

    } else {

        events.back().addSignal(sig);

    }

}

bool EventBuffer::empty() const {

    return events.empty();

}