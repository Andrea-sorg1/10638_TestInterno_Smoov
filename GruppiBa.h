//---------------------------------------------------------------------------

#ifndef GruppiBaH
#define GruppiBaH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
//---------------------------------------------------------------------------
class TFormGruppiBa : public TForm
{
__published:	// IDE-managed Components
  TComboBox *CBGruppo;
  TGroupBox *GbGroupConfig;
  TComboBox *CBTipo;
  TPanel *PanCol;
  TLabel *Label1;
  TLabel *Label2;
  TMaskEdit *EdPriorita;
  TBitBtn *BtnOk;
  TBitBtn *BtnClose;
  TColorDialog *ColorDialog1;
  TLabel *Label3;
  TMaskEdit *MaskEdit1;
  TLabel *Label4;
  TMaskEdit *MaskEdit2;
  TLabel *Label5;
  TMaskEdit *MaskEdit3;
  TBevel *Bevel1;
  TLabel *Label6;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall CBGruppoChange(TObject *Sender);
  void __fastcall BtnOkClick(TObject *Sender);
  void __fastcall PanColDblClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
  __fastcall TFormGruppiBa(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormGruppiBa *FormGruppiBa;
//---------------------------------------------------------------------------
#endif
