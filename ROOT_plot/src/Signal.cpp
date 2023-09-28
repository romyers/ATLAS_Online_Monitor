/*********************************
 *                               *
 * Define a trigger class        *
 * for reconstruction in         *
 * sMDT chambers                 *
 *                               *
 * Author:        Yuxiang Guo    *
 * Date:          Mar 13, 2021   *
 * Last Modified: Mar 13, 2021   *
 *                               *
 *********************************
 */

#include "src/EventID.cpp"

#ifndef MUON_SIGNAL
#define MUON_SIGNAL

namespace Muon {

  /*
   * Signal is a nice wrapper to a TDC2 word

   * 
   * The packet format is:
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
   */


  class Signal {
  public:
    Signal();
    Signal(uint64_t word);

    int Type()         const;
    int HeaderEID()    const;
    int TrailerEID()   const;
    int TriggerLEdge() const;
    int CSMID()        const;
    int TDC()          const;
    int Channel()      const;
    int Mode()         const;    
    int LEdge()        const;  
    int Width()        const;
    int HitCount()     const; 
    int TDCHeaderEID() const;   
    static const short HEADER    = 0b1010;
    static const short TRAILER   = 0b1100;
    static const short DATA_SIZE = 5; // bytes

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
  }

  int Signal:: Type()         const {return type        ;}
  int Signal:: HeaderEID()    const {return eventid     ;}
  int Signal:: TrailerEID()   const {return eventid_t   ;}
  int Signal:: TriggerLEdge() const {return triggerledge;}
  int Signal:: CSMID()        const {return csmid       ;}
  int Signal:: TDC()          const {return tdcid       ;}
  int Signal:: Channel()      const {return chnlid      ;}
  int Signal:: Mode()         const {return mode        ;}
  int Signal:: LEdge()        const {return ledge       ;}
  int Signal:: Width()        const {return width       ;}
  int Signal:: HitCount()     const {return hitcount    ;}
  int Signal:: TDCHeaderEID() const {return tdc_eventid ;}

}
#endif
