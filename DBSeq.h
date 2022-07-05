// ------------------------------------------------------------------------
//             file          DBSEQ.H
// ------------------------------------------------------------------------
#ifndef DBSEQH
#define DBSEQH
//---------------------------------------------------------------------------
#include <stdio.h>
#include <windows.h>

// --------------------------------
//      Variabili Globali
// --------------------------------
#ifndef MAINFILE                       // In caso non sia il MAINFILE
    #define TIPO_VARIABILE extern      // dichiararla come extern
#else
    #define TIPO_VARIABILE
#endif

#define DBSEQ_CONFR_MINORE      0
#define DBSEQ_CONFR_MIN_UGUALE  1
#define DBSEQ_CONFR_UGUALE      2
#define DBSEQ_CONFR_MAG_UGUALE  3
#define DBSEQ_CONFR_MAGGIORE    4

#define DBSEQ_VAR_STRINGA      0
#define DBSEQ_VAR_INTEGER      1

#define DBSEQ_TIPO_FILE_STANDARD   0
//#define DBSEQ_TIPO_FILE_IMBOTTITO  1

#define DBSEQ_MAX_LEN_CAMPO        90
#define DBSEQ_MAX_CAMPI            50



//--------------------------------  o  ---------------------------------
//--------------------
//  classe Tmsec
//--------------------
class Tmsec{
    public:

  SYSTEMTIME  TimeInizio;
  SYSTEMTIME  TimeFine;

  bool        Scaduto;
  int         Preset;
  int         msec;

        Tmsec();
  int   Start( int MsecPreset );
  int   Reset( );
  int   TimeTrascorso( );
  bool  TestScaduto( );

};

//--------------------
//  classe Tmsec
//--------------------
class Tsec{
    public:

  long  TimeInizio;
  long  TimeFine;

  bool        Scaduto;
  int         Preset;
  int         sec;

        Tsec();
  int   Start( int SecPreset );
  int   Reset( );
  int   TimeTrascorso( );
  bool  TestScaduto( );

};

//--------------------------------  o  ---------------------------------


//--------------------
//  classe DBASESEQ
//--------------------
class DBSeq{
    public:

     char PercorsoDBase [200];   // Viene automaticamente costruito il percorso completo del file
     char PercorsoLavoro[200];   // Percorso di lavoro del programma
     char SottoDirectory[100];   // Default: Nessuna, eventuale sottodirectory dove si trova il File
     char NomeDBase     [100];   // Nome del file (senza il percorso e l'estensione)
     char Estensione    [ 20];   // Default: .CSV,  estensione del file alternativa

     int  TipoFile;              // DBSEQ_TIPO_FILE_STANDARD  0 = File sequenziale standard
                                 // DBSEQ_TIPO_FILE_.....     1 = File sequenziale con record imbottito
     char CarattereSeparatore;   // Default: '|'
                                 // In un file .CSV la discriminzazione tra i vari campi
                                 // è rappresentato da un carattere separatore che può
                                 // assumere qualsiasi valore ',' ';' ecc..
     int  LenRecordImbottito;    // Default: 0
                                 // se impostato è da calcolare oltre la dimensione
                                 // ragionevole del record in modo tale da ottenere una dimensione fissa
                                 // ( con molte implementazioni) gestendo in maniera elastica  i vari campi
     int  MaxLenRecord;          // Default=500

     char *PtrStrCampo[500];     // Array di puntatori a stringhe di campi
                                 // Deafult -> puntano direttamente alle stringhe StrCampi che dovrebbero essere
                                 // adeguate per la maggior parte delle applicazioni in caso contrario
                                 // E' NECESSARIO utilizzare un'array esterno alla classe dimensionato opportunamente

     char StrCampi[DBSEQ_MAX_CAMPI+5][DBSEQ_MAX_LEN_CAMPO+5]; // Array di stringhe utilizzate internamente
                                 // per appoggiare i campi del Record

     char StringaRecord[1100];   // Stringa su cui si appoggia il Record durante le perazioni di lettura/scrittura..

     int  MillisecDiffTime;      // Viene calcolato automaticamente in seguito al richiamo
                                 // delle funz. TimeStart() e TimeEnd()

     long int PtrUltimoRecord;   // Puntatore a OFFSET ULTIMO recrod restituito da ReadLastRecord()
     
     FILE *FILEPtrFile;
     SYSTEMTIME TimeInizio;
     SYSTEMTIME TimeFine;

     // Funzioni di GESTIONE del file
           DBSeq();                                                             // costruttore classe DBSEQ
     char *CurrentDirectory(char *path);          // Recuper la directory di lavoro corrente
     int   TimeStart();                           // Memorizza Time Inizio
     int   TimeEnd();                             // Memorizza Time Fine e calcola DiffTime in msec

     int   GetCampoRecord( char *StringaCampo, int NumCampo, char *all_mess );  // Recupera il campo richiesto nel record
     int   GetTuttiCampiRecord( int TotCampi, char *all_mess );                 // Recupera più campi all'interno del record
     int   SetCampoRecord( char *StringaCampo, int NumCampo, char *all_mess );  // Compila campo nel Record
     int   SetTuttiCampiRecord( int TotCampi, char *all_mess );                 // Compila tutti i campi nel Record

     int   Create               (char *all_mess);                      // Crea il file vuoto e chiude
     int   Open                 (char *all_mess);                      // Apre il file in RW alla posizione iniziale
     int   OpenAppend           (char *all_mess);                      // Apre il file in APPEND per la scrittura record
     int   Close                (              );                      // Chiude il file
     int   Delete               (              );                      // Cancella il file se esiste

     int   DBWriteLine          ( FILE *RFile, char *Stringa);         // Scrive una riga in un file binario aagiungendo CR LF alla fine
     int   DBWriteRecord        ( char *all_mess );                    // Insert RECORD alla posizione corrente
     int   InsertRecord         ( char *all_mess );                    // Insert RECORD alla posizione di END OF FILE
     int   OpenInsertRecordClose( char *all_mess );                    // Open il file in APPEND Insert RECORD e Close

     int   RecuperaDimensioneFile( );                                  // Calcola la dimensione del FIlE con file aperto
     int   ReadFirstRecord      ( char *all_mess );                    // Legge dal file il primo record e si posiziona in coda
     int   ReadLastRecord       ( char *all_mess );                    // Recupera ultimo record e relativa posizione

     int   ReadNextRecord       ( char *all_mess );                    // Legge dal file il record alla prossima posizione
     int   CercaRecord          ( int NumCampo, int Intestazione,      // Cerca il record < <= = >= >
                                  char* ValoreCercato, int TipoRicerca,
                                  int TipoCampo, int* PosizioneRecord,
                                  char *all_mess );
     int   ScrollFile( int intestazione, int scroll, char *all_mess);  // Fa uno scroll dei Record
     int   ScrollLongFile( int intestazione, int scroll, long int *OffsetPosition, int  NumRecordPerScrollTemp, char *all_mess);  // Fa uno scroll dei Record
};


#endif

