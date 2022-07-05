//---------------------------------------------------------------------------
#ifndef ALGOSCAMBIO.H
#define ALGOSCAMBIO.H
//---------------------------------------------------------------------------
#include "Libreria.h"
#include "DBSeq.h"

//---------------------------------------------------------------------------
// Variabili Globali
//---------------------------------------------------------------------------
#ifndef MAINFILE                       // In caso non sia il MAINFILE
    #define TIPO_VARIABILE extern      // dichiararla come extern
#else
    #define TIPO_VARIABILE
#endif

#define MAX_AREA_DM_PLCTERRA        19000    // Old erano MAX_AREA_DM_PLCTERRA        20000
#define VERSIONE_DATI                   2    // Versione struttura scambio dati
#define MAX_PLC_COLLEGATI             100


//----------------------------------------------------------------------
// Strutture globali
//----------------------------------------------------------------------
struct AreaScambioPlc{
  int                 VersioneDati;   // Versione Struttura scambio DATI
  unsigned short int  DM     [MAX_AREA_DM_PLCTERRA+20];          // Tabella area DM PLCTERRA
  unsigned char       DM_Mod [MAX_AREA_DM_PLCTERRA+20];          // Tabella dei Modificati da Supervisore

  unsigned short int  AuxWord1[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned short int  AuxWord2[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned short int  AuxWord3[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned short int  AuxWord4[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned short int  AuxWord5[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned short int  AuxWord6[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned short int  AuxWord7[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned short int  AuxWord8[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned short int  AuxWord9[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria

  unsigned char       AuxChar1[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned char       AuxChar2[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned char       AuxChar3[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned char       AuxChar4[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned char       AuxChar5[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned char       AuxChar6[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned char       AuxChar7[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned char       AuxChar8[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria
  unsigned char       AuxChar9[MAX_PLC_COLLEGATI+10];            // Informazione ausiliaria

  unsigned char       AuxCharSingolo1;                           // Informazione ausiliaria
  unsigned char       AuxCharSingolo2;                           // Informazione ausiliaria
  unsigned char       AuxCharSingolo3;                           // Informazione ausiliaria
  unsigned char       AuxCharSingolo4;                           // Informazione ausiliaria
  unsigned char       AuxCharSingolo5;                           // Informazione ausiliaria
  unsigned char       AuxCharSingolo6;                           // Informazione ausiliaria
  unsigned char       AuxCharSingolo7;                           // Informazione ausiliaria
  unsigned char       AuxCharSingolo8;                           // Informazione ausiliaria
  unsigned char       AuxCharSingolo9;                           // Informazione ausiliaria
  unsigned char       AuxCharSingolo10;                          // Informazione ausiliaria

  long                AuxLongSingolo1;                           // Informazione ausiliaria
  long                AuxLongSingolo2;                           // Informazione ausiliaria
  long                AuxLongSingolo3;                           // Informazione ausiliaria
  long                AuxLongSingolo4;                           // Informazione ausiliaria
  long                AuxLongSingolo5;                           // Informazione ausiliaria

  bool                ComunicazioneOk[MAX_PLC_COLLEGATI+10];     // Array di comunnicazioni OK per tutti i PLC
  bool                MacchinaDiCarico[MAX_PLC_COLLEGATI+10];    // Array dove il SUPERVISORE indica se macchina di carico

  int                 DatiAttendibili;// Dati attendibili da PLCTERRA
  int                 Progressivo;    // Il SUPERVISORE Incrementa e Il SERVER fa ECHO
  char                Messaggio[200]; // Eventuale messaggio di scambio
  int                 TotMacchine;    // Il Supervisore informa il SERVER del numero di macchine presenti
  int                 TotAgv;         // Il Supervisore informa il SERVER del numero di AGV
  int                 Configurazione; // Il Supervisore informa il SERVER della configuarzione e tipo di comunicazione dell'impianto
                                      // 1 - Emulazione area DM BOX2005 e PLCTERRA reale con DM BOX2005 e controller link
                                      // 2 - Emulazione area DM BOX2005 + comunicazione diretta con macchine   BOX2005
                                      // 3 - Emulazione area DM BOX2005 + PLCTERRA reale con DM BOX2005 e controller link
                                      // 4 - Emulazione area DM BOX2006 + PLCTERRA reale con DM BOX2006 e controller link
                                      // 5 -
  int                 Aux1;           // aux
  int                 Aux2;           // aux
  int                 Aux3;           // aux
  int                 Aux4;           // aux
  int                 Aux5;           // aux
  int                 Aux6;           // aux
  int                 Aux7;           // aux
  int                 Aux8;           // aux
  int                 Aux9;           // aux
  int                 Aux10;          // aux
};



TIPO_VARIABILE struct              AreaScambioPlc Scambio_Area;
TIPO_VARIABILE struct              AreaScambioPlc Scambio_AreaRD;
TIPO_VARIABILE struct              AreaScambioPlc Scambio_AreaWD;
TIPO_VARIABILE char                Scambio_PercorsoScambio[300];
TIPO_VARIABILE char                Scambio_FileNameAreaDM_RD[300];
TIPO_VARIABILE char                Scambio_FileNameAreaDM_WD[300];
TIPO_VARIABILE char                Scambio_FileNameServerIni[300];


#define MAX_TAG                        50    // Numero MAX di TAG previste
#define MAX_CAMPI_TAG                  10    // Numero MAX di CAMPI TAG previsti

struct AreaTag{
  int                 TipoComando;    // Tipo comando: 1-SOLO Lettura       (SEMPRE SOLO LETTURA   )
                                      //               2-SOLO Scrittura     (SEMPRE SOLO SCRITTURA )
                                      //               3-Lettura/Scrittura  (Lettura normalmente e scrittura se forzato da scrivere )
  int                 DMIni;          // DM Iniziale sul PLC Virtuale
  int                 NumDM;          // Numero di DM da copiare
  int                 IndicePlcReale; // Indice del PLC reale sorgente/destinazione dei dati
  int                 DMIniPlcReale;  // DM Iniziale sul PLC Reale
  int                 Aux6;           // aux
  int                 Aux7;           // aux
  int                 Aux8;           // aux
  int                 Aux9;           // aux
  int                 Aux10;          // aux
};

struct AreaScambioTag{
  struct AreaTag T[ MAX_TAG +10 ];
};

TIPO_VARIABILE struct              AreaScambioTag Scambio_Tag;
TIPO_VARIABILE char                Scambio_FileNameTag[300];
TIPO_VARIABILE char                Scambio_FileNameTagBak[300];


//--------------------------------  o  ---------------------------------

//-----------------------------------------------------
// Elenco Funzioni
//-----------------------------------------------------
void Scambio_Inizializza();
int  Scambio_LoadFileDM_RD(char *all_mess);
int  Scambio_SaveFileDM_RD(char *all_mess);
int  Scambio_LoadFileDM_WD(char *all_mess);
int  Scambio_SaveFileDM_WD(char *all_mess);
int  Scambio_SV_LetturaInizioProgramma(char *all_mess);
int  Scambio_SV_LetturaInizioCiclo(char *all_mess);
WORD Scambio_ld_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);
WORD Scambio_wr_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);
int  Scambio_SV_ScritturaFineCiclo(char *all_mess);

// Gestione TAG
void Scambio_InizializzaTag();
void Scambio_InizializzaTabellaTagCsv(char *path, DBSeq* DBSeqTabella);
void Scambio_SetCampiTabellaTagCSV(int IndiceTag, struct AreaTag *Record, DBSeq *Tabella, char* all_mess);
void Scambio_GetCampiTabellaTagCSV(int IndiceTag, struct AreaTag *Record, DBSeq *Tabella, char* all_mess);
int  Scambio_WriteTabellaTagCSV( char* all_mess);
int  Scambio_LoadTabellaTagCSV( char* all_mess);


//---------------------------------------------------------------------------
//  ALGO SCAMBIO DDE
//---------------------------------------------------------------------------

#define MAX_CANALI_SCAMBIO_DDE 10000

struct DatiScambioDdeGeneraliR{
  int                 VersioneDati;   // Versione Struttura scambio DATI
  bool                ComunicazioneOk[MAX_PLC_COLLEGATI+10];     // Array di comunnicazioni OK per tutti i PLC

  int                 DatiAttendibili;// Dati attendibili da PLCTERRA
  int                 Progressivo;    // Il SUPERVISORE Incrementa e Il SERVER fa ECHO
  char                Messaggio[200]; // Eventuale messaggio di scambio
};

struct DatiScambioDdeGeneraliW{
  bool                MacchinaDiCarico[MAX_PLC_COLLEGATI+10];    // Array dove il SUPERVISORE indica se macchina di carico
  int                 Progressivo;    // Il SUPERVISORE Incrementa e Il SERVER fa ECHO
  char                Messaggio[200]; // Eventuale messaggio di scambio
  int                 TotMacchine;    // Il Supervisore informa il SERVER del numero di macchine presenti
  int                 TotAgv;         // Il Supervisore informa il SERVER del numero di AGV
  int                 Configurazione; // Il Supervisore informa il SERVER della configuarzione e tipo di comunicazione dell'impianto
};


struct DatiScambioDdeCanali{
  int   err;                             // Eventuale errore restituito
  char  Messaggio[100];                  // Evdentuale messaggio di errore restituito
  WORD  Dm[MAX_CANALI_SCAMBIO_DDE+10];    // Array di WORD letti o scritti
};

extern DatiScambioDdeCanali DdeCanali;
extern int AlgoScambioModalitaDde;
extern int AlgoScambioTrsDdeInCorso;


int  ScambioDdeSV_LetturaDatiGenerali( char* all_mess);
int  ScambioDdeSV_ScritturaDatiGenerali( char* all_mess);
WORD ScambioDdeSV_ld_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);
WORD ScambioDdeSV_wr_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);



#endif;
