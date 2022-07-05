//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AbbPLC.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormAbbPlc *FormAbbPlc;
//---------------------------------------------------------------------------
__fastcall TFormAbbPlc::TFormAbbPlc(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormAbbPlc::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  switch (Key) {
     case VK_ESCAPE: Close();    break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormAbbPlc::FormActivate(TObject *Sender)
{
  char buffer[21]="";

  //---------------------------------------------
  // Refresh della finestra principale
  //---------------------------------------------
  sprintf(buffer, "%02d", PlcData.Plc);
  EditPlc->Text = buffer;
  // parametri di lettura
  sprintf(buffer, "%03d", PlcData.RD_NumDB);
  EditPlcRD1->Text = buffer;
  sprintf(buffer, "%04d", PlcData.RD_NrChIni);
  EditPlcRD2->Text = buffer;
  sprintf(buffer, "%03d", PlcData.RD_NrCh);
  EditPlcRD3->Text = buffer;
  // parametri di scrittura
  sprintf(buffer, "%03d", PlcData.WR_NumDB);
  EditPlcWR1->Text = buffer;
  sprintf(buffer, "%04d", PlcData.WR_NrChIni);
  EditPlcWR2->Text = buffer;
  sprintf(buffer, "%03d", PlcData.WR_NrCh);
  EditPlcWR3->Text = buffer;
  //---------------------------------------------
  // Abilitazione del Timer della Form
  //---------------------------------------------
  TimerClose->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormAbbPlc::FormClose(TObject *Sender, TCloseAction &Action)
{
  //----------------------------------------------
  // Diabilitazione del Timer di chiusura
  //----------------------------------------------
  TimerClose->Enabled=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormAbbPlc::BtnOKClick(TObject *Sender)
{
  //---------------------------------------------
  // Refresh della finestra principale
  //---------------------------------------------
  PlcData.Plc = atoi(EditPlc->Text.c_str());
  // parametri di lettura
  PlcData.RD_Area    = 1; // FORZATURA "DB"
  PlcData.RD_NumDB   = atoi(EditPlcRD1->Text.c_str());
  PlcData.RD_NrChIni = atoi(EditPlcRD2->Text.c_str());
  PlcData.RD_NrCh    = atoi(EditPlcRD3->Text.c_str());
  // parametri di scrittura
  PlcData.WR_Area    = 1; // FORZATURA "DB"
  PlcData.WR_NumDB   = atoi(EditPlcWR1->Text.c_str());
  PlcData.WR_NrChIni = atoi(EditPlcWR2->Text.c_str());
  PlcData.WR_NrCh    = atoi(EditPlcWR3->Text.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TFormAbbPlc::AzzeraTimerClick(TObject *Sender)
{
  //------------------------------------------------------
  // Disabilita/Abilita Timer per riprendere il conteggio
  // del tempo
  //------------------------------------------------------
  TimerClose->Enabled = false;
  TimerClose->Enabled = true;
}
//---------------------------------------------------------------------------


