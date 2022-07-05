//---------------------------------------------------------------------------
#ifndef MessaggH
#define MessaggH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TFormMessaggi : public TForm
{
__published:	// IDE-managed Components
    TComboBox *MessaggComboBox;
    TLabel *MessaggLabel1;
    TBitBtn *OkMessaggBitBtn;
    TBitBtn *EsciMessaggBitBtn;
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall OkMessaggBitBtnClick(TObject *Sender);
    
private:	// User declarations

public:		// User declarations
    __fastcall TFormMessaggi(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMessaggi *FormMessaggi;
//---------------------------------------------------------------------------
#endif
