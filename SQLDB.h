//---------------------------------------------------------------------------
#ifndef SQLDBH
#define SQLDBH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <DBGrids.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <ImgList.hpp>
#include <ADODB.hpp>
#include <Graphics.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
#include "XLSMini.hpp"
#include "CGAUGES.h"
//---------------------------------------------------------------------------
#include "Tecnofer.h"
#include "Thread1.h"

//---------------------------------------------------------------------------
class TFormSQLDB : public TForm
{
__published:	// IDE-managed Components
  TImageList *ImgLstDB;
  TTimer *TimerRefresh;
  TButton *BtnOpenTable;
  TButton *BtnCloseTable;
  TButton *BtnInsert;
  TStatusBar *StatusBarSQL;
  TPageControl *PageCSQL;
  TTabSheet *TabTab;
  TPanel *PanTabTab;
  TDBGrid *DBGridTab;
  TStaticText *TxtNRecTab;
  TImage *WinCorner;
  TSaveDialog *SaveDialog;
  TXLSMini *XLSReport;
  TCGauge *Gauge;
  TComboBox *cbListaTabelle;
  TBitBtn *btnDBGrid;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall TimerRefreshTimer(TObject *Sender);
  void __fastcall BtnOpenTableClick(TObject *Sender);
  void __fastcall BtnCloseTableClick(TObject *Sender);
  void __fastcall FormPaint(TObject *Sender);
  void __fastcall BtnInsertClick(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall DBGridTabTitleClick(TColumn *Column);
  void __fastcall SaveDialogFolderChange(TObject *Sender);
  void __fastcall btnDBGridClick(TObject *Sender);

private:	// User declarations
  bool TabIsOpened;
  bool InitTabOn;
  bool TableOpen;
  bool FlipFlop;
  short int index_refresh;
  long int time_err_sql;
  void InitTabsSQL();
  void DeleteTabsSQL();
  TCanvas *pCanvasRefreshDB;
  TDataSource *DSTab;
  TADOQuery   *ADOQueryTab;
  TADOTable   *ADOTabTab;

public:		// User declarations

  TMyThread1 *t_SQL;             // Thread principale di sistema

  bool ComandoSQL_ON;            // flag di comando in corso su una qualunque tabella SQL
  bool ForzaLettura;             // flag di forzatura stato di tutte le baie

  bool delay_on_err_SQL();

  int load_udc_from_SQL(int cmd, unsigned int locS, unsigned int locE, char *UdcS, char *UdcE, char *all_mess);
  int write_udc_to_SQL(int cmd, struct DATI_PRD *UdcTmp, char *all_mess);
  //--------------------------------------------------
  // Funzioni per DATE
  //--------------------------------------------------
  TDateTime Algo_TimeInt_To_DateTime(long int TimeInt);
  long int Algo_DateTime_To_TimeInt(TDateTime DateTimeTemp);
  char* Algo_TimeInt_To_DateTimeString(long int TimeInt, char* CharTimeSQL);
  void TimeSeparatorConvert(char* CharDateTime);

  __fastcall TFormSQLDB(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormSQLDB *FormSQLDB;
//---------------------------------------------------------------------------
#endif
