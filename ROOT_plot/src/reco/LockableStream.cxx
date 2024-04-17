/**
 * @file LockableStream.cpp
 *
 * @brief A container for an iostream that can be locked and unlocked.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */
#include "MuonReco/LockableStream.h"
namespace MuonReco{

  LockableStream::LockableStream() : stream(nullptr) {}

  void LockableStream::lock  () { m.lock  (); }
  void LockableStream::unlock() { m.unlock(); }
}