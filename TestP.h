//---------------------------------------------------------------------------
#ifndef TestPH
#define TestPH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
//---------------------------------------------------------------------------
#include "TecnoFer.h"
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TFormTest : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *TestBox;
  TPanel *BtnTestPanel;
  TSpeedButton *BtnStampa;
  TBitBtn *BtnTest;
  TStaticText *TextInizio;
  TMaskEdit *EditInizio;
  TListBox *TestListBox;
  TEdit *EditTestSpeciali;
  TStaticText *TextBlocco;
  TRadioGroup *RGTipo;
  TGroupBox *CompilaBox;
  TButton *BtnCompTutto;
  TButton *BtnCompSpec;
  TRadioButton *RGAttivaTest;
  TRadioButton *RGAttivaCompila;
  TButton *BtnCompMastSlave;
  TGroupBox *GroupBox1;
  TCheckBox *TestEsclZp1;
  TCheckBox *TestEsclZp2;
  TCheckBox *TestEsclZp4;
  TCheckBox *TestEsclZp3;
  TCheckBox *TestEsclZp5;
  TCheckBox *TestEsclZp6;
  TCheckBox *TestEsclZp7;
  TCheckBox *TestEsclZp8;
  TCheckBox *TestEsclZp9;
  TCheckBox *TestEsclZp10;
  TCheckBox *TestEsclZp11;
  TCheckBox *TestEsclZp12;
  TCheckBox *TestEsclZp14;
  TCheckBox *TestEsclZp13;
  TCheckBox *TestEsclZp15;
  TCheckBox *TestEsclZp16;
  TCheckBox *TestEsclZp17;
  TCheckBox *TestEsclZp18;
  TCheckBox *TestEsclZp19;
  TCheckBox *TestEsclusioneNodiSlave;
  TCheckBox *TestEsclZp0;
  TMaskEdit *EditAgv;
  TStaticText *StaticText1;
  TProgressBar *ProgressBar;
  TLabel *Label;
  TCheckBox *TestEsclusioneStartNodiPerc;
  TPrintDialog *PrintDialog;
  void __fastcall BtnTestClick(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall BtnCompSpecClick(TObject *Sender);
  void __fastcall BtnCompTuttoClick(TObject *Sender);
  void __fastcall RGAttivaTestClick(TObject *Sender);
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall RGAttivaCompilaClick(TObject *Sender);
  void __fastcall BtnCompMastSlaveClick(TObject *Sender);
  void __fastcall BtnStampaClick(TObject *Sender);
private:	// User declarations

  void Test(TObject *Sender, short int Inizio);

  int  CalcolaNum2(int i, int num0, int opt);
  void CalcolaRallentamento(int i, int n0, int n2, int pt0, int ps0, struct NOD *nod);
  void CalcolaDirezione(int i, int n0, int n1, struct NOD *nod);
  void CalcolaCaricoScarico(int i, int n0, struct NOD *nod);
  bool TestEsclZp[MAXZP+2];

  int    pt0[MAXPUNTINODO],
         zs0[MAXPUNTINODO],
         zp0[MAXPUNTINODO],
         ps0[MAXPUNTINODO];

  int    pt1[MAXPUNTINODO],
         zs1[MAXPUNTINODO],
         zp1[MAXPUNTINODO],
         ps1[MAXPUNTINODO];

public:		// User declarations
  __fastcall TFormTest(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormTest *FormTest;
//---------------------------------------------------------------------------
#endif
