// ------------------------------------------------------------------------
//                file       AllenBViaVbOpc.H
// ------------------------------------------------------------------------
#ifndef AlleBViaVbOpc.H
#define AlleBViaVbOpc.H

#ifndef MAINFILE
    #define TIPO_VARIABILE extern
#else
    #define TIPO_VARIABILE
#endif

#define MAX_LEN_RECORD        500
#define MAX_WORD_PLC          100
#define MAX_PLC_AB             25
//#define TOT_PLC_AB             25
#define TOT_READ_WORD          50
#define TOT_WRITE_WORD         10

// --------------------------
//  Elenco strutture
// --------------------------
struct DataRecordR{
  WORD DatiOk;
  bool DatiCambiati;

  char Item[30];
  WORD DatoIni;
  WORD NumDati;
  WORD NumDatiLetti;
  WORD Data[MAX_WORD_PLC+3];
  WORD OldData[MAX_WORD_PLC+3];

};

struct DataRecordW{
  bool DatiCambiati;

  char Item[30];
  WORD DatoIni;
  WORD NumDati;
  WORD Data[MAX_WORD_PLC+3];
  WORD OldData[MAX_WORD_PLC+3];
};


struct DatiPlc{
  int   TipoComunicazione; // 1=OPC Massimo
                           // 2=OPC Lucio
                           // 3=TCP Diretto
//  int   TotPlc;
  char  Topic[30];
  int   Indice;
  char  IpAddr[20];        // Indirizzo IP
  int   Port;              // Port
  WORD  CtrErroriConsecutivi;
  bool  ErrorePersistente;
  bool  DisableComunicazione;

  struct DataRecordR  R;
  struct DataRecordW  W;
};

// --------------------------
//  Elenco Variabili GLOBALI
// --------------------------
TIPO_VARIABILE AnsiString    PathGateway;
TIPO_VARIABILE short int     TypComunication;
TIPO_VARIABILE char          PercorsoAllenB[300];
TIPO_VARIABILE DatiPlc       DatiPLC[ MAX_PLC_AB+10 ];
TIPO_VARIABILE char          PtrStrCampo[MAX_WORD_PLC+3][20];
TIPO_VARIABILE int           AllenbPlcIni;
TIPO_VARIABILE int           AllenbTotPlc;
TIPO_VARIABILE int           AllenbCtrTxRx;
TIPO_VARIABILE int           CicloPlcAllen;


// --------------------------
//  Elenco Funzioni GLOBALI
// --------------------------
void AllenB_Percorso( );
int  AllenB_Inizializza   ( char* all_mess );
int  AllenB_InizializzaFileConfig( char* all_mess );
void AllenB_ReadIniProject();
void AllenB_WriteIniProject();

int  AllenB_InizializzaAreaPlc   ( char* all_mess );
int  AllenB_LetturaPlc   ( char* all_mess );
int  AllenB_SctritturaPlc( char* all_mess );
int  AllenB_FileDoc_Rx   ( char* NomeFile );
int  AllenB_FileDoc_Tx   ( char* NomeFile );

// Funzioni per interfaccia VB6 Vers 1 Massimo
int AllenB_MAS_InitRead_ScritturaFile( char* all_mess );
int AllenB_MAS_InitWrite_ScritturaFile( char* all_mess );
int AllenB_MAS_DataRead_LetturaFile( char* all_mess );
int AllenB_MAS_DataWrite_ScritturaFile( char* all_mess );

// Funzioni per interfaccia VB6 Vers 2 Lucio
int AllenB_LUC_CompilazioneFileTagCFG( char* all_mess );
int AllenB_LUC_CompilazioneFileOpcCFG( char* all_mess );
int AllenB_LUC_InitRead_ScritturaFile( char* all_mess );
int AllenB_LUC_DataRead_LetturaFile  ( char* all_mess );
int AllenB_LUC_DataWrite_ScritturaFile( char* all_mess );

// Funzioni SLC500 TCP
int AllenB_TcpSLC_DataRead( char* all_mess );
int AllenB_TcpSLC_DataWrite( char* all_mess );

#endif






