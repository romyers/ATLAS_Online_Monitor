#include "PlotWindow.h"

#include "GUI/Core/UISignals.h"

using namespace std;

void PlotWindow::teardown() {

	delete canvas;
	canvas = nullptr;

}

PlotWindow::PlotWindow(
	const TGWindow *p, 
	int canvasPanels,
	const string &title, 
	int w, 
	int h
) : UITab(p), panelCount(canvasPanels) {

	canvas = new TRootEmbeddedCanvas(title.data(), this, w, h);
	AddFrame(canvas, new TGLayoutHints(kLHintsCenterX));

    canvas->GetCanvas()->DivideSquare(canvasPanels);

	makeConnections();

	SetWindowName(title.data());

}

void PlotWindow::relayout(int canvasPanels) {

    // We only need to redivide the canvas if the number of panels has changed
    // if(panelCount == canvasPanels) return;

    canvas->GetCanvas()->Clear();

    canvas->GetCanvas()->DivideSquare(canvasPanels);

    panelCount = canvasPanels;

}