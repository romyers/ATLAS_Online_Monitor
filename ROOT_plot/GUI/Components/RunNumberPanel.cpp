#include "RunNumberPanel.h"

#include "DAQMonitor/DAQState.h"

using namespace std;

ClassImp(RunNumberPanel);

RunNumberPanel::RunNumberPanel(const TGWindow *p) 
    : TGVerticalFrame(p) {
		

    entryPanel = new TGVerticalFrame(this);
    AddFrame(entryPanel, new TGLayoutHints(kLHintsLeft, 10, 10, 10, 10));

    	label = new TGLabel(entryPanel, "Run Number:");
    	entryPanel->AddFrame(label, new TGLayoutHints(kLHintsLeft));

    	entryField = new TGTextEntry(entryPanel);
    	entryPanel->AddFrame(entryField, new TGLayoutHints(kLHintsLeft));

    entryField->Resize(250, 20);

	setRunNumber(0);

    makeConnections();

}

void RunNumberPanel::makeConnections() {

    entryField->Connect(
        "TextChanged(const char*)", 
        "RunNumberPanel", 
        this, 
        "TextChanged(const char*)"
    );

}

int RunNumberPanel::getRunNumber() {

	if(string(entryField->GetText()).empty()) return 0;

	return stoi(entryField->GetText());

}

void RunNumberPanel::setRunNumber(int runNumber) {

	entryField->SetText(to_string(runNumber).data(), false);

}

RunNumberPanel::~RunNumberPanel() {

}

void RunNumberPanel::enable() {

    label->Enable();
    entryField->SetEnabled(true);


}

void RunNumberPanel::disable() {

    entryField->SetEnabled(false);
    label->Disable();

}

void RunNumberPanel::TextChanged(const char *text) {

	string selection = string(text);

	// In this case, the user is probably in the middle of editing
	// the text, and we don't want to interrupt them. If they go
	// ahead and start the run, we'll just ignore what they did
	// and use the run number we have stored.
	if(selection.empty()) return;

	State::DAQState state = State::DAQState::getState();

	try {

		state.persistentState.runNumber = stoi(selection);

	} catch(std::exception &e) {

		setRunNumber(state.persistentState.runNumber);
		return;

	}

	state.commit();

}

void RunNumberPanel::update() {

	State::DAQState state = State::DAQState::getState();
	
	if(state.persistentState.runNumber == getRunNumber()) return;

	setRunNumber(state.persistentState.runNumber);

}