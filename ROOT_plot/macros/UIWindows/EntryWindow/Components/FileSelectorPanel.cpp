
#pragma once

using namespace std;


class FileSelector : public TGHorizontalFrame {

    RQ_OBJECT("FileSelector");

public:

    FileSelector(const TGWindow *p);
    virtual ~FileSelector() override;

    // METHODS

    void enable();
    void disable();

    void setFilename(char *name);

    // SIGNALS
    void Selected(char *name) {}

    // OPERATIONS
    void emitFilename(char *name); // *SIGNAL*

private:

    // VIEW

    TGLabel      *label        ;

    TGTextButton *browserButton;

};

FileSelector::FileSelector(const TGWindow *p) 
    : TGHorizontalFrame(p) {

    label = new TGLabel(this, "Select Data File:");
    AddFrame(label, new TGLayoutHints(kLHintsCenterY, 5, 5, 5, 5));

    browserButton = new TGTextButton(this, "Select File");
    AddFrame(browserButton, new TGLayoutHints(kLHintsCenterY, 5, 5, 5, 5));

}

FileSelector::~FileSelector() {

    TGHorizontalFrame::~TGHorizontalFrame();

}

void FileSelector::emitFilename(char *name) {

    Emit("Selected(char*)", name);

}

void FileSelector::setFilename(char *name) {

    // TODO: Implement

    emitFilename(name);

}

void FileSelector::enable() {

    label->Enable();
    browserButton->SetEnabled(true);


}

void FileSelector::disable() {

    browserButton->SetEnabled(false);
    label->Disable();

}