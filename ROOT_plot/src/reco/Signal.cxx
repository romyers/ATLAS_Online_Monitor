#include "MuonReco/Signal.h"

namespace MuonReco {

  Signal::Signal() {

    Signal(0);
    
  }
  
  Signal::Signal(uint64_t word) {

    std::bitset<4>  _type         ;
    std::bitset<12> _eventid      ;
    std::bitset<18> _eventidext   ;
    std::bitset<1>  _trigoverflow ;
    std::bitset<12> _eventid_t    ;
    std::bitset<17> _triggerledge ;
    std::bitset<3>  _csmid        ;
    std::bitset<5>  _tdcid        ;
    std::bitset<5>  _chnlid       ;
    std::bitset<2>  _mode         ;
    std::bitset<17> _ledge        ;
    std::bitset<8>  _width        ;
    std::bitset<10> _hitcount     ;
    std::bitset<12> _tdc_eventid  ;    
    std::bitset<12> _tdc_bcid     ;
    std::bitset<8>  _tdc_hdrTrlrID;
    std::bitset<4>  _tdc_hdr_count;
    std::bitset<4>  _tdc_tlr_count;
    std::bitset<1>  _hdrCountErr  ;
    std::bitset<1>  _tlrCountErr  ;

    _type          = word >> 36;
    _eventid       = word >> 17;
    _eventidext    = word >> 17;
    _trigoverflow  = word >> 35;
    _eventid_t     = word >> 16;    
    _triggerledge  = word >>  0;
    _csmid         = word >> 37;
    _tdcid         = word >> 32;
    _chnlid        = word >> 27;
    _mode          = word >> 25;
    _ledge         = word >>  8;
    _width         = word >>  0;
    _hitcount      = word >>  0;
    _tdc_eventid   = word >> 12;
    _tdc_bcid      = word >>  0;
    _tdc_hdrTrlrID = word >> 24;
    _tdc_hdr_count = word >> 32;
    _tdc_tlr_count = word >> 28;
    _hdrCountErr   = word >> 15;
    _tlrCountErr   = word >> 14;

    type           = static_cast<uint8_t >((_type         .to_ulong()));
    eventid        = static_cast<uint16_t>((_eventid      .to_ulong()));
    eventid_t      = static_cast<uint16_t>((_eventid_t    .to_ulong()));
    eventid_ext    = static_cast<uint32_t>((_eventidext   .to_ulong()));
    trigoverflow   = static_cast<uint8_t >((_trigoverflow .to_ulong()));
    
    triggerledge   = static_cast<uint32_t>((_triggerledge .to_ulong()));
    csmid          = static_cast<uint8_t >((_csmid        .to_ulong()));
    tdcid          = static_cast<uint8_t >((_tdcid        .to_ulong()));
    chnlid         = static_cast<uint8_t >((_chnlid       .to_ulong()));
    mode           = static_cast<uint8_t >((_mode         .to_ulong()));
    ledge          = static_cast<uint32_t>((_ledge        .to_ulong()));
    width          = static_cast<uint8_t >((_width        .to_ulong()));
    hitcount       = static_cast<uint16_t>((_hitcount     .to_ulong()));
    tdc_eventid    = static_cast<uint16_t>((_tdc_eventid  .to_ulong()));
    tdc_bcid       = static_cast<uint16_t>((_tdc_bcid     .to_ulong()));
    tdc_hdrTrlrID  = static_cast<uint8_t >((_tdc_hdrTrlrID.to_ulong()));
    tdc_hdr_count  = static_cast<uint8_t >((_tdc_hdr_count.to_ulong()));
    tdc_tlr_count  = static_cast<uint8_t >((_tdc_tlr_count.to_ulong()));
    hdrCountErr    = static_cast<uint8_t >((_hdrCountErr  .to_ulong()));
    tlrCountErr    = static_cast<uint8_t >((_tlrCountErr  .to_ulong()));

    time_in_ns = ledge * 0.78125;

    if(isTDCOverflow()) {

      std::bitset<8> K28_6_flag  = word >> 16;
      std::bitset<3> LSBFlag2    = word >> 13;
      std::bitset<5> LSBChannel2 = word >> 8 ;
      std::bitset<3> LSBFlag1    = word >> 5 ;
      std::bitset<5> LSBChannel1 = word >> 0 ;

      errorData.TDC         = TDC();

      errorData.K28_6_flag  = static_cast<uint8_t>(K28_6_flag .to_ulong());
      errorData.LSBFlag2    = static_cast<uint8_t>(LSBFlag2   .to_ulong());
      errorData.LSBChannel2 = static_cast<uint8_t>(LSBChannel2.to_ulong());
      errorData.LSBFlag1    = static_cast<uint8_t>(LSBFlag1   .to_ulong());
      errorData.LSBChannel1 = static_cast<uint8_t>(LSBChannel1.to_ulong());

    }

  }

  uint8_t  Signal:: Type           () const { return type                                   ; }
  uint16_t Signal:: HeaderEID      () const { return eventid                                ; }
  uint32_t Signal:: HeaderEIDext   () const { return eventid_ext                            ; }
  uint8_t  Signal:: TrigOverflow   () const { return trigoverflow                           ; }
  uint16_t Signal:: TrailerEID     () const { return eventid_t                              ; }
  uint32_t Signal:: TriggerLEdge   () const { return triggerledge                           ; }
  uint8_t  Signal:: CSMID          () const { return csmid                                  ; }
  uint8_t  Signal:: TDC            () const { return tdcid                                  ; }
  uint8_t  Signal:: Channel        () const { return chnlid                                 ; }
  uint8_t  Signal:: Mode           () const { return mode                                   ; }
  uint32_t Signal:: LEdge          () const { return ledge                                  ; }
  uint8_t  Signal:: Width          () const { return width                                  ; }
  uint16_t Signal:: HitCount       () const { return hitcount                               ; }
  uint16_t Signal:: TDCHeaderEID   () const { return tdc_eventid                            ; }
  uint16_t Signal:: TDCHeaderBCID  () const { return tdc_bcid                               ; }  
  uint8_t  Signal:: TDCHdrTrlrID   () const { return tdc_hdrTrlrID                          ; }
  uint8_t  Signal:: TDCHdrCount    () const { return tdc_hdr_count                          ; }
  uint8_t  Signal:: TDCTlrCount    () const { return tdc_tlr_count                          ; }
  uint8_t  Signal:: HeaderCountErr () const { return hdrCountErr                            ; }
  uint8_t  Signal:: TrailerCountErr() const { return tlrCountErr                            ; }

  bool     Signal:: isEventHeader  () const { return Type         () == HEADER              ; }
  bool     Signal:: isEventTrailer () const { return Type         () == TRAILER             ; }
  bool     Signal:: isTDCHeader    () const { return (!isEventHeader())&&(!isEventTrailer())&&(TDCHdrTrlrID () == TDC_HEADER_IDENT    ); }
  bool     Signal:: isTDCTrailer   () const { return (!isEventHeader())&&(!isEventTrailer())&&(TDCHdrTrlrID () == TDC_TRAILER_IDENT   ); }
  bool     Signal:: isTDCOverflow  () const { return (!isEventHeader())&&(!isEventTrailer())&&(TDCHdrTrlrID () == TDC_OVERFLOW_IDENT  ); }
  bool     Signal:: isTDCDecodeErr () const { return (!isEventHeader())&&(!isEventTrailer())&&(TDCHdrTrlrID () == TDC_DECODE_ERR_IDENT); }

  double   Signal:: Time           () const { return time_in_ns;                              }

  TDCErrorData Signal::getTDCError () const { return errorData                              ; }

}  //MuonReco

