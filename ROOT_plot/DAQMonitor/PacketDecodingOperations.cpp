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

// Should be bigger than MAX_SIGNALS_PER_LOOP * Signal::WORD_SIZE
size_t READ_BLOCK_SIZE = MAX_SIGNALS_PER_LOOP * Signal::WORD_SIZE * 10;

/**
 * The approximate rate at which monitor data is refreshed. Note that the
 * monitor will fall short of this rate if it must process too much data
 * at a time.
 */
const double DATA_REFRESH_RATE = 10.; // Hz

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool stop = true;
bool immediateStop = true;

void aggregateEventData(const DecodeData &loopData, DAQData &data);

void saveNoiseRate(const string &path, const DAQData &data);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Decode::markStart() {

	stop = false;
	immediateStop = false;

}

void Decode::stopSoftly() {

    stop = true;

}

void Decode::stopImmediately() {

	stop = true;
	immediateStop = true;

}

void Decode::startDecoding(
    LockableStream &dataStream, 
    DAQData &data, 
    const string &runLabel
) {

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

    // When the next noise rate CSV save operation should occur
    int noiseSavepoint = 0; // Events

	vector<unsigned char> dataBuffer;
	dataBuffer.reserve(READ_BLOCK_SIZE);

	// Problem: We don't want to stop if there's still data in the 
	//          live capture case, but we do want to be able to stop
	//          if there's still data and we're reading from a file

    while(true) {

		// RETRIEVE DATA FROM THE LOCKABLE STREAM

		if(dataBuffer.size() < MAX_SIGNALS_PER_LOOP * Signal::WORD_SIZE) {
			
			dataStream.lock();
			vector<unsigned char> newData = dataStream.get(
				READ_BLOCK_SIZE - dataBuffer.size()
			);
			dataStream.unlock();

			dataBuffer.insert(
				dataBuffer.end(),
				make_move_iterator(newData.begin()),
				make_move_iterator(newData.end())
			);

		}

		// PROCESS THE DATA

        DecodeData loopData;

        if(dataBuffer.size() > Signal::WORD_SIZE) {

            MonitorHooks::beforeUpdateData(data);

            // NOTE: We're not worried about thread safety with the geo object
            //       because writes to geo are done only at the very beginning
            //       of a data run, and can't happen concurrently with this 
            //       call.
			// NOTE: This call will remove data from dataBuffer.
            loopData = decoder.decodeStream(
                dataBuffer, 
                data.geo, 
                data.tc,
                data.recoUtil
            );

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

        } else if(stop) {

			// Stop when we're out of data and we've been told to stop

			break;

		}

		if(immediateStop) {

			// If we're supposed to stop immediately, stop
			// when we're told even if we're not out of data.

			break;

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