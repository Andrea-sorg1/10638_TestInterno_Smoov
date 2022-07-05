//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "TestPlc1.h"
#include "OmronETH.h"
#include "TecnoFer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormTestPlc *FormTestPlc;
//---------------------------------------------------------------------------
//-------------------------------
// Costruttore Classe PassWord
//-------------------------------
__fastcall TFormTestPlc::TFormTestPlc(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
//-----------------------
// Attivazione Finestra
//-----------------------
void __fastcall TFormTestPlc::FormActivate(TObject *Sender)
{
  //--------------------------------------
  // Inizializzazioni
  //--------------------------------------
  GridCanali->DefaultRowHeight = 16;
  GridCanali->DefaultColWidth  = 100;
  GridCanali->ColCount         = 5;
  GridCanali->RowCount         = 2;
  UpDownCh->Position           = 0;
  memset(&DM, 0, sizeof(DM));
  Plc=0;
  DmIni=0;
  TotDm=1;
  TipoCh=0;
  Port=COM2;
  ErroreDiComunicazione=false;
  strcpy(MessaggioErrore, "Error: non...");
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
}
//---------------------------------------------------------------------------
//-----------------------
// Messaggeria
//-----------------------
void __fastcall TFormTestPlc::Messaggi()
{
  FormTestPlc->Caption    = MESS[242];

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
void __fastcall TFormTestPlc::SetCaselle()
{
  short int i, auxint;
  char a, b;
  char Appoggio[51]="";
  char Decimale[31]="";
  char Esadecimale[31]="";
  char Binario[31]="";
  char Ascii[31]="";

  UpDownCh->Position   = DmIni;
  BoxTipoCh->ItemIndex = TipoCh;
  sprintf(Appoggio, "%02d", Plc);
  EditPlc1->Text = Appoggio;
  sprintf(Appoggio, "%05d", DmIni);
  EditPlc2->Text = Appoggio;
  sprintf(Appoggio, "%03d", TotDm);
  EditPlc3->Text = Appoggio;
  memset(&ColModificata, 0, sizeof(ColModificata));
  //--------------------------------------------
  // Rinfresco della Griglia
  //--------------------------------------------
  for(i=0; i<TotDm; i++){
     GridCanali->Cells[0][i+1] = "";
     GridCanali->Cells[1][i+1] = "";
     GridCanali->Cells[2][i+1] = "";
     GridCanali->Cells[3][i+1] = "";
     GridCanali->Cells[4][i+1] = "";
     //---------------------------------------
     // Valori letti
     //---------------------------------------
     if( CheckSigned->Checked==false ){
        sprintf(Decimale, "%u", DM[i]);
     }
     else{
        auxint = (short int)(DM[i]);
        sprintf(Decimale, "%d", auxint);
     }
     sprintf(Esadecimale, "%X", DM[i]);
     itoa(DM[i], Binario, 2);
     addzero(Binario, 16);
     a = (WORD)(DM[i]>>8);
     if(a==0) a=' ';
     b = (WORD)(DM[i] & 0x00ff);
     if(b==0) b=' ';
     sprintf(Ascii, "%c%c", a, b);
     sprintf(Appoggio, "CH%04d", DmIni+i);
     GridCanali->Cells[0][i+1] = Appoggio;
     GridCanali->Cells[1][i+1] = Decimale;
     GridCanali->Cells[2][i+1] = Esadecimale;
     GridCanali->Cells[3][i+1] = Binario;
     GridCanali->Cells[4][i+1] = Ascii;
  }
  //-----------------------------------------------------------------
  // Puntamento alla porta di comunicazione in base al PLC
  //-----------------------------------------------------------------
  Port=OmrPlc[Plc].Com;
  if(Plc==PLC_IO_REMOTO_01) Port=COM6;   // I/O REMOTI
  if(Plc==PLC_IO_REMOTO_02) Port=COM6;   // I/O REMOTI
  if(Plc==PLC_IO_REMOTO_03) Port=COM6;   // I/O REMOTI
  if(Plc==PLC_IO_REMOTO_04) Port=COM6;   // I/O REMOTI
  if(Plc==PLC_IO_REMOTO_05) Port=COM6;   // I/O REMOTI
  if(Plc==PLC_IO_REMOTO_06) Port=COM6;   // I/O REMOTI
  if(Plc==PLC_IO_REMOTO_07) Port=COM6;   // I/O REMOTI
  if(Plc==PLC_IO_REMOTO_08) Port=COM6;   // I/O REMOTI
  if(Plc==PLC_IO_REMOTO_09) Port=COM6;   // I/O REMOTI
  if(Plc==PLC_IO_REMOTO_10) Port=COM6;   // I/O REMOTI
  //-----------------------------------------------------------------
  // Pannelli Comunicazione
  //-----------------------------------------------------------------
  if( break_Com[Port] ){
     PanelOK->Color     = clBtnFace;
     PanelNO->Color     = clBtnFace;
     PanelINT->Color    = clYellow;
     TextError->Caption = "Error: none...";
  }
  else{
     if( ErroreDiComunicazione ){
        PanelOK->Color     = clBtnFace;
        PanelNO->Color     = clRed;
        PanelINT->Color    = clBtnFace;
        TextError->Caption = MessaggioErrore;
     }
     else{
        PanelOK->Color     = clLime;
        PanelNO->Color     = clBtnFace;
        PanelINT->Color    = clBtnFace;
        TextError->Caption = MessaggioErrore;
     }
  }
}
//---------------------------------------------------------------------------
//-----------------------
// GetCaselle
//-----------------------
void __fastcall TFormTestPlc::GetCaselle()
{
  int  i;
  WORD Valore;
  char Appoggio[51]="";
  char *ptr_char;

  Plc    = (WORD)(StrToInt(EditPlc1->Text));
  DmIni  = (WORD)(StrToInt(EditPlc2->Text));
  TotDm  = (WORD)(StrToInt(EditPlc3->Text));
  TipoCh = BoxTipoCh->ItemIndex;
  GridCanali->RowCount = TotDm+1;
  UpDownCh->Position   = DmIni;
  //----------------------------------------
  // Controllo attendibilità dei valori
  //----------------------------------------
  if(TotDm>999  ) TotDm =   999;
  if(Plc>20     ) Plc   =    20;
  //----------------------------------------
  // Nel caso di protocolli HOST-LINK non
  // è possibile leggere DM>9999
  //----------------------------------------
  if(OmrPlc[Plc].TypCom==0){
     if(DmIni>9999 ) DmIni = 9999;
  }
  else{
     if(DmIni>30000) DmIni = 30000;
  }
  //----------------------------------------
  // Recupero dei valori da settare in base
  // all'ultima cella codificara
  //----------------------------------------
  for(i=0; i<TotDm; i++){
     strcpy(Appoggio, GridCanali->Cells[ColModificata[i+1]][i+1].c_str());
     Erase_White_End(Appoggio);
     Erase_White_Begin(Appoggio);
     switch(ColModificata[i+1]){
        case  1: Valore = (WORD)(atoi(Appoggio));
                 DM[i] = Valore;
                 break;
        case  2: Valore = (WORD)(strtol(Appoggio, &ptr_char, 16));
                 DM[i] = Valore;
                 break;
        case  3: Valore = (WORD)(strtol(Appoggio, &ptr_char,  2));
                 DM[i] = Valore;
                 break;
        case  4: Valore = (WORD)(Appoggio[0]<<8);
                 Valore = (WORD)(Valore) | (WORD)(Appoggio[1]);
                 DM[i] = Valore;
                 break;
        default: break;
     }
  }
}
//---------------------------------------------------------------------------
//-------------------------------------
// Chiusura della Finestra da tastiera
//-------------------------------------
void __fastcall TFormTestPlc::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Key == VK_ESCAPE ) Close();
}
//---------------------------------------------------------------------------
//-----------------------------------------------------------
// Lettura da Plc
//-----------------------------------------------------------
void __fastcall TFormTestPlc::BtnLeggiClick(TObject *Sender)
{
  //------------------------
  // Recupero da grafica
  //------------------------
  GetCaselle();
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
void __fastcall TFormTestPlc::BtnScriviClick(TObject *Sender)
{
  //------------------------
  // Recupero da grafica
  //------------------------
  GetCaselle();
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
void __fastcall TFormTestPlc::GridCanaliSetEditText(TObject *Sender, int ACol, int ARow, const AnsiString Value)
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
void __fastcall TFormTestPlc::LetturaPlc()
{
  WORD err=0;
  char buf[11];
  char buffer[101];

  memset(&DM, 0, sizeof(DM));
  ErroreDiComunicazione=false;
  //---------------------------------------------------
  // Comunicazione distattivata
  //---------------------------------------------------
  if( break_Com[Port] ) return;
  //---------------------------------------------------
  // In base al tipo di canale viene copiato in buf[]
  // il relativo comando di lettura
  //---------------------------------------------------
  strcpy(buf, "R#AAA");
  switch(TipoCh){
     case  0: strcpy(buf, "RD"); break;
     case  1: strcpy(buf, "RH"); break;
     case  2: strcpy(buf, "RL"); break;
     case  3: strcpy(buf, "RJ"); break;
     case  4: strcpy(buf, "RR"); break;
     case  5: strcpy(buf, "R#"); break;
     case  6: strcpy(buf, "R#"); break;
     default: break;
  }
  // **** read
  if(buf[1]!='#') err = OM.ld_canale (&DM[0], Plc, buf, DmIni,  TotDm, buffer);
  if(TipoCh==5  ) err = OM.ld_TIM_CNT(&DM[0], Plc, buf, "TIM ", DmIni, buffer);
  if(TipoCh==6  ) err = OM.ld_TIM_CNT(&DM[0], Plc, buf, "CNT ", DmIni, buffer);
  if(err){
     ErroreDiComunicazione=true;
     sprintf(MessaggioErrore, "Error: %s", buffer);
  }
  else{
     strcpy(MessaggioErrore, "Error: none...");
  }
}
//---------------------------------------------------------------------------
//-----------------------
// ScritturaPlc
//-----------------------
void __fastcall TFormTestPlc::ScritturaPlc()
{
  WORD err=0;
  char buf[11];
  char buffer[101];

  ErroreDiComunicazione=false;
  //---------------------------------------------------
  // Comunicazione distattivata
  //---------------------------------------------------
  if( break_Com[Port] ) return;
  //---------------------------------------------------
  // In base al tipo di canale viene copiato in buf[]
  // il relativo comando di lettura
  //---------------------------------------------------
  strcpy(buf, "W#AAA");
  switch(TipoCh){
     case  0: strcpy(buf, "WD"); break;
     case  1: strcpy(buf, "WH"); break;
     case  2: strcpy(buf, "WL"); break;
     case  3: strcpy(buf, "WJ"); break;
     case  4: strcpy(buf, "WR"); break;
     case  5: strcpy(buf, "W#"); break;
     case  6: strcpy(buf, "W#"); break;
     default: break;
  }
  // **** write
  if(buf[1]!='#') err = OM.wr_canale (&DM[0], Plc, buf, DmIni,  TotDm, buffer);
  if(TipoCh==5  ) err = OM.wr_TIM_CNT(&DM[0], Plc, buf, "TIM ", DmIni, buffer);
  if(TipoCh==6  ) err = OM.wr_TIM_CNT(&DM[0], Plc, buf, "CNT ", DmIni, buffer);
  if(err){
     ErroreDiComunicazione=true;
     sprintf(MessaggioErrore, "Error: %s", buffer);
  }
  else{
     strcpy(MessaggioErrore, "Error: none...");
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormTestPlc::GridCanaliGetEditMask(TObject *Sender,
      int ACol, int ARow, AnsiString &Value)
{
  switch(ACol){
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
void __fastcall TFormTestPlc::EditPlcExit(TObject *Sender)
{
  GetCaselle();
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormTestPlc::UpDownChClick(TObject *Sender, TUDBtnType Button)
{
  DmIni = UpDownCh->Position;
  //---------------------------
  // Lettura del PLC
  //---------------------------
  LetturaPlc();
  SetCaselle();
}
//---------------------------------------------------------------------------


void __fastcall TFormTestPlc::UpDownChChanging(TObject *Sender,
      bool &AllowChange)
{
//GetCaselle();
//UpDownCh->Position = DmIni;
}
//---------------------------------------------------------------------------

void __fastcall TFormTestPlc::CheckSignedClick(TObject *Sender)
{
  GetCaselle();
  SetCaselle();
}
//---------------------------------------------------------------------------

