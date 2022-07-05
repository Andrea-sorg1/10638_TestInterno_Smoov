//---------------------------------------------------------------------------
#ifndef Batwin
#define Batwin
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
//---------------------------------------------------------------------------
#include "Tecnofer.h"
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TFormCambioBatteria : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel3;
  //------------------- bottoni scorrimento
  TBitBtn *BtnCerca;
  TBitBtn *BtnCopia;
  TBitBtn *BtnInserimento;
  TBitBtn *BtnStampa;
  TBitBtn *BtnCancella;
  TBitBtn *BtnSalva;
  TBitBtn *BtnEsci;
  TBitBtn *BtnIndietro;
  TBitBtn *BtnAvanti;
  TTimer *TimerClose;
  TPanel *PanelCBatt;
  TPanel *EditPanel;
  TPanel *NRecordPanel;
  TStaticText *txtlNum;
  TMaskEdit *EdNum;
  TEdit *edSigla;
  TStaticText *txtNodo;
  TMaskEdit *EdNod;
  TGroupBox *GBPlc;
  TCheckBox *CBBlocco;
  TCheckBox *CBTgvIng;
  TCheckBox *CBOkIng;
  TCheckBox *CBRicIng;
  TBevel *Bevel5;
  TMaskEdit *EdPlc;
  TStaticText *txtIDPLC;
  TCheckBox *CBAutomatico;
  TGroupBox *GBStatoB;
  TCheckBox *CBEsclM;
  TCheckBox *CBEsclCom;
  TGroupBox *GroupBox1;
  TPrintDialog *PrintDialog;
  TListView *ListView;
  TBitBtn *btnRefresh;
  TBitBtn *BtnRiservato;
  TComboBox *cbGruppo;
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall BtnCercaClick(TObject *Sender);
  void __fastcall BtnCopiaClick(TObject *Sender);
  void __fastcall BtnSalvaClick(TObject *Sender);
  void __fastcall BtnStampaClick(TObject *Sender);
  void __fastcall BtnCancellaClick(TObject *Sender);
  void __fastcall BtnInserimentoClick(TObject *Sender);
  void __fastcall BtnAvantiClick(TObject *Sender);
  void __fastcall BtnIndietroClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall AzzeraTimerClick(TObject *Sender);
  void __fastcall TimerCloseTimer(TObject *Sender);
  void __fastcall AzzeraTimerMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
  void __fastcall TxtPuntiAuxClick(TObject *Sender);
  void __fastcall ListViewCustomDrawItem(TCustomListView *Sender, TListItem *Item, TCustomDrawState State, bool &DefaultDraw);
  void __fastcall ListViewCustomDrawSubItem(TCustomListView *Sender, TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw);

private:	// User declarations

  void __fastcall Gruppi();
  void __fastcall Messaggi();
  void __fastcall GetCaselle();
  void __fastcall SetCaselle();
  void __fastcall LeggiDati();
  void __fastcall Avanti();
  void __fastcall Indietro();
  void __fastcall Cerca();
  void __fastcall Copia();
  void __fastcall Salva();
  void __fastcall Inser();
  void __fastcall Cancel();
  void __fastcall CompilaAllarmi();
  void __fastcall TFormCambioBatteria::AlternateRowColor(TCustomListView *Sender, TListItem *Item);

  //------------------------
  // variabili di editor
  //------------------------
  bool Inserimento;
  int  NumRec;
  short int IndiceCBatt;

public:		// User declarations
  short int NumCBatt;

  __fastcall TFormCambioBatteria(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormCambioBatteria *FormCambioBatteria;
//---------------------------------------------------------------------------
#endif;
