#include "ConfFileSelector.h"

#include <iostream>

using namespace std;

ClassImp(ConfFileSelector);

ConfFileSelector::ConfFileSelector(const TGWindow *p) 
    : TGVerticalFrame(p) {

    selectorPanel = new TGVerticalFrame(this);
    AddFrame(selectorPanel, new TGLayoutHints(kLHintsLeft, 10, 10, 10, 10));

    	label = new TGLabel(selectorPanel, "Configuration File:");
    	selectorPanel->AddFrame(label, new TGLayoutHints(kLHintsLeft));

    	entryField = new TGTextEntry(selectorPanel);
    	selectorPanel->AddFrame(entryField, new TGLayoutHints(kLHintsLeft));

    entryField->Resize(250, 20);

    makeConnections();

}

ConfFileSelector::~ConfFileSelector() {



}

void ConfFileSelector::makeConnections() {

    entryField->Connect(
        "TextChanged(const char*)", 
        "ConfFileSelector", 
        this, 
        "entryChanged(const char*)"
    );

}

void ConfFileSelector::enable() {

    label->Enable();
    entryField->SetEnabled(true);

}

void ConfFileSelector::disable() {

    entryField->SetEnabled(false);
    label->Disable();

}

void ConfFileSelector::setEntry(const string &entry) {

	entryField->SetText(entry.data(), false);

}

void ConfFileSelector::entryChanged(const char *text) {

    Emit("entryChanged(const char*)", text);

}