//---------------------------------------------------------------------------
#ifndef OMRONETHTCP.H
#define OMRONETHTCP.H
//---------------------------------------------------------------------------

#include "Libreria.h"
#include "UdpServer1.h"
//---------------------------------------------------------------------------
// Variabili Globali
//---------------------------------------------------------------------------
#ifndef MAINFILE                       // In caso non sia il MAINFILE
    #define TIPO_VARIABILE extern      // dichiararla come extern
#else
    #define TIPO_VARIABILE
#endif




//----------------------------------------------------------------------
// Strutture globali
//----------------------------------------------------------------------
TIPO_VARIABILE AnsiString OmrEthTcp_IndirizzoIpMittente;               // --> Uso interno OK
TIPO_VARIABILE int        OmrEthTcp_TrasmissioneInCorso;               // --> Uso interno OK
TIPO_VARIABILE int        OmrEthTcp_MultiPlcTrasmissioneInCorso;       // --> Uso interno OK
TIPO_VARIABILE WORD       OmrEthTcp_RicezioneOK[200];                  // --> Uso interno OK

TIPO_VARIABILE TWSocket *WSocketFins[MAXPLCCON+2];

TIPO_VARIABILE char       OmrEthTcp_ConnessioneSocketInCorso[MAXPLCCON+2];   // --> Uso interno OK
TIPO_VARIABILE bool       OmrEthTcp_MultiPlcReadWriteOk[MAXPLCCON+2];        // --> Uso interno OK
TIPO_VARIABILE int        OmrEthTcp_TentativiTrs;                            // --> Uso interno OK
TIPO_VARIABILE WORD       NrPacchettoFinsTCP;                                // --> Uso interno OK

TIPO_VARIABILE unsigned char    OmrEthTcp_ClientNodeSA1[MAXPLCCON+2];        // --> Uso interno OK
TIPO_VARIABILE unsigned char    OmrEthTcp_ServerNodeDA1[MAXPLCCON+2];        // --> Uso interno OK



//--------------------------------  o  ---------------------------------
//--------------------
//  classe TmsecA
//--------------------
class TmsecA{
    public:

  SYSTEMTIME  TimeInizio;
  SYSTEMTIME  TimeFine;

  bool        Scaduto;
  int         Preset;
  int         msec;

        TmsecA();
  int   Start( int MsecPreset );
  int   Reset( );
  int   TimeTrascorso( );
  bool  TestScaduto( );

};


//-----------------------------------------------------
// Elenco Funzioni
//-----------------------------------------------------
int OmrEthTcp_InitComunicazioni();
int OmrEthTcp_ConnessioneSocketIniziale( int IndicePlc, char* all_mess);

//int OmrEthTcp_InitSeriale(WORD Port);
int OmrEthTcp_ConvertiWordToStringa( WORD* ArrayWORD, char* ArrayCHAR, int LenCHAR);
int OmrEthTcp_ConvertiStringaToWord( WORD* ArrayWORD, char* ArrayCHAR, int LenCHAR);
int OmrEthTcp_ConvertiIntToStringa( int* ArrayInt,    char* ArrayCHAR, int LenCHAR);
int OmrEthTcp_ConvertiStringaToInt( int* ArrayInt,    char* ArrayCHAR, int LenCHAR);

int OmrEthTcp_TxPacchettoTCP( WORD IndicePlc,     char* PacchettoDaTrasmettere,    int LenTrs, char* all_mess);
int OmrEthTcp_RxPacchettoTCP( WORD IndicePlc,     char* PacchettoRicevuto,         int BuffSize, int ByteDaRicevere,  int* ByteRicevuti, int AttesaRisposta, char* all_mess) ;
int OmrEthTcp_TxRxPacchettoTCP( WORD IndicePlc,   char* PacchettoDaTrasmettere,  char* PacchettoRicevuto, int BuffSize, int LenTrs, int ByteDaRicevere,  int* ByteRicevuti, int AttesaRisposta, char* all_mess);

int OmrEthTcp_CompilaStringaFins(WORD IndicePlc, WORD TipoVariabile, WORD TipoComando, WORD NrDatoIni, WORD NrDati, WORD* WordDati, char* StringaFinsTx, int*  NumBytesDaTrasmettere, int*  NumBytesDaRicevere, WORD AttesaRisposta, char* all_mess);
int OmrEthTcp_AggiungiHeaderFinsTcp( WORD IndicePlc, WORD TipoComando, char* StringaTrasmissione, int* NumBytesDaTrasmettere, char* all_mess);
int OmrEthTcp_VerificaStringaHeaderFinsTcpRicevuta(WORD IndicePlc, char* StringaRicezione, int* NumBytesHederFinsTcp, int NumBytesRicevuti, char* all_mess);
int OmrEthTcp_VerificaStringaFinsRicevuta(WORD IndicePlc, WORD TipoVariabile, WORD TipoComando, WORD NrDatoIni, WORD NrDati, char* StringaTrasmissione, char* StringaRicezione, int NumBytesDaRicevere, int NumBytesRicevuti, char* all_mess);

int  OmrEthTcp_LeggiScriviCanale(WORD IndicePlc, WORD TipoVariabile, WORD TipoComando, WORD NrDatoIni, WORD NrDati, WORD* WordDati, WORD AttesaRisposta, char* all_mess);

int  OmrEthTcp_MultiPlcLeggiScriviCanale   ( WORD PlcIni, WORD NumPlc, WORD TipoVariabile, WORD TipoComando, WORD NrDatoIni, WORD NrDati, WORD AttesaRisposta, char *all_mess);
int  OmrEthTcp_MultiPlcLeggiScriviCanaleRip( WORD PlcIni, WORD NumPlc, WORD TipoVariabile, WORD TipoComando, WORD NrDatoIni, WORD NrDati, WORD AttesaRisposta, char *all_mess);

//---------------------------------------------------------------------------
#endif;
