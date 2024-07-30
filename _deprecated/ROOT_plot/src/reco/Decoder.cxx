/**
 * @file Decoder.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

///////////////////////////////////////////////////////////////////////////////
//////////////////////////// DEPENDENCIES /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "MuonReco/Decoder.h"


///////////////////////////////////////////////////////////////////////////////
/////////////////////// IMPLEMENTATION ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace MuonReco{
  using namespace std;
  Decoder::Decoder(LockableStream &in, DAQData &dataOut) 
    : reader(in), data(dataOut) {}

  bool Decoder::isStale() {

    return reader.isReady();

  }

  void Decoder::refresh() {

    // TimeCorrection tc = TimeCorrection(cp);
    // tc.Read();

    //To do: Must pass geo to Decoder
    Geometry geo = Geometry();

    while(isStale()) {

      Signal sig = reader.extractSignal();

      // if(validateSignalErrors(sig, geo)) {

      //   // TODO: We should keep some metadata if it's a TDC header or
      //   //       trailer. See DecodeOffline.cpp

      //   signalBuffer.push_back(sig);

      // } else {

      //   cerr << "Dropped signal" << endl;

      //   data.lock();
      //   ++data.droppedSignals;
      //   data.unlock();

      // }

      // validateSignalWarnings(sig);

      if(isEvent(signalBuffer)) {

        Event e = assembleEvent(signalBuffer);

        if(validateEventErrors(e)) {

          eventBuffer.push_back(e);

        } else {

          cerr << "Dropped event" << endl;

          data.lock();
          ++data.droppedEvents;
          data.unlock();

        }

        signalBuffer.clear();

        validateEventWarnings(e);

      }

    }

    data.lock();
    data.newEvents.clear();
    data.unlock();

    for(Event &e : eventBuffer) {

      // Condition ignores empty events
      if(e.TrigSignals().at(1).HitCount() != 0) {

        // processEvent(&e,tc,geo);

        data.lock();
        data.processedEvents.push_back(e);
        data.newEvents.push_back(e);
        // data.plots.binEvent(e);
        data.unlock();

      }

    }

    // TODO: Make it clear that totalEventCount does not include events that
    //       were dropped
    data.lock();
    data.totalEventCount += eventBuffer.size();
    // data.plots.updateHitRate(data.totalEventCount);
    data.unlock();

    eventBuffer.clear();

    // NOTE: processedEvents will maintain a record of each event received
    //       by the monitor

  }

  bool isEvent(const vector<Signal> &signals) {

    if(signals.empty()) return false;

    return signals.back().isEventTrailer();

  }
}