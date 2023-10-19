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

// TODO: Make check buttons uncheck when a plot window is closed

class CanvasSelector : public TGVerticalFrame {

public:

	CanvasSelector(
		const TGWindow *parent, 
		const string &title = "", 
		UInt_t options = kChildFrame | kVerticalFrame
	);

	virtual ~CanvasSelector() override;

	// OPERATIONS

	void openPlots ();
	void closePlots();

private:

	// VIEW

	TGButtonGroup *plotButtons;

		TGCheckButton *ADC_Plots;
		TGCheckButton *TDC_Plots;

	TGButtonGroup *commandButtons;
		TGTextButton *openButton;
		TGTextButton *closeButton;

    // CONNECTIONS

    void makeConnections();

};

void CanvasSelector::makeConnections() {

	openButton ->Connect("Clicked()", "CanvasSelector", this, "openPlots()");
	closeButton->Connect("Clicked()", "CanvasSelector", this, "closePlots()");

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

	commandButtons = new TGButtonGroup(this, "", kHorizontalFrame);
	AddFrame(commandButtons, new TGLayoutHints(kLHintsCenterX));

		openButton = new TGTextButton(commandButtons, "Open Selected");
		commandButtons->AddFrame(openButton, new TGLayoutHints(kLHintsRight));

		closeButton = new TGTextButton(commandButtons, "Close All");
		commandButtons->AddFrame(closeButton, new TGLayoutHints(kLHintsRight));

	makeConnections();

}

void CanvasSelector::openPlots() {

	// TODO: It would be nice if CanvasSelecter didn't have to know what plots
	//       exist

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
	
void CanvasSelector::closePlots() {

	Plotting::closeADCWindow();
	Plotting::closeTDCWindow();

	ADC_Plots->SetState(kButtonUp);
	TDC_Plots->SetState(kButtonUp);

}

CanvasSelector::~CanvasSelector() {}