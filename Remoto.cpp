// ------------------------------------------------------------------------
//             file     REMOTO.CPP
// ------------------------------------------------------------------------
//
// Funzioni di accesso ai files in una configurazione SERVER/CLIENT
//
#include <io.h>
#include <stdio.h>
#include <direct.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <share.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mem.h>
#include <time.h>

#include "Layout.h"
#include "Tecnofer.h"
#include "UdpServer1.h"
#include "OmronEth.h"
#include "Remoto.h"
#include "Agvwin.h"
#include "Main.h"

//  ---------------------
//   STRUTTURA NODI
//  ---------------------
//  0,1 = x,y posizione centrale nodo
//  2 = numero nodo
//  3 = occupazione agv ( numero AGV oppure libero = 0 )
//-------------------------------------------------------
extern int ConvLeggiFileGraphPuntiAscii( char *NomeFile, char* all_mess);

//-------------------------------------------------
// Elenco Funzioni:
// int RemLoadIdentificativoPC( char *all_mess)
// int RemGestioneServer( char *all_mess );
// int RemGestioneClient( char *all_mess );
// int RemConfrontaDateFile( char *FileSorg, char *FileDest );
// int RemSincronizzaTimeFile( char *FileSorg, char *FileDest, char *all_mess );
//
//-------------------------------------------------
//------------------------------- o ----------------------------------

int RemotoCiclico=0;
int RemIdClientTrasmesso=0;


// -----------------------------
//   load_file_visualizza_tgv()
// -----------------------------
// Dal file VISUATGV.DAT per visualizzazione in tempo reale TGV
//
int load_file_visualizza_tgv(char *all_mess, bool *CaricoScarico)
/**********************************/
{
  int i, err=0;
  int len, handle;

  *CaricoScarico=false;
  strcpy(all_mess, "OK");
  memset(&VisualizzaTgv, 0, sizeof( VisualizzaTgv ));
  //-------------------------------------------------
  // Controllo se il File pu• essere Letto aprendolo
  // in modalit… di Condivisione.
  //-------------------------------------------------
  handle = sopen( visulizza_tgv_name_sorg, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
      close( handle );
      sprintf(all_mess, MESS[112], visulizza_tgv_name_sorg);
      return 1;
  }
  //-------------------------------------------------
  // Ciclo lettura file VISUATGV.DAT
  //-------------------------------------------------
  len = read( handle, &VisualizzaTgv, sizeof( VisualizzaTgv ) );
  if(len != sizeof(VisualizzaTgv)) err=1;
  if(err){
     memset(&VisualizzaTgv, 0, sizeof( VisualizzaTgv ));
     sprintf(all_mess, MESS[112], visulizza_tgv_name_sorg);
  }
  close( handle );
  //-------------------------------------------------
  // Controllo se almeno un TGV ha completato una
  // missione di carico/scarico
  //-------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(VisualizzaTgv[i].Old_s.bit.carA==AGV[i]->stato.s.bit.carA &&
        VisualizzaTgv[i].Old_s.bit.carB==AGV[i]->stato.s.bit.carB) continue;
     *CaricoScarico=true;
     break;
  }

  return err;
}

// -----------------------------
//   save_file_visualizza_tgv()
// -----------------------------
// Dal file VISUATGV.DAT per visualizzazione in tempo reale TGV
//
int save_file_visualizza_tgv(char *all_mess)
/**********************************/
{
  int i, err=0;
  int len, handle;

  //----------------------------------
  // Aggiornamento struttura
  //----------------------------------
  for(i=1; i<=MAXAGV; i++){
     VisualizzaTgv[i].OldPos         = AGV[i]->stato.pos;
     VisualizzaTgv[i].OldOffSetNodo  = AGV[i]->stato.offset_da_nodo;
     if(AGV[i]->mission.GestioneLaser==false) VisualizzaTgv[i].OldRot = AGV[i]->stato.rot_calamite;
     else                                     VisualizzaTgv[i].OldRot = AGV[i]->stato.rot_laser;
     VisualizzaTgv[i].OldGLaser      = AGV[i]->stato.g_laser;
     VisualizzaTgv[i].OldXLaser      = AGV[i]->stato.x_laser;
     VisualizzaTgv[i].OldYLaser      = AGV[i]->stato.y_laser;
     VisualizzaTgv[i].Old_s.val      = AGV[i]->stato.s.val;
  }
  //----------------------------------
  // Apertura file VISUATGV.DAT
  //----------------------------------
  handle = sopen( visulizza_tgv_name_dest, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
  if( handle<0 ){
     close( handle );
     sprintf(all_mess, MESS[113], visulizza_tgv_name_dest);
     return 1;
  }
  //----------------------------------
  // Ciclo Scrittura file VISUATGV.DAT
  //----------------------------------
  len= write( handle, &VisualizzaTgv, sizeof( VisualizzaTgv ));
  if (len != sizeof( VisualizzaTgv )){
     err = 1;
     memset(&VisualizzaTgv, 0, sizeof( VisualizzaTgv ));
     sprintf(all_mess, MESS[113], visulizza_tgv_name_dest);
  }
  close( handle );
  return err;
}

//--------------------------------------------------------------------
// Funzioni di accesso ai files in una configurazione SERVER/CLIENT
//--------------------------------------------------------------------
// -----------------------
//  RemGestioneServer()
// -----------------------
//  Funzione principale per la gestione del SERVER
//
int RemGestioneServer( char *all_mess )
/*************************************************/
{
  int err;
  int NumAgv;
  short int stato_rot;

  //-------------------------------------------------
  // Verifico se l'dentificativo Š valido
  //-------------------------------------------------
  err =0;
  strcpy( all_mess, "" );
  if( IdPC!=1 ) return 0;

  //-------------------------------------------------
  // Operazioni da eseguire solo la prima scansione
  //-------------------------------------------------
  if( RemPrimaScansione==true ){
     sprintf(PercorsoComandi,  "%sCOMANDI\\",  PathProject.c_str());
     sprintf(PercorsoStato,    "%sSTATO\\",    PathProject.c_str());
     sprintf(PercorsoOld,      "%sOLD\\",      PathProject.c_str());
     sprintf(PercorsoSorg,     "%sSORG\\",     PathProject.c_str());
     sprintf(PercorsoLog,"%s%s", PercorsoStato, "Server.log" );
     //---------------------------------------
     // DB in rete
     //---------------------------------------
     sprintf(PercorsoProduzio, "%sPRODUZIO",   PathProject.c_str());

     strcpy(P.chiamate_name_sorg, "CHIAMATE.DAT");
     strcpy(P.chiamate_name_dest, "CHIAMATE.DAT");

     strcpy(P.baie_name_sorg, "BAIE.DAT");
     strcpy(P.baie_name_dest, "BAIE.DAT");

     strcpy(P.gruppibaie_name_sorg, "GRUPPIBA.DAT");
     strcpy(P.gruppibaie_name_dest, "GRUPPIBA.DAT");

     strcpy(porteimp_name_sorg, "PORTEIMP.DAT");
     strcpy(porteimp_name_dest, "PORTEIMP.DAT");

     strcpy(M.mag_name_sorg, "MAGAZZIN.DAT");
     strcpy(M.mag_name_dest, "MAGAZZIN.DAT");

     strcpy(P.cbat_name_sorg, "CAMBIOBAT.DAT");
     strcpy(P.cbat_name_dest, "CAMBIOBAT.DAT");


     strcpy(M.tipibox_name_sorg, "TIPIBOX.DAT");
     strcpy(M.tipibox_name_dest, "TIPIBOX.DAT");

     strcpy(N.FileNameNodoBusySorg, "NODOBUSY.DAT");
     strcpy(N.FileNameNodoBusyDest, "NODOBUSY.DAT");

     strcpy( N.FileNameNodoGrdSorg, "NODOGRD.DAT");
     strcpy( N.FileNameNodoGrdDest, "NODOGRD.DAT");

     strcpy(N.file_name_sorg, "NODO.DAT");
     strcpy(N.file_name_dest, "NODO.DAT");

     for( NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        sprintf(AGV[NumAgv]->mission_name_sorg, "MISSION%d.DAT", NumAgv);
        sprintf(AGV[NumAgv]->mission_name_dest, "MISSION%d.DAT", NumAgv);
        //----------------------------------------
        // GESTIONE_ASRV
        //----------------------------------------
        sprintf(AGV[NumAgv]->ASRV_file_stato_sorg, "ASRVSTAT%d.DAT", NumAgv);
        sprintf(AGV[NumAgv]->ASRV_file_stato_dest, "ASRVSTAT%d.DAT", NumAgv);
        sprintf(AGV[NumAgv]->ASRV_file_mission_sorg, "ASRVMISS%d.DAT", NumAgv);
        sprintf(AGV[NumAgv]->ASRV_file_mission_dest, "ASRVMISS%d.DAT", NumAgv);
     }

     //----------------------------------------------------
     // FILE DI VISUALIZZAZIONE TGV
     //----------------------------------------------------
     strcpy(visulizza_tgv_name_sorg, "VISUATGV.DAT");
     strcpy(visulizza_tgv_name_dest, "VISUATGV.DAT");
     //----------------------------------------------------
     // Archivio delle statistiche TGV ---> Gestione DB4
     //----------------------------------------------------
     strcpy(FileTabellaTimTgv, "TIMTGV");
  }
  //---------------------------------------------------------------------
  // (A) Operazioni di aggiornamento x monitoraggio per il CLIENT
  //---------------------------------------------------------------------
  // (A00) BACKUP
  if( RemBackupDirectoryLavoro( all_mess )){
     if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
  }
  //--------------------------------------------------
  // (A00) SALVATAGGIO FILE DI VISUALIZZAZIONE TGV
  //--------------------------------------------------
  err = save_file_visualizza_tgv(all_mess);
  if(err) RemCronologicoGenerico( PercorsoLog, all_mess);

  //--------------------------------------------------
  // (A01) Aggiorno missione AGV e NODO_BUSY
  //--------------------------------------------------
  for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
     //--------------------------------------------------
     // Recupero lo stato di comunicazione in base al
     // tipo di guida
     //--------------------------------------------------
     stato_rot = AGV[ NumAgv]->stato.rot_calamite;
     if(AGV[ NumAgv]->mission.GestioneLaser==true) stato_rot = AGV[ NumAgv]->stato.rot_laser;
     //--------------------------------------------------
     // Aggiorno lo stato missione a ogni variazione
     //--------------------------------------------------
     if( AGV[ NumAgv]->mission.OldPos              != AGV[ NumAgv]->stato.pos ||
         AGV[ NumAgv]->mission.OldRot              != stato_rot ||
         memcmp(&AGV[NumAgv]->mission.old_allarm[0], &AGV[NumAgv]->allarme_esterno[0], sizeof(AGV[NumAgv]->mission.old_allarm))!=0 ||
         AGV[ NumAgv]->mission.old_allarme_interno != AGV[ NumAgv]->old_allarme_interno ||
         AGV[ NumAgv]->mission.OldStart            != AGV[ NumAgv]->stato.start ){
        err = AGV[NumAgv]->save_mission(all_mess);
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     }
  }

  //---------------------------------------------------------------------
  // (B) Intercetto eventuali comandi inviati da CLIENTs
  //---------------------------------------------------------------------
  // CHIAMATE
  err = RemLeggiComandoModificaMac( all_mess );
  if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
  // BAIE
  err = RemLeggiComandoModificaBaia( all_mess );
  if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
  // MAGAZZINI
  err = RemLeggiComandoModificaMag( all_mess );
  if(err) RemCronologicoGenerico( PercorsoLog, all_mess);

  //--------------------------------------------------
  // (B21) NODI Recupero le eventuali modifiche
  //--------------------------------------------------
  err = RemLeggiComandoModificaNodi( all_mess );
  if(err) RemCronologicoGenerico( PercorsoLog, all_mess);

  //----------------------------------------------------
  // (B4..) TGV Recupero le eventuali modifiche:
  //----------------------------------------------------
  //---- (B41) Posizione
  err =  RemLeggiComandoModificaPosTGV( all_mess );
  if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
  //---- (B42) Stop manuale TGV
  err =  RemLeggiComandoModificaStopTGV( all_mess );
  if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
  //---- (B43) Missione ( Dati generali incl., escl, ecc. )
  //
  // IMPORTANTE:
  // Lo stato missione deve essere salvato per ultimo
  // in modo da conservare STOP MANUALE e POSIZIONE
  //
  err = RemLeggiComandoModificaMissTGV( all_mess );
  if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
  //---- (B44) Semiautomatico
  err = RemLeggiComandoSemiautomatico( all_mess );
  if(err) RemCronologicoGenerico( PercorsoLog, all_mess);

  //---- (B45) Annulla missione TGV
  err = RemLeggiComandoResetMissioneTGV( all_mess );
  if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
  //---- (B45)b Annulla chiamata TGV  AL-23/01 (3) separato comando di reset chiamata
  err = RemLeggiComandoResetChiamataTGV( all_mess );
  if(err) RemCronologicoGenerico( PercorsoLog, all_mess);

  //---- (B46) Libera nodi dell'Impianto
  err = RemLeggiComandoLiberaNodi( all_mess );
  if(err) RemCronologicoGenerico( PercorsoLog, all_mess);

  //---- (B49) Lettura LOG OPERATORE da PC CLIENTE
  err = RemLeggiComandoLogOperatore( all_mess );
  if(err) RemCronologicoGenerico(PercorsoLog, all_mess);

  //---- (B50) Lettura BACKUP MANUALE da PC CLIENTE
  err = RemLeggiComandoBackupManuale( all_mess );
  if(err) RemCronologicoGenerico(PercorsoLog, all_mess);


  //---------------------------------------------------
  // Controllo delle Condizioni di Blocco Sistema
  //---------------------------------------------------
  RemGestioneBloccoSistema(TRUE);


  RemPrimaScansione=false;
  return err;
}
//---------------------------------------------------------------------------
// -----------------------
//  RemGestioneClient()
// -----------------------
//  Funzione principale per la gestione del CLIENT
//
int RemGestioneClient( char *all_mess )
/*************************************************/
{
  int i, err=0;
  int Rest;
  int msec=0;
  int NumeroPlc;
  char NomeFileSorg[100]="";
  char NomeFileDest[100]="";
  char Path[100];
  char buffer[101]="";
  struct ffblk ffblk;
  SYSTEMTIME TimeClient1;
  SYSTEMTIME TimeClient2;
  bool CaricoScarico;
  static int NumAgv=1;
  unsigned char node;

  //-------------------------------------------------------------------
  // Verifico se l'dentificativo Š valido
  //-------------------------------------------------------------------
  err = 0;
  strcpy( all_mess, "" );
  if( IdPC<2 ) return 0;

  GetLocalTime( &TimeClient1 );         // start ciclo client
  //------------------------------------------------------------------
  // Verifico se connesso a rete  (deve esserci almeno un file in S:)
  //------------------------------------------------------------------
  if( RemRitardoAllaComunicazione>0                      ) RemRitardoAllaComunicazione--;
  if( RemRitardoAllaComunicazione && !RemPrimaScansione  ){Automatico=false; return 0;}
  Automatico=true;
  FormMain->SBarImpianto->Panels->Items[0]->Text = "";
  sprintf( Path, "%s\\*.*", DriveServer );
  if( findfirst( Path, &ffblk, 0)){
     sprintf( all_mess, "Manca la connessione di rete %s", DriveServer);
     sprintf( PercorsoLog,"%s\\STATO\\PC%d.LOG", PathProject.c_str(), IdPC );
     RemCronologicoGenerico( PercorsoLog, all_mess);
     FormMain->SBarImpianto->Panels->Items[0]->Text = all_mess;
     RemRitardoAllaComunicazione=30;
     RemPrimaScansione=false;
     Automatico=false;
     return 1;
  }
  else FormMain->SBarImpianto->Panels->Items[0]->Text="";

  //-------------------------------------------------
  // Operazioni da eseguire solo la prima scansione
  //-------------------------------------------------
  if( RemPrimaScansione==true ){
     //---------------------------------------
     // Imposto i percorsi di rete
     //---------------------------------------
     sprintf( PercorsoComandi, "%s\\COMANDI\\", DriveServer);
     sprintf( PercorsoStato,   "%s\\STATO\\",   DriveServer);
     sprintf( PercorsoOld,     "%sOLD\\",       PathProject.c_str());
     sprintf( PercorsoSorg,    "%sSORG\\",      PathProject.c_str());
     sprintf( PercorsoLog, "%sSTATO\\PC%d.LOG", PathProject.c_str(), IdPC );
     //---------------------------------------
     // DB in rete
     //---------------------------------------
     sprintf( PercorsoProduzio, "%s\\PRODUZIO",   DriveServer);

     //---------------------------------------
     // Struttura missioni dei TGV
     //---------------------------------------
     for( NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        sprintf(AGV[NumAgv]->mission_name_sorg, "%s\\MISSION%d.DAT", DriveServer,NumAgv);
        sprintf(AGV[NumAgv]->mission_name_dest, "MISSION%d.DAT", NumAgv);
        //----------------------------------------
        // GESTIONE_ASRV
        //----------------------------------------
        sprintf(AGV[NumAgv]->ASRV_file_stato_sorg, "%s\\ASRVSTAT%d.DAT", DriveServer,NumAgv);
        sprintf(AGV[NumAgv]->ASRV_file_stato_dest, "ASRVSTAT%d.DAT", NumAgv);
        sprintf(AGV[NumAgv]->ASRV_file_mission_sorg, "%s\\ASRVMISS%d.DAT", DriveServer,NumAgv);
        sprintf(AGV[NumAgv]->ASRV_file_mission_dest, "ASRVMISS%d.DAT", NumAgv);
     }

     //----------------------------------------------------
     // FILE DI VISUALIZZAZIONE TGV
     //----------------------------------------------------
     sprintf(visulizza_tgv_name_sorg, "%s\\VISUATGV.DAT", DriveServer);
     strcpy(visulizza_tgv_name_dest,  "VISUATGV.DAT");

     //---------------------------------------
     // File con le strutture di sistema
     //---------------------------------------
     sprintf(P.chiamate_name_sorg, "%s\\CHIAMATE.DAT", DriveServer);
     strcpy(P.chiamate_name_dest, "CHIAMATE.DAT");

     sprintf(P.baie_name_sorg, "%s\\BAIE.DAT", DriveServer);
     strcpy(P.baie_name_dest, "BAIE.DAT");

     sprintf(P.gruppibaie_name_sorg, "%s\\GRUPPIBA.DAT", DriveServer);
     strcpy(P.gruppibaie_name_dest, "GRUPPIBA.DAT");

     sprintf(porteimp_name_sorg, "%s\\PORTEIMP.DAT", DriveServer);
     strcpy(porteimp_name_dest,  "PORTEIMP.DAT");

     sprintf(M.mag_name_sorg, "%s\\MAGAZZIN.DAT", DriveServer);
     strcpy(M.mag_name_dest, "MAGAZZIN.DAT");

     sprintf(M.tipibox_name_sorg, "%s\\TIPIBOX.DAT", DriveServer);
     strcpy(M.tipibox_name_dest, "TIPIBOX.DAT");

     sprintf(N.FileNameNodoBusySorg, "%s\\NODOBUSY.DAT", DriveServer);
     strcpy(N.FileNameNodoBusyDest, "NODOBUSY.DAT");

     sprintf( N.FileNameNodoGrdSorg, "%s\\NODOGRD.DAT", DriveServer);
     sprintf( N.FileNameNodoGrdDest, "NODOGRD.DAT");

     sprintf(N.file_name_sorg, "%s\\NODO.DAT", DriveServer);
     strcpy(N.file_name_dest, "NODO.DAT");


     sprintf(P.cbat_name_sorg, "%s\\CAMBIOBAT.DAT", DriveServer);
     strcpy(P.cbat_name_dest, "CAMBIOBAT.DAT");

     //----------------------------------------------------
     // Archivio dei TEMPI TGV ---> Gestione DB4
     //----------------------------------------------------
     sprintf(FileTabellaTimTgv, "%s\\TIMTGV", PercorsoProduzio );
  }

  //---------------------------------------------------------------------
  // (A) Operazioni di aggiornamento x monitoraggio da parte del CLIENT
  //---------------------------------------------------------------------
  // (A00) BACKUP
  if( RemBackupDirectoryLavoro( all_mess )){
     if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
  }

  //--------------------------------------------------
  // (A00) AGV Rinfresco VISUALIZZAZIONE STATO TGV
  //--------------------------------------------------
  CaricoScarico=false;
  err = load_file_visualizza_tgv(all_mess, &CaricoScarico);
  if(err) RemCronologicoGenerico( PercorsoLog, all_mess);

  //--------------------------------------------------
  // (A01) AGV Rinfresco Missioni AGV
  //--------------------------------------------------
  err=0;
  if(NumAgv>MAXAGV) NumAgv=1;
  if(FormTgv->EditorAgvAttivo!=NumAgv){      // carico lo stato dal SERVER solo a EDITOR CHIUSO
     err = AGV[NumAgv]->load_mission(all_mess);
     if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
  }
  if(!err){
     //-----------------------------------------
     // Aggiornamento grafico di tutti i TGV
     //-----------------------------------------
     for(i=1; i<=MAXAGV; i++){
        AGV[i]->stato.pos            = VisualizzaTgv[i].OldPos;
        AGV[i]->stato.s.val          = VisualizzaTgv[i].Old_s.val;
        AGV[i]->stato.offset_da_nodo = VisualizzaTgv[i].OldOffSetNodo;
        if(AGV[i]->mission.GestioneLaser==false) AGV[i]->stato.rot_calamite = VisualizzaTgv[i].OldRot;
        else                                     AGV[i]->stato.rot_laser    = VisualizzaTgv[i].OldRot;
        AGV[i]->stato.g_laser        = VisualizzaTgv[i].OldGLaser;
        AGV[i]->stato.x_laser        = VisualizzaTgv[i].OldXLaser;
        AGV[i]->stato.y_laser        = VisualizzaTgv[i].OldYLaser;
     }
     //-----------------------------------------
     // Aggiornamento struttura di un TGV
     // alla volta
     //-----------------------------------------
     AGV[NumAgv]->stato.start          = AGV[ NumAgv]->mission.OldStart;
     memcpy(&AGV[NumAgv]->allarme_esterno[0], &AGV[NumAgv]->mission.old_allarm[0], sizeof(AGV[NumAgv]->allarme_esterno));
     AGV[NumAgv]->old_allarme_interno  = AGV[NumAgv]->mission.old_allarme_interno;
     AGV[NumAgv]->allarme_interno      = AGV[NumAgv]->mission.old_allarme_interno;
     err = AGV[NumAgv]->save_mission(all_mess);
     if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
  }
  NumAgv++;


  //--------------------------------------------------
  // INIPROJECT.INI
  //--------------------------------------------------
  sprintf(NomeFileSorg, "%s\\INIPROJECT.INI", DriveServer);
  sprintf(NomeFileDest, "INIPROJECT.INI");
  Rest = RemConfrontaDateFile(NomeFileSorg, NomeFileDest);
  if(Rest>=1 || RemPrimaScansione){
     //--------------------------------------------------
     // Rilettura da file INI SERVER dei dati modificati
     //--------------------------------------------------
     RemReadIniProject();
     //--------------------------------------------------
     // Salvataggio in locale
     //--------------------------------------------------
     FormMain->WriteIniProject();
     //--------------------------------------------------
     // Sincronizzazione data/ora
     //--------------------------------------------------
     err = RemSincronizzaTimeFile( NomeFileSorg, NomeFileDest, all_mess );
     if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
  }

  //-----------------------------------------
  // (A22) CHIAMATE Rinfresco DATI
  //-----------------------------------------
  if( RemotoCiclico==0 || RemotoCiclico==1 ){
     //--------------------------------------------
     // (A20) Impostazioni Impianto "GRAPHVAR.DAT"
     //--------------------------------------------
     sprintf(NomeFileSorg, "%s\\GRAPHVAR.DAT", DriveServer);
     sprintf(NomeFileDest, "GRAPHVAR.DAT");
     Rest = RemConfrontaDateFile( NomeFileSorg, NomeFileDest );
     if( Rest>=1 || RemPrimaScansione ){
        CopyFile( NomeFileSorg, NomeFileDest, FALSE );
        //--------------------------------------------------------
        // Sincronizzazione data/ora file locale e remoto
        //
        // 31/12/2013 - solo file in locale esiste per evitare
        //              di generarlo a 0 byte in remoto
        //--------------------------------------------------------
        if(!access(NomeFileDest, 0)){
           err = RemSincronizzaTimeFile( NomeFileSorg, NomeFileDest, all_mess );
           if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
        }
        //--------------------------------------------------------
        // Recupero da GRAPHVAR.DAT la nuova struttura Punti[][5]
        //--------------------------------------------------------
        ConvLeggiFileGraphPuntiAscii("GRAPHVAR.DAT", all_mess);
     }
     //--------------------------------------------------
     // (A23) Copiatura dei file di configurazione PLC
     //--------------------------------------------------
     sprintf(NomeFileSorg, "%s\\CONFIGPLC.DAT", DriveServer);
     sprintf(NomeFileDest, "CONFIGPLC.DAT");
     Rest = RemConfrontaDateFile( NomeFileSorg, NomeFileDest );
     if( Rest>=1 || RemPrimaScansione ){
        CopyFile( NomeFileSorg, NomeFileDest, FALSE );
        //-----------------------------------------------------
        // Rilettura dei dati recuerati
        //-----------------------------------------------------
        OmrLeggiConfigurazionePLC();
        //-----------------------------------------------------
        // Aggiornamento dei dati in base al IP LOCAL ADDRESS
        // del Pc Client
        //-----------------------------------------------------
        for(NumeroPlc=0; NumeroPlc<=MAXPLCCON; NumeroPlc++){
           FormServerUDP->IPToNode(FormServerUDP->LocalIPAddr, &node);
           OmrPlc[ NumeroPlc ].SA1 = node;
        }
        //-----------------------------------------------------
        // Salvataggio della configurazione aggiornata
        //-----------------------------------------------------
      //OmrSalvaConfigurazionePLC();
        RemSalvaConfigurazionePLC();
        //-----------------------------------------------------
        // Sincronizzazione dei file
        //-----------------------------------------------------
        err = RemSincronizzaTimeFile( NomeFileSorg, NomeFileDest, all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     }
     //--------------------------------------------------
     // (A24) Copiatura dei file di configurazione COM
     //--------------------------------------------------
     sprintf(NomeFileSorg, "%s\\CONFIGCOM.DAT", DriveServer);
     sprintf(NomeFileDest, "CONFIGCOM.DAT");
     Rest = RemConfrontaDateFile( NomeFileSorg, NomeFileDest );
     if( Rest>=1 || RemPrimaScansione ){
        CopyFile( NomeFileSorg, NomeFileDest, FALSE );
        err = RemSincronizzaTimeFile( NomeFileSorg, NomeFileDest, all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     }
     //--------------------------------------------------
     // CHIAMATE:
     // - MACCHINE
     // - BAIE
     // - MAGAZZINI
     //--------------------------------------------------
     // MACCHINE
     Rest = RemConfrontaDateFile( P.chiamate_name_sorg, P.chiamate_name_dest );
     if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     if( Rest>=1 || RemPrimaScansione ){
        err = P.load_file_chiamate( all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
        if( !err) err = P.save_file_chiamate( all_mess );
        err = RemSincronizzaTimeFile( P.chiamate_name_sorg, P.chiamate_name_dest, all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     }
     // BAIE
     Rest = RemConfrontaDateFile( P.baie_name_sorg, P.baie_name_dest );
     if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     if( Rest>=1 || RemPrimaScansione ){
        err = P.load_file_baie( all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
        if( !err) err = P.save_file_baie( all_mess );
        err = RemSincronizzaTimeFile( P.baie_name_sorg, P.baie_name_dest, all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     }
     // GRUPPIBAIE
     Rest = RemConfrontaDateFile( P.gruppibaie_name_sorg, P.gruppibaie_name_dest );
     if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     if( Rest>=1 || RemPrimaScansione ){
        err = P.load_file_gruppibaie( all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
        if( !err) err = P.save_file_gruppibaie( all_mess );
        err = RemSincronizzaTimeFile( P.gruppibaie_name_sorg, P.gruppibaie_name_dest, all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     }
     // TIPIBOX
     Rest = RemConfrontaDateFile( M.tipibox_name_sorg, M.tipibox_name_dest );
     if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     if( Rest>=1 || RemPrimaScansione ){
        err = M.load_file_tipibox( all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
        if( !err) err = M.save_file_tipibox( all_mess );
        err = RemSincronizzaTimeFile( M.tipibox_name_sorg, M.tipibox_name_dest, all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     }
     // MAGAZZINI
     Rest = RemConfrontaDateFile( M.mag_name_sorg, M.mag_name_dest );
     if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     if( Rest>=1 || RemPrimaScansione ){
        err = M.load_file_mag( all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
        if( !err) err = M.save_file_mag( all_mess );
        err = RemSincronizzaTimeFile( M.mag_name_sorg, M.mag_name_dest, all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     }
  }
  //-----------------------------------------
  // (A31) NODI Rinfresco DATI
  //-----------------------------------------
  if( RemotoCiclico==0 || RemotoCiclico==2 ){
     // --- NODOBUSY.DAT ---
     err = N.load_nodo_busy( all_mess );
     Rest = RemConfrontaDateFile( N.FileNameNodoBusySorg, N.FileNameNodoBusyDest );
     if( Rest>=1 || RemPrimaScansione ){
        if(!err) err = N.save_nodo_busy( all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
        err = RemSincronizzaTimeFile( N.FileNameNodoBusySorg, N.FileNameNodoBusyDest, all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     }
     // --- NODOGRD.DAT ---
     err = N.load_nodo_grd( all_mess );
     if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     Rest = RemConfrontaDateFile( N.FileNameNodoGrdSorg, N.FileNameNodoGrdDest );
     if( Rest>=1 || RemPrimaScansione ){
        if( !err) err = N.save_nodo_grd( all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
        err = RemSincronizzaTimeFile( N.FileNameNodoGrdSorg, N.FileNameNodoGrdDest, all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     }
     // --- NODO.DAT ---
     Rest = RemConfrontaDateFile( N.file_name_sorg, N.file_name_dest );
     if( Rest>=1 || RemPrimaScansione ){
        err = N.load_file_nodi( all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
        if( !err) err = N.save_file_nodi( all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
        err = RemSincronizzaTimeFile( N.file_name_sorg, N.file_name_dest, all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     }
     //--------------------------------------------------
     // PRIORITA.DAT
     //--------------------------------------------------
     sprintf(NomeFileSorg, "%s\\PRIORITA.DAT", DriveServer);
     sprintf(NomeFileDest, "PRIORITA.DAT");
     Rest = RemConfrontaDateFile(NomeFileSorg, NomeFileDest);
     if(Rest>=1 || RemPrimaScansione){
        CopyFile( NomeFileSorg, NomeFileDest, FALSE );
        err = RemSincronizzaTimeFile( NomeFileSorg, NomeFileDest, all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     }
     //--------------------------------------------------
     // PORTEIMP.DAT
     //--------------------------------------------------
     Rest = RemConfrontaDateFile( porteimp_name_sorg, porteimp_name_dest );
     if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     if( Rest>=1 || RemPrimaScansione ){
        err = load_file_porteimp( all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
        if( !err) err = save_file_porteimp( all_mess );
        err = RemSincronizzaTimeFile( porteimp_name_sorg, porteimp_name_dest, all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     }
     // CAMBIOBATTERIE
     Rest = RemConfrontaDateFile( P.cbat_name_sorg, P.cbat_name_dest );
     if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     if( Rest>=1 || RemPrimaScansione ){
        err = P.load_file_cbat( all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
        if( !err) err = P.save_file_cbat( all_mess );
        err = RemSincronizzaTimeFile( P.cbat_name_sorg, P.cbat_name_dest, all_mess );
        if(err) RemCronologicoGenerico( PercorsoLog, all_mess);
     }
  }


  //----------------------------------------
  // Gestione OTTIMIZZAZIONE CICLO CLIENT
  //----------------------------------------
  RemotoCiclico++;
  if( RemotoCiclico>3 ) RemotoCiclico=1;

  GetLocalTime( &TimeClient2 );         // stop ciclo client

  // --- durata ciclo automatico ---
  msec = (int) (TimeClient2.wSecond - TimeClient1.wSecond);
  msec = msec * 1000;
  msec = msec + (int) (TimeClient2.wMilliseconds - TimeClient1.wMilliseconds);
  if( msec<0    ) msec=0;
  if( msec>5000 ) msec=5000;

  //---------------------------------------------------------------------
  // Visualizzazione sulla barra di stato dei tempi di Ciclo
  //---------------------------------------------------------------------
  sprintf(buffer, "%dmsec", msec);
  FormMain->SBarImpianto->Panels->Items[5]->Text = buffer;
  

  //-----------------------------------------------------------------
  // Trasmetti Identificativo CLIENT al SERVER (all'accensione)
  // e controlla che sia sempre lo stesso ai collegamenti sucessivi
  //-----------------------------------------------------------------
  if( RemIdClientTrasmesso==0 ){
     err = RemScriviIndirizzoClient( all_mess );
  }else{
     RemControlloClientAutorizzato( all_mess );
  }


  RemPrimaScansione=false;
  return err;
}
//---------------------------------------------------------------------------
// ---------------------------
//  RemBackupDirectoryLavoro
// ---------------------------
//
int RemBackupDirectoryLavoro( char *all_mess)
/*************************************************/
{
  char NomeFile[100]="";
  char NomeFileCompleto[100]="";
  char StringaComando[151]="";
  int  MesePrecedenteDue;
  int  AnnoPrecedente;
  struct ffblk BlkDest;
  int  err;
  SYSTEMTIME TimeAttuale;

  GetLocalTime( &TimeAttuale );
  //--------------------------------------
  // Cancella i file di 2 mesi precedenti
  //--------------------------------------
  AnnoPrecedente    = TimeAttuale.wYear;
  MesePrecedenteDue = TimeAttuale.wMonth;
  if( TimeAttuale.wMonth  >2 )  MesePrecedenteDue-=2;
  if( TimeAttuale.wMonth ==1 ) {MesePrecedenteDue=11;  AnnoPrecedente--;}
  if( TimeAttuale.wMonth ==2 ) {MesePrecedenteDue=12;  AnnoPrecedente--;}
  sprintf( NomeFile, "%04d%02d*.zip", AnnoPrecedente, MesePrecedenteDue );
  sprintf( NomeFileCompleto, "%s%s", PercorsoOld, NomeFile );
  err = findfirst( NomeFileCompleto, &BlkDest, 0);
  if( err==0 ){
     sprintf( NomeFileCompleto, "%s%s", PercorsoOld, BlkDest.ff_name );
     unlink ( NomeFileCompleto );
  }

  //-------------------------------------------------------
  // Costruisci il Nome del File .ZIP  e se esiste ->SKIP
  //-------------------------------------------------------
  sprintf( NomeFile, "%04d%02d%02d", TimeAttuale.wYear, TimeAttuale.wMonth, TimeAttuale.wDay );
  sprintf( NomeFileCompleto, "%s%s.ZIP", PercorsoOld,  NomeFile );
  if( Created( NomeFileCompleto )) return 0;

  //---------------------------------------------------
  // Backup solo di domenica ????
  //---------------------------------------------------
  //if( TimeAttuale.wDayOfWeek!=0 ) return 0;

  //--------------------------------------------------
  // Cancellazione dei file da reinizializzare
  //--------------------------------------------------
//unlink("LOGTGV.DAT");

  //---------------------------------
  // Crea File ZIP
  //---------------------------------
  sprintf(StringaComando, "pkzip25 -add %s%s -include *.exe *.ita *.ini *.dat *.dbf *.mdx *.lst *.csv *.txt *.xls *.mdb -dir -excl=Produzio", PercorsoOld, NomeFile);
  system( StringaComando );
/*
  BackupInCorso=true;
  FormMain->ZipImpianto->ZipFilename = NomeFileCompleto;
  FormMain->ZipImpianto->AddOptions  = FormMain->ZipImpianto->AddOptions << AddUpdate;   // Update
  FormMain->ZipImpianto->FSpecArgs->Clear();
  strcpy(StringaComando, "*.ini");
  FormMain->ZipImpianto->FSpecArgs->Add(StringaComando);    // Specify filenames.
  strcpy(StringaComando, "*.dat");
  FormMain->ZipImpianto->FSpecArgs->Add(StringaComando);    // Specify filenames.
  strcpy(StringaComando, "*.txt");
  FormMain->ZipImpianto->FSpecArgs->Add(StringaComando);    // Specify filenames.
  strcpy(StringaComando, "*.exe");
  FormMain->ZipImpianto->FSpecArgs->Add(StringaComando);    // Specify filenames.
  strcpy(StringaComando, "*.dbf");
  FormMain->ZipImpianto->FSpecArgs->Add(StringaComando);    // Specify filenames.
  strcpy(StringaComando, "*.mdx");
  FormMain->ZipImpianto->FSpecArgs->Add(StringaComando);    // Specify filenames.
	try{
		FormMain->ZipImpianto->Add();
	}
  catch(...){
		FormMain->SBarImpianto->Panels->Items[0]->Text = "Errore nella compattazione dei file...";
		return 1;
	}
  FormMain->SBarImpianto->Panels->Items[0]->Text = "";
  BackupInCorso=false;
*/
  return 0;
}
//---------------------------------------------------------------------------
//------------------------------------
//     RemSalvaConfigurazionePLC()
//------------------------------------
// Salva su file la configurazione dei PLCs  IP Address ecc.
//
int RemSalvaConfigurazionePLC()
{
  FILE *ptrfile;
  char  stringa[101]="";
  char  FileName[201]="";
  int   NumeroPlc;
  int   NumeroCom;

  //-------------------------------------------------------
  // CONFIGPLC.DAT
  //-------------------------------------------------------
  sprintf(FileName, "%sConfigPlc.DAT", PathProject.c_str());
  ptrfile = fopen( FileName, "wb");
  if(ptrfile<=0) return 1;
  //----------------------------------------------------------------------------
  // Scrittura del file *.INI di configurazione dei PLCs
  //----------------------------------------------------------------------------
  for( NumeroPlc=0; NumeroPlc<=MAXPLCCON; NumeroPlc++){
     sprintf( stringa, "[Plc%d]",        NumeroPlc );
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "DescrPlc=%s",     OmrPlc[ NumeroPlc ].DescrPlc.c_str());
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "TypCom=%d",       OmrPlc[ NumeroPlc ].TypCom);
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "Com=%d",          OmrPlc[ NumeroPlc ].Com);
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "IndexPlc=%d",     OmrPlc[ NumeroPlc ].IndexPlc);
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "LocalPort=%d",    OmrPlc[ NumeroPlc ].LocalPort);
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "RemotePort=%d",   OmrPlc[ NumeroPlc ].RemotePort);
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "RemoteHost=%s",   OmrPlc[ NumeroPlc ].RemoteHost.c_str());
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "ICF=%d",          (unsigned int)(OmrPlc[ NumeroPlc ].ICF));
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "RSV=%d",          (unsigned int)(OmrPlc[ NumeroPlc ].RSV));
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "GCT=%d",          (unsigned int)(OmrPlc[ NumeroPlc ].GCT));
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "SNA=%d",          (unsigned int)(OmrPlc[ NumeroPlc ].SNA));
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "SA1=%d",          (unsigned int)(OmrPlc[ NumeroPlc ].SA1));
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "SA2=%d",          (unsigned int)(OmrPlc[ NumeroPlc ].SA2));
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "DNA=%d",          (unsigned int)(OmrPlc[ NumeroPlc ].DNA));
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "DA1=%d",          (unsigned int)(OmrPlc[ NumeroPlc ].DA1));
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "DA2=%d",          (unsigned int)(OmrPlc[ NumeroPlc ].DA2));
     Write_Line(ptrfile, stringa);
  }
  fclose(ptrfile);
  //-------------------------------------------------------
  // CONFIGCOM.DAT
  //-------------------------------------------------------
  sprintf(FileName, "%sConfigCom.DAT", PathProject.c_str());
  ptrfile = fopen( FileName, "wb");
  if(ptrfile<=0) return 1;
  for( NumeroCom=0; NumeroCom<=MAXCOM; NumeroCom++){
     sprintf( stringa, "[Com%d]",        NumeroCom );
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "DescrPlc=%s",     InCom[ NumeroCom ].DescrCom.c_str());
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "Baudrate=%d",     InCom[ NumeroCom ].Baudrate);
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "Dati=%d",         InCom[ NumeroCom ].Dati);
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "Parita=%d",       InCom[ NumeroCom ].Parita);
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "StopBit=%d",      InCom[ NumeroCom ].StopBit);
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "NoCtsRts=%d",     InCom[ NumeroCom ].NoCtsRts);
     Write_Line(ptrfile, stringa);
  }
  fclose(ptrfile);
  return 0;
}
//---------------------------------------------------------------------------
// -----------------------
//  RemLoadIdent()
// -----------------------
//  Lettura del file identificativo del PC IDENT.DAT
//
int RemLoadIdentificativoPC(char *filename, int *Id, char *all_mess)
/*************************************************/
{
    FILE *ptrfile;
    int  err;
    char Ident[50];

    *Id=0;
    strcpy(all_mess,"");
    //---------------------------------------------------
    // Controllo se esiste il file di identificazione PC
    //---------------------------------------------------
    ptrfile = fopen(filename, "rb");
    if(ptrfile==0){
        sprintf( all_mess, "Not Exist: %s", filename);
        fclose(ptrfile);
        return 1;
    }
    //-------------------------------------------------
    // Verifico se l'identificativo è valido
    //-------------------------------------------------
    err=0;
    strcpy(Ident, "");
    err=!Read_Line( ptrfile, Ident, 20);
    fclose(ptrfile);
    *Id= atoi(Ident);
    if( *Id<0 || *Id>9 ) err=1;
    if( err ){
        sprintf( all_mess, "Identificativo PC errato:(%s) valori ammessi: 0,2..", Ident );
    }
    return err;
}
//---------------------------------------------------------------------------
//--------------------------
//  RemSaveTimeZone
//--------------------------
//
int RemSaveTimeZone ( char *all_mess )
// *************************************************
{
  FILE *ptrfile;
  char stringa[101]="";
  char filename[100]="";

  strcpy( all_mess, "" );
  //-------------------------------------------------
  // Scrivi su server identificativo CLIENT
  //-------------------------------------------------
  sprintf(stringa, "%ld", _timezone);
  strcpy(filename, "TIMEZONE.DAT");
  ptrfile = fopen(filename, "wb");
  if(ptrfile==0){
     sprintf( all_mess, "Not open file: %s", filename);
     fclose(ptrfile);
     return 1;
  }
  Write_Line( ptrfile, stringa);
  fclose(ptrfile);

  return 0;
}
//---------------------------------------------------------------------------
//--------------------------
//  RemLoadTimeZone
//--------------------------
//
int RemLoadTimeZone(long int *plant_timezone, char *all_mess)
// *************************************************
{
  FILE *ptrfile;
  char stringa[101]="";
  char filename[100]="";

  *plant_timezone=0;
  //------------------------------------
  // Inizializzo variabili
  //------------------------------------
  strcpy( all_mess, "" );
  //-------------------------------------------------
  // Scrivi su server identificativo CLIENT
  //-------------------------------------------------
  strcpy(filename, "TIMEZONE.DAT");
  ptrfile = fopen(filename, "rb");
  if(ptrfile==0){
     sprintf( all_mess, "Not open file: %s", filename);
     fclose(ptrfile);
     return 1;
  }
  Read_Line( ptrfile, stringa, 30);
  fclose(ptrfile);
  //------------------------------------------------
  // Conversione stringa in Long Int
  //------------------------------------------------
  *plant_timezone = atol(stringa);
  return 0;
}
//---------------------------------------------------------------------------
// -----------------------------
//    RemCronologicoGenerico()
// -----------------------------
// Lascia una traccia di tutte le missioni scatenate
//
int RemCronologicoGenerico(char *FileName, char *messaggio)
/**********************************************************/
{
    FILE *ptrfile;
    char buffer[140],
         buffer1[20],
         buffer2[20];
    int  pt, err;
    long len=0;

    //---------------------------------------
    // Se troppo lungo allora SCROLL
    //---------------------------------------
    pt = open(FileName, 0);
    if(pt >-1){
        len = filelength(pt);
    }
    close(pt);
    if(len > 30000){
       err = scroll_file(FileName, 3, 50, buffer);
       if(err) return 0;
    }
    //-------------------------------------------
    // Se non esiste, crea e inizializza il file
    //-------------------------------------------
    if(!Created(FileName)){
        ptrfile = fopen(FileName, "wb");
        if (ptrfile==0)  return 1;
        Write_Line(ptrfile, "-------------------------------------------------------------------");
        Write_Line(ptrfile, "   DATA    ORA     MESSAGGIO                                       ");
        Write_Line(ptrfile, "-------------------------------------------------------------------");
        fclose(ptrfile);
    }
    //---------------------
    // apre in APPEND
    //---------------------
    ptrfile = fopen(FileName, "ab");
    if (ptrfile==0)  return 1;

    sprintf(buffer, "%8s %8s  %s ",Today(buffer1),
                                   ora(buffer2),
                                   messaggio);
    Write_Line(ptrfile, buffer);
    fclose(ptrfile);
    return 0;
}
//---------------------------------------------------------------------------
// -------------------------
//  RemConfrontaDateFile()
// -------------------------
// Confronta le date tra due file
// Restituisce:
// -1 Se il FileSorg sorgente non esiste
//  0 Se il FileSorg ha data più vecchia della destinazione o FileDest inesistente
//  1 Se il FileDest ha data più vecchia della destinazione
//
int RemConfrontaDateFile( char *FileSorg, char *FileDest )
/*************************************************/
{
    struct ffblk BlkSorg, BlkDest;
    unsigned long TimeSorg, TimeDest;
    int err;

    //-----------------------
    // Recupero Data/time
    //-----------------------
    err = findfirst( FileSorg, &BlkSorg,0);
    if( err<0 ) return err;
    err = findfirst( FileDest, &BlkDest,0);
    if( err<0 ) return 0;

    TimeSorg = (long) BlkSorg.ff_fdate;
    TimeSorg = (long) (TimeSorg<<16);
    TimeSorg = (long) ( TimeSorg | (long) (BlkSorg.ff_ftime));

    TimeDest = (long) BlkDest.ff_fdate;
    TimeDest = (long) (TimeDest<<16);
    TimeDest = (long) ( TimeDest | (long) (BlkDest.ff_ftime));

    if( TimeDest >= TimeSorg ) return 0;
    if( TimeDest  < TimeSorg ) return 1;

    return -2;
}
//---------------------------------------------------------------------------
//--------------------------
//  RemSincronizzaTimeFile
//--------------------------
// Sincronizza il time di un FileSorg -> FileDest
// Restituisce:
//  0 Se il Tutto ok
//  1 Se c'è un errore
//
int RemSincronizzaTimeFile( char *FileSorg, char *FileDest, char *all_mess )
/*************************************************/
{
    struct   std::ftime FtSorg;
    int      handle;

    //-------------------------------------------------------------
    // Apro FileSorg e recupero Data/time
    //-------------------------------------------------------------
    handle = sopen( FileSorg, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
    if( handle<0 ){
        close( handle );
        sprintf(all_mess, MESS[96], FileSorg);
        return 1;
    }
    getftime( handle, &FtSorg );
    close( handle );

    //-------------------------------------------------------------
    // Apro FileDest e imposto Data/time
    //-------------------------------------------------------------
    handle = sopen( FileDest, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
    if( handle<0 ){
        close( handle );
        sprintf(all_mess, MESS[96], FileDest);
        return 1;
    }
    setftime( handle, &FtSorg );
    close( handle );

    return 0;
}
//---------------------------------------------------------------------------
//--------------------------
//  RemCreaFileComando
//--------------------------
// E' una funzione che crea una file comando per il SERVER con i
// seguenti parametri :
//
// File Comando       : Nome del file da creare viene aggiunto
//                    : dalla funzione il percorso e l'estensione,
//                    : DriveServer:\comandi\xxxx.PCx
// PtrStruttura       : Puntatore (char*) alla struttura da copiare nel file
// SizeStruttura      : Dimensione della struttura da copiare
//
// N.B. Se PtrStruttura o SizeStruttura sono a 0 non viene copiato niente
//      e viene creato il file vuoto.
//
// Restituisce:
//  0 Se il Tutto ok
//  1 Se c'è un errore
//
int RemCreaFileComando ( char *FileComando, BYTE *PtrStruttura, int SizeStruttura, char *all_mess )
/*************************************************/
{
  char FileTemp[100];
  char FileDest[100];
  int  handle;
  int  i, err;
  int  len;

  //------------------------------------
  // Inizializzo variabili
  //------------------------------------
  err = 0;
  strcpy( all_mess, "" );
//if( IdPC<2            ) return 0;       // 17/01/2013 Anche il PC SERVER può inviare dei comandi
                                          // a se stesso tramite AlgoDroid (es. Backup).
  if( RemPrimaScansione ) return 0;       // solo se è stato eseguito almeno un ciclo Client/Server


  sprintf( FileTemp, "%sTemp%d"   , PercorsoComandi, IdPC );
  sprintf( FileDest, "%s%s.PC%d"  , PercorsoComandi, FileComando, IdPC );
  //---------------------------------------
  // Se il comando è di tipo LOG OPERATORE
  // viene creato indicizzato.
  // (Possono essercene vari in coda)
  //---------------------------------------
  if(strcmp(FileComando, "LOGOP")==0){
     for(i=0; i<50; i++){
        sprintf(FileDest, "%s%s%d.PC%d"  , PercorsoComandi, FileComando, i, IdPC );
        if(access(FileDest, 0)!=0) break;
     }
     if(i==50) return 0;
  }
  //------------------------------------
  // Creo un file temporaneo
  //------------------------------------
  handle = _rtl_creat(FileTemp, 0);
  if( handle<0 ){
     close( handle );
     sprintf(all_mess, MESS[96], FileTemp);
     return 1;
  }
  //------------------------------------
  // Copio i dati nel file temporaneo
  //------------------------------------
  if( PtrStruttura!=NULL && SizeStruttura!=NULL ){
     len= write( handle, PtrStruttura, SizeStruttura );
     if (len != SizeStruttura ){
        err=1;
        sprintf(all_mess, MESS[113], FileTemp);
     }
  }
  close(handle);
  //-------------------------------------
  // Rinomino il file temp-> FileComando
  //-------------------------------------
  if( err==0 ) rename( FileTemp, FileDest );
  return err;
}
//---------------------------------------------------------------------------
//-------------------------------
//  RemReadIniProject()
//-------------------------------
// Rilegge da Remoto le variabili modificate
// nell'INIPROJECT...ad esclusione del
// PATH SERVER
//
//       DriveServer\IniProject
//
// *************************************************
void RemReadIniProject()
{
  TIniFile   *IniFile;
  AnsiString FBuff;
  AnsiString FIniFileName;

  FBuff        = DriveServer;
  FIniFileName = FBuff + "\\IniProject.ini";
  IniFile      = new TIniFile(FIniFileName);
  //-------------------------------------------------------
  // Recupero della lingua di sistema
  //-------------------------------------------------------
  FBuff = IniFile->ReadString("Messaggeria", "Lingua", LinguaSistema);
  strcpy(LinguaSistema, FBuff.c_str());
  //-----------------------------------------------------
  // Gestione Password opertatore
  //-------------------------------------------------------
  AttivaPassword3Veloce = IniFile->ReadBool("Password", "AttivaPassword3Veloce", AttivaPassword3Veloce);
  Pass2[ 1] = IniFile->ReadString("Password", "Password2_01", Pass2[ 1]);
  Pass2[ 2] = IniFile->ReadString("Password", "Password2_02", Pass2[ 2]);
  Pass2[ 3] = IniFile->ReadString("Password", "Password2_03", Pass2[ 3]);
  Pass2[ 4] = IniFile->ReadString("Password", "Password2_04", Pass2[ 4]);
  Pass2[ 5] = IniFile->ReadString("Password", "Password2_05", Pass2[ 5]);
  Pass2[ 6] = IniFile->ReadString("Password", "Password2_06", Pass2[ 6]);
  Pass2[ 7] = IniFile->ReadString("Password", "Password2_07", Pass2[ 7]);
  Pass2[ 8] = IniFile->ReadString("Password", "Password2_08", Pass2[ 8]);
  Pass2[ 9] = IniFile->ReadString("Password", "Password2_09", Pass2[ 9]);
  Pass2[10] = IniFile->ReadString("Password", "Password2_10", Pass2[10]);
  Pass3[ 1] = IniFile->ReadString("Password", "Password3_01", Pass3[ 1]);
  Pass3[ 2] = IniFile->ReadString("Password", "Password3_02", Pass3[ 2]);
  Pass3[ 3] = IniFile->ReadString("Password", "Password3_03", Pass3[ 3]);
  Pass3[ 4] = IniFile->ReadString("Password", "Password3_04", Pass3[ 4]);
  Pass3[ 5] = IniFile->ReadString("Password", "Password3_05", Pass3[ 5]);
  Pass3[ 6] = IniFile->ReadString("Password", "Password3_06", Pass3[ 6]);
  Pass3[ 7] = IniFile->ReadString("Password", "Password3_07", Pass3[ 7]);
  Pass3[ 8] = IniFile->ReadString("Password", "Password3_08", Pass3[ 8]);
  Pass3[ 9] = IniFile->ReadString("Password", "Password3_09", Pass3[ 9]);
  Pass3[10] = IniFile->ReadString("Password", "Password3_10", Pass3[10]);
  //-------------------------------------------------------
  // Gestione Percorsi
  //-------------------------------------------------------
  MaxMetriOccupabili       = (short)(IniFile->ReadInteger("Percorsi", "MaxMetriOccupabili",      30));
  //-------------------------------------------------------
  // Gestione Impostazioni varie del sistema
  //-------------------------------------------------------
  DisattivaDelaySpeciale        = IniFile->ReadBool("Sistema", "DisattivaDelaySpeciale",              DisattivaDelaySpeciale);
  ManualeConNodiOccupati        = IniFile->ReadBool("Sistema", "ManualeConNodiOccupati",              ManualeConNodiOccupati);
  AbilitaRotazioneSuiRettilinei = IniFile->ReadBool("Sistema", "AbilitaRotazioneSuiRettilinei",       AbilitaRotazioneSuiRettilinei);
  AbilitaPuntoIntermedioSuRotaz = IniFile->ReadBool("Sistema", "AbilitaPuntoIntermedioSuRotaz",       AbilitaPuntoIntermedioSuRotaz);
  AbilitaLetturaMultiPLC        = IniFile->ReadBool("Sistema", "AbilitaLetturaMultiPLC",              true);
  CanaleCronoAllarmiTGV         = (short) IniFile->ReadInteger("Sistema", "CanaleCronoAllarmiTGV",    CanaleCronoAllarmiTGV);
  TempoPerMissioneRicarica      = (short) IniFile->ReadInteger("Sistema", "TempoPerMissioneRicarica",  1);
  GrdMinPerAttivaDeviazioni     = (short) IniFile->ReadInteger("Sistema", "GrdMinPerAttivaDeviazioni", 5);
  //-------------------------------------------------------
  // Comunicazioni
  //-------------------------------------------------------
  break_Com[COM1] = true; // sempre TRUE per i CLIENT
  break_Com[COM2] = true; // sempre TRUE per i CLIENT
//break_Com[COM3] = true; // la connessione con il SQL anche dai CLIENTs
  break_Com[COM4] = true; // sempre TRUE per i CLIENT
  break_Com[COM5] = true; // sempre TRUE per i CLIENT
  break_Com[COM6] = true; // sempre TRUE per i CLIENT
  break_Com[COM7] = true; // sempre TRUE per i CLIENT
  break_Com[COM8] = true; // sempre TRUE per i CLIENT
  break_Com[COM9] = true; // sempre TRUE per i CLIENT
  MinErrConsecutiviTGV   = (short) IniFile->ReadInteger("Comunicazioni", "MinErrConsecutiviTGV",   10);
  //-------------------------------------------------------
  // Parametrizzazioni BDASE + TABELLE
  //-------------------------------------------------------
  NomeSRVSQL             = IniFile->ReadString("DBASE", "NomeSRVSQL", NomeSRVSQL);
  NomeDBase              = IniFile->ReadString("DBASE", "NomeDBase",  NomeDBase);
  NomeTabA               = IniFile->ReadString("DBASE", "NomeTabA",   NomeTabA);
  DBaseUser              = IniFile->ReadString("DBASE", "DBaseUser",  DBaseUser);
  DBasePsw               = IniFile->ReadString("DBASE", "DBasePsw",   DBasePsw);
  MinRiconnessioneSQL    = (short) IniFile->ReadInteger("DBASE", "MinRiconnessioneSQL",     5);
  SecConnectioTimeOutSQL = (short) IniFile->ReadInteger("DBASE", "SecConnectioTimeOutSQL",  15);
  SecComandoTimeOutSQL   = (short) IniFile->ReadInteger("DBASE", "SecComandoTimeOutSQL",    30);
  SecDelayOnErrSQL       = (short) IniFile->ReadInteger("DBASE", "SecDelayOnErrSQL",        120);
                                                                                                     // 0 - nessuno
                                                                                                     // 1 - corridoio porte 1 e 2
  delete IniFile;

}
//---------------------------------------------------------------------------
//-------------------------------
//  RemLeggiComandoModificaMac()
//-------------------------------
// E' una funzione che Legge il file di comando proveniente da CLIENT
// di richiesta modifica macchina:
//
//       DriveServer:\comandi\MACxxx.PCx
//
// Legge il record contenuto nel file lo aggiorna e salva sul file
// MAGAZIN.DAT
//
// Restituisce:
//  0 Se il Tutto ok
//  1 Se c'Š un errore
//
int RemLeggiComandoModificaMac( char *all_mess )
// *************************************************
{
  char     Percorso[100];
  char     NomeComando[100];
  char     buffer[200];
  int      handle;
  int      err;
  int      len;
  int      Trovato;
  struct   ffblk ffblk;
  char     NomeFile[100];
  char     StringRecord[100];
  int      NumRecord;
  struct   CHIAMATE ChLetto;

  //------------------------------------
  // Inizializzo variabili
  //------------------------------------
  err = 0;
  strcpy( all_mess, "" );
  strcpy( NomeComando, "Mac" );
  if( IdPC!=1 ) return 0;

  sprintf( Percorso,"%s%s*.*", PercorsoComandi, NomeComando );
  Trovato = !findfirst( Percorso, &ffblk, 0);
  if( Trovato==0 ) return 0;
  //------------------------------------
  // Recupera il Nome File e Record
  //------------------------------------
  strcpy( NomeFile, ffblk.ff_name );
  Copy  ( NomeFile, 4, 3, StringRecord );
  StringRecord[3]=0;
  NumRecord = atoi( StringRecord );
  if( NumRecord<1 || NumRecord>MAXCHIAMATE ){
     sprintf( all_mess, "Valore record errato" );
     err=1;
  }
  sprintf( Percorso,"%s%s", PercorsoComandi, NomeFile );
  //------------------------------------
  // Apro il File
  //------------------------------------
  if( err==0 ){
     handle = sopen( Percorso, O_RDONLY | O_BINARY, SH_DENYNO );
     if( handle<0 ){
        close( handle );
        sprintf(all_mess, MESS[96], NomeFile);
        return 1;
     }
     //------------------------------
     // Controllo lunghezza file
     //------------------------------
     len=0;
     if(handle >-1) len = filelength(handle);
     if( len !=  sizeof( CHIAMATE ) ){
        sprintf( all_mess, "Dimensioni record non corrette %d", len);
        err = 1;
     }
     if( err==0 ){
        //------------------------------------
        // Leggo il Record
        //------------------------------------
        len = read( handle, &ChLetto, sizeof( CHIAMATE ) );
        if(len != sizeof( CHIAMATE )){
           err = 1;
           sprintf( all_mess, "Dimensioni record non corrette %d", len);
        }
     }
     if(err==0){
        //-------------------------------------------------------------
        // Modifica struttura globale chiamate
        //-------------------------------------------------------------
        memcpy(P.ch[NumRecord], &ChLetto, sizeof(CHIAMATE));
     }
     //--------------------
     // Chiudo il File
     //--------------------
     close( handle );
  }
  //------------------------------------
  // Lascio traccia su file LOG
  //------------------------------------
  if( err==0 ){
     err = P.save_record_chiamate( NumRecord, all_mess );
     sprintf( buffer, "%12s Modifica Macchina %d eseguita correttamente", NomeFile, NumRecord );
  }
  if( err >0 ) sprintf( buffer, "%12s Errore %s", NomeFile, all_mess );
  RemCronologicoGenerico( PercorsoLog, buffer);
  //------------------------------------
  // Cancello File comando
  //------------------------------------
  unlink( Percorso );
  return err;
}
//---------------------------------------------------------------------------
//-------------------------------
//  RemLeggiComandoModificaBaia()
//-------------------------------
// E' una funzione che Legge il file di comando proveniente da CLIENT
// di richiesta modifica macchina:
//
//       DriveServer:\comandi\BAIxxx.PCx
//
// Legge il record contenuto nel file lo aggiorna e salva sul file
// MAGAZIN.DAT
//
// Restituisce:
//  0 Se il Tutto ok
//  1 Se c'Š un errore
//
int RemLeggiComandoModificaBaia( char *all_mess )
// *************************************************
{
  char     Percorso[100];
  char     NomeComando[100];
  char     buffer[200];
  int      handle;
  int      err;
  int      len;
  int      Trovato;
  struct   ffblk ffblk;
  char     NomeFile[100];
  char     StringRecord[100];
  int      NumRecord;
  struct   BAIE BaLetto;

  //------------------------------------
  // Inizializzo variabili
  //------------------------------------
  err = 0;
  strcpy( all_mess, "" );
  strcpy( NomeComando, "Bai" );
  if( IdPC!=1 ) return 0;

  sprintf( Percorso,"%s%s*.*", PercorsoComandi, NomeComando );
  Trovato = !findfirst( Percorso, &ffblk, 0);
  if( Trovato==0 ) return 0;
  //------------------------------------
  // Recupera il Nome File e Record
  //------------------------------------
  strcpy( NomeFile, ffblk.ff_name );
  Copy  ( NomeFile, 4, 4, StringRecord );
  StringRecord[4]=0;
  NumRecord = atoi( StringRecord );
  if( NumRecord<1 || NumRecord>MAXBAIE ){
     sprintf( all_mess, "Valore record errato" );
     err=1;
  }
  sprintf( Percorso,"%s%s", PercorsoComandi, NomeFile );
  //------------------------------------
  // Apro il File
  //------------------------------------
  if( err==0 ){
     handle = sopen( Percorso, O_RDONLY | O_BINARY, SH_DENYNO );
     if( handle<0 ){
        close( handle );
        sprintf(all_mess, MESS[96], NomeFile);
        return 1;
     }
     //------------------------------
     // Controllo lunghezza file
     //------------------------------
     len=0;
     if(handle >-1) len = filelength(handle);
     if( len !=  sizeof( BAIE ) ){
        sprintf( all_mess, "Dimensioni record non corrette %d", len);
        err = 1;
     }
     if( err==0 ){
        //------------------------------------
        // Leggo il Record
        //------------------------------------
        len = read( handle, &BaLetto, sizeof( BAIE ) );
        if(len != sizeof( BAIE )){
           err = 1;
           sprintf( all_mess, "Dimensioni record non corrette %d", len);
        }
     }
     if(err==0){
        //-------------------------------------------------------------
        // Modifica struttura globale chiamate
        //-------------------------------------------------------------
        memcpy(P.ba[NumRecord], &BaLetto, sizeof( BAIE ));
     }
     //--------------------
     // Chiudo il File
     //--------------------
     close( handle );
  }
  //------------------------------------
  // Lascio traccia su file LOG
  //------------------------------------
  if( err==0 ){
     err = P.save_record_baie( NumRecord, all_mess );
     sprintf( buffer, "%12s Modifica Baia %d eseguita correttamente", NomeFile, NumRecord );
  }
  if( err >0 ) sprintf( buffer, "%12s Errore %s", NomeFile, all_mess );
  RemCronologicoGenerico( PercorsoLog, buffer);
  //------------------------------------
  // Cancello File comando
  //------------------------------------
  unlink( Percorso );
  return err;
}
//---------------------------------------------------------------------------
//-------------------------------
//  RemLeggiComandoModificaMag()
//-------------------------------
// E' una funzione che Legge il file di comando proveniente da CLIENT
// di richiesta modifica macchina:
//
//       DriveServer:\comandi\MAGxxx.PCx
//
// Legge il record contenuto nel file lo aggiorna e salva sul file
// MAGAZIN.DAT
//
// Restituisce:
//  0 Se il Tutto ok
//  1 Se c'Š un errore
//
int RemLeggiComandoModificaMag( char *all_mess )
// *************************************************
{
  char     Percorso[100];
  char     NomeComando[100];
  char     buffer[200];
  int      handle;
  int      err;
  int      len;
  int      Trovato;
  struct   ffblk ffblk;
  char     NomeFile[100];
  char     StringRecord[100];
  int      NumRecord;
  struct   MAG MgLetto;

  //------------------------------------
  // Inizializzo variabili
  //------------------------------------
  err = 0;
  strcpy( all_mess, "" );
  strcpy( NomeComando, "Mag" );
  if( IdPC!=1 ) return 0;

  sprintf( Percorso,"%s%s*.*", PercorsoComandi, NomeComando );
  Trovato = !findfirst( Percorso, &ffblk, 0);
  if( Trovato==0 ) return 0;
  //------------------------------------
  // Recupera il Nome File e Record
  //------------------------------------
  strcpy( NomeFile, ffblk.ff_name );
  Copy  ( NomeFile, 4, 3, StringRecord );
  StringRecord[3]=0;
  NumRecord = atoi( StringRecord );
  if( NumRecord<1 || NumRecord>MAXMAG ){
     sprintf( all_mess, "Valore record errato" );
     err=1;
  }
  sprintf( Percorso,"%s%s", PercorsoComandi, NomeFile );
  //------------------------------------
  // Apro il File
  //------------------------------------
  if( err==0 ){
     handle = sopen( Percorso, O_RDONLY | O_BINARY, SH_DENYNO );
     if( handle<0 ){
        close( handle );
        sprintf(all_mess, MESS[96], NomeFile);
        return 1;
     }
     //------------------------------
     // Controllo lunghezza file
     //------------------------------
     len=0;
     if(handle >-1) len = filelength(handle);
     if( len !=  sizeof( MAG ) ){
        sprintf( all_mess, "Dimensioni record non corrette %d", len);
        err = 1;
     }
     if( err==0 ){
        //------------------------------------
        // Leggo il Record
        //------------------------------------
        len = read( handle, &MgLetto, sizeof( MAG ) );
        if(len != sizeof( MAG )){
           err = 1;
           sprintf( all_mess, "Dimensioni record non corrette %d", len);
        }
     }
     if(err==0){
        //-------------------------------------------------------------
        // Modifica struttura globale chiamate
        //-------------------------------------------------------------
        memcpy(M.m[NumRecord], &MgLetto, sizeof(MAG));
     }
     //--------------------
     // Chiudo il File
     //--------------------
     close( handle );
  }
  //------------------------------------
  // Lascio traccia su file LOG
  //------------------------------------
  if( err==0 ){
     err = M.save_record_mag( NumRecord, all_mess );
     sprintf( buffer, "%12s Modifica Mag %d eseguita correttamente", NomeFile, NumRecord );
  }
  if( err >0 ) sprintf( buffer, "%12s Errore %s", NomeFile, all_mess );
  RemCronologicoGenerico( PercorsoLog, buffer);
  //------------------------------------
  // Cancello File comando
  //------------------------------------
  unlink( Percorso );
  return err;
}
//---------------------------------------------------------------------------
//--------------------------------
//  RemLeggiComandoModificaNodi()
//--------------------------------
// E' una funzione che Legge il file di comando proveniente da CLIENT
// di richiesta modifica NODI:
//
//       DriveServer:\comandi\NODxxx.PCx
//
// Legge il record contenuto nel file lo aggiorna e salva sul file
// NODO.DAT
//
// Restituisce:
//  0 Se il Tutto ok
//  1 Se c'è un errore
//
int RemLeggiComandoModificaNodi( char *all_mess )
// *************************************************
{
    char     Percorso[100];
    char     NomeComando[100];
    char     buffer[200];
    int      handle;
    int      err;
    int      len;
    int      Trovato;
    struct   ffblk ffblk;
    char     NomeFile[100];
    char     StringRecord[100];
    int      NumRecord;
    struct   NOD NodLetto;

    //------------------------------------
    // Inizializzo variabili
    //------------------------------------
    err = 0;
    strcpy( all_mess, "" );
    strcpy( NomeComando, "NOD" );
    if( IdPC!=1 ) return 0;

    sprintf( Percorso,"%s%s*.*", PercorsoComandi, NomeComando );
    Trovato = !findfirst( Percorso, &ffblk, 0);
    if( Trovato==0 ) return 0;

    //------------------------------------
    // Recupera il Nome File e Record
    //------------------------------------
    strcpy( NomeFile, ffblk.ff_name );
    Copy  ( NomeFile, 4, 4, StringRecord );
    StringRecord[4]=0;
    NumRecord = atoi( StringRecord );
    if( NumRecord<1 || NumRecord>MAXPUNTI ){
        sprintf( all_mess, "Valore record errato" );
        err=1;
    }
    sprintf( Percorso,"%s%s", PercorsoComandi, NomeFile );
    //------------------------------------
    // Apro il File
    //------------------------------------
    if( err==0 ){
        handle = sopen( Percorso, O_RDONLY | O_BINARY, SH_DENYNO );
        if( handle<0 ){
            close( handle );
            sprintf(all_mess, MESS[96], NomeFile);
            return 1;
        }
        //------------------------------
        // Controllo lunghezza file
        //------------------------------
        len=0;
        if(handle >-1) len = filelength(handle);
        if( len !=  sizeof( NOD) ){
           sprintf( all_mess, "Dimensioni record non corrette %d", len);
           err = 1;
        }
        if( err==0 ){
           //------------------------------------
           // Leggo il Record
           //------------------------------------
           len = read( handle, &NodLetto, sizeof( NOD ) );
           if (len != sizeof( NOD )){
              err = 1;
              sprintf( all_mess, "Dimensioni record non corrette %d", len);
           }
        }
        if(err==0){
            memcpy( N.n[ NumRecord ], &NodLetto, sizeof( NOD ) );
        }
        //--------------------
        // Chiudo il File
        //--------------------
        close( handle );
    }
    //------------------------------------
    // Lascio traccia su file LOG
    //------------------------------------
    if( err==0 ){
        err = N.save_record_nodi( NumRecord,all_mess);
        //-----------------------------------------
        //  Compila Array Nodi occupati
        //-----------------------------------------
        N.nodo_busy[ NumRecord ] = N.n[ NumRecord ]->busy;
        if(!err) err = N.save_nodo_busy(all_mess);
        sprintf( buffer, "%12s Modifica Nodo %d eseguita correttamente", NomeFile, NumRecord );
    }
    if( err >0 ) sprintf( buffer, "%12s Errore %s", NomeFile, all_mess );
    RemCronologicoGenerico( PercorsoLog, buffer);
    //------------------------------------
    // Cancello File comando
    //------------------------------------
    unlink( Percorso );

    return err;
}
//---------------------------------------------------------------------------
//------------------------------------
//  RemLeggiComandoModificaMissTGV()
//------------------------------------
// E' una funzione che Legge il file di comando proveniente da CLIENT
// di richiesta modifica Missione (Dati generali incl, escluso, dati box ecc.)
// del TGV:
//
//       DriveServer:\comandi\MissTgvxx.PCx
//
// Legge il record contenuto nel file lo aggiorna e salva sul file
// MISSIONxx.DAT
//
// Restituisce:
//  0 Se il Tutto ok
//  1 Se c'Š un errore
//
int RemLeggiComandoModificaMissTGV( char *all_mess )
// *************************************************
{
    char     Percorso[100];
    char     NomeComando[100];
    char     buffer[200];
    int      handle;
    int      err;
    int      len;
    int      Trovato;
    struct   ffblk ffblk;
    char     NomeFile[100];
    char     StringRecord[100];
    int      NumTgv;
    struct   MISSION MissionBuff;
    struct   MISSION MissionLetto;

    //------------------------------------
    // Inizializzo variabili
    //------------------------------------
    err = 0;
    strcpy( all_mess, "" );
    strcpy( NomeComando, "MissTgv" );
    memset(&StringRecord[0], NULL, sizeof(StringRecord));

    if( IdPC!=1 ) return 0;

    sprintf( Percorso,"%s%s*.*", PercorsoComandi, NomeComando );
    Trovato = !findfirst( Percorso, &ffblk, 0);
    if( Trovato==0 ) return 0;

    //------------------------------------
    // Recupera il Nome File e Record
    //------------------------------------
    strcpy( NomeFile, ffblk.ff_name );
//    Copy  ( NomeFile, 8, 2, StringRecord );
    Copy  ( NomeFile, strlen(NomeComando)+1, 2, StringRecord );
    StringRecord[2]=0;
    NumTgv = atoi( StringRecord );
    if( NumTgv<1 || NumTgv>MAXAGV ){
        sprintf( all_mess, "Valore record errato" );
        err=1;
    }
    sprintf( Percorso,"%s%s", PercorsoComandi, NomeFile );
    //------------------------------------
    // Apro il File
    //------------------------------------
    if( err==0 ){
        handle = sopen( Percorso, O_RDONLY | O_BINARY, SH_DENYNO );
        if( handle<0 ){
            close( handle );
            sprintf(all_mess, "Errore di accesso al file <%s>", NomeFile);
            return 1;
        }
        //------------------------------
        // Controllo lunghezza file
        //------------------------------
        len=0;
        if(handle >-1) len = filelength(handle);
        if( len !=  sizeof( MISSION ) ){
           sprintf( all_mess, "Dimensioni record non corrette %d", len);
           err = 1;
        }
        if( err==0 ){
           //------------------------------------
           // Leggo il Record
           //------------------------------------
           len = read( handle, &MissionLetto, sizeof( MISSION ) );
           if (len != sizeof( MISSION )){
              err = 1;
              sprintf( all_mess, "Dimensioni record non corrette %d", len);
           }
        }
        if(err==0){
           //-------------------------------------------
           // Memorizzo il vecchio stato della missione
           //-------------------------------------------
           memcpy(&MissionBuff, &AGV[ NumTgv ]->mission, sizeof( MISSION ) );
           //----------------------------------
           // Aggiorno intera struttura
           //----------------------------------
           memcpy( &AGV[ NumTgv ]->mission, &MissionLetto, sizeof( MISSION ) );
           //----------------------------------
           // SE IL SEGNALE DI STOP ERA BASSO
           // LO RIABBASSO PER EVITARE STAGNATE
           //----------------------------------
           if(MissionBuff.StopManuale==false){
              AGV[ NumTgv ]->mission.StopManuale = MissionBuff.StopManuale;
           }
           //----------------------------------
           // 16/12/2010
           // i dati della missione devono
           // rimanere quelli originali del
           // Server
           //----------------------------------
           memcpy(&AGV[ NumTgv ]->mission.step,            &MissionBuff.step,            sizeof(AGV[ NumTgv ]->mission.step));
           memcpy(&AGV[ NumTgv ]->mission.punto,           &MissionBuff.punto,           sizeof(AGV[ NumTgv ]->mission.punto));
           memcpy(&AGV[ NumTgv ]->mission.dati_perc,       &MissionBuff.dati_perc,       sizeof(AGV[ NumTgv ]->mission.dati_perc));
           memcpy(&AGV[ NumTgv ]->mission.punto_prenotato, &MissionBuff.punto_prenotato, sizeof(AGV[ NumTgv ]->mission.punto_prenotato));
           AGV[ NumTgv ]->mission.Typ                      = MissionBuff.Typ;
           AGV[ NumTgv ]->mission.pstart                   = MissionBuff.pstart;
           AGV[ NumTgv ]->mission.pend                     = MissionBuff.pend;
           AGV[ NumTgv ]->mission.pintstart                = MissionBuff.pintstart;
           AGV[ NumTgv ]->mission.pintend                  = MissionBuff.pintend;
           AGV[ NumTgv ]->mission.PIntForzato              = MissionBuff.PIntForzato;
           AGV[ NumTgv ]->mission.pintendtrasmesso         = MissionBuff.pintendtrasmesso;
           AGV[ NumTgv ]->mission.TimeInitAttesa           = MissionBuff.TimeInitAttesa;
           AGV[ NumTgv ]->mission.TimeFineMission          = MissionBuff.TimeFineMission;
           AGV[ NumTgv ]->mission.Chiamata                 = MissionBuff.Chiamata;
           AGV[ NumTgv ]->mission.NodIngMag                = MissionBuff.NodIngMag;
           AGV[ NumTgv ]->mission.NumMissioni              = MissionBuff.NumMissioni;
           //----------------------------------
           // SE IL TGV HA LA MISSIONE IN CORSO
           //----------------------------------
           if(AGV[ NumTgv ]->stato.start!=0){
              //
              // dati da non aggiornare con missione in corso
              //
           }
        }
        //--------------------
        // Chiudo il File
        //--------------------
        close( handle );
    }
    //------------------------------------
    // Lascio traccia su file LOG
    //------------------------------------
    if( err==0 ){
        err = AGV[ NumTgv ]->save_mission( all_mess );
        sprintf( buffer, "%12s Modifica TGV %d eseguita correttamente", NomeFile, NumTgv );
    }
    if( err >0 ) sprintf( buffer, "%12s Errore %s", NomeFile, all_mess );
    RemCronologicoGenerico( PercorsoLog, buffer);
    //------------------------------------
    // Cancello File comando
    //------------------------------------
    unlink( Percorso );

    return err;
}
//---------------------------------------------------------------------------
//------------------------------------
//  RemLeggiComandoModificaPosTGV()
//------------------------------------
// E' una funzione che Legge il file di comando proveniente da CLIENT
// di richiesta modifica Missione (Dati generali incl, escluso, dati box ecc.)
// del TGV:
//
//       DriveServer:\comandi\PosTgvxx.PCx
//
// Legge la stringa contenuta nel file e salva la missione
//
// Restituisce:
//  0 Se il Tutto ok
//  1 Se c'Š un errore
//
int RemLeggiComandoModificaPosTGV( char *all_mess )
// *************************************************
{
    FILE    *ptrfile;
    char     Percorso[100];
    char     NomeComando[100];
    char     buffer[200];
    int      err;
    int      Trovato;
    struct   ffblk ffblk;
    char     NomeFile[100];
    char     StringRecord[100];
    int      NumTgv;
    int      NuovaPos;

    //------------------------------------
    // Inizializzo variabili
    //------------------------------------
    err = 0;
    strcpy( all_mess, "" );
    strcpy( NomeComando, "PosTgv" );
    if( IdPC!=1 ) return 0;

    sprintf( Percorso,"%s%s*.*", PercorsoComandi, NomeComando );
    Trovato = !findfirst( Percorso, &ffblk, 0);
    if( Trovato==0 ) return 0;

    //------------------------------------
    // Recupera il Nome File e Record
    //------------------------------------
    strcpy( NomeFile, ffblk.ff_name );
//    Copy  ( NomeFile, 7, 2, StringRecord );
    Copy  ( NomeFile, strlen(NomeComando)+1, 2, StringRecord );

    StringRecord[2]=0;
    NumTgv = atoi( StringRecord );
    if( NumTgv<1 || NumTgv>MAXAGV ){
        sprintf( all_mess, "Valore record errato" );
        err=1;
    }
    sprintf( Percorso,"%s%s", PercorsoComandi, NomeFile );
    //------------------------------------
    // Apro il File
    //------------------------------------
    if( err==0 ){
    strcpy(all_mess,"");
        //---------------------------------------------------
        // Controllo se esiste il file di identificazione PC
        //---------------------------------------------------
        ptrfile = fopen( Percorso, "rb");
        if(ptrfile==0){
           fclose(ptrfile);
           sprintf(all_mess, MESS[96], NomeFile);
           return 1;
        }
        //-------------------------------------------------
        // Verifico se i dati sono validi
        //-------------------------------------------------
        strcpy( StringRecord, "");
        err=!Read_Line( ptrfile, StringRecord, 30);
        StringRecord[30]=0;
        fclose(ptrfile);
        NuovaPos = atoi( StringRecord );
        if( NuovaPos<0 || NuovaPos> MAXPUNTI ){
           err=1;
           sprintf( all_mess, "Nuova posizione Non corretta: (%d)", NuovaPos );
        }
        if(err==0){
           AGV[ NumTgv ]->stato.pos = (short int ) NuovaPos;
           err = AGV[NumTgv]->trasmetti_new_pos_AGV(AGV[NumTgv]->stato.pos, buffer);
        }
    }
    //------------------------------------
    // Lascio traccia su file LOG
    //------------------------------------
    if( err==0 ){
        err = AGV[ NumTgv ]->save_mission( all_mess );
        sprintf( buffer, "%12s Modifica Posizione TGV %d Ok newpos%d", NomeFile, NumTgv, NuovaPos );
    }
    if( err >0 ) sprintf( buffer, "%12s Errore %s", NomeFile, all_mess );
    RemCronologicoGenerico( PercorsoLog, buffer);
    //------------------------------------
    // Cancello File comando
    //------------------------------------
    unlink( Percorso );

    return err;
}
//---------------------------------------------------------------------------
//------------------------------------
//  RemLeggiComandoModificaStopTGV()
//------------------------------------
// E' una funzione che Legge il file di comando proveniente da CLIENT
// di richiesta modifica Missione (Dati generali incl, escluso, dati box ecc.)
// del TGV:
//
//       DriveServer:\comandi\StopMxx.PCx
//
// Legge la stringa contenuta nel file e salva la missione
//
// Restituisce:
//  0 Se il Tutto ok
//  1 Se c'Š un errore
//
int RemLeggiComandoModificaStopTGV( char *all_mess )
// *************************************************
{
    char     Percorso[100];
    char     NomeComando[100];
    char     buffer[200];
    int      handle;
    int      err;
    int      len;
    int      Trovato;
    bool     StopManuale;
    struct   ffblk ffblk;
    char     NomeFile[100];
    char     StringRecord[100];
    int      NumTgv;

    //------------------------------------
    // Inizializzo variabili
    //------------------------------------
    err = 0;
    strcpy( all_mess, "" );
    strcpy( NomeComando, "StopM" );
    memset(&StringRecord[0], NULL, sizeof(StringRecord));

    if( IdPC!=1 ) return 0;

    sprintf( Percorso,"%s%s*.*", PercorsoComandi, NomeComando );
    Trovato = !findfirst( Percorso, &ffblk, 0);
    if( Trovato==0 ) return 0;

    //------------------------------------
    // Recupera il Nome File e Record
    //------------------------------------
    strcpy( NomeFile, ffblk.ff_name );
//    Copy  ( NomeFile, 8, 2, StringRecord );
    Copy  ( NomeFile, strlen(NomeComando)+1, 2, StringRecord );
    StringRecord[2]=0;
    NumTgv = atoi( StringRecord );
    if( NumTgv<1 || NumTgv>MAXAGV ){
        sprintf( all_mess, "Valore record errato" );
        err=1;
    }
    sprintf( Percorso,"%s%s", PercorsoComandi, NomeFile );
    //------------------------------------
    // Apro il File
    //------------------------------------
    if( err==0 ){
        handle = sopen( Percorso, O_RDONLY | O_BINARY, SH_DENYNO );
        if( handle<0 ){
            close( handle );
            sprintf(all_mess, MESS[96], NomeFile);
            return 1;
        }
        //------------------------------
        // Controllo lunghezza file
        //------------------------------
        len=0;
        if(handle >-1) len = filelength(handle);
        if( len !=  sizeof( AGV[NumTgv]->mission.StopManuale ) ){
           sprintf( all_mess, "Dimensioni record non corrette %d", len);
           err = 1;
        }
        if( err==0 ){
           //------------------------------------
           // Leggo il Record
           //------------------------------------
           len = read( handle, &StopManuale, sizeof( AGV[NumTgv]->mission.StopManuale ) );
           if (len != sizeof( AGV[NumTgv]->mission.StopManuale )){
              err = 1;
              sprintf( all_mess, "Dimensioni record non corrette %d", len);
           }
        }
        if(err==0){
           //----------------------------------
           // Aggiorno STATO IN STRUTTURA
           // solo se il TGV non ha già la
           // missione fino a destinazione
           //----------------------------------
           if(AGV[NumTgv]->mission.pintend!=AGV[NumTgv]->mission.pend && AGV[NumTgv]->stato.pos!=AGV[NumTgv]->mission.pend){
              AGV[ NumTgv ]->mission.StopManuale = StopManuale;
           }
        }
        //--------------------
        // Chiudo il File
        //--------------------
        close( handle );
    }
    //------------------------------------
    // Lascio traccia su file LOG
    //------------------------------------
    if( err==0 ){
        err = AGV[ NumTgv ]->save_mission( all_mess );
        sprintf( buffer, "%12s Modifica StopManuale TGV %d eseguita correttamente", NomeFile, NumTgv );
    }
    if( err >0 ) sprintf( buffer, "%12s Errore %s", NomeFile, all_mess );
    RemCronologicoGenerico( PercorsoLog, buffer);
    //------------------------------------
    // Cancello File comando
    //------------------------------------
    unlink( Percorso );

    return err;
}
//---------------------------------------------------------------------------
//-----------------------------------------
//  RemLeggiComandoSemiautomatico()
//-----------------------------------------
// E' una funzione che Legge il file di comando proveniente da CLIENT
// di richiesta Semiatomatico per Carico/scarico da macchina:
//
//       DriveServer:\comandi\Semiautomatico.PCx
//
// Legge la stringa contenuta nel file e lancia la missione
//
// Restituisce:
//  0 Se il Tutto ok
//  1 Se c'Š un errore
//
int RemLeggiComandoSemiautomatico(char *all_mess)
// *************************************************
{
    char     Percorso[100];
    char     NomeComando[100];
    char     buffer[200];
    int      handle;
    int      err;
    int      len;
    int      Trovato;
    struct   ffblk ffblk;
    char     NomeFile[100];
    struct   SEMIAUTOMATICO Dati[11];


    //------------------------------------
    // Inizializzo variabili
    //------------------------------------
    err = 0;
    strcpy( all_mess, "" );
    memset(&Dati[0], 0, sizeof(Dati));
    strcpy( NomeComando, "Semiautomatico" );
    if( IdPC!=1 ) return 0;

    sprintf(Percorso,"%s%s*.*", PercorsoComandi, NomeComando);
    Trovato = !findfirst(Percorso, &ffblk, 0);
    if( Trovato==0 ) return 0;

    //------------------------------------
    // Recupera il Nome File e Record
    //------------------------------------
    strcpy( NomeFile, ffblk.ff_name );
    sprintf( Percorso,"%s%s", PercorsoComandi, NomeFile );
    //------------------------------------
    // Apro il File
    //------------------------------------
    if( err==0 ){
        handle = sopen( Percorso, O_RDONLY | O_BINARY, SH_DENYNO );
        if( handle<0 ){
            close( handle );
            sprintf(all_mess, MESS[96], NomeFile);
            return 1;
        }
        //------------------------------
        // Controllo lunghezza file
        //------------------------------
        len=0;
        if(handle >-1) len = filelength(handle);
        if(len !=  sizeof(SEMIAUTOMATICO)*11){
           sprintf( all_mess, "Dimensioni record non corrette %d", len);
           err = 1;
        }
        if( err==0 ){
           //------------------------------------
           // Leggo il Record
           //------------------------------------
           len = read(handle, &Dati, sizeof(Dati));
           if(len != sizeof(SEMIAUTOMATICO)*11){
              err = 1;
              sprintf( all_mess, "Dimensioni record non corrette %d", len);
           }
        }
        //------------------------------------------------
        // Copia del record letto nella struttura globale
        //------------------------------------------------
        if(err==0){
           memcpy(&P.SemiAuto, &Dati, sizeof(Dati));
           sprintf( buffer, "%12s Struttura SemiAutomatico modificata correttamente", NomeFile);
        }
        //--------------------
        // Chiudo il File
        //--------------------
        close( handle );
    }
    //------------------------------------
    // Lascio traccia su file LOG
    //------------------------------------
    if( err >0 ) sprintf( buffer, "%12s Errore %s", NomeFile, all_mess );
    RemCronologicoGenerico( PercorsoLog, buffer);
    //------------------------------------
    // Cancello File comando
    //------------------------------------
    unlink( Percorso );
    return err;
}
//---------------------------------------------------------------------------
//-----------------------------------------
//  RemLeggiComandoResetMissioneTGV()
//-----------------------------------------
// E' una funzione che Legge il file di comando proveniente da CLIENT
// di richiesta RESET missione TGV:
//
//       DriveServer:\comandi\ResetMissTgvxx.PCx
//
// Cancella i dati in memoria e trasmette il reset al TGV
//
// Restituisce:
//  0 Se il Tutto ok
//  1 Se c'Š un errore
//
int RemLeggiComandoResetMissioneTGV( char *all_mess )
// *************************************************
{
  bool     ResetMissione;
  char     Percorso[100];
  char     NomeComando[100];
  char     buffer[200];
  int      err;
  int      Trovato;
  struct   ffblk ffblk;
  char     NomeFile[100];
  char     StringRecord[100];
  int      NumTgv;

  //------------------------------------
  // Inizializzo variabili
  //------------------------------------
  err = 0;
  strcpy( all_mess, "" );
  strcpy( NomeComando, "ResetMissTgv" );
  if( IdPC!=1 ) return 0;

  sprintf( Percorso,"%s%s*.*", PercorsoComandi, NomeComando );
  Trovato = !findfirst( Percorso, &ffblk, 0);
  if( Trovato==0 ) return 0;

  //------------------------------------
  // Recupera il Nome File e Record
  //------------------------------------
  strcpy( NomeFile, ffblk.ff_name );
  Copy  ( NomeFile, strlen(NomeComando)+1, 2, StringRecord );
  StringRecord[2]=0;
  NumTgv = atoi( StringRecord );
  if( NumTgv<1 || NumTgv>MAXAGV ){
     sprintf( all_mess, "Valore record errato" );
     err=1;
  }
  sprintf( Percorso,"%s%s", PercorsoComandi, NomeFile );
  //------------------------------------
  // Resetto MISSIONE
  //------------------------------------
  if( err==0 ){
     ResetMissione=true;
     //----------------------------------------------------------
     // Invio alla navette il comando di RESET MISSION (DM98=1)
     //----------------------------------------------------------
     if(ResetMissione==true && AGV[NumTgv]->trasmetti_reset_AGV(buffer)) ResetMissione=false;
     //----------------------------------------------------------
     // SICUREZZA
     // Invio due volte il comando di RESET MISSIONE TGV
     //----------------------------------------------------------
     if(ResetMissione==true && AGV[NumTgv]->trasmetti_reset_AGV(buffer)) ResetMissione=false;
     //----------------------------------------------------------
     // SICUREZZA 2
     // Invio 3 volte il comando di RESET MISSIONE TGV
     //----------------------------------------------------------
     if(ResetMissione==true && AGV[NumTgv]->trasmetti_reset_AGV(buffer)) ResetMissione=false;
     //----------------------------------------------------------
     // Segnalazione errore
     //----------------------------------------------------------
     if(ResetMissione==false){
        sprintf(all_mess, "RESET NOT POSSIBLE: Comunication error with TGV%d!", NumTgv);
        err=1;
     }
     //----------------------------------------------------------
     // Se sono verificate tutte le condizione necessarie per
     // il Reset delle Missioni al volo azzero anche la missione
     // in RAM.
     //----------------------------------------------------------
     if(err==0 && ResetMissione==true){
        // --- Azzeramento dati missione ---
        AGV[NumTgv]->reset_mission(buffer);
      //AGV[NumTgv]->reset_chiamata_AGV();        //AL-23/01 (3) attivato comando apposito per reset chiamata
        AGV[NumTgv]->mission.TGVInCambioMissione=false;
        AGV[NumTgv]->mission.TimeInitAttesa=0;
     }
  }
  //------------------------------------
  // Lascio traccia su file LOG
  //------------------------------------
  if( err==0 ){
     sprintf( buffer, "%12s Reset missione TGV %d eseguita", NomeFile, NumTgv );
  }
  if( err >0 ) sprintf( buffer, "%12s Errore %s", NomeFile, all_mess );
  RemCronologicoGenerico( PercorsoLog, buffer);
  //------------------------------------
  // Cancello File comando
  //------------------------------------
  unlink( Percorso );

  return err;
}
//---------------------------------------------------------------------------
//-----------------------------------------
//  RemLeggiComandoResetChiamataTGV()
//-----------------------------------------
// E' una funzione che Legge il file di comando proveniente da CLIENT
// di richiesta RESET CHIAMATA TGV:
//
//       DriveServer:\comandi\ResetChTgvxx.PCx
//
// Cancella i dati in memoria e trasmette il reset al TGV
//
// Restituisce:
//  0 Se il Tutto ok
//  1 Se c'Š un errore
//
int RemLeggiComandoResetChiamataTGV( char *all_mess )
// *************************************************
{
  bool     ResetChiamata;
  char     Percorso[100];
  char     NomeComando[100];
  char     buffer[200];
  int      err;
  int      Trovato;
  struct   ffblk ffblk;
  char     NomeFile[100];
  char     StringRecord[100];
  int      NumTgv;

  //------------------------------------
  // Inizializzo variabili
  //------------------------------------
  err = 0;
  strcpy( all_mess, "" );
  strcpy( NomeComando, "ResetChTgv" );
  if( IdPC!=1 ) return 0;

  sprintf( Percorso,"%s%s*.*", PercorsoComandi, NomeComando );
  Trovato = !findfirst( Percorso, &ffblk, 0);
  if( Trovato==0 ) return 0;

  //------------------------------------
  // Recupera il Nome File e Record
  //------------------------------------
  strcpy( NomeFile, ffblk.ff_name );
  Copy  ( NomeFile, strlen(NomeComando)+1, 2, StringRecord );
  StringRecord[2]=0;
  NumTgv = atoi( StringRecord );
  if( NumTgv<1 || NumTgv>MAXAGV ){
     sprintf( all_mess, "Valore record errato" );
     err=1;
  }
  sprintf( Percorso,"%s%s", PercorsoComandi, NomeFile );
  //------------------------------------
  // Resetto MISSIONE
  //------------------------------------
  if( err==0 ){
     ResetChiamata=true;
     //----------------------------------------------------------
     // Se sono verificate tutte le condizione necessarie per
     // il Reset delle Missioni al volo azzero anche la missione
     // in RAM.
     //----------------------------------------------------------
     if(ResetChiamata==true){
        // --- Azzeramento dati missione ---
        AGV[NumTgv]->reset_chiamata_AGV();
     }
  }
  //------------------------------------
  // Lascio traccia su file LOG
  //------------------------------------
  if( err==0 ){
     sprintf( buffer, "%12s Reset chiamata TGV %d eseguita", NomeFile, NumTgv );
  }
  if( err >0 ) sprintf( buffer, "%12s Errore %s", NomeFile, all_mess );
  RemCronologicoGenerico( PercorsoLog, buffer);
  //------------------------------------
  // Cancello File comando
  //------------------------------------
  unlink( Percorso );

  return err;
}
//---------------------------------------------------------------------------
//-----------------------------------------
//  RemLeggiComandoLiberaNodi()
//-----------------------------------------
// E' una funzione che Legge il file di comando proveniente da CLIENT
// di Liberazione di Nodi dell'Impianto:
//
//       DriveServer:\comandi\LiberaNodi.PCx
//
// Cancella i dati in memoria e trasmette il reset al TGV
//
// Restituisce:
//  0 Se il Tutto ok
//  1 Se c'è un errore
//
int RemLeggiComandoLiberaNodi( char *all_mess )
// *************************************************
{
    char     Percorso[100];
    char     NomeComando[100];
    char     buffer[200];
    int      err, i;
    int      Trovato;
    struct   ffblk ffblk;
    char     NomeFile[100];

    //------------------------------------
    // Inizializzo variabili
    //------------------------------------
    err = 0;
    strcpy( all_mess, "" );
    strcpy( NomeComando, "LiberaNodi" );
    if( IdPC!=1 ) return 0;

    sprintf( Percorso,"%s%s*.*", PercorsoComandi, NomeComando );
    Trovato = !findfirst( Percorso, &ffblk, 0);
    if( Trovato==0 ) return 0;

    strcpy( NomeFile, ffblk.ff_name );
    sprintf( Percorso,"%s%s", PercorsoComandi, NomeFile );
    //------------------------------------
    // Resetto MISSIONE
    //------------------------------------
    if( err==0 ){
        for(i=1; i<=MAXAGV; i++){
           err = AGV[i]->free_all_nodi(all_mess);
           if(err!=0) break;
        }
    }
    //------------------------------------
    // Lascio traccia su file LOG
    //------------------------------------
    if( err==0 ){
        sprintf( buffer, "%12s Libera nodi esguita", NomeFile);
    }
    if( err >0 ) sprintf( buffer, "%12s Errore %s", NomeFile, all_mess );
    RemCronologicoGenerico( PercorsoLog, buffer);
    //------------------------------------
    // Cancello File comando
    //------------------------------------
    unlink( Percorso );

    return err;
}
//---------------------------------------------------------------------------
//-----------------------------------------
//  RemLeggiComandoLogOperatore()
//-----------------------------------------
// E' una funzione che Legge il file di comando proveniente da CLIENT
// di lettura dei dati on-line dell'Impianto:
//
//       DriveServer:\comandi\LogOp.PCx
//
// Cancella i dati in memoria e trasmette il reset al TGV
//
// Restituisce:
//  0 Se il Tutto ok
//  1 Se c'è un errore
//
int RemLeggiComandoLogOperatore( char *all_mess )
// *************************************************
{
  FILE   *ptrfile;
  char   Percorso[100];
  char   NomeComando[100];
  char   buffer[200];
  int    i, err;
  int    Trovato;
  struct ffblk ffblk;
  char   NomeFile[100];
  char   StringRecord[151];
  int    Ch;

  //------------------------------------
  // Inizializzo variabili
  //------------------------------------
  err=0;
  Ch=NULL;
  strcpy(all_mess, "" );
  strcpy(NomeComando, "LogOp" );
  memset(&StringRecord[0], NULL, sizeof(StringRecord));

  if( IdPC!=1 ) return 0;
  //------------------------------------
  // Recupero il nome del file
  //------------------------------------
  sprintf( Percorso,"%s%s*.*", PercorsoComandi, NomeComando );
  Trovato = !findfirst(Percorso, &ffblk, 0);
  if( Trovato==0 ) return 0;
  //------------------------------------
  // Recupera il Nome File e Record
  //------------------------------------
  strcpy(NomeFile, ffblk.ff_name);
  sprintf(Percorso,"%s%s", PercorsoComandi, NomeFile);
  //------------------------------------
  // Apro il File
  //------------------------------------
  ptrfile = fopen(Percorso, "rb");
  if(ptrfile==0){
     fclose(ptrfile);
     sprintf(all_mess, MESS[96], NomeFile);
     return 1;
  }
  //------------------------------------
  // Lettura di un caratte alla volta
  //------------------------------------
  i=0;
  do{
     Ch = getc(ptrfile);
     StringRecord[i] = (char)(Ch);
     i++;
  }while(i<100 && Ch!=-1);
  fclose(ptrfile);
  //------------------------------------
  // Inserimento nel file di LOG LOCALE
  //------------------------------------
  file_cronologico_generico("LOGOP.DAT", StringRecord, 50000L);
  //------------------------------------
  // Lascio traccia su file LOG
  //------------------------------------
  if( err==0 ){
     sprintf( buffer, "%12s lettura comando LogOp eseguita", NomeFile);
  }
  if( err>0  ) sprintf( buffer, "%12s Errore %s", NomeFile, all_mess );
  RemCronologicoGenerico( PercorsoLog, buffer);
  //------------------------------------
  // Cancello File comando
  //------------------------------------
  unlink( Percorso );
  return err;
}
//---------------------------------------------------------------------------
//-----------------------------------------
//  RemLeggiComandoBackupManuale()
//-----------------------------------------
// E' una funzione che Legge il file di comando proveniente da CLIENT
// di Backup manuale dell'Impianto:
//
//       DriveServer:\comandi\Backup.PCx
//
// compattazione dei file di dato in formato "Bakxxx.zip"
//
// Restituisce:
//  0 Se il Tutto ok
//  1 Se c'è un errore
//
int RemLeggiComandoBackupManuale( char *all_mess )
// *************************************************
{
  char   Percorso[201];
  char   NomeComando[101];
  int    err;
  struct ffblk ffblk;
  char   NomeFile[100];
  WORD   dm[21];
  char   newname[101]="";
  char   messaggio[201]="";
  char   buffer[301]="";
  short int i;

  //------------------------------------
  // Inizializzo variabili
  //------------------------------------
  err=0;
  strcpy( all_mess, "" );
  memset(&dm[0], 0, sizeof(dm));
  //------------------------------------
  // Solo per i Server
  //------------------------------------
  if( IdPC>1 ) return 0;
  //-----------------------------------------
  // Verifico tutti i possibili backups
  //-----------------------------------------
  sprintf( NomeComando, "%sBackup.*", PercorsoComandi);
  if(findfirst( NomeComando, &ffblk, 0)) return 0;
  //-----------------------------------------
  // Recupero nome completo del file
  //-----------------------------------------
  strcpy( NomeFile, ffblk.ff_name );
  sprintf(Percorso, "%s%s", PercorsoComandi, NomeFile);
  //-----------------------------------------
  // File Traccia
  //-----------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(break_Com[COM1]         ) break;
     if(AGV[i]->mission.EsclPLC ) continue;
     sprintf(messaggio, "AGV:%d - POS:%03d - START:%03d - END:%03d - INT:%03d - INT_TRS:%03d - ROT:%d",
                          i,
                          AGV[i]->stato.pos,
                          AGV[i]->mission.pstart,
                          AGV[i]->mission.pend,
                          AGV[i]->mission.pintend,
                          AGV[i]->mission.pintendtrasmesso,
                          AGV[i]->stato.s.bit.aux2);
     FileTraccia(messaggio);
     err=OM.ld_canale(&dm[0], (WORD)(i), "RD", 88, 12, buffer);
     if(err){
        sprintf(messaggio, "AGV:%d - Errore lettura PTR_PRG!", i);
     }
     if(!err) err=OM.ld_canale(&dm[20], (WORD)(i), "RD", 1400, 1, buffer);
     if(err){
        sprintf(messaggio, "AGV:%d - Errore lettura PUNTO INTERMEDIO!", i);
     }
     if(!err) sprintf(messaggio, "AGV:%d - DM88:%04d - DM89:%04d - DM90:%04d - DM91:%04d - DM92:%04d - DM93:%04d - DM94:%04d - DM95:%04d - DM96:%04d - DM97:%04d - DM1400:%04d",
                      i, dm[0], dm[1], dm[2], dm[3], dm[4], dm[5], dm[6], dm[7], dm[8], dm[9], dm[20]);
     FileTraccia(messaggio);
     FileTraccia("========================================================================================================");
     //------------------------------------------------------
     // Lettura del programma passato al PLC del TGV
     //------------------------------------------------------
     err=AGV[i]->CompilaProgrammaGrafico( TRUE );
     sprintf(newname, "PROG%03d.DAT", i);
     rename("PROGTGV.DAT", newname);
  }
  //-------------------------------------------------------------------------
  // Salva i nodi occupati e le missioni in RAM prima del Backup
  //-------------------------------------------------------------------------
  N.save_nodo_grd( buffer );
  N.save_nodo_busy( buffer );
  N.save_nodi_tgv_lanciati_su_rettilineo( buffer );   // GESTIONE PRECEDENZE PER TGV IN RETTILINEO
  for( i=1; i<=MAXAGV; i++){
     AGV[i]->save_mission( buffer );
  }
  //-------------------------------------------------------------------------
  // Compattazione dei file
  //-------------------------------------------------------------------------
  RemSalvaAnomalie( TRUE );
  //-------------------------------------------------------------------------
  // Cancellazione dei file temporanei
  //-------------------------------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     sprintf(newname, "PROG%03d.DAT", i);
     unlink(newname);
  }
  //-----------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------
  if(IdPC>1) FileLogOperatore("(BACKUP ) - Backup Operatore da PC remoto");

  //------------------------------------
  // Lascio traccia su file LOG
  //------------------------------------
  if( err==0 ){
     sprintf( buffer, "%12s Backup nodi eseguita", NomeFile);
  }
  if( err >0 ) sprintf( buffer, "%12s Errore %s", NomeFile, all_mess );
  RemCronologicoGenerico( PercorsoLog, buffer);
  //------------------------------------
  // Cancello File comando
  //------------------------------------
  unlink( Percorso );

  return err;
}
//---------------------------------------------------------------------------
// -----------------------
//  RemSalvaAnomalie()
// -----------------------
//  Funzione che fa un backup dei file *.dat in
//  caso di Anomalia verificata all'interno del
//  sistema (es.: Collisioni o Blicchi)
//
void RemSalvaAnomalie( bool Tutto )
/*************************************************/
{
  short int i;
  char NomeFile[151]="";
  char StringaComando[251]="";

  //-------------------------------------------------
  // Creazione della directory per il Backup dei
  // file *.dat
  //-------------------------------------------------
  for(i=0; i<=900; i++){
     sprintf(NomeFile, "Bak%03d.zip", i);
     if(access(NomeFile, 0)!=0) break;
  }
  if(i>=900) return;
//sprintf(StringaComando, "pkzip25 -add %s -include *.ini *.dat *.dbf *.mdx", NomeFile);
  sprintf(StringaComando, "pkzip25 -add -pass=algo %s -include *.ini *.dat *.dbf *.mdx *.lst *.csv *.txt *.xls *.mdb Messapk.* Messagg.* Allarm*.* -dir -excl=Produzio", NomeFile);
  //---------------------------------
  // Crea File ZIP
  //---------------------------------
  system( StringaComando );
  //--------------------------------------------------
  // Cancellazione dei file da reinizializzare
  //--------------------------------------------------
//unlink("LOGTGV.DAT");
  return;
}
//---------------------------------------------------------------------------
// ----------------------------
//  RemGestioneBloccoSistema()
// ----------------------------
//  Funzione che gestisce il blocco del Sistema
//
void RemGestioneBloccoSistema(bool LeggiDM)
{
  WORD dm[3];
  int *pippo;
  int i, handle;
  WORD NumPlc=1;             // TGV "1" visto che il plc terra può non esserci
  WORD CanaleIniziale=4998;  // D4998 - DM per start blocco.
  long int TimeAttuale;
  long int TimeStop;
  char all_mess[301]="";
  char filename[81]="C:\\SYSTEM.SYS";

  if(break_Com[COM1]==true) return;
  //--------------------------------------------------
  // Verifica comunicazione diretta con macchine
  //--------------------------------------------------
  TimeAttuale=time(NULL);
  TimeStop=TimeAttuale+86400;   // Time attuale + 1 giorno
  memset(&dm, 0, sizeof(dm));
  //----------------------------------------------------
  // Se il contatore del ritardo supera i 2000 viene
  // azzerato comunque.
  //----------------------------------------------------
  RitardoAlBlocco++;
  if(RitardoAlBlocco>2000) RitardoAlBlocco=0;
  //----------------------------------------------------
  // Leggo il DM per il settaggi del Blocco del Sistema
  // DM6150 --> PLCTERRA
  //----------------------------------------------------
  if(OM.ld_canale(&dm[0], NumPlc, "RD", CanaleIniziale, 1, all_mess)) return;
  if(dm[0]==1 && RitardoAlBlocco>1000) BloccoSistema=true;
  else                                 BloccoSistema=false;
  //----------------------------------------------------
  // BLOCCO SISTEMA
  //----------------------------------------------------
  if(BloccoSistema==true){
   //dm[0]=0;
   //if(OM.wr_canale(&dm[0], 0, "WD", 6150, 1, all_mess)) return;
     //--------------------------------------------
     // Azzeramento conteggio "ritardo al blocco"
     //--------------------------------------------
     RitardoAlBlocco=0;
     //--------------------------------------------
     // Cancello il File per il blocco del Sistema
     //--------------------------------------------
     unlink(filename);
     //--------------------------------------------
     // Manda in blocco il Sistema
     //--------------------------------------------
     pippo=0;
     *pippo=0;
     return;
  }

  //----------------------------------------------------
  // Apertura del file
  //----------------------------------------------------
  handle = sopen(filename, O_RDWR | O_BINARY, SH_DENYNO);
  if( handle<0 ) return;

  //----------------------------------------------------
  // Lettura dei dati dal file
  //----------------------------------------------------
  i = read(handle, &TimeStop, sizeof(TimeStop));
  close(handle);
  if(i!=sizeof(TimeStop)) return;

  //----------------------------------------------------
  // Controllo se sono in condizione Blocco
  //----------------------------------------------------
  if(TimeAttuale>TimeStop){
     dm[0]=1;
     if(OM.wr_canale(&dm[0], NumPlc, "WD", CanaleIniziale, 1, all_mess)) return;
     //--------------------------------------------
     // Azzeramento conteggio "ritardo al blocco"
     //--------------------------------------------
     RitardoAlBlocco=0;
     //--------------------------------------------
     // Cancello il File per il blocco del Sistema
     //--------------------------------------------
     unlink(filename);
     //--------------------------------------------
     // Manda in blocco il Sistema
     //--------------------------------------------
     pippo=0;
     *pippo=0;
  }
  return;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Controllo dei PC CLIENT connessi tramite IDPC e IP_ADDRESS
//---------------------------------------------------------------------------

// --------------------------------
//  RemScriviIndirizzoClient()
// --------------------------------
//  Funzione di scrittura indirizzo CLIENT su SERVER
//
int RemScriviIndirizzoClient( char *all_mess )
{
  char FileComando[300]= "IdClient";
  char FileDest[100];
  FILE *ptrfile;

  strcpy( all_mess, "" );
  //-------------------------------------------------
  // Scrivi su server identificativo CLIENT
  //-------------------------------------------------
//strcpy( IpAddressClient, WSocket_inet_ntoa(FormServerUDP->FServerAddr));
  strcpy( IpAddressClient, FormServerUDP->LocalIPAddr.c_str());
  sprintf( FileDest, "%s%s.PC%d"  , PercorsoComandi, FileComando, IdPC );
  ptrfile = fopen(FileDest, "wb");
  if(ptrfile==0){
     sprintf( all_mess, "Not open file\: %s", FileComando);
     fclose(ptrfile);
     return 1;
  }
  Write_Line( ptrfile, IpAddressClient);
  fclose(ptrfile);

  RemIdClientTrasmesso = 1;

  return 0;
}


//--------------------------
//  RemLeggiIndirizzoClient
//--------------------------
//
int RemLeggiIndirizzoClient ( char *all_mess )
// *************************************************
{
  char FileComando[300]= "IdClient";
  char FileDest[100];
  FILE *ptrfile;
  char Ident  [200]="";

  //------------------------------------
  // Inizializzo variabili
  //------------------------------------
  strcpy( all_mess, "" );
  if( IdPC<2               ) return 0;
  if(!RemIdClientTrasmesso ) return 0;       // solo se ha eseguito la scrittura del file

  sprintf( FileDest, "%s%s.PC%d"  , PercorsoComandi, FileComando, IdPC );
  //---------------------------------------------------
  // Controllo se esiste il file di identificazione PC
  //---------------------------------------------------
  ptrfile = fopen(FileDest, "rb");
  if(ptrfile==0){
     sprintf( all_mess, "Not Exist: %s", FileComando);
     fclose(ptrfile);
     RemIdClientTrasmesso = 0;
     return 0;
  }
  Read_Line( ptrfile, Ident, 20);
  fclose(ptrfile);

  //-------------------------------------------------
  // Verifico se l'dentificativo è valido
  //-------------------------------------------------
  //strcpy( IpAddressClient, WSocket_inet_ntoa(FormServerUDP->FServerAddr));
/*
  if( strcmp( "127.0.0.1", Ident )==0 ){
     RemIdClientTrasmesso = 0;
     return 0;
  }
*/
  if( strcmp(IpAddressClient, Ident )!=0 ){
     //sprintf( all_mess, "Idirizzo IP Client non riconosciuto: %s ammesso: %s", IpAddressClient, Ident );
     sprintf( all_mess, "Client non ammesso Idirizzo IP : %s ", IpAddressClient );
     return 1;
  }

  return 0;
}



//---------------------------------------------------------------------------
// --------------------------------
//  RemControlloClientAutorizzato()
// --------------------------------
//  Funzione di controllo sulla abilitazione del PC CLIENT
//
int RemControlloClientAutorizzato( char *all_mess )
{
  char buffer[101]="";
  int IndiceClientAutorizzato=0;

  strcpy( all_mess, "" );
  sprintf(buffer, "CLIENT%d", IdPC);
  IndiceClientAutorizzato = MAX_IDPC_AUTORIZZATO;
  //-------------------------------------------------
  // Se 0 ammetti tutto
  //-------------------------------------------------
  if( IndiceClientAutorizzato==0 ) return 0;
  //-------------------------------------------------
  // Controllo su MAX CLIENT
  //-------------------------------------------------
  if( IdPC>IndiceClientAutorizzato ){
     sprintf( all_mess, "Identificativo PC errato:(%s) valori ammessi: 0,2..%d" , buffer, IndiceClientAutorizzato);
     Application->MessageBox( all_mess, "ERROR", MB_OK | MB_ICONEXCLAMATION);
     Application->Terminate();
     return 1;
  }

  if( RemLeggiIndirizzoClient ( all_mess ) ){
     Application->MessageBox( all_mess, "ERROR", MB_OK | MB_ICONEXCLAMATION);
     Application->Terminate();
     return 1;
  }
  return 0;
}


