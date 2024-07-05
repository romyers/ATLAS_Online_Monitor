#include "Plots.h"

#include <string>
#include <numeric>

#include "GlobalIncludes.h"

#include "MuonReco/Geometry.h"
#include "MuonReco/TrackParam.h"
#include "MuonReco/Track.h"

using namespace MuonReco;
using namespace std;

// TODO: Is there a better place to put this? E.g. Geometry.cpp?
const double MATCH_WINDOW = 1.5; // us

Plots::Plots(const Plots &other) : geo(other.geo), rtp(other.rtp) {

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

    nHits = other.nHits;
    nTotal = other.nTotal;

}

Plots::Plots(Geometry &geo, RTParam &rtp) : geo(geo), rtp(rtp) {

}

void Plots::initialize() {

    clear();
    
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

    nHits.resize(Geometry::MAX_TUBE_LAYER);
	nTotal.resize(Geometry::MAX_TUBE_LAYER);

	for(size_t i = 0; i < Geometry::MAX_TUBE_LAYER; ++i) {

		nHits[i].resize(Geometry::MAX_TUBE_COLUMN);
		nTotal[i].resize(Geometry::MAX_TUBE_COLUMN);

	}

    p_tdc_hit_rate_x.resize(Geometry::MAX_TDC_CHANNEL);
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

void Plots::updateHitRate(int total_events) {

	for(int tdc = 0; tdc < Geometry::MAX_TDC; ++tdc) {

		for(int chnl = 0; chnl < Geometry::MAX_TDC_CHANNEL; ++chnl) {

			// Hits / total_events * (1 / MATCH_WINDOW (us)) * 1000 (us / ms)
			p_tdc_hit_rate[tdc][chnl] 
				= p_adc_time[tdc][chnl]->GetEntries() / total_events 
				  *
				  1000 / MATCH_WINDOW;

			p_tdc_hit_rate_graph[tdc]->SetPoint(
				chnl, 
				chnl, 
				p_tdc_hit_rate[tdc][chnl]
			);

			double tmp_yrange = p_tdc_hit_rate_graph[tdc]->GetHistogram()->GetMaximum();
			p_tdc_hit_rate_graph[tdc]->GetHistogram()->SetMaximum(tmp_yrange > 0.5 ? tmp_yrange : 1);
			p_tdc_hit_rate_graph[tdc]->GetHistogram()->SetMinimum(0);
			p_tdc_hit_rate_graph[tdc]->GetXaxis()->SetLimits(-0.5, static_cast<double>(Geometry::MAX_TDC_CHANNEL) - 0.5);

		}

	}

}

void Plots::binEvent(Event &e, TTree &optTree) {

	// TODO: Event display
	// TODO: Go through DAQ.cpp and find everything we need to include
	// TODO: Make sure all plots print

	for(const Hit &hit : e.WireHits()) {

		p_tdc_tdc_time_corrected[hit.TDC()]->Fill(hit.CorrTime());
		p_tdc_adc_time          [hit.TDC()]->Fill(hit.ADCTime ());

		p_tdc_time_corrected[hit.TDC()][hit.Channel()]->Fill(hit.CorrTime ());
		p_tdc_time          [hit.TDC()][hit.Channel()]->Fill(hit.DriftTime());
		p_adc_time          [hit.TDC()][hit.Channel()]->Fill(hit.ADCTime  ());

		double tmp_yrange = p_tdc_hit_rate_graph[hit.TDC()]->GetHistogram()->GetMaximum();
		p_tdc_hit_rate_graph[hit.TDC()]->GetHistogram()->SetMaximum(tmp_yrange > 0.5 ? tmp_yrange : 1);

		int hitL, hitC;
		geo.GetHitLayerColumn(hit.TDC(), hit.Channel(), &hitL, &hitC);

		hitByLC->Fill(hitC, hitL);
		if(hit.CorrTime() < 0 || hit.CorrTime() > 400) { // TODO: Magic numbers

			badHitByLC->Fill(hitC, hitL);
		
		} else {
		
			goodHitByLC->Fill(hitC, hitL);
		
		}
		p_hits_distribution[hitL]->Fill(hitC);

	}

	// Residuals, efficiency, and event display modified from work by 
	// Rongqian Qian. See:
	// https://github.com/Rong-qian/ATLAS_Online_Monitor/
	if(e.Pass()) {

        TrackParam tp;

        tp.SetRT(&rtp);
        tp.setVerbose(0);
        tp.setMaxResidual(1000000);

		tp.setTarget(&optTree);
		tp.setRangeSingle(0);
		tp.setIgnoreNone();
		tp.optimize();

		// Populate residuals
		for(Cluster &c : e.Clusters()) {

			for(Hit &hit : c.Hits()) {

				residuals->Fill(tp.Residual(hit) * 1000.0);

			}

		}

		// Populate efficiency
		// Iterate through each tube via tdc and channel index
		for(int tdc_index = 0; tdc_index < Geometry::MAX_TDC; ++tdc_index) {

			for(int ch_index = 0; ch_index < Geometry::MAX_TDC_CHANNEL; ++ch_index) {

				// If the channel is active,
				if(geo.IsActiveTDCChannel(tdc_index, ch_index)) {

					int iL, iC;
                    double _hitX, _hitY;

					geo.GetHitLayerColumn(tdc_index, ch_index, &iL, &iC);
					geo.GetHitXY(tdc_index, ch_index, &_hitX, &_hitY);

					// get track x position and figure out what tube(s) it may go through
					double trackDist = tp.Distance(Hit(
						0, 0, 0, 0, 0, 0, iL, iC, _hitX, _hitY
					));

					if(trackDist <= Geometry::column_distance / 2) {

                        bool tubeIsHit = false;

						for(Hit hit : e.WireHits()) {

							int hit_layer;
							int hit_column;

							geo.GetHitLayerColumn(
								hit.TDC(), 
								hit.Channel(),
								&hit_layer,
								&hit_column
							);

                            // If this is true, the tube was hit
							if(hit_layer == iL && hit_column == iC) {

                                tubeIsHit = true;

							}

						}
                        
                        ++nTotal[iL][iC];

                        if(tubeIsHit) ++nHits[iL][iC];

					}

				}

			}

		}
        
		for(int iL = 0; iL < Geometry::MAX_TUBE_LAYER; ++iL) {

			for(int iC = 0; iC < Geometry::MAX_TUBE_COLUMN; ++iC) {

				if(nHits[iL][iC] != 0) {

                    // FIXME: Make sure the axes are right
					tube_efficiency->SetBinContent(
						iC + 1, 
						iL + 1, 
						nHits[iL][iC] / nTotal[iL][iC]
					);

				}

			}

		}

        e.AddTrack(Track(tp.vertical_angle(), tp.x_int()));
        eventDisplayBuffer.push_back(e);

	}

}

void Plots::clear() {

    // TODO: Use ROOT versions of delete?

    if(p_leading_time)  {
        delete p_leading_time;
        p_leading_time = nullptr;
    }

    if(p_trailing_time) {
        delete p_trailing_time;
        p_trailing_time = nullptr;
    }

	for(TH1F *h : p_hits_distribution) { 
        if(h) {
            delete h;
            h = nullptr;
        }
    }
    p_hits_distribution.clear();

	for(vector<TH1F*> &v : p_tdc_time) {

		for(TH1F *h : v) {
            if(h) {
                delete h;
                h = nullptr;
            }
        }

	}
    p_tdc_time.clear();

	for(vector<TH1F*> &v : p_tdc_time_original) {

		for(TH1F *h : v) {
            if(h) {
                delete h;
                h = nullptr;
            }
        }

	}
    p_tdc_time_original.clear();

	for(vector<TH1F*> &v : p_tdc_time_corrected) {

		for(TH1F *h : v) {
            if(h) {
                delete h;
                h = nullptr;
            }
        } 

	}
    p_tdc_time_corrected.clear();

	for(vector<TH1F*> &v : p_tdc_time_selected) {

		for(TH1F *h : v) {
            if(h) {
                delete h;
                h = nullptr;
            }
        }

	}
    p_tdc_time_selected.clear();

	for(vector<TH1F*> &v : p_adc_time) {

		for(TH1F *h : v) {
            if(h) {
                delete h;
                h = nullptr;
            }
        }

	}
    p_adc_time.clear();


	for(TH1F *h : p_tdc_tdc_time_original) { 
        if(h) {
            delete h;
            h = nullptr;
        }
    }
    p_tdc_tdc_time_original.clear();

	for(TH1F *h : p_tdc_tdc_time_corrected) { 
        if(h) {
            delete h;
            h = nullptr;
        }
    }
    p_tdc_tdc_time_corrected.clear();

	for(TH1F *h : p_tdc_tdc_time_selected) { 
        if(h) {
            delete h;
            h = nullptr;
        }
    }
    p_tdc_tdc_time_selected.clear();

	for(TH1F *h : p_tdc_adc_time) { 
        if(h) {
            delete h;
            h = nullptr;
        }
    }
    p_tdc_adc_time.clear();

	for(TH1F *h : p_tdc_channel) { 
        if(h) {
            delete h;
            h = nullptr;
        }
    }
    p_tdc_channel.clear();

	for(TH2F *h : p_adc_vs_tdc) { 
        if(h) {
            delete h;
            h = nullptr;
        }
    }
    p_adc_vs_tdc.clear();

    p_tdc_hit_rate.clear();

	for(TGraph *graph : p_tdc_hit_rate_graph) {

        if(graph) {
            delete graph;
            graph = nullptr;
        }

	}
    p_tdc_hit_rate_graph.clear();

    nHits.clear();
    nTotal.clear();

    if(hitByLC) {
        delete hitByLC;
        hitByLC = nullptr;
    }

    if(badHitByLC) {
        delete badHitByLC;
        badHitByLC = nullptr;
    }

    if(goodHitByLC) {
        delete goodHitByLC;
        goodHitByLC = nullptr;
    }

    if(tube_efficiency) {
        delete tube_efficiency;
        tube_efficiency = nullptr;
    }

    if(residuals) {
        delete residuals;
        residuals = nullptr;
    }

    eventDisplayBuffer.clear();

}