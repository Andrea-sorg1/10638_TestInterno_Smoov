//---------------------------------------------------------------------------
#ifndef CercaCodH
#define CercaCodH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <DBTables.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "Tecnofer.h"
//---------------------------------------------------------------------------
class TFormCercaCod : public TForm
{
__published:	// IDE-managed Components
  TEdit *Edit1;
  TEdit *Edit2;
  TButton *BtnOk;
  TButton *BtnCancel;
  TStringGrid *CercaGrid;
  TRadioGroup *RGOrdine;
  void __fastcall Edit1Change(TObject *Sender);
  void __fastcall Edit2Change(TObject *Sender);
  void __fastcall CercaGridClick(TObject *Sender);
  
  void __fastcall Edit1Enter(TObject *Sender);
  void __fastcall Edit1Exit(TObject *Sender);
  void __fastcall Edit2Exit(TObject *Sender);
  void __fastcall Edit2Enter(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall BtnOkClick(TObject *Sender);
  
  void __fastcall CercaGridDblClick(TObject *Sender);
  void __fastcall RGOrdineClick(TObject *Sender);
private:	// User declarations

  bool EditCod;
  bool EditDes;

public:		// User declarations

  short int TipoPRD;   // 0 - Codici semilavorati
                       // 1 - Codici prodotti finiti

  struct DATI_PRD CercaPRD;

  __fastcall TFormCercaCod(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormCercaCod *FormCercaCod;
//---------------------------------------------------------------------------
#endif
