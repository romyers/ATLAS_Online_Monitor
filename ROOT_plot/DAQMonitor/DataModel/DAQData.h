/**
 * @file DAQData.h
 *
 * @brief Singleton storage for data retrieved from the DAQ. 
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
#include <mutex>

#include "Plots.h"
#include "src/Event.h"

struct DAQData {

    // A vector of fully processed and populated events, with hit finding
    // already performed. Excludes empty events.
    std::vector<Muon::Event> processedEvents;

    // A vector of the fully processed and populated events decoded in the
    // last decode iteration, with hit finding already performed.
    // Excludes empty events.
    std::vector<Muon::Event> newEvents;

    // A struct of histograms containing aggregate event data. See
    // src/DataModel/Plots.cpp.
    Plots plots;

    int totalEventCount = 0;
    int packetCount     = 0;

    int lostPackets     = 0;

    int droppedSignals  = 0;
    int droppedEvents   = 0;

    void lock  () const; // Locks an internal mutex
    void unlock() const; // Unlocks an internal mutex

    /**
     * Clears all data stored in the DAQData. Clear is called at the
     * beginning of each run to clear any latent data from the last run.
     */
    void clear();

    // DAQData should not be copied
    DAQData       (      DAQData &other) = delete;
    void operator=(const DAQData &other) = delete;

    // Access the singleton DAQData instance by reference here.
    static DAQData &getInstance();

private:

    DAQData();

    mutable std::mutex dataLock;

};