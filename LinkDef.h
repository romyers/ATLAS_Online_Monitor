/**
 * @file LinkDef.h
 * 
 * @brief Link definitions for the DAQMonitor library. ROOT uses this file to
 * generate dictionaries for classes using signals and slots.
 * 
 * When defining a class that uses signals and slots:
 * 	1. Add a ClassDef macro to the class definition in the header file, like
 * 	   "ClassDef(MyClass, 0);".
 * 	2. Add a ClassImp macro to the source file, like "ClassImp(MyClass);".
 * 	3. Link the class in this file like "#pragma link C++ class MyClass+;".
 *  4. Include the class header in the root_generate_dictionary command found
 *     in the CMakeLists.txt file.
 * 
 * If you don't do all of these steps, you'll get messages like:
 * "Error in <TQObject::CheckConnectArgs>: slot myslot() does not exist"
 * when you try to connect signals and slots, even if the slot does in fact
 * exist.
 * 
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

// NOTE: We do NOT want to include any headers here. If we do, dictionary
//       generation will insert them into the dictionary source file, and
//       they won't be found due to path conflicts. We'll let the dictionary
//       generator take care of including the headers.

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#pragma link C++ class DAQ::MonitorView+;

#pragma link C++ class DAQ::App+;

#pragma link C++ class DAQ::FileExplorer+;

#pragma link C++ class DAQ::ControlPanel+;

#pragma link C++ class DAQ::SettingsPanel+;

#pragma link C++ class DAQ::TabPanel+;

#endif