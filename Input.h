//---------------------------------------------------------------------------
#ifndef InputH
#define InputH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <Mask.hpp>
#include <ComCtrls.hpp>
#include <DateUtils.hpp>
//---------------------------------------------------------------------------
class TFormInput : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel1;
  TLabel *lblTimeStart;
  TBitBtn *BtnOK;
  TBevel *Bevel3;
  TLabel *lblMacchina;
  TMaskEdit *edNumMac;
  TLabel *lblTimeEnd;
  TComboBox *cbTipoMac;
  TMaskEdit *edTipo;
  TLabel *lblTipo;
  TBevel *Bevel2;
  TBevel *Bevel1;
  TEdit *txtEdit;
  TBevel *Bevel4;
  TLabel *lblInputString;
  TBitBtn *BtnCancel;
  TCheckBox *Check1;
  TCheckBox *Check2;
  TRadioButton *RadioButton1;
  TRadioButton *RadioButton2;
  TMaskEdit *edDataStart;
  TMaskEdit *edOraStart;
  TMaskEdit *edDataEnd;
  TMaskEdit *edOraEnd;
  TDateTimePicker *DateTimePickerStart;
  TDateTimePicker *DateTimePickerEnd;
  TEdit *txtEdit1;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall BtnOKClick(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall DateTimePickerSEChange(TObject *Sender);

private:	// User declarations

public:		// User declarations

  int   Tipo;
  int   Index;
  int   TimeS;
  int   TypMac;
  int   TypCarScar;
  char  DataS[11];
  char  OraS[11];
  int   TimeE;
  char  DataE[11];
  char  OraE[11];

  __fastcall TFormInput(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormInput *FormInput;
//---------------------------------------------------------------------------
#endif
