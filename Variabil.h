// ------------------------------------------------------------------------
//             file          VARIABIL.H
// ------------------------------------------------------------------------
#ifndef VARIABIL.H
#define VARIABIL.H

//-------------------------
// costanti Grafiche
//-------------------------
#define MAXRID                  4        // Massimo rapporto di riduzione
#define RID_LENTE            1.35        // Rapporto riduzione Rid x riquadro lente
#define RID_SFONDO             .2        // Riduzione Max per disegno sfondo ( .1 - 1 )
#define RID_NUMERI            1.5        // Riduzione Max per disegno numeri ( .1 - 1 )
#define RID_NUMCAR            1.5        // Riduzione Max per disegno numeri carrelli ( .1 - 1 )
#define SIGLE_GRASSETTO         1        // 0 - disattivato, 1 - attivato
                                         //
#define ADJUST_GRAFICA_TGV      0        // 0 = PUNTO LOGICO IN CENTRO ALLA GRAFICA TGV
                                         //
                                         // Offset per la corretta visualizzazione del punto logico
                                         // del TGV sul nodo. Per regolarlo opportunamente è bene
                                         // dare un carico al TGV e posizionarlo su un nodo magaz, il
                                         // valore sarà correttamente impostato quando il punto logico
                                         // verrà visualizzato sul nodo e il box a bordo avrà i contorni
                                         // coincidenti con la posizione del box sul primo nodo in magazzino
                                         //
// -----------------------
//     Costanti FISSE
// -----------------------
#define  TOTCODICIMAG              100   // Numero massimo di codici in magazzino
#define  MAXZP                     20    // numero max zone Primarie
#define  MAXZS                     40    // numero max zone Secondarie
#define  MAXPUNTINODO              41    // numero max targets x ogni nodo
#define  MAXLRALLARMI              4     // numero max di canali allarmi TGV
#define  MAX_ALLARMI_MAC           32    // numero max di allarmi Macchina
#define  MAX_ALLARMI_CBAT          32    // numero max di allarmi CambioBatterie
#define  MAXBLOCCHIGRAFICI         1000  // numero max di blocchi grafici dell'impianto
#define  MAXBLOCCHISEMPLICI        100   // numero max di blocchi grafici dell'impianto
#define  MAXBLOCCHISPECIALI        50    // numero max di blocchi grafici dell'impianto
#define  MAXGRUPPIIMP              30    // numero max zone che compongono l'impianto
#define  MAXEVACUAZIONI            150   // numero max di evacuazioni codificate
#define  MAXBAIEPERGRUPPO          100   // numero max baie per gruppo l'impianto
#define  MAXTIPOBOXIMP             10    // numero max di tipi box gestiti in impianto
#define  MAXCODICI                 500   // numero max di formati prodotto ammessi
#define  MAXLISTABOX               50    // numero max box in lista manuale sulle macchine
#define  TRUE                      1
#define  FALSE                     0
#define  SU                        1
#define  GIU                       2
#define  SINISTRA                  3
#define  DESTRA                    4
#define  LIBERO                    0
#define  VUOTO                     1
#define  PIENO                     2
#define  LIFO                      0
#define  FIFO                      1

//--------------------------------------------------
// Gestione percorso alternativo veloce
//--------------------------------------------------
#define  PERC_ALTERNATIVO_VELOCE   9999     // Definisce una valore JOLLI nell'editor dei NODI, relativo ai punti terminali
                                            // che indica un percorso da efettuare SOLO con TGV SCARICO
#define  STD_PATH                     0     // Costante di attivazione percorso SOLO STANDARD
#define  FAST_PATH                    1     // Costante di attivazione percorso SOLO VELOCE
#define  CALCOLA_TIPO_PATH           99     // Passato alla funzione di calcolo permette il calcolo stesso

//--------------------------------------------------
// Gestione percorsi alternativi per NODO/ROT PREC
//--------------------------------------------------
#define  ABILITA_PERC_ALTERN_PER_PRECEDENTE      // Da definire negli impianti dove si vuole gestire il percorso alternativo iconcorde
                                                 // a nodo precedente o rotazione
#define  PT_JOLLY_PERC_ALTERN_PER_ROT      1999  // Definisce una valore JOLLI nell'editor dei NODI, relativo al punto precedente
                                                 // che indica un percorso da efettuare SOLO con rotazione coincidente con quella
                                                 // impostata per PS

//--------------------------------------------------
// Autoriazzazione Client/Server
//--------------------------------------------------
#define  MAX_IDPC_AUTORIZZATO   3     // Definisce il numero di IDPC massimo autorizzato (default 3 = 1 Server 2 Client):
                                      // identificativivi di valore superiore NON sono gestiti.


//--------------------------------------------------
// Definizione di costanti per "Reset Automatico"
// delle missioni del TGV.
//--------------------------------------------------
#define  MAXNODIRESET          50    // numero max di nodi entro il quale tentare il reset delle missioni

//--------------------------------------------------
// BLOCCO MODIFICA DISTANZA NELL'EDITOR NODI
//--------------------------------------------------
#define BLOCCO_MODIFICA_DISTANZA_DA_EDITOR_NODI    1

//--------------------------------------------------
// COSTANTI DI GESTIONE STATISTICHE TGV
//--------------------------------------------------
#define  MAXTGV_TIMTGV             20    // Numero totale TGV previsti in STATISTICHE TGV
#define  MAXFIELD_TIMTGV            9    // Numero totale CAMPI TEMPO previsti in STATISTICHE TGV
													 // ATTENZIONE: (valore minimo 9)
#define  GIORNI_TIMTGV             60    // Numero giorni archiviazione TEMPI TGV
#define  SEC_AGGIORNA_TIMTGV      180    // Secondi di precisione aggiornamento STATISTICHE TGV

//--------------------------------------------------
// Definizione Tipo Missioni SEMIAUTO
//--------------------------------------------------
#define  MISS_SEMIAUTO_SOLO_POSIZIONAMENTO  0
#define  MISS_SEMIAUTO_MACCHINA             1
#define  MISS_SEMIAUTO_MAGAZZINO            2
#define  MISS_SEMIAUTO_BAIA                 3

//--------------------------------------------------
// Definizione Tipo Missioni
//--------------------------------------------------
#define  MISS_SOLO_POSIZIONAMENTO        0
#define  MISS_CARICO_DA_MACCHINA         1
#define  MISS_SCARICO_SU_MACCHINA        2
#define  MISS_CARICO_DA_MAGAZZINO        3
#define  MISS_SCARICO_SU_MAGAZZINO       4
#define  MISS_CARICO_DA_BAIA             5
#define  MISS_SCARICO_SU_BAIA            6

//--------------------------------------------------
//  Gestione Gruppi
//--------------------------------------------------
#define GRUPPO_FUORI_INGOMBRO    0
#define GRUPPO_01                1
#define GRUPPO_02                2
#define GRUPPO_03                3
#define GRUPPO_04                4
#define GRUPPO_05                5
#define GRUPPO_06                6
#define GRUPPO_07                7
#define GRUPPO_08                8
#define GRUPPO_09                9
#define GRUPPO_10               10
#define GRUPPO_11               11
#define GRUPPO_12               12
#define GRUPPO_13               13
#define GRUPPO_14               14
#define GRUPPO_15               15
#define GRUPPO_16               16
#define GRUPPO_17               17
#define GRUPPO_18               18
#define GRUPPO_19               19

//--------------------------------------------------
// GESTIONE TIPO DESTINAZIONE
//--------------------------------------------------
#define DEST_TIPO_NODO_SEMPLICE       0
#define DEST_TIPO_NODO_MACCHINA       1
#define DEST_TIPO_NODO_MAGAZZINO      2
#define DEST_TIPO_NODO_BAIA           3
#define DEST_TIPO_NODO_MAG_MULTI_NOD  4

//--------------------------------------------------
// ZONE IMPIANTO
//--------------------------------------------------
#define ZONA_0_ALL           0     // Zona tutte le zone
/*
#define ZONA_1_MAG_ZAVORRE   1     // Zona magazzini zavorre
#define ZONA_2_DECESTONATORI 2     // Zona decestonatori
#define ZONA_3_AUTOCLAVI     3     // Zona autoclavi
#define ZONA_4_RUL_FALDE     4     // Zona rulliere falde
#define ZONA_5_PORTA_P1      5     // Zona oltra porta P1
#define ZONA_6_CESTONATORE   6     // Zona cestonatore
*/
//--------------------------------------------------
// Tipo di MACCHINE/STAZIONI
//--------------------------------------------------
#define  MC_NONE              0     // SCONOSCIUTA
#define  MC_TIPO1             1     // AVM
#define  MC_TIPO2             2     // ADS
#define  MC_TIPO3             3     // ADM
#define  MC_TIPO4             4     // ESSICCATOIO
#define  MC_TIPO5             5     // SMALTERIA
#define  MC_STAZ_ING          0     // STAZIONE INGRESSO
#define  MC_STAZ_OUT          1     // STAZIONE INGRESSO

//--------------------------------------------------
// Tipo di BAIA
//--------------------------------------------------
#define  BA_NONE              0     // BAIA ESCLUSA
#define  BA_PRELIEVO          1     // BAIA DI PRELIEVO
#define  BA_DEPOSITO          2     // BAIA DI DEPOSITO

//--------------------------------------------------
// Tipo di BAIA
//--------------------------------------------------
#define  GR_NONE              0     // GRUPPO TIPO GENERICO
#define  GR_BAIE              1     // GRUPPO BAIE
#define  GR_MAGAZZINO         2     // GRUPPO MAGAZZINO

//--------------------------------------------------
// Assegnazione/Tipo/Stato MAGAZZINI
//--------------------------------------------------
// assegnazione
#define  ASS_DINAMICA         0     // ASSEGNAZIONE AUTOMATICA MAGAZZINO
// assegnazione CODICE
#define  ASS_COD1             1     // PRODOTTO PROVENIENTE L1
#define  ASS_COD2             2     // PRODOTTO PROVENIENTE L2
#define  ASS_COD3             3     // PRODOTTO PROVENIENTE L3
#define  ASS_COD4             4     // PRODOTTO PROVENIENTE L4
#define  ASS_COD5             5     // PRODOTTO PROVENIENTE L5
#define  ASS_COD6             0     // PRODOTTO PROVENIENTE L6
#define  ASS_COD7             0     // PRODOTTO PROVENIENTE L7
#define  ASS_COD8            99     // PRODOTTO PROVENIENTE L8
#define  ASS_VUOTO            0     // PALETTE VUOTE
// assegnazione LAVORAZIONE
#define  ASS_LAV1            ""     // STATO LAVORAZIONE "1"
#define  ASS_LAV2            ""     // STATO LAVORAZIONE "2"
#define  ASS_LAV3            ""     // STATO LAVORAZIONE "3"
#define  ASS_LAV4            ""     // STATO LAVORAZIONE "4"
#define  ASS_LAV5            ""     // STATO LAVORAZIONE "5"
#define  ASS_LAV6            ""     // STATO LAVORAZIONE "6"
#define  ASS_LAV7            ""     // STATO LAVORAZIONE "7"
#define  ASS_LAV8            ""     // STATO LAVORAZIONE "8"
// tipo (ulteriore filtro)
#define  MG_ESCLUSO           0     // MAGAZZINO ESCLUSO
#define  MG_PROD_TUTTI        1     // PER PRODOTTO (PIENI)
#define  MG_PROD_TIPO1        2     // PER PRODOTTO (PIENI)
#define  MG_PROD_TIPO2        3     // PER PRODOTTO (PIENI)
#define  MG_VUOTI             4     // PER VUOTI    (VUOTI)
#define  MG_TEMPORANEO        5     // TEMPORANEO
// stato
#define  MG_ST_FREE           0     // STATO MAGAZZINO LIBERO
#define  MG_ST_PIENI          1     // STATO MAGAZZINO CON PIENI
#define  MG_ST_VUOTI          2     // STATO MAGAZZINO CON VUOTI

//--------------------------------------------------
// Tipo presenze TGV (valide per entrambe le forche)
//--------------------------------------------------
#define  LIBERA               0     // Posizione libera
#define  PRES_LAV1            1     // CESTONE COD1               (verde )
#define  PRES_LAV2            2     // CESTONE COD2               (verde )
#define  PRES_LAV3            3     // CESTONE COD3               (verde )
#define  PRES_LAV4            4     // CESTONE COD4               (verde )
#define  PRES_LAV5            5     // CESTONE COD5               (rosso )
#define  PRES_LAV6            6     // CESTONE COD5               (rosso )
#define  PRES_LAV7            7     // CESTONE COD5               (rosso )
#define  PRES_LAV8            8     // CESTONE COD5               (rosso )
#define  PRES_VUOTO           9     // CESTONE VUOTO              (bianco)
// tipo box
#define  TIPOBOX_1            0     // TIPO BOX 1000x1200
#define  TIPOBOX_2            1     // TIPO BOX 1200x1000

//--------------------------------------------------
// Tipo Asservimenti MACCHINE
//--------------------------------------------------
#define ASS_MAC_TUTTE         0     // Asservimento tutte le chiamate
#define ASS_MAC_SOLO_ING      1     // Asservimento solo richiesta nuovo
#define ASS_MAC_SOLO_OUT      2     // Asservimento solo espulsione vecchio

//--------------------------------------------------
// Tipo Chiamata su ricerca TGV
//--------------------------------------------------
#define CH_RICHIESTA          0     // Richiesta cestone da macchina/baia
#define CH_ESPULSIONE         1     // Espulsione cestone da macchina/baia

//--------------------------------------------------
// Stato Box
//--------------------------------------------------
#define ST_BOX_LAV1           1     // CESTONE CON COD1           (verde )
#define ST_BOX_LAV2           2     // CESTONE CON COD2           (verde )
#define ST_BOX_LAV3           3     // CESTONE CON COD3           (verde )
#define ST_BOX_LAV4           4     // CESTONE CON COD4           (verde )
#define ST_BOX_LAV5           5     // CESTONE CON COD5           (rosso )
#define ST_BOX_LAV6           6     // CESTONE CON COD6           (rosso )
#define ST_BOX_LAV7           7     // CESTONE CON COD7           (rosso )
#define ST_BOX_LAV8           8     // CESTONE CON COD8           (rosso )
#define ST_BOX_VUOTI          9     // CESTONE VUOTO              (bianco)

//--------------------------------------------------
// Costante che definisce la ricerca di tutti i
// tipi di box                                                                                              0x00FF8000
//--------------------------------------------------
#define SEARCH_ALL_IDBOX     0      // Non discrimina l'ID BOX nella ricerca PIENI
#define SEARCH_ALL_TIPOBOX  99      // Se il campo TipoBox ha questo valore
                                    // significa che il campo stesso perde di
                                    // significato

//--------------------------------------------------
// Colori grafici abbinati alla presenza
//--------------------------------------------------
#define COLOR_PRES_STAT1    0x00BBFFFF   // AD50
#define COLOR_PRES_STAT2    0x00FF8000   // AD51
#define COLOR_PRES_STAT3    clWhite      // AD52
#define COLOR_PRES_STAT4    clGreen      // AD21
#define COLOR_PRES_STAT5    0x0040FF00   // free
#define COLOR_PRES_STAT6    0x008C008C   // free
#define COLOR_PRES_STAT7    clAqua       // free
#define COLOR_PRES_STAT8    0x004A4AFF   // EMERGENZA      (99)
#define COLOR_PRES_VUOTI    0x008F8F8F   // PALLET VUOTI          (grigio)


//--------------------------------------------------
// NUMERO FORCA
//--------------------------------------------------
#define  N_NO_FORCHE         0     // Nessuna presa
#define  N_FORCA_B           1     // Presa con FORCA BASSA
#define  N_FORCA_A           2     // Presa con FORCA ALTA
#define  N_FORCA_AB          3     // Presa doppia

//--------------------------------------------------
// PROVENIENZA TGV
//--------------------------------------------------
#define  NN                  0     // Sconoscita
#define  PROV_MACCHINE       1     // Provenienza macchine
#define  PROV_MAGAZZINO      2     // Provenienza magazzino
#define  PROV_BAIE           3     // Provenienza baia

//--------------------------------------------------
// CODICI DI ERRORE LETTURA SU SQL
//--------------------------------------------------
#define  ERR_SQL_NONE        0     // Nessun errore attivo
#define  ERR_SQL_CONNECTION  1     // Errore connessione a DBase
#define  ERR_SQL_RD_TAB      2     // Errore lettura anagrafica   PRODOTTI SEMILAVORATI
#define  ERR_SQL_WR_TAB      3     // Errore scrittura anagrafica PRODOTTI SEMILAVORATI

//--------------------------------------------------
// COMANDI DI GESTIONE SQL
//--------------------------------------------------
#define  CMD_NO_SQL                    0     // Comando sconoscito
// -- READ
#define  CMD_RD_SQL_ALL_CHANGE         1     // tutti gli UDC con una CustomerChange
#define  CMD_RD_SQL_ALL_UDC            2     // tutti gli UDC con una locazione o CustomerChange
#define  CMD_RD_SQL_ALL_LOC            3     // tutti gli UDC con una locazione
#define  CMD_RD_SQL_RANGE_UDC          4     // tutti gli UDC in un range di UDC
#define  CMD_RD_SQL_RANGE_LOC          5     // tutti gli UDC in un range di locazioni
#define  CMD_RD_SQL_ONE_UDC            6     // tutti gli UDC con una UDC
#define  CMD_RD_SQL_ONE_LOC            7     // tutti gli UDC in una locazione
// -- WRITE
#define  CMD_WR_SQL_UDC_ALL            1     // aggiornamento di tutti i dati previsti (Stato + Locazione)
#define  CMD_WR_SQL_UDC_SET_LOC        2     // settaggio solo della LOCAZIONE
#define  CMD_WR_SQL_UDC_RST_LOC        3     // reset della LOCAZIONE
#define  CMD_WR_SQL_UDC_RST_CUCH       4     // reset del Customer Change

//--------------------------------------------------
// CODICI DI ALLARME INTERNO TGV
//--------------------------------------------------
#define  ALL_INT_RESET_DATI            1  // Reset dati a bordo TGV
#define  ALL_INT_SCAR_MANUALE          2  // Scarico manuale TGV o reset dati
#define  ALL_INT_POS_NOT_OK            3  // TGV non in posizione
#define  ALL_INT_STOP_TGV              4  // Stop TGV
#define  ALL_INT_DATI_ANOMALI          5  // Dati anomali
#define  ALL_INT_ERR_COMUNIC           6  // Errore persistente di comunicazione
#define  ALL_INT_RICARICA_OCC          7  // Ricarica occupata per missioni automatiche
                                          // con batteria in riserva.
#define  ALL_INT_START_DA_OCC          8  // Tgv è stato fatto ripartire da un nodo occupato
                                          // da un altro TGV
#define  ALL_INT_ID_NOT_OK             9  // Tgv ha a bordo un IDBOX non valido
#define  ALL_INT_DEST_IMPEGNATA       10  // Destinazione Scarico Tgv non disponibile
#define  ALL_INT_DATI_BOX_NON_VALIDI  11  // Dati Box non validi
#define  ALL_INT_UDC_DUPLICATO        12  // UDC/SSCC Duplicato
#define  ALL_INT_DEST_NON_VALIDA      13  // Destinazione Scarico Tgv non valida/trovata

//--------------------------------------------------
// Costanti di Comunicazione
//--------------------------------------------------
#define  COM1                       0x00  // comunicazione nr. 1 (TGV)
#define  COM2                       0x01  // comunicazione nr. 2 (MACCHINE = PLCTERRA o DIRETTA)
#define  COM3                       0x02  // comunicazione nr. 3 (DBASE)
#define  COM4                       0x03  // COM4 per comunicazione con PLC
#define  COM5                       0x04  // COM5 per comunicazione con PLC
#define  COM6                       0x05  // COM6 per comunicazione con PLC
#define  COM7                       0x06  // COM7 per comunicazione con PLC
#define  COM8                       0x07  // COM8 per comunicazione con PLC
#define  COM9                       0x08  // COM9 per comunicazione con PLC
#define  COM10                      0x09  // COM10 per comunicazione con PLC
#define  NUMPORTE                      2  // Numero di Porte utilizzate dal Sistema
#define  COMUNICAZIONE_DIRETTA         1  // Comunicazione diretta con i PLC
#define  DB_EXTERN                  0x00  // comunicazione con SQL SERVER
#define  COM_SIEMENS                0x01  //
#define  BECKHOFF_PORT               851  // Porta di comunicazione Beckoff
// ------------------------------------------------
//  GESTIONE SCHEDE IO_REMOTATE
// ------------------------------------------------
#define MAXPLC_IO_REMOTI              10  // Numero massimo di schede per la comunicazione diretta
#define PLC_IO_REMOTO_01              21  // PLC gestione comunicazione diretta I/O REMOTO dispositivo  1 (0 - non esiste)
#define PLC_IO_REMOTO_02               0  // PLC gestione comunicazione diretta I/O REMOTO dispositivo  2 (0 - non esiste)
#define PLC_IO_REMOTO_03               0  // PLC gestione comunicazione diretta I/O REMOTO dispositivo  3 (0 - non esiste)
#define PLC_IO_REMOTO_04               0  // PLC gestione comunicazione diretta I/O REMOTO dispositivo  4 (0 - non esiste)
#define PLC_IO_REMOTO_05               0  // PLC gestione comunicazione diretta I/O REMOTO dispositivo  5 (0 - non esiste)
#define PLC_IO_REMOTO_06               0  // PLC gestione comunicazione diretta I/O REMOTO dispositivo  6 (0 - non esiste)
#define PLC_IO_REMOTO_07               0  // PLC gestione comunicazione diretta I/O REMOTO dispositivo  7 (0 - non esiste)
#define PLC_IO_REMOTO_08               0  // PLC gestione comunicazione diretta I/O REMOTO dispositivo  8 (0 - non esiste)
#define PLC_IO_REMOTO_09               0  // PLC gestione comunicazione diretta I/O REMOTO dispositivo  9 (0 - non esiste)
#define PLC_IO_REMOTO_10               0  // PLC gestione comunicazione diretta I/O REMOTO dispositivo 10 (0 - non esiste)

// --------------------------------
//  Indici di comunicazione
// --------------------------------
#define  PLCTERRA                      0x00 // Indice di comunicazione con il PLC DI TERRA
#define  DB_DBASE                      0x00 // Indice di comunicazione con il DBASE KRAFT

// --------------------------------
//  GESTIONE_ASRV
// --------------------------------
#define  TYPCOM_BECKHOFF                  5 // tipo comunicazione Beckoff
#define  MAXSTEP_ASRV                   501 // n.max passi di programma
#define  RISERVA_ASRV                    30 // sotto al 30% l'ASRV è in batteia scarica
#define  ASRV_NODO_TRANSITO               0
#define  ASRV_NODO_ACCUMULO               1
#define  ASRV_NODO_SERVIZIO               2

// --------------------------------
// Costanti di gestione PERCORSI
// --------------------------------
#define  MAXPERCORSI                    300 // n.max punti per percorso
#define  MAXSTEP                       1590 // n.max passi di programma
#define  MINPERCORSI                      0 // n.min percorsi per missioni intermedie
#define  MAXNODIATTESA                  150 // n.max nodi attesa
#define  MINNODIPINTSUCC                  0 // minimo numero di nodi a cui si deve trovare il PInt successivo
                                            // (N.B: non vale su start missione)
#define  OCCUPA_METRI_CON_OFFSET_DA_NODO  1 // Abilita il conteggio dei metri già percorsi con offeset da nodo
                                            // D93 per il calcolo dei metri ancora da occupare
#define  MINPERCORSI_FUORI_INGOMBRO       2 // n.min percorsi per ripartenza da nodi FUORI INGOMBRO
#define  MIN_NODI_SBLOCCO_DEADLOCK        4 // n.max distanza nodi per gestione limitrofe
#define  MAX_DIST_FUORI_INGOMBRO         99 // Mt max di distanza in coodinate tra nodo origine e nodi
                                            // desitnazione in caso di MissioniFuoriIngombro
                                            // 99 = TUTTI I NODI DELL'IMPIANTO SONO AMMESSI
#define  DIST_MANTIENI_ING_RULL         150 // Distanza per mantenere ingombro a rulliere

#define  DIST_INGOMBRO_BAIE             200 // Distanza minima tra nodo Baia e nodo traccia esterna x ingombro Tgv
                                            // defualt = 200
#define  ANTICIPO_RICH_INGRESSO_MAC       5 // Numero di nodi per anticipare la richiesta di ingresso MACCHINE/BAIE
                                            // il riferimento è il nodo macchina/baia:
                                            // 0, 1 - Richiesta ingresso su nodo precedente.
                                            // 2... = N° nodi da nodo macchina/baia.
#define  NUM_NODI_DA_PINT_TGVALL          2 // Gestione D88: Questo numero di nodi indica a quanto far fermare
                                            // un TGV in inseguimento di un'altra navetta in allarme
                                            // per evitare l'inforco
#define  MIN_NODI_FUORI_INGOMBRO          1 // Numero minimo di nodi fuori ingombro

#define  MIN_NODI_VERIFICA_PATH_VELOCE   15 // n.max attesa altro tgv in transito per scelta path veloce
                                            // default = 999 per controllo intero percorso fino a destinazione

#define  MAX_NODI_ATTESA_PATH_VELOCE    999 // n.max attesa altro tgv in transito per scelta path veloce

#define  VERIFICA_NODI_CON_PINTFORZATO    1 // 0 - Non esegue il controllo nodi contrapposti e BloccoRot
                                            //     se altro TGV a PIntForzato;


#define  DIST_MAX_PER_RAL_ANTICIPATO     50 // Distanza sotto alla quale viene dato
                                            // il rallentamento anticipato 2 nodi prima
                                            // di una rotazione

#define  INGOMBRO_BOX_A_BORDO             0  // Ingombro del box a bordo del TGV

#define  DIST_MIN_RETTE_PARALLELE        90  // Distanza minima per tracce parallele

#define  DIST_MIN_ROT_SINGOLA           120  // Distanza minima perch‚ 1 tgv possa
                                             // ruotare mentre l'altro si muove
                                             // perpendicolarmente ( 300 Š il
                                             // di sicurezza x tutti gli impianti )

#define  DIST_MIN_ROT_DOPPIA_MIN        240  // Distanza minima perchè 2 tgv possano      //§§§ ok
                                             // iseguirsi su un tratto rettilineo
                                             // (soprattutto significativo per PAL)

#define  DIST_MIN_ROT_DOPPIA_MAX        240  // Coincide con DIST_MIN_ROT_DOPPIA          //§§§
                                             // Distanza minima perchè 2 tgv possano
                                             // ruotare senza collisione ( 340 Š il
                                             // di sicurezza x tutti gli impianti )
                                             //
                                             // sostituisce DIST_MIN_ROT_DOPPIA

#define  DIST_MIN_ALLONTANAMENTO        240  // Distanza minima perchè 2 tgv possano
                                             // iseguirsi su un tratto rettilineo
                                             // (soprattutto significativo per PAL)

#define  DIST_MAX_SALTO_NODI            300  // Distanza massima tra 2 nodi all'interno della quale
                                             // cercare un SALTO NODI

#define  CICLI_ATTESA_PER_AGV_IN_RESET       20   // Conta i cicli da aspettare per inizializzare la missione per dare la precedenza
                                                  // agli agv in reset missione fermi sul percorso

#define  OFFSET_MAX_COORD_TRATTO_INCLINATO   15   // Distanza max tra le coordinate di 2 nodi
                                                  // su traccia obliqua
#define  OFFSET_MIN_COORD_TRATTO_INCLINATO    5   // Distanza min tra le coordinate di 2 nodi
                                                  // su traccia obliqua

// --------------------------------------
//  DISTANZA/ABILITA SALTO NODI AUTO
// --------------------------------------
#define  DIST_SALTO_NODI              0  // Distanza per attivazione SALTO NODI AUTOMATICO
                                         // (==0 disattivata CONSIGLIATA)

// --------------------------------------
//  GESTIONE LOCAZIONI LIMITROFE
// --------------------------------------
#define  MAX_NODI_SBLOCCO_DEADLOCK              3  // n.max distanza nodi per gestione limitrofe
#define  MAX_NODI_LIMITROFI                   999  // n.max distanza nodi per gestione limitrofe
#define  DISTANZA_MINIMA_OBLIQUA              200  //
#define  DISTANZA_MINIMA_CONTRAPPOSTA         250  //
#define  DISTANZA_LIMITROFA_MINIMA            120  // 230  //

// --------------------------------------
//  GESTIONE INGOMBRO AGV "ING."
// --------------------------------------
#define ABILITA_GESTIONE_INGOMBRO_AGV_GRAFICO

#define  DIST_LATERALE_INGOMBRO_AGV       80
#define  DIST_AVANTI_INGOMBRO_AGV         80
#define  DIST_INDIETRO_INGOMBRO_AGV       80

//--------------------------------------------------
// GESTIONE AVVICINAMENTO
// Numero nodi entro i quali il TGV esegue un
// reset del PIntForzato in caso di avvicinamento
// a baia
//--------------------------------------------------
#define NUM_NODI_PER_RESET_PINTFORZATO          2
#define MAX_AGV_IN_AVVICINAMENTO                2

// --------------------------------------
//  PRECEDENZE PER TGV IN RETTILINEO
// --------------------------------------
#define  AREA_PER_TGV_LANCIATI_IN_RETTILINEO   20   // valore espresso in metri che identifica un area
                                                    // da considerare impegnata anche se non occupata
                                                    // dai TGV lanciati in rettilineo

// -----------------------
//     Costanti VAR
// -----------------------
#define  MAXBOXIMPIANTO            1000  // numero max di box nell'impianto
#define  MAXPUNTI                  3000  // numero max punti impianto
#define  MAXBATT                     10  // numero max di caricabatterie nell'impianto
#define  MAXCHIAMATE                 50  // numero max di macchine impianto
#define  MAXSTAZMAC                   5  // numero max di stazioni sulle macchine
#define  MAXPOSTISTAZ                10  // numero max di posti stazione macchine
#define  MAXBAIE                     10  // numero max di baie nell'impianto
#define  MAXPOSBAIA                   4  // mumero max di posizioni in profondità baia
#define  MAXMAG                     200  // numero max di magazzini
#define  MAXMAGABB                   50  // numero max di piani per magazzino
#define  MAXTERRAMAG                 40  // numero max di posti a terra in magazzino
#define  MAXPIANIMAG                  1  // numero max di piani per magazzino
#define  MAXPORTEIMP                 20  // numero max di porte
#define  PASS_CLIENTE           "SMOOV"  // Password di accesso a Livello 1
#define  PASS_TECNO            "ICAMTF"  // Password di accesso a Livello 2
#define  PASS_LEVEL3                 ""  // Password di accesso a Livello 3
                                         // (Se è "" viene calcolato gg+mm+aaaa)
#define  VERSIONE_IMPIANTO           10  // Versione di aggiornamento dell'Impianto
#define  COMM_ALGOSISTEMI         10638  // COMMESSA ALGO (fisso)

#define  PLANT_PLUGIN         "ICAM.CPP" // Nome impianto.CPP
//================================================
// INTEFACCIA ANDROID
// Numero di connessioni ANDROID ammesse su
// questa applicazione
//================================================
#define  MAX_PERMESSI_CLIENTE         0
#define  MAX_PERMESSI_SPECIALI        1

//------------------------------------------------
// CODICE DI ACCESSO APPLICAZIONI ANDROID
//------------------------------------------------
#define  CODICE_BASE_ANDROID          10638    // (default = COMMESSA ALGO) max 6 cifre
#define  CODICE_CLIENTE               0

//------------------------------------------------
//  Impostazioni per Impianto
//------------------------------------------------
#define MAXAGV               2           // n.max AGV
#define POSINPRESA           1           // mumero max di posizioni previsti per presa TGV
#define PIANIINPRESA         1           // mumero max di piani previsti per presa TGV
#define TOTPORTE             0           // Numero totale porte lungo il percorso
#define TOTPUNTI             250         // Numero totale nodi-percorso
#define TOTCHIAMATE          0           // Numero reale di decestonatori impianto
#define TOTMAG               31          // Numero totale magazzini
#define TOTBAIE              1           // Numero totale magazzini
#define TOTBATT              0           // Numero totale Cambio Batterie
#define LUNG_AGV             80          // Lunghezza AGV (cm) - TENERE CONTO DEI 50cm di bumper che l'applicazione aggiunge in automatico
#define LARG_AGV             80          // Larghezza AGV (cm)
#define LUNG_AREA_CARICO     80          // Lunghezza FORCHE AGV (cm) default dimensione max TipoBox trasportato
#define LARG_AREA_CARICO    120          // Larghezza FORCHE AGV (cm) default dimensione max TipoBox trasportato
#define LUNG_CEST            80          // Lunghezza Cestone Lato (cm)
#define LARG_CEST           120          // Larghezza Cestone Lato (cm)
#define TIPO_CARICO_TGV      0           // Tipo di carico TGV:
                                         // 0 - Visualizzazione tipo pallet (rettangolare)
                                         // 1 - Visualizzazione tipo bobina (rotonda)
#define CARICO_IN_PUNTA      0           // Tipo di carico TGV:
                                         // 0 - a battuta gruppo forche
                                         // 1 - carico in punta forche
#define DIST_CEST_IN_MAG     10          // Distanza tra cestone e cestone in magazzino
#define RAP_ENCODER          1.000       // Rapporto tra (cm) e impulsi Encoder
#define MAXCODA              18

// --------------------------------------
//  ABILITAZIONE TGV BOX
// --------------------------------------
// es. ABILITATGVBOX = 0x000E ---> 0000 0000 0000 1110:
//
// TGV1  - non abilitato.
// TGV2  - abilitato.
// TGV3  - abilitato.
// TGV4  - abilitato.
// TGV5  - non abilitato.
// ...
// TGV16 - non abilitato.
//------------------------------------------------------------------------
const int ABILITATGVBOX = 0xFFFF;

//------------------------------------------------
//    Movimento AGV su Layout Grafico
//------------------------------------------------
//      AVANTIDESTRO   = 1 -> avanti   x+
//                         -> indietro x-
//      AVANTIDESTRO   = 0 -> avanti   x-
//                         -> indietro x+
//      AVANTIBASSO    = 1 -> avanti   y+
//                         -> indietro y-
//      AVANTIBASSO    = 0 -> avanti   y-
//                         -> indietro y+
//      ROTAZIONEINV = 0   -> Normale: Con AGV a 0 gradi il lato lungo Š
//                            parallelo alla coordinata X
//      ROTAZIONEINV = 1   -> Invertito: Con AGV a 0 gradi il lato lungo Š
//                            parallelo alla coordinata Y
//
#define AVANTIDESTRO      0
#define AVANTIBASSO       0
#define ROTAZIONEINV      1

//------------------------------------------------
//  GESTIONE UDC (SQL)
//------------------------------------------------
#define ATTIVA_GESTIONE_UDC        0    // 0 - disattivata
                                        // 1 - con UDC UNIVOCI
                                        // 2 - con UDC NON UNIVOCI o NON OBBLIGATORI
#define UDC_ALL_POS               99    // costante che identifica TUTTI LE POSIZINI
#define UDC_ALL_STZ               99    // costante che identifica TUTTI LE STAZIONI MACCHINA
#define UDC_ALL_LEV               99    // costante che identifica TUTTI I PIANI MAG
#define CMD_UDC_REFRESH            1    // comando AGGIORNAMENTO STATO UDC
#define CMD_UDC_SET_LOC            2    // comando SET LOCAZIONE UDC
#define CMD_UDC_RST_LOC            3    // comando RESET LOCAZIONE UDC


//------------------------------------------------
//  GESTIONE DOPPIA FORCA (PRELIEVI/DEPOSITI)
//------------------------------------------------
#define GESTIONE_DOPPIA_FORCA                 0   // Attivazione gestione doppia forza su TGV
                                                  // 0 - non attive
                                                  // 1 - doppie forche reali
                                                  // 2 - forca singola con doppio carico
//#define PREL_TERRA_SOLO_FORCA_B                 // Il TGV può prelevare a terra solo con FORCA_B
                                                  //
#define CONTROLLO_STATO_BOX_A_TERRA_IN_DEPOSITO   // Con questo #define attivo si attiva il controllo di
                                                  // una variabile di stato box (che potrebbe cambiare da
                                                  // impianto a impianto) che informa il sistema della
                                                  // possibiltà di sovrapporre altri box al box considerato.
                                                  // SENZA QUESTO #DEFINE OGNI BOX A TERRA IN MAGAZZINO E' SOVRAPPONIBILE
                                                  // l'unico riferimento sono i piani abilitati in magazzino
//------------------------------------------------
//  GESTIONE MAGAZZINI
//------------------------------------------------
#define ABBINA_CODICE_A_STATO_BOX                 // Da definire solo negli impianti in cui c'è una corrispondenza
                                                  // univoca tra CODICE e STATOBOX
                                                  // (possibile solo fino a MAX 8 CODICI)
#define GESTIONE_CAMBIO_LOTTO                 0   // Abilita cambio posizione su cambio lotto
                                                  // 0 - no controllo lotto
                                                  // 1 - cambio fila con cambio lotto
                                                  // 2 - salto una posizione ma non cambio fila
#define GESTIONE_CAMBIO_TURNO                 0   // Abilita cambio posizione su cambio turno
                                                  // 0 - no controllo turno
                                                  // 1 - cambio fila con cambio turno
                                                  // 2 - salto una posizione ma non cambio fila
#define ABILITA_MAGAZZINI_MULTI_NODO              // Abilita la gestione di un nodo per ogni posizione nella gestione
                                                  // delle missioni TGV
#define GESTIONE_DINAMICA_MAG                 0   // Abilita la gestione posizione di dinamica in magazzino:
                                                  // Il supervisore calcola la posizione reale di deposito in magazzino
#define DEPOSITO_NEL_PIU_VICINO               0   // 0 - deposito a numerazione
                                                  // 1 - deposito nel più vicino
                                                  // recuperandola dall'offset da nodo del TGV. (0 = non attiva)
#define SOVRAPPOSIZIONE_CARWASH               0   // Abilitazione sovrappozione anche palette CAR_WASH
//#define OBBLIGO_ASSEGNAZIONE_MANUALE_MAG          // Non è ammessa Assegnazione==MG_ASS_AUTO, è obbligatoria
                                                  // l'assegnazione manuale, in caso di MG_ASS_AUTO il mag
                                                  // viene scartato per il deposito
#define SOVRAPPOSIZIONE_PRESE_INCOMPLETE          // Attiva la sovrapposizione a mag anche delle prese che risultano
#define CONTROLLO_NODO_DEPOSITO               0   // Controlla nodo occupato/riservato magazzino

#define SVUOTAMENTO_VUOTI_MENO_PRESENZE           // Nello svuotamento magazzini vuoti privilegia il magazzino con meno
                                                  // presenze rispetto al magazzino + vicino

#define CONTROLLO_ASSEGNAZIONE_SPOSTAMENTO    1   // Tipo spostamento pallet in magazzino
                                                  // 0 - NON controlla l'ASSEGNAZIONE del CODICE
                                                  // 1 - controlla l'ASSEGNAZIONE del CODICE
                                                  // Nota: lavora in abbinato allo stato "AbilitaSvuotamentoManualeMag"

//------------------------------------------------
//  GESTIONE_ASRV
//------------------------------------------------
#define GESTIONE_ASRV

//------------------------------------------------
//  GESTIONE DELLA GUIDA LASER
//------------------------------------------------
//#define GESTIONE_CURVE                       // Flags di abilitazione della gestione curve nel
                                             // programma del TGV
#define CURVE_DI_BEZIER                   2  // Gestione curve di Bezier:
                                             // 0 - disattivate (solo ARCHI DI CRF)
                                             // 1 - tutte.
                                             // 2 - solo quadratiche.
                                             // 3 - solo cubiche.
#define TYP_NODO_RIENTRO                  1  // Gestione da EDITOR NODI PER RIENTRO
                                             //
#define DIST_MIN_ING_CURVA               20  // Valore minimo tra 2 nodi con il secondo di inizio
                                             // curva
#define OFFSET_LASER_RUOTA             1150  // Offset tra Posizione del laser e baricentro TGV
#define OFFSET_ROTAZIONE_STANDARD        90  // Angolo di sfasamento tra il riferimento del LASER e
                                             // il riferimento CARTESIANO tradizionale (0 - nella
                                             // casella di Editor).
#define OFFSET_RAGGIO_DWG_RAGGIO_CALC     3  // Offset ammesso tra raggio dichiarato su DWG e raggio
                                             // calcolato da PC (espesso in cm)
#define MAXZONEMAPPING                   40  // Numero massimo di zone mapping
#define MAXRIFLETTORI                    32  // Numero massimo di riflettori all'interno di una zona
#define CAMBIO_ROTAZIONE_SU_START_CURVA   0  // Ha validità nella conversione CURVA <--> ROTAZIONI nella gestione
                                             // precedenze.
                                             // 1 - Già sul primo nodo della curva, il TGV viene considerato
                                             //     con la rotazione che avrà a fine curva.
                                             // 0 - Solo sul nodo di uscita curva, il TGV viene considerato
                                             //     con la rotazione di fine curva.
#define AREA_DM_COORDINATE_COMPATTATA        // Solo per gli impianti predisposti per evitare sforamento area DM coordinate
                                             // vengono trasferiti solo 2 DM a Nodo (DM1=Nodo DM2=X DM3=Y  --> DM1=X DM2=Y )

#define DIST_MIN_PER_CURVA_STRETTA     100   // Se una curva non ha i nodi che in X e Y differiscono di
                                             // almeno questo valore, il TGV viene considerato in rotazione
                                             // durante tutto l'arco
#define RALL_CURVA_NON_IN_INGOMBRO       5   // Ha significato nel campo Rallentamento Editor NODI
                                             // Impostando questo valore si assume che l'AGV abbia un percorso curvilineo
                                             // NON in ingombro ai fini di una ottimizzazione
                                             // della occupazione nodi (non in ingombro ai percorsi vicini):
                                             // - E' come se l'AGV arrivasse da un tratto rettilineo
                                             // - Tale assunto permette di occupare il nodo uscita curva se le condizioni dei nodi adiacenti lo permettono
                                             // - Per ora rientrano in questa categoria i percorsi su "rette parallele"
//------------------------------------------------
//      dal file    OMRON.CPP
//------------------------------------------------
#define   TIMEOUTSERIALE              300    // Valore di TimeOut seriale utilizzato
#define   WINCOM16                      2    // Da impostare con il numero di seriali
                                             // da gestire con API WINDOWS es:
                                             // WINCOM16=3   COM1..3 con api e
                                             // COM4.. con funzioni DOS

// ------------------------------------------------
//  Reset fine operazione a baia
// ------------------------------------------------
#define NUM_CICLI_RESET_FINEOP         30    // Numero cicli dopodichè azzerare il "Fine Operazione" su stazione

// ------------------------------------------------
//  Ottimizzazioni specifiche per i vari Impianti.
// ------------------------------------------------
//#define RITARDO_PARTENZA_TGV_DA_PT           // Da attivare per aumentare i controlli di occupazione nodi
                                             // in partenza dai punti terminali
//#define GESTIONE_TIPOBOX                     // Da abilitare solo se si desidera assegnare i magazzini
                                             // in base al TipoBox.
// ------------------------------------------------
//   ABILITAZIONI RISERVATE AL TEST_INTERNO
// ------------------------------------------------
//#define TEST_INTERNO_TECNOFERRARI

#endif;


