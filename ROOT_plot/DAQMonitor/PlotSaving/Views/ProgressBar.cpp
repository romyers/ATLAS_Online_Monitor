#include "ProgressBar.h"

using namespace std;

// TODO: Disable the close window button

ProgressBar::ProgressBar(const TGWindow *p) : TGTransientFrame(p) {

	bar = new TGHProgressBar(this, TGProgressBar::kFancy, 300);
	AddFrame(bar, new TGLayoutHints(kLHintsExpandX));

	bar->SetBarColor("green");

	bar->ShowPosition();

}

void ProgressBar::increment(float inc) {

	bar->Increment(inc);

}