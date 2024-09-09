#include "FileExplorer.h"

#include "TSystem.h"
#include "TGFileBrowser.h"

using namespace DAQ;

ClassImp(FileExplorer);

FileExplorer::FileExplorer(
	const TGWindow *p,
	TBrowser *browser,
	UInt_t w, 
	UInt_t h
) : TGFileBrowser(p, browser, w, h) {

	// TODO: Block GUI interaction while the file browser is open.
	// TODO: Explore plugging into a TRootBrowser
	// TODO: Use a getFile() function or similar to automatically open one and
	//       wait for user input.
	// TODO: Handle what happens when a user selects a file
	// TODO: Allow external code to set the directory to look in so we can make
	//       it remember the last directory it was in.
	// TODO: Add an 'Open' button
	// TODO: Make sure we handle case where user closes the window
	// TODO: Examine this for QT file dialog:
	//       https://root.cern/doc/v606/QtFileDialog_8C.html

	AddFSDirectory("/", "/");
	GotoDir(gSystem->pwd());

	SetWindowName("File Explorer");

}

FileExplorer::~FileExplorer() {

}