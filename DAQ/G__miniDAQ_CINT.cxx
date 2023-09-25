// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME G__miniDAQ_CINT
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// The generated code does not explicitly qualifies STL entities
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "EthernetCap.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *PcapDev_Dictionary();
   static void PcapDev_TClassManip(TClass*);
   static void *new_PcapDev(void *p = 0);
   static void *newArray_PcapDev(Long_t size, void *p);
   static void delete_PcapDev(void *p);
   static void deleteArray_PcapDev(void *p);
   static void destruct_PcapDev(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PcapDev*)
   {
      ::PcapDev *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::PcapDev));
      static ::ROOT::TGenericClassInfo 
         instance("PcapDev", "EthernetCap.h", 8,
                  typeid(::PcapDev), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &PcapDev_Dictionary, isa_proxy, 4,
                  sizeof(::PcapDev) );
      instance.SetNew(&new_PcapDev);
      instance.SetNewArray(&newArray_PcapDev);
      instance.SetDelete(&delete_PcapDev);
      instance.SetDeleteArray(&deleteArray_PcapDev);
      instance.SetDestructor(&destruct_PcapDev);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PcapDev*)
   {
      return GenerateInitInstanceLocal((::PcapDev*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PcapDev*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *PcapDev_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::PcapDev*)0x0)->GetClass();
      PcapDev_TClassManip(theClass);
   return theClass;
   }

   static void PcapDev_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *EthernetCap_Dictionary();
   static void EthernetCap_TClassManip(TClass*);
   static void *new_EthernetCap(void *p = 0);
   static void *newArray_EthernetCap(Long_t size, void *p);
   static void delete_EthernetCap(void *p);
   static void deleteArray_EthernetCap(void *p);
   static void destruct_EthernetCap(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::EthernetCap*)
   {
      ::EthernetCap *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::EthernetCap));
      static ::ROOT::TGenericClassInfo 
         instance("EthernetCap", "EthernetCap.h", 19,
                  typeid(::EthernetCap), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &EthernetCap_Dictionary, isa_proxy, 4,
                  sizeof(::EthernetCap) );
      instance.SetNew(&new_EthernetCap);
      instance.SetNewArray(&newArray_EthernetCap);
      instance.SetDelete(&delete_EthernetCap);
      instance.SetDeleteArray(&deleteArray_EthernetCap);
      instance.SetDestructor(&destruct_EthernetCap);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::EthernetCap*)
   {
      return GenerateInitInstanceLocal((::EthernetCap*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::EthernetCap*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *EthernetCap_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::EthernetCap*)0x0)->GetClass();
      EthernetCap_TClassManip(theClass);
   return theClass;
   }

   static void EthernetCap_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_PcapDev(void *p) {
      return  p ? new(p) ::PcapDev : new ::PcapDev;
   }
   static void *newArray_PcapDev(Long_t nElements, void *p) {
      return p ? new(p) ::PcapDev[nElements] : new ::PcapDev[nElements];
   }
   // Wrapper around operator delete
   static void delete_PcapDev(void *p) {
      delete ((::PcapDev*)p);
   }
   static void deleteArray_PcapDev(void *p) {
      delete [] ((::PcapDev*)p);
   }
   static void destruct_PcapDev(void *p) {
      typedef ::PcapDev current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PcapDev

namespace ROOT {
   // Wrappers around operator new
   static void *new_EthernetCap(void *p) {
      return  p ? new(p) ::EthernetCap : new ::EthernetCap;
   }
   static void *newArray_EthernetCap(Long_t nElements, void *p) {
      return p ? new(p) ::EthernetCap[nElements] : new ::EthernetCap[nElements];
   }
   // Wrapper around operator delete
   static void delete_EthernetCap(void *p) {
      delete ((::EthernetCap*)p);
   }
   static void deleteArray_EthernetCap(void *p) {
      delete [] ((::EthernetCap*)p);
   }
   static void destruct_EthernetCap(void *p) {
      typedef ::EthernetCap current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::EthernetCap

namespace {
  void TriggerDictionaryInitialization_libminiDAQ_CINT_Impl() {
    static const char* headers[] = {
"EthernetCap.h",
0
    };
    static const char* includePaths[] = {
"/usr/ROOT/include",
"/home/muonuser/gitfolder/phase2_MiniDAQ/DAQ",
"/usr/ROOT/include",
"/home/muonuser/gitfolder/phase2_MiniDAQ/DAQ/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libminiDAQ_CINT dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$EthernetCap.h")))  PcapDev;
class __attribute__((annotate("$clingAutoload$EthernetCap.h")))  EthernetCap;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libminiDAQ_CINT dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "EthernetCap.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"EthernetCap", payloadCode, "@",
"PcapDev", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libminiDAQ_CINT",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libminiDAQ_CINT_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libminiDAQ_CINT_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libminiDAQ_CINT() {
  TriggerDictionaryInitialization_libminiDAQ_CINT_Impl();
}
