#include "Track.h"

using namespace std;
using namespace Muon;

Track::Track() {
  slope = 0; 
  y_int = 0;
}

Track::Track(Double_t slope_in, Double_t y_int_in) {
  slope = slope_in;
  y_int = y_int_in;
}

Double_t Track::Slope() {
  return slope;
}

Double_t Track::YInt() {
  return y_int;
}

void Track::SetSlope(Double_t slope_in) {
  slope = slope_in;
}

void Track::SetYInt(Double_t y_int_in) {
  y_int = y_int_in;
}

Double_t Track::Distance(double x, double y) {
  return TMath::Abs(x*slope + y_int - y)/TMath::Sqrt(slope*slope + 1);
}

void Track::Draw() {
  return;
}