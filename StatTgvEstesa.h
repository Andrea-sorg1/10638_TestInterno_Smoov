//---------------------------------------------------------------------------
#ifndef StatTgvEstesaH
#define StatTgvEstesaH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "CGAUGES.h"
#include "XLSMini.hpp"
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <Mask.hpp>
#include "CSPIN.h"
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TFormStatTgvEstesa : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel2;
  TBitBtn *BtnStampa;
  TBitBtn *btnExportExcel;
  TBitBtn *BtnExit;
  TSaveDialog *SaveDialog;
  TXLSMini *XLSReport;
  TPrintDialog *PrintDialog;
  TPopupMenu *PopupMenu;
  TMenuItem *popSelAll;
  TMenuItem *popCopy;
  TStaticText *StaticText1;
  TUpDown *UpDownTGV;
  TUpDown *UpDownGiorni;
  TEdit *edGiorni;
  TEdit *edTgvNum;
  TStaticText *StaticText2;
  TPanel *Panel1;
  TListView *ListTGV;
  TCGauge *CGauge;
  TMaskEdit *edDataStart;
  TMaskEdit *edOraStart;
  TDateTimePicker *DateTimePickerStart;
  TImage *Image1;
  TSpeedButton *btnCalendarOK;
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall ListTGVCustomDrawItem(TCustomListView *Sender, TListItem *Item, TCustomDrawState State, bool &DefaultDraw);
  void __fastcall ListTGVCustomDrawSubItem(TCustomListView *Sender, TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw);
  void __fastcall ListTGVInsert(TObject *Sender, TListItem *Item);
  void __fastcall btnCalendarOKClick(TObject *Sender);
  void __fastcall btnExportExcelClick(TObject *Sender);
  void __fastcall SaveDialogFolderChange(TObject *Sender);
  void __fastcall edTgvNumChange(TObject *Sender);
  void __fastcall UpDownGiorniClick(TObject *Sender, TUDBtnType Button);
  void __fastcall edGiorniKeyPress(TObject *Sender, char &Key);
  void __fastcall edGiorniKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall edTgvNumKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall UpDownTGVClick(TObject *Sender, TUDBtnType Button);
  void __fastcall edTgvNumKeyPress(TObject *Sender, char &Key);
  void __fastcall popSelAllClick(TObject *Sender);
  void __fastcall popCopyClick(TObject *Sender);
  void __fastcall ListTGVKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall BtnStampaClick(TObject *Sender);
  void __fastcall DateTimePickerStartChange(TObject *Sender);

private:	// User declarations
  int Giorni;
  int NumRighe;
  char DataRegistrazDB[51];
  char DataS[11];
  char OraS[11];
  long int Tempo;
  short int Batteria;
  short int NumMissioni;

  unsigned short int Km;
  AnsiString Stringa[51][21];

  void __fastcall ListInsertValues();
  void __fastcall CompilaLista();
  void __fastcall AlternateRowColor(TCustomListView *Sender, TListItem *Item);

public:		// User declarations
  int IndiceRiga;
  int tgv;

  __fastcall TFormStatTgvEstesa(TComponent* Owner);
  void __fastcall myEditMask(TEdit *Edit, char &Key, int Digit);

};
//---------------------------------------------------------------------------
extern PACKAGE TFormStatTgvEstesa *FormStatTgvEstesa;
//---------------------------------------------------------------------------
#endif
