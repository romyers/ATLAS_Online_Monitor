#include "PlotSavingOperations.h"

#include <thread>

#include <sys/stat.h>

#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"

#include "DAQState.h"

#include "MuonReco/Geometry.h"

#include "DataModel/DAQData.h"

#include "GUI/Components/ProgressBar.h"

using namespace std;
using namespace MuonReco;

// TODO: Remove dependency on the progress bar

void makeDirectory(const string &path);

bool pathDirectoryExists(const string &path);

void saveNoiseRate();

void PlotSaving::savePlots() {

    // TODO: We might need to protect this from data race conditions
    static bool isSaving = false;

    if(isSaving) {

        cout << "Please wait for the current snapshot to be saved before "
             << "saving again." << endl;

        return;

    }

    isSaving = true;

    cout << "Saving snapshot..." << endl;

    makeDirectory("../output");
    cout << "Created output directory." << endl;

    State::DAQState state = State::DAQState::getState();
    string outputDirName = string("../output/") + state.tempState.runLabel;

    // If the directory already exists, save a second directory.
    //int i = 1;
    //string temp = outputDirName;
    //while(pathDirectoryExists(temp)) {

    //    temp = outputDirName + " (" + to_string(i) + ")";
    //    ++i;

    //}
    //outputDirName = temp;
    //

    if (!pathDirectoryExists(outputDirName)) {

        makeDirectory(outputDirName);

    }
    cout << "Created directory " << outputDirName << endl;

    // TODO: We need to save the root output too -- see DecodeOffline.cpp:253

    // TODO: Figure out behavior for saving the same run twice
    //         -- I think it should overwrite the previous save

    // Plot saving should happen in a separate thread, while still updating the
    // progress bar. Otherwise we block the decode thread.

    DAQData &data = DAQData::getInstance();

    data.lock();
    Plots snapshot(data.plots);
    data.unlock();

    // NOTE: We can speed things up a lot by setting gROOT->SetBatch(), but
    //       this also breaks all the graphics. Useful if saves only happen
    //       on exit, but not otherwise. Setting outputCanvas->SetBatch()
    //       has no effect on performance.

    TRootEmbeddedCanvas *outputCanvas = new TRootEmbeddedCanvas("Output Canvas", gClient->GetRoot());

    // NOTE: We avoid displaying the window by omitting the calls to 
    //       MapSubwindows() and MapWindow()

    outputCanvas->SetWindowName("Output Preview");
    outputCanvas->Resize(700, 700);
    outputCanvas->GetCanvas()->cd();

    ProgressBar *progressBar = new ProgressBar(gClient->GetRoot());

    progressBar->SetWindowName("Save Progress");
    progressBar->MapSubwindows();
    progressBar->Resize(progressBar->GetDefaultSize());
    progressBar->MapWindow();

    int activeTDCs = 0;
    int activeChannels = 0;

    for(int tdc = 0; tdc < Geometry::MAX_TDC; ++tdc) {

        if(data.geo.IsActiveTDC(tdc)) {

            ++activeTDCs;

        }

        for(int chnl = 0; chnl < Geometry::MAX_TDC_CHANNEL; ++chnl) {

            if(
                data.geo.IsActiveTDCChannel(tdc, chnl) || 
                (tdc == data.geo.TRIGGER_MEZZ)
            ) {

                ++activeChannels;

            }

        }

    }

    // Each TDC makes a noise plot, a tdc overview plot, and an adc overview plot.
    // Each channel makes an adc_time plot, a tdc_time plot, and a tdc_time_corrected
    // plot.
    float incr = 100. / (3 * activeTDCs + 3 * activeChannels);

    for(int tdc = 0; tdc < Geometry::MAX_TDC; ++tdc) {

        if(data.geo.IsActiveTDC(tdc)) {

            string dirName = outputDirName + "/NoiseRate";

            makeDirectory(dirName);

            cout << "Created directory " << dirName << endl;

            string filename = string("tdc_") + to_string(tdc) + string("_hit_rate.png");

            snapshot.p_tdc_hit_rate_graph[tdc]->Draw("AB");
            outputCanvas->GetCanvas()->SaveAs((dirName + "/" + filename).data());

            cout << "Saved TDC " << tdc << " NoiseRate plot." << endl;

            progressBar->increment(incr);

            dirName = outputDirName
                + "/TDC_" 
                + to_string(tdc) 
                + "_of_" 
                + to_string(Geometry::MAX_TDC) 
                + "_Time_Spectrum";

            makeDirectory(dirName);

            cout << "Created directory " << dirName << endl;

            // TODO: Consider this implementation:
            //       https://root.cern/doc/v608/pad2png_8C.html

            snapshot.p_tdc_tdc_time_corrected[tdc]->Draw("colz");
            filename = string("tdc_") + tdc + "_tdc_time_spectrum_corrected.png";
            outputCanvas->GetCanvas()->SaveAs((dirName + "/" + filename).data());

            cout << "Saved TDC " << tdc << "TDC overview plot." << endl;

            progressBar->increment(incr);

            snapshot.p_tdc_adc_time[tdc]->Draw("colz");
            filename = string("tdc_") + tdc + "_adc_time_spectrum.png";
            outputCanvas->GetCanvas()->SaveAs((dirName + "/" + filename).data());

            cout << "Saved TDC " << tdc << "ADC overview plot." << endl;

            progressBar->increment(incr);

            for(int chnl = 0; chnl < Geometry::MAX_TDC_CHANNEL; ++chnl) {

                if(
                    data.geo.IsActiveTDCChannel(tdc, chnl) || 
                    (tdc == data.geo.TRIGGER_MEZZ)
                ) {

                    snapshot.p_tdc_time_corrected[tdc][chnl]->Draw("colz");
                    filename = string("tdc_") + tdc + "__channel_" + chnl + "__tdc_time_spectrum_corrected.png";
                    outputCanvas->GetCanvas()->SaveAs((dirName + "/" + filename).data());

                    progressBar->increment(incr);

                    snapshot.p_tdc_time[tdc][chnl]->Draw("colz");
                    filename = string("tdc_") + tdc + "__channel_" + chnl + "__tdc_time_spectrum.png";
                    outputCanvas->GetCanvas()->SaveAs((dirName + "/" + filename).data());

                    progressBar->increment(incr);

                    snapshot.p_adc_time[tdc][chnl]->Draw("colz");
                    filename = string("tdc_") + tdc + "__channel_" + chnl + "__adc_time_spectrum.png";
                    outputCanvas->GetCanvas()->SaveAs((dirName + "/" + filename).data());

                    progressBar->increment(incr);

                }

            }

            cout << "Saved TDC " << tdc << " channel plots." << endl;

        }

    }

    delete outputCanvas;
    delete progressBar;

    cout << "Plots saved!" << endl;

    isSaving = false;

}

void makeDirectory(const string &path) {

    if(mkdir(path.data(), 0777) == -1) {

        cerr << strerror(errno) << endl;

    }

}

bool pathDirectoryExists(const string &path) {

    struct stat sb;

    if(stat(path.data(), &sb) == 0) {

        return true;

    }

    return false;

}