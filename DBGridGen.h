//---------------------------------------------------------------------------

#ifndef DBGridGenH
#define DBGridGenH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ADODB.hpp>
#include <DB.hpp>
#include <DBGrids.hpp>
#include <Grids.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <DBTables.hpp>
#include "CGAUGES.h"
#include <DBCtrls.hpp>
#include <Mask.hpp>
#include <DBClient.hpp>
#include <DBLocal.hpp>
#include <DBLocalB.hpp>
#include <Provider.hpp>
#include "XLSMini.hpp"
#include <Dialogs.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
#define MAX_COMBO 30
#define MAX_COMBOLINE 1000
//---------------------------------------------------------------------------
class TFormDBGridGen : public TForm
{
__published:	// IDE-managed Components
  TDBGrid *DBGrid;
  TDataSource *DataSource;
  TADOQuery *ADOQuery;
  TCGauge *Gauge;
  TPanel *pnlBottoni;
  TBitBtn *btnClear;
  TBitBtn *btnFilter;
  TPanel *pnlCombo;
  TLabel *lblCB1;
  TImage *WinCorner;
  TComboBox *ComboBoxDef1;
  TBitBtn *BtnExit;
  TBitBtn *btnExcel;
  TXLSMini *XLSReport;
  TTimer *TimerClose;
  TSaveDialog *SaveDialog;
  TPrintDialog *PrintDialog;
  TComboBox *ComboBoxDef2;
  TLabel *lblSQL;
  TBitBtn *btnMaxRecord;
  TBitBtn *btnStampa;
  TLabel *lbl_Message;
  TBitBtn *btnAutoFitColumn;
  TPopupMenu *PopupMenu;
  TMenuItem *popCopy;
  TBitBtn *BitBtn1;
  TBitBtn *btnSpec01;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall DBGridTitleClick(TColumn *Column);
  void __fastcall btnFilterClick(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall BtnExitClick(TObject *Sender);
  void __fastcall btnExcelClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall btnMaxRecordClick(TObject *Sender);
  void __fastcall btnStampaClick(TObject *Sender);
  void __fastcall btnClearClick(TObject *Sender);
  void __fastcall btnAutoFitColumnClick(TObject *Sender);
  void __fastcall DBGridDblClick(TObject *Sender);
  void __fastcall popCopyClick(TObject *Sender);
  void __fastcall BitBtn1Click(TObject *Sender);
  void __fastcall SaveDialogFolderChange(TObject *Sender);
  void __fastcall DBGridCellClick(TColumn *Column);
  void __fastcall rbOrarioClick(TObject *Sender);
  void __fastcall btnSpec01Click(TObject *Sender);
  void __fastcall DBGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall ComboBoxDefChange(TObject *Sender);

private:	// User declarations
  void GestioneComboBox();
  void GestioneCreateComboBox();
  void CopyCell();
  void __fastcall myClose();

  bool  FlipFlop;
  bool  KeyStop;
  int   ComboCount;
  int   ColSize[MAX_COMBO+1];
  int   EscStatus;
  TComboBox**    dbCB1;
  TComboBox**    dbCB2;
  TLabel**       dbLabel;
  TStringList**  myList;
  TFieldType     myFieldType[MAX_COMBO+1];

public:		// User declarations
  __fastcall TFormDBGridGen(TComponent* Owner);

  AnsiString SQL_Query;
  AnsiString SQL_Table;
  AnsiString myMaxRecord;
  int  NumCombo;
  bool Executed;
  bool SqlQueryExt;
  bool EnableTableCB;
  bool Reload;

  void AggiornaCombo();
  void ShowMessHU(char* HU);
  int  ParseDateTime(char *strDateTime);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormDBGridGen *FormDBGridGen;
//---------------------------------------------------------------------------
#endif
