//---------------------------------------------------------------------------
#ifndef MappingH
#define MappingH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Mask.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
#include <Math.hpp>
#include <Buttons.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TFormMappa : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *GroupBox1;
  TLabel *Label1;
  TLabel *Label2;
  TLabel *Label3;
  TStringGrid *GridCoordinate;
  TEdit *EditRot;
  TEdit *EditX0;
  TEdit *EditY0;
  TButton *BtnTest;
  TGroupBox *GroupBox2;
  TStaticText *TextXls;
  TStaticText *TextYls;
  TStaticText *TextRotls;
  TGroupBox *GroupBox3;
  TStringGrid *GridZone;
  TButton *BtnMapping;
  TTimer *TimerMapping;
  TStaticText *TextXly;
  TStaticText *TextYly;
  TStaticText *TextRotly;
  TStaticText *TextStato;
  TGroupBox *GroupBox4;
  TLabel *Label5;
  TEdit *EditMinR;
  TLabel *Label6;
  TEdit *EditMaxZone;
  TButton *BtnCalEncoder;
  TButton *BtnCalSterzo;
  TLabel *Label4;
  TStaticText *TextRotN;
  TStaticText *TextNodo;
  TLabel *Label7;
  TLabel *Label8;
  TLabel *Label9;
  TStaticText *TextGRDOffset;
  TLabel *Label10;
  TEdit *EditScostPLogico;
  TButton *BtnCancella;
  TLabel *Label12;
  TEdit *EditRiflInUso;
  TLabel *Label13;
  TEdit *EditG;
  TLabel *Label14;
  TEdit *EditZona;
  TPanel *Panel1;
  TSpeedButton *BtnLed;
  TButton *BtnVisualizza;
  TLabel *Label11;
  TEdit *EditNumScanMediaMap;
  TEdit *EdLaserMappaRiflettoriTGV;
  TLabel *Label15;
  TEdit *EdLaserMappaRiflettoriZona;
  TLabel *Label16;
  TLabel *Label17;
  TEdit *EditGMediaMap;
  TLabel *Label18;
  TCheckBox *CheckBoxRotTesta;
  TCheckBox *CBVisualizzaTrasparente;
  TLabel *Label19;
  TEdit *EditRaggioRif;
  TPanel *pnlChangeTGV;
  TImage *imgTgv;
  TShape *tgv_color;
  TBitBtn *btnTgvUP;
  TBitBtn *btnTgvDN;
  TStaticText *txtTgv;
  TBitBtn *BtnUpload;
  TBitBtn *BtnDownload;
  TBitBtn *BtnChangeReflector;
  TBitBtn *BtnSave;
  TBitBtn *BtnLoad;
  TBitBtn *BtnWrite;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall BtnWriteClick(TObject *Sender);
  void __fastcall BtnLoadClick(TObject *Sender);
  void __fastcall BtnTestClick(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall BtnMappingClick(TObject *Sender);
  void __fastcall TimerMappingTimer(TObject *Sender);
  void __fastcall BtnUploadClick(TObject *Sender);
  void __fastcall BtnDownloadClick(TObject *Sender);
  void __fastcall BtnCalEncoderClick(TObject *Sender);
  void __fastcall BtnCalSterzoClick(TObject *Sender);
  void __fastcall BtnCancellaClick(TObject *Sender);
  void __fastcall BtnSaveClick(TObject *Sender);
  void __fastcall GridCoordinateDrawCell(TObject *Sender, int Col, int Row, TRect &Rect, TGridDrawState State);
  void __fastcall BtnVisualizzaClick(TObject *Sender);
  void __fastcall BtnChangeReflectorClick(TObject *Sender);
  void __fastcall EdLaserMappaRiflettoriTGVChange(TObject *Sender);
  void __fastcall EdLaserMappaRiflettoriZonaChange(TObject *Sender);
  void __fastcall CBVisualizzaTrasparenteClick(TObject *Sender);
  void __fastcall btnTgvUpDnClick(TObject *Sender);

private:	// User declarations
  struct ZONEMAPPING DatiZona;
  WORD DmMappa[(MAXPUNTI*3)+11];
  int  NumZona;
  int  MinRiflettori;
  int  NumScanMediaMap;
  int  RaggioRiflettori;

  void __fastcall SetCaselle();
  void __fastcall GetCaselle();
  void __fastcall LoadDati();
  void __fastcall SaveDati();
  void __fastcall ScaricaMappaSuPLC();


public:		// User declarations
  short int NumTgv;
  bool AutoSend;

  __fastcall TFormMappa(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMappa *FormMappa;
//---------------------------------------------------------------------------
#endif
