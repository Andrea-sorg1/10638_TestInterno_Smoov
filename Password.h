//---------------------------------------------------------------------------
#ifndef PasswordH
#define PasswordH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TFormPassword : public TForm
{
__published:	// IDE-managed Components
  TEdit   *PassWordEdit;
  TLabel  *PassWordLabel;
  TBitBtn *BottPassOK;
  TBitBtn *BottPassCancel;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall BottPassOKClick(TObject *Sender);
  void __fastcall BottPassCancelClick(TObject *Sender);

  
  
  
private:	// User declarations

public:		// User declarations

  AnsiString Password;

    __fastcall TFormPassword(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormPassword *FormPassword;
//---------------------------------------------------------------------------
#endif
