#pragma once

#include "src/RecoObject.h"

namespace Muon {

  class Hit : public RecoObject {
  public:
    Hit();
    Hit(double tdc_time, double adc_time, double drift_time, 
           double corr_time, int _tdc, int _chan);
    ~Hit() {};

    double     TDCTime  () const;
    double     ADCTime  () const;
    double     DriftTime() const;
    double     CorrTime () const;
    int        TDC      () const;
    int        Channel  () const;
    void       Draw     () override;     
  private:
    double     tdctime;
    double     adctime;
    double     driftTime;
    double     corrTime;
    int        tdc;
    int        chan;

  };

}