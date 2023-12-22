#include "DAQManager.h"

using namespace std;
using namespace Muon;

ClassImp(DAQManager);

// NOTE: We MUST addFrame every GUI element to ensure proper cleanup

void DAQManager::makeConnections() {

    exitButton ->Connect("Clicked()", "DAQManager", this, "handlePressExit()");
    startButton->Connect("Clicked()", "DAQManager", this, "handlePressStart()");
    stopButton ->Connect("Clicked()", "DAQManager", this, "handlePressStop()");

    dataSourcePanel->Connect(
        "selectedFileSource()",
        "DAQManager",
        this,
        "selectedFileSource()"
    );

    dataSourcePanel->Connect(
        "selectedDeviceSource()",
        "DAQManager",
        this,
        "selectedDeviceSource()"
    );

    dataSourcePanel->Connect(
        "selectedDevice(const char*)", 
        "DAQManager", 
        this, 
        "selectedDevice(const char*)"
    );

    dataSourcePanel->Connect(
        "selectedFile(const char*)",
        "DAQManager",
        this,
        "selectedFile(const char*)"
    );

}

void DAQManager::selectedFileSource  () { Emit("selectedFileSource()"  ); }
void DAQManager::selectedDeviceSource() { Emit("selectedDeviceSource()"); }

void DAQManager::selectedDevice(const char *selection) {

    Emit("selectedDevice(const char*)", selection);

}

void DAQManager::selectedFile(const char *selection) {

    Emit("selectedFile(const char*)", selection);

}

DAQManager::DAQManager(
        const TGWindow *p, 
        int w, 
        int h, 
        int options
) : TGMainFrame(p, w, h, options), viewer(nullptr) {

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

        statusTag = new TGLabel(bottomPanel, "");
        bottomPanel->AddFrame(statusTag, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 30, 5, 5, 5));

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

void DAQManager::setDeviceSelectorEntry(const string &entry) {

    dataSourcePanel->setDeviceSelectorEntry(entry);

}

void DAQManager::setFileSelectorEntry(const string &entry) {

    dataSourcePanel->setFileSelectorEntry(entry);

}

void DAQManager::setDeviceSelectorOptions(const vector<string> &entries) {

    dataSourcePanel->setDeviceSelectorOptions(entries);

}

void DAQManager::setFileDataSource() {

    dataSourcePanel->setFileSource();

}

void DAQManager::setDeviceDataSource() {

    dataSourcePanel->setDeviceSource();

}

void DAQManager::update(const UpdatePacket &packet) {

    viewer->update(packet);

    statusTag->SetText(packet.statusTag.data());

    statusTag->Resize(statusTag->GetDefaultSize());
    
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