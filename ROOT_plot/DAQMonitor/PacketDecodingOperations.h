/**
 * @file PacketDecodingOperations.cpp
 *
 * @brief Top-level logic flow for the decoder.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>

#include "LockableStream.h"

#include "DataModel/DAQData.h"

namespace Decode {

    void startDecoding(
        LockableData &dataStream, 
        DAQData &data, 
        const std::string &runLabel
    );

    void stopDecoding();

}