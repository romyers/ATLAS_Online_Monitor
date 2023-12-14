/**
 * @file FileSelectorPanel.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGWindow.h"
#include "TGLabel.h"
#include "TGTextEntry.h"

using namespace std;

class FileSelector : public TGVerticalFrame {

public:

    FileSelector(const TGWindow *p);
    virtual ~FileSelector() override;

    // METHODS

    void enable();
    void disable();

    string getFilename();

    void setFilename(const string &name);

private:

    // VIEW

    ///////////////////////////////////////////////////////////////////////////
    TGLabel      *label        ;
    TGTextEntry  *entryField   ;
    ///////////////////////////////////////////////////////////////////////////

};

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