//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "Password.h"
#include "Tecnofer.h"
#include "Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormPassword *FormPassword;
//---------------------------------------------------------------------------
//-------------------------------
// Costruttore Classe PassWord
//-------------------------------
__fastcall TFormPassword::TFormPassword(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
//-----------------------
// Attivazione Finestra
//-----------------------
void __fastcall TFormPassword::FormActivate(TObject *Sender)
{
  PassWordEdit->Text = "";
  PassWordEdit->SetFocus();
  //---------------------------------------
  // Messaggeria della Finestra Password
  //---------------------------------------
  FormPassword->Caption                 = MESS[101];
  FormPassword->PassWordLabel->Caption  = MESS[102];
}
//---------------------------------------------------------------------------
//-------------------------------------
// Chiusura della Finestra da tastiera
//-------------------------------------
void __fastcall TFormPassword::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  if( Key == VK_ESCAPE ) Close();
}
//---------------------------------------------------------------------------

void __fastcall TFormPassword::BottPassOKClick(TObject *Sender)
{
  Password = PassWordEdit->Text.UpperCase();
}
//---------------------------------------------------------------------------

void __fastcall TFormPassword::BottPassCancelClick(TObject *Sender)
{
  //-------------------------------------------
  // se premuto cancel, azzera il testo immesso
  //-------------------------------------------
  FormPassword->Text = "";
}
//---------------------------------------------------------------------------




