/**
 * @file EntryView.cpp
 *
 * @brief Top-level menu view for the DAQ manager program.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "MainMenu/Views/Components/DataSourcePanel.cpp"
#include "MainMenu/EntryOperations.cpp"

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

    // OPERATIONS

    void disableStartButton();
    void enableStartButton ();

    void disableStopButton ();
    void enableStopButton  ();

private:

    // VIEW

    TGGroupFrame *settings;

        DataSourcePanel *dataSourcePanel;

    TGButtonGroup *buttonGroup;

        TGTextButton *startButton;
        TGTextButton *stopButton ;
        TGTextButton *exitButton ;

    // CONNECTIONS

    void makeConnections();

};

void EntryView::makeConnections() {

    exitButton ->Connect("Clicked()", "EntryOperations", nullptr        , "exitAll()"           );

    startButton->Connect("Clicked()", "EntryView"      , this           , "enableStopButton()"  );
    startButton->Connect("Clicked()", "EntryView"      , this           , "disableStartButton()");
    startButton->Connect("Clicked()", "EntryOperations", nullptr        , "startRun()"          );
    startButton->Connect("Clicked()", "DataSourcePanel", dataSourcePanel, "disable()"           );

    stopButton->Connect("Clicked()", "EntryView"       , this           , "disableStopButton()" );
    stopButton->Connect("Clicked()", "EntryView"       , this           , "enableStartButton()" );
    stopButton->Connect("Clicked()", "EntryOperations" , nullptr        , "stopRun()"           );
    stopButton->Connect("Clicked()", "DataSourcePanel" , dataSourcePanel, "enable()"            );

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

        stopButton = new TGTextButton(buttonGroup, "Stop Run");
        buttonGroup->AddFrame(stopButton, new TGLayoutHints(kLHintsRight));

        exitButton  = new TGTextButton(buttonGroup, "Exit"     );
        buttonGroup->AddFrame(exitButton, new TGLayoutHints(kLHintsRight));

    ChangeOptions(GetOptions() | kFixedSize);

    makeConnections();

    disableStopButton();

}

EntryView::~EntryView() {

    TGCompositeFrame::~TGCompositeFrame();

}

void EntryView::disableStartButton() { startButton->SetEnabled(false); }
void EntryView::enableStartButton () { startButton->SetEnabled(true ); }

void EntryView::disableStopButton () { stopButton->SetEnabled (false); }
void EntryView::enableStopButton  () { stopButton->SetEnabled (true ); }