/**
 * @file RadioPanel.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */


#pragma once

#include <string>
#include <map>

using namespace std;

// Reusable panel for submit and cancel buttons
class SubmissionPanel : public TGButtonGroup {

    // FIXME: This spawns warnings. I shouldn't need to do it, since I'm
    //        inheriting from a descendant of TQObject. Figure out why I 
    //        need this for things to work.
    RQ_OBJECT("SubmissionPanel")

public:

    SubmissionPanel(const TGWindow *frame, const string &title = "");

    virtual ~SubmissionPanel() override;

    void Cancel(); // *SIGNAL*
    void Submit(); // *SIGNAL*

    // TODO: Better -- extend TGWidget and generate events on cancel and
    //       submit that we can call with 'Connect'

};

SubmissionPanel::SubmissionPanel(
    const TGWindow *frame, 
    const string &title = ""
) : TGButtonGroup(frame, title, kHorizontalFrame) {

    TGTextButton *cancelButton = new TGTextButton(this, "Cancel");
    TGTextButton *submitButton = new TGTextButton(this, "Submit");

    cancelButton->Connect("Clicked()", "SubmissionPanel", this, "Cancel()");
    submitButton->Connect("Clicked()", "SubmissionPanel", this, "Submit()");

}

SubmissionPanel::~SubmissionPanel() {

    TGCompositeFrame::~TGCompositeFrame();

    // TODO: Cleanup

}

void SubmissionPanel::Cancel() {

    Emit("Cancel()");

}


void SubmissionPanel::Submit() {

    Emit("Submit()");

}

// Reusable radio button panel
class RadioPanel : public TGGroupFrame {

public:

    RadioPanel(
        const TGWindow *frame, 
        const string &title = "", 
        int config = kChildFrame | kVerticalFrame,
        TGLayoutHints *layout = new TGLayoutHints(kLHintsCenterX, 0, 0, 0, 0)
    );
    virtual ~RadioPanel() override;

    void addOption(const string &option);
    void setDefaultOption(const string &option);

    void Select();

private:

    TGGroupFrame *container;

    TGButtonGroup *buttonGroup;

    map<string, TGRadioButton*> buttons;

};

RadioPanel::RadioPanel(
    const TGWindow *frame, 
    const string &title = "", 
    int config = kChildFrame | kVerticalFrame,
    TGLayoutHints *layout = new TGLayoutHints(kLHintsCenterX, 0, 0, 0, 0)
) : TGGroupFrame(frame, title.data(), kVerticalFrame) {

    buttonGroup = new TGButtonGroup(this, "", config);
    AddFrame(buttonGroup, layout);

    SubmissionPanel *submit = new SubmissionPanel(this);
    AddFrame(submit, new TGLayoutHints(kLHintsBottom, 0, 0, 0, 0));

    submit->Connect("Submit()", "RadioPanel", this, "Select()");

}

RadioPanel::~RadioPanel() {

    // TODO: Cleanup

}

// TODO: Return the TGRadioButton object?
void RadioPanel::addOption(const string &option) {

    if(buttons.find(option) != buttons.end()) {

        throw UIException(
            "RadioPanel::addOption -- Option added multiple times."
        );

    }

    buttons.insert({option, new TGRadioButton(buttonGroup, option.data())});

}

void RadioPanel::setDefaultOption(const string &option) {

    auto button = buttons.find(option);
    if(button == buttons.end()) {

        throw UIException(
            "RadioPanel::SetDefaultOption -- This option does not exist."
        );

    }

    // TODO: Since we're iterating anyway, maybe we want to use a vector
    //       instead of a map
    for(auto iter = buttons.begin(); iter != buttons.end(); ++iter) {

        iter->second->SetState(kButtonUp);

    }

    button->second->SetState(kButtonDown);

}