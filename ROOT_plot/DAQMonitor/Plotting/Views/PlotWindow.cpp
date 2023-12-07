/**
 * @file PlotWindow.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>

#include "macros/UIFramework/UISignals.cpp"

using namespace std;

// TODO: Subclass this and HistogramPlotter from something higher up.

class PlotWindow : public TGMainFrame {

public:

	PlotWindow(
		const TGWindow *p, 
		int canvasPanels = 1,
		const string &title = "", 
		int w = 1, 
		int h = 1,
		int rows = 1
	);

	~PlotWindow();

	virtual void CloseWindow() override;

	virtual void update();

protected:

	// VIEW

	TRootEmbeddedCanvas *canvas;

	// CONNECTIONS

	void makeConnections ();
	void breakConnections();

};

void PlotWindow::CloseWindow() {

	breakConnections();

	TGMainFrame::CloseWindow();

}

PlotWindow::PlotWindow(
	const TGWindow *p, 
	int canvasPanels = 1,
	const string &title = "", 
	int w = 1, 
	int h = 1,
	int rows = 1
) : TGMainFrame(p) {

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