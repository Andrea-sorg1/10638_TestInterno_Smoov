// ------------------------------------------------------------------------
//             file          TECNOFER.H
// ------------------------------------------------------------------------
#ifndef TECNOFERH
#define TECNOFERH

//---------------------------------------------------------------------------
#include <windows.h>
#include <vcl.h>
#include <vcl/dstring.h>
#include <DBTables.hpp>
#include <Math.hpp>
#include <ADODB.hpp>

#include "DBSeq.h"
#include "Variabil.h"
#include "Libreria.h"
#include "Impianti.h"
#include "OmronETH.h"

// --------------------------------
//      Variabili Globali
// --------------------------------
#ifndef MAINFILE                       // In caso non sia il MAINFILE
    #define TIPO_VARIABILE extern      // dichiararla come extern
#else
    #define TIPO_VARIABILE
#endif;


#define AGGIORNAMENTO               0  // Aggiornamento dell'Impianto

#define MAX_TRACCE_CURVA_DWG        1000

#if ATTIVA_GESTIONE_UDC==0
  #define MAXUDCBOX       0
  #define MAXUDCIMPIANTO  1
#else
  #define MAXUDCBOX       POSINPRESA*PIANIINPRESA
  #define MAXUDCIMPIANTO  5000
#endif;

// ----------------------------------------------
//  Dati anagrafici da collegamento SQL
// ----------------------------------------------
struct DATI_PRD{
              int   id_record;
         long int   time;
             char   Udc[21];
             char   code[21];
             char   description[31];
             char   size[11];
             char   batch[11];
             char   state[5];
     unsigned int   source;
     unsigned int   location;
     unsigned int   destination;
        short int   weight;
        short int   height;
        short int   quantity;
             char   aus1[11];
             char   aus2[11];
             char   aus3[11];
             char   shift[11];     // ### ex aus3[11]
     unsigned int   loc_prov;
              int   aux2;
              int   aux3;
             bool   var1;
             bool   var2;
             bool   var3;
};


// ----------------------------------------------
//  Dati box trasportato
// ----------------------------------------------
struct BOX{
     short int ID;                                               // ID BOX
     short int Pos;                                              // Posizione del BOX nell'impianto (NODO)
           int ProvBox;                                          // Provenienza del box
     short int GruppoProv;                                       // Gruppo provenienza
     short int GruppoDest;                                       // Gruppo di destionazione
     short int StatoLav;                                         // Stato lavorazione abbinato al box (TIPO CODICE)
                                                                 // 0 - vuoto
                                                                 // 1 - pieno
                                                                 // ...
     short int TipoBox;                                          // tipo pallet
          BYTE Codice;                                           // Codice paletta;
          BYTE aux1;                                             // aux
           int Lotto;                                            // Lotto paletta
     short int Altezza;                                          // Altezza espressa in mm
     short int CarWash;                                          // Gestione Emergenza/Non conforme
          bool Basso;                                            // Pallet basso
          bool Sovrap;                                           // Sovrapposizione pallet alto in mag.
          bool Presa[POSINPRESA+1][PIANIINPRESA+1];              // Matrice di configurazione presa
                                                                 // max 3 posizioni a terra
                                                                 // max 2 posizioni in altezza (ammesse solo se pallet bassi)
     struct DATI_PRD Prd[MAXUDCBOX+1];                           // enentuali dati prodotto (UN UDC PER OGNI PALLET IN PRESA)
      long int TimeC;                 // Data di carico
      long int TimeS;                 // Data di spedizione
          bool Ruotato180;            // Pallet Ruotato di 180°
          bool var1;                  // Variabile ausiliaria
          bool var2;                  // Variabile ausiliaria
          bool var3;                  // Variabile ausiliaria
          bool var4;                  // Variabile ausiliaria
          bool var5;                  // Variabile ausiliaria
          bool var6;                  // Variabile ausiliaria
          bool var7;                  // Variabile ausiliaria
          bool var8;                  // Variabile ausiliaria
          bool var9;                  // Variabile ausiliaria
     short int aux3;                  // Variabile ausiliaria
     short int aux4;                  // Variabile ausiliaria
     short int aux5;                  // Variabile ausiliaria
     short int aux6;                  // Variabile ausiliaria
     short int aux7;                  // Variabile ausiliaria
     short int aux8;                  // Variabile ausiliaria
     short int aux9;                  // Variabile ausiliaria
          char aus1[11];              // Variabile ausiliaria
          char aus2[11];              // Variabile ausiliaria
          char aus3[11];              // Variabile ausiliaria
          char aus4[11];              // Variabile ausiliaria
          char aus5[11];              // Variabile ausiliaria
};

//--------------------
//  classe BOX.CPP
//--------------------
class box{                               // definizione classe carrelli
    public:
    struct BOX EditBox;

    // Funzioni dal file CARRELLI.CPP

    box();     // creatore classe silos

    // GESTIONE "UDC"
    unsigned int StructToLocation(short int Tgv, short int Mac, short int Mag, short int Bai, short int Stz, short int Pos, short int Pin, short int Forc, short int NPal);
    int  LocationToStruct(unsigned int Loc, short int *Tgv, short int *Mac, short int *Mag, short int *Bai, short int *Stz, short int *Pos, short int *Pin, short int *Forc, short int *NPal);
    int  VerificaUDCBoxDuplicati(char *Udc, unsigned int location, char *all_mess);
    int  UdcTmpToStruct(struct DATI_PRD *UdcTmp, char *all_mess);
    int  StructToUdcTmp(short int cmd, short int Tgv, short int Mac, short int Mag, short int Bai, short int Stz, short int Pos, short int Pin, short int Forc, char *all_mess);
};

// ----------------------------------------------
//      dal file    NODO.CPP
// ----------------------------------------------
//
//  Struttura del file NODO.DAT. Tale file viene consultato nella costruzione
//  dei percorsi AGV. Ad ogni nodo sono abbinate diverse soluzioni di continuit…
//  ( rec_linea ) per raggiungere una determinata destinazione. Tale destinazione
//  pu• essere [pt] [zs] [zp], per ognuna Š indicato il punto successivo e il
//  comportamento che l'AGV deve tenere per raggiungerlo
//
struct rec_linea{           // record del file NODO.DAT

           short int  pt,       // punto terminale    ( da raggiungere )
                      zs,       // Zs di appartenenza ( da raggiungere )
                      zp,       // Zp di appartenenza ( da raggiungere )
                      ps;       // punto successivo
           byte       av;       // avanti             (AGV)
           byte       NodPrecL; // Nodo Precedente Parte bassa
           byte       ind;      // indietro           (AGV)
           byte       NodPrecH; // Nodo Precedente Parte alta
           short int  rot,      // rotazione          (AGV 0,90,180,270)
                      ral,      // rallentamento      (AGV)
                      dist;     // distanza al prossimo nodo
};


//
//  La struttura NOD conterr… un insieme rec_linea per raggiungere tutti i
//  possibili nodi dell'impianto e informazioni tipiche del nodo
//
struct  NOD{
     short int  num,            // numero nodo
                busy;           // numero AGV che lo impegna ( 0=free)
           char nom[22];        // stringa di identificazione
   //short int  direz;          // direzione da seguire in car/scar ( 0 nulla,1 av, 2ind )
           byte direz;          // Direzione da seguire in car/scar ( 0 nulla,1 av, 2ind )
           byte ZoneMap;        // Zona Mapping di default =0 se diverso da 0 forza la zona mapping del nodo 255-->zona 0
          bool  blocco_rot;     // flag per blocco rotazioni di 180°
          bool  inverti_180;    // flag di inversione rotazione 180° su start missione;
     short int  zp;             // Zona primaria di appartenenza punto
     short int  zs;             // Zona secondaria di appartenenza del punto
          BYTE  typ;            // tipo nodo
          BYTE  corsia;         // corsi appartenza 
     short int  piano;          // piano di appartenenza del nodo
     short int  NodoMaster;     // Ha valenza per i nodi SLAVE per indicarne il relativo NODO MASTER
     short int  aux1,           // variabili ausiliarie
                aux2;           // variabili ausiliarie

         struct rec_linea rec[MAXPUNTINODO];
};

//  DEFINIZIONE CURVE PER EDITOR
struct CONSENSI_CURVE{
  bool upsx,                  // 0 Dall'alto -> al basso a sx
       sxdw,                  // 1 Da sx     -> al basso
       dwdx,                  // 2 Dal basso -> all'alto a dx
       dxup,                  // 3 Da dx     -> all'alto
       sxup,                  // 4 Da sx     -> all'alto
       dwsx,                  // 5 Dal basso -> all'alto a sx
       dxdw,                  // 6 Da dx     -> al basso
       updx;                  // 7 Dall'alto -> al basso a dx
};

struct BLOCCHI_MANUALI{
     short int pintend;
     short int succ1;
     short int succ2;
     short int succ3;
     short int succ4;
     short int succ5;
     short int aux1;
     short int aux2;
     short int aux3;
     short int aux4;
     short int aux5;
     short int aux6;
     short int aux7;
     short int aux8;
     short int aux9;
     short int occu1;
     short int occu2;
     short int pren1;
     short int pren2;
     short int pren3;
     short int pren4;
     short int pren5;
};

struct ZONE_FUORI_ING_MAN{
     short int pintend;
     short int succ1;
     short int succ2;
     short int succ3;
     short int succ4;
     short int succ5;
     short int aux1;
     short int aux2;
     short int aux3;
     short int aux4;
     short int aux5;
     short int aux6;
     short int aux7;
     short int aux8;
     short int aux9;
     short int occu1;
     short int occu2;
     short int pren1;
     short int pren2;
     short int pren3;
     short int pren4;
     short int pren5;
};

struct PINT_OBBLIGATO{
     short int pintend;
     short int succ;
     short int prec;
};

//--------------------
//  classe NODO
//--------------------
class nodo{
    public:
    char      file_name_sorg[100];
    char      file_name_dest[100];
    char      FileNameNodoBusySorg[100];
    char      FileNameNodoBusyDest[100];
    char      FileNameNodoGrdSorg[100];
    char      FileNameNodoGrdDest[100];
    struct    NOD *n[MAXPUNTI+2];                       // Tabella di puntatori a
                                                        // strutture NOD
    short int nodo_busy[MAXPUNTI+2];                    // Tabella di occupazione nodi
                                                        // con num di AGV che la occupa
         bool punti_notevoli[MAXPUNTI+1];               // Tabella di punti terminali  appartenenti
                                                        // a macchine magazzini posiz. di parcheggio
    short int nodo_circostante[MAXPUNTI+1][4];          // Tabella di compilazione matrice
                                                        // nodi circostanti
    short int nodo_in_ingombro[MAXPUNTI+1];             // Tabella di nodi terminali
                                                        // con nodi adiacenti in ingombro
         bool nodi_in_ingombro_per_porte[MAXPUNTI+1];   // Nodi da sgomberare per IncendioInCorso
                                                        //
    short int nodo_contrapposto[MAXPUNTI+1];            // Tabella dei nodi contrapposti in
                                                        // ingombro.
    short int occupa_nodo_salato[MAXPUNTI+1];           // Abbinamento occupazione nodi saltati ad altri nodi
                                                        //
    short int nodo_minpercorsi[MAXPUNTI+1];             // Tabella che associa a un nodo un MINPERCORSI
                                                        // diverso da quello impostato in Variabil.h
    short int nodo_con_tgv_lanciato[MAXPUNTI+1];        // Nodo non occupato, ma riservato e su un rettilineo
                                                        // con il TGV lanciato.
    short int nodi_interbloccati[51][51];               // Elenco nodi intebloccati

         bool pt_in_ingombro[MAXPUNTI+1];               // Tabella di tutti quei Punti Terminali che devono
                                                        // essere tenuti in considerazione nella gestione
                                                        // di occupa_nodi() perchè a rischio di collisioni.
                                                        // Gestito in GRAPHVAR.CPP

    struct PINT_OBBLIGATO pint_obbligato[101];             // Punto intermedio obbligato (es. PER ROTAZIONI DA FERMO)

    short int nodi_attesa[MAXGRUPPIIMP+1][MAXNODIATTESA+1];// Elenco possibili nodi attesa

    short int raggi_curve_laser[MAXPUNTI+1][5];            // Matrice delle curve laser con i raggi teorici
                                                           // ed effettivi.
    short int nodo_grd[MAXAGV+1][MAXPUNTI+1];              // Struttura per la visualizzazione grafica dei
                                                           // gradi assegnati a un nodo nella missione TGV
    struct BLOCCHI_MANUALI blocco_man[201];                // Gestione dei blocchi manuali di impianto.
    struct ZONE_FUORI_ING_MAN zone_f_ing_man[201];         // Gestione zone fuori ingombro manuali.

    short int rec_attuale;

    // Funzioni di GESTIONE dal file NODO.CPP
          nodo();                                                      // costruttore classe nodo
    void  init_DB(struct FileSpecType  *ptr );                         // inizializza i parametri BTRV per creazione
    int   crea_DB(char *file_btr, int modo);                           // crea file NODO.DAT
    void  nod_char_to_int();                                           // conversione per SALVA in file
    void  nod_int_to_char();                                           // conversione per LEGGI da file
    int   free_nodi(char *all_mess);                                   // libera tutti i nodi occupati dagli AGV
    int   load_nodo_busy(char *all_mess);                              // legge il file dei nodi occupati
    int   save_nodo_busy(char *all_mess);                              // salva il file dei nodi occupati
    int   load_nodo_grd(char *all_mess);                               // legge il file dei gradi di rotazione sui nodi
    int   save_nodo_grd(char *all_mess);                               // salva il file dei gradi di rotazione sui nodi
    int   compila_punti_notevoli(char *all_mess);                      // Compila i punti terminali
    int   compila_nodo_circostante(char *all_mess);                    // compila la matrice di nodi circostanti
    int   compila_blocchi_manuali_percorso(char *all_mess);            // funzione per la compilazione della struttura
                                                                       // dei blocchi sulla gestione dei percorsi
    bool  verifica_blocco_manuale(short int num_agv, short int punto, short int *agv_bloccante); // Verifica se per un dato nodo sono previsti
                                                                                                 // blocchi di percorso.
    bool  verifica_pint_forzato(short int punto);                      // Gestione Pint forzato da file
    int   compila_zone_fuori_ingombro_manuali(char *all_mess);         // funzione per la compilazione della struttura
                                                                       // delle zone fuori ingombro manuali
    bool  verifica_zona_fuori_ingombro_manuale(short int num_agv, short int punto, short int agv_bloccante); // Verifica se per un dato nodo sono previste
                                                                                    // delle condizioni di zone fuori ingombro forzate.
    bool  verifica_nodo_fuori_ingombro(int punto);                                  // Verifica singolo nodo fuori ingombrose per un dato nodo sono previste
                                                                                    // delle condizioni di zone fuori ingombro forzate.

    int   load_file_nodi(char *all_mess);
    int   save_record_nodi(int numero, char *all_mess);
    int   save_file_nodi(char *all_mess);
    int   num_record();
    int   cerca_record(struct NOD *nod, int numero);
    int   VisualizzaCompilazioneNodi(int Nodo);
    int   ricalcolo_distanze_nodi(char *all_mess);             // ricalcolo distanze nodi
    int   VerificaDistanzaPunti( int Punto1, int Punto2, int *DistX, int *DistY, double *Dist);

    //-----------------------------------------------------
    //   GESTIONE CURVE
    //-----------------------------------------------------
    int  CercaCurva( bool SoloCurveGrafiche, int P1, int P2, int* Raggio, int* Circonferenza, int* IdCurva);
    int  CercaCurvaBezier(bool SoloCurveGrafiche, int P1, int P2, int *grado, TPoint *p);
    bool CalcoloRaggioArcoTangente(short int P0, short int P1, short int P2, short int P3, int *raggio, int *arco);
    int  CalcolaGrdRotazioneSuStartTGV(short int num_agv, short int ptr_path, short int *rot_succ, bool *antioraria);
    int  calcola_raggi_curva(short int *nodi_perc);
    int  CalcolaRotUscita(short int num_agv, short int IndicePath, int NodoInizio, struct rec_linea dati_path, short int *RotFinaleArrivo);

    //-----------------------------------------------------
    //   GESTIONE PERCORSI DA AGV.CPP
    //-----------------------------------------------------
    //int  cerca_path_into_nodo(short int tipo_percorso, int percorso_speciale, short int source, short int target, struct NOD *nod, struct rec_linea *dati);
    int  cerca_path_into_nodo1(short int tipo_percorso, int percorso_speciale, short int source, short int target, short int NodoPrec, short int RotFinaleArrivo, struct NOD *nod, struct rec_linea *dati);
    int  cerca_path_into_nodo (short int tipo_percorso, int percorso_speciale, short int source, short int target, short int NodoPrec, short int RotFinaleArrivo, struct NOD *nod, struct rec_linea *dati);
    int  cerca_path(short int num_agv, short int tipo_percorso, short int sorgente, short int destinazione, short int *nodi_perc, struct DATI_PERCORSO *dati_perc, char *all_mess);

    //-----------------------------------------------------
    //  GESTIONE PRECEDENZE PER TGV IN RETTILINEO
    //-----------------------------------------------------
    void verifica_tgv_lanciato_su_rettilineo();
    bool blocco_per_tgv_lanciato_su_rettilineo(short int num_agv, short int punto, short int *altro_agv);
    int  load_nodi_tgv_lanciati_su_rettilineo(char *all_mess);
    int  save_nodi_tgv_lanciati_su_rettilineo(char *all_mess);               // salva il file dei nodi occupati
};



// --------------------------------------
//      dal file    AGV.CPP
// --------------------------------------
//
//  *** DEFINIZIONE STATO AGV ***
//
struct stat_comandi {                          // definizione WORD stato AGV
        unsigned grd0              : 1;        // AGV a zero gradi
        unsigned grd90             : 1;        // AGV a 90 gradi
        unsigned av                : 1;        // AGV in movimento avanti
        unsigned ind               : 1;        // AGV in movimento indietro
        unsigned allarme           : 1;        // AGV in allarme
        unsigned okprog            : 1;        // AGV ok per trasferimento programma
        unsigned inattesa          : 1;        // AGV in attesa
        unsigned funz              : 1;        // AGV in automatico
        unsigned sunod             : 1;        // AGV in posizione su nodo
        unsigned ris               : 1;        // AGV in riserva batteria
        unsigned riposiz           : 1;        // AGV in riposizionamento su nodo magazzino
        unsigned grd180            : 1;        // AGV a 180ø
        unsigned grd270            : 1;        // AGV a 270ø
        unsigned carB              : 1;        // AGV con carico su postazione "B"
        unsigned carA              : 1;        // AGV con carico su postazione "A"
        unsigned aux2              : 1;        // AGV bit ausiliario.

        unsigned ForcaB_FuoriIng   : 1;        // AGV bit Fuori Ingombro Forche A
        unsigned ForcaA_FuoriIng   : 1;        // AGV bit Fuori Ingombro Forche B
        unsigned Bit1_2            : 1;        // AGV bit ausiliario.
        unsigned Bit1_3            : 1;        // AGV bit ausiliario.
        unsigned Bit1_4            : 1;        // AGV bit ausiliario.
        unsigned Bit1_5            : 1;        // AGV bit ausiliario.
        unsigned Bit1_6            : 1;        // AGV bit ausiliario.
        unsigned Bit1_8            : 1;        // AGV bit ausiliario.
        unsigned Bit1_9            : 1;        // AGV bit ausiliario.
        unsigned Bit1_10           : 1;        // AGV bit ausiliario.
        unsigned Bit1_11           : 1;        // AGV bit ausiliario.
        unsigned Bit1_12           : 1;        // AGV bit ausiliario.
        unsigned Bit1_13           : 1;        // AGV bit ausiliario.
        unsigned Bit1_14           : 1;        // AGV bit ausiliario.
        unsigned Bit1_15           : 1;        // AGV bit ausiliario.
        };

union  def_stat{
        struct stat_comandi bit;
        unsigned int        val;      // valore globale word stato
        };

//
//  *** DEFINIZIONE COMANDI AGV ***
//
struct comandi {        // comandi per AGV
        unsigned grd0      : 1;        // AGV a zero gradi
        unsigned grd90     : 1;        // AGV a 90 gradi
        unsigned vel       : 1;        // AGV veloce se a 1
        unsigned av        : 1;        // AGV in movimento avanti
        unsigned ind       : 1;        // AGV in movimento indietro
        unsigned carico    : 1;        // AGV inizia carico
        unsigned scarico   : 1;        // AGV inizia scarico
        unsigned grd180    : 1;        // AGV a 180ø
        unsigned grd270    : 1;        // AGV a 270ø
        unsigned forchealte: 1;        // Gestione forche alte
        unsigned curva     : 1;        // LASER   = Bit di curva
                                       // MAGNETI = Bit di rifasamento DM97
        unsigned antiora   : 1;        // AGV in curva anti-oraria
        unsigned avvic     : 1;        // Manovra di ricerca pos. x car.- scar.
        unsigned end       : 1;        // Fine programma
        unsigned ral       : 1;        // Bit di sincronismo rallentamento
        unsigned az        : 1;        // Bit di azione


        };

union  def_comandi{
        struct comandi bit;
        unsigned int   val;
        };

//
// Situazione attuale navetta
//
struct STATO{
        union def_stat s;            // Bit di stato AGV
        short int ptr_step,          // Puntatore al passo di programma
                  pos,               // Posizione attuale o ultima occupata
                  start,             // Missione in corso START
                  offset_da_nodo;    // impulsi da nodo (dm93) (11.02.94)
              int x_laser;           // Coordinata X passata dal laser
              int y_laser;           // Coordinata Y passata dal laser
        short int rot_laser;         // Rotazione passata dal laser
        short int g_laser;           // G del laser
        short int rot_calamite;      // Rotazione recuparata dalle calamite
            };

//
// Dati di percorso
//
struct DATI_PERCORSO{
       short int ps;                 // Punto sucessivo
       short int dir;                // Direzione di avanzamento (0 - Av, 1 - Ind)
       short int rot;                // Rotazione
       short int ral;                // Rallentamento
       short int dist;               // Distanza
       short int salta_a_ps;         // Salto a punto successivo
       short int salta_a_dist;       // Distanza di salto a punto successivo
};

//
// Definizione missione navetta
//
struct MISSION{
              bool  NoRiposoAuto;                    // Blocca il ritorno automatico a riposo
              bool  SemiAutomatico;                  // Variabile che definisce che il TGV è in
                                                     // gestione SEMIAUTOMATICA.
          long int  TimeInitAttesa;                  // Variabile di inzializzazionre tempo di attesa
          long int  TimeInitMission;                 // Variabile tempo di inzio missione
          long int  TimeFineMission;                 // Variabile tempo di fine missione
              bool  Esclusione;                      // Flag che indica che l'AGV è escluso
              bool  EsclPLC;                         // Flag di esclusione lettura PLC
         short int  Typ;                             // Tipo Missione:
                                                     // 0     = Posizionamento semplice
                                                     // 1-3-5 = Ricerca posizione per carico
                                                     // 2-4-6 = Ricerca posizione per scarico
          short int  TypPresa;                       // Tipo presa (forche previste 1=(A)lte 2=(B)asse 3=A+B))
          short int  KmPercorsi;                     // KmPercorsi dal TGV
          short int  pstart,                         // Punto start missione
                     pend,                           // Punto finale
                     pintstart,                      // Punto intermedio start (falso obbiettivo)
                     pintend,                        // Punto intermedio end   (falso obbiettivo)
                     pintendtrasmesso;               // Punto intermedio end   (falso obbiettivo trasmesso)
               bool  ResetAlVolo;                    // Gestione cambio missione al volo
              BYTE   buff1;                          // Variabile ausiliaria
          short int  buff2;                          // Variabile ausiliaria
               int   Chiamata;                       // Numero chiamata da asservire
         short int   PIntForzato;                    // Punto Intermedio obbligato
         short int   OldPos,                         // Old posizione prima dello spegnimento
                     OldRot,                         // Old rotazione su calamite
                     OldStart;                       // Old valore START prima dello spegnimento
  unsigned int       OldComandi;                     // Old Bit di stato prima dello spegnimento
  unsigned short int old_allarm[MAXLRALLARMI+1];     // Vecchi valori corrispondenti agli
                                                     // LR di allarme letti da AGV
  unsigned short int old_allarme_interno;            // allarmi interni all'AGV:
              bool   StopManuale;                    // Gestione dello stop sul punto intermedio
                                                     // manuale
              bool   TGVInCambioMissione;            // TGV fermo per cambio missione
           short int TypPath;                        // Tipo PATH SCELTO DAL TGV
  unsigned short int punto[MAXPERCORSI];             // Array di punti percorso
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];       // Array di dati del percorso
  unsigned short int step[MAXSTEP];                  // Array di step  programma
  unsigned short int punto_prenotato[MAXPERCORSI];   // Array di punti percorso
                                                     // da pos. attuale a dest
               int  Provenienza;                     // Provenienza del Carrello
         short int  NodIngMag;                       // Contiene il numero del NODO di ingresso/uscita del magazzino
                                                     // (quando in realtà il PEnd è una posizione intermedia)
         short int  NumMissioni;                     // Conteggio del numero delle missioni inizializzate
         short int  TimeRipristino;                  // Tempo di ripristino Tgv dopo allarme
         short int  PosRicarica;                     // Posizione per ricarica Tgv.
         short int  PosAttesa;                       // Posizione per attesa Tgv prima della ricarica
            double  RapEncoder;                      // Rapporto encoder.
            double  buff4;                           // Variabile ausiliaria
            TColor  Col;                             // Colore della navetta.
              bool  NoMissioniAlVolo;                // Disattiva missioni al volo.
              bool  GestioneLaser;                   // Attivazione gestione laser
         short int  MacInIngombro;                   // Numero macchina ingombrata dal TGV.
         short int  BaiaInIngombro;                  // Numero baia ingombrata dal TGV.
              bool  PorInIngombro[MAXPORTEIMP+1];    // Numero porta ingombrata dal TGV.
         short int  AsservMac[MAXCHIAMATE+1];        // Elenco delle macchine in asservimento dal TGV
         short int  TipoAssMac[MAXCHIAMATE+1];       // Tipo asservimento decestonatori
         short int  AsservBaie[MAXGRUPPIIMP+1];      // Asservimento gruppi baie
              BYTE  VoltBatteria;                    // Volt Batteria;
              BYTE  PercCaricaBatt;                  // Percentuale di carica batteria;
         short int  PesoLordo;                       // Peso lordo;
         short int  Zona;                            // Zona posizione del TGV
//
// DATI PRESENZA A BORDO
//
//
// DATI PRESENZA A BORDO
//
// presenza cestone B - BASSA
         short int  PresCEST_B;                      // Presenza di un cestone sulle forche BASSE
                                                     // !=0 - chiamata annullata
            struct  BOX BoxB;                        // Dati prodotto semilavorato.
// presenza cestone A - ALTA
         short int  PresCEST_A;                      // Presenza di un cestone sulle forche ALTE
                                                     // !=0 - chiamata annullata
            struct  BOX BoxA;                        // Dati prodotto semilavorato.

// Gestione errori di comunicazioni
              bool  ErrCom;                          // Errore di comunicazione
// VARIABILI AUSILIARIE
              bool  var1;                            // Variabili Ausiliarie;
              bool  var2;                            // Variabili Ausiliarie;
              bool  var3;                            // Variabili Ausiliarie;
              bool  var4;                            // Variabili Ausiliarie;
              bool  var5;                            // Variabili Ausiliarie;
              bool  var6;                            // Variabili Ausiliarie;
              bool  var7;                            // Variabili Ausiliarie;
              bool  var8;                            // Variabili Ausiliarie;
              bool  var9;                            // Variabili Ausiliarie;
         short int  aux1;                            // Variabili Ausiliarie;
         short int  aux2;                            // Variabili Ausiliarie;
         short int  aux3;                            // Variabili Ausiliarie;
         short int  aux4;                            // Variabili Ausiliarie;
         short int  aux5;                            // Variabili Ausiliarie;
         short int  aux6;                            // Variabili Ausiliarie;
         short int  aux7;                            // Variabili Ausiliarie;
         short int  aux8;                            // Variabili Ausiliarie;
         short int  aux9;                            // Variabili Ausiliarie;
};

struct ALLTGV{
  char Time[11];                      // Conservo ora  dell'allarme
  char Date[11];                      // Conservo data dell'allarme
  short int Posizione;                // Recupero posizione allarme
  short int NumAgv;                   // Recupero numero del Tgv in allarme
  char Allarme[201];                  // Messaggio di errore
};


// ======================================
//  GESTIONE_ASRV
// ======================================
//
struct ASRV_stato1{                // definizione WORD1 stato ASRV
        unsigned mov_X:      1;    // ASRV in movimento X
        unsigned mov_Y:      1;    // ASRV in movimento Y
        unsigned mov_av:     1;    // ASRV in movimento AVANTI
        unsigned mov_ind:    1;    // ASRV in movimento INDIETRO
        unsigned allarme:    1;    // ASRV in ALLARME
        unsigned ok_prog:    1;    // ASRV in OK PROGRAMMA
        unsigned in_auto:    1;    // ASRV in AUTOMATICO
        unsigned in_man:     1;    // ASRV in MANUALE
        unsigned su_nod:     1;    // ASRV su NODO
        unsigned carico:     1;    // ASRV con CARICO a bordo
        unsigned sollev0:    1;    // ASRV SOLLEVAMENTO (bit0)
        unsigned sollev1:    1;    // ASRV SOLLEVAMENTO (bit1)
        unsigned ric_rst:    1;    // ASRV richiesta reset a PC
        unsigned aux13:      1;    // ASRV bit ausiliario.
        unsigned aux14:      1;    // ASRV bit ausiliario.
        unsigned aux15:      1;    // ASRV bit ausiliario.
};
struct ASRV_stato2{                // definizione WORD2 stato ASRV
        unsigned stop_X:     1;    // ASRV fermo in coordinata X
        unsigned stop_Y:     1;    // ASRV fermo in coordinata Y
        unsigned rot_X:      1;    // ASRV in rotazione verso dir X
        unsigned rot_Y:      1;    // ASRV in rotazione verso dir Y
        unsigned ricarica:   1;    // ASRV in ricarica
        unsigned cb_batt:    1;    // ASRV in cambio batteria
        unsigned manutenz:   1;    // ASRV in manutenzione
        unsigned aux7:       1;    // ASRV bit ausiliario.
        unsigned aux8:       1;    // ASRV bit ausiliario.
        unsigned aux9:       1;    // ASRV bit ausiliario.
        unsigned aux10:      1;    // ASRV bit ausiliario.
        unsigned aux11:      1;    // ASRV bit ausiliario.
        unsigned aux12:      1;    // ASRV bit ausiliario.
        unsigned aux13:      1;    // ASRV bit ausiliario.
        unsigned aux14:      1;    // ASRV bit ausiliario.
        unsigned aux15:      1;    // ASRV bit ausiliario.
};

struct ASRV_cmd{                   // definizione WORD comandi da PC a ASRV
        unsigned stop:       1;    // ASRV stop in emergenza
        unsigned aux1:       1;    // ASRV fermo in coordinata Y
        unsigned aux2:       1;    // ASRV in rotazione verso dir X
        unsigned aux3:       1;    // ASRV in rotazione verso dir Y
        unsigned aux4:       1;    // ASRV in ricarica
        unsigned aux5:       1;    // ASRV in cambio batteria
        unsigned aux6:       1;    // ASRV in manutenzione
        unsigned aux7:       1;    // ASRV bit ausiliario.
        unsigned aux8:       1;    // ASRV bit ausiliario.
        unsigned aux9:       1;    // ASRV bit ausiliario.
        unsigned aux10:      1;    // ASRV bit ausiliario.
        unsigned aux11:      1;    // ASRV bit ausiliario.
        unsigned aux12:      1;    // ASRV bit ausiliario.
        unsigned aux13:      1;    // ASRV bit ausiliario.
        unsigned aux14:      1;    // ASRV bit ausiliario.
        unsigned aux15:      1;    // ASRV bit ausiliario.
};


struct ASRV_cmd_step{              // definizione WORD passi missione da PC a ASRV
        unsigned mov_X:      1;    // ASRV in movimento X
        unsigned mov_Y:      1;    // ASRV in movimento Y
        unsigned mov_av:     1;    // ASRV in movimento AVANTI
        unsigned mov_ind:    1;    // ASRV in movimento INDIETRO
        unsigned sollev0:    1;    // ASRV Carico sollevamento0 (bit0)
        unsigned sollev1:    1;    // ASRV Carico sollevamento1 (bit1)
        unsigned scarica:    1;    // ASRV Scarico
        unsigned lenta:      1;    // ASRV in Lenta
        unsigned end:        1;    // ASRV ultimo passo programma
        unsigned aux9:       1;    // ASRV bit ausiliario.
        unsigned aux10:      1;    // ASRV bit ausiliario.
        unsigned aux11:      1;    // ASRV bit ausiliario.
        unsigned aux12:      1;    // ASRV bit ausiliario.
        unsigned aux13:      1;    // ASRV bit ausiliario.
        unsigned aux14:      1;    // ASRV bit ausiliario.
        unsigned aux15:      1;    // ASRV bit ausiliario.
};


struct ASRV_STATO{
           short int wdog;
           short int pos;
           short int start;
  struct ASRV_stato1 sbit1;              // Bit di stato1 ASRV
  struct ASRV_stato2 sbit2;              // Bit di stato2 ASRV
        short int ptr_step;              // Puntatore al passo di programma
        short int offset_da_nodo;        // impulsi da nodo
             WORD allarmi[MAXLRALLARMI]; // canali di allarme
        short int batteria;              // Percentuale batteria
        short int peso;                  // Peso
        short int aux1;                  // variabile ausiliaria
        short int aux2;                  // variabile ausiliaria
        short int aux3;                  // variabile ausiliaria
        short int aux4;                  // variabile ausiliaria
        short int aux5;                  // variabile ausiliaria
};

struct ASRV_COMANDI{
           short int wdog;
           short int start;
           short int end;
           short int pintend;            // Punto intermedio
           short int new_pos;            // Nuova posizione ASRV
     struct ASRV_cmd cbit;               // Bit di comando ASRV
};

struct ASRV_STEP{
     struct ASRV_cmd_step cmd;           // comando step missione
            short int ps;                // punto successivo
            short int dist;              // distanza da punto successivo
            short int rall;              // Rallentamento specifico nello Step (0..9)
            short int aux1;              // Variabile ausiliaria
            short int aux2;              // Variabile ausiliaria
            short int aux3;              // Variabile ausiliaria

};

struct ASRV_MISSION{
     struct ASRV_STEP step[MAXSTEP_ASRV];
            short int tot_step;
            short int p_start;
            short int p_end;
            short int typ_car;
            short int aux1;              // Variabile ausiliaria
            short int aux2;              // Variabile ausiliaria
            short int aux3;              // Variabile ausiliaria

};

// --------------------------------------
//   Gestione TELECAMERINA
// --------------------------------------
struct LOGTGV{
            long int Time;                         // Data/Ora del salvataggio del record
           short int Pos;                          // Posizione del TGV
           short int OffsetDaNodo;                 // Impulsi da Encoder nodo
           short int Rot;                          // Stato rotazione (LASER/CALAMITE)
           short int GLaser;                       // G guida laser
           short int Start;                        // Stato del bit di start
           short int PStart;                       // Punto Start missione
           short int PEnd;                         // Punto finale
           short int PIntStart;                    // Punto intermedio start (falso obbiettivo)
           short int PIntEnd;                      // Punto intermedio end   (falso obbiettivo)
           short int PIntForzato;                  // Punto intermedio forzato  (falso obbiettivo)
                 int Chiamata;                     // Numero chiamata da asservire
           short int NodIngMag;                    // Contiene il numero del NODO di ingresso/uscita del magazzino
                                                   // (quando in realtà il PEnd è una posizione intermedia)
  unsigned short int punto[MAXPERCORSI];           // Array di punti percorso
  unsigned short int punto_prenotato[MAXPERCORSI]; // Array di punti percorso prenotati
              bool  NodiOccupati[MAXPERCORSI];     // Viene messo a TRUE in corrispondenza
                                                   // dei nodi occupati dal TGV.
              bool  ResetAlVolo;                   // TGV programmato per Reset missione al volo
              bool  NoRiposoAuto;                  // Blocca il ritorno automatico a riposo
              bool  SemiAutomatico;                // Variabile che definisce che il TGV è in
                                                   // Gestione SEMIAUTOMATICA.
              bool  Esclusione;                    // Flag che indica che l'AGV è escluso
              bool  EsclPLC;                       // Flag di esclusione lettura PLC
              bool  StopManuale;                   // Stop manuale TGV da editor
              bool  TGVInCambioMissione;           // Falg di TGV fermo in attesa
         short int  Typ;                           // Tipo Missione:
                                                   // 0     = Posizionamento semplice
                                                   // 1-3-5 = Ricerca posizione per carico
                                                   // 2-4-6 = Ricerca posizione per scarico
         short int  TypPresa;                      // Tipo presa (forche previste 1=(A)lte 2=(B)asse 3=A+B))
         short int  TypPath;                       // Tipo percorso scelto dal TGV
         short int  KmPercorsi;                    // Km percorsi dal TGV
               int  Provenienza;                   // Provenienza del TGV.
         short int  MacInIngombro;                 // Numero macchina ingombrata dal TGV.
         short int  BaiaInIngombro;                // Numero Baia ingombrata dal TGV.
              bool  PorInIngombro[MAXPORTEIMP+1];  // Numero porta ingombrata dal TGV.
         short int  PresCEST_A;                    // Presenza di un cestone su postazione ALTA
         short int  PresCEST_B;                    // Presenza di un cestone su postazione BASSA
          bool PresaA[POSINPRESA+1][PIANIINPRESA+1];// Matrice di configurazione presa
                                                    // max 3 posizioni a terra
                                                    // max 2 posizioni in altezza (ammesse solo se pallet bassi)
          bool PresaB[POSINPRESA+1][PIANIINPRESA+1];// Matrice di configurazione presa
                                                    // max 3 posizioni a terra
                                                    // max 2 posizioni in altezza (ammesse solo se pallet bassi)
              BYTE  CodA;                           // Codice "A"
              BYTE  CodB;                           // Codice "B"
               int  LottoA;                         // Lotto "A"
               int  LottoB;                         // Lotto "B"
         short int  GrpDestA;                       // Gruppo destinazione "A"
         short int  GrpDestB;                       // Gruppo destinazione "B"
         short int  CarWashA;                       // CarWash "A"
         short int  CarWashB;                       // CarWash "B"
              bool  SovrapA;                        // Sovrap "A"
              bool  SovrapB;                        // Sovrap "B"
              bool  BassoA;                         // Basso "A"
              bool  BassoB;                         // Basso "B"
         short int  DestBloccati;                   // Destinazione bloccati
       unsigned int Comandi;                        // Bit di stato TGV
          short int allarme_interno;                // Stato degli allarmi Interni.
               WORD lr_allarmi[MAXLRALLARMI+1];     // Stato degli LR 6,7,8 di allarme.
               BYTE VoltBatteria;                   // Volt Batteria;
               BYTE PercCaricaBatt;                 // Percentuale di carica batteria;
};

// --------------------------------
//  Per TGV laser
//

struct LAYER{
  int Xl;                      // X Layer
  int Yl;                      // Y Layer
  int Al;                      // A Layer
  int Rl;                      // Raggio Layer (30 mt circa)
  int Nr;                      // N° di riflettori trovati
  int Minr;                    // N° Minimo di riflettori
  int Rr;                      // Raggio del riflettore
  int Xr[MAXRIFLETTORI+1];     // Coordinata X riflettore
  int Yr[MAXRIFLETTORI+1];     // Coordinata Y riflettore
  int ZAbiltata;               // Zona Abilitata alla navigazione
  int aux2;
};


// --------------------------------
// Per TGV
// Coordinate delle zone di mapping
//
struct ZONEMAPPING{
  int    X_0_layout;                  // offset X dell'origine del riferimento layout
  int    Y_0_layout;                  // offset Y dell'origine del riferimento layout
  int    X_0_laser;                   // offset X dell'origine del riferimento laser
  int    Y_0_laser;                   // offset Y dell'origine del riferimento laser
  double GRD_laser;                   // Angolo di rotazione Layout del TGV nel punto di mapping
  double GRD_offset;                  // Angolo di differenza tra rif. cartesiano e rif. laser
  int    ScostamentoPuntoLogico;      // Scostamento in mm tra "Punto logico" TGV e testa laser

  int    MaxZoneMappate;
  int    aux2;
  int    aux3;
  int    aux4;

  double aux5;
  double aux6;

  struct LAYER Z[MAXZONEMAPPING+1];
};

//-------------------------------------------
// Prototipo Struttura VARIABILI_OCCUPA_NODO
//-------------------------------------------
struct VARIABILI_OCCUPA_NODO{
//char        FaseOccupazioneStr[100];
  int         FaseE_PercorsoLibero;
  int         FaseE_NodoInterruzionePercorso;
  int         FaseE_AgvInIngombro;
  int         FaseE_SatelliteOccupato;

  int         FaseG3_NodoInterruzionePercorso;
  int         FaseG3_AgvInIngombro;
  int         FaseG3_SatelliteOccupato;

  int         FaseG4_0_EsisteZonaFuoriIngombro;

  int         FaseG4_1_NodoInterruzionePercorso;
  int         FaseG4_1_AgvInIngombro;
  int         FaseG4_1_SatelliteOccupato;


  int         FaseEnd;

};

//---------------------------------------
//  classe AGV
//---------------------------------------
class agv{
          public:
          struct   MISSION    mission;
          struct   STATO      stato;
          struct   ALLTGV     alltgv;
          struct   ZONEMAPPING zone_mapping;

          struct   rec_linea dati_path;

          short int  agv_num;                     //Numero dell' AGV

          short int      ptr_path,                //Puntatore al percorso
                         ptr_step,                //Puntatore step
                         old_rot;                 //Old rotazione 0(0 gradi) 1(90 gradi)
          struct    stat_comandi *sbit;           //Puntatore alla struct bit di stato
          struct    comandi *cbit;                //Puntatore a struct bit di comando

          char      filemapping[51];              //Nome del file di "Zone mapping"

          char      mission_name_sorg[100];       //Nome del file missione sorg
          char      mission_name_dest[100];       //Nome del file missione dest

  unsigned short int buf_prg[MAXSTEP];
  unsigned short int old_allarm[MAXLRALLARMI+1];  //Vecchi valori corrispondenti agli
                                                  //LR di allarme letti da AGV
  unsigned short int allarme_presa;               //ALLARMI DI PRESA TGV
                                                  //
  unsigned short int allarme_interno;             //Variabili che codificano alcuni allarmi:
                                                  // 1 - Scarico manuale
                                                  // 2 - Carico manuale
                                                  // 3 - Posizione errata
                                                  // 4 - IDBox==0 o IDBox duplicato
                                                  // 5 - Errore di percorso.
  unsigned short int old_allarme_interno;         //allarmi interni all'AGV:
                                                  // 0 : Nessun allarme
                                                  // 1 : Allarme BOX scaricato manualmente (caricare o azzerare dati)
                                                  // 2 : Allarme BOX caricato manualmente (scaricare o introdurre dati)
           WORD allarme_esterno[MAXLRALLARMI+1];  // Tengo memorizzato il valore degli LR di allarme.

           WORD dm88_stop_tgv;                    // DM88 di stop TGV per caduta consensi
           WORD dm1000_versione_allarmi;          // DM1000  versione allarmi TGV

           WORD PercorsiSpeciali;                 // Indice blocco percorsi 0=None; 1=Porta1; 2=Porta2 ecc..

          short int CicliAttesaPerAgvInReset;     // Cicli attesa TGV in cambio missione



          // Funzioni del modulo AGV.CPP
                agv(short int num_agv);           // creatore classe AGV
          int   crea_agv();

          int   load_stato(char *all_mess);
          int   stop_TGV_per_caduta_consensi(char *all_mess);
          int   write_allarmi_interni_a_plc(char *all_mess);
          int   AzzeraAllarmi(char *all_mess);
          int   load_mission(char *all_mess);
          int   save_mission(char *all_mess);
          int   delete_mission(char *all_mess);
          void  reset_ram_mission();
          void  reset_ram_mission_per_manuale();
          void  reset_chiamata_AGV();
          int   reset_mission(char *all_mess);

          int   test_mission_on();
          int   test_reset_on();
          int   test_agv_escluso();
          int   distanza(short int tipo_percorso, short int altro_punto, short int target, short int *nodi_perc, struct DATI_PERCORSO *dati_perc, char *all_mess, short int *num_nod, short int *num_rot);
          int   occupa_metri(short int pstart, short int pend, short int metri_max, short int *punto, short int *tot_nodi, short int *metri, char *all_mess);
          int   SelezioneTipoPath(bool deadlock, short int forza_tipo_path, short int pstart, short int pend, bool *froza_tutto, bool *tutto_libero);
          void  AvvicinamentoADestinazione(short int tipo_percorso, short int dest, short int passa_da, short int *pint);
          void  Rallentamenti();
          void  SaltaNodi();
          void  CambiaDirezione();
          int   BloccoRotazioni180();
          int   BloccoSuDeviazione();
          int   test_percorso_disponibile(short int *nodi_perc, struct DATI_PERCORSO *dati_perc, short int *altro_agv, short int *MinPercorsi, short int *MinPercorsiToStart,  short int *nodo_di_interruzione, char *all_mess);
          //int   test_percorso_disponibile_fino_a_dest(short int *nodi_perc, struct DATI_PERCORSO *dati_perc, short int *altro_agv, short int *MinPercorsi, short int *nodo_di_interruzione, char *all_mess);
          int   free_nodi(char *all_mess);
          int   free_all_nodi(char *all_mess);
          int   rot_standard(int ptr_percorso);
          int   occupa_nodi(char *all_mess, bool tutto, short int *agv_bloccante);
          int   occupa_nodi_ingombro_agv(char *all_mess, bool tutto, short int *agv_bloccante);
          int   rioccupa_nodi(char *all_mess);
          int   compila_prg(char *all_mess, int *NumNodo);
          int   compila_passo_prg(struct NOD *nod, char *all_mess);
          int   calcolo_grd_calamite(short int ptr_path, bool anti_oraria, long double *gradi, long double *impulsi);
          int   test_position_into_path(char *all_mess);
          int   trs_mission(char *all_mess);
          int   load_prg(WORD *prg, char *all_mess);

          int   trasmetti_reset_AGV( char *all_mess);
          int   trasmetti_new_pos_AGV(int pos, char *all_mess);
          int   test_percorso_to_target(short int *metri);
          int   history_allarmi();
          int   CompilaPercorsoGrafico(char *Intestazione);
          int   CompilaProgrammaGrafico(bool Plc);
          void  CompilaAllarmiAttuali();

          int   ReloadALLARMI(char *all_mess);
          int   SaveAllTgv(short int NumTgv, struct ALLTGV *Dati, char *all_mess);
          int   TrasmettiResetBloccoIngAGV(char *all_mess);
          //
          //  Gestione LASER
          //
          int   load_zone_mapping(char *all_mess);
          int   save_zone_mapping(char *all_mess);
          void  calcolo_zona_mapping(short int Punto, short int *Zona);
          void  convertiXY_laser_to_layout(double Xls, double Yls, double *Xly, double *Yly);
          void  convertiXY_layout_to_laser(double *Xls, double *Yls, double Xly, double Yly);
          void  convertiGRD_laser_to_layout(double GRDls,  double *GRDly);
          void  convertiGRD_layout_to_laser(double *GRDls, double GRDly);
          void  CalcoloNodoTgvLaser();

          void  SetResetPIntForzatoTGV();
          void  SpostaTGVInAttesa();
          void  AttivaPercorsoSpeciale();
          void  LiberaTgvInCambioMissione();

          // ======================================
          //  GESTIONE_ASRV
          // ======================================
          char   ASRV_file_stato_sorg[100];     // Nome del file missione sorg
          char   ASRV_file_stato_dest[100];     // Nome del file missione dest
          char   ASRV_file_mission_sorg[100];     // Nome del file missione sorg
          char   ASRV_file_mission_dest[100];     // Nome del file missione dest

          struct ASRV_STATO   asrv_stato;
          struct ASRV_COMANDI asrv_comandi;
          struct ASRV_MISSION asrv_mission;


          int  ASRV_load_plc_stato(WORD *dm, char *all_mess);
          int  ASRV_load_prg_mission(struct ASRV_MISSION *asrv_buffmiss, char *all_mess);
          int  ASRV_trs_mission(char *all_mess);
          int  ASRV_trs_command(char *all_mess);
          int  ASRV_compila_programma_grafico( bool Plc );
          int  ASRV_load_file_stato(char *all_mess);
          int  ASRV_save_file_stato(char *all_mess);
          int  ASRV_load_file_mission(char *all_mess);
          int  ASRV_save_file_mission(char *all_mess);
          void ASRV_attraversa_corsie(short int perc[], struct DATI_PERCORSO dati[], short int *new_perc, struct DATI_PERCORSO *new_dati);


          }; /*** class agv ***/

//---------------------------------------
//  classe  INGOMBRO GRAFICO AGV
//---------------------------------------
class ingombroagv{
  public:

  int NumAgv;
  int DistA;
  int DistI;
  int DistL;

  struct LINE{
     TPoint p1;
     TPoint p2;
  };

  struct LINE AreaOccupata[1000];
  //------------------------------------------------
  // Costruttore
  //------------------------------------------------
  ingombroagv(int NumAgv);
  //-------------------------------------------------------------------
  // Setta l'ingombro dell'Agv
  //-------------------------------------------------------------------
  int ingombroagv::SettaIngombroSuTratto(int NodoStart, int NodoEnd, int DirRot, char *all_mess);
  void ingombroagv::AzzeraIngombroAgv();
  void ingombroagv::calcola_ingombro_agv_su_singolo_punto(int PosX, int PosY, double rot, struct LINE *SegmentiIngombroAgv);

  int ingombroagv::VerificaIntersezioneAgv(ingombroagv *AltroAgv, char *all_mess);
  //-----------------------------------------------------------------------
  // Disegna l'ingombro settato dell'agv su sfondo grafico
  //-----------------------------------------------------------------------
  void ingombroagv::disegna_ingombro_agv(ingombroagv **ListaAgv, int TotAgv, Graphics::TBitmap *Sfondo);
  //---------------------------------------------------------------
  // Utility per calcoli ingombri
  //---------------------------------------------------------------
  int ingombroagv::ccw (TPoint p0, TPoint p1, TPoint p2);
  int ingombroagv::intersect(struct LINE l1, struct LINE l2);
  
};


struct PARAMETRI_COM{
           int  Plc;
           // SIEMENS
           int  RD_Area;         // DB
           int  RD_NumDB;
           int  RD_NrChIni;
           int  RD_NrCh;
           int  WR_Area;         // DB
           int  WR_NumDB;
           int  WR_NrChIni;
           int  WR_NrCh;
};

struct POS_STAZ_MAC{
             WORD QuotaP;             // Quota di deposito in profondità
// PREDISPOSIZIONE DOPPIA FORCA
//
// presenza cestone B - BASSA
         short int  PresCEST_B;                      // Presenza di un cestone BASSO
                                                     // !=0 - chiamata annullata
            struct  BOX BoxB;                        // Dati prodotto semilavorato.
// presenza cestone A - ALTA
         short int  PresCEST_A;                      // Presenza di un cestone ALTO
                                                     // !=0 - chiamata annullata
            struct  BOX BoxA;                        // Dati prodotto semilavorato.
// VARIABILI AUSILIARIE
               bool var1;             // Variabile Ausiliaria.
               bool var2;             // Variabile Ausiliaria.
               bool var3;             // Variabile Ausiliaria.
               bool var4;             // Variabile Ausiliaria.
               bool var5;             // Variabile Ausiliaria.
          short int aux1;             // Variabile Ausiliaria.
          short int aux2;             // Variabile Ausiliaria.
          short int aux3;             // Variabile Ausiliaria.
          short int aux4;             // Variabile Ausiliaria.
          short int aux5;             // Variabile Ausiliaria.
};

struct STAZIONE_MAC{
        short int RotLayout;                         // Rotazione a layout
        short int Tipo;                              // Tipo stazione:
                                                     // Ingresso    (SCARICO CON TGV)
                                                     // Uscita      (CARICO CON TGV)
                                                     // Falde       (CARICO/SCARICO CON TGV)
                                                     // Intermedia  (NESSUN CARICO/SCARICO)
        short int buff;                              // Variabile Ausiliaria.
        short int PuntoIng;                          // Nodo abbinato alla stazione (ingresso cestoni)
        short int PuntoOut;                          // Nodo abbinato alla stazione (uscita cestoni)
        short int Npt;                               // Numero max cestoni ammessi su stazione
        short int NumCestoni;                        // Numero cestoni presenti sul stazione
        short int Urgenza;                           // Urgenza impostabile da PC (costante da sommare ai sec di chiamata).
                                                     // dalle varie postazioni
         long int TimeChiamata;                      // data e ora chiamata
             bool Esclusione;                        // Esclusione stazione
             bool Blocco;                            // Stazione in blocco;
        short int BaiaRialzata;                      // Flag di baia rialzata
             bool NoCodice;                          // Codice non disponibile
             bool NoMagaz;                           // Magazzino di deposito non disponibile
// DATI LETTI/SCRITTI PLC (seconda del tipo di macchina)
             bool Richiesta;                         // Chiamata RICHIESTA PIENO
             bool Espulsione;                        // Chiamata ESPULSIONE PIENO
             bool NonConforme;                       // NON CONFORME
        short int StatoLav;                          // Stato lavorazione
        short int Codice;                            // Tipo materiale
        short int TipoBox;                           // PREVISTO NON GESTITO
        short int buff1;                             // Variabile Ausiliaria.
        //short int IDBox;                             // ID del carrello
// CONSENSI DA PLC A TGV
             bool OkIngresso;                        // OK ingresso per stazione FALDE
// CONSENSI DA TGV A PLC
             bool TgvInIngombro;                     // TGV in ingombro
             bool RicIngresso;                       // Richiesta ingresso
             bool FineOperazioneTGV;                 // Bit di fine operazione
             bool OkForcheAlte;                      // Ok forche fuori ingombro
// DATI BOX
     struct POS_STAZ_MAC Pos[MAXPOSTISTAZ+1];        // Dati legati alla singola posizione
// VARIABILI AUSILIARIE
               bool var1;                            // Variabile Ausiliaria.
               bool var2;                            // Variabile Ausiliaria.
               bool var3;                            // Variabile Ausiliaria.
               bool var4;                            // Variabile Ausiliaria.
               bool var5;                            // Variabile Ausiliaria.
               bool var6;                            // Variabile Ausiliaria.
               bool var7;                            // Variabile Ausiliaria.
               bool var8;                            // Variabile Ausiliaria.
               bool var9;                            // Variabile Ausiliaria.
               bool var10;                           // Variabile Ausiliaria.
          short int aux1;                            // Variabile Ausiliaria.
          short int aux2;                            // Variabile Ausiliaria.
          short int aux3;                            // Variabile Ausiliaria.
          short int aux4;                            // Variabile Ausiliaria.
          short int aux5;                            // Variabile Ausiliaria.
          short int aux6;                            // Variabile Ausiliaria.
          short int aux7;                            // Variabile Ausiliaria.
          short int aux8;                            // Variabile Ausiliaria.
          short int aux9;                            // Variabile Ausiliaria.
          short int aux10;                           // Variabile Ausiliaria.
               char aus1[11];                        // Variabile Ausiliaria.
               char aus2[11];                        // Variabile Ausiliaria.
               char aus3[11];                        // Variabile Ausiliaria.
               char aus4[11];                        // Variabile Ausiliaria.
               char aus5[11];                        // Variabile Ausiliaria.
};

struct CHIAMATE{
          short int Num;                             // Numero della Macchina
          short int Tipo;                            // Tipo di Macchina:
                                                     // 0 - Sconoscito
                                                     // 1 - DECESTONATORE
                                                     // 2 - CESTONATORE
                                                     // 3 - ...
               char Sigla[41];                       // Sigla della locazione
// PARAMETRIZZAZIONE COMUNICAZIONE
           struct PARAMETRI_COM Plc;                 // Paramentrizzazione comunicazione in lettura
               bool PlcErr;                          // Plc macchina in errore ricorsivo
               bool Esclusione;                      // Esclusione
               bool EsclComunic;                     // Esclusione comunicazione con DECESTONATORE
               bool FineProd;                        // Falg di fine produzione letto da PLC
          short int dist_box_in_mac;                 // Distanza Box in macchina (cm) per PLC TGV
          short int Priorita;                        // Priorità calcolata per la locazione
          short int StatoMac;                        // Memorizzazione Stato della Macchina Essicatoio es:
                                                     // 0=DaRiempire 1=Riempimento 2=Essicazione 3=Essicato
//    DATI PER SINGOLA MACCHINA
          short int Codice;                          // Codice legato alla macchina
          
// SEGNALI PER SINGOLA MACCHINA
               bool Blocco;                          // Macchine in blocco.
               bool Automatico;                      // Macchina in Automatico/Manuale
               bool RichIngMac;                      // Richiesta ingresso a macchina
               bool OkIngMac;                        // Ok accesso da macchina
               bool TgvIngMac;                       // Tgv in ingombro a macchina
               bool FineOpMac;                       // Fine operazione TGV a macchina
// STAZIONI
        struct STAZIONE_MAC Staz[MAXSTAZMAC+2];      // Elenco dati che compongono la stazione.
// ELENCO BOX DA CONSEGNARE IN MACCHINA
        short int IDRic[(MAXSTAZMAC*MAXPOSTISTAZ)+1];// Elenco BOX richiesti dalla macchina
        short int IDRicMan[MAXLISTABOX];             // Elenco BOX richiesti dalla macchina (MANUALE)
// ASSOCIAZIONE MAGAZZINI A MACCHINA
          short int MagAbbPieni[MAXMAGABB+1];        // Serve all'autoclave per sapere dove avvicinare i cestoni.
          short int MagAbbVuoti[MAXMAGABB+1];        // Serve direttamente al decestonatore per sapere dove portare i vuoti
                                                     //
// VARIABILI AUSILIARIE
               bool var1;                            // Porta essicatoio P1 OK
               bool var2;                            // Porta essicatoio P2 OK
               bool var3;                            // Variabile Ausiliaria.
               bool var4;                            // Variabile Ausiliaria.
               bool var5;                            // Variabile Ausiliaria.
               bool var6;                            // Variabile Ausiliaria.
               bool var7;                            // Variabile Ausiliaria.
               bool var8;                            // Variabile Ausiliaria.
               bool var9;                            // Variabile Ausiliaria.
               bool var10;                           // Variabile Ausiliaria.

          short int aux1;                            // Variabile Ausiliaria.
          short int aux2;                            // Variabile Ausiliaria.
          short int aux3;                            // Variabile Ausiliaria.
          short int aux4;                            // Variabile Ausiliaria.
          short int aux5;                            // Variabile Ausiliaria.
          short int aux6;                            // Variabile Ausiliaria.
          short int aux7;                            // Variabile Ausiliaria.
          short int aux8;                            // Variabile Ausiliaria.
          short int aux9;                            // Variabile Ausiliaria.
          short int aux10;                           // Variabile Ausiliaria.
               char aus1[11];                        // Variabile Ausiliaria.
               char aus2[11];                        // Variabile Ausiliaria.
               char aus3[11];                        // Variabile Ausiliaria.
               char aus4[11];                        // Variabile Ausiliaria.
               char aus5[11];                        // Variabile Ausiliaria.
};

struct BAIE{
          short int Num;                             // Numero della Macchina
               char Sigla[41];                       // Sigla della locazione
          short int Punto;                           // Numero del nodo baia
          short int PuntoAux[6];                     // Punti ausiliari BAIA
// PARAMETRI GESTIONE BAIA
          short int Tipo;                            // Tipo di Macchina:
                                                     // 0 - Sconoscito
                                                     // 1 - BAIA DI PRELIEVO
                                                     // 2 - BAIA DI DEPOSITO
                                                     // 3 - ...
               bool Chiamata;                        // Chiamata baia
          short int StatoPres;                       // Stato presenza in baia
          short int Gruppo;                          // Gruppo di lavoro di appartenenza
          short int buff;                            // Variabile Ausiliaria.
                                                     // ...
          short int GruppoDest;                      // Gruppo di lavoro di destinazione
          short int TgvInAsserv;                     // Tgv in asservimento
          short int aux10;                           // Variabile Ausiliaria.
          short int aux11;                           // Variabile Ausiliaria.
          short int aux12;                           // Variabile Ausiliaria.
// PARAMETR DI ABILITAZIONE/DISABILITAZIONE
               bool Esclusione;                      // Esclusione
               bool EsclComunic;                     // Esclusione comunicazione dati da tabella
               bool BloccoSovrap;                    // Blocco sovrapposizione in rulliera
          short int Priorita;                        // Priorità calcolata per la locazione
          short int Urgenza;                         // Urgenza impostabile da PC (costante da sommare ai sec di chiamata).
           long int TimeChiamata;                    // data e ora chiamata
          short int BaiaRialzata;                    // Altezza Baia da terra
               bool NoMagaz;                         // No magazzino (deposito/prelievo in base al tipo baia)
               bool NoTgvDisp;                       // No tgv disponibile
// GESTIONE CONSENSI
          short int Plc;                             // PLC di riferimento struttura COMUNICAZIONI
          short int PlcErr;                          // PLC in errore
               bool Automatico;                      // Baia in automatico
               bool Blocco;                          // Blocco baia
               bool TuttoPieno;                      // bit di tutto pieno
               bool OkForcheAlte;                    // bit di forche alte
               bool RichIng;                         // Richiesta ingresso a baia
               bool OkIng;                           // Ok accesso da baia
               bool TgvIng;                          // Tgv in ingombro a baia
               bool FineOp;                          // Fine operazione TGV a baia
          short int NumCicliResetFineOpraz;          // Memorizza FineOperazione da azzerare (numero di cicli)
//
// DATI DELLE PRIME 5 POSIZIONI DI PRESA IN RULLIERA (0 = PRIMA POSIZINE = POSIZIONE DI PRELIEVO)
//
           short int TipoCod[MAXPOSBAIA+1];                              // TIPO CODICE (per le 5 posizioni in profondità)
                                                                         // 0 - vuoto
                                                                         // 1 - pieno
                BYTE Codice[MAXPOSBAIA+1];                               // LINEA PROVENIENZA (per le 5 posizioni in profondità)
                BYTE buff4[MAXPOSBAIA+1];                                // array di variabili ausiliarie (per le 5 posizioni in profondità)
                 int Lotto[MAXPOSBAIA+1];                                // LOTTO (per le 5 posizioni in profondità)
           short int Altezza[MAXPOSBAIA+1];                              // ALTEZZA espressa in mm (per le 5 posizioni in profondità)
                bool Basso[MAXPOSBAIA+1];                                // PALLET BASSO (per le 5 posizioni in profondità)
                bool Sovrap[MAXPOSBAIA+1];                               // SOVRAPPOSIZIONE ALTO IN MAGAZZINO (per le 5 posizioni in profondità)
           short int TipoBox[MAXPOSBAIA+1];                              // TIPO PALLET:
                                                                         // 0. 1000x1200
                                                                         // 1. 800x1200
                                                                         //
           short int CarWash[MAXPOSBAIA+1];                              // Gestione Emergenza/No conforme
                bool Presa[MAXPOSBAIA+1][POSINPRESA+1][PIANIINPRESA+1];  // Matrice di configurazione presa
                                                                         // max 3 posizioni a terra
                                                                         // max 2 posizioni in altezza (ammesse solo se pallet bassi)
                bool Ruotato180[MAXPOSBAIA+1];                           // Pallet Ruotato di 180° (per le 5 posizioni in profondità)
                bool buff2[MAXPOSBAIA+1];                                // array di variabili ausiliarie (per le 5 posizioni in profondità)
                bool buff3[MAXPOSBAIA+1];                                // array di variabili ausiliarie (per le 5 posizioni in profondità)
           short int buff7[MAXPOSBAIA+1];                                // array di variabili ausiliarie (per le 5 posizioni in profondità)
           short int buff5[MAXPOSBAIA+1];                                // array di variabili ausiliarie (per le 5 posizioni in profondità)
           short int buff6[MAXPOSBAIA+1];                                // array di variabili ausiliarie (per le 5 posizioni in profondità)
//
// PREDISPOSIZIONE DOPPIA FORCA
//
// presenza cestone B - BASSA
         short int  PresCEST_B;                      // Presenza di un cestone BASSO
                                                     // !=0 - chiamata annullata
            struct  BOX BoxB;                        // Dati prodotto semilavorato.
// presenza cestone A - ALTA
         short int  PresCEST_A;                      // Presenza di un cestone ALTO
                                                     // !=0 - chiamata annullata
            struct  BOX BoxA;                        // Dati prodotto semilavorato.
// VARIABILI AUSILIARIE
               bool var1;                            // Variabile Ausiliaria.
               bool var2;                            // Variabile Ausiliaria.
               bool var3;                            // Variabile Ausiliaria.
               bool var4;                            // Variabile Ausiliaria.
               bool var5;                            // Variabile Ausiliaria.
               bool var6;                            // Variabile Ausiliaria.
               bool var7;                            // Variabile Ausiliaria.
               bool var8;                            // Variabile Ausiliaria.
               bool var9;                            // Variabile Ausiliaria.
               bool var10;                           // Variabile Ausiliaria.
          short int Sollevamento;                    // Quanto sollevare al carico/scarico
          short int aux1;                            // Variabile Ausiliaria.
          short int aux2;                            // Variabile Ausiliaria.
          short int aux3;                            // Variabile Ausiliaria.
          short int aux4;                            // Variabile Ausiliaria.
          short int aux5;                            // Variabile Ausiliaria.
          short int aux6;                            // Variabile Ausiliaria.
          short int aux7;                            // Variabile Ausiliaria.
          short int aux8;                            // Variabile Ausiliaria.
          short int aux9;                            // Variabile Ausiliaria.
               char aus1[11];                        // Variabile Ausiliaria.
               char aus2[11];                        // Variabile Ausiliaria.
               char aus3[11];                        // Variabile Ausiliaria.
               char aus4[11];                        // Variabile Ausiliaria.
               char aus5[11];                        // Variabile Ausiliaria.
};

struct GRUPPIBAIE{
               char Nome[31];                        // Nome del gruppo
          short int Tipo;                            // Tipo di gruppo
          short int Priorita;                        // Priorità per spostamento
             TColor Col;                             // Colore grafico assegnato
               bool var1;                            // Variabile Ausiliaria.
               bool var2;                            // Variabile Ausiliaria.
               bool var3;                            // Variabile Ausiliaria.
               bool var4;                            // Variabile Ausiliaria.
               bool var5;                            // Variabile Ausiliaria.
               bool var6;                            // Variabile Ausiliaria.
               bool var7;                            // Variabile Ausiliaria.
               bool var8;                            // Variabile Ausiliaria.
               bool var9;                            // Variabile Ausiliaria.
               bool var10;                           // Variabile Ausiliaria.
          short int aux1;                            // Variabile Ausiliaria.
          short int aux2;                            // Variabile Ausiliaria.
          short int aux3;                            // Variabile Ausiliaria.
          short int aux4;                            // Variabile Ausiliaria.
          short int aux5;                            // Variabile Ausiliaria.
          short int aux6;                            // Variabile Ausiliaria.
          short int aux7;                            // Variabile Ausiliaria.
          short int aux8;                            // Variabile Ausiliaria.
          short int aux9;                            // Variabile Ausiliaria.
          short int aux10;                           // Variabile Ausiliaria.
               char aus1[11];                        // Variabile Ausiliaria.
               char aus2[11];                        // Variabile Ausiliaria.
               char aus3[11];                        // Variabile Ausiliaria.
               char aus4[11];                        // Variabile Ausiliaria.
               char aus5[11];                        // Variabile Ausiliaria.
};

//-----------------------------------------------------
// Struttura delle porte lungo il percorso
//-----------------------------------------------------
struct PORTEIMP{
  short int Punto[11];                               // Nodi per apertura delle porte
  short int PuntoSemaforo[11];                       // Nodi su cui gestire il semaforo
  short int AnticipoRichiesta;                       // Anticipo richiesta porte
  bool      RichIngresso;                            // TRUE = Richiesta di ingresso da parte del TGV
  bool      TgvInIngombro;                           // TRUE = TGV in ingombro sotto il Palettizzatore
  bool      OkIngresso;                              // TRUE = Consenso all'ingresso da parte del Cel.
  bool      TgvConCarico;                            // TRUE = Attraversamento con carico a bordo.
  bool      AperturaParziale;                        // TRUE = Porta apera solo parzialmente
  bool      PortaInBlocco;                           // TRUE = Porta bloccata (o utilizzato da muletto)
  bool      TgvInArrivo;                             // TRUE = Tgv in attraversamento
  bool      DirUp;                                   // TRUE = Tgv verso l'alto
  bool      DirDown;                                 // TRUE = Tgv verso il basso
  bool      DirLeft;                                 // TRUE = Tgv verso sinistra
  bool      DirRight;                                // TRUE = Tgv verso destra
  bool      var1;                                    // Variabile ausiliaria
  bool      var2;                                    // Variabile ausiliaria
  bool      var3;                                    // Variabile ausiliaria
  bool      var4;                                    // Variabile ausiliaria
  bool      var5;                                    // Variabile ausiliaria
  bool      var6;                                    // Variabile ausiliaria
  bool      var7;                                    // Variabile ausiliaria
  bool      var8;                                    // Variabile ausiliaria
  bool      EsclusionePLC;                           // 05/09/2011 - Porta esclusa da PLC (attraversabile)
  short int PortaInterbloccata;                      // 22/06/2011 - Porta interbloccata
  short int aux2;                                    // Variabile ausiliaria
  short int aux3;                                    // Variabile ausiliaria
  short int aux4;                                    // Variabile ausiliaria
  short int aux5;                                    // Variabile ausiliaria
  short int aux6;                                    // Variabile ausiliaria
  short int aux7;                                    // Variabile ausiliaria
  short int aux8;                                    // Variabile ausiliaria
  short int aux9;                                    // Variabile ausiliaria
};

TIPO_VARIABILE char   porteimp_name_sorg[100];
TIPO_VARIABILE char   porteimp_name_dest[100];
TIPO_VARIABILE struct PORTEIMP PorteImp[MAXPORTEIMP+1];

int  load_file_porteimp(char *all_mess);
int  load_consensi_porteimp(char *all_mess);
int  write_consensi_porteimp(char *all_mess);
int  save_file_porteimp(char *all_mess);


struct CAMBIOBAT{
          short int Num;                             // Numero della Macchina
               char Sigla[41];                       // Sigla della locazione
          short int Punto;                           // Numero del nodo CambioBatteria
          short int PuntoAux[6];                     // Punti ausiliari MACC

// PARAMETRI GESTIONE CAMBIOBAT
          short int Tipo;                            // Tipo di Macchina:
          short int Gruppo;                          // Gruppo di lavoro di appartenenza
          short int aux1[9];                         // Variabile Ausiliaria.

// PARAMETRI DI ABILITAZIONE/DISABILITAZIONE
          short int Plc;                             // PLC di riferimento struttura COMUNICAZIONI
          short int PlcErr;                          // PLC in errore
               bool Esclusione;                      // Esclusione
               bool EsclComunic;                     // Esclusione comunicazione dati da tabella
               bool AbilitaReadWrite;                // Abilita ciclo comunicazione

// GESTIONE CONSENSI
          short int WatchDog;                        // WatchDog di lettura dalla macchina
          short int BitAllarmi[4];                   // Allarmi previsti per
               bool Automatico;                      // Macc in automatico
               bool Blocco;                          // Blocco Macc
               bool RichIng;                         // Richiesta ingresso a Macc
               bool OkIng;                           // Ok accesso da Macc
               bool TgvIng;                          // Tgv in ingombro a Macc
               bool FineOp;                          // Fine operazione TGV a Macc

// VARIABILI AUSILIARIE
               bool var1[10];                        // Variabile Ausiliaria.
          short int var2[20];                        // Variabile Ausiliaria.
          short int var3[20];                        // Variabile Ausiliaria.
               char aus1[11];                        // Variabile Ausiliaria.
               char aus2[11];                        // Variabile Ausiliaria.
               char aus3[11];                        // Variabile Ausiliaria.
               char aus4[11];                        // Variabile Ausiliaria.
               char aus5[11];                        // Variabile Ausiliaria.
};

//-----------------------------------------------------
// Struttura delle missioni di Semi-Automatico
//-----------------------------------------------------
struct SEMIAUTOMATICO{
  short int Tgv;                                   // Numero del TGV che deve eseguire la missione.
  short int Dest;                                  // Nodo di destinazione della missione.
  short int Mac;                                   // Macchina di destinazione.
  short int Baia;                                  // Baia di destinazione.
  short int Mag;                                   // Magazzino di destinazione.
  short int LatoMag;                               // Lato magazzino di destinazione (0 - default, 1 - ingresso, 2 - uscita)
  short int TypMiss;                               // Tipo di miss. da eseguire 0=Pos Semplice 1=Auicl 2=CesDec 3=Rull 4=Mag/Baie
  short int TypPresa;                              // Tipo presa (forche previste 1=(A)lte 2=(B)asse 3=A+B)
};

//--------------------
//  classe PROCESS
//--------------------
class process{

           public:

           short int rec_attuale;

           struct SEMIAUTOMATICO SemiAuto[11];                   // Struttura delle missioni in Semi-Automatico.
           bool   AbilitaSemiAutoCiclico;                        // Variabile di abilitazione semiautomatico continuo
        short int IdMissioneSemiAuto;                            // Indice missione semi automatica attiva

           struct CHIAMATE EditMac;
           struct CHIAMATE *ch[MAXCHIAMATE+1];

              int NumMaccCicloLettura;                           // Cicli di Lettura   macchina
              int NumMaccCicloScrittura;                         // Cicli di Scrittura macchina

           struct BAIE EditBaia;
           struct BAIE *ba[MAXBAIE+1];

           struct GRUPPIBAIE GruppiBaie[MAXGRUPPIIMP+1];

           char   chiamate_name_sorg[100];
           char   chiamate_name_dest[100];

           char   baie_name_sorg[100];
           char   baie_name_dest[100];

           char   gruppibaie_name_sorg[100];
           char   gruppibaie_name_dest[100];

           //------ cambiobatterie ------
           struct CAMBIOBAT EditCambioBatt;
           struct CAMBIOBAT *cb[MAXBATT+1];
           char   cbat_name_sorg[100];
           char   cbat_name_dest[100];

           bool   RunAutomaticoEseguito;

           // Funzioni di gestione dal file PROCESS.CPP

                process();

           int  load_incendio_in_corso(char *all_mess);
           int  load_chiamate_macchine (int *NumMaccCicloLettura,   char *all_mess);
           int  write_chiamate_macchine(int *NumMaccCicloScrittura, char *all_mess);
           int  setta_consensi_chiamate(char *all_mess);

           // BAIE
           int  read_baie(int num_baia, struct BAIE *bb, char *all_mess);
           int  load_file_baie(char *all_mess);
           int  save_record_baie(int numero, char *all_mess);
           int  save_file_baie(char *all_mess);
           int  test_punto_presente_baia(char *all_mess, short int punto, short int *num_baia);
           int  num_record_baie();
           int  cerca_record_baie(struct BAIE *bb, int numero);
           int  load_chiamate_baie(char *all_mess);
           int  write_chiamate_baie(char *all_mess);
           int  setta_consensi_baie(char *all_mess);

           // CAMBIO BATTERIA
           int  read_cbat(int num_cb, struct CAMBIOBAT *cbb, char *all_mess);
           int  load_file_cbat(char *all_mess);
           int  save_record_cbat(int numero, char *all_mess);
           int  save_file_cbat(char *all_mess);
           int  test_punto_presente_cbat(char *all_mess, short int punto, short int *num_cb);
           int  num_record_cbatt();
           int  cerca_record_cbat(struct CAMBIOBAT *cbb, int numero);
           int  setta_consensi_cbat(char *all_mess);
           int  load_consensi_cbat(int NumCBatt, char *all_mess);
           int  write_consensi_cbat(int NumCBatt, char *all_mess);

           // GRUPPI BAIE
           int  load_file_gruppibaie(char *all_mess);
           int  save_file_gruppibaie(char *all_mess);

           // CHIAMATE
           int  read_chiamate(int num_mac, struct CHIAMATE *mc, char *all_mess);
           int  load_file_chiamate(char *all_mess);
           int  save_record_chiamate(int numero, char *all_mess);
           int  save_file_chiamate(char *all_mess);
           int  test_punto_presente_mac(char *all_mess, short int punto, short int *num_mac, short int *num_stz);
           int  num_record_chiamate();
           int  cerca_record_chiamate(struct CHIAMATE *mc, int numero);

           int  VerificaTipoPrelievo(bool lato_ingresso, short int num_mac, short int num_stz, short int *Posizione, short int *PosizioneTgv, char *all_mess);
           int  VerificaTipoDeposito(bool lato_uscita,   short int num_mac, short int num_stz, short int *Posizione, short int *PosizioneTgv, char *all_mess);

           void OrdineMac(short int *OrdinaMac);
           int  GestioneChiamateMac_Ric(int macchina, char *all_mess);
           int  GestioneChiamateMac_Esp(int macchina, char *all_mess);

           void OrdineBaie(short int *OrdinaBaie);
           int  GestioneChiamateBaie_Ric(int baia, char *all_mess);
           int  GestioneChiamateBaie_Esp(int baia, char *all_mess);

           int  GestioneSpostaPallet(char *all_mess);

           int  RunAutomatico(char *all_mess);
           int  init_mission(bool deadlock, short int forza_tipo_path, short int num_agv, int chiamata, short int dest, short int pos_dest, short int pint, short int tipo_mission, short int TypPresa, bool *OK, char *all_mess);
           int  InizializzaMissioneFuoriIngombro(short int num_agv, bool *Ok, char *all_mess);
           int  ImpostazioneCurve(short int perc[], struct DATI_PERCORSO dati[], short int *new_perc, struct DATI_PERCORSO *new_dati, char *all_mess);
           int  ImpostazioneDeviazioni(short int num_agv, short int perc[], struct DATI_PERCORSO dati[], short int *new_perc, struct DATI_PERCORSO *new_dati, char *all_mess);
           int  maintenance_mission(short int num_agv, char *all_mess);
           int  SemiAutomatico(char *all_mess);
           int  PosizionamentoManualeAGV(short int num_agv, short int punto, short int TypPresa, bool *Ok, char *all_mess);
           int  CarScarMacManualeAGV(short int num_agv, short int num_mac, short int TypPresa, bool *Ok, char *all_mess);
           int  CarScarBaiaManualeAGV(short int num_agv, short int num_baia, short int TypPresa, bool *Ok, char *all_mess);
           int  CarScarMagManualeAGV(short int num_agv, short int num_mag, short int lato_mag, short int TypPresa, bool *Ok, char *all_mess);


           int  history(int num_agv, char *tipo);
           int  error_trasmit(int num_agv, char *messaggio);
           int  ConvertiTempCronoall(char *nome_file_source, char *nome_file_dest, short int NumAgv, char *all_mess);
           int  CalcolaTempo(long int TempoSecondi, char *Tempo);

           // Test chiamate dal file PRCHIAMA.CPP
           int  test_chiamata_asservita(int chiamata);
           int  test_dest_occupata(short int dest, short int *num_agv);
           int  test_dest_riservata(short int dest, short int *num_agv, short int mio_agv);
           int  test_dest_limitrofe(short int dest, short int *num_agv, short int *vicino, short int mio_agv);
           void TestNumChiamata(int *chiamata, short int Dest, short int Num);
           bool TestChiamataNum(int chiamata, short int *num_mag, short int *num_baia, short int *num_mac, short int *num_stz);
           bool TestProvenienzaBox(int provenienza, short int *tipo, short int *num_mac, short int *num_baia, short int *num_mag);
           int  TestChiamata(short int Dest, short int Num);
           void AssegnaPriorita();
           int  AgvConCaricoDestMagazzino(char *all_mess);
           int  CercaAgvLibero(bool tutto, short int dest, int chiamata, short int tipo_chiamata, short int zona, short int *num_agv, char *all_mess);
           int  CercaAgvLiberoPerReset(bool tutto, short int dest, int chiamata, short int tipo_chiamata, short int zona, short int *num_agv, char *all_mess);

           int  CercaAgvConCaricoPIENO(bool tutto, short int dest, int chiamata, short int tipo_prov_box, short int gruppo_dest_box, short int *num_agv, char *all_mess);
           int  CercaAgvConCaricoVUOTO(bool tutto, short int dest, int chiamata, short int tipo_prov_box, short int tipo_carico, short int tipo_box, short int *num_agv, char *all_mess);

           // Conclusione delle missioni PRCARICA.CPP
           int  CaricoDaMacchina(short int num_agv, char *all_mess);
           int  CaricoDaBaia(short int num_agv, char *all_mess);
           int  CaricoDaMagazzino(short int num_agv, char *all_mess);
           int  ScaricoSuMacchina(short int num_agv, char *all_mess);
           int  ScaricoSuBaia(short int num_agv, char *all_mess);
           int  ScaricoSuMagazzino(short int num_agv, char *all_mess);

           // GESTIONE ID RISERVATO
           bool CercaIDRisevatoMac(short int ID, short int *num_mac);
           bool CercaIDInMac(short int ID, short int *num_mac, short int *num_stz, short int *num_pos);

           // Simulazione TGV
           int  CicloSimulazioneAGV();

           // Procedura di sblocco TGV
           void SbloccoDeadLock();

           // Posizionamento in ricarica
           int  PosizioneRiposoAgv(short int NumAgv, char *all_mess);

           // Fuori ingombro per Incendio
           int  FuoriInGombroPortePerIncendio(short int NumAgv, char *all_mess);
           int  LogMovimenti(char *Operazione, char *Coordinate, short int num_agv, struct BOX BoxB, struct BOX BoxA, short int TipoPresa, int NMacc);
};


struct POS_MAG{
             bool Presenza[MAXPIANIMAG+1];           // Presenza al piano
             WORD QuotaH[MAXPIANIMAG+1];             // Quota di deposito in altezza
             WORD QuotaP[MAXPIANIMAG+1];             // Quota di deposito in profondità
      struct BOX Box[MAXPIANIMAG+1];                 // Dati legati a ogni piano del magazzino
// VARIABILI AUSILIARIE
               bool var1[MAXPIANIMAG+1];             // Variabile Ausiliaria.
               bool var2[MAXPIANIMAG+1];             // Variabile Ausiliaria.
               bool var3[MAXPIANIMAG+1];             // Variabile Ausiliaria.
               bool var4[MAXPIANIMAG+1];             // Variabile Ausiliaria.
          short int aux1[MAXPIANIMAG+1];             // Variabile Ausiliaria.
          short int aux2[MAXPIANIMAG+1];             // Variabile Ausiliaria.
          short int aux3[MAXPIANIMAG+1];             // Variabile Ausiliaria.
// VARIABILI AUSILIARIE
          short int buff1;                           // Variabile Ausiliaria.
          short int buff2;                           // Variabile Ausiliaria.
          short int buff3;                           // Variabile Ausiliaria.
          short int buff4;                           // Variabile Ausiliaria.
               bool buff5;                           // Variabile Ausiliaria.
               bool buff6;                           // Variabile Ausiliaria.
               bool buff7;                           // Variabile Ausiliaria.
               bool buff8;                           // Variabile Ausiliaria.
          short int NodoIng;                         // Nodo singola posizione INGRESSO
          short int MaxHPos;                         // Altezza massima posizione
          short int NodoOut;                         // Nodo singola posizione USCITA
};



struct MAG{
          short int Num;                             // Numero del MAGAZZINO
          short int PuntoIng;                        // Nodo abbinato al deposito materiale (INGRESSO)
          short int PuntoOut;                        // Nodo abbinato al prelievo materiale (USCITA  )
               BYTE Gruppo;                          // Gruppo magazzino
               BYTE Tipo;                            // Tipo di MAGAZZINO:
                                                     // 0 - Sconoscito
                                                     // 1 - Cestoni prodotto (vuoti/pieni)
                                                     // 2 - Zavorre
                                                     // 3 - Falde piene
                                                     // 4 - Falde vuote
          short int Stato;                           // Stato magazzino
                                                     // 0 - Libero
                                                     // 1 - Non libero (vuoti)
                                                     // 2 - Non libero (pieni)
               bool Fifo;                            // Gestione FIFO del MAGAZZINO
               bool Completo;                        // Massima disponibilità raggiunta
          short int Npt;                             // Numero posizioni magazzino ( a terra )
          short int NumCestoni;                      // Numero posizioni occupate (totali
               BYTE Sovrapposizione;                 // Consenso alla sovrapposizione
               BYTE aux;                             // Variabile Ausiliaria.
          short int TipoBox;                         // Tipo box ammesso in magazzino
                int Assegnazione;                    // Codifica numerica per gestire l'abbinamento
                                                     // del magazzino a un macchina, baia,....
                                                     // A seconda degli impianti:
                                                     // ALLGLASS: linea di provenienza materiale
           long int Time;                            // Data primo inserimento
        struct POS_MAG Pos[MAXTERRAMAG+1];           // Array dati sigola posizione

// CONFIGURAZIONE DEPOSITO
          short int Profondita;                      // Area di deposito in cm
          short int BaiaRialzata;                    // Flag di baia rialzata
          short int OffSetDaNodo;                    // Offset da nodo sul primo deposito

// VARIABILI AUSILIARIE
               BYTE PerEmergenza;                    // Abilitazione a contenere solo pallet in CAR-WASH
               BYTE PerPalRuotati;                   // Attivazione Pallet ruotati
               bool var3;                            // Variabile Ausiliaria.
               bool var4;                            // Variabile Ausiliaria.
               bool var5;                            // Variabile Ausiliaria.
          short int Sollevamento;                    // Sollevamento da Terra.
          short int aux2;                            // Variabile Ausiliaria.
          short int aux3;                            // Variabile Ausiliaria.
          short int aux4;                            // Variabile Ausiliaria.
          short int aux5;                            // Variabile Ausiliaria.
               char aus1[21];                        // Variabile Ausiliaria.
               char aus2[21];                        // Variabile Ausiliaria.
               char aus3[21];                        // Variabile Ausiliaria.
               char aus4[21];                        // Variabile Ausiliaria.
               char aus5[21];                        // Variabile Ausiliaria.
};

struct TIPIBOX{
               char Nome[31];                        // Nome del gruppo
          short int Lung;                            // Lunghezza (cm)
          short int Larg;                            // Larghezza (cm)
          short int offset_tgv_da_nodo_mag;          // Offset da nodo legato a tipo box in magazzino
          short int dist_box_in_magaz;               // Distanza tra le palette in magazzino (cm)
             double eccesso_count_pos_mag;           // Valore decimale oltre quale viene arrotodonato alla pos. successiva
               bool var1;                            // Variabile Ausiliaria.
               bool var2;                            // Variabile Ausiliaria.
               bool var3;                            // Variabile Ausiliaria.
               bool var4;                            // Variabile Ausiliaria.
               bool var5;                            // Variabile Ausiliaria.
          short int Altez;                           // Altezza (cm).
          short int aux2;                            // Variabile Ausiliaria.
          short int aux3;                            // Variabile Ausiliaria.
          short int aux4;                            // Variabile Ausiliaria.
          short int aux5;                            // Variabile Ausiliaria.
             double aus1;                            // Variabile Ausiliaria.
             double aus2;                            // Variabile Ausiliaria.
             double aus3;                            // Variabile Ausiliaria.
             double aus4;                            // Variabile Ausiliaria.
             double aus5;                            // Variabile Ausiliaria.
};

//--------------------
//  classe MAGAZ
//--------------------
class magaz{                          // definizione classe  Magazzini

    public:

    short int rec_attuale;

    struct MAG  EditMag;
    struct MAG  *m[MAXMAG+1];


    struct TIPIBOX TipiBox[MAXTIPOBOXIMP+1];

    short int AccoppiaMag[MAXMAG+1];


    char   mag_name_sorg[100];
    char   mag_name_dest[100];

    char   tipibox_name_sorg[100];
    char   tipibox_name_dest[100];

    // Funzioni dal file MAG.CPP
           magaz();     // creatore classe magazzino

           // TIPIBOX
           int  load_file_tipibox(char *all_mess);
           int  save_file_tipibox(char *all_mess);

           // MAGAZZINI
           int  read_mag(int num_mag, struct MAG *mg, char *all_mess);
           int  load_file_mag(char *all_mess);
           int  save_record_mag(int numero, char *all_mess);
           int  save_file_mag(char *all_mess);
           int  test_punto_presente_mag(char *all_mess, short int punto, short int *num_mag, short int *num_pos);
           int  num_record_mag();
           int  cerca_record_mag(struct MAG *mg, int numero);


           // FUNZIONI DI SELEZIONE TIPO PRELIEVO/DEPOSITO IN MAGAZZINO

           int  VerificaTipoDeposito(bool lato_uscita, short int num_mag, short int num_pos, short int TypPresa, short int TipoBox, bool Presa[POSINPRESA+1][PIANIINPRESA+1], bool Sovrapponibile, bool Basso, bool Salta, short int *Posizione, short int *Piano, short int *PosizioneTgv, char *all_mess);
           int  VerificaTipoPrelievo(bool lato_ingresso, short int num_mag, short int TypPresa, short int *Posizione, short int *Piano, short int *PosizioneTgv, char *all_mess);

           // CONVERTE DELLE QUOTE RECUPERATE DAL PLC IN POSIZIONI IN MAGAZZINO
           int  ConvertiQuotePLC(bool lato_uscita, short int num_mag, double RapEncP, short int tipo_box, WORD QuotaP, WORD QuotaH_A, WORD QuotaH_B, short int TypPresa, short int Posizione, short int Piano, short int *TypPresaPLC, short int *PosizionePLC, short int *PianoPLC, char *all_mess);

           // CERCA SE UN ID E' PRESENTE IN MACCHINA
           bool CercaIDInMag(short int ID, short int *num_mag, short int *num_pos, short int *num_pin);

           int  CercaMagDepositoPIENO(bool libero, short int singolo_mag, bool abilita_controllo_nodo, bool deposito_nel_piu_vicino, bool lato_uscita, bool disp_sovrap, short int gruppo_dest, short int *num_mag, short int *num_pos, short int *num_pin, int provenienza, short int num_agv, short int typ_presa, short int tipo_carico, short int tipo_box, bool presa[POSINPRESA+1][PIANIINPRESA+1], short int codice, int lotto, short int car_wash, bool basso, bool rotaz, struct DATI_PRD *Udc, bool presa_sovrapponibile, char *all_mess);

           int  CercaMagTempDeposito(bool libero, short int singolo_mag, bool abilita_controllo_nodo, bool lato_uscita, bool disp_sovrap, short int gruppo_dest, short int *num_mag, short int *num_pos, short int *num_pin, int provenienza, short int num_agv, short int typ_presa, short int tipo_carico, short int tipo_box, bool presa[POSINPRESA+1][PIANIINPRESA+1], short int codice, int lotto, short int car_wash, bool basso, bool rotaz, struct DATI_PRD *Udc, bool presa_sovrapponibile, char *all_mess);

           int  CercaMagPrelievoPIENO(bool incompleto, short int singolo_mag, bool abilita_controllo_nodo, bool lato_ingresso, short int gruppo_dest, short int *num_mag, short int *num_pos, short int *num_pin, short int num_agv, short int typ_presa, short int gruppo_baia, char *all_mess);
           int  CercaMagPrelievoVUOTO(bool incompleto, short int singolo_mag, bool abilita_controllo_nodo, bool lato_ingresso, short int gruppo_dest, short int *num_mag, short int *num_pos, short int *num_pin, short int num_agv, short int typ_presa, short int tipo_carico, short int tipo_box, char *all_mess);

           int  CercaMagDaSvuotarePIENO(bool incompleto, bool lato_ingresso, bool non_raggiungibile, short int tipo_mag, short int gruppo_dest, short int num_agv, short int typ_presa, short int *num_mag, short int *num_pos, short int *num_pin, short int *pos_spost, short int *pin_spost, bool *cambia_rot);
           int  CercaMagRiempirePIENO(bool libero, bool abilita_controllo_nodo, bool lato_uscita, bool disp_sovrap, short int singolo_mag, short int gruppo_dest, short int *num_mag, short int *num_pos, short int *num_pin, int provenienza, short int num_agv, short int typ_presa, short int tipo_carico, short int tipo_box, bool presa[POSINPRESA+1][PIANIINPRESA+1], short int codice, int lotto, short int car_wash, bool basso, bool rotaz, struct DATI_PRD *Udc, bool presa_sovrapponibile, char *all_mess);
};

//-------------------------------------------------------------
// Esegue alcuni controlli prima di inizializzare la missione
//-------------------------------------------------------------
int  new_init_mission(bool deadlock, short int forza_tipo_path, short int num_agv, int chiamata, short int dest, short int pos_dest, short int pint, short int tipo_mission, short int TypPresa, bool *Ok, char *all_mess);
void FileTraccia(char *messaggio);
void FileLogOperatore(char *messaggio);



//=============================================================
// Prototipo Struttura dei TEMPI TGV ---> Gestione DB4
//=============================================================
/***********************************************************************************************
struct TIMTGV{
     char DataAttuale[11];                // Data attuale  ---> Chiave di Ricerca DB4
     char TimeAttuale[51][13];            // Data/Ora attuali
     char TimeUltimo[51][13];             // Data/Ora da ultima lettura
     char TimeAutomatico[51][13];         // Data/Ora con TGV in Automatico
     char TimeSemiAutomatico[21][13];     // Data/Ora con TGV in SemiAutomatico
     char TimeRicarica[51][13];           // Data/Ora con TGV in Ricarica
     char TimeRiserva[51][13];            // Data/Ora con TGV in Riserva
     char TimeAllarme[51][13];            // Data/Ora con TGV in Allarme
     char TimeAttivo[51][13];             // Data/Ora con TGV in Missione Attiva
     char NumMissioni[51][13];            // Numero missioni giornaliere
     char Aux1[51][13];                   // Variabili ausiliaria
     char Aux2[51][13];                   // Variabili ausiliaria
     char Aux3[51][13];                   // Variabili ausiliaria
};
***********************************************************************************************/
struct TIMTGV{
     char DataAttuale[11];                              // Data attuale  ---> Chiave di Ricerca DB4
     char Time[MAXTGV_TIMTGV+1][MAXFIELD_TIMTGV+1][13]; // Data/Ora attuali
};

TIPO_VARIABILE char DescRigheStatTGV[MAXFIELD_TIMTGV+10][61]; // diciture Intestazione Righe Lista

TIPO_VARIABILE char FileTabellaTimTgv[81];      // Nome della Tabella che gestisce il DB4
                                                // dei Tempi TGV.
TIPO_VARIABILE  TTable *TimTgvTable;            // Puntatore globale alla Tabella
                                                // delle Statistiche TGV.
TIPO_VARIABILE  bool TimTgvTableNonAccessibile; // Blocco tutti tentativi di accesso alla
                                                // Tabella.
TIPO_VARIABILE  long int LastRefreshTimTgv;     // Sec ultimo aggiornamento

void TimTgvInizializzaTabella(char *Percorso, char *NomeTabella, TTable *Tabella);
int  TimTgvCreaTabella();
void TimTgvGetCampiTabella(struct TIMTGV *Record, TTable *Tabella);
void TimTgvSetCampiTabella(struct TIMTGV *Record, TTable *Tabella);
void TimTgvCompattaTabella( TTable *Tabella);
int  TimTgvCercaPrimoRecord(struct TIMTGV *Record, char *NomeIndice, char *all_mess);
int  TimTgvCercaUltimoRecord(struct TIMTGV *Record, char *NomeIndice, char *all_mess);
//int  TimTgvCercaRecord(struct TIMTGV *Record, char *NomeIndice, char *Chiave, char *all_mess);
int  TimTgvCercaRecord(struct TIMTGV *Record, char *NomeIndice, char *Chiave1, long int Chiave2, bool VisMsgBox, char *all_mess);
void TimTgvAggiornaDB();

// ---------------------------------------------------------
//      dal file    GRAPHVAR.CPP e GRAPHWIN.CPP
// ---------------------------------------------------------
//
// funzioni non appartenenti alle classi
void setta_nodi_contrapposti();
void setta_pt_in_ingombro();
void setta_nodi_in_ingombro();
void setta_nodi_in_ingombro_per_porte();
void setta_nodi_minpercorsi();
int  SettaZonaSpeciale(int NumAgv, int ZonaSpeciale, int NodoTarget);

//===========================================================================
// Settaggio configurazioni impianto
//===========================================================================
void SettaConfigurazioneImpianto();

//===========================================================================
//  ( I/O REMOTATI ) Funzioni personalizzate di lettura da I/O remotati
//===========================================================================
int Graf_IORemoto_LoadCanali( char *all_mess );
int Graf_IORemoto_WriteCanali( char *all_mess );

//-------------------------------------------------------------
// Fuzioni di carattere Globale
//-------------------------------------------------------------
double sqrt_algo( double Variabile);
int load_GRUPPI(char *filename);
int load_TIPOBOX(char *filename);
int load_ALLMAC(char *filename);
int load_ALLCBAT(char *filename);

//-------------------------------------------------------
// Definizione delle classi di sistema
//-------------------------------------------------------
TIPO_VARIABILE agv           *AGV[MAXAGV+1];                             // classe AGV
TIPO_VARIABILE ingombroagv   *IngombroAgvGrafico[MAXAGV+1];              // classe visualizzazione INGOMBRO GRAFICO AGV
TIPO_VARIABILE nodo           N;                                         // classe NODO
TIPO_VARIABILE process        P;                                         // classe PROCESS
TIPO_VARIABILE magaz          M;                                         // classe MAGAZ
TIPO_VARIABILE box            B;                                         // classe BOX

TIPO_VARIABILE char DESCGRUPPI[MAXGRUPPIIMP+1][31];                      // diciture GRUPPI (da file TXT)
TIPO_VARIABILE char DESCTIPOBOX[MAXTIPOBOXIMP+1][31];                    // diciture TIPI BOX (da file TXT)
TIPO_VARIABILE char DESCALLMAC[MAX_ALLARMI_MAC+1][61];                   // diciture Allarmi Macchina (da file TXT)
TIPO_VARIABILE char DESCALLCBAT[MAX_ALLARMI_CBAT+1][61];                 // diciture Allarmi CambioBatteria (da file TXT)

//-------------------------------------------------------
// Oggetti dinamici di gestione del DBase SQL
//-------------------------------------------------------
TIPO_VARIABILE short int   MinRiconnessioneSQL;                          // Minunti dopo i quali ritentare la connessione SQL
TIPO_VARIABILE short int   SecConnectioTimeOutSQL;                       // Secondi di tentativo connessione SQL
TIPO_VARIABILE short int   SecComandoTimeOutSQL;                         // Secondi di tentativo in comando SQL
TIPO_VARIABILE short int   SecDelayOnErrSQL;                             // Attesa su prossima comunicazione in caso di errore
TIPO_VARIABILE short int   SecReloadOnThreadSQL;                         // Secondi elaborazione Thread (0 - disattivato)
TIPO_VARIABILE bool        NoLockQuery;                                  // Compilazione stringhe Query con istruzione "WITH (no lock)" allegata
TIPO_VARIABILE bool        AggiornaLogSQL;                               // Generazione LOG di scrittura su SQL
TIPO_VARIABILE AnsiString  ProviderADO;                                  // Selezione del provider ADO di connessione
TIPO_VARIABILE AnsiString  NomeSRVSQL;                                   // Nome del SERVER SQL di collegamento (solo per SQL SERVER).
TIPO_VARIABILE AnsiString  NomeDBase;                                    // Nome DATA BASE DI SCAMBIO KRAFT (Labeltech)
TIPO_VARIABILE AnsiString  NomeTabA;                                     // Nome TABELLA ANAGRAFICA su DB_KRAFT
TIPO_VARIABILE AnsiString  DBasePsw;                                     // Password di accesso al DataBase
TIPO_VARIABILE AnsiString  DBaseUser;                                    // User ID di accesso al DataBase
//-------------------------------------------------------
// Rapporto di riduzione del layout grafico
//-------------------------------------------------------
TIPO_VARIABILE float Rid;

//-------------------------------------------------------
// Variabili di Impostazione SISTEMA
//-------------------------------------------------------
TIPO_VARIABILE AnsiString PathProject;                                   // Path del progetto
TIPO_VARIABILE AnsiString PathConfig;                                    // Path del progetto
TIPO_VARIABILE AnsiString Pass0;                                         // Password a Livello 0.
TIPO_VARIABILE AnsiString Pass1;                                         // Password a Livello 1.
TIPO_VARIABILE AnsiString Pass2[11];                                     // Array di password a Livello 2.
TIPO_VARIABILE AnsiString Pass3[11];                                     // Array di password a Livello 3.
TIPO_VARIABILE AnsiString PlantName;                                     // Nome impianto ALL-GLASS
TIPO_VARIABILE bool RiposoFlottante;                                     // Flag per gestione ricarica flottante.
TIPO_VARIABILE bool VisualizzaZoneImpianto;                              // Visualizzazione delle zone dell'impianto.
TIPO_VARIABILE bool AttivaPassword3Veloce;                               // Attivazione della password veloce di sistema
TIPO_VARIABILE bool DisattivaDelaySpeciale;                              // Attiva/Disattiva delay speciale
TIPO_VARIABILE bool ManualeConNodiOccupati;                              // Mantiene i nodi occupati con il TGV in manuale
TIPO_VARIABILE bool AbilitaRiposoFlottante;                              // Abilitazione riposo flottante
TIPO_VARIABILE bool AbilitaRotazioneSuiRettilinei;                       // Abilitazione della rotazione dei TGV sui rettilinei
TIPO_VARIABILE bool AbilitaPuntoIntermedioSuRotaz;                       // Abilitazione del punto intermedio solo sulle rotazioni
TIPO_VARIABILE bool AbilitaLetturaMultiPLC;                              // Attivazione comunicazione MULTI PLC legate alla comunicazione Eth TCP/IP
TIPO_VARIABILE bool AbilitaGestionePercorsiVeloci;                       // Attivazione la procedura di gestione percorsi veloci
TIPO_VARIABILE bool AbilitaPIntPerConsensiMacchina;                      // Attivazione PINT forzato per attivazione consensi ingresso macchina/baia
TIPO_VARIABILE bool AbilitaPTPerSpostaTgv;                               // Utilizza anche i PT come possibile destinazione in caso di Spostamento tgv.
TIPO_VARIABILE bool OccupaTuttoSuPathVeloci;                             // Cerca di occupare tutta la traccia nella gestione dei
                                                                         // percorsi veloci
TIPO_VARIABILE short int AbilitaAvvicinamentoADest;                      // Attivazione la procedura di avvicinamento dei TGV ai Punti terminali
                                                                         // il TGV parirà pur impostando un PInt forzato sul quale valuterà
                                                                         // la destinazione finale
                                                                         // 0 - disattivata
                                                                         // 1 - attiva per BAIE ESPULSIONE
                                                                         // 2 - attiva per BAIE RICHIESTA
                                                                         // 3 - attivo per tutte le baie
TIPO_VARIABILE bool ModernHint;                                          // Abilita Visualizzazione Hint invece del testo nell'angolo superiore del Layout
TIPO_VARIABILE bool NodoDiAttesaVersoRicarica;                           // Flag di attivazione del nodo di attesa solo se questo
                                                                         // cade sul percorso di ritorno in ricarica
TIPO_VARIABILE bool AbilitaControlloRetteParalleleInclinate;             // Abilita Nuovo controllo distanza rette parallele incvlinate
TIPO_VARIABILE bool ForzaPIntPerNavettaInAttesa;                         // Blocco i punti intermedi su nodi per altro TGV in attesa
TIPO_VARIABILE bool NoPIntSuStartDeviazione;                             // Blocco i punti intermedi su nodi che non siano di inizio rotazione
TIPO_VARIABILE bool NoPIntSuStartCurva;                                  // Blocco i punti intermedi su nodi che non siano di inizio curva
TIPO_VARIABILE bool AbilitaVerificaZonaFuoriIngombro;                    // Abilita/Disabilita VerificaZoneFuoriIng automatiche
TIPO_VARIABILE bool AbilitaPrenotatiConCompattaTGV;                      // Abilita/Disabilita compattazione TGV in inseguimento
TIPO_VARIABILE bool ZoneFuoriIngManConStopPrenotati;                     // 0 - nelle zone fuori ingombro manuali i prenotati possono essere anche occupati
                                                                         // 1 - nelle zone fuori ingombro manuali i prenotati NON DEVONO essere anche occupati
TIPO_VARIABILE short int AbilitaGestioneIngombroCurvePerStopTGV;         // 0 - disattivato.
                                                                         // 1 - Abilita D88.14 con Ingombro ING
TIPO_VARIABILE bool PosSuFuoriIngombroPreRicarica;                       // Posizionamento su un punto fuori ingombro prima di andare in ricarica
TIPO_VARIABILE bool AnticipoPintPerPtInIngombro;                         // Anticipa di 2 nodi il pint se il TGV bloccante è su un PT in ingombro
TIPO_VARIABILE bool AttesaTgvLiberoFuoriIngombro;                        // Attesa dei TGV destinati ai fuori ingombro nella ricerca TGV liberi
TIPO_VARIABILE short int AbilitaDepositoMagSovrapposto;                  // Attivazione la procedura di gestione percorsi veloci
                                                                         // 0 - no possibilità di sovrapporre
                                                                         // 1 - sì possibilità di sovrapporre FORCA_A
                                                                         // 2 - sì possibilità di sovrapporre FORCA_B
                                                                         // 3 - sì possibilità di sovrapporre sia FORCA_A che FORCA_B
TIPO_VARIABILE short int AbilitaSvuotamentoManualeMag;                   // Abilita svuotamento magazzino con tasto DX del mouse
                                                                         // 0 - disattivata.
                                                                         // 1 - cancellazione del magazzino
                                                                         //     abilita/disabilita GRUPPO
                                                                         // 2 - spostamento pallet in un qualunque altro gruppo_dest
                                                                         // 3 - spostamento pallet in un qualunque altro gruppo_dest con ritorno
TIPO_VARIABILE bool DisattivazioneManualeMagTmp;                         // Disattivazione manuale MAGAZZINI TEMPORANEI 
TIPO_VARIABILE short int MSecIntervalloTimerCiclo;                       // Configurazione intervallo ciclo di Timer principale
TIPO_VARIABILE short int MinErrConsecutiviTGV;                           // Numero errori consecutivi prima di segnalazione errore comunicazione TGV
TIPO_VARIABILE short int CanaleCronoAllarmiTGV;                          // Canale iniziale da cui recuperare
TIPO_VARIABILE short int TempoPerMissioneRicarica;                       // Tempo di attesa in minuti prima di inizializzare la missione di ricarica
TIPO_VARIABILE short int TimeAttesaSuPTInIngombroNoSblocco;              // Time Attesa per spostamento TGV su PT IN INGOMBRO
TIPO_VARIABILE short int GrdMinPerAttivaDeviazioni;                      // Gradi minimi per attivazione deviazioni
TIPO_VARIABILE short int NumCicliPerSbloccoDeadLock;                     // Numero cicli per attesa sblocco Dead Lock
TIPO_VARIABILE bool ProseguiPerSbloccoDeadLock;                          // Attiva procedura di PROSEGUI per sblocco DeadLock
TIPO_VARIABILE int  RilascioApp;                                         // numero di cicli dopo il quale riasciare l'applicazione


//-------------------------------------------------------
// Gestione TELECAMERINA
//-------------------------------------------------------
TIPO_VARIABILE bool LogTgvAttivato;

//-------------------------------------------------------
// Gestione Percorsi TGV
//-------------------------------------------------------
TIPO_VARIABILE short int MaxMetriOccupabili;                             // Metri occupabili consecutivamente nell'esecuzione di una missione.

//-------------------------------------------------------
// Con alcuni editor aperti è necessario bloccare le
// missioni dei TGV
//-------------------------------------------------------
TIPO_VARIABILE short int EditorBoxAttivo;                               // Flag di editor BOX aperto
TIPO_VARIABILE short int EditorCBattAttivo;                             // Flag di editor Cambio Batteria aperto
TIPO_VARIABILE short int EditorMagazzinoAttivo;                         // Flag di editor MAGAZZINO aperto
TIPO_VARIABILE short int EditorMacchineAttivo;                          // Flag di editor MACCHINE aperto
TIPO_VARIABILE short int EditorBaieAttivo;                              // Flag di editor BAIE aperto

//-------------------------------------------------------
// GESTIONE SOVRAPPOSIZIONE
//-------------------------------------------------------
TIPO_VARIABILE short int MagSovrappMAX_BASSI;                            // Variabile sovrapposizione BASSI
TIPO_VARIABILE short int MagSovrappMAX_ALTI;                             // Variabile sovrapposizione ALTI

//-------------------------------------------------------
// Definizione variabili globali di Sistema
//-------------------------------------------------------
TIPO_VARIABILE bool      PcAlgo;                                         // PC ALGOSISTEMI
TIPO_VARIABILE bool      AbilitaSimulazione;                             // Variabile di abilitazione della Simulazione delle Missioni
TIPO_VARIABILE bool      AbilitaConfigurazioneImpianto;                  // Abilita Configurazione iniziale impianto (modifica di tutte le variabili)
TIPO_VARIABILE bool      TestInternoTecnoFerrari;                        // Flag di abilitazioni TEST_INTERNO_TECNOFERRAI
TIPO_VARIABILE bool      Automatico;                                     // Ciclo abilitato
TIPO_VARIABILE bool      BackupInCorso;                                  // Flag di backup in corso.
TIPO_VARIABILE bool      IncendioInCorso;                                // Flag di incendio in corso.
TIPO_VARIABILE short int AbilitaResetMissioniAlVolo;                     // Flag di abilitazione CAMBIO MISSIONI AL VOLO
TIPO_VARIABILE bool      PrioritaCercaAgvLiberoNONRicarica;              // Flag di priortià AGV LIBERO NON A RIPOSO
TIPO_VARIABILE short int CercaAgvLiberoInRicarica;                       // Cerca Agv libero in ricarica:
                                                                         // 0 - più vicino
                                                                         // 1 - in ricarica da più tempo
                                                                         // 2 - con percentuale di carica maggiore
TIPO_VARIABILE short int CurveDiBezier;                                  // Abilitazione curve di Bezier (da #define)
                                                                         // 0 - disattivate (solo ARCHI DI CRF)
                                                                         // 1 - tutte.
                                                                         // 2 - solo quadratiche.
                                                                         // 3 - solo cubiche.
TIPO_VARIABILE bool      AbilitaControlloNodoDest;                       // Abilita gestione CONTROLLO_NODO_DEPOSITO
TIPO_VARIABILE bool      AbilitaControlloPresaSuTGV;                     // Abilita DM controllo PRESA TGV.
TIPO_VARIABILE bool      AbbinaCodiceStatoBox;                           // Da definire solo negli impianti in cui c'è una corrispondenza
                                                                         // univoca tra CODICE e STATOBOX (possibile solo fino a MAX 8 CODICI)
TIPO_VARIABILE bool      MagazziniMultiNodo;                             // Abilitazione magazzini con NODO su ogni posizione
TIPO_VARIABILE short int GestioneDinamicaMagazzini;                      // Abilitazione magazzini dinamici
                                                                         // 0 - non attiva
                                                                         // 1 - attiva
TIPO_VARIABILE short int GestioneDoppiaForcaTGV;                         // Abilitazione gestione doppia forca TGV (settato su #define all'accensione)
                                                                         // 0 - non attive
                                                                         // 1 - doppie forche reali
                                                                         // 2 - forca singola con doppio carico
TIPO_VARIABILE short int GestioneUdc;                                    // Gestione UDC
                                                                         // 0 - disattivata
                                                                         // 1 - con UDC UNIVOCI
                                                                         // 2 - con UDC NON UNIVOCI o NON OBBLIGATORI
TIPO_VARIABILE short int ComunicazioneDirettaPLC;                        // Comunicazione diretta con i PLC delle macchine
TIPO_VARIABILE bool      ModificaCoordinateNodi;                         // Consente agli operatori di modificare le coordinate di un
TIPO_VARIABILE char      LinguaSistema[101];                             // Lingua di sistema
TIPO_VARIABILE short int NrOperatore;                                    // Numero identificativo dell'operatore
TIPO_VARIABILE short int LivelloPassword;                                // Livello Password
TIPO_VARIABILE short int LivelloMinPassword;                             // Livello min Password consentito sul PC
TIPO_VARIABILE short int LivelloMaxPassword;                             // Livello max Password consentito sul PC
TIPO_VARIABILE bool      break_Com[21];                                  // Array delle comunicazioni
TIPO_VARIABILE short int errore_comunicazione[MAXCOM+1][MAXPLCCON+1];    // Errori di Comunicazione
TIPO_VARIABILE bool      Lampeggio;                                      // Variabile flip/flop
TIPO_VARIABILE bool      PrimaScansione;                                 // Gestione della prima scansione Timer principale
TIPO_VARIABILE char      DriveServer[101];                               // Path del PC SERVER
TIPO_VARIABILE short int nodi_prenotati[TOTPUNTI+10][MAXAGV];            // Struttura dei nodi prenotati dal
TIPO_VARIABILE bool      RiattivaMapping[MAXAGV+1];                      // Falg di riattivazione comando mapping
                                                                         // TGV.
TIPO_VARIABILE struct VARIABILI_OCCUPA_NODO VarOccNodo[MAXAGV+2];        // Array per visualizzazione struttura di occupazione nodi
TIPO_VARIABILE short int AgvBloccati[MAXAGV+1];                          // Array contenente per ogni TGV il numero dell'altro
                                                                         // TGV che lo Blocca nell'occupazione dei nodi.
TIPO_VARIABILE short int BloccoInvariato[MAXAGV+1];                      // Array contenente per ogni TGV il numero dell'altro
                                                                         // TGV che lo Blocca nell'occupazione dei nodi.
TIPO_VARIABILE short int CountCondizioneDiBlocco;                        // Contatore dei Cicli di RunAutomatico() per cui
                                                                         // rimane verificata la condizione di blocco.
#endif;
