/**
 * @file CanvasSelector.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "DAQMonitor/Plotting/PlottingOperations.cpp"

using namespace std;
using namespace Muon;

class CanvasSelector : public TGVerticalFrame {

public:

	CanvasSelector(
		const TGWindow *parent, 
		const string &title = "", 
		UInt_t options = kChildFrame | kVerticalFrame
	);

	virtual ~CanvasSelector() override;

	// OPERATIONS

	void openPlots();

private:

	// VIEW

	TGButtonGroup *plotButtons;

		TGCheckButton *ADC_Plots;
		TGCheckButton *TDC_Plots;

	TGTextButton *openButton;

    // CONNECTIONS

    void makeConnections();

};

void CanvasSelector::makeConnections() {

	openButton->Connect("Clicked()", "CanvasSelector", this, "openPlots()");

}

CanvasSelector::CanvasSelector(
	const TGWindow *parent, 
	const string &title = "", 
	UInt_t options = kChildFrame | kVerticalFrame
) : TGVerticalFrame(parent) {

	plotButtons = new TGButtonGroup(this, title, options);
	AddFrame(plotButtons, new TGLayoutHints(kLHintsCenterX));

		ADC_Plots = new TGCheckButton(plotButtons, "ADC Plots");
		plotButtons->AddFrame(ADC_Plots, new TGLayoutHints(kLHintsTop));

		TDC_Plots = new TGCheckButton(plotButtons, "TDC Plots");
		plotButtons->AddFrame(TDC_Plots, new TGLayoutHints(kLHintsTop));

	openButton = new TGTextButton(this, "Open Plots");
	AddFrame(openButton, new TGLayoutHints(kLHintsRight));

	makeConnections();

}

void CanvasSelector::openPlots() {

	if(ADC_Plots->IsDown()) {

		Plotting::openADCPlots();

	} else {

		Plotting::closeADCWindow();

	}

	if(TDC_Plots->IsDown()) {

		Plotting::openTDCPlots();

	} else {

		Plotting::closeTDCWindow();

	}

}

CanvasSelector::~CanvasSelector() {}