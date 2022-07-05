//---------------------------------------------------------------------------
#ifndef ListaCarH
#define ListaCarH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
#include "Libreria.h"
#include "Tecnofer.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TFormListaCar : public TForm
{
__published:	// IDE-managed Components
  TListView *ListView;
  TBitBtn *BtnEsci;
  TSpeedButton *BtnStampa1;
  TLabel *Label2;
  TComboBox *ComboTypM;
  TBitBtn *BtnRefresh;
  TStaticText *TextTot;
  TBevel *Bevel1;
  TPrintDialog *PrintDialog1;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall ListViewColumnClick(TObject *Sender,
          TListColumn *Column);
  void __fastcall ListViewCompare(TObject *Sender, TListItem *Item1,
          TListItem *Item2, int Data, int &Compare);
  void __fastcall ListViewInsert(TObject *Sender, TListItem *Item);
  void __fastcall BtnEsciClick(TObject *Sender);
  
  void __fastcall BtnStampa1Click(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall BtnRefreshClick(TObject *Sender);
private:	// User declarations

  int Colonna;
  bool Up[11];
  short int indice;
  char Stringa [MAXBOXIMPIANTO+1][51];   // Prima   Colonna
  char Stringa1[MAXBOXIMPIANTO+1][51];   // Seconda Colonna
  char Stringa2[MAXBOXIMPIANTO+1][51];   // Terza   Colonna
  char Stringa3[MAXBOXIMPIANTO+1][51];   // Quarta  Colonna
  char Stringa4[MAXBOXIMPIANTO+1][51];   // Quinta  Colonna
  char Stringa5[MAXBOXIMPIANTO+1][51];   // Sesta   Colonna
  char Stringa6[MAXBOXIMPIANTO+1][51];   // Settima Colonna
  char Stringa7[MAXBOXIMPIANTO+1][51];   // Ottava  Colonna
  char Stringa8[MAXBOXIMPIANTO+1][51];   // Nona    Colonna
  char Stringa9[MAXBOXIMPIANTO+1][51];   // Decima  Colonna

  short int TypM[1001];

public:		// User declarations
  __fastcall TFormListaCar(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormListaCar *FormListaCar;
//---------------------------------------------------------------------------
#endif
