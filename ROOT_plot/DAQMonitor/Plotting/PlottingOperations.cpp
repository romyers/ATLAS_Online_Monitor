/**
 * @file PlottingOperations.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <string>

#include "macros/UIFramework/UISignals.cpp"

#include "src/DataModel/DAQData.cpp"

using namespace std;

// TODO: Plotting really only needs to happen at the end of the decode loop.
//       But still make sure we keep it on the UI thread.

// FIXME: Sometimes we get segfaults with window opens and closes. Make it stable

// TODO: Only update plots when new data comes in

namespace Muon {
namespace Plotting {

	void openADCPlots  (int tdc = -1);
    void closeADCPlots (int tdc = -1);
    void closeADCWindow(int tdc = -1);

    void openTDCPlots  (int tdc = -1);
    void closeTDCPlots (int tdc = -1);
    void closeTDCWindow(int tdc = -1);

    int getNumTDCs();

}
namespace PlottingIMPL {

    struct PlotFrame {

    	TGMainFrame *frame          = nullptr;
    	TRootEmbeddedCanvas *canvas = nullptr;

    	PlotFrame() = default;

    	PlotFrame(TGMainFrame *frame, TRootEmbeddedCanvas *canvas)
    		: frame(frame), canvas(canvas) {}

    	bool isOpen() { return frame != nullptr && canvas != nullptr; }

    };

    class PlotWindows {

	public:

		PlotWindows   (      PlotWindows &other) = delete;
		void operator=(const PlotWindows &other) = delete;

		// OPERATIONS

		void openADCPlots  (int tdc = -1);
		void runADCUpdate  ();
		void closeADCPlots (int tdc = -1);
		void closeADCWindow(int tdc = -1);

		void openTDCPlots  (int tdc = -1);
		void runTDCUpdate  ();
		void closeTDCPlots (int tdc = -1);
		void closeTDCWindow(int tdc = -1);

		static PlotWindows &getInstance();

	private:

		PlotWindows();

		PlotFrame tdc_plots;
		PlotFrame adc_plots;

        vector<PlotFrame> tdc_chnl_plots;
        vector<PlotFrame> adc_chnl_plots;

    };

	PlotFrame plotHistograms(
        vector<TH1*> &histograms, 
        const string &title,
        int width = 800, 
        int height = 800,
        int rows = 3
    );

}
}

void Muon::Plotting::openADCPlots(int tdc = -1) {

	PlottingIMPL::PlotWindows::getInstance().openADCPlots(tdc);

}

void Muon::Plotting::closeADCPlots(int tdc = -1) {

	PlottingIMPL::PlotWindows::getInstance().closeADCPlots(tdc);

}

void Muon::Plotting::closeADCWindow(int tdc = -1) {

	PlottingIMPL::PlotWindows::getInstance().closeADCWindow(tdc);

}

void Muon::Plotting::openTDCPlots(int tdc = -1) {

	PlottingIMPL::PlotWindows::getInstance().openTDCPlots(tdc);

}

void Muon::Plotting::closeTDCPlots(int tdc = -1) {

	PlottingIMPL::PlotWindows::getInstance().closeTDCPlots(tdc);

}

void Muon::Plotting::closeTDCWindow(int tdc = -1) {

	PlottingIMPL::PlotWindows::getInstance().closeTDCWindow(tdc);

}

int Muon::Plotting::getNumTDCs() {

    return DAQData::getInstance().plots.p_tdc_time.size();

}


Muon::PlottingIMPL::PlotFrame Muon::PlottingIMPL::plotHistograms(
    vector<TH1*> &histograms, 
    const string &title,
    int width = 800, 
    int height = 800,
    int rows = 3
) {

    // TODO: Normalize histogram y-axes for more pleasant viewing?
    //         -- Otherwise the y axis keeps growing as the run continues


    TGMainFrame *plotFrame = new TGMainFrame(gClient->GetRoot());
    TRootEmbeddedCanvas *canvas = new TRootEmbeddedCanvas(title.data(), plotFrame, width, height);
    plotFrame->AddFrame(canvas, new TGLayoutHints(kLHintsCenterX));

    plotFrame->SetWindowName(title.data());
    plotFrame->MapSubwindows();
    plotFrame->Resize(plotFrame->GetDefaultSize());
    plotFrame->MapWindow();

    int count = histograms.size();
    int cols = count / rows;
    if(count % rows != 0) ++cols;

    canvas->GetCanvas()->Divide(cols, rows);

    return { plotFrame, canvas };

}

Muon::PlottingIMPL::PlotWindows::PlotWindows() {

    DAQData &data = DAQData::getInstance();

    tdc_chnl_plots.resize(data.plots.p_tdc_time_corrected.size());
    adc_chnl_plots.resize(data.plots.p_adc_time.size());

    UISignalBus::getInstance().Connect(
        "onUpdate()", 
        "PlottingIMPL::PlotWindows", 
        this,
        "runTDCUpdate()"
    );

    UISignalBus::getInstance().Connect(
        "onUpdate()",
        "PlottingIMPL::PlotWindows", 
        this,
        "runADCUpdate()"
    );

}

void Muon::PlottingIMPL::PlotWindows::openADCPlots(int tdc = -1) {

    if(tdc == -1 && adc_plots.isOpen()) return;

    if(tdc != -1 && adc_chnl_plots[tdc].isOpen()) return;

    DAQData &data = DAQData::getInstance();

    vector<TH1*> adc_plot_list;

    if(tdc == -1) {

        for(TH1 *plot : data.plots.p_tdc_adc_time) {
            adc_plot_list.push_back(plot);
        }

        adc_plots = plotHistograms(adc_plot_list, "ADC Plots", 1800, 900);

    } else {

        cout << tdc << endl;

        for(TH1 *plot : data.plots.p_adc_time[tdc]) {
            adc_plot_list.push_back(plot);
        }

        adc_chnl_plots[tdc] = plotHistograms(
            adc_plot_list, 
            string(string("ADC Channel Plots, TDC ") + tdc).data(), 
            1800, 
            900
        );

    }

    /*

    adc_plots.frame->Connect(
    	"CloseWindow()", 
    	"PlottingIMPL::PlotWindows", 
    	this, 
    	"closeADCPlots()"
    );
    */

}

void Muon::PlottingIMPL::PlotWindows::runADCUpdate() {

	DAQData &data = DAQData::getInstance();

    if(adc_plots.isOpen()) {

        for(int i = 0; i < data.plots.p_tdc_adc_time.size(); ++i) {

            adc_plots.canvas->GetCanvas()->cd(i + 1);

            data.lock();
            data.plots.p_tdc_adc_time[i]->Draw();
            data.unlock();

            // TODO: Do we need to call TCanvas::Modified()?

        }

        // TODO: Check this works
        adc_plots.canvas->GetCanvas()->Update();

    }

    for(int tdc = 0; tdc < adc_chnl_plots.size(); ++tdc) {

        if(adc_chnl_plots[tdc].isOpen()) {

            for(int i = 0; i < data.plots.p_adc_time[tdc].size(); ++i) {

                adc_chnl_plots[tdc].canvas->GetCanvas()->cd(i + 1);

                data.lock();
                data.plots.p_adc_time[tdc][i]->Draw();
                data.unlock();

            }

            adc_chnl_plots[tdc].canvas->GetCanvas()->Update();

        }

    }

}

void Muon::PlottingIMPL::PlotWindows::closeADCPlots(int tdc = -1) {

    if(tdc == -1) {

        if(!adc_plots.isOpen()) return;

        // UISignalBus::getInstance().Disconnect("onUpdate()", this, "runADCUpdate()");

        adc_plots = PlotFrame();

    } else {

        if(!adc_chnl_plots[tdc].isOpen()) return;

        adc_chnl_plots[tdc] = PlotFrame();

    }

}

void Muon::PlottingIMPL::PlotWindows::closeADCWindow(int tdc = -1) {

    if(tdc == -1) {

        if(!adc_plots.isOpen()) return;

        TGMainFrame *frame = adc_plots.frame;

        closeADCPlots();

        frame->CloseWindow();

    } else {

        if(!adc_chnl_plots[tdc].isOpen()) return;

        TGMainFrame *frame = adc_chnl_plots[tdc].frame;

        closeADCPlots(tdc);

        frame->CloseWindow();

    }

}

void Muon::PlottingIMPL::PlotWindows::openTDCPlots(int tdc = -1) {

    if(tdc == -1 && tdc_plots.isOpen()) return;

    if(tdc != -1 && tdc_chnl_plots[tdc].isOpen()) return;

    DAQData &data = DAQData::getInstance();

    vector<TH1*> tdc_plot_list;

    if(tdc == -1) {

        for(TH1 *plot : data.plots.p_tdc_tdc_time_corrected) {
            tdc_plot_list.push_back(plot);
        }

        tdc_plots = PlottingIMPL::plotHistograms(tdc_plot_list, "TDC Plots", 1800, 900);

    } else {

        for(TH1 *plot : data.plots.p_tdc_time_corrected[tdc]) {
            tdc_plot_list.push_back(plot);
        }

        tdc_chnl_plots[tdc] = PlottingIMPL::plotHistograms(
            tdc_plot_list,
            string(string("TDC Channel Plots, TDC ") + tdc).data(), 
            1800, 
            900
        );

    }

    /*
    UISignalBus::getInstance().Connect(
    	"onUpdate()", 
    	"PlottingIMPL::PlotWindows", 
    	this,
    	"runTDCUpdate()"
    );

    tdc_plots.frame->Connect(
    	"CloseWindow()", 
    	"PlottingIMPL::PlotWindows", 
    	this, 
    	"closeTDCPlots()"
    );
    */

}

void Muon::PlottingIMPL::PlotWindows::runTDCUpdate() {

	DAQData &data = DAQData::getInstance();

    if(tdc_plots.isOpen()) {

        for(int i = 0; i < data.plots.p_tdc_tdc_time_corrected.size(); ++i) {

            tdc_plots.canvas->GetCanvas()->cd(i + 1);

            data.lock();
            data.plots.p_tdc_tdc_time_corrected[i]->Draw();
            data.unlock();

            // TODO: Do we need to call TCanvas::Modified()?

        }
          
        tdc_plots.canvas->GetCanvas()->Update();

    }

    for(int tdc = 0; tdc < tdc_chnl_plots.size(); ++tdc) {

        if(tdc_chnl_plots[tdc].isOpen()) {

            for(int i = 0; i < data.plots.p_tdc_time_corrected[tdc].size(); ++i) {

                tdc_chnl_plots[tdc].canvas->GetCanvas()->cd(i + 1);

                data.lock();
                data.plots.p_tdc_time_corrected[tdc][i]->Draw();
                data.unlock();

            }

            tdc_chnl_plots[tdc].canvas->GetCanvas()->Update();

        }

    }
	
}

void Muon::PlottingIMPL::PlotWindows::closeTDCPlots(int tdc = -1) {

    if(tdc == -1) {

        if(!tdc_plots.isOpen()) return;

        // UISignalBus::getInstance().Disconnect("onUpdate()", this, "runADCUpdate()");

        tdc_plots = PlotFrame();

    } else {

        if(!tdc_chnl_plots[tdc].isOpen()) return;

        tdc_chnl_plots[tdc] = PlotFrame();

    }

}

void Muon::PlottingIMPL::PlotWindows::closeTDCWindow(int tdc = -1) {

    if(tdc == -1) {

        if(!tdc_plots.isOpen()) return;

        TGMainFrame *frame = tdc_plots.frame;

        closeTDCPlots();

        frame->CloseWindow();

    } else {

        if(!tdc_chnl_plots[tdc].isOpen()) return;

        TGMainFrame *frame = tdc_chnl_plots[tdc].frame;

        closeTDCPlots(tdc);

        frame->CloseWindow();

    }

}

Muon::PlottingIMPL::PlotWindows &Muon::PlottingIMPL::PlotWindows::getInstance() {

	static PlotWindows instance;

	return instance;

}