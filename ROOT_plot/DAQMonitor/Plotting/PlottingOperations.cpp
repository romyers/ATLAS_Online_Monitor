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

namespace Muon {
namespace Plotting {

	void openADCPlots  ();
    void closeADCPlots ();
    void closeADCWindow();

    void openTDCPlots  ();
    void closeTDCPlots ();
    void closeTDCWindow();

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

		void openADCPlots  ();
		void runADCUpdate  ();
		void closeADCPlots ();
		void closeADCWindow();

		void openTDCPlots  ();
		void runTDCUpdate  ();
		void closeTDCPlots ();
		void closeTDCWindow();

		static PlotWindows &getInstance();

	private:

		PlotWindows();

		PlotFrame tdc_plots;
		PlotFrame adc_plots;

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

void Muon::Plotting::openADCPlots() {

	PlottingIMPL::PlotWindows::getInstance().openADCPlots();

}

void Muon::Plotting::closeADCPlots() {

	PlottingIMPL::PlotWindows::getInstance().closeADCPlots();

}

void Muon::Plotting::closeADCWindow() {

	PlottingIMPL::PlotWindows::getInstance().closeADCWindow();

}

void Muon::Plotting::openTDCPlots() {

	PlottingIMPL::PlotWindows::getInstance().openTDCPlots();

}

void Muon::Plotting::closeTDCPlots() {

	PlottingIMPL::PlotWindows::getInstance().closeTDCPlots();

}

void Muon::Plotting::closeTDCWindow() {

	PlottingIMPL::PlotWindows::getInstance().closeTDCWindow();

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

Muon::PlottingIMPL::PlotWindows::PlotWindows() {}

void Muon::PlottingIMPL::PlotWindows::openADCPlots() {

	if(adc_plots.isOpen()) return;

    DAQData &data = DAQData::getInstance();

    vector<TH1*> adc_plot_list;

    for(TH1 *plot : data.plots.p_tdc_adc_time) {
        adc_plot_list.push_back(plot);
    }

	adc_plots = plotHistograms(adc_plot_list, "ADC Plots", 1800, 900);

    UISignalBus::getInstance().Connect(
    	"onUpdate()", 
    	"PlottingIMPL::PlotWindows", 
    	this,
    	"runADCUpdate()"
    );

    adc_plots.frame->Connect(
    	"CloseWindow()", 
    	"PlottingIMPL::PlotWindows", 
    	this, 
    	"closeADCPlots()"
    );

}

void Muon::PlottingIMPL::PlotWindows::runADCUpdate() {

	DAQData &data = DAQData::getInstance();

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

void Muon::PlottingIMPL::PlotWindows::closeADCPlots() {

	if(!adc_plots.isOpen()) return;

	UISignalBus::getInstance().Disconnect("onUpdate()", this, "runADCUpdate()");

	adc_plots = PlotFrame();

}

void Muon::PlottingIMPL::PlotWindows::closeADCWindow() {

	if(!adc_plots.isOpen()) return;

	TGMainFrame *frame = adc_plots.frame;

	closeADCPlots();

	frame->CloseWindow();

}

void Muon::PlottingIMPL::PlotWindows::openTDCPlots() {

	if(tdc_plots.isOpen()) return;

    DAQData &data = DAQData::getInstance();

    vector<TH1*> tdc_plot_list;

    for(TH1 *plot : data.plots.p_tdc_tdc_time_corrected) {
        tdc_plot_list.push_back(plot);
    }

    tdc_plots = PlottingIMPL::plotHistograms(tdc_plot_list, "TDC Plots", 1800, 900);

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

}

void Muon::PlottingIMPL::PlotWindows::runTDCUpdate() {

	DAQData &data = DAQData::getInstance();

    for(int i = 0; i < data.plots.p_tdc_tdc_time_corrected.size(); ++i) {

        tdc_plots.canvas->GetCanvas()->cd(i + 1);

        data.lock();
        data.plots.p_tdc_tdc_time_corrected[i]->Draw();
        data.unlock();

        // TODO: Do we need to call TCanvas::Modified()?

    }
    
    // TODO: This is new -- check once I have data again    
    tdc_plots.canvas->GetCanvas()->Update();
	
}

void Muon::PlottingIMPL::PlotWindows::closeTDCPlots() {


	if(!tdc_plots.isOpen()) return;

	UISignalBus::getInstance().Disconnect("onUpdate()", this, "runTDCUpdate()");

	tdc_plots = PlotFrame();

}

void Muon::PlottingIMPL::PlotWindows::closeTDCWindow() {

	if(!tdc_plots.isOpen()) return;

	TGMainFrame *frame = tdc_plots.frame;

	closeTDCPlots();

	frame->CloseWindow();

}

Muon::PlottingIMPL::PlotWindows &Muon::PlottingIMPL::PlotWindows::getInstance() {

	static PlotWindows instance;

	return instance;

}