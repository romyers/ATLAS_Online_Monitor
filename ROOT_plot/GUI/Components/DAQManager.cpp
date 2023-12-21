#include "DAQManager.h"

#include "GUI/Core/UISignals.h"

using namespace std;
using namespace Muon;

ClassImp(DAQManager);

// NOTE: We MUST addFrame every GUI element to ensure proper cleanup

void DAQManager::makeConnections() {

    UISignalBus &bus = UISignalBus::getInstance();
    bus.Connect("onUpdate()", "DAQManager", this, "update()");

    exitButton ->Connect("Clicked()", "DAQManager", this, "handlePressExit()");
    startButton->Connect("Clicked()", "DAQManager", this, "handlePressStart()");
    stopButton ->Connect("Clicked()", "DAQManager", this, "handlePressStop()");

}

DAQManager::DAQManager(
        const TGWindow *p, 
        int w, 
        int h, 
        int options, 
        Pixel_t back
) : TGCompositeFrame(p, w, h, options, back)/*, viewer(nullptr) */{

    menuBar = new RunMenuBar(this);
    AddFrame(menuBar, new TGLayoutHints(kLHintsTop | kLHintsLeft));

    menuLine = new TGHorizontal3DLine(this);
    AddFrame(menuLine, new TGLayoutHints(kLHintsTop | kLHintsExpandX));

    ///////////////////////////////////////////////////////////////////////////

    mainPanel = new TGHorizontalFrame(this);
    AddFrame(mainPanel, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

        leftPanel = new TGVerticalFrame(mainPanel, 430, 300, kRaisedFrame);
        mainPanel->AddFrame(leftPanel, new TGLayoutHints(kLHintsExpandY | kLHintsLeft));
        leftPanel->ChangeOptions(leftPanel->GetOptions() | kFixedWidth);

            viewer = new RunView(leftPanel);
            leftPanel->AddFrame(viewer, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

            settings = new TGGroupFrame(leftPanel, "Settings", kVerticalFrame);
            leftPanel->AddFrame(settings, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

                dataSourcePanel = new DataSourcePanel(settings);
                settings->AddFrame(dataSourcePanel);

        // splitter = new TGVSplitter(mainPanel);
        // mainPanel->AddFrame(splitter, new TGLayoutHints(kLHintsExpandY));

        // splitter->SetFrame(leftPanel, true);

        viewport = new TGTab(mainPanel);
        mainPanel->AddFrame(viewport, new TGLayoutHints(kLHintsExpandY));

            // TODO: Base tab can be expanded into an info panel of some sort -- like a 
            //       readme or like the info screens that pop up sometimes talking about e.g.
            //       program title/version/usage
            baseTab = new TGCompositeFrame(viewport, 1250, 850, kFixedSize);
            viewport->AddTab("Home", baseTab);

                baseLabel = new TGLabel(baseTab, "Open tabs from the view menu");
                baseTab->AddFrame(baseLabel, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY));

        // TODO: This is an ugly solution
        menuBar->setTabber(viewport);


    ///////////////////////////////////////////////////////////////////////////

    bottomPanel = new TGHorizontalFrame(this);
    AddFrame(bottomPanel, new TGLayoutHints(kLHintsExpandX));

        buttonGroup = new TGButtonGroup(bottomPanel, "", kHorizontalFrame);
        bottomPanel->AddFrame(buttonGroup, new TGLayoutHints(kLHintsRight, 5, 50, 5, 5));

            startButton = new TGTextButton(buttonGroup, "Start Run");
            buttonGroup->AddFrame(startButton, new TGLayoutHints(kLHintsRight));

            stopButton = new TGTextButton(buttonGroup, "Stop Run");
            buttonGroup->AddFrame(stopButton, new TGLayoutHints(kLHintsRight));

            exitButton  = new TGTextButton(buttonGroup, "Exit");
            buttonGroup->AddFrame(exitButton, new TGLayoutHints(kLHintsRight));

    makeConnections();

    disableStopButton();

}

DAQManager::~DAQManager() {

}

void DAQManager::update() {

    viewer->update();
    
}

void DAQManager::disableStartButton() { startButton->SetEnabled(false); }
void DAQManager::enableStartButton () { startButton->SetEnabled(true ); }

void DAQManager::disableStopButton () { stopButton->SetEnabled (false); }
void DAQManager::enableStopButton  () { stopButton->SetEnabled (true ); }

// TODO: Use these
void DAQManager::pressedStart() { Emit("pressedStart()"); }
void DAQManager::pressedStop () { Emit("pressedStop()" ); }
void DAQManager::pressedExit () { Emit("pressedExit()" ); }

void DAQManager::handlePressStart() {

    disableStartButton();
    enableStopButton();

    dataSourcePanel->disable();

    pressedStart();

}

void DAQManager::handlePressStop() {

    disableStopButton();
    enableStartButton();

    dataSourcePanel->enable();

    pressedStop();

}

void DAQManager::handlePressExit() {

    pressedExit();

}