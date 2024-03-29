#include "PacketDecodingOperations.h"

using namespace std;

#include <sstream>
#include <thread>

#include "Decoder/src/Decoder.h"

#include "GUI/Core/UISignals.h"
#include "GUI/Core/UILock.h"

#include "src/Geometry.h"

using namespace Muon;

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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Decode::stopDecoding() {

    isDecodeRunning = false;

}

void Decode::startDecoding(LockableStream &dataStream, DAQData &data) {

    if(isDecodeRunning) return;

    // Update data with everything zeroed out
    // TODO: Put this with the code that clears DAQData.
    Muon::UI::UILock.lock();
    UISignalBus::getInstance().onUpdate();
    Muon::UI::UILock.unlock();

    Decoder decoder(300000);

    isDecodeRunning = true;

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

        dataStream.lock();
        if(hasNewData(*dataStream.stream)) {

            loopData = decoder.decodeStream(*dataStream.stream);
            hasData = true;

        }
        dataStream.unlock();

        if(hasData) {

            data.lock();
            aggregateEventData(loopData, data);
            data.unlock();

            // TODO: This blocks the decode thread while the plots are 
            //       updating. Not a big deal, but it would be nice if
            //       that didn't happen.
            // TODO: For DAT file sources, this won't run until the entire
            //       file has been read. Not exactly ideal.
            Muon::UI::UILock.lock();
            UISignalBus::getInstance().onUpdate();
            Muon::UI::UILock.unlock();

        }

        // TODO: This is hacky; fix it. The idea here is to clear processed
        //       data from the dataStream so we don't produce a de facto
        //       memory leak. But we'd rather the code not have to care what
        //       kind of stream dataStream is. Perhaps we make our own kind
        //       of iostream that clears after read?
        //       https://stackoverflow.com/questions/63034484/how-to-create-stream-which-handles-both-input-and-output-in-c
        //       https://stackoverflow.com/questions/12410961/c-connect-output-stream-to-input-stream
        //       https://stackoverflow.com/questions/26346320/how-to-redirect-input-stream-to-output-stream-in-one-line
        // TODO: We might be able to hook our file and data output streams
        //       together so we only have to write to one of them:
        //       https://stackoverflow.com/questions/1760726/how-can-i-compose-output-streams-so-output-goes-multiple-places-at-once
        // TODO: Might it make sense to make the data stream unbuffered? See:
        //       https://stackoverflow.com/questions/52581080/usage-of-output-stream-buffer-in-context-to-stdcout-and-stdendl
        // TODO: Anyway, we can revisit how we want to handle the data streams
        //       later. For now, this will suffice.
        dataStream.lock();
        stringstream *temp = dynamic_cast<stringstream*>(dataStream.stream);
        if(temp) {
            string unread = temp->eof() ?
                "" : temp->str().substr(temp->tellg());
            temp->str(unread);
        }
        dataStream.unlock();

        // TODO: This thread logic should be at a higher level....
        this_thread::sleep_for(chrono::milliseconds((int)(1000 / DATA_REFRESH_RATE)));

    }

    cout << "Suspended data decoding." << endl; // TODO: mutex

    data.lock();
    cout << "Processed "
         << data.totalEventCount
         << " events."
         << endl;

    cout << "Processed " 
         << data.processedEvents.size() 
         << " nonempty events." 
         << endl;
    data.unlock();

}

void aggregateEventData(const DecodeData &loopData, DAQData &data) {

    data.totalEventCount += loopData.eventCount    ;

    data.droppedSignals  += loopData.droppedSignals;
    data.droppedEvents   += loopData.droppedEvents ;

    data.newEvents = loopData.nonemptyEvents;

    data.processedEvents.insert(
        data.processedEvents.end(), 
        data.newEvents.cbegin  (), 
        data.newEvents.cend    ()
    );

    for(Event &e : data.newEvents) {

        data.plots.binEvent(e);
        
    }

    data.plots.updateHitRate(data.totalEventCount);
    data.plots.saveNoiseRate();
}