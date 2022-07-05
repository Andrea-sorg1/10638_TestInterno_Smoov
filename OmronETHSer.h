//---------------------------------------------------------------------------
#ifndef OMRONETHSER.H
#define OMRONETHSER.H
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


#define ETHSER_MAX_DM                      25
#define ETHSER_BYTE_INTESTAZIONE_TRS        5
#define ETHSER_BYTE_INTESTAZIONE_RSV        7
#define ETHSER_MAX_ERRORICONSECUTIVI       30


//----------------------------------------------------------------------
// Strutture globali
//----------------------------------------------------------------------
TIPO_VARIABILE            TWSocket *WSocket[MAXPLCCON+2];
TIPO_VARIABILE int        OmrEthSer_TrasmissioneInCorso;
TIPO_VARIABILE int        OmrEthSer_MultiPlcTrasmissioneInCorso;
TIPO_VARIABILE char       OmrEthSer_ConnessioneSocketInCorso[MAXPLCCON+2];
TIPO_VARIABILE bool       OmrEthSer_MultiPlcReadWriteOk[MAXPLCCON+2];
TIPO_VARIABILE int        OmrEthSer_TentativiTrs;
TIPO_VARIABILE int        OmrEthSer_ErroriConsecutivi[200];

//--------------------------------  o  ---------------------------------

//-----------------------------------------------------
// Elenco Funzioni
//-----------------------------------------------------
int OmrEthSer_InitComunicazioni();
int OmrEthSer_InitSeriale(WORD Port);
int OmrEthSer_ConvertiIntToStringa( WORD* ArrayWORD, char* ArrayCHAR, int num_canali);
int OmrEthSer_ConvertiStringaToInt( WORD* ArrayWORD, char* ArrayCHAR, int num_canali);
int OmrEthSer_CalcolaBCC(char *stringa, int LenStringa, unsigned char *Bcc, char *all_mess);

int OmrEthSer_TrasmettiRiceviPacchettoTCP( WORD IndicePlc, char* PacchettoDaTrasmettere, char* PacchettoRicevuto, int BuffSize, int LenTrs, int ByteDaRicevere,  int &ByteRicevuti, int AttesaRisposta, char* all_mess);
int OmrEthSer_TrasmettiPacchettoTCP( WORD IndicePlc, char* PacchettoDaTrasmettere, int BuffSize, int LenTrs,  char* all_mess);
int OmrEthSer_RiceviPacchettoTCP( WORD IndicePlc, char* PacchettoRicevuto, int BuffSize, int ByteDaRicevere,  int &ByteRicevuti, char* all_mess);
int OmrEthSer_RiceviPacchettoTCP_SenzaAttesa( WORD IndicePlc, char* PacchettoRicevuto, int BuffSize, int ByteDaRicevere,  int &ByteRicevuti, char* all_mess);

WORD OmrEthSer_Modo( WORD plc, WORD modo, char *all_mess);
WORD OmrEthSer_LoadCanale( WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);
WORD OmrEthSer_WriteCanale( WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);
WORD OmrEthSer_Load_TIM_CNT( WORD *canale, WORD plc, char *cmd, char *tipo, WORD numero, char *all_mess);
WORD OmrEthSer_Write_TIM_CNT( WORD *canale, WORD plc, char *cmd, char *tipo, WORD numero, char *all_mess);



WORD OmrEthSer_MultiPlcLoadCanale ( WORD PlcIni, WORD NumPlc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);
WORD OmrEthSer_MultiPlcWriteCanale( WORD PlcIni, WORD NumPlc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);

WORD OmrEthSer_MultiPlcLoadCanaleRip ( WORD PlcIni, WORD NumPlc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);
WORD OmrEthSer_MultiPlcWriteCanaleRip( WORD PlcIni, WORD NumPlc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);

//---------------------------------------------------------------------------
#endif;
