// ------------------------------------------------------------------------
//                file       REMOTO.H
// ------------------------------------------------------------------------
#ifndef RemotoH
#define RemotoH
//#include <stdlib.h>
//#include <classlib/file.h>

#include "TecnoFer.h"
#include "Variabil.h"

#ifndef MAINFILE
    #define TIPO_VARIABILE extern
#else
    #define TIPO_VARIABILE
#endif

union  OldComandi{
        struct stat_comandi bit;        
        unsigned int        val;        // valore globale word stato
        };

struct VISUALIZZA_TGV{
          short int OldPos;             // Old posizione
          short int OldOffSetNodo;      // Old offset da nodo
          short int OldRot;             // Old rotazioni a calamita/laser
          short int OldGLaser;          // Old G del laser         
                int OldXLaser;          // Old coordinata X laser
                int OldYLaser;          // Old coordinata Y laser
          union     OldComandi Old_s;   // Old Bit di stato prima dello spegnimento
};

// --------------------------
//  BLOCCO SISTEMA
// --------------------------
TIPO_VARIABILE bool BloccoSistema;                                       // Gestione del blocco del Sistema;
TIPO_VARIABILE short int RitardoAlBlocco;                                // Gestione si un Ritardo sul Blocco del Sistema.

// --------------------------
//  Elenco Variabili GLOBALI
// --------------------------
TIPO_VARIABILE int  IdPC;
TIPO_VARIABILE int  RemRitardoAllaComunicazione;
TIPO_VARIABILE bool RemPrimaScansione;
TIPO_VARIABILE char PercorsoComandi[200];
TIPO_VARIABILE char PercorsoStato[200];
TIPO_VARIABILE char PercorsoProduzio[200];
TIPO_VARIABILE char PercorsoDB4[200];
TIPO_VARIABILE char PercorsoArchPal[200];
TIPO_VARIABILE char PercorsoOld[200];
TIPO_VARIABILE char PercorsoSorg[200];
TIPO_VARIABILE char PercorsoLog[100];
TIPO_VARIABILE char IpAddressClient[200];
TIPO_VARIABILE VISUALIZZA_TGV VisualizzaTgv[MAXAGV+1];


// --------------------------
//  Elenco Funzioni GLOBALI
// --------------------------
TIPO_VARIABILE char visulizza_tgv_name_sorg[200];
TIPO_VARIABILE char visulizza_tgv_name_dest[200];

int  load_file_visualizza_tgv(char *all_mess);
int  save_file_visualizza_tgv(char *all_mess);

// --------------------------
//  Elenco Funzioni GLOBALI
// --------------------------
void  RemSalvaAnomalie( bool Tutto );
int   RemGestioneServer( char *all_mess );
int   RemGestioneClient( char *all_mess );
void  RemReadIniProject();
int   RemBackupDirectoryLavoro( char *all_mess);
int   RemLoadIdentificativoPC(char *filename, int* Id, char *all_mess);
int   RemCronologicoGenerico(char *FileName, char *messaggio);
int   RemConfrontaDateFile( char *FileSorg, char *FileDest );
int   RemSincronizzaTimeFile( char *FileSorg, char *FileDest, char *all_mess );
int   RemCreaFileComando ( char *FileComando, BYTE *PtrStruttura, int SizeStruttura, char *all_mess );
int   RemLeggiComandoModificaMac( char *all_mess );
int   RemLeggiComandoModificaBaia( char *all_mess );
int   RemLeggiComandoModificaMag( char *all_mess );
int   RemLeggiComandoModificaNodi( char *all_mess );
int   RemLeggiComandoModificaMissTGV( char *all_mess );
int   RemLeggiComandoModificaPosTGV( char *all_mess );
int   RemLeggiComandoModificaStopTGV( char *all_mess );
int   RemLeggiComandoSemiautomatico( char *all_mess );
int   RemLeggiComandoResetMissioneTGV( char *all_mess );
int   RemLeggiComandoResetChiamataTGV( char *all_mess );
int   RemLeggiComandoLiberaNodi( char *all_mess );
int   RemLeggiComandoLogOperatore( char *all_mess );
int   RemLeggiComandoBackupManuale( char *all_mess );
int   RemSalvaConfigurazionePLC();

int   RemSaveTimeZone ( char *all_mess );
int   RemLoadTimeZone (long int *plant_timezone, char *all_mess );

//--------------------------------------------------
// Autorizzazioni Client/Server
//--------------------------------------------------
int   RemScriviIndirizzoClient( char *all_mess );
int   RemLeggiIndirizzoClient ( char *all_mess );
int   RemControlloClientAutorizzato( char *all_mess );

//--------------------------------------------------
// Gestione blocco del sistema
//--------------------------------------------------
void  RemGestioneBloccoSistema(bool LeggiDM);

#endif;
