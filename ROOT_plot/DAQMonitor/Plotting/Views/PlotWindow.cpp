#include "PlotWindow.h"

#include "macros/UIFramework/UISignals.h"

using namespace std;

void PlotWindow::teardown() {

	breakConnections();

	delete canvas;
	canvas = nullptr;

}

/*
void PlotWindow::CloseWindow() {

	breakConnections();

	TGMainFrame::CloseWindow();

}
*/

PlotWindow::PlotWindow(
	const TGWindow *p, 
	int canvasPanels,
	const string &title, 
	int w, 
	int h,
	int rows
) : TGCompositeFrame(p) {

	canvas = new TRootEmbeddedCanvas(title.data(), this, w, h);
	AddFrame(canvas, new TGLayoutHints(kLHintsCenterX));

	int count = canvasPanels;
	int cols = count / rows;
    if(count % rows != 0) ++cols;

    canvas->GetCanvas()->Divide(cols, rows);

	makeConnections();

	SetWindowName(title.data());
	MapSubwindows();
	Resize(GetDefaultSize());
	MapWindow();

}

PlotWindow::~PlotWindow() {

}

void PlotWindow::update() {

}

void PlotWindow::makeConnections() {

	UISignalBus::getInstance().Connect(
		"onUpdate()", 
		"GraphPlotter", 
		this, 
		"update()"
	);

}

void PlotWindow::breakConnections() {

	UISignalBus::getInstance().Disconnect("onUpdate()", this, "update()");

}