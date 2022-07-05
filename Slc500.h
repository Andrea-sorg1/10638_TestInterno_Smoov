//---------------------------------------------------------------------------
#ifndef Slc500H
#define Slc500H
#include "Libreria.h"
//#include "UdpServer1.h"
//---------------------------------------------------------------------------
// Variabili Globali
//---------------------------------------------------------------------------
#ifndef MAINFILE                       // In caso non sia il MAINFILE
    #define TIPO_VARIABILE extern      // dichiararla come extern
#else
    #define TIPO_VARIABILE
#endif

#define SLC500_MAX_PLC          100
#define SLC500_MAX_WORD_MULTI   100

//-------------------------------------  o  --------------------------------------------

//-----------------------------
// Header cspv4 (28 byte)
//-----------------------------
struct cspv4_header{
  unsigned char        mode;          // ( 1 byte) 01 = request, 02 = response
  unsigned char        submode;       // ( 1 byte) 01 = connection, 07 = PCCC?? (not sure the name)
  unsigned short int   data_lenght;   // ( 2 byte) how many bytes AFTER this header
  unsigned int         conn_id;       // ( 4 byte) ID assigned by slave/server
  unsigned int         status;        // ( 4 byte) status/err; 0=good, otherwise ???
  unsigned char        context[16];   // (16 byte) client info - slave/server just echos
};

//-----------------------------
// Header LSAP (4 byte)
//-----------------------------
struct local_lsap{
  unsigned char        dst;        // ( 1 byte) Destination byte - like slve address
  unsigned char        res5;       // ( 1 byte) Control byte = seems fixed as 5 (not sure what it does)
  unsigned char        src;        // ( 1 byte) Source byte - like master address
  unsigned char        lsap;       // ( 1 byte) LSAP point = fixeed (0)
};



struct Slc500_Plc{
/*
  TIPO_VARIABILE WORD       OmrEth_ErrPlc[200];                       // --> Da rendere globale
  TIPO_VARIABILE char       OmrEth_MessaggioAllarmePlc[110][200];     // --> Da rendere globale
  TIPO_VARIABILE WORD       OmrEth_DM_TxRx[110][100];                 // --> Da rendere globale
  TIPO_VARIABILE bool       OmrEth_MultiPlcEsclusionePLC[110];        // --> Da rendere globale
*/
  int   TrasmissioneInCorso;       // --> Uso interno OK
  char  ConnessioneSocketInCorso;  // --> Uso interno OK
  int   ErrPlc;
  bool  EsclusionePlc;
  bool  MultiRxPlcOk;

  WORD  Multi_W_TxRx[SLC500_MAX_WORD_MULTI];            // Array di word per leggere e scrivere in MultiPLC
  WORD  Multi_NumFile;                                  // Numero del File es: 7 (N7)
  WORD  Multi_Data_Ini;                                 // Numero Word Iniziale
  WORD  Multi_Data_Len;                                 // Numero di Word da leggere Scrivere

  AnsiString RemoteHost;           // Indirizzo IP del Plc remoto
  int        RemotePort;           // Numero della Porta remota
  TWSocket   *WSocket;
  struct cspv4_header HeaderCspv4Tx;
  struct cspv4_header HeaderCspv4Rx;
};


//----------------------------------------------------------------------
// Strutture globali
//----------------------------------------------------------------------
TIPO_VARIABILE struct Slc500_Plc SlcPlc[SLC500_MAX_PLC+3];
TIPO_VARIABILE int    Slc500_PlcIni;
TIPO_VARIABILE int    Slc500_TotPlc;


//-----------------------------------------------------
// Elenco Funzioni
//-----------------------------------------------------
int Slc500_ConvertiWORDToStringa( WORD* ArrayWORD, unsigned char* ArrayCHAR, int num_canali);
int Slc500_ConvertiStringaToWORD( WORD* ArrayWORD, unsigned char* ArrayCHAR, int num_canali);

int Slc500_TrasmettiRiceviPacchettoTCP( WORD Plc, char* PacchettoDaTrasmettere, char* PacchettoRicevuto, int BuffSize, int LenTrs, int ByteDaRicevere,  int &ByteRicevuti, int AttesaRisposta, char* all_mess);
int Slc500_TxPacchettoTCP( WORD Plc, char* PacchettoDaTrasmettere, int LenTrs, char* all_mess);
int Slc500_RxPacchettoTCP( WORD Plc, char* PacchettoRicevuto, int BuffSize, int ByteDaRicevere,  int &ByteRicevuti, int AttesaRisposta, char* all_mess);


int Slc500_InitComunicazioni( );
int Slc500_SocketConnection( int Plc, char* all_mess);
int Slc500_PcccConnection( int Plc, char* all_mess);
int Slc500_PcccLeggiWord ( int Plc, int NumFile, int WordIni, int NumWord, WORD Data[],char* all_mess);
int Slc500_PcccScriviWord( int Plc, int NumFile, int WordIni, int NumWord, WORD Data[],char* all_mess);

int Slc500_SocketMultiConnection( char* all_mess);
int Slc500_PcccMultiConnection  ( char* all_mess);
int Slc500_PcccMultiLeggiWord   (char* all_mess );
int Slc500_PcccMultiScriviWord  (char* all_mess );

//---------------------------------------------------------------------------
//extern TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif;
