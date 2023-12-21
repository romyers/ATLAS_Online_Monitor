/**
 * @file EntryView.h
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

#include "MainMenu/Views/Components/DataSourcePanel.h"

#include "DAQMonitor/Views/RunView.h"
#include "DAQMonitor/Views/Components/RunMenuBar.h"



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