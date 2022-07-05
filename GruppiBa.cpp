//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>

#pragma hdrstop

#include "TecnoFer.h"
#include "GruppiBa.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormGruppiBa *FormGruppiBa;
//---------------------------------------------------------------------------
__fastcall TFormGruppiBa::TFormGruppiBa(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormGruppiBa::FormActivate(TObject *Sender)
{
  short int i;

  //-----------------------------------
  // Caption della form
  //-----------------------------------
  FormGruppiBa->Caption = MESS[715];
  //-----------------------------------
  // Compliazione array GRUPPI AMMESSI
  //-----------------------------------
  CBGruppo->Clear();
  CBGruppo->Items->Add("NONE");
  for(i=1; i<MAXGRUPPIIMP; i++){
     if(strcmp(P.GruppiBaie[i].Nome, "")==0) break;  // interrombo la primo non compilato
     CBGruppo->Items->Add(P.GruppiBaie[i].Nome);
  }
  CBGruppo->ItemIndex=0;
  //-----------------------------------
  // Refresh
  //-----------------------------------
  GbGroupConfig->Enabled=false;
  CBTipo->ItemIndex = 0;
  EdPriorita->Text  = "00";
  PanCol->Color     = clBtnFace;
}
//---------------------------------------------------------------------------
void __fastcall TFormGruppiBa::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  if( Key==VK_ESCAPE ) Close();
}
//---------------------------------------------------------------------------

void __fastcall TFormGruppiBa::CBGruppoChange(TObject *Sender)
{
  char buffer[101]="";
  int  group = CBGruppo->ItemIndex;

  if(group>GRUPPO_FUORI_INGOMBRO){
     GbGroupConfig->Enabled=true;
     CBTipo->ItemIndex = P.GruppiBaie[group].Tipo;
     sprintf(buffer, "%02d", P.GruppiBaie[group].Priorita);
     EdPriorita->Text  = buffer;
     PanCol->Color     = P.GruppiBaie[group].Col;
  }
  else{
     GbGroupConfig->Enabled=false;
     CBTipo->ItemIndex = 0;
     EdPriorita->Text  = "00";
     PanCol->Color     = clBtnFace;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormGruppiBa::BtnOkClick(TObject *Sender)
{
  int err=0;
  char all_mess[101]="";
  int group = CBGruppo->ItemIndex;

  //---------------------------------------------
  // Recurpero dati dall'editor
  //---------------------------------------------
  P.GruppiBaie[group].Tipo     = CBTipo->ItemIndex;
  P.GruppiBaie[group].Priorita = (short int)(atoi(EdPriorita->Text.c_str()));
  P.GruppiBaie[group].Col      = PanCol->Color;
  //---------------------------------------------
  // Salvataggio variazioni su file
  //---------------------------------------------
  err = P.save_file_gruppibaie(all_mess);
  if(err) ShowMessage(all_mess);
}
//---------------------------------------------------------------------------
void __fastcall TFormGruppiBa::PanColDblClick(TObject *Sender)
{
  //--------------------------------------------------
  // Recupero del Colore selezionato
  //--------------------------------------------------
  if(ColorDialog1->Execute()==mrOk){
     PanCol->Color = ColorDialog1->Color;
  }
}
//---------------------------------------------------------------------------


