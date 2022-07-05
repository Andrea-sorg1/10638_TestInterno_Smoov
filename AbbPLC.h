//---------------------------------------------------------------------------
#ifndef AbbPLCH
#define AbbPLCH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
//---------------------------------------------------------------------------
#include "Tecnofer.h"
//---------------------------------------------------------------------------
class TFormAbbPlc : public TForm
{
__published:	// IDE-managed Components
  TBitBtn *BtnOK;
  TBevel *Bevel1;
  TLabel *Label4;
  TMaskEdit *EditPlc;
  TGroupBox *GBForRD;
  TLabel *Label1;
  TLabel *Label2;
  TLabel *Label3;
  TMaskEdit *EditPlcRD1;
  TMaskEdit *EditPlcRD2;
  TMaskEdit *EditPlcRD3;
  TGroupBox *GBForWR;
  TLabel *Label5;
  TLabel *Label6;
  TLabel *Label7;
  TMaskEdit *EditPlcWR1;
  TMaskEdit *EditPlcWR2;
  TMaskEdit *EditPlcWR3;
  TTimer *TimerClose;
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall BtnOKClick(TObject *Sender);
  void __fastcall AzzeraTimerClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations

public:		// User declarations

  struct PARAMETRI_COM PlcData;

  __fastcall TFormAbbPlc(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormAbbPlc *FormAbbPlc;
//---------------------------------------------------------------------------
#endif
