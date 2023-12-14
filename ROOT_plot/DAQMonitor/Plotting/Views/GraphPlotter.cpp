/**
 * @file GraphPlotter.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <string>

#include "TGraph.h"

#include "macros/UIFramework/UISignals.cpp"

#include "DAQMonitor/Plotting/Views/PlotWindow.cpp"

#include "src/DataModel/DAQData.cpp"

using namespace std;

// TODO: Subclass this and HistogramPlotter from something higher up.

class GraphPlotter : public PlotWindow {

public:

	GraphPlotter(
		const TGWindow *p, 
		vector<TGraph*> graphs, 
		const string &title = "", 
		int w = 1, 
		int h = 1,
		int rows = 1
	);

	~GraphPlotter();

	virtual void update() override;

private:

	// DATA

	vector<TGraph*> graphs;

};

GraphPlotter::GraphPlotter(
	const TGWindow *p, 
	vector<TGraph*> graphs, 
	const string &title, 
	int w, 
	int h,
	int rows
) : PlotWindow(p, graphs.size(), title, w, h, rows), 
    graphs(graphs) {

    update();

}

GraphPlotter::~GraphPlotter() {

}

void GraphPlotter::update() {

	for(int i = 0; i < graphs.size(); ++i) {

		double tmp_yrange = graphs[i]->GetHistogram()->GetMaximum();
		graphs[i]->GetHistogram()->SetMaximum(tmp_yrange > 0.5 ? tmp_yrange : 1);

		canvas->GetCanvas()->cd(i + 1);

		// TODO: Try to break this dependence on DAQData. It's messy as heck,
		//       and makes it weird to use the GraphPlotter for anything 
		//       that's not in DAQData

		DAQData &data = DAQData::getInstance();

		data.lock();
		graphs[i]->Draw("AB");
		data.unlock();

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

	canvas->GetCanvas()->Update();

}