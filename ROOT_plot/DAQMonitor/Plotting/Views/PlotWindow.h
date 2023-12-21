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

class PlotWindow : public TGCompositeFrame {

public:

	PlotWindow(
		const TGWindow *p, 
		int canvasPanels = 1,
		const std::string &title = "", 
		int w = 1, 
		int h = 1,
		int rows = 1
	);

	~PlotWindow();

	void teardown();

	virtual void update();

protected:

	// VIEW

	TRootEmbeddedCanvas *canvas;

	// CONNECTIONS

	void makeConnections ();
	void breakConnections();

};