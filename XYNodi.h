//---------------------------------------------------------------------------
#ifndef XYNodiH
#define XYNodiH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------

#include <checklst.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <CheckLst.hpp>
#include <Graphics.hpp>
#include <Mask.hpp>

//---------------------------------------------------------------------------
#include "TecnoFer.h"
//---------------------------------------------------------------------------
class TFormXYNodi : public TForm
{
__published:	// IDE-managed Components
  TPanel *PanelTotNodi;
  TGroupBox *GBXYNodi;
  TStaticText *TextNodi5;
  TStaticText *TextNodi6;
  TEdit *EditY;
  TEdit *EditX;
  TUpDown *udNX;
  TUpDown *udNY;
  TUpDown *udNX1;
  TCheckListBox *CLBNodi;
  TUpDown *udNY1;
  TSpeedButton *BtnOk;
  TSpeedButton *BtnAbort;
  TSpeedButton *BtnEsci;
  TImage *Image;
  TBevel *Bevel1;
  TStaticText *StaticText1;
  TEdit *EditXV;
  TUpDown *udVX;
  TUpDown *udVX1;
  TStaticText *StaticText2;
  TEdit *EditYV;
  TUpDown *udVY;
  TUpDown *udVY1;
  TGroupBox *GroupZoneMapping;
  TStaticText *StaticText7;
  TStaticText *TxtLegenda;
  TEdit *EditZ;
  TStaticText *stCurva;
  TUpDown *udCurva;
  TLabel *Label1;
  TSpeedButton *BtnZone;
  TCheckBox *chkbSendAllTGV;
  TGroupBox *gbIngombroAgvGrafico;
  TBevel *Bevel2;
  TLabel *Label3;
  TLabel *Label4;
  TLabel *Label18;
  TLabel *Label19;
  TMaskEdit *meRotazS;
  TMaskEdit *meAGVsel;
  TMaskEdit *meRotazE;
  TComboBox *cbDirezioneAgv;
  TLabel *Label2;
  TLabel *Label5;
  TLabel *Label6;
  
  void __fastcall BtnOkClick(TObject *Sender);
  
  void __fastcall CLBNodiClickCheck(TObject *Sender);
  void __fastcall BtnEsciClick(TObject *Sender);
  void __fastcall BtnAbortClick(TObject *Sender);
  void __fastcall udNX1Click(TObject *Sender, TUDBtnType Button);
  void __fastcall udNY1Click(TObject *Sender, TUDBtnType Button);
  void __fastcall udNXClick(TObject *Sender, TUDBtnType Button);
  void __fastcall udNYClick(TObject *Sender, TUDBtnType Button);
  void __fastcall EditXChange(TObject *Sender);
  void __fastcall EditYChange(TObject *Sender);
  void __fastcall udCurvaClick(TObject *Sender, TUDBtnType Button);
  void __fastcall udVX1Click(TObject *Sender, TUDBtnType Button);
  void __fastcall udVY1Click(TObject *Sender, TUDBtnType Button);
  void __fastcall udVXClick(TObject *Sender, TUDBtnType Button);
  void __fastcall udVYClick(TObject *Sender, TUDBtnType Button);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall EditYVChange(TObject *Sender);
  void __fastcall EditXVChange(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall stCurvaDblClick(TObject *Sender);

private:	// User declarations

  bool traslazione_vc;
  int  XY_Punti[MAXPUNTI][5];
  int  XY_PercorsoCurvaBezier[MAX_TRACCE_CURVA_DWG][7];

  //-------------------------------------------------
  // Data/Ora ultimo file GRAPHVAR.DAT
  //-------------------------------------------------
  long int TimeGraphvar;

  void __fastcall Abort();
  void __fastcall Aggiorna();

  //----------------------------------------
  // Traslazione GRAFICA
  //----------------------------------------
  int   TraslazioneGrafica( int X, int Y, char* all_mess);       // Da usare solo nei casi di traslazione del Layout
  void  WriteSfondoGrafico();
  void  TraslazioneCurveGrafica(int X, int Y);

public:		// User declarations

  int   CurvaSelezionata;
  short int ElencoNodi[MAXPUNTI+1][2];

  int    PuntoPerIngombro[MAXAGV+1];
  int    SegmentoPerIngombro[MAXAGV+1];
  int    CurvaPerIngombro[MAXAGV+1];
  int    DirezionePerIngombro[MAXAGV+1];

  int    RotazioneIngombroS[MAXAGV+1];
  int    RotazioneIngombroE[MAXAGV+1];

  void __fastcall Refresh(bool Open);
  void __fastcall InsertNodi(short int nodo);
  void __fastcall DeleteNodi(short int nodo);
  short int __fastcall CercaNodo(short int nodo);

  //-------------------------------------------------
  // Ricaricamento GRAFICA IMPIANTO
  //-------------------------------------------------
  void ReloadGRAPHVAR(bool messagebox);

  __fastcall TFormXYNodi(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormXYNodi *FormXYNodi;
//---------------------------------------------------------------------------
#endif
