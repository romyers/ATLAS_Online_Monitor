#include "HistogramPlotter.h"

#include "TPaveLabel.h"

#include "DAQMonitor/DataModel/DAQData.h"

#include "GUI/Core/UILock.h"

using namespace std;

HistogramPlotter::HistogramPlotter(
	const TGWindow *p, 
	vector<TH1F*> &histograms, 
	const string &title = "", 
	int w = 1, 
	int h = 1
) : PlotWindow(p, histograms.size(), title, w, h), 
    histograms(histograms) {

	update();

}

HistogramPlotter::~HistogramPlotter() {

}

void HistogramPlotter::update() {

    canvas->GetCanvas()->Clear();

    DAQData &data = DAQData::getInstance();

    data.lock();

    relayout(histograms.size());

    if(histograms.size() == 0) {

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

	for(int i = 0; i < histograms.size(); ++i) {

		canvas->GetCanvas()->cd(i + 1);

		// TODO: Try to break this dependence on DAQData. It's messy as heck,
		//       and makes it weird to use the HistogramPlotter for anything 
		//       that's not in DAQData

		histograms[i]->Draw();

	}

	canvas->GetCanvas()->Update();

    data.unlock();

}