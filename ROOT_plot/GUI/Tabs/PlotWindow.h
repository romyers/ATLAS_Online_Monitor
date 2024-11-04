/**
 * @file PlotWindow.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "UITab.h"
#include "GUI/Components/PageCanvas.h"

#include "TCanvas.h"
#include "TRootEmbeddedCanvas.h"
#include "TGButton.h"
#include "TGLabel.h"
#include "TH1.h"
#include "TGraph.h"

#include <string>

class PlotWindow : public UITab {

public:

	PlotWindow(
		const TGWindow *p,
		std::vector<TH1F*> *histograms,
		const std::string &title = "",
		int w = 1,
		int h = 1
	);

	PlotWindow(
		const TGWindow *p,
		std::vector<TGraph*> *graphs,
		const std::string &title = "",
		int w = 1,
		int h = 1
	);
	~PlotWindow();

	virtual void update() override;

	virtual size_t plotCount();

	virtual void handleResize(Event_t *event);

protected:

	// VIEW

	PageCanvas *canvas;

	virtual void makeConnections() override;
	virtual void breakConnections() override;

private:

    // DATA

	std::vector<TH1F*> *histograms;
	std::vector<TGraph*> *graphs;

	PlotWindow(
		const TGWindow *p,
		const std::string &title = "",
		int w = 1,
		int h = 1
	);

	int plotsWide;
	int plotsTall;
	int numPages;

};