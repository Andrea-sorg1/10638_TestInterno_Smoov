//---------------------------------------------------------------------------
#ifndef TestPlc1H
#define TestPlc1H
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
class TFormTestPlc : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel1;
  TLabel *Label1;
  TLabel *Label2;
  TStringGrid *GridCanali;
  TBitBtn *BtnLeggi;
  TBevel *Bevel1;
  TBitBtn *BtnScrivi;
  TGroupBox *GroupBox1;
  TPanel *PanelOK;
  TPanel *PanelNO;
  TPanel *PanelINT;
  TRadioGroup *BoxTipoCh;
  TLabel *Label3;
  TUpDown *UpDownCh;
  TStaticText *TextError;
  TEdit *EditPlc1;
  TEdit *EditPlc2;
  TEdit *EditPlc3;
  TCheckBox *CheckSigned;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);




  void __fastcall BtnLeggiClick(TObject *Sender);
  void __fastcall BtnScriviClick(TObject *Sender);


  void __fastcall GridCanaliSetEditText(TObject *Sender, int ACol,
          int ARow, const AnsiString Value);
  void __fastcall GridCanaliGetEditMask(TObject *Sender, int ACol,
          int ARow, AnsiString &Value);

  
  void __fastcall EditPlcExit(TObject *Sender);
  
  
  void __fastcall UpDownChClick(TObject *Sender, TUDBtnType Button);
  void __fastcall UpDownChChanging(TObject *Sender, bool &AllowChange);
  void __fastcall CheckSignedClick(TObject *Sender);
private:	// User declarations
  void __fastcall Messaggi();
  void __fastcall SetCaselle();
  void __fastcall GetCaselle();
  void __fastcall LetturaPlc();
  void __fastcall ScritturaPlc();

  WORD  DM[1000];
  WORD  Plc;
  WORD  DmIni;
  WORD  TotDm;
  int  TipoCh;
  int  Port;
  int  ColModificata[1001];
  bool ErroreDiComunicazione;
  char MessaggioErrore[101];
public:		// User declarations
    __fastcall TFormTestPlc(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormTestPlc *FormTestPlc;
//---------------------------------------------------------------------------
#endif
