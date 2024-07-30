/**
 * @file DAQData.cpp
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
#ifndef MUON_DAQDATA
#define MUON_DAQDATA

#include <vector>

#include "MuonReco/Event.h"


namespace MuonReco {

  struct DAQData {

    // A vector of fully processed and populated events, with hit finding
    // already performed.
    std::vector<Event> processedEvents;

    // A vector of the fully processed and populated events decoded in the
    // last decode iteration, with hit finding already performed.
    std::vector<Event> newEvents;

    // A struct of histograms containing aggregate event data. See
    // src/DataModel/Plots.cpp.
    // Plots plots;

    int totalEventCount ;
    int packetCount     ;

    int lostPackets     ;

    int droppedSignals  ;
    int droppedEvents   ;

    void lock  () const; // Locks an internal mutex
    void unlock() const; // Unlocks an internal mutex

    /**
     * Clears all data stored in the DAQData. Clear is called at the
     * beginning of each run to clear any latent data from the last run.
     */
    void clear();
    DAQData();

    // // DAQData should not be copied
    // DAQData       (      DAQData &other) = delete;
    // void operator=(const DAQData &other) = delete;

    // Access the singleton DAQData instance by reference here.
    // static DAQData &getInstance();
    // static DAQdata DAQdata_instance;

  private:
    mutable std::mutex dataLock;

  };


} //MuonReco
#endif
