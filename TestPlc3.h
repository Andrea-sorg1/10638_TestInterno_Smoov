//---------------------------------------------------------------------------
#ifndef TestPlc3H
#define TestPlc3H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFormTestPlc3 : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel1;
  TLabel *Label1;
  TMaskEdit *EditPlc1;
  TLabel *Label2;
  TMaskEdit *EditPlc2;
  TLabel *Label3;
  TMaskEdit *EditPlc3;
  TStringGrid *GridCanali;
  TBitBtn *BtnLeggi;
  TBevel *Bevel1;
  TBitBtn *BtnScrivi;
  TGroupBox *GroupBox1;
  TPanel *PanelOK;
  TPanel *PanelNO;
  TPanel *PanelINT;
  TStaticText *TextError;
  TLabel *Label4;
  TMaskEdit *EditPlc0;
  TCheckBox *CheckBoxEnableWrite;
  TLabel *Label5;
  TEdit *EditPlc4;
  TLabel *Label6;
  TEdit *EditPlc5;
  TUpDown *UpDownCh;
  TTimer *TimerCiclo;
  TCheckBox *CheckSigned;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  
  
  
  
  void __fastcall BtnLeggiClick(TObject *Sender);
  void __fastcall BtnScriviClick(TObject *Sender);


  void __fastcall GridCanaliSetEditText(TObject *Sender, int ACol,
          int ARow, const AnsiString Value);
  void __fastcall GridCanaliGetEditMask(TObject *Sender, int ACol,
          int ARow, AnsiString &Value);
  void __fastcall CheckBoxEnableWriteClick(TObject *Sender);
  void __fastcall UpDownChClick(TObject *Sender, TUDBtnType Button);
  void __fastcall TimerCicloTimer(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall CheckSignedClick(TObject *Sender);
  


private:	// User declarations
  void __fastcall Messaggi();
  void __fastcall SetCaselle();
  int  __fastcall GetCaselle();
  void __fastcall LetturaPlc();
  void __fastcall ScritturaPlc();

  unsigned short int Dw[556];
  short int NumPlc;
  short int DB;
  short int DW;
  short int nDW;
  char  nFile[30];
  short int ColModificata[65];
  bool  ErroreDiComunicazione;
  char  MessaggioErrore[401];

public:		// User declarations
    __fastcall TFormTestPlc3(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormTestPlc3 *FormTestPlc3;
//---------------------------------------------------------------------------
#endif
