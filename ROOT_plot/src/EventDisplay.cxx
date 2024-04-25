#include "EventDisplay.h"

namespace MuonReco {

  EventDisplay::EventDisplay() {
  }

  EventDisplay::~EventDisplay() {
    Clear();
  }

  void EventDisplay::Clear() {

    for (auto it = boxes.begin(); it != boxes.end(); ++it) {
      delete (*it);
    }
    boxes = std::vector<TBox*>();
  }
  
  void EventDisplay::Divide(TCanvas *eCanv, int nX, int nY) {
    eCanv->Divide(nX, nY);
    eCanv->Update();
  }

  void EventDisplay::DrawTDCBorders() {
    for (int tdc = 0; tdc != Geometry::MAX_TDC; tdc++) {
      for (int ch = 0; ch != Geometry::MAX_TDC_CHANNEL; ch++) {
        
      }
    }
  }

  void EventDisplay::SetRT(Callable* rtp) {
    rtfunction = rtp;
  }

  void EventDisplay::SetOutputDir(TString dir) {
    _dir = dir;
  }

  void EventDisplay::DrawEvent(
    TCanvas *eCanv, 
    Event &e, 
    Geometry &geo, 
    TDirectory* outdir/*=NULL*/, 
    bool isphase2data
  ) {

    // If the canvas is updated after hit_model and track_model are cleared,
    // the canvas will not draw hits and tracks, no matter when the draw()
    // call was made. So we need hit_model and track_model to be persistent.
    // But we also need them to be in a clear state at the beginning of the
    // draw call to avoid repeats, so we reset them at the beginning of the
    // draw.
    for(TEllipse *hit : hit_model) {
      delete hit;
    }
    hit_model.clear();

    for(TLine *track : track_model) {
      delete track;
    }
    track_model.clear();

    double hit_x, hit_y;
    int hit_l, hit_c;

    std::vector<bool>      hit_model_orientation;
    std::vector<bool>      track_model_orientation;

    // draw the background geometry
    
    bool has_perp = false;
    for (int counter = 0; counter < geo.orientation().size(); counter++) {
      if (geo.orientation().at(counter) == 1) {
        has_perp = true;
      }
    }

    if (has_perp) {
      eCanv->cd(2);
      geo.Draw(e.ID(), true);
      eCanv->cd(1);
      geo.Draw(e.ID(), false);
    }
    else {
      eCanv->cd(1);
      geo.Draw(e.ID());
    }

    TubeMap<bool> wireIsDrawn = TubeMap<bool>(Geometry::MAX_TUBE_LAYER, 
                                              Geometry::MAX_TUBE_COLUMN);       
    // draw this event using the highest level avaliable reconstructed objects
    if (e.Tracks().size() != 0) {
      for (Track t : e.Tracks()) {
        // FIXME: Vertical line problem with y intercept and tangent
        track_model.push_back(
          new TLine(
            0, t.YInt(), 1000, -1000 * TMath::Tan(t.Theta()) + t.YInt()
          ));
        track_model.at(track_model.size()-1)->SetLineWidth(1);
        track_model.at(track_model.size()-1)->SetLineColor(kBlack);
        if (t.Orientation()) {
          track_model_orientation.push_back(true);
        }
        else {
          track_model_orientation.push_back(false);
        }
/*
        for (int i = 0; i < geo.orientation().size(); i++) {
          if ((t.Orientation() && geo.orientation().at(i) == 1) || (!t.Orientation() && geo.orientation().at(i) == 0)) {
             track_model.push_back(new TLine((t.YInt() - i * Geometry::ML_distance) / t.Slope(), i * Geometry::ML_distance, (t.YInt() - (i + 1) * Geometry::ML_distance) / t.Slope(), (i + 1) * Geometry::ML_distance));
             track_model.at(track_model.size()-1)->SetLineWidth(1);
             track_model.at(track_model.size()-1)->SetLineColor(kBlack);
          }
        }
*/
      }
    }

    if (e.Clusters().size() != 0) {

      // draw using clusters 
      for (Cluster c : e.Clusters()) {
        for (Hit h : c.Hits()) {

          hit_x = h.X();
          hit_y = h.Y();
          hit_model_orientation.push_back(geo.IsPerpendicular(h.Layer()));
      
          if (rtfunction != 0) {
            hit_model.push_back(new TEllipse(hit_x, hit_y, rtfunction->Eval(h), rtfunction->Eval(h)));
            hit_model.back()->SetFillColor(kBlue);
          }
          else {
            hit_model.push_back(new TEllipse(hit_x, hit_y, Geometry::radius, Geometry::radius));
            hit_model.back()->SetFillColor(kGreen);
          }
          wireIsDrawn.set(h.Layer(), h.Column(), 1);
        }
      }
    }

    if (e.WireSignals().size() != 0) {
      // draw using signals
      for (size_t i = 0; i < e.WireSignals().size(); i++) {
        geo.GetHitLayerColumn(e.WireSignals().at(i).TDC(), e.WireSignals().at(i).Channel(), &hit_l, &hit_c);
        if (!wireIsDrawn.get(hit_l, hit_c)) {
          if (isphase2data||(e.WireSignals().at(i).Type() == Signal::RISING)) {
            geo.GetHitXY(hit_l, hit_c, &hit_x, &hit_y);
            hit_model.push_back(new TEllipse(hit_x, hit_y, Geometry::radius, Geometry::radius));
            hit_model.back()->SetFillColor(kRed);
            wireIsDrawn.set(hit_l, hit_c, 1);
            hit_model_orientation.push_back(geo.IsPerpendicular(hit_l));
          }
        }
      }
    }

    for (int i = 0; i < hit_model.size(); i++) {
      if (hit_model_orientation.at(i)) {
        eCanv->cd(2);
      }
      else {
        eCanv->cd(1);
      }
      hit_model.at(i)->Draw();
    }

    for (int i = 0; i < track_model.size(); i++) {
      if (track_model_orientation.at(i)) {
        eCanv->cd(2);
      }
      else {
        eCanv->cd(1);
      }
      track_model.at(i)->Draw();
    }

/*
    for (auto hit : hit_model) {
      
      hit->Draw();
    }
    for (auto track: track_model) {
      track->Draw();
    }
*/    
    eCanv->cd(0);
    if (outdir != NULL) {
      outdir->WriteTObject(eCanv);
    }
    eCanv->Modified();
    eCanv->Update();

    
  } // end method: Event Display :: Draw Event

/*
  void EventDisplay::DrawTrackZoom(Event &e, Geometry &geo, int ML, TDirectory* outdir) {
    
    // TString title = TString();
    // title.Form("Track zoom on MultiLayer %d", ML);
    // zoomCanv = new TCanvas("zoomCanv", title.Data(), 800, 800);
    
    char canvas_name[256];
    char canvas_output_name[256];
    sprintf(canvas_name, "event_id_%lu_ML_%i", e.ID(), ML);
    strcpy(canvas_output_name, canvas_name);
    strcat(canvas_output_name, ".png");

    eCanv->cd(1);

    double xmin, xmax, ymin, ymax, ymid;
    Track tr = e.Tracks().back();
    ymin = Geometry::ML_distance*(ML);

    ymax = ymin + 3*Geometry::layer_distance + 2*Geometry::radius;
    ymid = (ymax+ymin)/2;
    xmin = (tr.YInt()-ymin)/tr.Slope();
    xmax = (tr.YInt()-ymax)/tr.Slope();
    double extra = (2*(ymax - ymin) - (xmax - xmin))/2.0;
    xmin -= extra;
    xmax += extra;

    TString ml_label;
    ml_label.Form("MultiLayer %d", ML);
    geo.Draw(e.ID(), xmin, ymin, xmax, ymax, ml_label);

    for (auto hit : hit_model) {
      hit->Draw();
    }
    for (auto track: track_model) {
      track->Draw();
    }

    eCanv->Update();

    if (outdir != NULL) {
      outdir->WriteTObject(eCanv);
    }
    if (_dir.CompareTo("")) {
      eCanv->SaveAs(IOUtility::join(_dir, canvas_output_name));
    }
  } // end method: Event Display:: Draw Track Zoom
*/

}
