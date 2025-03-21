#include "DAQData.h"

#include "MuonReco/ConfigParser.h"

using namespace std;
using namespace MuonReco;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: Consider implementing copy constructor to handle geo better.

DAQData::DAQData() : plots(geo), dirty(true) {}

DAQData &DAQData::getInstance() {

    static DAQData data;

    return data;

}

void DAQData::lock  () const { dataLock.lock  (); }
void DAQData::unlock() const { dataLock.unlock(); }

void DAQData::initialize(
    const string &confFile
) {

    clear();

    ConfigParser cp(confFile);
    // cp.Print();

    geo.Configure(cp.items("Geometry"));

    // Plots must be initialized before rtp, but after geo.
    plots.initialize();

    tc = TimeCorrection(cp);
    // tc.Read();
    // cp.items("RecoUtility").Print();

    recoUtil = RecoUtility(cp.items("RecoUtility"));

	dirty = true;

}

void DAQData::markClean() {

	dirty = false;

}

void DAQData::markDirty() {

	dirty = true;

}

bool DAQData::isDirty() const {

	return dirty;

}

void DAQData::clear () {

    plots.clear();

    totalEventCount    = 0;
    nonemptyEventCount = 0;
    packetCount        = 0;

    lostPackets        = 0;

    droppedSignals     = 0;
    droppedEvents      = 0;

	dirty = true;

}

bool DAQData::isPopulated() const {

    return totalEventCount > 0;

}