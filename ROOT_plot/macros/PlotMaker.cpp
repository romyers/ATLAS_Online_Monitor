/**
 * @file PlotMaker.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <string>
#include <mutex>

#include "macros/GlobalIncludes.h"
#include "macros/Mutexes.cpp"

#include "src/Geometry.cpp"

using namespace std;

// TODO: Eventually I'll want to revamp this. Right now I'm in get 
//       things working and clean up later mode
// NOTE: EASY CLEANUP: Consolidate all the plots into an std::map. A lot of
//       things become easier if we just iterate over it

// TODO: Make a map of these
TCanvas *adc_canvas = new TCanvas("c1", "ADC Plots",0,0,2160,750);
TCanvas *tdc_canvas = new TCanvas("c2", "TDC Plots",0,750,2160,750);
TCanvas *rate_canvas = new TCanvas("c3", "Hit Rate Plots",2160,0,1800,750);
TCanvas *trigger_rate_canvas = new TCanvas("c4", "Trigger Board",1440,750,400,300);
TCanvas *residual_canvas = new TCanvas("c5", "Residuals", 2100,900,400,300);
TCanvas *EDCanvas = new TCanvas("c6", "Event Display", 2700, 900, 800, 800);
TCanvas *eff_canvas = new TCanvas("C7", "Efficiency", 2300, 900, 400, 300);

struct Plots {

	Plots();

	TH1F *                p_leading_time          ;
	TH1F *                p_trailing_time         ;

	vector<TH1F*>         p_hits_distribution     ;

	vector<vector<TH1F*>> p_tdc_time              ;
	vector<vector<TH1F*>> p_tdc_time_original     ;
	vector<vector<TH1F*>> p_tdc_time_corrected    ;
	vector<vector<TH1F*>> p_tdc_time_selected     ;
	vector<vector<TH1F*>> p_adc_time              ;


	vector<TH1F*>         p_tdc_tdc_time_original ;
	vector<TH1F*>         p_tdc_tdc_time_corrected;
	vector<TH1F*>         p_tdc_tdc_time_selected ;
	vector<TH1F*>         p_tdc_adc_time          ;
	vector<TH1F*>         p_tdc_channel           ;

	vector<TH2F*>         p_adc_vs_tdc            ;

	TGraph *              p_tdc_hit_rate_graph    ;

	TH2D *                hitByLC                 ;
	TH2D *                badHitByLC              ;
	TH2D *                goodHitByLC             ;

};

class PlotMaker {

public:

	PlotMaker();

	void binEvent(const Event &e);

private:

	Plots plots;

};

Plots::Plots() {

	TString plot_name_buffer;

	p_leading_time  = new TH1F("leading time spectrum" , "leading time spectrum", 100, 0, 1000);
	p_trailing_time = new TH1F("trailing time spectrum", "trailing time spectrum", 100, 0, 1000);

	p_hits_distribution.reserve(Geometry::MAX_TUBE_LAYER);
	for(int layer_id = 0; layer_id != Geometry::MAX_TUBE_LAYER; ++layer_id) {

		plot_name_buffer.Form("layer_%d_hits_distribution", layer_id);
		p_hits_distribution.push_back(
			new TH1F(
				plot_name_buffer, 
				plot_name_buffer, 
				Geometry::MAX_TUBE_COLUMN, 
				-0.5, 
				Geometry::MAX_TUBE_COLUMN - 0.5
			)
		);

	}

	p_tdc_time              .resize(Geometry::MAX_TDC) ;
	p_tdc_time_original     .resize(Geometry::MAX_TDC) ;
	p_tdc_time_corrected    .resize(Geometry::MAX_TDC) ;
	p_tdc_time_selected     .resize(Geometry::MAX_TDC) ;
	p_adc_time              .resize(Geometry::MAX_TDC) ;

	p_tdc_tdc_time_original .reserve(Geometry::MAX_TDC);
	p_tdc_tdc_time_corrected.reserve(Geometry::MAX_TDC);
	p_tdc_tdc_time_selected .reserve(Geometry::MAX_TDC);
	p_tdc_adc_time          .reserve(Geometry::MAX_TDC);
	p_tdc_channel           .reserve(Geometry::MAX_TDC);

	p_adc_vs_tdc            .reserve(Geometry::MAX_TDC);

	// TODO: Set up p_tdc_hit_rate_graph

	for(int tdc = 0; tdc < Geometry::MAX_TDC; ++tdc) {

		// TODO: Eventually restore this check and make sure everything still
		//       lines up well. Adapt canvas divisions to the actual number of
		//       active TDCs, and make sure we're converting TDC number into
		//       the correct index with each plot.
		// TODO: We will want to store some metadata with everything to e.g.
		//       map TDC number to a plot index.

		// if(!Geometry::getInstance()->IsActiveTDC(tdc)) continue;

		plot_name_buffer.Form("tdc_%d_adc_time_spectrum", tdc);
		p_tdc_adc_time.push_back(new TH1F(
			plot_name_buffer,
			plot_name_buffer,
			ADC_HIST_TOTAL_BIN,
			ADC_HIST_LEFT,
			ADC_HIST_RIGHT
		));
		p_tdc_adc_time.back()->GetXaxis()->SetTitle("time/ns");
		p_tdc_adc_time.back()->GetYaxis()->SetTitle("entries");

		string p_tdc_adc_time_corrected_title = "tdc_";
		p_tdc_adc_time_corrected_title += tdc;
		p_tdc_adc_time_corrected_title += "_tdc_time_spectrum_corrected";

		plot_name_buffer.Form("tdc_%d_tdc_time_spectrum_corrected", tdc);
		p_tdc_tdc_time_corrected.push_back(new TH1F(
			plot_name_buffer, 
			plot_name_buffer,
			TDC_HIST_TOTAL_BIN,
			TDC_HIST_LEFT,
			TDC_HIST_RIGHT
		));
		p_tdc_tdc_time_corrected.back()->GetXaxis()->SetTitle("time/ns");
		p_tdc_tdc_time_corrected.back()->GetYaxis()->SetTitle("entries");

		for(int channel = 0; channel < Geometry::MAX_TDC_CHANNEL; ++channel) {

			plot_name_buffer.Form("tdc_%d_channel_%d_tdc_time_spectrum_corrected", tdc, channel);
			p_tdc_time_corrected[tdc].push_back(new TH1F(
				plot_name_buffer,
				plot_name_buffer,
				TDC_HIST_TOTAL_BIN,
				TDC_HIST_LEFT,
				TDC_HIST_RIGHT
			));
	        p_tdc_time_corrected[tdc].back()->GetXaxis()->SetTitle("time/ns");
	        p_tdc_time_corrected[tdc].back()->GetYaxis()->SetTitle("entries");

	        plot_name_buffer.Form("tdc_%d_channel_%d_tdc_time_spectrum", tdc, channel);
	        p_tdc_time[tdc].push_back(new TH1F(
	        	plot_name_buffer,
	        	plot_name_buffer, 
	        	TDC_HIST_TOTAL_BIN,
	        	TDC_HIST_LEFT, 
	        	TDC_HIST_RIGHT
	        ));
	        p_tdc_time[tdc].back()->GetXaxis()->SetTitle("time/ns");
	        p_tdc_time[tdc].back()->GetYaxis()->SetTitle("entries");

	        plot_name_buffer.Form("tdc_%d_channel_%d_adc_time_spectrum", tdc, channel);
	        p_adc_time[tdc].push_back(new TH1F(
	        	plot_name_buffer, 
	        	plot_name_buffer,
	        	ADC_HIST_TOTAL_BIN, 
	        	ADC_HIST_LEFT, 
	        	ADC_HIST_RIGHT
	        ));
	        p_adc_time[tdc].back()->GetXaxis()->SetTitle("time/ns");
	        p_adc_time[tdc].back()->GetYaxis()->SetTitle("entries");

		}

	}

	hitByLC = new TH2D(
		"hitByLC", 
		"All hits on tubes (that passed clustering)", 
		54,
		-0.5,
		53.5,
		8,
		-0.5,
		7.5
	);
	hitByLC->SetStats(0);

	badHitByLC = new TH2D(
		"badHitByLC", 
		"Hits on tubes outside window (that passed clustering)", 
		54,
		-0.5,
		53.5,
		8,
		-0.5,
		7.5
	);
	badHitByLC->SetStats(0);

	goodHitByLC = new TH2D(
		"goodHitByLC", 
		"Hits on tubes inside window (that passed clustering)", 
		54,
		-0.5,
		53.5,
		8,
		-0.5,
		7.5
	);
	goodHitByLC->SetStats(0);

}

PlotMaker::PlotMaker() {

	UILock.lock();

	// TODO: These divisions should be determined by histogram array sizes
	adc_canvas->Divide(6, 2);
	tdc_canvas->Divide(6, 2);
	rate_canvas->Divide(6, 2);

	for(int i = 0; i < 11; ++i) {

		adc_canvas->cd(i + 1);
		plots.p_tdc_adc_time[i]->Draw();
		adc_canvas->Update();

		tdc_canvas->cd(i + 1);
		plots.p_tdc_tdc_time_corrected[i]->Draw();
		tdc_canvas->Update();

	}

	UILock.unlock();

}

void PlotMaker::binEvent(const Event &e) {

	// TODO: Event display
	// TODO: Go through DAQ.cpp and find everything we need to include
	// TODO: Make sure all plots print
	// TODO: Clean up and redesign UI

	for(const Hit &hit : e.Hits()) {

		plots.p_tdc_tdc_time_corrected[hit.TDC()]->Fill(hit.CorrTime());
		plots.p_tdc_adc_time[hit.TDC()]->Fill(hit.ADCTime());

		plots.p_tdc_time_corrected[hit.TDC()][hit.Channel()]->Fill(hit.CorrTime());
		plots.p_tdc_time          [hit.TDC()][hit.Channel()]->Fill(hit.DriftTime());
		plots.p_adc_time          [hit.TDC()][hit.Channel()]->Fill(hit.ADCTime());

		int hitL, hitC;
		Geometry::getInstance().GetHitLayerColumn(hit.TDC(), hit.Channel(), &hitL, &hitC);

		plots.hitByLC->Fill(hitC, hitL);
		if(hit.CorrTime() < 0 || hit.CorrTime() > 400) { // TODO: Magic numbers
			plots.badHitByLC->Fill(hitC, hitL);
		} else {
			plots.goodHitByLC->Fill(hitC, hitL);
		}
		plots.p_hits_distribution[hitL]->Fill(hitC);

		int panelIndex = hit.TDC() + 1;

		UILock.lock();

		adc_canvas->cd(panelIndex);
		plots.p_tdc_adc_time[hit.TDC()]->Draw();
		adc_canvas->Update();

		tdc_canvas->cd(panelIndex);
		plots.p_tdc_tdc_time_corrected[hit.TDC()]->Draw();
		tdc_canvas->Update();

		UILock.unlock();

	}

}