//------------------------------------------------------------------------
//               file       LIBRERIA.H
//------------------------------------------------------------------------
#ifndef LIBRERIA.H
#define LIBRERIA.H

//#include <vcl.h>
//#include <vcl/dstring.h>
#include <stdio.h>
#include <sys\timeb.h>
#include <stdlib.h>
#include <windows.h>

// Variabili di tempo utilizzato in TIMEWIN.CPP
#ifndef MAINFILE
    #define TIPO_VARIABILE extern
#else
    #define TIPO_VARIABILE
#endif

//-------------------------------------
// costanti per comunicazione con PLC
//-------------------------------------
#define STX            0x02
#define ETX            0x03
#define EOT            0x04
#define ENQ            0x05
#define ACK            0x06
#define NAK            0x15
#define TIMEOUT        0x06
#define NOTDLE         0x07
#define DLE            0x10
#define NOCONNECT      0x08
#define MAX_ALL_3964   10

#define  MAXPOSTIARCHIVIOPAL 63      // numero max l'archivio del pallettizzatore
                                     // e il valore max dell'identificativo.

#define  PORT_RADIO       0  // Variabile che INDICA il numero di
                             // port utilizzato per la radio
                             // N.B. ha significato solo in
                             //      con la variabile sucessiva
#define  OFFSET_PLC_AGV   0  // Variabile che schifta il numero
                             // di PLC del valore impostato es: 0

#define  MAX_MESSAGG   800   // numero max di messaggi mel file di Messaggeria
#define  MAX_ALLARM    200   // numero max Allarmi in elenco
// ---------------------------------------------------------------
//   Variabili globali
// ---------------------------------------------------------------
struct Canale { bool Bit[16]; };

TIPO_VARIABILE unsigned long  CalDelay;
TIPO_VARIABILE int       GloRptLine;
TIPO_VARIABILE char      ALLARM[MAX_ALLARM][101];                        // Messaggeria Allarmi
TIPO_VARIABILE char      MESS[MAX_MESSAGG][101];                         // Messaggeria di Sistema
TIPO_VARIABILE int       abilita_delay_speciale;                         // Gestione delay speciale



int Ciccio();

// ---------------------------------------------------------------
//   dal file    UTILITY.CPP
// ---------------------------------------------------------------
int  file_cronologico_generico( char *nome_file, char *messaggio, long int NumByte );
int  file_cronologico_generico_csv( char *nome_file, char *messaggio, long int NumByte );
int  scroll_file(char *nome_file,
                 int intestazione,
                 int scroll,
                 char *all_mess);
int  inverti_sequenza_file_cron(char *nome_file_source,
               char *nome_file_dest,
               int intestazione,
               int coda,
               int *tot_linee);
int  load_MESSAGG(char *file_lingua);
int  load_ALLARM(char *file_lingua);
int  LoadMessDefault(char *file_lingua);
int  SaveMessDefault(char *indice, char *file_lingua);
int  SaveMessDefault(char *indice, char *file_lingua);
int  ReadLineaFile(char *filename, short int linea, char *stringa);
int  SaveLineaFile(char *filename, char *stringa);
int  TestCampiDiEditor(char Tipo, char *Text,
                       char *StringaConfronto,
                       int ValoreIniziale,
                       int ValoreFinale);
void TraduciPCtoPLCDoppia(char *Doppia, WORD *ValoreCanaleH, WORD *ValoreCanaleL);
void TraduciPLCtoPCDoppia(char *Doppia, WORD *ValoreCanaleH, WORD *ValoreCanaleL);
void TraduciPLCtoPC(WORD *ValoreCanale);
void TraduciPCtoPLC(WORD *ValoreCanale);
void CanaleInt(Canale *ChBool, unsigned short int *Parola);
void IntCanale(unsigned short int Parola, Canale *ChBool);
void Spezza_Codice(char *codice, char *cod1, char *cod2, int parte);

//-----------------------------------------------------------------
//    dal file      FILE.CPP
//-----------------------------------------------------------------
char Copy_File(char *OFName,
              char *DFName);

char Created(char *FName);

long File_Size(FILE *Stream);

char Read_Line(FILE *RFile,
               char *Line,
               int MaxLineSize);

void Write_Line(FILE *RFile,
                char *Line);

void Write_String(FILE *RFile,
                  char *Line);

int  AppendStringASCIIFile( char *file_name,
                            char *messaggio);


//+----------------------------------------------------------------------+
//|    funzioni  WINDOW di gestione seriali                              |
//+----------------------------------------------------------------------+
#define MAXCOM  10
//---------------------------------------
//definizione codici errore
//---------------------------------------
#define COMM_NO_ERR              0
#define COMM_ERR_HANDLE          1 //errore inizializzazione
#define COMM_ERR_PORT_IN_USE     2 //errore porta gia usata
#define COMM_ERR_PORT            3 //errore porta diversa  o errata
#define COMM_ERR_IO              4 //errore I/O
#define COMM_ERR_FRAME           5 //errore in un frame
#define COMM_ERR_BREAK           6 //errore di break
#define COMM_ERR_MODE            7 //modo di comunicazione errato
#define COMM_ERR_OVERRUN         8 //errore di overrun
#define COMM_ERR_TXFULL          9 //buffer di scrittura pieno
#define COMM_ERR_OVERFLOW       10 //errore di buffer oferflow
#define COMM_ERR_RXPARITY       11 // errore di parità
#define COMM_ERR_TIMEOUT        12 //errore timeout
#define COMM_ERR_SET_TIMEOUT    13 //errore timeout non corretti
#define COMM_ERR_SCONOSCIUTO 65535 //errore sconosciuto


//-----------------------------------------------------------------
//  dal file   SERIALE.CPP
//-----------------------------------------------------------------
    // inizializza la porta
WORD init_port(WORD port, int velocita, WORD lunghezza,
              unsigned char parita, WORD bitstop, int TimeOut);
    // Trasmette un carattere sulla porta specificata
WORD trs_char(WORD port, unsigned char carattere);
    // trasmette una stringa di ncar
WORD trs_str(WORD port, char *str,int ncar);
    // Riceve un carattere dalla porta
WORD recive_char(WORD port, unsigned char *carattere);
    // Restituisce lo stato della porta
WORD svuota_buffer(WORD port);
    // Vuota la porta da eventuali caratteri presenti
WORD stat_port(WORD port);
    // Restituisce 0 se la porta e' collegata
WORD port_collegata(WORD port);
    // Restituisce 0 se c'e' un carattere presente sulla porta
WORD SvuotaRx(WORD port);
    // Restituisce 0 se c'e' un carattere presente sulla porta
WORD test_char(WORD port);
    // Setta  RTS
WORD SetRTS(WORD port);
    // Reset  RTS
WORD ClearRTS(WORD port);
    // Test CTS
WORD TestCTS(WORD port);
    // Setup timeout porte seriali
WORD setup_timeout_seriale(WORD port,DWORD r_ti,DWORD r_tm,DWORD r_tc,DWORD w_tm,DWORD w_tc);
    // Numero caratteri in attesa ricezione
WORD CaratteriInAttesa(WORD port, int *n_byte);
    // Chiudi COM
WORD ChiudiPortSeriale(WORD port);


//-----------------------------------------------------------------
// dal file     STRING.CPP
//-----------------------------------------------------------------
//
void addzero(char *Bufasci,
             int Num_cifre);

void addblank( char *Bufasci,
             int Num_cifre);
void addblankcoda( char *Bufasci,
             int Num_cifre);

char *Cen(char *CStr,
          char *TStr,
          int FW);

int Common_Chars(char *Str1,
                 char *Str2);

char *Copy(char *Source,
           int Position,
           int NumChars,
           char *Destin);

void Delete(char *DlStr,
            int Start,
            int NumChars);

char *Erase_White(char *St);

char *Erase_White_Begin(char *St);

char *Erase_White_End(char *St);

void Insert(char *ObStr,
            char *TrgStr,
            int Position);

char Is_Text(char *Txt);

char *LJ(char *LStr,
         char *TStr,
         int FW);

int Num_Chars(char Ch,
              char *St);

int Num_Decs(char *Num);

int Num_Ints(char *Num);

char *Pattern(char *RlStr,
              int FldWdth,
              int NumDecs,
              char *Buffer);

int Pos(char Ch,
        char *St);

float Range_Max(char *Rng);

float Range_Min(char *Rng);

char *Replace(char *Src,
              char *Target,
              int Position);

char *RJ(char *RStr,
         char *TStr,
         int FW);

char *Rpt(char *Rpc,
          char *RStr,
          int Repts);

char Str_IsInt(char *PNum);

int Str_Pos(char *SearchStr,
            char *TargStr,
            char CaseFlag);

void OraData(char * Ora,char * Data);
int  data_inversione_to_AAAA_MM_GG(char *data1, char *data2);
int  data_inversione_to_GG_MM_AAAA(char *data1, char *data2);


//-----------------------------------------------------------------
//  dal file   GEST_BITS.CPP
//-----------------------------------------------------------------
void SetBit(char *ptr, unsigned bitnum);
    /* Setta il bit n di un byte puntato da ptr
       l'ordine Š dal bit a peso minore in calando */
void ClrBit(char *ptr, unsigned bitnum);
    /* Cancella il bit n di un byte puntato da ptr
       l'ordine Š dal bit a peso minore in crescendo */
char TestBit(char *ptr, unsigned bitnum);
    /* Restituisce 0 se il bit Š a 0, 1 altrimenti
       l'ordine Š dal bit - significativo in crescendo */
void show_bit (char *ptr, char *stringa);
    /* Traduce in una stringa in formato binario il contenuto di un char */



//-----------------------------------------------------------------
//     dal file TIMEWIN.CPP
//----------------------------------------------------------------
//
void Calibrate_Delay(void);
void delay(int msec);
void delay_speciale( int MsecAttesa);
char *ora(char * ora);
void timeint_to_timechar(long *tim, char *data, char *ora);
void timechar_to_timeint(long *tim, char *data, char *ora);
char *Today(char *Buffer);
void data_inversione(char *data1, char *data2);

// --------------------------------
//      dal file OMRON.CPP
// --------------------------------
//
//--------------------
//  classe OMRON
//--------------------
class omron{
   public:
       short int fact_err;
       short int break_plc;

           omron();
       WORD init_porta(WORD port, int TimeOut, char *all_mess);
       WORD modo(WORD port, WORD plc, WORD modo, char *all_mess);
       WORD md(WORD plc, WORD md, char *all_mess);
       WORD load_canale(WORD port, WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);
       WORD write_canale(WORD port, WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);
       WORD ld_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);
       WORD wr_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);
       WORD wr_canale_NoAttesaRisposta(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, WORD NoAttesa, char *all_mess);
       WORD load_TIM_CNT(WORD port, WORD *SV, WORD plc, char *cmd, char *tipo, WORD numero, char *all_mess);
       WORD ld_TIM_CNT(WORD *SV, WORD plc, char *cmd, char *tipo, WORD numero, char *all_mess);
       WORD write_TIM_CNT(WORD port, WORD *SV, WORD plc, char *cmd, char *tipo, WORD numero, char *all_mess);
       WORD wr_TIM_CNT(WORD *SV, WORD plc, char *cmd, char *tipo, WORD numero, char *all_mess);
       WORD long_ld_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);
       WORD long_wr_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess);

       //Funzioni di controllo della seriale e del modem
       WORD ready(WORD port);
       WORD out_char(WORD port, unsigned char carattere);
       WORD set_RTS(WORD port);
       WORD reset_RTS(WORD port);
       WORD SvuotaBuffer(WORD port);
       WORD test(WORD port, char *all_mess);
       }; //*** class omron ***

// --------------------------------
//      dal file SIEMENS.CPP
// --------------------------------

//-----------------------------------------------------------------
// classe     SIEMENS
//-----------------------------------------------------------------
#define  INIT      1
#define  LETTURA   2
#define  SCRITTURA 3

class siemens{
       public:
            WORD        Procedura_R;        // indica P3964 R
            WORD        Bcc;                // Block Control Character
            WORD        BccR;               // Block Control Character Ricezione
            WORD        break_plc;          // Inibizione comunicazione
            WORD        ErroreComunicazione;
            WORD        Port_PLC1;          // Porta di comunicazione con PLC1
            WORD        Port_PLC2;          // Porta di comunicazione con PLC1
            int         Break_Port_PLC1;    // Break comunicazione con PLC1
            int         Break_Port_PLC2;    // Break comunicazione con PLC2
            //--------------------------
            // Costruttore
            //--------------------------
                    siemens();
            //--------------------------------
            // Inizializzazione porta seriale
            //--------------------------------
            WORD     init_port3964r(WORD porta);

            //------------------------------------------------------------
            // Gestione comunicazione con PLC attraverso protocollo P3964r
            //------------------------------------------------------------
            WORD     protP3964r(WORD porta, WORD FUNZ, unsigned char  DB,
                               unsigned char DW,    unsigned char nDW,
                               WORD * dati, char * err_string);

            //--------------------------------
            // Lettura di un DataBlock da PLC
            //--------------------------------
            WORD     read3964r(WORD porta, unsigned char DB, unsigned char DW,
                              unsigned char nDW, WORD * dati);

            //--------------------------------
            // Scrittura di un DataBlock su PLC
            //--------------------------------
            WORD     write3964r(WORD porta, unsigned char DB, unsigned char DW,
                               unsigned char nDW, WORD * dati);

            //----------------------------------------------
            // Calcola il Bcc e trasmette il dato.
            // ritrona errore.
            //----------------------------------------------
            void    Control_Block(WORD Dato); // aggiorna Bcc

            //----------------------------------------------
            // Calcola il BccR e trasmette il dato.
            // ritrona errore.
            //----------------------------------------------
            void    Control_Block_Ricevuto(WORD Dato); // aggiorna Bcc


       }; /*** class siemens ***/


TIPO_VARIABILE siemens    SIM;                 // classe SIEMENS
TIPO_VARIABILE omron      OM;                  // classe OMRON


#endif;
