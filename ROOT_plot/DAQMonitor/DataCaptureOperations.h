/**
 * @file DataCaptureOperations.h
 *
 * @brief Top-level logic flow for the data capture module.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>

#include "LockableStream.h"

#include "DataModel/DAQData.h"

namespace Muon {
namespace DataCapture {

    void runDataCapture(
        LockableStream &dataStream, 
        DAQData &data, 
        std::string runLabel
    );

}
}