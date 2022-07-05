//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "Main.h"
#include "Stato.h"
#include "Messagg.h"
#include "Tecnofer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMessaggi *FormMessaggi;
//---------------------------------------------------------------------------
__fastcall TFormMessaggi::TFormMessaggi(TComponent* Owner) : TForm(Owner)
{

}
//---------------------------------------------------------------------------
//-----------------------------------------------------
// Funzione richiamata all' Attivazione della Finestra
//-----------------------------------------------------
void __fastcall TFormMessaggi::FormActivate(TObject *Sender)
{
  MessaggComboBox->ItemIndex=0;
  //-------------------------------------------
  // Messaggeria della Finestra di Messaggeria
  //-------------------------------------------
  FormMessaggi->Caption                = MESS[106];
  FormMessaggi->MessaggLabel1->Caption = MESS[107];
}
//---------------------------------------------------------------------------
void __fastcall TFormMessaggi::OkMessaggBitBtnClick(TObject *Sender)
{
  //-------------------------------------------------
  // Recupero i la Lingua Impostata
  //-------------------------------------------------
  MessaggComboBox->GetTextBuf(LinguaSistema, 4);
  //-------------------------------------------------
  // Compilazione Messaggi nella finestra principale
  //-------------------------------------------------
  FormMain->Messaggi();
  FormStato->Messaggi();
  //-------------------------------------------------
  // Registro la modifica nel Log Operatore
  //-------------------------------------------------
  FileLogOperatore("(MESSAGG) - Modifica messaggeria di sistema");
}
//---------------------------------------------------------------------------

