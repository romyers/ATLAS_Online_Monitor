#ifndef MUON_ABSTRACT_TRACK_PARAM
#define MUON_ABSTRACT_TRACK_PARAM

#include <vector>
#include <iostream>


#include "MuonReco/Parameterization.h"
#include "MuonReco/Event.h"
#include "MuonReco/Geometry.h"
#include "MuonReco/Track.h"
#include "MuonReco/Callable.h"
#include "MuonReco/Hit.h"


namespace MuonReco {
  /*! \class AbstractTrackParam AbstractTrackParam.h "MuonReco/AbstractTrackParam.h"
   * \brief Used as parent class for track parameterizations.
   *                                     
   * \author:        Dylan Ponman          
   *                 dponman@umich.edu
   * \date:          June 6, 2022          
   *
   */
  class AbstractTrackParam : public Optimizer, virtual public Parameterization {
  public:
    AbstractTrackParam();
    virtual void                SetRT             (Callable* rtp )   = 0;   
    virtual bool                IsRight           (Hit h)            = 0;
    virtual void                RemoveSFs         ()                 = 0;
    virtual std::vector<double> getVerticalAngle  ()                 = 0;
    virtual double              deltaT0           ()                 = 0;
    virtual std::vector<Track>  makeTracks        ()                 = 0;
    virtual std::vector<double> getImpactParameter()                 = 0;

    virtual void tracksystematics(TString systname, int * systindex, double * systerror, double * maxshift, TString & rrsystname, double * systsf, Bool_t floatup) = 0;

  protected:
    Callable* rtfunction;
    friend class ResolutionResult;
    std::vector<double> initialAngle;
  };
}
#endif
