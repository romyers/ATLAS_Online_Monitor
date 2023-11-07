/**
 * @file PacketDecodingOperations.cpp
 *
 * @brief Top-level logic flow for the decoder.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

using namespace std;

#include <sstream>

#include "monitorConfig.cpp"

#include "DAQMonitor/LockableStream.cpp"
#include "DAQMonitor/PacketDecoding/src/Decoder.cpp"

#include "src/Geometry.cpp"
#include "src/ProgramControl/Terminator.cpp"
#include "src/DataModel/DAQData.cpp"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

namespace Muon {
namespace Decode {

    void runDecoding(LockableStream &dataStream, DAQData &data);

}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


// TODO: I'd like the termination condition to be defined with the thread,
//       rather than in the run function.
void Decode::runDecoding(LockableStream &dataStream, DAQData &data) {

    Decoder decoder(dataStream, data);

    // TODO: Move termination condition up to where the thread is defined
    while(!Terminator::getInstance().isTerminated("RUN_FLAG")) {

        // TODO: Performance analysis. I'd like this loop to run faster
        //         -- I think binning and drawing is our weak point. Let's
        //            bin every event before drawing

        // FIXME: In file reading mode, this will read the whole file before
        //        terminating on ctrl+c

        decoder.refresh();

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

        this_thread::sleep_for(chrono::milliseconds((int)(1000 / DATA_REFRESH_RATE)));

    }

    cout << "Suspended data decoding." << endl; // TODO: mutex

    data.lock();
    cout << "Processed "
         << data.totalEventCount
         << " events."
         << endl;

    // TODO: Should this include empty events?
    cout << "Processed " 
         << data.processedEvents.size() 
         << " nonempty events." 
         << endl;
    data.unlock();

}