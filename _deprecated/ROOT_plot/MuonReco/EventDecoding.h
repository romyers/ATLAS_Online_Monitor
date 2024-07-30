#ifndef MUON_EVENT_DECODING
#define MUON_EVENT_DECODING

#include <vector>
#include <string>
#include <algorithm>


#include "MuonReco/Signal.h"
#include "MuonReco/Event.h"
#include "MuonReco/RecoUtility.h"
#include "MuonReco/TimeCorrection.h"
#include "MuonReco/Geometry.h"
#include "MuonReco/TimeCorrection.h"

namespace MuonReco {
  Event assembleEvent        (std::vector<Signal>  signals);
  bool  validateEventErrors  (const Event          &e     );
  void  validateEventWarnings(const Event          &e     );
  void  processEvent         (Event *e, TimeCorrection* tc, Geometry& geo);

}
#endif
