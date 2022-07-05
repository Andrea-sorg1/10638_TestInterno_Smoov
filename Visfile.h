//---------------------------------------------------------------------------
#ifndef VisFileH
#define VisFileH
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
class TFormVisualizzaFile : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel1;
  TRichEdit *VisualizzaFileRichEdit;
  TPanel *Panel2;
  TBitBtn *BtnEsci;
  TBitBtn *BtnStampa;
  TPanel *PanelIntestazione;
  TBitBtn *BtnAzzera;
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall FormActivate(TObject *Sender);

  void __fastcall BtnStampaClick(TObject *Sender);

  void __fastcall BtnAzzeraClick(TObject *Sender);
  
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  
  
private:	// User declarations
  void __fastcall Messaggi();
  void __fastcall LeggiDati();
  void __fastcall Stampa();
  void __fastcall Azzera();

  char NomeFile[101];
  char IntestazioneFile[101];
  char CaptionFile[101];


public:		// User declarations

  char NomeFileVisualizza[101];
  char IntestazioneFileVisualizza[101];
  char CaptionFileVisualizza[101];

  __fastcall TFormVisualizzaFile(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormVisualizzaFile *FormVisualizzaFile;
//---------------------------------------------------------------------------
#endif
