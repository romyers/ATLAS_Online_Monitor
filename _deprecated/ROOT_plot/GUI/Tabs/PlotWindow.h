/**
 * @file PlotWindow.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>

#include "TCanvas.h"
#include "TRootEmbeddedCanvas.h"

#include "UITab.h"

class PlotWindow : public UITab {

public:

	PlotWindow(
		const TGWindow *p, 
		int canvasPanels = 1,
		const std::string &title = "", 
		int w = 1, 
		int h = 1
	);

    virtual void relayout(int canvasPanels);

	virtual void teardown();

protected:

	// VIEW

	TRootEmbeddedCanvas *canvas;

private:

    // DATA

    int panelCount;

};