/**
 * @file LogView.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

using namespace std;
using namespace Muon;

class LogView : public TGVerticalFrame {

public:

	LogView(const TGWindow *p);

	virtual ~LogView() override;

	void addEntry(const string &type, const string &msg);

	TRootEmbeddedCanvas *logWindow;
	TPaveText *text;

private:

};

LogView::LogView(const TGWindow *p) : TGVerticalFrame(p) {

	logWindow = new TRootEmbeddedCanvas("c", this, 300, 200);
	AddFrame(logWindow, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

	text = new TPaveText(.05, .1, .95, .8, "NB");
	text->UseCurrentStyle();

}

LogView::~LogView() {

	TGVerticalFrame::~TGVerticalFrame();

}

void LogView::addEntry(const string &type, const string &msg) {

	cout << "yay" << endl;

	logWindow->GetCanvas()->cd();

	text->AddText((type +"\t" + msg).data());
	text->Draw();

	logWindow->GetCanvas()->Update();


}