//---------------------------------------------------------------------------
#ifndef AgvwinH
#define AgvwinH
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
#include "Libreria.h"
#include "TecnoFer.h"
#include <Dialogs.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TFormTgv : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel1;
  TGroupBox *GroupBoxTgv3;
  TCheckBox *BoxEsclusioneTgv1;
  TRadioGroup *GroupBoxTgv2;
  TCheckBox *BoxEsclusioneTgv2;
  TCheckBox *BoxEsclusioneTgv3;
  TRadioGroup *GroupBoxTgv5;
  TGroupBox *GroupBoxTgv1;
  TStaticText *TextTgv1;
  TMaskEdit *EditTgv1;
  TStaticText *TextTgv4;
  TStaticText *TextTgv5;
  TPanel *Panel2;
  TBitBtn *BtnReset;
  TBitBtn *BtnSalva;
  TBitBtn *BtnCarica;
  TCheckBox *BoxEsclusioneTgv4;
  TCheckBox *BoxStopTgv;
  TSpeedButton *BtnColore;
  TColorDialog *ColorDialog1;
  TStaticText *TextTgv2;
  TMaskEdit *EditTgv2;
  TEdit *EditRap;
  TBitBtn *BtnEsci;
  TBitBtn *BtnAsservimento;
  TStaticText *TextTgv6;
  TStaticText *TextTgv7;
  TGroupBox *GroupBoxTgv6;
  TBevel *Bevel3;
  TStaticText *TextGrd;
  TStaticText *TextTgv3;
  TMaskEdit *EditTgv3;
  TGroupBox *GBBox_A;
  TTimer *TimerClose;
  TGroupBox *GBBox_B;
  TMaskEdit *EdProv;
  TStaticText *StaticText1;
  TStaticText *TextBloccati;
  TBitBtn *BtnMapping;
  TBitBtn *BtnBoxA;
  TBitBtn *BtnBoxB;
  TComboBox *CBPresA;
  TComboBox *CBPresB;
  TPanel *pnlChangeTGV;
  TImage *imgTgv;
  TShape *tgv_color;
  TBitBtn *btnTgvUP;
  TBitBtn *btnTgvDN;
  TStaticText *txtTgv;
  TPageControl *PageControl1;
  TTabSheet *TabASRV;
  TTabSheet *TabTGV;
  TGroupBox *gbTGV;
  TCheckBox *BoxStatoTgv1;
  TCheckBox *BoxStatoTgv2;
  TCheckBox *BoxStatoTgv3;
  TCheckBox *BoxStatoTgv4;
  TCheckBox *BoxStatoTgv5;
  TCheckBox *BoxStatoTgv6;
  TCheckBox *BoxStatoTgv7;
  TCheckBox *BoxStatoTgv8;
  TCheckBox *BoxStatoTgv9;
  TCheckBox *BoxStatoTgv10;
  TCheckBox *BoxStatoTgv11;
  TCheckBox *BoxStatoTgv12;
  TCheckBox *BoxStatoTgv13;
  TCheckBox *BoxStatoTgv14;
  TCheckBox *BoxStatoTgv15;
  TCheckBox *BoxStatoTgv16;
  TCheckBox *BoxStatoTgv17;
  TCheckBox *BoxStatoTgv18;
  TCheckBox *BoxStatoTgv19;
  TCheckBox *BoxStatoTgv20;
  TGroupBox *gbASRV;
  TCheckBox *BoxStatoAsrv1;
  TCheckBox *BoxStatoAsrv2;
  TCheckBox *BoxStatoAsrv3;
  TCheckBox *BoxStatoAsrv4;
  TCheckBox *BoxStatoAsrv5;
  TCheckBox *BoxStatoAsrv6;
  TCheckBox *BoxStatoAsrv7;
  TCheckBox *BoxStatoAsrv8;
  TCheckBox *BoxStatoAsrv9;
  TCheckBox *BoxStatoAsrv10;
  TCheckBox *BoxStatoAsrv11;
  TCheckBox *BoxStatoAsrv12;
  TCheckBox *BoxStatoAsrv13;
  TCheckBox *BoxStatoAsrv14;
  TCheckBox *BoxStatoAsrv15;
  TCheckBox *BoxStatoAsrv16;
  TCheckBox *BoxStatoAsrv17;
  TCheckBox *BoxStatoAsrv18;
  TCheckBox *BoxStatoAsrv19;
  TCheckBox *BoxStatoAsrv20;
  TCheckBox *BoxStatoAsrv21;
  TCheckBox *BoxStatoAsrv22;
  TCheckBox *BoxStatoAsrv23;
  TCheckBox *BoxStatoAsrv24;
  TCheckBox *BoxStatoAsrv25;
  TCheckBox *BoxStatoAsrv26;
  TCheckBox *BoxStatoAsrv27;
  TCheckBox *BoxStatoAsrv28;
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall BtnResetClick(TObject *Sender);
  void __fastcall BtnSalvaClick(TObject *Sender);
  void __fastcall BtnCaricaClick(TObject *Sender);
  void __fastcall BoxStopTgvClick(TObject *Sender);
  void __fastcall BtnColoreClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall TextGrdDblClick(TObject *Sender);
  void __fastcall BtnAsservimentoClick(TObject *Sender);
  void __fastcall TimerCloseTimer(TObject *Sender);
  void __fastcall AzzeraTimerClick(TObject *Sender);
  void __fastcall AzzeraTimerMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall OffDaNodoDblClick(TObject *Sender);
  void __fastcall BtnMappingClick(TObject *Sender);
  void __fastcall BtnBoxAClick(TObject *Sender);
  void __fastcall BtnBoxBClick(TObject *Sender);
  void __fastcall CBPresBChange(TObject *Sender);
  void __fastcall CBPresAChange(TObject *Sender);
  void __fastcall btnTgvUpDnClick(TObject *Sender);

private:	// User declarations
  void __fastcall Messaggi();
  void __fastcall GetCaselle();
  void __fastcall SetCaselle();
  void __fastcall LeggiDati();
  void __fastcall Carica();
  void __fastcall Reset();
  void __fastcall Salva();

  short int NumTgv;
  short int NewPos;
  short int OldPos;
  short int myTgv;
  short int NewPresCEST_A;
  short int OldPresCEST_A;
  short int NewPresCEST_B;
  short int OldPresCEST_B;

  // dati caricati a bordo TGV
  struct BOX EdBoxA;
  struct BOX EdBoxB;

  public:		// User declarations

  short int NumeroTgv;
  
  short int EditorAgvAttivo;                       // Flag di editor AGV attivo
                                                   // viene inibito la conclusione
                                                   // missione carico\scarico
  __fastcall TFormTgv(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormTgv *FormTgv;
//---------------------------------------------------------------------------
#endif
