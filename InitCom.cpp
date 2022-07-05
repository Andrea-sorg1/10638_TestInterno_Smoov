//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "InitCom.h"
#include "OmronETH.h"
#include "Libreria.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormSetCom *FormSetCom;
//---------------------------------------------------------------------------
__fastcall TFormSetCom::TFormSetCom(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormSetCom::FormCreate(TObject *Sender)
{
  short int i;

  //---------------------------------------
  // Aggiornamento COMBO delle seriali
  //---------------------------------------
  for(i=0; i<=MAXCOM; i++){
     ListaCOM->Items->Add(InCom[i].DescrCom);
  }
  ListaCOM->ItemIndex=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormSetCom::FormActivate(TObject *Sender)
{
  //---------------------------------------
  // Rinfresco della form
  //---------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
// ---------------------------
//    GetCaselle()
// ---------------------------
// Funzione che legge dalle caselle di dialogo (TEdit) di WINDOWS
// e scarica sulla struttura dei PLC
//
void __fastcall TFormSetCom::GetCaselle()
{
  char all_mess[101]="";

  if( Index>MAXCOM ) return;

  ListaCOM->GetTextBuf(all_mess, 30);
  InCom[ Index ].DescrCom = StrUpper(all_mess);
  switch(BoxBaudrate->ItemIndex){
     case  0: InCom[ Index ].Baudrate=   1200; break;
     case  1: InCom[ Index ].Baudrate=   2400; break;
     case  2: InCom[ Index ].Baudrate=   4800; break;
     case  3: InCom[ Index ].Baudrate=   9600; break;
     case  4: InCom[ Index ].Baudrate=  19200; break;
     case  5: InCom[ Index ].Baudrate=  38400; break;
     case  6: InCom[ Index ].Baudrate=  57600; break;
     case  7: InCom[ Index ].Baudrate= 115200; break;
     default: InCom[ Index ].Baudrate=   9600; break;
  }
  switch(BoxDati->ItemIndex){
     case  0: InCom[ Index ].Dati= 7; break;
     case  1: InCom[ Index ].Dati= 8; break;
     default: InCom[ Index ].Dati= 7; break;
  }
  switch(BoxParita->ItemIndex){
     case  0: InCom[ Index ].Parita= 'E'; break;
     case  1: InCom[ Index ].Parita= 'O'; break;
     case  2: InCom[ Index ].Parita= 'N'; break;
     default: InCom[ Index ].Parita= 'E'; break;
  }
  switch(BoxStopBit->ItemIndex){
     case  0: InCom[ Index ].StopBit= 1; break;
     case  1: InCom[ Index ].StopBit= 2; break;
     default: InCom[ Index ].StopBit= 2; break;
  }
  //----------------------------------------------
  // Gestione del CTS/RTS
  //----------------------------------------------
  InCom[ Index ].NoCtsRts = !BoxCtsRts->Checked;
}
//---------------------------------------------------------------------------
// ---------------------------
//    SetCaselle()
// ---------------------------
// Funzione che forza i nuovi valori nelle caselle di dialogo (TEdit)
// di WINDOWS recuperandoli dalla struttura dei PLC
//
void __fastcall TFormSetCom::SetCaselle()
{
  if( Index>MAXCOM ) return;

  //------------------------------------------------
  // Seriale
  //------------------------------------------------
  ListaCOM->ItemIndex = Index;
  switch(InCom[ Index ].Baudrate){
     case    1200: BoxBaudrate->ItemIndex=0; break;
     case    2400: BoxBaudrate->ItemIndex=1; break;
     case    4800: BoxBaudrate->ItemIndex=2; break;
     case    9600: BoxBaudrate->ItemIndex=3; break;
     case   19200: BoxBaudrate->ItemIndex=4; break;
     case   38400: BoxBaudrate->ItemIndex=5; break;
     case   57600: BoxBaudrate->ItemIndex=6; break;
     case  115200: BoxBaudrate->ItemIndex=7; break;
     default     : BoxBaudrate->ItemIndex=3;
  }
  switch(InCom[ Index ].Dati){
     case  7: BoxDati->ItemIndex=0; break;
     case  8: BoxDati->ItemIndex=1; break;
     default: BoxDati->ItemIndex=0;
  }
  switch(InCom[ Index ].Parita){
     case  'E': BoxParita->ItemIndex=0; break;
     case  'O': BoxParita->ItemIndex=1; break;
     case  'N': BoxParita->ItemIndex=2; break;
     default  : BoxParita->ItemIndex=0;
  }
  switch(InCom[ Index ].StopBit){
     case  1: BoxStopBit->ItemIndex=0; break;
     case  2: BoxStopBit->ItemIndex=1; break;
     default: BoxStopBit->ItemIndex=1;
  }
  //----------------------------------------------
  // Gestione del CTS/RTS
  //----------------------------------------------
  BoxCtsRts->Checked = !InCom[ Index ].NoCtsRts;
}
//---------------------------------------------------------------------------
void __fastcall TFormSetCom::BtnAttivaClick(TObject *Sender)
{
  WORD err;
  int Baudrate;
  char Parita;
  WORD Dati;
  WORD StopBit;
  WORD Port;
  char all_mess[101]="";

  //----------------------------------
  // Aggiornamento della struttura
  //----------------------------------
  GetCaselle();

  //----------------------------------
  // Inizializzazione struttura
  //----------------------------------
  Port = (WORD)(Index);
  Baudrate = InCom[ Index ].Baudrate;
  Dati     = InCom[ Index ].Dati;
  StopBit  = InCom[ Index ].StopBit;
  Parita   = InCom[ Index ].Parita;
  //err = WINCOM[Index].init_port(Port, Baudrate, Dati, Parita, StopBit, 300);
  err = init_port(Port, Baudrate, Dati, Parita, StopBit, 300);
  if(err){
     sprintf(all_mess, "Errore inizializzazione COM%d.", Port+1);
     ShowMessage(all_mess);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormSetCom::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  if( Key == VK_ESCAPE ) Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormSetCom::ListaCOMClick(TObject *Sender)
{
  Index = ListaCOM->ItemIndex;
  SetCaselle();
}
//---------------------------------------------------------------------------


