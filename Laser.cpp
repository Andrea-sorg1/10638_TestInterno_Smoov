// ------------------------------------------------------------------------
//                      file     LASER.CPP
// ------------------------------------------------------------------------
#include <stdio.h>
#include <mem.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <vcl\inifiles.hpp>

#include "tecnofer.h"
#include "Mapping.h"
#include "laser.h"



#define MAXSTRINGA                  100
#define MSEC_TIMEOUT                300

extern int  OmrTrasmissioneInCorso; 
extern int  MainTimerPrincipaleInCorso;

//--------------------------------  o  ---------------------------------

#define BUFSIZE   (1024)


//-------------------------------
//   ReadIniParametriLaser()
//-------------------------------
// Lettura del file *.ini di parametrizzazione LASER
//
void ReadIniParametriLaser()
{
  TIniFile   *IniFile;
  AnsiString FPath;
  AnsiString FIniFileName;

  FPath          = ExtractFilePath(ParamStr(0));
  FIniFileName   = FPath + "LASER\\ParamLaser.ini";
  IniFile        = new TIniFile(FIniFileName);
  //-------------------------------------------------------
  // TIMEOUT
  //-------------------------------------------------------
  MsecTimeOutSA  = IniFile->ReadInteger("TimeOut", "SA_Command",  3000);
  MsecTimeOutPA  = IniFile->ReadInteger("TimeOut", "PA_Command",  3000);
  MsecTimeOutPC  = IniFile->ReadInteger("TimeOut", "PC_Command",  3000);
  MsecTimeOutPP  = IniFile->ReadInteger("TimeOut", "PP_Command",  3000);
  MsecTimeOutPL  = IniFile->ReadInteger("TimeOut", "PL_Command",  3000);
  MsecTimeOutMA  = IniFile->ReadInteger("TimeOut", "MA_Command",  3000);
  MsecTimeOutMS  = IniFile->ReadInteger("TimeOut", "MS_Command", 30000);
  MsecTimeOutMM  = IniFile->ReadInteger("TimeOut", "MM_Command", 30000);
  MsecTimeOutMN  = IniFile->ReadInteger("TimeOut", "MN_Command", 30000);
  MsecTimeOutMR  = IniFile->ReadInteger("TimeOut", "MR_Command",  3000);
  MsecTimeOutDA  = IniFile->ReadInteger("TimeOut", "DA_Command",  3000);
  MsecTimeOutDR  = IniFile->ReadInteger("TimeOut", "DR_Command",  3000);
  MsecTimeOutUA  = IniFile->ReadInteger("TimeOut", "UA_Command",  3000);
  MsecTimeOutUR  = IniFile->ReadInteger("TimeOut", "UR_Command",  3000);
  MsecTimeOutRS  = IniFile->ReadInteger("TimeOut", "RS_Command",  3000);

  //-------------------------------------------------------
  // PARAMETRI VARI
  //-------------------------------------------------------
  Laser_RotationMode         =IniFile->ReadInteger("PARAM", "Laser_RotationMode",  1);
  Laser_AbilitaZonaMapSuNodo =IniFile->ReadBool("PARAM", "Laser_AbilitaZonaMapSuNodo",  1);

  delete IniFile;
}
//---------------------------------------------------------------------------

//-------------------------------
//   WriteIniParametriLaser()
//-------------------------------
// Scrittura del file *.ini di sistema
//
void WriteIniParametriLaser()
{
  TIniFile *IniFile;
  AnsiString FPath;
  AnsiString FIniFileName;

  FPath        = ExtractFilePath(ParamStr(0));
  FIniFileName = FPath + "LASER\\ParamLaser.ini";
  IniFile      = new TIniFile(FIniFileName);
  //-------------------------------------------------------
  // Scrittura su file della release del software
  //-------------------------------------------------------
  IniFile->WriteInteger("TimeOut", "SA_Command",  MsecTimeOutSA);
  IniFile->WriteInteger("TimeOut", "PA_Command",  MsecTimeOutPA);
  IniFile->WriteInteger("TimeOut", "PC_Command",  MsecTimeOutPC);
  IniFile->WriteInteger("TimeOut", "PP_Command",  MsecTimeOutPP);
  IniFile->WriteInteger("TimeOut", "PL_Command",  MsecTimeOutPL);
  IniFile->WriteInteger("TimeOut", "MA_Command",  MsecTimeOutMA);
  IniFile->WriteInteger("TimeOut", "MS_Command",  MsecTimeOutMS);
  IniFile->WriteInteger("TimeOut", "MM_Command",  MsecTimeOutMM);
  IniFile->WriteInteger("TimeOut", "MN_Command",  MsecTimeOutMN);
  IniFile->WriteInteger("TimeOut", "MR_Command",  MsecTimeOutMR);
  IniFile->WriteInteger("TimeOut", "DA_Command",  MsecTimeOutDA);
  IniFile->WriteInteger("TimeOut", "DR_Command",  MsecTimeOutDR);
  IniFile->WriteInteger("TimeOut", "UA_Command",  MsecTimeOutUA);
  IniFile->WriteInteger("TimeOut", "UR_Command",  MsecTimeOutUR);
  IniFile->WriteInteger("TimeOut", "RS_Command",  MsecTimeOutRS);

  //-------------------------------------------------------
  // PARAMETRI VARI
  //-------------------------------------------------------
  IniFile->WriteInteger("PARAM", "Laser_RotationMode",           Laser_RotationMode);
  IniFile->WriteBool   ("PARAM", "Laser_AbilitaZonaMapSuNodo",   Laser_AbilitaZonaMapSuNodo);
  delete IniFile;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// LaserProcessaWindowsMessage()
// Attendi per x msec processando tutti i messaggi di WINDOWS
//---------------------------------------------------------------------------
void LaserProcessaWindowsMessage( int MsecAttesa)
{
  SYSTEMTIME TimeInizio;
  SYSTEMTIME TimeFine;
  int WaitReponse;
  int msec;

  WaitReponse=0;
  GetLocalTime( &TimeInizio );
  for( ;; ){
     Application->ProcessMessages();
     if (Application->Terminated)   break;
     if( LaserInterrompiComando )   break;
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
        if( msec > MsecAttesa ) break;
        WaitReponse=0;
     }
     WaitReponse++;
  }

  return ;
}


//---------------------------------------------------------------------------
// LaserWaitConclusioneComandoEthernet()
// Attendi per x msec processando tutti i messaggi di WINDOWS
//---------------------------------------------------------------------------
int LaserWaitConclusioneComandoEthernet( int MsecAttesa )
{
  SYSTEMTIME TimeInizio;
  SYSTEMTIME TimeFine;
  int WaitReponse;
  int msec;
  int err;

  WaitReponse=0;
  err =0;
  GetLocalTime( &TimeInizio );
  for( ;; ){
     if (Application->Terminated)   break;
     if( LaserInterrompiComando )   break;
     if( OmrTrasmissioneInCorso==0 )   break;

     Application->ProcessMessages();
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
        if( msec > MsecAttesa ) {err=1; break;}
        WaitReponse=0;
     }
     WaitReponse++;
  }

  return err ;
}



//-----------------------------------
// LaserConvertiWordToStringa()
//-----------------------------------
// Converi un array di WORD in array di CHAR  di lunghezza LenCHAR
// mantenendo ordine dei byte per essere coerente con LASER SICK
// SX (-) Significativo ---> DX (+) Significativo
//---------------------------------------------------------------------------
int LaserConvertiWordToStringa( WORD* ArrayWORD, char* ArrayCHAR, int LenCHAR)
{
  int i;
  char *PtrWORD;

  PtrWORD = (char *) ArrayWORD;
  //----------------------------------
  // Inverti i byte di peso maggiore
  // con quelli di peso minore
  //----------------------------------
  for( i=0; i<LenCHAR; i+=2){
     ArrayCHAR[i+0]=PtrWORD[i+0];
     ArrayCHAR[i+1]=PtrWORD[i+1];
  }
  return 0;
}

//-----------------------------------
// LaserConvertiStringaToWord()
//-----------------------------------
// Converi un array di CHAR in array di WORD  di lunghezza LenCHAR
// mantenendo ordine dei byte per essere coerente con LASER SICK
// SX (-) Significativo ---> DX (+) Significativo
//---------------------------------------------------------------------------
int LaserConvertiStringaToWord( WORD* ArrayWORD, char* ArrayCHAR, int LenCHAR)
{
  int i;
  char *PtrWORD;

  PtrWORD = (char *) ArrayWORD;
  //----------------------------------
  // Inverti i byte di peso maggiore
  // con quelli di peso minore
  //----------------------------------
  for( i=0; i<LenCHAR; i+=2){
     PtrWORD[i+0] = ArrayCHAR[i+0];
     PtrWORD[i+1] = ArrayCHAR[i+1];
  }
  return 0;
}



//-----------------------------------
// LaserConvertiIntToStringa()
//-----------------------------------
// Converi un array di INT in array di CHAR  di lunghezza LenCHAR
// mantenendo ordine dei byte per essere coerente con LASER SICK
// SX (-) Significativo ---> DX (+) Significativo
//---------------------------------------------------------------------------
int LaserConvertiIntToStringa( int *ArrayINT, char* ArrayCHAR, int LenCHAR)
{
  int i;
  char *PtrINT;

  PtrINT = (char *) ArrayINT;
  //----------------------------------
  // Inverti i byte di peso maggiore
  // con quelli di peso minore
  //----------------------------------
  for( i=0; i<LenCHAR; i+=4){
     ArrayCHAR[i+0]=PtrINT[i+0];
     ArrayCHAR[i+1]=PtrINT[i+1];
     ArrayCHAR[i+2]=PtrINT[i+2];
     ArrayCHAR[i+3]=PtrINT[i+3];
  }
  return 0;
}

//-----------------------------------
// LaserConvertiStringaToInt()
//-----------------------------------
// Converi un array di CHAR in array di INT  di lunghezza LenCHAR
// mantenendo ordine dei byte per essere coerente con LASER SICK
// SX (-) Significativo ---> DX (+) Significativo
//---------------------------------------------------------------------------
int LaserConvertiStringaToInt( int* ArrayINT, char* ArrayCHAR, int LenCHAR)
{
  int i;
  char *PtrINT;

  PtrINT = (char *) ArrayINT;
  //----------------------------------
  // Inverti i byte di peso maggiore
  // con quelli di peso minore
  //----------------------------------
  for( i=0; i<LenCHAR; i+=4){
     PtrINT[i+0] = ArrayCHAR[i+0];
     PtrINT[i+1] = ArrayCHAR[i+1];
     PtrINT[i+2] = ArrayCHAR[i+2];
     PtrINT[i+3] = ArrayCHAR[i+3];
  }
  return 0;
}


//-----------------------------------
// LaserConvertiDmToStringa()
//-----------------------------------
// Converi un array di WORD in array di CHAR  di lunghezza LenCHAR
// mantenendo ordine dei byte per essere coerente con LASER SICK
// SX (-) Significativo ---> DX (+) Significativo
//---------------------------------------------------------------------------
int LaserConvertiDmToStringa( WORD* ArrayWORD, char* ArrayCHAR, int LenCHAR)
{
  int i;
  char *PtrWORD;

  PtrWORD = (char *) ArrayWORD;
  //----------------------------------
  // Inverti i byte di peso maggiore
  // con quelli di peso minore
  //----------------------------------
  for( i=0; i<LenCHAR; i+=2){
     ArrayCHAR[i+0]=PtrWORD[i+1];
     ArrayCHAR[i+1]=PtrWORD[i+0];
  }
  return 0;
}

//-----------------------------------
// LaserConvertiStringaToDm()
//-----------------------------------
// Converi un array di CHAR in array di WORD  di lunghezza LenCHAR
// mantenendo ordine dei byte per essere coerente con LASER SICK
// SX (-) Significativo ---> DX (+) Significativo
//---------------------------------------------------------------------------
int LaserConvertiStringaToDm( WORD* ArrayWORD, char* ArrayCHAR, int LenCHAR)
{
  int i;
  char *PtrWORD;

  PtrWORD = (char *) ArrayWORD;
  //----------------------------------
  // Inverti i byte di peso maggiore
  // con quelli di peso minore
  //----------------------------------
  for( i=0; i<LenCHAR; i+=2){
     PtrWORD[i+0] = ArrayCHAR[i+1];
     PtrWORD[i+1] = ArrayCHAR[i+0];
  }
  return 0;
}


//--------------------------------
//     LaserCalcolaBCC()
//--------------------------------
// Calcola il Frame Check Sum facendo un XOR dei caratteri della stringa
// restituisce un unsigned char contenente il BCC calcolato
//
int LaserCalcolaBCC(char *stringa, int LenStringa, unsigned char* Bcc, char* all_mess)
{
  int  i;
  unsigned char BCCCalcolato;

  strcpy( all_mess, "");
  BCCCalcolato=NULL;
  *Bcc=0;
  for( i=0; i<=MAXSTRINGA; i++){
    if( i>= LenStringa ) break;
    BCCCalcolato ^= stringa[i];      //XOR
  }

  *Bcc = BCCCalcolato;

  if( i>=MAXSTRINGA){
    sprintf(all_mess, "Error buffer exceeds %d \n-LaserCalcolaBCC()", MAXSTRINGA);
    return 1;
  }
  return 0;
}

//--------------------------------------- o ------------------------------------


//---------------------------------------------
//  LaserAbilitaTipoComunicazione()
//---------------------------------------------
// Settando questa variabile sul PLC si imposta il tipo di comunicazione
// e/o il tipo di guida....
// DM1  = 0 -Controllo comunicazione da parte del PC (invio di comandi diertti al LASER)
//      = 1 -Guida con magneti (si informa il PLC che ora deve eseguire la guida con i magneti)
//      = 2 -Guida LASER ( si informa il PLC che deve utilizzare la guida LASER e riabilitare quindi
//           anche la comunicazione seriale diretta con il LASER)
//
int LaserAbilitaTipoComunicazione( int NumeroTgv, int Modo, char* all_mess)
{
  int  err=0;
  WORD dm[20];
  WORD NumCanali=1;
  WORD NumCanaleIniziale=1;

  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  dm[0] = (WORD) Modo;
  err = OM.wr_canale( &dm[0], (WORD)(NumeroTgv), "WD", NumCanaleIniziale, NumCanali, all_mess);

  //-----------------------------------------------------
  // Tempo di attesa di eventuali comunicazioni in corso
  //-----------------------------------------------------
  //if(err==0) delay(1000);

  return err;
}



//---------------------------------------------
//  LaserLeggiAreaDmLaser()
//---------------------------------------------
// Leggi da PLC i DM interesati dal LASER DM0..DM250
//
int LaserLeggiAreaDmLaser( int NumeroTgv, short int* DM, char* all_mess)
{
  int  err=0;
  WORD dm[20];
  WORD NumCanali=1;
  WORD NumCanaleIniziale=0;  //&&&&&&&

  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  NumCanali=240;
  NumCanaleIniziale=0;

  err = OM.ld_canale((unsigned short int*) DM, (WORD)(NumeroTgv), "RD", NumCanaleIniziale, NumCanali, all_mess);
  //--------------------------------
  // Converti valore BCD in binario
  // DM129 - Numero riflettori
  //--------------------------------
//TraduciPLCtoPC(&dm[129]);

  return err;
}


//---------------------------------------------
//  LaserInvioValoreDiTimeOut()
//---------------------------------------------
// Invio da PC anche il TimeOut richiesto per il comando
// DM 131
//
int LaserInvioValoreDiTimeOut( int NumeroTgv, int MsecTimeOutTrs, char* all_mess)
{
  int  err=0;
  WORD dm[20];
  WORD NumCanali=1;
  WORD NumCanaleIniziale=0;  //&&&&&&&

  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  NumCanali=1;
  NumCanaleIniziale=131;

  dm[0] = (WORD) (MsecTimeOutTrs/100);
  err = OM.wr_canale( &dm[0], (WORD)(NumeroTgv), "WD", NumCanaleIniziale, NumCanali, all_mess);

  return err;
}

//---------------------------------------------
//  LaserLeggiNumeroSpecchi()
//---------------------------------------------
// Leggi da PLC anche il numero di specchi da utilizzare (ultimo impostato)
// DM 130
//
int LaserLeggiNumeroSpecchi( int NumeroTgv, int* Specchi, char* all_mess)
{
  int  err=0;
  WORD dm[20];
  WORD NumCanali=1;
  WORD NumCanaleIniziale=0;  //&&&&&&&

  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  NumCanali=1;
  NumCanaleIniziale=130;

  err = OM.ld_canale( &dm[0], (WORD)(NumeroTgv), "RD", NumCanaleIniziale, NumCanali, all_mess);
  if( err==0 ){
     TraduciPLCtoPC(&dm[0]);   // Converti valore BCD in binario
     *Specchi = (int) dm[0];
  }

  return err;
}


//---------------------------------------------
//  LaserInvioNumeroSpecchi()
//---------------------------------------------
// Invio da PC anche il numero di specchi da utilizzare per la triangolazione
// DM 130
//
int LaserInvioNumeroSpecchi( int NumeroTgv, int Specchi, char* all_mess)
{
  int  err=0;
  WORD dm[20];
  WORD NumCanali=1;
  WORD NumCanaleIniziale=0;  //&&&&&&&

  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  NumCanali=1;
  NumCanaleIniziale=130;

  dm[0] = (WORD)  Specchi;
  TraduciPCtoPLC(&dm[0]);   // Converti valore BCD in binario
  err = OM.wr_canale( &dm[0], (WORD)(NumeroTgv), "WD", NumCanaleIniziale, NumCanali, all_mess);

  return err;
}

//---------------------------------------------
//  LaserInvioNumeroMaxZone()
//---------------------------------------------
// Invio da PC anche il numero massimo di zone  da utilizzare per la triangolazione
// DM 139
int LaserInvioNumeroMaxZone( int NumeroTgv, int MaxZone, char* all_mess)
{
  int  err=0;
  WORD dm[20];
  WORD NumCanali=1;
  WORD NumCanaleIniziale=0;  //&&&&&&&

  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  NumCanali=1;
  NumCanaleIniziale=139;

  dm[0] = (WORD)  MaxZone;
  err = OM.wr_canale( &dm[0], (WORD)(NumeroTgv), "WD", NumCanaleIniziale, NumCanali, all_mess);

  return err;
}

//---------------------------------------------
//  LaserInvioUltimaZonaMapping()
//---------------------------------------------
// Invio da PC l'ultima zona in cui è stato fatto il mapping
// DM 140
int LaserInvioUltimaZonaMapping  ( int NumeroTgv, int Zona, char* all_mess)
{
  int  err=0;
  WORD dm[20];
  WORD NumCanali=1;
  WORD NumCanaleIniziale=0;  //&&&&&&&

  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  NumCanali=1;
  NumCanaleIniziale=140;

  dm[0] = (WORD)  Zona;
  err = OM.wr_canale( &dm[0], (WORD)(NumeroTgv), "WD", NumCanaleIniziale, NumCanali, all_mess);

  return err;
}



//---------------------------------------------
//  LaserProceduraCalibratura()
//---------------------------------------------
// Settando questa variabile sul PLC si imposta il tipo di procedura in corso
// Modo DM0  = 0 -Nessuna procedura in corso
//           = 1 -Procedura calcolo sviluppo encoder in corso
//           = 2 -Procedura calibratura sterzo in corso
//
int LaserProceduraCalibratura( int NumeroTgv, int Modo, char* all_mess)
{
  int  err=0;
  WORD dm[20];
  WORD NumCanali=1;
  WORD NumCanaleIniziale=1;

  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  NumCanaleIniziale=0;
  dm[0] = (WORD) Modo;
  err = OM.wr_canale( &dm[0], (WORD)(NumeroTgv), "WD", NumCanaleIniziale, NumCanali, all_mess);


  return err;
}


//---------------------------------------------
//  LaserInvioRapportoEncoder()
//---------------------------------------------
//
int LaserInvioRapportoEncoder( int NumeroTgv, int Val, char* all_mess)
{
  int  err=0;
  WORD dm[3];
  WORD NumCanali=1;
  WORD NumCanaleIniziale=1;

  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  NumCanaleIniziale=200;
  dm[0] = (WORD) Val;
  err = OM.wr_canale( &dm[0], (WORD)(NumeroTgv), "WD", NumCanaleIniziale, NumCanali, all_mess);


  return err;
}


//---------------------------------------------
//  LaserLeggiCentroSterzo()
//---------------------------------------------
// DM28
//
int LaserLeggiCentroSterzo( int NumeroTgv, int* Val, char* all_mess)
{
  int  err=0;
  WORD dm[20];
  WORD NumCanali=1;
  WORD NumCanaleIniziale=1;

  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  NumCanaleIniziale=28;
  err = OM.ld_canale( &dm[0], (WORD)(NumeroTgv), "RD", NumCanaleIniziale, NumCanali, all_mess);
  if( err==0 ){
     TraduciPLCtoPC(&dm[0]);   // Converti valore BCD in binario
     *Val = (int) dm[0];
  }

  return err;
}



//---------------------------------------------
//  LaserInvioCentroSterzo()
//---------------------------------------------
//
int LaserInvioCentroSterzo( int NumeroTgv, int Val, char* all_mess)
{
  int  err=0;
  WORD dm[20];
  WORD NumCanali=1;
  WORD NumCanaleIniziale=1;

  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  NumCanaleIniziale=28;
  dm[0] = (WORD) Val;
  TraduciPCtoPLC(&dm[0]);   // Converti valore in BCD
  err = OM.wr_canale( &dm[0], (WORD)(NumeroTgv), "WD", NumCanaleIniziale, NumCanali, all_mess);

  return err;
}


//---------------------------------------------
//  LaserLeggiScostamentoPuntoLogico()
//  DM 210
//---------------------------------------------
//
int LaserLeggiScostamentoPuntoLogico( int NumeroTgv, int* Val, char* all_mess)
{
  int  err=0;
  WORD dm[20];
  WORD NumCanali=1;
  WORD NumCanaleIniziale=1;

  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  NumCanaleIniziale=210;
  err = OM.ld_canale( &dm[0], (WORD)(NumeroTgv), "RD", NumCanaleIniziale, NumCanali, all_mess);
  if( err==0 ){
     *Val = (int) dm[0];
  }

  return err;
}


//-----------------------------------------  o   -------------------------------------

//---------------------------------------------
//  LaserTrsComandoGenerico()
//---------------------------------------------
// - Viene passata la stringa da trasmettere e la lunghezza (incluso il BCC):
//   in realtà il BCC viene aggiunto da questa funzione
// - Restituisce la stringa ricevuta facendo anche il controllo del BCC
//
//
int LaserTrsComandoGenerico( int NumeroTgv, int MsecTimeOutTrs, char *StringaTrasmissione, char *StringaRicezione, int LenTrasmissione, int LenRicezione, char* all_mess)
{
  int  err=0;
  WORD dm[20];
  WORD NumCanaleIniziale=0;
  WORD NumCanali=1;
  SYSTEMTIME TimeInizio;
  SYSTEMTIME TimeFine1;
  unsigned char CharBccCalcolato;
  unsigned char CharBccRicevuto;
  int MillisecAttesaRisposta;
  char buffer[200];
  char LenRx[5];

  strcpy( all_mess, "OK");
  memset(&dm[0],    0,    sizeof(dm));
  memset(&LenRx[0], NULL, sizeof(LenRx));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;
  if( LenTrasmissione<1 ) return 1;
  if( LenRicezione<1    ) return 1;

  //-------------------------------------------
  // 16/11/2002
  // Prova funzione di attesa
  //-------------------------------------------
  //LaserWaitConclusioneComandoEthernet( 1000 );

  //-------------------------------------------------------------------
  // (0) Trasmetti anche al PLC il valore di TIME-OUT per il comando
  //-------------------------------------------------------------------
  err= LaserInvioValoreDiTimeOut( NumeroTgv, MsecTimeOutTrs, all_mess);
  if( err ) return err;
  //-------------------------------------------------------
  // (A) TRASMISSIONE
  //-------------------------------------------------------
  // Prepara Dati trasmissione
  // DM 4799 = Numero dei byte che si aspetta di ricevere
  // DM 4800 = Inizio stringa trasmissione
  // DM 4798 = START (Numero dei byte da trasmettere)
  //-------------------------------------------------------
  LaserCalcolaBCC( &StringaTrasmissione[0], LenTrasmissione-1, &CharBccCalcolato, all_mess);
  StringaTrasmissione[ LenTrasmissione-1 ]= CharBccCalcolato;

  NumCanaleIniziale = 4799;
  NumCanali         = 12;
  dm[0] = (WORD) LenRicezione;
  LaserConvertiStringaToDm( &dm[1], StringaTrasmissione, LenTrasmissione);
  err = OM.wr_canale( &dm[0], (WORD)(NumeroTgv), "WD", NumCanaleIniziale, NumCanali, all_mess);
  if( err ) return err;

  //-------------------------------------------------------
  // Setta il DM 4798
  // DM 4798 = START (Numero dei byte da trasmettere)
  //-------------------------------------------------------
  NumCanaleIniziale = 4798;
  NumCanali         = 1;
  dm[0] = (WORD) LenTrasmissione;
  err = OM.wr_canale( &dm[0], (WORD)(NumeroTgv), "WD", NumCanaleIniziale, NumCanali, all_mess);
  if( err ) return err;


//delay(100);
  //-------------------------------------------------------
  // (B) RICEZIONE
  //-------------------------------------------------------
  // Attesa RISPOSTA
  // DM 4898 = Numero dei byte ricevuti
  // DM 4899 = Errore: TimeOut, Bcc, ... Messaggio di errore
  //           del LASER es: "comando non riconosciuto" ecc..
  // DM 4900 = Inizio stringa Ricezione
  //-------------------------------------------------------
  GetLocalTime( &TimeInizio );
  memset(&dm[0], 0, sizeof(dm));
  for(;;){
     NumCanaleIniziale = 4898;
     NumCanali         = 12;
     err = OM.ld_canale( &dm[0], (WORD)(NumeroTgv), "RD", NumCanaleIniziale, NumCanali, all_mess);
     if( err ){
        return err;
     }

     //...
     //dm[0]=0;
     if( dm[0]!=0 ) break;
     if( dm[1]!=0 ) break;

     if( LaserInterrompiComando ) {
        sprintf( all_mess, "Comando interrotto da utente");
        return 1;
     }

     //-------------------------------------
     // Controllo TimeOut
     //-------------------------------------
     GetLocalTime( &TimeFine1 );
     OM.ld_canale( &dm[0], 0, "RD", NumCanaleIniziale, NumCanali, buffer);
     MillisecAttesaRisposta = (int) (TimeFine1.wMinute - TimeInizio.wMinute);
     if( MillisecAttesaRisposta<0 ) MillisecAttesaRisposta+=60; // Se sto cambiando i minuti
     MillisecAttesaRisposta = (MillisecAttesaRisposta * 60000);
     MillisecAttesaRisposta = MillisecAttesaRisposta + (int)( 1000 * (TimeFine1.wSecond - TimeInizio.wSecond));
     MillisecAttesaRisposta = MillisecAttesaRisposta + (int) (TimeFine1.wMilliseconds - TimeInizio.wMilliseconds);
     if( MillisecAttesaRisposta>MsecTimeOutTrs ){
        sprintf( all_mess, "Time Out scaduto %d msec (%d)", MsecTimeOutTrs, MillisecAttesaRisposta);
        return 1;
     }
     //-------------------------------------
     // Visualizzazione stringa di attesa
     // per comandi:
     // MS, MM e MN
     //-------------------------------------
//   if( MsecTimeOutTrs>10000 ){
     if(StringaTrasmissione[2]=='M' && (StringaTrasmissione[3]=='S' || StringaTrasmissione[3]=='M' || StringaTrasmissione[3]=='N')){
       sprintf( buffer, "MS Command wait time %d", MsecTimeOutTrs-MillisecAttesaRisposta );
       FormMappa->TextStato->Caption = buffer;
       // &&&& Per visualizzazione
       LaserProcessaWindowsMessage( 30 );
     }
     //--------------------------------------
  }
  //---------------------------------------------------
  // GB-15/03/2012 In uscita leggi ulteriormente per
  // essere sicuro che la striga sia compilata
  //---------------------------------------------------
  NumCanaleIniziale = 4898;
  NumCanali         = 12;
  err = OM.ld_canale( &dm[0], (WORD)(NumeroTgv), "RD", NumCanaleIniziale, NumCanali, all_mess);
  if( err ) return err;
  //------------------------------------------------------------
  // Messaggio di errore da parte del LASER
  // DM4898==9  str[5]=(F1) str[6]=(F2) str[7]=(F3)
  //------------------------------------------------------------
  if( (dm[0]==9) && (dm[1]!=0) ){
     LaserConvertiDmToStringa( &dm[2], StringaRicezione, dm[0]);
     sprintf( all_mess, "Messaggio di errore LASER comando=%c%c: F1=%d, F2=%d, F3=%d",
              StringaRicezione[2], StringaRicezione[4],
              StringaRicezione[5], StringaRicezione[6], StringaRicezione[7]);
     return 1;
  }
  //------------------------------------------------------------
  // Errore di ricezione: TimeOut, BCC ecc..
  //------------------------------------------------------------
  if( dm[1]!=0 ){
     sprintf( all_mess, "Errore ricezione da parte PLC (TimeOut) " );
     return 1;
  }
  //------------------------------------------------------------
  // Lunghezza stringa non corretta
  //------------------------------------------------------------
  if( dm[0]!=LenRicezione ){
     sprintf( all_mess, "Lunghezza stringa di risposta non corretta: attesa=%d, ricevuta=%d",LenRicezione, dm[0] );
     return 1;
  }

  LaserConvertiDmToStringa( &dm[2], StringaRicezione, LenRicezione);
  LaserCalcolaBCC( &StringaRicezione[0], LenRicezione-1, &CharBccCalcolato, all_mess);
  //------------------------------------------------------------
  // Calcolo BCC non corretto
  //------------------------------------------------------------
  CharBccRicevuto = StringaRicezione[LenRicezione-1];
  if( CharBccCalcolato!=CharBccRicevuto ){
     sprintf( all_mess, "Calcolo BCC errato ricevuto=%d, calcolato=%d", CharBccRicevuto, CharBccCalcolato);
     return 1;
  }

  return err;
}



//---------------------------------------------
//  LaserTrsComando_SA()
//---------------------------------------------
// SA command (Activate Standby Mode)
//
int LaserTrsComando_SA( int NumeroTgv, char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;


  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 5;
  LenRicezione    = 5;
  TimeOut         = MsecTimeOutSA;

  StringaTrasmissione[ 0 ]=STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='S';
  StringaTrasmissione[ 3 ]='A';

  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);
  if( err ) { strcat( all_mess, "- SA_COMMAND" );  return err; }
  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     (  StringaTrasmissione[ 3 ] != StringaRicezione[ 3 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c risposta: %c%c",
           StringaTrasmissione[2],
           StringaTrasmissione[3],
           StringaRicezione[2],
           StringaRicezione[3] );
        err =1;
  }

  return err;
}


//---------------------------------------------
//  LaserTrsComando_SU()
//---------------------------------------------
// SU command (Enter direction of rotation)
// Serve per invertire la rotazione della TESTA LASER
// 0=Antioraria
// 1=Oraria (default)
//
int LaserTrsComando_SU( int NumeroTgv, int Rotation, char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;

  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 6;
  LenRicezione    = 6;
  TimeOut         = MsecTimeOutSA;

  StringaTrasmissione[ 0 ]=STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='S';
  StringaTrasmissione[ 3 ]='U';
  StringaTrasmissione[ 4 ]=(char) Rotation;

  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);
  if( err ) { strcat( all_mess, "- SU_COMMAND" );  return err; }
  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     (  StringaTrasmissione[ 3 ] != StringaRicezione[ 3 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c risposta: %c%c",
           StringaTrasmissione[2],
           StringaTrasmissione[3],
           StringaRicezione[2],
           StringaRicezione[3] );
        err =1;
  }

  return err;
}


//---------------------------------------------
//  LaserTrsComando_PA()
//---------------------------------------------
// PA command (Activate Position Mode)
//
int LaserTrsComando_PA( int NumeroTgv, char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;


  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 5;
  LenRicezione    = 5;
  TimeOut         = MsecTimeOutPA;

  StringaTrasmissione[ 0 ]=STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='P';
  StringaTrasmissione[ 3 ]='A';

  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);
  if( err ) { strcat( all_mess, "- PA_COMMAND" );  return err; }
  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     (  StringaTrasmissione[ 3 ] != StringaRicezione[ 3 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c risposta: %c%c",
           StringaTrasmissione[2],
           StringaTrasmissione[3],
           StringaRicezione[2],
           StringaRicezione[3] );

        err =1;
  }

  return err;
}


//---------------------------------------------
//  LaserTrsComando_PC()
//---------------------------------------------
// PC command (Slection of number of N nearest)
// Setta il numero minimo di specchi per la triangolazione
//
int LaserTrsComando_PC( int NumeroTgv, int N_Min, char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;

  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 6;
  LenRicezione    = 6;
  TimeOut         = MsecTimeOutPC;

  StringaTrasmissione[ 0 ]= STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='P';
  StringaTrasmissione[ 3 ]='C';
  StringaTrasmissione[ 4 ]=(char) N_Min;

  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);

  if( err ) {  strcat( all_mess, "- PC_COMMAND" );  return err; }

  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     (  StringaTrasmissione[ 3 ] != StringaRicezione[ 3 ] ) ||
     (  StringaTrasmissione[ 4 ] != StringaRicezione[ 4 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c%d risposta: %c%c%d",
           StringaTrasmissione[2],
           StringaTrasmissione[3],
           StringaTrasmissione[4],
           StringaRicezione[2],
           StringaRicezione[3],
           StringaRicezione[4] );
        err =1;
  }

  return err;
}


//---------------------------------------------
//  LaserTrsComando_PP()
//---------------------------------------------
// PP command (Request for position with automatic speed determination)
//
int LaserTrsComando_PP( int NumeroTgv, int* X, int* Y, int* A, int* G, int* N_Trovato, char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;
  WORD A_;
  short int  A__;

  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 5;
  LenRicezione    = 17;
  TimeOut         = MsecTimeOutPP;

  StringaTrasmissione[ 0 ]=STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='P';
  StringaTrasmissione[ 3 ]='P';

  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);
  if( err ) {  strcat( all_mess, "- PP_COMMAND" );  return err; }

  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     (  StringaTrasmissione[ 3 ] != StringaRicezione[ 3 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c risposta: %c%c",
           StringaTrasmissione[2],
           StringaTrasmissione[3],
           StringaRicezione[2],
           StringaRicezione[3] );
        err =1;
  }
  //----------------------------------------
  // Risposta corretta
  //----------------------------------------
  LaserConvertiStringaToInt ( X,   &StringaRicezione[  4], 4);
  LaserConvertiStringaToInt ( Y,   &StringaRicezione[  8], 4);
  LaserConvertiStringaToWord( &A_, &StringaRicezione[ 12], 2);
  A__       =A_;
  *A        =A__;
  *G        =StringaRicezione[ 14 ];
  *N_Trovato=StringaRicezione[ 15 ];

  return err;
}


//---------------------------------------------
//  LaserTrsComando_PP_Media()
//---------------------------------------------
// Esegue una media aritmetica su 10 letture del comando PP
// PP command (Request for position with automatic speed determination)
//
//
int LaserTrsComando_PP_Media( int NumeroTgv, int* X, int* Y, int* A, int* G, int* N_Trovato, char* all_mess)
{
//char buffer[301]="";
  char buffer1[301]="";
  int i, err, Nr;
  int NrLetture;
  int NrLettureValide;
  int X_,Y_,A_,G_;
  int Xm,Ym,Am,Gm;

  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  NrLettureValide=0;
  NrLetture=10;
  Xm=Ym=Am=Gm=0;
  err=0;
  //---------------------------------------------------------
  // Faccio la media su n letture
  //---------------------------------------------------------
  for( i=1; i<=NrLetture; i++ ){
     if(i==0){
        sprintf( buffer1, "PP Command Numero lettura %d           ", i);
        FormMappa->TextStato->Caption = buffer1;
     }
     err = LaserTrsComando_PP( NumeroTgv, &X_, &Y_, &A_, &G_, &Nr, all_mess);
     //-----------------------------------------------------
     // Se lettura andata a buon fine aggiungo al conteggio
     //-----------------------------------------------------
     if( err==0 ){
        //if( G_>80 ){ // %di precisione della lettura
        if( G_>G_MinimoMediaMap ){ // %di precisione della lettura
           NrLettureValide++;
           sprintf( buffer1, "PP Command X=%d, Y=%d, A=%d, G=%d, Nr=%d", X_, Y_, A_, G_, Nr);
           FormMappa->TextStato->Caption = buffer1;
           LaserProcessaWindowsMessage( 20 );
           delay( 200 ); // Per visualizzazione !!
           Xm+=X_;
           Ym+=Y_;
           Am+=A_;
           Gm+=G_;
        }
     }
     if( Nr<0 ) break;
     if( err  ) break;
  }
  if( NrLettureValide<1 ){
     sprintf( all_mess, "Nessuna lettura valida G>%d", G_MinimoMediaMap );
     err=1;
  }
  //----------------------------------------------
  // Faccio la media sulle letture valide
  //----------------------------------------------
  if( err==0 ){
     *X=Xm/NrLettureValide;
     *Y=Ym/NrLettureValide;
     *A=Am/NrLettureValide;
     *G=Gm/NrLettureValide;
  }

  return err;
}




//---------------------------------------------
//  LaserTrsComando_PL()
//---------------------------------------------
// PL command (Layer Selection)
// Setta il numero minimo di specchi per la triangolazione
//
int LaserTrsComando_PL( int NumeroTgv, int E, char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;

  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 6;
  LenRicezione    = 6;
  TimeOut         = MsecTimeOutPL;

  StringaTrasmissione[ 0 ]= STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='P';
  StringaTrasmissione[ 3 ]='L';
  StringaTrasmissione[ 4 ]=(char) E;

  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);

  if( err ) {  strcat( all_mess, "- PL_COMMAND" );  return err; }

  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     (  StringaTrasmissione[ 3 ] != StringaRicezione[ 3 ] ) ||
     (  StringaTrasmissione[ 4 ] != StringaRicezione[ 4 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c%d risposta: %c%c%d",
           StringaTrasmissione[2],
           StringaTrasmissione[3],
           StringaTrasmissione[4],
           StringaRicezione[2],
           StringaRicezione[3],
           StringaRicezione[4] );
        err =1;
  }

  return err;
}





//---------------------------------------------
//  LaserTrsComando_MA()
//---------------------------------------------
// MA command (Activate mapping mode)
//
int LaserTrsComando_MA( int NumeroTgv, char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;


  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 5;
  LenRicezione    = 5;
  TimeOut         = MsecTimeOutMA;

  StringaTrasmissione[ 0 ]=STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='M';
  StringaTrasmissione[ 3 ]='A';

  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);
  if( err ) {  strcat( all_mess, "- MA_COMMAND" );  return err; }
  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     (  StringaTrasmissione[ 3 ] != StringaRicezione[ 3 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c risposta: %c%c",
           StringaTrasmissione[2],
           StringaTrasmissione[3],
           StringaRicezione[2],
           StringaRicezione[3] );
        err =1;
  }

  return err;
}




//---------------------------------------------
//  LaserTrsComando_MS()
//---------------------------------------------
// MS command (Start Mapping)
//
int LaserTrsComando_MS( int NumeroTgv, int E, int X, int Y, int A, int R, int* N_Trovato, char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;
  int  X_, Y_;
  WORD A_;


  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 17;
  LenRicezione    = 7;
  TimeOut         = MsecTimeOutMS;

  X_ = X;
  Y_ = Y;
  A_ = (WORD) A;

  StringaTrasmissione[ 0 ]=STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='M';
  StringaTrasmissione[ 3 ]='S';
  StringaTrasmissione[ 4 ]=(char) E;  // Layer o Zona
  LaserConvertiIntToStringa ( &X_, &StringaTrasmissione[ 5], 4);
  LaserConvertiIntToStringa ( &Y_, &StringaTrasmissione[ 9], 4);
  LaserConvertiWordToStringa( &A_, &StringaTrasmissione[13], 2);
  StringaTrasmissione[15 ]=(char) R;  // Raggio dei rifelttori (fisso!!)

  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);
  if( err ) {  strcat( all_mess, "- MS_COMMAND" );  return err; }

  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     (  StringaTrasmissione[ 3 ] != StringaRicezione[ 3 ] ) ||
     (  StringaTrasmissione[ 4 ] != StringaRicezione[ 4 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c%d risposta: %c%c%d",
           StringaTrasmissione[2],
           StringaTrasmissione[3],
           StringaTrasmissione[4],
           StringaRicezione[2],
           StringaRicezione[3],
           StringaRicezione[4] );
        err =1;
  }
  //----------------------------------------
  // Risposta corretta
  //----------------------------------------
  *N_Trovato=StringaRicezione[ 5 ];

  return err;
}


//---------------------------------------------
//  LaserTrsComando_MM()
//---------------------------------------------
// MM command (Start Mapping with scan averaging) è come MS ma fa una media dei riflettori trovati
//
int LaserTrsComando_MM( int NumeroTgv, int E, int S, int X, int Y, int A, int R, int* N_Trovato, char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;
  int  X_, Y_;
  WORD A_;


  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 18;
  LenRicezione    = 7;
  TimeOut         = MsecTimeOutMM;

  X_ = X;
  Y_ = Y;
  A_ = (WORD) A;

  StringaTrasmissione[ 0 ]=STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='M';
  StringaTrasmissione[ 3 ]='M';
  StringaTrasmissione[ 4 ]=(char) E;  // Layer o Zona
  StringaTrasmissione[ 5 ]=(char) S;  // Numero Scansioni (per media)
  LaserConvertiIntToStringa ( &X_, &StringaTrasmissione[ 6], 4);
  LaserConvertiIntToStringa ( &Y_, &StringaTrasmissione[10], 4);
  LaserConvertiWordToStringa( &A_, &StringaTrasmissione[14], 2);
  StringaTrasmissione[16 ]=(char) R;  // Raggio dei rifelttori (fisso!!)

  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);
  if( err ) {  strcat( all_mess, "- MM_COMMAND" );  return err; }

  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     //(  StringaTrasmissione[ 3 ] != StringaRicezione[ 3 ] ) ||
     (                              StringaRicezione[ 3 ] != 'S' ) ||
     (  StringaTrasmissione[ 4 ] != StringaRicezione[ 4 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c%d risposta: %c%c%d",
           StringaTrasmissione[2],
           StringaTrasmissione[3],
           StringaTrasmissione[4],
           StringaRicezione[2],
           StringaRicezione[3],
           StringaRicezione[4] );
        err =1;
  }
  //----------------------------------------
  // Risposta corretta
  //----------------------------------------
  *N_Trovato=StringaRicezione[ 5 ];

  return err;
}


//---------------------------------------------
//  LaserTrsComando_MN()
//---------------------------------------------
// MN command (Start Negative Mapping with scan averaging) cerca solo i NUOVI riflettori in una determinata ZONA
//
int LaserTrsComando_MN( int NumeroTgv, int E, int S, int X, int Y, int A, int R, int* N_Trovato, char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;
  int  X_, Y_;
  WORD A_;


  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 18;
  LenRicezione    = 7;
  TimeOut         = MsecTimeOutMN;

  X_ = X;
  Y_ = Y;
  A_ = (WORD) A;

  StringaTrasmissione[ 0 ]=STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='M';
  StringaTrasmissione[ 3 ]='N';
  StringaTrasmissione[ 4 ]=(char) E;  // Layer o Zona
  StringaTrasmissione[ 5 ]=(char) S;  // Numero Scansioni (per media)
  LaserConvertiIntToStringa ( &X_, &StringaTrasmissione[ 6], 4);
  LaserConvertiIntToStringa ( &Y_, &StringaTrasmissione[10], 4);
  LaserConvertiWordToStringa( &A_, &StringaTrasmissione[14], 2);
  StringaTrasmissione[16 ]=(char) R;  // Raggio dei rifelttori (fisso!!)

  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);
  if( err ) {  strcat( all_mess, "- MN_COMMAND" );  return err; }

  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     //(  StringaTrasmissione[ 3 ] != StringaRicezione[ 3 ] ) ||
     (                              StringaRicezione[ 3 ] != 'S' ) ||
     (  StringaTrasmissione[ 4 ] != StringaRicezione[ 4 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c%d risposta: %c%c%d",
           StringaTrasmissione[2],
           StringaTrasmissione[3],
           StringaTrasmissione[4],
           StringaRicezione[2],
           StringaRicezione[3],
           StringaRicezione[4] );
        err =1;
  }
  //----------------------------------------
  // Risposta corretta
  //----------------------------------------
  *N_Trovato=StringaRicezione[ 5 ];

  return err;
}



//---------------------------------------------
//  LaserTrsComando_MR()
//---------------------------------------------
// MR command (Request for Mapping position)
//
int LaserTrsComando_MR( int NumeroTgv, int E, int N_Richesto, int* N_Trovato, int* X, int* Y,  char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;
  //WORD F1,F2,F3;


  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 7;
  LenRicezione    = 15;
  TimeOut         = MsecTimeOutMR;

  StringaTrasmissione[ 0 ]=STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='M';
  StringaTrasmissione[ 3 ]='R';
  StringaTrasmissione[ 4 ]=(char) E;           // Layer o Zona
  StringaTrasmissione[ 5 ]=(char) N_Richesto;  // Numero rflettore


  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);
  if( err ) {  strcat( all_mess, "- MR_COMMAND" );  return err; }

  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     (  StringaTrasmissione[ 3 ] != StringaRicezione[ 3 ] ) ||
     (  StringaTrasmissione[ 4 ] != StringaRicezione[ 4 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c(Zona%d)-Rifl%d risposta: %c%c(Zona%d)-Rifl%d",
           StringaTrasmissione[2],
           StringaTrasmissione[3],
           StringaTrasmissione[4],
           StringaTrasmissione[5],
           StringaRicezione[2],
           StringaRicezione[3],
           StringaRicezione[4],
           StringaRicezione[5] );
        if( StringaRicezione[3]=='E'){ // Messaggio di errore  es: MER..F1..F2..F3
           //LaserConvertiStringaToWord( &F1, &StringaRicezione[ 12], 2);
           sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c(Zona%d)-Rifl%d risposta: %c%c%c-F1=%d-F2=%d-F3=%d",
              StringaTrasmissione[2],
              StringaTrasmissione[3],
              StringaTrasmissione[4],
              StringaTrasmissione[5],
              StringaRicezione[2],
              StringaRicezione[3],
              StringaRicezione[4],
              StringaRicezione[5],
              StringaRicezione[6],
              StringaRicezione[7] );
        }
        err =1;
  }
  //----------------------------------------
  // Risposta corretta
  //----------------------------------------
  *N_Trovato=StringaRicezione[ 5 ];
  LaserConvertiStringaToInt( X, &StringaRicezione[ 6 ], 4);
  LaserConvertiStringaToInt( Y, &StringaRicezione[ 10], 4);

  return err;
}



//---------------------------------------------
//  LaserTrsComando_DA()
//---------------------------------------------
// DA command (Activate Download)
//
int LaserTrsComando_DA( int NumeroTgv, char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;


  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 5;
  LenRicezione    = 5;
  TimeOut         = MsecTimeOutDA;

  StringaTrasmissione[ 0 ]=STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='D';
  StringaTrasmissione[ 3 ]='A';

  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);
  if( err ) {  strcat( all_mess, "- DA_COMMAND" );  return err; }
  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     (  StringaTrasmissione[ 3 ] != StringaRicezione[ 3 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c risposta: %c%c",
           StringaTrasmissione[2],
           StringaTrasmissione[3],
           StringaRicezione[2],
           StringaRicezione[3] );
        err =1;
  }

  return err;
}



//---------------------------------------------
//  LaserTrsComando_DR()
//---------------------------------------------
// DR command ( Download Reflector position)
//
int LaserTrsComando_DR( int NumeroTgv, int E, int N_Richesto, int X, int Y, int* N_Trovato, char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;
  int  X_, Y_;


  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 15;
  LenRicezione    = 7;
  TimeOut         = MsecTimeOutDR;

  X_ = X;
  Y_ = Y;

  StringaTrasmissione[ 0 ]=STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='D';
  StringaTrasmissione[ 3 ]='R';
  StringaTrasmissione[ 4 ]=(char) E;           // Layer o Zona
  StringaTrasmissione[ 5 ]=(char) N_Richesto;  // Riflettore da salvare
  LaserConvertiIntToStringa ( &X_, &StringaTrasmissione[ 6], 4);
  LaserConvertiIntToStringa ( &Y_, &StringaTrasmissione[10], 4);

  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);
  if( err ) {  strcat( all_mess, "- DR_COMMAND" );  return err; }
  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     (  StringaTrasmissione[ 3 ] != StringaRicezione[ 3 ] ) ||
     (  StringaTrasmissione[ 4 ] != StringaRicezione[ 4 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c%d risposta: %c%c%d",
           StringaTrasmissione[2],
           StringaTrasmissione[3],
           StringaTrasmissione[4],
           StringaRicezione[2],
           StringaRicezione[3],
           StringaRicezione[4] );
        err =1;
  }
  //----------------------------------------
  // Risposta corretta
  //----------------------------------------
  *N_Trovato=StringaRicezione[ 5 ];

  return err;
}



//---------------------------------------------
//  LaserTrsComando_UA()
//---------------------------------------------
// UA command (Activate Upload)
//
int LaserTrsComando_UA( int NumeroTgv, char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;


  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 5;
  LenRicezione    = 5;
  TimeOut         = MsecTimeOutUA;

  StringaTrasmissione[ 0 ]=STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='U';
  StringaTrasmissione[ 3 ]='A';

  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);
  if( err ) {  strcat( all_mess, "- UA_COMMAND" );  return err; }
  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     (  StringaTrasmissione[ 3 ] != StringaRicezione[ 3 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c risposta: %c%c",
           StringaTrasmissione[2],
           StringaTrasmissione[3],
           StringaRicezione[2],
           StringaRicezione[3] );
        err =1;
  }

  return err;
}



//---------------------------------------------
//  LaserTrsComando_UR()
//---------------------------------------------
// MR command (Request Uppload trasmission)
//
int LaserTrsComando_UR( int NumeroTgv, int E, int* N_Trovato, int* X, int* Y,  char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;


  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 6;
  LenRicezione    = 15;
  TimeOut         = MsecTimeOutUR;

  StringaTrasmissione[ 0 ]=STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='U';
  StringaTrasmissione[ 3 ]='R';
  StringaTrasmissione[ 4 ]=(char) E;           // Layer o Zona

  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);
  if( err ) {  strcat( all_mess, "- UR_COMMAND" );  return err; }

  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     (  StringaTrasmissione[ 3 ] != StringaRicezione[ 3 ] ) ||
     (  StringaTrasmissione[ 4 ] != StringaRicezione[ 4 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c%d risposta: %c%c%d",
           StringaTrasmissione[2],
           StringaTrasmissione[3],
           StringaTrasmissione[4],
           StringaRicezione[2],
           StringaRicezione[3],
           StringaRicezione[4] );
        err =1;
  }
  //----------------------------------------
  // Risposta corretta
  //----------------------------------------
  *N_Trovato=StringaRicezione[ 5 ];
  LaserConvertiStringaToInt( X, &StringaRicezione[ 6 ], 4);
  LaserConvertiStringaToInt( Y, &StringaRicezione[ 10], 4);

  return err;
}

//---------------------------------------------
//  LaserTrsComando_RS()
//---------------------------------------------
// RS command ( Set Reflector radius )
//
int LaserTrsComando_RS( int NumeroTgv, int E, int R, char* all_mess)
{
  int  err=0;
  char StringaTrasmissione[ MAXSTRINGA+10 ];
  char StringaRicezione[ MAXSTRINGA+10 ];
  int  LenTrasmissione;
  int  LenRicezione;
  int  TimeOut;


  memset( StringaTrasmissione, 0, sizeof( StringaTrasmissione));
  memset( StringaRicezione,    0, sizeof( StringaRicezione));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]                ) return 0;
  if(AGV[NumeroTgv]->mission.EsclPLC) return 0;

  //-------------------------------------------------------
  // Prepara Dati trasmissione
  //-------------------------------------------------------
  LenTrasmissione = 7;
  LenRicezione    = 7;
  TimeOut         = MsecTimeOutRS;

  StringaTrasmissione[ 0 ]=STX;
  StringaTrasmissione[ 1 ]= (char) LenTrasmissione;
  StringaTrasmissione[ 2 ]='R';
  StringaTrasmissione[ 3 ]='S';
  StringaTrasmissione[ 4 ]=(char) E;           // Layer o Zona
  StringaTrasmissione[ 5 ]=(char) R;           // Radius del riflettore

  err = LaserTrsComandoGenerico( NumeroTgv, TimeOut, StringaTrasmissione, StringaRicezione,
        LenTrasmissione, LenRicezione, all_mess);
  if( err ) {  strcat( all_mess, "- RS_COMMAND" );  return err; }

  //----------------------------------------
  // Controllo intestazione risposta
  //----------------------------------------
  if((  StringaTrasmissione[ 2 ] != StringaRicezione[ 2 ] ) ||
     (  'G'                      != StringaRicezione[ 3 ] ) ||
     (  StringaTrasmissione[ 4 ] != StringaRicezione[ 4 ] ) ||
     (  StringaTrasmissione[ 5 ] != StringaRicezione[ 5 ] ) ){
        sprintf( all_mess, "Errore intestazione messaggio di risposta comando richiesto:%c%c%d%d risposta: %c%c%d%d",
           StringaTrasmissione[2],
           'G',
           StringaTrasmissione[4],
           StringaTrasmissione[5],
           StringaRicezione[2],
           StringaRicezione[3],
           StringaRicezione[4],
           StringaRicezione[5] );
        err =1;
  }

  return err;
}



//-------------------------------
//   ReadIniLaser()
//-------------------------------
// Lettura del file *.ini della struttura
//
int  ReadIniLaser(int NumeroTgv, int E, struct LAYER *Dati, char* all_mess)
{
  int i;
  int err =0;
  char buffer[101]="";
  char filename[101]="";
  TIniFile    *IniFile;
  AnsiString  Buffer;

  //-------------------------------------------------
  // Verifica esistenza percorso
  //-------------------------------------------------
  sprintf(filename, "LASER\\TGV%02d\\zona%02d.ini", NumeroTgv, E);
  if(access( filename, 0)){
     sprintf( all_mess, "File configurazione %s non trovato !", filename);
     err=1;
     MkDir("LASER");
     sprintf(buffer, "LASER\\TGV%02d", NumeroTgv);
     MkDir(buffer);
  }
  IniFile = new TIniFile(filename);
  //-------------------------------------------------
  // Lettura della struttura di MAPPING
  //-------------------------------------------------
  Dati->Xl   = IniFile->ReadInteger(LayerSection, LayerKeyXL,   0);
  Dati->Yl   = IniFile->ReadInteger(LayerSection, LayerKeyYL,   0);
  //Dati->Al   = IniFile->ReadInteger(LayerSection, LayerKeyAL,   0);
  Dati->Al   = IniFile->ReadInteger(LayerSection, LayerKeyAL,   0)*16384/90;
  Dati->Rl   = IniFile->ReadInteger(LayerSection, LayerKeyRL,   30);
  Dati->Minr = IniFile->ReadInteger(LayerSection, LayerKeyMinR, 3);
  Dati->Nr   = IniFile->ReadInteger(LayerSection, LayerKeyNR,   0);
  Dati->Rr   = IniFile->ReadInteger(LayerSection, LayerKeyRR,   50);
  for(i=0; i<MAXRIFLETTORI; i++){
     sprintf(buffer, "%s%02d", RifletSection, i);
     Dati->Xr[i] = IniFile->ReadInteger(buffer, RifletKeyXR, 0);
     Dati->Yr[i] = IniFile->ReadInteger(buffer, RifletKeyYR, 0);
  }
  return err;

}
//---------------------------------------------------------------------------


//-------------------------------
//   WriteIniLaser()
//-------------------------------
// Scrittura del file *.ini della struttura
//
void WriteIniLaser(int NumeroTgv, int E, struct LAYER *Dati)
{
  int i;
  char buffer[101]="";
  char filename[101]="";
  TIniFile    *IniFile;
  AnsiString  Buffer;


  //-------------------------------------------------
  // Verifica esistenza percorso
  //-------------------------------------------------
  sprintf(filename, "LASER\\TGV%02d\\zona%02d.ini", NumeroTgv, E);
  if(access( filename, 0)){
     MkDir("LASER");
     sprintf(buffer, "LASER\\TGV%02d", NumeroTgv);
     MkDir(buffer);
  }

  IniFile = new TIniFile(filename);
  //-------------------------------------------------
  // Scrittura della struttura di MAPPING
  //-------------------------------------------------
  IniFile->WriteInteger(LayerSection, LayerKeyXL,   Dati->Xl);
  IniFile->WriteInteger(LayerSection, LayerKeyYL,   Dati->Yl);
  //IniFile->WriteInteger(LayerSection, LayerKeyAL,   Dati->Al);
  IniFile->WriteInteger(LayerSection, LayerKeyAL,   Dati->Al*90/16384);
  IniFile->WriteInteger(LayerSection, LayerKeyRL,   Dati->Rl);
  IniFile->WriteInteger(LayerSection, LayerKeyMinR, Dati->Minr);
  IniFile->WriteInteger(LayerSection, LayerKeyNR,   Dati->Nr);
  IniFile->WriteInteger(LayerSection, LayerKeyRR,   Dati->Rr);

  for(i=0; i<MAXRIFLETTORI; i++){
     sprintf(buffer, "%s%02d", RifletSection, i);
     IniFile->WriteInteger(buffer, RifletKeyXR, Dati->Xr[i]);
     IniFile->WriteInteger(buffer, RifletKeyYR, Dati->Yr[i]);
  }
}
//---------------------------------------------------------------------------







//-------------------------------
//   LaserWriteTgvMappingTxt()
//-------------------------------
// Salvataggio su file di testo della struttura Mapping del TGV
//
int  LaserWriteTgvMappingTxt(int NumeroTgv, char* FileName, char* all_mess)
// ******************************************
{
  FILE   *ptrfile;
  //  char   FileName[301]="MAPPAxx.TXT";
  char Buffer[301]="";
  char Date[11]="";
  char Time[11]="";
  int  NumZona, NumRiflettori;

  OraData(Time, Date);
  //-----------------------------------------------------
  // Nome File
  //-----------------------------------------------------
  sprintf(FileName, "MAPPA%02d.TXT", NumeroTgv);
  //---------------------------------------
  // Crea il file di visualizzazione
  //---------------------------------------
  ptrfile = fopen(FileName, "wb");
  if (ptrfile==0) return 1;

  //---------------------------------------
  // Dati GENERALI
  //---------------------------------------
  Write_Line(ptrfile, "--------------------------------------------------------------------------------");
  sprintf( Buffer,    "    Mapping del TGV%d Data:%s",  NumeroTgv, Date ); Write_Line(ptrfile, Buffer );
  Write_Line(ptrfile, "--------------------------------------------------------------------------------");
  sprintf( Buffer, "Coordinate Zero Layout X         =%6d",   AGV[NumeroTgv]->zone_mapping.X_0_laser ); Write_Line(ptrfile, Buffer );
  sprintf( Buffer, "Coordinate Zero Layout Y         =%6d",   AGV[NumeroTgv]->zone_mapping.Y_0_laser ); Write_Line(ptrfile, Buffer );
  sprintf( Buffer, "Coordinate Zero Layout Grd       =%6.1f", AGV[NumeroTgv]->zone_mapping.GRD_laser ); Write_Line(ptrfile, Buffer );
  sprintf( Buffer, "Coordinate Zero Layout GrdOffset =%6.1f", AGV[NumeroTgv]->zone_mapping.GRD_laser ); Write_Line(ptrfile, Buffer );
  sprintf( Buffer, "Scostamento Punto Logico         =%6d",   AGV[NumeroTgv]->zone_mapping.ScostamentoPuntoLogico); Write_Line(ptrfile, Buffer );
  sprintf( Buffer, "Numero di Zone/Layer mappate     =%6d",   AGV[NumeroTgv]->zone_mapping.MaxZoneMappate ); Write_Line(ptrfile, Buffer );
  Write_Line(ptrfile, "");

  //---------------------------------------
  // Leggo tutti gli Step di programma
  //---------------------------------------
  for(NumZona=0; NumZona<AGV[NumeroTgv]->zone_mapping.MaxZoneMappate; NumZona++){
     Write_Line(ptrfile, "--------------------------------------------------------------------------------");
     sprintf( Buffer,    " Zona/Layer %d",  NumZona ); Write_Line(ptrfile, Buffer );
     Write_Line(ptrfile, "--------------------------------------------------------------------------------");
     sprintf( Buffer, "Coordinata X            =%7d",  AGV[NumeroTgv]->zone_mapping.Z[NumZona].Xl );       Write_Line(ptrfile, Buffer );
     sprintf( Buffer, "Coordinata Y            =%7d",  AGV[NumeroTgv]->zone_mapping.Z[NumZona].Yl );       Write_Line(ptrfile, Buffer );
     sprintf( Buffer, "Angolo (rad)            =%7d",  AGV[NumeroTgv]->zone_mapping.Z[NumZona].Al );       Write_Line(ptrfile, Buffer );
     sprintf( Buffer, "Raggio zona (mt)        =%7d",  AGV[NumeroTgv]->zone_mapping.Z[NumZona].Rl );       Write_Line(ptrfile, Buffer );
     sprintf( Buffer, "Zona Abilitata          =%7d",  AGV[NumeroTgv]->zone_mapping.Z[NumZona].ZAbiltata); Write_Line(ptrfile, Buffer );
     sprintf( Buffer, "Min Riflettori          =%7d",  AGV[NumeroTgv]->zone_mapping.Z[NumZona].Minr);      Write_Line(ptrfile, Buffer );
     sprintf( Buffer, "Raggio Riflettori (mm)  =%7d",  AGV[NumeroTgv]->zone_mapping.Z[NumZona].Rr);        Write_Line(ptrfile, Buffer );
     sprintf( Buffer, "Riflettori trovati      =%7d",  AGV[NumeroTgv]->zone_mapping.Z[NumZona].Nr );       Write_Line(ptrfile, Buffer );

     Write_Line(ptrfile, "");
     for(NumRiflettori=0; NumRiflettori<AGV[NumeroTgv]->zone_mapping.Z[NumZona].Nr; NumRiflettori++){
//     for(NumRiflettori=0; NumRiflettori<MAXRIFLETTORI; NumRiflettori++){
//if( AGV[NumeroTgv]->zone_mapping.Z[NumZona].Xr[NumRiflettori]==0) break;
        if( NumRiflettori>=MAXRIFLETTORI ) break;
        if( NumRiflettori==0 ){
           Write_Line(ptrfile, "   ------------------------");
           Write_Line(ptrfile, "    Nr.     X[mm]     Y[mm]");
           Write_Line(ptrfile, "   ------------------------");
        }
        if(NumRiflettori>=0 ){
           sprintf( Buffer,    "%7d   %7d   %7d",
              NumRiflettori,
              AGV[NumeroTgv]->zone_mapping.Z[NumZona].Xr[NumRiflettori],
              AGV[NumeroTgv]->zone_mapping.Z[NumZona].Yr[NumRiflettori]);
           Write_Line(ptrfile, Buffer );
        }
     }
     Write_Line(ptrfile, "");
  }
  fclose(ptrfile);
  return 0;
}
