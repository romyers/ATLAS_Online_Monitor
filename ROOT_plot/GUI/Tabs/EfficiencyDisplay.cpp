#include "EfficiencyDisplay.h"

#include "TCanvas.h"

#include "DAQMonitor/DataModel/DAQData.h"

using namespace std;

EfficiencyDisplay::EfficiencyDisplay(const TGWindow *p, int width, int height) 
	: CanvasTab(p, "Efficiency Display", width, height) {

}

EfficiencyDisplay::~EfficiencyDisplay() {

}

void EfficiencyDisplay::update() {

	DAQData &data = DAQData::getInstance();

	data.lock();
	GetCanvas()->cd();
	if(data.plots.tube_efficiency) data.plots.tube_efficiency->Draw("colz");
	GetCanvas()->Update();
	data.unlock();

}