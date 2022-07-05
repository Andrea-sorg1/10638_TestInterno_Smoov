//---------------------------------------------------------------------------
#ifndef BaiwinH
#define BaiwinH
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
class TFormBaie : public TForm
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
  TPanel *PanBaia;
  TPanel *EditPanel;
  TPanel *NRecordPanel;
  TStaticText *TXNum;
  TMaskEdit *EdNum;
  TRadioGroup *GBTipo;
  TEdit *EdSig;
  TStaticText *TXUrg;
  TMaskEdit *EdUrg;
  TLabel *lblPriorita;
  TStaticText *TextPriorita;
  TStaticText *TXNodo;
  TMaskEdit *EdNod;
  TGroupBox *GBPlc;
  TCheckBox *CBBlocco;
  TCheckBox *CBTuttoPieno;
  TCheckBox *CBFineOp;
  TCheckBox *CBTgvIng;
  TCheckBox *CBOkIng;
  TCheckBox *CBRicIng;
  TBevel *Bevel5;
  TMaskEdit *EdPlc;
  TStaticText *lblIDPLC;
  TCheckBox *CBOkForcAlte;
  TGroupBox *RGDati;
  TStaticText *lblGruppo;
  TComboBox *CBGruppo;
  TCheckBox *CBChiamata;
  TPageControl *PCDatiPal;
  TComboBox *CBStato;
  TStaticText *lblStato;
  TStaticText *lblPallet;
  TMaskEdit *EdCodPal;
  TStaticText *lblCodice;
  TCheckBox *CBBasso;
  TCheckBox *CBSovrap;
  TStaticText *lblAltezza;
  TMaskEdit *EdAltPal;
  TCheckBox *CBForcB;
  TCheckBox *CBForcA;
  TGroupBox *gbPresa;
  TTabSheet *TSPos1;
  TCheckBox *CBAutomatico;
  TTabSheet *TSPos2;
  TTabSheet *TSPos3;
  TTabSheet *TSPos4;
  TTabSheet *TSPos5;
  TCheckBox *CBPos1A;
  TCheckBox *CBPos2A;
  TCheckBox *CBPos3A;
  TCheckBox *CBPos3B;
  TCheckBox *CBPos2B;
  TCheckBox *CBPos1B;
  TCheckBox *CBCarW;
  TStaticText *TxtStPresenze;
  TGroupBox *GBStatoBaia;
  TCheckBox *CBEsclM;
  TCheckBox *CBEsclCom;
  TCheckBox *CBNoMagaz;
  TCheckBox *CBNoTgvDisp;
  TCheckBox *CBBlSovrap;
  TStaticText *TxtPuntiAux;
  TStaticText *lblLotto;
  TMaskEdit *EdLotPal;
  TCheckBox *CBRuotato;
  TComboBox *CBTipPal;
  TPrintDialog *PrintDialog;
  TComboBox *CBGruppoDest;
  TStaticText *lblSollev;
  TMaskEdit *EdSollev;
  TMaskEdit *EdAltez;
  TStaticText *TXAltez;
  TLabel *lblPresCestA;
  TLabel *lblPresCestB;
  TBitBtn *BtnBoxA;
  TBitBtn *BtnBoxB;
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
  void __fastcall PCDatiPalChange(TObject *Sender);
  void __fastcall TxtPuntiAuxClick(TObject *Sender);
  void __fastcall CBStatoChange(TObject *Sender);
  void __fastcall BtnBoxClick(TObject *Sender);
  void __fastcall CBForcClick(TObject *Sender);

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
  bool Inserimento;
  int  NumRec;
  short int IndiceBaia;
  short int IndicePosBaia;
  // dati Box
  struct BOX EdBoxA;
  struct BOX EdBoxB;




  TCheckBox *CBPresa[11][11];

public:		// User declarations

  short int NumeroBaia;

  __fastcall TFormBaie(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormBaie *FormBaie;
//---------------------------------------------------------------------------
#endif;
