//---------------------------------------------------------------------------
#ifndef InitComH
#define InitComH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormSetCom : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *BoxHostLink;
  TRadioGroup *BoxBaudrate;
  TGroupBox *BoxCOM;
  TComboBox *ListaCOM;
  TRadioGroup *BoxDati;
  TRadioGroup *BoxParita;
  TRadioGroup *BoxStopBit;
  TButton *BtnAttiva;
  TCheckBox *BoxCtsRts;
  void __fastcall GetCaselle();
  void __fastcall SetCaselle();
  void __fastcall BtnAttivaClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall ListaCOMClick(TObject *Sender);
  void __fastcall FormActivate(TObject *Sender);
private:	// User declarations
public:		// User declarations

  int Index;

  __fastcall TFormSetCom(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormSetCom *FormSetCom;
//---------------------------------------------------------------------------
#endif
