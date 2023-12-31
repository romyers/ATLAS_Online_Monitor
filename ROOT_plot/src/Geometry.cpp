#include "Geometry.h"

using namespace std;
using namespace Muon;

Geometry &Geometry::getInstance() {

  static Geometry instance;

  return instance;

}

Geometry::Geometry() {

  runN         = 0;
  TRIGGER_CH   = 0;
  TRIGGER_MEZZ = 18;

  for (int i = 0; i < Geometry::MAX_TDC_CHANNEL; i++) {
    hit_layer_map[i] = i%4;
    hit_column_map[i] = 5 - i/4;
  }
  for (int i = 0; i < Geometry::MAX_TDC; i++) {
    TDC_ML[i]  = 0;
    TDC_COL[i] = 0;
  }

  track_corner_x[0] = 0;
  track_corner_x[1] = 800;
  track_corner_y[0] = 0;
  track_corner_y[1] = 320;
  axes.push_back(new TGraph(2, track_corner_x, track_corner_y));

  // create the background of the event display: the outlines of the tubes
  for (Int_t layer_id = 0; layer_id != Geometry::MAX_TUBE_LAYER/2; layer_id++) {
    for (Int_t column_id = 0; column_id != Geometry::MAX_TUBE_COLUMN; column_id++) {
      center_x = radius + column_id * column_distance + ((layer_id + 1) % 2) * column_distance / 2.0;
      center_y = radius + layer_id * layer_distance;
      drawable.push_back(new TEllipse(center_x, center_y, radius, radius));

      if ((column_id / 6) % 2 == 0) {
        drawable.at(layer_id*Geometry::MAX_TUBE_COLUMN + column_id)->SetFillColor(kGray);
      }
    }
  }
  for (Int_t layer_id = Geometry::MAX_TUBE_LAYER/2; layer_id != Geometry::MAX_TUBE_LAYER; layer_id++) {
    for (Int_t column_id = 0; column_id != Geometry::MAX_TUBE_COLUMN; column_id++) {
      center_x = radius + column_id * column_distance + ((layer_id + 1) % 2) * column_distance / 2.0;
      center_y = radius + (layer_id - Geometry::MAX_TUBE_LAYER/2) * layer_distance + ML_distance;
      drawable.push_back(new TEllipse(center_x, center_y, radius, radius));

      if ((column_id / 6) % 2 == 0) {
        drawable.at(layer_id*Geometry::MAX_TUBE_COLUMN + column_id)->SetFillColor(kGray);
      }
    }
  }

  axes.at(0)->SetTitle("event");
  axes.at(0)->SetMarkerStyle(0);
} // end method: Geometry initialization

void Geometry::Draw(int eventN) {
  TString title;
  title.Form("Event %d  Run %d", eventN, runN);
  axes.at(0)->SetTitle(title);
  axes.at(0)->Draw("AP");
  for (auto tube : drawable) tube->Draw();
} // end method: Geometry :: Draw 

void Geometry::Draw(int eventN, double xmin, double ymin, double xmax, double ymax, TString additionalText) {
  TString title;
  title.Form("Event %d  Run %d", eventN, runN);
  title += additionalText;
  Double_t xlim[2];
  Double_t ylim[2];
  
  xlim[0] = xmin;
  xlim[1] = xmax;
  ylim[0] = ymin;
  ylim[1] = ymax;

  if (axes.size() == 1)
    axes.push_back(new TGraph(2,xlim,ylim));
  else
    axes[1] = new TGraph(2,xlim,ylim);
  
  axes.at(1)->SetTitle(title);
  axes.at(1)->Draw("AP");

  for (auto tube : drawable) tube->Draw();
}

void Geometry::Draw(const char* title) {
  axes.at(0)->SetTitle(title);
  axes.at(0)->Draw("AP");
  for (auto tube : drawable) tube->Draw();
}

void Geometry::DrawTDCLabel() {
  TString str;
  double hitX, hitY;
  for (int tdc = 0; tdc != Geometry::MAX_TDC; tdc++) {
    if (IsActiveTDC(tdc) && tdc != TRIGGER_MEZZ) {
GetHitXY(tdc, 0, &hitX, &hitY);
str.Form("TDC %d", tdc);
text.push_back(new TPaveText(hitX-9*radius, hitY+9*radius, hitX-3*radius, hitY+11*radius));
text.back()->AddText(str.Data());
text.back()->Draw();
    }
  }
}

void Geometry::ResetText() {
  
  return;
}

void Geometry::GetHitLayerColumn(unsigned int tdc_id, unsigned int channel_id, int *hit_layer, int *hit_column) const {
  *hit_column = 6*TDC_COL[tdc_id] + hit_column_map[channel_id];
  *hit_layer  = 4*TDC_ML[tdc_id]  + hit_layer_map[channel_id];
}

void Geometry::GetHitXY(unsigned int tdc_id, unsigned int channel_id, double *hitX, double *hitY) const {
  // get hit layer and column
  int hitL, hitC;
  GetHitLayerColumn(tdc_id, channel_id, &hitL, &hitC);
  
  *hitX = Geometry::radius + hitC * column_distance + ((hitL + 1) % 2) * column_distance / 2.0;
  *hitY = Geometry::radius + hitL * layer_distance  + (224.255-4*layer_distance)*TDC_ML[tdc_id];
}    

int Geometry::MultiLayer(Cluster c) const {
  int hitL, hitC;
  GetHitLayerColumn(c.Hits().at(0).TDC(), c.Hits().at(0).Channel(), &hitL, &hitC);
  return hitL/MAX_TDC_LAYER;
}

int Geometry::MultiLayer(int layer) const {
  return layer/MAX_TDC_LAYER;
}

void Geometry::ResetAdjacencyMatrix() {
  for (int i = 0; i < Geometry::MAX_TDC_CHANNEL*Geometry::MAX_TDC; i++) {
    adj[i].reset();
  }
  int hitL1, hitC1, hitL2, hitC2;
  for (int tdc1 = 0; tdc1 < Geometry::MAX_TDC; tdc1++) {
    for (int ch1 = 0; ch1 < Geometry::MAX_TDC_CHANNEL; ch1++) {
    	GetHitLayerColumn(tdc1, ch1, &hitL1, &hitC1);
    	for (int tdc2 = 0; tdc2 < Geometry::MAX_TDC; tdc2++) {
    	  for (int ch2 = 0; ch2 < Geometry::MAX_TDC_CHANNEL; ch2++) {
    	    GetHitLayerColumn(tdc2, ch2, &hitL2, &hitC2);
    	    if ((hitL1< 4 && hitL2< 4) || (hitL1>=4 && hitL2>=4)) { // must cluster within same multilayer
    	      if ((hitC1 == hitC2) && (hitL1==hitL2+1 || hitL1==hitL2-1)) {
    		      adj[tdc1*Geometry::MAX_TDC_CHANNEL + ch1][tdc2*Geometry::MAX_TDC_CHANNEL + ch2] = 1;
    	      }
    	      else if ((hitL1==hitL2) && (hitC1==hitC2+1 || hitC1==hitC2-1)) {
    		      adj[tdc1*Geometry::MAX_TDC_CHANNEL + ch1][tdc2*Geometry::MAX_TDC_CHANNEL + ch2] = 1;
    	      }
    	      else if ((hitL1%2==1) && (hitC1==hitC2+1) && (hitL1==hitL2+1 || hitL1==hitL2-1)) {
    		      adj[tdc1*Geometry::MAX_TDC_CHANNEL + ch1][tdc2*Geometry::MAX_TDC_CHANNEL + ch2] = 1;
    	      }
    	      else if ((hitL1%2==0) && (hitC1==hitC2-1) && (hitL1==hitL2+1 || hitL1==hitL2-1)) {
    		      adj[tdc1*Geometry::MAX_TDC_CHANNEL + ch1][tdc2*Geometry::MAX_TDC_CHANNEL + ch2] = 1;
    	      }
    }
  }
}
    }
  }
}

Bool_t Geometry::AreAdjacent(unsigned int tdc1, unsigned int ch1, unsigned int tdc2, unsigned int ch2) const {
  return adj[tdc1*Geometry::MAX_TDC_CHANNEL + ch1][tdc2*Geometry::MAX_TDC_CHANNEL + ch2];
}

Bool_t Geometry::AreAdjacent(Cluster c1, Cluster c2) const {
  for (Hit h1 : c1.Hits()) {
    for (Hit h2 : c2.Hits()) {
      if (AreAdjacent(h1.TDC(), h1.Channel(), h2.TDC(), h2.Channel()))
      return kTRUE;	 	
    }
  }
  return kFALSE;
}

Bool_t Geometry::IsActiveTDC(unsigned int tdc) const {
  return isActiveTDC[tdc];
}

Bool_t Geometry::IsActiveTDCChannel(unsigned int tdc, unsigned int ch) const {
  if (ch >= Geometry::MAX_TDC_CHANNEL)
    return 0;//ch == TRIGGER_CH;
  else
    return isActiveTDC[tdc];
}

int  Geometry::GetRunN() const {
  return runN;
}

void Geometry::SetRunN(int runNum) {
  // runN = runNum;
//    if (runN == 187691 || 
// runN == 187846) {
//      TRIGGER_CH   = 23;
//      TRIGGER_MEZZ =  1;
    
//      TDC_ML[8]  = 0;
//      TDC_ML[9]  = 1;
//      TDC_ML[10] = 1;
//      TDC_ML[11] = 0;

//      TDC_COL[8]  = 5;
//      TDC_COL[9]  = 5;
//      TDC_COL[10] = 4;
//      TDC_COL[11] = 4;
    
//      isActiveTDC.reset();
//      isActiveTDC[1]  = 1;
//      isActiveTDC[8]  = 1;
//      isActiveTDC[9]  = 1;
//      isActiveTDC[10] = 1;
//      isActiveTDC[11] = 1;
//    }
//    else if (runN == 187903) {
//      TRIGGER_CH   = 23;
//      TRIGGER_MEZZ =  5;
    
//      TDC_ML[2]  = 0;
//      TDC_ML[3]  = 0;
//      TDC_ML[8]  = 1;
//      TDC_ML[9]  = 1;
//      TDC_ML[10] = 1;
    
//      TDC_COL[2]  = 0;
//      TDC_COL[3]  = 1;
//      TDC_COL[8]  = 0;
//      TDC_COL[9]  = 1;
//      TDC_COL[10] = 2;
    
//      isActiveTDC.reset();
//      isActiveTDC[5]  = 1;
//      isActiveTDC[2]  = 1;
//      isActiveTDC[3]  = 1;
//      isActiveTDC[9]  = 1;
//      isActiveTDC[10] = 1;
//    }
//    else if (runN == 187939 || runN == 187982 || runN == 187984 ||
//      runN == 187985 || runN == 188734) {
    TRIGGER_CH   = 23;
    TRIGGER_MEZZ =  18;

//      TDC_ML[0]  = 0;
//      TDC_ML[2]  = 0;
//      TDC_ML[8]  = 1;
//      TDC_ML[9]  = 1;
//      TDC_ML[10] = 1;

//      TDC_COL[0]  = 1;
//      TDC_COL[2]  = 2;
//      TDC_COL[8]  = 0;
//      TDC_COL[9]  = 1;
//      TDC_COL[10] = 2;

//      isActiveTDC.reset();
//      isActiveTDC[5]  = 1;
//      isActiveTDC[0]  = 1;
//      isActiveTDC[2]  = 1;
//      isActiveTDC[8]  = 1;
//      isActiveTDC[9]  = 1;
//      isActiveTDC[10] = 1;
//    }
//  8  9
// //  7  1
//       // TDC_ML[7]  = 0;
//       TDC_ML[0]  = 0;
//       TDC_ML[1]  = 1;
//       TDC_ML[2]  = 0;
//       // TDC_ML[8]  = 1;
//       TDC_ML[3]  = 1;


//       // TDC_COL[7]  = 1;
//       TDC_COL[0]  = 0;
//       TDC_COL[1]  = 0;
//       // TDC_COL[8]  = 1;
//       TDC_COL[2]  = 1;
//       TDC_COL[3]  = 1;

//       isActiveTDC.reset();
//       isActiveTDC[0] = 1;
//       isActiveTDC[1] = 1;
//       isActiveTDC[2] = 1;
//       isActiveTDC[3] = 1;
//       isActiveTDC[4] = 1;
//       isActiveTDC[5] = 1;
//       isActiveTDC[6] = 1;
//       isActiveTDC[7] = 1;
    // isActiveTDC[8]  = 1;
    isActiveTDC.reset();
    for (int i = 0; i < 20; ++i)
    {
      isActiveTDC[i]  = 1;
      TDC_ML[i]  = i%2;
      TDC_COL[i] = i/2*6;
    }
    // isActiveTDC[0] = 0;
    // isActiveTDC[1] = 0;

  
  ResetAdjacencyMatrix();
} // end function: SetRunN