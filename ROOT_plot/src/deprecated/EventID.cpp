/*********************************
 *                               *
 * Define a trigger class        *
 * for reconstruction in         *
 * sMDT chambers                 *
 *                               *
 * Author:        Kevin Nelson   *
 * Date:          May 13, 2018   *
 * Last Modified: May 13, 2018   *
 *                               *
 *********************************
 */

#include "EventID.h"

#include <bitset>

using namespace std;
using namespace Muon;
  
EventID::EventID() {
  EventID(0x00000000);
}
  
EventID::EventID(unsigned int word) {
  bitset<4>  _type;
  bitset<4>  _tdc;
  bitset<12> _event_id;
  bitset<12> _bunch_id;

  _type     = word >> 28;
  _tdc      = word >> 24;
  _event_id = word >> 12;
  _bunch_id = word;

  unsigned long event_id, bunch_id;

  type     = static_cast<unsigned int >((_type.to_ulong()));
  tdc      = static_cast<unsigned int >((_tdc.to_ulong()));
  event_id = static_cast<unsigned long>((_event_id.to_ulong()));
  bunch_id = static_cast<unsigned long>((_bunch_id.to_ulong()));

  this->id = 4096UL*bunch_id + event_id;
}

unsigned int EventID::Type() {
  return this->type;
}

unsigned int EventID::TDC() {
  return this->tdc;
}

unsigned long EventID::ID() {
  return this->id;
}