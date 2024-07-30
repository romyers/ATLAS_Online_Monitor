/**
 * @file AlertView.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#include "TGWindow.h"

namespace AlertPopup {
    
    void popupAlert(
        const TGWindow *p, 
        const char *alertType, 
        const char *alertMsg
    );

}