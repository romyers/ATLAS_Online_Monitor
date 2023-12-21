#pragma once

#include <vector>

#include "src/RecoObject.cpp"
#include "src/Hit.h"

namespace Muon {
  class Cluster : public RecoObject {
  public:
    Cluster ();
    Cluster (Hit h);
    ~Cluster()      {};
    
    void        Merge(Cluster c);
    std::vector<Hit> Hits ();
    int         Size ();

    void        Draw () override;
    
  private:
    std::vector<Hit> hits;    
  };

}