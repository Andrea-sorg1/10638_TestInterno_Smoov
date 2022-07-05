//---------------------------------------------------------------------------
#ifndef MagwinH
#define MagwinH
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
class TFormMagaz : public TForm
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
  TPanel *PanMagaz;
  TBevel *Bevel1;
  TPanel *EditPanel;
  TPanel *NRecordPanel;
  TStaticText *TextNum;
  TMaskEdit *EdNum;
  TTimer *TimerClose;
  TCheckBox *CBFifo;
  TMaskEdit *EdNpt;
  TLabel *TextNpt;
  TLabel *TextCest;
  TMaskEdit *EdNCest;
  TLabel *TextSovrap;
  TMaskEdit *EdSovrap;
  TStaticText *TextDat;
  TStaticText *TextOra;
  TMaskEdit *EdOra;
  TMaskEdit *EdData;
  TRadioGroup *GBStato;
  TCheckBox *CBCompleto;
  TStaticText *TextNod1;
  TMaskEdit *EdNodIng;
  TStaticText *TextNod2;
  TMaskEdit *EdNodOut;
  TMaskEdit *EdProfond;
  TLabel *lblProfondita;
  TMaskEdit *EdBaAlta;
  TLabel *lblAltezza;
  TMaskEdit *EdOffNod;
  TLabel *lblOffsetNode;
  TGroupBox *GBAssegnaz;
  TComboBox *CBAsseg;
  TGroupBox *GBPosizioni;
  TPageControl *PageControlMag;
  TTabSheet *TabPos1;
  TTabSheet *TabPos2;
  TTabSheet *TabPos3;
  TTabSheet *TabPos4;
  TTabSheet *TabPos5;
  TTabSheet *TabPos6;
  TTabSheet *TabPos7;
  TTabSheet *TabPos8;
  TTabSheet *TabPos9;
  TTabSheet *TabPos10;
  TTabSheet *TabPos11;
  TTabSheet *TabPos12;
  TTabSheet *TabPos13;
  TTabSheet *TabPos14;
  TTabSheet *TabPos15;
  TTabSheet *TabPos16;
  TTabSheet *TabPos17;
  TTabSheet *TabPos18;
  TTabSheet *TabPos19;
  TTabSheet *TabPos20;
  TTabSheet *TabPos21;
  TTabSheet *TabPos22;
  TTabSheet *TabPos23;
  TTabSheet *TabPos24;
  TTabSheet *TabPos25;
  TTabSheet *TabPos26;
  TTabSheet *TabPos27;
  TTabSheet *TabPos28;
  TTabSheet *TabPos29;
  TTabSheet *TabPos30;
  TTabSheet *TabPos31;
  TTabSheet *TabPos32;
  TTabSheet *TabPos33;
  TTabSheet *TabPos34;
  TTabSheet *TabPos35;
  TTabSheet *TabPos36;
  TTabSheet *TabPos37;
  TTabSheet *TabPos38;
  TTabSheet *TabPos39;
  TTabSheet *TabPos40;
  TGroupBox *GBPiano0;
  TBitBtn *BtnBox0;
  TStaticText *TxQP0_1;
  TStaticText *TxQP0_2;
  TGroupBox *GBPiano1;
  TBitBtn *BtnBox1;
  TStaticText *TextNodP;
  TMaskEdit *EdNodP1;
  TRadioGroup *GBTipo;
  TGroupBox *GBPiano2;
  TBitBtn *BtnBox2;
  TStaticText *TextMaxPianoP;
  TMaskEdit *MaskEdit1;
  TMaskEdit *edMaxPianiP;
  TBevel *Bevel2;
  TLabel *lblSollevam;
  TMaskEdit *EdSoll;
  TRadioButton *RBRotTutti;
  TRadioButton *RBSoloRuotati;
  TRadioButton *RBNoRuotati;
  TGroupBox *GBConfigDeposito;
  TComboBox *CBGruppo;
  TComboBox *CBTipoBox;
  TStaticText *TxQH1;
  TStaticText *TxQH2;
  TPrintDialog *PrintDialog;
  TMaskEdit *EdNodP2;
  TPanel *pnlPres2;
  TCheckBox *CBPres2;
  TPanel *pnlPres1;
  TCheckBox *CBPres1;
  TPanel *pnlPres0;
  TCheckBox *CBPres0;
  TBevel *Bevel3;
  TRadioGroup *RGCarWash;
  TStaticText *TxtPalRetrival;
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
  void __fastcall AzzeraTimerMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall PageControlMagChange(TObject *Sender);
  void __fastcall CBPres0Click(TObject *Sender);
  void __fastcall BtnBoxClick(TObject *Sender);
  void __fastcall CBPres1Click(TObject *Sender);
  void __fastcall CBAssegChange(TObject *Sender);
  void __fastcall TxQP0_2DblClick(TObject *Sender);
  void __fastcall CBPres2Click(TObject *Sender);
  void __fastcall TxQHClick(TObject *Sender);

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

  //------------------------
  // variabili di editor
  //------------------------
  TCheckBox *CBPres[3];

  bool Inserimento;
  int  DatiBoxModificati;

  int  NumRec;
  short int IndiceMagazzino;
  short int IndicePosizione;
  short int NumMagAccoppiato;
  short int NumMagTemporaneo;

public:		// User declarations

  short int NumeroMagazzino;
  short int NumeroPosizione;

  __fastcall TFormMagaz(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMagaz *FormMagaz;
//---------------------------------------------------------------------------
#endif
