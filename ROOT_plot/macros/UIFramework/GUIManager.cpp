/**
 * @file GUIManager.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <mutex>
#include <vector>

using namespace std;

class GUIElement {

public:

	GUIElement(int panel = 0);

	virtual void Draw() const = 0;

	void setPanel(int index);

	int getPanelIndex() const;

private:

	int panelIndex;

};

GUIElement::GUIElement(int panel) : panelIndex(panel) {}

void GUIElement::setPanel      (int index)        { panelIndex = index; }
int  GUIElement::getPanelIndex (         ) const  { return panelIndex;  }



class Histogram : public GUIElement {

public:

	Histogram();

	TH1 *getPlot();

	virtual void Draw() const;

private:

	TH1 *plot;

};

Histogram::Histogram() : GUIElement() {}

TH1 *Histogram::getPlot() { return plot; }

void Histogram::Draw() const {

	// TODO: We need to draw at the right panel on the right canvas

}



class Canvas {

public:

	Canvas(const char *name, const char *title = "", int form = 1                           );
	Canvas(const char *name, const char *title     , int ww      , int wh                   );
	Canvas(const char *name, const char *title     , int wtopx   , int wtopy, int ww, int wh);

	Canvas(TCanvas *tCanvas);

	void addElement(GUIElement *elt);

	void Draw() const;

private:

	vector<GUIElement*> elements;

	TCanvas *canvas;

};

Canvas::Canvas(
	const char *name, const char *title = "", int form = 1
) {

	canvas = new TCanvas(name, title, form);

}

Canvas::Canvas(
	const char *name, const char *title, int ww, int wh
) {

	canvas = new TCanvas(name, title, ww, wh);

}

Canvas::Canvas(
	const char *name, const char *title, int wtopx, int wtopy, int ww, int wh
) {

	canvas = new TCanvas(name, title, wtopx, wtopy, ww, wh);

}

Canvas::Canvas(TCanvas *tCanvas) : canvas(tCanvas) {}

void Canvas::addElement(GUIElement *elt) {

	elements.push_back(elt);

}

void Canvas::Draw() const {

	for(const GUIElement &elt : elements) {

		elt->Draw();

	}

}




class GUIManager {

public:

	GUIManager    (      GUIManager &other) = delete;
	bool operator=(const GUIManager &other) = delete;

	void refresh();

	Canvas *makeCanvas(const char *name, const char *title = "", int form = 1                           );
	Canvas *makeCanvas(const char *name, const char *title     , int ww      , int wh                   );
	Canvas *makeCanvas(const char *name, const char *title     , int wtopx   , int wtopy, int ww, int wh);

	Canvas *makeCanvas(TCanvas *tCanvas);

	static GUIManager *getInstance();

private:

	GUIManager();

	vector<Canvas> canvases;

	mutex UILock;

	static GUIManager *instance;

};

GUIManager::GUIManager() {}

void GUIManager::refresh() {

	UILock.lock();
	for(const Canvas &canvas : canvases) {

		canvas.Draw();

	}
	gSystem->ProcessEvents();
	UILock.unlock();

}

Canvas *GUIManager::makeCanvas(
	const char *name, const char *title = "", int form = 1
) {

	canvases.emplace_back(name, title, form);

	return &canvases.back();

}

Canvas *GUIManager::makeCanvas(
	const char *name, const char *title, int ww, int wh
) {

	canvases.emplace_back(name, title, ww, wh);

	return &canvases.back();

}

Canvas *GUIManager::makeCanvas(
	const char *name, const char *title, int wtopx, int wtopy, int ww, int wh
) {

	canvases.emplace_back(name, title, wtopx, wtopy, ww, wh);

	return &canvases.back();

}

Canvas *GUIManager::makeCanvas(TCanvas *tCanvas) {

	canvases.emplace_back(tCanvas);

	return &canvases.back();

}

GUIManager *GUIManager::instance = nullptr;
GUIManager *GUIManager::getInstance() {

	if(instance == nullptr) {

		instance = new GUIManager();

	}

	return instance;

}