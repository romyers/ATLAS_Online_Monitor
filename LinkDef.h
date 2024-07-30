#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#include "src/MonitorView.h"
#pragma link C++ class DAQ::MonitorView+;

#include "src/DAQApp.h"
#pragma link C++ class DAQ::App+;

#endif