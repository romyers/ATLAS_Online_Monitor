#include "UISignals.h"

using namespace std;

UISignalBus::UISignalBus() : TQObject() {}

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