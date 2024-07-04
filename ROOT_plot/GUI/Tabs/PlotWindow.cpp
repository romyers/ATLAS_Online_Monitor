#include "PlotWindow.h"

#include "GUI/Core/UISignals.h"

using namespace std;

PlotWindow::PlotWindow(
	const TGWindow *p, 
	int canvasPanels,
	const string &title, 
	int w, 
	int h
) : CanvasTab(p, title, w, h), panelCount(canvasPanels) {

    GetCanvas()->DivideSquare(canvasPanels);

	makeConnections();

	SetWindowName(title.data());

}

void PlotWindow::relayout(int canvasPanels) {

    // We only need to redivide the canvas if the number of panels has changed
    // if(panelCount == canvasPanels) return;

    GetCanvas()->Clear();

    GetCanvas()->DivideSquare(canvasPanels);

    panelCount = canvasPanels;

}

PlotWindow::~PlotWindow() {

}