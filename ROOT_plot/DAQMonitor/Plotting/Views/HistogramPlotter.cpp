/**
 * @file HistogramPlotter.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <string>

#include "macros/UIFramework/UISignals.cpp"

#include "DAQMonitor/Plotting/Views/PlotWindow.cpp"

#include "src/DataModel/DAQData.cpp"

using namespace std;

class HistogramPlotter : public PlotWindow {

public:

	HistogramPlotter(
		const TGWindow *p, 
		vector<TH1*> histograms, 
		const string &title = "", 
		int w = 1, 
		int h = 1,
		int rows = 1
	);

	~HistogramPlotter();

	virtual void update() override;

private:

	// DATA

	vector<TH1*> histograms;

};

HistogramPlotter::HistogramPlotter(
	const TGWindow *p, 
	vector<TH1*> histograms, 
	const string &title = "", 
	int w = 1, 
	int h = 1,
	int rows = 1
) : PlotWindow(p, histograms.size(), title, w, h, rows), 
    histograms(histograms) {

	update();

}

HistogramPlotter::~HistogramPlotter() {

}

void HistogramPlotter::update() {

	for(int i = 0; i < histograms.size(); ++i) {

		canvas->GetCanvas()->cd(i + 1);

		// TODO: Try to break this dependence on DAQData. It's messy as heck,
		//       and makes it weird to use the HistogramPlotter for anything 
		//       that's not in DAQData

		DAQData &data = DAQData::getInstance();

		data.lock();
		histograms[i]->Draw();
		data.unlock();

	}

	canvas->GetCanvas()->Update();

}