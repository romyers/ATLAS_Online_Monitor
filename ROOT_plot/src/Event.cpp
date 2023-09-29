#include "src/RecoObject.cpp"
#include "src/Signal.cpp"
#include "src/EventID.cpp"
#include "src/Hit.cpp"
#include "src/Cluster.cpp"
#include "src/Track.cpp"

#ifndef MUON_EVENT
#define MUON_EVENT

namespace Muon {

  /***********************************************
   * Event is a container for all reconstructed  *
   * objects in the reco chain for sMDT chambers *
   *                                             *
   * Author: Kevin Nelson                        *
   * Date:   May    2019                         *
   * Update: May 21 2019                         *
   *                                             *
   * Updated for phase 2 online monitor sofware  *
   *                                             *
   * Author: Robert Myers                        *
   * Date:   October 2023                        *
   ***********************************************
   */
  class Event : public RecoObject {
  public:
    Event();
    Event(Signal header, Signal trailer, vector<Signal> signals);
    Event(Signal header, Signal trailer, vector<Signal> signals, vector<Hit> wirehits, 
                  vector <Cluster> clusts, vector<Track> trks);
    Event(const Event &e);

    int             ID      () const;
    Signal          Header  () const;
    Signal          Trailer () const;
    vector<Signal>  Signals () const;
    vector<Hit>     Hits    () const;
    vector<Cluster> Clusters() const;
    vector<Track>   Tracks  () const;
    bool            Pass    () const;

    vector<Cluster> mClusters();

    void   AddHit       (Hit h);
    void   update       ();
    void   SetPassCheck (bool b);
    void   AddCluster   (Cluster c);
    void   AddTrack     (Track t);

    // temporary
    void CheckClusterTime();
    bool AnyWireHit(int TDC, int Channel);


    void Draw() override;

  private:
    Signal          hd;           
    Signal          trl;          
    vector<Signal>  sigs;         
    vector<Hit>     hits;         
    vector<Cluster> clusters;     
    vector<Track>   tracks;       
    int             nSigs         = 0;
    bool            pass          = kFALSE;
    bool            hasBadHitTime = kFALSE;
  };

  /**
   * Provides a stream insertion operator for easy debug printing.
   * 
   * @author Robert Myers
   */
  ostream &operator<<(ostream &out, const Event &e) {

    cout << "------------------------------------" << endl;
    cout << "EVENT SIGNAL COUNT: " << e.Signals().size() << endl;
    cout << "PRINTING EVENT:" << endl << endl;

    cout << "EVENT HEADER:" << endl;
    cout << "\tType: " << e.Header().Type() << endl;

    cout << "EVENT TRAILER:" << endl;
    cout << "\tType: " << e.Trailer().Type() << endl;
    cout << "\tHit Count: " << e.Trailer().HitCount() << endl;

    for(size_t i = 0; i < e.Signals().size(); ++i) {

      cout << "SIGNAL " << i + 1 << endl;
      cout << "\tType: " << e.Signals()[i].Type() << endl;
      cout << "\tTDC: " << e.Signals()[i].TDC() << endl;
      cout << "\tChannel: " << e.Signals()[i].Channel() << endl;
      cout << endl;

    }

    for(size_t i = 0; i < e.Hits().size(); ++i) {

      cout << "HIT " << i + 1 << endl;
      cout << "\tTDC: " << e.Hits()[i].TDC() << endl;
      cout << "\tChannel: " << e.Hits()[i].Channel() << endl;
      cout << "\tTDCTime: " << e.Hits()[i].TDCTime() << endl;
      cout << "\tADCTime: " << e.Hits()[i].ADCTime() << endl;
      cout << "\tDrift Time: " << e.Hits()[i].DriftTime() << endl;
      cout << "\tCorrected Time: " << e.Hits()[i].CorrTime() << endl;
      cout << endl;

    }

    for(size_t i = 0; i < e.Clusters().size(); ++i) {

      cout << "CLUSTER " << i + 1 << endl;
      cout << "\tSize: " << e.Clusters()[i].Size() << endl;
      cout << endl;

    }

    return out;

  }

  Event::Event() {
    hd       = Signal();
    trl      = Signal();
    sigs     = vector<Signal>();
    hits     = vector<Hit>();
    clusters = vector<Cluster>();
    tracks   = vector<Track>();
    nSigs    = 0;
  }

  Event::Event(Signal header, Signal trailer, vector<Signal> signals){
    hd       = header;
    trl      = trailer;
    sigs     = signals;
    hits     = vector<Hit>();
    clusters = vector<Cluster>();
    tracks   = vector<Track>();
    nSigs    = 0;
  }

  Event::Event(Signal header, Signal trailer, vector<Signal> signals, vector<Hit> wirehits,
         vector<Cluster> clusts, vector<Track> trks) {
    hd       = header;
    trl      = trailer;
    sigs     = signals;
    hits     = wirehits;
    clusters = clusts;
    tracks   = trks;
    nSigs    = 0;
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
  
  int             Event::ID      () const { return Header().HeaderEID(); }
  Signal          Event::Header  () const { return hd                  ; }
  Signal          Event::Trailer () const { return trl                 ; }
  vector<Signal>  Event::Signals () const { return sigs                ; }
  vector<Hit>     Event::Hits    () const { return hits                ; }
  vector<Cluster> Event::Clusters() const { return clusters            ; }
  vector<Track>   Event::Tracks  () const { return tracks              ; }
  
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
}

#endif
