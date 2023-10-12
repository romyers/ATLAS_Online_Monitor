/**
 * @file GUIMainFrame.cpp
 *
 * @brief Extension of TGMainFrame that sets the terminator flag when closed.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "src/ProgramControl/Terminator.cpp"

using namespace std;

class GUIMainFrame : public TGMainFrame {

public:

    using TGMainFrame::TGMainFrame;

    // Overriding this function allows us to control the behavior of the close button.
    virtual void CloseWindow() override {

        TGMainFrame::CloseWindow();

        Terminator::getInstance().terminate();

    }


private:



};