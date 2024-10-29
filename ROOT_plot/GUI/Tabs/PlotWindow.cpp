#include "PlotWindow.h"

#include "GUI/Core/UISignals.h"

#include "DAQMonitor/DataModel/DAQData.h"

#include "TPaveLabel.h"

// TODO: DEBUG
#include <iostream>

using namespace std;

PlotList::PlotList(vector<TH1F*> *histograms) 
	: isHistograms(true), histograms(histograms) {}

PlotList::PlotList(vector<TGraph*> *graphs)
	: isHistograms(false), graphs(graphs) {}

void PlotList::Draw(TCanvas *canvas) {

	// Relayout the canvas for the current number of plots
	canvas->DivideSquare(size());

	// Display the no-data text
	canvas->GetCanvas()->cd(1);

	TPaveLabel *label = new TPaveLabel(
		0.2, 0.4, 
		0.8, 0.6, 
		"Waiting for data..."
	);

	label->SetFillColor(16);
	label->SetTextFont(52);

	label->Draw();

	// Draw the histograms or graphs over the no-data text, if there
	// are any to draw.
	if(isHistograms) {

		for(int i = 0; i < size(); ++i) {

			canvas->cd(i + 1);

			(*histograms)[i]->Draw();

		}

	} else {

		for(int i = 0; i < size(); ++i) {

			canvas->cd(i + 1);

			(*graphs)[i]->Draw("AB");

			/*
			TText *xlabel = new TText();
			xlabel->SetNDC();
			xlabel->SetTextFont(42);
			xlabel->SetTextSize(0.05);
			xlabel->SetTextAngle(0);
			string text_content;
			xlabel->DrawText(0.5, 0.9, text_content.c_str());
			*/

		}

	}

};

size_t PlotList::size() {

	return isHistograms ? histograms->size() : graphs->size();

}

void PlotWindow::teardown() {

	delete canvas;
	canvas = nullptr;

}

PlotWindow::PlotWindow(
	const TGWindow *p, 
	PlotList plotList,
	const string &title, 
	int w, 
	int h
) : UITab(p), plotList(plotList) {

	mainView = new TGHorizontalFrame(this);
	AddFrame(mainView, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY | kLHintsTop));

		canvas = new TRootEmbeddedCanvas(title.data(), mainView, w, h);
		mainView->AddFrame(canvas, new TGLayoutHints(kLHintsCenterX | kLHintsExpandX | kLHintsExpandY));

	buttonFrame = new TGHorizontalFrame(this);
	AddFrame(buttonFrame, new TGLayoutHints(kLHintsExpandX | kLHintsBottom));

		pageNum = new TGLabel(buttonFrame, "Page 1/1");
		buttonFrame->AddFrame(pageNum, new TGLayoutHints(kLHintsLeft | kLHintsCenterY));

		endButton = new TGTextButton(buttonFrame, "->|");
		buttonFrame->AddFrame(endButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

		rightButton = new TGTextButton(buttonFrame, "-->");
		buttonFrame->AddFrame(rightButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

		leftButton = new TGTextButton(buttonFrame, "<--");
		buttonFrame->AddFrame(leftButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

		begButton = new TGTextButton(buttonFrame, "|<-");
		buttonFrame->AddFrame(begButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

    canvas->GetCanvas()->DivideSquare(plotList.size());

	makeConnections();

	SetWindowName(title.data());

	update();

}

PlotWindow::PlotWindow(
	const TGWindow *p,
	std::vector<TH1F*> *histograms,
	const std::string &title,
	int w,
	int h
) : PlotWindow(p, PlotList(histograms), title, w, h) {}

PlotWindow::PlotWindow(
	const TGWindow *p,
	vector<TGraph*> *graphs,
	const string &title,
	int w,
	int h
) : PlotWindow(p, PlotList(graphs), title, w, h) {}

void PlotWindow::update() {

	// We need data so we can lock it while we're drawing plots,
	// and prevent the plots from changing while we're drawing them.
    DAQData &data = DAQData::getInstance();

    data.lock();
	canvas->GetCanvas()->Clear();
	plotList.Draw(canvas->GetCanvas());

	canvas->GetCanvas()->Update();
	data.unlock();

}