#include "FileSelectorPanel.h"

using namespace std;

ClassImp(FileSelector);

FileSelector::FileSelector(const TGWindow *p) 
    : TGVerticalFrame(p) {

    label = new TGLabel(this, "Select Data File:");
    AddFrame(label, new TGLayoutHints(kLHintsLeft));

    entryField = new TGTextEntry(this);
    AddFrame(entryField, new TGLayoutHints(kLHintsLeft));

    makeConnections();

}

void FileSelector::makeConnections() {

    entryField->Connect(
        "TextChanged(const char*)", 
        "FileSelector", 
        this, 
        "TextChanged(const char*)"
    );

}

string FileSelector::getFilename() {

    return string(entryField->GetText());

}

void FileSelector::setFilename(const string &name) {

    entryField->SetText(name.data(), false);

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

void FileSelector::TextChanged(const char *text) {

    Emit("TextChanged(const char*)", text);

}