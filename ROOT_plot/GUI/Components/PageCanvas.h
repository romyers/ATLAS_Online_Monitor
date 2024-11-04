/**
 * @file PageCanvas.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"
#include "TGLabel.h"
#include "TGButton.h"

#include <vector>
#include <functional>

class PageCanvas : public TGVerticalFrame {

public:

	PageCanvas(
		const TGWindow *p,
		const std::string &title = "", 
		int w = 1,
		int h = 1
	);
	~PageCanvas();

	/**
	 * @brief Redraws the current page.
	 */
	void update();

	/**
	 * @brief Adds a page to the canvas.
	 * 
	 * @param pageDrawer A function that draws the page. The function should
	 * take a TCanvas pointer as an argument, and draw on that canvas as it
	 * would any other TCanvas. PageCanvas will not clear the canvas between
	 * updates, or navigate to the canvas. The pageDrawer is responsible
	 * for clearing the canvas, cd-ing to it, drawing on it, and updating it.
	 * 
	 * PageCanvas just takes care of which pageDrawer to call and when.
	 */
	void addPage(std::function<void(TCanvas *c)> pageDrawer);

	/**
	 * @brief Removes all pages from the canvas, and clears the canvas.
	 */
	void clear();

	/**
	 * @brief Returns the number of pages in the canvas.
	 * 
	 * @returns The number of pages in the canvas.
	 */
	int numPages();

	/**
	 * @brief Displays a specified page, if it exists. Otherwise, does nothing.
	 * Note that the page numbering is 1-indexed.
	 * 
	 * @param page The page to display
	 */
	void showPage(int page);

	void showFirstPage();
	void showPrevPage();
	void showNextPage();
	void showLastPage();

	/**
	 * @brief Returns the canvas contained in the PageCanvas.
	 * 
	 * @returns A pointer to the TCanvas contained in the PageCanvas.
	 */
	TCanvas *GetCanvas();

private:

	TRootEmbeddedCanvas *canvas;

	TGHorizontalFrame *buttonFrame;

		TGLabel      *pageLabel    ;

		TGTextButton *firstButton  ;
		TGTextButton *prevButton ;
		TGTextButton *nextButton;
		TGTextButton *lastButton  ;

	void makeConnections();
	void breakConnections();

	void reconfigurePagePanel();

	int currentPage;

	std::vector<std::function<void(TCanvas *c)>> pageDrawers;

};