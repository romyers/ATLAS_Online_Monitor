#include "SettingsPanel.h"

using namespace DAQ;

ClassImp(SettingsPanel);

SettingsPanel::SettingsPanel(
	const TGWindow *p,
	const TString &title,
	UInt_t options
) : TGGroupFrame(p, title, options) {

}

SettingsPanel::~SettingsPanel() {

}