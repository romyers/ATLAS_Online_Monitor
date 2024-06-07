#include "GraphPlotter.h"

#include "DAQMonitor/DataModel/DAQData.h"

using namespace std;

GraphPlotter::GraphPlotter(
	const TGWindow *p, 
	vector<TGraph*> &graphs, 
	const string &title, 
	int w, 
	int h
) : PlotWindow(p, graphs.size(), title, w, h), 
    graphs(graphs) {

    update();

}

GraphPlotter::~GraphPlotter() {

}

void GraphPlotter::update() {

    DAQData &data = DAQData::getInstance();

    data.lock();

    relayout(graphs.size());

	for(int i = 0; i < graphs.size(); ++i) {

		canvas->GetCanvas()->cd(i + 1);

		graphs[i]->Draw("AB");

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

    data.unlock();

	canvas->GetCanvas()->Update();

}