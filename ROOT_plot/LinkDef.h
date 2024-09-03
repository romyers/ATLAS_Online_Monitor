#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#include "GUI/Core/UISignals.h"
#pragma link C++ class UISignalBus+;

#include "GUI/Components/Submenu.h"
#pragma link C++ class Submenu+;

#include "GUI/Components/DeviceSelectorPanel.h"
#pragma link C++ class DeviceSelector+;

#include "GUI/Components/FileSelectorPanel.h"
#pragma link C++ class FileSelector+;

#include "GUI/Components/DataSourcePanel.h"
#pragma link C++ class DataSourcePanel+;

#include "GUI/Components/ConfFileSelector.h"
#pragma link C++ class ConfFileSelector+;

#include "GUI/Components/RunNumberPanel.h"
#pragma link C++ class RunNumberPanel+;

#include "GUI/Components/DAQManager.h"
#pragma link C++ class DAQManager+;

#include "DAQMonitor/DataRunOperations.h"
#pragma link C++ namespace DataRun+;
#pragma link C++ defined_in DataRun+;

#include "SignalHandlers.h"
#pragma link C++ namespace SigHandlers+;
#pragma link C++ defined_in SigHandlers+;

#endif