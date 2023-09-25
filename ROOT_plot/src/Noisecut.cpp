#include "src/Geometry.cpp"

#ifndef MUON_NOISECUT
#define MUON_NOISECUT


namespace Muon {

  /**********************************************
   *                                            *
   * An implementation of the singleton pattern *
   * to allow global access to one instance     *
   * of the Noisecut object, which describes    *
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
  class Noisecut {
  public:
    Noisecut();
    double ADCTimeCut(unsigned int tdc_id, unsigned int channel_id);




  private:
    double adctimecut[Geometry::MAX_TDC][Geometry::MAX_TDC_CHANNEL];


  };


  Noisecut::Noisecut(){
    memset(adctimecut, 0, sizeof(adctimecut)); 

    //TDC1
    adctimecut[1][0]  = 86.0;
    adctimecut[1][1]  = 105.0;
    adctimecut[1][2]  = 130.0;
    adctimecut[1][3]  = 115.0;
    adctimecut[1][4]  = 118.0;
    adctimecut[1][5]  = 92.0;
    adctimecut[1][6]  = 120.0;
    adctimecut[1][7]  = 88.0;
    adctimecut[1][8]  = 120.0;
    adctimecut[1][9]  = 95.0;
    adctimecut[1][10] = 120.0;
    adctimecut[1][11] = 82.0;
    adctimecut[1][12] = 117.0;
    adctimecut[1][13] = 92.0;
    adctimecut[1][14] = 118.0;
    adctimecut[1][15] = 90.0;
    adctimecut[1][16] = 112.0;
    adctimecut[1][17] = 108.0;
    adctimecut[1][18] = 96.0;
    adctimecut[1][19] = 101.0;
    adctimecut[1][20] = 122.0;
    adctimecut[1][21] = 96.0;
    adctimecut[1][22] = 95.0;
    adctimecut[1][23] = 90.0;

    //TDC7 
    adctimecut[7][0]  = 125.0;
    adctimecut[7][1]  = 100.0;
    adctimecut[7][2]  = 85.0;
    adctimecut[7][3]  = 81.0;
    adctimecut[7][4]  = 103.0;
    adctimecut[7][5]  = 120.0;
    adctimecut[7][6]  = 106.0;
    adctimecut[7][7]  = 94.0;
    adctimecut[7][8]  = 114.0;
    adctimecut[7][9]  = 110.0;
    adctimecut[7][10] = 123.0;
    adctimecut[7][11] = 96.0;
    adctimecut[7][12] = 128.0;
    adctimecut[7][13] = 95.0;
    adctimecut[7][14] = 93.0;
    adctimecut[7][15] = 100.0;
    adctimecut[7][16] = 120.0;
    adctimecut[7][17] = 103.0;
    adctimecut[7][18] = 88.0;
    adctimecut[7][19] = 112.0;
    adctimecut[7][20] = 117.0;
    adctimecut[7][21] = 95.0;
    adctimecut[7][22] = 110;
    adctimecut[7][23] = 90;

    //TDC8
    adctimecut[8][0]  = 115.0;
    adctimecut[8][1]  = 105.0;
    adctimecut[8][2]  = 92.0;
    adctimecut[8][3]  = 94.0;
    adctimecut[8][4]  = 102.0;
    adctimecut[8][5]  = 130.3;
    adctimecut[8][6]  = 110.3;
    adctimecut[8][7]  = 92.0;
    adctimecut[8][8]  = 115;
    adctimecut[8][9]  = 90;
    adctimecut[8][10] = 100;
    adctimecut[8][11] = 105;
    adctimecut[8][12] = 115;
    adctimecut[8][13] = 90;
    adctimecut[8][14] = 125;
    adctimecut[8][15] = 86;
    adctimecut[8][16] = 130;
    adctimecut[8][17] = 100;
    adctimecut[8][18] = 90;
    adctimecut[8][19] = 95;
    adctimecut[8][20] = 117;
    adctimecut[8][21] = 90;
    adctimecut[8][22] = 83;
    adctimecut[8][23] = 88;

    //TDC9
    adctimecut[9][0]  = 135;
    adctimecut[9][1]  = 90;
    adctimecut[9][2]  = 106;
    adctimecut[9][3]  = 94;
    adctimecut[9][4]  = 105;
    adctimecut[9][5]  = 130;
    adctimecut[9][6]  = 90;
    adctimecut[9][7]  = 85;
    adctimecut[9][8]  = 110;
    adctimecut[9][9]  = 100;
    adctimecut[9][10] = 82;
    adctimecut[9][11] = 90;
    adctimecut[9][12] = 121;
    adctimecut[9][13] = 102;
    adctimecut[9][14] = 114;
    adctimecut[9][15] = 103;
    adctimecut[9][16] = 125;
    adctimecut[9][17] = 105;
    adctimecut[9][18] = 105;
    adctimecut[9][19] = 104;
    adctimecut[9][20] = 112;
    adctimecut[9][21] = 115;
    adctimecut[9][22] = 95;
    adctimecut[9][23] = 92;

  }

  double Noisecut::ADCTimeCut(unsigned int tdc_id, unsigned int channel_id){
    return adctimecut[tdc_id][channel_id];
  }

  

}

#endif

