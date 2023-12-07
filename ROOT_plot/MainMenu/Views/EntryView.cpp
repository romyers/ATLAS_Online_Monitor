/**
 * @file EntryView.cpp
 *
 * @brief Top-level menu view for the DAQ manager program.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "macros/UIFramework/UISignals.cpp"

#include "MainMenu/Views/Components/DataSourcePanel.cpp"
#include "MainMenu/EntryOperations.cpp"

#include "DAQMonitor/Views/RunView.cpp"

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

    // METHODS

    void disableStartButton();
    void enableStartButton ();

    void disableStopButton ();
    void enableStopButton  ();

    void openRunViewer     ();
    void closeRunViewer    ();

private:

    // VIEW

    TGHorizontalFrame *mainPanel;

        TGGroupFrame *settings;

            DataSourcePanel *dataSourcePanel;

        RunView *viewer;

    TGButtonGroup *buttonGroup;

        TGTextButton *startButton  ;
        TGTextButton *stopButton   ;
        TGTextButton *runViewButton;
        TGTextButton *exitButton   ;

    // CONNECTIONS

    void makeConnections();

};

void EntryView::makeConnections() {

    UISignalBus &bus = UISignalBus::getInstance();
    
    bus.Connect("onRunStop()", "EntryView"      , this           , "disableStopButton()");
    bus.Connect("onRunStop()", "EntryView"      , this           , "enableStartButton()");
    bus.Connect("onRunStop()", "DataSourcePanel", dataSourcePanel, "enable()"           );

    bus.Connect("onRunStart()", "EntryView"      , this           , "enableStopButton()"  );
    bus.Connect("onRunStart()", "EntryView"      , this           , "disableStartButton()");
    bus.Connect("onRunStart()", "DataSourcePanel", dataSourcePanel, "disable()"           );

    exitButton->Connect("Clicked()", "EntryOperations", nullptr, "exitAll()");

    startButton->Connect("Clicked()", "EntryOperations", nullptr, "startRun()"     );

    stopButton->Connect("Clicked()", "EntryOperations" , nullptr, "stopRun()");

    runViewButton->Connect("Clicked()", "EntryView", this, "openRunViewer()");

}

EntryView::EntryView(
        const TGWindow *p = nullptr, 
        int w = 1, 
        int h = 1, 
        int options = 0, 
        Pixel_t back = GetDefaultFrameBackground()
) : TGCompositeFrame(p, w, h, options, back), viewer(nullptr) {

    ///////////////////////////////////////////////////////////////////////////

    mainPanel = new TGHorizontalFrame(this);
    AddFrame(mainPanel, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));

        settings = new TGGroupFrame(mainPanel, "Settings", kVerticalFrame);
        mainPanel->AddFrame(settings, new TGLayoutHints(kLHintsCenterY, 5, 5, 5, 5));

            dataSourcePanel = new DataSourcePanel(settings);
            settings->AddFrame(dataSourcePanel);

        viewer = new RunView(mainPanel);
        mainPanel->AddFrame(viewer, new TGLayoutHints(kLHintsCenterY, 5, 5, 5, 5));


    ///////////////////////////////////////////////////////////////////////////

    buttonGroup = new TGButtonGroup(this, "", kHorizontalFrame);
    AddFrame(buttonGroup, new TGLayoutHints(kLHintsRight, 5, 50, 5, 5));

        startButton = new TGTextButton(buttonGroup, "Start Run");
        buttonGroup->AddFrame(startButton, new TGLayoutHints(kLHintsRight));

        stopButton = new TGTextButton(buttonGroup, "Stop Run");
        buttonGroup->AddFrame(stopButton, new TGLayoutHints(kLHintsRight));

        runViewButton = new TGTextButton(buttonGroup, "View Run");
        buttonGroup->AddFrame(runViewButton, new TGLayoutHints(kLHintsRight));

        exitButton  = new TGTextButton(buttonGroup, "Exit");
        buttonGroup->AddFrame(exitButton, new TGLayoutHints(kLHintsRight));

    ChangeOptions(GetOptions() | kFixedSize);

    makeConnections();

    disableStopButton();

}

EntryView::~EntryView() {

}

void EntryView::disableStartButton() { startButton->SetEnabled(false); }
void EntryView::enableStartButton () { startButton->SetEnabled(true ); }

void EntryView::disableStopButton () { stopButton->SetEnabled (false); }
void EntryView::enableStopButton  () { stopButton->SetEnabled (true ); }

void EntryView::openRunViewer() {

    if(!viewer) {

        viewer = new RunView(gClient->GetRoot());
        viewer->Connect(
            "CloseWindow()", 
            "EntryView", 
            this, 
            "closeRunViewer()"
        );

    }

    // A quality-of-life feature that just makes sure the viewer pops up to the
    // top when opened while already open, making sure it's obvious to the 
    // user.
    viewer->RaiseWindow();

}

void EntryView::closeRunViewer() {

    viewer = nullptr;

}