#include "FileSelectorPanel.h"

using namespace std;

string FileSelector::getFilename() {

    return string(entryField->GetText());

}

void FileSelector::setFilename(const string &name) {

    entryField->SetText(name.data(), false);

}

FileSelector::FileSelector(const TGWindow *p) 
    : TGVerticalFrame(p) {

    label = new TGLabel(this, "Select Data File:");
    AddFrame(label, new TGLayoutHints(kLHintsLeft));

    entryField = new TGTextEntry(this);
    AddFrame(entryField, new TGLayoutHints(kLHintsLeft));

}

FileSelector::~FileSelector() {

}

void FileSelector::enable() {

    label->Enable();
    entryField->SetEnabled(true);


}

void FileSelector::disable() {

    entryField->SetEnabled(false);
    label->Disable();

}