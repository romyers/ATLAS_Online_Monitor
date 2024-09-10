#include "SettingsPanel.h"

using namespace DAQ;

ClassImp(SettingsPanel);

SettingsPanel::SettingsPanel(
	SettingsManager &settings,
	const TGWindow *p,
	const TString &title,
	UInt_t options
) : TGGroupFrame(p, title, options), settings(settings) {

	// Register setting defaults
	settings.setDefault("run_number", "1");
	settings.setDefault("conf_file", "");
	settings.setDefault("source_type", "1");
	settings.setDefault("dat_file", "");

	// Validate the run number -- it needs to be an integer with at most 5
	// digits
	if(
		settings.at("run_number").empty() ||
		settings.at("run_number").size() > 5 ||
		std::find_if(
			settings.at("run_number").begin(), 
			settings.at("run_number").end(), 
			[](char c) { 
				return !std::isdigit(c); 
			}
		) != settings.at("run_number").end()
	) {
		
		settings["run_number"] = "1";

	}

	// TODO: We have to do a little extra work to validate the device setting
	settings.setDefault("device", "");

	// TODO: Validate settings file fields

	runNumberPanel = new TGVerticalFrame(this);
	AddFrame(runNumberPanel, new TGLayoutHints(kLHintsExpandX, 10, 10, 10, 10));

		runNumberEntryLabel = new TGLabel(runNumberPanel, "Run Number");
		runNumberPanel->AddFrame(runNumberEntryLabel, new TGLayoutHints(kLHintsLeft));

		runNumberEntry = new TGNumberEntry(runNumberPanel, 
			stoi(settings.at("run_number")), 
			5, -1,
			TGNumberFormat::kNESInteger,
			TGNumberFormat::kNEANonNegative
		);
		runNumberPanel->AddFrame(runNumberEntry, new TGLayoutHints(kLHintsExpandX));
		runNumberEntry->Connect(
			"ValueSet(Long_t)",
			"DAQ::SettingsPanel",
			this,
			"handleRunNumberChange()"
		);

	confFileSelector = new FileSelector(
		this, 
		"Configuration File:", 
		settings.at("conf_file")
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
			settings.at("dat_file")
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

	// Make sure source_type is an integer
	if(
		settings.at("source_type").empty() ||
		std::find_if(
			settings.at("source_type").begin(), 
			settings.at("source_type").end(), 
			[](char c) { 
				return !std::isdigit(c); 
			}
		) != settings.at("source_type").end()
	) {

		settings["source_type"] = "1";

	}

	// Bound-check sourceType against the number of buttons
	int sourceType = stoi(settings.at("source_type"));
	if(sourceType <= 0 || sourceType > sourceTypePanel->GetCount()) {

		sourceType = 1;
		settings["source_type"] = "1";

	}

	// Set the appropriate button
	sourceTypePanel->SetButton(sourceType);

	// TODO: Probably an unnecessary save
	settings.save();

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

	settings.at("source_type") = std::to_string(button);
	settings.save();

}

void SettingsPanel::handleRunNumberChange() {

	settings.at("run_number") = std::to_string(runNumberEntry->GetIntNumber());
	settings.save();

	// FIXME: Run number is not kept in sync with changes typed into the number
	//        field unless enter is pressed. This absolutely needs to be fixed.

}