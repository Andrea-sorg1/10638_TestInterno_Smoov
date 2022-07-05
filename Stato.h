//---------------------------------------------------------------------------
#ifndef StatoH
#define StatoH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "Variabil.h"
#include <Graphics.hpp>

//---------------------------------------------------------------------------

class TFormStato : public TForm
{
__published:	// IDE-managed Components
  TPageControl *PageControlTGV;
  TTabSheet *TabTGV1;
  TTabSheet *TabTGV2;
  TTabSheet *TabTGV3;
  TTabSheet *TabTGV4;
  TTabSheet *TabTGV5;
  TTabSheet *TabTGV6;
  TTabSheet *TabTGV7;
  TTabSheet *TabTGV8;
  TTabSheet *TabTGV9;
  TGroupBox *GroupBox1;
  TPanel *Panel1;
  TSpeedButton *BtnEditor;
  TShape *ShColoreTGV;
  TGroupBox *GroupBox2;
  TStaticText *TextPos;
  TStaticText *TextPStart;
  TStaticText *TextPEnd;
  TStaticText *TextPInt;
  TLabel *Label1;
  TLabel *LabPStart;
  TLabel *LabPEnd;
  TLabel *Label4;
  TStaticText *TextStart;
  TLabel *LabStat2;
  TStaticText *TextFunz;
  TButton *BtnReset1;
  TLabel *LabStat3;
  TStaticText *TextChiamata;
  TSpeedButton *BtnPercorso;
  TSpeedButton *BtnProgrPLC;
  TSpeedButton *BtnMissioni;
  TSpeedButton *BtnCronoAll;
  TLabel *LabStat1;
  TBevel *Bevel3;
  TGroupBox *GroupBox3;
  TStaticText *TextAllarme;
  TPanel *Panel2;
  TImage *ImageLogo;
  TStaticText *TextMissioni;
  TStaticText *TextBatteria;
  TBevel *Bevel1;
  TLabel *LabStat4;
  TGroupBox *BoxPriorita;
  TLabel *LabStat5;
  TStaticText *TextZona;
  TStaticText *TextListAll;
  TLabel *LabGrd;
  TPanel *PanelScroll;
  TLabel *LabelScroll1;
  TLabel *LabelScroll2;
  TUpDown *UpDownScroll;
  TStaticText *TextPIntF;
  TLabel *Label5;
  TStaticText *TextPresa;
  TLabel *Label6;
  TLabel *Label7;
  TStaticText *TextAttesa;
  TLabel *LabPercSpec;
  TListBox *ListPrioritaMac;
  TLabel *LabGLaser;
  TListBox *ListPrioritaBaie;
  TTabSheet *TabTGV10;
  TTabSheet *TabTGV11;
  TTabSheet *TabTGV12;
  TTabSheet *TabTGV13;
  TTabSheet *TabTGV14;
  TTabSheet *TabTGV15;
  TTabSheet *TabTGV16;
  TTabSheet *TabTGV17;
  TTabSheet *TabTGV18;
  TTabSheet *TabTGV19;
  TTabSheet *TabTGV20;
  TTabSheet *TabTGV21;
  TBevel *Bevel2;
  TGroupBox *gbTools;
  TSpeedButton *btnLogView;
  TSpeedButton *SpeedButton2;
  TSpeedButton *SpeedButton3;
  TSpeedButton *SpeedButton4;
  TSpeedButton *btnSearchUDC;
  TSpeedButton *btnAttivaFunzSpec;
  TSpeedButton *btnGliphDown;
  TSpeedButton *btnGliphUp;
  TSpeedButton *btnDebug1;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall BtnEditorClick(TObject *Sender);

  void __fastcall PageControlTGVChange(TObject *Sender);

  void __fastcall BtnReset1Click(TObject *Sender);
  void __fastcall BtnPercorsoClick(TObject *Sender);
  void __fastcall BtnProgrPLCClick(TObject *Sender);
  void __fastcall BtnMissioniClick(TObject *Sender);
  void __fastcall BtnCronoAllClick(TObject *Sender);


  void __fastcall UpDownScrollClick(TObject *Sender, TUDBtnType Button);
  void __fastcall LabelScroll2Click(TObject *Sender);

  void __fastcall LabGrdDblClick(TObject *Sender);
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall TextChiamataDblClick(TObject *Sender);
  void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
  void __fastcall btnLogViewClick(TObject *Sender);
  void __fastcall btnSearchUDCClick(TObject *Sender);
  void __fastcall btnAttivaFunzSpecClick(TObject *Sender);
  void __fastcall LabGLaserDblClick(TObject *Sender);
  void __fastcall TextBatteriaDblClick(TObject *Sender);

private:	// User declarations
  int   count;
  bool  toggle;

public:		// User declarations
  TTabSheet *TabTGV[31];
  short int NumeroTGV;
  void __fastcall VisualizzaStatoTGV();
  void __fastcall Messaggi();

  __fastcall TFormStato(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormStato *FormStato;
//---------------------------------------------------------------------------
#endif
