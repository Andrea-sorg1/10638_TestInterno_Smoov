//---------------------------------------------------------------------------
#ifndef PorteImpH
#define PorteImpH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Mask.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFormPorteImp : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *gbNodi;
  TLabel *Label1;
  TMaskEdit *EditNodo1;
  TLabel *Label2;
  TMaskEdit *EditNodo2;
  TLabel *Label3;
  TMaskEdit *EditNodo3;
  TLabel *Label4;
  TMaskEdit *EditNodo4;
  TLabel *Label5;
  TMaskEdit *EditNodo5;
  TLabel *Label6;
  TMaskEdit *EditNodo6;
  TLabel *Label7;
  TMaskEdit *EditNodo7;
  TLabel *Label8;
  TMaskEdit *EditNodo8;
  TLabel *Label9;
  TMaskEdit *EditNodo9;
  TLabel *Label10;
  TMaskEdit *EditNodo10;
  TGroupBox *gbConsensi;
  TCheckBox *CBRich;
  TCheckBox *CBOk;
  TCheckBox *CBIng;
  TBitBtn *BtnOk;
  TGroupBox *gbDirezione;
  TCheckBox *CBUp;
  TCheckBox *CBDown;
  TCheckBox *CBLeft;
  TCheckBox *CBRight;
  TLabel *lblAnticipoRich;
  TMaskEdit *EdAnticipo;
  TCheckBox *CBOkPar;
  TCheckBox *CBCarico;
  TGroupBox *gbSemafori;
  TLabel *Label12;
  TLabel *Label13;
  TLabel *Label14;
  TLabel *Label15;
  TLabel *Label16;
  TLabel *Label17;
  TLabel *Label18;
  TLabel *Label19;
  TLabel *Label20;
  TLabel *Label21;
  TMaskEdit *EditNsem1;
  TMaskEdit *EditNsem2;
  TMaskEdit *EditNsem3;
  TMaskEdit *EditNsem4;
  TMaskEdit *EditNsem5;
  TMaskEdit *EditNsem6;
  TMaskEdit *EditNsem7;
  TMaskEdit *EditNsem8;
  TMaskEdit *EditNsem9;
  TMaskEdit *EditNsem10;
  TCheckBox *CBBlocco;
  TCheckBox *CBTgvInArrivo;
  TUpDown *UpDwPort;
  TLabel *lblInterbloc;
  TMaskEdit *EdInterblocco;
  TCheckBox *CBEsclusa;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall BtnOkClick(TObject *Sender);
  void __fastcall UpDwPortClick(TObject *Sender, TUDBtnType Button);

private:	// User declarations

  TMaskEdit *EditN[11];
  TMaskEdit *EditNS[11];
  void __fastcall SetCaselle();
  void __fastcall GetCaselle();
  void __fastcall Messaggi();

public:		// User declarations

  short int NumeroPortaImp;

  __fastcall TFormPorteImp(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormPorteImp *FormPorteImp;
//---------------------------------------------------------------------------
#endif
