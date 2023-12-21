#include "UISignals.h"

using namespace std;

ClassImp(UISignalBus);

UISignalBus::UISignalBus() : TQObject() {}

UISignalBus &UISignalBus::getInstance() {

    static UISignalBus instance;

    return instance;

}

void UISignalBus::onUpdate() {

    Emit("onUpdate()");

}