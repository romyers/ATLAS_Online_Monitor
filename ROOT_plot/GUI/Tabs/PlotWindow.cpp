#include "PlotWindow.h"

#include "GUI/Core/UISignals.h"

#include "DAQMonitor/DataModel/DAQData.h"

#include "TPaveLabel.h"
#include <TMath.h>
#include <iostream>
#include <sstream>
#include <iomanip>   // Required for std::setprecision

using namespace std;

const int PLOT_WIDTH = 300;
const int PLOT_HEIGHT = 200;

void drawSplashScreen(TCanvas *canvas) {

	canvas->cd(1);
	TPaveLabel *label = new TPaveLabel(
		0.2, 0.4, 
		0.8, 0.6, 
		"Waiting for data..."
	);
	label->SetFillColor(16);
	label->SetTextFont(52);
	label->Draw();

}

size_t PlotWindow::plotCount() {

	if(histograms) return histograms->size();
	if(graphs) return graphs->size();

	return 0;

}

PlotWindow::PlotWindow(
	const TGWindow *p, 
	const string &title, 
	int w, 
	int h
) : UITab(p), histograms(nullptr), graphs(nullptr) {

	plotsWide = 0;
	plotsTall = 0;
	numPages = 0;
	
	canvas = new PageCanvas(this, title, w, h);
	AddFrame(canvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

	makeConnections();

	SetWindowName(title.data());

	// Without this, the tab will not detect the right canvas size if the
	// tab is opened after the window is resized.
	Resize(GetDefaultSize());

}

PlotWindow::PlotWindow(
	const TGWindow *p,
	std::vector<TH1F*> *histograms,
	const std::string &title,
	int w,
	int h
) : PlotWindow(p, title, w, h) {

	this->histograms = histograms;
	
	update();

}

PlotWindow::PlotWindow(
	const TGWindow *p,
	vector<TGraph*> *graphs,
	const string &title,
	int w,
	int h
) : PlotWindow(p, title, w, h) {

	this->graphs = graphs;

	update();

}

void PlotWindow::update() {

	// Figure out how many plots we can fit on each page
	int t_plotsWide = std::max((int)canvas->GetWidth() / PLOT_WIDTH, 1);
	int t_plotsTall = std::max((int)canvas->GetHeight() / PLOT_HEIGHT, 1);

	// Figure out how many pages we need
	int t_numPages = ceil((double)plotCount() / (t_plotsWide * t_plotsTall));

	// If the required number of pages has changed, repage the canvas
	if(t_plotsWide != plotsWide || t_plotsTall != plotsTall || t_numPages != numPages) {

		plotsWide = t_plotsWide;
		plotsTall = t_plotsTall;
		numPages = t_numPages;

		canvas->clear();

		for(int i = 0; i < numPages; ++i) {

			canvas->addPage([this, i](TCanvas *c) {

				// We need data so we can lock it while we're drawing plots,
				// and prevent the plots from changing while we're drawing them.
				DAQData &data = DAQData::getInstance();

				data.lock();

				// Clear and relayout the canvas for the required canvas size
				c->Clear();
				c->Divide(plotsWide, plotsTall);

				// Draw the plots over the no-data text, if there are any to
				// draw.
				for(int j = 0; j < plotsWide * plotsTall; ++j) {

					int plotIndex = i * plotsWide * plotsTall + j;

					if(plotIndex >= plotCount()) break;

					c->cd(j + 1);

					if(histograms) {

						(*histograms)[plotIndex]->Draw();

					} else {

						(*graphs)[plotIndex]->Draw("AB");
						
						int nPoints = (*graphs)[plotIndex]->GetN();  // Get number of points

						if (nPoints > 0) {
							TText *xlabel = new TText();
							string text_content;
							xlabel -> SetTextColor(kBlack);
							xlabel -> SetNDC();
							xlabel -> SetTextFont(42);
							xlabel -> SetTextSize(0.05);
							xlabel -> SetTextAngle(0);
						    double* y_values = (*graphs)[plotIndex]->GetY();  // Get pointer to Y-values array

						    double maxY = TMath::MaxElement(nPoints, y_values);  // Get max value
						    double meanY = TMath::Mean(nPoints, y_values);  // Get mean value

						    std::ostringstream oss_max, oss_mean;
						    oss_max << std::fixed << std::setprecision(2) << maxY;  // Format max value
						    oss_mean << std::fixed << std::setprecision(2) << meanY;  // Format mean value

						    text_content = "Max = " + oss_max.str() + " kHz";
						    xlabel->DrawText(0.2, 0.85, text_content.c_str());

						    text_content = "Mean = " + oss_mean.str() + " kHz";
						    xlabel->DrawText(0.6, 0.85, text_content.c_str());
						}

					}

				}

				c->Update();

				data.unlock();

			});

		}

	}

	if(canvas->numPages() == 0) {

		drawSplashScreen(canvas->GetCanvas());

	}

	canvas->update();	

}

void PlotWindow::makeConnections() {

	UITab::makeConnections();

	canvas->Connect("ProcessedEvent(Event_t*)", "PlotWindow", this, "handleResize(Event_t*)");

}

void PlotWindow::breakConnections() {

	UITab::breakConnections();

}

PlotWindow::~PlotWindow() {

	breakConnections();

}

void PlotWindow::handleResize(Event_t *event) {

	// Experimentation shows that kExpose is the type of event that fires
	// when a resize occurs
	if(event->fType == kExpose) {

		update();

	}

}