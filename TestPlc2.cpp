//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "Libreria.h"
#include "TecnoFer.h"
#include "Modbus.h"
#include "ComunicaSiemens.h"
#include "TestPlc2.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormTestPlc2 *FormTestPlc2;
//---------------------------------------------------------------------------
//-------------------------------
// Costruttore Classe PassWord
//-------------------------------
__fastcall TFormTestPlc2::TFormTestPlc2(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
//-----------------------
// Attivazione Finestra
//-----------------------
void __fastcall TFormTestPlc2::FormActivate(TObject *Sender)
{
//char all_mess[200];

  //--------------------------------------
  // Inizializzazioni
  //--------------------------------------
  ErroreDiComunicazione=false;
  memset(&Dw, 0, sizeof(Dw));
  NumPlc=11;
  DW=0;
  DB=45;
  nDW=49;
  GridCanali->DefaultRowHeight = 16;
  GridCanali->DefaultColWidth  = 100;
  GridCanali->ColCount         = 5;
  GridCanali->RowCount         = nDW+1;
  strcpy(MessaggioErrore, "Error: none...");
  memset(&ColModificata, 0, sizeof(ColModificata));
  CheckSigned->Checked=false;
  //--------------------------------------
  // Inizializza porta
  //--------------------------------------
//if(CBEth->Checked==false){
//   MODBUS_InitPort( PLC_MACCHINE, all_mess );
//}
  //--------------------------------------
  // Rilettura dei dati da Plc
  //--------------------------------------
  if(OmrPlc[NumPlc].Com==COM_SIEMENS){
     LetturaPlc();
  }
  //--------------------------------------
  // Messaggeria
  //--------------------------------------
  Messaggi();
  //--------------------------------------
  // Rinfresco Pagina
  //--------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
//-----------------------
// Messaggeria
//-----------------------
void __fastcall TFormTestPlc2::Messaggi()
{
  FormTestPlc2->Caption   = "Test PLC's SIEMENS";

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
void __fastcall TFormTestPlc2::SetCaselle()
{
  int  Com;
  short int i, auxint;
  char a, b;
  char Appoggio[51]="";
  char Decimale[31]="";
  char Esadecimale[31]="";
  char Binario[31]="";
  char Ascii[31]="";
//char BinH[11];
//char BinL[11];

  sprintf(Appoggio, "%02d", NumPlc);
  EditPlc0->Text = Appoggio;
  sprintf(Appoggio, "%03d", DB);
  EditPlc1->Text = Appoggio;
  sprintf(Appoggio, "%04d", DW);
  EditPlc2->Text = Appoggio;
  sprintf(Appoggio, "%02d", nDW);
  EditPlc3->Text = Appoggio;
  memset(&ColModificata, 0, sizeof(ColModificata));
  //--------------------------------------------
  // Se la comunicazione non è Ethernet
  // blocco l'accesso al campo delle DB
  //--------------------------------------------
  if(CBEth->Checked==false){
     EditPlc1->Enabled=false;
     EditPlc1->Color=clBtnFace;
  }
  else{
     EditPlc1->Enabled=true;
     EditPlc1->Color=clWindow;
  }
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
     //--------------------------------------
     // Scomposizione binario per Siemens
     //--------------------------------------
//   memcpy(&BinH[0], &Binario[0], 8);
//   BinH[8] = NULL;
//   memcpy(&BinL[0], &Binario[8], 8);
//   BinL[8] = NULL;
//   sprintf(Binario, "%s%s", BinL, BinH);
     a = (WORD)(Dw[DW+(i-1)]>>8);
     if(a==0) a=' ';
     b = (WORD)(Dw[DW+(i-1)] & 0x00ff);
     if(b==0) b=' ';
     sprintf(Ascii, "%c%c", a, b);
     sprintf(Appoggio, "DW%03d", (DW+((i-1)*2)));
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
  // Pannelli di stato comunicazione
  //-----------------------------------------------------------------
  Com = COM_SIEMENS;
  //-----------------------------
  // Stato comunicazione
  //-----------------------------
  if( break_Com[Com] ){
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
int __fastcall TFormTestPlc2::GetCaselle()
{
  short int i;
  WORD Valore;
  char Appoggio[51]="";
  char *ptr_char;
  bool SaltaScrittura;

  NumPlc = (short int)(atoi(EditPlc0->Text.c_str()));
  DB     = (short int)(atoi(EditPlc1->Text.c_str()));
  DW     = (short int)(atoi(EditPlc2->Text.c_str()));
  DW     = DW/2;
  DW     = DW*2;
  nDW    = (short int)(atoi(EditPlc3->Text.c_str()));
  //----------------------------------------
  // Non sono ammessi numeri di verifico
  //----------------------------------------
  if(OmrPlc[ NumPlc ].Com!=COM_SIEMENS){
     ShowMessage("ATTENTION: Plc not SIEMENS!");
     return 1;
  }
  //----------------------------------------
  // Se non è abilitata l'Ethernet forzo
  // DB=1;
  //----------------------------------------
  if(CBEth->Checked==false) DB=1;

  //----------------------------------------
  // Controllo attendibilità dei valori
  //----------------------------------------
  if(DB<1 || DB>500){
     strcpy(MessaggioErrore, "Error: The DB's number is incorrect (value between 1 and 255)...");
     return 1;
  }
  if(DW<0 || DW>2000){
     strcpy(MessaggioErrore, "Error: The DW's number is incorrect (value between 0 and 2000)...");
     return 1;
  }
  if(nDW<1 || nDW>64){
     strcpy(MessaggioErrore, "Error: The number of DW is incorrect (value between 1 and 64)...");
     return 1;
  }
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
void __fastcall TFormTestPlc2::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Key == VK_ESCAPE ) Close();
}
//---------------------------------------------------------------------------
//-----------------------------------------------------------
// Lettura da Plc
//-----------------------------------------------------------
void __fastcall TFormTestPlc2::BtnLeggiClick(TObject *Sender)
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
void __fastcall TFormTestPlc2::BtnScriviClick(TObject *Sender)
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
void __fastcall TFormTestPlc2::GridCanaliSetEditText(TObject *Sender, int ACol, int ARow, const AnsiString Value)
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
void __fastcall TFormTestPlc2::LetturaPlc()
{
  int err=0;

  ErroreDiComunicazione=false;
  memset(&Dw, 0, sizeof(Dw));
  //-----------------------------------------------------------
  // Fase di lettura tramite protocollo 3964 della Siemens
  //-----------------------------------------------------------
  //  err = SIM.protP3964r(COM1, LETTURA, DB, DW, nDW, &Dw[0], MessaggioErrore);
  //-----------------------------------------------------------
  // Fase di lettura tramite protocollo MODBUS
  //-----------------------------------------------------------
  if(CBEth->Checked==false){
     err = MODBUS_RD_Registri( NumPlc, NumPlc, DW, nDW, &Dw[DW], MessaggioErrore );
  }
  else{
     //------------------------------------------------------------
     // RICONNESSIONE
     // Nel test PLC se sono stati raggiunti i 3 errore consecutivi
     // eseguo la procedura automatica di riconnessione
     //------------------------------------------------------------
     if(errore_comunicazione[COM_SIEMENS][NumPlc]>3 && SIM_PlcConnesso[NumPlc]==false){
        err = riconnetti_porta_siemens(NumPlc, 0, MessaggioErrore);   // slot=2 per S300
     }
     //------------------------------------------------------------
     // Trasmetti DW Bit Parametri invertendo i BYTE x SIEMENS
     //           DW altre correttamente compilate
     //------------------------------------------------------------
     if(!err) err = SIM_S7ReadWordPlc( NumPlc, 1, DB, DW, nDW, &Dw[DW], MessaggioErrore );
  }
  if( err) ErroreDiComunicazione=true;
  if(!err) strcpy(MessaggioErrore, "Error: none...");
}
//---------------------------------------------------------------------------
//-----------------------
// ScritturaPlc
//-----------------------
void __fastcall TFormTestPlc2::ScritturaPlc()
{
  int err=0;

  ErroreDiComunicazione=false;
  //-----------------------------------------------------------
  // Fase di scrittura tramite protocollo 3964 della Siemens
  //-----------------------------------------------------------
  //  err = SIM.protP3964r(COM1, SCRITTURA, DB, DW, nDW, &Dw[0], MessaggioErrore);
  //-----------------------------------------------------------
  // Fase di scrittura tramite protocollo MODBUS
  //-----------------------------------------------------------
  if(CBEth->Checked==false){
     err = MODBUS_WR_Registri( NumPlc, NumPlc, DW, nDW, &Dw[DW], MessaggioErrore );
  }
  else{
     //------------------------------------------------------------
     // RICONNESSIONE
     // Nel test PLC se sono stati raggiunti i 3 errore consecutivi
     // eseguo la procedura automatica di riconnessione
     //------------------------------------------------------------
     if(errore_comunicazione[COM_SIEMENS][NumPlc]>3 && SIM_PlcConnesso[NumPlc]==false){
        err = riconnetti_porta_siemens(NumPlc, 0, MessaggioErrore);   // slot=2 per S300
     }
     //------------------------------------------------------------
     // Trasmetti DW Bit Parametri invertendo i BYTE x SIEMENS
     //           DW altre correttamente compilate
     //------------------------------------------------------------
     if(!err) err = SIM_S7WriteWordPlc( NumPlc, 1, DB, DW, nDW, &Dw[DW], MessaggioErrore);
  }
  if( err) ErroreDiComunicazione=true;
  if(!err) strcpy(MessaggioErrore, "Error: none...");
}
//---------------------------------------------------------------------------
void __fastcall TFormTestPlc2::GridCanaliGetEditMask(TObject *Sender,
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
void __fastcall TFormTestPlc2::CBEthClick(TObject *Sender)
{
  //--------------------------------------------
  // Se la comunicazione non è Ethernet
  // blocco l'accesso al campo delle DB
  //--------------------------------------------
  if(CBEth->Checked==false){
     EditPlc1->Enabled=false;
     EditPlc1->Color=clBtnFace;
  }
  else{
     EditPlc1->Enabled=true;
     EditPlc1->Color=clWindow;
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormTestPlc2::CheckSignedClick(TObject *Sender)
{
  GetCaselle();
  SetCaselle();
}
//---------------------------------------------------------------------------

