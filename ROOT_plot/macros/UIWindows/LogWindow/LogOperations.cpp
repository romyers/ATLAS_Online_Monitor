/**
 * @file LogOperations.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "macros/UIWindows/LogWindow/LogView.cpp"

using namespace std;
using namespace Muon;

namespace Muon {
namespace Logging {

    LogView *view;

    void log(const string &type, const string &entry) {

        cout << "Logging" << endl;

        view->addEntry(type, entry);

    }

}
}