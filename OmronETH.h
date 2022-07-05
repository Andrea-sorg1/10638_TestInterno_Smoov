//---------------------------------------------------------------------------
#ifndef OMRONETH.H
#define OMRONETH.H
//---------------------------------------------------------------------------
//#include <vcl\Classes.hpp>
//#include <vcl\Controls.hpp>
//#include <vcl\StdCtrls.hpp>
//#include <vcl\Forms.hpp>
//#include "WSocket.hpp"
//#include <NMUDP.hpp>
//#include <ScktComp.hpp>

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

#define BUFSIZE_OMRON             10000
#define MAXSTRINGA_OMRON          20000
#define MAX_DATA_WORD             10000
#define NR_WORD_INTESTAZIONE         10
#define BYTE_INTESTAZIONE_TRS        18
#define BYTE_INTESTAZIONE_RSV        14
#define MAXPLCCON                   100

#define OMRON_UDP_STANDARD_PORT    9600
#define LOCAL_UDP_PORT             2100

#define MSEC_TIMEOUT_OMRON          300

TIPO_VARIABILE AnsiString OmrRicevutoDaIP;
TIPO_VARIABILE AnsiString OmrLocalHost;
TIPO_VARIABILE WORD       NrPacchettoUDP;
TIPO_VARIABILE int        TimeOutSocketUDP;
TIPO_VARIABILE int        TimeOutSocketTCP;
TIPO_VARIABILE int        TimeOutHostLink;

TIPO_VARIABILE int        OmrPacchettoUdpPronti;
TIPO_VARIABILE int        OmrNumeroBytePronti;
TIPO_VARIABILE int        OmrNumeroBytesRicevuti;
TIPO_VARIABILE char       OmrPacchettoUdpRicevuto[MAXSTRINGA_OMRON+200];
TIPO_VARIABILE int        OmrTrasmissioneInCorso;

//----------------------------------------------------------------
// Da rendere Globali OmrEthTcp_  --> OmrEth_    in OMRONETH.H
//----------------------------------------------------------------
TIPO_VARIABILE WORD       OmrEth_ErrPlc[200];                           // --> Da rendere globale
TIPO_VARIABILE char       OmrEth_MessaggioAllarmePlc[MAXPLCCON+2][200]; // --> Da rendere globale
TIPO_VARIABILE WORD       OmrEth_DM_TxRx[MAXPLCCON+2][100];             // --> Da rendere globale
TIPO_VARIABILE bool       OmrEth_MultiPlcEsclusionePLC[MAXPLCCON+2];    // --> Da rendere globale
TIPO_VARIABILE bool       OmrEthTcp_CloseSocketTimeOut;                 // --> 0= ininfluente, altrimenti dopo N tentativi comunicazione
                                                                        //     senza risposta forza la chiusura del socket
//-----------------------------------------------------
// Connessione PLC
// Struttura che riepiloga alcuni parametri relativi
// ad ogni connessione PLC, la maggiore parte dei
// dati sono sempre uguali
//-----------------------------------------------------
struct ConnessioniPlc{
  AnsiString DescrPlc;             // Descrizione del Plc
  int        TypCom;               // Protocollo di comunicazione:
                                   // 0 - Host Link;
                                   // 1 - Fins Protocol;
  //
  // Header HOST LINK
  //
  unsigned char IndexPlc;          // Indice del Plc
  unsigned char Com;               // Seriale di comunicazione (Host Link)
  //
  // Header FINS COMMAND
  //
  AnsiString RemoteHost;           // Indirizzo IP del Plc remoto (Fins Command)
  int        RemotePort;           // Numero della Porta remota default = 9600 (Fins Command)
  int        LocalPort;            // Numero della Porta locale default = 9600 (Fins Command)
  unsigned char ICF;               // 0x80 Sempre per reponse (Fins Command)
  unsigned char RSV;               // Riservata (Fins Command)
  unsigned char GCT;               // Gateway Count: sempre a 02 (Fins Command)
  unsigned char DNA;               // Destination unit address (Fins Command)
  unsigned char DA1;               // Destination Node Address (Fins Command)
  unsigned char DA2;               // Destination Unit Address (Fins Command)
  unsigned char SNA;               // Source Unit Addrres (Fins Command)
  unsigned char SA1;               // Source Node Address (Fins Command)
  unsigned char SA2;               // Source Unit Address (Fins Command)
  unsigned char SID;               // Servic ID  -> Numero progressivo del pacchetto 00..FF (Fins Command)
};

//-----------------------------------------------------
// Parametrizzazione COM
// Struttura che riepiloga alcuni parametri relativi
// alle parametrizzazioni delle COM del Pc.
//-----------------------------------------------------
struct ParametriCom{
  AnsiString DescrCom;             // Descrizione della seriale
  int        Baudrate;             // Velocità di comunicazione (Host Link)
  char       Dati;                 // Numero dei Bit di dati (Host Link)
  char       Parita;               // Bit di parità (Host Link): E - Pari
                                   //                            O - Dispari
                                   //                            N - Nessuna
  char       StopBit;              // Bit di stop (Host Link).
  bool       NoCtsRts;             // DISATTIVA Gestione controllo CTS/RTS
};

//----------------------------------------------------------------------
// Strutture globali
//----------------------------------------------------------------------
TIPO_VARIABILE ConnessioniPlc OmrPlc[ MAXPLCCON +2 ];
TIPO_VARIABILE ParametriCom   InCom [ MAXCOM    +2 ];

//--------------------------------  o  ---------------------------------

//-----------------------------------------------------
// Elenco Funzioni
//-----------------------------------------------------
int OmrInitComunicazioni();
int OmrInitSeriale(WORD Port);
int OmrLeggiConfigurazionePLC();
int OmrSalvaConfigurazionePLC();
int OmrConvertiIntToStringa( WORD* ArrayWORD, char* ArrayCHAR, int LenCHAR);
int OmrConvertiStringaToInt( WORD* ArrayWORD, char* ArrayCHAR, int LenCHAR);
int OmrCalcolaBCC(char *stringa, int LenStringa, unsigned char *Bcc, char *all_mess);
int OmrTrasmettiRiceviPacchettoUDP1( WORD IndicePlc, char* PacchettoDaTrasmettere, char* PacchettoRicevuto, int BuffSize, int LenTrs, int ByteDaRicevere, int &ByteRicevuti, int AttesaRisposta, char* all_mess);
int OmrLeggiScriviCanale(WORD IndicePlc, WORD TipoVariabile, WORD TipoComando, WORD NrDatoIni, WORD NrDati, WORD* WordDati, WORD AttesaRisp, char* all_mess);

//---------------------------------------------------------------------------
//extern TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif;
