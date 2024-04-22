#include "PlotWindow.h"

#include "GUI/Core/UISignals.h"

using namespace std;

void PlotWindow::teardown() {

	UITab::teardown();

	delete canvas;
	canvas = nullptr;

}

PlotWindow::PlotWindow(
	const TGWindow *p, 
	int canvasPanels,
	const string &title, 
	int w, 
	int h,
	int rows
) : UITab(p) {

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