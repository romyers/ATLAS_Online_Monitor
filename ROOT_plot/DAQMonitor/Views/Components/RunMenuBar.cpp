/**
 * @file RunMenuBar.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <string>

#include "macros/UIFramework/UIException.cpp"

#include "DAQMonitor/Plotting/Views/HistogramPlotter.cpp"
#include "DAQMonitor/Plotting/Views/GraphPlotter.cpp"
#include "DAQMonitor/Plotting/Views/PlotWindow.cpp"

#include "DAQMonitor/PlotSaving/PlotSavingOperations.cpp"

#include "DAQMonitor/ErrorHandling/Views/ErrorView.cpp"

#include "src/DataModel/DAQData.cpp"

using namespace std;

// TODO: Pull TDC and ADC plot submenus to other components
// TODO: Break direct dependence on DAQData. Interface with the data model
//       through an operation.
// TODO: Clean this up -- lots of cumbersome code.

const int SAVE_PLOTS_ID      = 1;
const int SHOW_ERROR_LOG_ID  = 2;
const int CLOSE_ALL_ID       = 4;
const int PLOT_ENTRY_BASE    = 300000;

const int OVERVIEW_TDC       = -1;

struct PlotEntry {

public:

	PlotEntry() {

		// TODO: Do we care that this doesn't get reset when the run view
		//       is closed?
		static int instanceNum = PLOT_ENTRY_BASE;

		entryID = instanceNum;

		++instanceNum;

	}

	TGPopupMenu *parent = nullptr;

	PlotWindow *plot = nullptr;
	int tdc = OVERVIEW_TDC;

	int getEntryID() const { return entryID; }

	void close() {

		if(parent) {
			parent->UnCheckEntry(entryID);
		}
		plot = nullptr;

	}

private:

	int entryID;

};

class RunMenuBar : public TGMenuBar {

public:

	RunMenuBar(const TGWindow *p);

	void handleFileMenuActivate(int id);
	void handleViewMenuActivate(int id);

	// METHODS

	void openADCOverview  ();

	void openTDCOverview  ();

	void openErrorViewer  ();
	void closeErrorViewer ();

	void openNoiseDisplay ();
	void closeNoiseDisplay();

	void openADCChannelPlot (PlotEntry &entry);

	void openTDCChannelPlot (PlotEntry &entry);

private:

	void closeAllWindows();

	// DATA

	DAQData &data;

	// COMPONENTS

	TGPopupMenu *fileMenu;

	TGPopupMenu *viewMenu;

		TGPopupMenu *adcPlotSelector;
		TGPopupMenu *tdcPlotSelector;

	// SUBVIEWS

	// TODO: Pull all these out to somewhere else

	PlotEntry adc_overview;
	PlotEntry tdc_overview;

	vector<PlotEntry> adc_chnl_plots;
	vector<PlotEntry> tdc_chnl_plots;

	PlotEntry noiseDisplay;

	ErrorView *errorViewer;

	// CONNECTIONS

	void makeConnections();

};

RunMenuBar::RunMenuBar(const TGWindow *p) 
	: TGMenuBar(p, 100, 20, kHorizontalFrame), data(DAQData::getInstance()) {

	adc_chnl_plots.resize(data.plots.p_adc_time.size()          );
	tdc_chnl_plots.resize(data.plots.p_tdc_time_corrected.size());

	for(int i = 0; i < adc_chnl_plots.size(); ++i) {

		adc_chnl_plots[i].tdc = i;
	}

	for(int i = 0; i < tdc_chnl_plots.size(); ++i) {

		tdc_chnl_plots[i].tdc = i;

	}

	errorViewer = nullptr;

	fileMenu = new TGPopupMenu(gClient->GetRoot());
	AddPopup("&File", fileMenu, new TGLayoutHints(kLHintsTop | kLHintsLeft));

		fileMenu->AddEntry("Save Plots", SAVE_PLOTS_ID);


	viewMenu = new TGPopupMenu(gClient->GetRoot());
	AddPopup("&View", viewMenu, new TGLayoutHints(kLHintsTop | kLHintsLeft));

		viewMenu->AddSeparator();

		adcPlotSelector = new TGPopupMenu(gClient->GetRoot());
		viewMenu->AddPopup("ADC Plots", adcPlotSelector);

			adcPlotSelector->AddEntry("ADC Overview", adc_overview.getEntryID());

			adcPlotSelector->AddSeparator();

			for(const PlotEntry &entry : adc_chnl_plots) {

				adcPlotSelector->AddEntry(
					string("TDC ") + entry.tdc, entry.getEntryID()
				);

			}

		tdcPlotSelector = new TGPopupMenu(gClient->GetRoot());
		viewMenu->AddPopup("TDC Plots", tdcPlotSelector);

			tdcPlotSelector->AddEntry("TDC Overview", tdc_overview.getEntryID());

			tdcPlotSelector->AddSeparator();

			for(const PlotEntry &entry : tdc_chnl_plots) {

				tdcPlotSelector->AddEntry(
					string("TDC ") + entry.tdc, entry.getEntryID()
				);

			}

		viewMenu->AddSeparator();

		viewMenu->AddEntry("Noise Rate", noiseDisplay.getEntryID());
		viewMenu->AddEntry("Error Log", SHOW_ERROR_LOG_ID);

		viewMenu->AddSeparator();

		viewMenu->AddEntry("Close All", CLOSE_ALL_ID);

	adc_overview.parent = adcPlotSelector;
	tdc_overview.parent = tdcPlotSelector;

	for(int i = 0; i < adc_chnl_plots.size(); ++i) {

		adc_chnl_plots[i].parent = adcPlotSelector;

	}

	for(int i = 0; i < tdc_chnl_plots.size(); ++i) {

		tdc_chnl_plots[i].parent = tdcPlotSelector;

	}

	noiseDisplay.parent = viewMenu;

	makeConnections();

}

void RunMenuBar::handleFileMenuActivate(int id) {

	switch(id) {

		case SAVE_PLOTS_ID:
			PlotSaving::savePlots();
			break;

	}

}

void RunMenuBar::handleViewMenuActivate(int id) {

	for(PlotEntry &entry : adc_chnl_plots) {

		if(entry.getEntryID() == id) {

			if(adcPlotSelector->IsEntryChecked(entry.getEntryID())) {

				entry.plot->CloseWindow();
				entry.close();

			} else {

				openADCChannelPlot(entry);

			}

			return;

		}

	}

	for(PlotEntry &entry : tdc_chnl_plots) {

		if(entry.getEntryID() == id) {

			if(tdcPlotSelector->IsEntryChecked(entry.getEntryID())) {

				entry.plot->CloseWindow();
				entry.close();

			} else {

				openTDCChannelPlot(entry);

			}

			return;

		}

	}

	if(id == adc_overview.getEntryID()) {

		if(adcPlotSelector->IsEntryChecked(adc_overview.getEntryID())) {

			adc_overview.plot->CloseWindow();
			adc_overview.close();

		} else {

			openADCOverview();

		}

		return;

	}

	if(id == tdc_overview.getEntryID()) {

		if(tdcPlotSelector->IsEntryChecked(tdc_overview.getEntryID())) {

			tdc_overview.plot->CloseWindow();
			tdc_overview.close();

		} else {

			openTDCOverview();

		}

		return;

	}

	if(id == noiseDisplay.getEntryID()) {

		if(viewMenu->IsEntryChecked(noiseDisplay.getEntryID())) {

			noiseDisplay.plot->CloseWindow();
			noiseDisplay.close();

		} else {

			openNoiseDisplay();

		}

		return;

	}

	if(id == SHOW_ERROR_LOG_ID) {

		if(viewMenu->IsEntryChecked(SHOW_ERROR_LOG_ID)) {

			errorViewer->CloseWindow();
			closeErrorViewer();

		} else {

			openErrorViewer();

		}

		return;

	}

	if(id == CLOSE_ALL_ID) {

		closeAllWindows();

		return;

	}

	throw UIException("Menu option not recognized.");

}

void RunMenuBar::makeConnections() {

	fileMenu->Connect("Activated(Int_t)", "RunMenuBar", this, "handleFileMenuActivate(int)");
	viewMenu->Connect("Activated(Int_t)", "RunMenuBar", this, "handleViewMenuActivate(int)");

}

void RunMenuBar::openADCOverview() {

	vector<TH1*> plotList;
	plotList.reserve(data.plots.p_tdc_adc_time.size());

	for(TH1F *plot : data.plots.p_tdc_adc_time) {

		plotList.push_back(plot);

	}

	adc_overview.plot = new HistogramPlotter(
		gClient->GetRoot(), 
		plotList, 
		"ADC Plots",
		1800,
		900,
		3
	);

	adc_overview.plot->Connect("CloseWindow()", "PlotEntry", &adc_overview, "close()");

	adcPlotSelector->CheckEntry(adc_overview.getEntryID());

}

void RunMenuBar::openTDCOverview() {

	vector<TH1*> plotList;
	plotList.reserve(data.plots.p_tdc_tdc_time_corrected.size());

	for(TH1F *plot : data.plots.p_tdc_tdc_time_corrected) {

		plotList.push_back(plot);

	}

	tdc_overview.plot = new HistogramPlotter(
		gClient->GetRoot(), 
		plotList, 
		"TDC Plots",
		1800,
		900,
		3
	);

	tdc_overview.plot->Connect("CloseWindow()", "PlotEntry", &tdc_overview, "close()");

	tdcPlotSelector->CheckEntry(tdc_overview.getEntryID());

}

void RunMenuBar::openADCChannelPlot(PlotEntry &entry) {

	vector<TH1*> plotList;
	plotList.reserve(data.plots.p_adc_time[entry.tdc].size());

	for(TH1F *plot : data.plots.p_adc_time[entry.tdc]) {

		plotList.push_back(plot);

	}

	string plotTitle = "ADC Channel Plots, TDC ";
	plotTitle += to_string(entry.tdc);

	entry.plot = new HistogramPlotter(
		gClient->GetRoot(), 
		plotList, 
		plotTitle,
		1800,
		900,
		3
	);

	entry.plot->Connect("CloseWindow()", "PlotEntry", &entry, "close()");

	adcPlotSelector->CheckEntry(entry.getEntryID());

}

void RunMenuBar::openTDCChannelPlot(PlotEntry &entry) {

	vector<TH1*> plotList;
	plotList.reserve(data.plots.p_tdc_time_corrected[entry.tdc].size());

	for(TH1F *plot : data.plots.p_tdc_time_corrected[entry.tdc]) {

		plotList.push_back(plot);

	}

	string plotTitle = "TDC Channel Plots, TDC ";
	plotTitle += to_string(entry.tdc);

	entry.plot = new HistogramPlotter(
		gClient->GetRoot(), 
		plotList, 
		plotTitle,
		1800,
		900,
		3
	);

	entry.plot->Connect("CloseWindow()", "PlotEntry", &entry, "close()");

	tdcPlotSelector->CheckEntry(entry.getEntryID());


}

void RunMenuBar::openErrorViewer() {

    errorViewer = new ErrorView(gClient->GetRoot());

    errorViewer->Connect("CloseWindow()", "RunMenuBar", this, "closeErrorViewer()");

    viewMenu->CheckEntry(SHOW_ERROR_LOG_ID);

}

void RunMenuBar::closeErrorViewer() {

	viewMenu->UnCheckEntry(SHOW_ERROR_LOG_ID);

	errorViewer = nullptr;

}

void RunMenuBar::openNoiseDisplay() {

	noiseDisplay.plot = new GraphPlotter(
		gClient->GetRoot(), 
		data.plots.p_tdc_hit_rate_graph, 
		"Noise Rate Display",
		1800,
		900,
		3
	);

	noiseDisplay.plot->Connect("CloseWindow()", "PlotEntry", &noiseDisplay, "close()");
	viewMenu->CheckEntry(noiseDisplay.getEntryID());

}

void RunMenuBar::closeAllWindows() {

	if(errorViewer) {
		errorViewer->CloseWindow();
		closeErrorViewer();
	}

	if(noiseDisplay.plot) {
		noiseDisplay.plot->CloseWindow();
		noiseDisplay.close();
	}
	
	if(adc_overview.plot) {
		adc_overview.plot->CloseWindow();
		adc_overview.close();
	}

	if(tdc_overview.plot) {
		tdc_overview.plot->CloseWindow();
		tdc_overview.close();
	}

	for(PlotEntry &entry : adc_chnl_plots) {

		if(entry.plot) {

			entry.plot->CloseWindow();
			entry.close();

		}

	}

	for(PlotEntry &entry : tdc_chnl_plots) {

		if(entry.plot) {

			entry.plot->CloseWindow();
			entry.close();

		}

	}

}