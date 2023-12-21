#pragma once

#include "src/RecoObject.cpp"

#include "TMath.h"

namespace Muon {
	
  class Track : public RecoObject {
  public:
    Track();
    Track(Double_t slope_in, Double_t y_int_in);

    Double_t Slope   ();
    Double_t YInt    ();
    Double_t Distance(double x, double y);
    void     SetSlope(Double_t slope_in);
    void     SetYInt (Double_t y_int_in);
    
    void     Draw    () override;

  private:
    Double_t slope, y_int;
  };

}