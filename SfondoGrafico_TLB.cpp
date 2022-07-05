// ************************************************************************ //
// WARNING                                                                  //
// -------                                                                  //
// The types declared in this file were generated from data read from a     //
// Type Library. If this type library is explicitly or indirectly (via      //
// another type library referring to this type library) re-imported, or the //
// 'Refresh' command of the Type Library Editor activated while editing the //
// Type Library, the contents of this file will be regenerated and all      //
// manual modifications will be lost.                                       //
// ************************************************************************ //

/* File generated on 07/08/2002 17.19.55 from Type Library described below. */

// ************************************************************************ //
// Type Lib: D:\CLIENTI\TecnoFerrari\Box2000\SfondoGrafico\SfondoGrafico.tlb
// IID\LCID: {1D6B6D04-5B71-42C8-B707-201D175DA364}\0
// Helpfile: D:\CLIENTI\TecnoFerrari\Box2000\SfondoGrafico\SfondoGrafico.tlb
// DepndLst: 
//   (1) v2.0 stdole, (C:\WINNT\System32\stdole2.tlb)
// ************************************************************************ //

#include <vcl.h>
#pragma hdrstop
#if defined(USING_ATLVCL)
#include <atl\atlvcl.h>
#endif

#include "SfondoGrafico_TLB.h"

#if !defined(__PRAGMA_PACKAGE_SMART_INIT)
#define      __PRAGMA_PACKAGE_SMART_INIT
#pragma package(smart_init)
#endif

namespace Sfondografico_tlb
{


// *********************************************************************//
// GUIDS declared in the TypeLibrary                                    //
// *********************************************************************//
extern "C" const GUID LIBID_SfondoGrafico = {0x1D6B6D04, 0x5B71, 0x42C8,{ 0xB7, 0x07, 0x20, 0x1D, 0x17, 0x5D, 0xA3, 0x64} };
extern "C" const GUID IID_ITSfondoGrafico = {0xD068D73D, 0xD19D, 0x4B44,{ 0xA9, 0xC4, 0x9C, 0xD8, 0x06, 0xE1, 0x3E, 0x77} };
extern "C" const GUID CLSID_TSfondoGrafico = {0xF7A9BCE9, 0x8582, 0x4C4C,{ 0x86, 0x49, 0xF1, 0x29, 0x80, 0x4C, 0xB7, 0x5D} };



// *********************************************************************//
// COCLASS DEFAULT INTERFACE CREATOR
// (The following methods provide an easy way to create an instance of
//  the default interface, ITSfondoGrafico, of the CoClass TSfondoGrafico)
// *********************************************************************//

HRESULT CoTSfondoGrafico::Create(TCOMITSfondoGrafico& _intf)
{
  static TInitOle __initializeOle;
  return CoCreateInstance(CLSID_TSfondoGrafico, IID_ITSfondoGrafico, (LPVOID*)&_intf);
};

TCOMITSfondoGrafico CoTSfondoGrafico::Create(void)
{
  TCOMITSfondoGrafico _intf;
  Create(_intf);
  return _intf;
};

HRESULT CoTSfondoGrafico::CreateRemote(LPWSTR machineName, TCOMITSfondoGrafico& _intf)
{
  static TInitOle __initializeOle;
  COSERVERINFO ServerInfo = { 0, machineName, NULL, 0 };
  MULTI_QI MQI = { &IID_ITSfondoGrafico, NULL, 0 };
  HRESULT _hr = ::CoCreateInstanceEx(CLSID_TSfondoGrafico, NULL,
                                     CLSCTX_LOCAL_SERVER|
                                     CLSCTX_REMOTE_SERVER|
                                     CLSCTX_INPROC_SERVER,
                                     &ServerInfo, 1, &MQI);
  if (SUCCEEDED(_hr))
    _intf = (ITSfondoGrafico*)MQI.pItf;
  return _hr;
};

TCOMITSfondoGrafico CoTSfondoGrafico::CreateRemote(LPWSTR machineName)
{
  TCOMITSfondoGrafico _intf;
  CreateRemote(machineName, _intf);
  return _intf;
};

};     // namespace Sfondografico_tlb
