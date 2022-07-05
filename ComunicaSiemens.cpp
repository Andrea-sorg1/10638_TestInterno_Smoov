//----------------------------------------------------------------------------
//                     ComunicaSiemens.CPP
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------
   by  GianFranco   12/01/2007
   Test TCT PROTOCOL

   Send and Recive procedure  (Client Server)

---------------------------------------------------------------------------*/
#include <vcl.h>
#include <vcl\inifiles.hpp>
#include <stdio.h>
#include <stdlib.h>
#pragma hdrstop

//#include "Sistema.h"
#include "TecnoFer.h"
#include "Variabil.h"


#include "ComunicaSiemens.h"
#include "libreria.h"
#include "AlgoNoDave.h"
#include "OmronETH.h"   // Contiene le strutture per le impostazioni di comunicazione

//---------------------------------------------------------------------------
//#pragma hdrstop
#pragma package(smart_init)
#pragma link "WSocket"




//----------------------------------------------------------------------------
// Variabili Globali
//----------------------------------------------------------------------------


//--------------------------------- o ----------------------------------------


//----------------------------------------------------- O  ------------------------------

//-----------------------------------------------------------------------------
//  Funzioni per PLC
//-----------------------------------------------------------------------------



// --------------------------------
//      init_porta_siemens()
// --------------------------------
// inizializzazione porta comunicazione LibNonDave
//
int init_porta_siemens(int dispositivo, int slot, char *all_mess)
/*********************************************/
{
  int   TipoCom=OmrPlc[ dispositivo ].TypCom;                    // Tipo di comunicazione con PLC ( 0=S5, 1=PPI, 2=MPI, 3=ETHERNET )
  int   NumCom= OmrPlc[ dispositivo ].Com;                       // Numero Com: 1=Com1 2=Com2 ecc...
  int   Slot  = OmrPlc[ dispositivo ].IndexPlc;                  // 2 o 3 a seconda se S7300 o S7400
                                                                 // Per dare al tecnico la possibilità di variarlo
  int   Rec   = 0;                                               // Fisso
  char* Baudreate="38400";                                       // Fisso
  char* IpAddress= OmrPlc[ dispositivo ].RemoteHost.c_str();     // "192.168.1.91";
  int   TcpPort  = OmrPlc[ dispositivo ].RemotePort;             // Porta TCP remota (default = 102)

  int   err=0;
  int   Com;
  int   MsecRisp;

  if( slot>0 ) Slot = slot;                                      // Se Passato come campo (visto che è previsto) ha la priorità
  if( Slot<0 ) Slot = 2;                                         // 2 o 3 a seconda se S7300 o S7400
  if( Slot>10) Slot = 2;                                         // 2 o 3 a seconda se S7300 o S7400

  //---------------------------------------------
  // Gestione porte di comunicazione
  //---------------------------------------------
  Com = COM_SIEMENS;
  err = SIM_ConnectToPlc( dispositivo, TipoCom, NumCom, Baudreate, IpAddress, TcpPort, Slot, Rec, all_mess);
  if( err ) AlgoNoDave_DisconessionePlc( dispositivo);


  //---------------------------------------------
  // Sul tentativo di connessione azzero gli
  // errori di comunicazione
  //---------------------------------------------
  errore_comunicazione[Com][dispositivo]=0;
  if(!err) SIM_PlcConnesso[dispositivo]=true;

  return err;
}

// --------------------------------
//      riconnetti_porta_siemens()
// --------------------------------
// inizializzazione porta comunicazione LibNonDave
//
int riconnetti_porta_siemens(int dispositivo, int slot, char *all_mess)
/*********************************************/
{
  int   TipoCom=OmrPlc[ dispositivo ].TypCom;                    // Tipo di comunicazione con PLC ( 0=S5, 1=PPI, 2=MPI, 3=ETHERNET )
  int   NumCom= OmrPlc[ dispositivo ].Com;                       // Numero Com: 1=Com1 2=Com2 ecc...
  int   Slot  = OmrPlc[ dispositivo ].IndexPlc;                  // 2 o 3 a seconda se S7300 o S7400
                                                                 // Per dare al tecnico la possibilità di variarlo
  int   Rec   = 0;                                               // Fisso
  char* Baudreate="38400";                                       // Fisso
  char* IpAddress= OmrPlc[ dispositivo ].RemoteHost.c_str();     // "192.168.1.91";
  int   TcpPort  = OmrPlc[ dispositivo ].RemotePort;             // Porta TCP remota (default = 102)

  int   err=0;
  int   Com;

  if( slot>0  ) Slot = slot;                                     // Se Passato come campo (visto che è previsto) ha la priorità
  if( Slot<0  ) Slot = 2;                                        // 2 o 3 a seconda se S7300 o S7400
  if( Slot>10 ) Slot = 2;                                        // 2 o 3 a seconda se S7300 o S7400
  //---------------------------------------------
  // Gestione porte di comunicazione
  //---------------------------------------------
  Com = COM_SIEMENS;

  err = SIM_RiconnectToPlc( dispositivo, TipoCom, NumCom, Baudreate, IpAddress, TcpPort, Slot, Rec, all_mess);
  //---------------------------------------------
  // Sul tentativo di connessione azzero gli
  // errori di comunicazione
  //---------------------------------------------
  errore_comunicazione[Com][dispositivo]=0;
  if(!err) SIM_PlcConnesso[dispositivo]=true;


  return err;
}

//---------------------------------
// SIM_ConnectToPlc()
//---------------------------------
int SIM_ConnectToPlc( int NumDispositivo, int TipoCom, int NumCom, char* Baudreate, char* IpAddress, int TcpPort, int Slot, int Rec, char* all_mess)
{
  int  err, Com;
  char Buffer[300];

  // int NumDispositivo =1;    // Numero del dispositivo (nel caso ce ne siano più di uno)
  // int TipoCom;              // Tipo di comunicazione con PLC ( 0=S5, 1=PPI, 2=MPI, 3=ETHERNET )
  // int NumCom;               // Numero Com: 1=Com1 2= Com2 ecc...
  //strcpy(  AlgoDevice[ NumDispositivo ].TcpAddress,  "192.168.1.141" );
  //strcpy(  AlgoDevice[ NumDispositivo ].ComName, "COM1" );

  err=0;
  SIM_PlcConnesso[NumDispositivo]=false;
  //---------------------------------------------
  // Gestione porte di comunicazione
  //---------------------------------------------
  Com = COM_SIEMENS;

  //------------------------------
  // Parametri Generali
  //------------------------------
  sprintf( Buffer, "COM%d", NumCom );
  strcpy(  AlgoDevice[ NumDispositivo ].ComName,     Buffer );
  strcpy(  AlgoDevice[ NumDispositivo ].Baud,        Baudreate);
  strcpy(  AlgoDevice[ NumDispositivo ].TcpAddress,  IpAddress );
  AlgoDevice[ NumDispositivo ].TcpPort = TcpPort;

  //------------------------------
  // Comunicazione S5
  //------------------------------
  if(TipoCom==0){
     //---------------------------
     // Parametrizzazione
     //---------------------------
     //strcpy(  AlgoDevice[ NumDispositivo ].ComName, "COM1" );
     //strcpy(  AlgoDevice[ NumDispositivo ].ComName, Buffer );
     //strcpy(  AlgoDevice[ NumDispositivo ].Baud,    "9600" );
  }
  //------------------------------
  // Comunicazione PPI
  //------------------------------
  if(TipoCom==1){
     //---------------------------
     // Parametrizzazione
     //---------------------------
     //strcpy(  AlgoDevice[ NumDispositivo ].ComName, "COM1" );
     //strcpy(  AlgoDevice[ NumDispositivo ].ComName, Buffer );
     //strcpy(  AlgoDevice[ NumDispositivo ].Baud,    "9600" );
  }
  //------------------------------
  // Comunicazione MPI
  //------------------------------
  if(TipoCom==2){
     //---------------------------
     // Parametrizzazione
     //---------------------------
     //strcpy(  AlgoDevice[ NumDispositivo ].ComName, "COM1" );
     //strcpy(  AlgoDevice[ NumDispositivo ].ComName, Buffer );
     //strcpy(  AlgoDevice[ NumDispositivo ].Baud,    "38400" );
  }
  //------------------------------
  // Comunicazione ETHERNET
  //------------------------------
  if(TipoCom==3){
     //---------------------------
     // Parametrizzazione
     //---------------------------
     //strcpy(  AlgoDevice[ NumDispositivo ].TcpAddress,  "192.168.1.141" );
  }


  if( AlgoNoDave_ConessionePlc( NumDispositivo, TipoCom, Slot, Rec, all_mess )){
     AlgoNoDave_DisconessionePlc( NumDispositivo);
     err = 1;
  }
  if( !err ){
     errore_comunicazione[Com][NumDispositivo]=0;
     SIM_PlcConnesso[NumDispositivo]=true;
  }
  if( err  ){
     errore_comunicazione[Com][NumDispositivo]++;  // Flag che indica un errore comunicazione
     if(errore_comunicazione[Com][NumDispositivo]>100) errore_comunicazione[Com][NumDispositivo]=1;
  }


  return err;
}
//---------------------------------------------------------------------------



//------------------------------------------------------------
// SIM_RiconnectToPlc()
// In caso di errore di comunicazione dosconnetto e ritento
//------------------------------------------------------------
int SIM_RiconnectToPlc( int NumDispositivo, int TipoCom, int NumCom, char* Baudreate, char* IpAddress, int TcpPort, int Slot, int Rec, char* all_mess)
{
  int Com, err=0;

  //---------------------------------------------
  // Gestione porte di comunicazione
  //---------------------------------------------
  Com = COM_SIEMENS;
  //-------------------------------------------------------
  // Se errore Riprovare connessione
  //-------------------------------------------------------
  //BtnConnection->Enabled = true;
  AlgoNoDave_DisconessionePlc( NumDispositivo);
  err = SIM_ConnectToPlc( NumDispositivo, TipoCom, NumCom, Baudreate, IpAddress, TcpPort, Slot, Rec, all_mess);

  if( !err ) errore_comunicazione[Com][NumDispositivo]=0;
  if( err  ){
     errore_comunicazione[Com][NumDispositivo]++;  // Flag che indica un errore comunicazione
     if(errore_comunicazione[Com][NumDispositivo]>100) errore_comunicazione[Com][NumDispositivo]=1;
  }

  return err;
}
//---------------------------------------------------------------------------


//---------------------------------
// SIM_S5ReadWordPlc()
//---------------------------------
int  SIM_S5ReadWordPlc( int NumDispositivo, int Area, int NumDB, int NrDatoIni, int NrDati, WORD* WordDati, char* all_mess)
{
  int   Com, res=0;
  int   NumWord;
  uc    StringaUc[700];

  //---------------------------------------------
  // Gestione porte di comunicazione
  //---------------------------------------------
  Com = COM_SIEMENS;

  try{
     NumWord=NrDati;

     res = AlgoNoDave_ReadBytes( NumDispositivo, Area, NumDB, NrDatoIni*2, NrDati*2, StringaUc, all_mess);
     if(res==0){
        AlgoNoDave_ConvertiStringaToWORD(WordDati, StringaUc, NumWord);
     }
  }
  catch (Exception &exception){
     sprintf( all_mess, "Exeption funzione S5_ReadPlc" );
     res = 1;
  }

  if( !res ) errore_comunicazione[Com][NumDispositivo]=0;
  if( res  ){
     errore_comunicazione[Com][NumDispositivo]++;  // Flag che indica un errore comunicazione
     if(errore_comunicazione[Com][NumDispositivo]>100) errore_comunicazione[Com][NumDispositivo]=1;
  }   

  return res;

}



//---------------------------------
// SIM_S5WriteWordPlc()
//---------------------------------
int  SIM_S5WriteWordPlc(int NumDispositivo, int Area, int NumDB, int NrDatoIni, int NrDati, WORD* WordDati, char* all_mess)
{
  int   Com, res=0;
  int   NumWord;
  uc    StringaUc[700];

  //---------------------------------------------
  // Gestione porte di comunicazione
  //---------------------------------------------
  Com = COM_SIEMENS;

  try{
     NumWord=NrDati;
     AlgoNoDave_ConvertiWORDToStringa( WordDati, StringaUc, NumWord);
     res = AlgoNoDave_WritedBytes( NumDispositivo, Area, NumDB, NrDatoIni*2, NrDati*2, StringaUc, all_mess);
  }
  catch (Exception &exception){
     sprintf( all_mess, "Exeption funzione S5_WritePlc" );
     res = 1;
  }

  if( !res ) errore_comunicazione[Com][NumDispositivo]=0;
  if( res  ){
     errore_comunicazione[Com][NumDispositivo]++;  // Flag che indica un errore comunicazione
     if(errore_comunicazione[Com][NumDispositivo]>100) errore_comunicazione[Com][NumDispositivo]=1;
  }

  return res;
}


//---------------------------------
// SIM_S7ReadWordPlc()
//---------------------------------
int  SIM_S7ReadWordPlc( int NumDispositivo, int Area, int NumDB, int NrDatoIni, int NrDati, WORD* WordDati, char* all_mess)
{
  int   Com, res=0;
  int   NumWord;
  uc    StringaUc[700];

  //---------------------------------------------
  // Gestione porte di comunicazione
  //---------------------------------------------
  Com = COM_SIEMENS;

  try{
     NumWord=NrDati;
     //res = AlgoNoDave_ReadBytes( NumDispositivo, Area, NumDB, NrDatoIni, NrDati*2, StringaUc, all_mess);
     res = SIM_S7ReadBytePlcSiemens( NumDispositivo, Area, NumDB, NrDatoIni, NrDati*2, StringaUc, all_mess);
     if(res==0){
        AlgoNoDave_ConvertiStringaToWORD(WordDati, StringaUc, NumWord);
     }
  }
  catch (Exception &exception){
     sprintf( all_mess, "Exeption funzione S7_ReadPlc" );
     res = 1;
  }

  if( !res ) errore_comunicazione[Com][NumDispositivo]=0;
  if( res  ){
     errore_comunicazione[Com][NumDispositivo]++;  // Flag che indica un errore comunicazione
     if(errore_comunicazione[Com][NumDispositivo]>100) errore_comunicazione[Com][NumDispositivo]=1;
  }

  return res;

}



//---------------------------------
// SIM_S7WriteWordPlc()
//---------------------------------
int  SIM_S7WriteWordPlc(int NumDispositivo, int Area, int NumDB, int NrDatoIni, int NrDati, WORD* WordDati, char* all_mess)
{
  int   Com, res=0;
  int   NumWord;
  uc    StringaUc[700];

  //---------------------------------------------
  // Gestione porte di comunicazione
  //---------------------------------------------
  Com = COM_SIEMENS;

  try{
     NumWord=NrDati;
     AlgoNoDave_ConvertiWORDToStringa( WordDati, StringaUc, NumWord);
     // res = AlgoNoDave_WritedBytes( NumDispositivo, Area, NumDB, NrDatoIni, NrDati*2, StringaUc, all_mess);
     res = SIM_S7WriteBytePlcSiemens( NumDispositivo, Area, NumDB, NrDatoIni, NrDati*2, StringaUc, all_mess);
  }
  catch (Exception &exception){
     sprintf( all_mess, "Exeption funzione S7_WritePlc" );
     res = 1;
  }

  if( !res ) errore_comunicazione[Com][NumDispositivo]=0;
  if( res  ){
     errore_comunicazione[Com][NumDispositivo]++;  // Flag che indica un errore comunicazione
     if(errore_comunicazione[Com][NumDispositivo]>100) errore_comunicazione[Com][NumDispositivo]=1;
  }

  return res;
}


//-------------------------------------------------------------------------------------------------
//                                 FUNZIONI SIEMENS AGGIUNTIVE
//-------------------------------------------------------------------------------------------------


//---------------------------------------   o   -----------------------------------


//---------------------------------------
//  SIM_S7ReadBytePlcSiemens()
//---------------------------------------
int  SIM_S7ReadBytePlcSiemens( int NumDispositivo, int TipoArea, int  NumDB,  int  NrByteIni, int NumByte, BYTE * DataByte, char* all_mess)
{
  short int err=0;
  int  i;
  char* IpAddress;        // IP ADDRESS PLC
  bool  SemprreON=true;

  IpAddress= OmrPlc[ NumDispositivo ].RemoteHost.c_str();                 // IP ADDRES PLC
  //-----------------------------------------------------------------
  // 22/07/2008 Se TipoCom=3 -> Comunicazione EThernet con NoDave
  //            altrimenti   -> Standard 3964R
  //-----------------------------------------------------------------
  if( SemprreON ){
     strcpy( all_mess, "");
     //-----------------------------------------------------------------------
     // Se PLC NON connesso --> SKIP
     //-----------------------------------------------------------------------
     if( SIM_PlcConnesso[NumDispositivo]==0 ){
        sprintf( all_mess, "Plc %02d Not Connected IpAddress: %s", NumDispositivo, IpAddress );
        return 1;
     }
     //-----------------------------------------------------------------------
     // LETTURA
     //-----------------------------------------------------------------------
     if( err==0 ){
        try{
           err = (short int) AlgoNoDave_ReadBytes( NumDispositivo, TipoArea, NumDB, NrByteIni, NumByte, DataByte, all_mess);
        }
        catch (Exception &exception){
           sprintf( all_mess, "Exeption funzione ReadPlcSiemens()" );
           return 1;
        }
     }
     //----------------------------------------------------------------------
     // Sconnetti il PLC
     //----------------------------------------------------------------------
     if( err  ) SIM_PlcConnesso[NumDispositivo]=false;
     if( SIM_PlcConnesso[NumDispositivo]==0 ){
        AlgoNoDave_DisconessionePlc( NumDispositivo);
     }
  }

  return err;
}


//---------------------------------------
//  SIM_S7WriteBytePlcSiemens()
//---------------------------------------
int  SIM_S7WriteBytePlcSiemens( int NumDispositivo, int TipoArea, int  NumDB,  int  NrByteIni, int NumByte, BYTE * DataByte, char* all_mess)
{
  short int err=0;
  int  i;
  char* IpAddress;                               // IP ADDRESS PLC
  bool  SemprreON=true;

  IpAddress= OmrPlc[ NumDispositivo ].RemoteHost.c_str();                 // IP ADDRES PLC
  //-----------------------------------------------------------------
  // 22/07/2008 Se TipoCom=3 -> Comunicazione EThernet con NoDave
  //            altrimenti   -> Standard 3964R
  //-----------------------------------------------------------------
  if( SemprreON ){
     strcpy( all_mess, "");
     //-----------------------------------------------------------------------
     // Se PLC NON connesso --> SKIP
     //-----------------------------------------------------------------------
     if( SIM_PlcConnesso[NumDispositivo]==0 ){
        sprintf( all_mess, "Plc %02d Not Connected IpAddress: %s", NumDispositivo, IpAddress );
        return 1;
     }
     //-----------------------------------------------------------------------
     // SCRITTURA
     //-----------------------------------------------------------------------
     if( err==0 ){
        try{
           err = (short int) AlgoNoDave_WritedBytes( NumDispositivo, TipoArea, NumDB, NrByteIni, NumByte, DataByte, all_mess);
        }
        catch (Exception &exception){
           sprintf( all_mess, "Exeption funzione WritePlcSiemens()" );
           return 1;
        }
     }
     //----------------------------------------------------------------------
     // Sconnetti il PLC
     //----------------------------------------------------------------------
     if( err  ) SIM_PlcConnesso[NumDispositivo]=false;
     if( SIM_PlcConnesso[NumDispositivo]==0 ){
        AlgoNoDave_DisconessionePlc( NumDispositivo);
     }
  }

  return err;
}



//---------------------------------------------------------------------------
// SIM_S7ReadDBSiemens()
//
// Lettura di una DB in più step di lettura
//---------------------------------------------------------------------------
int  SIM_S7ReadDBSiemens( int NumDispositivo, int  NumDB,  int NrByteIni, int LenDb, BYTE * DataByte, char* all_mess)
{
  int   TipoArea      = 1; // DB
  int   err=0;
  int   NumByte=0;
  int   MaxLenPacchetto=200;
  int   NumCicliTx;
  int   Ciclo;
  int   ByteRestanti=0;
  int   ByteOffset=0;


  ByteOffset = NrByteIni;
  //-------------------------------------------------------------------
  // Verifica cicli di Trasmissione
  //-------------------------------------------------------------------
  if( err==0 ){
     //-------------------------------------------------------------------
     // Compila Dati
     //-------------------------------------------------------------------
     NumCicliTx = LenDb/MaxLenPacchetto;
     for( Ciclo=0; Ciclo<=NumCicliTx  ; Ciclo++){
        //----------------------------------
        // Calcola Offeset e Byte Restanti
        //----------------------------------
        ByteOffset     = ByteOffset+NumByte;
        ByteRestanti   = LenDb-ByteOffset;

        if( err              ) break;
        if( ByteRestanti<= 0 ) break;
        //----------------------------------
        // Calcola Byte da TxRx
        //----------------------------------
        NumByte = MaxLenPacchetto;
        if( ByteRestanti<=MaxLenPacchetto ){
           NumByte = ByteRestanti;
        }
        //----------------------------------
        // Lettura Area DB
        //----------------------------------
        NrByteIni=ByteOffset;
        err = SIM_S7ReadBytePlcSiemens( NumDispositivo, TipoArea, NumDB, ByteOffset, NumByte, &DataByte[ ByteOffset], all_mess);
        if( err ) break;
     }
  }

  return err;
}


//---------------------------------------------------------------------------
// SIM_S7WriteDBSiemens()
//
// Lettura di una DB in più step di lettura
//---------------------------------------------------------------------------
int  SIM_S7WriteDBSiemens(int NumDispositivo, int  NumDB,  int NrByteIni, int LenDb, BYTE * DataByte, char* all_mess)
{
  int   TipoArea      = 1; // DB
  int   err=0;
  int   NumByte=0;
  int   MaxLenPacchetto=200;
  int   NumCicliTx;
  int   Ciclo;
  int   ByteRestanti=0;
  int   ByteOffset=0;


  ByteOffset = NrByteIni;
  //-------------------------------------------------------------------
  // Verifica cicli di Trasmissione
  //-------------------------------------------------------------------
  if( err==0 ){
     //-------------------------------------------------------------------
     // Compila Dati
     //-------------------------------------------------------------------
     NumCicliTx = LenDb/MaxLenPacchetto;
     for( Ciclo=0; Ciclo<=NumCicliTx  ; Ciclo++){
        //----------------------------------
        // Calcola Offeset e Byte Restanti
        //----------------------------------
        ByteOffset     = ByteOffset+NumByte;
        ByteRestanti   = LenDb-ByteOffset;

        if( err              ) break;
        if( ByteRestanti<= 0 ) break;
        //----------------------------------
        // Calcola Byte da TxRx
        //----------------------------------
        NumByte = MaxLenPacchetto;
        if( ByteRestanti<=MaxLenPacchetto ){
           NumByte = ByteRestanti;
        }
        //----------------------------------
        // Lettura Area DB
        //----------------------------------
        NrByteIni=ByteOffset;
        err = SIM_S7WriteBytePlcSiemens( NumDispositivo, TipoArea, NumDB, ByteOffset, NumByte, &DataByte[ ByteOffset ], all_mess);
        if( err ) break;
     }
  }

  return err;
}




