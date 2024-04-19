#pragma once

#include <bitset>
#include <vector>

#include "TEllipse.h"
#include "TGraph.h"
#include "TPaveText.h"

#include "src/Cluster.h"

namespace Muon {

  /**********************************************
   *                                            *
   * An implementation of the singleton pattern *
   * to allow global access to one instance     *
   * of the Geometry object, which describes    *
   * the geometry of the muon chamber and       *
   * implements useful functions                *
   *                                            *
   * This class contains information about      *
   * mapping tdc/channel to spatial location    *
   * and should be updated with new run         *
   * configurations as new data is taken        *
   *                                            *
   **********************************************
   */
  class Geometry {
  public:
    
    /*
     * NOTE: I've left the constructor public to preserve backwards
     *       compatibility. But any new code should use this as a singleton, 
     *       using the static getInstance function to obtain a geometry.
     *         -- Robert Myers (9-29-2023)
     */
    Geometry();

    void   Draw              (int eventN);
    void   Draw              (int eventN, double xmin, double ymin, double xmax, double ymax, TString additionalText="");
    void   Draw              (const char * title);
    void   DrawTDCLabel      ();
    void   ResetText         ();
    void   GetHitLayerColumn (unsigned int tdc_id, unsigned int channel_id, int *hit_layer, int *hit_column) const;
    void   GetHitXY          (unsigned int tdc_id, unsigned int channel_id, double *hitX, double *hitY) const;
    void   SetRunN           (int runNum);
    Bool_t AreAdjacent       (unsigned int tdc1, unsigned int ch1, unsigned int tdc2, unsigned int ch2) const;
    Bool_t AreAdjacent       (Cluster c1, Cluster c2) const;
    Bool_t IsActiveTDC       (unsigned int tdc) const;
    Bool_t IsActiveTDCChannel(unsigned int tdc, unsigned int ch) const;
    int    MultiLayer        (Cluster c) const;
    int    MultiLayer        (int layer) const;
    int    GetRunN           () const;

    static const Int_t MAX_TDC         = 24;
    static const Int_t MAX_TDC_CHANNEL = 24;
    static const Int_t MAX_TUBE_LAYER  =  8;
    static const Int_t MAX_TUBE_COLUMN = 72;
    static const Int_t MAX_TDC_COLUMN  =  12;
    static const Int_t MAX_TDC_LAYER   =  4;

    static constexpr Double_t layer_distance  = 13.0769836;
    static constexpr Double_t column_distance = 15.1;
    static constexpr Double_t radius          = 7.5;
    static constexpr Double_t ML_distance     = 224.231;
    static constexpr Double_t min_drift_dist  = 0.0;
    static constexpr Double_t max_drift_dist  = 7.1;

    int hit_column_map[Geometry::MAX_TDC_CHANNEL];
    int hit_layer_map [Geometry::MAX_TDC_CHANNEL];
    int runN;            // configurable on the fly
    short TRIGGER_CH;
    short TRIGGER_MEZZ;
    short TDC_ML [Geometry::MAX_TDC];
    short TDC_COL[Geometry::MAX_TDC];

    static Geometry &getInstance();

  private:
    void ResetAdjacencyMatrix();
    std::bitset<Geometry::MAX_TDC*Geometry::MAX_TDC_CHANNEL> adj[Geometry::MAX_TDC*Geometry::MAX_TDC_CHANNEL];
    std::bitset<Geometry::MAX_TDC> isActiveTDC;

    Double_t center_x, center_y;
    Double_t track_corner_x[2];
    Double_t track_corner_y[2];

    std::vector<TEllipse*> drawable;
    std::vector<TGraph*>    axes;
    std::vector<TPaveText*> text;

  };

}