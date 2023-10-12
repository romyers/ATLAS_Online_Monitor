/**
 * @file EntryView.cpp
 *
 * @brief Top-level menu view for the DAQ manager program.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "macros/UIWindows/EntryWindow/Components/DataSourcePanel.cpp"
#include "macros/UIWindows/EntryWindow/EntryOperations.cpp"

#include "src/ProgramControl/Terminator.cpp"

using namespace std;
using namespace Muon;

// NOTE: We MUST addFrame every GUI element to ensure proper cleanup

class EntryView : public TGCompositeFrame {

public:

    EntryView(
        const TGWindow *p = nullptr, 
        int w = 1, 
        int h = 1, 
        int options = 0, 
        Pixel_t back = GetDefaultFrameBackground()
    );

    virtual ~EntryView() override;

private:

    // VIEW

    TGGroupFrame *settings;

        DataSourcePanel *dataSourcePanel;

    TGButtonGroup *buttonGroup;

        TGTextButton *startButton;
        TGTextButton *exitButton ;

    // CONNECTIONS

    void makeConnections();

};

void EntryView::makeConnections() {

    exitButton ->Connect("Clicked()", "EntryOperations", nullptr, "exitAll()");
    startButton->Connect("Clicked()", "EntryOperations", nullptr, "startRun()");

    // TODO: Disable the settings menu and start button when a run is started

}

EntryView::EntryView(
        const TGWindow *p = nullptr, 
        int w = 1, 
        int h = 1, 
        int options = 0, 
        Pixel_t back = GetDefaultFrameBackground()
) : TGCompositeFrame(p, w, h, options, back) {

    ///////////////////////////////////////////////////////////////////////////

    settings = new TGGroupFrame(this, "Settings", kVerticalFrame);
    AddFrame(settings, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));

        dataSourcePanel = new DataSourcePanel(settings);
        settings->AddFrame(dataSourcePanel);


    ///////////////////////////////////////////////////////////////////////////

    buttonGroup = new TGButtonGroup(this, "", kHorizontalFrame);
    AddFrame(buttonGroup, new TGLayoutHints(kLHintsRight, 5, 50, 5, 5));

        startButton = new TGTextButton(buttonGroup, "Start Run");
        buttonGroup->AddFrame(startButton, new TGLayoutHints(kLHintsRight));

        exitButton  = new TGTextButton(buttonGroup, "Exit"     );
        buttonGroup->AddFrame(exitButton, new TGLayoutHints(kLHintsRight));

    ChangeOptions(GetOptions() | kFixedSize);

    makeConnections();

}

EntryView::~EntryView() {

    TGCompositeFrame::~TGCompositeFrame();

}