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

TCanvas *canvas = new TCanvas("test", "test", 100, 100, 600, 300);

struct Histograms {

	Histograms();

	std::vector<TH1F*> p_tdc_adc_time;
	std::vector<TH1F*> p_tdc_tdc_time_corrected;

};

class PlotMaker {

public:

	PlotMaker();

	void binEvent(const Event &e);

private:

	Histograms histograms;

};

Histograms::Histograms() {

	for(int tdc = 0; tdc < Geometry::MAX_TDC; ++tdc) {

		if(!Geometry::getInstance()->IsActiveTDC(tdc)) continue;

		TString plot_name_buffer;

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

	}

}

PlotMaker::PlotMaker() {

	canvas->cd();
	canvas->Divide(2, 1, 0, 0);

}

void PlotMaker::binEvent(const Event &e) {

	for(const Hit &hit : e.Hits()) {

		histograms.p_tdc_adc_time[hit.TDC()]->Fill(hit.ADCTime());

		histograms.p_tdc_tdc_time_corrected[hit.TDC()]->Fill(hit.CorrTime());

		UILock.lock();
		canvas->cd(1);
		histograms.p_tdc_adc_time[hit.TDC()]->Draw();
		canvas->cd(2);
		histograms.p_tdc_tdc_time_corrected[hit.TDC()]->Draw();
		canvas->Update();
		UILock.unlock();

	}

}