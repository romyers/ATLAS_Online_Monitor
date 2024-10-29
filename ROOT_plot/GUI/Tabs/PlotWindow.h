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
#include "TGButton.h"
#include "TGLabel.h"
#include "TH1.h"
#include "TGraph.h"

#include "UITab.h"

class PlotList {

public:

	PlotList(std::vector<TH1F*> *histograms);
	PlotList(std::vector<TGraph*> *graphs);

	void Draw(TCanvas *canvas);

	size_t size();

private:

	std::vector<TH1F*>   *histograms;
	std::vector<TGraph*> *graphs;

	bool isHistograms;

};

class PlotWindow : public UITab {

public:

	PlotWindow(
		const TGWindow *p,
		PlotList plotList,
		const std::string &title = "",
		int w = 1,
		int h = 1
	);

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

	virtual void update() override;

	virtual void teardown() override;

protected:

	// VIEW

	TGHorizontalFrame *mainView;

		TRootEmbeddedCanvas *canvas;
	
	TGHorizontalFrame *buttonFrame;

		TGLabel      *pageNum    ;

		TGTextButton *begButton  ;
		TGTextButton *leftButton ;
		TGTextButton *rightButton;
		TGTextButton *endButton  ;

private:

    // DATA

	PlotList plotList;

};