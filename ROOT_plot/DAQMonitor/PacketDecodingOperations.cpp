#include "PacketDecodingOperations.h"

using namespace std;

#include <thread>
#include <iostream>

#include "Logging/ErrorLogger.h"

#include "FileManagement/FileManager.h"

#include "Decoder/src/Decoder.h"

#include "GUI/Core/UISignals.h"
#include "GUI/Core/UILock.h"

#include "analysis/MonitorHooks.h"

#include "MuonReco/Geometry.h"

using namespace MuonReco;

size_t MAX_SIGNALS_PER_LOOP = 10000;

/**
 * The approximate rate at which monitor data is refreshed. Note that the
 * monitor will fall short of this rate if it must process too much data
 * at a time.
 */
const double DATA_REFRESH_RATE = 10.; // Hz

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool isDecodeRunning = false;

void aggregateEventData(const DecodeData &loopData, DAQData &data);

void saveNoiseRate(const string &path, const DAQData &data);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Decode::stopDecoding() {

    isDecodeRunning = false;

}

void Decode::startDecoding(
    LockableStream &dataStream, 
    DAQData &data, 
    const string &runLabel
) {

    if(isDecodeRunning) return;

    // Update data with everything zeroed out
    // TODO: Put this with the code that clears DAQData.
    UI::UILock.lock();
    UISignalBus::getInstance().onUpdate();
    UI::UILock.unlock();

    Decoder decoder(MAX_SIGNALS_PER_LOOP);

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////

    // Set up noise rate CSV saving.

    // TODO: Validation and error reporting if the directory structure doesn't
    //       get set up right.
    // TODO: Make sure we don't overwrite an existing file if for some reason
    //       it exists already.

    createIfMissing("../output");

    string outputPath("../output/");
    outputPath += runLabel;

    createDirectory(outputPath);

    outputPath += string("/") + runLabel + string("_rate.csv");

    cout << "Saving noise rate data to: " << outputPath << endl;

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////

    isDecodeRunning = true;

    // When the next noise rate CSV save operation should occur
    int noiseSavepoint = 0; // Events

	vector<unsigned char> dataBuffer;
	dataBuffer.reserve(dataStream.getCacheCapacity());

    while(isDecodeRunning) {

        // TODO: Performance analysis. I'd like this loop to run faster
        //         -- I think binning and drawing is our weak point. Let's
        //            bin every event before drawing

        // FIXME: In file reading mode, this will read the whole file before
        //        terminating on ctrl+c

        // NOTE: Checking any independent member of loopData to determine if
        //       new data has been processed is unreliable. E.g. if there
        //       are new signals but each one was dropped, loopData's
        //       event vector will be empty, but the dropped signals
        //       still need to be recorded.
        DecodeData loopData;
        bool hasData = false;

		// RETRIEVE DATA FROM THE LOCKABLE STREAM

		// Prepare the data buffer for reading
		size_t endPos = dataBuffer.size();
		dataBuffer.resize(MAX_SIGNALS_PER_LOOP * Signal::WORD_SIZE + dataBuffer.size());

		dataStream.lock();

		// Read as much as we can from the file and resize the buffer to fit
		// what we actually were able to read
		dataBuffer.resize(
			dataStream.read(
				(char*)dataBuffer.data() + endPos,
				dataBuffer.size()
			) + endPos
		);

		dataStream.unlock();

		// PROCESS THE DATA

        if(dataBuffer.size() > Signal::WORD_SIZE) {

            MonitorHooks::beforeUpdateData(data);

            // NOTE: We're not worried about thread safety with the geo object
            //       because writes to geo are done only at the very beginning
            //       of a data run, and can't happen concurrently with this 
            //       call.
            loopData = decoder.decodeStream(
                dataBuffer, 
                data.geo, 
                data.tc,
                data.recoUtil
            );

            hasData = true;

        }

        if(hasData) {

            data.lock();

            aggregateEventData(loopData, data);

            // Save the noise CSV file every 1000 events.
            if(data.totalEventCount >= noiseSavepoint) {

                // TODO: Try to avoid doing file I/O within DAQData's critical
                //       section.
                saveNoiseRate(outputPath, data);

                do {

                    noiseSavepoint += 1000;

                } while(data.totalEventCount >= noiseSavepoint);

            }

            data.unlock();

            MonitorHooks::updatedData(data);

        }
        
    }

    cout << "Suspended data decoding." << endl; // TODO: mutex

    data.lock();
    cout << "Processed "
         << data.totalEventCount
         << " events."
         << endl;

    cout << "Processed " 
         << data.nonemptyEventCount 
         << " nonempty events." 
         << endl;
    data.unlock();

}

void aggregateEventData(const DecodeData &loopData, DAQData &data) {

    data.totalEventCount += loopData.eventCount;
    data.nonemptyEventCount += loopData.nonemptyEvents.size();

    data.droppedSignals  += loopData.droppedSignals;
    data.droppedEvents   += loopData.droppedEvents ;

    data.newEvents = loopData.nonemptyEvents;

    for(Event &e : data.newEvents) {

        data.plots.binEvent(e);
        
    }

    data.plots.updateHitRate(data.totalEventCount);

	data.markDirty();

}

void saveNoiseRate(const string &path, const DAQData &data) {

    static bool lastSaveFailed = false;

    // We have to reopen the file every time in order to clear it :'(
    ofstream out(path, ofstream::trunc);
    if(!out.is_open()) {

        ErrorLogger::getInstance().logError(
            string("Couldn't open noiserate CSV output file.")
                + string(" Latest noiserate data not saved.") + path,
            "packetDecoding",
            WARNING
        );

        lastSaveFailed = true;

        return;

    }

    // If the last save failed, let the user know things worked
    // out this time.
    if(lastSaveFailed) {

        ErrorLogger::getInstance().logError(
            string("Noiserate CSV output file reopened"),
            "packetDecoding",
            INFO
        );

        lastSaveFailed = false;

    }

    out << "tdc_id,";

    for(int chnl = 0; chnl < Geometry::MAX_TDC_CHANNEL; ++chnl) {

        out << chnl << ",";

    }
    out << endl;

    for(int tdc = 0; tdc < Geometry::MAX_TDC; ++tdc) {

        out << tdc << ",";

        for(int chnl = 0; chnl < Geometry::MAX_TDC_CHANNEL; ++chnl) {

            out << data.plots.p_tdc_hit_rate[tdc][chnl];
            out << ",";

        }
        out << endl;

    }

    out.close();

}