#ifndef MUON_TRACK_PARAMETERIZATION_PLUS
#define MUON_TRACK_PARAMETERIZATION_PLUS

#include <vector>
#include <iostream>

#include "TMath.h"

#include "MuonReco/Geometry.h"
#include "MuonReco/Hit.h"
#include "MuonReco/Parameterization.h"
#include "MuonReco/Optimizer.h"
#include "MuonReco/Event.h"
#include "MuonReco/RTParam.h"
#include "MuonReco/Callable.h"
#include "MuonReco/AbstractTrackParam.h"
#include "MuonReco/Track.h"

namespace MuonReco {
  
  /*****************************************
   * Parameterization of track in 3D       *
   * in terms of intercepts and slopes     *
   *                                       *
   * Can be optimized by a Muon::Optimizer *
   * object                                *
   *                                       *
   * Author:        Dylan Ponman           *
   * Date:          June 7, 2022           *
   * Last Modified: June 7, 2022           *
   *                                       *
   *****************************************
   */
  class TrackParamPlus : virtual public AbstractTrackParam, virtual public Parameterization {
  public:
    TrackParamPlus(Geometry* geometry);
    ~TrackParamPlus();

    void                SetRT             (Callable* rtp) override;
    void                Initialize        (Event *e)         override;
    void                Print             ()                 override;
    std::vector<Track>  makeTracks        () override;
    double              D                 (int index, Hit h) override;
    double              Residual          (Hit h)            override;
    double              Distance          (Hit h)            override;   
    bool                IsRight           (Hit h)            override;
    double              deltaT0           () override;
    void                RemoveSFs         () override;
    std::vector<double> getVerticalAngle  () override;
    std::vector<double> getImpactParameter() override;

    void tracksystematics(TString systname, int * systindex, double * systerror, double * maxshift, TString & rrsystname, double * systsf, Bool_t floatup) override;

    double   LeastSquares(std::vector<double> x, std::vector<double> y,  std::vector<double> r, double* slopeOut, double* intOut);

    static const int THETA1        = 0;
    static const int INTERCEPT1    = 1;
    static const int THETA2        = 2;
    static const int INTERCEPT2    = 3;
    static const int DELTAT0       = 4;
    static const int SLEWFACTOR    = 5;
    static const int SIGPROPFACTOR = 6;
    static const int NPARS         = 5;

  private:
    Geometry* geo;
  };

}
#endif
