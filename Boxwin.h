//---------------------------------------------------------------------------
#ifndef BoxwinH
#define BoxwinH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Mask.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
#include "TecnoFer.h"
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFormBox : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *GBDati;
  TMaskEdit *EdIdBox;
  TLabel *lblID;
  TComboBox *CBStatLav;
  TTimer *TimerClose;
  TPanel *NRecordPanel;
  TPanel *EditPanel;
  TGroupBox *RGDati;
  TStaticText *lblTipoPallet;
  TComboBox *CBTipPal;
  TCheckBox *CBBas;
  TCheckBox *CBSovrapp;
  TBevel *Bevel1;
  TCheckBox *CBCarW;
  TGroupBox *gbConfigPresa;
  TCheckBox *CBPos1B;
  TCheckBox *CBPos2B;
  TCheckBox *CBPos3B;
  TCheckBox *CBPos1A;
  TCheckBox *CBPos2A;
  TCheckBox *CBPos3A;
  TCheckBox *CBRot;
  TComboBox *CBGruppoSorg;
  TComboBox *CBGruppoDest;
  TLabel *Label1;
  TGroupBox *GBUdc;
  TPanel *PnBtn;
  TSpeedButton *BtnAdd;
  TSpeedButton *BtnDel;
  TSpeedButton *BtnFw;
  TSpeedButton *BtnRw;
  TBitBtn *BtnOK;
  TBitBtn *BtnEsci;
  TPageControl *PgCUDC;
  TTabSheet *Tab01;
  TTabSheet *Tab02;
  TTabSheet *Tab03;
  TTabSheet *Tab04;
  TTabSheet *Tab05;
  TTabSheet *Tab06;
  TStaticText *lblCod;
  TMaskEdit *EdCod;
  TStaticText *lblLot;
  TMaskEdit *EdLot;
  TStaticText *lblAlt;
  TMaskEdit *EdAltezza;
  TBevel *Bevel3;
  TPanel *prdPanel;
  TStaticText *txtSource;
  TEdit *edSize;
  TStaticText *txtSize;
  TStaticText *txtBatch;
  TEdit *edBatch;
  TStaticText *txtDest;
  TStaticText *txtQuantity;
  TEdit *edState;
  TStaticText *txtState;
  TStaticText *txtHeight;
  TStaticText *txtWeight;
  TStaticText *txtDesc;
  TEdit *edDesc;
  TEdit *edCode;
  TStaticText *txtCode;
  TStaticText *txtUDC;
  TEdit *edUDC;
  TStaticText *txtLocation;
  TMaskEdit *edLocation;
  TMaskEdit *edSource;
  TMaskEdit *edDest;
  TMaskEdit *edQty;
  TMaskEdit *edHeight;
  TMaskEdit *edWeight;
  TMaskEdit *EdData;
  TMaskEdit *EdOra;
  TStaticText *txtDate;
  TStaticText *txtTime;
  TStaticText *txtShift;
  TEdit *edShift;
  TStaticText *TxtLoc2;
  TStaticText *TxtLoc1;
  TStaticText *txtProv;
  TStaticText *txtTime1;
  TStaticText *txtTime2;
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall AzzeraTimerClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall BtnOKClick(TObject *Sender);
  void __fastcall CBBasClick(TObject *Sender);
  void __fastcall CBStatLavChange(TObject *Sender);
  void __fastcall PgCUDCChange(TObject *Sender);
  void __fastcall CBPosClick(TObject *Sender);
  void __fastcall TimerCloseTimer(TObject *Sender);

private:	// User declarations
  void __fastcall Gruppi();
  void __fastcall Messaggi();
  void __fastcall GetCaselle();
  void __fastcall SetCaselle();
  void __fastcall Salva();

  TCheckBox  *CBPresa[POSINPRESA+1][PIANIINPRESA+1];
  TTabSheet  *Tab[4][4];
  //------------------------
  // variabili di editor
  //------------------------
  bool Attivazione;
  int  MaxUDC;
  int  idUDC;

public:		// User declarations
  bool  Autoclose;
  bool  Saved;
  short int Tgv;
  short int Mac;
  short int Mag;
  short int Bai;
  short int Pos;
  short int Stz;
  short int Pin;
  short int Forc;
  bool SoloLettura;

  __fastcall TFormBox(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormBox *FormBox;
//---------------------------------------------------------------------------
#endif
