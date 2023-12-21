#include "RunMenuBar.h"

#include <vector>
#include <string>

#include "macros/UIFramework/UIException.h"

#include "DAQMonitor/Plotting/Views/HistogramPlotter.h"
#include "DAQMonitor/Plotting/Views/GraphPlotter.h"

#include "DAQMonitor/PlotSaving/PlotSavingOperations.h"


using namespace std;
using namespace Muon;

// TODO: Pull TDC and ADC plot submenus to other components
// TODO: Break direct dependence on DAQData. Interface with the data model
//       through an operation.
// TODO: Clean this up -- lots of cumbersome code.

const int SAVE_PLOTS_ID      = 1;

int createUniqueID();

void removeTab(TGTab *tabs, const string &name) {

	for(int i = 0; i < tabs->GetNumberOfTabs(); ++i) {

		if(tabs->GetTabTab(i)->GetString() == TString(name.data())) {

			tabs->RemoveTab(i);
			tabs->Layout();

		}

	}

}

RunMenuBar::RunMenuBar(const TGWindow *p) 
	: TGMenuBar(p, 100, 20, kHorizontalFrame), data(DAQData::getInstance()), tabber(nullptr) {

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


	tabSelector = new TabSelector(gClient->GetRoot());
	AddPopup("&View", tabSelector, new TGLayoutHints(kLHintsTop | kLHintsLeft));

		adcPlotSelector = new TGPopupMenu(gClient->GetRoot());
		tabSelector->AddPopup("ADC Plots", adcPlotSelector);

			adcPlotSelector->AddEntry("ADC Overview", createUniqueID());

			adcPlotSelector->AddSeparator();

			for(const PlotEntry &entry : adc_chnl_plots) {

				// TODO: I don't want to have to add "ADC Channels" to everything.
				//       Use a unique identifier other than the entry name.
				adcPlotSelector->AddEntry(
					string("TDC ") + entry.tdc + string(" ADC Channels"), createUniqueID()
				);

			}

		tdcPlotSelector = new TGPopupMenu(gClient->GetRoot());
		tabSelector->AddPopup("TDC Plots", tdcPlotSelector);

			tdcPlotSelector->AddEntry("TDC Overview", createUniqueID());

			tdcPlotSelector->AddSeparator();

			for(const PlotEntry &entry : tdc_chnl_plots) {

				tdcPlotSelector->AddEntry(
					string("TDC ") + entry.tdc + string(" TDC Channels"), createUniqueID()
				);

			}

		tabSelector->AddSeparator();

		tabSelector->AddEntry("Noise Rate", createUniqueID());
		tabSelector->AddEntry("Error Log", createUniqueID());

		tabSelector->AddSeparator();

		tabSelector->AddEntry("Close All", createUniqueID());

	makeConnections();

}

// TODO: This shouldn't go in the run menu bar
void RunMenuBar::handleCloseTab(int id) {

	// TODO: We should select the tab just before or just after the
	//       closed tab after closing

	string plotTitle(tabber->GetTabTab(id)->GetString());

	tabber->RemoveTab(id);

	if(plotTitle == "ADC Overview") {

		adc_overview.close();

	} else if(plotTitle == "TDC Overview") {

		tdc_overview.close();

	} else if(plotTitle == "Noise Rate") {

		noiseDisplay.close();

	} else if(plotTitle == "Error Log") {

		closeErrorViewer();

	} else {

		for(PlotEntry &plot : adc_chnl_plots) {

			if(plotTitle == string("TDC ") + plot.tdc + string(" ADC Channels")) {

				plot.close();
				return;

			}

		}

		for(PlotEntry &plot : tdc_chnl_plots) {

			if(plotTitle == string("TDC ") + plot.tdc + string(" TDC Channels")) {

				plot.close();
				return;

			}

		}

	}

}

// TODO: This is an ugly solution
void RunMenuBar::setTabber(TGTab *tabs) {

	if(tabber) tabber->Disconnect("CloseTab(Int_t)", this, "handleCloseTab(int)");

	tabber = tabs;

	// TODO: It's not clean to put this here.
	//         -- This whole thing needs a redesign quite badly
	tabber->Connect("CloseTab(Int_t)", "RunMenuBar", this, "handleCloseTab(int)");

}

void RunMenuBar::handleFileMenuActivate(int id) {

	switch(id) {

		case SAVE_PLOTS_ID:
			PlotSaving::savePlots();
			break;

	}

}

void RunMenuBar::handleTabSelect(const char *selection) {

	string name(selection);

	if(name == "ADC Overview") {

		if(!adc_overview.isOpen()) openADCOverview();
		tabber->SetTab("ADC Overview");

	} else if(name == "TDC Overview") {

		if(!tdc_overview.isOpen()) openTDCOverview();
		tabber->SetTab("TDC Overview");

	} else if(name == "Noise Rate") {

		if(!noiseDisplay.isOpen()) openNoiseDisplay();
		tabber->SetTab("Noise Rate");

	} else if(name == "Error Log") {

		if(!errorViewer) openErrorViewer();
		tabber->SetTab("Error Log");

	} else if(name == "Close All") {

		closeAllWindows();

	} else {

		for(int i = 0; i < adc_chnl_plots.size(); ++i) {

			string plotTitle = string("TDC ") + to_string(i) + string(" ADC Channels");

			if(name == plotTitle) {

				if(!adc_chnl_plots[i].isOpen()) openADCChannelPlot(adc_chnl_plots[i]);
				tabber->SetTab(plotTitle.data());

				return;

			}

		}

		for(int i = 0; i < tdc_chnl_plots.size(); ++i) {

			string plotTitle = string("TDC ") + to_string(i) + string(" TDC Channels");

			if(name == plotTitle) {

				if(!tdc_chnl_plots[i].isOpen()) openTDCChannelPlot(tdc_chnl_plots[i]);
				tabber->SetTab(plotTitle.data());

				return;

			}

		}

	}

}

void RunMenuBar::makeConnections() {

	fileMenu->Connect("Activated(Int_t)", "RunMenuBar", this, "handleFileMenuActivate(int)");

	tabSelector->getSignalBus().Connect(
		"onSelected(const char*)", 
		"RunMenuBar", 
		this, 
		"handleTabSelect(const char*)"
	);

}

void RunMenuBar::openADCOverview() {

	vector<TH1*> plotList;
	plotList.reserve(data.plots.p_tdc_adc_time.size());

	for(TH1F *plot : data.plots.p_tdc_adc_time) {

		plotList.push_back(plot);

	}

	adc_overview.plot = new HistogramPlotter(
		tabber, 
		plotList, 
		"ADC Plots",
		1250,
		850,
		4
	);
	tabber->AddTab("ADC Overview", adc_overview.plot);
	tabber->MapSubwindows();
	tabber->Layout();

	TGTabElement *tab = tabber->GetTabTab("ADC Overview");

	tab->ShowClose();

}

void RunMenuBar::openTDCOverview() {

	vector<TH1*> plotList;
	plotList.reserve(data.plots.p_tdc_tdc_time_corrected.size());

	for(TH1F *plot : data.plots.p_tdc_tdc_time_corrected) {

		plotList.push_back(plot);

	}

	tdc_overview.plot = new HistogramPlotter(
		tabber, 
		plotList, 
		"TDC Plots",
		1250,
		850,
		4
	);
	tabber->AddTab("TDC Overview", tdc_overview.plot);
	tabber->MapSubwindows();
	tabber->Layout();

	TGTabElement *tab = tabber->GetTabTab("TDC Overview");

	tab->ShowClose();

}

void RunMenuBar::openADCChannelPlot(PlotEntry &entry) {

	vector<TH1*> plotList;
	plotList.reserve(data.plots.p_adc_time[entry.tdc].size());

	for(TH1F *plot : data.plots.p_adc_time[entry.tdc]) {

		plotList.push_back(plot);

	}

	string plotTitle = string("TDC ") + to_string(entry.tdc) + string(" ADC Channels");

	entry.plot = new HistogramPlotter(
		tabber, 
		plotList, 
		plotTitle,
		1250,
		850,
		4
	);
	tabber->AddTab(plotTitle.data(), entry.plot);
	tabber->MapSubwindows();
	tabber->Layout();

	TGTabElement *tab = tabber->GetTabTab(plotTitle.data());

	tab->ShowClose();

}

void RunMenuBar::openTDCChannelPlot(PlotEntry &entry) {

	vector<TH1*> plotList;
	plotList.reserve(data.plots.p_tdc_time_corrected[entry.tdc].size());

	for(TH1F *plot : data.plots.p_tdc_time_corrected[entry.tdc]) {

		plotList.push_back(plot);

	}

	string plotTitle = string("TDC ") + to_string(entry.tdc) + string(" TDC Channels");

	entry.plot = new HistogramPlotter(
		tabber, 
		plotList, 
		plotTitle,
		1250,
		850,
		4
	);
	tabber->AddTab(plotTitle.data(), entry.plot);
	tabber->MapSubwindows();
	tabber->Layout();

	TGTabElement *tab = tabber->GetTabTab(plotTitle.data());

	tab->ShowClose();


}

void RunMenuBar::openErrorViewer() {

    errorViewer = new ErrorView(tabber);
    tabber->AddTab("Error Log", errorViewer);
	tabber->MapSubwindows();
	tabber->Layout();

	TGTabElement *tab = tabber->GetTabTab("Error Log");

	tab->ShowClose();

}

void RunMenuBar::closeErrorViewer() {

	errorViewer = nullptr;

}

void RunMenuBar::openNoiseDisplay() {

	noiseDisplay.plot = new GraphPlotter(
		tabber, 
		data.plots.p_tdc_hit_rate_graph, 
		"Noise Rate Display",
		1250,
		850,
		4
	);
	tabber->AddTab("Noise Rate", noiseDisplay.plot);
	tabber->MapSubwindows();
	tabber->Layout();

	TGTabElement *tab = tabber->GetTabTab("Noise Rate");

	tab->ShowClose();

}

void RunMenuBar::closeAllWindows() {

	if(errorViewer) {
		removeTab(tabber, "Error Log");
		closeErrorViewer();
	}

	if(noiseDisplay.plot) {
		removeTab(tabber, "Noise Rate");
		noiseDisplay.close();
	}
	
	if(adc_overview.plot) {
		removeTab(tabber, "ADC Overview");
		adc_overview.close();
	}

	if(tdc_overview.plot) {
		removeTab(tabber, "TDC Overview");
		tdc_overview.close();
	}

	for(PlotEntry &entry : adc_chnl_plots) {

		if(entry.plot) {

			removeTab(tabber, string("TDC ") + to_string(entry.tdc) + string(" ADC Channels"));
			entry.close();

		}

	}

	for(PlotEntry &entry : tdc_chnl_plots) {

		if(entry.plot) {

			removeTab(tabber, string("TDC ") + to_string(entry.tdc) + string(" TDC Channels"));
			entry.close();

		}

	}

}

int createUniqueID() {

	static int counter = 0;

	++counter;

	return counter;

}