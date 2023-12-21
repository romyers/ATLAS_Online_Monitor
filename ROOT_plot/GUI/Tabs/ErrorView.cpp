#include "ErrorView.h"

#include <string>

#include "GUI/Core/UISignals.h"

#include "Logging/ErrorLogger.h"

using namespace std;

// TODO: Make sure scrolling doesn't get reset every time it updates

// TODO: By default, scroll from bottom

// TODO: Only update when there's new data

const string DEFAULT_COLOR = "black"  ;
const string DEBUG_COLOR   = "black"  ;
const string WARNING_COLOR = "orange" ;
const string ERROR_COLOR   = "red"    ;
const string FATAL_COLOR   = "#880808";

string getColor(const ErrorData &error);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ErrorView::ErrorView(const TGWindow *p) : TGCompositeFrame(p) {

	errorPanel = new TGHtml(this, 800, 600);
	AddFrame(errorPanel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

	SetWindowName("Error Log");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
    
	update();

	makeConnections();

}

ErrorView::~ErrorView() {

	breakConnections();

}

void ErrorView::update() {

	// TODO: Allow filtering by error level (use checkbuttons for this)

	// TODO: Clean up error logging system

	// TODO: Expand to be a "ConsoleView" module that displays console and
	//       debug info

	vector<ErrorData> errors = ErrorLogger::getInstance().getErrors();

	string htmlString = "";
	for(const ErrorData &error : errors) {

		htmlString += "<p>";

			htmlString += "<font color=";
			htmlString += getColor(error);
			htmlString += ">";

				htmlString += error.msg;

			htmlString += "</font>";

		htmlString += "</p>";

	}

	char *htmlText = new char[htmlString.size() + 1];

	try {

		strcpy(htmlText, htmlString.data());

		errorPanel->Clear();

		errorPanel->ParseText(htmlText);

		errorPanel->Layout();

	} catch(exception &e) {

		delete[] htmlText;
		htmlText = nullptr;

		throw e;

	}

	delete[] htmlText;
	htmlText = nullptr;

}

void ErrorView::makeConnections() {

	UISignalBus::getInstance().Connect("onUpdate()", "ErrorView", this, "update()");

}

void ErrorView::breakConnections() {

	UISignalBus::getInstance().Disconnect("onUpdate()", this, "update()");

}

string getColor(const ErrorData &error) {

	switch(error.level) {

		case DEBUG:
			return DEBUG_COLOR;
			break;

		case WARNING:
			return WARNING_COLOR;
			break;

		case ERROR:
			return ERROR_COLOR;
			break;

		case FATAL:
			return FATAL_COLOR;
			break;

		default:
			return DEFAULT_COLOR;

	}

}