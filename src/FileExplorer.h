/**
 * @file FileExplorer.h
 * 
 * @brief A ROOT GUI based file explorer window.
 * 
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGFrame.h"
#include "TGFileBrowser.h"

namespace DAQ {

	class FileExplorer final : public TGTransientFrame {

	public:

		FileExplorer(
			const TGWindow *p = nullptr, 
			const TGWindow *main = nullptr,
			UInt_t w = 1,
			UInt_t h = 1,
			UInt_t options = kVerticalFrame
		);
		~FileExplorer() = default;

	private:

		TGFileBrowser *browser;

	};

}