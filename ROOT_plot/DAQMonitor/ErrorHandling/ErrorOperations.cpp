/**
 * @file ErrorOperations.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "DAQMonitor/ErrorHandling/Views/ErrorView.cpp"

using namespace std;

namespace Muon {
namespace ErrorHandling {

    void openErrorViewer ();
    void closeErrorViewer();

}
namespace ErrorHandlingIMPL {

    class ErrorWindows {

    public:

        ErrorWindows  (      ErrorWindows &other) = delete;
        void operator=(const ErrorWindows &other) = delete;

        void openErrorViewer       ();
        void closeErrorViewer      ();
        void closeErrorViewerWindow();

        static ErrorWindows &getInstance();

    private:

        ErrorWindows();

        ErrorView *errorViewer;

    };

}
}

void Muon::ErrorHandling::openErrorViewer() {

    ErrorHandlingIMPL::ErrorWindows::getInstance().openErrorViewer();

}

void Muon::ErrorHandling::closeErrorViewer() {

    ErrorHandlingIMPL::ErrorWindows::getInstance().closeErrorViewerWindow();

}

void Muon::ErrorHandlingIMPL::ErrorWindows::openErrorViewer() {

    if(errorViewer != nullptr) return;

    errorViewer = new ErrorView(gClient->GetRoot());

    errorViewer->SetWindowName("Error Log");
    errorViewer->MapSubwindows();
    errorViewer->Resize(errorViewer->GetDefaultSize());
    errorViewer->MapWindow();

    errorViewer->Connect("CloseWindow()", "ErrorHandlingIMPL::ErrorWindows", this, "closeErrorViewer()");

}

void Muon::ErrorHandlingIMPL::ErrorWindows::closeErrorViewer() {

    if(errorViewer == nullptr) return;

    errorViewer->teardown();

    errorViewer = nullptr;

}

void Muon::ErrorHandlingIMPL::ErrorWindows::closeErrorViewerWindow() {

    if(errorViewer == nullptr) return;

    ErrorView *temp = errorViewer;

    closeErrorViewer();

    temp->CloseWindow();

}

Muon::ErrorHandlingIMPL::ErrorWindows &Muon::ErrorHandlingIMPL::ErrorWindows::getInstance() {

    static ErrorWindows instance;

    return instance;

}

Muon::ErrorHandlingIMPL::ErrorWindows::ErrorWindows() : errorViewer(nullptr) {}