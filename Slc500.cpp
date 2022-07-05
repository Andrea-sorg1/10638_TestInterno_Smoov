// ------------------------------------------------------------------------
//             file     SLC500.CPP
// ------------------------------------------------------------------------
#include <stdio.h>
#include <mem.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "libreria.h"
#include "Main.h"
#include "TestPlc3.h"
#include "AllenBViaVbOpc.h"
//#include "UdpServer1.h"

#include "Slc500.h"

int           AB_TimeOutSocketUDP=300;


//------------------------------------------   o   ------------------------------------

//---------------------------------------------------------------------------
// Slc500_ConvertiWORDToStringa
// Converi un array di WORD ( num_canali WORD ) in array di CHAR
// invertendo ordine dei byte per essere coerente con Siemens
//---------------------------------------------------------------------------
int Slc500_ConvertiWORDToStringa( WORD* ArrayWORD, unsigned char* ArrayCHAR, int num_canali)
{
  int i, j;

  j=0;
  //-------------------------------------
  // Conversione WORD in stringa BYTE
  //-------------------------------------
  for(i=0; i<num_canali; i++){
     ArrayCHAR[ j ] = (unsigned char) (ArrayWORD[i]>>8);      j++;
     ArrayCHAR[ j ] = (unsigned char) (ArrayWORD[i]&0x00ff);  j++;
  }

  return 0;
}

//---------------------------------------------------------------------------
// Slc500_ConvertiStringaToWORD()
// Converi un array di CHAR in array di WORD  ( num_canali WORD )
// invertendo ordine dei byte per essere coerente con Siemens
//---------------------------------------------------------------------------
int Slc500_ConvertiStringaToWORD( WORD* ArrayWORD, unsigned char* ArrayCHAR, int num_canali)
{
  int i, j;

  j=0;
  //-------------------------------------
  // Conversione stringa BYTE in WORD
  //-------------------------------------
  for(i=0; i<num_canali; i++){
     ArrayWORD[ i ] = (WORD) (( ArrayCHAR[ j ]<<8) | ( ArrayCHAR[ j+1 ])) ; j=j+2;
  }

  return 0;
}

//------------------------------------
//     Slc500_InitComunicazioni()
//------------------------------------
// Da richiamare all'inizio per impostare variabili
//
int Slc500_InitComunicazioni( )
{
  char  all_mess[200];
//  int IndicePlc;
  int Plc;

  //------------------------------------------------------
  // Inizializzazione
  //------------------------------------------------------
  Slc500_TotPlc=AllenbTotPlc;

  //-------------------------------------
  // Inizializzazione n PLCs
  //-------------------------------------
  SlcPlc[ 1].WSocket = FormMain->WSocket1;
  SlcPlc[ 2].WSocket = FormMain->WSocket2;
  SlcPlc[ 3].WSocket = FormMain->WSocket3;
  SlcPlc[ 4].WSocket = FormMain->WSocket4;
  SlcPlc[ 5].WSocket = FormMain->WSocket5;
  SlcPlc[ 6].WSocket = FormMain->WSocket6;
  SlcPlc[ 7].WSocket = FormMain->WSocket7;
  SlcPlc[ 8].WSocket = FormMain->WSocket8;
  SlcPlc[ 9].WSocket = FormMain->WSocket9;
  SlcPlc[10].WSocket = FormMain->WSocket10;
  SlcPlc[11].WSocket = FormMain->WSocket11;
  SlcPlc[12].WSocket = FormMain->WSocket12;
  SlcPlc[13].WSocket = FormMain->WSocket13;
  SlcPlc[14].WSocket = FormMain->WSocket14;
  SlcPlc[15].WSocket = FormMain->WSocket15;
  SlcPlc[16].WSocket = FormMain->WSocket16;
  SlcPlc[17].WSocket = FormMain->WSocket17;
  SlcPlc[18].WSocket = FormMain->WSocket18;
  SlcPlc[19].WSocket = FormMain->WSocket19;
  SlcPlc[20].WSocket = FormMain->WSocket20;
  SlcPlc[21].WSocket = FormMain->WSocket21;
  SlcPlc[22].WSocket = FormMain->WSocket22;
  SlcPlc[23].WSocket = FormMain->WSocket23;
  SlcPlc[24].WSocket = FormMain->WSocket24;
  SlcPlc[25].WSocket = FormMain->WSocket25;
  SlcPlc[26].WSocket = FormMain->WSocket26;
  SlcPlc[27].WSocket = FormMain->WSocket27;
  SlcPlc[28].WSocket = FormMain->WSocket28;
  SlcPlc[29].WSocket = FormMain->WSocket29;
  SlcPlc[30].WSocket = FormMain->WSocket30;

  //------------------------------------
  // Inizializza Socket SLC500
  //------------------------------------
  for( Plc=1; Plc<=Slc500_TotPlc; Plc++ ){
     SlcPlc[Plc].RemotePort    = DatiPLC[Plc].Port;//2222;
     SlcPlc[Plc].RemoteHost    = DatiPLC[Plc].IpAddr; //"192.168.1.60";
     SlcPlc[Plc].EsclusionePlc = DatiPLC[Plc].DisableComunicazione;
     Slc500_SocketConnection( Plc, all_mess);
  }

  return 0;
}

//---------------------------------------------------------------------------
// Slc500_SocketConnection()
// Fa un tentativo di connessione iniziale di tutti i SOCKET
//---------------------------------------------------------------------------
int Slc500_SocketConnection( int Plc, char* all_mess)
{
  if (SlcPlc[ Plc ].WSocket->State != wsClosed) {
     //---------------------------------------------
     // Inizializza solo se Socket chiuso
     //---------------------------------------------
     if (SlcPlc[ Plc ].WSocket->State == wsClosed) {
        SlcPlc[ Plc ].WSocket->Proto = "tcp";
        SlcPlc[ Plc ].WSocket->Port  = SlcPlc[Plc].RemotePort;
        SlcPlc[ Plc ].WSocket->Addr  = SlcPlc[Plc].RemoteHost;
//        SlcPlc[ Plc ].WSocket->Connect();
        SlcPlc[ Plc ].ConnessioneSocketInCorso=0;
        SlcPlc[ Plc ].HeaderCspv4Rx.conn_id=0; // Azzera connessione
     }
  }

  return 0;
}




//---------------------------------------------------------------------------
// Slc500_TrasmettiRiceviPacchettoTCP()
// Trasmetti e riceve un pacchetto TCP  tramite il componente WSOCKET (Pieffe)
//---------------------------------------------------------------------------
int Slc500_TrasmettiRiceviPacchettoTCP( WORD Plc, char* PacchettoDaTrasmettere, char* PacchettoRicevuto, int BuffSize, int LenTrs, int ByteDaRicevere,  int &ByteRicevuti, int AttesaRisposta, char* all_mess)
{
  SYSTEMTIME TimeInizio;
  SYSTEMTIME TimeFine;
  int WaitReponse;
  int msec, pos1;
  int ByteInArrivo;
  TSockAddrIn Src;
  int         SrcLen;

  ByteRicevuti=0;
  strcpy( PacchettoRicevuto, "");
  strcpy( all_mess, "");

  //--------------------------
  // Controllo
  //--------------------------
  if( Plc >100 ){     sprintf( all_mess, "SLC500 TCP: Indice PLC errato");     return 1;  }

  //-----------------------------------
  // Se Socket NON connesso
  //-----------------------------------
  if (SlcPlc[ Plc ].WSocket->State != wsConnected) {
     SlcPlc[ Plc ].HeaderCspv4Rx.conn_id=0; // Azzera connessione
     //---------------------------------------------
     // Inizializza solo se Socket chiuso
     //---------------------------------------------
     if (SlcPlc[ Plc ].WSocket->State == wsClosed) {
        SlcPlc[ Plc ].WSocket->Proto = "tcp";
        SlcPlc[ Plc ].WSocket->Port  = SlcPlc[Plc].RemotePort;
        SlcPlc[ Plc ].WSocket->Addr  = SlcPlc[Plc].RemoteHost;
        SlcPlc[ Plc ].WSocket->Connect();
        SlcPlc[ Plc ].ConnessioneSocketInCorso=0;
     }
     if( SlcPlc[ Plc ].WSocket->State == wsConnecting) {
           //-------------------------------------------------------------
           // 07/04/2005 Modifica per controllo connessione senza attesa
           //-------------------------------------------------------------
           //if( SlcPlc[ Plc ].ConnessioneSocketInCorso<=50 )  SlcPlc[ Plc ].ConnessioneSocketInCorso++;

     }
  }
  if ( SlcPlc[ Plc ].WSocket->State != wsConnected) {
     sprintf( all_mess, "Connessione in corso al Disposisivo IP:%s Port=%s",
     SlcPlc[ Plc ].WSocket->Addr.c_str(),  SlcPlc[ Plc ].WSocket->Port.c_str()  );
     return 1;
  }
  // Be sure we are connected before sending anything
  if (SlcPlc[ Plc ].WSocket->State == wsConnected){
     try{
        SlcPlc[ Plc ].WSocket->Send( PacchettoDaTrasmettere, LenTrs );
     }
     catch (Exception &exception){
        //Application->ShowException(&exception);
        pos1 = exception.Message.Pos("\n");
        exception.Message.Delete(pos1-1, 1);
        exception.Message.Insert(" - ", pos1);
        strcpy(all_mess, exception.Message.c_str());
        SlcPlc[ Plc ].WSocket->Close();
        return 1;
     }
     SrcLen = sizeof(Src);
     //--------------------------------------------------------------------
     // ATTESA PACCHETTO SENZA BLOCCO EVENTI !!!!
     // Connect is asynchronous (non-blocking). We will wait while the
     // session is connecting or application terminated.
     //--------------------------------------------------------------------
     if( AttesaRisposta ){
        msec=0;
        WaitReponse=0;
        GetLocalTime( &TimeInizio );

        strcpy(  PacchettoRicevuto, "" );
        for( ;; ){
           //-------------------------------------------------------
           // Controllo se arrivato il pacchetto completo
           //-------------------------------------------------------
           if( SlcPlc[ Plc ].WSocket->RcvdCount ){
              ByteInArrivo    = SlcPlc[ Plc ].WSocket->RcvdCount;
              if( ByteInArrivo>=ByteDaRicevere ) break;
           }
           //------------------------
           // Controllo TIME_OUT
           //------------------------
           if( WaitReponse>=1000 ){
              GetLocalTime( &TimeFine );
              msec = (int) (TimeFine.wSecond - TimeInizio.wSecond);
              if( msec<0 ) msec+=60;
              msec = msec * 1000;
              msec = msec + (int) (TimeFine.wMilliseconds - TimeInizio.wMilliseconds);
              // Verifica che il ritardo non superi TimeOut
              if( msec > AB_TimeOutSocketUDP ) break;
              if( msec < 0 ) break;
              WaitReponse=0;
           }
           WaitReponse++;
        }
        //--------------------------
        // Ricezione
        //--------------------------
        ByteRicevuti    =  SlcPlc[ Plc ].WSocket->ReceiveFrom(PacchettoRicevuto, BuffSize, Src, SrcLen);

        if( ByteRicevuti<=0 ){
           //SlcPlc[ Plc ].WSocket->Close();
           sprintf( all_mess, "Time-Out risposta Dispositivo msec %d", msec ); return 1;
        }
        // Recupero indirizzo IP reale Host Remotodati ricevuti
        //IndirizzoIpMittente = WSocket_inet_ntoa(Src.sin_addr);
     }
     //   SlcPlc[ Plc ].WSocket->Close();
     return 0;
  }

  sprintf( all_mess, "Impossibile connettersi al Dispositivo IP:%s Port=%d", &SlcPlc[Plc].RemoteHost, SlcPlc[Plc].RemotePort );
  SlcPlc[ Plc ].WSocket->Close();
  return 1;
}



//---------------------------------------------------------------------------
// Slc500_TxPacchettoTCP()
// Trasmette un pacchetto TCP
//---------------------------------------------------------------------------
int Slc500_TxPacchettoTCP( WORD Plc, char* PacchettoDaTrasmettere, int LenTrs, char* all_mess)
{
//  SYSTEMTIME TimeInizio;
//  SYSTEMTIME TimeFine;
//  int WaitReponse;
//  int msec, pos1;
//  int ByteInArrivo;
//  TSockAddrIn Src;
//  int         SrcLen;
    int pos1;

  strcpy( all_mess, "");
  //--------------------------
  // Controllo
  //--------------------------
  if( Plc >100 ){     sprintf( all_mess, "SLC500 TCP: Indice PLC errato");     return 1;  }

  //-----------------------------------
  // Se Socket NON connesso
  //-----------------------------------
  if (SlcPlc[ Plc ].WSocket->State != wsConnected) {
     SlcPlc[ Plc ].HeaderCspv4Rx.conn_id=0; // Azzera connessione
     //---------------------------------------------
     // Inizializza solo se Socket chiuso
     //---------------------------------------------
     if (SlcPlc[ Plc ].WSocket->State == wsClosed) {
        SlcPlc[ Plc ].WSocket->Proto = "tcp";
        SlcPlc[ Plc ].WSocket->Port  = SlcPlc[Plc].RemotePort;
        SlcPlc[ Plc ].WSocket->Addr  = SlcPlc[Plc].RemoteHost;
        SlcPlc[ Plc ].WSocket->Connect();
        SlcPlc[ Plc ].ConnessioneSocketInCorso=0;
     }
     if( SlcPlc[ Plc ].WSocket->State == wsConnecting) {
           //-------------------------------------------------------------
           // 07/04/2005 Modifica per controllo connessione senza attesa
           //-------------------------------------------------------------
           //if( SlcPlc[ Plc ].ConnessioneSocketInCorso<=50 )  SlcPlc[ Plc ].ConnessioneSocketInCorso++;
     }
  }
  if ( SlcPlc[ Plc ].WSocket->State != wsConnected) {
     sprintf( all_mess, "Connessione in corso al Disposisivo IP:%s Port=%s",
     SlcPlc[ Plc ].WSocket->Addr.c_str(),  SlcPlc[ Plc ].WSocket->Port.c_str()  );
     return 1;
  }
  // Be sure we are connected before sending anything
  if (SlcPlc[ Plc ].WSocket->State == wsConnected){
     try{
        SlcPlc[ Plc ].WSocket->Send( PacchettoDaTrasmettere, LenTrs );
     }
     catch (Exception &exception){
        //Application->ShowException(&exception);
        pos1 = exception.Message.Pos("\n");
        exception.Message.Delete(pos1-1, 1);
        exception.Message.Insert(" - ", pos1);
        strcpy(all_mess, exception.Message.c_str());
        SlcPlc[ Plc ].WSocket->Close();
        return 1;
     }
     return 0;
  }

  sprintf( all_mess, "Impossibile connettersi al Dispositivo IP:%s Port=%d", &SlcPlc[Plc].RemoteHost, SlcPlc[Plc].RemotePort );
  SlcPlc[ Plc ].WSocket->Close();
  return 1;
}



//---------------------------------------------------------------------------
// Slc500_RxPacchettoTCP()
// Riceve un pacchetto TCP  tramite il componente WSOCKET (Pieffe)
//---------------------------------------------------------------------------
int Slc500_RxPacchettoTCP( WORD Plc, char* PacchettoRicevuto, int BuffSize, int ByteDaRicevere,  int &ByteRicevuti, int AttesaRisposta, char* all_mess)
{
//  SYSTEMTIME TimeInizio;
//  SYSTEMTIME TimeFine;
//  int WaitReponse;
//  int msec, pos1;
  int ByteInArrivo;
  TSockAddrIn Src;
  int         SrcLen;

  ByteRicevuti=0;
  strcpy( PacchettoRicevuto, "");
  strcpy( all_mess, "");

  //--------------------------
  // Controllo
  //--------------------------
  if( Plc >100 ){     sprintf( all_mess, "SLC500 TCP: Indice PLC errato");     return 1;  }

  //-----------------------------------
  // Se Socket NON connesso
  //-----------------------------------
  if ( SlcPlc[ Plc ].WSocket->State != wsConnected) {
     sprintf( all_mess, "Plc%d Socket non connesso", Plc );
     return 1;
  }

  //-------------------------------------------------------
  // Controllo se arrivato il pacchetto completo
  //-------------------------------------------------------
  SrcLen = sizeof(Src);
  ByteInArrivo=0;
  if( SlcPlc[ Plc ].WSocket->RcvdCount ){
     ByteInArrivo    = SlcPlc[ Plc ].WSocket->RcvdCount;
     if( ByteInArrivo>=ByteDaRicevere ){
        //--------------------------
        // Ricezione
        //--------------------------
        ByteRicevuti    =  SlcPlc[ Plc ].WSocket->ReceiveFrom(PacchettoRicevuto, BuffSize, Src, SrcLen);
        if( ByteRicevuti>0 ){
           return 0;
        }
     }
  }

  sprintf( all_mess, "Plc%d Dati non arrivati", Plc );
  return 1;
}


//-------------------------------------------   o  -----------------------------------------------







//-----------------------------------------
//     Slc500_PcccConnection()
// Singolo PLC
//-----------------------------------------
int Slc500_PcccConnection( int Plc, char* all_mess)
{
  int   err=0;
  int   LenHeader;
  char  PacchettoTx[900];
  char  PacchettoRx[900];
  int   BuffSize = 500;
  int   ByteDaRicevere=28;
  int   ByteRicevuti;
  int   LenTx;

  // Calcolo della lunghezza del pacchetto
  sprintf( all_mess, "");
  memset( PacchettoTx, 0, sizeof(PacchettoTx));
  memset( PacchettoRx, 0, sizeof(PacchettoRx));
  //-----------------------------------------------
  // Compila Header
  //-----------------------------------------------
  memset( &SlcPlc[ Plc ].HeaderCspv4Tx, 0, sizeof( cspv4_header));
  SlcPlc[ Plc ].HeaderCspv4Tx.mode        =1;            // ( 1 byte) 01 = request, 02 = response
  SlcPlc[ Plc ].HeaderCspv4Tx.submode     =1;            // ( 1 byte) 01 = connection, 07 = PCCC?? (not sure the name)
  SlcPlc[ Plc ].HeaderCspv4Tx.data_lenght =0;            // ( 2 byte) how many bytes AFTER this header
  SlcPlc[ Plc ].HeaderCspv4Tx.conn_id     =0;            // ( 4 byte) ID assigned by slave/server
  SlcPlc[ Plc ].HeaderCspv4Tx.status      =0;            // ( 1 byte) 01 = request, 02 = response
  SlcPlc[ Plc ].HeaderCspv4Tx.context[1]  =4;            // (16 byte) client info - slave/server just echos
  SlcPlc[ Plc ].HeaderCspv4Tx.context[3]  =5;            // (16 byte) client info - slave/server just echos

  LenHeader=sizeof( cspv4_header );
  memcpy( &PacchettoTx[ 0], &SlcPlc[ Plc ].HeaderCspv4Tx, LenHeader );

  //-----------------------------------------------
  // Trasmetti Stringa
  //-----------------------------------------------
  //if (SlcPlc[ Plc ].WSocket->State != wsConnected) { sprintf( all_mess, "Plc%d Tcp unconnected ", Plc ); err=1; return 1;}
  LenTx=LenHeader ; // 28 Byte
  err= Slc500_TrasmettiRiceviPacchettoTCP( (WORD) Plc, (char *) PacchettoTx, (char *) PacchettoRx, BuffSize, LenTx, ByteDaRicevere,  ByteRicevuti, true, all_mess);

  //-----------------------------------------------
  // Verifica Stringa Ricevuta
  //-----------------------------------------------
  if( err==0 ){
     if( ByteRicevuti!=28 ) { sprintf( all_mess, "Plc%d Slc500_PccConnection() Len Data NotOk ", Plc ); err=1; return 1;}
     memcpy( &SlcPlc[ Plc ].HeaderCspv4Rx, &PacchettoRx[ 0 ], sizeof( cspv4_header ) );
     if( SlcPlc[ Plc ].HeaderCspv4Rx.status!= 0 ) {  sprintf( all_mess, "Plc%d Slc500_PccConnection() Plc ha restituito errore", Plc ); err=1; return 1;}
  }

  return err;
}


int Slc500_PcccLeggiWord( int Plc, int NumFile, int WordIni, int NumWord, WORD Data[],char* all_mess)
{
  int   err=0;
  int   LenHeader;
  char  PacchettoTx[900];
  char  PacchettoRx[900];
  int   BuffSize = 500;
  int   ByteDaRicevere=20;
  int   ByteRicevuti;
  int   LenTx;
  unsigned short LenData;
  struct local_lsap Lsap;
  int   NumByteDaRicevere;

  if( NumWord<1 )NumWord=1;
  memset( PacchettoTx, 0, sizeof(PacchettoTx));
  memset( PacchettoRx, 0, sizeof(PacchettoRx));
  //-----------------------------------------------
  // Se manca connessione SOCKET
  //-----------------------------------------------
  if (SlcPlc[ Plc ].WSocket->State != wsConnected) {
     SlcPlc[ Plc ].HeaderCspv4Rx.conn_id=0;
     Slc500_SocketConnection( Plc, all_mess);
     sprintf( all_mess, "Plc%d Slc500_PcccLeggiWord() Socket connection ", Plc );
     return 1;
  }
  //-----------------------------------------------
  // Se manca Connessione PCCC
  //-----------------------------------------------
  if( SlcPlc[ Plc ].HeaderCspv4Rx.conn_id==0 ){
     err = Slc500_PcccConnection( Plc, all_mess);
     if( err ) return 1;
  }
  //-----------------------------------------------
  // Compila Stringa Header Cspv4
  //-----------------------------------------------
  LenData = 14;
  memset( &SlcPlc[ Plc ].HeaderCspv4Tx, 0, sizeof(cspv4_header));
  SlcPlc[ Plc ].HeaderCspv4Tx.mode        =1;            // ( 1 byte) 01 = request, 02 = response
  SlcPlc[ Plc ].HeaderCspv4Tx.submode     =7;            // ( 1 byte) 01 = connection, 07 = PCCC?? (not sure the name)
  Slc500_ConvertiWORDToStringa( &LenData, (unsigned char*) &SlcPlc[ Plc ].HeaderCspv4Tx.data_lenght, 1); // Inverti byte BIG_INDIAN
  SlcPlc[ Plc ].HeaderCspv4Tx.conn_id     =SlcPlc[ Plc ].HeaderCspv4Rx.conn_id;            // ( 4 byte) ID assigned by slave/server
  SlcPlc[ Plc ].HeaderCspv4Tx.status      =0;            // ( 1 byte) 01 = request, 02 = response
  SlcPlc[ Plc ].HeaderCspv4Tx.context[1]  =4;            // (16 byte) client info - slave/server just echos
  SlcPlc[ Plc ].HeaderCspv4Tx.context[3]  =5;            // (16 byte) client info - slave/server just echos

  LenHeader=sizeof( cspv4_header );
  memcpy( &PacchettoTx[ 0], &SlcPlc[ Plc ].HeaderCspv4Tx, LenHeader );
  //-----------------------------------------------
  // Compila Stringa Header LSAP
  //-----------------------------------------------
  Lsap.dst=   0;        // ( 1 byte) Destination byte - like slve address
  Lsap.res5=  5;        // ( 1 byte) Control byte = seems fixed as 5 (not sure what it does)
  Lsap.src=   0;        // ( 1 byte) Source byte - like master address
  Lsap.lsap=  0;        // ( 1 byte) LSAP point = fixeed (0)

  memcpy( &PacchettoTx[28], &Lsap, 4 );
  //-----------------------------------------------
  // Compila Stringa Header PCCC
  //-----------------------------------------------
  PacchettoTx[32] = 0x0F;                // Function    0x0F
  PacchettoTx[33] = 0x00;                // STS ...
  PacchettoTx[34] = 0x00;                // TNS ( unique transactionidentifier)
  PacchettoTx[35] = 0x02;                // TNS ( unique transactionidentifier) sembra ok qualsiasi valore
  PacchettoTx[36] = 0xA2;                // Subfunction 0xA2
  PacchettoTx[37] = (byte)(NumWord*2);   // Size of byte
  PacchettoTx[38] = (byte)(NumFile);     // Numero del File (es 7)
  PacchettoTx[39] = 0x89;                // Tipo di file es 89 = File di tipo 'N'
  PacchettoTx[40] = (byte) WordIni;      // Element number    // Indice del campo iniziale
  PacchettoTx[41] = 0x00;                // Sub element number

  //-----------------------------------------------
  // Trasmetti Stringa
  //-----------------------------------------------

  LenTx=LenHeader + LenData; // 28 byte Header  +4 byte Lsap + x byte Pcc
  err= Slc500_TrasmettiRiceviPacchettoTCP( (WORD) Plc, (char *) PacchettoTx, (char *) PacchettoRx, BuffSize, LenTx, ByteDaRicevere,  ByteRicevuti, true, all_mess);

  //-----------------------------------------------
  // Verifica Stringa Ricevuta
  //-----------------------------------------------
  if( err==0 ){
     NumByteDaRicevere=36+NumWord*2;
     if( ByteRicevuti!=NumByteDaRicevere ) { sprintf( all_mess, "Plc%d Slc500_PcccLeggiWord() Len Data NotOk ", Plc ); err=1; }
     if( err==0 )  memcpy( &SlcPlc[ Plc ].HeaderCspv4Rx, &PacchettoRx[ 0 ], sizeof( cspv4_header ) );
     if( err==0 && SlcPlc[ Plc ].HeaderCspv4Rx.status!= 0 ) {  sprintf( all_mess, "Plc%d Slc500_PcccLeggiWord() Error code from Plc", Plc ); err=1;}
     if( err==0 ) memcpy( Data, &PacchettoRx[ 36 ], NumWord*2 );
  }
  if( ByteRicevuti==0 ) SlcPlc[ Plc ].HeaderCspv4Rx.conn_id=0;

  return err;
}



int Slc500_PcccScriviWord( int Plc, int NumFile, int WordIni, int NumWord, WORD Data[],char* all_mess)
{
  int   err=0;
  int   LenHeader;
  char  PacchettoTx[900];
  char  PacchettoRx[900];
  int   BuffSize = 500;
  int   ByteDaRicevere=20;
  int   ByteRicevuti;
  int   LenTx;
  unsigned short LenData;
  struct local_lsap Lsap;
  int   NumByteDaRicevere;

  if( NumWord<1 )NumWord=1;
  memset( PacchettoTx, 0, sizeof(PacchettoTx));
  memset( PacchettoRx, 0, sizeof(PacchettoRx));
  //-----------------------------------------------
  // Se manca connessione SOCKET
  //-----------------------------------------------
  if (SlcPlc[ Plc ].WSocket->State != wsConnected) {
     SlcPlc[ Plc ].HeaderCspv4Rx.conn_id=0;
     Slc500_SocketConnection( Plc, all_mess);
     sprintf( all_mess, "Plc%d Slc500_PcccLeggiWord() Socket connection ", Plc );
     return 1;
  }
  //-----------------------------------------------
  // Se manca Connessione PCCC
  //-----------------------------------------------
  if( SlcPlc[ Plc ].HeaderCspv4Rx.conn_id==0 ){
     err = Slc500_PcccConnection( Plc, all_mess);
     if( err ) return 1;
  }
  //-----------------------------------------------
  // Compila Stringa Header Cspv4
  //-----------------------------------------------
  LenData =(unsigned short)( 14 + (NumWord*2));
  memset( &SlcPlc[ Plc ].HeaderCspv4Tx, 0, sizeof(cspv4_header));
  SlcPlc[ Plc ].HeaderCspv4Tx.mode        =1;            // ( 1 byte) 01 = request, 02 = response
  SlcPlc[ Plc ].HeaderCspv4Tx.submode     =7;            // ( 1 byte) 01 = connection, 07 = PCCC?? (not sure the name)
  //HeaderCspv4Tx.data_lenght =LenData;      // ( 2 byte) how many bytes AFTER this header
  Slc500_ConvertiWORDToStringa( &LenData, (unsigned char*) &SlcPlc[ Plc ].HeaderCspv4Tx.data_lenght, 1);
  SlcPlc[ Plc ].HeaderCspv4Tx.conn_id     =SlcPlc[ Plc ].HeaderCspv4Rx.conn_id;            // ( 4 byte) ID assigned by slave/server
  SlcPlc[ Plc ].HeaderCspv4Tx.status      =0;            // ( 1 byte) 01 = request, 02 = response
  SlcPlc[ Plc ].HeaderCspv4Tx.context[1]  =4;            // (16 byte) client info - slave/server just echos
  SlcPlc[ Plc ].HeaderCspv4Tx.context[3]  =5;            // (16 byte) client info - slave/server just echos

  LenHeader=sizeof( cspv4_header );
  memcpy( &PacchettoTx[ 0], &SlcPlc[ Plc ].HeaderCspv4Tx, LenHeader );
  //-----------------------------------------------
  // Compila Stringa Header LSAP
  //-----------------------------------------------
  Lsap.dst=   0;        // ( 1 byte) Destination byte - like slve address
  Lsap.res5=  5;        // ( 1 byte) Control byte = seems fixed as 5 (not sure what it does)
  Lsap.src=   0;        // ( 1 byte) Source byte - like master address
  Lsap.lsap=  0;        // ( 1 byte) LSAP point = fixeed (0)

  memcpy( &PacchettoTx[28], &Lsap, 4 );
  //-----------------------------------------------
  // Compila Stringa Header PCCC
  //-----------------------------------------------
  PacchettoTx[32] = 0x0F;                // Function    0x0F
  PacchettoTx[33] = 0x00;                // STS ...
  PacchettoTx[34] = 0x00;                // TNS ( unique transactionidentifier)
  PacchettoTx[35] = 0x02;                // TNS ( unique transactionidentifier)  Divrso da zero
  PacchettoTx[36] = 0xAA;                // Subfunction 0xA2
  PacchettoTx[37] = (byte)(NumWord*2);   // Size of byte
  //PacchettoTx[38] = 0x07;                // Numero del File (es 7)
  PacchettoTx[38] = (byte)(NumFile);     // Numero del File (es 7)
  PacchettoTx[39] = 0x89;                // Tipo di file es 89 = File di tipo 'N'
  PacchettoTx[40] = (byte) WordIni;      // Element number    // Indice del campo iniziale
  PacchettoTx[41] = 0x00;                // Sub element number

  //-----------------------------------------------
  // Compila Dati da scrivere
  //-----------------------------------------------
  memcpy( &PacchettoTx[ 42 ], Data, NumWord*2 );

  //-----------------------------------------------
  // Trasmetti Stringa
  //-----------------------------------------------

  LenTx=LenHeader + LenData; // 28 byte Header  +4 byte Lsap + x byte Pcc
  err= Slc500_TrasmettiRiceviPacchettoTCP( (WORD) Plc, (char *) PacchettoTx, (char *) PacchettoRx, BuffSize, LenTx, ByteDaRicevere,  ByteRicevuti, true, all_mess);

  //-----------------------------------------------
  // Verifica Stringa Ricevuta
  //-----------------------------------------------
  if( err==0 ){
     NumByteDaRicevere=36;
     if( ByteRicevuti!=NumByteDaRicevere ) { sprintf( all_mess, "Plc%d Slc500_PcccScriviWord() Len Data NotOk ", Plc ); err=1; }
     if( err==0 )  memcpy( &SlcPlc[ Plc ].HeaderCspv4Rx, &PacchettoRx[ 0 ], sizeof( cspv4_header ) );
     if( err==0 && SlcPlc[ Plc ].HeaderCspv4Rx.status!= 0 ) {  sprintf( all_mess, "Plc%d Slc500_PcccScriviWord() Error code from Plc", Plc ); err=1;}
  }
  if( ByteRicevuti==0 ) SlcPlc[ Plc ].HeaderCspv4Rx.conn_id=0;

  return err;
}


//-------------------------------------------------------  o  -----------------------------------------------

//---------------------------------------------------------------------------
// Slc500_SocketMultiConnection()
//---------------------------------------------------------------------------
int Slc500_SocketMultiConnection( char* all_mess)
{
  int i;

  //---------------------------------------------
  // Inizializza tutti i Socket
  //---------------------------------------------
  for( i=1; i<=Slc500_TotPlc; i++ ){
     Slc500_SocketConnection( i, all_mess);
  }

  return 0;
}






//-----------------------------------------
//     Slc500_PcccMultiConnection()
//-----------------------------------------
int Slc500_PcccMultiConnection(char* all_mess )
{
  SYSTEMTIME TimeInizio;
  SYSTEMTIME TimeFine;
  char  all_mess2[200];
  int   WaitReponse;
  int   msec; //, pos1;
  int   err=0;

  int   LenHeader;
  char  PacchettoTx[900];
  char  PacchettoRx[900];
  int   BuffSize = 500;
  int   ByteDaRicevere=28;
  int   ByteRicevuti;
  int   LenTx;
  int   Plc;
  bool  SempreON=true;
  int   RicezioneCompleta;


  //--------------------------
  // (0) Inizializza
  //--------------------------
  for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
     SlcPlc[Plc].ErrPlc=0;
     SlcPlc[Plc].MultiRxPlcOk=0;
     if( SlcPlc[ Plc ].HeaderCspv4Rx.conn_id ) SlcPlc[Plc].MultiRxPlcOk=1;
  }
  //-----------------------------------------------------------------------------
  // (1) Trasmetti Pacchetto TCP a tutti i PLCs
  //-----------------------------------------------------------------------------
  // Calcolo della lunghezza del pacchetto
  sprintf( all_mess, "");
  for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
     if( SlcPlc[Plc].MultiRxPlcOk  ) continue;  // Se già OK al precedente tentativo          -->SKIP
     memset( PacchettoTx, 0, sizeof(PacchettoTx));
     memset( PacchettoRx, 0, sizeof(PacchettoRx));
     //-----------------------------------------------
     // Compila Header
     //-----------------------------------------------
     memset( &SlcPlc[ Plc ].HeaderCspv4Tx, 0, sizeof( cspv4_header));
     SlcPlc[ Plc ].HeaderCspv4Tx.mode        =1;            // ( 1 byte) 01 = request, 02 = response
     SlcPlc[ Plc ].HeaderCspv4Tx.submode     =1;            // ( 1 byte) 01 = connection, 07 = PCCC?? (not sure the name)
     SlcPlc[ Plc ].HeaderCspv4Tx.data_lenght =0;            // ( 2 byte) how many bytes AFTER this header
     SlcPlc[ Plc ].HeaderCspv4Tx.conn_id     =0;            // ( 4 byte) ID assigned by slave/server
     SlcPlc[ Plc ].HeaderCspv4Tx.status      =0;            // ( 1 byte) 01 = request, 02 = response
     SlcPlc[ Plc ].HeaderCspv4Tx.context[1]  =4;            // (16 byte) client info - slave/server just echos
     SlcPlc[ Plc ].HeaderCspv4Tx.context[3]  =5;            // (16 byte) client info - slave/server just echos

     LenHeader=sizeof( cspv4_header );
     memcpy( &PacchettoTx[ 0], &SlcPlc[ Plc ].HeaderCspv4Tx, LenHeader );

     //-----------------------------------------------
     // Trasmetti Stringa
     //-----------------------------------------------
     LenTx=LenHeader ; // 28 Byte
     //err= Slc500_TxrasmettiRiceviPacchettoTCP( (WORD) Plc, (char *) PacchettoTx, (char *) PacchettoRx, BuffSize, LenTx, ByteDaRicevere,  ByteRicevuti, true, all_mess);
     //err = Slc500_TxPacchettoTCP( WORD Plc, char* PacchettoDaTrasmettere, int LenTrs, char* all_mess);
     err = Slc500_TxPacchettoTCP((WORD) Plc, (char *) PacchettoTx, LenTx, all_mess);
  }


  //---------------------------------------------------------------------------
  // (2) Attesa risposta da TUTTI i PLC per il massimo di TimeOut
  //---------------------------------------------------------------------------
  if( SempreON ){
     WaitReponse=0;
     GetLocalTime( &TimeInizio );
     for(;;){
        RicezioneCompleta =1;
        for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
           if( SlcPlc[Plc].EsclusionePlc ) continue;  // Se la comunicazione con AGV è esclusa      -->SKIP
           if( SlcPlc[Plc].ErrPlc>0      ) continue;  // Se già errore in SEND inutile aspettare risposta
           if( SlcPlc[Plc].MultiRxPlcOk  ) continue;  // Se già OK al precedente tentativo          -->SKIP

           //----------------------------------------------------------
           // Controllo se arrivato il pacchetto completo senza errori
           //----------------------------------------------------------
           if( SlcPlc[ Plc ].WSocket->State != wsConnected     ) {RicezioneCompleta=0; continue;}
           if( SlcPlc[ Plc ].WSocket->RcvdCount < 10 ) RicezioneCompleta=0;
        }

        //-------------------------------------
        // Risposta OK tutti i PLC --> break
        // Risposta      TIME_OUT  --> break
        //-------------------------------------
        if( RicezioneCompleta )          break;
        if( WaitReponse>=100 ){
           GetLocalTime( &TimeFine );
           msec = (int) (TimeFine.wSecond - TimeInizio.wSecond);
           if( msec<0 ) msec+=60;
           msec = msec * 1000;
           msec = msec + (int) (TimeFine.wMilliseconds - TimeInizio.wMilliseconds);
           // Verifica che il ritardo non superi TimeOut
           if( msec > AB_TimeOutSocketUDP ) break;
           if( msec < 0 ) break;
           WaitReponse=0;
        }
        WaitReponse++;
     }
  }


  //---------------------------------------------------------------------------
  // (3) Controllo Risposta dei vari PLC
  //---------------------------------------------------------------------------
  if( SempreON ){
     for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
        err = 0;
        if( SlcPlc[Plc].EsclusionePlc ) continue;  // Se la comunicazione con AGV è esclusa      -->SKIP
        if( SlcPlc[Plc].ErrPlc>0      ) continue;  // Se già errore in SEND inutile aspettare risposta
        if( SlcPlc[Plc].MultiRxPlcOk  ) continue;  // Se già OK al precedente tentativo          -->SKIP
        memset( PacchettoRx,        0, sizeof( PacchettoRx   ));
        //------------------------------
        // Recupero e controllo stringa
        //------------------------------
        if( err==0 ) err = Slc500_RxPacchettoTCP( (WORD) Plc, PacchettoRx, BuffSize,  ByteDaRicevere, ByteRicevuti, WaitReponse, all_mess2);
        if( err==0 && ByteRicevuti!=28 ) { err=1; sprintf( all_mess, "Plc%d Slc500_PccConnection() Len Data NotOk ", Plc ); SlcPlc[Plc].ErrPlc=1; continue;}
        memcpy( &SlcPlc[ Plc ].HeaderCspv4Rx, &PacchettoRx[ 0 ], sizeof( cspv4_header ) );
        if( err==0 && SlcPlc[ Plc ].HeaderCspv4Rx.status!= 0 ) { SlcPlc[Plc].ErrPlc=1; sprintf( all_mess, "Plc%d Slc500_PccConnection() Plc ha restituito errore", Plc ); continue;}
        /*
        //-------------------------------------------
        // Se lettura OK copio i dati ricevuti
        //-------------------------------------------
        if( err==0 ){
           if( TipoComando==1 ){
              //---------------------------------------------
              // Elabora Stringa
              // Calcolo dell'array di WORD i byte invertiti
              //---------------------------------------------
              memset( WORDRicezione, 0, sizeof( WORDRicezione) );
              OmrEthTcp_ConvertiStringaToWord( WORDRicezione, &StringaRicezione[BYTE_INTESTAZIONE_RSV], (NrDati*2));
              memcpy( &OmrEth_DM_TxRx[ IndicePlc ][0], WORDRicezione, NrDati*2);
              strcpy( &OmrEth_MessaggioAllarmePlc[ IndicePlc ][0], "" );
           }
           OmrEthTcp_RicezioneOK[IndicePlc]=1;   // Ricezione OK
        }
        */
        //----------------------------------
        // Memorizza errore relativo al PLC
        //----------------------------------
        if( err ){
           SlcPlc[Plc].ErrPlc=err;
           strcpy( all_mess, all_mess2 );
           //strcpy( &OmrEth_MessaggioAllarmePlc[ IndicePlc ][0], all_mess );
        }
     }
  }
  //OmrEthTcp_MultiPlcTrasmissioneInCorso=0;
  //--------------------------------------------------
  // Se presente anche un solo errore --> ERR
  //--------------------------------------------------
  err=0;
  for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
     if( SlcPlc[Plc].ErrPlc>0 ) return 1;
  }

  return err;
}




//-----------------------------------------
//     Slc500_PcccMultiLeggiWord()
//-----------------------------------------
int Slc500_PcccMultiLeggiWord(char* all_mess )
{
  SYSTEMTIME TimeInizio;
  SYSTEMTIME TimeFine;
  char  all_mess2[200];
  int   WaitReponse;
  int   msec; //, pos1;
  int   err=0;

  int   LenHeader;
  char  PacchettoTx[900];
  char  PacchettoRx[900];
  int   BuffSize = 500;
  int   ByteDaRicevere=28;
  int   ByteRicevuti;
  int   LenTx;
  int   Plc;
  bool  SempreON=true;
  int   RicezioneCompleta;
  unsigned short LenData;
  struct local_lsap Lsap;
  int NumWord;


  //--------------------------------------------------------------
  // Connetti tutti i Socket e tutti i canali di connessione PCCC
  //--------------------------------------------------------------
  Slc500_SocketMultiConnection( all_mess );
  Slc500_PcccMultiConnection  ( all_mess );
  //--------------------------
  // (0) Inizializza
  //--------------------------
  for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
     SlcPlc[Plc].ErrPlc=0;
     SlcPlc[Plc].MultiRxPlcOk=0;
  }
  //-----------------------------------------------------------------------------
  // (1) Trasmetti Pacchetto TCP a tutti i PLCs
  //-----------------------------------------------------------------------------
  // Calcolo della lunghezza del pacchetto
  sprintf( all_mess, "");
  for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
     if( SlcPlc[Plc].MultiRxPlcOk  ) continue;  // Se già OK al precedente tentativo          -->SKIP
     memset( PacchettoTx, 0, sizeof(PacchettoTx));
     memset( PacchettoRx, 0, sizeof(PacchettoRx));

     //-----------------------------------------------
     // Compila Stringa Header Cspv4
     //-----------------------------------------------
     LenData = 14;
     memset( &SlcPlc[ Plc ].HeaderCspv4Tx, 0, sizeof(cspv4_header));
     SlcPlc[ Plc ].HeaderCspv4Tx.mode        =1;            // ( 1 byte) 01 = request, 02 = response
     SlcPlc[ Plc ].HeaderCspv4Tx.submode     =7;            // ( 1 byte) 01 = connection, 07 = PCCC?? (not sure the name)
     Slc500_ConvertiWORDToStringa( &LenData, (unsigned char*) &SlcPlc[ Plc ].HeaderCspv4Tx.data_lenght, 1); // Inverti byte BIG_INDIAN
     SlcPlc[ Plc ].HeaderCspv4Tx.conn_id     =SlcPlc[ Plc ].HeaderCspv4Rx.conn_id;            // ( 4 byte) ID assigned by slave/server
     SlcPlc[ Plc ].HeaderCspv4Tx.status      =0;            // ( 1 byte) 01 = request, 02 = response
     SlcPlc[ Plc ].HeaderCspv4Tx.context[1]  =4;            // (16 byte) client info - slave/server just echos
     SlcPlc[ Plc ].HeaderCspv4Tx.context[3]  =5;            // (16 byte) client info - slave/server just echos

     LenHeader=sizeof( cspv4_header );
     memcpy( &PacchettoTx[ 0], &SlcPlc[ Plc ].HeaderCspv4Tx, LenHeader );
     //-----------------------------------------------
     // Compila Stringa Header LSAP
     //-----------------------------------------------
     Lsap.dst=   0;        // ( 1 byte) Destination byte - like slve address
     Lsap.res5=  5;        // ( 1 byte) Control byte = seems fixed as 5 (not sure what it does)
     Lsap.src=   0;        // ( 1 byte) Source byte - like master address
     Lsap.lsap=  0;        // ( 1 byte) LSAP point = fixeed (0)

     memcpy( &PacchettoTx[28], &Lsap, 4 );
     //-----------------------------------------------
     // Compila Stringa Header PCCC
     //-----------------------------------------------
     PacchettoTx[32] = 0x0F;                                  // Function    0x0F
     PacchettoTx[33] = 0x00;                                  // STS ...
     PacchettoTx[34] = 0x00;                                  // TNS ( unique transactionidentifier)
     PacchettoTx[35] = 0x02;                                  // TNS ( unique transactionidentifier) sembra ok qualsiasi valore
     PacchettoTx[36] = 0xA2;                                  // Subfunction 0xA2
     PacchettoTx[37] = (byte)(SlcPlc[ Plc ].Multi_Data_Len*2);// Size of byte
     PacchettoTx[38] = (byte)(SlcPlc[ Plc ].Multi_NumFile);   // Numero del File (es 7)
     PacchettoTx[39] = 0x89;                                  // Tipo di file es 89 = File di tipo 'N'
     PacchettoTx[40] = (byte)(SlcPlc[ Plc ].Multi_Data_Ini);  // Element number    // Indice del campo iniziale
     PacchettoTx[41] = 0x00;                                  // Sub element number

     //-----------------------------------------------
     // Trasmetti Stringa
     //-----------------------------------------------
     LenTx=LenHeader + LenData; // 28 byte Header  +4 byte Lsap + x byte Pcc
     //err= Slc500_TxrasmettiRiceviPacchettoTCP( (WORD) Plc, (char *) PacchettoTx, (char *) PacchettoRx, BuffSize, LenTx, ByteDaRicevere,  ByteRicevuti, true, all_mess);
     //err = Slc500_TxPacchettoTCP( WORD Plc, char* PacchettoDaTrasmettere, int LenTrs, char* all_mess);
     err = Slc500_TxPacchettoTCP((WORD) Plc, (char *) PacchettoTx, LenTx, all_mess);
  }


  //---------------------------------------------------------------------------
  // (2) Attesa risposta da TUTTI i PLC per il massimo di TimeOut
  //---------------------------------------------------------------------------
  if( SempreON ){
     WaitReponse=0;
     GetLocalTime( &TimeInizio );
     for(;;){
        RicezioneCompleta =1;
        for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
           if( SlcPlc[Plc].EsclusionePlc ) continue;  // Se la comunicazione con AGV è esclusa      -->SKIP
           if( SlcPlc[Plc].ErrPlc>0      ) continue;  // Se già errore in SEND inutile aspettare risposta
           if( SlcPlc[Plc].MultiRxPlcOk  ) continue;  // Se già OK al precedente tentativo          -->SKIP

           //----------------------------------------------------------
           // Controllo se arrivato il pacchetto completo senza errori
           //----------------------------------------------------------
           if( SlcPlc[ Plc ].WSocket->State != wsConnected     ) {
              RicezioneCompleta=0;
              SlcPlc[Plc].ErrPlc=1;
              continue;
           }
           if( SlcPlc[ Plc ].WSocket->RcvdCount < 10 ) RicezioneCompleta=0;
        }

        //-------------------------------------
        // Risposta OK tutti i PLC --> break
        // Risposta      TIME_OUT  --> break
        //-------------------------------------
        if( RicezioneCompleta )          break;
        if( WaitReponse>=100 ){
           GetLocalTime( &TimeFine );
           msec = (int) (TimeFine.wSecond - TimeInizio.wSecond);
           if( msec<0 ) msec+=60;
           msec = msec * 1000;
           msec = msec + (int) (TimeFine.wMilliseconds - TimeInizio.wMilliseconds);
           // Verifica che il ritardo non superi TimeOut
           if( msec > AB_TimeOutSocketUDP ) break;
           if( msec < 0 ) break;
           WaitReponse=0;
        }
        WaitReponse++;
     }
  }


  //---------------------------------------------------------------------------
  // (3) Controllo Risposta dei vari PLC
  //---------------------------------------------------------------------------
  if( SempreON ){
     for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
        err = 0;
        if( SlcPlc[Plc].EsclusionePlc ) continue;  // Se la comunicazione con AGV è esclusa      -->SKIP
        if( SlcPlc[Plc].ErrPlc>0      ) continue;  // Se già errore in SEND inutile aspettare risposta
        if( SlcPlc[Plc].MultiRxPlcOk  ) continue;  // Se già OK al precedente tentativo          -->SKIP
        memset( PacchettoRx,        0, sizeof( PacchettoRx   ));
        //------------------------------
        // Recupero e controllo stringa
        //------------------------------
        if( err==0 ) err = Slc500_RxPacchettoTCP( (WORD) Plc, PacchettoRx, BuffSize,  ByteDaRicevere, ByteRicevuti, WaitReponse, all_mess2);
        NumWord =  SlcPlc[ Plc ].Multi_Data_Len;
        ByteDaRicevere=36+NumWord*2;
        if( err==0 && ByteRicevuti!=ByteDaRicevere         ) { err=1; sprintf( all_mess, "Plc%d Slc500_PcccMultiLeggiWord() Len Data NotOk ", Plc ); SlcPlc[Plc].ErrPlc=1; continue;}
        if( err==0                                            ) memcpy( &SlcPlc[ Plc ].HeaderCspv4Rx, &PacchettoRx[ 0 ], sizeof( cspv4_header ) );
        if( err==0 && SlcPlc[ Plc ].HeaderCspv4Rx.status!= 0  ) { SlcPlc[Plc].ErrPlc=1; sprintf( all_mess, "Plc%d Slc500_PcccMultiLeggiWord() Plc ha restituito errore", Plc ); continue;}
        if( err==0 ){
           memcpy( SlcPlc[ Plc ].Multi_W_TxRx, &PacchettoRx[ 36 ], NumWord*2 );
           SlcPlc[Plc].MultiRxPlcOk = 1;
        }
        if( ByteRicevuti==0 ) SlcPlc[ Plc ].HeaderCspv4Rx.conn_id=0;

        //----------------------------------
        // Memorizza errore relativo al PLC
        //----------------------------------
        if( err ){
           SlcPlc[Plc].ErrPlc=err;
           strcpy( all_mess, all_mess2 );
           //strcpy( &OmrEth_MessaggioAllarmePlc[ IndicePlc ][0], all_mess );
        }
     }
  }
  //OmrEthTcp_MultiPlcTrasmissioneInCorso=0;
  //--------------------------------------------------
  // Se presente anche un solo errore --> ERR
  //--------------------------------------------------
  err=0;
  for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
     if( SlcPlc[Plc].ErrPlc>0 ) return 1;
  }

  return err;
}







//-----------------------------------------
//     Slc500_PcccMultiScriviWord()
//-----------------------------------------
int Slc500_PcccMultiScriviWord(char* all_mess )
{
  SYSTEMTIME TimeInizio;
  SYSTEMTIME TimeFine;
  char  all_mess2[200];
  int   WaitReponse;
  int   msec; //, pos1;
  int   err=0;

  int   LenHeader;
  char  PacchettoTx[900];
  char  PacchettoRx[900];
  int   BuffSize = 500;
  int   ByteDaRicevere=28;
  int   ByteRicevuti;
  int   LenTx;
  int   Plc;
  bool  SempreON=true;
  int   RicezioneCompleta;
  unsigned short LenData;
  struct local_lsap Lsap;
  int NumWord;


  //--------------------------------------------------------------
  // Connetti tutti i Socket e tutti i canali di connessione PCCC
  //--------------------------------------------------------------
  Slc500_SocketMultiConnection( all_mess );
  Slc500_PcccMultiConnection  ( all_mess );
  //--------------------------
  // (0) Inizializza
  //--------------------------
  for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
     SlcPlc[Plc].ErrPlc=1;
     SlcPlc[Plc].MultiRxPlcOk=0;
  }
  //-----------------------------------------------------------------------------
  // (1) Trasmetti Pacchetto TCP a tutti i PLCs
  //-----------------------------------------------------------------------------
  // Calcolo della lunghezza del pacchetto
  sprintf( all_mess, "");
  for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
     if( SlcPlc[Plc].MultiRxPlcOk  ) continue;  // Se già OK al precedente tentativo          -->SKIP
     memset( PacchettoTx, 0, sizeof(PacchettoTx));
     memset( PacchettoRx, 0, sizeof(PacchettoRx));

     //-----------------------------------------------
     // Compila Stringa Header Cspv4
     //-----------------------------------------------
     NumWord =  SlcPlc[ Plc ].Multi_Data_Len;
     LenData =(unsigned short)( 14 + (NumWord*2));
     memset( &SlcPlc[ Plc ].HeaderCspv4Tx, 0, sizeof(cspv4_header));
     SlcPlc[ Plc ].HeaderCspv4Tx.mode        =1;            // ( 1 byte) 01 = request, 02 = response
     SlcPlc[ Plc ].HeaderCspv4Tx.submode     =7;            // ( 1 byte) 01 = connection, 07 = PCCC?? (not sure the name)
     Slc500_ConvertiWORDToStringa( &LenData, (unsigned char*) &SlcPlc[ Plc ].HeaderCspv4Tx.data_lenght, 1); // Inverti byte BIG_INDIAN
     SlcPlc[ Plc ].HeaderCspv4Tx.conn_id     =SlcPlc[ Plc ].HeaderCspv4Rx.conn_id;            // ( 4 byte) ID assigned by slave/server
     SlcPlc[ Plc ].HeaderCspv4Tx.status      =0;            // ( 1 byte) 01 = request, 02 = response
     SlcPlc[ Plc ].HeaderCspv4Tx.context[1]  =4;            // (16 byte) client info - slave/server just echos
     SlcPlc[ Plc ].HeaderCspv4Tx.context[3]  =5;            // (16 byte) client info - slave/server just echos

     LenHeader=sizeof( cspv4_header );
     memcpy( &PacchettoTx[ 0], &SlcPlc[ Plc ].HeaderCspv4Tx, LenHeader );

     //-----------------------------------------------
     // Compila Stringa Header LSAP
     //-----------------------------------------------
     Lsap.dst=   0;        // ( 1 byte) Destination byte - like slve address
     Lsap.res5=  5;        // ( 1 byte) Control byte = seems fixed as 5 (not sure what it does)
     Lsap.src=   0;        // ( 1 byte) Source byte - like master address
     Lsap.lsap=  0;        // ( 1 byte) LSAP point = fixeed (0)

     memcpy( &PacchettoTx[28], &Lsap, 4 );
     //-----------------------------------------------
     // Compila Stringa Header PCCC
     //-----------------------------------------------
     PacchettoTx[32] = 0x0F;                // Function    0x0F
     PacchettoTx[33] = 0x00;                // STS ...
     PacchettoTx[34] = 0x00;                // TNS ( unique transactionidentifier)
     PacchettoTx[35] = 0x02;                // TNS ( unique transactionidentifier)  Divrso da zero
     PacchettoTx[36] = 0xAA;                // Subfunction 0xAA
     PacchettoTx[37] = (byte)(SlcPlc[ Plc ].Multi_Data_Len*2);// Size of byte
     PacchettoTx[38] = (byte)(SlcPlc[ Plc ].Multi_NumFile);   // Numero del File (es 7)
     PacchettoTx[39] = 0x89;                                  // Tipo di file es 89 = File di tipo 'N'
     PacchettoTx[40] = (byte)(SlcPlc[ Plc ].Multi_Data_Ini);  // Element number    // Indice del campo iniziale
     PacchettoTx[41] = 0x00;                                  // Sub element number

     //-----------------------------------------------
     // Compila Dati da scrivere
     //-----------------------------------------------
     memcpy( &PacchettoTx[ 42 ], SlcPlc[ Plc ].Multi_W_TxRx, NumWord*2 );

     //-----------------------------------------------
     // Trasmetti Stringa
     //-----------------------------------------------
     LenTx=LenHeader + LenData; // 28 byte Header  +4 byte Lsap + x byte Pcc
     //err= Slc500_TxrasmettiRiceviPacchettoTCP( (WORD) Plc, (char *) PacchettoTx, (char *) PacchettoRx, BuffSize, LenTx, ByteDaRicevere,  ByteRicevuti, true, all_mess);
     //err = Slc500_TxPacchettoTCP( WORD Plc, char* PacchettoDaTrasmettere, int LenTrs, char* all_mess);
     err = Slc500_TxPacchettoTCP((WORD) Plc, (char *) PacchettoTx, LenTx, all_mess);
  }


  //---------------------------------------------------------------------------
  // (2) Attesa risposta da TUTTI i PLC per il massimo di TimeOut
  //---------------------------------------------------------------------------
  if( SempreON ){
     WaitReponse=0;
     GetLocalTime( &TimeInizio );
     for(;;){
        RicezioneCompleta =1;
        for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
           if( SlcPlc[Plc].EsclusionePlc ) continue;  // Se la comunicazione con AGV è esclusa      -->SKIP
           if( SlcPlc[Plc].ErrPlc>0      ) continue;  // Se già errore in SEND inutile aspettare risposta
           if( SlcPlc[Plc].MultiRxPlcOk  ) continue;  // Se già OK al precedente tentativo          -->SKIP

           //----------------------------------------------------------
           // Controllo se arrivato il pacchetto completo senza errori
           //----------------------------------------------------------
           if( SlcPlc[ Plc ].WSocket->State != wsConnected     ) {RicezioneCompleta=0; continue;}
           if( SlcPlc[ Plc ].WSocket->RcvdCount < 10 ) RicezioneCompleta=0;
        }

        //-------------------------------------
        // Risposta OK tutti i PLC --> break
        // Risposta      TIME_OUT  --> break
        //-------------------------------------
        if( RicezioneCompleta )          break;
        if( WaitReponse>=100 ){
           GetLocalTime( &TimeFine );
           msec = (int) (TimeFine.wSecond - TimeInizio.wSecond);
           if( msec<0 ) msec+=60;
           msec = msec * 1000;
           msec = msec + (int) (TimeFine.wMilliseconds - TimeInizio.wMilliseconds);
           // Verifica che il ritardo non superi TimeOut
           if( msec > AB_TimeOutSocketUDP ) break;
           if( msec < 0 ) break;
           WaitReponse=0;
        }
        WaitReponse++;
     }
  }


  //---------------------------------------------------------------------------
  // (3) Controllo Risposta dei vari PLC
  //---------------------------------------------------------------------------
  if( SempreON ){
     for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
        err = 0;
        if( SlcPlc[Plc].EsclusionePlc ) continue;  // Se la comunicazione con AGV è esclusa      -->SKIP
        if( SlcPlc[Plc].ErrPlc>0      ) continue;  // Se già errore in SEND inutile aspettare risposta
        if( SlcPlc[Plc].MultiRxPlcOk  ) continue;  // Se già OK al precedente tentativo          -->SKIP
        memset( PacchettoRx,        0, sizeof( PacchettoRx   ));
        //------------------------------
        // Recupero e controllo stringa
        //------------------------------
        if( err==0 ) err = Slc500_RxPacchettoTCP( (WORD) Plc, PacchettoRx, BuffSize,  ByteDaRicevere, ByteRicevuti, WaitReponse, all_mess2);
        ByteDaRicevere=36;
        if( err==0 && ByteRicevuti!=ByteDaRicevere            ) { err=1; sprintf( all_mess, "Plc%d Slc500_PcccMultiScriviWord() Len Data NotOk ", Plc ); SlcPlc[Plc].ErrPlc=1; continue;}
        if( err==0                                            ) memcpy( &SlcPlc[ Plc ].HeaderCspv4Rx, &PacchettoRx[ 0 ], sizeof( cspv4_header ) );
        if( err==0 && SlcPlc[ Plc ].HeaderCspv4Rx.status!= 0  ) { SlcPlc[Plc].ErrPlc=1; sprintf( all_mess, "Plc%d Slc500_PcccMultiScriviWord() Plc ha restituito errore", Plc ); continue;}
        if( err==0                                            ) SlcPlc[Plc].MultiRxPlcOk = 1;
        if( ByteRicevuti==0 ) SlcPlc[ Plc ].HeaderCspv4Rx.conn_id=0;

        //----------------------------------
        // Memorizza errore relativo al PLC
        //----------------------------------
        if( err ){
           SlcPlc[Plc].ErrPlc=err;
           strcpy( all_mess, all_mess2 );
           //strcpy( &OmrEth_MessaggioAllarmePlc[ IndicePlc ][0], all_mess );
        }
     }
  }



  //OmrEthTcp_MultiPlcTrasmissioneInCorso=0;
  //--------------------------------------------------
  // Se presente anche un solo errore --> ERR
  //--------------------------------------------------
  err=0;
  for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
     if( SlcPlc[Plc].ErrPlc>0 ) return 1;
  }

  return err;
}





