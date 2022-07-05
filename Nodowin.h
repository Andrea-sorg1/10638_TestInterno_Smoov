//---------------------------------------------------------------------------
#ifndef NodowinH
#define NodowinH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
//---------------------------------------------------------------------------
#include <Grids.hpp>


#include "Tecnofer.h"
#include <Graphics.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TFormNodi : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel1;
  TRadioGroup *GroupBoxNodi2;
  TGroupBox *GroupBoxNodi1;
  TStaticText *TextNodi1;
  TMaskEdit *EditNodi1;
  TRadioGroup *GroupBoxNodi3;
  TStaticText *TextNodi2;
  TMaskEdit *EditNodi2;
  TStaticText *TextNodi3;
  TMaskEdit *EditNodi3;
  TStaticText *TextNodi4;
  TMaskEdit *EditNodi4;
  TRadioGroup *GroupBoxNodi4;
  TPanel *NRecordPanel;
  TPanel *EditPanel;
  TPanel *Panel3;
  TBitBtn *BtnCerca;
  TBitBtn *BtnCopia;
  TBitBtn *BtnInserimento;
  TBitBtn *BtnStampa;
  TBitBtn *BtnCancella;
  TBitBtn *BtnSalva;
  TBitBtn *BtnEsci;
  TBitBtn *BtnIndietro;
  TBitBtn *BtnAvanti;
  TGroupBox *GroupBoxNodi5;
  TBitBtn *BtnSommaPerc;
  TBitBtn *BtnSottrazPerc;
  TStringGrid *PercorsoGrid;
  TGroupBox *GroupBoxNodi6;
  TStaticText *TextNodi5;
  TEdit *EditNodi5;
  TStaticText *TextNodi6;
  TEdit *EditNodi6;
  TUpDown *UpDown5;
  TUpDown *UpDown6;
  TImage *Image1;
  TCheckBox *CBBloccoRot;
  TBitBtn *BitCompilaMaster;
  TBitBtn *BtnCreaSlave;
  TStaticText *StaticText1;
  TMaskEdit *EditNodi8;
  TStaticText *StaticText3;
  TMaskEdit *EditNodi7;
  TCheckBox *CBInvertiRot;
  TGroupBox *GroupZoneMapping;
  TStaticText *StaticText7;
  TMaskEdit *EditNodiZoneMap;
  TPrintDialog *PrintDialog;
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall Range(TObject *Sender);

  void __fastcall BtnSommaPercClick(TObject *Sender);
  void __fastcall BtnSottrazPercClick(TObject *Sender);

  void __fastcall BtnIndietroClick(TObject *Sender);
  void __fastcall BtnAvantiClick(TObject *Sender);
  void __fastcall BtnCercaClick(TObject *Sender);
  void __fastcall BtnCopiaClick(TObject *Sender);
  void __fastcall BtnSalvaClick(TObject *Sender);
  void __fastcall BtnStampaClick(TObject *Sender);
  void __fastcall BtnCancellaClick(TObject *Sender);
  void __fastcall BtnInserimentoClick(TObject *Sender);

 
  void __fastcall PercorsoGridClick(TObject *Sender);

  void __fastcall PercorsoGridGetEditMask(TObject *Sender, int ACol,
          int ARow, AnsiString &Value);
  
  
  void __fastcall PercorsoGridSelectCell(TObject *Sender, int Col, int Row,
          bool &CanSelect);
  void __fastcall BitCompilaMasterClick(TObject *Sender);
  void __fastcall BtnCreaSlaveClick(TObject *Sender);
  void __fastcall PercorsoGridDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
  private:	// User declarations
  int  __fastcall VerificaInput();
  void __fastcall Gruppi();
  void __fastcall Messaggi();
  void __fastcall GetCaselle();
  void __fastcall SetCaselle();
  void __fastcall LeggiDati();
  void __fastcall Avanti();
  void __fastcall Indietro();
  void __fastcall SommaRigaPercorso();
  void __fastcall SottraiRigaPercorso();
  void __fastcall Cerca();
  void __fastcall Copia();
  void __fastcall Salva();
  void __fastcall Inser();
  void __fastcall Cancel();

  void __fastcall CompilaNodoMaster();
  void __fastcall CreaNodiSlave();

  bool      CoordinateModificate;
  bool      SelPulsanti;
  bool      Errore;
  bool      Inserimento;
  short int Indice;
  short int NumRec;

  struct NOD nod;

public:		// User declarations

  short int NodoIniziale;

  __fastcall TFormNodi(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormNodi *FormNodi;
//---------------------------------------------------------------------------
#endif
