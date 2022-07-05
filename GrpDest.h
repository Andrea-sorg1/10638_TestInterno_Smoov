//---------------------------------------------------------------------------
#ifndef GrpDestH
#define GrpDestH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Mask.hpp>
#include <CheckLst.hpp>
//---------------------------------------------------------------------------
#include "Variabil.h"
//---------------------------------------------------------------------------
class TFormGrpDest : public TForm
{
__published:	// IDE-managed Components
  TComboBox *CBGruppoDest;
  TBitBtn *BtnOk;
  TBitBtn *BtnClose;
  TMaskEdit *EdQty;
  TStaticText *TextQty;
  TCheckListBox *ChListUdc;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall BtnOkClick(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall BtnCloseClick(TObject *Sender);
  void __fastcall CBGruppoDestChange(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:	// User declarations
public:		// User declarations

  short int NumMagazzino;
  short int Group;
  short int QtyPrel;
  AnsiString PosToMove[(MAXTERRAMAG*MAXPIANIMAG)+1];

  __fastcall TFormGrpDest(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormGrpDest *FormGrpDest;
//---------------------------------------------------------------------------
#endif
