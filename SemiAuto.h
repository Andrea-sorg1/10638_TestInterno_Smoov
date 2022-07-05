//---------------------------------------------------------------------------
#ifndef SemiAutoH
#define SemiAutoH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
#include "Tecnofer.h"
//---------------------------------------------------------------------------
class TFormSemiAuto : public TForm
{
__published:	// IDE-managed Components
  TRadioGroup *GroupStato2;
  TGroupBox *GroupStato1;
  TBevel *Bevel2;
  TGroupBox *GBPos;
  TMaskEdit *EditNodo;
  TLabel *Label2;
  TGroupBox *GBMac;
  TLabel *Label4;
  TBitBtn *BtnConferma;
  TBitBtn *BtnEsci;
  TGroupBox *GroupBox1;
  TLabel *Label1;
  TMaskEdit *EditTgv;
  TGroupBox *gbTipoOperazione;
  TRadioButton *RBPos;
  TRadioButton *RBMac;
  TRadioButton *RBMag;
  TGroupBox *GBMag;
  TLabel *Label6;
  TMaskEdit *EditMag;
  TRadioButton *RBForcheA;
  TRadioButton *RBForcheB;
  TRadioButton *RBForcheAB;
  TComboBox *EditComboMac;
  TRadioGroup *RGLatoMag;
  TGroupBox *GBBaia;
  TLabel *Label3;
  TMaskEdit *EditBaia;
  TRadioButton *RBBaia;
  TCheckBox *CBAttivaCicloContinuo;
  TBitBtn *BtnReset;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall RBTypMissClick(TObject *Sender);
  void __fastcall GroupStato2Click(TObject *Sender);
  void __fastcall BtnConfermaClick(TObject *Sender);
  void __fastcall BtnResetClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall EditTgvChange(TObject *Sender);

private:	// User declarations

  void __fastcall Gruppi();
  void __fastcall Messaggi();
  void __fastcall GetCaselle();
  void __fastcall SetCaselle();
  int __fastcall Conferma();

  //------------------------
  // variabili di editor
  //------------------------
  short int Indice;
  struct SEMIAUTOMATICO Dati[11];

public:		// User declarations
  __fastcall TFormSemiAuto(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormSemiAuto *FormSemiAuto;
//---------------------------------------------------------------------------
#endif
