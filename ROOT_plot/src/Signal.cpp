/*********************************
 *                               *
 * Define a trigger class        *
 * for reconstruction in         *
 * sMDT chambers                 *
 *                               *
 * Author       : Yuxiang Guo    *
 * Date         : Mar 13, 2021   *
 * Last Modified: Mar 13, 2021   *
 *                               *
 * Updated for phase 2 online    *
 * monitor software              *
 *                               *
 * Author:        Robert Myers   *
 * Date  :        October 2023   *
 *********************************
 */

#include "src/EventID.cpp"

#ifndef MUON_SIGNAL
#define MUON_SIGNAL

namespace Muon {

  struct TDCErrorData {

    unsigned int  TDC         = 0;
    unsigned int  LSBChannel1 = 0;
    unsigned int  LSBChannel2 = 0;
    unsigned int  LSBFlag1    = 0;
    unsigned int  LSBFlag2    = 0;
    unsigned long K28_6_flag  = 0;

  };

  /*
   * Signal is a nice wrapper to a TDC2 word

   * 
   * The packet format is:

   ////////////////////////////////////////////////////////////////////////////

   In triggerless mode:


   Header:
   39-36     35-29    28-17        16-0  
   4'b1010   7'b0     12'EventID   17'TriggerLEdge

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
   39-36     35-29    28-17        16-0  
   4'b1010   7'b0     12'EventID   17'TriggerLEdge

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


  class Signal {
  public:
    Signal();
    Signal(uint64_t word);

    int Type           () const;
    int HeaderEID      () const;
    int TrailerEID     () const;
    int TriggerLEdge   () const;
    int CSMID          () const;
    int TDC            () const;
    int Channel        () const;
    int Mode           () const;    
    int LEdge          () const;  
    int Width          () const;
    int HitCount       () const; 
    int TDCHeaderEID   () const;   

    bool isEventHeader () const;
    bool isEventTrailer() const;
    bool isTDCHeader   () const;
    bool isTDCTrailer  () const;
    bool isTDCError    () const;

    TDCErrorData getTDCError() const;

    static const short HEADER           = 0b1010 ; // bits 37-39
    static const short TRAILER          = 0b1100 ; // bits 37-39

    static const int   TDC_ERROR_CHNL   = 0b11101; // bits 27-31
    static const int   TDC_HEADER_CHNL  = 0b11111; // bits 27-31
    static const int   TDC_TRAILER_CHNL = 0b11110; // bits 27-31

    static const short WORD_SIZE        = 5      ; // bytes

  private:
    int type        ;
    int eventid     ;
    int eventid_t   ;
    int triggerledge;
    int csmid       ;
    int tdcid       ;
    int chnlid      ;
    int mode        ;
    int ledge       ;
    int width       ;
    int hitcount    ;
    int tdc_eventid ;

    TDCErrorData errorData;

  };

  Signal::Signal() {
    Signal(0);
  }
  
  Signal::Signal(uint64_t word) {

    bitset<4>  _type;
    bitset<12> _eventid;
    bitset<12> _eventid_t;
    bitset<17> _triggerledge;
    bitset<3>  _csmid;
    bitset<5>  _tdcid;
    bitset<5>  _chnlid;
    bitset<2>  _mode;
    bitset<17> _ledge;
    bitset<8>  _width;
    bitset<10> _hitcount;
    bitset<12> _tdc_eventid;


    _type         = word >> 36;
    _eventid      = word >> 17;
    _eventid_t    = word >> 16;
    _triggerledge = word >>  0;
    _csmid        = word >> 37;
    _tdcid        = word >> 32;
    _chnlid       = word >> 27;
    _mode         = word >> 25;
    _ledge        = word >>  8;
    _width        = word >>  0;
    _hitcount     = word >>  0;
    _tdc_eventid  = word >> 12;

    type          = static_cast<int>((_type.to_ulong()));
    eventid       = static_cast<int>((_eventid.to_ulong()));
    eventid_t     = static_cast<int>((_eventid_t.to_ulong()));
    triggerledge  = static_cast<int>((_triggerledge.to_ulong()));
    csmid         = static_cast<int>((_csmid.to_ulong()));
    tdcid         = static_cast<int>((_tdcid.to_ulong()));
    chnlid        = static_cast<int>((_chnlid.to_ulong()));
    mode          = static_cast<int>((_mode.to_ulong()));
    ledge         = static_cast<int>((_ledge.to_ulong()));
    width         = static_cast<int>((_width.to_ulong()));
    hitcount      = static_cast<int>((_hitcount.to_ulong()));
    tdc_eventid   = static_cast<int>((_tdc_eventid.to_ulong()));

    if(isTDCError()) {

      bitset<8> K28_6_flag  = word >> 16;
      bitset<3> LSBFlag2    = word >> 13;
      bitset<5> LSBChannel2 = word >> 8 ;
      bitset<3> LSBFlag1    = word >> 5 ;
      bitset<5> LSBChannel1 = word >> 0 ;

      errorData.TDC         = TDC();

      errorData.K28_6_flag  = static_cast<unsigned int>(K28_6_flag.to_ulong ());
      errorData.LSBFlag2    = static_cast<unsigned int>(LSBFlag2.to_ulong   ());
      errorData.LSBChannel2 = static_cast<unsigned int>(LSBChannel2.to_ulong());
      errorData.LSBFlag1    = static_cast<unsigned int>(LSBFlag1.to_ulong   ());
      errorData.LSBChannel1 = static_cast<unsigned int>(LSBChannel1.to_ulong());

    }

  }

  int  Signal:: Type          () const { return type                         ; }
  int  Signal:: HeaderEID     () const { return eventid                      ; }
  int  Signal:: TrailerEID    () const { return eventid_t                    ; }
  int  Signal:: TriggerLEdge  () const { return triggerledge                 ; }
  int  Signal:: CSMID         () const { return csmid                        ; }
  int  Signal:: TDC           () const { return tdcid                        ; }
  int  Signal:: Channel       () const { return chnlid                       ; }
  int  Signal:: Mode          () const { return mode                         ; }
  int  Signal:: LEdge         () const { return ledge                        ; }
  int  Signal:: Width         () const { return width                        ; }
  int  Signal:: HitCount      () const { return hitcount                     ; }
  int  Signal:: TDCHeaderEID  () const { return tdc_eventid                  ; }

  bool Signal:: isEventHeader () const { return Type   () == HEADER          ; }
  bool Signal:: isEventTrailer() const { return Type   () == TRAILER         ; }
  bool Signal:: isTDCHeader   () const { return Channel() == TDC_HEADER_CHNL ; }
  bool Signal:: isTDCTrailer  () const { return Channel() == TDC_TRAILER_CHNL; }
  bool Signal:: isTDCError    () const { return Channel() == TDC_ERROR_CHNL  ; }

  TDCErrorData Signal::getTDCError() const { return errorData; }

}
#endif
