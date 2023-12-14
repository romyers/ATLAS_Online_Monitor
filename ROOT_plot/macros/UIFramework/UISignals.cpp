/**
 * @file UISignals.cpp
 *
 * @brief Defines a signal bus for UI signals.
 * 
 * To define a new signal, add a function to UISignalBus that
 * emits itself. That signal may be connected to other behavior
 * elsewhere by calling UISignalBus::getInstance().Connect(...).
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TQObject.h"

using namespace std;

class UISignalBus {

    RQ_OBJECT("UISignalBus");

public:

    UISignalBus   (      UISignalBus &other) = delete;
    void operator=(const UISignalBus &other) = delete;

    // Called when the decoder finishes processing data
    void onUpdate(); // *SIGNAL*

    // TODO: This is too tightly coupled to data runs to belong in a general
    //       UI element. Fix this.
    void onRunStart(); // *SIGNAL*
    void onRunStop (); // *SIGNAL*

    static UISignalBus &getInstance();

private:

    UISignalBus();

};

UISignalBus::UISignalBus() {}

UISignalBus &UISignalBus::getInstance() {

    static UISignalBus instance;

    return instance;

}

void UISignalBus::onUpdate() {

    Emit("onUpdate()");

}

void UISignalBus::onRunStart() {

    Emit("onRunStart()");

}

void UISignalBus::onRunStop() {

    Emit("onRunStop()");

}