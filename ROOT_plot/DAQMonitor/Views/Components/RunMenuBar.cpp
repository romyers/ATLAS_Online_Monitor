/**
 * @file RunMenuBar.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "DAQMonitor/Plotting/PlottingOperations.cpp"

#include "DAQMonitor/PlotSaving/PlotSavingOperations.cpp"

#include "DAQMonitor/ErrorHandling/ErrorOperations.cpp"

using namespace std;

const int SAVE_PLOTS_ID      = 1;
const int SHOW_ERROR_LOG_ID  = 2;
const int SHOW_NOISE_RATE_ID = 3;
const int ADC_OVERVIEW_ID    = 4;
const int TDC_OVERVIEW_ID    = 5;
const int OPEN_ALL_ID        = 6;
const int CLOSE_ALL_ID       = 7;
const int ADC_CHANNEL_BASE   = 1000;
const int TDC_CHANNEL_BASE   = 2000;

class RunMenuBar : public TGMenuBar {

public:

	RunMenuBar(const TGWindow *p);

	void handleFileMenuActivate(int id);

	void handleViewMenuActivate(int id);

private:

	void openAllWindows ();
	void closeAllWindows();

	// VIEW

	TGPopupMenu *fileMenu;

	TGPopupMenu *viewMenu;

		TGPopupMenu *adcPlots;
		TGPopupMenu *tdcPlots;

	// CONNECTIONS

	void makeConnections();

};

void RunMenuBar::handleFileMenuActivate(int id) {

	switch(id) {

		case SAVE_PLOTS_ID:
			PlotSaving::savePlots();
			break;

	}

}

void RunMenuBar::handleViewMenuActivate(int id) {

	if(id >= ADC_CHANNEL_BASE && id < TDC_CHANNEL_BASE) {

		if(adcPlots->IsEntryChecked(id)) {

			Plotting::closeADCWindow(id - ADC_CHANNEL_BASE);
			adcPlots->UnCheckEntry(id);

		} else {

			Plotting::openADCPlots(id - ADC_CHANNEL_BASE);
			adcPlots->CheckEntry(id);

		}

		return;

	} else if(id >= TDC_CHANNEL_BASE) {

		if(tdcPlots->IsEntryChecked(id)) {

			Plotting::closeTDCWindow(id - TDC_CHANNEL_BASE);
			tdcPlots->UnCheckEntry(id);

		} else {

			Plotting::openTDCPlots(id - TDC_CHANNEL_BASE);
			tdcPlots->CheckEntry(id);

		}

		return;

	}

	// TODO: Uncheck view menu options when user closes the window with the
	//       close button

	switch(id) {

		case ADC_OVERVIEW_ID:
			if(adcPlots->IsEntryChecked(ADC_OVERVIEW_ID)) {
				Plotting::closeADCWindow();
				adcPlots->UnCheckEntry(ADC_OVERVIEW_ID);
			} else {
				Plotting::openADCPlots();
				adcPlots->CheckEntry(ADC_OVERVIEW_ID);
			}
			break;

		case TDC_OVERVIEW_ID:
			if(tdcPlots->IsEntryChecked(TDC_OVERVIEW_ID)) {
				Plotting::closeTDCWindow();
				tdcPlots->UnCheckEntry(TDC_OVERVIEW_ID);
			} else {
				Plotting::openTDCPlots();
				tdcPlots->CheckEntry(TDC_OVERVIEW_ID);
			}
			break;

		case SHOW_NOISE_RATE_ID:
			cout << "Noise rate display not yet implemented." << endl;
			break;

		case SHOW_ERROR_LOG_ID:

			if(viewMenu->IsEntryChecked(SHOW_ERROR_LOG_ID)) {
				ErrorHandling::closeErrorViewer();
				viewMenu->UnCheckEntry(SHOW_ERROR_LOG_ID);
			} else {
				ErrorHandling::openErrorViewer();
				viewMenu->CheckEntry(SHOW_ERROR_LOG_ID);
			}
			break;

		default:
			cout << "Menu option not recognized" << endl;

	}

}

void RunMenuBar::makeConnections() {

	fileMenu->Connect("Activated(Int_t)", "RunMenuBar", this, "handleFileMenuActivate(int)");

	viewMenu->Connect("Activated(Int_t)", "RunMenuBar", this, "handleViewMenuActivate(int)");

}

RunMenuBar::RunMenuBar(const TGWindow *p) : TGMenuBar(p, 100, 20, kHorizontalFrame) {

	fileMenu = new TGPopupMenu(gClient->GetRoot());
	AddPopup("&File", fileMenu, new TGLayoutHints(kLHintsTop | kLHintsLeft));

		fileMenu->AddEntry("Save Plots", SAVE_PLOTS_ID);


	viewMenu = new TGPopupMenu(gClient->GetRoot());
	AddPopup("&View", viewMenu, new TGLayoutHints(kLHintsTop | kLHintsLeft));

		viewMenu->AddSeparator();

		adcPlots = new TGPopupMenu(gClient->GetRoot());
		viewMenu->AddPopup("ADC Plots", adcPlots);

			adcPlots->AddEntry("ADC Overview", ADC_OVERVIEW_ID);

			adcPlots->AddSeparator();

			for(int i = 0; i < Plotting::getNumTDCs(); ++i) {

				adcPlots->AddEntry(string("TDC ") + i, ADC_CHANNEL_BASE + i);

			}

		tdcPlots = new TGPopupMenu(gClient->GetRoot());
		viewMenu->AddPopup("TDC Plots", tdcPlots);

			tdcPlots->AddEntry("TDC Overview", TDC_OVERVIEW_ID);

			tdcPlots->AddSeparator();

			for(int i = 0; i < Plotting::getNumTDCs(); ++i) {

				tdcPlots->AddEntry(string("TDC ") + i, TDC_CHANNEL_BASE + i);

			}

		viewMenu->AddSeparator();

		viewMenu->AddEntry("Noise Rate", SHOW_NOISE_RATE_ID);

		viewMenu->AddEntry("Error Log", SHOW_ERROR_LOG_ID);

	makeConnections();

}