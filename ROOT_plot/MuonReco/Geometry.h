#ifndef MUON_GEOMETRY
#define MUON_GEOMETRY

#include <bitset>
#include <vector>
#include <stdbool.h>

#include "TGraph.h"
#include "TPaveText.h"
#include "TEllipse.h"
#include "TMath.h"

#include "MuonReco/Cluster.h"
#include "MuonReco/ParameterSet.h"
#include "MuonReco/ConfigParser.h"

namespace MuonReco {
  /*! \class Geometry Geometry.h "MuonReco/Geometry.h"
   * \brief Encapsulate the geometry and run configuration of the chamber
   *
   * Many aspects of the Geometry are static, i.e. the tube radius and number of 
   * drift tubes.  However, some DAQ information is configured for each run.
   * For example, the number and location of the mezzanine cards is permitted to 
   * change, and the settings are listed in the configuration file.  This information
   * is crucial to designate the trigger channel and to know which Hits are adjacent
   * to one another.
   * 
   * \author Kevin Nelson
   *         kevin.nelson@cern.ch
   * \date   17 July 2020
   */
  class Geometry {
  public:
    Geometry();
    Geometry(ConfigParser& cp);
    ~Geometry();

    void   Draw              (int eventN);
    void   Draw              (int eventN, double xmin, double ymin, double xmax, double ymax, TString additionalText="");
    void   Draw              (const char * title);
    void   Draw              (int eventN, bool view);
    void   DrawTDCLabel      ();
    void   ResetText         ();
    void   GetHitLayerColumn (unsigned int tdc_id, unsigned int channel_id, int *hit_layer, int *hit_column) const;
    void   SetRunN           (int runNum);
    bool   IsActiveTDC       (unsigned int tdc) const;
    bool   IsActiveTDCChannel(unsigned int tdc, unsigned int ch) const;
    bool   IsActiveLayerColumn(int layer, int column) const;
    int    MultiLayer        (Cluster c) const;

    int    GetRunN           () const;
    void   Configure         (ParameterSet ps);
    bool   IsPerpendicular   (int layer);
    std::vector<int> orientation();

    void   GetHitXY   (int hitL, int hitC, double *hitX, double *hitY);
    static bool   AreAdjacent(double x1, double x2, double y1, double y2);
    static bool   AreAdjacent(Cluster c1, Cluster c2);
    static int    MultiLayer (int layer);
    static double getMeanYPosition();

    static Int_t MAX_TDC;
    static Int_t MAX_TDC_CHANNEL;
    static Int_t MAX_TUBE_LAYER;
    static Int_t MAX_TUBE_COLUMN;
    static Int_t MAX_TDC_COLUMN;
    static Int_t MAX_TDC_LAYER;

    static double layer_distance;
    static double column_distance;
    static double radius;
    static double ML_distance;
    static double min_drift_dist;
    static double max_drift_dist;
    static double tube_length;

    std::vector<int> hit_column_map = std::vector<int>();
    std::vector<int> hit_layer_map  = std::vector<int>();
    int runN;            // configurable on the fly
    int tdcColByTubeNo = 0;
    int flipTDCs = 0;
    short TRIGGER_CH;
    short TRIGGER_MEZZ;
    std::vector<short> TDC_ML   = std::vector<short>();
    std::vector<short> TDC_COL  = std::vector<short>();
    std::vector<short> TDC_FLIP = std::vector<short>();
    TString chamberType = "C";
    std::vector<double> layerOffset = std::vector<double>();
    std::vector<double> layerSlope  = std::vector<double>();
    std::vector<int>  ML_orientation = std::vector<int>();

  private:

    void ResetTubeLayout();
    std::vector<bool> isActiveTDC = std::vector<bool>();

    double center_x, center_y;
    double track_corner_x[2];
    double track_corner_y[2];

    std::vector<TEllipse*> drawable;
    std::vector<TGraph*>    axes;
    std::vector<TPaveText*> text;
  };
    
}
#endif
