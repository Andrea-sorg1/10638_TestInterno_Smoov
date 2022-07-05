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
#ifndef   __SfondoGrafico_TLB_h__
#define   __SfondoGrafico_TLB_h__

#pragma option push -b


#include <system.hpp>
#include <utilcls.h>
#include <stdvcl.hpp>
#include <ocxproxy.h>

namespace Sfondografico_tlb
{

// *********************************************************************//
// HelpString: Project1 Library
// Version:    1.0
// *********************************************************************//


// *********************************************************************//
// GUIDS declared in the TypeLibrary. Following prefixes are used:      //
//   Type Libraries     : LIBID_xxxx                                    //
//   CoClasses          : CLSID_xxxx                                    //
//   DISPInterfaces     : DIID_xxxx                                     //
//   Non-DISP interfaces: IID_xxxx                                      //
// *********************************************************************//
DEFINE_GUID(LIBID_SfondoGrafico, 0x1D6B6D04, 0x5B71, 0x42C8, 0xB7, 0x07, 0x20, 0x1D, 0x17, 0x5D, 0xA3, 0x64);
DEFINE_GUID(IID_ITSfondoGrafico, 0xD068D73D, 0xD19D, 0x4B44, 0xA9, 0xC4, 0x9C, 0xD8, 0x06, 0xE1, 0x3E, 0x77);
DEFINE_GUID(CLSID_TSfondoGrafico, 0xF7A9BCE9, 0x8582, 0x4C4C, 0x86, 0x49, 0xF1, 0x29, 0x80, 0x4C, 0xB7, 0x5D);

// *********************************************************************//
// Forward declaration of interfaces defined in Type Library            //
// *********************************************************************//
interface ITSfondoGrafico;
struct    BLOCCOGRAFICO;
struct    IMPIANTO;

// *********************************************************************//
// Declaration of CoClasses defined in Type Library                     //
// (NOTE: Here we map each CoClass to it's Default Interface            //
// *********************************************************************//
typedef ITSfondoGrafico TSfondoGrafico;

// *********************************************************************//
// Declaration of Structures and Unions defined in Type Library         //
// *********************************************************************//
struct BLOCCOGRAFICO
{
  int X;
  int Y;
  int LatoX;
  int LatoY;
  short Rotazione;
  int ColPen;
  int ColBrush;
  signed char Stringa[51];
  int ColStringa;
  short Tipo;
  signed char Selezionato;
  short Sviluppo;
  short Speciale;
};

struct IMPIANTO
{
  signed char Percorso[101];
};

// *********************************************************************//
// Interface: ITSfondoGrafico
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {D068D73D-D19D-4B44-A9C4-9CD806E13E77}
// *********************************************************************//
interface ITSfondoGrafico : public IDispatch
{
public:
  virtual HRESULT STDMETHODCALLTYPE DatiBlocco(Sfondografico_tlb::BLOCCOGRAFICO* Dati/*[in]*/) = 0; // [1]
  virtual HRESULT STDMETHODCALLTYPE DatiImpianto(Sfondografico_tlb::IMPIANTO* Dati/*[in]*/) = 0; // [3]
  virtual HRESULT STDMETHODCALLTYPE MouseLeft(int X/*[in]*/, int Y/*[in]*/) = 0; // [2]
  virtual HRESULT STDMETHODCALLTYPE MouseRight(int X/*[in]*/, int Y/*[in]*/) = 0; // [4]
};

// *********************************************************************//
// SmartIntf: TCOMITSfondoGrafico
// Interface: ITSfondoGrafico
// *********************************************************************//
class TCOMITSfondoGrafico : public TComInterface<ITSfondoGrafico>
{
public:
  TCOMITSfondoGrafico() {}
  TCOMITSfondoGrafico(ITSfondoGrafico *intf, bool addRef = false) : TComInterface<ITSfondoGrafico>(intf, addRef) {}
};

// *********************************************************************//
// DispIntf:  ITSfondoGrafico
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {D068D73D-D19D-4B44-A9C4-9CD806E13E77}
// *********************************************************************//
class ITSfondoGraficoDisp : public TAutoDriver<ITSfondoGrafico>
{
  typedef TDispId<ITSfondoGrafico> _TDispID;
public:

  ITSfondoGraficoDisp()
  {}

  ITSfondoGraficoDisp& operator=(ITSfondoGrafico *pintf)
  {
    TAutoDriver<ITSfondoGrafico>::Bind(pintf);
  }

  HRESULT BindDefault(/*Binds to CoClass TSfondoGrafico*/)
  {
    return OLECHECK(Bind(CLSID_TSfondoGrafico));
  }

  HRESULT /*[VT_HRESULT:0]*/  DatiBlocco(/*AUTO_PARAM_ERROR(Sfondografico_tlb::BLOCCOGRAFICO* Dati)*/ Variant&  Dati)
  {
    static _TDispID _dispid(*this, OLETEXT("DatiBlocco"), DISPID(1));
    TAutoArgs<1> _args;
    _args[1] = Dati /*[VT_USERDEFINED:1]*/;
    return OleFunction(_dispid, _args);
  }

  HRESULT /*[VT_HRESULT:0]*/  DatiImpianto(/*AUTO_PARAM_ERROR(Sfondografico_tlb::IMPIANTO* Dati)*/ Variant&  Dati)
  {
    static _TDispID _dispid(*this, OLETEXT("DatiImpianto"), DISPID(3));
    TAutoArgs<1> _args;
    _args[1] = Dati /*[VT_USERDEFINED:1]*/;
    return OleFunction(_dispid, _args);
  }

  HRESULT /*[VT_HRESULT:0]*/  MouseLeft(int X/*[in]*/,int Y/*[in]*/)
  {
    static _TDispID _dispid(*this, OLETEXT("MouseLeft"), DISPID(2));
    TAutoArgs<2> _args;
    _args[1] = X /*[VT_INT:0]*/;
    _args[2] = Y /*[VT_INT:0]*/;
    return OleFunction(_dispid, _args);
  }

  HRESULT /*[VT_HRESULT:0]*/  MouseRight(int X/*[in]*/,int Y/*[in]*/)
  {
    static _TDispID _dispid(*this, OLETEXT("MouseRight"), DISPID(4));
    TAutoArgs<2> _args;
    _args[1] = X /*[VT_INT:0]*/;
    _args[2] = Y /*[VT_INT:0]*/;
    return OleFunction(_dispid, _args);
  }

};


// *********************************************************************//
// COCLASS DEFAULT INTERFACE CREATOR
// CoClass  : TSfondoGrafico
// Interface: TCOMITSfondoGrafico
// *********************************************************************//
class CoTSfondoGrafico : public CoClassCreator
{
public:
  static TCOMITSfondoGrafico Create(void);
  static TCOMITSfondoGrafico CreateRemote(LPWSTR machineName);

  static HRESULT Create(TCOMITSfondoGrafico& defIntfObj);
  static HRESULT CreateRemote(LPWSTR machineName, TCOMITSfondoGrafico& defIntfObj);
};

};     // namespace Sfondografico_tlb

#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using  namespace Sfondografico_tlb;
#endif

#pragma option pop

#endif // __SfondoGrafico_TLB_h__
