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
#include <vector>

#include "LockableStream.h"

#include "DataModel/DAQData.h"

#include "EthernetCapture/src/PCapDevice.h"

namespace DataCapture {

    void startDataCapture(
        LockableStream &dataStream, 
        DAQData &data, 
        const std::string &runLabel
    );

    void stopDataCapture();

    std::vector<PCapDevice> getNetworkDevices();

}