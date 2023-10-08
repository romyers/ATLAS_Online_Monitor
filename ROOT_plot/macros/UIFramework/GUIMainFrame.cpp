/**
 * @file GUIMainFrame.cpp
 *
 * @brief Extension of TGMainFrame that closes the entire application when the
 * UI is closed.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

using namespace std;

class GUIMainFrame : public TGMainFrame {

public:

    using TGMainFrame::TGMainFrame;

    // Overriding this function allows us to control the behavior of the close button.
    virtual void CloseWindow() override {

    	// TODO: Emit an event that we'll catch somewhere else to call the Terminator

        TGMainFrame::CloseWindow();

        gApplication->Terminate(0);

    }


private:



};