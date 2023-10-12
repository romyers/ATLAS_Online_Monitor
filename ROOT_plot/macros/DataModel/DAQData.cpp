/**
 * @file DAQData.cpp
 *
 * @brief Singleton state storage for data retrieved from the DAQ. 
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 * 
 * CAUTION: DAQData is not innately threadsafe, and must be locked
 * before modification to ensure thread safety. Locking and unlocking
 * functions are provided.
 */

#pragma once

#include <vector>

#include "macros/DataModel/Plots.cpp"

#include "src/Event.cpp"

using namespace std;
using namespace Muon;

struct DAQData {

    vector<Event> processedEvents;

    Plots plots;

    void lock  () const;
    void unlock() const;

    DAQData       (      DAQData &other) = delete;
    void operator=(const DAQData &other) = delete;

    static DAQData &getInstance();

private:

    DAQData();

    mutable mutex dataLock;

};

DAQData::DAQData() {}

DAQData &DAQData::getInstance() {

    static DAQData data;

    return data;

}

void DAQData::lock  () const { dataLock.lock  (); }
void DAQData::unlock() const { dataLock.unlock(); }