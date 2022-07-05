//---------------------------------------------------------------------------
#ifndef ElCar2H
#define ElCar2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
#include "Tecnofer.h"
//---------------------------------------------------------------------------
class TFormElBox : public TForm
{
__published:	// IDE-managed Components
  TStringGrid *GridElenco;
  TBitBtn *BtnOk;
  TBitBtn *BtnEsci;
  TBitBtn *BtnRESET;
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall BtnOkClick(TObject *Sender);
  void __fastcall BtnRESETClick(TObject *Sender);
private:	// User declarations

  void __fastcall GetCaselle();
  void __fastcall SetCaselle();

public:		// User declarations

  short int ElencoBox[MAXLISTABOX+1];

  __fastcall TFormElBox(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormElBox *FormElBox;
//---------------------------------------------------------------------------
#endif
