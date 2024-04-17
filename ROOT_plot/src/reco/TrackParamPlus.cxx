#include "MuonReco/TrackParamPlus.h"

namespace MuonReco {

  TrackParamPlus::TrackParamPlus(Geometry*  geometry) : AbstractTrackParam(), Parameterization(5) {
    param[THETA1]      = 0;
    param[INTERCEPT1]  = 1;
    param[THETA2]      = 0;
    param[INTERCEPT2]  = 1;
    param[DELTAT0]     = 0;
    geo                = geometry;
    initialAngle.push_back(0);
    initialAngle.push_back(0);
  }

  TrackParamPlus::~TrackParamPlus() {
  }

  void TrackParamPlus::SetRT(Callable* rtp) {
    rtfunction = rtp;
  }

  std::vector<Track> TrackParamPlus::makeTracks() {
    double slope1 = -1.0*TMath::Tan(TMath::Pi()/2 + param[THETA1]);
    double slope2 = -1.0*TMath::Tan(TMath::Pi()/2 + param[THETA2]);
    double y_int1 = slope1*param[INTERCEPT1];
    double y_int2 = slope2*param[INTERCEPT2];
    std::vector<Track> tracks;
    tracks.push_back(Track(slope1, y_int1));
    tracks.push_back(Track(slope2, y_int2, true));
    return tracks;
  }

  std::vector<double> TrackParamPlus::getVerticalAngle() {
    std::vector<double> angle;
    angle.push_back(param[THETA1]);
    angle.push_back(param[THETA2]);
    return angle;
  }

  std::vector<double> TrackParamPlus::getImpactParameter() {
    std::vector<double> impact;
    impact.push_back(param[INTERCEPT1]);
    impact.push_back(param[INTERCEPT2]);
    return impact;
  }

  double TrackParamPlus::deltaT0() {
    return param[TrackParamPlus::DELTAT0];
  }

  void TrackParamPlus::RemoveSFs() {
    if (this->size() > NPARS) {
      param.erase(param.begin()+NPARS, param.end());
    }
  }

  bool TrackParamPlus::IsRight(Hit h) {
    bool hit_orientation = geo->IsPerpendicular(h.Layer());
    if (!hit_orientation) {
      return (h.X() > param[INTERCEPT1]-TMath::Tan(param[THETA1])*h.Y());
    }
    if (hit_orientation) {
      return (h.X() > param[INTERCEPT2]-TMath::Tan(param[THETA2])*h.Y());
    }
  }

  double TrackParamPlus::D(int index, Hit h) {
    bool hit_orientation = geo->IsPerpendicular(h.Layer());
    double hitX = h.X();
    double hitY = h.Y();
    double c, s, sign;
    if (!hit_orientation) {
      c = TMath::Cos(param[THETA1]);
      s = TMath::Sin(param[THETA1]);
      sign = (c*(hitX-param[INTERCEPT1]) + hitY*s > 0) ? 1.0 : -1.0;
    }  
    if (hit_orientation) {
      c = TMath::Cos(param[THETA2]);
      s = TMath::Sin(param[THETA2]);
      sign = (c*(hitX-param[INTERCEPT2]) + hitY*s > 0) ? 1.0 : -1.0;
    }
    if (index == THETA1) {
      if (hit_orientation) {
        return 0;
      }
      else {
      return sign*(-s*(hitX-param[INTERCEPT1]) + hitY*c);
      } 
    } 
 
    else if (index == THETA2) {
      if (!hit_orientation) {
        return 0;
      }
      else {
        return sign*(-s*(hitX-param[INTERCEPT2]) + hitY*c);
      }
    }

    else if (index == INTERCEPT1) {
      if (hit_orientation) {
        return 0;
      }
      else {
        return -sign*c;
      }
    }
    else if (index == INTERCEPT2) {
      if (!hit_orientation) {
        return 0;
      }
      else {
        return -sign*c;
      }
    }
    else if (index == DELTAT0) {
      /*
      if (param[THETA]>0)
  	return (rtfunction->Eval(h, param[DELTAT0]+1)-rtfunction->Eval(h, param[DELTAT0]));
  	else */
      return (rtfunction->Eval(h, param[DELTAT0])-rtfunction->Eval(h, param[DELTAT0]+1));
    }

    else if (index == SLEWFACTOR) {
      return 0;
    }

    else std::cout << "Attempting to access invalid index" << std::endl;   
    return 0;
  }


  double TrackParamPlus::Residual(Hit h) {
    double hitX = h.X();
    double hitY = h.Y();

    if (this->size() <= SLEWFACTOR) {
      return (Distance(h) - rtfunction->Eval(h, param[DELTAT0]));
    }
    else if (this->size() <= SIGPROPFACTOR) {
      return (Distance(h) - rtfunction->Eval(h, param[DELTAT0], param[SLEWFACTOR]));    
    }
    else {
      return (Distance(h) - rtfunction->Eval(h, param[DELTAT0], param[SLEWFACTOR], 
					     param[SIGPROPFACTOR]));
    }
  }


  double TrackParamPlus::Distance(Hit h) {
    double hitX = h.X();
    double hitY= h.Y();
    bool hit_orientation = geo->IsPerpendicular(h.Layer());
    if (!hit_orientation) {
      double c = TMath::Cos(param[THETA1]);
      double s = TMath::Sin(param[THETA1]); 
      return TMath::Abs(c*(hitX-param[INTERCEPT1]) + hitY*s);
    }
    if (hit_orientation) {
      double c = TMath::Cos(param[THETA2]);
      double s = TMath::Sin(param[THETA2]); 
      return TMath::Abs(c*(hitX-param[INTERCEPT2]) + hitY*s);
    }
  }


  void TrackParamPlus::Initialize(Event* e) {
    
    // create lists of doubles to hold x, y, radius
    std::vector<double> x, y, r;
    std::vector<bool> o;
    double hitX, hitY;
    bool hit_orientation;
    int npts_norm = 0;
    int npts_orth = 0;
    for (Cluster c : e->Clusters()) {
      for (Hit h : c.Hits()) {
	hitX = h.X();
	hitY = h.Y();
        hit_orientation = geo->IsPerpendicular(h.Layer());
	x.push_back(hitX);
	y.push_back(hitY);
	r.push_back(rtfunction->Eval(h));
        o.push_back(hit_orientation);
      }      
    }
    std::cout << "Orientation size is " << o.size() << std::endl;
    for (int i = 0; i < o.size(); i++) {
      std::cout << o[i] << std::endl;
    }
    // keep track of n pts
    // iterate over 2^n tries, doing a fit each time
    double bestChiSq = DBL_MAX;
    double chiSq1, chiSq2;
    std::vector<double> xnorm, ynorm, rnorm, xorth, yorth, rorth;
    for (int i = 0; i < x.size(); i++) {
      if (o.at(i)) {
        xorth.push_back(x[i]);
        yorth.push_back(y[i]);
        rorth.push_back(r[i]);
        npts_orth++;
      }
      else if (!o.at(i)) {
        xnorm.push_back(x[i]);
        ynorm.push_back(y[i]);
        rnorm.push_back(r[i]);
        npts_norm++;
      }
    }
// End of this test block
    double fitSlope1, fitInt1, fitSlope2, fitInt2;
    for (int bitmap = 0; bitmap < 1<<npts_norm; bitmap++) {
      // declare containers
      std::vector<double> xnorm_trial = std::vector<double>(xnorm.size());
      for (int i = 0; i < xnorm.size(); i++) {
	if (bitmap & 1<<i) {
	  // here the ith hit should be R
	  xnorm_trial[i] = xnorm[i] + rnorm[i];
	}
	else {
	  // here the th hit should be L
	  xnorm_trial[i] = xnorm[i] - rnorm[i];
	}
      }
      // do fitting
      chiSq1 = LeastSquares(xnorm_trial, ynorm, rnorm, &fitSlope1, &fitInt1);
//      std::cout << "BestChiSq (1) as of this iteration: " << bestChiSq << "\n";
      if (chiSq1 < bestChiSq) {
	bestChiSq         = chiSq1;
	param[THETA1]     = -1.0*TMath::ATan(1.0/fitSlope1);
	param[INTERCEPT1] = -1.0*fitInt1/fitSlope1;

      }
    }
    bestChiSq =DBL_MAX;
    for (int bitmap = 0; bitmap < 1<<npts_orth; bitmap++) {
      // declare containers
      std::vector<double> xorth_trial = std::vector<double>(xorth.size());
      for (int i = 0; i < xorth.size(); i++) {
       	if (bitmap & 1<<i) {
	  // here the ith hit should be R
	  xorth_trial[i] = xorth[i] + rorth[i];
	}
	else {
	  // here the th hit should be L
	  xorth_trial[i] = xorth[i] - rorth[i];
	} 
      }
      // do fitting
      chiSq2 = LeastSquares(xorth_trial, yorth, rorth, &fitSlope2, &fitInt2);
//      std::cout << "BestChiSq (2)  as of this iteration: " << bestChiSq << std::endl;
      if (chiSq2 < bestChiSq) {
	bestChiSq         = chiSq2;
        param[THETA2]     = -1.0*TMath::ATan(1.0/fitSlope2);
        param[INTERCEPT2] = -1.0*fitInt2/fitSlope2;

      }
    }
 

    std::cout << "Exited BitMap" << std::endl;
    param[DELTAT0] = 0;
    std::cout << "Set DeltaT0" << std::endl;
    initialAngle[0] = param[THETA1];
    std::cout << "Set Theta1" << std::endl;
    initialAngle[1] = param[THETA2];
    std::cout << "Set Theta2" << std::endl;
    Print();
  }

  double TrackParamPlus::LeastSquares(std::vector<double> x, std::vector<double> y, std::vector<double> r, double* slopeOut, double* intOut) {
    if (x.size() == 0 || y.size() == 0 || r.size() == 0) {
      *slopeOut = 3;
      *intOut = 10;
      return DBL_MAX * 0.99;
    }
    else {
      double xmean = 0.0;
      double ymean = 0.0;
      for (int i = 0; i < x.size(); i++) {
        xmean += x.at(i);
        ymean += y.at(i);
      }
      xmean /= x.size();
      ymean /= y.size();
      
      double cov = 0.0;
      double var = 0.0;
      for (int i = 0; i < x.size(); i++) {
        cov += (x[i]-xmean)*(y[i]-ymean);
        var += (x[i]-xmean)*(x[i]-xmean);
      }

      *slopeOut = cov/var;
      *intOut   = ymean - (*slopeOut)*xmean;
    
      double chiSq = 0.0;
      double dist;
      for (int i = 0; i < x.size(); i++) {
        dist   = TMath::Abs(x[i]*(*slopeOut) + (*intOut) - y[i])/TMath::Sqrt((*slopeOut)*(*slopeOut) + 1);
        chiSq += TMath::Power((dist)/(Hit::RadiusError(r[i])), 2);
        return chiSq;
      }
    }
  }
  

  void TrackParamPlus::Print() {
    std::cout << "Track Parameterization Object"   << std::endl;
    std::cout << "Parallel Theta:            " << param[THETA1]     << std::endl;
    std::cout << "Perpendicular Theta:       " << param[THETA2]     << std::endl;
    std::cout << "X Intercept:               " << param[INTERCEPT1] << std::endl;
    std::cout << "Y Intercept:               " << param[INTERCEPT2] << std::endl;
    std::cout << "Delta T0:                  " << param[DELTAT0]    << std::endl;
    std::cout << std::endl;
  }
  
  void TrackParamPlus::tracksystematics(TString systname, int * systindex, double * systerror, double * maxshift, TString & rrsystname, double * systsf, Bool_t floatup) {
    if (!systname.CompareTo("t0")) {
      *systindex = TrackParamPlus::DELTAT0;
      *systerror = 10.0;
      *maxshift = (floatup) ? 4 : -4;
      rrsystname = "Delta t_{0} [ns]";
    }
    else if (!systname.CompareTo("slope1")) {
      *systindex = TrackParamPlus::THETA1;
      *systerror = 0.005;
      *maxshift = (floatup) ? 1.5 : -1.5;
      rrsystname = "theta1 [mrad]";
      *systsf   = 1000;
    }
    else if (!systname.CompareTo("slope2")) {
      *systindex = TrackParamPlus::THETA2;
      *systerror = 0.005;
      *maxshift = (floatup) ? 1.5 : -1.5;
      rrsystname = "theta2 [mrad]";
      *systsf   = 1000;
    }
    else if (!systname.CompareTo("intercept1")) {
      *systindex = TrackParamPlus::INTERCEPT1;
      *systerror = 0.1;
      *maxshift = (floatup) ? 0.1 : -0.1;
      rrsystname = "intercept1 [mm]";
    }
    if (!systname.CompareTo("intercept2")) {
      *systindex = TrackParamPlus::INTERCEPT2;
      *systerror = 0.1;
      *maxshift = (floatup) ? 0.1 : -0.1;
      rrsystname = "intercept2 [mm]";
    }
    if (!systname.CompareTo("slew")) {
      *systindex = TrackParamPlus::SLEWFACTOR;
      *systerror = 0.25;
    }
    if (!systname.CompareTo("sigProp")) {
      *systindex = TrackParamPlus::SIGPROPFACTOR;
      *systerror = 1.0;
    }
  }
}
