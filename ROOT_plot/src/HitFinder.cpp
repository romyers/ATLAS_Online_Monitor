
#ifndef MUON_HITFINDER
#define MUON_HITFINDER

#include "src/Event.cpp"
#include "src/Hit.cpp"
#include "src/Signal.cpp"
#include "src/TimeCorrection.cpp"
#include "src/Noisecut.cpp"
#define BINSIZE 0.78125
#define WIDTHSEL 2
#define ROLLOVER 131072
// 2^17 = 131072
using namespace std;
using namespace Muon;

int rollover_bindiff_cal(int a, int b, int rollover){
    int bindiff;
    bindiff = a-b;
    if (bindiff > rollover/2)   bindiff -= rollover;
    else if (bindiff < - (rollover/2)) bindiff += rollover;
    return bindiff;
}

void DoHitFinding(Event *e, TimeCorrection tc, Noisecut ncut, int relative) {
  Hit h;
  double adc_time   = 0;
  double tdc_time   = 0;
  double drift_time = 0;
  double corr_time  = 0;
  // do trigger hit finding
  for (auto sig : e->Signals()) {
    adc_time = sig.Width()*BINSIZE*WIDTHSEL;
    if(adc_time>=ncut.ADCTimeCut(sig.TDC(),sig.Channel())){
      tdc_time = sig.LEdge()*BINSIZE;
      if (relative){
        drift_time = sig.LEdge()*BINSIZE;
      }
      else {
        drift_time = rollover_bindiff_cal(sig.LEdge(),e->Header().TriggerLEdge(),ROLLOVER)*BINSIZE;
      }
      corr_time  = drift_time - tc.SlewCorrection(adc_time);
      h = Hit(tdc_time,adc_time,drift_time,corr_time,sig.TDC(),sig.Channel());
      e->AddHit(h);
    }
  }
}

void DoHitFinding(Event *e, TimeCorrection tc, double adc_cut, int relative) {
  Hit h;
  double adc_time   = 0;
  double tdc_time   = 0;
  double drift_time = 0;
  double corr_time  = 0;
  // do trigger hit finding
  for (auto sig : e->Signals()) {
    adc_time = sig.Width()*BINSIZE*WIDTHSEL;
    if(adc_time>=adc_cut){
      tdc_time = sig.LEdge()*BINSIZE;
      if (relative){
        drift_time = sig.LEdge()*BINSIZE;
      }
      else {
        drift_time = rollover_bindiff_cal(sig.LEdge(),e->Header().TriggerLEdge(),ROLLOVER)*BINSIZE;
      }
      corr_time  = drift_time - tc.SlewCorrection(adc_time);
      h = Hit(tdc_time,adc_time,drift_time,corr_time,sig.TDC(),sig.Channel());
      e->AddHit(h);
   }
  }
}

#endif
