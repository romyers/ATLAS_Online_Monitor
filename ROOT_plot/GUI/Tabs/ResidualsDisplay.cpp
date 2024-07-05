#include "ResidualsDisplay.h"

#include "TCanvas.h"

#include "DAQMonitor/DataModel/DAQData.h"

using namespace std;

ResidualsDisplay::ResidualsDisplay(const TGWindow *p, int width, int height) 
	: CanvasTab(p, "Residuals Display", width, height) {

}

ResidualsDisplay::~ResidualsDisplay() {

}

void ResidualsDisplay::update() {

	DAQData &data = DAQData::getInstance();

	data.lock();
	GetCanvas()->cd();
	if(data.plots.residuals) data.plots.residuals->Draw();
	GetCanvas()->Update();
	data.unlock();
	
}