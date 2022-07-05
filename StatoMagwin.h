//---------------------------------------------------------------------------

#ifndef StatoMagwinH
#define StatoMagwinH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "TecnoFer.h"
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TFormStatoMagaz : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *GroupBox1;
  TListView *LVMagaz;
  TLabel *lblTitolo;
  TListView *LVCodici;
  TBitBtn *BtnOk;
  TBevel *Bevel3;
  TSpeedButton *BtnStampa;
  TPrintDialog *PrintDialog;
  TTimer *TimerRefresh;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall BtnStampaClick(TObject *Sender);
  void __fastcall LVMagazCompare(TObject *Sender, TListItem *Item1, TListItem *Item2, int Data, int &Compare);
  void __fastcall LVMagazColumnClick(TObject *Sender, TListColumn *Column);
  void __fastcall TimerRefreshTimer(TObject *Sender);
  void __fastcall LVCodiciColumnClick(TObject *Sender, TListColumn *Column);
  void __fastcall LVCodiciCompare(TObject *Sender, TListItem *Item1, TListItem *Item2, int Data, int &Compare);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall LVMagazCustomDrawItem(TCustomListView *Sender, TListItem *Item, TCustomDrawState State, bool &DefaultDraw);
  void __fastcall LVMagazCustomDrawSubItem(TCustomListView *Sender, TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw);
  void __fastcall LVCodiciCustomDrawItem(TCustomListView *Sender, TListItem *Item, TCustomDrawState State, bool &DefaultDraw);
  void __fastcall LVCodiciCustomDrawSubItem(TCustomListView *Sender, TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw);

private:	// User declarations
  void __fastcall TFormStatoMagaz::Messaggi();
  void __fastcall TFormStatoMagaz::Refresh();
  int ColonnaMag;
  int ColonnaCod;
  bool Up[6];

public:		// User declarations
  __fastcall TFormStatoMagaz(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormStatoMagaz *FormStatoMagaz;
//---------------------------------------------------------------------------
#endif
