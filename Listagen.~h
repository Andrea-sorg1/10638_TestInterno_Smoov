//---------------------------------------------------------------------------
#ifndef ListagenH
#define ListagenH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <Graphics.hpp>
#include "CGAUGES.h"
#include "XLSMini.hpp"
//---------------------------------------------------------------------------
#define MAXCOL1 33
#define MAXLIN 10000
//---------------------------------------------------------------------------
class TFormListagen : public TForm
{
__published:	// IDE-managed Components
  TPanel *pnlButtons;
  TBitBtn *btnEsci;
  TRichEdit *StampaRichEdit;
  TTimer *TimerClose;
  TPrintDialog *PrintDialog;
  TBitBtn *btnStampa;
  TBitBtn *btnRinfresca;
  TBitBtn *btnAzzera;
  TBitBtn *btnExportExcel;
  TBitBtn *btnSaveAsCSV;
  TPanel *pnlFilter;
  TComboBox *cbFilter;
  TSpeedButton *btnAttivaFiltri;
  TMainMenu *MainMenu;
  TMenuItem *Edit1;
  TMenuItem *menu_SelAll;
  TMenuItem *menu_Copy;
  TLabel *Label;
  TPopupMenu *PopupMenu;
  TMenuItem *popSelAll;
  TMenuItem *popCopy;
  TMenuItem *menu_Exit;
  TSpeedButton *btnGliphDown;
  TSpeedButton *btnGliphUp;
  TCGauge *CGauge;
  TComboBox *cbSelFilter;
  TSaveDialog *SaveDialog;
  TXLSMini *XLSReport;
  TBitBtn *btnFilterOFF;
  TPanel *pnlListView;
  TListView *ListView;
  TImage *WinCorner;
  TLabel *lblSelected;
  TBitBtn *btnClose;
  TBitBtn *BitBtn1;
  TBitBtn *btnFilterAdd;
  TBitBtn *btnFilterSub;
  TBitBtn *btnFilterON;
  TSpeedButton *btnAND;
  TSpeedButton *btnOR;
  TBitBtn *btnLampOFF;
  TBitBtn *btnLampON;
  TCheckBox *chkOnlyDate;
  void __fastcall ListViewInsert(TObject *Sender, TListItem *Item);
  void __fastcall ListViewColumnClick(TObject *Sender, TListColumn *Column);
  void __fastcall ListViewCompare(TObject *Sender, TListItem *Item1, TListItem *Item2, int Data, int &Compare);
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall btnAzzeraClick(TObject *Sender);
  void __fastcall btnRinfrescaClick(TObject *Sender);
  void __fastcall btnStampaClick(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall TimerCloseTimer(TObject *Sender);
  void __fastcall AzzeraTimerMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall btnExportExcelClick(TObject *Sender);
  void __fastcall SaveDialogFolderChange(TObject *Sender);
  void __fastcall btnSaveAsCSVClick(TObject *Sender);
  void __fastcall ListViewChange(TObject *Sender, TListItem *Item, TItemChange Change);
  void __fastcall cbFilterChange(TObject *Sender);
  void __fastcall ListViewKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall menu_SelAllClick(TObject *Sender);
  void __fastcall menu_CopyClick(TObject *Sender);
  void __fastcall popCopyClick(TObject *Sender);
  void __fastcall popSelAllClick(TObject *Sender);
  void __fastcall btnAttivaFiltriClick(TObject *Sender);
  void __fastcall menu_ExitClick(TObject *Sender);
  void __fastcall ListViewCustomDrawItem(TCustomListView *Sender, TListItem *Item, TCustomDrawState State, bool &DefaultDraw);
  void __fastcall ListViewCustomDrawSubItem(TCustomListView *Sender, TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw);
  void __fastcall btnFilterONClick(TObject *Sender);
  void __fastcall btnFilterOFFClick(TObject *Sender);
  void __fastcall btnEsciClick(TObject *Sender);
  void __fastcall btnCloseClick(TObject *Sender);
  void __fastcall ListViewMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall btnFilterAddClick(TObject *Sender);
  void __fastcall btnFilterSubClick(TObject *Sender);

private:	// User declarations
  int   Colonna;
  int   EscStatus;

  short int Tipo;
  bool  Up[7];
  bool  FilterWindow;
  bool  OneShot;
  bool  bitON;
  char  fileDAT[105];
  char  fileTMP[105];
  char  Stringa0[3001][101];   // Prima   Colonna
  char  Stringa1[3001][101];   // Seconda Colonna
  char  Stringa2[3001][101];   // Terza   Colonna
  char  Stringa3[3001][101];   // Quarta  Colonna
  char  Stringa4[3001][101];   // Quinta  Colonna
  char  Stringa5[3001][101];   // Sesta   Colonna
  char  Stringa6[3001][101];   // Settima Colonna
  int   oldFormWidth;
  int   oldListWidth;
  short int indice;
  short int myIndex;

  TComboBox * FilterCombo[10];
  TComboBox * SelFilterCombo[10];

  void __fastcall cbNewFilterChange(TObject *Sender);
  void __fastcall Messaggi();
  void __fastcall Refresh1();
  void __fastcall StampaAvanzata();
  void __fastcall AggiornaSelFiltro(TComboBox * Filter, TComboBox * SelFilter);
  void __fastcall AggiornaFiltro(TComboBox * Filter, TComboBox * SelFilter);  
  void __fastcall Filter_ON();

public:		// User declarations
  short int TipoFileLista;     // Lista legata ai TGV
                               // Lista legata ai PALETTIZZATORI
                               // Lista legata agli AVVOLGITORI
  char NomeFileLista[101];     // Nome del file cronologico da visualizzare
  char NomeFileAzzera[101];    // Eventuale file da cancellare per azzeramento
  char CaptionFileLista[101];  // Caption del File

  //----------------------------------------------------------
  // Dati validi solo con Tipo>=5
  int  ColNum;                               // Numero Colonne (MAXCOL1=15)
  char CaptionColumn[MAXCOL1+1][101];        // Caption Colonne
  char ColumnText[MAXCOL1+1][MAXLIN+1][201]; // Stringa della colonna
  int  ColSize[MAXCOL1+1];                   // Dimensione delle colonne (-1=Autosize)
  char CarSeparatore;                        // Carattere Separatore. Default: ';'
  bool reverseFile;
  int ComboCount;
  bool KeyStop;



  __fastcall TFormListagen(TComponent* Owner);
  void __fastcall AlternateRowColor(TCustomListView *Sender, TListItem *Item);
  void __fastcall CopyClipboard(TListView *myListView, TCGauge *myGauge);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormListagen *FormListagen;
//---------------------------------------------------------------------------
#endif
