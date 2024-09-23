#include "MuonReco/Event.h"

namespace MuonReco {

  Event::Event() 
  	:nTrigs(0),
	 nSigs(0),
	 id(0) {

  }

  /*
  Event e(hdr, trailer, signals, id);

  e.AddTriggerHit(hit);
  */

  Event::Event(const Signal &header, const Signal &trailer, const std::vector<Signal> &signals, uint16_t eID)
  	: trigs({header, trailer}), 
	  sigs(signals), 
	  nTrigs(0), 
	  nSigs(0), 
	  id(eID) {
  }


  Event::Event(const std::vector<Signal> &triggers, const std::vector<Signal> &signals, EventID eID) 
  	: trigs(triggers), 
	  sigs(signals), 
	  id(eID.ID()), 
	  nTrigs(0), 
	  nSigs(0) {
  }

  Event::Event(const std::vector<Signal> &triggers, const std::vector<Signal> &signals, const std::vector<Hit> &wHits, const std::vector<Hit>    &tHits,    const std::vector<Cluster> &clusts, const std::vector<Track> &trks, unsigned long ID) 
  	: trigs(triggers),
	  sigs(signals),
	  trigHits(tHits),
	  sigHits(wHits),
	  clusters(clusts),
	  tracks(trks),
	  id(ID) {

    update();
	
  }

  Event::Event(const Event &e) {
    trigs    = e.TrigSignals();
    sigs     = e.WireSignals();
    trigHits = e.TriggerHits();
    sigHits  = e.WireHits();
    clusters = e.Clusters();
    tracks   = e.Tracks();
    id       = e.ID();
    update();
  }
  
  std::vector<Signal> Event::WireSignals() const {
    return sigs;
  }
  
  std::vector<Signal> Event::TrigSignals() const {
    return trigs;
  }
  
  std::vector<Hit> Event::TriggerHits() const {
    return trigHits;
  }

  std::vector<Hit> Event::WireHits() const {
    return sigHits;
  }

  std::vector<Cluster> Event::Clusters() const {
    return clusters;
  }

  std::vector<Cluster> Event::mClusters() {
    return clusters;
  }

  std::vector<Track> Event::Tracks() const {
    return tracks;
  }

  unsigned long Event::ID() const {
    return id;
  }

  Bool_t Event::Pass() const {
    return pass;
  }

  void Event::AddTriggerHit(Hit h) {
    trigHits.push_back(h);
    nTrigs++;
  }

  void Event::AddSignalHit(Hit h) {
    sigHits.push_back(h);
    nSigs++;
  }

  void Event::SetPassCheck(Bool_t b) {
    pass = b;
  }
  
  void Event::AddCluster(Cluster c) {
    clusters.push_back(c);
  }

  void Event::AddTrack(Track t) {
    tracks.push_back(t);
  }

  void Event::update() {
    nTrigs = trigHits.size();
    nSigs  = sigHits.size();
  }

  void Event::setID(unsigned long newID) {
    id = newID;
  }

  void Event::CheckClusterTime() {
    for (Cluster c : clusters) {
      for (Hit h : c.Hits()) {
      if (h.CorrTime()>200 || h.CorrTime()<-200) {
        hasBadHitTime = kTRUE;
        return;
      }
      }
    }
  }

  Bool_t Event::AnyWireHit(unsigned int TDC, unsigned int Channel) {
    for (Hit h : sigHits) {
      if (h.TDC() == TDC && h.Channel() == Channel)
      return kTRUE;
    }
    return kFALSE;
  }

  void Event::Draw() {}
} //MuonReco


