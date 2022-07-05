//---------------------------------------------------------------------------

#ifndef TipiBoxH
#define TipiBoxH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
//---------------------------------------------------------------------------
class TFormTipiBox : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel1;
  TComboBox *CBTipoBox;
  TGroupBox *gbConfig;
  TGroupBox *GroupBox2;
  TLabel *lblTipoBox;
  TBitBtn *btnSalva;
  TBitBtn *btnEsci;
  TLabel *lblDistanza;
  TLabel *lblArrotond;
  TMaskEdit *MEArr;
  TMaskEdit *MEDist;
  TGroupBox *gbDimensioni;
  TLabel *lblLarg;
  TLabel *lblLung;
  TMaskEdit *MELarg;
  TMaskEdit *MELung;
  TLabel *lblOffset;
  TMaskEdit *MEOffset;
  TLabel *lblAltezza;
  TMaskEdit *MEAltez;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall CBTipoBoxChange(TObject *Sender);
  void __fastcall btnSalvaClick(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);

private:	// User declarations
   void __fastcall Messaggi();
   void __fastcall GetCaselle();
   void __fastcall SetCaselle();

public:		// User declarations

__fastcall TFormTipiBox(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormTipiBox *FormTipiBox;
//---------------------------------------------------------------------------
#endif
