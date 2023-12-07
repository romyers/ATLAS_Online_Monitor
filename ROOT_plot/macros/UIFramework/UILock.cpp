/**
 * @file UILock.cpp
 *
 * @brief A common mutex for UI operations.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 * 
 * NOTE: This lock is used to guard the call to gSystem->ProcessEvents() and
 *       the call to UISignalBus::getInstance().onUpdate(). The former occurs
 *       in StartDAQ.cpp, the latter in PacketDecodingOperations.cpp.
 */

// TODO: Make it more obvious where this lock is used.

#pragma once

#include <mutex>

using namespace std;

namespace Muon{
namespace UI {

	mutex UILock;

}
}