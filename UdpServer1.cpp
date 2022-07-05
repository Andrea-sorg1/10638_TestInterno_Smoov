/*---------------------------------------------------------------------------
   by  GianFranco   07/05/2002
   Test UDP/TCP PROTOCOL

   Send and Recive procedure con PLC CJ OMRON

---------------------------------------------------------------------------*/
#if __BORLANDC__ == 0x520     // BCB1 is BC5.20   BCB3 is BC5.30
    #define _WINSOCKAPI_      // Prevent winsock.h from being included
    #define s_addr S_addr
#endif

#include <vcl\vcl.h>
#include <vcl\inifiles.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>

#pragma hdrstop

#include "Libreria.h"
#include "OmronEth.h"
#include "AlgoScambio.h"
#include "UdpServer1.h"
#include "InitCom.h"
#include "Variabil.h"
#include "OmronEthSer.h"
#include "OmronEthTcp.h"

#ifndef SERVER_COMUNICAZIONI
  #define SectionWindow     "Window"
  #define SectionData       "Data"
  #define KeyNumCom         "NumCom"
  #define KeyTimeOutTCP     "TimeOutTCP"
  #define KeyTimeOutUDP     "TimeOutUDP"
  #define KeyTimeOutHOST    "TimeOutHOST"
#else
  #define SectionData       "Data"
  #define KeyPort           "Port"
  #define KeyServer         "Server"
  #define KeyRemotePort     "RemotePort"
#endif



//---------------------------------------------------------------------------
#pragma link "WSocket"
#pragma link "Ping"
#pragma resource "*.dfm"

TFormServerUDP *FormServerUDP;


//---------------------------------------------------------------------------
// Variabili Globali
//---------------------------------------------------------------------------

int  Index;

//--------------------------------  o  ---------------------------------

//---------------------------------------------------------------------------
__fastcall TFormServerUDP::TFormServerUDP(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormServerUDP::FormCreate(TObject *Sender)
{
  int i;
  static BOOL FirstTime = TRUE;
  #ifndef SERVER_COMUNICAZIONI
    TIniFile    *IniFile;
    AnsiString  LocalIP;
  #endif

  // Build Ini file name
  FIniFileName = LowerCase(ExtractFileName(Application->ExeName));
  FPath        = ExtractFilePath(ParamStr(0));
  //FIniFileName = FIniFileName.SubString(1, FIniFileName.Length() - 3) + "ini";
  FIniFileName = FPath + FIniFileName.SubString(1, FIniFileName.Length() - 3) + "ini";


  if (FirstTime) {
     FirstTime                   = FALSE;
     #ifndef SERVER_COMUNICAZIONI
        IniFile          = new TIniFile(FIniFileName);
        NumCom           = IniFile->ReadInteger(SectionWindow, KeyNumCom,      NUMPORTE);
        TimeOutSocketUDP = IniFile->ReadInteger(SectionData,   KeyTimeOutUDP,  MSEC_TIMEOUT_OMRON);
        TimeOutSocketTCP = IniFile->ReadInteger(SectionData,   KeyTimeOutTCP,  MSEC_TIMEOUT_OMRON);
        TimeOutHostLink  = IniFile->ReadInteger(SectionData,   KeyTimeOutHOST, TIMEOUTSERIALE);
        delete IniFile;
     #endif
     Index                       = 0;
     DataAvailableLabel->Caption = "";
     NrDatoIniEdit->Text         = 0;
     NrDatiEdit->Text            = 10;
     BoxCmd->ItemIndex           = 0;
     PortEdit->Text              = "600";
     RemoteIpEdit->Text          = "0.0.0.0";
     RemotePortEdit->Text        = "6969";
     TCPTimeOutEdit->Text        = TimeOutSocketTCP;
     UDPTimeOutEdit->Text        = TimeOutSocketUDP;
     HOSTTimeOutEdit->Text       = TimeOutHostLink;
     LocalIPAddr                 = "0.0.0.0";
     //---------------------------------------
     // Inizializzazione delle comunicazioni
     //---------------------------------------
     OmrInitComunicazioni();
     //---------------------------------------
     // Aggiornamento COMBO dei Plc
     //---------------------------------------
     for(i=0; i<=MAXPLCCON; i++){
        ListaPLC->Items->Add(OmrPlc[i].DescrPlc);
     }
     ListaPLC->ItemIndex=0;
     //---------------------------------------
     // Aggiornamento COMBO delle seriali
     //---------------------------------------
     for(i=0; i<=MAXCOM; i++){
        ListaCOM->Items->Add(InCom[i].DescrCom);
     }
     ListaCOM->ItemIndex=0;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormServerUDP::FormActivate(TObject *Sender)
{
  //---------------------------------------
  // Rinfresco della Form
  //---------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormServerUDP::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  // Build Ini file name
  FIniFileName = LowerCase(ExtractFileName(Application->ExeName));
  FPath        = ExtractFilePath(ParamStr(0));
  //FIniFileName = FIniFileName.SubString(1, FIniFileName.Length() - 3) + "ini";
  FIniFileName = FPath + FIniFileName.SubString(1, FIniFileName.Length() - 3) + "ini";


  #ifndef SERVER_COMUNICAZIONI
     TIniFile *IniFile;

     IniFile = new TIniFile(FIniFileName);
     IniFile->WriteInteger(SectionWindow, KeyNumCom,      NumCom);
     IniFile->WriteInteger(SectionData,   KeyTimeOutUDP,  TimeOutSocketUDP);
     IniFile->WriteInteger(SectionData,   KeyTimeOutTCP,  TimeOutSocketTCP);
     IniFile->WriteInteger(SectionData,   KeyTimeOutHOST, TimeOutHostLink);

     delete IniFile;
  #endif
}
//---------------------------------------------------------------------------

// ---------------------------
//    GetCaselle()
// ---------------------------
// Funzione che legge dalle caselle di dialogo (TEdit) di WINDOWS
// e scarica sulla struttura dei PLC
//
int __fastcall TFormServerUDP::GetCaselle()
{
  char all_mess[101]="";
  unsigned char node;
  #ifndef SERVER_COMUNICAZIONI
//   int i;
  #endif   
  int NumPlc;

  if( Index>MAXPLCCON ) return 1;
  LocalIPAddr= LocalIpEdit->Text;  // Indirizzo IP Locale
  //-------------------------------------------------------------
  // Tipo di Interfaccia scelta
  //-------------------------------------------------------------
//Index = ListaPLC->ItemIndex;
  ListaPLC->GetTextBuf(all_mess, 30);
  OmrPlc[ Index ].DescrPlc = StrUpper(all_mess);
  //-------------------------------------------------------------
  // Tipo di Comunicazione
  //-------------------------------------------------------------
  OmrPlc[ Index ].TypCom = BoxComunicazione->ItemIndex;
  //-------------------------------------------------------------
  // Parametri della comunicazione HOST LINK
  //-------------------------------------------------------------
  if(OmrPlc[ Index ].TypCom==0){
     OmrPlc[ Index ].IndexPlc = (char)atoi(IndicePlcEdit->Text.c_str());
     OmrPlc[ Index ].Com      = (char)(ListaCOM->ItemIndex);
     TimeOutHostLink          = atoi(HOSTTimeOutEdit->Text.c_str());
     #ifndef SERVER_COMUNICAZIONI
/*
        //------------------------------------------------------
        // 18/04/2005
        // Controlla che la stessa COM e Indice plc sia
        // ripetutuo per altri PLC !!!!!
        //------------------------------------------------------
        for( i=0; i<20; i++ ){
           if( i==Index) continue;
           if(OmrPlc[ i ].TypCom   != 0)                         continue;
           if(OmrPlc[ i ].Com      != OmrPlc[ Index ].Com )      continue;
           if(OmrPlc[ i ].IndexPlc != OmrPlc[ Index ].IndexPlc ) continue;

           sprintf( all_mess,  "Stessa COM e INDICE PLC già utilizzate per PLC/TGV:%d", i );
           ShowMessage( all_mess );
        }
*/
     #endif
  }
  //-------------------------------------------------------------
  // Parametri della comunicazione FINS
  //-------------------------------------------------------------
  if(OmrPlc[ Index ].TypCom==1){
   //  OmrPlc[ Index ].IndexPlc = (char)Index;
   //OmrPlc[ Index ].IndexPlc = 0;
     TimeOutSocketUDP           = atoi(UDPTimeOutEdit->Text.c_str());
     LocalIPAddr                = LocalIpEdit->Text;
     OmrPlc[ Index ].LocalPort  = atoi(PortEdit->Text.c_str());
     OmrPlc[ Index ].RemotePort = atoi(RemotePortEdit->Text.c_str());
     OmrPlc[ Index ].RemoteHost = RemoteIpEdit->Text;
     OmrPlc[ Index ].SNA = (char) atoi( FinsSnaEdit->Text.c_str() );
     OmrPlc[ Index ].SA1 = (char) atoi( FinsSa1Edit->Text.c_str() );
     OmrPlc[ Index ].SA2 = (char) atoi( FinsSa2Edit->Text.c_str() );
     OmrPlc[ Index ].DNA = (char) atoi( FinsDnaEdit->Text.c_str() );
     OmrPlc[ Index ].DA1 = (char) atoi( FinsDa1Edit->Text.c_str() );
     OmrPlc[ Index ].DA2 = (char) atoi( FinsDa2Edit->Text.c_str() );
  }
  //-------------------------------------------------------------
  // Parametri della comunicazione NONE
  //-------------------------------------------------------------
  if(OmrPlc[ Index ].TypCom==2){
     #ifdef SERVER_COMUNICAZIONI
        ShowMessage("This tipe of comunication not availabled...");
     #endif
     //return 1;
  }
  //-------------------------------------------------------------
  // Parametri della comunicazione TCP
  // TCP/IP
  // ETH/SERIALE
  // BECKOFF
  //-------------------------------------------------------------
//if(OmrPlc[ Index ].TypCom==3 || OmrPlc[ Index ].TypCom==4 ){
  if(OmrPlc[ Index ].TypCom>2 ){
     OmrPlc[ Index ].IndexPlc   = (char)atoi(EditPlcTCP->Text.c_str());
     TimeOutSocketTCP           = atoi(TCPTimeOutEdit->Text.c_str());
     OmrPlc[ Index ].RemotePort = atoi(EditPortTCP->Text.c_str());
     OmrPlc[ Index ].RemoteHost = EditIpTCP->Text;
  }
  //-------------------------------------------------------------
  // Ricalcolo degli SA1 e DA1 in base al Local e Remote address
  //-------------------------------------------------------------
  IPToNode(OmrPlc[ Index ].RemoteHost, &node);
  OmrPlc[ Index ].DA1 = node;
  IPToNode(LocalIPAddr, &node);
  OmrPlc[ Index ].SA1 = node;

  //-------------------------------------------------------------
  // 21/09/2005 Aggiorn tutti gli SA1
  //-------------------------------------------------------------
  for(NumPlc=0; NumPlc<=MAXPLCCON; NumPlc++){
     OmrPlc[ NumPlc ].SA1 = node;
  }

  //-------------------------------------------------------------
  // BECKHOFF: utilizza FINS "DNA" e FINS "DA2" come parametri
  // aggiuntivi del ADS-AmsNetID modificabili solo da ConfigPlc
  // e non editabili (default DNA = 1 e DA2 = 1).
  //-------------------------------------------------------------
  if(OmrPlc[ Index ].TypCom==5){
     if(OmrPlc[ Index ].DNA==0) OmrPlc[ Index ].DNA=1;
     if(OmrPlc[ Index ].DA2==0) OmrPlc[ Index ].DA2=1;
  }
  return 0;
}
//---------------------------------------------------------------------------
// ---------------------------
//    SetCaselle()
// ---------------------------
// Funzione che forza i nuovi valori nelle caselle di dialogo (TEdit)
// di WINDOWS recuperandoli dalla struttura dei PLC
//
int __fastcall TFormServerUDP::SetCaselle()
{
  int  i, IdCom, pos, tot_pos;
  char all_mess[101]="";
  AnsiString Testo;
  int  TypCom;

  if( Index>MAXPLCCON ) return 1;

  TextId->Caption = Index;
  //------------------------------------------------
  // Tipo di Interfaccia
  //------------------------------------------------
  ListaPLC->ItemIndex  = Index;
  //------------------------------------------------
  // Tipo di Comunicazione
  //------------------------------------------------
  BoxComunicazione->ItemIndex = OmrPlc[ Index ].TypCom;
  TypCom                      = OmrPlc[ Index ].TypCom;
  //------------------------------------------------
  // Parametri generici
  //------------------------------------------------
  IndicePlcEdit->Text = IntToStr(OmrPlc[ Index ].IndexPlc);
  EditPlcTCP->Text    = IntToStr(OmrPlc[ Index ].IndexPlc);
  //------------------------------------------------
  // Parametri della comunicazione HOST LINK
  //------------------------------------------------
  IdCom = OmrPlc[ Index ].Com;
  ListaCOM->ItemIndex = IdCom;
  sprintf(all_mess, "COM%02d: %d, %d, %c, %d", (IdCom+1), InCom[ IdCom ].Baudrate,
                                               InCom[ IdCom ].Dati, InCom[ IdCom ].Parita,
                                               InCom[ IdCom ].StopBit);
  TextParCom->Caption = all_mess;
  //------------------------------------------------
  // Parametri della comunicazione FINS
  //------------------------------------------------
//LocalIpEdit->Text    = WSocket_inet_ntoa(FServerAddr);
  //-------------------------------------------------------
  // Se il Local IP address non è ancora stato
  // compilato lo ricavo
  //     RemoteHost + SA1;
  //-------------------------------------------------------
  if(LocalIPAddr=="0.0.0.0" || LocalIPAddr=="127.0.0.1" ){
     LocalIpEdit->Text = OmrPlc[ Index ].RemoteHost;
     tot_pos = 0;
     Testo   = LocalIpEdit->Text;
     for(i=0; i<3; i++){
        pos = Testo.Pos(".");
        Testo = Testo.SubString(pos+1, 15);
        tot_pos=tot_pos + pos;
     }
     //--------------------------------------------
     // Recupero tutto l'IP remoto tranne l'ultima
     // parte.
     //--------------------------------------------
     if(tot_pos!=0){
        Testo = LocalIpEdit->Text.SubString(1, tot_pos);
        LocalIpEdit->Text = Testo + OmrPlc[ Index ].SA1;
     }
  }
  else{
     LocalIpEdit->Text  = LocalIPAddr;
  }
  PortEdit->Text        = OmrPlc[ Index ].LocalPort;
  RemotePortEdit->Text  = OmrPlc[ Index ].RemotePort;
  EditPortTCP->Text     = OmrPlc[ Index ].RemotePort;
  RemoteIpEdit->Text    = OmrPlc[ Index ].RemoteHost;
  EditIpTCP->Text       = OmrPlc[ Index ].RemoteHost;
  FinsSnaEdit->Text     = IntToStr( OmrPlc[ Index ].SNA );
  FinsSa1Edit->Text     = IntToStr( OmrPlc[ Index ].SA1 );
  FinsSa2Edit->Text     = IntToStr( OmrPlc[ Index ].SA2 );
  FinsDnaEdit->Text     = IntToStr( OmrPlc[ Index ].DNA );
  FinsDa1Edit->Text     = IntToStr( OmrPlc[ Index ].DA1 );
  FinsDa2Edit->Text     = IntToStr( OmrPlc[ Index ].DA2 );
  UDPTimeOutEdit->Text  = TimeOutSocketUDP;
  TCPTimeOutEdit->Text  = TimeOutSocketTCP;
  HOSTTimeOutEdit->Text = TimeOutHostLink;

  Label19->Visible=true;
  EditSA1Tcp->Visible=true;
  switch(TypCom){
     case  0:
     case  1:
     case  2:
     case  3: Label19->Caption="SA1";
              EditSA1Tcp->Text=OmrPlc[ Index ].SA1;
              break;
     case  4: Label19->Caption="SA1";
              EditSA1Tcp->Text=OmrEthTcp_ClientNodeSA1[ Index ];  // Visualizza in TCP Indirizzo recuperato da PLC
              break;
     case  5: Label19->Visible=false;
              EditSA1Tcp->Visible=false;
              break;
     default: Label19->Caption="SA1";
              EditSA1Tcp->Text=OmrPlc[ Index ].SA1;
              break;
  }

  return 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormServerUDP::BtnSalvaClick(TObject *Sender)
{
  int i;
  int OldTypCom     = OmrPlc[ Index ].TypCom;
  int OldRemotePort = OmrPlc[ Index ].RemotePort;


  if(GetCaselle()) return;
  //---------------------------------------------------------------
  // Se la Comunicazione è TCP e vengono cambiati dei paramteri
  // --> chiudi Socket
  //---------------------------------------------------------------
  if( OldTypCom==4 ){
     if( OldTypCom     != OmrPlc[ Index ].TypCom     ) WSocketFins[ Index ]->Close();
     if( OldRemotePort != OmrPlc[ Index ].RemotePort ) WSocketFins[ Index ]->Close();
  }
  //----------------------------------
  // Salvataggio nuova configurazione
  //----------------------------------
  OmrSalvaConfigurazionePLC();
  //----------------------------------
  // Ricompilazione Lista Plc
  //----------------------------------
  ListaPLC->Clear();
  for(i=0; i<=MAXPLCCON; i++){
     ListaPLC->Items->Add(OmrPlc[i].DescrPlc);
  }
  ListaPLC->ItemIndex=Index;
  SetCaselle();

  //--------------------------------------------
  // Salva Indirizzo IP LOCALE
  //--------------------------------------------
  TIniFile  *IniFile = new TIniFile("C:\\WINDOWS\\ALGOSISTEMI\\TecnoETH.ini");
  IniFile->WriteString("IP Address", "LocalIP", LocalIPAddr);
  delete IniFile;
}

//---------------------------------------------------------------------------
void __fastcall TFormServerUDP::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Key == VK_ESCAPE ) Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormServerUDP::ListaPLCClick(TObject *Sender)
{
  Index =  ListaPLC->ItemIndex;
  //-------------------------------
  // Rinfresco della Form
  //-------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormServerUDP::BoxComunicazioneClick(TObject *Sender)
{
  int TipoDiComunicazione;
  //------------------------------------------------
  // Tipo di Comunicazione
  //------------------------------------------------
  TipoDiComunicazione = BoxComunicazione->ItemIndex;
  //------------------------------------------------
  // Disattivazione degli oggetti della Form
  // 0 - Host Link
  // 1 - Fins Protocol UDP
  // 2 - Algo Comunication (NONE)
  // 3 - Ethernet-Seriale tramite radio
  // 4 - Fins Protocol TCP
  //------------------------------------------------
  if(TipoDiComunicazione==0){
     BoxFinsProtocol->Enabled = false;
     BoxHostLink->Enabled     = true;
     BoxEthRS232->Enabled     = false;
  }
  if(TipoDiComunicazione==1){
     BoxFinsProtocol->Enabled = true;
     BoxHostLink->Enabled     = false;
     BoxEthRS232->Enabled     = false;
  }
  if(TipoDiComunicazione==2){
     BoxFinsProtocol->Enabled = false;
     BoxHostLink->Enabled     = false;
     BoxEthRS232->Enabled     = false;
     #ifndef SERVER_COMUNICAZIONI
        // -- inizializzazione algo com.
        Scambio_Inizializza();
     #endif
  }
  //-----------------------------------------
  // TCP/IP
  // ETH/SERIALE
  // BECKOFF
  //-----------------------------------------
//if(TipoDiComunicazione==3 || TipoDiComunicazione==4){
  if(TipoDiComunicazione>2){
     BoxFinsProtocol->Enabled = false;
     BoxHostLink->Enabled     = false;
     BoxEthRS232->Enabled     = true;
  }

}
//---------------------------------------------------------------------------
void __fastcall TFormServerUDP::SendButtonClick(TObject *Sender)
{
  char cmd[5]="";
  char stringa[400];
  char all_mess[400];
  int  i, err;
  WORD Port;
  WORD IdPlc;
  WORD DBx[500];
  WORD NrDatoIni;
  WORD NrDati;
  WORD TipoComando;
  WORD TipoVariabile=1;
  int msec;
  AnsiString Testo;
  SYSTEMTIME TimeInizio;
  SYSTEMTIME TimeFine;

  if(OmrTrasmissioneInCorso) return;
  //------------------------------
  // Recupera dati  da editor
  //------------------------------
  if(GetCaselle()) return;
  Port              = OmrPlc[Index].Com;
  IdPlc             = OmrPlc[Index].IndexPlc;
  NrDatoIni         = (WORD) atoi(NrDatoIniEdit->Text.c_str());
  NrDati            = (WORD) atoi(NrDatiEdit->Text.c_str());
  TipoComando       = (WORD) (BoxCmd->ItemIndex+1);
  strcpy(cmd, "RD");
  //-----------------------------------------------
  // Imposta alcuni Dati da trasmettere  per PROVA
  //-----------------------------------------------
  memset(DBx, 0, sizeof(DBx));
  if(TipoComando==2){
     DBx[ 0] = 9; //
     DBx[ 1] = 8; //
     DBx[ 2] = 7; //
     DBx[ 3] = 6; //
     DBx[ 4] = 5; //
     DBx[ 5] = 4; //
     DBx[ 6] = 3; //
     DBx[ 7] = 2; //
     DBx[ 8] = 1; //
     DBx[ 9] = 0; //
     strcpy(cmd, "WD");
  }
  //------------------------
  // Memorizza tempo
  //------------------------
  msec=0;
  GetLocalTime( &TimeInizio );
  //---------------------------------------------------------
  // Test della Comunicazione HOST LINK
  //---------------------------------------------------------
  if(BoxComunicazione->ItemIndex==0){
     // --- Lettura DM
     if(TipoComando==1){
        for(i=0;i<3;i++){
           err = OM.load_canale(Port, DBx, IdPlc, cmd, NrDatoIni, NrDati, all_mess);
           if(err==0) break;
        }
     }
     // --- Scrittura DM
     if(TipoComando==2){
        for(i=0;i<3;i++){
           err = OM.write_canale(Port, DBx, IdPlc, cmd, NrDatoIni, NrDati, all_mess);
           if(err==0) break;
        }
     }
     GetLocalTime( &TimeFine );
     if( err ) ShowMessage( all_mess );
     Testo = IntToStr(atoi(DataAvailableLabel->Caption.c_str()) + 1) +
             ": (HOST LINK) - COM:" + Port + " PLC:" + IdPlc;
  }
  //---------------------------------------------------------
  // Test della Comunicazione FINS PROTOCOL
  //---------------------------------------------------------
  if(BoxComunicazione->ItemIndex==1){
     err = OmrLeggiScriviCanale((short int)(Index), TipoVariabile, TipoComando, NrDatoIni, NrDati, DBx, 1, all_mess);
     GetLocalTime( &TimeFine );
     if( err ) ShowMessage( all_mess );
     Testo = IntToStr(atoi(DataAvailableLabel->Caption.c_str()) + 1) +
             ": (FINS) - Host IP Address:" + OmrRicevutoDaIP;
  }
  //---------------------------------------------------------
  // Test della Comunicazione ALGO COMUNICATION
  //---------------------------------------------------------
  if(BoxComunicazione->ItemIndex==2){
     #ifndef SERVER_COMUNICAZIONI
     // --- Lettura DM
     if(TipoComando==1) err = Scambio_ld_canale(DBx, IdPlc, cmd, NrDatoIni, NrDati, all_mess);
     // --- Scrittura DM
     if(TipoComando==2) err = Scambio_wr_canale(DBx, IdPlc, cmd, NrDatoIni, NrDati, all_mess);
     GetLocalTime( &TimeFine );
     if( err ) ShowMessage( all_mess );
     Testo = IntToStr(atoi(DataAvailableLabel->Caption.c_str()) + 1) +
             ": (ALGO COMUNICATION) - PLC:" + IdPlc;
     #endif
  }
  //---------------------------------------------------------
  // Test della Comunicazione ETHERNET-SERIALE
  //---------------------------------------------------------
  if(BoxComunicazione->ItemIndex==3){
     // --- Lettura DM
     if(TipoComando==1){
        //err = OM.ld_canale(&DBx[0], (WORD)(Index), cmd, NrDatoIni, NrDati, all_mess);
        err = OmrEthSer_LoadCanale(&DBx[0], (WORD)(Index), cmd, NrDatoIni, NrDati, all_mess);
     }
     // --- Scrittura DM
     if(TipoComando==2){
        //err = OM.wr_canale(&DBx[0], (WORD)(Index), cmd, NrDatoIni, NrDati, all_mess);
        err = OmrEthSer_WriteCanale(&DBx[0], (WORD)(Index), cmd, NrDatoIni, NrDati, all_mess);
     }
     GetLocalTime( &TimeFine );
     if( err ) ShowMessage( all_mess );
     Testo = IntToStr(atoi(DataAvailableLabel->Caption.c_str()) + 1) +
//           ": (ETH-SER) - Host IP Address:" + OmrRicevutoDaIP;
           ": (ETH-SER) - Host IP Address:" + RemoteIpEdit->Text ;
  }
  //---------------------------------------------------------
  // Test della Comunicazione FINS ETHERNET-TCP
  //---------------------------------------------------------
  if(BoxComunicazione->ItemIndex==4){
     err = OmrEthTcp_LeggiScriviCanale((short int)(Index), TipoVariabile, TipoComando, NrDatoIni, NrDati, DBx, 1, all_mess);
     GetLocalTime( &TimeFine );
     if( err ) ShowMessage( all_mess );
     Testo = IntToStr(atoi(DataAvailableLabel->Caption.c_str()) + 1) +
             ": (FINS) - Host IP Address:" + OmrRicevutoDaIP;
  }
  //---------------------------------------------------------
  // Controllo tempo trascorso
  //---------------------------------------------------------
  msec = (int) (TimeFine.wSecond - TimeInizio.wSecond);
  msec = msec * 1000;
  msec = msec + (int) (TimeFine.wMilliseconds - TimeInizio.wMilliseconds);
  Testo = Testo + " Tempo trascorso msec:" + IntToStr(msec);
  //---------------------------------------------------------
  // Visualizza Da che HOST
  //---------------------------------------------------------
  DataAvailableLabel->Caption = Testo;
  //---------------------------------------------------------
  // Visualizza Dati ricevuti
  //---------------------------------------------------------
  ListBox1->Clear();
  sprintf( stringa, "Data: 1-%d,2-%d,3-%d,4-%d,5-%d,6-%d,7-%d,8-%d,9-%d,10-%d ",
           DBx[ 0],
           DBx[ 1],
           DBx[ 2],
           DBx[ 3],
           DBx[ 4],
           DBx[ 5],
           DBx[ 6],
           DBx[ 7],
           DBx[ 8],
           DBx[ 9] );
  ListBox1->Items->Add(stringa);
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormServerUDP::BtnEsciClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormServerUDP::BtnSetComClick(TObject *Sender)
{
  int IdCom;
  char all_mess[101]="";

  //------------------------------------------------
  // Apertura della Form di settaggio COM
  //------------------------------------------------
  FormSetCom->Index = ListaCOM->ItemIndex;
  FormSetCom->ShowModal();
  //------------------------------------------------
  // Parametri della comunicazione HOST LINK
  //------------------------------------------------
  IdCom = ListaCOM->ItemIndex;
  sprintf(all_mess, "COM%02d: %d, %d, %c, %d", (IdCom+1), InCom[ IdCom ].Baudrate,
                                               InCom[ IdCom ].Dati, InCom[ IdCom ].Parita,
                                               InCom[ IdCom ].StopBit);
  TextParCom->Caption = all_mess;
}
//---------------------------------------------------------------------------
void __fastcall TFormServerUDP::ListaCOMClick(TObject *Sender)
{
  int IdCom;
  char all_mess[101]="";

  //------------------------------------------------
  // Parametri della comunicazione HOST LINK
  //------------------------------------------------
  IdCom = ListaCOM->ItemIndex;
  sprintf(all_mess, "COM%02d: %d, %d, %c, %d", (IdCom+1), InCom[ IdCom ].Baudrate,
                                               InCom[ IdCom ].Dati, InCom[ IdCom ].Parita,
                                               InCom[ IdCom ].StopBit);
  TextParCom->Caption = all_mess;
}
//---------------------------------------------------------------------------
void TFormServerUDP::IPToNode(AnsiString IPAddr, unsigned char *Node)
{
  int i, pos;
  AnsiString Testo;

  Testo = IPAddr;
  //----------------------------------------------------
  // Recupero la posizione del "node" all'interno della
  // stringa di address
  //----------------------------------------------------
  for(i=0; i<3; i++){
     pos = Testo.Pos(".");
     Testo = Testo.SubString(pos+1, 15);
  }
  //----------------------------------------------------
  // NODE = parte finale IP ADDRESS
  //----------------------------------------------------
  *Node = (unsigned char)(StrToInt(Testo));
}
//---------------------------------------------------------------------------


// -----------------------------------
//    AlgoPing()
// -----------------------------------
//
int AlgoPing( char* IpAddress, int TimeOut, int* MsecRisp, char* all_mess )
// *********************************
{
  AnsiString Stringa;
  int err=0;

  //------------
  // Ping()
  //------------
  FormServerUDP->Ping1->Timeout   =TimeOut;
  FormServerUDP->Ping1->Address   =IpAddress;
  FormServerUDP->Ping1->Ping();
  *MsecRisp=0;
  //----------------------------------------------
  // Risposta OK
  //----------------------------------------------
  if( FormServerUDP->Ping1->Reply.DataSize>0 ){
     Stringa= "Ping: Received " + IntToStr(FormServerUDP->Ping1->Reply.DataSize) + " bytes from " + FormServerUDP->Ping1->HostIP + " in " + IntToStr(FormServerUDP->Ping1->Reply.RTT) + " msecs";
     *MsecRisp= FormServerUDP->Ping1->Reply.RTT;
     strcpy( all_mess,  Stringa.c_str() );
  }else{
     Stringa = "Cannot ping host (" + FormServerUDP->Ping1->HostIP + ") : " +  FormServerUDP->Ping1->ErrorString;
     strcpy( all_mess,  Stringa.c_str() );
     err=1;
  }

  return err;
}



void __fastcall TFormServerUDP::BtnPingClick(TObject *Sender)
{
  //int err;
  int MsecRisp;
  char all_mess[300];

  AlgoPing( OmrPlc[ Index ].RemoteHost.c_str(), 500, &MsecRisp, all_mess );
  ListBox1->Clear();
  ListBox1->Items->Add(all_mess);

}
//---------------------------------------------------------------------------

