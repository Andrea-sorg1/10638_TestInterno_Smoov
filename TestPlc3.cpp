//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "Libreria.h"
#include "TecnoFer.h"
#include "Modbus.h"
#include "TestPlc3.h"
#include "AllenBViaVbOpc.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormTestPlc3 *FormTestPlc3;
//---------------------------------------------------------------------------
//-------------------------------
// Costruttore Classe PassWord
//-------------------------------
__fastcall TFormTestPlc3::TFormTestPlc3(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
//-----------------------
// Attivazione Finestra
//-----------------------
void __fastcall TFormTestPlc3::FormActivate(TObject *Sender)
{
  char all_mess[200];

  //--------------------------------------
  // Posizionamento Finestra
  //--------------------------------------
//FormTestPlc->Left = 20;
//FormTestPlc->Top  = 20;

  //--------------------------------------
  // Inizializzazioni
  //--------------------------------------
  GridCanali->DefaultRowHeight = 16;
  GridCanali->DefaultColWidth  = 100;
  GridCanali->ColCount         = 5;
  GridCanali->RowCount         = 11;
  UpDownCh->Position           = 0;
  memset(&Dw, 0, sizeof(Dw));
  NumPlc=1;
  DW=0;
  DB=1;
  nDW=10;
  ErroreDiComunicazione=false;
  strcpy(MessaggioErrore, "Error: none...");
  memset(&ColModificata, 0, sizeof(ColModificata));
  CheckSigned->Checked=false;
  //--------------------------------------
  // Rilettura dei dati da Plc
  //--------------------------------------
  LetturaPlc();
  //--------------------------------------
  // Messaggeria
  //--------------------------------------
  Messaggi();
  //--------------------------------------
  // Rinfresco Pagina
  //--------------------------------------
  SetCaselle();
  TimerCiclo->Enabled=true;
}
//---------------------------------------------------------------------------
//-----------------------
// Messaggeria
//-----------------------
void __fastcall TFormTestPlc3::Messaggi()
{
//  FormTestPlc3->Caption   = "Test PLC's SIEMENS";

  GridCanali->Cells[0][0] = "Channels";
  GridCanali->Cells[1][0] = "Decimal";
  GridCanali->Cells[2][0] = "Hexadecimal";
  GridCanali->Cells[3][0] = "Binary";
  GridCanali->Cells[4][0] = "Ascii";
}
//---------------------------------------------------------------------------
//-----------------------
// SetCaselle
//-----------------------
void __fastcall TFormTestPlc3::SetCaselle()
{
  short int i, auxint;
  char a, b;
  char Appoggio[51]="";
  char Decimale[31]="";
  char Esadecimale[31]="";
  char Binario[31]="";
  char Ascii[31]="";


  UpDownCh->Position   = NumPlc;
  sprintf(Appoggio, "%02d", NumPlc);
  EditPlc0->Text = Appoggio;
  //---------------------------------------
  // Visualizza dati in letura o scrittura
  //---------------------------------------
  if( !CheckBoxEnableWrite->Checked ){
     DW    = DatiPLC[ NumPlc ].R.DatoIni;
     nDW   = DatiPLC[ NumPlc ].R.NumDati;
     sprintf(Appoggio, "%s", DatiPLC[ NumPlc ].R.Item);
     EditPlc5->Text = Appoggio;
  }else{
     DW    = DatiPLC[ NumPlc ].W.DatoIni;
     nDW   = DatiPLC[ NumPlc ].W.NumDati;
     sprintf(Appoggio, "%s", DatiPLC[ NumPlc ].W.Item);
     EditPlc5->Text = Appoggio;
  }

  sprintf(Appoggio, "%03d", DB);
  EditPlc1->Text = Appoggio;
  EditPlc1->ReadOnly = true;
  sprintf(Appoggio, "%03d", DW);
  EditPlc2->Text = Appoggio;
  sprintf(Appoggio, "%02d", nDW);
  EditPlc3->Text = Appoggio;
  sprintf(Appoggio, "%s", DatiPLC[ NumPlc ].Topic);
  EditPlc4->Text = Appoggio;


  memset(&ColModificata, 0, sizeof(ColModificata));
  //--------------------------------------------
  // Rinfresco della Griglia
  //--------------------------------------------
  for(i=1; i<=65; i++){
   //sprintf(Decimale, "%u", Dw[DW+(i-1)]);
     if( CheckSigned->Checked==false ){
        sprintf(Decimale, "%u", Dw[DW+(i-1)]);
     }
     else{
        auxint = (short int)(Dw[DW+(i-1)]);
        sprintf(Decimale, "%d", auxint);
     }
     sprintf(Esadecimale, "%X", Dw[DW+(i-1)]);
     itoa(Dw[DW+(i-1)], Binario, 2);
     addzero(Binario, 16);
     a = (WORD)(Dw[DW+(i-1)]>>8);
     if(a==0) a=' ';
     b = (WORD)(Dw[DW+(i-1)] & 0x00ff);
     if(b==0) b=' ';
     sprintf(Ascii, "%c%c", a, b);
     //sprintf(Appoggio, "DW%03d", (DW+(i-1)));
     sprintf(Appoggio, "DW%03d", (i-1));
     GridCanali->Cells[0][i] = Appoggio;
     GridCanali->Cells[1][i] = Decimale;
     GridCanali->Cells[2][i] = Esadecimale;
     GridCanali->Cells[3][i] = Binario;
     GridCanali->Cells[4][i] = Ascii;
  }
  //---------------------------------------
  // Pannelli di stato comunicazione
  //---------------------------------------
  TextError->Caption = MessaggioErrore;
  //-----------------------------------------------------------------
  // Pannelli Comunicazione
  //-----------------------------------------------------------------
  if( break_Com[COM2] ){
     PanelOK->Color    = clBtnFace;
     PanelNO->Color    = clBtnFace;
     PanelINT->Color   = clYellow;
  }
  else{
     if( ErroreDiComunicazione ){
        PanelOK->Color    = clBtnFace;
        PanelNO->Color    = clRed;
        PanelINT->Color   = clBtnFace;
     }
     else{
        PanelOK->Color    = clLime;
        PanelNO->Color    = clBtnFace;
        PanelINT->Color   = clBtnFace;
     }
  }
}
//---------------------------------------------------------------------------
//-----------------------
// GetCaselle
//-----------------------
int __fastcall TFormTestPlc3::GetCaselle()
{

  short int i;
  WORD Valore;
  char Appoggio[51]="";
  char *ptr_char;
  bool SaltaScrittura;
  NumPlc   = (short int)(atoi(EditPlc0->Text.c_str()));
  DB       = (short int)(atoi(EditPlc1->Text.c_str()));
  DW       = (short int)(atoi(EditPlc2->Text.c_str()));
  nDW      = (short int)(atoi(EditPlc3->Text.c_str()));

  UpDownCh->Position   = NumPlc;
/*
  //----------------------------------------
  // Controllo attendibilità dei valori
  //----------------------------------------
  if(DB<1 || DB>255){
     strcpy(MessaggioErrore, "Error: The DB's number is incorrect (value between 1 and 255)...");
     return 1;
  }
  if(DW<0 || DW>255){
     strcpy(MessaggioErrore, "Error: The DW's number is incorrect (value between 0 and 255)...");
     return 1;
  }
  if(nDW<1 || nDW>64){
     strcpy(MessaggioErrore, "Error: The number of DW is incorrect (value between 1 and 64)...");
     return 1;
  }
*/
  //----------------------------------------
  // Dimensionamento griglia
  //----------------------------------------
  GridCanali->RowCount = nDW+1;
  //----------------------------------------
  // Recupero dei valori da settare in base
  // all'ultima cella codificara
  //----------------------------------------
  for(i=DW; i<(short int)(DW+nDW); i++){
     SaltaScrittura=false;
     strcpy(Appoggio, GridCanali->Cells[ColModificata[(i-DW)+1]][(i-DW)+1].c_str());
     Erase_White_End(Appoggio);
     Erase_White_Begin(Appoggio);
     switch(ColModificata[(i-DW)+1]){
        case   1: Valore = (WORD)(atoi(Appoggio)); break;
        case   2: Valore = (WORD)(strtol(Appoggio, &ptr_char, 16)); break;
        case   3: Valore = (WORD)(strtol(Appoggio, &ptr_char,  2)); break;
        case   4: Valore = (WORD)(Appoggio[0]<<8);
                  Valore = (WORD)(Valore) | (WORD)(Appoggio[1]);
                  break;
        default : Valore = 0;
                  SaltaScrittura=true;
                  break;
     }
     //------------------------------------------------
     // Compilazione dell'array di trasferimento al PLC
     //------------------------------------------------
     if(SaltaScrittura==false) Dw[i] = Valore;
  }

  return 0;
}
//---------------------------------------------------------------------------
//-------------------------------------
// Chiusura della Finestra da tastiera
//-------------------------------------
void __fastcall TFormTestPlc3::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Key == VK_ESCAPE ) Close();
}
//---------------------------------------------------------------------------
//-----------------------------------------------------------
// Lettura da Plc
//-----------------------------------------------------------
void __fastcall TFormTestPlc3::BtnLeggiClick(TObject *Sender)
{
  //------------------------
  // Recupero da grafica
  //------------------------
  if(GetCaselle()) return;
  // --- rilettura da Plc ---
  LetturaPlc();
  //------------------------
  // Refresh grafico
  //------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
//-----------------------------------------------------------
// Scrittura su Plc
//-----------------------------------------------------------
void __fastcall TFormTestPlc3::BtnScriviClick(TObject *Sender)
{
  //------------------------
  // Recupero da grafica
  //------------------------
  if(GetCaselle()) return;
  // --- riscrittura su Plc ---
  ScritturaPlc();
  //------------------------
  // Refresh grafico
  //------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
//-----------------------------------------------------------
// Selezione della casella modificata
//-----------------------------------------------------------
void __fastcall TFormTestPlc3::GridCanaliSetEditText(TObject *Sender, int ACol, int ARow, const AnsiString Value)
{
  switch(ACol){
     case   1: ColModificata[ARow]=1; break;   // Modificato il valore decimale
     case   2: ColModificata[ARow]=2; break;   // Modificato il valore esadecimale
     case   3: ColModificata[ARow]=3; break;   // Modificato il valore binario
     case   4: ColModificata[ARow]=4; break;   // Modificato il valore ascii
     default : ColModificata[ARow]=0; break;   // nessuna modifica
  }
}
//---------------------------------------------------------------------------
//-----------------------
// LetturaPlc
//-----------------------
void __fastcall TFormTestPlc3::LetturaPlc()
{
  //int err=0;

  memset(&Dw, 0, sizeof(Dw));
  ErroreDiComunicazione=false;
  //-----------------------------------------------------------
  // Fase di lettura tramite file verso applicazione VB
  //-----------------------------------------------------------
  //memset( &DataReadOPC, 0, sizeof( DataReadOPC));
  //err = AllenB_LetturaPlc( MessaggioErrore ); vengono letti ricorsivamente

  if( NumPlc< AllenbPlcIni ) NumPlc = AllenbPlcIni;
  if( NumPlc> AllenbTotPlc ) NumPlc = AllenbTotPlc;
  if( !CheckBoxEnableWrite->Checked ){
     //if( DatiPLC[ NumPlc ].R.DatiOk ){
        for( int i=0; i< TOT_READ_WORD; i++ ){
           if( i<=DatiPLC[ NumPlc ].R.NumDati ){
              Dw[ i+DW ] = DatiPLC[ NumPlc ].R.Data[i];
           }
        }
     //}
  }else{
     for( int i=0; i< TOT_WRITE_WORD; i++ ){
        if( i<=DatiPLC[ NumPlc ].W.NumDati ){
           Dw[ i+DW ] = DatiPLC[ NumPlc ].W.Data[i];
        }
     }
  }

  if( DatiPLC[ NumPlc ].ErrorePersistente ) ErroreDiComunicazione=true;
  if(!DatiPLC[ NumPlc ].ErrorePersistente ) strcpy(MessaggioErrore, "Error: none...");
  //if( DatiPLC[ NumPlc ].R.DatiOk==0 ) ErroreDiComunicazione=true;
  //if( DatiPLC[ NumPlc ].R.DatiOk!=0                     ) strcpy(MessaggioErrore, "Error: none...");
}
//---------------------------------------------------------------------------
//-----------------------
// ScritturaPlc
//-----------------------
void __fastcall TFormTestPlc3::ScritturaPlc()
{
/*
  int err=0;

  ErroreDiComunicazione=false;
  //-----------------------------------------------------------
  // Fase di scrittura tramite protocollo 3964 della Siemens
  //-----------------------------------------------------------
  //  err = SIM.protP3964r(COM1, SCRITTURA, DB, DW, nDW, &Dw[0], MessaggioErrore);

  //-----------------------------------------------------------
  // Fase di scrittura tramite file verso applicazione VB
  //-----------------------------------------------------------
  memset( &DataWriteOPC, 0, sizeof( DataWriteOPC));
  for( int i=1; i<= MAX_PLC_AB; i++ ){
     DataWriteOPC.Plc[ i ].Indice   = i;
     DataWriteOPC.Plc[ i ].NumDati  = TOT_WRITE_WORD;
  }
  for( int i=0; i< TOT_WRITE_WORD; i++ ){
     DataWriteOPC.Plc[ NumPlc ].Data[i] = Dw[ i ];
  }

  err = AllenB_SctritturaFile( MessaggioErrore );
  //-----------------------------------------------------------
  if( err) ErroreDiComunicazione=true;
  if(!err) strcpy(MessaggioErrore, "Error: none...");
*/
}
//---------------------------------------------------------------------------
void __fastcall TFormTestPlc3::GridCanaliGetEditMask(TObject *Sender,
      int ACol, int ARow, AnsiString &Value)
{
  switch(ACol){
   //case  1: Value = "!99999;1; "; break;
     case  1: if( CheckSigned->Checked==false ){
                 Value = "!99999;1; ";
              }
              else{
                 Value = "!#9999;1; ";
              }
              break;
     case  2: Value = ">cccc;1; "; break;
     case  3: Value = "!9999999999999999;1; "; break;
     case  4: Value = ">cc;1; "; break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormTestPlc3::CheckBoxEnableWriteClick(TObject *Sender)
{


  SetCaselle();
  GetCaselle();
  SetCaselle();
}

//---------------------------------------------------------------------------

void __fastcall TFormTestPlc3::UpDownChClick(TObject *Sender,
      TUDBtnType Button)
{
  NumPlc = UpDownCh->Position;
  //---------------------------
  // Lettura del PLC
  //---------------------------
  LetturaPlc();
  SetCaselle();
  GetCaselle();
  SetCaselle();

}
//---------------------------------------------------------------------------

void __fastcall TFormTestPlc3::TimerCicloTimer(TObject *Sender)
{
  TimerCiclo->Enabled=false;

  //------------------------
  // Recupero da grafica
  //------------------------
  if(GetCaselle()) return;
  // --- rilettura da Plc ---
  LetturaPlc();
  //------------------------
  // Refresh grafico
  //------------------------
  SetCaselle();

  TimerCiclo->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormTestPlc3::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  TimerCiclo->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormTestPlc3::CheckSignedClick(TObject *Sender)
{
  GetCaselle();
  SetCaselle();
}
//---------------------------------------------------------------------------

