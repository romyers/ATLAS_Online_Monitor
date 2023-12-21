/**
 * @file PacketDecodingOperations.cpp
 *
 * @brief Top-level logic flow for the decoder.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "LockableStream.h"

#include "DataModel/DAQData.h"

namespace Muon {
namespace Decode {

    void runDecoding(LockableStream &dataStream, DAQData &data);

}
}