/**
 * @file PacketDecodingOperations.cpp
 *
 * @brief Top-level logic flow for the decoder.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "DAQMonitor/LockableStream.h"

#include "src/DataModel/DAQData.h"

namespace Muon {
namespace Decode {

    void runDecoding(LockableStream &dataStream, DAQData &data);

}
}