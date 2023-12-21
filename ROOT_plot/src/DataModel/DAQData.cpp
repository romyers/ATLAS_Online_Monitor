#include "DAQData.h"

using namespace std;
using namespace Muon;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DAQData::DAQData() {}

DAQData &DAQData::getInstance() {

    static DAQData data;

    return data;

}

void DAQData::lock  () const { dataLock.lock  (); }
void DAQData::unlock() const { dataLock.unlock(); }

void DAQData::clear () {

    processedEvents.clear();
    plots.clear();

    totalEventCount = 0;
    packetCount     = 0;

    lostPackets     = 0;

    droppedSignals  = 0;
    droppedEvents   = 0;

}