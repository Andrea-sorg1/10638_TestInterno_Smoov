//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Input.h"
#include "Tecnofer.h"
#include "Remoto.h"
#include "SQLDB.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormInput *FormInput;
//---------------------------------------------------------------------------
__fastcall TFormInput::TFormInput(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormInput::FormActivate(TObject *Sender)
{
  char buffer[51]="";
  char data[11];
  char ora[11];
  char Date[11]="";
  char Time[11]="";
  int  vOffset=0;
  long int tempo;

  //---------------------------------
  // Caption della form
  //---------------------------------
  FormInput->Caption = "Input Form " + IntToStr(Tipo);
  cbTipoMac->Items->Clear();
  cbTipoMac->Items->Add(MESS[618]);
  cbTipoMac->Items->Add(MESS[619]);
  lblTipo->Caption = MESS[684];
  //---------------------------------
  // Inizializzazione della Form
  //---------------------------------
  cbTipoMac->Visible      = false;
  lblMacchina->Visible    = false;
  edNumMac->Visible       = false;
  edTipo->Visible         = false;
  lblTipo->Visible        = false;

  lblTimeStart->Caption   = "Time Start";
  lblTimeStart->Visible   = false;
  edDataStart->Visible    = false;
  edOraStart->Visible     = false;

  lblTimeEnd->Caption     = "Time End";
  lblTimeEnd->Visible     = false;
  edDataEnd->Visible      = false;
  edOraEnd->Visible       = false;

  lblInputString->Visible = false;
  txtEdit->Visible        = false;
  txtEdit1->Visible       = false;

  Check1->Visible         = false;
  Check1->State           = cbUnchecked;
  Check2->Visible         = false;
  Check2->State           = cbUnchecked;
  RadioButton1->Visible   = false;
  RadioButton2->Visible   = false;

  Bevel1->Visible         = false;
  Bevel2->Visible         = false;
  Bevel3->Visible         = false;

  Bevel4->Visible         = true;
  BtnOK->Visible          = true;
  BtnCancel->Visible      = true;

  DateTimePickerStart->Visible = false;
  DateTimePickerEnd->Visible   = false;

//-------------------------------------------------
// Coordinate Verticali di default oggetti mobili
//-------------------------------------------------
  Bevel1->Top          =  68;
  Bevel2->Top          = 125;
  Bevel3->Top          = 180;
  Bevel4->Top          = 270;

  lblMacchina->Top     =  20;
  edNumMac->Top        =  15;
  lblTipo->Top         =  20;
  edTipo->Top          =  15;
  cbTipoMac->Top       =  43;

  lblTimeStart->Top    =  80;
  edDataStart->Top     =  75;
  edOraStart->Top      =  75;

  lblTimeEnd->Top      = 135;
  edDataEnd->Top       = 130;
  edOraEnd->Top        = 130;

  lblInputString->Top  = 190;
  txtEdit->Top         = 210;
  Check1->Top          = 233;
  Check2->Top          = 253;
  RadioButton1->Top    = 233;
  RadioButton2->Top    = 253;

  BtnOK->Top           = 280;
  BtnCancel->Top       = 280;
  FormInput->Height    = 355;

  switch(Tipo){
     case  1: lblMacchina->Caption = MESS[21]; // TGV  + Start
              edNumMac->Visible       = true;
              lblMacchina->Visible    = true;
              lblTimeStart->Visible   = true;
              edDataStart->Visible    = true;
              edOraStart->Visible     = true;
              Bevel1->Visible         = true;
              vOffset = -25;
              Bevel1->Top       += vOffset;
              lblTimeStart->Top += vOffset;
              edDataStart->Top  += vOffset;
              edOraStart->Top   += vOffset;
              vOffset = -170;
              break;

     case  2: cbTipoMac->Visible      = true;  // Car/Scar + Start + End
              lblTimeStart->Visible   = true;
              edDataStart->Visible    = true;
              edOraStart->Visible     = true;
              lblTimeEnd->Visible     = true;
              edDataEnd->Visible      = true;
              edOraEnd->Visible       = true;
              Bevel1->Visible         = true;
              Bevel2->Visible         = true;
              vOffset = -30;
              Bevel1->Top       += vOffset;
              Bevel2->Top       += vOffset;
              cbTipoMac->Top    += vOffset;

              lblTimeStart->Top += vOffset;
              edDataStart->Top  += vOffset;
              edOraStart->Top   += vOffset;

              lblTimeEnd->Top   += vOffset;
              edDataEnd->Top    += vOffset;
              edOraEnd->Top     += vOffset;

              vOffset = -120;
              break;

     case  3: txtEdit->Visible      = true;  // Input Box
              txtEdit->SetFocus();
              lblInputString->Visible = true;
              RadioButton1->Visible = true;
              RadioButton2->Visible = true;
              if(IdPC==1) Check1->Visible = true;
              vOffset = -180;
              lblInputString->Top  += vOffset;
              Check1->Top          += vOffset;
              Check2->Top          += vOffset;
              RadioButton1->Top    += vOffset;
              RadioButton2->Top    += vOffset;
              txtEdit->Top         += vOffset;
              vOffset = -190;
              break;

     case  4: txtEdit->Visible      = true;  // Input Box senza Radio Button
              txtEdit->SetFocus();
              lblInputString->Visible = true;
              if(IdPC==1) Check1->Visible = true;
              vOffset = -180;
              lblInputString->Top  += vOffset;
              Check1->Top          += vOffset;
              Check2->Top          += vOffset;
              txtEdit->Top         += vOffset;
              vOffset = -190;
              break;

     case  5: txtEdit1->Visible       = true;
              lblTimeStart->Visible   = true;
              edDataStart->Visible    = true;
              edOraStart->Visible     = true;
              lblTimeEnd->Visible     = true;
              edDataEnd->Visible      = true;
              edOraEnd->Visible       = true;
              Bevel1->Visible         = true;
              Bevel2->Visible         = true;
              vOffset = -30;
              Bevel1->Top       += vOffset;
              Bevel2->Top       += vOffset;

              lblTimeStart->Top += vOffset;
              edDataStart->Top += vOffset;
              edOraStart->Top  += vOffset;

              lblTimeEnd->Top   += vOffset;
              edDataEnd->Top   += vOffset;
              edOraEnd->Top    += vOffset;

              vOffset = -120;
              break;

     case  6:
              lblTimeStart->Caption   = "DATE";
              lblTimeStart->Visible   = true;
              edDataStart->Visible    = true;
              edOraStart->Visible     = true;
              Bevel1->Visible         = true;
              vOffset = -25;
              Bevel1->Top       += vOffset;
              lblTimeStart->Top += vOffset;
              edDataStart->Top  += vOffset;
              edOraStart->Top   += vOffset;
              vOffset = -170;
              break;
  }
  Bevel4->Top       += vOffset;
  BtnOK->Top        += vOffset;
  BtnCancel->Top    += vOffset;
  FormInput->Height += vOffset;

  //-----------------------------------------------------------------------
  // Abilita Visualizzazione TimePicker
  //-----------------------------------------------------------------------
  if(edDataStart->Visible==true){
     DateTimePickerStart->Visible = true;
     DateTimePickerStart->Top     = edDataStart->Top+25;
     DateTimePickerStart->Date    = Today();
  }
  if(edDataEnd->Visible==true){
     DateTimePickerEnd->Visible   = true;
     DateTimePickerEnd->Top       = edDataEnd->Top+25;
     DateTimePickerEnd->Date      = Today();
  }
  //---------------------------------
  // Inizializzazione variabili
  //---------------------------------
  Index       = 1;
  TypMac      = 1;
  TypCarScar  = 0;
  cbTipoMac->ItemIndex = 0;

  sprintf(buffer, "%02d", Index);
  edNumMac->Text = buffer;

  sprintf(buffer, "%03d", TypCarScar);
  edTipo->Text = buffer;

  if(Tipo!=6){
     OraData(Time, Date);
     strcpy(DataS, Date);
     strcpy(DataE, Date);
     strcpy(OraS, "00.00.00");
     strcpy(OraE, "23.59.59");

     edDataStart->Text = DataS;
     edDataEnd->Text   = DataE;
     edOraStart->Text  = OraS;
     edOraEnd->Text    = OraE;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormInput::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Shift.Contains(ssAlt) && Key==VK_F4 ) return;

  switch (Key){ case VK_ESCAPE: Close(); break; }
}
//---------------------------------------------------------------------------
void __fastcall TFormInput::BtnOKClick(TObject *Sender)
{
  long int tempo;

  Index = atoi(edNumMac->Text.c_str());
  //-------------------------------
  // Tipo di macchina
  //-------------------------------
  TypMac = cbTipoMac->ItemIndex;
  //-------------------------------
  // Tipo di scarico
  //-------------------------------            
  TypCarScar = atoi(edTipo->Text.c_str());
  //-------------------------------
  // Data di partenza
  //-------------------------------
  strcpy(DataS, edDataStart->Text.c_str());
  strcpy(OraS,  edOraStart->Text.c_str());
  timechar_to_timeint(&tempo, &DataS[0], &OraS[0]);
  TimeS = (int)(tempo);
  //-------------------------------
  // Data di arrivo
  //-------------------------------
  strcpy(DataE, edDataEnd->Text.c_str());
  strcpy(OraE,  edOraEnd->Text.c_str());
  timechar_to_timeint(&tempo, &DataE[0], &OraE[0]);
  TimeE = (int)(tempo);
}
//---------------------------------------------------------------------------
void __fastcall TFormInput::DateTimePickerSEChange(TObject *Sender)
{
  int  myTag;
  char Data[11]="";
  char Ora[11]="";
  long int tempo=0;

  myTag = ((TDateTimePicker *)Sender)->Tag;

  if(myTag==0){
     tempo=FormSQLDB->Algo_DateTime_To_TimeInt(DateTimePickerStart->Date);
     timeint_to_timechar(&tempo, &Data[0], &Ora[0]);
     edDataStart->Text = Data;
  }
  if(myTag==1){
     tempo=FormSQLDB->Algo_DateTime_To_TimeInt(DateTimePickerEnd->Date);
     timeint_to_timechar(&tempo, &Data[0], &Ora[0]);
     edDataEnd->Text = Data;
  }
}
//---------------------------------------------------------------------------

