//---------------------------------------------------------------------------
#ifndef DisRiflettoriH
#define DisRiflettoriH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------

#include <checklst.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <CheckLst.hpp>
#include <Graphics.hpp>

//---------------------------------------------------------------------------
class TFormDisRiflettori : public TForm
{
__published:	// IDE-managed Components
  TCheckListBox *CLBNodi;
  TPanel *PanelTotRiflettori;
  TBitBtn *BtnOk;
  TBitBtn *BtnEsci;
  TGroupBox *GroupBox1;
  TImage *Image1;
  TStaticText *TextNodi5;
  TStaticText *TextNodi6;
  TEdit *EditY;
  TEdit *EditX;
  TUpDown *UpDown5;
  TUpDown *UpDown6;
  TLabel *Label1;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);

  void __fastcall BtnOkClick(TObject *Sender);

  void __fastcall CLBNodiClickCheck(TObject *Sender);

  void __fastcall CLBNodiDblClick(TObject *Sender);
  
private:	// User declarations


public:		// User declarations

  struct    ZONEMAPPING DatiZona;
  short int ElencoSpecchi[MAXRIFLETTORI+1];
  int       NumZona;
  int       NumSpecchiIniziale;
  int       NumSpecchiModificato;
  bool      ZonaModificata;

  __fastcall TFormDisRiflettori(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormDisRiflettori *FormDisRiflettori;
//---------------------------------------------------------------------------
#endif
