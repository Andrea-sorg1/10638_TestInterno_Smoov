//---------------------------------------------------------------------------
#include <vcl.h>
#include <io.h>
#pragma hdrstop

#include "Visfile.h"
#include "Tecnofer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormVisualizzaFile *FormVisualizzaFile;
//---------------------------------------------------------------------------
__fastcall TFormVisualizzaFile::TFormVisualizzaFile(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFormVisualizzaFile::FormActivate(TObject *Sender)
{
  // --- Inizializzazione delle variabili globali ---
  strcpy(NomeFile,         NomeFileVisualizza);
  strcpy(IntestazioneFile, IntestazioneFileVisualizza);
  strcpy(CaptionFile,      CaptionFileVisualizza);
  BtnAzzera->Glyph->LoadFromFile("trash.bmp");
  VisualizzaFileRichEdit->Clear();

  // --- imposta i messaggi degli oggetti ---
  Messaggi();

  // --- Lettura struttura da file  ---
  LeggiDati();
}
//---------------------------------------------------------------------------

// +--------------------------------------------------+
// | Intercettazione dei Pulsanti premuti da Tastiera |
// +--------------------------------------------------+
void __fastcall TFormVisualizzaFile::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Shift.Contains(ssAlt) && Key==VK_F4 ) return;

  switch (Key){ case VK_ESCAPE: Close(); break; }
}
//---------------------------------------------------------------------------

// +------------------------------------------+
// | Imposta i messaggi abbinati agli oggetti |
// +------------------------------------------+
void __fastcall TFormVisualizzaFile::Messaggi()
{
  // --- Intestazione Form ---
  FormVisualizzaFile->Caption = CaptionFile;
  PanelIntestazione->Caption  = IntestazioneFile;
}

//---------------------------------------------------------------------------

// +------------------------+
// | Legge i Dati dai file  |
// +------------------------+
void __fastcall TFormVisualizzaFile::LeggiDati()
{
  char buffer[201]="";

  try{
     VisualizzaFileRichEdit->Lines->LoadFromFile(NomeFile);
  }
  catch (...){
     sprintf(buffer, MESS[112], NomeFile);
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
}
//---------------------------------------------------------------------------

// ---------------------------
//    Stampa()
// ---------------------------
//
void __fastcall TFormVisualizzaFile::Stampa()
{
  FILE *ptrfile1;
  FILE *ptrfile2;
  char buffer[101]="";
  char stringa1[101]="";
  char stringa2[101]="";
  short int i=0;
  int err=0;

  //-----------------------------------------
  // Apertura del File originale in Lettura
  //-----------------------------------------
  ptrfile1 = fopen(NomeFile, "rb");
  if(ptrfile1==0){
     sprintf(buffer, MESS[98], NomeFile);
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  //-----------------------------------------
  // Apertura del File appoggio in Scrittura
  //-----------------------------------------
  ptrfile2 = fopen("STAMPA.TMP", "wb");
  if(ptrfile2==0){
     sprintf(buffer, MESS[98], NomeFile);
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  //----------------------------------------
  // Scrittura dell'intestazione nel File di
  // Stampa.
  //----------------------------------------
  strcpy(stringa2, "=================================================================================");
  Write_Line(ptrfile2, stringa2);
  Write_Line(ptrfile2, IntestazioneFile);
  Write_Line(ptrfile2, stringa2);
  strcpy(stringa2, "                                                                                ");
  Write_Line(ptrfile2, stringa2);
  //----------------------------------------
  // Copiatura del File Origine nel File di
  // Stampa.
  //----------------------------------------
  for(i=0;;i++){
     err = !Read_Line(ptrfile1, stringa1, 151);
     if(!err) Write_Line(ptrfile2, stringa1);
     else break;
  }

  //----------------------------------------
  // Chiusura del File originale e del File
  // di Stampa.
  //----------------------------------------
  fclose( ptrfile1 );
  fclose( ptrfile2 );


//VisualizzaFileRichEdit->Font->Name = "Fixedsys";
  //---------------------------------------
  // Dimensionamento caratteri
  //---------------------------------------
  VisualizzaFileRichEdit->Font->Size  = 10;
  VisualizzaFileRichEdit->Font->Color = clBlack;


  //---------------------------------------
  // Carico a monitor del File di Stampa
  //---------------------------------------
  try{
     VisualizzaFileRichEdit->Lines->LoadFromFile("STAMPA.TMP");
  }
  catch (...){
     sprintf(buffer, MESS[98], NomeFile);
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }

  //---------------------------------------
  // Stampa del file completo
  //---------------------------------------
  try{

     VisualizzaFileRichEdit->Print("STAMPA.TMP");

  }
  catch (...){
     sprintf(buffer, MESS[98], NomeFile);
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  //---------------------------------------
  // Carico a monitor del File Originale
  //---------------------------------------
  try{
     VisualizzaFileRichEdit->Lines->LoadFromFile(NomeFile);
  }
  catch (...){
     sprintf(buffer, MESS[98], NomeFile);
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }

  VisualizzaFileRichEdit->Font->Size  = 12;
  VisualizzaFileRichEdit->Font->Color = clYellow;
/*
  //----------------------------------------------
  // Gestione della stampa di una riga alla volta
  // ( da mettere a punto ).
  //----------------------------------------------

  short int i;
  short int pagina;
  char stringa[201];
  TPrinter *Prntr = Printer();

  VisualizzaFileRichEdit->Font->Size = 10;

  Prntr->BeginDoc();
  pagina=0;
  for(i=0;;i++){
     if(!strcmp(VisualizzaFileRichEdit->Lines->Strings[i].c_str(), "")) break;
     strcpy(stringa, VisualizzaFileRichEdit->Lines->Strings[i].c_str());
     Prntr->Canvas->Font->Assign(VisualizzaFileRichEdit->Font);
     Prntr->Canvas->TextOut(0, (pagina*50), stringa);
     if(pagina==67){
        Prntr->NewPage();
        pagina=-1;
     }
     pagina++;
  }
  Prntr->EndDoc();
*/
  return ;
}
//---------------------------------------------------------------------------

// ---------------------------
//    Azzera()
// ---------------------------
//
void __fastcall TFormVisualizzaFile::Azzera()
{
  char all_mess[201]="";
  int status=0;

  //----------------------------------------
  // Cancellazione fisica del file
  //----------------------------------------
  unlink(NomeFile);

  //----------------------------------------
  // Test accesso al file appena cancellato
  //----------------------------------------
  status = access(NomeFile, 0);
  if(status==0){
     BtnAzzera->Glyph->LoadFromFile("trash.bmp");  
     sprintf(all_mess, MESS[112], NomeFile);
     Application->MessageBox(all_mess, MESS[99], MB_OK | MB_ICONINFORMATION);
  }
  else{
     BtnAzzera->Glyph->LoadFromFile("trashful.bmp");
     Application->MessageBox(MESS[438], MESS[99], MB_OK | MB_ICONINFORMATION);
     //----------------------------------------
     // Chiudo la pagina
     //----------------------------------------
     Close();
  }
  return ;
}
//---------------------------------------------------------------------------

// +--------------------------------------------------------+
// | Stampa del file NomeFile                               |
// +--------------------------------------------------------+
void __fastcall TFormVisualizzaFile::BtnStampaClick(TObject *Sender)
{
  Stampa();
}
//---------------------------------------------------------------------------

// +--------------------------------------------------------+
// | Azzeramento temporaneo del File visualizzato           |
// +--------------------------------------------------------+
void __fastcall TFormVisualizzaFile::BtnAzzeraClick(TObject *Sender)
{
  Azzera();
}
//---------------------------------------------------------------------------

void __fastcall TFormVisualizzaFile::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
  VisualizzaFileRichEdit->Clear();
}
//---------------------------------------------------------------------------


