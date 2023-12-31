#ifndef MUON_CLUSTER
#define MUON_CLUSTER

#include <vector>

#include "MuonReco/Hit.h"
#include "MuonReco/RecoObject.h"

namespace MuonReco {
  /*! \class Cluster Cluster.h "MuonReco/Cluster.h"
   *  \brief Aggregates adjacent hit objects for track finding
   *
   *  A Cluster can be merged with another cluster.  The clustering algorithm
   *  is implemented in RecoUtility
   */
  class Cluster : public RecoObject {
  public:
    Cluster ();
    Cluster (Hit h);
    ~Cluster()      {};

    void             Merge(Cluster c);
    std::vector<Hit> Hits ();
    int              Size ();

    void             Draw () override;

  private:
    std::vector<Hit> hits; //< underlying hit collection
  };
}
#endif
