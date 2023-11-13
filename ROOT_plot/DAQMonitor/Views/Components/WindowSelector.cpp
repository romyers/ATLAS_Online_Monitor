/**
 * @file WindowSelector.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "DAQMonitor/Plotting/PlottingOperations.cpp"
#include "DAQMonitor/ErrorHandling/ErrorOperations.cpp"

using namespace std;
using namespace Muon;

// TODO: Make check buttons uncheck when a plot window is closed

class WindowSelector : public TGVerticalFrame {

public:

	WindowSelector(
		const TGWindow *parent, 
		const string &title = "", 
		UInt_t options = kChildFrame | kVerticalFrame
	);
	~WindowSelector() {}

	// OPERATIONS

	void openWindows ();
	void closeWindows();

private:

	// VIEW

	TGButtonGroup *windowButtons;

		TGCheckButton *ADC_Plots;
		TGCheckButton *TDC_Plots;
		TGCheckButton *noiseDisplay;
		TGCheckButton *errorLog;

	TGButtonGroup *commandButtons;
		TGTextButton *openButton;
		TGTextButton *closeButton;

    // CONNECTIONS

    void makeConnections();

};

void WindowSelector::makeConnections() {

	openButton ->Connect("Clicked()", "WindowSelector", this, "openWindows()");
	closeButton->Connect("Clicked()", "WindowSelector", this, "closeWindows()");

}

WindowSelector::WindowSelector(
	const TGWindow *parent, 
	const string &title = "", 
	UInt_t options = kChildFrame | kVerticalFrame
) : TGVerticalFrame(parent) {

	windowButtons = new TGButtonGroup(this, title, options);
	AddFrame(windowButtons, new TGLayoutHints(kLHintsCenterX));

		ADC_Plots = new TGCheckButton(windowButtons, "ADC Plots");
		windowButtons->AddFrame(ADC_Plots, new TGLayoutHints(kLHintsTop));

		TDC_Plots = new TGCheckButton(windowButtons, "TDC Plots");
		windowButtons->AddFrame(TDC_Plots, new TGLayoutHints(kLHintsTop));

		noiseDisplay = new TGCheckButton(windowButtons, "Noise Rate Display");
		windowButtons->AddFrame(noiseDisplay, new TGLayoutHints(kLHintsTop));

		errorLog = new TGCheckButton(windowButtons, "Error Log");
		windowButtons->AddFrame(errorLog, new TGLayoutHints(kLHintsTop));

	commandButtons = new TGButtonGroup(this, "", kHorizontalFrame);
	AddFrame(commandButtons, new TGLayoutHints(kLHintsCenterX));

		openButton = new TGTextButton(commandButtons, "Open Selected");
		commandButtons->AddFrame(openButton, new TGLayoutHints(kLHintsRight));

		closeButton = new TGTextButton(commandButtons, "Close All");
		commandButtons->AddFrame(closeButton, new TGLayoutHints(kLHintsRight));

	makeConnections();

}

void WindowSelector::openWindows() {

	// TODO: It would be nice if CanvasSelector didn't have to know what plots
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

	if(noiseDisplay->IsDown()) {

		cout << "Noise rate display not yet implemented" << endl;

	} else {



	}

	if(errorLog->IsDown()) {

		ErrorHandling::openErrorViewer();

	} else {

		ErrorHandling::closeErrorViewer();
		
	}

}
	
void WindowSelector::closeWindows() {

	Plotting::closeADCWindow();
	Plotting::closeTDCWindow();
	
	ErrorHandling::closeErrorViewer();

	ADC_Plots   ->SetState(kButtonUp);
	TDC_Plots   ->SetState(kButtonUp);
	noiseDisplay->SetState(kButtonUp);
	errorLog    ->SetState(kButtonUp);

}