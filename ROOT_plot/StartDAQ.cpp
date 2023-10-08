/**
 * @file StartDAQ.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <thread>
#include <mutex>

#include "monitorConfig.cpp"

#include "macros/UIFramework/GUIMainFrame.cpp"

#include "src/ProgramControl/Terminator.cpp"
#include "src/ProgramControl/SigHandlers.cpp"
#include "src/EthernetCapture/DeviceManager.cpp"

using namespace std;

// TODO: Look at this for using QT in root:
//       https://root.cern/doc/v606/QtFileDialog_8C.html

// TODO: This needs a better place to live
mutex UILock;

class DeviceSelector : public TGHorizontalFrame {

public:

    DeviceSelector(const TGWindow *p);

    virtual ~DeviceSelector() override;

    void initialize();

    bool ready();

    void enable();
    void disable();

private:

    TGLabel *label;
    TGComboBox *dropdown;

    bool initialized;

};

DeviceSelector::DeviceSelector(const TGWindow *p) 
    : TGHorizontalFrame(p), initialized(false) {

    label = new TGLabel(this, "Select Device:");
    AddFrame(label, new TGLayoutHints(kLHintsCenterY, 5, 5, 5, 5));


    dropdown = new TGComboBox(this);
    AddFrame(dropdown, new TGLayoutHints(kLHintsCenterY, 5, 5, 5, 5));

    dropdown->Resize(150, 20);

}

DeviceSelector::~DeviceSelector() {

    TGHorizontalFrame::~TGHorizontalFrame();

}

void DeviceSelector::initialize() {

    DeviceManager devices;
    devices.initialize();

    vector<PCapDevice> deviceList = devices.getAllDevices();

    for(int i = 0; i < deviceList.size(); ++i) {

        dropdown->AddEntry(deviceList[i].name(), i + 1);

    }

    initialized = true;

}

bool DeviceSelector::ready() {

    return initialized;

}


void DeviceSelector::enable() {

    dropdown->SetEnabled(true);

    label->Enable();

}

void DeviceSelector::disable() {

    dropdown->SetEnabled(false);

    label->Disable();

}

class FileSelector : public TGHorizontalFrame {

public:

    FileSelector(const TGWindow *p);
    virtual ~FileSelector() override;

    void enable();
    void disable();

private:

    TGLabel *label;
    TGTextButton *browserButton;

};

FileSelector::FileSelector(const TGWindow *p) 
    : TGHorizontalFrame(p) {

    label = new TGLabel(this, "Select Data File:");
    AddFrame(label, new TGLayoutHints(kLHintsCenterY, 5, 5, 5, 5));

    browserButton = new TGTextButton(this, "Select File", "new TBrowser(\"Browser\")");
    AddFrame(browserButton, new TGLayoutHints(kLHintsCenterY, 5, 5, 5, 5));

}

FileSelector::~FileSelector() {

    TGHorizontalFrame::~TGHorizontalFrame();

}

void FileSelector::enable() {

    label->Enable();
    browserButton->SetEnabled(true);


}

void FileSelector::disable() {

    browserButton->SetEnabled(false);
    label->Disable();

}

class DataSourcePanel : public TGVerticalFrame {

public:

    DataSourcePanel(const TGWindow *p);
    virtual ~DataSourcePanel() override;

    void showFileSelector();
    void showDeviceSelector();

private:

    TGButtonGroup *menu;

    DeviceSelector *deviceSelector;
    FileSelector *fileSelector;

};

DataSourcePanel::DataSourcePanel(const TGWindow *p) 
    : TGVerticalFrame(p) {

    TGHorizontalFrame *sourceTypePanel = new TGHorizontalFrame(this);
    AddFrame(sourceTypePanel, new TGLayoutHints(kLHintsCenterX, 0, 0, 0, 0));

    TGLabel *dataSourceLabel = new TGLabel(sourceTypePanel, "Data Source:");
    sourceTypePanel->AddFrame(dataSourceLabel, new TGLayoutHints(kLHintsCenterY, 0, 0, 0, 0));

    menu = new TGButtonGroup(sourceTypePanel, "", kHorizontalFrame);
    sourceTypePanel->AddFrame(menu, new TGLayoutHints(kLHintsCenterY, 0, 0, 0, 0));

    TGButton *fileButton = new TGRadioButton(menu, "File");
    TGButton *deviceButton = new TGRadioButton(menu, "Network Device");

    fileButton->Resize(50, 20);
    deviceButton->Resize(50, 20);

    fileButton->Connect("Clicked()", "DataSourcePanel", this, "showFileSelector()");
    deviceButton->Connect("Clicked()", "DataSourcePanel", this, "showDeviceSelector()");

    deviceSelector = new DeviceSelector(this);
    AddFrame(deviceSelector, new TGLayoutHints(kLHintsCenterX, 0, 0, 0, 0));

    fileSelector = new FileSelector(this);
    AddFrame(fileSelector, new TGLayoutHints(kLHintsCenterX, 0, 0, 0, 0));

    // FIXME: fileSelector is not initially hidden.
    deviceButton->SetState(kButtonDown);
    showDeviceSelector();

}

DataSourcePanel::~DataSourcePanel() {

    TGVerticalFrame::~TGVerticalFrame();

}
    
void DataSourcePanel::showFileSelector() {

    HideFrame(deviceSelector);
    ShowFrame(fileSelector);

}

void DataSourcePanel::showDeviceSelector() {


    if(!deviceSelector->ready()) {

        deviceSelector->initialize();

    }
    HideFrame(fileSelector);
    ShowFrame(deviceSelector);

}

class MainMenu : public TGCompositeFrame {

public:

    MainMenu(
        const TGWindow *p = nullptr, 
        int w = 1, 
        int h = 1, 
        int options = 0, 
        Pixel_t back = GetDefaultFrameBackground()
    );

    virtual ~MainMenu() override;

private:

    TGButtonGroup *buttonGroup;

};

MainMenu::MainMenu(
        const TGWindow *p = nullptr, 
        int w = 1, 
        int h = 1, 
        int options = 0, 
        Pixel_t back = GetDefaultFrameBackground()
) : TGCompositeFrame(p, w, h, options, back) {

    // TODO: Make sure all these pointers get cleaned up. I'm not clear on how
    //       ROOT GUI does memory management yet

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    TGGroupFrame *settings = new TGGroupFrame(this, "Settings", kVerticalFrame);
    AddFrame(settings, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));


    ///////////////////////////////////////////////////////////////////////////

    DataSourcePanel *dataSourcePanel = new DataSourcePanel(settings);
    settings->AddFrame(dataSourcePanel);


    ///////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    buttonGroup = new TGButtonGroup(this, "", kHorizontalFrame);
    AddFrame(buttonGroup, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));

    TGTextButton *exitButton = new TGTextButton(buttonGroup, "Exit");
    TGTextButton *startButton = new TGTextButton(buttonGroup, "Start Run");

    exitButton->Connect("Clicked()", "Terminator", &Terminator::getInstance(), "terminate()");

    ChangeOptions(GetOptions() | kFixedSize);

}

MainMenu::~MainMenu() {

    TGCompositeFrame::~TGCompositeFrame();

    // TODO: Cleanup

}

void StartDAQ() {

    // THIS MUST BE CALLED BEFORE STARTING ANY THREADS.
    // It intercepts SIGINT/SIGTERM/SIGQUIT to cleanly terminate threads.
    setTerminationHandlers(flagForTermination);

    // Start the UI update thread
    thread UIThread([](){

        // TODO: Terminate this loop on exit conditions
        while(!Terminator::getInstance().isTerminated()) {

            UILock.lock();
            gSystem->ProcessEvents();
            UILock.unlock();

            this_thread::sleep_for(chrono::milliseconds((int)(1000 / GUI_REFRESH_RATE)));

        }

        // TODO: Teardown UI cleanly without killing all other threads

    });

    // TODO: Open main menu

    UILock.lock();
    GUIMainFrame *mainFrame = new GUIMainFrame(gClient->GetRoot());

    MainMenu *menu = new MainMenu(mainFrame, 500, 500);
    mainFrame->AddFrame(menu, new TGLayoutHints(kLHintsCenterX, 0, 0, 0, 0));

    mainFrame->SetWindowName("DAQ Manager");
    mainFrame->MapSubwindows();
    mainFrame->Resize(mainFrame->GetDefaultSize());
    mainFrame->MapWindow();

    UILock.unlock();

    // Wait for the threads to be done before terminating
    UIThread.join();

    mainFrame->Cleanup();
    delete mainFrame;
    mainFrame = nullptr;

    // This makes sure the GUI closes when the main thread terminates
    gApplication->Terminate(0);

}