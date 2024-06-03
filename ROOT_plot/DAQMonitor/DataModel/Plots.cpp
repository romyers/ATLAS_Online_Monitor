#include "Plots.h"

#include <string>
#include <numeric>

#include "GlobalIncludes.h"

#include "MuonReco/Geometry.h"

using namespace MuonReco;
using namespace std;

Plots::Plots(const Plots &other) {

	p_leading_time  = dynamic_cast<TH1F*>(other.p_leading_time ->Clone());
	p_trailing_time = dynamic_cast<TH1F*>(other.p_trailing_time->Clone());

	p_hits_distribution.reserve(Geometry::MAX_TUBE_LAYER);
	for(int layer_id = 0; layer_id < Geometry::MAX_TUBE_LAYER; ++layer_id) {

		p_hits_distribution.push_back(dynamic_cast<TH1F*>(other.p_hits_distribution[layer_id]->Clone()));

	}

	p_tdc_time              .resize (Geometry::MAX_TDC) ;
	p_tdc_time_original     .resize (Geometry::MAX_TDC) ;
	p_tdc_time_corrected    .resize (Geometry::MAX_TDC) ;
	p_tdc_time_selected     .resize (Geometry::MAX_TDC) ;
	p_adc_time              .resize (Geometry::MAX_TDC) ;

	p_tdc_tdc_time_original .reserve(Geometry::MAX_TDC);
	p_tdc_tdc_time_corrected.reserve(Geometry::MAX_TDC);
	p_tdc_tdc_time_selected .reserve(Geometry::MAX_TDC);
	p_tdc_adc_time          .reserve(Geometry::MAX_TDC);
	p_tdc_channel           .reserve(Geometry::MAX_TDC);

	p_adc_vs_tdc            .reserve(Geometry::MAX_TDC);

	p_tdc_hit_rate_graph    .reserve(Geometry::MAX_TDC);

	for(int tdc = 0; tdc < Geometry::MAX_TDC; ++tdc) {

		p_tdc_adc_time          .push_back(dynamic_cast<TH1F*  >(other.p_tdc_adc_time[tdc]          ->Clone()));
		p_tdc_tdc_time_corrected.push_back(dynamic_cast<TH1F*  >(other.p_tdc_tdc_time_corrected[tdc]->Clone()));
		p_tdc_hit_rate_graph    .push_back(dynamic_cast<TGraph*>(other.p_tdc_hit_rate_graph[tdc]    ->Clone()));

		for(int channel = 0; channel < Geometry::MAX_TDC_CHANNEL; ++channel) {

			p_tdc_time_corrected[tdc].push_back(dynamic_cast<TH1F*>(other.p_tdc_time_corrected[tdc][channel]->Clone()));
			p_tdc_time[tdc]          .push_back(dynamic_cast<TH1F*>(other.p_tdc_time[tdc][channel]          ->Clone()));
			p_adc_time[tdc]          .push_back(dynamic_cast<TH1F*>(other.p_adc_time[tdc][channel]          ->Clone()));

		}

	}

	hitByLC         = dynamic_cast<TH2D*>(other.hitByLC        ->Clone());
	badHitByLC      = dynamic_cast<TH2D*>(other.badHitByLC     ->Clone());
	goodHitByLC     = dynamic_cast<TH2D*>(other.goodHitByLC    ->Clone());
	tube_efficiency = dynamic_cast<TH2D*>(other.tube_efficiency->Clone());

	residuals       = dynamic_cast<TH1D*>(other.residuals      ->Clone());

}

Plots::Plots() {

	TString plot_name_buffer;

	p_leading_time  = new TH1F("leading time spectrum" , "leading time spectrum", 100, 0, 1000);
	p_trailing_time = new TH1F("trailing time spectrum", "trailing time spectrum", 100, 0, 1000);

	p_hits_distribution.reserve(Geometry::MAX_TUBE_LAYER);
	for(int layer_id = 0; layer_id < Geometry::MAX_TUBE_LAYER; ++layer_id) {

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

	p_tdc_time              .resize (Geometry::MAX_TDC) ;
	p_tdc_time_original     .resize (Geometry::MAX_TDC) ;
	p_tdc_time_corrected    .resize (Geometry::MAX_TDC) ;
	p_tdc_time_selected     .resize (Geometry::MAX_TDC) ;
	p_adc_time              .resize (Geometry::MAX_TDC) ;

	p_tdc_tdc_time_original .reserve(Geometry::MAX_TDC);
	p_tdc_tdc_time_corrected.reserve(Geometry::MAX_TDC);
	p_tdc_tdc_time_selected .reserve(Geometry::MAX_TDC);
	p_tdc_adc_time          .reserve(Geometry::MAX_TDC);
	p_tdc_channel           .reserve(Geometry::MAX_TDC);

	p_adc_vs_tdc            .reserve(Geometry::MAX_TDC);

	p_tdc_hit_rate          .resize (Geometry::MAX_TDC);
	for(vector<double> &vec : p_tdc_hit_rate) {

		vec.resize(Geometry::MAX_TDC_CHANNEL);

	}

	p_tdc_hit_rate_graph    .reserve(Geometry::MAX_TDC);

	vector<double> p_tdc_hit_rate_x(Geometry::MAX_TDC_CHANNEL);
	iota(p_tdc_hit_rate_x.begin(), p_tdc_hit_rate_x.end(), 0.);

	for(int tdc = 0; tdc < Geometry::MAX_TDC; ++tdc) {

		// TODO: Eventually restore this check and make sure everything still
		//       lines up well. Adapt canvas divisions to the actual number of
		//       active TDCs, and make sure we're converting TDC number into
		//       the correct index with each plot.
		// TODO: We will want to store some metadata with everything to e.g.
		//       map TDC number to a plot index.

		// if(!Geometry::getInstance()->IsActiveTDC(tdc)) continue;

		plot_name_buffer.Form("TDC %d ADC Time Spectrum", tdc);
		p_tdc_adc_time.push_back(new TH1F(
			plot_name_buffer,
			plot_name_buffer,
			ADC_HIST_TOTAL_BIN,
			ADC_HIST_LEFT,
			ADC_HIST_RIGHT
		));
		p_tdc_adc_time.back()->GetXaxis()->SetTitle("time/ns");
		p_tdc_adc_time.back()->GetYaxis()->SetTitle("entries");

		plot_name_buffer.Form("TDC %d TDC Time Spectrum (Corrected)", tdc);
		p_tdc_tdc_time_corrected.push_back(new TH1F(
			plot_name_buffer, 
			plot_name_buffer,
			TDC_HIST_TOTAL_BIN,
			TDC_HIST_LEFT,
			TDC_HIST_RIGHT
		));
		p_tdc_tdc_time_corrected.back()->GetXaxis()->SetTitle("time/ns");
		p_tdc_tdc_time_corrected.back()->GetYaxis()->SetTitle("entries");

		plot_name_buffer.Form("tdc_%d_hit_rate", tdc);

		TGraph *graph = new TGraph(
			Geometry::MAX_TDC_CHANNEL,
			&p_tdc_hit_rate_x[0],
			&p_tdc_hit_rate[tdc][0]
		);
		graph->SetFillColor(4);
		graph->SetTitle(plot_name_buffer);
		graph->GetXaxis()->SetTitle("Channel No.");
		double tmp_yrange = graph->GetHistogram()->GetMaximum();
		graph->GetHistogram()->SetMaximum(tmp_yrange > 0.5 ? tmp_yrange : 1);
		graph->GetHistogram()->SetMinimum(0);
		graph->GetXaxis()->SetLimits(-0.5, static_cast<double>(Geometry::MAX_TDC_CHANNEL) - 0.5);
		graph->GetYaxis()->SetTitle("Rate(kHz)");
		p_tdc_hit_rate_graph.push_back(graph);


		for(int channel = 0; channel < Geometry::MAX_TDC_CHANNEL; ++channel) {

			plot_name_buffer.Form("TDC %d Channel %d TDC Time Spectrum (Corrected)", tdc, channel);
			p_tdc_time_corrected[tdc].push_back(new TH1F(
				plot_name_buffer,
				plot_name_buffer,
				TDC_HIST_TOTAL_BIN,
				TDC_HIST_LEFT,
				TDC_HIST_RIGHT
			));
	        p_tdc_time_corrected[tdc].back()->GetXaxis()->SetTitle("time/ns");
	        p_tdc_time_corrected[tdc].back()->GetYaxis()->SetTitle("entries");

	        plot_name_buffer.Form("TDC %d Channel %d TDC Time Spectrum", tdc, channel);
	        p_tdc_time[tdc].push_back(new TH1F(
	        	plot_name_buffer,
	        	plot_name_buffer, 
	        	TDC_HIST_TOTAL_BIN,
	        	TDC_HIST_LEFT, 
	        	TDC_HIST_RIGHT
	        ));
	        p_tdc_time[tdc].back()->GetXaxis()->SetTitle("time/ns");
	        p_tdc_time[tdc].back()->GetYaxis()->SetTitle("entries");

	        plot_name_buffer.Form("TDC %d Channel %d ADC Time Spectrum", tdc, channel);
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

	tube_efficiency = new TH2D(
		"Tube Efficiency",
		";Layer;Column",
		Geometry::MAX_TUBE_COLUMN, -0.5, Geometry::MAX_TUBE_COLUMN - 0.5,
		Geometry::MAX_TUBE_LAYER , -0.5, Geometry::MAX_TUBE_LAYER  - 0.5
	);

	residuals = new TH1D(
		"residuals",
		"Residuals;Residual [#mum];Number of hits/2 [#mum]",
		500,
		-500,
		500
	);
	residuals->SetStats(0);

}

void Plots::clear() {

	// TODO: We can also delete and remake everything to reset.

	p_leading_time ->Reset();
	p_trailing_time->Reset();

	for(TH1F *h : p_hits_distribution) { h->Reset(); }

	for(vector<TH1F*> &v : p_tdc_time) {

		for(TH1F *h : v) h->Reset();

	}

	for(vector<TH1F*> &v : p_tdc_time_original) {

		for(TH1F *h : v) h->Reset();

	}

	for(vector<TH1F*> &v : p_tdc_time_corrected) {

		for(TH1F *h : v) h->Reset();

	}

	for(vector<TH1F*> &v : p_tdc_time_selected) {

		for(TH1F *h : v) h->Reset();

	}

	for(vector<TH1F*> &v : p_adc_time) {

		for(TH1F *h : v) h->Reset();

	}


	for(TH1F *h : p_tdc_tdc_time_original ) { h->Reset(); }
	for(TH1F *h : p_tdc_tdc_time_corrected) { h->Reset(); }
	for(TH1F *h : p_tdc_tdc_time_selected ) { h->Reset(); }
	for(TH1F *h : p_tdc_adc_time          ) { h->Reset(); }
	for(TH1F *h : p_tdc_channel           ) { h->Reset(); }

	for(TH2F *h : p_adc_vs_tdc            ) { h->Reset(); }

	for(vector<double> &vec : p_tdc_hit_rate) {

		vec.clear();
		vec.resize(Geometry::MAX_TDC_CHANNEL);

	}

	for(TGraph *graph : p_tdc_hit_rate_graph) {

		for(int i = 0; i < Geometry::MAX_TDC_CHANNEL; ++i) {

			graph->SetPoint(i, i, 0.);
			double tmp_yrange = graph->GetHistogram()->GetMaximum();
			graph->GetHistogram()->SetMaximum(tmp_yrange > 0.5 ? tmp_yrange : 1);
			graph->GetHistogram()->SetMinimum(0);
			graph->GetXaxis()->SetLimits(-0.5, static_cast<double>(Geometry::MAX_TDC_CHANNEL) - 0.5);

		}

	}

	hitByLC        ->Reset();
	badHitByLC     ->Reset();
	goodHitByLC    ->Reset();
	tube_efficiency->Reset();

	residuals      ->Reset();

}
