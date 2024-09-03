/**
 * @file DAQManager.h
 *
 * @brief Top-level menu view for the DAQ manager program.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <string>

#include "TG3DLine.h"
#include "TGWindow.h"
#include "TGTab.h"
#include "TGButtonGroup.h"
#include "TGButton.h"
#include "TGLabel.h"

#include "RunNumberPanel.h"
#include "DataSourcePanel.h"
#include "ConfFileSelector.h"
#include "UpdatePacket.h"

#include "RunView.h"
#include "Menu.h"

#include "../Tabs/TabPanel.h"

class DAQManager : public TGMainFrame {

ClassDef(DAQManager, 0);

public:

    DAQManager(
        const TGWindow *p = nullptr, 
        int w = 1, 
        int h = 1, 
        int options = 0
    );

    virtual ~DAQManager();

    // SIGNALS

    void pressedStart(); // *SIGNAL*
    void pressedStop (); // *SIGNAL*
    void pressedExit (); // *SIGNAL*

    void selectedFileSource  (); // *SIGNAL*
    void selectedDeviceSource(); // *SIGNAL*

    void selectedDevice  (const char *selection); // *SIGNAL*
    void selectedFile    (const char *selection); // *SIGNAL*
    void selectedConfFile(const char *selection); // *SIGNAL*

    // METHODS

    void disable();

    void disableStartButton     ();
    void enableStartButton      (); 

    void disableStopButton      ();
    void enableStopButton       ();

    void disableExitButton      ();
    void enableExitButton       ();

    void disableConfFileSelector();
    void enableConfFileSelector ();

    void disableDataSourcePanel ();
    void enableDataSourcePanel  ();

	void disableRunNumberPanel  ();
	void enableRunNumberPanel   ();

    void handlePressStart       ();
    void handlePressStop        ();
    void handlePressExit        ();

    void setConfFileSelectorEntry(const std::string &entry               );
    void setDeviceSelectorOptions(const std::vector<std::string> &entries);
    void setDeviceSelectorEntry  (const std::string &entry               );
    void setFileSelectorEntry    (const std::string &entry               );

    void setFileDataSource();
    void setDeviceDataSource();

    void update(const UpdatePacket &packet);

private:

    // VIEW
    Menu *menuBar;

    TGHorizontal3DLine *menuLine;

    TGHorizontalFrame *mainPanel;

        TGVerticalFrame *leftPanel;

            RunView *viewer;

            TGGroupFrame *settings;

				RunNumberPanel   *runNumberPanel;
                ConfFileSelector *confFileSelector;
                DataSourcePanel  *dataSourcePanel;

        // TGVSplitter *splitter;

        TabPanel *viewport;

            TGCompositeFrame *baseTab;

                TGLabel *baseLabel;

    TGHorizontalFrame *bottomPanel;

        TGLabel *statusTag;

        TGButtonGroup *buttonGroup;

            TGTextButton *startButton  ;
            TGTextButton *stopButton   ;
            TGTextButton *exitButton   ;

    // CONNECTIONS

    void makeConnections();

};