/**
 * @file UISignals.h
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

class UISignalBus : public TQObject {

ClassDef(UISignalBus, 0);

public:

    UISignalBus   (      UISignalBus &other) = delete;
    void operator=(const UISignalBus &other) = delete;

    // Called when the decoder finishes processing data
    void onUpdate  (); // *SIGNAL*
    void onRunStart(); // *SIGNAL*
    void onRunStop (); // *SIGNAL*

    static UISignalBus &getInstance();

private:

    UISignalBus();

};