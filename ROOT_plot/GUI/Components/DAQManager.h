/**
 * @file DAQManager.h
 *
 * @brief Top-level menu view for the DAQ manager program.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TG3DLine.h"
#include "TGWindow.h"
#include "TGTab.h"
#include "TGButtonGroup.h"
#include "TGButton.h"
#include "TGLabel.h"

#include "DataSourcePanel.h"

#include "RunView.h"
#include "RunMenuBar.h"

class DAQManager : public TGCompositeFrame {

ClassDef(DAQManager, 0);

public:

    DAQManager(
        const TGWindow *p = nullptr, 
        int w = 1, 
        int h = 1, 
        int options = 0, 
        Pixel_t back = GetDefaultFrameBackground()
    );

    virtual ~DAQManager();

    // SIGNALS

    void pressedStart(); // *SIGNAL*
    void pressedStop (); // *SIGNAL*
    void pressedExit (); // *SIGNAL*

    // METHODS

    void disableStartButton();
    void enableStartButton ();

    void disableStopButton ();
    void enableStopButton  ();

    void handlePressStart();
    void handlePressStop();
    void handlePressExit();

    void update();

private:

    // VIEW
    RunMenuBar *menuBar;

    TGHorizontal3DLine *menuLine;

    TGHorizontalFrame *mainPanel;

        TGVerticalFrame *leftPanel;

            RunView *viewer;

            TGGroupFrame *settings;

                DataSourcePanel *dataSourcePanel;

        // TGVSplitter *splitter;

        TGTab *viewport;

            TGCompositeFrame *baseTab;

                TGLabel *baseLabel;

    TGHorizontalFrame *bottomPanel;

        TGButtonGroup *buttonGroup;

            TGTextButton *startButton  ;
            TGTextButton *stopButton   ;
            TGTextButton *exitButton   ;

    // CONNECTIONS

    void makeConnections();

};