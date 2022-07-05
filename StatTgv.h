//---------------------------------------------------------------------------
#ifndef StatTgvH
#define StatTgvH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
#include <ComCtrls.hpp>

#include "Tecnofer.h"
#include "SQLDB.h"
#include <Graphics.hpp>
#include <ImgList.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include "XLSMini.hpp"
//---------------------------------------------------------------------------
class TFormStatTgv : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel1;
  TPrintDialog *PrintDialog;
  TListView *ListTGV;
  TSpeedButton *btnCalendarOK;
  TPopupMenu *PopupMenu;
  TMenuItem *popSelAll;
  TMenuItem *popCopy;
  TSaveDialog *SaveDialog;
  TXLSMini *XLSReport;
  TPanel *Panel2;
  TBitBtn *btnStatTgvEstesa;
  TBitBtn *BtnStampa;
  TBitBtn *btnExportExcel;
  TBitBtn *BtnListTotMiss;
  TBitBtn *BtnExit;
  TMaskEdit *edDataStart;
  TMaskEdit *edOraStart;
  TSpeedButton *btnDayDN;
  TSpeedButton *btnDayUP;
  TDateTimePicker *DateTimePickerStart;
  TPanel *PanResa;
  TImage *Image1;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall BtnStampaClick(TObject *Sender);
  void __fastcall btnListaStatClick(TObject *Sender);
  void __fastcall BtnListTotMissClick(TObject *Sender);
  void __fastcall ListTGVInsert(TObject *Sender, TListItem *Item);
  void __fastcall ListTGVCustomDrawItem(TCustomListView *Sender, TListItem *Item, TCustomDrawState State, bool &DefaultDraw);
  void __fastcall ListTGVCustomDrawSubItem(TCustomListView *Sender, TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw);
  void __fastcall btnCalendarOKClick(TObject *Sender);
  void __fastcall popSelAllClick(TObject *Sender);
  void __fastcall popCopyClick(TObject *Sender);
  void __fastcall ListTGVKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall btnExportExcelClick(TObject *Sender);
  void __fastcall SaveDialogFolderChange(TObject *Sender);
  void __fastcall btnStatTgvEstesaClick(TObject *Sender);
  void __fastcall btnDayDOWNClick(TObject *Sender);
  void __fastcall btnDayUPClick(TObject *Sender);
  void __fastcall DateTimePickerStartChange(TObject *Sender);

private:	// User declarations
  void __fastcall Messaggi();
  void __fastcall Gruppi();
  void __fastcall SetCaselle();
  void __fastcall CompilaStrutture();
  void __fastcall CompilaLista();
  void __fastcall ListInsertValues();
  void __fastcall AlternateRowColor(TCustomListView *Sender, TListItem *Item);
  int  __fastcall Load_MSGStatTGV();
  void __fastcall CopyClipboard();
  void __fastcall ButtonClick(TObject *Sender);

  int       NumRighe;
  bool      DataNONattuale;
  char      DateAttuale[11];
  char      TimeAttuale[11];
  char      DataOggi[11];
  char      DataRegistrazDB[81];
  char      DataS[11];
  char      OraS[11];
  long int  Tempo;
  long int  Ore[15];
  long int  Min[15];
  long int  Sec[15];
  struct    TIMTGV  EditTimTgv;
  short int NumMissioni;
  unsigned short int Km;
  AnsiString Stringa[MAXAGV+1][MAXFIELD_TIMTGV+1];

public:		// User declarations
  int   IndiceRiga;
  char  DataStatTgv[11];

  __fastcall TFormStatTgv(TComponent* Owner);
  void __fastcall ScomponiTempi( long int sec, long int *hh, long int *mm, long int *ss  );
};
//---------------------------------------------------------------------------
extern PACKAGE TFormStatTgv *FormStatTgv;
//---------------------------------------------------------------------------
#endif
