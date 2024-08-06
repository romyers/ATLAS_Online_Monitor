#include "FileExplorer.h"

#include "TSystem.h"
#include "TGFileBrowser.h"

using namespace DAQ;

FileExplorer::FileExplorer(
	const TGWindow *p,
	const TGWindow *main,
	UInt_t w, 
	UInt_t h,
	UInt_t options
) : TGTransientFrame(p, main, w, h, options) {

	// TODO: Consider making my own file browser out of a TGListTree. I don't
	//       like the layout of TGFileBrowsers.

	// TODO: Block GUI interaction while the file browser is open.
	// TODO: Explore plugging into a TRootBrowser
	// TODO: Use a getFile() function or similar to automatically open one and
	//       wait for user input.
	// TODO: Handle what happens when a user selects a file

	browser = new TGFileBrowser(this, nullptr, w, h);
	AddFrame(browser, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 10));

	browser->AddFSDirectory("/", "/");
	browser->GotoDir(gSystem->pwd());

	// Set the window title
	SetWindowName("File Explorer");

	// Map the window
	MapSubwindows();
	Resize(GetDefaultSize());
	MapWindow();

}