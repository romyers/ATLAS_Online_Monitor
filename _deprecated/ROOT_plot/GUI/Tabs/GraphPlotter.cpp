#include "GraphPlotter.h"

#include "DAQMonitor/DataModel/DAQData.h"

#include "TPaveLabel.h"

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

    if(graphs.size() == 0) {

        canvas->GetCanvas()->cd(1);

        TPaveLabel *label = new TPaveLabel(
            0.2, 0.4, 
            0.8, 0.6, 
            "Waiting for data..."
        );

        label->SetFillColor(16);
        label->SetTextFont(52);

        label->Draw();

    }

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

    // For some reason we get segfaults if the update happens outside the
    // critical section.
	canvas->GetCanvas()->Update();

    data.unlock();

}