// ------------------------------------------------------------------------
//                file       ModBus.H
// ------------------------------------------------------------------------
#ifndef Modbus.H
#define Modbus.H

//#include "Variabil.h"
//#include "DBBox_TLB.h"

#ifndef MAINFILE
    #define TIPO_VARIABILE extern
#else
    #define TIPO_VARIABILE
#endif

#define ABILITA_CHK_BIBERONAGGIO




// --------------------------
//  Elenco Variabili GLOBALI
// --------------------------
/*
TIPO_VARIABILE  short int COMPlcTgvBeT;                          // Porta seriale per la comunicazione con i TGV
*/
// --------------------------
//  Elenco Funzioni GLOBALI
// --------------------------
unsigned short MODBUS_CalcolaCRC ( unsigned char *puchMsg, unsigned short usDataLen, unsigned char* CRC_CalcolatoHi, unsigned char* CRC_CalcolatoLo );
int            MODBUS_InitPort       ( short int Port, char* all_mess );
int            MODBUS_Leggi_Registri ( short int Port, short int NumPlc, short int IndIni, short int NumRegistri, unsigned short int* Registri, char* all_mess );
int            MODBUS_Scrivi_Registri( short int Port, short int NumPlc, short int IndIni, short int NumRegistri, unsigned short int* Registri, char* all_mess );
int            MODBUS_Leggi_Registri_Tcp(short int NumPlc, short int IndIni, short int NumRegistri, unsigned short int* Registri, char* all_mess );
int            MODBUS_Scrivi_Registri_Tcp(short int NumPlc, short int IndIni, short int NumRegistri, unsigned short int* Registri, char* all_mess );
int            MODBUS_TrasmettiRiceviPacchettoTCP( WORD IndicePlc, char* PacchettoDaTrasmettere, char* PacchettoRicevuto, int BuffSize, int LenTrs, int ByteDaRicevere,  int &ByteRicevuti, int AttesaRisposta, char* all_mess);
int            MODBUS_RD_Registri ( short int Port, short int NumPlc, short int IndIni, short int NumRegistri, unsigned short int* Registri, char* all_mess );
int            MODBUS_WR_Registri( short int Port, short int NumPlc, short int IndIni, short int NumRegistri, unsigned short int* Registri, char* all_mess );
int            MODBUS_RD_Registri_Tcp(short int NumPlc, short int IndIni, short int NumRegistri, unsigned short int* Registri, char* all_mess );
int            MODBUS_WR_Registri_Tcp(short int NumPlc, short int IndIni, short int NumRegistri, unsigned short int* Registri, char* all_mess );

#endif






