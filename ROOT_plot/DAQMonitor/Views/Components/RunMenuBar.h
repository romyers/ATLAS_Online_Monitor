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

#include "TGTab.h"

#include "TabSelector.h"

#include "DAQMonitor/Plotting/Views/PlotWindow.h"
#include "DAQMonitor/ErrorHandling/Views/ErrorView.h"

// TODO: Get rid of this dependency
#include "src/DataModel/DAQData.h"

struct PlotEntry {

public:

	PlotEntry() {}

	PlotWindow *plot = nullptr;
	int tdc = -1;

	bool isOpen() const { return plot != nullptr; }

	void close() {

		// We need to explicitly teardown the plot in order to make sure the
		// connections are broken and the embedded canvas is destroyed.
		plot->teardown();
		plot = nullptr;

	}

};

class RunMenuBar : public TGMenuBar {

public:

	RunMenuBar(const TGWindow *p);

	void handleFileMenuActivate(int id);
	void handleViewMenuActivate(int id);

	void handleTabSelect(const char *selection);

	// METHODS

	void openADCOverview  ();

	void openTDCOverview  ();

	void openErrorViewer  ();
	void closeErrorViewer ();

	void openNoiseDisplay ();
	void closeNoiseDisplay();

	void openADCChannelPlot (PlotEntry &entry);

	void openTDCChannelPlot (PlotEntry &entry);

	void setTabber(TGTab *tabs);

	void handleCloseTab(int id);

private:

	void closeAllWindows();

	// DATA

	DAQData &data;

	// COMPONENTS

	TGPopupMenu *fileMenu;

	TabSelector *tabSelector;

		TGPopupMenu *adcPlotSelector;
		TGPopupMenu *tdcPlotSelector;

	// SUBVIEWS

	TGTab *tabber;

	// TODO: Pull all these out to somewhere else

	PlotEntry adc_overview;
	PlotEntry tdc_overview;

	std::vector<PlotEntry> adc_chnl_plots;
	std::vector<PlotEntry> tdc_chnl_plots;

	PlotEntry noiseDisplay;

	ErrorView *errorViewer;

	// CONNECTIONS

	void makeConnections();

};