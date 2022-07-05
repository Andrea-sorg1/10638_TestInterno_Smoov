//---------------------------------------------------------------------------
#ifndef MacwinH
#define MacwinH
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
class TFormMacchine : public TForm
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
  TPanel *PanMacch;
  TPanel *EditPanel;
  TPanel *NRecordPanel;
  TStaticText *TXNum;
  TMaskEdit *EdNum;
  TRadioGroup *GBTipo;
  TEdit *EdSig;
  TGroupBox *GBMagAbb;
  TMaskEdit *EdMagP1;
  TStaticText *StaticText5;
  TMaskEdit *EdMagP2;
  TStaticText *StaticText2;
  TMaskEdit *EdMagP3;
  TStaticText *StaticText4;
  TMaskEdit *EdMagP4;
  TStaticText *StaticText6;
  TMaskEdit *EdMagP5;
  TStaticText *StaticText7;
  TMaskEdit *EdMagP6;
  TStaticText *StaticText8;
  TMaskEdit *EdMagP7;
  TStaticText *StaticText9;
  TMaskEdit *EdMagP8;
  TStaticText *StaticText10;
  TMaskEdit *EdMagP9;
  TStaticText *StaticText11;
  TMaskEdit *EdMagP10;
  TStaticText *StaticText12;
  TMaskEdit *EdMagP11;
  TStaticText *StaticText13;
  TMaskEdit *EdMagP12;
  TStaticText *StaticText14;
  TMaskEdit *EdMagP13;
  TStaticText *StaticText15;
  TMaskEdit *EdMagP14;
  TStaticText *StaticText16;
  TMaskEdit *EdMagP15;
  TStaticText *StaticText17;
  TLabel *LbPieni;
  TMaskEdit *EdMagV1;
  TMaskEdit *EdMagV2;
  TMaskEdit *EdMagV3;
  TMaskEdit *EdMagV4;
  TMaskEdit *EdMagV5;
  TMaskEdit *EdMagV6;
  TMaskEdit *EdMagV7;
  TMaskEdit *EdMagV8;
  TMaskEdit *EdMagV9;
  TMaskEdit *EdMagV10;
  TMaskEdit *EdMagV11;
  TMaskEdit *EdMagV12;
  TMaskEdit *EdMagV13;
  TMaskEdit *EdMagV14;
  TMaskEdit *EdMagV15;
  TLabel *LbVuoti;
  TGroupBox *GBDatiS;
  TPageControl *PageControlMac;
  TTabSheet *TabStaz1;
  TTabSheet *TabStaz2;
  TTabSheet *TabStaz3;
  TTabSheet *TabStaz4;
  TTabSheet *TabStaz5;
  TTabSheet *TabStaz6;
  TTabSheet *TabStaz7;
  TTabSheet *TabStaz8;
  TTabSheet *TabStaz9;
  TTabSheet *TabStaz10;
  TMaskEdit *EdNod1;
  TStaticText *TXNod;
  TStaticText *TXUrg;
  TMaskEdit *EdUrgS;
  TGroupBox *GBPlcS;
  TGroupBox *GBConsS;
  TCheckBox *CBIngS;
  TCheckBox *CBOkForc;
  TCheckBox *CBRic;
  TPanel *PlcErrPanel;
  TStaticText *TXPrior;
  TMaskEdit *EdPrior;
  TGroupBox *GBPlcM;
  TCheckBox *CBAuto;
  TCheckBox *CBBloc;
  TGroupBox *GBConsM;
  TCheckBox *CBRicM;
  TCheckBox *CBOkM;
  TCheckBox *CBIngM;
  TCheckBox *CBEndM;
  TStaticText *TXCodM;
  TMaskEdit *EdCodM;
  TMaskEdit *EdNod2;
  TGroupBox *GBStatoS;
  TCheckBox *CBEsclS;
  TCheckBox *CBNoCod;
  TCheckBox *CBNoMag;
  TCheckBox *CBEsp;
  TCheckBox *CBRicS;
  TCheckBox *CBOkS;
  TCheckBox *CBEndS;
  TStaticText *TXCodS;
  TMaskEdit *EdCodS;
  TStaticText *TXStatBS;
  TGroupBox *CBComPLC;
  TBitBtn *BtnPLC;
  TMaskEdit *EdNpt;
  TStaticText *TXNpt;
  TMaskEdit *EdNumC;
  TGroupBox *GBDatiPos;
  TBitBtn *BtnBox0;
  TBitBtn *BtnBox1;
  TBitBtn *BtnBox2;
  TBitBtn *BtnBox3;
  TBitBtn *BtnBox4;
  TBitBtn *BtnBox5;
  TBitBtn *BtnBox6;
  TBitBtn *BtnBox7;
  TMaskEdit *EDIdBox0;
  TMaskEdit *EDIdBox1;
  TMaskEdit *EDIdBox2;
  TMaskEdit *EDIdBox3;
  TMaskEdit *EDIdBox4;
  TMaskEdit *EDIdBox5;
  TMaskEdit *EDIdBox6;
  TMaskEdit *EDIdBox7;
  TCheckBox *CBPres0;
  TCheckBox *CBPres1;
  TCheckBox *CBPres2;
  TCheckBox *CBPres3;
  TCheckBox *CBPres4;
  TCheckBox *CBPres5;
  TCheckBox *CBPres6;
  TCheckBox *CBPres7;
  TStaticText *TXGrd;
  TStaticText *StaticText1;
  TMaskEdit *EdMagP16;
  TMaskEdit *EdMagV16;
  TMaskEdit *EdMagV17;
  TMaskEdit *EdMagV18;
  TMaskEdit *EdMagV19;
  TMaskEdit *EdMagP20;
  TMaskEdit *EdMagP19;
  TMaskEdit *EdMagP18;
  TMaskEdit *EdMagP17;
  TStaticText *StaticText3;
  TStaticText *StaticText18;
  TStaticText *StaticText19;
  TStaticText *StaticText20;
  TMaskEdit *EdMagV20;
  TStaticText *TxtIDRic;
  TStaticText *TXBoxRic;
  TSpeedButton *BtnReload;
  TBitBtn *BtnListaBoxMan;
  TComboBox *CBStatoMac;
  TRadioButton *RGIngS;
  TRadioButton *RGOutS;
  TComboBox *CBStato;
  TComboBox *CBTipPal;
  TStaticText *TXTipoBS;
  TGroupBox *GBStatoM;
  TCheckBox *CBEsclM;
  TCheckBox *CBEsclCom;
  TCheckBox *CBFineProd;
  TPrintDialog *PrintDialog;
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
  void __fastcall PageControlMacChange(TObject *Sender);
  void __fastcall AzzeraTimerMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
  void __fastcall TXGrdDblClick(TObject *Sender);
  void __fastcall BtnPLCClick(TObject *Sender);
  void __fastcall BtnBoxClick(TObject *Sender);
  void __fastcall BtnListaBoxManClick(TObject *Sender);
  void __fastcall BtnReloadClick(TObject *Sender);
  void __fastcall CBStatoChange(TObject *Sender);
private:	// User declarations

  TMaskEdit *EdMagAbbP[MAXMAGABB+1];
  TMaskEdit *EdMagAbbV[MAXMAGABB+1];
  TCheckBox *CBPres[MAXPOSTISTAZ+1];
  TMaskEdit *EDIdBox[MAXPOSTISTAZ+1];
  TBitBtn   *BtnBox[MAXPOSTISTAZ+1];

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
  short int IndiceMacchina;
  short int IndiceStazione;
  short int IndicePosizione;

public:		// User declarations

  short int NumeroMacchina;
  short int NumeroStazione;
  short int NumeroPosizione;

  __fastcall TFormMacchine(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMacchine *FormMacchine;
//---------------------------------------------------------------------------
#endif
