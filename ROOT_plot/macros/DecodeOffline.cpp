
/*******************************************************************************
  file name: DecodeRawData.cxx
  author: Zhe Yang
  created: 01/25/2019
  last modified: 04/26/2019

  description:
  -Decode .raw data from HPTDC and save data to ntuple

  remark:
  -Learned basic decode method from Shuzhou Zhang, redeveloped and added new
  function for new HPTDC data format.

*******************************************************************************/
#include "macros/GlobalIncludes.h"


#include <stdio.h>
#include <iostream>
#include <bitset>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TDirectory.h"
#include "TNtuple.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"

// Geometry includes
// #include "src/CheckEvent.cpp"

// Muon Reconstruction includes
#include "src/Signal.cpp"
#include "src/EventID.cpp"
#include "src/Event.cpp"
// #include "src/EventDisplay.cpp"
#include "src/Geometry.cpp"
#include "src/Hit.cpp"
#include "src/Cluster.cpp"
#include "src/TimeCorrection.cpp"
#include "src/HitFinder.cpp"
// #include "src/HitFinder_BCID.cpp"
#include "src/HitCluster.cpp"
#include "src/Noisecut.cpp"

//#define DECODER_DEBUG // comment this line when debugging information is not needed
//#define SET_MAXWORDS // comment this line if you want to decode the whole data words
#define SAVE_TRACKS_OUT_OF_ROOT // comment this line if you don't need to save plots out of rootfile 
#define ERROR_WORD 29
#define HEADER_WORD 31
#define TRAILER_WORD 30



using namespace std;
using namespace Muon;


/*
 * Get File Name from a Path with or without extension
 */
std::string getFileName(std::string filePath, bool withExtension = true, char seperator = '/')
{
    // Get last dot position
    std::size_t dotPos = filePath.rfind('.');
    std::size_t sepPos = filePath.rfind(seperator);
    if(sepPos != std::string::npos)
    {
        return filePath.substr(sepPos + 1, filePath.size() - (withExtension || dotPos != std::string::npos ? 1 : dotPos) );
    }
    return "";
}


int DecodeOffline(TString filename = "run_20210906_100742.dat") {
  gROOT->SetBatch(kTRUE); // set to batch mode to inprove the speed
  int maxEventCount = 100000000;
  // int maxEventCount = 100;
  // gStyle->SetOptStat(10); //only print entries
  gStyle->SetOptStat(1110);  //std, mean, entris, name printed
  // gStyle->SetTitleX(999.);//hist no title
  // gStyle->SetTitleY(999.);
  gStyle->SetStatY(0.9);                
  // Set y-position (fraction of pad size)
  gStyle->SetStatX(0.9);                
  // Set x-position (fraction of pad size)
  gStyle->SetStatW(0.25);                
  // Set width of stat-box (fraction of pad size)
  gStyle->SetStatH(0.25);                
  // Set height of stat-box (fraction of pad size)

  double match_window = 1.5;
  // open input file
  
  // TString input_filename = "/home/mdt-user/phase2_MiniDAQ/ROOT_plot/data/";
  TString input_filename = "data/";
  TString fn = TString(filename);
  input_filename += filename;
  ifstream data_in_flow;
  std::cout << input_filename.Data() << std::endl;
  data_in_flow.open(input_filename.Data());

  data_in_flow.seekg(0, data_in_flow.end);
  int data_in_flow_length = data_in_flow.tellg(); // get file size
  if (data_in_flow_length <=0) {
    printf("file name incorrect!\n");
    printf("file size = %d\n",data_in_flow_length);
    return 1;
  }
  else{
    printf("file size = %d\n",data_in_flow_length);
  }
  data_in_flow.seekg(0, data_in_flow.beg);

  // create output file
  system("mkdir output");
  chdir("output");
  char output_directoryname[256];
  strcpy(output_directoryname, filename);
  strcat(output_directoryname, ".dir");
  std::cout << "creating directory " << output_directoryname << std::endl;
  if (mkdir(output_directoryname, 0777) == -1) {
    cerr << strerror(errno) << endl;
  }
  chdir(output_directoryname);
  system("mkdir NoiseRate");
  char output_filename[256];
  strcpy(output_filename, filename);
  strcat(output_filename, ".out");

  char output_root_filename[200];
  strcpy(output_root_filename, output_filename);
  strcat(output_root_filename, ".root");
  TFile *p_output_rootfile = new TFile(output_root_filename, "RECREATE");


  // prepare file structure for event display
  TDirectory *event_track[2];
  char track_group_name[128];

  int runN = ((TObjString*)(TString(fn(3,256)).Tokenize("_")->At(0)))->String().Atoi();
  Geometry geo = Geometry();
  geo.SetRunN(runN);
  static TimeCorrection tc = TimeCorrection();
  // static EventDisplay   ed = EventDisplay();

  Noisecut ncut = Noisecut();


  TH1F *p_leading_time = new TH1F("leading time spectrum", "leading time spectrum", 100, 0, 1000);
  TH1F *p_trailing_time = new TH1F("trailing time spectrum", "trailing time spectrum", 100, 0, 1000);
  TH1F *p_hits_distribution[Geometry::MAX_TUBE_LAYER];
  char histogram_name[256];
  for (Int_t layer_id = 0; layer_id != Geometry::MAX_TUBE_LAYER; layer_id++) {
    sprintf(histogram_name, "layer_%d_hits_distribution", layer_id);
    p_hits_distribution[layer_id] = new TH1F(histogram_name, histogram_name, Geometry::MAX_TUBE_COLUMN, -0.5, Geometry::MAX_TUBE_COLUMN-0.5);
  }

  TH1F *p_tdc_time[Geometry::MAX_TDC][Geometry::MAX_TDC_CHANNEL];
  TH1F *p_tdc_time_original[Geometry::MAX_TDC][Geometry::MAX_TDC_CHANNEL];
  TH1F *p_tdc_time_corrected[Geometry::MAX_TDC][Geometry::MAX_TDC_CHANNEL];
  TH1F *p_tdc_time_selected[Geometry::MAX_TDC][Geometry::MAX_TDC_CHANNEL];
  TH1F *p_adc_time[Geometry::MAX_TDC][Geometry::MAX_TDC_CHANNEL];
  TH1F *p_tdc_tdc_time[Geometry::MAX_TDC];
  TH1F *p_tdc_tdc_time_original[Geometry::MAX_TDC];
  TH1F *p_tdc_tdc_time_corrected[Geometry::MAX_TDC];
  TH1F *p_tdc_tdc_time_selected[Geometry::MAX_TDC];
  TH1F *p_tdc_adc_time[Geometry::MAX_TDC];
  TH1F *p_tdc_channel[Geometry::MAX_TDC];


  TH2F *p_adc_vs_tdc[Geometry::MAX_TDC];
  TDirectory *tdc_directory[Geometry::MAX_TDC];
  char directory_name[256];
  double p_tdc_hit_rate[Geometry::MAX_TDC][Geometry::MAX_TDC_CHANNEL];
  double p_tdc_hit_rate_x[Geometry::MAX_TDC_CHANNEL];
  TGraph *p_tdc_hit_rate_graph[Geometry::MAX_TDC];
  memset(p_tdc_hit_rate, 0, sizeof(p_tdc_hit_rate));
  for (int i = 0; i < Geometry::MAX_TDC_CHANNEL; i++){
    p_tdc_hit_rate_x[i] = i;
  }

  TString h_name;

  for (Int_t tdc_id = 0; tdc_id != Geometry::MAX_TDC; tdc_id++) {
   if (geo.IsActiveTDC(tdc_id)) {    
    // create output directories
      sprintf(directory_name, "TDC_%02d_of_%02d_Time_Spectrum", tdc_id,Geometry::MAX_TDC);
      tdc_directory[tdc_id] = p_output_rootfile->mkdir(directory_name);
      tdc_directory[tdc_id]->cd();
      if (mkdir(directory_name, 0777) == -1) {
        cerr << strerror(errno) << endl;
      }
      
      h_name.Form("tdc_%d_tdc_time_spectrum_corrected", tdc_id);
      p_tdc_tdc_time_corrected[tdc_id] = new TH1F(h_name, h_name,TDC_HIST_TOTAL_BIN, TDC_HIST_LEFT, TDC_HIST_RIGHT);
      p_tdc_tdc_time_corrected[tdc_id]->GetXaxis()->SetTitle("time/ns");
      p_tdc_tdc_time_corrected[tdc_id]->GetYaxis()->SetTitle("entries");
      
      h_name.Form("tdc_%d_adc_time_spectrum", tdc_id);
      p_tdc_adc_time[tdc_id] = new TH1F(h_name, h_name, ADC_HIST_TOTAL_BIN, ADC_HIST_LEFT, ADC_HIST_RIGHT);
      p_tdc_adc_time[tdc_id]->GetXaxis()->SetTitle("time/ns");
      p_tdc_adc_time[tdc_id]->GetYaxis()->SetTitle("entries");

      
      
      for (Int_t channel_id = 0; channel_id != Geometry::MAX_TDC_CHANNEL; channel_id++) {
        h_name.Form("tdc_%d_channel_%d_tdc_time_spectrum_corrected", tdc_id, channel_id);
        p_tdc_time_corrected[tdc_id][channel_id] = new TH1F(h_name,h_name, TDC_HIST_TOTAL_BIN,TDC_HIST_LEFT, TDC_HIST_RIGHT);
        p_tdc_time_corrected[tdc_id][channel_id]->GetXaxis()->SetTitle("time/ns");
        p_tdc_time_corrected[tdc_id][channel_id]->GetYaxis()->SetTitle("entries");
        
        h_name.Form("tdc_%d_channel_%d_tdc_time_spectrum", tdc_id, channel_id);
        p_tdc_time[tdc_id][channel_id] = new TH1F(h_name,h_name, TDC_HIST_TOTAL_BIN,TDC_HIST_LEFT, TDC_HIST_RIGHT);
        p_tdc_time[tdc_id][channel_id]->GetXaxis()->SetTitle("time/ns");
        p_tdc_time[tdc_id][channel_id]->GetYaxis()->SetTitle("entries");

        h_name.Form("tdc_%d_channel_%d_adc_time_spectrum", tdc_id, channel_id);
        p_adc_time[tdc_id][channel_id] = new TH1F(h_name, h_name,ADC_HIST_TOTAL_BIN, ADC_HIST_LEFT, ADC_HIST_RIGHT);
        p_adc_time[tdc_id][channel_id]->GetXaxis()->SetTitle("time/ns");
        p_adc_time[tdc_id][channel_id]->GetYaxis()->SetTitle("entries");
        
      }
   }
  } // end for: all TDC



  TH2D* hitByLC = new TH2D("hitByLC", "All hits on tubes (that passed clustering)", 54,-0.5,53.5,8,-0.5,7.5);
  hitByLC->SetStats(0);
  TH2D* badHitByLC = new TH2D("badHitByLC", "Hits on tubes outside window (that passed clustering)", 54,-0.5,53.5,8,-0.5,7.5);
  badHitByLC->SetStats(0);
  TH2D* goodHitByLC = new TH2D("goodHitByLC", "Hits on tubes inside window (that passed clustering)", 54,-0.5,53.5,8,-0.5,7.5);
  goodHitByLC->SetStats(0);

  int hitL, hitC;
  int current_track_group = 0;
  Bool_t first_signal_flag[Geometry::MAX_TDC][Geometry::MAX_TDC_CHANNEL];
  int temp_track_group;
  bool pass_event_check;
  uint64_t word=0;  //sizeof(long)=8, which equals uint64_t.
  // unsigned int word;
  char readbuff[5]={0,0,0,0,0};
  int unexpected_data=0;
  unsigned int header_type;
  unsigned int current_event_ID=0;
  unsigned int previous_event_ID=-1;
  unsigned int tdc_header_eventID_current = 0;
  unsigned int tdc_header_eventID_previous = -1;


  // EventID currEventID;
  // EventID prevEventID = EventID(0x00000000);
  vector<Signal>  sigVec;
  bitset<4> header;
  bitset<1> bit1;
  bitset<3> bit3;
  bitset<3> bit4;
  bitset<5> bit5;

  unsigned int header_count_Event_trailer;
  unsigned int trailer_count_Event_trailer;
  unsigned int header_count_TDC_data;
  unsigned int trailer_count_TDC_data;

  unsigned int error_flag;
  unsigned int error_channel;
  int error_word_count;
  Signal sig, header_sig, trailer_sig;
  Event  event = Event();
  TTree* eTree = new TTree("eTree", "eTree");
  eTree->Branch("event", "Event", &event);
  cout << "Processing..." << endl;

  unsigned long total_triggers = 0;
  unsigned long total_events   = 0;
  unsigned long total_triggers_pass = 0;
  unsigned long total_events_pass = 0;
  unsigned long total_signals = 0;
  unsigned long event_signals = 0;
  unsigned long total_signals_pass = 0;
  unsigned long total_events_fail  = 0;
  unsigned long event_print = 100;


  int nloop = 0;
  int readbytes = 0;
  while (data_in_flow.read(readbuff, 5)  && total_events<maxEventCount) {
    // while (data_in_flow.read((char *) &word, 4)  && nloop<maxEventCount) {
    nloop++;
    word = (((uint64_t)readbuff[4])&0xff) +
          ((((uint64_t)readbuff[3])&0xff)<<8)+
          ((((uint64_t)readbuff[2])&0xff)<<16)+
          ((((uint64_t)readbuff[1])&0xff)<<24)+
          ((((uint64_t)readbuff[0])&0xff)<<32);
          //networking uses big-endian while processor uses small-endian, so a byte-swap is used here.

    // word = readbuff[0] + readbuff[0]*256 + readbuff[0]*65536 + readbuff[0]*16777216 + readbuff[0]*4294967296;
    header = word >> 36; // get the four bits header of this word
    header_type = static_cast<unsigned int>((header.to_ulong()));

    
    if (header_type == Signal::TRAILER) {
      // analyze data if we reached a trailer for an event 
      if (total_events % event_print == 0) {
        std::cout << "Processing Event " << total_events << std::endl;
        if (TMath::Floor(TMath::Log10(total_events)) > TMath::Floor(TMath::Log10(event_print))) 
          event_print*=10;
      }
      trailer_sig = Signal(word);
      if(trailer_sig.TrailerEID()!=header_sig.HeaderEID()){
        printf("Warning! Event ID mismatch! HEADER ID= %d, TRAILER ID = %d\n",header_sig.HeaderEID(),trailer_sig.TrailerEID());
        sigVec.clear();
        continue;
      }
      bit4 = word >> 32; // get the TDC header count in Event trailer 
      header_count_Event_trailer = static_cast<unsigned int>((bit4.to_ulong()));
      if(header_count_Event_trailer!=header_count_TDC_data){
        printf("Warning! %d header(s) found in data, event trailer indicates %d!\n",header_count_TDC_data,header_count_Event_trailer);
      }
      bit4 = word >> 28; // get the TDC trailer count in Event trailer 
      trailer_count_Event_trailer = static_cast<unsigned int>((bit4.to_ulong()));
      if(trailer_count_Event_trailer!=trailer_count_TDC_data){
        printf("Warning! %d trailer(s) found in data, event trailer indicates %d!\n",header_count_TDC_data,header_count_Event_trailer);
      }
      bit1 = word >> 15; // get the TDC header count error flag in Event trailer
      unsigned int error_flag_tmp;
      error_flag_tmp = static_cast<unsigned int>((bit1.to_ulong()));
      if(error_flag_tmp){
        printf("Warning! %d header count error flag. Got %d header(s)!\n",header_count_Event_trailer);
      }
      bit1 = word >> 14; // get the TDC trailer count error flag in Event trailer
      error_flag_tmp = static_cast<unsigned int>((bit1.to_ulong()));
      if(error_flag_tmp){
        printf("Warning! %d trailer count error flag. Got %d trailer(s)!\n",trailer_count_Event_trailer);
      }

      total_events++;
      total_signals += event_signals;
      if(trailer_sig.HitCount()!=(sigVec.size()+error_word_count)){
        printf("Warning! Hit count in trailer = %d, real hit count = %d, error word count = %d\n",\
          trailer_sig.HitCount(),sigVec.size(),error_word_count);
      }
      

      // std::cout << "trailer_sig.HitCount() "<< trailer_sig.HitCount() << std::endl; 
      if(trailer_sig.HitCount()!=0){
        event = Event(header_sig,trailer_sig,sigVec);
        // DoHitFinding(&event,tc,ncut,0);
       DoHitFinding(&event,tc,0,0);
        //DoHitClustering(&event, geo);
        pass_event_check = kTRUE;
        //pass_event_check = CheckEvent(event, geo);
        event.SetPassCheck(pass_event_check);
        event.CheckClusterTime();
        if (pass_event_check) {      
            eTree->Fill();
        }
        // for (Cluster c : event.Clusters()) {
          for (Hit h : event.Hits()) {

            // for (Hit h : c.Hits()) {

              p_tdc_tdc_time_corrected[h.TDC()]->Fill(h.CorrTime());
              p_tdc_adc_time          [h.TDC()]->Fill(h.ADCTime()); 

              p_tdc_time_corrected[h.TDC()][h.Channel()]->Fill(h.CorrTime());
              p_tdc_time          [h.TDC()][h.Channel()]->Fill(h.DriftTime());
	      
	      // std::cout << "TDC "<< h.TDC() << " Channel " << h.Channel() << " CorrTime " << h.CorrTime() << " DriftTime " << h.DriftTime() << std::endl;
	      
// 	      if(h.TDC() == 2){
// 		      std::cout << " TDC 2 Channel " << h.Channel() << " CorrTime " << h.CorrTime() << " DriftTime " << h.DriftTime() << std::endl;
// 	      }
// 	      if(h.TDC() == 3){
// 		      std::cout << "TDC 3 Channel " << h.Channel() << " CorrTime " << h.CorrTime() << " DriftTime " << h.DriftTime() << std::endl;
// 	      }

              p_adc_time          [h.TDC()][h.Channel()]->Fill(h.ADCTime());

              geo.GetHitLayerColumn(h.TDC(), h.Channel(), &hitL, &hitC);
              hitByLC->Fill(hitC, hitL);
              if (h.CorrTime() <0 || h.CorrTime()>400)
                badHitByLC->Fill(hitC, hitL);
              else
                goodHitByLC->Fill(hitC, hitL);
              p_hits_distribution[hitL]->Fill(hitC);
          }  //end for (Hit h : event.Hits())
        // } //end for (Cluster c : event.Clusters())


        if (pass_event_check) {
          total_events_pass++;
        }
        else {
          total_events_fail++;
        }

          // plot the event
          // plot the first 10 events meeting and not meeting the pass event check criteria
        if ((pass_event_check && total_events_pass < 10) || (!pass_event_check && total_events_fail < 10)) { 

          if (pass_event_check)
            sprintf(track_group_name, "events_passing");
          else
            sprintf(track_group_name, "events_failing");
          
          if ((pass_event_check && total_events_pass == 1) || (!pass_event_check&& total_events_fail == 1) )
            event_track[(int)pass_event_check] = p_output_rootfile->mkdir(track_group_name);

          
          #ifdef SAVE_TRACKS_OUT_OF_ROOT
          if (mkdir(track_group_name, 0777) == -1) {
            cerr << strerror(errno) << endl;
          }
          #endif

          event_track[(int)pass_event_check]->cd();
          // chdir(track_group_name);
          // ed.DrawEvent(event, geo, event_track[(int)pass_event_check]);
          // chdir("..");
          // ed.Clear();
        } // end if: pass event check for first 100 events
      }
    } // end if: got event trailer

    //got event header
    else if (header_type == Signal::HEADER){
      header_sig = Signal(word);
      current_event_ID = header_sig.HeaderEID();
      if((current_event_ID!=(previous_event_ID+1)%4096)&&(previous_event_ID!=-1))
        printf("Warning! Event lost! Curr=%d, Pre=%d\n", current_event_ID,previous_event_ID);
      if(current_event_ID==previous_event_ID)
        printf("Warning! Repeating event! Curr=%d, Pre=%d\n", current_event_ID,previous_event_ID);
      previous_event_ID = current_event_ID;
      sigVec.clear();
      event_signals = 0;
      error_word_count = 0;
      tdc_header_eventID_current = 0;
      tdc_header_eventID_previous = -1; //reset tdc header eventID for each event
      header_count_TDC_data = 0;
      trailer_count_TDC_data = 0;
    }

    //got hit data
    else {
      sig = Signal(word);
      if(geo.IsActiveTDC(sig.TDC())&&sig.Channel()<Geometry::MAX_TDC_CHANNEL){
        // printf("data from TDC %d, Channel = %d\n",sig.TDC(),sig.Channel());
        sigVec.push_back(sig);
      }
      else if(geo.IsActiveTDC(sig.TDC())&&sig.Channel()==ERROR_WORD){  //TDC triggerless error word
        bit3 = word >> 5; // get the LSB error flag 
        error_flag = static_cast<unsigned int>((bit3.to_ulong()));
        error_word_count++;
        if(error_flag>0){
          bit5 = word >> 0; // get the LSB error channel 
          error_channel = static_cast<unsigned int>((bit5.to_ulong()));
          printf("Warning! TDCID = %d, 	Channel = %d 	overflowed!\n",sig.TDC(),error_channel);
        }
        bit3 = word >> 13; // get the second LSB error flag 
        error_flag = static_cast<unsigned int>((bit3.to_ulong()));
        if(error_flag>0){
          bit5 = word >> 8; // get the second LSB error channel 
          error_channel = static_cast<unsigned int>((bit5.to_ulong()));
          printf("Warning! TDCID = %d, 	Channel = %d 	overflowed!\n",sig.TDC(),error_channel);
        }
      }
      else if(geo.IsActiveTDC(sig.TDC())&&sig.Channel()==HEADER_WORD){
        header_count_TDC_data++;
        tdc_header_eventID_current = sig.TDCHeaderEID();
        if((tdc_header_eventID_current!=tdc_header_eventID_previous)&&(tdc_header_eventID_previous!=-1)){
          printf("TDC %d EventID mismatch! Current = %d, previous = %d\n",sig.TDC(),tdc_header_eventID_current,tdc_header_eventID_previous);
        }
        tdc_header_eventID_previous = tdc_header_eventID_current;
      }
      else if(geo.IsActiveTDC(sig.TDC())&&sig.Channel()==TRAILER_WORD){
        trailer_count_TDC_data++;
      }
      else{
        printf("Error! At event %d unexpected data TDCID = %d, Channel=%d\n",total_events,sig.TDC(),sig.Channel());
        unexpected_data++;
        if(unexpected_data>10) return(1);
      }      
      event_signals++;
      // if(event_signals>1000)return 0;
    }
  } // end while dataloop
  cout << "Decoding completed !" << endl;
  if(total_events<=1){
    cout << "No event! Terminated." << endl;
    return 1;
  }

  /* plot the time spectrum for leading and trailing edge */
  cout << "Making plots... " << endl;


  // cout << endl << "Plotting total leading edge spectrum... " << endl;
  // TCanvas *p_leading_canvas = new TCanvas("leading", "leading", 0, 0, 800,
  //                                         450);
  // p_leading_canvas->cd();
  // p_leading_time->GetXaxis()->SetTitle("time/ns");
  // p_leading_time->Draw();

  // cout << endl << "Plotting total trailing edge spectrum... " << endl;
  // TCanvas *p_trailing_canvas = new TCanvas("trailing", "trailing", 0, 450,
  //                                          800, 450);
  // p_trailing_canvas->cd();
  // p_trailing_time->GetXaxis()->SetTitle("time/ns");
  // p_trailing_time->Draw();

  // cout << endl << "Plotting hits distribution... " << endl;
  // TCanvas *p_hits_canvas[Geometry::MAX_TUBE_LAYER];
  // TCanvas *p_hits_together_canvas = new TCanvas("layer_distribution", "layer_distribution");
  // char canvas_name[256];
  // for (Int_t layer_id = Geometry::MAX_TUBE_LAYER - 1; layer_id != -1; layer_id--) {
  //   sprintf(canvas_name, "layer_%d_hits_distribution", layer_id);
  //   //p_hits_canvas[layer_id] = new TCanvas(canvas_name, canvas_name);
  //   //p_hits_canvas[layer_id]->cd();
  //   //p_hits_distribution[layer_id]->Draw();
  //   p_hits_together_canvas->cd();
  //   p_hits_distribution[layer_id]->SetLineColor(layer_id + 1);
  //   p_hits_distribution[layer_id]->Draw("same");
  // }

  // p_hits_together_canvas->BuildLegend(0.75, 0.75, 1, 1);
  // p_hits_together_canvas->SaveAs("layer_distributions.png");



  // TDirectory* metaPlots = p_output_rootfile->mkdir("composite");
  // ed.DrawTubeHist(geo, hitByLC,     metaPlots);
  // ed.Clear();
  // cout << "1" << endl;
  // ed.DrawTubeHist(geo, goodHitByLC, metaPlots);
  // ed.Clear();
  // cout << "2" << endl;
  // ed.DrawTubeHist(geo, badHitByLC,  metaPlots);
  // ed.Clear();
  // cout << "3" << endl;
  p_output_rootfile->cd();
  // eTree->Write();
  // hitByLC->Write();
  // badHitByLC->Write();
  // goodHitByLC->Write();
  p_output_rootfile->Write();

  // export data to output directory
  #ifdef SAVE_TRACKS_OUT_OF_ROOT
  TCanvas *p_output_canvas = new TCanvas("", "");
  p_output_canvas->cd();
  for (Int_t tdc_id = 0; tdc_id != Geometry::MAX_TDC; tdc_id++) {
    if (geo.IsActiveTDC(tdc_id)) {
      for(int tdc_chnl_id = 0; tdc_chnl_id != Geometry::MAX_TDC_CHANNEL; tdc_chnl_id++){
        p_tdc_hit_rate[tdc_id][tdc_chnl_id] = 
        p_adc_time[tdc_id][tdc_chnl_id]->GetEntries()/match_window*1000/total_events;
      }

      chdir("NoiseRate");
      h_name.Form("tdc_%d_hit_rate", tdc_id);
      sprintf(output_filename, "tdc_%d_hit_rate.png", tdc_id);
      p_tdc_hit_rate_graph[tdc_id] = new TGraph(Geometry::MAX_TDC_CHANNEL, p_tdc_hit_rate_x, p_tdc_hit_rate[tdc_id]);
      p_tdc_hit_rate_graph[tdc_id]->SetFillColor(4);
      p_tdc_hit_rate_graph[tdc_id]->SetTitle(h_name);
      p_tdc_hit_rate_graph[tdc_id]->GetXaxis()->SetTitle("Channel No.");
      double tmp_yrange = p_tdc_hit_rate_graph[tdc_id]->GetHistogram()->GetMaximum();
      p_tdc_hit_rate_graph[tdc_id]->GetHistogram()->SetMaximum(tmp_yrange>0.5?tmp_yrange:1);  
      p_tdc_hit_rate_graph[tdc_id]->GetHistogram()->SetMinimum(0);      
      p_tdc_hit_rate_graph[tdc_id]->GetXaxis()->SetLimits(-0.5,23.5);
      p_tdc_hit_rate_graph[tdc_id]->GetYaxis()->SetTitle("Rate(kHz)");          
      p_tdc_hit_rate_graph[tdc_id]->Draw("AB");
      TText *xlabel = new TText();
      xlabel -> SetNDC();
      xlabel -> SetTextFont(42);
      xlabel -> SetTextSize(0.05);
      xlabel -> SetTextAngle(0);
      string text_content;
      xlabel -> DrawText(0.5, 0.9, text_content.c_str());
      text_content = "Max  = "+to_string(TMath::MaxElement(MAX_CHNL_COUNT,p_tdc_hit_rate[tdc_id])).substr(0,6)+" kHz";
      xlabel -> DrawText(0.5, 0.85, text_content.c_str());
      text_content = "Mean = "+to_string(TMath::Mean(MAX_CHNL_COUNT,p_tdc_hit_rate[tdc_id])).substr(0,6)+" kHz";
      xlabel -> DrawText(0.5, 0.8, text_content.c_str());
      TLine *l = new TLine(-0.5,0.5,23.5,0.5);
      l->Draw();
      p_output_canvas->SaveAs(output_filename);
      chdir("..");
      delete p_tdc_hit_rate_graph[tdc_id];

      printf("TDC %d has total entries %f \n", tdc_id,p_tdc_adc_time[tdc_id]->GetEntries());
      sprintf(directory_name, "TDC_%02d_of_%02d_Time_Spectrum", tdc_id, Geometry::MAX_TDC);
      chdir(directory_name);

      p_tdc_tdc_time_corrected[tdc_id]->Draw();
      sprintf(output_filename, "tdc_%d_tdc_time_spectrum_corrected.png", tdc_id);
      p_output_canvas->SaveAs(output_filename);
      
      p_tdc_adc_time[tdc_id]->Draw();
      sprintf(output_filename, "tdc_%d__adc_time_spectrum.png", tdc_id);
      p_output_canvas->SaveAs(output_filename);

      for (Int_t channel_id = 0; channel_id != Geometry::MAX_TDC_CHANNEL;channel_id++) {
        if (geo.IsActiveTDCChannel(tdc_id, channel_id)||(tdc_id==geo.TRIGGER_MEZZ)) {

          p_tdc_time_corrected[tdc_id][channel_id]->Draw();
          sprintf(output_filename,"tdc_%d__channel_%d__tdc_time_spectrum_corrected.png",tdc_id, channel_id);
          p_output_canvas->SaveAs(output_filename);
          
          p_tdc_time[tdc_id][channel_id]->Draw();
          sprintf(output_filename,"tdc_%d__channel_%d__tdc_time_spectrum.png",tdc_id, channel_id);
          p_output_canvas->SaveAs(output_filename);

          p_adc_time[tdc_id][channel_id]->Draw();
          sprintf(output_filename, "tdc_%d__channel_%d__adc_time_spectrum.png",tdc_id, channel_id);
          p_output_canvas->SaveAs(output_filename);
        }
      }
      chdir("..");
    } // end if: active TDC
  }
  #endif

  int nEntries = eTree->GetEntries();
  
  delete p_output_rootfile;
  //delete track_base;
  // delete p_leading_canvas;
  // delete p_trailing_canvas;
  

  cout << endl;
  cout << "Total Triggers: " << total_triggers << endl;
  cout << "Pass  Triggers: " << total_triggers_pass << endl;
  cout << endl;
  cout << "Total Events:   " << total_events   << endl;
  cout << "Pass  Events:   " << total_events_pass << endl;
  cout << endl;
  cout << "Total Signals:  " << total_signals  << endl;
  cout << "Pass  Signals:  " << total_signals_pass << endl;
  cout << endl;
  cout << "N tree entries: " << nEntries << endl;


  
  //gROOT->SetBatch(kFALSE);
  return 0;
}
    
