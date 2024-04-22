#include "UITab.h"

#include "GUI/Core/UISignals.h"

using namespace std;

UITab::UITab(const TGWindow *p) : TGCompositeFrame(p) {

	makeConnections();

}

void UITab::teardown() {

	breakConnections();

}

void UITab::makeConnections() {

	UISignalBus::getInstance().Connect(
		"onUpdate()",
		"UITab",
		this,
		"update()"
	);

}

void UITab::breakConnections() {

	UISignalBus::getInstance().Disconnect("onUpdate()", this, "update()");

}