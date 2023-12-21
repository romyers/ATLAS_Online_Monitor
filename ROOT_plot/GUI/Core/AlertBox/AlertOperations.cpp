#include "AlertOperations.h"

#include "TGMsgBox.h"

// For some reason, bad things happen if we try to extend TGMsgBox,
// so we'll just the box by itself.
//   -- I think there's something odd happening with destruction logic
//      when we subclass
void Muon::Error::popupAlert(const TGWindow *p, const char *alertType, const char *alertMsg) {

    TGMsgBox *box = new TGMsgBox(gClient->GetRoot(), p, alertType, alertMsg);

    // The box should clean itself up when user presses dismiss.

    // FIXME: We have to force CTRL+C if an alert box is open.
    //        Find a way to make it respond to SIGINT

}