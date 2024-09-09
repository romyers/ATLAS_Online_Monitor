/**
 * @file FileExplorer.h
 * 
 * @brief A ROOT GUI based file explorer window.
 * 
 * In order to get information out of this window, it's best to access the
 * TGListTree object directly via TGFileBrowser::GetListTree(). You can then
 * connect its signals to your own slots.
 * 
 * To create a new FileExplorer window, simply create a new instance of this
 * class with the parent window set to gClient->GetRoot(). 
 * 
 * The FileExplorer will automatically open in the current working directory,
 * but this can be changed by calling TGFileBrowser::GotoDir() on the 
 * FileExplorer object.
 * 
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGFrame.h"
#include "TGFileBrowser.h"

namespace DAQ {

	class FileExplorer final : public TGFileBrowser {

	ClassDef(FileExplorer, 0);

	public:

		/**
		 * @brief Construct a new FileExplorer object.
		 * 
		 * @param p The parent window for this FileExplorer.
		 * @param b The browser object to use for this FileExplorer, if desired.
		 * @param w The starting width for the FileExplorer window.
		 * @param h The starting height for the FileExplorer window.
		 */
		FileExplorer(
			const TGWindow *p = nullptr, 
			TBrowser *b = nullptr,
			UInt_t w = 200,
			UInt_t h = 200
		);
		virtual ~FileExplorer();

	private:

		TGListTree *tree;

	};

}