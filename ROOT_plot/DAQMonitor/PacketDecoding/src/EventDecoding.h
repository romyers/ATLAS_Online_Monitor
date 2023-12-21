/**
 * @file EventDecoding.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>

#include "src/Signal.h"
#include "src/Event.h"

Muon::Event assembleEvent        (      std:: vector<Muon::Signal>  signals);
bool        validateEventErrors  (const Muon::Event                &e      );
void        validateEventWarnings(const Muon::Event                &e      );
void        processEvent         (      Muon::Event                &e      );