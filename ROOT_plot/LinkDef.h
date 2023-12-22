#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#include "GUI/Core/UISignals.h"
#pragma link C++ class UISignalBus+;

#include "GUI/Components/DeviceSelectorPanel.h"
#pragma link C++ class DeviceSelector+;

#include "GUI/Components/FileSelectorPanel.h"
#pragma link C++ class FileSelector+;

#include "GUI/Components/DataSourcePanel.h"
#pragma link C++ class DataSourcePanel+;

#include "GUI/Components/TabSelector.h"
#pragma link C++ class TabSelector+;

#include "GUI/Components/DAQManager.h"
#pragma link C++ class DAQManager+;

#include "DAQMonitor/DataRunOperations.h"
#pragma link C++ namespace Muon::DataRun+;
#pragma link C++ defined_in Muon::DataRun+;

#include "SignalHandlers.h"
#pragma link C++ namespace Muon::SigHandlers+;
#pragma link C++ defined_in Muon::SigHandlers+;

#endif