#pragma once

#include "src/RecoObject.h"
#include "src/Signal.h"
#include "src/Hit.h"
#include "src/Cluster.h"
#include "src/Track.h"

#include <vector>
#include <iostream>

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
    Event(Signal header, Signal trailer, std::vector<Signal> signals);
    Event(Signal header, Signal trailer, std::vector<Signal> signals, std::vector<Hit> wirehits, 
                  std::vector <Cluster> clusts, std::vector<Track> trks);
    Event(const Event &e);

    int                  ID              () const;
    Signal               Header          () const;
    Signal               Trailer         () const;
    std::vector<Signal>  Signals         () const;
    std::vector<Hit>     Hits            () const;
    std::vector<Cluster> Clusters        () const;
    std::vector<Track>   Tracks          () const;
    bool                 Pass            () const;

    std::vector<Cluster> mClusters();

    void   AddHit       (Hit h    );
    void   update       (         );
    void   SetPassCheck (bool b   );
    void   AddCluster   (Cluster c);
    void   AddTrack     (Track t  );

    // temporary
    void CheckClusterTime();
    bool AnyWireHit(int TDC, int Channel);


    void Draw() override;

  private:
    Signal               hd      ;           
    Signal               trl     ;          
    std::vector<Signal>  sigs    ;         
    std::vector<Hit>     hits    ;         
    std::vector<Cluster> clusters;     
    std::vector<Track>   tracks  ;       
    int                  nSigs           = 0     ;
    bool                 pass            = kFALSE;
    bool                 hasBadHitTime   = kFALSE;
  };

}

std::ostream &operator<<(std::ostream &out, const Muon::Event &e);