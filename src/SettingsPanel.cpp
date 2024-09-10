#include "SettingsPanel.h"

using namespace DAQ;

ClassImp(SettingsPanel);

SettingsPanel::SettingsPanel(
	const TGWindow *p,
	const TString &title,
	UInt_t options
) : TGGroupFrame(p, title, options) {

	runNumberPanel = new TGVerticalFrame(this);
	AddFrame(runNumberPanel, new TGLayoutHints(kLHintsExpandX, 10, 10, 10, 10));

		runNumberEntryLabel = new TGLabel(runNumberPanel, "Run Number");
		runNumberPanel->AddFrame(runNumberEntryLabel, new TGLayoutHints(kLHintsLeft));

		runNumberEntry = new TGNumberEntry(runNumberPanel, 1, 5, -1,
			TGNumberFormat::kNESInteger,
			TGNumberFormat::kNEANonNegative
		);
		runNumberPanel->AddFrame(runNumberEntry, new TGLayoutHints(kLHintsExpandX));

	confFileSelector = new FileSelector(
		this, 
		"Configuration File:", 
		""
	);
	AddFrame(confFileSelector, new TGLayoutHints(kLHintsExpandX, 10, 10, 10, 10));

	sourceTypePanel = new TGButtonGroup(this, "Data Source");
	AddFrame(sourceTypePanel, new TGLayoutHints(kLHintsExpandX, 10, 10, 10, 10));
	sourceTypePanel->Connect(
		"Clicked(Int_t)",
		"DAQ::SettingsPanel",
		this,
		"handleSourceTypeSelection(Int_t)"
	);

		fileButton = new TGRadioButton(sourceTypePanel, "DAT File");
		sourceTypePanel->AddFrame(fileButton, new TGLayoutHints(kLHintsLeft));

		datFileSelector = new FileSelector(
			sourceTypePanel,
			"Select Data File:",
			""
		);
		sourceTypePanel->AddFrame(
			datFileSelector, 
			new TGLayoutHints(kLHintsExpandX, 10, 10, 10, 10)
		);

		networkButton = new TGRadioButton(sourceTypePanel, "Network Device");
		sourceTypePanel->AddFrame(networkButton, new TGLayoutHints(kLHintsLeft));

		devicePanel = new TGVerticalFrame(sourceTypePanel);
		sourceTypePanel->AddFrame(devicePanel, new TGLayoutHints(kLHintsExpandX, 10, 10, 10, 10));

			deviceLabel = new TGLabel(devicePanel, "Select Device:");
			devicePanel->AddFrame(deviceLabel, new TGLayoutHints(kLHintsLeft));

			deviceDropdown = new TGComboBox(devicePanel);
			devicePanel->AddFrame(deviceDropdown, new TGLayoutHints(kLHintsExpandX));
			deviceDropdown->Resize(150, 20);

	sourceTypePanel->SetButton(1);

}

SettingsPanel::~SettingsPanel() {

}

void SettingsPanel::enable() {

	runNumberEntryLabel->Enable();
	runNumberEntry->SetState(kTRUE);

	confFileSelector->enable();

	sourceTypePanel->SetState(kTRUE);

	if(networkButton->GetState() == kButtonDown) {

		deviceLabel->Enable();
		deviceDropdown->SetEnabled(kTRUE);

	} 
	
	if(fileButton->GetState() == kButtonDown) {

		datFileSelector->enable();

	}

}

void SettingsPanel::disable() {

	runNumberEntryLabel->Disable();
	runNumberEntry->SetState(kFALSE);

	confFileSelector->disable();

	sourceTypePanel->SetState(kFALSE);

	datFileSelector->disable();
	deviceLabel->Disable();
	deviceDropdown->SetEnabled(kFALSE);

}

void SettingsPanel::handleSourceTypeSelection(Int_t button) {

	// TODO: It would be nice to set things up so that when we add a new button
	//       the process of adding a button automatically handles this logic

	if(button == sourceTypePanel->GetId(fileButton)) {

		datFileSelector->enable();
		deviceLabel->Disable();
		deviceDropdown->SetEnabled(kFALSE);

	} else if(button == sourceTypePanel->GetId(networkButton)) {

		datFileSelector->disable();
		deviceLabel->Enable();
		deviceDropdown->SetEnabled(kTRUE);

	} else {

		throw std::logic_error(
			"SettingsPanel found invalid source type"
		);

	}

}