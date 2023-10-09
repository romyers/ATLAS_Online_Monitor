/**
 * @file EntryMenu.cpp
 *
 * @brief Top-level menu for the DAQ manager program.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "macros/UIWindows/EntryWindow/Components/DataSourcePanel.cpp"

#include "src/ProgramControl/Terminator.cpp"

using namespace std;
using namespace Muon;

class EntryMenu : public TGCompositeFrame {

public:

    EntryMenu(
        const TGWindow *p = nullptr, 
        int w = 1, 
        int h = 1, 
        int options = 0, 
        Pixel_t back = GetDefaultFrameBackground()
    );

    virtual ~EntryMenu() override;

private:

    // VIEW

    TGGroupFrame *settings;

        DataSourcePanel *dataSourcePanel;

    TGButtonGroup *buttonGroup;

        TGTextButton *exitButton ;
        TGTextButton *startButton;

    // CONNECTIONS

    void makeConnections();

};

void EntryMenu::makeConnections() {

    exitButton->Connect("Clicked()", "Terminator", &Terminator::getInstance(), "terminate()");

}

EntryMenu::EntryMenu(
        const TGWindow *p = nullptr, 
        int w = 1, 
        int h = 1, 
        int options = 0, 
        Pixel_t back = GetDefaultFrameBackground()
) : TGCompositeFrame(p, w, h, options, back) {

    // TODO: Make sure all these pointers get cleaned up. I'm not clear on how
    //       ROOT GUI does memory management yet

    ///////////////////////////////////////////////////////////////////////////

    settings = new TGGroupFrame(this, "Settings", kVerticalFrame);
    AddFrame(settings, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));

        dataSourcePanel = new DataSourcePanel(settings);
        settings->AddFrame(dataSourcePanel);


    ///////////////////////////////////////////////////////////////////////////

    buttonGroup = new TGButtonGroup(this, "", kHorizontalFrame);
    AddFrame(buttonGroup, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));

        exitButton  = new TGTextButton(buttonGroup, "Exit"     );
        startButton = new TGTextButton(buttonGroup, "Start Run");

    ChangeOptions(GetOptions() | kFixedSize);

    makeConnections();

}

EntryMenu::~EntryMenu() {

    TGCompositeFrame::~TGCompositeFrame();

    // TODO: Cleanup

}