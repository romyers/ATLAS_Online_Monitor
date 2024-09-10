#include "FileSelector.h"

#include "TSystem.h"
#include "TGFileBrowser.h"
#include "TGListTree.h"

#include <sys/stat.h>

using namespace DAQ;

// TODO: Once we finish prototyping, make this all nice and robust

// TODO: Block GUI interaction while the file browser is open.
// TODO: Allow external code to set the directory to look in so we can make
//       it remember the last directory it was in.
// TODO: Examine this for QT file dialog:
//       https://root.cern/doc/v606/QtFileDialog_8C.html
// TODO: Validate file selections -- they must exist and obey directoriesAllowed

ClassImp(FileSelector);

FileSelector::FileSelector(
	const TGWindow *p,
	const TString &label,
	const TString &defaultEntry
) : TGVerticalFrame(p), fileExplorer(nullptr) {

	labelField = new TGLabel(this, label);
	AddFrame(labelField, new TGLayoutHints(kLHintsLeft));

	inputPanel = new TGHorizontalFrame(this);
	AddFrame(inputPanel, new TGLayoutHints(kLHintsExpandX));

		entryField = new TGTextEntry(inputPanel, defaultEntry);
		inputPanel->AddFrame(entryField, new TGLayoutHints(kLHintsExpandX));

		// This uses an icon from ROOT's icon set
		fileButton = new TGPictureButton(
			inputPanel, 
			gClient->GetPicture("icons/ed_open.png")
		);
		inputPanel->AddFrame(
			fileButton, 
			new TGLayoutHints(kLHintsRight, 0, 0, 0, 0)
		);
		fileButton->Connect(
			"Clicked()", 
			"DAQ::FileSelector", 
			this, 
			"handleFileButton()"
		);

}

FileSelector::~FileSelector() {

}

/*
 * This handler is called when the user clicks the file button. It opens
 * a file explorer window that allows the user to select a file. If the
 * user selects a file, 'handleBrowserSelection' is called. If the user
 * closes the file explorer without selecting a file, nothing happens.
 */
void FileSelector::handleFileButton() {

	// We don't want to open multiple file explorers
	if(fileExplorer) return;

	fileExplorer = new TGFileBrowser(gClient->GetRoot(), nullptr, 800, 400);

	fileExplorer->SetWindowName("File Explorer");

	// Configure the file browser to open in the current working directory
	fileExplorer->AddFSDirectory("/", "/");
	fileExplorer->GotoDir(gSystem->pwd());

	fileExplorer->Connect(
		"CloseWindow()",
		"DAQ::FileSelector",
		this,
		"closeBrowser()"
	);

	// Hook the file browser up to the selection handler
	TGListTree *tree = fileExplorer->GetListTree();
	tree->Connect(
		"DoubleClicked(TGListTreeItem*,Int_t)",
		"DAQ::FileSelector",
		this,
		"handleBrowserSelection(TGListTreeItem*)"
	);

	// Make the UI non-interactable until the user closes the file explorer
	gClient->WaitFor(fileExplorer);

	// TODO: We need to make sure that the user doesn't do something bad with
	//       the main GUI while the file explorer is open. E.g. the user should
	//       not be able to start a run while the file explorer is open.
	//       Otherwise the user can change the settings for the run while it's
	//       running, which is behavior I'd prefer not to have to define

}

// TODO: Reorganize this for easier unit testing
/*
 * This handler is called when the user selects a file from the file explorer.
 * When this happens, the path to the selected file is copied into the entry
 * field and the file explorer is closed.
 * 
 * The entry field can later be queried to recover the path in client code.
 */
void FileSelector::handleBrowserSelection(TGListTreeItem *selection) {

	if(selection == nullptr) return;

	TString path = fileExplorer->FullPathName(selection);

	// This is a Linux-specific struct we can use to validate the user's
	// selection against the filesystem
	struct stat fileStat;

	// If something went wrong, refresh the explorer and alert the user
	if(stat(path.Data(), &fileStat) != 0) {

		if(fileExplorer) fileExplorer->Refresh();

		throw std::runtime_error("The selected file could not be accessed");

	}

	// If we allow directories, and this is a directory, just set the entry
	// field and leave
	if(directoriesAllowed) {

		if(S_ISDIR(fileStat.st_mode)) {

			entryField->SetText(path);
			closeBrowser();
			return;

		}

	}

	// If it's a normal file, set the entry field and leave
	if(S_ISREG(fileStat.st_mode)) {

		entryField->SetText(path);
		closeBrowser();
		return;

	}

	// If it's not a valid selection, but it does exist, do nothing

}

void FileSelector::closeBrowser() {

	// ROOT's memory management scheme is a little hard to navigate. As best I
	// can tell, DeleteWindow will take care of closing the pointer for us.

	if(fileExplorer) {
		fileExplorer->DeleteWindow();
	}

	fileExplorer = nullptr;

	// NOTE: Using DeleteWindow() is a little less responsive than just
	//       deleting the pointer, but I think DeleteWindow() is the way
	//       we're supposed to do it.

}

void FileSelector::allowDirectorySelections(bool allow) {

	directoriesAllowed = allow;

}

void FileSelector::enable() {

	labelField->Enable();

	entryField->SetEnabled(kTRUE);
	fileButton->SetEnabled(kTRUE);

}

void FileSelector::disable() {

	labelField->Disable();

	entryField->SetEnabled(kFALSE);
	fileButton->SetEnabled(kFALSE);

}