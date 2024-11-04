#include "PageCanvas.h"

#include "TCanvas.h"

/*
 * DEVELOPER NOTES: 
 *   - The currentPage variable is one-indexed -- be careful to remember this.
 *     It is allowed to be zero in the case where it does not refer to any page.
 */

PageCanvas::PageCanvas(
	const TGWindow *p,
	const std::string &title, 
	int w,
	int h
) : TGVerticalFrame(p, w, h), currentPage(0) {

	canvas = new TRootEmbeddedCanvas(title.data(), this, w, h);
	AddFrame(canvas, new TGLayoutHints(kLHintsCenterX | kLHintsExpandX | kLHintsExpandY));

	buttonFrame = new TGHorizontalFrame(this);
	AddFrame(buttonFrame, new TGLayoutHints(kLHintsExpandX | kLHintsBottom));

		pageLabel = new TGLabel(buttonFrame, "No pages");
		buttonFrame->AddFrame(pageLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY));

		lastButton = new TGTextButton(buttonFrame, "->|");
		buttonFrame->AddFrame(lastButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

		nextButton = new TGTextButton(buttonFrame, "-->");
		buttonFrame->AddFrame(nextButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

		prevButton = new TGTextButton(buttonFrame, "<--");
		buttonFrame->AddFrame(prevButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

		firstButton = new TGTextButton(buttonFrame, "|<-");
		buttonFrame->AddFrame(firstButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

	makeConnections();

	SetWindowName(title.data());

	update();

}

PageCanvas::~PageCanvas() {

	breakConnections();

}

void PageCanvas::update() {

	showPage(currentPage);

}

void PageCanvas::addPage(std::function<void(TCanvas *)> pageDrawer) {

	pageDrawers.push_back(pageDrawer);

	if(currentPage == 0) {

		currentPage = 1;

	} else {

		reconfigurePagePanel();

	}

}

void PageCanvas::clear() {

	pageDrawers.clear();
	canvas->GetCanvas()->Clear();

	currentPage = 0;

	update();

}

void PageCanvas::showPage(int page) {

	if (page < 1 || page > numPages()) {

		reconfigurePagePanel();
		
		return;

	}

	currentPage = page;

	canvas->GetCanvas()->Clear();

	// We've already bounds-checked page, so we know pageDrawers[page] exists
	pageDrawers[currentPage - 1](canvas->GetCanvas());

	// Page panel must be reconfigured AFTER currentPage is updated
	reconfigurePagePanel();

}

TCanvas *PageCanvas::GetCanvas() {

	return canvas->GetCanvas();

}

void PageCanvas::makeConnections() {

	firstButton->Connect("Clicked()", "PageCanvas", this, "showFirstPage()");

	prevButton->Connect("Clicked()", "PageCanvas", this, "showPrevPage()");

	nextButton->Connect("Clicked()", "PageCanvas", this, "showNextPage()");

	lastButton->Connect("Clicked()", "PageCanvas", this, "showLastPage()");

}

void PageCanvas::breakConnections() {

	firstButton->Disconnect("Clicked()", this, "showFirstPage()");
	prevButton->Disconnect("Clicked()", this, "showPrevPage()");
	nextButton->Disconnect("Clicked()", this, "showNextPage()");
	lastButton->Disconnect("Clicked()", this, "showLastPage()");

}

void PageCanvas::reconfigurePagePanel() {

	pageLabel->SetText(Form("Page %d/%d", currentPage, numPages()));

	if(currentPage == 0) pageLabel->SetText("No pages");

	if(currentPage > 1) {

		firstButton->SetEnabled(kTRUE);
		prevButton->SetEnabled(kTRUE);

	} else {

		firstButton->SetEnabled(kFALSE);
		prevButton->SetEnabled(kFALSE);

	}

	// WARNING -- If you replace numPages() with an expression with an unsigned
	//            type, you will get underflow when there are no pages, causing
	//            the next and last buttons to erroneously be disabled in this
	//            case.
	if(currentPage < numPages()) {

		nextButton->SetEnabled(kTRUE);
		lastButton->SetEnabled(kTRUE);

	} else {

		nextButton->SetEnabled(kFALSE);
		lastButton->SetEnabled(kFALSE);

	}

}

int PageCanvas::numPages() {

	return pageDrawers.size();

}

void PageCanvas::showFirstPage() { showPage(1); }
void PageCanvas::showPrevPage()  { showPage(currentPage - 1); }
void PageCanvas::showNextPage()  { showPage(currentPage + 1); }
void PageCanvas::showLastPage()  { showPage(numPages()); }