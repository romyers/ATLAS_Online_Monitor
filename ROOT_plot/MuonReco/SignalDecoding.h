#ifndef MUON_SIGNAL_DECODING
#define MUON_SIGNAL_DECODING

#include <stdio.h>
#include <string>
#include <istream>

#include "MuonReco/Signal.h"
#include "MuonReco/Geometry.h"
#include "MuonReco/LockableStream.h"

namespace MuonReco {

  // Byte swap from big-endian to little-endian or vice versa
  uint64_t byteSwap(uint64_t data, uint8_t dataSize);

  /**
  * Provides validation of 'sig', logging an error to the error logger
  * and returning false should sig produce an error.
  * 
  * Signals that fail error validation should be dropped.
  * 
  * @param sig The signal to validate.
  */
  bool validateSignalErrors(const Signal &sig, Geometry &geo);

  /**
 * Reads signals on command from a filestream.
 */
  class SignalReader {
  public:
    SignalReader(LockableStream &in);
    bool   isReady      ();
    Signal extractSignal();
  private:
    LockableStream &dataStream;
  };

}
#endif
