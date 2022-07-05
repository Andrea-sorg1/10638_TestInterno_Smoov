// -----------------------------------------------------------------------
//         file        AlgoNoDave.CPP
// -----------------------------------------------------------------------
#include <stdio.h>


#include "AlgoNoDave.h"


//------------------------------
//     Variabili
//------------------------------
_openFunc SCP_open;
_closeFunc SCP_close;
_sendFunc SCP_send;
_receiveFunc SCP_receive;
//_SetHWndMsgFunc SetSinecHWndMsg;
//_SetHWndFunc SetSinecHWnd;
_get_errnoFunc SCP_get_errno;

AlgoNoDave_Interfaccia AlgoDevice[ ALGO_NODAVE_MAX_PLC+1 ];

//------------------------------
//     Funzioni
//------------------------------


/*
Forse NON Necessaria
//-------------------------------------
//  AlgoNoDave_SetTimeoutSer()
//-------------------------------------
WORD AlgoNoDave_SetTimeoutSer( HANDLE handleComm, DWORD r_ti,DWORD r_tm,DWORD r_tc,DWORD w_tm,DWORD w_tc)
{
  COMMTIMEOUTS timeout;
  WORD  fSuccess;  if( (int) (handleComm) <=0  ) return 0;  //---------------------------------------  // Recupero valori attuali e forzo nuovi  //---------------------------------------  fSuccess = (WORD) GetCommTimeouts( handleComm, &timeout);  if( !fSuccess ) return( 1 );
  timeout.ReadIntervalTimeout         =r_ti;  timeout.ReadTotalTimeoutMultiplier  =r_tm;  timeout.ReadTotalTimeoutConstant    =r_tc;  timeout.WriteTotalTimeoutMultiplier =w_tm;  timeout.WriteTotalTimeoutConstant   =w_tc;  fSuccess =(WORD) SetCommTimeouts(handleComm, &timeout);  if (!fSuccess) return 0;  return 1;
}
*/


//-------------------------------------
//  AlgoNoDave_OpenCom()
//-------------------------------------
HANDLE AlgoNoDave_OpenCom( char* ComName, char* Baud, int NumBit, char Parity, int StopBit )
{
	DCB dcb;
	HANDLE handleComm;
//  WORD   TimeOut=200;

  //-----------------------------------
  // Inizializzazione della porta
  //-----------------------------------
  handleComm = CreateFile( ComName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, 0);

  GetCommState(handleComm,&dcb);

  dcb.ByteSize = (BYTE) NumBit; //8;
  dcb.fOutxCtsFlow=FALSE;
  dcb.fOutxDsrFlow=FALSE;
  dcb.fDtrControl=DTR_CONTROL_ENABLE;

  dcb.fDsrSensitivity=FALSE;
  dcb.fInX=FALSE;
  dcb.fOutX=FALSE;
  dcb.fNull=FALSE;
  dcb.fAbortOnError=FALSE;
  dcb.fBinary=TRUE;
  dcb.fParity=TRUE;
  dcb.fOutxCtsFlow=FALSE;
	dcb.fOutxDsrFlow=FALSE;
	dcb.fRtsControl=TRUE;
	dcb.fTXContinueOnXoff=TRUE;
	dcb.StopBits=(BYTE) StopBit; // 2;  ///that was 2 !!!
  //---------------------------
  // Baudrate
  //---------------------------
	if      (0==strncmp(Baud,"115200", 6)) dcb.BaudRate = CBR_115200;
  else if (0==strncmp(Baud,"57600",  5))	dcb.BaudRate = CBR_57600;
  else if (0==strncmp(Baud,"38400",  5))	dcb.BaudRate = CBR_38400;
  else if (0==strncmp(Baud,"19200",  5))	dcb.BaudRate = CBR_19200;
  else if (0==strncmp(Baud,"9600",   4))	dcb.BaudRate = CBR_9600;
  else if (0==strncmp(Baud,"4800",   4))	dcb.BaudRate = CBR_4800;
  else if (0==strncmp(Baud,"2400",   4))	dcb.BaudRate = CBR_2400;
  else if (0==strncmp(Baud,"1200",   4))	dcb.BaudRate = CBR_1200;
  else if (0==strncmp(Baud,"600",    3))	dcb.BaudRate = CBR_600;
  else if (0==strncmp(Baud,"300",    3))	dcb.BaudRate = CBR_300;
  //---------------------------
  // Parity
  //---------------------------
	Parity = (char) tolower(Parity);
  if      (Parity == 'e') dcb.Parity = 2;
	else if (Parity == 'o') dcb.Parity = 1;
	else if (Parity == 'n') dcb.Parity = 0;
  //---------------------------
  // Set COM
  //---------------------------
	SetCommState(handleComm,&dcb);

  //---------------------------
  // Set COM
  //---------------------------
//AlgoNoDave_SetTimeoutSer( handleComm, 10, 10, TimeOut, 10, TimeOut);


  return (HANDLE) handleComm;
}





//-------------------------------------
//  AlgoNoDave_OpenSocket()
//-------------------------------------
HANDLE AlgoNoDave_OpenSocket(const int port, const char * peer, char* all_mess) {
  SOCKET fd;
  WSADATA wsadata;
  WSAStartup(MAKEWORD(2,0), &wsadata);
  struct sockaddr_in addr;
  int addrlen;
  int  err=0;
  int opt;
  unsigned long int dontblock;
  struct timeval Time_Out;
  //FD_SET readfds;
  FD_SET writefds;
  //FD_SET Error;

  //---------------------------------------------------
  // (A) Apertura SOCKETGestione Connessione del SOCKET con Time-Out
  //---------------------------------------------------
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons((  u_short) port);
  addr.sin_addr.s_addr = inet_addr(peer);
  addrlen = sizeof(addr);

  fd = socket(AF_INET, SOCK_STREAM, 6);
  if( fd<1 ){
     fd=-1;
     err =  WSAGetLastError (); // Restituisce il codice di errore
     sprintf( all_mess, "Open Socket error: %d", err );
  }
  //---------------------------------------------------
  // (B) Gestione Connessione del SOCKET con Time-Out
  //---------------------------------------------------
  if( fd>0 ){
     //----------------------------------------------------------------------
     // Setta SOCKET NOBLOCKING e -> CONNETTI
     // Altrimenti aspetta la concluzsione della connessione con
     // un  Tim-Out che di (default=30sec !!!)
     //----------------------------------------------------------------------
     dontblock = 1;
     ioctlsocket(fd, FIONBIO, &dontblock);

     connect(fd, (struct sockaddr *) & addr, addrlen);
     //------------------------------------------------------------
     // Verifica Connessione tramite SELECT e Time-Out di 0.010 sec
     // Check della del campo writefds (pronto in scrittura)
     //------------------------------------------------------------
     Time_Out.tv_sec  = 0;
//   Time_Out.tv_usec = 10000; //100000;
     Time_Out.tv_usec = 100000; // 15/11/2007 Aumentato a 0.1 sec per connettersi a VIPA

     FD_ZERO( &writefds );
     FD_SET( fd, &writefds );
     writefds.fd_count = writefds.fd_count;

     err = select( NULL, NULL, &writefds, NULL, &Time_Out );
     if( !FD_ISSET( fd, &writefds )){
	      closesocket(fd);
        fd = -1;               // Socket NON Pronto to write ( NON CONNESSO)
        err =  WSAGetLastError (); // Restituisce il codice di errore
        sprintf( all_mess, "Connect Socket error: %d", err );
     }
     //----------------------------------------------------------------------
     // Setta SOCKET BLOCKING per le sucessive operazioni di READ WRITE
     //----------------------------------------------------------------------
     if( fd>0 ){
        dontblock = 0;
        ioctlsocket(fd, FIONBIO, &dontblock);
     }
  }
  //-----------------------------------------------------------------------------
  // (C) Impostiamo i Time-Out per le operazioni di READ e WRITE (default=30sec):
  // Da documentazione API le impostazioni del parametro SO_RCVTIMEO
  // dovrebbe agire anche sulla CONNECT in realtà NO
  //----------------------------------------------------------------------------
  if( fd>0 ){
	   opt=10; // 10 msec
	   setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *) &opt, sizeof(opt));
	   setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *) &opt, sizeof(opt));
	   //opt=0;
	   //res=getsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *) &opt, &optlen);
	   //res=getsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *) &opt, &optlen);
	   //res=getsockopt(fd, SOL_SOCKET, TCP_NODELAY, (char *) &opt, &optlen);
  }

  return (HANDLE) fd;
}



//-------------------------------------
//  AlgoNoDave_ConessionePlc()
//-------------------------------------
int AlgoNoDave_ConessionePlc( int NumDispositivo, int TipoInterfaccia, int Slot, int Rec, char* all_mess)
{
  int  res, i, j, a, initSuccess;
  char buf1 [davePartnerListSize];


  int localMPI=0;
  int plcMPI =2;
  int useSlot;
  int useRec;
  int useProtocol;

  //-------------------------------------------------
  // Inizializzazione variabili globali di Default
  //-------------------------------------------------
  AlgoDevice[ NumDispositivo ].TipoCom     = TipoInterfaccia;
  AlgoDevice[ NumDispositivo ].localMPI    = 0;
  AlgoDevice[ NumDispositivo ].plcMPI      = 2;
  AlgoDevice[ NumDispositivo ].useSlot     = 0;
  AlgoDevice[ NumDispositivo ].useRec      = 0;
//AlgoDevice[ NumDispositivo ].useProtocol = daveProtoMPI2;


  //------------------------------
  // modalità - S511
  //------------------------------
  if(TipoInterfaccia==0){
     AlgoDevice[ NumDispositivo ].hComm = AlgoNoDave_OpenCom( AlgoDevice[ NumDispositivo ].ComName,
                                                              AlgoDevice[ NumDispositivo ].Baud, 8, 'E', 2 );
     //---------------------------
     // Parametrizzazione
     //---------------------------
     AlgoDevice[ NumDispositivo ].fds.rfd     = AlgoDevice[ NumDispositivo ].hComm;
     AlgoDevice[ NumDispositivo ].fds.wfd     = AlgoDevice[ NumDispositivo ].fds.rfd;
     AlgoDevice[ NumDispositivo ].useSlot     = 0;
     AlgoDevice[ NumDispositivo ].useProtocol = daveProtoAS511;
  }
  //------------------------------
  // modalità - PPI
  //------------------------------
  if(TipoInterfaccia==1){
     AlgoDevice[ NumDispositivo ].hComm = AlgoNoDave_OpenCom( AlgoDevice[ NumDispositivo ].ComName,
                                                              AlgoDevice[ NumDispositivo ].Baud, 8, 'E', 2 );
     //---------------------------
     // Parametrizzazione
     //---------------------------
     AlgoDevice[ NumDispositivo ].fds.rfd     = AlgoDevice[ NumDispositivo ].hComm;
     AlgoDevice[ NumDispositivo ].fds.wfd     = AlgoDevice[ NumDispositivo ].fds.rfd;
     AlgoDevice[ NumDispositivo ].useSlot     = 0;
     AlgoDevice[ NumDispositivo ].useProtocol = daveProtoPPI;
  }
  if(TipoInterfaccia==2){
     //hComm = AlgoNoDave_OpenCom( char* ComName, char* Baud, int NumBit, char Parity, int StopBit )
     //AlgoDevice[ NumDispositivo ].hComm = AlgoNoDave_OpenCom( "COM1", "38400", 8, 'O', 2 );
     AlgoDevice[ NumDispositivo ].hComm = AlgoNoDave_OpenCom( AlgoDevice[ NumDispositivo ].ComName,
                                                              AlgoDevice[ NumDispositivo ].Baud, 8, 'O', 2 );
     //---------------------------
     // Parametrizzazione
     //---------------------------
     AlgoDevice[ NumDispositivo ].fds.rfd     = AlgoDevice[ NumDispositivo ].hComm;
     AlgoDevice[ NumDispositivo ].fds.wfd     = AlgoDevice[ NumDispositivo ].fds.rfd;
     AlgoDevice[ NumDispositivo ].useSlot     = 0;
     AlgoDevice[ NumDispositivo ].useProtocol = daveProtoMPI2;
  }
  //------------------------------
  // Socket
  //------------------------------
  if(TipoInterfaccia==3){
     //AlgoDevice[ NumDispositivo ].fds.rfd     = AlgoNoDave_OpenSocket(102, "192.168.1.141", all_mess);
     AlgoDevice[ NumDispositivo ].fds.rfd     = AlgoNoDave_OpenSocket(AlgoDevice[ NumDispositivo ].TcpPort, AlgoDevice[ NumDispositivo ].TcpAddress, all_mess);
     AlgoDevice[ NumDispositivo ].fds.wfd     = AlgoDevice[ NumDispositivo ].fds.rfd;
     AlgoDevice[ NumDispositivo ].useSlot     = Slot;    // 2 o 3 a seconda della Scheta Ethernet di comunicazione
     AlgoDevice[ NumDispositivo ].useRec      = Rec;     // 0 di default
     AlgoDevice[ NumDispositivo ].useProtocol = daveProtoISOTCP;
  }
  //------------------------------
  // Interfaccia
  //------------------------------
  if (AlgoDevice[ NumDispositivo ].fds.rfd>0) {

     localMPI      = AlgoDevice[ NumDispositivo ].localMPI;
     plcMPI        = AlgoDevice[ NumDispositivo ].plcMPI;
     useSlot       = AlgoDevice[ NumDispositivo ].useSlot;
     useRec        = AlgoDevice[ NumDispositivo ].useRec;
     useProtocol   = AlgoDevice[ NumDispositivo ].useProtocol;

     //------------------------
     // Configura interfaccia
     //------------------------
	   AlgoDevice[ NumDispositivo ].di =daveNewInterface( AlgoDevice[ NumDispositivo ].fds, "IF1", localMPI, useProtocol, daveSpeed187k);
     if(TipoInterfaccia!=2) daveSetTimeout( AlgoDevice[ NumDispositivo ].di, 100000);
	   else                   daveSetTimeout( AlgoDevice[ NumDispositivo ].di, 1000000); // solo per MPI

     //-------------------------------------
     // Tento per max 3 volte di iniz.
     // l'adattatore
     //-------------------------------------
     initSuccess =0;
	   for(i=0; i<3; i++) {
        if(0==daveInitAdapter(AlgoDevice[ NumDispositivo ].di)){
		      initSuccess=1;
		      a = daveListReachablePartners(AlgoDevice[ NumDispositivo ].di, buf1);
		      sprintf(all_mess, "daveListReachablePartners List length: %d\n", a);
           //ShowMessage(all_mess);
		      if(a>0) {
		         for (j=0;j<a;j++) {
			         if(buf1[j]==daveMPIReachable){
                    sprintf(all_mess, "Device at address:%d\n", j);
                   //ShowMessage(all_mess);
                 }
		         }
		      }
		      break;
	      }else daveDisconnectAdapter(AlgoDevice[ NumDispositivo ].di);
     }
	   if (!initSuccess) {
	      sprintf(all_mess, "Couldn't connect to Adapter!.\n Please try again. You may also try the option -2 for some adapters.\n");
        return 1;
	   }
     //------------------------
     // Connessione
     //------------------------
     AlgoDevice[ NumDispositivo ].dc = daveNewConnection( AlgoDevice[ NumDispositivo ].di, plcMPI, useRec, useSlot);
     //------------------------
     // Connessione al PLC
     //------------------------
     res = daveConnectPLC(AlgoDevice[ NumDispositivo ].dc);
     if(res!=0){
        sprintf(all_mess, "Error %d=%s", res, daveStrerror(res));
        return 1;
     }


     //---------------------------------------------------------
     // Sucessivamente alla connessione impostiamo un Time-Out
     // più basso
     //---------------------------------------------------------
     if(res==0){
  	   daveSetTimeout( AlgoDevice[ NumDispositivo ].di, 100000);
     }
  }

  if ((int) AlgoDevice[ NumDispositivo ].fds.rfd<=0)  return 1;

  return 0;
}
//---------------------------------------------------------------------------

//-------------------------------------
//  AlgoNoDave_DisconessionePlc()
//-------------------------------------
int AlgoNoDave_DisconessionePlc( int NumDispositivo)
{
	HANDLE hComm;
  daveInterface * di;
  daveConnection * dc;
  int NumSocket;


  if( (int) AlgoDevice[ NumDispositivo ].fds.rfd<=0 ) return 0;
  NumSocket   = (int) AlgoDevice[ NumDispositivo ].fds.rfd;
  hComm       = AlgoDevice[ NumDispositivo ].hComm;
  dc          = AlgoDevice[ NumDispositivo ].dc;
  di          = AlgoDevice[ NumDispositivo ].di;

  //------------------------
  // Disconnessione
  //------------------------
  daveDisconnectPLC(dc);
  daveFree(dc);
  daveDisconnectAdapter(di);
  daveFree(di);

  //--------------------------------
  // Chiusura COM e chiusura SOCKET
  //--------------------------------
  if((AlgoDevice[ NumDispositivo ].TipoCom==0 ||
      AlgoDevice[ NumDispositivo ].TipoCom==1 ||
      AlgoDevice[ NumDispositivo ].TipoCom==2) && ( (int) hComm)){
     //---------------------------------------------------------
     // l'handle della COM viene chiusa solo se il tipo di
     // comunicazione è seriale
     //---------------------------------------------------------
     CloseHandle(hComm);
     hComm = NULL;
  }
  closesocket( (int) (NumSocket) );

  AlgoDevice[ NumDispositivo ].fds.rfd = (HANDLE) -1;

  return 0;
}
//---------------------------------------------------------------------------


//-------------------------------------
//  AlgoNoDave_StartPlc()
//-------------------------------------
int AlgoNoDave_StartPlc( int NumDispositivo )
{
  return daveStart( AlgoDevice[ NumDispositivo ].dc );
}
//---------------------------------------------------------------------------


//-------------------------------------
//  AlgoNoDave_StopPlc()
//-------------------------------------
int AlgoNoDave_StopPlc( int NumDispositivo )
{
  return daveStop( AlgoDevice[ NumDispositivo ].dc );
}
//---------------------------------------------------------------------------


//-------------------------------------
//
//  AlgoNoDave_ReadBytes()
//
//  NumDispositivo 1..100 Dispositivo PLC
//
//  Area = 1 daveDB;
//         2 daveFlags;
//         3 daveInputs;
//         4 daveOutputs;
//         5 daveTimer;
//         6 daveCounter;
//         7 daveP;
//
//  Start     Numero Byte di Start
//
//  Len       Numero di Byte da leggere/scrivere
//
//  ArrayDati Puntatore a un array di BYTE conetenente i dati
//
//-------------------------------------
int AlgoNoDave_ReadBytes( int NumDispositivo, int Area, int DB, int Start, int Len, void* ArrayDati, char* all_mess)
{
  int err;
  int NoDaveArea;
  int i;
  uc *PtrByte=(BYTE *) ArrayDati;
  
  //------------------------------------------
  // Controllo Valori campo
  //------------------------------------------
  if( Area<1 || Area >7 ) { sprintf( all_mess, "Area Range 1..7" ); return 1; }
  if( (int) (AlgoDevice[ NumDispositivo ].fds.rfd) <= 0 ){ sprintf( all_mess, "Device %d NOT Connect", NumDispositivo ); return 1; }

  if(Area==1) NoDaveArea = daveDB;
  if(Area==2) NoDaveArea = daveFlags;
  if(Area==3) NoDaveArea = daveInputs;
  if(Area==4) NoDaveArea = daveOutputs;
  if(Area==5) NoDaveArea = daveTimer;
  if(Area==6) NoDaveArea = daveCounter;
  if(Area==7) NoDaveArea = daveP;

  //-----------------------------------------------------------------------
  // Modifica del 16/01/2007 Non funzionava in lettura ...
  //-----------------------------------------------------------------------
  //err = daveReadBytes( AlgoDevice[ NumDispositivo ].dc, NoDaveArea, DB, Start, Len, ArrayDati);
  err = daveReadBytes( AlgoDevice[ NumDispositivo ].dc, NoDaveArea, DB, Start, Len, NULL);
  if( err==0 ){
     for( i=0; i<Len; i++){
        PtrByte[ i ] = (uc) daveGetU8(AlgoDevice[ NumDispositivo ].dc);
     }
  }
  if( err )  sprintf(all_mess, "Error %d=%s", err, daveStrerror(err));

  return err;
}
//---------------------------------------------------------------------------


//-------------------------------------
//
//  AlgoNoDave_WritedBytes()
//
//  NumDispositivo 1..100 Dispositivo PLC
//
//  Area = 1 daveDB;
//         2 daveFlags;
//         3 daveInputs;
//         4 daveOutputs;
//         5 daveTimer;
//         6 daveCounter;
//         7 daveP;
//
//  Start     Numero Byte di Start
//
//  Len       Numero di Byte da leggere/scrivere
//
//  ArrayDati Puntatore a un array di BYTE conetenente i dati
//
//-------------------------------------
int AlgoNoDave_WritedBytes( int NumDispositivo, int Area, int DB, int Start, int Len, void* ArrayDati, char* all_mess)
{
  int err;
  int NoDaveArea;

  //------------------------------------------
  // Controllo Valori campo
  //------------------------------------------
  if( Area<1 || Area >7 ) { sprintf( all_mess, "Area Range 1..7" ); return 1; }
  if( (int) (AlgoDevice[ NumDispositivo ].fds.rfd) <= 0 ){ sprintf( all_mess, "Device %d NOT Connect", NumDispositivo ); return 1; }

  if(Area==1) NoDaveArea = daveDB;
  if(Area==2) NoDaveArea = daveFlags;
  if(Area==3) NoDaveArea = daveInputs;
  if(Area==4) NoDaveArea = daveOutputs;
  if(Area==5) NoDaveArea = daveTimer;
  if(Area==6) NoDaveArea = daveCounter;
  if(Area==7) NoDaveArea = daveP;

  err = daveWriteBytes( AlgoDevice[ NumDispositivo ].dc, NoDaveArea, DB, Start, Len, ArrayDati);
  if( err )  sprintf(all_mess, "Error %d=%s", err, daveStrerror(err));

  return err;
}
//---------------------------------------------------------------------------



//--------------------------------- o ------------------------------------

//---------------------------------------------------------------------------
//  Funzioni ausiliarie per conversione in WORD (2 byte) e INT (4 byte)
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// AlgoNoDave_ConvertiWORDToStringa
// Converi un array di WORD ( num_canali WORD ) in array di CHAR
// invertendo ordine dei byte per essere coerente con Siemens
//---------------------------------------------------------------------------
int AlgoNoDave_ConvertiWORDToStringa( WORD* ArrayWORD, unsigned char* ArrayCHAR, int num_canali)
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
// AlgoNoDave_ConvertiStringaToWORD()
// Converi un array di CHAR in array di WORD  ( num_canali WORD )
// invertendo ordine dei byte per essere coerente con Siemens
//---------------------------------------------------------------------------
int AlgoNoDave_ConvertiStringaToWORD( WORD* ArrayWORD, unsigned char* ArrayCHAR, int num_canali)
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


//---------------------------------------------------------------------------
// AlgoNoDave_ConvertiIntToStringa
// Converi un array di INT ( num_canali INT ) in array di CHAR
// invertendo ordine dei byte per essere coerente con Siemens
//---------------------------------------------------------------------------
int AlgoNoDave_ConvertiIntToStringa( int* ArrayInt, unsigned char* ArrayCHAR, int num_canali)
{
  int i, j;

  j=0;
  //-------------------------------------
  // Conversione INT in stringa BYTE
  //-------------------------------------
  for(i=0; i<num_canali; i++){
     ArrayCHAR[ j ] = (unsigned char) ((ArrayInt[i]>>24) & 0x000000ff);  j++;
     ArrayCHAR[ j ] = (unsigned char) ((ArrayInt[i]>>16) & 0x000000ff);  j++;
     ArrayCHAR[ j ] = (unsigned char) ((ArrayInt[i]>> 8) & 0x000000ff);  j++;
     ArrayCHAR[ j ] = (unsigned char) ((ArrayInt[i]    ) & 0x000000ff);  j++;
  }

  return 0;
}




//---------------------------------------------------------------------------
// AlgoNoDave_ConvertiStringaToInt()
// Converi un array di INT  ( num_canali INT ) in array di CHAR
// invertendo ordine dei byte per essere coerente con Siemens
//---------------------------------------------------------------------------
int AlgoNoDave_ConvertiStringaToInt( int* ArrayInt, unsigned char* ArrayCHAR, int num_canali)
{
  int i, j;

  j=0;
  //-------------------------------------
  // Conversione stringa BYTE in INT
  //-------------------------------------
  for(i=0; i<num_canali; i++){
     ArrayInt[ i ] = (int) (  ArrayCHAR[ j ]<<24                     ); j++;
     ArrayInt[ i ] = (int) ( (ArrayCHAR[ j ]<<16) | ( ArrayInt[ i ] )); j++;
     ArrayInt[ i ] = (int) ( (ArrayCHAR[ j ]<<8 ) | ( ArrayInt[ i ] )); j++;
     ArrayInt[ i ] = (int) ( (ArrayCHAR[ j ]    ) | ( ArrayInt[ i ] )); j++;
  }

  return 0;
}



