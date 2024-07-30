#ifndef MUON_DECODER
#define MUON_DECODER

#include <vector>
#include <istream>

#include "MuonReco/SignalDecoding.h"
#include "MuonReco/EventDecoding.h"
#include "MuonReco/Signal.h"
#include "MuonReco/Event.h"
#include "MuonReco/DAQData.h"
#include "MuonReco/LockableStream.h"
#include "MuonReco/Geometry.h"

namespace MuonReco {
	// TODO: Rename Decoder methods to something semantically more appropriate

  class Decoder {
  public:
    Decoder(LockableStream &in, DAQData &dataOut);
    bool isStale();
    void refresh();
  private:
    SignalReader reader;
    DAQData &data;
    std::vector<Signal> signalBuffer;
    std::vector<Event > eventBuffer ;
  };

  ///////////////////////////////////////////////////////////////////////////////
  //////////////////////// HELPER FUNCTIONS /////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  /**
   * Returns true if signals represents a complete event (i.e. if signals ends
   * with a trailer).
   */
  bool isEvent(const std::vector<Signal> &signals);
}
#endif
