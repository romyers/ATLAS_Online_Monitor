#ifndef MUON_SIGNAL
#define MUON_SIGNAL

#include <bitset>
#include <iostream>
/*
   * Signal is a nice wrapper to a TDC2 word

   * 
   * The packet format is:

   ////////////////////////////////////////////////////////////////////////////

   In triggerless mode:


   Header:
   39-36     35                 34-29             28-17        16-0  
   4'b1010   1'TriggerOverflow  6'EventIDext      12'EventID   17'TriggerLEdge

   TDC Data:
   39-37     36-32    31-27       26-25   24-8      7-0
   3'CSMID   5'TDCID  5'CHNLID    2'mode  17'LEdge  8'Width

   TDC Error:
   39-37     36-32    31-24         23-16       15-13       12-8    7-5         4-0
   3'CSMID   5'TDCID  8'b1110_1000  K28.6_flag  is_overflow channel is_overflow channel

   TDC Header:
   39-37     36-32    31-24         23-12         11-0
   3'CSMID   5'TDCID  8'b1111_1000  TDC_Event_ID  trigger_BCID

   TDC Trailer:
   39-37     36-32    31-24         23-12  11           10        9-0
   3'CSMID   5'TDCID  8'b1111_0000  12'b0  trigger_lost time_out  TDC_hit_count

   Trailer:
   39-36     35-28    27-16       15-10   9-0  
   4'b1100   8'b0     12'EventID  6'b0    10'HitCount

   ////////////////////////////////////////////////////////////////////////////

   And in triggered mode:

   Header:
   39-36     35                 34-29             28-17        16-0  
   4'b1010   1'TriggerOverflow  6'EventIDext      12'EventID   17'TriggerLEdge

   TDC Data:
   39-37     36-32    31-27       26-25   24-8      7-0
   3'b000    5'TDCID  5'CHNLID    2'mode  17'LEdge  8'Width

   TDC Header:
   39-37     36-32    31-24         23-12         11-0
   3'b000    5'TDCID  8'b1111_1000  TDC_Event_ID  trigger_BCID

   TDC Trailer:
   39-37     36-32    31-24         23-12  11           10        9-0
   3'b000    5'TDCID  8'b1111_0000  12'b0  trigger_lost time_out  TDC_hit_count

   Trailer:
   39-36     35-32                31-28                 27-16       15                    14                     13-10  9-0  
   4'b1100   4'TDC header count   4'TDC trailer count   12'EventID  1'header count error  1'trailer count error  4'b0   10'HitCount
*/

namespace MuonReco {

  struct TDCErrorData {

    uint8_t  TDC         = 0;
    uint8_t  LSBChannel1 = 0;
    uint8_t  LSBChannel2 = 0;
    uint8_t  LSBFlag1    = 0;
    uint8_t  LSBFlag2    = 0;
    uint8_t  K28_6_flag  = 0;

  };

  class Signal {

  public:
    
    Signal(             );
    Signal(uint64_t word);

    // prefix
    uint8_t      Type           () const;
    uint8_t      CSMID          () const;
    

    // TDC Data triggerless mode public info
    uint8_t      TDC            () const;
    uint8_t      Channel        () const; 
    uint8_t      Mode           () const;  
    uint32_t     LEdge          () const; 
    uint8_t      Width          () const;

    // MiniDAQ Event Header triggerless mode public info
    uint16_t     HeaderEID      () const;
    uint32_t     HeaderEIDext   () const;
    uint8_t      TrigOverflow   () const;
    uint32_t     TriggerLEdge   () const;

    // MiniDAQ Event Trailer triggerless mode public info
    uint16_t     TrailerEID     () const;
    uint16_t     HitCount       () const; 


    // Note: Triggered mode shares all triggerless mode public
    //       info and add more extra public info

    // TDC Data triggered mode extra public info
    uint16_t     TDCHeaderEID   () const;  
    uint16_t     TDCHeaderBCID  () const;
    uint8_t      TDCHdrTrlrID   () const;

    // MiniDAQ Event Trailer triggered mode extra public info
    uint8_t      TDCHdrCount    () const;
    uint8_t      TDCTlrCount    () const;
    uint8_t      HeaderCountErr () const;
    uint8_t      TrailerCountErr() const;

    // public word flag, returned from Type() and TDCHdrTrlrID()
    bool         isEventHeader  () const;
    bool         isEventTrailer () const;
    bool         isTDCHeader    () const;
    bool         isTDCTrailer   () const;
    bool         isTDCOverflow  () const;
    bool         isTDCDecodeErr () const;
    double       Time           () const;

    TDCErrorData getTDCError    () const;

    static const uint8_t HEADER               = 0b1010    ; // bits 37-39
    static const uint8_t TRAILER              = 0b1100    ; // bits 37-39

    static const uint8_t TDC_HEADER_IDENT     = 0b11111000; // bits 31-24
    static const uint8_t TDC_TRAILER_IDENT    = 0b11110000; // bits 31-24
    static const uint8_t TDC_OVERFLOW_IDENT   = 0b11101000; // bits 31-24
    static const uint8_t TDC_DECODE_ERR_IDENT = 0b11110111; // bits 31-24

    static const uint8_t WORD_SIZE            = 5         ; // bytes

    static const short RISING                 = 4         ; //for phase1 compatibility
    static const short FALLING                = 5         ; //for phase1 compatibility
 
  private:

    uint8_t  type         ;
    uint16_t eventid      ;
    uint16_t eventid_t    ;
    uint32_t eventid_ext  ;
    uint8_t  trigoverflow ;
    uint32_t triggerledge ;
    uint8_t  csmid        ;
    uint8_t  tdcid        ;
    uint8_t  chnlid       ;
    uint8_t  mode         ;
    uint32_t ledge        ;
    uint8_t  width        ;
    uint16_t hitcount     ;
    uint16_t tdc_eventid  ;
    uint16_t tdc_bcid     ;
    uint8_t  tdc_hdrTrlrID;
    uint8_t  tdc_hdr_count;
    uint8_t  tdc_tlr_count;
    uint8_t  hdrCountErr  ;
    uint8_t  tlrCountErr  ;

    double      time_in_ns;

    TDCErrorData errorData;

  }; //class Signal

} //MuonReco

#endif
