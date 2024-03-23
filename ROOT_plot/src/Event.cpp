#include "Event.h"

#include <algorithm>

#include "src/EventID.h"

using namespace std;
using namespace Muon;

/**
 * Provides a stream insertion operator for easy debug printing.
 * 
 * @author Robert Myers
 */
std::ostream &operator<<(std::ostream &out, const Event &e) {

  std::cout << "------------------------------------" << std::endl;
  std::cout << "EVENT SIGNAL COUNT: " << e.Signals().size() << std::endl;
  std::cout << "PRINTING EVENT:" << std::endl << std::endl;

  std::cout << "EVENT HEADER:" << std::endl;
  std::cout << "\tType: " << e.Header().Type() << std::endl;

  std::cout << "EVENT TRAILER:" << std::endl;
  std::cout << "\tType: " << e.Trailer().Type() << std::endl;
  std::cout << "\tHit Count: " << e.Trailer().HitCount() << std::endl;

  for(size_t i = 0; i < e.Signals().size(); ++i) {

    std::cout << "SIGNAL " << i + 1 << std::endl;
    std::cout << "\tType: " << e.Signals()[i].Type() << std::endl;
    std::cout << "\tTDC: " << e.Signals()[i].TDC() << std::endl;
    std::cout << "\tChannel: " << e.Signals()[i].Channel() << std::endl;
    std::cout << std::endl;

  }

  for(size_t i = 0; i < e.Hits().size(); ++i) {

    std::cout << "HIT " << i + 1 << std::endl;
    std::cout << "\tTDC: " << e.Hits()[i].TDC() << std::endl;
    std::cout << "\tChannel: " << e.Hits()[i].Channel() << std::endl;
    std::cout << "\tTDCTime: " << e.Hits()[i].TDCTime() << std::endl;
    std::cout << "\tADCTime: " << e.Hits()[i].ADCTime() << std::endl;
    std::cout << "\tDrift Time: " << e.Hits()[i].DriftTime() << std::endl;
    std::cout << "\tCorrected Time: " << e.Hits()[i].CorrTime() << std::endl;
    std::cout << std::endl;

  }

  for(size_t i = 0; i < e.Clusters().size(); ++i) {

    std::cout << "CLUSTER " << i + 1 << std::endl;
    std::cout << "\tSize: " << e.Clusters()[i].Size() << std::endl;
    std::cout << std::endl;

  }

  return out;

}

Event::Event() {
  hd              = Signal();
  trl             = Signal();
  sigs            = std::vector<Signal>();
  hits            = std::vector<Hit>();
  clusters        = std::vector<Cluster>();
  tracks          = std::vector<Track>();
  nSigs           = 0;
}

Event::Event(Signal header, Signal trailer, std::vector<Signal> signals){
  hd              = header;
  trl             = trailer;
  sigs            = signals;
  hits            = std::vector<Hit>();
  clusters        = std::vector<Cluster>();
  tracks          = std::vector<Track>();
  nSigs           = 0;

}

Event::Event(Signal header, Signal trailer, std::vector<Signal> signals, std::vector<Hit> wirehits,
       std::vector<Cluster> clusts, std::vector<Track> trks) {
  hd              = header;
  trl             = trailer;
  sigs            = signals;
  hits            = wirehits;
  clusters        = clusts;
  tracks          = trks;
  nSigs           = 0;

  update();
}

Event::Event(const Event &e) {
  hd       = e.Header  ();
  trl      = e.Trailer ();
  sigs     = e.Signals ();
  hits     = e.Hits    ();
  clusters = e.Clusters();
  tracks   = e.Tracks  ();
  update();
}

int             Event::ID              () const { return Header().HeaderEID(); }

Signal          Event::Header          () const { return hd                  ; }
Signal          Event::Trailer         () const { return trl                 ; }

std::vector<Signal> Event::Signals() const { 

  vector<Signal> result = sigs;

  // Filter out TDC headers/errors/trailers
  result.erase(

    remove_if(result.begin(), result.end(), [](const Signal &sig) {

      if(sig.isTDCHeader ()) return true;
      if(sig.isTDCTrailer()) return true;
      if(sig.isTDCOverflow()) return true;

      return false;

    }), 

    result.end()

  );

  return result; 

}

std::vector<Signal>  Event::Signals_All     () const { return sigs                ; }
std::vector<Hit>     Event::Hits            () const { return hits                ; }
std::vector<Cluster> Event::Clusters        () const { return clusters            ; }
std::vector<Track>   Event::Tracks          () const { return tracks              ; }

bool Event::Pass() const {
  return pass;
}

void Event::AddHit(Hit h) {
  hits.push_back(h);
  nSigs++;
}

void Event::SetPassCheck(bool b) {
  pass = b;
}

void Event::AddCluster(Cluster c) {
  clusters.push_back(c);
}

void Event::AddTrack(Track t) {
  tracks.push_back(t);
}

void Event::update() {
  nSigs  = hits.size();
}

void Event::CheckClusterTime() {
  for (Cluster c : clusters) {
    for (Hit h : c.Hits()) {
      if (h.CorrTime()>400 || h.CorrTime()<0) {
        hasBadHitTime = kTRUE;
        return;
      }
    }
  }
}

bool Event::AnyWireHit(int TDC, int Channel) {
  for (Hit h : hits) {
    if (h.TDC() == TDC && h.Channel() == Channel)
      return kTRUE;
  }
  return kFALSE;
}

void Event::Draw() {}