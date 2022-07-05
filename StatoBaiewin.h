//---------------------------------------------------------------------------

#ifndef StatoBaiewinH
#define StatoBaiewinH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "TecnoFer.h"
#include <Buttons.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormStatoBaie : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *GroupBox1;
  TListView *LVChiamBaie;
  TLabel *lblTitolo;
  TBitBtn *BtnOk;
  TSpeedButton *BtnStampa;
  TPrintDialog *PrintDialog;
  TTimer *TimerRefresh;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall BtnStampaClick(TObject *Sender);
  void __fastcall LVChiamBaieColumnClick(TObject *Sender, TListColumn *Column);
  void __fastcall LVChiamBaieCompare(TObject *Sender, TListItem *Item1, TListItem *Item2, int Data, int &Compare);
  void __fastcall TimerRefreshTimer(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall LVChiamBaieCustomDrawItem(TCustomListView *Sender, TListItem *Item, TCustomDrawState State, bool &DefaultDraw);
  void __fastcall LVChiamBaieCustomDrawSubItem(TCustomListView *Sender, TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw);

private:	// User declarations
  void __fastcall TFormStatoBaie::Messaggi();
  void __fastcall TFormStatoBaie::Refresh();
  int Colonna;
  bool Up[12];

public:		// User declarations
  __fastcall TFormStatoBaie(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormStatoBaie *FormStatoBaie;
//---------------------------------------------------------------------------
#endif
