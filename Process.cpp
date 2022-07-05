// ------------------------------------------------------------------------
//             file     PROCESS.CPP
// ------------------------------------------------------------------------
// Elenco funzioni del controllo di processo
//      process()
//
#include <stdio.h>
#include <io.h>
#include <math.h>
#include <mem.h>
#include <string.h>
#include <dir.h>
#include <conio.h>

#include <fcntl.h>
#include <errno.h>
#include <share.h>
#include <sys\stat.h>

#include "Tecnofer.h"
#include "OmronETH.h"
#include "OmronEthSer.h"
#include "OmronEthTcp.h"
#include "Main.h"
#include "Mapping.h"
#include "Modbus.h"
#include "Remoto.h"
#include "Agvwin.h"
#include "Stato.h"
#include "Layout.h"
#include "XYNodi.h"
#include "SQLDB.h"
#include "AllenBViaVbOpc.h"
#include "Slc500.h"
#include "ComunicaSiemens.h"
//--------------------------------------------------------------------
// Variabili globali
//--------------------------------------------------------------------
//  ---------------------
//   STRUTTURA NODI
//  ---------------------
//  0,1 = x,y posizione centrale nodo
//  2 = numero nodo
//  3 = occupazione agv ( numero AGV oppure libero = 0 )
//-------------------------------------------------------
extern int Punti[][5];

//  -------------------------
//   STRUTTURA MACCHINE
//  -------------------------
//  1  - Direzione di arrivo AGV ( 0>y+ 1>y- 2>x+ 3>x- )
//  2  - Stazioni macchina
//  3  - Ascissa  (X) per la sigla della macchina
//  4  - Ordinata (Y) per la sigla della macchina
//---------------------------------------------------------
extern int Macch[][5];

//--------------------------------------------------------------------------
// NB: mentre la scelta preleva il cotto nel magazzino pi— vecchio, il
//     forno deposita nel pi— giovane, in modo tale che se la scelta
//     toglie un cestone dal magazzino X, automaticamente il forno continua
//     a inserire cestoni nell'ultimo magazzino Y e non vada a inserirli nel
//     magazzino X, pi— vecchio e ora non pi— completo.
//--------------------------------------------------------------------------
//---------------------
// STRUTTURA PERCORSO
//---------------------
//  0 = Numero nodo partenza
//  1 = Numero nodo arrivo
//-------------------------------------------------------
extern int Percorso[][2];

//---------------------
// VARIABILI GLOBALI
//---------------------
int  NumAgvLoadStato=0;

// --------------------------------
//        process()
// --------------------------------
//  inizializza tutte le variabili globali della classe PROCESS.
//
process::process()
/****************/
{
  char all_mess[101];

  RunAutomaticoEseguito =false;      // Flag per sincronizzazione Run Automatico
  //----------------------------------------------------------------
  // Azzeramento della struttura dei Semiautomatici
  //----------------------------------------------------------------
  memset(&SemiAuto, 0, sizeof(SemiAuto));
  //----------------------------------------------------------------
  // File gestione CHIAMATE IMPIANTO
  //----------------------------------------------------------------
  strcpy( chiamate_name_sorg,  "CHIAMATE.DAT");
  strcpy( chiamate_name_dest,  "CHIAMATE.DAT");
  //----------------------------------------------------------------
  // File gestione BAIE IMPIANTO
  //----------------------------------------------------------------
  strcpy( baie_name_sorg,  "BAIE.DAT");
  strcpy( baie_name_dest,  "BAIE.DAT");
  //----------------------------------------------------------------
  // File gestione GRUPPI BAIE IMPIANTO
  //----------------------------------------------------------------
  strcpy( gruppibaie_name_sorg,  "GRUPPIBA.DAT");
  strcpy( gruppibaie_name_dest,  "GRUPPIBA.DAT");
  //----------------------------------------------------------------
  // File gestione PORTE IMPIANTO
  //----------------------------------------------------------------
  strcpy( porteimp_name_sorg, "PORTEIMP.DAT");
  strcpy( porteimp_name_dest, "PORTEIMP.DAT");
  //----------------------------------------------------------------
  // CAMBIO BATTERIE
  //----------------------------------------------------------------
  strcpy( cbat_name_sorg, "CAMBIOBAT.DAT");
  strcpy( cbat_name_dest, "CAMBIOBAT.DAT");
  //----------------------------------------------------------------
  // Lettura del file PORTEIMP.DAT
  //----------------------------------------------------------------
  load_file_porteimp(all_mess);
}
//---------------------------------------------------------------------------

// -----------------------------
//    load_chiamate_macchine()
// -----------------------------
//  Funzione che legge i dati dalla macchina di carico
//
int process::load_chiamate_macchine(int *NumMaccCicloLettura, char *all_mess)
//*****************************************
{
/*
  int err=0;
  int i;
  int  NumPlc;
  int  TipoArea=1;
  int  NumDB, NrDatoIni, NrDati;
  int  NumDispositivo;
  WORD WordDati[200];
  WORD WordDatiStaz[200];
  bool LetturaEseguita=false;
  int  NumStazioni;
  int  NumMac;
  int  Staz;
  int  CicloNumMacEnd;
  int  IndicePiazzola=0;
  int  CicloNumMacStessoPlc=1;
  int  MsecRisp;
  //int  Index=1;
  bool EsclComunicMacc;


  err=0;
  memset(&WordDati,       0, sizeof(WordDati));
  memset(&WordDatiStaz,   0, sizeof(WordDatiStaz));
  if( *NumMaccCicloLettura<1           ) *NumMaccCicloLettura=1;
  if( *NumMaccCicloLettura>TOTCHIAMATE ) *NumMaccCicloLettura=1;
  //--------------------------------------------------
  // se break_plc allora SKIP !!!!
  //--------------------------------------------------
  if(break_Com[COM2]) return 0;
  //--------------------------------------------------
  // Macchine 4..11 8 macchine su stesso PLC
  //--------------------------------------------------
  CicloNumMacStessoPlc=1;
  if( *NumMaccCicloLettura==4 )   CicloNumMacStessoPlc=8;
  //--------------------------------------------------
  // Verifica se almeno una macchina è abilitata
  // alla comunicazione
  //--------------------------------------------------
  CicloNumMacEnd=*NumMaccCicloLettura+CicloNumMacStessoPlc;
  EsclComunicMacc=true;
  for(NumMac=*NumMaccCicloLettura; NumMac<CicloNumMacEnd; NumMac++){
     if( P.ch[NumMac]->EsclComunic==false ){
        EsclComunicMacc=false;
        break;
     }
  }
  //---------------------------------------------------------------
  // Lettura da PLC
  // Ciclico per tutte le macchine ad eccezzione delle AVM1..8...??
  //---------------------------------------------------------------
  err=0;
  LetturaEseguita=false;
  CicloNumMacEnd=*NumMaccCicloLettura+1;
  for(NumMac=*NumMaccCicloLettura; NumMac<CicloNumMacEnd; NumMac++){
     if( P.ch[NumMac]->Num==0            ) continue;
     if( EsclComunicMacc==true           ) continue;  // Tiene conto anche di più macchine sullo stesso PLC
     NumDispositivo = P.ch[NumMac]->Plc.Plc;
     NumDB          = P.ch[NumMac]->Plc.RD_NumDB;
     NrDatoIni      = P.ch[NumMac]->Plc.RD_NrChIni;
     NrDati         = P.ch[NumMac]->Plc.RD_NrCh;

     //-------------------------------------------------------------
     // Le macchine su stesso PLC vengon lette contemporaneamente
     //-------------------------------------------------------------
     if( CicloNumMacStessoPlc>1 ){
        NrDati=NrDati*CicloNumMacStessoPlc;
     }
     //-------------------------------------------------------------
     // Lettura PLC a bamchi di dati NON superiori a 64
     //-------------------------------------------------------------
     if( NrDati<=60 ){
        if(err==0) err=SIM_S7ReadWordPlc( NumDispositivo, TipoArea, NumDB, NrDatoIni, NrDati, WordDati, all_mess);
     }else{
        if(err==0) err=SIM_S7ReadWordPlc( NumDispositivo, TipoArea, NumDB, NrDatoIni    ,        60, &WordDati[ 0], all_mess);
        if(err==0) err=SIM_S7ReadWordPlc( NumDispositivo, TipoArea, NumDB, NrDatoIni+120, NrDati-60, &WordDati[60], all_mess);
     }
     if(err==0) LetturaEseguita=true;
  }
  //---------------------------------------------------------------
  // Compilazione dati Letti PLC
  //---------------------------------------------------------------
  CicloNumMacEnd=*NumMaccCicloLettura+CicloNumMacStessoPlc;
  for(NumMac=*NumMaccCicloLettura; NumMac<CicloNumMacEnd; NumMac++){
     P.ch[NumMac]->PlcErr=err;

     if( P.ch[NumMac]->Num==0            ) continue;
     if( P.ch[NumMac]->EsclComunic==true ) continue;
     if( P.ch[NumMac]->PlcErr>0          ) continue;
     if( LetturaEseguita==false          ) continue;

     //-----------------------------------------
     // Macchina Tipo SACMI ESSICATOIO
     //-----------------------------------------
     if(P.ch[NumMac]->Tipo==MC_ESSICCATOIO ){
        // N.B. Il byte + leggero è dal bit 8...
        P.ch[NumMac]->Automatico   = (bool)(TestBit((char *) &WordDati[0], 8)); //DB400.dbx0.0
        P.ch[NumMac]->FineCicloEs  = (bool)(TestBit((char *) &WordDati[0], 9)); //DB400.dbx0.1
        P.ch[NumMac]->OkPortaP1    = (bool)(TestBit((char *) &WordDati[0],10)); //DB400.dbx0.2
        P.ch[NumMac]->Blocco       = (bool)(TestBit((char *) &WordDati[0],11)); //DB400.dbx0.3
        P.ch[NumMac]->OkPortaP2    = (bool)(TestBit((char *) &WordDati[0],12)); //DB400.dbx0.4
     }
     //-----------------------------------------
     // Macchina Tipo GAIOTTO
     //-----------------------------------------
     else{
        NumStazioni=Macch[NumMac][1];
        //-----------------------------------------
        // Compila tutte le stazioni
        //-----------------------------------------
        for(Staz=0; Staz<NumStazioni; Staz++){
           if( Staz>=MAXSTAZMAC ) break;
           if( CicloNumMacStessoPlc>1 ){
              IndicePiazzola=NumMac-*NumMaccCicloLettura;
           }else{
              IndicePiazzola=Staz;
           }
           memcpy( &WordDatiStaz[0], &WordDati[IndicePiazzola*10], sizeof(WORD)*10);
           P.ch[NumMac]->Staz[Staz].Richiesta        = (bool)(TestBit((char *) &WordDatiStaz[0], 8));
           P.ch[NumMac]->Staz[Staz].Espulsione       = (bool)(TestBit((char *) &WordDatiStaz[0], 9));
           P.ch[NumMac]->Staz[Staz].GiraBox          = (bool)(TestBit((char *) &WordDatiStaz[0],10));
           P.ch[NumMac]->Staz[Staz].OkIngresso       = (bool)(TestBit((char *) &WordDatiStaz[0],11));
           P.ch[NumMac]->Staz[Staz].Blocco           = (bool)(TestBit((char *) &WordDatiStaz[0],12));
           P.ch[NumMac]->Staz[Staz].RicIspezManuale  = (bool)(TestBit((char *) &WordDatiStaz[0],13));
           P.ch[NumMac]->Staz[Staz].TempoInMag       = (short int) WordDatiStaz[1];
           P.ch[NumMac]->Staz[Staz].IDEssiccazione   = (short int) WordDatiStaz[2];
           P.ch[NumMac]->Staz[Staz].StatoBox         = (short int) WordDatiStaz[3];
           P.ch[NumMac]->Staz[Staz].TipoMateriale    = (short int) WordDatiStaz[4];
           P.ch[NumMac]->Staz[Staz].TipoBoxVuoto     = (short int) WordDatiStaz[5];
        }
     }
  }

  *NumMaccCicloLettura+=CicloNumMacStessoPlc;
  if( *NumMaccCicloLettura>TOTCHIAMATE ) *NumMaccCicloLettura=1;
  return err;
*/
  return 0;
}


// -----------------------------
//    write_chiamate_macchine()
// -----------------------------
//  Funzione che compila i dati verso le macchine
//
int process::write_chiamate_macchine(int* NumMaccCicloScrittura, char *all_mess)
//*****************************************
{
/*
  int err=0;
  int i;
  int  NumPlc;
  int  TipoArea=1;
  int  NumDB, NrDatoIni, NrDati;
  int  NumDispositivo;
  WORD WordDati[200];
  WORD WordDatiStaz[200];
//  bool ScritturaEseguita=false;
  int  NumStazioni;
  int  NumMac;
  int  Staz;
  int  CicloNumMacEnd;
  int  IndicePiazzola=0;
  int  CicloNumMacStessoPlc=1;
  bool EsclComunicMacc;
  bool ErrComunicazione;

  err=0;
  memset(&WordDati,       0, sizeof(WordDati));
  memset(&WordDatiStaz,   0, sizeof(WordDatiStaz));
  if( *NumMaccCicloScrittura<1           ) *NumMaccCicloScrittura=1;
  if( *NumMaccCicloScrittura>TOTCHIAMATE ) *NumMaccCicloScrittura=1;
  //--------------------------------------------------
  // se break_plc allora SKIP !!!!
  //--------------------------------------------------
  if(break_Com[COM2]) return 0;
  //--------------------------------------------------
  // Macchine 4..11 8 macchine su stesso PLC
  //--------------------------------------------------
  CicloNumMacStessoPlc=1;
  if( *NumMaccCicloScrittura==4 )   CicloNumMacStessoPlc=8;
  //-----------------------------------------------------------------
  // Verifica se almeno una macchina è abilitata alla comunicazione
  //-----------------------------------------------------------------
  CicloNumMacEnd=*NumMaccCicloScrittura+CicloNumMacStessoPlc;
  EsclComunicMacc=true;
  ErrComunicazione=true;
  for(NumMac=*NumMaccCicloScrittura; NumMac<CicloNumMacEnd; NumMac++){
     if( P.ch[NumMac]->EsclComunic==false ){
        EsclComunicMacc=false;
     }
     if( P.ch[NumMac]->PlcErr==false ){
        ErrComunicazione=false;
     }
  }

  //---------------------------------------------------------------
  // Compilazione dati da Scrivere su PLC
  //---------------------------------------------------------------
  CicloNumMacEnd=*NumMaccCicloScrittura+CicloNumMacStessoPlc;
  for(NumMac=*NumMaccCicloScrittura; NumMac<CicloNumMacEnd; NumMac++){
     //-----------------------------------------
     // Macchina Tipo SACMI ESSICATOIO
     //-----------------------------------------
     if(P.ch[NumMac]->Tipo==MC_ESSICCATOIO ){
        WordDati[0]=0;
        WordDati[1]=0;
        // N.B. Il byte + leggero è dal bit 8...
        if(P.ch[NumMac]->RichApP1_X     ) SetBit((char *) &WordDati[0], 8);
        if(P.ch[NumMac]->RichChP1_Y     ) SetBit((char *) &WordDati[0], 9);
        if(P.ch[NumMac]->RichApP2_X     ) SetBit((char *) &WordDati[0],10);
        if(P.ch[NumMac]->RichChP2_Y     ) SetBit((char *) &WordDati[0],11);
        if(P.ch[NumMac]->RichCicloEs    ) SetBit((char *) &WordDati[0],12);
        WordDati[1]=P.ch[NumMac]->IDEssicMacMax;                          // ID Ricetta 1..3
     }
     //-----------------------------------------
     // Macchina Tipo GAIOTTO
     //-----------------------------------------
     else{
        NumStazioni=Macch[NumMac][1];
        //-----------------------------------------
        // Compila tutte le stazioni
        //-----------------------------------------
        for(Staz=0; Staz<NumStazioni; Staz++){
           if( Staz>=MAXSTAZMAC ) break;
           if( CicloNumMacStessoPlc>1 ){
              IndicePiazzola=NumMac-*NumMaccCicloScrittura;
           }else{
              IndicePiazzola=Staz;
           }
           memset(&WordDatiStaz,   0, sizeof(WordDatiStaz));
           // N.B. Il byte + leggero è dal bit 8...
           if(P.ch[NumMac]->Staz[Staz].RicIngresso         ) SetBit((char *) &WordDatiStaz[0], 8);
           if(P.ch[NumMac]->Staz[Staz].FineOperazioneTGV   ) SetBit((char *) &WordDatiStaz[0], 9);


           WordDatiStaz[1]=P.ch[NumMac]->Staz[Staz].Pos[0].Box.ID ;
           WordDatiStaz[2]=0;
           WordDatiStaz[3]=P.ch[NumMac]->Staz[Staz].Pos[0].Box.StatoBox ;
           WordDatiStaz[4]=P.ch[NumMac]->Staz[Staz].Pos[0].Box.TipoMateriale ;
           //------------------------------------------
           // Se comunicazione sclusa azzera comunque
           //------------------------------------------
           if( P.ch[NumMac]->Num==0 || P.ch[NumMac]->EsclComunic==true ){
              memset(&WordDatiStaz,   0, sizeof(WordDatiStaz));
           }
           memcpy( &WordDati[IndicePiazzola*10], &WordDatiStaz[0], sizeof(WORD)*10);
        }
     }
  }



  //---------------------------------------------------------------
  // Scrittura PLC
  // Ciclico per tutte le macchine ad eccezzione delle AVM1..8...??
  //---------------------------------------------------------------
  err=0;
//  ScritturaEseguita=false;
  CicloNumMacEnd=*NumMaccCicloScrittura+1;
  for(NumMac=*NumMaccCicloScrittura; NumMac<CicloNumMacEnd; NumMac++){
     if( P.ch[NumMac]->Num==0            ) continue;
     if( EsclComunicMacc==true           ) continue;  // Tiene conto anche di più macchine sullo stesso PLC
     if( ErrComunicazione==true          ) continue;  // Se errore PLC Vuol dire che la lettura precedente NON è
                                                      // andata a buon fine quindi inutile scrivere!!
     NumDispositivo = P.ch[NumMac]->Plc.Plc;
     NumDB          = P.ch[NumMac]->Plc.WR_NumDB;
     NrDatoIni      = P.ch[NumMac]->Plc.WR_NrChIni;
     NrDati         = P.ch[NumMac]->Plc.WR_NrCh;

     //-------------------------------------------------------------
     // Le macchine su stesso PLC vengon lette contemporaneamente
     //-------------------------------------------------------------
     if( CicloNumMacStessoPlc>1 ){
        NrDati=NrDati*CicloNumMacStessoPlc;
     }
     //-------------------------------------------------------------
     // Lettura PLC a bamchi di dati NON superiori a 64
     //-------------------------------------------------------------
     if( NrDati<=60 ){
        if(err==0) err=SIM_S7WriteWordPlc( NumDispositivo, TipoArea, NumDB, NrDatoIni, NrDati, WordDati, all_mess);
     }else{
        if(err==0) err=SIM_S7WriteWordPlc( NumDispositivo, TipoArea, NumDB, NrDatoIni    ,        60, &WordDati[ 0], all_mess);
        if(err==0) err=SIM_S7WriteWordPlc( NumDispositivo, TipoArea, NumDB, NrDatoIni+120, NrDati-60, &WordDati[60], all_mess);
     }
     //if(err==0) ScritturaEseguita=true;
  }

  *NumMaccCicloScrittura+=CicloNumMacStessoPlc;
  if( *NumMaccCicloScrittura>TOTCHIAMATE ) *NumMaccCicloScrittura=1;
  return err;
*/
  return 0;
}


// ------------------------------------
//    setta_consensi_chiamate( )
// ------------------------------------
//
int process::setta_consensi_chiamate(char *all_mess)
// ********************************************************
{
/*
  WORD err=0;
  int  x1, x2, y1, y2;
  int  distX, distY;
  bool Ingombro;
  bool OkIngresso;
  bool RichiestaIngresso;
  short int i, Idx, NumAgv, Mac, Staz, Dest, PuntoIng, PuntoOut, PuntoPrecedente;
  double dist;
  bool SalvaMacchina[MAXCHIAMATE+2];


  strcpy( all_mess, "" );
  memset(&SalvaMacchina, 0, sizeof(SalvaMacchina));
  //-----------------------------------------------------
  // A) Verifica se c'è un AGV in ingombro alla macchina
  //-----------------------------------------------------
  for(Mac=1; Mac<=MAXCHIAMATE; Mac++){
     if(ch[Mac]->Num==0) continue;
     for(Staz=0; Staz<=MAXSTAZMAC; Staz++){
        Ingombro=false;
        RichiestaIngresso=false;
        PuntoIng=ch[Mac]->Staz[Staz].PuntoIng;
        PuntoOut=ch[Mac]->Staz[Staz].PuntoOut;
        if(PuntoIng==0 && PuntoOut==0) continue;
        OkIngresso = ch[Mac]->Staz[Staz].OkIngresso;
        //---------------------------------
        // Verifica se è sul nodo stazione
        //---------------------------------
        for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
           if(AGV[NumAgv]->stato.pos==PuntoIng || AGV[NumAgv]->stato.pos==PuntoOut){
              Ingombro=true;
              AGV[ NumAgv ]->mission.MacInIngombro=0;
              RichiestaIngresso=true;
              break;
           }
        }
        //-------------------------------------------------------------
        // Non più su nodo macchina
        //-------------------------------------------------------------
        for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
           if(AGV[NumAgv]->mission.MacInIngombro!=PuntoIng &&
              AGV[NumAgv]->mission.MacInIngombro!=PuntoOut) continue;
           if(N.nodo_busy[PuntoIng]==NumAgv               ) continue;
           if(N.nodo_busy[PuntoOut]==NumAgv               ) continue;
           AGV[ NumAgv ]->mission.MacInIngombro=0;
        }
        //-------------------------------------------------------------
        // Verifica se AGV in transito verso INGOMBRO STAZ MACCHINA
        //-------------------------------------------------------------
        for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
           if(AGV[ NumAgv ]->mission.MacInIngombro==PuntoIng || AGV[ NumAgv ]->mission.MacInIngombro==PuntoOut){
              Ingombro=true;
              RichiestaIngresso=true;
              break;
           }
        }
        //-----------------------------------------------
        // Considero fuori ingombro la navetta quando è
        // sul primo nodo a una distanza superiore ai
        // 250cm dalla stazione di partenza.
        //-----------------------------------------------
        for( NumAgv=1; NumAgv<=MAXAGV; NumAgv++ ){
           if(AGV[ NumAgv ]->stato.pos==PuntoIng || AGV[ NumAgv ]->stato.pos==PuntoOut            ) continue;
           if(AGV[ NumAgv ]->mission.pstart!=PuntoIng && AGV[ NumAgv ]->mission.pstart!=PuntoOut  ) continue;
           //----------------------------------
           // Recupero nodo di partenza
           //----------------------------------
           Dest==0;
           if(AGV[NumAgv]->mission.pstart==PuntoIng) Dest=PuntoIng;
           if(AGV[NumAgv]->mission.pstart==PuntoOut) Dest=PuntoOut;
           if(Dest==0) continue;
           //----------------------------------
           // Recupero coordinate
           //----------------------------------
           x1 = Punti[AGV[ NumAgv ]->stato.pos][0];
           y1 = Punti[AGV[ NumAgv ]->stato.pos][1];
           x2 = Punti[Dest][0];
           y2 = Punti[Dest][1];
           //----------------------------------
           // Se i nodi si trovano in retta la
           // distanza viene calcolata con il
           // teorema di pitagora.
           //----------------------------------
           distX = abs(x1-x2);
           distY = abs(y1-y2);
           //----------------------------------
           // Punti troppo lontani
           //----------------------------------
           if(distX>30000 || distX>30000) continue;
           //----------------------------------
           // Calcolo distanza dalla stazione
           //----------------------------------
           dist = (double)(sqrt((distX*distX)+(distY*distY)));
           //----------------------------------
           // Se la distanza<250 sono ancora
           // in ingombro
           //----------------------------------
           if(dist<200) Ingombro=true;
           break;
        }
        //-------------------------------------------------------------
        // Verifica se è su nodo precedente in richiesta di accesso
        //-------------------------------------------------------------
        for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
           if((AGV[NumAgv]->mission.pend==PuntoIng && N.nodo_busy[PuntoIng]==NumAgv) || (AGV[NumAgv]->mission.pend==PuntoOut && N.nodo_busy[PuntoOut]==NumAgv)){
              if(AGV[ NumAgv ]->stato.s.bit.inattesa){
                 //----------------------------------------------
                 // Memorizza che l'AGV fa richiesta di ingresso
                 //----------------------------------------------
                 RichiestaIngresso=true;
                 break;
              }
           }
        }
        //-------------------------------------------------------------
        // Verifica se è sul nodo precedente alla stazione
        //-------------------------------------------------------------
        if( OkIngresso ){
           for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
              Dest==0;
              if(AGV[NumAgv]->mission.pend==PuntoIng) Dest=PuntoIng;
              if(AGV[NumAgv]->mission.pend==PuntoOut) Dest=PuntoOut;
              if(Dest>0 && Dest<TOTPUNTI){
                 for(i=0; i<MAXPERCORSI; i++){
                    if(AGV[NumAgv]->mission.punto[i]==0    ) break;
                    if(AGV[NumAgv]->mission.punto[i]==Dest ) break;
                 }
                 if(i>0 && AGV[ NumAgv ]->mission.punto[i]==Dest){
                    PuntoPrecedente=AGV[ NumAgv ]->mission.punto[i-1];
                    if(AGV[NumAgv]->stato.pos==PuntoPrecedente){
                       //----------------------------------------------
                       // Memorizza che l'AGV sta entrando in ingombro
                       //----------------------------------------------
                       Ingombro=true;
                       AGV[NumAgv]->mission.MacInIngombro=Dest;
                       RichiestaIngresso=true;
                       break;
                    }
                 }
              }
           }
        }
        //-------------------------------------------------------------
        // Verifica se è su nodo precedente e sta entrando su stazione
        //-------------------------------------------------------------
        if( OkIngresso ){
           for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
              Dest==0;
              if(AGV[NumAgv]->mission.pend==PuntoIng) Dest=PuntoIng;
              if(AGV[NumAgv]->mission.pend==PuntoOut) Dest=PuntoOut;
              if(Dest>0 && Dest<TOTPUNTI){
                 if(N.nodo_busy[Dest]==NumAgv){
                    if(AGV[ NumAgv ]->stato.s.bit.inattesa){
                       //----------------------------------------------
                       // Memorizza che l'AGV sta entrando in ingombro
                       //----------------------------------------------
                       Ingombro=true;
                       AGV[NumAgv]->mission.MacInIngombro=Dest;
                       RichiestaIngresso=true;
                       break;
                    }
                 }
              }
              if( Ingombro ) break;
           }
        }
        //-----------------------------------------------
        // Compilazione consensi su struttura
        //-----------------------------------------------
        ch[Mac]->Staz[Staz].TgvInIngombro = Ingombro;
        ch[Mac]->Staz[Staz].RicIngresso   = RichiestaIngresso;
        //-----------------------------------------------
        // E) Se Consenso Ok da PAL azzera DM BLOCCO AGV
        //-----------------------------------------------
        if(OkIngresso){
           for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
              if(AGV[ NumAgv ]->stato.s.bit.inattesa){
                 if(AGV[ NumAgv ]->mission.pend==PuntoIng || AGV[ NumAgv ]->mission.pend==PuntoOut){
                    if(AGV[ NumAgv ]->TrasmettiResetBloccoIngAGV(all_mess)) return 1;
                 }
              }
           }
        }
        //-----------------------------------------------
        // E1) Set/Reset Fine Operazione
        //-----------------------------------------------
        ch[Mac]->Staz[Staz].FineOperazioneTGV =0;
        if( ch[Mac]->Staz[Staz].NumCicliResetFineOpraz>NUM_CICLI_RESET_FINEOP) ch[Mac]->Staz[Staz].NumCicliResetFineOpraz=NUM_CICLI_RESET_FINEOP;
        if( ch[Mac]->Staz[Staz].NumCicliResetFineOpraz<0                     ) ch[Mac]->Staz[Staz].NumCicliResetFineOpraz=0;
        if( ch[Mac]->Staz[Staz].NumCicliResetFineOpraz>0){
           if( ch[Mac]->Staz[Staz].RicIngresso==false && Ingombro==false ){
              if( ch[Mac]->Staz[Staz].Richiesta==false && ch[Mac]->Staz[Staz].Espulsione==false ){
                 ch[Mac]->Staz[Staz].FineOperazioneTGV=true;
                 ch[Mac]->Staz[Staz].NumCicliResetFineOpraz--;
                 if( ch[Mac]->Staz[Staz].NumCicliResetFineOpraz==0 ){
                    SalvaMacchina[Mac]=true;
                 }
              }
           }
        }
     }
  }
  //-----------------------------------------------
  // F) Setta Consenso al movimento (forche alte)
  //-----------------------------------------------
  for(Mac=1; Mac<=MAXCHIAMATE; Mac++){
     if(ch[Mac]->Num==0) continue;
     for(Staz=0; Staz<=MAXSTAZMAC; Staz++){
        //-----------------------------------------------------
        // Verifica se è su nodo pronto per entrare
        //-----------------------------------------------------
        ch[Mac]->Staz[Staz].OkForcheAlte=false;
        for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
           if( AGV[ NumAgv ]->stato.pos==PuntoIng || AGV[ NumAgv ]->stato.pos==PuntoOut ){
              if(AGV[ NumAgv ]->stato.s.bit.ForcaA_FuoriIng &&
                 AGV[ NumAgv ]->stato.s.bit.ForcaB_FuoriIng ){
                 ch[Mac]->Staz[Staz].OkForcheAlte=true;
              }
           }
        }
     }
  }


  //-----------------------------------------------
  // H) Compila CONSENSI Stato Essicatoio
  //-----------------------------------------------
  bool MaccVuota, MaccPiena;
  for(Mac=1; Mac<=MAXCHIAMATE; Mac++){
     if(ch[Mac]->Num==0               ) continue;
     if(ch[Mac]->Tipo!=MC_ESSICCATOIO ) continue;
     ch[Mac]->TgvIngMac = Ingombro= false;
     MaccVuota=true; MaccPiena=true;

     //------------------------------------------------------
     // Verifica se AGV in INGOMBRO Macchina
     //------------------------------------------------------
     for(Staz=0; Staz<=MAXSTAZMAC; Staz++){
        if( ch[Mac]->Staz[Staz].Npt==0 ) continue;
        if( ch[Mac]->Staz[Staz].TgvInIngombro==true ){
           ch[Mac]->TgvIngMac = Ingombro=true;
        }
     }
     //-----------------------------------------------------
     // Verifica se Essicatoio COMPLETAMENTE VUOTO
     //-----------------------------------------------------
     for(Staz=0; Staz<=MAXSTAZMAC; Staz++){
        if(Staz>=Macch[Mac][1]                             ) continue;  // Valuta solo le stazioni previste per la macchina
        if(ch[Mac]->Staz[Staz].Npt==0                      ) continue;
        if(ch[Mac]->Staz[Staz].NumCestoni==0               ) continue;
        MaccVuota=false;
     }
     //-----------------------------------------------------
     // Verifica se Essicatoio COMPLETAMENTE PIENO
     //-----------------------------------------------------
     for(Staz=0; Staz<=MAXSTAZMAC; Staz++){
        if(Staz>=Macch[Mac][1]                                      ) continue;  // Valuta solo le stazioni previste per la macchina
        if(ch[Mac]->Staz[Staz].Npt==0                               ) continue;
        if(ch[Mac]->Staz[Staz].NumCestoni>=ch[Mac]->Staz[Staz].Npt  ) continue;
        MaccPiena=false;
     }
     //-----------------------------------------------------
     // COMPILA STATO:
     // 0=VUOTO
     // 1=RIMPIMENTO
     // 2=ESSICCARE
     // 3=ESSICATO
     //-----------------------------------------------------
     if( ch[Mac]->StatoMac>-1                  && MaccVuota            ){  // --> 0 VUOTO
        ch[Mac]->StatoMac=ST_ESS_VUOTO;
     }
     if( ch[Mac]->StatoMac==ST_ESS_VUOTO       && !MaccVuota           ){  // --> 1 RIEMPIMENTO
        ch[Mac]->StatoMac=ST_ESS_RIEMPIMENTO;
     }
     if( ch[Mac]->StatoMac==ST_ESS_RIEMPIMENTO && MaccPiena            ){  // --> 2 PIENO DA ESSICCARE
        ch[Mac]->StatoMac=ST_ESS_CICLOESSICA;
     }
     if( ch[Mac]->StatoMac==ST_ESS_CICLOESSICA && ch[Mac]->FineCicloEs ){  // --> 3 FINE ESSICAZIONE DA EVACUARE
        ch[Mac]->StatoMac=ST_ESS_FINECICLO;
     }
     //-----------------------------------------------------
     // Compila chiamate
     //-----------------------------------------------------
     for(Staz=0; Staz<=MAXSTAZMAC; Staz++){
        if(Staz>=Macch[Mac][1]                                      ) continue;  // Valuta solo le stazioni previste per la macchina
        if(ch[Mac]->Staz[Staz].Npt==0                               ) continue;
        ch[Mac]->Staz[Staz].Richiesta=false;
        ch[Mac]->Staz[Staz].Espulsione=false;
        if( ch[Mac]->Automatico==false ) continue;      // Se manca l'automatico azzera la chiamata ?
        if( ch[Mac]->Blocco     ==true ) continue;      // Se manca l'automatico azzera la chiamata ?

        //--------------------------------------------------------
        // OK ACCESSO
        // Compila Bit di OK accesso su stazione se PORTA APERTA
        // Dopo la richiesta del TGV (per sicurezza)
        //--------------------------------------------------------
        ch[Mac]->Staz[Staz].OkIngresso=false;
        if( ch[Mac]->Staz[Staz].RicIngresso==true || ch[Mac]->Staz[Staz].TgvInIngombro==true ){
           if( Staz==0 ) ch[Mac]->Staz[Staz].OkIngresso= ch[Mac]->OkPortaP1;
           if( Staz==1 ) ch[Mac]->Staz[Staz].OkIngresso= ch[Mac]->OkPortaP2;
        }
        //--------------------------------------
        // 0-1) ESSICATOIO VUOTO o RIEMPIMENTO
        //--------------------------------------
        if( ch[Mac]->StatoMac==ST_ESS_VUOTO || ch[Mac]->StatoMac==ST_ESS_RIEMPIMENTO ){
           if(ch[Mac]->Staz[Staz].NumCestoni<ch[Mac]->Staz[Staz].Npt  ){
              ch[Mac]->Staz[Staz].Richiesta=true;
           }
        }
        //--------------------------------------
        // 2 ) ESSICATOIO --> INIZIO CICLO
        //     Chiusura Porte 1 e 2
        //--------------------------------------
        if( ch[Mac]->StatoMac==ST_ESS_CICLOESSICA ){
           if( Ingombro==0 ){
              ch[Mac]->RichCicloEs=true;

              ch[Mac]->RichApP1_X=false;
              ch[Mac]->RichChP1_Y=true;

              ch[Mac]->RichApP2_X=false;
              ch[Mac]->RichChP2_Y=true;
           }
        }else{
           ch[Mac]->RichCicloEs=false;
        }
        //--------------------------------------
        // 3 ) ESSICATOIO --> SVUOTARE
        //--------------------------------------
        if( ch[Mac]->StatoMac==ST_ESS_FINECICLO ){
           if(ch[Mac]->Staz[Staz].NumCestoni>0  ){
              ch[Mac]->Staz[Staz].Espulsione=true;
           }
        }
        //--------------------------------------
        // APERTURA PORTE
        //--------------------------------------
        if( ch[Mac]->Staz[Staz].RicIngresso==true || ch[Mac]->Staz[Staz].TgvInIngombro==true ){
           if( Staz==0 ){
              ch[Mac]->RichApP1_X=true;
              ch[Mac]->RichChP1_Y=false;
           }
           if( Staz==1 ){
              ch[Mac]->RichApP2_X=true;
              ch[Mac]->RichChP2_Y=false;
           }
        }
     }
  }

  //-----------------------------------------------
  // Z) Salva record macchina modificati
  //-----------------------------------------------
  for(Mac=1; Mac<=MAXCHIAMATE; Mac++){
     if(ch[Mac]->Num==0               ) continue;

     //----------------------------
     // Salva Record macchina
     //----------------------------
     if( SalvaMacchina[Mac]==true){
        save_record_chiamate(Mac, all_mess);
     }
  }

  return err;
*/
  return 0;
}


//------------------------------- o ----------------------------------



//=========== CHIAMATE

// ----------------------------------------------
//       read_chiamate()
// ----------------------------------------------
// Legge il record "ch" in base a "num_mac" passato come parametro
//
int process::read_chiamate(int num_mac, struct CHIAMATE *mc, char *all_mess)
/***********************************************/
{
  int  stato=0;

  stato = cerca_record_chiamate(mc, num_mac );
  if(stato){
     //---------------------------------
     // record non trovato ! -> errore
     //---------------------------------
     sprintf(all_mess, "Error in read macchina %d (%s)", num_mac, "read_chiamate()");
     return stato;
  }
  return 0;

}

// ----------------------------------------------
//       test_punto_presente_mac()
// ----------------------------------------------
// Verifica se  "punto" è + un nodo di una Macchina. Se si ritorna in
// "macchina" il numero della macchina trovato.
//
int  process::test_punto_presente_mac(char *all_mess, short int punto, short int *num_mac, short int *num_stz)
/**********************************************************************/
{
  bool trovato;
    int n_rec, stato,i;
  struct CHIAMATE mc;

  strcpy( all_mess, "OK");
    //--------------------------------
  // Azzeramento variabili
  //--------------------------------
  *num_mac =0;
  *num_stz =0;
  //--------------------------------
  // Nodo non valido
  //--------------------------------
  if(punto<1 || punto>TOTPUNTI){
     sprintf(all_mess, "(0) Error node %d does not good for machines archive...", punto);
     return 1;
  }
  //--------------------------------
  // Cerca fra tutte le Macchine
  //--------------------------------
  for(n_rec=1; n_rec<=MAXCHIAMATE; n_rec++){
     if(n_rec>TOTCHIAMATE    ) break;
     if(ch[ n_rec ]->Num==0  ) continue;
     trovato=false;
     for(i=0; i<MAXSTAZMAC; i++){
        if(punto!=ch[ n_rec ]->Staz[i].PuntoIng && punto!=ch[ n_rec ]->Staz[i].PuntoOut) continue;
        *num_mac = ch[ n_rec ]->Num;
        *num_stz = i;
        trovato  = true;
        break;
     }
     if(trovato==true) break;
  }
  if(*num_mac==0){
     sprintf(all_mess, "Error search node %d in machine archive...", punto);
     return 1;
  }
  return 0;
}


// ------------------------
//  load_file_chiamate()
// ------------------------
// Dal file AUTOCLAV.DAT compila i record in memoria precedentemente allocati
//
int process::load_file_chiamate(char *all_mess)
/**********************************/
{
  int  err=0;
  int  i, len;
  int  handle;

  strcpy( all_mess, "OK" );
  //------------------------------
  // Azzera records in memoria
  //------------------------------
  for( i=0; i<=MAXCHIAMATE; i++){
     memset( ch[i], 0, sizeof( CHIAMATE ));
  }
  //------------------------------
  // Apertura file CHIAMATE.DAT
  //------------------------------
  //-------------------------------------------------
  // Controllo se il File pu• essere Letto aprendolo
  // in modalit… di Condivisione.
  //-------------------------------------------------
  handle = sopen( chiamate_name_sorg, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
      close( handle );
      sprintf(all_mess, MESS[112], chiamate_name_sorg);
      return 1;
  }
  //--------------------------------
  // Lettura file LOCAZIONE.DAT
  //--------------------------------
  for( i=0; i<=MAXCHIAMATE; i++){
     len = read( handle, ch[i], sizeof( CHIAMATE ) );
     if (len != sizeof( CHIAMATE ))  err = 1;
     if(err){
        memset( ch[i], 0, sizeof( CHIAMATE ));
        sprintf(all_mess, MESS[351], i, chiamate_name_sorg);
        break;
     }
  }
  close( handle );

  return err;
}


// ------------------------
//  save_record_chiamate()
// ------------------------
// Salva sul file AUTOCLAV.DAT i records di macchine in memoria
//
int process::save_record_chiamate(int numero, char *all_mess)
/**********************************/
{
  int  err=0;
  long curpos;
  int  len;
  unsigned long length;
  int  handle;

  strcpy( all_mess, "OK" );
  //------------------------------
  // Apertura file CHIAMATE.DAT
  //------------------------------
  //-------------------------------------------------
  // Controllo se il File pu• essere Letto aprendolo
  // in modalit… di Condivisione.
  //-------------------------------------------------
  handle = sopen( chiamate_name_dest, O_RDWR | O_BINARY , SH_DENYNO );
  if( handle<0 ){
     close( handle );
     sprintf(all_mess, MESS[112], chiamate_name_dest);
     return 1;
  }
  //------------------------------
  // Controllo lunghezza file
  //------------------------------
  length=0;
  if(handle >-1) length = filelength(handle);
  if( length < ((long)(numero)* sizeof( CHIAMATE ))){
     sprintf(all_mess, MESS[113], chiamate_name_dest);
     close(handle);
     return 1;
  }

  //--------------------------------------
  // Calcolo dell'OFFSET e posizionamento
  //--------------------------------------
  curpos = (long ) numero * sizeof( CHIAMATE );
  if( lseek( handle, curpos, SEEK_SET )<1){  // pos. il puntatore a inzio record
     sprintf(all_mess, "errore puntamento al record %d del file %s...", numero, chiamate_name_dest);
     close( handle );
     return 1;
  }
  //-------------------------------
  // Scrittura Record LOCAZION.DAT
  //-------------------------------
  len= write( handle, ch[numero], sizeof( CHIAMATE ));
  if(len != sizeof( CHIAMATE )){
     err=1;
     sprintf(all_mess, "errore scrittura del record %d del file %s...", numero, chiamate_name_dest);
  }
  close(handle);

  return err;
}


// ------------------------
//    save_file_chiamate()
// ------------------------
// Salva sul file AUTOCLAV.DAT i records delle macchine in memoria
//
int process::save_file_chiamate(char *all_mess)
/**********************************/
{
  int err=0;
  int i, len;
  int handle;

  strcpy( all_mess, "OK" );
  //------------------------------
  // Apertura file LOCAZION.DAT
  //------------------------------
  //-------------------------------------------------
  // Controllo se il File pu• essere Letto aprendolo
  // in modalit… di Condivisione.
  //-------------------------------------------------
  handle = sopen( chiamate_name_dest, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
  if( handle<0 ){
     close( handle );
     sprintf(all_mess, "Errore scrittura file: %s", chiamate_name_dest);
     return 1;
  }
  //----------------------------------
  // Ciclo Scrittura file CHIAMATE.DAT
  //----------------------------------
  for( i=0; i<=MAXCHIAMATE; i++){
     len= write( handle, ch[i], sizeof( CHIAMATE ));
     if (len != sizeof( CHIAMATE ))  err = 1;
     if(err){
         memset( ch[i], 0, sizeof( CHIAMATE ));
         sprintf(all_mess, "errore scrittura del record %d del file %s...", i, chiamate_name_dest);
         break;
     }
  }
  close( handle );
  return err;
}

// ------------------------
//    num_record()
// ------------------------
// Restituisce il numero di Record registrati, viene fatto un controllo
// sul numero della macchina (se diverso da zero)
//
int process::num_record_chiamate()
/**********************************/
{
  int numero_rec=0;
  int i;

  for( i=0; i<MAXCHIAMATE; i++){
     if( ch[i]->Num >0 ) numero_rec++;
  }

  return numero_rec;
}

// ------------------------
//    cerca_record()
// ------------------------
// Cerca il record memorizzato in memoria e lo copia nella struttura
// ch, restituisce errore in caso di superamento dei limiti o
// record non inizializzato
//
int process::cerca_record_chiamate(struct CHIAMATE *mc, int numero)
/**********************************/
{
  if( numero > MAXCHIAMATE ) return 1;
  if( ch[ numero ]->Num ==0      ) return 1;

  memcpy( mc, ch[ numero ], sizeof( CHIAMATE ) );

  return 0;
}


// -----------------------------------------
//       VerificaTipoDeposito()
// -----------------------------------------
//  Restituisce  Posizione( dati magazzino ), Piano
//
//  Posizione :   99 -> nessuna posizione disponibile rispetto al lato di ingresso
//                 0 -> Deposito su 1° Posizione
//                 1 -> Deposito su 2° Posizione
//                 2 -> Deposito su 3° Posizione
//                 3 -> Deposito su 4° Posizione
//
//  Posizione TGV: 0 -> nessuna posizione disponibile rispetto al lato di ingresso
//                 1 -> Deposito su 1° Posizione
//                 2 -> Deposito su 2° Posizione
//                 3 -> Deposito su 3° Posizione
//                 4 -> Deposito su 4° Posizione
//
int process::VerificaTipoDeposito(bool lato_uscita, short int num_mac, short int num_stz, short int *Posizione, short int *PosizioneTgv, char *all_mess)
/*************************************************************************/
{
  short int i;
  bool deposito_su_out;
  short int pos_trovata;

  *Posizione=99;
  *PosizioneTgv=0;
  pos_trovata=99;
  strcpy(all_mess, "");
  //-----------------------------------------------
  // Verifica condizioni Generali
  //-----------------------------------------------
  sprintf( all_mess, "Machine %d NOT ok", num_mac );
  if( ch[num_mac]->Num<1                                                     ) return 1;
  if( ch[num_mac]->Num>TOTCHIAMATE                                           ) return 1;
  if( ch[num_mac]->Staz[num_stz].PuntoIng<1                                  ) return 1;
  if( ch[num_mac]->Staz[num_stz].PuntoIng>TOTPUNTI                           ) return 1;
  if( ch[num_mac]->Staz[num_stz].PuntoOut<1                                  ) return 1;
  if( ch[num_mac]->Staz[num_stz].PuntoOut>TOTPUNTI                           ) return 1;
  if( ch[num_mac]->Staz[num_stz].Npt==0                                      ) return 1;
  if( ch[num_mac]->Staz[num_stz].NumCestoni>=ch[num_mac]->Staz[num_stz].Npt  ) return 1;

  //-----------------------------------------------
  // Determino il tipo di riempimento del magazzino
  // Impossibile deposito in uscita su macch LIFO
  //-----------------------------------------------
  deposito_su_out = lato_uscita;
  if(ch[num_mac]->Staz[num_stz].PuntoIng==ch[num_mac]->Staz[num_stz].PuntoOut) deposito_su_out=false;

  //-----------------------------------------------
  // STAZIONE GIA' OCCUPATA
  // Verifica Prima o Ultima Posizione Occupata
  // a seconda del lato da cui si vuole depositare
  //-----------------------------------------------
  if(ch[num_mac]->Staz[num_stz].NumCestoni>0){
     if(deposito_su_out==false){
        for(i=0; i<MAXPOSTISTAZ; i++){
           if(ch[num_mac]->Staz[num_stz].Pos[i].PresCEST_B==LIBERA) continue;
           pos_trovata=i-1;
           break;
        }
     }
     else{
        for(i=ch[num_mac]->Staz[num_stz].Npt-1; i>0; i--){
           if(ch[num_mac]->Staz[num_stz].Pos[i].PresCEST_B==LIBERA) continue;
           pos_trovata=i-1;
           break;
        }
     }
  }
  //-----------------------------------------------
  // STAZIONE COMPLETAMENTE VUOTA
  // La posizione di deposito coinciderà con la
  // prima o l'ultima a senconda del lato di
  // ingresso
  //-----------------------------------------------
  else{
     if(deposito_su_out==false) pos_trovata = ch[num_mac]->Staz[num_stz].Npt-1;
     else                       pos_trovata = 0;
  }

  //-------------------------
  // Posizione NON trovata
  //-------------------------
  if( pos_trovata==99 ){
     *Posizione    = 99;
     *PosizioneTgv = 0;
     return 1;
  }
  //-------------------------
  // Posizione TROVATA
  //-------------------------
  *Posizione    = (short int)(pos_trovata);
  *PosizioneTgv = (short int)(pos_trovata+1);
  if(deposito_su_out==true) *PosizioneTgv = ch[num_mac]->Staz[num_stz].Npt-pos_trovata;

  return 0;
}


// -----------------------------------------
//       VerificaTipoPrelievo()
// -----------------------------------------
//  Restituisce  Posizione( dati magazzino ), Piano, Posizione TGV
//
//  Posizione :  99 -> Nessun prelievo possibile
//                0 -> Deposito su 1° Posizione
//                1 -> Deposito su 2° Posizione
//                2 -> Deposito su 3° Posizione
//                3 -> Deposito su 4° Posizione
//                ...
//  PosizioneTgv: 0 -> nessuna posizione disponibile rispetto al lato di ingresso
//                1 -> Deposito su 1° Posizione
//                2 -> Deposito su 2° Posizione
//                3 -> Deposito su 3° Posizione
//                4 -> Deposito su 4° Posizione
//                ...
//
int process::VerificaTipoPrelievo(bool lato_ingresso, short int num_mac, short int num_stz, short int *Posizione, short int *PosizioneTgv, char *all_mess)
/*************************************************************************/
{
  short int i;
  bool  prelievo_da_ing;
  short int pos_trovata;

  *Posizione=99;
  *PosizioneTgv=0;
  pos_trovata=99;
  strcpy(all_mess, "");
  //---------------------------------
  // Verifica condizioni Generali
  //---------------------------------
  sprintf( all_mess, "Machine %d NOT ok", num_mac );
  if( ch[num_mac]->Num<1                           ) return 1;
  if( ch[num_mac]->Num>TOTCHIAMATE                 ) return 1;
  if( ch[num_mac]->Staz[num_stz].PuntoIng<1        ) return 1;
  if( ch[num_mac]->Staz[num_stz].PuntoIng>TOTPUNTI ) return 1;
  if( ch[num_mac]->Staz[num_stz].PuntoOut<1        ) return 1;
  if( ch[num_mac]->Staz[num_stz].PuntoOut>TOTPUNTI ) return 1;
  if( ch[num_mac]->Staz[num_stz].Npt==0            ) return 1;
  if( ch[num_mac]->Staz[num_stz].NumCestoni==0     ) return 1;

  //-----------------------------------------------
  // TIPO DI PRELIEVO IN BASE AL LATO
  //-----------------------------------------------
  prelievo_da_ing=false;
  if(ch[num_mac]->Staz[num_stz].PuntoIng==ch[num_mac]->Staz[num_stz].PuntoOut) prelievo_da_ing=true;
  if(lato_ingresso==true                                                     ) prelievo_da_ing=true;

  if(prelievo_da_ing==true){
     for(i=0; i<MAXPOSTISTAZ; i++){
        if(ch[num_mac]->Staz[num_stz].Pos[i].PresCEST_B==LIBERA) continue;
        pos_trovata=i;
        break;
     }
  }
  else{
     for(i=MAXPOSTISTAZ-1; i>=0; i--){
        if(ch[num_mac]->Staz[num_stz].Pos[i].PresCEST_B==LIBERA) continue;
        pos_trovata=i;
        break;
     }
  }

  //-------------------------
  // controllo range ammesso
  //-------------------------
  if(pos_trovata>ch[num_mac]->Staz[num_stz].Npt) pos_trovata=99;
  if(pos_trovata<0                             ) pos_trovata=99;

  //-------------------------
  // Posizione NON trovata
  //-------------------------
  if( pos_trovata==99 ){
     *Posizione    = 99;
     *PosizioneTgv = 0;
     return 1;
  }
  //-------------------------
  // Posizione TROVATA
  //-------------------------
  *Posizione    = (short int)(pos_trovata);
  *PosizioneTgv = (short int)(pos_trovata+1);
  if(prelievo_da_ing==false) *PosizioneTgv = ch[num_mac]->Staz[num_stz].Npt-pos_trovata;

  return 0;
}



//=========== BAIE

// ------------------------------------
//   load_chiamate_baie()
// ------------------------------------
//  Funzione che legge i dati dalla macchina di carico
//
int process::load_chiamate_baie(char *all_mess)
//*****************************************
{
  int err=0;

  //-----------------------------------------------
  // Lettura stato baie
  //-----------------------------------------------
  err = load_chiamate_baie_IMPIANTI(all_mess);

  return err;
}

// ------------------------------------
//   write_chiamate_baie()
// ------------------------------------
//  Funzione che legge i dati dalla macchina di carico
//
int process::write_chiamate_baie(char *all_mess)
//*****************************************
{
  int err=0;

  //-----------------------------------------------
  // Scrittura stato baie
  //-----------------------------------------------
  err = write_chiamate_baie_IMPIANTI(all_mess);

  return err;
}

// ------------------------------------
//    setta_consensi_baie( )
// ------------------------------------
//
int process::setta_consensi_baie(char *all_mess)
// ********************************************************
{

  WORD err=0;
  int  x1, x2, y1, y2;
  int  count, distX, distY;
  bool Ingombro;
  bool OkIngresso;
  bool RichiestaIngresso;
  bool forche_fuori_ing;
  short int i, Idx, NumAgv, Baia, Dest, Punto, PuntoPrecedente;
  double dist;
  bool SalvaBaia[MAXBAIE+2];


  strcpy( all_mess, "" );
  memset(&SalvaBaia, false, sizeof(SalvaBaia));
  //-----------------------------------------------------
  // A) Verifica se c'è un AGV in ingombro alla macchina
  //-----------------------------------------------------
  for(Baia=1; Baia<=MAXBAIE; Baia++){
     if(ba[Baia]->Num<1         ) continue;
     if(ba[Baia]->Num>TOTBAIE   ) continue;
     if(ba[Baia]->Punto<1       ) continue;
     if(ba[Baia]->Punto>TOTPUNTI) continue;
     //-------------------------------------------------------------
     // Memorizzo se baia in ok ingresso
     //-------------------------------------------------------------
     Punto             = ba[Baia]->Punto;
     Ingombro          = false;
     RichiestaIngresso = false;
     OkIngresso        = ba[Baia]->OkIng;
     //-------------------------------------------------------------
     // PUNTI AUSILIARI BAIA
     // Verifico se esiste un TGV destinato a un punto AUX
     // e in quel caso,
     //-------------------------------------------------------------
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        for(i=0; i<5; i++){
           if(ba[Baia]->PuntoAux[i]<1       ) continue;
           if(ba[Baia]->PuntoAux[i]>TOTPUNTI) continue;
           //-------------------------------------------------------
           // Memorizzo PuntoAux come destinazine TGV se impegnato
           // o destinazione missione
           //-------------------------------------------------------
           if(AGV[NumAgv]->stato.pos==ba[Baia]->PuntoAux[i] || AGV[NumAgv]->mission.pend==ba[Baia]->PuntoAux[i]){
              Punto = ba[Baia]->PuntoAux[i];
           }
        }
     }
     //-------------------------------------------------------------
     // Verifica se è sul nodo stazione
     //-------------------------------------------------------------
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        if(AGV[NumAgv]->stato.pos!=Punto) continue;
        Ingombro=true;
        AGV[ NumAgv ]->mission.BaiaInIngombro=0;
        RichiestaIngresso=true;
        break;
     }
     //-------------------------------------------------------------
     // Non più su nodo macchina
     //-------------------------------------------------------------
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        if(AGV[NumAgv]->mission.BaiaInIngombro!=Punto) continue;
        if(N.nodo_busy[Punto]==NumAgv                ) continue;
        AGV[ NumAgv ]->mission.BaiaInIngombro=0;
     }
     //-------------------------------------------------------------
     // Verifica se AGV in transito verso INGOMBRO STAZ MACCHINA
     //-------------------------------------------------------------
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        if(AGV[ NumAgv ]->mission.BaiaInIngombro!=Punto) continue;
        Ingombro=true;
        RichiestaIngresso=true;
        break;
     }
     //-------------------------------------------------------------
     // Considero fuori ingombro la navetta quando è sul primo nodo
     // a una distanza superiore ai 250cm dalla stazione di start.
     //-------------------------------------------------------------
     for( NumAgv=1; NumAgv<=MAXAGV; NumAgv++ ){
        if(AGV[ NumAgv ]->stato.pos==Punto       ) continue;
        if(AGV[ NumAgv ]->mission.pstart!=Punto  ) continue;
        //----------------------------------
        // Recupero coordinate
        //----------------------------------
        x1 = Punti[AGV[ NumAgv ]->stato.pos][0];
        y1 = Punti[AGV[ NumAgv ]->stato.pos][1];
        x2 = Punti[Punto][0];
        y2 = Punti[Punto][1];
        //----------------------------------
        // Se i nodi si trovano in retta la
        // distanza viene calcolata con il
        // teorema di pitagora.
        //----------------------------------
        distX = abs(x1-x2);
        distY = abs(y1-y2);
        //----------------------------------
        // Punti troppo lontani
        //----------------------------------
        if(distX>30000 || distX>30000) continue;
        //----------------------------------
        // Calcolo distanza dalla stazione
        //----------------------------------
        dist = (double)(sqrt((distX*distX)+(distY*distY)));
        //----------------------------------------------
        // Se la distanza<250 sono ancora
        // in ingombro
        // Se definita costante, altrimenti default 200
        //----------------------------------------------
        if(dist<DIST_INGOMBRO_BAIE) Ingombro=true;
        break;
     }
     //-------------------------------------------------------------
     // Verifica se è su nodo precedente in richiesta di accesso
     //-------------------------------------------------------------
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        //---------------------------------------------------------
        // RICHIESTA INGRESSO CON BIT ATTESA DA PLC TGV
        //---------------------------------------------------------
        if(AGV[NumAgv]->mission.pend==Punto && N.nodo_busy[Punto]==NumAgv){
           if(AGV[ NumAgv ]->stato.s.bit.inattesa){
              //----------------------------------------------
              // Memorizza che l'AGV fa richiesta di ingresso
              //----------------------------------------------
              RichiestaIngresso=true;
              break;
           }
        }
        //---------------------------------------------------------
        // RICHIESTA INGRESSO CON PINT NODO PRECEDENTE DA PC
        //---------------------------------------------------------
        if(AbilitaPIntPerConsensiMacchina==true && AGV[NumAgv]->mission.pend==Punto){
           //--------------------------------------------
           // Verifico distanza da destinazione
           //--------------------------------------------
           for(i=0; i<MAXPERCORSI; i++){
              if(AGV[NumAgv]->mission.punto[i]==0                     ) break;
              if(AGV[NumAgv]->mission.punto[i]==AGV[NumAgv]->stato.pos) break;
           }
           count=0;
           PuntoPrecedente=0;
           for(;i<MAXPERCORSI; i++){
              if(AGV[NumAgv]->mission.punto[i]==0) break;
              //----------------------------------------------
              // Raggiune la destinazione --> skip!
              //----------------------------------------------
              if(AGV[NumAgv]->mission.punto[i]==AGV[ NumAgv ]->mission.pend && AGV[NumAgv]->mission.punto[i+1]==0) break;
              //----------------------------------------------
              // Memorizzo punto precedente a destinazione
              //----------------------------------------------
              if(AGV[ NumAgv ]->mission.punto[i+1]==AGV[ NumAgv ]->mission.pend){
                 PuntoPrecedente = AGV[ NumAgv ]->mission.punto[i];
              }
              //----------------------------------------------
              // Conteggio nodi mancanti a destinazione
              //----------------------------------------------
              count++;
           }
           //-------------------------------------------------------------------------
           // Se TGV su nodo precedente o a una distanza <=ANTICIPO_RICH_INGRESSO_MAC
           // dalla destinazione scatena la richiesta ingresso
           //-------------------------------------------------------------------------
           if(AGV[NumAgv]->mission.PIntForzato==0){
              if(AGV[NumAgv]->stato.pos==PuntoPrecedente || count<=ANTICIPO_RICH_INGRESSO_MAC){
                 //----------------------------------------------
                 // Memorizza che l'AGV fa richiesta di ingresso
                 //----------------------------------------------
                 RichiestaIngresso=true;
                 //----------------------------------------------
                 // IMPORTANTE: Se il TGV ha già occupato
                 // la destinazione il PINT non è più forzabile
                 // sul nodo precedente, quindi è necessario
                 // alzare anche l'ingombro
                 //----------------------------------------------
                 if(N.nodo_busy[Punto]==NumAgv){
                    Ingombro=true;
                    AGV[NumAgv]->mission.BaiaInIngombro=Punto;
                 }
                 break;
              }
           }
        }
     }
     //-------------------------------------------------------------
     // Verifica se è sul nodo precedente alla stazione
     //-------------------------------------------------------------
     if( OkIngresso ){
        for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
           for(i=1; i<MAXPERCORSI; i++){                         // IMPORTANTE: scarto il nodo di start missione perchè il controllo
              if(AGV[NumAgv]->mission.punto[i]==0     ) break;   // prevede di recuperare il punto_precedente con indice i-1.
              if(AGV[NumAgv]->mission.punto[i]==Punto ) break;
           }
           if(AGV[ NumAgv ]->mission.punto[i]==Punto){
              PuntoPrecedente=AGV[ NumAgv ]->mission.punto[i-1];
              if(AGV[NumAgv]->stato.pos!=PuntoPrecedente)  continue;
              //----------------------------------------------
              // Memorizza che l'AGV sta entrando in ingombro
              //----------------------------------------------
              Ingombro=true;
              AGV[NumAgv]->mission.BaiaInIngombro=Punto;
              RichiestaIngresso=true;
              break;
           }
        }
     }
     //-------------------------------------------------------------
     // Verifica se è su nodo precedente e sta entrando su stazione
     //-------------------------------------------------------------
     if( OkIngresso ){
        for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
           if(N.nodo_busy[Punto]!=NumAgv           ) continue;
           if(!AGV[ NumAgv ]->stato.s.bit.inattesa ) continue;
           //----------------------------------------------
           // Memorizza che l'AGV sta entrando in ingombro
           //----------------------------------------------
           Ingombro=true;
           AGV[NumAgv]->mission.BaiaInIngombro=Dest;
           RichiestaIngresso=true;
           break;
        }
     }
     //-----------------------------------------------
     // Verifico se c'è una variazione di stato
     //-----------------------------------------------
     if(ba[Baia]->RichIng==false && RichiestaIngresso==true ) SalvaBaia[Baia]=true;
     if(ba[Baia]->TgvIng==false  && Ingombro==true          ) SalvaBaia[Baia]=true;
     if(ba[Baia]->RichIng==true  && RichiestaIngresso==false) SalvaBaia[Baia]=true;
     if(ba[Baia]->TgvIng==true   && Ingombro==false         ) SalvaBaia[Baia]=true;
     //-----------------------------------------------
     // Compilazione consensi su struttura
     //-----------------------------------------------
     ba[Baia]->TgvIng  = Ingombro;
     ba[Baia]->RichIng = RichiestaIngresso;
     //-----------------------------------------------
     // E) Se Consenso Ok da PAL azzera DM BLOCCO AGV
     //-----------------------------------------------
     if(OkIngresso){
        for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
           if(!AGV[ NumAgv ]->stato.s.bit.inattesa) continue;
           if(AGV[ NumAgv ]->mission.pend!=Punto  ) continue;
           //-----------------------------------------
           // Comando al TGV di proseguire DM89=0
           //-----------------------------------------
           if(AGV[ NumAgv ]->TrasmettiResetBloccoIngAGV(all_mess)) return 1;
        }
     }
     //-----------------------------------------------
     // E1) Set/Reset Fine Operazione
     //-----------------------------------------------
     ba[Baia]->FineOp =0;
     if( ba[Baia]->NumCicliResetFineOpraz>NUM_CICLI_RESET_FINEOP) ba[Baia]->NumCicliResetFineOpraz=NUM_CICLI_RESET_FINEOP;
     if( ba[Baia]->NumCicliResetFineOpraz<0                     ) ba[Baia]->NumCicliResetFineOpraz=0;
     if( ba[Baia]->NumCicliResetFineOpraz>0){
        if( ba[Baia]->RichIng==false && ba[Baia]->TgvIng==false && ba[Baia]->Chiamata==false){
           ba[Baia]->FineOp=true;
           ba[Baia]->NumCicliResetFineOpraz--;
           //------------------------------------------------
           // Al raggiungimento del nr. di cicli per il
           // reset del Fine Operazione memorizzo anche il
           // salvataggio su file
           //------------------------------------------------
           if( ba[Baia]->NumCicliResetFineOpraz==0 ) SalvaBaia[Baia]=true;
        }
     }
  }
  //-----------------------------------------------
  // F) Setta Consenso al movimento (forche alte)
  //-----------------------------------------------
  for(Baia=1; Baia<=MAXBAIE; Baia++){
     if(ba[Baia]->Num<1      ) continue;
     if(ba[Baia]->Num>TOTBAIE) continue;
     //-----------------------------------------------------
     // PUNTI AUSILIARI BAIA
     // Verifico se esiste un TGV destinato a un punto AUX
     // e in quel caso,
     //-----------------------------------------------------
     Punto = ba[Baia]->Punto;
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        for(i=0; i<5; i++){
           if(ba[Baia]->PuntoAux[i]<1       ) continue;
           if(ba[Baia]->PuntoAux[i]>TOTPUNTI) continue;
           //-------------------------------------------------------
           // Memorizzo PuntoAux come destinazine TGV se impegnato
           // o destinazione missione
           //-------------------------------------------------------
           if(AGV[NumAgv]->stato.pos==ba[Baia]->PuntoAux[i] || AGV[NumAgv]->mission.pend==ba[Baia]->PuntoAux[i]){
              Punto = ba[Baia]->PuntoAux[i];
           }
        }
     }
     //-----------------------------------------------------
     // Verifica se è su nodo pronto per entrare
     //-----------------------------------------------------
     ba[Baia]->OkForcheAlte=false;
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        if(AGV[NumAgv]->stato.pos!=Punto) continue;
        //-------------------------------------------------------------------
        // GESTIONE DOPPIA FORCA
        // La condizione di forche fuori ingombro è legata a questa gestione
        // GestioneDoppiaForcaTGV==0 -> Forca singola
        // GestioneDoppiaForcaTGV==1 -> Forca doppia reale
        // GestioneDoppiaForcaTGV==2 -> Forca singola a doppio carico
        //-------------------------------------------------------------------
        forche_fuori_ing=false;
        if((GestioneDoppiaForcaTGV==0 || GestioneDoppiaForcaTGV==2) && AGV[NumAgv]->stato.s.bit.ForcaB_FuoriIng==true                    ) forche_fuori_ing=true;
        if( GestioneDoppiaForcaTGV==1 && AGV[NumAgv]->stato.s.bit.ForcaA_FuoriIng==true && AGV[NumAgv]->stato.s.bit.ForcaB_FuoriIng==true) forche_fuori_ing=true;
        if(forche_fuori_ing==true){
           ba[Baia]->OkForcheAlte=true;
           break;
        }
     }
  }

  //-----------------------------------------------
  // Z) Salva record baia modificati
  //-----------------------------------------------
  for(Baia=1; Baia<=MAXBAIE; Baia++){
     if(ba[Baia]->Num<1         ) continue;
     if(ba[Baia]->Num>TOTBAIE   ) continue;
     //----------------------------
     // Salva Record macchina
     //----------------------------
     if( SalvaBaia[Baia]==true){
        save_record_baie(Baia, all_mess);
     }
  }

  return err;
}

// ----------------------------------------------
//       read_baie()
// ----------------------------------------------
// Legge il record "ba" in base a "num_baia" passato come parametro
//
int process::read_baie(int num_baia, struct BAIE *bb, char *all_mess)
/***********************************************/
{
  int  stato=0;

  stato = cerca_record_baie(bb, num_baia );
  if(stato){
     //---------------------------------
     // record non trovato ! -> errore
     //---------------------------------
     sprintf(all_mess, "Error in read baia %d (%s)", num_baia, "read_baie()");
     return stato;
  }
  return 0;

}

// ----------------------------------------------
//       test_punto_presente_baia()
// ----------------------------------------------
// Verifica se  "punto" è + un nodo di una Macchina. Se si ritorna in
// "macchina" il numero della macchina trovato.
//
int  process::test_punto_presente_baia(char *all_mess, short int punto, short int *num_baia)
/**********************************************************************/
{
  bool trovato;
  int  n_rec, i, stato;

  strcpy( all_mess, "OK");
  //--------------------------------
  // Azzeramento variabili
  //--------------------------------
  trovato=false;
  *num_baia =0;

  //--------------------------------
  // Nodo non valido
  //--------------------------------
  if(punto<1 || punto>TOTPUNTI){
     sprintf(all_mess, "(0) Error node %d does not good for baie archive...", punto);
     return 1;
  }
  //--------------------------------
  //  cerca il primo record
  //--------------------------------
  for( n_rec=1; n_rec<=MAXBAIE; n_rec++){
     if( ba[ n_rec ]->Num==0 ) continue;
     if(n_rec>TOTBAIE) break;
     trovato = false;
     //--------------------------------------
     // Controllo nodo principale
     //--------------------------------------
     if(punto==ba[ n_rec ]->Punto){
        trovato   = true;
        *num_baia = ba[ n_rec ]->Num;
        break;
     }
     //--------------------------------------
     // Controllo anche nodi ausiliari
     //--------------------------------------
     for(i=0; i<5; i++){
        if(ba[ n_rec ]->PuntoAux[i]!=punto) continue;
        trovato   = true;
        *num_baia = ba[ n_rec ]->Num;
        break;
     }
     if(trovato==true) break;
  }
  if(*num_baia==0){
     sprintf(all_mess, "Error search node %d in baie archive...", punto);
     return 1;
  }

  return 0;
}


// ------------------------
//  load_file_baie()
// ------------------------
// Dal file BAIE.DAT compila i record in memoria precedentemente allocati
//
int process::load_file_baie(char *all_mess)
/**********************************/
{
  int  err=0;
  int  i, len;
  int  handle;

  strcpy( all_mess, "OK" );
  //------------------------------
  // Azzera records in memoria
  //------------------------------
  for( i=0; i<=MAXBAIE; i++){
     memset( ba[i], 0, sizeof( BAIE ));
  }
  //------------------------------
  // Apertura file BAIE.DAT
  //------------------------------
  //-------------------------------------------------
  // Controllo se il File pu• essere Letto aprendolo
  // in modalit… di Condivisione.
  //-------------------------------------------------
  handle = sopen( baie_name_sorg, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
      close( handle );
      sprintf(all_mess, MESS[112], baie_name_sorg);
      return 1;
  }
  //--------------------------------
  // Lettura file BAIE.DAT
  //--------------------------------
  for( i=0; i<=MAXBAIE; i++){
     len = read( handle, ba[i], sizeof( BAIE ) );
     if (len != sizeof( BAIE ))  err = 1;
     if(err){
        memset( ba[i], 0, sizeof( BAIE ));
        sprintf(all_mess, MESS[351], i, baie_name_sorg);
        break;
     }
  }
  close( handle );

  return err;
}


// ------------------------
//  save_record_baie()
// ------------------------
// Salva sul file BAIE.DAT i records di macchine in memoria
//
int process::save_record_baie(int numero, char *all_mess)
/**********************************/
{
  int  err=0;
  long curpos;
  int  len;
  unsigned long length;
  int  handle;

  strcpy( all_mess, "OK" );
  //------------------------------
  // Apertura file BAIE.DAT
  //------------------------------
  //-------------------------------------------------
  // Controllo se il File pu• essere Letto aprendolo
  // in modalit… di Condivisione.
  //-------------------------------------------------
  handle = sopen( baie_name_dest, O_RDWR | O_BINARY , SH_DENYNO );
  if( handle<0 ){
     close( handle );
     sprintf(all_mess, MESS[112], baie_name_dest);
     return 1;
  }
  //------------------------------
  // Controllo lunghezza file
  //------------------------------
  length=0;
  if(handle >-1) length = filelength(handle);
  if( length < ((long)(numero)* sizeof( BAIE ))){
     sprintf(all_mess, MESS[113], baie_name_dest);
     close(handle);
     return 1;
  }

  //--------------------------------------
  // Calcolo dell'OFFSET e posizionamento
  //--------------------------------------
  curpos = (long ) numero * sizeof( BAIE );
  if( lseek( handle, curpos, SEEK_SET )<1){  // pos. il puntatore a inzio record
     sprintf(all_mess, "errore puntamento al record %d del file %s...", numero, baie_name_dest);
     close( handle );
     return 1;
  }
  //-------------------------------
  // Scrittura Record LOCAZION.DAT
  //-------------------------------
  len= write( handle, ba[numero], sizeof( BAIE ));
  if(len != sizeof( BAIE )){
     err=1;
     sprintf(all_mess, "errore scrittura del record %d del file %s...", numero, baie_name_dest);
  }
  close(handle);

  return err;
}


// ------------------------
//    save_file_baie()
// ------------------------
// Salva sul file BAIE.DAT i records delle macchine in memoria
//
int process::save_file_baie(char *all_mess)
/**********************************/
{
  int err=0;
  int i, len;
  int handle;

  strcpy( all_mess, "OK" );
  //------------------------------
  // Apertura file BAIE.DAT
  //------------------------------
  //-------------------------------------------------
  // Controllo se il File pu• essere Letto aprendolo
  // in modalit… di Condivisione.
  //-------------------------------------------------
  handle = sopen( baie_name_dest, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
  if( handle<0 ){
     close( handle );
     sprintf(all_mess, "Errore scrittura file: %s", baie_name_dest);
     return 1;
  }
  //----------------------------------
  // Ciclo Scrittura file BAIE.DAT
  //----------------------------------
  for( i=0; i<=MAXBAIE; i++){
     len= write( handle, ba[i], sizeof( BAIE ));
     if (len != sizeof( BAIE ))  err = 1;
     if(err){
         memset( ba[i], 0, sizeof( BAIE ));
         sprintf(all_mess, "errore scrittura del record %d del file %s...", i, baie_name_dest);
         break;
     }
  }
  close( handle );
  return err;
}

// ------------------------
//    num_record_baie()
// ------------------------
// Restituisce il numero di Record registrati, viene fatto un controllo
// sul numero della macchina (se diverso da zero)
//
int process::num_record_baie()
/**********************************/
{
  int numero_rec=0;
  int i;

  for( i=0; i<MAXBAIE; i++){
     if( ba[i]->Num >0 ) numero_rec++;
  }

  return numero_rec;
}

// ------------------------
//    cerca_record()
// ------------------------
// Cerca il record memorizzato in memoria e lo copia nella struttura
// ch, restituisce errore in caso di superamento dei limiti o
// record non inizializzato
//
int process::cerca_record_baie(struct BAIE *bb, int numero)
/**********************************/
{
  if( numero > MAXBAIE      ) return 1;
  if( ba[ numero ]->Num==0  ) return 1;

  memcpy( bb, ba[ numero ], sizeof( BAIE ) );

  return 0;
}


//=========== GRUPPI BAIE


// ------------------------
//   load_file_gruppibaie()
// ------------------------
// Dal file GRUPPIBA.DAT compila i record in memoria precedentemente allocati
//
int process::load_file_gruppibaie(char *all_mess)
/**********************************/
{
  int i, err=0;
  int len, handle;

  strcpy(all_mess, "OK");
  memset(&GruppiBaie, 0, sizeof(GruppiBaie ));
  //-------------------------------------------------
  // Assegnazione diciture di default
  //-------------------------------------------------
  for(i=0; i<MAXGRUPPIIMP; i++){
     if(strcmp(DESCGRUPPI[i], "")==0) continue;
     strcpy(P.GruppiBaie[i].Nome, DESCGRUPPI[i]);
     P.GruppiBaie[i].Col = clLtGray;
  }
  //-------------------------------------------------
  // Controllo se il File pu• essere Letto aprendolo
  // in modalit… di Condivisione.
  //-------------------------------------------------
  handle = sopen( gruppibaie_name_sorg, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
      close( handle );
      sprintf(all_mess, MESS[112], gruppibaie_name_sorg);
      return 1;
  }
  //-------------------------------------------------
  // Ciclo lettura file GRUPPIBA.DAT
  //-------------------------------------------------
  len = read( handle, &GruppiBaie, sizeof( GruppiBaie ) );
  if(len != sizeof(GruppiBaie)) err=1;
  if(err){
     memset(&GruppiBaie, 0, sizeof( GruppiBaie ));
     sprintf(all_mess, MESS[112], gruppibaie_name_sorg);
  }
  close( handle );

  //-------------------------------------------------
  // FORZATURA NOMI DA FILE
  //-------------------------------------------------
  if(!err){
     for(i=0; i<MAXGRUPPIIMP; i++){
        if(strcmp(DESCGRUPPI[i], "")==0) continue;
        strcpy(P.GruppiBaie[i].Nome, DESCGRUPPI[i]);
     }
  }
  return err;
}


// ------------------------
//   save_file_gruppibaie()
// ------------------------
// Salva sul file PORTEIMP.DAT i records delle macchine in memoria
//
int process::save_file_gruppibaie(char *all_mess)
/**********************************/
{
  int err=0;
  int len, handle;

  //------------------------------
  // Apertura file PORTEIMP.DAT
  //------------------------------
  handle = sopen( gruppibaie_name_dest, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
  if( handle<0 ){
     close( handle );
     sprintf(all_mess, MESS[113], gruppibaie_name_dest);
     return 1;
  }
  //----------------------------------
  // Ciclo Scrittura file PORTEIMP.DAT
  //----------------------------------
  len= write( handle, &GruppiBaie, sizeof( GruppiBaie ));
  if (len != sizeof( GruppiBaie )){
     err = 1;
     memset(&GruppiBaie, 0, sizeof( GruppiBaie ));
     sprintf(all_mess, MESS[113], gruppibaie_name_dest);
  }
  close( handle );
  return err;
}


//=========== PORTE


// ------------------------
//   load_file_porteimp()
// ------------------------
// Dal file PORTEIMP.DAT compila i record in memoria precedentemente allocati
//
int load_file_porteimp(char *all_mess)
/**********************************/
{
  int err=0;
  int len, handle;

  strcpy(all_mess, "OK");
  memset(&PorteImp, 0, sizeof(PorteImp ));
  //-------------------------------------------------
  // Controllo se il File pu• essere Letto aprendolo
  // in modalit… di Condivisione.
  //-------------------------------------------------
  handle = sopen( porteimp_name_sorg, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
      close( handle );
      sprintf(all_mess, MESS[112], porteimp_name_sorg);
      return 1;
  }
  //--------------------------------
  // Ciclo lettura file PORTEIMP.DAT
  //--------------------------------
  len = read( handle, &PorteImp, sizeof( PorteImp ) );
  if(len != sizeof(PorteImp)) err=1;
  if(err){
     memset(&PorteImp, 0, sizeof( PorteImp ));
     sprintf(all_mess, MESS[112], porteimp_name_sorg);
  }
  close( handle );
  return err;
}


// ------------------------
//   save_file_porteimp()
// ------------------------
// Salva sul file PORTEIMP.DAT i records delle macchine in memoria
//
int save_file_porteimp(char *all_mess)
/**********************************/
{
  int err=0;
  int len, handle;

  //------------------------------
  // Apertura file PORTEIMP.DAT
  //------------------------------
  handle = sopen( porteimp_name_dest, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
  if( handle<0 ){
     close( handle );
     sprintf(all_mess, MESS[113], porteimp_name_dest);
     return 1;
  }
  //----------------------------------
  // Ciclo Scrittura file PORTEIMP.DAT
  //----------------------------------
  len= write( handle, &PorteImp, sizeof( PorteImp ));
  if (len != sizeof( PorteImp )){
     err = 1;
     memset(&PorteImp, 0, sizeof( PorteImp ));
     sprintf(all_mess, MESS[113], porteimp_name_dest);
  }
  close( handle );
  return err;
}

// ------------------------------
//    load_incendio_in_corso()
// ------------------------------
//  Legge da PLC di terra i segnali per lo scambio di consensi
//  tra TGV e macchine di carico/scarico salami
//
int process::load_incendio_in_corso(char *all_mess)
//*****************************************
{
  int  err=0;
  WORD dm[3];
  short int NumCanali      = 1;
  short int NumPlc         = 0;
  short int CanaleIniziale = 18000;

  //--------------------------------------------------
  // se break_plc allora SKIP !!!!
  //--------------------------------------------------
  if(break_Com[COM2]) return 0;
  //--------------------------------------------------
  // Comunicazione diretta con PLC macchine
  //--------------------------------------------------
  if(ComunicazioneDirettaPLC!=0) return 0;

  IncendioInCorso=false;
  //--------------------------------------------------
  // Lettura consensi da PLC di terra
  // DM 18000 da tutti  i PLC connessi
  //--------------------------------------------------
  err = OM.ld_canale(&dm[0], NumPlc, "RD", CanaleIniziale, (WORD)(NumCanali), all_mess);
  if(err){
     sprintf(all_mess, "Error in loading BURNING signals (PLC%02d)...", NumPlc);
     return 1;
  }
  //-------------------------------------
  // Basta un PLC che segnali l'incendio
  //-------------------------------------
  if(!err && dm[0]!=0) IncendioInCorso=true;

  return err;
}

// ------------------------------
//    load_consensi_porteimp()
// ------------------------------
//  Legge da PLC di terra i segnali per lo scambio di consensi
//  tra TGV e porte impianto
//
int load_consensi_porteimp(char *all_mess)
//*****************************************
{
  int  err=0;
  WORD dm[MAXPORTEIMP+1];
  static WORD old_dm[MAXPORTEIMP+1];
  short int i;
  short int NumCanali      = MAXPORTEIMP;
  short int NumPlc         = 0;
  short int CanaleIniziale = 11;

  //--------------------------------------------------
  // se break_plc allora SKIP !!!!
  //--------------------------------------------------
  if(break_Com[COM2]) return 0;
  //--------------------------------------------------
  // Comunicazione diretta con PLC macchine
  //--------------------------------------------------
  if(ComunicazioneDirettaPLC!=0) return 0;
  //--------------------------------------------------
  // Se il numero di canali da leggere è "0" perchè
  // non ci sono delle porte --> Skip!!
  //--------------------------------------------------
  if(NumCanali<1) return 0;
  //--------------------------------------------------
  // Lettura consensi da PLC di terra
  // DM 51 = porta 1
  // DM 52 = porta 2
  // .....
  //--------------------------------------------------
  memset(&dm , 0, sizeof(dm));
  err = OM.ld_canale(&dm[1], NumPlc, "RD", CanaleIniziale, (WORD)(NumCanali), all_mess);
  if(err){
     strcpy(all_mess, "Errore nella lettura DM scambio consensi con porte...");
     return 1;
  }
  //--------------------------------------------------
  // Aggiornamento Consensi:
  // nb:  DM X.0  - Consenso Ingresso
  //      DM X.1  - Richiesta Ingresso
  //      DM X.2  - Agv in ingombro
  //      DM X.3  - Agv in arrivo
  //      ...
  //      DM X.13 - Porta in blocco
  //      DM X.14 - Apertura parziale
  //      DM X.15 - TgvConCarico
  //--------------------------------------------------
  for(i=1; i<=NumCanali; i++){
     PorteImp[i].OkIngresso        = TestBit((char *) &dm[i], 0);
     PorteImp[i].RichIngresso      = TestBit((char *) &dm[i], 1);
     PorteImp[i].TgvInIngombro     = TestBit((char *) &dm[i], 2);
     PorteImp[i].TgvInArrivo       = TestBit((char *) &dm[i], 3);
     PorteImp[i].PortaInBlocco     = TestBit((char *) &dm[i],13);
     PorteImp[i].AperturaParziale  = TestBit((char *) &dm[i],14);
     PorteImp[i].TgvConCarico      = TestBit((char *) &dm[i],15);
     //--------------------------------------------------------
     // GESTIONE ATTRAVERAMENTO MULETTI (FATER)
     // Se trovo una porta in Ok Accesso senza avere l'ingombro
     // ho richiesta da parte del TGV significa che è stata
     // aperta da un muletto e forzo la condizone di porta in
     // blocco.
     //
     // 30/06/2011 - Inserito segnale direttamente da PORTE
     //
     //--------------------------------------------------------
/*
     if(PorteImp[i].OkIngresso==true && PorteImp[i].TgvInIngombro==false && PorteImp[i].RichIngresso==false){
        PorteImp[i].PortaInBlocco=true;
     }
     if(PorteImp[i].OkIngresso==false){
        PorteImp[i].PortaInBlocco=false;
     }
*/     
  }
  //--------------------------------------------------------
  // Se c'è stata una variazione nei DM salvo anche su file
  //--------------------------------------------------------
  if(memcmp(&old_dm[0], &dm[0], sizeof(old_dm))!=0){
     err = save_file_porteimp(all_mess);
     //------------------------------------------
     // Salvataggio della nuova situazione
     //------------------------------------------
     memcpy(&old_dm[0], &dm[0], sizeof(old_dm));
  }
  return err;
}

// ------------------------------
//    write_consensi_porteimp()
// ------------------------------
//  Legge da PLC di terra i segnali per lo scambio di consensi
//  tra TGV e porte impianto
//
int write_consensi_porteimp(char *all_mess)
//*****************************************
{
  int  err=0;
  WORD dm[100];
  WORD NumCanali;
  WORD NumPlc;
  WORD CanaleIniziale;
  bool direzione_ok;
  bool nodo_porta_occupato;
  bool AlmenoUnNodoOccupato;
  bool OkTrasmissioneGenerico;
  bool interbloccata_in_apertura;
  short int old_indice;
  short int rot_su_nodo;
  short int indice_semaforo;
  short int indice_ingombro;
  short int succ_sotto_porta;
  short int AlmenoUnoInArrivo;
  short int InIngresso, AlmenoUnoInIngombro;
  short int NumAgv, i, j, k, z, Port, Port1, indice;
  short int count, count_nodi_porta;
  short int PortConAgvInArrivo[MAXPORTEIMP+1];
  short int PortConAgvInIngombro[MAXPORTEIMP+1];
  short int PortConAgvInRichiestaIngresso[MAXPORTEIMP+1];

  //--------------------------------------------------
  // se break_plc allora SKIP !!!!
  //--------------------------------------------------
//if(break_Com[COM2]) return 0;

  InIngresso=0;
  AlmenoUnoInArrivo=0;
  AlmenoUnoInIngombro=0;
  memset(&dm, 0, sizeof(dm));
  //-----------------------------------------------------------
  // Elaborazione dei consensi con PORTE
  //-----------------------------------------------------------
  memset(&PortConAgvInArrivo[0],            0, sizeof(PortConAgvInArrivo));
  memset(&PortConAgvInIngombro[0],          0, sizeof(PortConAgvInIngombro));
  memset(&PortConAgvInRichiestaIngresso[0], 0, sizeof(PortConAgvInRichiestaIngresso));
  //-----------------------------------------------------------
  // Resetta l'informazione di AGV in ingombro
  // se ESCLUSO o su un NODO
  //-----------------------------------------------------------
  for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
     if((AGV[NumAgv]->stato.s.bit.sunod && AGV[NumAgv]->stato.start==0) || AGV[NumAgv]->mission.Esclusione){
        memset(&AGV[NumAgv]->mission.PorInIngombro[0], false, sizeof(AGV[NumAgv]->mission.PorInIngombro));
     }
  }
  //-----------------------------------------------------------
  // Verifico se a tutti i TGV per cui ho memorizzato il
  // transito sotto a una porta, hanno ancora almeno un
  // nodo della porta occupato
  //-----------------------------------------------------------
  for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
     AlmenoUnNodoOccupato=false;
     for(Port=1; Port<=TOTPORTE; Port++){
        if(AGV[NumAgv]->mission.PorInIngombro[Port]==false) continue;
        //-----------------------------------------
        // Verifico se almeno un nodo è occupato
        //-----------------------------------------
        for(i=0; i<11; i++){
           if(PorteImp[Port].Punto[i]==0                  ) continue;
           if(N.nodo_busy[PorteImp[Port].Punto[i]]!=NumAgv) continue;
           //---------------------------------------------
           // Se il nodo che risulta occupato concide con
           // la posizione della navetta e con l'ultimo
           // nodo della missione è come se il TGV
           // si azzera la memoria di transito
           //---------------------------------------------
           if(PorteImp[Port].Punto[i]==AGV[NumAgv]->stato.pos && PorteImp[Port].Punto[i]==AGV[NumAgv]->mission.pend) continue;
           //---------------------------------------------
           // almeno un nodo occupato
           //---------------------------------------------
           AlmenoUnNodoOccupato=true;
           break;
        }
        //-----------------------------------------
        // Se nessun nodo è occupato abbasso la
        // memoria di TGV in transito
        //-----------------------------------------
        if(AlmenoUnNodoOccupato==false) AGV[NumAgv]->mission.PorInIngombro[Port]=false;
     }
  }
  //-----------------------------------------------------------
  // A) Verifica se c'è un AGV in richiesta ingresso PORT
  //-----------------------------------------------------------
  for(Port=1; Port<=MAXPORTEIMP; Port++){
     InIngresso=0;
     Port1 = PorteImp[Port].PortaInterbloccata;
     for(i=0; i<11; i++){
        if(PorteImp[Port].Punto[i]==0) continue;
        //-----------------------------------------------------------
        // 22/06/2011
        // Se la porta intebloccata con quella considerata risulta
        // già in richiesta ingresso --> skip
        // (IMPOSSIBILE APRIRE 2 PORTE INTERBLOCCATE INSIEME)
        //-----------------------------------------------------------
        if(Port1>0 && Port1<=MAXPORTEIMP){
           interbloccata_in_apertura=false;
           for(j=0; j<11; j++){
              if(PorteImp[Port1].Punto[j]==0   ) continue;
              if(!PorteImp[Port1].RichIngresso ) continue;
              interbloccata_in_apertura=true;
              break;
           }
           if(interbloccata_in_apertura==true) continue;
        }
        //-----------------------------------------------------------
        // Verifico se ho ogià dei TGV in attraversamento
        //-----------------------------------------------------------
        AlmenoUnoInIngombro=0;
        for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
           if(AGV[NumAgv]->mission.PorInIngombro[Port]==false) continue;
           AlmenoUnoInIngombro=NumAgv;
           break;
        }
        //-----------------------------------------------------------
        // RICHIESTA INGRESSO SU PORTE CHIUSE
        // Se la porta risulta già aperta non eseguo la
        // richiesta ingresso
        //-----------------------------------------------------------
        if(PorteImp[Port].OkIngresso && AlmenoUnoInIngombro==0) continue;
        //-----------------------------------------------------------
        // Verifica se AGV in richiesta di ingresso
        //-----------------------------------------------------------
        for( NumAgv=1; NumAgv<=MAXAGV; NumAgv++ ){
           //-----------------------------------------------------
           // Se il TGV ha la missione in corso
           //-----------------------------------------------------
           if(!AGV[NumAgv]->test_mission_on()) continue;
           //-----------------------------------------------------
           // Conto a quanti nodi di distanza è il TGV dal nodo
           // intermedio
           //-----------------------------------------------------
           indice=count=0;
           direzione_ok=false;
           nodo_porta_occupato=false;
           for(k=0; k<MAXPERCORSI; k++){
              if(AGV[NumAgv]->mission.punto[k]==0) break;
              if(AGV[NumAgv]->mission.punto[k]!=AGV[NumAgv]->stato.pos) continue;
              indice=k;
              break;
           }
           //-----------------------------------------------------
           // Se il TGV è già su uno dei nodi o al max a 2 nodi
           // di distanza posso passare sotto la porta
           //-----------------------------------------------------
           for(k=indice; k<MAXPERCORSI; k++){
              if(AGV[NumAgv]->mission.punto[k]==0) break;
              //------------------------------------------------------
              // Controllo se il percorso TGV passa per una porta
              //------------------------------------------------------
              if(AGV[NumAgv]->mission.punto[k]==PorteImp[Port].Punto[i]){
                 //------------------------------------------------------
                 // 29/06/2011 Verifico se il nodo della porta è già
                 // occupato
                 //------------------------------------------------------
                 if(N.nodo_busy[PorteImp[Port].Punto[i]]==NumAgv) nodo_porta_occupato=true;
                 //------------------------------------------------------
                 // Verifico se il TGV nel nodo considerato si muove
                 // nella direzione di ingombro alla porta
                 //------------------------------------------------------
                 rot_su_nodo = AGV[NumAgv]->rot_standard(k);
                 if(AGV[NumAgv]->mission.dati_perc[k].dir==1){
                    if(rot_su_nodo==0 && PorteImp[Port].DirDown==true ) direzione_ok=true;
                    if(rot_su_nodo==1 && PorteImp[Port].DirRight==true) direzione_ok=true;
                    if(rot_su_nodo==2 && PorteImp[Port].DirUp==true   ) direzione_ok=true;
                    if(rot_su_nodo==3 && PorteImp[Port].DirLeft==true ) direzione_ok=true;
                 }
                 if(AGV[NumAgv]->mission.dati_perc[k].dir==0){
                    if(rot_su_nodo==0 && PorteImp[Port].DirUp==true   ) direzione_ok=true;
                    if(rot_su_nodo==1 && PorteImp[Port].DirLeft==true ) direzione_ok=true;
                    if(rot_su_nodo==2 && PorteImp[Port].DirDown==true ) direzione_ok=true;
                    if(rot_su_nodo==3 && PorteImp[Port].DirRight==true) direzione_ok=true;
                 }
                 //------------------------------------------------------
                 // Verifico se il TGV è in attraversamento per
                 // considerare se un dato nodo sarà in ingombro
                 //------------------------------------------------------
                 succ_sotto_porta=0;
                 for(z=0; z<11; z++){
                    if(PorteImp[Port].Punto[z]==0                              ) continue;
                    if(AGV[NumAgv]->mission.punto[k+1]!=PorteImp[Port].Punto[z]) continue;
                    succ_sotto_porta = PorteImp[Port].Punto[z];
                    break;
                 }
                 //------------------------------------------------------
                 // 05/08/2011
                 // Se non esiste il successivo sotto porta, mi
                 // assicuro che la porta abbia più nodi assegnati
                 //------------------------------------------------------
                 if(succ_sotto_porta==0){
                    count_nodi_porta=0;
                    for(z=0; z<11; z++){
                       if(PorteImp[Port].Punto[z]==0) continue;
                       count_nodi_porta++;
                    }
                    if(count_nodi_porta<2) succ_sotto_porta=PorteImp[Port].Punto[i];
                 }
                 //------------------------------------------------------
                 // TGV NON IN ATTRAVERSAMENTO
                 //------------------------------------------------------
                 if(succ_sotto_porta==0) direzione_ok=false;
                 //------------------------------------------------
                 // Nodo nella direzione di movimento del TGV
                 //------------------------------------------------
                 if(direzione_ok) break;
              }
              count++;
           }
           if(AGV[NumAgv]->mission.punto[k]==0       ) continue;
           //-----------------------------------------------------
           // Anticipo richiesta porte
           // NOTA: anche se sono molto distante, ma risultano
           //       già occupati almeno 2 nodi della porta
           //       alzo comunque la richiesta
           //-----------------------------------------------------
           if(count>PorteImp[Port].AnticipoRichiesta && AlmenoUnoInIngombro==0) continue;
           //-----------------------------------------------------
           // 29/06/2011 PORTE INTERBLOCCATE
           // Se la porta è interbloccata, anticipo la richiesta
           // solo se almeno un nodo della porta è già occupato
           //-----------------------------------------------------
           if(Port1>0 && Port1<=MAXPORTEIMP && nodo_porta_occupato==false) continue;
           //-----------------------------------------------------
           // Abilito la richiesta ingresso con il TGV fermo
           //-----------------------------------------------------
         //if(!AGV[NumAgv]->stato.s.bit.inattesa     ) continue;
           InIngresso=NumAgv;
           break;
        }
        if(InIngresso!=0) break;
     }
     PortConAgvInRichiestaIngresso[ Port ] = InIngresso;
  };

  //-----------------------------------------------
  // C) Ingresso AGV in ingombro alla MAC
  //-----------------------------------------------
  for(Port=1; Port<=MAXPORTEIMP; Port++){
     //-----------------------------------------------------
     // Verifico se ho ogià dei TGV in attraversamento
     //-----------------------------------------------------
     AlmenoUnoInIngombro=0;
     for( NumAgv=1; NumAgv<=MAXAGV; NumAgv++ ){
        if(AGV[NumAgv]->mission.PorInIngombro[Port]==false) continue;
        AlmenoUnoInIngombro=NumAgv;
        break;
     }
     //-----------------------------------------------------------
     // Se la porta non è in OK INGRESSO non forzo l'ingombro
     //
     // NOTA: Se il TGV ha già cominciato l'attraversamento
     //       tengo alto l'ingombro anche se conde il consenso
     //-----------------------------------------------------------
     if(!PorteImp[Port].OkIngresso && AlmenoUnoInIngombro==0) continue;
     //-----------------------------------------------------------
     // B1) Verifica se c'è già un AGV in ingombro a PORTA
     //-----------------------------------------------------------
     AlmenoUnoInIngombro=0;
     for(i=0; i<11; i++){
        if(PorteImp[Port].Punto[i]==0) continue;
        //-----------------------------------------------------------
        // TGV sul nodo per apertura della porta
        //-----------------------------------------------------------
        for( NumAgv=1; NumAgv<=MAXAGV; NumAgv++ ){
           //-------------------------------------------------
           // Verifico che il TGV abbia occupato almeno uno
           // dei nodi di apertura porta
           //-------------------------------------------------
           if(N.nodo_busy[PorteImp[Port].Punto[i]]!=NumAgv) continue;
           //-------------------------------------------------
           // Considero il TGV in ingombro se non è in attesa
           //-------------------------------------------------
         //if(AGV[NumAgv]->stato.s.bit.inattesa           ) continue;
           //-------------------------------------------------
           // Recupero l'indice di missione del TGV a cui
           // si trova il nodo della porta
           //-------------------------------------------------
           indice=0;
           for(k=0; k<MAXPERCORSI; k++){
              if(AGV[NumAgv]->mission.punto[k]==0) break;
              if(AGV[NumAgv]->mission.punto[k]!=PorteImp[Port].Punto[i]) continue;
              indice=k;
              break;
           }
           //-------------------------------------------------
           // Verifico se sul punto ho la rotazione di
           // ingombro
           //-------------------------------------------------
           direzione_ok=false;
           if(AGV[NumAgv]->stato.pos==PorteImp[Port].Punto[i]){
              if(PorteImp[Port].DirUp==true || PorteImp[Port].DirDown==true){
                 if(AGV[NumAgv]->stato.s.bit.grd0  ) direzione_ok=true;
                 if(AGV[NumAgv]->stato.s.bit.grd180) direzione_ok=true;
              }
              if(PorteImp[Port].DirRight==true || PorteImp[Port].DirLeft==true){
                 if(AGV[NumAgv]->stato.s.bit.grd90 ) direzione_ok=true;
                 if(AGV[NumAgv]->stato.s.bit.grd270) direzione_ok=true;
              }
           }
           else{
              for(k=indice; k<MAXPERCORSI; k++){
                 if(AGV[NumAgv]->mission.punto[k]==0                      ) break;
                 if(AGV[NumAgv]->mission.punto[k]!=PorteImp[Port].Punto[i]) continue;
                 //-------------------------------------------------
                 // Verifico la rotazione sul punto
                 //-------------------------------------------------
                 rot_su_nodo = AGV[NumAgv]->rot_standard(k);
                 if(PorteImp[Port].DirUp==true || PorteImp[Port].DirDown==true){
                    if(rot_su_nodo==0) direzione_ok=true;
                    if(rot_su_nodo==2) direzione_ok=true;
                 }
                 if(PorteImp[Port].DirRight==true || PorteImp[Port].DirLeft==true){
                    if(rot_su_nodo==1) direzione_ok=true;
                    if(rot_su_nodo==3) direzione_ok=true;
                 }
              }
           }
           //------------------------------------------------
           // PORTE CORRIDOIO
           // (ALZO L'INGOMBRO SOLO IN ATTRAVERSAMENTO)
           // Solo sulle porte di corridoio controllo che
           // anche il nodo successivo della missione sia
           // in ingombro alla porta altrimenti NON forzo
           // il punto intermedio
           //------------------------------------------------
           succ_sotto_porta=0;
           if(AGV[NumAgv]->mission.punto[indice+1]!=0){
              //--------------------------------------
              // Runto successivo al nodo considerato
              //--------------------------------------
              for(z=0; z<11; z++){
                 if(PorteImp[Port].Punto[z]==0                                   ) continue;
                 if(AGV[NumAgv]->mission.punto[indice+1]!=PorteImp[Port].Punto[z]) continue;
                 succ_sotto_porta = PorteImp[Port].Punto[z];
                 break;
              }
              //------------------------------------------------------
              // 05/08/2011
              // Se non esiste il successivo sotto porta, mi
              // assicuro che la porta abbia più nodi assegnati
              //------------------------------------------------------
              if(succ_sotto_porta==0){
                 count_nodi_porta=0;
                 for(z=0; z<11; z++){
                    if(PorteImp[Port].Punto[z]==0) continue;
                    count_nodi_porta++;
                 }
                 if(count_nodi_porta<2) succ_sotto_porta=PorteImp[Port].Punto[i];
              }
              //------------------------------------------------------
              // TGV non in attraversamento (TOLGO L'INGOMBRO)
              //------------------------------------------------------
              if(succ_sotto_porta==0){
                 direzione_ok=false;
                 //---------------------------------------------
                 // Per le porte di corridoio azzero il flag di
                 // transito sotto le porte non appena il
                 // punto successio è fuori ingombro
                 //---------------------------------------------
                 AGV[NumAgv]->mission.PorInIngombro[Port]=false;
              }
           }
           //-------------------------------------------------
           // Il TGV viene considerato in ingombro solo
           // se la direzione di movimento sul nodo è
           // quella impostata da Editor nodi
           //-------------------------------------------------
           if(direzione_ok==false) continue;
           //-------------------------------------------------
           // TGV in ingombro
           //-------------------------------------------------
           AlmenoUnoInIngombro=NumAgv;
           //-------------------------------------------------
           // Memorizzo che è stato alzato l'ingombro
           //-------------------------------------------------
           AGV[NumAgv]->mission.PorInIngombro[Port]=true;
           break;
        }
     }
     //-----------------------------------------------------------
     // C3) Se ho il TGV in transito verso la porta setto anche
     //     il segnale di INGOMBRO
     //-----------------------------------------------------------
     for( NumAgv=1; NumAgv<=MAXAGV; NumAgv++ ){
        if(AGV[NumAgv]->mission.PorInIngombro[Port]==true){
           AlmenoUnoInIngombro=NumAgv;
           break;
        }
     }
     //-----------------------------------------------------------
     // Memorizzo il TGV trovato in ingombro alla porta
     //-----------------------------------------------------------
     PortConAgvInIngombro[Port] = AlmenoUnoInIngombro;
  };

  //----------------------------------------------------------
  // D) TGV IN ARRIVO SU PORTA
  // Controllo se un TGV ha nodi per gestione semaforo
  // lungo il suo tragitto verso la porta, altrimenti setto
  // il bit di TGV in arrivo
  //----------------------------------------------------------
  for(Port=1; Port<=MAXPORTEIMP; Port++){
     //-----------------------------------------------------------
     // Se la porta non è in OK INGRESSO non forzo l'ingmbro
     //-----------------------------------------------------------
     if(PorteImp[Port].OkIngresso   ) continue;
     //-----------------------------------------------------------
     // Verifico se ho una porta con TGV in arrivo
     //-----------------------------------------------------------
     AlmenoUnoInArrivo=PortConAgvInIngombro[Port];
     for(i=1; i<=MAXAGV; i++){
        //---------------------------------------
        // TGV senza missione in corso --> skip!
        //---------------------------------------
        if(!AGV[i]->test_mission_on()    ) continue;
        //---------------------------------------
        // TGV già su porta --> skip!
        //---------------------------------------
        if(PortConAgvInArrivo[Port]==true) break;
        //---------------------------------------
        // Se il TGV ha già occupato fino a
        // destinazione --> skip!
        //---------------------------------------
        if(AGV[i]->mission.pend==AGV[i]->mission.pintend) break;
        //---------------------------------------
        // Recupero l'indice di missione dei
        // nodi solo prenotati
        //---------------------------------------
        indice=0;
        for(j=0; j<MAXPERCORSI; j++){
           if(AGV[i]->mission.punto_prenotato[j]==0                      ) break;
           if(AGV[i]->mission.punto_prenotato[j]!=AGV[i]->mission.pintend) continue;
           indice=j;
           break;
        }
        //---------------------------------------
        // Scarto tutti i TGV che non devono
        // attraversare la porta considerata
        //---------------------------------------
        old_indice=9999;
        indice_ingombro=0;
        for(j=indice; j<MAXPERCORSI; j++){
           if(AGV[i]->mission.punto_prenotato[j+1]==0) break;
           //-------------------------------------------
           // Verifico se tra i nodi ancora da occupare
           // c'è un nodo di porta
           //-------------------------------------------
           for(k=0; k<11; k++){
              if(PorteImp[Port].Punto[k]==0                                  ) continue;
              if(PorteImp[Port].Punto[k]!=AGV[i]->mission.punto_prenotato[j] ) continue;
              if(j>old_indice                                                ) continue;
              //---------------------------------------------------
              // Per avere la condizione di attraversamento
              // almeno 2 nodi devono essere nodi in ingombro
              //---------------------------------------------------
              succ_sotto_porta=0;
              for(z=0; z<11; z++){
                 if(PorteImp[Port].Punto[z]==0                                    ) continue;
                 if(PorteImp[Port].Punto[z]!=AGV[i]->mission.punto_prenotato[j+1] ) continue;
                 succ_sotto_porta=PorteImp[Port].Punto[z];
                 break;
              }
              if(succ_sotto_porta==0) continue;
              //---------------------------------------------------
              // TGV IN ARRIVO SU PORTA
              //---------------------------------------------------
              indice_ingombro=j;
              old_indice=j;
           }
           if(indice_ingombro!=0) break;
        }
        //---------------------------------------
        // Il TGV non deve attraversare la porta
        //---------------------------------------
        if(indice_ingombro==0) continue;
        //---------------------------------------
        // Controllo se tra i nodi ancora da
        // occupare compreso quello dove mi
        // trovo esistono dei nodi di semaforo
        //---------------------------------------
        old_indice=9999;
        indice_semaforo=0;
        AlmenoUnoInArrivo=i;
        for(j=indice; j<MAXPERCORSI; j++){
           if(AGV[i]->mission.punto_prenotato[j+1]==0) break;
           //-------------------------------------------
           // Verifico se tra i nodi ancora da occupare
           // c'è un nodo di porta
           //-------------------------------------------
           for(k=0; k<11; k++){
              if(PorteImp[Port].PuntoSemaforo[k]==0                                 ) continue;
              if(PorteImp[Port].PuntoSemaforo[k]!=AGV[i]->mission.punto_prenotato[j]) continue;
              if(j>old_indice                                                       ) continue;
              indice_semaforo=j;
              old_indice=j;
              break;
           }
        }
        //---------------------------------------
        // Se il TGV incontra prima i nodi di
        // semaforo piuttosto che quelli di
        // ingombro NON E' IN ARRIVO
        //---------------------------------------
        if(indice_semaforo!=0 && indice_semaforo<indice_ingombro) AlmenoUnoInArrivo=0;
        //---------------------------------------
        // Se almeno un TGV in attraversamento
        // NON ha punti di semaforo devo alzare
        // il bit di TGV IN ARRIVO SU PORTA
        //---------------------------------------
        if(AlmenoUnoInArrivo!=0) break;
     }
     PortConAgvInArrivo[Port] = AlmenoUnoInArrivo;
  }

  //-----------------------------------------------------------------
  // E) Verifica se consensi cambiati x PLCTERRA
  //-----------------------------------------------------------------
  OkTrasmissioneGenerico=false;
  for(Port=1; Port<=MAXPORTEIMP; Port++){
     //------------------------------------------
     // Porta inesistente
     //------------------------------------------
     for(i=0; i<11; i++){
        if(PorteImp[Port].Punto[i]==0) continue;
        break;
     }
     if(i>9) continue;
     //---------------------------------------------
     // Verifico cambio stato consensi
     //---------------------------------------------
     if(!PorteImp[Port].RichIngresso &&  PortConAgvInRichiestaIngresso[Port]){
        OkTrasmissioneGenerico=true;
     }
     if(PorteImp[Port].RichIngresso  && !PortConAgvInRichiestaIngresso[Port]){
        OkTrasmissioneGenerico=true;
     }
     if(!PorteImp[Port].TgvInIngombro && PortConAgvInIngombro[Port]){
        OkTrasmissioneGenerico=true;
     }
     if(PorteImp[Port].TgvInIngombro && !PortConAgvInIngombro[Port]){
        OkTrasmissioneGenerico=true;
     }
     if(PorteImp[Port].TgvInArrivo && !PortConAgvInArrivo[Port]){
        OkTrasmissioneGenerico=true;
     }
     if(!PorteImp[Port].TgvInArrivo && PortConAgvInArrivo[Port]){
        OkTrasmissioneGenerico=true;
     }
  }
  //-----------------------------------------------------------------------
  // D) Se consensi cambiati TRASMETTI a PLCTERRA:
  // nb:  DM X.0 - Consenso Ingresso
  //      DM X.1 - Richiesta Ingresso
  //      DM X.2 - Agv in ingombro
  //-----------------------------------------------------------------------
  if(OkTrasmissioneGenerico){
     memset(&dm , 0, sizeof(dm));
     for(Port=1; Port<=MAXPORTEIMP; Port++){
        //------------------------------------------
        // Porta inesistente
        //------------------------------------------
        for(i=0; i<11; i++){
           if(PorteImp[Port].Punto[i]==0) continue;
           break;
        }
        if(i>9) continue;
        //----------------------------------------------------------
        // Aggiornamento struttura porte
        //----------------------------------------------------------
        PorteImp[Port].RichIngresso  = PortConAgvInRichiestaIngresso[ Port ];
        PorteImp[Port].TgvInIngombro = PortConAgvInIngombro[ Port ];
        PorteImp[Port].TgvInArrivo   = PortConAgvInArrivo[ Port ];
        //----------------------------------------------------------
        // Se ho un TGV in RICHIESTA o IN INGOMBRO verifico se ha un
        // CARRO A BORDO
        //----------------------------------------------------------
        NumAgv=0;
        PorteImp[Port].TgvConCarico  = false;
        if(PortConAgvInIngombro[Port]>0 && PortConAgvInIngombro[Port]<=MAXAGV                  ) NumAgv=PortConAgvInIngombro[Port];
        if(PortConAgvInRichiestaIngresso[Port]>0 && PortConAgvInRichiestaIngresso[Port]<=MAXAGV) NumAgv=PortConAgvInRichiestaIngresso[Port];
        if(NumAgv!=0){
           PorteImp[Port].TgvConCarico = AGV[NumAgv]->stato.s.bit.carA | AGV[NumAgv]->stato.s.bit.carB;
        }
        //----------------------------------------------------------
        // Compilazione DM in scrittura su PLC
        //----------------------------------------------------------
        if( PorteImp[Port].OkIngresso       ) SetBit((char *) &dm[Port], 0);
        if( PorteImp[Port].RichIngresso     ) SetBit((char *) &dm[Port], 1);
        if( PorteImp[Port].TgvInIngombro    ) SetBit((char *) &dm[Port], 2);
        if( PorteImp[Port].PortaInBlocco    ) SetBit((char *) &dm[Port], 13);
        if( PorteImp[Port].AperturaParziale ) SetBit((char *) &dm[Port], 14);
        if( PorteImp[Port].TgvConCarico     ) SetBit((char *) &dm[Port], 15);
     }
     //--------------------------------------------------
     // Scrittura consensi su PLC di terra
     // DM 1 = Macchina 1
     // DM 2 = Macchina 2
     // .....
     //--------------------------------------------------
     if(!break_Com[COM2] && ComunicazioneDirettaPLC==0){
        NumCanali          = MAXPORTEIMP;
        NumPlc             = 0;
        CanaleIniziale     = 11;
        //--------------------------------------------------
        // Se il numero di canali da scrivere è "0" perchè
        // non ci sono delle porte --> Skip!!
        //--------------------------------------------------
        if(NumCanali<1) return 0;
        err = OM.wr_canale(&dm[1], NumPlc, "WD", CanaleIniziale, NumCanali, all_mess);
     }
  }
  if(err){
     strcpy(all_mess, "Errore scrittura consensi con porte impianto");
  }
  return err;
}


// -----------------------------------
//     OrdineMac()
// -----------------------------------
//  Ordina Priorita macchina
//
void process::OrdineMac(short int *OrdinaMac)
/*****************************************/
{
  bool ordine_ok;
  short int Mac, Mac1, Mac2, i, j;

  //----------------------------------------
  // Compilazione struttura locale con
  // locazioni in ordine di priorità
  //----------------------------------------
  Mac=0;
  for(i=1; i<=MAXCHIAMATE; i++){
     if(ch[i]->Num<1                ) continue;
     if(ch[i]->Num>MAXCHIAMATE ) continue;
     if(ch[i]->Tipo==NN             ) continue;
     if(ch[i]->Priorita==0          ) continue;
     OrdinaMac[Mac]=ch[i]->Num;
     Mac++;
  }
  //--------------------------------------------
  // Ordinamento
  //--------------------------------------------
  ordine_ok=true;
  for(j=0; j<MAXCHIAMATE; j++){
     for(i=0; i<MAXCHIAMATE; i++){
        if(OrdinaMac[i+1]==0) break;
        Mac1=OrdinaMac[i  ];
        Mac2=OrdinaMac[i+1];
        if(ch[Mac1]->Priorita<ch[Mac2]->Priorita){
           OrdinaMac[i  ]=Mac2;
           OrdinaMac[i+1]=Mac1;
           ordine_ok=false;
        }
     }
     if(ordine_ok==true) break;
  }
  return;
}


// -----------------------------------
//     OrdineBaie()
// -----------------------------------
//  Ordina Priorita macchina
//
void process::OrdineBaie(short int *OrdineBaie)
/*****************************************/
{
  bool ordine_ok;
  short int OrdineGrp[MAXGRUPPIIMP+1];
  short int Baia, Baia1, Baia2, i, j;
  short int Grp, Grp1, Grp2;

  //----------------------------------------
  // Metto in ordine i gruppi
  //----------------------------------------
  Grp=Grp1=Grp2=0;
  memset(&OrdineGrp[0], 0, sizeof(OrdineGrp));
  for(i=1; i<=MAXGRUPPIIMP; i++){
     if(strcmp(P.GruppiBaie[i].Nome, "")==0) continue;
     OrdineGrp[Grp]=i;
     Grp++;
  }
  //----------------------------------------
  // Ordinamento GRUPPI
  //----------------------------------------
  ordine_ok=true;
  for(j=0; j<MAXGRUPPIIMP; j++){
     for(i=0; i<MAXGRUPPIIMP; i++){
        if(OrdineGrp[i+1]==0) break;
        Grp1=OrdineGrp[i  ];
        Grp2=OrdineGrp[i+1];
        if(P.GruppiBaie[Grp1].Priorita<P.GruppiBaie[Grp2].Priorita){
           OrdineGrp[i  ]=Grp2;
           OrdineGrp[i+1]=Grp1;
           ordine_ok=false;
        }
     }
     if(ordine_ok==true) break;
  }
  //----------------------------------------
  // Gestione CHIAMATE BAIE in ordine per
  // gruppo
  //----------------------------------------
  Baia=0;
  for(j=0; j<MAXGRUPPIIMP; j++){
     //----------------------------------------
     // Compilazione struttura locale con
     // locazioni in ordine di priorità
     //----------------------------------------
     Grp = OrdineGrp[j];
     if(Grp==NN) continue;
     for(i=1; i<=MAXBAIE; i++){
        if(ba[i]->Num<1            ) continue;
        if(ba[i]->Num>MAXBAIE      ) continue;
        if(ba[i]->Tipo==NN         ) continue;
      //if(ba[i]->Priorita==0      ) continue;
        if(ba[i]->Gruppo!=Grp      ) continue;
        OrdineBaie[Baia]=ba[i]->Num;
        Baia++;
     }
  }
  //--------------------------------------------
  // Ordinamento BAIE
  //--------------------------------------------
  ordine_ok=true;
  for(j=0; j<MAXBAIE; j++){
     for(i=0; i<MAXBAIE; i++){
        if(OrdineBaie[i+1]==0) break;
        Baia1=OrdineBaie[i  ];
        Baia2=OrdineBaie[i+1];
        if(ba[Baia1]->Priorita<ba[Baia2]->Priorita){
           OrdineBaie[i  ]=Baia2;
           OrdineBaie[i+1]=Baia1;
           ordine_ok=false;
        }
     }
     if(ordine_ok==true) break;
  }
  return;
}


// -----------------------------------
//     RunAutomatico()
// -----------------------------------
//  Ciclo Principale
//
int process::RunAutomatico(char *all_mess)
/*****************************************/
{
  int err=0;
  WORD dm[21];
  bool trovato;
  bool macchina_ok;
  bool TgvSuRicarica;
  bool tgv_senza_niente_a_bordo;
  short int i, j, k, z, MinPercorsi;
  short int altro_agv;
  short int macchina, baia;
  short int new_dist, new_rot, num_pin, dest;
  short int OrdinaMac[MAXCHIAMATE+1];
  short int OrdinaBaie[MAXBAIE+1];
  short int NumAgv, AutomaticoTGV, OkProgramma;
  char FileNameSource[21]="";
  char FileNameDest[21]  ="";
  bool ResetMissione;
  bool ResetChiamata;
  SYSTEMTIME TimeInizio;
  SYSTEMTIME TimeLoadStatoStart;
  SYSTEMTIME TimeLoadStatoEnd;
  SYSTEMTIME TimeLoadSQLStart;
  SYSTEMTIME TimeLoadSQLEnd;
  SYSTEMTIME TimeFine;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  int msec=0;
  int msec1=0;
  int msec2=0;
  short int NumCanali;
  static short int count_err_com[MAXAGV+1];
  static short int count_err_com_allarme_interno[MAXAGV+1];
  bool AlmenoUnPlcConnesso=false;
  char buffer[300];
  int  NumLettureConsecutive=4;

  GetLocalTime( &TimeInizio );         // start run_automatico
  //----------------------------------------------
  // Gestione LOG TGV Attiva
  //----------------------------------------------
  if(LogTgvAttivato) return 0;

  //----------------------------------------------
  // Test percorsi TecnoFerrari
  //----------------------------------------------
  if(FormLayout->TestPercorsiTecnoFerrari) return 0;


  //----------------------------------------------
  // Lettura da PLCs Allen Bradley
  //----------------------------------------------
  if( !break_Com[COM2] ){
     CicloPlcAllen++;
     //---------------------------------------------------------
     // 06/01/2012 Azzera comunque il flag di comunicazione ok
     //---------------------------------------------------------
     for( i=1; i<= MAX_PLC_AB; i++ ){
        DatiPLC[ i ].R.DatiOk=0;
     }
     if( CicloPlcAllen>4 ){
        err = AllenB_LetturaPlc( all_mess );
        CicloPlcAllen=0;

        for( i=1; i<= MAX_PLC_AB; i++ ){
           if( DatiPLC[ i ].R.DatiOk ){
              AlmenoUnPlcConnesso=true;
           }
        }
        if( AlmenoUnPlcConnesso ) AllenbCtrTxRx++;
        if(!AlmenoUnPlcConnesso ) AllenbCtrTxRx--;
     }
  }
  //if(err) error_trasmit(0, all_mess);


  //----------------------------------------------
  // LETTURA CHIAMATE MACCHINA
  // Lettura stato chiamate macchine
  // (Ogni ciclo leggo una macchina sola)
  //----------------------------------------------
  for(i=1; i<=NumLettureConsecutive; i++ ){
     err = load_chiamate_macchine(&NumMaccCicloLettura, all_mess);
     if(err) error_trasmit(0, all_mess);
  }

  //----------------------------------------------
  // LETTURA CHIAMATE BAIA
  // Lettura stato chiamate da baia
  //----------------------------------------------
  err = load_chiamate_baie(all_mess);
  if(err) error_trasmit(0, all_mess);

  //----------------------------------------------
  // LETTURA DA SQL SERVER
  //----------------------------------------------
  if(SecReloadOnThreadSQL==0){
     GetLocalTime( &TimeLoadSQLStart );        // start lettura da SQL SERVER
     err = FormSQLDB->load_udc_from_SQL(CMD_RD_SQL_ALL_CHANGE, 0, 0, "", "", all_mess);
     if(err) error_trasmit(0, all_mess);
     GetLocalTime( &TimeLoadSQLEnd );         // start lettura da SQL SERVER
  }

  //----------------------------------------------
  // LETTURA CONSENSI CARICABATTERIA
  //----------------------------------------------
  for(i=1; i<=TOTBATT; i++ ){
     err = load_consensi_cbat(i, all_mess);
     if(err) error_trasmit(0, all_mess);
  }

  //----------------------------------------------
  // Lettura DM di scambio consensi con porte
  //----------------------------------------------
  err = load_consensi_porteimp(all_mess);
  if(err) error_trasmit(0, all_mess);

  //----------------------------------------------
  // Lettura DM di incendio in corso
  //----------------------------------------------
  err = load_incendio_in_corso(all_mess);
//if(err) error_trasmit(0, all_mess);

  //====================================================
  // ( I/O REMOTATI ) Funzioni personalizzate di READ
  //====================================================
  if(break_Com[COM6]==false){
     err = Graf_IORemoto_LoadCanali( all_mess );
     if(err) error_trasmit( 0, all_mess);
  }

  //----------------------------------------------
  // ALLARMI INTERNI A PLC TGV (per sirena)
  // Viene messo a "1" il DM4999
  //----------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(break_Com[COM1]==true        ) continue;
     if(AGV[i]->mission.EsclPLC==true) continue;
     if(AGV[i]->allarme_interno==0   ) continue;
     // traccia
     //----------------------------------------
     // SCRITTURA ALLARMI INTERNI A PLC
     //----------------------------------------
     if(AGV[i]->write_allarmi_interni_a_plc(all_mess)){
//      error_trasmit(i, all_mess);
     }
  }

  //----------------------------------------------------------------------------
  // Gestione SBLOCCO AUTOMATICO DEAD_LOCK
  //
  // AL-23/02 visto che lo sblocco dead lock gestisce anche le missioni di
  //          spostamento dai PT_in_ingombro, sposto la funzione in fondo al
  //          run_automatico, in modo da verificare che le normali missioni
  //          TGV, non si sblocchino già da sole le condizioni
  //----------------------------------------------------------------------------
//SbloccoDeadLock();

  NumAgvLoadStato++;
  if(NumAgvLoadStato<1 || NumAgvLoadStato>MAXAGV ) NumAgvLoadStato=1;
  if( AbilitaLetturaMultiPLC ){
     //----------------------------------------------
     // Multi Lettura dei PLC in Ethernet-Seriale
     // N.B. Solo 4 DATA MEMORY
     // DM 88 = dm di stop TGV per caduta consensi
     // DM 89 = dm di attesa ingresso macchina
     // DM 90 = posizione attuale AGV
     // DM 91 = start missione AGV ( 0,1,2 )
     // DM 92 = bit di stato AGV   ( HR 3  )
     // DM 93 = offset impulsi da nodo ( 11.02.94 )
     // DM 94 = bit di stato AGV aux
     // DM 95 = Volt batterie
     // DM 96 = Peso lordo
     // DM 97 = gradi calamite
     //----------------------------------------------
     NumCanali = 10;
     if( AGV[ 1 ]->mission.GestioneLaser) NumCanali=45;
     GetLocalTime( &TimeLoadStatoStart );
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        if( OmrPlc[ NumAgv ].TypCom!=3 && OmrPlc[ NumAgv ].TypCom!=4 ) continue; // Se NON è Ethernet-TCP o Ethernet-Seriale -->SKIP
      //OmrEth_MultiPlcEsclusionePLC[NumAgv] = 0;
        if(AGV[ NumAgv ]->mission.EsclPLC ) OmrEth_MultiPlcEsclusionePLC[NumAgv]=true;
        if(break_Com[COM1]                ) OmrEth_MultiPlcEsclusionePLC[NumAgv]=true;
        //----------------------------------------------
        // Gestione errore di comunicazione
        //----------------------------------------------
        if( !OmrEth_ErrPlc[NumAgv] ) errore_comunicazione[COM1][NumAgv]=0;
        if( OmrEth_ErrPlc[NumAgv]  ) errore_comunicazione[COM1][NumAgv]=1;  // Flag che indica un errore comunicazione
     }
     err = OmrEthSer_MultiPlcLoadCanaleRip       ( 1, MAXAGV, "RD", 88, NumCanali, all_mess);
     err = OmrEthTcp_MultiPlcLeggiScriviCanaleRip( 1, MAXAGV, 1, 1, 88, NumCanali, 1, all_mess);
     GetLocalTime( &TimeLoadStatoEnd );
  }
  //----------------------------------------------------
  // Reset strtuttura ingombro AGV
  //----------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(!ModificaCoordinateNodi){
        for(j=1;j<=MAXAGV;j++){
           IngombroAgvGrafico[i]->AzzeraIngombroAgv();
           FormXYNodi->SegmentoPerIngombro[i] = 999999;
           FormXYNodi->PuntoPerIngombro[i]    = 999999;
           FormXYNodi->CurvaPerIngombro[i]    = 999999;
        }
     }
  }
  //----------------------------------------------------------------------------
  // (B) - Gestione TGV
  //----------------------------------------------------------------------------
  for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
     AutomaticoTGV = (short int)(AGV[NumAgv]->stato.s.bit.funz);    // AGV in marcia + automatico
     OkProgramma   = (short int)(AGV[NumAgv]->stato.s.bit.okprog); // AGV pronto aricevere nuovo prg.

     //-------------------------------------------------------------------------
     // lettura stato AGV.
     // ( compila  AGV[num_agv]->stato )
     //-------------------------------------------------------------------------
     if( NumAgv==NumAgvLoadStato && (!AbilitaLetturaMultiPLC || OmrEth_MultiPlcEsclusionePLC[NumAgv])) GetLocalTime( &TimeLoadStatoStart );
     err = AGV[NumAgv]->load_stato(all_mess);
     if( NumAgv==NumAgvLoadStato && (!AbilitaLetturaMultiPLC || OmrEth_MultiPlcEsclusionePLC[NumAgv])) GetLocalTime( &TimeLoadStatoEnd );
     if(err) error_trasmit(NumAgv, all_mess);

     //----------------------------------------------
     // STOP TGV PER CADUTA CONSENSI + INCENDIO
     // Viene alzato un bit sul DM88
     //----------------------------------------------
     if(AGV[NumAgv]->stop_TGV_per_caduta_consensi(all_mess)){
        error_trasmit(NumAgv, all_mess);
     }
     //----------------------------------------------
     // Time di fine lettura stato TGV
     //----------------------------------------------
     if(NumAgv==1) GetLocalTime( &TimeLoadStatoEnd );
     //-------------------------------------------------------------------------
     // Cronologico allarmi
     //-------------------------------------------------------------------------
     AGV[NumAgv]->history_allarmi();
     if(AutomaticoTGV && !AGV[NumAgv]->stato.s.bit.allarme && AGV[NumAgv]->allarme_interno==0){
        if(AGV[NumAgv]->mission.TimeRipristino==1){
           sprintf(FileNameSource, "TEMALL%d.DAT", NumAgv);
           strcpy(FileNameDest, "CRONOALL.DAT");
           err = ConvertiTempCronoall(FileNameSource, FileNameDest, NumAgv, all_mess);
           if(err){
              P.error_trasmit(NumAgv, all_mess);
           }
        }
     }
     //----------------------------------------------------
     // 14/06/2011 LIBERA NODI DI TGV FUORI PERCORSO
     //
     // Se il TGV risulta:
     // - nessuna missione in corso
     // - escluso
     // - e con una posizione inesistente dell'impianto
     // ESEGUO UN LIBERA NODI
     //----------------------------------------------------
     if(!AGV[NumAgv]->test_mission_on() && (AGV[NumAgv]->mission.Esclusione || AGV[NumAgv]->mission.EsclPLC)){
        if(AGV[NumAgv]->stato.pos>=0 && AGV[NumAgv]->stato.pos<=MAXPUNTI){
           if(N.n[AGV[NumAgv]->stato.pos]->num==0){
              AGV[NumAgv]->reset_ram_mission();
              if(AGV[NumAgv]->free_all_nodi(all_mess)) return 1;
           }
        }
     }
     //-------------------------------------------------------------------------
     // (B2) - Se l'AGV MANUALE : azzera missione e libera nodi
     //-------------------------------------------------------------------------
     if(!AutomaticoTGV && OkProgramma){
        //----------------------------------------------------
        // Rileggo per sicurezza lo stato del TGV.
        //----------------------------------------------------
        err = AGV[NumAgv]->load_stato(all_mess);
        if(!err && !AGV[NumAgv]->stato.s.bit.funz){
           //-------------------------------------------------------
           // Se Laser in manuale gli trasmetto :
           // - il nodo più vicino
           // - la zona abilitata
           //-------------------------------------------------------
           if(AGV[NumAgv]->mission.GestioneLaser) AGV[NumAgv]->CalcoloNodoTgvLaser();
           //-------------------------------------------------------
           // Verifico se il TGV è su un PUNTO TERMINALE
           //
           // 21/07/2011 NO LIBERA NODI IN MANUALE
           // Nel caso di FATER non bisogno liberare i nodi in
           // manuale nemmeno se il TGV è su un PT.
           //
           // NOTA: SOLO SE E' IN UNA RICARICA O IN UN NODO
           //       INESISTENTE
           //-------------------------------------------------------
           TgvSuRicarica=false;
           if(AGV[NumAgv]->stato.pos>0 && AGV[NumAgv]->stato.pos<100){
              TgvSuRicarica=true;
           }
           //-------------------------------------------------------
           // Liberazione dei nodi occupati con TGV in manuale
           // Se missione attiva e AGV in MANUALE azzera missione
           //-------------------------------------------------------
           if(AGV[NumAgv]->test_mission_on()){
              AGV[NumAgv]->reset_ram_mission_per_manuale();
            //AGV[NumAgv]->reset_ram_mission();
              AGV[NumAgv]->mission.TGVInCambioMissione=false;
              strcpy(all_mess, MESS[363]);
              error_trasmit(NumAgv, all_mess);
           }
           //-------------------------------------------------------
           // Libera i nodi occupati TGV IN MANUALE
           //-------------------------------------------------------
           if(!ManualeConNodiOccupati || TgvSuRicarica){
              //-------------------------------------------
              // Reset mission del TGV
              //-------------------------------------------
              AGV[NumAgv]->reset_ram_mission();
              //-------------------------------------------
              // Reset chiamata a bordo TGV
              // 1 - TGV in ricarica senza cestoni
              // 2 - TGV su percorso con chiamata di
              //     evacuazione a ATC o DEC e solo 1 cest
              //-------------------------------------------
              tgv_senza_niente_a_bordo=false;
              if(AGV[NumAgv]->mission.PresCEST_A==LIBERA && AGV[NumAgv]->mission.PresCEST_B==LIBERA) tgv_senza_niente_a_bordo=true;
              if(AGV[NumAgv]->mission.Chiamata!=0){
                 // 1 - TGV in ricarica senza cestoni
                 if(TgvSuRicarica==true && tgv_senza_niente_a_bordo==true){
                    AGV[NumAgv]->reset_chiamata_AGV();
                    sprintf(all_mess, "Reset chiamata (%06d) per TGV in manuale su ricarica", AGV[NumAgv]->mission.Chiamata);
                    error_trasmit(NumAgv, all_mess);
                 }
              }
              //-------------------------------------------
              // Liberazione nodi
              //-------------------------------------------
              if(AGV[NumAgv]->free_all_nodi(all_mess)) return 1;
           }
        }
     }
     //-------------------------------------------------------------------------
     // 17/06/2011
     // SICUREZZA OCCUPAZIONE NODO SU CUI SI TROVA IL TGV
     // se un TGV si trova su un nodo senza missione è in automatico
     // per sicurezza marco BUSY il nodo su cui si trova
     //-------------------------------------------------------------------------
     if(AutomaticoTGV && OkProgramma && !AGV[NumAgv]->test_mission_on() && N.nodo_busy[AGV[NumAgv]->stato.pos]==0){
        N.nodo_busy[AGV[NumAgv]->stato.pos] = NumAgv;
     }
     //-------------------------------------------------------------------------
     // INCENDIO IN CORSO
     //-------------------------------------------------------------------------
     if(AutomaticoTGV && OkProgramma && !AGV[NumAgv]->test_mission_on() && N.punti_notevoli[AGV[NumAgv]->stato.pos]==false){
        //--------------------------------------------
        // 08/09/2011 INCENDIO IN CORSO
        // Devo liberare i nodi dei TGV fermi
        //--------------------------------------------
        if(IncendioInCorso==true){
           AGV[NumAgv]->free_all_nodi(all_mess);
           AGV[NumAgv]->reset_ram_mission();
        }
     }
     //-------------------------------------------------------------------------
     // (B3) - AZZERAMENTO MISSIONE E CHIAMATA AGV:
     //        Se l'AGV non ha missioni ed è Escluso e in Posizione
     //        di ricarica.
     //-------------------------------------------------------------------------
     if(OkProgramma && (!AGV[NumAgv]->test_mission_on())){
        //---------------------------------
        // Verifico se l'AGV è in Ricarica
        //---------------------------------
        TgvSuRicarica=false;
        if(AGV[NumAgv]->stato.pos>0 && AGV[NumAgv]->stato.pos<100 ) TgvSuRicarica=true;
        if(AGV[NumAgv]->mission.Esclusione && TgvSuRicarica){
           //-------------------------------
           // Reset missione
           //-------------------------------
           if(AGV[NumAgv]->mission.Chiamata!=0){
              AGV[NumAgv]->reset_mission(all_mess);
              AGV[NumAgv]->reset_chiamata_AGV();
              sprintf(all_mess, "Reset missione dovuto a esclusione o ricarica - Pos:%d", AGV[NumAgv]->stato.pos);
              error_trasmit(NumAgv, all_mess);
           }
        }
     }
     //--------------------------------------------------------
     // resetta AGV se mission.start=2 e non esistono missioni
     // in corso
     //--------------------------------------------------------
     if((AGV[NumAgv]->stato.start==2) && (!AGV[NumAgv]->test_mission_on()) ){
        AGV[NumAgv]->trasmetti_reset_AGV( all_mess );
        strcpy(all_mess, MESS[364]);
        error_trasmit(NumAgv, all_mess);
     }

     //-------------------------------------------------------------------------
     // (B5) - AZZERAMENTO DELLA MISSIONE AL VOLO PER STOP MANUALE:
     //        Se l'AGV si trova su un punto intermedio ed e stato forzato
     //        lo "Stop Manuale" sull'Editor del TGV.
     //-------------------------------------------------------------------------
     if(AGV[NumAgv]->mission.StopManuale && AGV[NumAgv]->stato.pos==AGV[NumAgv]->mission.pintend){
        ResetMissione=true;
        //----------------------------------------------------------
        // Se ci sono le condizioni per eseguire un Reset delle
        // Missioni al volo la prima cosa che faccio è trasmettere
        // il Reset al PLC (DM98) in modo da bloccare il reset
        // della missione in RAM in caso di errore di comunicazione
        //----------------------------------------------------------
        if(ResetMissione==true && !AGV[NumAgv]->stato.s.bit.sunod           ) ResetMissione=false;
        if(ResetMissione==true && AGV[NumAgv]->trasmetti_reset_AGV(all_mess)) ResetMissione=false;
        //----------------------------------------------------------
        // SICUREZZA
        // Invio due volte il comando di RESET MISSIONE TGV
        //----------------------------------------------------------
        if(ResetMissione==true && AGV[NumAgv]->trasmetti_reset_AGV(all_mess)) ResetMissione=false;
        //----------------------------------------------------------
        // Verifico lo stato del DM91 sul TGV per vedere se
        // l'azzeramento è andato a buon fine.
        //----------------------------------------------------------
        if(ResetMissione==true && !break_Com[COM1]){
           memset(&dm[0], 0, sizeof(dm));
           err = OM.ld_canale(&dm[0], (WORD)(NumAgv), "RD", 91, 2, all_mess);
           //-------------------------------------------------
           // Se la missione non è stata azzerata --> Skip!
           // 1 - errore di comunicazione
           // 2 - DM91!=0
           // 3 - OK PROGRAMMA==0 (bit5 DM92)
           //-------------------------------------------------
           if(err!=0 || dm[0]!=0 || !(TestBit((char *) &dm[1], 5))) ResetMissione=false;
        }
        //----------------------------------------------------------
        // Se sono verificate tutte le condizione necessarie per
        // il Reset delle Missioni al volo azzero anche la missione
        // in RAM.
        //----------------------------------------------------------
        if(ResetMissione==true){
           AGV[NumAgv]->reset_ram_mission_per_manuale();
           AGV[NumAgv]->free_all_nodi(all_mess);

           AGV[NumAgv]->stato.start=0;
           AGV[NumAgv]->stato.s.bit.okprog=1;
           AGV[NumAgv]->mission.TGVInCambioMissione=false;
           //-----------------------------------------------------------------
           // Lascio una traccia su file di un avvenuto Reset della Missione.
           //-----------------------------------------------------------------
           strcpy(all_mess, "(3) Reset Missione - (STOP MANUALE)");
           error_trasmit(NumAgv, all_mess);
        }
     }
  };

  //----------------------------------------------------------------------------
  // 10/04/2019 - GESTIONE MULTI PLC SU DM88 DI STOP TGV
  //----------------------------------------------------------------------------
  if( AbilitaLetturaMultiPLC ){
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        if( OmrPlc[ NumAgv ].TypCom!=3 && OmrPlc[ NumAgv ].TypCom!=4 ) continue; // Se NON è Ethernet-TCP o Ethernet-Seriale -->SKIP
        OmrEth_MultiPlcEsclusionePLC[NumAgv] = false;
        if(AGV[ NumAgv ]->mission.EsclPLC ) OmrEth_MultiPlcEsclusionePLC[NumAgv]=true;
        if(break_Com[COM1]                ) OmrEth_MultiPlcEsclusionePLC[NumAgv]=true;
        //----------------------------------------------
        // Compilazione struttura DM88
        //----------------------------------------------
        OmrEth_DM_TxRx[NumAgv][0] = AGV[NumAgv]->dm88_stop_tgv;
     }
     //--------------------------------------------------------------------
     // 13/07/2017  (1)Escludere questa parte:
     //             NON funzione con NumCanali>25 -->ritorna sempre errore
     //             Ma inoltre dopo 3 tentativi va anche in attesa Timeout
     //--------------------------------------------------------------------
     err = OmrEthSer_MultiPlcWriteCanaleRip      ( 1, MAXAGV, "WD", 88, 1, all_mess);
     err = OmrEthTcp_MultiPlcLeggiScriviCanaleRip( 1, MAXAGV, 1, 2, 88, 1, 0, all_mess);
  }

  //----------------------------------------------------------------------------
  // (C) - AUTOMATICO: Gestione automatica dell'Impianto.
  //----------------------------------------------------------------------------
  if(Automatico==true){
     //-----------------------------------------------------------
     // Prima di ogni valutazione sulle chiamate libero
     // eventuali TGV fermi
     //-----------------------------------------------------------
     for(i=1; i<=MAXAGV; i++){
        //---------------------------------------------------------
        // LIBERA I TGV IN CAMBIO MISSIONE
        //---------------------------------------------------------
        AGV[i]->LiberaTgvInCambioMissione();
        //---------------------------------------------------------
        // GESTIONE PINTFORZATO
        //---------------------------------------------------------
        AGV[i]->SetResetPIntForzatoTGV();
        //---------------------------------------------------------
        // ATTIVAZIONE DINAMICA PERCORSI SPECIALI
        // Sullo start di una nuova missione verifico se ci sono le
        // condizioni di gestione di un percorso speciale
        //---------------------------------------------------------
        AGV[i]->AttivaPercorsoSpeciale();
     }
     //-----------------------------------------------------------
     // Assegnazione priorità alle locazioni
     //-----------------------------------------------------------
     AssegnaPriorita();
     //-----------------------------------------------------------
     // Ordinamento delle chiamate delle macchine
     //-----------------------------------------------------------
     // CHIAMATE
     memset(&OrdinaMac,  0, sizeof(OrdinaMac));
     memset(&OrdinaBaie, 0, sizeof(OrdinaBaie));
     OrdineMac(&OrdinaMac[0]);
     OrdineBaie(&OrdinaBaie[0]);
     //-----------------------------------------------------------
     // 01 - TUTTI LE MACCHINE IN RICHIESTE/ESPULSIONE BOX
     //      (in base alla Priorità)
     //-----------------------------------------------------------
     for( i=0; i<MAXCHIAMATE; i++ ){
        //-----------------------------------------------------------
        // Recupero della macchina dalla struttura ordinata in base
        // alla PRIORITA' ASSERVIMENTO
        //-----------------------------------------------------------
        macchina = OrdinaMac[i];
        //-----------------------------------------------------------
        // Macchina disabilitata
        //-----------------------------------------------------------
        if(ch[macchina]->Num==0     ) continue;    // Numero della macchina errato
        if(ch[macchina]->Tipo==NN   ) continue;    // Numero della macchina annulata
        if(ch[macchina]->Esclusione ) continue;    // Macchina ESCLUSA
        //-----------------------------------------------------------
        // Gestione chiamate RICHIESTA
        //-----------------------------------------------------------
        if(GestioneChiamateMac_Ric(macchina, all_mess)) return 1;
        //-----------------------------------------------------------
        // Gestione chiamate ESPULSIONE
        //-----------------------------------------------------------
        if(GestioneChiamateMac_Esp(macchina, all_mess)) return 1;
     }
     //-----------------------------------------------------------
     // 02 - TUTTI LE BAIE IN CHIAMATA RICHIESTA/ESPULSIONE
     //      (in base alla Priorità)
     //-----------------------------------------------------------
     for( i=0; i<MAXBAIE; i++ ){
        //-----------------------------------------------------------
        // Recupero della macchina dalla struttura ordinata in base
        // alla PRIORITA' ASSERVIMENTO
        //-----------------------------------------------------------
        baia = OrdinaBaie[i];
        //-----------------------------------------------------------
        // Macchina disabilitata
        //-----------------------------------------------------------
        if(ba[baia]->Num==0     ) continue;    // Numero della macchina errato
        if(ba[baia]->Tipo==NN   ) continue;    // Numero della macchina annulata
        if(ba[baia]->Esclusione ) continue;    // Macchina ESCLUSA
        //-----------------------------------------------------------
        // Gestione chiamate RICHIESTA
        //-----------------------------------------------------------
        if(GestioneChiamateBaie_Ric(baia, all_mess)) return 1;
        //-----------------------------------------------------------
        // Gestione chiamate ESPULSIONE
        //-----------------------------------------------------------
        if(GestioneChiamateBaie_Esp(baia, all_mess)) return 1;
     }

     //-------------------------------------------------------------
     // (20) - Missioni di scarico su STOCCAGGIO
     //-------------------------------------------------------------
     if(AgvConCaricoDestMagazzino(all_mess)) return 1;

     //-------------------------------------------------------------
     // (22) - SPOSTAMENTO PALLET IN MAGAZZINI NON IDONEI
     //-------------------------------------------------------------
     if(GestioneSpostaPallet(all_mess)) return 1;

     //-------------------------------------------------------------
     // (23) - Se non ci sono missioni in corso e il TGV è in
     //        ricarica forza il riposo AGV.
     //-------------------------------------------------------------
     for(i=1; i<=MAXAGV; i++){
        if(!AGV[i]->stato.s.bit.ris       ) continue;
        if(PosizioneRiposoAgv(i, all_mess)) return 1;
     }
     //-------------------------------------------------------------
     // (24) - Se non ci sono missioni in corso, forza il riposo AGV
     //-------------------------------------------------------------
     for(i=1; i<=MAXAGV; i++){
        if(PosizioneRiposoAgv(i, all_mess)) return 1;
     }

     //-------------------------------------------------------------
     // (C9) - Sposta TGV in ingombro alle porte
     //-------------------------------------------------------------
     for(i=1; i<=MAXAGV; i++){
        if(FuoriInGombroPortePerIncendio(i, all_mess)) return 1;
     }
  }

  //--------------------------------------------------------------
  // Gestione SBLOCCO AUTOMATICO DEAD_LOCK
  //
  // AL-23/02 In questa posizione abbiamo se esiste ancora una
  //          condizione di stallo, siamo sicuri che non si
  //          sblocchi più, nemmeno con le missioni standard
  //          gestite dal run_automatico.
  //--------------------------------------------------------------
  SbloccoDeadLock();

  //--------------------------------------------------------------
  // GESTIONE PRECEDENZE PER TGV IN RETTILINEO
  // Compilo la struttura dei nodi prenotati dai TGV lanciati in
  // rettilineo
  //--------------------------------------------------------------
  N.verifica_tgv_lanciato_su_rettilineo();

  //--------------------------------------------------------------
  // (D) - MANUTENZIONE MISSIONE:
  //       1) Verifica se ci sono missioni inizializzate e esegue la
  //          trasmissione parziale o completa della missione all'AGV.
  //       2) Si prende carico dell'occupazione e liberazione nodi.
  //       3) Si prende carico di fare le operazioni di carico e
  //          scarico dei dati cestoni.
  //--------------------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     err = maintenance_mission(i, all_mess);
     if(err) error_trasmit(i, all_mess);
     //----------------------------------------------
     // GESTIONE_ASRV (trasmissione comandi ciclici
     //----------------------------------------------
     #ifdef GESTIONE_ASRV
        if(!err){
           err=AGV[i]->ASRV_trs_command(all_mess);
           if(err) error_trasmit(i, all_mess);
        }
     #endif;
  }

  //------------------------------------------------------------------
  // (H) - Scrittura consensi per accesso alle porte
  //------------------------------------------------------------------
  err = write_consensi_porteimp(all_mess);
  if( err ) P.error_trasmit( 0, all_mess );

  //------------------------------------------------------------------
  // Compilazione Dati per Macchine e Scrittura a PLCs Allen Bradley
  //------------------------------------------------------------------
  setta_consensi_chiamate(all_mess);

  //------------------------------------------------------------------
  // Compilazione Dati per Baie
  //------------------------------------------------------------------
  setta_consensi_baie(all_mess);

  //------------------------------------------------------------------
  // Compilazione Dati per Cambiobatterie
  //------------------------------------------------------------------
  setta_consensi_cbat(all_mess);

  //------------------------------------------------------------------
  // SCRITTURA SU MACCHINE
  // Scrittura segnali macchine
  //------------------------------------------------------------------
  for(i=1; i<=NumLettureConsecutive; i++ ){
     err = write_chiamate_macchine(&NumMaccCicloScrittura, all_mess);
     if(err) error_trasmit(0, all_mess);
  }
  NumMaccCicloScrittura=NumMaccCicloLettura;

  //----------------------------------------------
  // SCRITTURA SU BAIE
  // Stato consensi
  //----------------------------------------------
  err = write_chiamate_baie(all_mess);
  if(err) error_trasmit(0, all_mess);

  //----------------------------------------------
  // WRITE CONSENSI CARICABATTERIA
  //----------------------------------------------
  for(i=1; i<=TOTBATT; i++ ){
     err = write_consensi_cbat(i, all_mess);
     if(err) error_trasmit(0, all_mess);
  }

  //----------------------------------------------
  // Gestione ALLEN BRADLEY
  //----------------------------------------------
  if( !break_Com[COM2] ){
     if( CicloPlcAllen==0 ){
        err = AllenB_SctritturaPlc( all_mess );
     }
  }

  //------------------------------------------------------------------
  // (I) - WATCH DOG su PLC di terra:
  //       DM300 = Viene alzato il bit relativo al TGV in allarme.
  //               TGV da  1...16.
  //       DM301 = Viene alzato il bit relativo al TGV in allarme.
  //               TGV da 17...20.
  //       DM302 = Settato con il valore 1 per informare il PLC che
  //               che il ciclo automatico si è completato (viene
  //               azzerato dal PLC).
  //               Settato con il valore 2 per informare il PLC che
  //               il software è stato chiuso correttamente.
  //       DM303 = Zona 1 impegnata da un TGV.
  //       DM304 = Zona 2 impegnata da un TGV.
  //       DM305 = Zona 3 impegnata da un TGV.
  //       DM306 = Zona 4 impegnata da un TGV.
  //       DM307 = Zona 5 impegnata da un TGV.
  //       DM308 = Zona 6 impegnata da un TGV.
  //       DM309 = Zona 7 impegnata da un TGV.
  //       DM310 = Zona 8 impegnata da un TGV.
  //       DM311 = Zona 9 impegnata da un TGV.
  //       DM312 = Zona 10 impegnata da un TGV.
  //
  // N.B.: Se il Software o il PC si bloccano o vengono chiusi in
  //       maniera errata il DM302 rimane a "0".
  //------------------------------------------------------------------
  memset(&dm, 0, sizeof(dm));
/*
  //------------------------------------------------------------------
  // - DM300 Setto il bit corrispondente al TGV in ALLARME da  1 a 16.
  // - DM301 Setto il bit corrispondente al TGV in ALLARME da 17 a 20.
  // - DM302 Setto la dm a 1 per indicare che il ciclo automatico
  //         si è svolto correttamente senza blocchi del sistema.
  // - DM303 Zona 1 impegnata da un TGV.
  // - DM304 Zona 2 impegnata da un TGV.
  // - DM305 Zona 3 impegnata da un TGV.
  // - DM306 Zona 4 impegnata da un TGV.
  // - DM307 Zona 5 impegnata da un TGV.
  // - DM308 Zona 6 impegnata da un TGV.
  // - DM309 Zona 7 impegnata da un TGV.
  // - DM310 Zona 8 impegnata da un TGV.
  // - DM311 Zona 9 impegnata da un TGV.
  // - DM312 Zona10 impegnata da un TGV.
  //------------------------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(!AGV[i]->stato.s.bit.allarme ) continue;
     if(AGV[i]->mission.EsclPLC      ) continue;
     if(AGV[i]->mission.Esclusione   ){ // tgv esclusi in ricarica
        if(AGV[i]->stato.pos>0 && AGV[i]->stato.pos<100) continue;
     }
     //-------------------------------
     // Bit di TGV in allarme
     //-------------------------------
     SetBit((char *) &dm[0], i-1);             // AGV in allarme
  }

  //------------------------------------------------------------------
  // Gestione zone impianto impegnate da TGV
  //------------------------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(AGV[i]->mission.Zona== 1) SetBit((char *) &dm[ 3], i-1);
     if(AGV[i]->mission.Zona== 2) SetBit((char *) &dm[ 4], i-1);
     if(AGV[i]->mission.Zona== 3) SetBit((char *) &dm[ 5], i-1);
     if(AGV[i]->mission.Zona== 4) SetBit((char *) &dm[ 6], i-1);
     if(AGV[i]->mission.Zona== 5) SetBit((char *) &dm[ 7], i-1);
     if(AGV[i]->mission.Zona== 6) SetBit((char *) &dm[ 8], i-1);
     if(AGV[i]->mission.Zona== 7) SetBit((char *) &dm[ 9], i-1);
     if(AGV[i]->mission.Zona== 8) SetBit((char *) &dm[10], i-1);
     if(AGV[i]->mission.Zona== 9) SetBit((char *) &dm[11], i-1);
     if(AGV[i]->mission.Zona==10) SetBit((char *) &dm[12], i-1);
  }
  //------------------------------------------------------------------
  // Scrittura a PLC DI TERRA
  //------------------------------------------------------------------
  if(!break_Com[COM2]){
     dm[2]=1;
     if(OM.wr_canale(&dm[0], 0, "WD", 300, 13, all_mess)) return 1;
  }
*/
  //====================================================
  // ( I/O REMOTATI ) Funzioni personalizzate di WRITE
  //====================================================
  if(break_Com[COM6]==false){
     err = Graf_IORemoto_WriteCanali( all_mess );
     if(err) error_trasmit( 0, all_mess);
  }

  //-------------------------------------------------------------------------
  // (L) - GESTIONE ERRORE RIPERITIVO
  // Se è stato superato "MinErroriComTgvConsecutivi" azzero la memoria
  // di messaggio di errore comunicazione archiviato per procedere a una
  // ulteriore scrittura negli errori di sistema
  //-------------------------------------------------------------------------
  for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
     if(errore_comunicazione[COM1][NumAgv]==1){
        if(count_err_com[NumAgv]<MinErrConsecutiviTGV){
           count_err_com[NumAgv]++;
           AGV[NumAgv]->mission.ErrCom=true;
        }
        else{
           count_err_com[NumAgv]=0;
           AGV[NumAgv]->mission.ErrCom=false;
           count_err_com_allarme_interno[NumAgv]++;
        }
     }
     else{
        count_err_com[NumAgv]=0;
        AGV[NumAgv]->mission.ErrCom=false;
        //------------------------------------------------
        // se si ripristina la comunicazione viene
        // abbassato anche l'allarme interno del TGV
        // con il relativo conteggio
        //------------------------------------------------
        count_err_com_allarme_interno[NumAgv]=0;
        if(AGV[NumAgv]->allarme_interno==ALL_INT_ERR_COMUNIC){
           AGV[NumAgv]->allarme_interno=0;
           AGV[NumAgv]->old_allarme_interno=0;
        }
     }
  }

  //-------------------------------------------------------------------------
  // (L1) - GESTIONE ALLARME INTERNO PER ERRORI DI COMUNICAZIONE
  //        Se l'errore di "comunicazione ripetivo" si ripete per più
  //        di 10 volte consecutivamente senza mai ripristinarsi la
  //        comunicazione, viene mandato il TGV in allarme interno
  //-------------------------------------------------------------------------
  for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
     if(count_err_com_allarme_interno[NumAgv]>10){
        AGV[NumAgv]->allarme_interno=ALL_INT_ERR_COMUNIC;
        count_err_com_allarme_interno[NumAgv]=11;    // blocco il conteggio per evitare sforamenti di memoria
     }
  }

  //----------------------------------------------
  // Sincronizzazione con RunAutomatico
  //----------------------------------------------
  RunAutomaticoEseguito=true;

  GetLocalTime( &TimeFine );
  // --- durata ciclo automatico ---
  msec = (int) (TimeFine.wSecond - TimeInizio.wSecond);
  msec = msec * 1000;
  msec = msec + (int) (TimeFine.wMilliseconds - TimeInizio.wMilliseconds);
  if( msec<0    ) msec=0;
  if( msec>5000 ) msec=5000;
  sprintf(all_mess, "%dmsec", msec);
  FormMain->SBarImpianto->Panels->Items[5]->Text = all_mess;
  // --- durata load_stato() ---
  msec1 = (int) (TimeLoadStatoEnd.wSecond - TimeLoadStatoStart.wSecond);
  msec1 = msec1 * 1000;
  msec1 = msec1 + (int) (TimeLoadStatoEnd.wMilliseconds - TimeLoadStatoStart.wMilliseconds);
  if( msec1<0    ) msec1=0;
  if( msec1>5000 ) msec1=5000;
  sprintf(all_mess, "%dmsec", msec1);
  FormMain->SBarImpianto->Panels->Items[6]->Text = all_mess;
  // --- SQL ---
  if(SecReloadOnThreadSQL==0){
     msec2 = (int) (TimeLoadSQLEnd.wSecond - TimeLoadSQLStart.wSecond);
     msec2 = msec2 * 1000;
     msec2 = msec2 + (int) (TimeLoadSQLEnd.wMilliseconds - TimeLoadSQLStart.wMilliseconds);
     if( msec2<0    ) msec2=0;
     if( msec2>5000 ) msec2=5000;
     sprintf(all_mess, "%dmsec", msec2);
     FormMain->SBarImpianto->Panels->Items[7]->Text = all_mess;
  }
  return 0;
}

// -----------------------------------
//     SemiAutomatico()
// -----------------------------------
//  Esecuzione delle missioni predisposte in
//  Semiautomatico.
//
int process::SemiAutomatico(char *all_mess)
{
  int err;
  short int i;
  short int NumTgv;
  short int Punto;
  short int TypMiss;
  short int TypPresa;
  short int Mac;
  short int Mag;
  short int Baia;
  short int LatoMag;
  short int ScaffNum;
  short int ScaffPil;
  short int ScaffPin;

  char buffer[101]="";
  bool Ok, MissioneNulla, NessunTgvInSemiAuto;

  //-------------------------------------------------------
  // Nessun TGV dell'impianto è in SEMIAUTOMATICO
  // Svuotamento lista
  //-------------------------------------------------------
  NessunTgvInSemiAuto = true;
  for(i=1; i<=MAXAGV; i++){
     if(AGV[i]->mission.SemiAutomatico==false) continue;
     NessunTgvInSemiAuto=false;
     break;
  }
  //-------------------------------------------------------
  // Se nessun TGV in semiautomatico reset intera struttura
  //-------------------------------------------------------
  if(NessunTgvInSemiAuto==true){
     IdMissioneSemiAuto=1;          // prima missione;
     AbilitaSemiAutoCiclico=false;
     memset(&SemiAuto[0], 0, sizeof(SemiAuto));
     return 0;
  }

  //-------------------------------------------------------
  // Inizializzazione delle variabili
  //-------------------------------------------------------
  err      = 0;
  Ok       = false;
  NumTgv   = SemiAuto[IdMissioneSemiAuto].Tgv;
  TypMiss  = SemiAuto[IdMissioneSemiAuto].TypMiss;
  TypPresa = SemiAuto[IdMissioneSemiAuto].TypPresa;
  Punto    = SemiAuto[IdMissioneSemiAuto].Dest;
  Mac      = SemiAuto[IdMissioneSemiAuto].Mac;
  Mag      = SemiAuto[IdMissioneSemiAuto].Mag;
  LatoMag  = SemiAuto[IdMissioneSemiAuto].LatoMag;
  Baia     = SemiAuto[IdMissioneSemiAuto].Baia;

  //---------------------------------------------------------
  // Nessuna missione impostata  --> skip!
  //---------------------------------------------------------
  if(NumTgv==0 && TypMiss==0 && Punto==0) return 0;

  //---------------------------------------------------------
  // Verifica se la missione è corretta
  //---------------------------------------------------------
  MissioneNulla=false;
  if(NumTgv==0 || (Punto==0 && Mac==0 && Mag==0 && Baia==0)) MissioneNulla=true;
  //---------------------------------------------------------
  // Condizioni di Skip!!!
  //---------------------------------------------------------
  if(NumTgv!=0){
     if(AGV[NumTgv]->test_mission_on()      ) return 0;
     if(!AGV[NumTgv]->mission.SemiAutomatico) return 0;
     if(!AGV[NumTgv]->stato.s.bit.okprog    ) return 0;
     if(!AGV[NumTgv]->stato.s.bit.funz      ) return 0;   // AGV in automatico
  }
  //-----------------------------------------------------------
  // (0) Verifica se la missione è un POSIZIONAMENTO SEMPLICE
  //-----------------------------------------------------------
  if(!MissioneNulla && !Ok && err==0 && TypMiss==MISS_SEMIAUTO_SOLO_POSIZIONAMENTO && Punto!=0){
     err = PosizionamentoManualeAGV(NumTgv, Punto, N_NO_FORCHE, &Ok, buffer);
  }
  //--------------------------------------------------------------
  // (1) Verifica se la missione è un CARICO DA MACCHINE
  //--------------------------------------------------------------
  if(!MissioneNulla && !Ok && err==0 && TypMiss==MISS_SEMIAUTO_MACCHINA && Mac>0 && Mac<=TOTCHIAMATE){
     err = CarScarMacManualeAGV(NumTgv, Mac, TypPresa, &Ok, buffer);
  }
  //--------------------------------------------------------------
  // (2) Verifica se la missione è un CARICO DA BAIE
  //--------------------------------------------------------------
  if(!MissioneNulla && !Ok && err==0 && TypMiss==MISS_SEMIAUTO_BAIA && Baia>0 && Baia<=TOTBAIE){
     err = CarScarBaiaManualeAGV(NumTgv, Baia, TypPresa, &Ok, buffer);
  }
  //------------------------------------------------------------------------
  // (3) Verifica se la missione è un CARICO/SCARICO DA MAGAZZINI
  //------------------------------------------------------------------------
  if(!MissioneNulla && !Ok && err==0 && TypMiss==MISS_SEMIAUTO_MAGAZZINO && Mag>0 && Mag<=TOTMAG){
     err = CarScarMagManualeAGV(NumTgv, Mag, LatoMag, TypPresa, &Ok, buffer);
  }
  //---------------------------------------------------------
  // Se è impossibile inizializzare una missione --> Skip!
  // Viene lasciata una traccia negli Errori di sistema e
  // viene cancellata dalla lista delle missione da eseguire
  //---------------------------------------------------------
  if(err){
     sprintf(all_mess, "(SEMIAUTOMATIC MODE) - %s", buffer);
     //------------------------------------------------
     // GESTIONE LISTA MISSIONI CICLICA O A SCROLL
     //------------------------------------------------
     // (1) - scroll
     if(AbilitaSemiAutoCiclico==false){
        IdMissioneSemiAuto=1; // forzatura su prima missione;
        for(i=1; i<10; i++){
           memcpy(&SemiAuto[i], &SemiAuto[i+1], sizeof(SEMIAUTOMATICO));
           memset(&SemiAuto[i+1], 0, sizeof(SEMIAUTOMATICO));
        }
     }
     // (2) - ciclica
     else{
        //--------------------------------------------------
        // Incremento della missione fino alla prima valida
        //--------------------------------------------------
        do{
           IdMissioneSemiAuto++;
           NumTgv   = SemiAuto[IdMissioneSemiAuto].Tgv;
           TypMiss  = SemiAuto[IdMissioneSemiAuto].TypMiss;
           Punto    = SemiAuto[IdMissioneSemiAuto].Dest;
        }while(IdMissioneSemiAuto<11 && (NumTgv==0 && TypMiss==0 && Punto==0));
        //--------------------------------------
        // Scrollata tutta la lista riprendo
        //--------------------------------------
        if(IdMissioneSemiAuto>10) IdMissioneSemiAuto=1;
     }
     return 1;
  }
  //---------------------------------------------------------
  // GESTIONE SEMIAUTOMATICO CICLICO
  // A seconda che sia attiva o meno questa gestione decido
  // se scrollare la lista delle missioni semi-automatiche
  // o se lasciarla compilata ed eseguirle in sequenza
  //---------------------------------------------------------
  // (1) SCROLL LISTA
  if(AbilitaSemiAutoCiclico==false){
     IdMissioneSemiAuto=1; // forzatura su prima missione;
     //------------------------------------------------------
     // Scorrimento lista verso l'alto
     //------------------------------------------------------
     if(Ok || MissioneNulla || AGV[NumTgv]->mission.Esclusione==true){
        for(i=1; i<10; i++){
           memcpy(&SemiAuto[i],   &SemiAuto[i+1], sizeof(SEMIAUTOMATICO));
           memset(&SemiAuto[i+1],              0, sizeof(SEMIAUTOMATICO));
        }
     }
  }
  // (2) GESTIONE MISSIONI IN SEQUENZA
  else{
     //--------------------------------------------------
     // Incremento della missione fino alla prima valida
     //--------------------------------------------------
     do{
        IdMissioneSemiAuto++;
        NumTgv   = SemiAuto[IdMissioneSemiAuto].Tgv;
        TypMiss  = SemiAuto[IdMissioneSemiAuto].TypMiss;
        Punto    = SemiAuto[IdMissioneSemiAuto].Dest;
     }while(IdMissioneSemiAuto<11 && (NumTgv==0 && TypMiss==0 && Punto==0));
     //--------------------------------------
     // Scrollata tutta la lista riprendo
     //--------------------------------------
     if(IdMissioneSemiAuto>10) IdMissioneSemiAuto=1;
  }

  return 0;
}


// ---------------------------
//    init_mission()
// ---------------------------
//  Inizializzazione missione
//
int process::init_mission(bool deadlock, short int forza_tipo_path, short int num_agv, int chiamata, short int dest, short int pos_dest, short int pint, short int tipo_mission, short int TypPresa, bool *OK, char *all_mess)
/**********************************************************************************************/
{
  int err, tipo;
  int OldChiamata;
  bool lato_uscita;
  bool lato_ingresso;
  bool forche_fuori_ing;
  bool nodo_fuori_ingombro;
  bool tutto_il_path_free;
  bool forza_occupa_tutto_il_path;
  bool almeno_un_tgv_in_cambio_missione;
  int aut, okprog, oldagv;
  int punto_per_blocco_rot180;
  int blocco_su_deviazione;
  short int pint_new, tipo_path;
  short int metri, MinPercorsi, limitrofo;
  short int i, j, agv_bloccante, altro_agv;
  short int pos_tgv, dist, rot, new_dist, new_rot;
  short int num_baia, num_mag, num_pos;
  short int nodo_ing_mag;
  short int perc[MAXPERCORSI];
  short int app_perc[MAXPERCORSI];
  short int new_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati[MAXPERCORSI];
  struct DATI_PERCORSO new_dati[MAXPERCORSI];
  struct LAYER zone_null[MAXZONEMAPPING+1];
  char buffer[101]="";
  char stringa_log[501]="";

  err=0;
  *OK=false;
  metri=MinPercorsi=0;
  tutto_il_path_free=false;
  forza_occupa_tutto_il_path=false;
  pos_tgv = AGV[ num_agv ]->stato.pos;
  memset(&perc[0],      0, sizeof(perc));
  memset(&dati[0],      0, sizeof(dati));
  memset(&app_perc[0],  0, sizeof(app_perc));
  memset(&new_perc[0],  0, sizeof(new_perc));
  memset(&new_dati[0],  0, sizeof(new_dati));
  memset(&zone_null[0], 0, sizeof(zone_null));
  //----------------------------------------------------
  // Minimo percorsi inizializzazione missione
  //----------------------------------------------------
  MinPercorsi = MINPERCORSI;
  //----------------------------------------------------
  // Se il TGV non ha il Rapporto Encoder impostato
  // ---> Skip!!!
  //----------------------------------------------------
  if(AGV[num_agv]->mission.RapEncoder==0){
     sprintf(all_mess, "Error in Rap Encoder TGV%d!", num_agv);
     AGV[num_agv]->reset_ram_mission();
     return 1;
  }
  //----------------------------------------------------
  // Se il TGV E LASER e non sono state zone_mapping
  // ---> Skip!
  //----------------------------------------------------
  if(AGV[num_agv]->mission.GestioneLaser && memcmp(&AGV[num_agv]->zone_mapping.Z[0], &zone_null[0], sizeof(zone_null))==0){
     sprintf(all_mess, "Invalid ZONE MAPPING for TGV%d!", num_agv);
     AGV[num_agv]->reset_ram_mission();
     return 1;
  }
  //----------------------------------------------------
  // 11/05/97 Verifica se AGV in posizione di ricarica
  // con destinazione ricarica -> SKIP
  //----------------------------------------------------
  if(tipo_mission==MISS_SOLO_POSIZIONAMENTO){
     if(pos_tgv==dest && AGV[num_agv]->stato.pos==AGV[num_agv]->mission.PosRicarica) return 0;
  }

  //----------------------------------------------------
  // MAGAZZINI MULTI NODO
  // (FINO AD ORA LA DEST ERA IL NODO DI INGRESSO MAG.)
  // Se la destinazione è un magazzino è più precisamente si tratta
  // di un magazzino con nodi nelle posizioni intermedie è necessario
  // incollare alla missione originale il pezzo di percorso tra il
  // nodo di ingresso e la posizione reale
  //-------------------------------------------------------------------
  nodo_ing_mag=dest;
  if(tipo_mission==MISS_CARICO_DA_MAGAZZINO || tipo_mission==MISS_SCARICO_SU_MAGAZZINO){
     M.test_punto_presente_mag(all_mess, dest, &num_mag, &num_pos);
     //---------------------------------------------
     // Se la destinazione è un magazzino recupero
     // il nodo reale e lo assegno a "dest"
     //---------------------------------------------
     if(num_mag!=0 && pos_dest!=0 && pos_dest<=M.m[num_mag]->Npt){
        if(dest==M.m[num_mag]->PuntoIng) dest = M.m[num_mag]->Pos[pos_dest].NodoIng;
        if(dest==M.m[num_mag]->PuntoOut) dest = M.m[num_mag]->Pos[pos_dest].NodoOut;
     }
  }
  //---------------------------------------------------------------------
  // verifica se AGV pronto a ricevere programma
  //---------------------------------------------------------------------
  okprog = AGV[num_agv]->stato.s.bit.okprog;// AGV pronto aricevere nuovo prg.
  if(!okprog) return 0;
  //---------------------------------------------------------------------
  // verifica se AGV in automatico
  //---------------------------------------------------------------------
  aut = AGV[num_agv]->stato.s.bit.funz;   // AGV in automatico
  if(!aut   ) return 0;                      // Test se AGV in automatico
  //---------------------------------------------------------------------
  // verifica se AGV escluso o in riserva con
  // destinazione >< da posizone di ricarica
  //---------------------------------------------------------------------
  if(AGV[num_agv]->test_agv_escluso() ) return 0;
  if(AGV[num_agv]->stato.start        ) return 0;      // Test start >< 0
  if(AGV[num_agv]->mission.pstart     ) return 0;      // missione in corso
  //---------------------------------------------------------------------
  // GESTIONE CARICHI ANOMALI
  // Se ho uno degli allarmi interni per carico incongruente non
  // inizializzo la mssione
  //---------------------------------------------------------------------
  if(AGV[num_agv]->allarme_interno==ALL_INT_RESET_DATI  ) return 0;
  if(AGV[num_agv]->allarme_interno==ALL_INT_SCAR_MANUALE) return 0;
  //----------------------------------------------------------------------
  // PRIORITA' AI TGV IN CAMBIO MISSIONE
  // Non inizializzo nuove missione con TGV fino a che ce ne sono
  // alcuni in cambio missione
  //----------------------------------------------------------------------
  if(AGV[num_agv]->mission.TGVInCambioMissione==false){
     almeno_un_tgv_in_cambio_missione=false;
     for(i=1; i<=MAXAGV; i++){
        if(i==num_agv                                ) continue;
        if(AGV[i]->mission.TGVInCambioMissione==false) continue;
        almeno_un_tgv_in_cambio_missione=true;
        break;
     }
     if(almeno_un_tgv_in_cambio_missione) return 0;
  }
  //----------------------------------------------------------------------
  // INCENDIO IN CORSO: il TGV non riparte per nuove missioni
  // se si trattano di missioni di spostamento per fuori ingombro
  // le attivo per garantire l'uscita dalle porte
  //----------------------------------------------------------------------
  if(IncendioInCorso==true){
     AGV[num_agv]->free_all_nodi(all_mess);
     AGV[num_agv]->reset_ram_mission();
     //---------------------------------------
     // Verifico se la destinazione è
     // un punto fuori ingombro
     //---------------------------------------
     nodo_fuori_ingombro=false;
     for(i=0; i<MAXNODIATTESA; i++){
        if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]<1         ) continue;
        if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]>MAXPUNTI  ) continue;
        if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]!=dest     ) continue;
        nodo_fuori_ingombro=true;
        break;
     }
     //---------------------------------------
     // Le destinazioni fuori ingombro sono
     // ammesse per spostare i TGV che bloccano
     // altri che a loro volta blocca porte
     //---------------------------------------
     if(nodo_fuori_ingombro==false) return 0;
  }

  //---------------------------------------------------------------------
  // Verifica se macchina asservita da altro AGV
  //---------------------------------------------------------------------
  if(chiamata && pint==0){ //10/06/2015 No bloccare se definito PINT
     oldagv = P.test_chiamata_asservita(chiamata);
     if((oldagv >0) && (oldagv != num_agv)){
        //----------------------------------------------------------
        // In caso di abilita avvicinamento a destinazione il
        // controllo della chiamata asservita ha significato solo
        // per i TGV già diretti alla stessa destinazione e senza
        // PIntForzato
        //----------------------------------------------------------
        if(AbilitaAvvicinamentoADest==0) return 0;
        else{
           if(AGV[oldagv]->mission.pend==dest && AGV[oldagv]->mission.PIntForzato==0) return 0;
        }
     }
  }

  //-------------------------------------------------------------------
  // SELEZIONE DEL TIPO PERCORSO DA FARE
  //-------------------------------------------------------------------
  tipo_path = (short int)(AGV[num_agv]->SelezioneTipoPath(deadlock, forza_tipo_path, pos_tgv, dest, &forza_occupa_tutto_il_path, &tutto_il_path_free));
  //-------------------------------------------------------------------
  // Forzatura occupazione intero percorso
  //-------------------------------------------------------------------
  if(OccupaTuttoSuPathVeloci==true && tipo_path==FAST_PATH && forza_occupa_tutto_il_path==false){
     if(tutto_il_path_free==true) forza_occupa_tutto_il_path=true;
  }
  //-------------------------------------------------------------------
  // RICALCOLO NODO DI AVVICINAMENTO IN BASE A TIPO PERCORSO SCELTO
  // Se la missione prevedeva un PIntForzato, lo ricalcolo
  // in base al tipo percorso disponibile trovato
  //-------------------------------------------------------------------
  if(pint!=0){
     pint_new=0;
     AGV[num_agv]->AvvicinamentoADestinazione(tipo_path, dest, 0, &pint_new);
     //----------------------------------------------------------
     // Se non ho un nuovo pint mentre originariamente c'era
     // devo controllare che la destinazione non sia occupata
     //----------------------------------------------------------
     if(pint_new!=0) pint=pint_new;
     if(pint_new==0){
        altro_agv=limitrofo=0;
        sprintf(all_mess, "TGV%d: destinazione non raggiungibile (PATH%d)...", num_agv, tipo_path);
        if(P.test_dest_limitrofe(dest, &altro_agv, &limitrofo, num_agv) && altro_agv!=0 && altro_agv!=num_agv){
           error_trasmit(num_agv, all_mess);
           AGV[num_agv]->allarme_interno = ALL_INT_STOP_TGV;
           return 0;
        }
        if(test_dest_riservata(dest, &altro_agv, num_agv) && altro_agv!=0 && altro_agv!=num_agv){
           error_trasmit(num_agv, all_mess);
           AGV[num_agv]->allarme_interno = ALL_INT_STOP_TGV;
           return 0;
        }
        if(test_dest_occupata(dest, &altro_agv) && altro_agv!=0 && altro_agv!=num_agv){
           error_trasmit(num_agv, all_mess);
           AGV[num_agv]->allarme_interno = ALL_INT_STOP_TGV;
           return 0;
        }
     }
  }

  //-------------------------------------------------------------------
  // RECUPERO I DATI DEL PERCORSO STANDARD
  //-------------------------------------------------------------------
  err = AGV[num_agv]->distanza(tipo_path, 0, dest, &perc[0], &dati[0], all_mess, &dist, &rot);
  if(err!=0 || dist>MAXPERCORSI){
     error_trasmit(num_agv, all_mess);
     AGV[num_agv]->allarme_interno = ALL_INT_STOP_TGV;
     return 0;
  }
  //-------------------------------------------------------------------
  // GESTIONE_ASRV (attraversamento corsie)
  //-------------------------------------------------------------------
  #ifdef GESTIONE_ASRV
     memset(&new_perc[0],  0, sizeof(new_perc));
     memset(&new_dati[0],  0, sizeof(new_dati));
     AGV[num_agv]->ASRV_attraversa_corsie(&perc[0], &dati[0], &new_perc[0], &new_dati[0]);
     //-------------------------------------------------------------------
     // Modifico il percorso
     //-------------------------------------------------------------------
     if(memcmp(&new_perc[0], &app_perc[0], sizeof(app_perc))!=0){
        memcpy(&perc, &new_perc, sizeof(perc));
        memcpy(&dati, &new_dati, sizeof(dati));
     }
  #endif;
  //-------------------------------------------------------------------
  // VERIFICA PRESENZA CURVE
  // Verifica presenza di curve lungo il percorso
  //-------------------------------------------------------------------
  if(AGV[num_agv]->mission.GestioneLaser==true){
     memset(&new_perc[0],  0, sizeof(new_perc));
     memset(&new_dati[0],  0, sizeof(new_dati));
     err = ImpostazioneCurve(&perc[0], &dati[0], &new_perc[0], &new_dati[0], all_mess);
     if(err!=0){
        error_trasmit(num_agv, all_mess);
        AGV[num_agv]->allarme_interno = ALL_INT_STOP_TGV;
        return 0;
     }
     //-------------------------------------------------------------------
     // Modifico il percorso in caso di curve
     //-------------------------------------------------------------------
     if(memcmp(&new_perc[0], &app_perc[0], sizeof(app_perc))!=0){
        memcpy(&perc, &new_perc, sizeof(perc));
        memcpy(&dati, &new_dati, sizeof(dati));
     }
  }
  //-------------------------------------------------------------------
  // VERIFICA PRESENZA DEVIAZIONI
  // Verifica presenza di curve lungo il percorso
  //-------------------------------------------------------------------
  memset(&new_perc[0],  0, sizeof(new_perc));
  memset(&new_dati[0],  0, sizeof(new_dati));
  err = ImpostazioneDeviazioni(num_agv, &perc[0], &dati[0], &new_perc[0], &new_dati[0], all_mess);
  if(err!=0){
     error_trasmit(num_agv, all_mess);
     AGV[num_agv]->allarme_interno = ALL_INT_STOP_TGV;
     return 0;
  }
  //-------------------------------------------------------------------
  // GESTIONE SCARICO DIRETTO SU POSTAZIONE SENZA INGRESSO + USCITA
  // Se su un punto terminale con la predisposizione a uscire poi
  // rientrare per le movimentazioni, ho la possibiltà di filtrare
  // che per le manovre di scarico con presenza a bordo si resti
  // sul punto.
  //-------------------------------------------------------------------
  forche_fuori_ing=false;
  if((GestioneDoppiaForcaTGV==0 || GestioneDoppiaForcaTGV==2) && AGV[num_agv]->stato.s.bit.ForcaB_FuoriIng==true                     ) forche_fuori_ing=true;
  if( GestioneDoppiaForcaTGV==1 && AGV[num_agv]->stato.s.bit.ForcaA_FuoriIng==true && AGV[num_agv]->stato.s.bit.ForcaB_FuoriIng==true) forche_fuori_ing=true;
  if(forche_fuori_ing==true && dist==2 && AGV[num_agv]->stato.pos==dest && new_perc[dist]==dest){  // missione di 3 nodi ==> USCITA + RIENTRO
     //-----------------------------------------
     // Mantengo solo l'ultimo passo programma
     //-----------------------------------------
     new_perc[0] = new_perc[2];
     new_dati[0] = new_dati[2];
     new_perc[1] = 0; memset(&new_dati[1], 0, sizeof(DATI_PERCORSO));
     new_perc[2] = 0; memset(&new_dati[2], 0, sizeof(DATI_PERCORSO));
  }
  //-------------------------------------------------------------------
  // Modifico il percorso revisionato
  //-------------------------------------------------------------------
  if(memcmp(&new_perc[0], &app_perc[0], sizeof(app_perc))!=0){
     memcpy(&perc, &new_perc, sizeof(perc));
     memcpy(&dati, &new_dati, sizeof(dati));
  }
  //-------------------------------------------------------------------
  // Set dei parametri di missione
  //-------------------------------------------------------------------
  OldChiamata                       = AGV[num_agv]->mission.Chiamata;
  AGV[num_agv]->mission.ResetAlVolo = false;
  AGV[num_agv]->mission.Chiamata    = chiamata;  // --- anche numero macchina
  AGV[num_agv]->mission.pstart      = AGV[num_agv]->stato.pos;
  AGV[num_agv]->mission.pintstart   = AGV[num_agv]->stato.pos;
  AGV[num_agv]->mission.pintend     = AGV[num_agv]->stato.pos;
  AGV[num_agv]->mission.pend        = dest;
  AGV[num_agv]->mission.NodIngMag   = nodo_ing_mag;  // IMPORTANTE: deve contenere sempre la destinazione TGV
                                                     // ma verrà utilizzato solo in caso di car/scar su magaz.
  AGV[num_agv]->mission.PIntForzato = pint;
  AGV[num_agv]->mission.Typ         = tipo_mission;
  AGV[num_agv]->mission.TypPresa    = TypPresa;
  AGV[num_agv]->mission.TypPath     = tipo_path;     // Tipo percorso scelto FAST/STANDARD
  // DATI PERCORSO MISSIONE
  memcpy(&AGV[num_agv]->mission.punto[0],     &perc[0], sizeof(AGV[num_agv]->mission.punto));
  memcpy(&AGV[num_agv]->mission.dati_perc[0], &dati[0], sizeof(AGV[num_agv]->mission.dati_perc));

  //-------------------------------------------------------------------
  // GESTIONE PINTFORZATO
  // Elaboro eventuali forzature PIntForzato, alternative
  //-------------------------------------------------------------------
  AGV[num_agv]->SetResetPIntForzatoTGV();

  //-------------------------------------------------------------------
  // MODIFICA MOVIMENTAZIONE TGV
  // Verifico se il TGV è predisposto per muoversi sempre in una
  // direzione
  //-------------------------------------------------------------------
  AGV[num_agv]->CambiaDirezione();

  //-------------------------------------------------------------------
  // INSERIMENTO DEI RALLENTAMENTI AUTOMATICI
  // Controllo il Percorso del TGV e inserisco i rallentamenti in
  // automatico nel caso in cui la riga di percorso associata ai nodi
  // contenga un rallentamento pari a "9" (qualunque altro rallenta-
  // mento rimane invariato).
  // N.B. Se il Percorso è stato invertito i Rallentamenti vengono
  //      calcolati al momento della compilazione del programma.
  //-------------------------------------------------------------------
  AGV[num_agv]->Rallentamenti();

  //-------------------------------------------------------------------
  // GESTIONE SALTO NODI A TGV PER OTTIMIZZAZIONE RALLENTAMENTI
  // Controllo il Percorso del TGV e inserisco l'informazione di
  // "SALTO A NODO" a tutti i punti la cui distanza dal nodo
  // successivo non è sufficiente per raggiundere "DistSaltaNodoInCm"
  //
  // N.B. Se c'è la gestione delle curve il salto nodi non è possibile
  //      per motivi di sicurezza.
  //-------------------------------------------------------------------
  #ifndef GESTIONE_CURVE
     AGV[num_agv]->SaltaNodi();
  #endif;

  //-------------------------------------------------------------------
  // BLOCCHI ROTAZIONE 180° SU TUTTI I PUNTI DI UN RETTILINEO
  // Verifico se sono presenti rotazioni a 180° su tratti dove non è
  // possibile farle.
  //-------------------------------------------------------------------
  punto_per_blocco_rot180=AGV[num_agv]->BloccoRotazioni180();
  if(punto_per_blocco_rot180 != 0){
     AGV[num_agv]->allarme_interno = ALL_INT_POS_NOT_OK;
     //-------------------------------------------
     // Azzero dati missione
     //-------------------------------------------
     AGV[num_agv]->mission.Chiamata = OldChiamata;
     AGV[num_agv]->free_all_nodi(all_mess);
     AGV[num_agv]->reset_ram_mission();
     //-------------------------------------------
     // Lascio traccia negli errori di sistema.
     //-------------------------------------------
     sprintf(all_mess, "%d :Impossible rotation at 180°...", punto_per_blocco_rot180);
     error_trasmit(num_agv, all_mess);
     return 0;
  }
  //----------------------------------------------------------------------
  // BLOCCO RIPARTENZE SU NODI IN CUI INIZIA UNA DEVIAZIONE
  // Il TGV non può ripartire dal nodo iniziale di un tratto inclinato
  //----------------------------------------------------------------------
  blocco_su_deviazione=AGV[num_agv]->BloccoSuDeviazione();
  if(blocco_su_deviazione != 0){
     AGV[num_agv]->allarme_interno = ALL_INT_POS_NOT_OK;
     //-------------------------------------------
     // Lascio traccia negli errori di sistema.
     //-------------------------------------------
     sprintf(all_mess, "%d :Can not start with a deviation...", blocco_su_deviazione);
     error_trasmit(num_agv, all_mess);
     return 0;
  }
  //----------------------------------------------------------------------
  // Se la missione può essere inizializzata prima di fare partire il
  // TGV libero tutti i nodi occupati
  //----------------------------------------------------------------------
  AGV[num_agv]->free_all_nodi(all_mess);
  //----------------------------------------------------------------------
  // Occupa i nodi del percorso con numero AGV. Se un punto è occupato da
  // altro AGV, allora battezza il punto precedente come punto intermedio
  // di fine.
  // ( setta anche "pintend" )
  //----------------------------------------------------------------------
  if(AGV[num_agv]->occupa_nodi(all_mess, forza_occupa_tutto_il_path, &agv_bloccante)){
     AGV[num_agv]->mission.Chiamata = OldChiamata;
     AGV[num_agv]->reset_ram_mission();
     return 1;
  }
  //-------------------------------------------------------------------
  // Setto eventuali agv bloccanti fino a che non sono sicuro di
  // aver inizializzato la missione
  //-------------------------------------------------------------------
  AgvBloccati[num_agv]=agv_bloccante;
  //-------------------------------------------------------------------
  // Gestione MINPERCORSI
  // se il nodo di partenza coincide con uno di quelli impostati
  // nella struttura "nodo_minpercorsi[]" di Graphvar.cpp allora
  // MINPERCORSI diventa quello impostato nella struttura
  //-------------------------------------------------------------------
  if(N.nodo_minpercorsi[AGV[num_agv]->mission.pstart]!=99){
     MinPercorsi = N.nodo_minpercorsi[AGV[num_agv]->mission.pstart];
  }

  //--------------------------------------------------------------
  // 11/04/2019
  // Garanzia di ingresso se Nodo successivo è Baia successiva
  //--------------------------------------------------------------
  if( AGV[num_agv]->mission.punto[1]==dest && AGV[num_agv]->mission.punto[2]==0 ){
     MinPercorsi=0;
  }

  //-------------------------------------------------------------------
  // Verifica che la missione intermedia superi un valore minimo
  // di nodi percorso, altrimenti SKIP 06/04/95
  //
  // AL-24/02 Controllo il MinPercorsi sono le le missioni standard
  //          e non quelle di fuori ingombro
  //-------------------------------------------------------------------
  if(deadlock==false){
     if(AGV[num_agv]->test_percorso_to_target(&metri)<MinPercorsi || AGV[num_agv]->mission.pintend==0){
        AGV[num_agv]->mission.Chiamata = OldChiamata;
        AGV[num_agv]->free_all_nodi(all_mess);
        AGV[num_agv]->reset_ram_mission();
        return 0;
     }
  }

  //-----------------------------------------------------------------------------------
  // 08/07/2016 SICUREZZA: Se MinPercorsi > 0 controllo che PStart sia diverso da PInt
  // Altrimenti SKIP --> per assicurarsi di non inizializzare missione  (Rick)
  //-----------------------------------------------------------------------------------
  if(MinPercorsi>0 && AGV[num_agv]->stato.pos!=dest && AGV[num_agv]->mission.pintend!=0 && AGV[num_agv]->mission.pintend!=AGV[num_agv]->mission.PIntForzato){
     if(AGV[num_agv]->mission.pintstart==AGV[num_agv]->mission.pintend ){
        AGV[num_agv]->mission.Chiamata = OldChiamata;
        AGV[num_agv]->free_all_nodi(all_mess);
        AGV[num_agv]->reset_ram_mission();
        AgvBloccati[num_agv]=agv_bloccante;
        return 0;
     }
  }

  //-----------------------------------------------------------------------------------
  // AL-13/12/2018 SICUREZZA: Anche se MinPercorsi==0, ma il TGV non riesce a
  // occupare nessun nodo (pintend==0) resetto la missione
  //-----------------------------------------------------------------------------------
  if(AGV[num_agv]->stato.pos!=dest && AGV[num_agv]->mission.pintend==0){
     AGV[num_agv]->mission.Chiamata = OldChiamata;
     AGV[num_agv]->free_all_nodi(all_mess);
     AGV[num_agv]->reset_ram_mission();
     return 0;
  }

  //-------------------------------------------------------------------
  // AZZERAMENTO PROVENIENZA TGV
  // Tutte le volte che il TGV parte per una nuova missione senza
  // niente a bordo, azzero la provenienza, altrimenti lascio
  // impostata l'ultima (server per l'ottimizzazione magazzini
  //-------------------------------------------------------------------
  if( AGV[num_agv]->mission.PresCEST_A==LIBERA && AGV[num_agv]->mission.PresCEST_B==LIBERA ){
     AGV[num_agv]->mission.Provenienza = NN;
  }

  //-------------------------------------------------------------------
  // Resetta Allarme Interno di destinazione non valida se TGV PARTE
  //-------------------------------------------------------------------
  if(AGV[num_agv]->allarme_interno == ALL_INT_DEST_NON_VALIDA){
     AGV[num_agv]->allarme_interno = 0;
  }

  //---------------------------------------------------------
  // LOG SCELTA TGV LIBERO DA RICARICA
  // n.b.: prima del reset TimeIniAttesa
  //---------------------------------------------------------
  if(CercaAgvLiberoInRicarica>0 && AGV[num_agv]->stato.pos<51){
     sprintf(stringa_log, "DEST:%4d - (TGV%02d) %013dsec, %3d%", dest, num_agv, AGV[num_agv]->mission.TimeInitAttesa, AGV[num_agv]->mission.PercCaricaBatt);
     for(i=1; i<=MAXAGV; i++){
        if(i==num_agv           ) continue;
        if(AGV[i]->stato.pos>50 ) continue;
        sprintf(buffer, ", (TGV%02d) %013dsec, %3d%", i, AGV[i]->mission.TimeInitAttesa, AGV[i]->mission.PercCaricaBatt);
        strcat(stringa_log, buffer);
     }
     file_cronologico_generico("LOGMISRIC.TXT", stringa_log, 80000L);
  }

  //-------------------------------------------------------------------
  // - Aggiorna la data di inizio missione.
  // - Incremento delle missioni giornaliere.
  //-------------------------------------------------------------------
  AGV[num_agv]->mission.TGVInCambioMissione = false;
  AGV[num_agv]->mission.TimeInitAttesa      = 0;  // --- Time di inizio attesa per macchina in blocco
  AGV[num_agv]->mission.TimeInitMission     = time(NULL);
//12/02/2013 Conteggio missioni solo su manovre di scarico
//per avere l'effettivo numero box movimentati
//AGV[num_agv]->mission.NumMissioni++;

  //-------------------------------------------------------------------
  // Salva la missione su file DOS ( MISSIONEn.DAT  dove n=num_AGV )
  // La trasmissione della missione all'AGV sarà in Maintenance_mission().
  //-------------------------------------------------------------------
  if(AGV[num_agv]->save_mission(all_mess)){
     AGV[num_agv]->mission.Chiamata = OldChiamata;
     AGV[num_agv]->reset_ram_mission();
     return 1;
  }

  //-------------------------------------------------------------------
  // Se arrivo a questo punto della funzione ho la certezza di avere
  // Inizializzato correttamente la MISSIONE.
  //-------------------------------------------------------------------
  *OK=true;
  history(num_agv, "START");           // Traccia operazioni TGV
  //-------------------------------------------------------
  if(*OK){
     bool trovato=false;
     char Operazione[101]="";
     char Coordinate[101]="";

     switch (tipo_mission){
        case MISS_CARICO_DA_MACCHINA:    sprintf(Operazione,"Start Miss.Pickup Machine"  );                break;
        case MISS_SCARICO_SU_MACCHINA:   sprintf(Operazione,"Start Miss.Dropoff Machine" );                break;
        case MISS_CARICO_DA_MAGAZZINO:   sprintf(Operazione,"Start Miss.Pickup Storage"  ); trovato=true;  break;
        case MISS_SCARICO_SU_MAGAZZINO:  sprintf(Operazione,"Start Miss.Dropoff Storage" ); trovato=true;  break;
        case MISS_CARICO_DA_BAIA:        sprintf(Operazione,"Start Miss.Pickup Bay"      ); trovato=true;  break;
        case MISS_SCARICO_SU_BAIA:       sprintf(Operazione,"Start Miss.Dropoff Bay"     ); trovato=true;  break;
        case MISS_SOLO_POSIZIONAMENTO:   sprintf(Operazione,"Start Miss.Positioning"     );                break;
        default:                         sprintf(Operazione,"Start Miss.Unknown "        );                break;
     }
     if(trovato==true){
        LogMovimenti(Operazione,Coordinate,num_agv, AGV[num_agv]->mission.BoxA, AGV[num_agv]->mission.BoxB, AGV[num_agv]->mission.TypPresa,0);
     }
  }
  //-------------------------------------------------------
  return 0 ;
}


// ---------------------------------------
//    InizializzaMissioneFuoriIngombro()
// ---------------------------------------
// Inizializzazione di una missione verso il Magazzino, Ricarica, o Falso Obiettivo più
// vicino
//
int process::InizializzaMissioneFuoriIngombro(short int num_agv, bool *Ok, char *all_mess)
/**********************************************************************************************/
{
  int err;
  int x1, y1, x2, y2;
  bool  perc_libero, ingresso;
  bool  baia_ok, magazzino_ok;
  bool  nodo_prenotato;
  bool  ResetMissione;
  short int p1, p2, Pos;
  double Dist, DistX, DistY;
  short int tot_bloccati;
  short int agv_bloccato;
  short int agv_bloccante, rot_su_start;
  int MaxDistFuoriIngombro=MAX_DIST_FUORI_INGOMBRO;
  short int i, j, k, z, altro_agv, limitrofo, MinPercorsi, MinPercToStart, nodo_interr;
  short int PContrapposto, Punto, PuntoPrec, PuntoMag, PuntoFuoriIngombro;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  short int num_rot=0, new_dist=0, old_dist=9999;
  struct NOD nod;
  WORD dm[3];

  //--------------------------------------------------------
  // Inizializzazione delle variabili
  //--------------------------------------------------------
  err=0;
  *Ok=false;
  altro_agv=limitrofo=0;
  PuntoFuoriIngombro=0;
  agv_bloccante=AgvBloccati[num_agv];
  memset(&nod, 0, sizeof(NOD));
  //-----------------------------------------------------------
  // Se TGV CARICO cerco MAGAZZINO DOVE FARE UN POSIZIONAMENTO
  // SEMPLICE o FALSO OBIETTIVO
  //-----------------------------------------------------------
  num_rot=0, new_dist=0, old_dist=9999;
  memset(&nodi_perc, 0, sizeof(nodi_perc));
  memset(&dati_perc, 0, sizeof(dati_perc));
  for(i=1; i<=TOTPUNTI; i++){
     //---------------------------------------------------
     // Verifico se il nodo esiste
     //---------------------------------------------------
     if(N.cerca_record(&nod, i) ) continue;
     //---------------------------------------------------
     // Scarto tutti i punti con blocco
     // rotazione a 180°
     //---------------------------------------------------
     if(nod.blocco_rot==1) continue;
     //------------------------------------
     // Scarto nodi troppo distanti
     //------------------------------------
     if(MaxDistFuoriIngombro<99){
        x1 = Punti[ AGV[num_agv]->stato.pos ][0];
        y1 = Punti[ AGV[num_agv]->stato.pos ][1];
        x2 = Punti[ nod.num ][0];
        y2 = Punti[ nod.num ][1];
        //----------------------------------------------
        // recupera distanza da ogni punto prenotato
        // da agv bloccato
        //----------------------------------------------
        DistX = (double) (x1-x2);
        DistY = (double) (y1-y2);
        if(DistX>30000 || DistY>30000) continue;
        Dist  = (double)(sqrt_algo((DistX*DistX)+(DistY*DistY)));
        if(Dist>(MaxDistFuoriIngombro*100)) continue;
     }   
     //---------------------------------------------------
     // POSIZIONEMANTO SEMPLICE verso il nodo
     // dell'impianto più vicino
     //---------------------------------------------------
     Punto=nod.num;
     memset(&nodi_perc, 0, sizeof(nodi_perc));
     memset(&dati_perc, 0, sizeof(dati_perc));
     //---------------------------------------------------
     // Se il punto coincide con la pos attuale del TGV
     // --> Skip!
     //---------------------------------------------------
     if(AGV[num_agv]->stato.pos==Punto) continue;
     //---------------------------------------------------
     // Verifico se il punto individuato è contrapposto
     // a un nodo già occupato o riservato
     //---------------------------------------------------
     altro_agv=0;
     nodo_prenotato=false;
     if(N.punti_notevoli[Punto]==true){
        PContrapposto=N.nodo_contrapposto[Punto];
        if(PContrapposto!=0 && N.punti_notevoli[PContrapposto]==true){
           //---------------------------------------------------------------
           // Verifico se il nodo contrapposto è riservato da un altro TGV
           //---------------------------------------------------------------
           if(P.test_dest_riservata(PContrapposto, &altro_agv, num_agv) && altro_agv!=0 && altro_agv!=num_agv) nodo_prenotato=true;
           //---------------------------------------------------------------
           // Verifico se il nodo contrapposto è occupato da un altro TGV
           //---------------------------------------------------------------
           if(P.test_dest_occupata(PContrapposto, &altro_agv) && altro_agv!=0 && altro_agv!=num_agv ) nodo_prenotato=true;
        }
     }
     if(nodo_prenotato==true) continue;
     //---------------------------------------------------
     // Se il nodo appartiene a un magazzino ci posso
     // andare con il TGV libero o il TGV carico se la
     // prima posizione è libera.
     // IMPORTANTE: il nodo deve essere fuori ingombro
     //             per posizionamento semplice
     //---------------------------------------------------
     magazzino_ok=false;
     for(j=1; j<=TOTMAG; j++){
        if(M.m[j]->Num<1                                      ) continue;
        if(M.m[j]->Num>TOTMAG                                 ) continue;
        if(M.m[j]->Npt==0                                     ) continue;
        if(M.m[j]->Tipo==NN                                   ) continue;
        if(M.m[j]->PuntoIng!=Punto && M.m[j]->PuntoOut!=Punto ) continue;
        //---------------------------------------------------
        // Memorizzo il nodo del magazzino considerato
        //---------------------------------------------------
        PuntoMag=0;
        ingresso=false;
        if(M.m[j]->PuntoIng==M.m[j]->PuntoOut){
           if(Punto==M.m[j]->PuntoIng){PuntoMag=M.m[j]->PuntoIng; ingresso=true;}
        }
        else{
           if(Punto==M.m[j]->PuntoIng){PuntoMag=M.m[j]->PuntoIng; ingresso=true;}
           if(Punto==M.m[j]->PuntoOut){PuntoMag=M.m[j]->PuntoOut; ingresso=false;}
        }
        if(PuntoMag==0) continue;
        Pos=99;
        // LATO DI INGRESSO
        if(ingresso==true){
           for(k=1; k<=MAXTERRAMAG; k++){
              if(M.m[j]->Pos[k].Presenza[0]==false) continue;
              Pos=k-1;
              break;
           }
           if(Pos<2) continue;      // prima posizione impegnata
        }
        // LATO DI USCITA
        else{
           for(k=M.m[j]->Npt; k>0; k--){
              if(M.m[j]->Pos[k].Presenza[0]==false) continue;
              Pos=k+1;
              break;
           }
           if(Pos>=M.m[j]->Npt) continue;  // prima posizione impegnata
        }
        //---------------------------------------------------
        // Nessuna posizione trovata --> skip!
        //---------------------------------------------------
        if(Pos==99) continue;
        //---------------------------------------------------
        // Per i magazzini MULTI NODO faccio sempre
        // riferimento al nodo più lontano
        //---------------------------------------------------
        if(MagazziniMultiNodo==true){
           if(ingresso==true) PuntoMag = M.m[j]->Pos[Pos].NodoIng;
           else               PuntoMag = M.m[j]->Pos[Pos].NodoOut;
        }
        //---------------------------------------------------
        // Per recuperare la distanza simulo il ritorno in
        // ricarica con partenza dal magazzino
        //---------------------------------------------------
        new_dist=num_rot=0;
        memset(&nodi_perc, 0, sizeof(nodi_perc));
        memset(&dati_perc, 0, sizeof(dati_perc));
        err = AGV[num_agv]->distanza(STD_PATH, PuntoMag, AGV[num_agv]->mission.PosRicarica, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
        if(new_dist!=0 && !err && new_dist<MAXPERCORSI){
           p1 = nodi_perc[0];
           p2 = nodi_perc[1];
           x1 = Punti[p1][0];
           y1 = Punti[p1][1];
           x2 = Punti[p2][0];
           y2 = Punti[p2][1];
           //-------------------------------------------------------------
           // Calcolo delle distanze da punto a punto
           //-------------------------------------------------------------
           DistX = abs(y1-y2);
           DistY = abs(x1-x2);
           if(DistX>30000 || DistY>30000) continue;
           Dist  = (double)(sqrt((DistX*DistX)+(DistY*DistY)));
           //-------------------------------------------------------------
           // Memorizzo la distanza tra posizione di partenza e primo
           // nodo di percorso
           //
           // Se la distanza è inferiore a quella di rot singola skip!!
           //
           // NOTA: tranne nel caso dei multi nodo
           //-------------------------------------------------------------
           if(Dist<DIST_MIN_ROT_SINGOLA) continue;
           magazzino_ok=true;
           break;
        }
     }
     //---------------------------------------------------
     // Se il nodo appartiene a un magazzino ci posso
     // andare con il TGV libero o il TGV carico se la
     // prima posizione è libera.
     // IMPORTANTE: il nodo deve essere fuori ingombro
     //             per posizionamento semplice
     //---------------------------------------------------
     baia_ok=false;
     for(j=1; j<=TOTBAIE; j++){
        if(magazzino_ok==true           ) break;
        if(P.ba[j]->Num<1               ) continue;
        if(P.ba[j]->Num>TOTBAIE         ) continue;
        if(P.ba[j]->Punto!=Punto        ) continue;
        if(P.ba[j]->Esclusione==true    ) continue;
        if(P.ba[j]->Tipo!=BA_DEPOSITO   ) continue;
        if(P.ba[j]->StatoPres!=NN       ) continue;
        //---------------------------------------------------
        // Per recuperare la distanza simulo il ritorno in
        // ricarica con partenza dal magazzino
        //---------------------------------------------------
        new_dist=num_rot=0;
        memset(&nodi_perc, 0, sizeof(nodi_perc));
        memset(&dati_perc, 0, sizeof(dati_perc));
        err = AGV[num_agv]->distanza(STD_PATH, Punto, AGV[num_agv]->mission.PosRicarica, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
        if(new_dist>0 && new_dist<MAXPERCORSI && !err){
           p1 = nodi_perc[0];
           p2 = nodi_perc[1];
           x1 = Punti[p1][0];
           y1 = Punti[p1][1];
           x2 = Punti[p2][0];
           y2 = Punti[p2][1];
           //-------------------------------------------------------------
           // Calcolo delle distanze da punto a punto
           //-------------------------------------------------------------
           DistX = abs(y1-y2);
           DistY = abs(x1-x2);
           if(DistX>30000 || DistY>30000) continue;
           Dist  = (double)(sqrt((DistX*DistX)+(DistY*DistY)));
           //-------------------------------------------------------------
           // Memorizzo la distanza tra posizione di partenza e primo
           // nodo di percorso
           //-------------------------------------------------------------
           if(Dist<DIST_MIN_ROT_SINGOLA) continue;
           baia_ok=true;
           break;
        }
     }
     //--------------------------------------------------------------
     // Con il TGV carico scarto le ricariche
     //--------------------------------------------------------------
     if(Punto<51 && (AGV[num_agv]->stato.s.bit.carA || AGV[num_agv]->stato.s.bit.carB)){
        Punto=0;
     }
     //--------------------------------------------------------------
     // SCARTO COME POSSIBILI FUORI INGOMBRI TUTTI I PUNTI TRANNE
     // LE RICARICHE E I MAGAZZINI NON OK
     //--------------------------------------------------------------
     if(N.punti_notevoli[Punto]==true && (Punto<1 || Punto>50) && magazzino_ok==false && baia_ok==false){
        Punto=0;
     }
     //--------------------------------------------------------------
     // Se Nodo non trovato --> Skipp!!!
     //--------------------------------------------------------------
     if(Punto==0) continue;
     //--------------------------------------------------------------
     // PER I PUNTI NOTEVOLI CONTROLLO ANCHE DEST. RISERVATA E
     // LIMITROFA, per i punti di percorso controllo solo i nodi
     // occupati
     //--------------------------------------------------------------
     if(N.punti_notevoli[Punto]==true){
        if(test_dest_limitrofe(Punto, &altro_agv, &limitrofo, num_agv)) continue;
        if(test_dest_riservata(Punto, &altro_agv, num_agv) && altro_agv!=0 && num_agv!=altro_agv) continue;
     }
     //--------------------------------------------------------------
     // Verifico se nodo occupato
     //--------------------------------------------------------------
     err = test_dest_occupata(Punto, &altro_agv);
     if((err != 0) && (num_agv != altro_agv)) continue;
     //--------------------------------------------------------------
     // Se il punto trovato non è terminale, deve essere distante
     // almeno DIST_MIN_ROT_DOPPIA_MAX da ogni nodo prenotato da
     // altri TGV bloccati
     //--------------------------------------------------------------
     if(!N.punti_notevoli[Punto]){
        perc_libero=true;
        //---------------------------------------------------
        // Valuto solo TGV bloccati
        //---------------------------------------------------
        x1 = Punti[ Punto ][0];
        y1 = Punti[ Punto ][1];
        for(j=1; j<=MAXAGV; j++){
           if(AgvBloccati[j]<1       ) continue;
           if(AgvBloccati[j]>MAXAGV  ) continue;
           if(AgvBloccati[j]==num_agv) continue;
           //---------------------------------------------------
           // Verifico che il nodo scelto non sia prenotato
           // da un altro TGV bloccato
           //---------------------------------------------------
           for(k=0; k<MAXPERCORSI; k++){
              if(AGV[j]->mission.punto_prenotato[k]==0) break;
              x2 = Punti[ AGV[j]->mission.punto_prenotato[k] ][0];
              y2 = Punti[ AGV[j]->mission.punto_prenotato[k] ][1];
              //----------------------------------------------
              // recupera distanza da ogni punto prenotato
              // da agv bloccato
              //----------------------------------------------
              DistX = (double) (x1-x2);
              DistY = (double) (y1-y2);
              if(DistX>30000 || DistY>30000) continue;
              Dist  = (double)(sqrt((DistX*DistX)+(DistY*DistY)));
              if( Dist> DIST_MIN_ROT_DOPPIA_MAX ) continue;
              perc_libero=false;
              break;
           }
           if(perc_libero==false) break;
        }
        if(perc_libero==false) continue;       // nodo non valido
     }
     //--------------------------------------------------------------
     // Cerco tra tutti quelli trovati il più vicino a una dist
     // almeno di 4 nodi per essere "sicuro" di allontanarmi a
     // una distanza sufficiente per sbloccare i TGV
     //--------------------------------------------------------------
     new_dist=num_rot=0;
     memset(&nodi_perc, 0, sizeof(nodi_perc));
     memset(&dati_perc, 0, sizeof(dati_perc));
     err = AGV[num_agv]->distanza(STD_PATH, 0, Punto, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
     if(((!N.punti_notevoli[Punto] && new_dist>=MIN_NODI_SBLOCCO_DEADLOCK) || N.punti_notevoli[Punto]) && new_dist<old_dist && !err && new_dist<MAXPERCORSI){
        perc_libero=true;
        //------------------------------------------------------------
        // AL-20/12/2019  !!! IMPORTANTE !!!
        // Se il punto di partenza dei TGV ha il blocco rot 180
        // il primo passo programmo non deve prevedere varizioni
        // di rotazione di nessun tipo
        //------------------------------------------------------------
        if(N.n[AGV[num_agv]->stato.pos]->blocco_rot==true){
           rot_su_start=0;
           if(AGV[num_agv]->stato.s.bit.grd0   ) rot_su_start=0;
           if(AGV[num_agv]->stato.s.bit.grd90  ) rot_su_start=1;
           if(AGV[num_agv]->stato.s.bit.grd180 ) rot_su_start=2;
           if(AGV[num_agv]->stato.s.bit.grd270 ) rot_su_start=3;
           if(rot_su_start!=dati_perc[0].rot   ) perc_libero=false;
        }
        //------------------------------------------------------------
        // GESTIONE OCCUPAZIONE NODO CON "INGOMBRO AGV"
        // Controlli NON necessari
        //------------------------------------------------------------
        if(AbilitaGestioneIngombroCurvePerStopTGV<2){
           //------------------------------------------------------------
           // Verifico percorso completamente libero
           //------------------------------------------------------------
           for(j=1; j<=MIN_NODI_SBLOCCO_DEADLOCK; j++){
              if(nodi_perc[j]==0   ) break;
              if(perc_libero==false) break;
              //------------------------------------------------
              // Nodo occupato da altro TGV
              //------------------------------------------------
              if(N.nodo_busy[nodi_perc[j]]!=0 && N.nodo_busy[nodi_perc[j]]!=num_agv){
                 perc_libero=false;
                 break;
              }
              //------------------------------------------------
              // Circostante = PT IN INGOMBRO OCCUPATO
              //------------------------------------------------
              if(N.punti_notevoli[nodi_perc[j]]==true && N.pt_in_ingombro[nodi_perc[j]]==true){
                 if(N.nodo_circostante[nodi_perc[j]][0]!=0 && N.nodo_busy[N.nodo_circostante[nodi_perc[j]][0]]!=0 && N.nodo_busy[N.nodo_circostante[nodi_perc[j]][0]]!=num_agv){
                    perc_libero=false;
                    break;
                 }
                 if(N.nodo_circostante[nodi_perc[j]][1]!=0 && N.nodo_busy[N.nodo_circostante[nodi_perc[j]][1]]!=0 && N.nodo_busy[N.nodo_circostante[nodi_perc[j]][1]]!=num_agv){
                    perc_libero=false;
                    break;
                 }
                 if(N.nodo_circostante[nodi_perc[j]][2]!=0 && N.nodo_busy[N.nodo_circostante[nodi_perc[j]][2]]!=0 && N.nodo_busy[N.nodo_circostante[nodi_perc[j]][2]]!=num_agv){
                    perc_libero=false;
                    break;
                 }
                 if(N.nodo_circostante[nodi_perc[j]][3]!=0 && N.nodo_busy[N.nodo_circostante[nodi_perc[j]][3]]!=0 && N.nodo_busy[N.nodo_circostante[nodi_perc[j]][3]]!=num_agv){
                    perc_libero=false;
                    break;
                 }
              }
              //------------------------------------------------
              // AL-28/02
              // In tutti i punti in cui il TGV deve compiere
              // una rotazione ci deve essere la distanza
              // Rotazione doppia
              //------------------------------------------------
              if(dati_perc[j-1].rot!=dati_perc[j].rot){
                 //-------------------------------------------------------------
                 // Coordinate del punto su cui il TGV deve ruotare
                 //-------------------------------------------------------------
                 x1 = Punti[nodi_perc[j  ]][0];
                 y1 = Punti[nodi_perc[j  ]][1];
                 //-------------------------------------------------------------
                 // Nessun nodo occupato dell'impianto deve essere in prossimità
                 // dei punti di rotazione del TGV
                 //-------------------------------------------------------------
                 for(k=1; k<=TOTPUNTI; k++){
                    if(k==nodi_perc[j  ]       ) continue;
                    if(N.nodo_busy[k]==0       ) continue;
                    if(N.nodo_busy[k]==num_agv ) continue;
                    //-------------------------------------------------------------
                    // Coordinate degli altri punti impegnati dai TGV
                    //-------------------------------------------------------------
                    x2 = Punti[k][0];
                    y2 = Punti[k][1];
                    //-------------------------------------------------------------
                    // Calcolo delle distanze da punto a punto
                    //-------------------------------------------------------------
                    DistX = abs(y1-y2);
                    DistY = abs(x1-x2);
                    if(DistX>30000 || DistY>30000) continue;
                    Dist = (double)(sqrt((DistX*DistX)+(DistY*DistY)));
                    //-------------------------------------------------------------
                    // Se la distanza è inferiore a DIST_MIN_ROT_DOPPIA_MAX:
                    // --> perc_libero=false;
                    //-------------------------------------------------------------
                    if(Dist>=DIST_MIN_ROT_DOPPIA_MAX) continue;
                    perc_libero=false;
                    break;
                 }
                 if(perc_libero==false) break;
              }
           }
        }
        //------------------------------------------------------------
        // Verifico se la destinazione è in senso opposto
        // al TGV che mi sta bloccando
        //------------------------------------------------------------
        if(perc_libero==true && !N.punti_notevoli[Punto] && agv_bloccante>0 && agv_bloccante<=MAXAGV){
           PuntoPrec=0;
           for(j=1; j<MAXPERCORSI; j++){
              if(nodi_perc[j]==0    ) break;
              if(nodi_perc[j]!=Punto) continue;
              PuntoPrec=nodi_perc[j-1];
              break;
           }
           for(j=1; j<MAXPERCORSI; j++){
              if(PuntoPrec==0                           ) break;
              if(AGV[agv_bloccante]->mission.punto[j]==0) break;
              if(AGV[agv_bloccante]->mission.punto[j]!=PuntoPrec && AGV[agv_bloccante]->mission.punto[j-1]!=Punto) continue;
              perc_libero=false;
              break;
           }
        }
        //------------------------------------------------------------
        // Verifico se effettivamente il Nodo trovato è raggiungibile
        //------------------------------------------------------------
        AGV[num_agv]->test_percorso_disponibile(nodi_perc, dati_perc, &altro_agv, &MinPercorsi, &MinPercToStart, &nodo_interr, all_mess);
        if(perc_libero==true && MinPercorsi>=MinPercToStart){
           old_dist = new_dist;
           PuntoFuoriIngombro = Punto;
        }
     }
  }
  //------------------------------------------------------------------
  // Inizializzazione della missione verso il Punto di Fuori Ingombro
  //------------------------------------------------------------------
  if(PuntoFuoriIngombro!=0){
     ResetMissione=true;
     //----------------------------------------------------------
     // TGV IN MANUALE --> SKIP
     //----------------------------------------------------------
     if(!AGV[num_agv]->stato.s.bit.funz) ResetMissione=false;
     //----------------------------------------------------------
     // Se ci sono le condizioni per eseguire un Reset delle
     // Missioni al volo la prima cosa che faccio è trasmettere
     // il Reset al PLC (DM98) in modo da bloccare il reset
     // della missione in RAM in caso di errore di comunicazione
     //
     // NOTA: tolgo il controllo di TGV su nodo perchè
     //       sui nodi delle deviazioni non sempre ce l'ho.
     //
     //----------------------------------------------------------
//   if(ResetMissione==true && !AGV[num_agv]->stato.s.bit.sunod           ) ResetMissione=false;
     if(ResetMissione==true && AGV[num_agv]->trasmetti_reset_AGV(all_mess)) ResetMissione=false;
     //----------------------------------------------------------
     // SICUREZZA
     // Lancio due volte consecutive il reset_mission al TGV
     // per assicurarmi che lo abbia fatto
     //----------------------------------------------------------
     if(ResetMissione==true && AGV[num_agv]->trasmetti_reset_AGV(all_mess)) ResetMissione=false;
     //----------------------------------------------------------
     // Verifico lo stato del DM91 sul TGV per vedere se
     // l'azzeramento è andato a buon fine.
     //----------------------------------------------------------
     if(ResetMissione==true && !break_Com[COM1]){
        memset(&dm[0], 0, sizeof(dm));
        err = OM.ld_canale(&dm[0], (WORD)(num_agv), "RD", 91, 2, all_mess);
        //-------------------------------------------------
        // Se la missione non è stata azzerata --> Skip!
        // 1 - errore di comunicazione
        // 2 - DM91!=0
        // 3 - OK PROGRAMMA==0 (bit5 DM92)
        //-------------------------------------------------
        if(err!=0 || dm[0]!=0 || !(TestBit((char *) &dm[1], 5))) ResetMissione=false;
     }
     //----------------------------------------------------------
     // Se ho le Condizioni per azzerare la missione
     //----------------------------------------------------------
     if(ResetMissione==true){
        //-----------------------------------------
        // Azzeramento delle missioni precedenti
        //-----------------------------------------
        AGV[num_agv]->reset_mission(all_mess);
        AGV[num_agv]->stato.start=0;
        AGV[num_agv]->stato.s.bit.okprog=1;
        //-----------------------------------------------------------------
        // AL-23/02
        // Recupero l'agv che sta bloccando la navetta che il sistema
        // vuole spostare per lasciarne traccia in fase di annullamento
        // della vecchia missione per la dettatura della nuova
        //-----------------------------------------------------------------
        tot_bloccati=0;
        agv_bloccato=0;
        for(i=1; i<=MAXAGV; i++){
           if(i==num_agv              ) continue;
           if(AgvBloccati[i]!=num_agv ) continue;
           agv_bloccato=i;
           tot_bloccati++;
        }
        //-----------------------------------------------------------------
        // Lascio una traccia su file di un avvenuta Dead Lock.
        //-----------------------------------------------------------------
        sprintf(all_mess, "Dead Lock - (TGV:%d, PUNTO:%d, BLOCCANTE:%d, BLOCCATO:%d, TOT:%d)", num_agv, PuntoFuoriIngombro, AgvBloccati[num_agv], agv_bloccato, tot_bloccati);
        error_trasmit(num_agv, all_mess);
        //-----------------------------------------------------------------
        // INIZIALIZZAZIONE DELLA MISSIONE COME POSIZIONAMENTO SEMPLICE
        //-----------------------------------------------------------------
        if(new_init_mission(TRUE, CALCOLA_TIPO_PATH, num_agv, AGV[num_agv]->mission.Chiamata, PuntoFuoriIngombro, 0, 0, MISS_SOLO_POSIZIONAMENTO, 0, Ok, all_mess)) return 1;
        //-----------------------------------------------------------------
        // Anche se l'inizializzazione della missione fuori ingombro
        // ha avuto esito positivo, ma non sono stati occupati almeno
        // un nodo tento di spostare un altro TGV
        //-----------------------------------------------------------------
        if(!AGV[num_agv]->test_mission_on()) *Ok=false;
        else{
           //----------------------------------------------
           // Primo nodo non occupato
           //----------------------------------------------
           if(N.nodo_busy[AGV[num_agv]->mission.punto[1]]!=num_agv){
              *Ok=false;
           }
        }
     }
  }
  return 0;
}

// -----------------------------
//  PosizionamentoManualeAGV()
// -----------------------------
// Verifica che non ci siano missioni ON per l'AGV. Se non esistono
// forza il posizionamento dell'AGV
//
int process::PosizionamentoManualeAGV(short int num_agv, short int punto, short int TypPresa, bool *Ok, char *all_mess)
/*************************************************************/
{
  int chiamata;
  short int i;
  short int altro_agv;
  short int limitrofo;
  short int okprog;
  short int err = 0;
  bool nodo_fuori_ingombro;

  *Ok=false;
  //----------------------------------------------------
  // Test AGV pronto
  //----------------------------------------------------
  if(punto == 0) return 0;
  okprog = (short int)(AGV[num_agv]->stato.s.bit.okprog); // AGV pronto aricevere nuovo prg.
  if(!okprog) err = 1;
  if(!AGV[num_agv]->stato.s.bit.funz)  err = 1;           // Test se AGV in automatico
  if(AGV[num_agv]->test_mission_on())  err = 1;
  if(AGV[num_agv]->mission.Esclusione) err = 1;           // Test se escluso
  if(AGV[num_agv]->stato.pos == 0)     err = 1;
  if(err) {strcpy(all_mess, MESS[356]); return 1;}

  //----------------------------------------------------
  // (1) GESTIONE FORCA DOPPIA  CON CARICO DOPPIO
  // (2) GESTIONE FORCA SINGOLA CON CARICO DOPPIO
  //----------------------------------------------------
  if(GestioneDoppiaForcaTGV!=2){
     //----------------------------------------------------
     // Errore mancanza cestone scaricato manualmente
     //----------------------------------------------------
     if(!AGV[num_agv]->stato.s.bit.carA && AGV[num_agv]->mission.PresCEST_A!=LIBERA){
        strcpy(all_mess, MESS[360]);
        return 1;
     }
     if(!AGV[num_agv]->stato.s.bit.carB && AGV[num_agv]->mission.PresCEST_B!=LIBERA){
        strcpy(all_mess, MESS[360]);
        return 1;
     }
     //----------------------------------------------------
     // Errore caricamento manuale di un cestone
     //----------------------------------------------------
     if(AGV[num_agv]->stato.s.bit.carA && AGV[num_agv]->mission.PresCEST_A==LIBERA){
        strcpy(all_mess, MESS[361]);
        return 1;
     }
     if(AGV[num_agv]->stato.s.bit.carB && AGV[num_agv]->mission.PresCEST_B==LIBERA){
        strcpy(all_mess, MESS[361]);
        return 1;
     }
  }
  else{
     //----------------------------------------------------
     // Errore mancanza cestone scaricato manualmente
     //----------------------------------------------------
     if(!AGV[num_agv]->stato.s.bit.carB && (AGV[num_agv]->mission.PresCEST_B!=LIBERA || AGV[num_agv]->mission.PresCEST_A!=LIBERA)){
        strcpy(all_mess, MESS[360]);
        return 1;
     }
     //----------------------------------------------------
     // Errore caricamento manuale di un cestone
     //----------------------------------------------------
     if(AGV[num_agv]->stato.s.bit.carB && (AGV[num_agv]->mission.PresCEST_B==LIBERA && AGV[num_agv]->mission.PresCEST_A==LIBERA)){
        strcpy(all_mess, MESS[361]);
        return 1;
     }
  }
  //---------------------------------------
  // Verifico se la destinazione è
  // un punto fuori ingombro
  //---------------------------------------
  nodo_fuori_ingombro=false;
  for(i=0; i<MAXNODIATTESA; i++){
     if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]<1         ) continue;
     if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]>MAXPUNTI  ) continue;
     if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]!=punto    ) continue;
     nodo_fuori_ingombro=true;
     break;
  }

  //---------------------------------------
  // Test destinazione occupata
  //---------------------------------------
  if(nodo_fuori_ingombro==false || N.punti_notevoli[punto]==true){
     if(P.test_dest_riservata(punto, &altro_agv, num_agv)){
        if(num_agv != altro_agv) err = 1;
     }
     if(P.test_dest_occupata(punto, &altro_agv)){
        if(num_agv != altro_agv) err = 1;
     }
     //---------------------------------------
     // Test destinazione limitrofe
     //---------------------------------------
     if(P.test_dest_limitrofe(punto, &altro_agv, &limitrofo, num_agv)){
        if(num_agv != altro_agv) err = 1;
     }
     if(err){
        AGV[num_agv]->allarme_interno=ALL_INT_DEST_IMPEGNATA;
        strcpy(all_mess, MESS[365]);
        return 1;
     }
  }

  //---------------------------------------
  // Passo la chiamata già abbinata al TGV
  //---------------------------------------
  chiamata = AGV[num_agv]->mission.Chiamata;  //???

  //---------------------------------------
  // scatena missione ( AGV -> su posizione
  // viene comunque abbinata un tipo presa
  //---------------------------------------
  if(new_init_mission(FALSE, CALCOLA_TIPO_PATH, num_agv, chiamata, punto, 0, 0, MISS_SOLO_POSIZIONAMENTO, 0, Ok, all_mess)) return 1;
  return 0;
}

// -----------------------------
//  CarScarMacManualeAGV()
// -----------------------------
// Lancia una missione di carico/scarico verso macchina  (lo stabilisce
// autonomamente a seconda che l'AGV sia carico o scarico) facendo un test di
// presenza cestone, richiesta cestone da parte della macchina, e destinazione
// occupata
//
int  process::CarScarMacManualeAGV(short int num_agv, short int num_mac, short int TypPresa, bool *Ok, char *all_mess)
{
  int err=0;
  int carico;
  bool lato_ingresso;
  bool lato_uscita;
  int chiamata;
  short int num_stz, dest;
  short int altro_agv, limitrofo, okprog;
  short int tipo_mission;
  int       i;
  bool      OkChiamata;
  short int Posizione;
  short int Piano;
  short int PosizioneTgv=0;
  struct CHIAMATE mc;


  dest=0;
  *Ok=false;
  num_stz=0;
  //-----------------------------------------
  // Verifico attendibilità della MAC
  //-----------------------------------------
  memset(&mc, 0, sizeof(CHIAMATE));
  if( P.read_chiamate( num_mac, &mc, all_mess)) return 1;

  //-----------------------------------------
  // Test AGV pronto
  //-----------------------------------------
  if(AGV[num_agv]->load_stato(all_mess)) err = 1;
  okprog =(short int)(AGV[num_agv]->stato.s.bit.okprog);              // AGV pronto aricevere nuovo prg.
  if(!okprog) err = 1;
  if(!AGV[num_agv]->stato.s.bit.funz)  err = 1;                       // Test se AGV in automatico
  if(AGV[num_agv]->test_mission_on())  err = 1;
  if(AGV[num_agv]->mission.Esclusione) err = 1;                       // Test se escluso
  if(AGV[num_agv]->stato.pos == 0)     err = 1;
  if(err) {strcpy(all_mess, MESS[356]); return 1;}
  //-----------------------------------------
  // Recupero se la posizione analizzata del
  // TGV è CARICA o SCARICA
  // 0 = Scarica
  // 1 = ForcaB
  // 2 = ForcaA
  // 3 = ForcaAB
  //-----------------------------------------
  carico=0;
  //if(TestBit((char *) AGV[num_agv]->sbit, 13)) carico=1;  // carico
  if( AGV[num_agv]->sbit->carA                             ) carico=N_FORCA_A;
  if( AGV[num_agv]->sbit->carB                             ) carico=N_FORCA_B;
  if( AGV[num_agv]->sbit->carA && AGV[num_agv]->sbit->carB ) carico=N_FORCA_AB;

  //---------------------------------------------------------
  // Controllo TIPO PRESA
  // coerenza tra tipo di presa (FORCA) e stato di carico
  //---------------------------------------------------------
  if((TypPresa<N_FORCA_B || TypPresa>N_FORCA_AB)||
     (TypPresa==N_FORCA_A  && ( carico==N_FORCA_B || carico==N_FORCA_AB ))|| // TypPresa 1 e prsenza su forca 2
     (TypPresa==N_FORCA_AB && ( carico==N_FORCA_A || carico==N_FORCA_B  ))){ // TypPresa 3 e forche non VUOTE o non PIENE
     sprintf(all_mess, MESS[358] );                // Errore Tipo Presa (Numero Forca) sbagliato
     return 1;
  }

  tipo_mission=MISS_CARICO_DA_MACCHINA;
  if( carico==N_FORCA_A && TypPresa==N_FORCA_A ) tipo_mission=MISS_SCARICO_SU_MACCHINA; // Se ho la presenza su forca A e lavoro con forca A--> SCARICO
  if( carico==N_FORCA_B                        ) tipo_mission=MISS_SCARICO_SU_MACCHINA; // Se ho la presenza su forca B --> SCARICO
  if( carico==N_FORCA_AB                       ) tipo_mission=MISS_SCARICO_SU_MACCHINA; // Se ho la presenza su forca B --> SCARICO


  //-------------------------------------------
  // Postazione non in chiamata --> skip!!
  //
  // recupero anche il numero della stazione
  // con il tipo di chiamata richiesto
  //-------------------------------------------
  OkChiamata=false;
  for(i=0; i<MAXSTAZMAC; i++){                       // per tutte le stazioni
     if(mc.Esclusione==true           ) break;
     if(mc.Staz[i].PuntoIng<1         ) continue;
     if(mc.Staz[i].PuntoIng>TOTPUNTI  ) continue;
     if(mc.Staz[i].PuntoOut<1         ) continue;
     if(mc.Staz[i].PuntoOut>TOTPUNTI  ) continue;
     if(mc.Staz[i].Esclusione==true   ) continue;
     if(tipo_mission==MISS_SCARICO_SU_MACCHINA){
        if(mc.Staz[i].Richiesta==false ) continue;   // nessuna richiesta cestone
     }
     if(tipo_mission==MISS_CARICO_DA_MACCHINA){
        if(mc.Staz[i].Espulsione==false) continue;   // nessuna espulsione cestone
     }
     //---------------------------
     // Chiamata trovata
     //---------------------------
     OkChiamata=true;
     num_stz=i;
     break;
  }
  //---------------------------------------------------
  // Nessuna chiamata idonea trovata
  //---------------------------------------------------
  if(OkChiamata==false){
     strcpy(all_mess, MESS[357]);
     return 1;
  }

  //---------------------------------------------------
  // Gestione lato prelievo/deposito in magazzino
  //---------------------------------------------------
  lato_ingresso = false;   // STANDARD
  lato_uscita   = false;

  //------------------------------------------------------
  // Destinazione missione
  // PuntoIng = Deposito
  // PuntoOut = prelievo
  //------------------------------------------------------
  dest=0;
  if( tipo_mission==MISS_SCARICO_SU_MACCHINA ){
     if(lato_uscita==false   ) dest = mc.Staz[num_stz].PuntoIng;
     if(lato_uscita==true    ) dest = mc.Staz[num_stz].PuntoOut;
  }
  if( tipo_mission==MISS_CARICO_DA_MACCHINA  ){
     if(lato_ingresso==false ) dest = mc.Staz[num_stz].PuntoOut;
     if(lato_ingresso==true  ) dest = mc.Staz[num_stz].PuntoIng;
  }

  //------------------------------------------------------
  // Locazione SBAGLIATA --> skip!!
  // Locazione non presente o NON adatta al tipo missione
  //------------------------------------------------------
  if(dest==0){                                                   // Destinazione NOT OK
     strcpy(all_mess, MESS[375]);
     return 1;
  }

  //---------------------------------------------------
  // Postazione non in disponibile --> skip!!
  // N.B. Le forche B possono prel/dep solo in basso
  //---------------------------------------------------
  OkChiamata=false;
  if( tipo_mission==MISS_CARICO_DA_MACCHINA ){
     OkChiamata=!P.VerificaTipoPrelievo(lato_ingresso, num_mac, num_stz, &Posizione, &PosizioneTgv, all_mess);
  }
  if( tipo_mission==MISS_SCARICO_SU_MACCHINA ){
     OkChiamata=!P.VerificaTipoDeposito(lato_uscita, num_mac, num_stz, &Posizione, &PosizioneTgv, all_mess);
  }
  if( OkChiamata==false){
     //strcpy(all_mess, MESS[357]);
     sprintf(all_mess, "Machine nr.%d, not ok!", num_mac);
     return 1;
  }


  //-------------------------------------------
  // Test destinazione occupata
  //-------------------------------------------
  if(P.test_dest_riservata(dest, &altro_agv, num_agv)){
     if(num_agv != altro_agv) err = 1;
  }
  if(P.test_dest_occupata(dest, &altro_agv)){
     if(num_agv != altro_agv) err = 1;
  }
  //---------------------------------------
  // Test destinazione limitrofe
  //---------------------------------------
  if(P.test_dest_limitrofe(dest, &altro_agv, &limitrofo, num_agv)){
     if(num_agv != altro_agv) err = 1;
  }
  if(err) {strcpy(all_mess, MESS[365]); return 1;}

  //----------------------------------------------
  // Forzo il piano di deposito e recupero la
  // chiamata dal nodo di destinazione
  //----------------------------------------------
  TestNumChiamata(&chiamata, dest, mc.Num);
  //----------------------------------------------
  // Inizializzazione missione
  //----------------------------------------------
  if(new_init_mission(FALSE, CALCOLA_TIPO_PATH, num_agv, chiamata, dest, Posizione, 0, tipo_mission, TypPresa, Ok, all_mess)) return 1;

  return 0;
}


// -----------------------------
//  CarScarBaiaManualeAGV()
// -----------------------------
// Lancia una missione di carico/scarico verso macchina  (lo stabilisce
// autonomamente a seconda che l'AGV sia carico o scarico) facendo un test di
// presenza cestone, richiesta cestone da parte della macchina, e destinazione
// occupata
//
int  process::CarScarBaiaManualeAGV(short int num_agv, short int num_baia, short int TypPresa, bool *Ok, char *all_mess)
{
  int err=0;
  int carico;
  int chiamata;
  short int dest;
  short int altro_agv, limitrofo, okprog;
  short int tipo_mission;
  int       i;
  bool      OkChiamata;
  struct BAIE bb;

  dest=0;
  *Ok=false;
  //-----------------------------------------
  // Verifico attendibilità della MAC
  //-----------------------------------------
  memset(&bb, 0, sizeof(BAIE));
  if( P.read_baie( num_baia, &bb, all_mess)) return 1;

  //-----------------------------------------
  // Test AGV pronto
  //-----------------------------------------
  if(AGV[num_agv]->load_stato(all_mess)) err = 1;
  okprog =(short int)(AGV[num_agv]->stato.s.bit.okprog);              // AGV pronto aricevere nuovo prg.
  if(!okprog) err = 1;
  if(!AGV[num_agv]->stato.s.bit.funz)  err = 1;                       // Test se AGV in automatico
  if(AGV[num_agv]->test_mission_on())  err = 1;
  if(AGV[num_agv]->mission.Esclusione) err = 1;                       // Test se escluso
  if(AGV[num_agv]->stato.pos == 0)     err = 1;
  if(err) {strcpy(all_mess, MESS[356]); return 1;}
  //-----------------------------------------
  // Recupero se la posizione analizzata del
  // TGV è CARICA o SCARICA
  // 0 = Scarica
  // 1 = ForcaB
  // 2 = ForcaA
  // 3 = ForcaAB
  //-----------------------------------------
  carico=0;
  //if(TestBit((char *) AGV[num_agv]->sbit, 13)) carico=1;  // carico
  if( AGV[num_agv]->sbit->carA                             ) carico=N_FORCA_A;
  if( AGV[num_agv]->sbit->carB                             ) carico=N_FORCA_B;
  if( AGV[num_agv]->sbit->carA && AGV[num_agv]->sbit->carB ) carico=N_FORCA_AB;

  //---------------------------------------------------------
  // Controllo TIPO PRESA
  // coerenza tra tipo di presa (FORCA) e stato di carico
  //---------------------------------------------------------
  if((TypPresa<N_FORCA_B || TypPresa>N_FORCA_AB)||
     (TypPresa==N_FORCA_A  && ( carico==N_FORCA_B || carico==N_FORCA_AB ))|| // TypPresa 1 e prsenza su forca 2
     (TypPresa==N_FORCA_AB && ( carico==N_FORCA_A || carico==N_FORCA_B  ))){ // TypPresa 3 e forche non VUOTE o non PIENE
     sprintf(all_mess, MESS[358] );                // Errore Tipo Presa (Numero Forca) sbagliato
     return 1;
  }

  tipo_mission=MISS_CARICO_DA_BAIA;
  if( carico==N_FORCA_A && TypPresa==N_FORCA_A ) tipo_mission=MISS_SCARICO_SU_BAIA; // Se ho la presenza su forca A e lavoro con forca A--> SCARICO
  if( carico==N_FORCA_B                        ) tipo_mission=MISS_SCARICO_SU_BAIA; // Se ho la presenza su forca B --> SCARICO
  if( carico==N_FORCA_AB                       ) tipo_mission=MISS_SCARICO_SU_BAIA; // Se ho la presenza su forca B --> SCARICO


  //-------------------------------------------
  // Postazione non in chiamata --> skip!!
  //
  // recupero anche il numero della stazione
  // con il tipo di chiamata richiesto
  //-------------------------------------------
  OkChiamata=true;
  if(bb.Num<1             ) OkChiamata=false;
  if(bb.Num>TOTBAIE       ) OkChiamata=false;
  if(bb.Punto<1           ) OkChiamata=false;
  if(bb.Punto>TOTPUNTI    ) OkChiamata=false;
  if(bb.Esclusione==true  ) OkChiamata=false;
  if(tipo_mission==MISS_SCARICO_SU_BAIA){
   //if(bb.Tipo!=BA_DEPOSITO ) OkChiamata=false;
     if(bb.StatoPres!=NN     ) OkChiamata=false;
  }
  if(tipo_mission==MISS_CARICO_DA_BAIA){
   //if(bb.Tipo!=BA_PRELIEVO ) OkChiamata=false;
     if(bb.StatoPres==NN     ) OkChiamata=false;
  }
  //---------------------------------------------------
  // Nessuna chiamata idonea trovata
  //---------------------------------------------------
  if(OkChiamata==false){
     strcpy(all_mess, MESS[357]);
     return 1;
  }
  //------------------------------------------------------
  // Destinazione missione
  //------------------------------------------------------
  dest=bb.Punto;

  //------------------------------------------------------
  // Locazione SBAGLIATA --> skip!!
  // Locazione non presente o NON adatta al tipo missione
  //------------------------------------------------------
  if(dest==0){                                                   // Destinazione NOT OK
     strcpy(all_mess, MESS[375]);
     return 1;
  }

  //-------------------------------------------
  // Test destinazione occupata
  //-------------------------------------------
  if(P.test_dest_riservata(dest, &altro_agv, num_agv)){
     if(num_agv != altro_agv) err = 1;
  }
  if(P.test_dest_occupata(dest, &altro_agv)){
     if(num_agv != altro_agv) err = 1;
  }
  //---------------------------------------
  // Test destinazione limitrofe
  //---------------------------------------
  if(P.test_dest_limitrofe(dest, &altro_agv, &limitrofo, num_agv)){
     if(num_agv != altro_agv) err = 1;
  }
  if(err) {strcpy(all_mess, MESS[365]); return 1;}

  //----------------------------------------------
  // Assegno le chiamate al TGV solo sulle
  // missioni di carico
  //----------------------------------------------
  chiamata=0;
  if(tipo_mission==MISS_CARICO_DA_BAIA) TestNumChiamata(&chiamata, dest, bb.Num);
  //----------------------------------------------
  // Inizializzazione missione
  //----------------------------------------------
  if(new_init_mission(FALSE, CALCOLA_TIPO_PATH, num_agv, chiamata, dest, 0, 0, tipo_mission, TypPresa, Ok, all_mess)) return 1;

  return 0;
}


// -----------------------------
//  CarScarMagManualeAGV()
// -----------------------------
// Lancia una missione di carico/scarico verso macchina  (lo stabilisce
// autonomamente a seconda che l'AGV sia carico o scarico) facendo un test di
// presenza cestone, richiesta cestone da parte della macchina, e destinazione
// occupata
//
int  process::CarScarMagManualeAGV(short int num_agv, short int num_mag, short int lato_mag, short int TypPresa, bool *Ok, char *all_mess)
{
  int i, err=0;
  int carico;
  int chiamata;
  bool lato_ingresso;
  bool lato_uscita;
  int  SovrapCarWash = SOVRAPPOSIZIONE_CARWASH;
  short int altro_agv, limitrofo, okprog;
  short int tipo_mission, dest;
  bool      Basso, PresaSovrap, OkMagazzino;
  short int TipoBox;
  short int Posizione;
  short int Piano;
  short int PosizioneTgv=0;
  struct MAG mg;
  bool Presa[POSINPRESA+1][PIANIINPRESA+1];


  dest=0;
  *Ok=false;
  //-----------------------------------------
  // Verifico attendibilità della MAC
  //-----------------------------------------
  memset(&mg, 0, sizeof(MAG));
  if( M.read_mag( num_mag, &mg, all_mess)) return 1;

  //-----------------------------------------
  // Test AGV pronto
  //-----------------------------------------
  if(AGV[num_agv]->load_stato(all_mess)) err = 1;
  okprog =(short int)(AGV[num_agv]->stato.s.bit.okprog);              // AGV pronto aricevere nuovo prg.
  if(!okprog) err = 1;
  if(!AGV[num_agv]->stato.s.bit.funz)  err = 1;                       // Test se AGV in automatico
  if(AGV[num_agv]->test_mission_on())  err = 1;
  if(AGV[num_agv]->mission.Esclusione) err = 1;                       // Test se escluso
  if(AGV[num_agv]->stato.pos == 0)     err = 1;
  if(err) {strcpy(all_mess, MESS[356]); return 1;}
  //-----------------------------------------
  // Recupero se la posizione analizzata del
  // TGV è CARICA o SCARICA
  // 0 = Scarica
  // 1 = ForcaB
  // 2 = ForcaA
  // 3 = ForcaAB
  //-----------------------------------------
  carico=0;
  //if(TestBit((char *) AGV[num_agv]->sbit, 13)) carico=1;  // carico
  if( AGV[num_agv]->sbit->carA                             ) carico=N_FORCA_A;
  if( AGV[num_agv]->sbit->carB                             ) carico=N_FORCA_B;
  if( AGV[num_agv]->sbit->carA && AGV[num_agv]->sbit->carB ) carico=N_FORCA_AB;

  //---------------------------------------------------------
  // Controllo TIPO PRESA
  // coerenza tra tipo di presa (FORCA) e stato di carico
  //---------------------------------------------------------
  if((TypPresa<N_FORCA_B || TypPresa>N_FORCA_AB)||
     (TypPresa==N_FORCA_A  && ( carico==N_FORCA_B || carico==N_FORCA_AB ))|| // TypPresa 1 e prsenza su forca 2
     (TypPresa==N_FORCA_AB && ( carico==N_FORCA_A || carico==N_FORCA_B  ))){ // TypPresa 3 e forche non VUOTE o non PIENE
     sprintf(all_mess, MESS[358] );                // Errore Tipo Presa (Numero Forca) sbagliato
     return 1;
  }
  tipo_mission=MISS_CARICO_DA_MAGAZZINO;
  if( carico==N_FORCA_A && TypPresa==N_FORCA_A ) tipo_mission=MISS_SCARICO_SU_MAGAZZINO; // Se ho la presenza su forca A e lavoro con forca A--> SCARICO
  if( carico==N_FORCA_B                        ) tipo_mission=MISS_SCARICO_SU_MAGAZZINO; // Se ho la presenza su forca B --> SCARICO
  if( carico==N_FORCA_AB                       ) tipo_mission=MISS_SCARICO_SU_MAGAZZINO; // Se ho la presenza su forca B --> SCARICO

  //---------------------------------------------------
  // Recupero se box sovrapponibile in base al tipo
  // presa
  //
  // AL-20/06/2013 LE PRESE SONO TUTTE SOVRAPPONIBILI
  // tranne quelle con il flag di Car-Wash.
  // Il bit di "sovrapposizione" è importante per il
  // controllo dello stato della presa su cui sovraporre
  // che ovviamente deve essere ON
  //---------------------------------------------------
  PresaSovrap  = true;
  memset(&Presa[0][0], false, sizeof(Presa));
  if(TypPresa==N_FORCA_A ){
     Basso       = AGV[num_agv]->mission.BoxA.Basso;
     TipoBox     = AGV[num_agv]->mission.BoxA.TipoBox;
     if(SovrapCarWash==0 && AGV[num_agv]->mission.BoxA.CarWash!=0) PresaSovrap=false;
     memcpy(&Presa[0][0], &AGV[num_agv]->mission.BoxA.Presa[0][0], sizeof(Presa));
  }
  if(TypPresa==N_FORCA_B ){
     Basso       = AGV[num_agv]->mission.BoxB.Basso;
     TipoBox     = AGV[num_agv]->mission.BoxB.TipoBox;
     if(SovrapCarWash==0 && AGV[num_agv]->mission.BoxB.CarWash!=0) PresaSovrap=false;
     memcpy(&Presa[0][0], &AGV[num_agv]->mission.BoxB.Presa[0][0], sizeof(Presa));
  }
  if(TypPresa==N_FORCA_AB){
     Basso       = AGV[num_agv]->mission.BoxB.Basso;
     TipoBox     = AGV[num_agv]->mission.BoxB.TipoBox;
     if(SovrapCarWash==0 && AGV[num_agv]->mission.BoxB.CarWash!=0) PresaSovrap=false;
     memcpy(&Presa[0][0], &AGV[num_agv]->mission.BoxB.Presa[0][0], sizeof(Presa));
  }
  //---------------------------------------------------
  // Gestione lato prelievo/deposito in magazzino
  //---------------------------------------------------
  switch(lato_mag){
     case  0: lato_ingresso = false;   // STANDARD
              lato_uscita   = false;
              break;
     case  1: lato_ingresso = true;    // INGRESSO
              lato_uscita   = false;
              break;
     case  2: lato_ingresso = false;   // USCITA
              lato_uscita   = true;
              break;
     default: lato_ingresso = false;   // STANDARD
              lato_uscita   = false;
              break;
  }

  //------------------------------------------------------
  // Destinazione missione
  // PuntoIng = Deposito
  // PuntoOut = prelievo
  //------------------------------------------------------
  dest=0;
  if( tipo_mission==MISS_SCARICO_SU_MAGAZZINO ){
     if(lato_uscita==false   ) dest = mg.PuntoIng;
     if(lato_uscita==true    ) dest = mg.PuntoOut;
  }
  if( tipo_mission==MISS_CARICO_DA_MAGAZZINO  ){
     if(lato_ingresso==false ) dest = mg.PuntoOut;
     if(lato_ingresso==true  ) dest = mg.PuntoIng;
  }

  //------------------------------------------------------
  // Locazione SBAGLIATA --> skip!!
  // Locazione non presente o NON adatta al tipo missione
  //------------------------------------------------------
  if(dest==0){                                                   // Destinazione NOT OK
     strcpy(all_mess, MESS[375]); return 1;
  }

  //---------------------------------------------------
  // Postazione non in disponibile --> skip!!
  // N.B. Le forche B possono prel/dep solo in basso
  //---------------------------------------------------
  OkMagazzino=false;
  if( tipo_mission==MISS_CARICO_DA_MAGAZZINO ){
     OkMagazzino=!M.VerificaTipoPrelievo(lato_ingresso, num_mag, TypPresa, &Posizione, &Piano, &PosizioneTgv, all_mess);
  }
  if( tipo_mission==MISS_SCARICO_SU_MAGAZZINO ){
     OkMagazzino=!M.VerificaTipoDeposito(lato_uscita, num_mag, 0, TypPresa, TipoBox, Presa, PresaSovrap, Basso, FALSE, &Posizione, &Piano, &PosizioneTgv, all_mess);
  }

  if( OkMagazzino==0){
     //strcpy(all_mess, MESS[357]);
     sprintf(all_mess, "Store nr.%d, not ok!", num_mag);
     return 1;
  }

  //-------------------------------------------
  // Test destinazione occupata
  //-------------------------------------------
  if(P.test_dest_riservata(dest, &altro_agv, num_agv)){
     if(num_agv != altro_agv) err = 1;
  }
  if(P.test_dest_occupata(dest, &altro_agv)){
     if(num_agv != altro_agv) err = 1;
  }

  //---------------------------------------
  // Test destinazione limitrofe
  //---------------------------------------
  if(P.test_dest_limitrofe(dest, &altro_agv, &limitrofo, num_agv)){
     if(num_agv != altro_agv) err = 1;
  }
  if(err) {strcpy(all_mess, MESS[365]); return 1;}

  //----------------------------------------------
  // Assegno il piano di prelievo e forzo la
  // chiamata a "0".
  //----------------------------------------------
  chiamata=0;
  //----------------------------------------------
  // Inizializzo la missione
  //----------------------------------------------
  if(new_init_mission(FALSE, CALCOLA_TIPO_PATH, num_agv, chiamata, dest, Posizione, 0, tipo_mission, TypPresa, Ok, all_mess)) return 1;

  return 0;
}


// ---------------------------
//    maintenance_mission()
// ---------------------------
//  Manutenzione missione
//  Viene eseguita per tutti gli AGV
//
int process::maintenance_mission(short int num_agv, char *all_mess)
/************************************************************/
{
  short int num_mac;
  short int num_stz;
  short int num_mag;
  short int num_baia;
  short int agv_bloccante;
  int pos, inizio, end, intend , gia_target =0, end_mission =0, aut, okprog, NumNodo;
  int reset_giafatto=0, into_path, err=0;
  bool TgvCarA, TgvCarB;

  agv_bloccante=0;
  //-----------------------------------------------------------------------------
  // stato attuale AGV
  //-----------------------------------------------------------------------------
  pos       = AGV[num_agv]->stato.pos;
  inizio    = AGV[num_agv]->mission.pstart;
  end       = AGV[num_agv]->mission.pend;
  intend    = AGV[num_agv]->mission.pintend;
  aut       = AGV[num_agv]->stato.s.bit.funz;     // AGV in marcia + automatico
  okprog    = AGV[num_agv]->stato.s.bit.okprog;   // AGV pronto aricevere nuovo prg.
  TgvCarA   = AGV[num_agv]->stato.s.bit.carA;     // AGV carico
  TgvCarB   = AGV[num_agv]->stato.s.bit.carB;     // AGV carico
  //----------------------------------------------------
  // (2) GESTIONE FORCA SINGOLA CON CARICO DOPPIO
  //----------------------------------------------------
  if(GestioneDoppiaForcaTGV==2 && TgvCarB==true){
     if(AGV[num_agv]->mission.TypPresa==N_FORCA_A ){TgvCarB=false; TgvCarA=true;}
     if(AGV[num_agv]->mission.TypPresa==N_FORCA_AB){               TgvCarA=true;}
  }
  //-----------------------------------------------------------------------------
  // AGV su obbiettivo finale ?
  //-----------------------------------------------------------------------------
  if(inizio == end) gia_target  =1;
  //-----------------------------------------------------------------------------
  // fine missione da AGV ? ( DM91=2 )
  //-----------------------------------------------------------------------------
  if(AGV[num_agv]->stato.start == 2)  end_mission =1;
  //-----------------------------------------------------------------------------
  // Testa se la posizione attuale dell' AGV Š compresa nel percorso
  //-----------------------------------------------------------------------------
  into_path = !AGV[num_agv]->test_position_into_path(all_mess);
  //-----------------------------------------------------------------------------
  // se AGV missione ON, non escluso e in marcia automatica e nel percorso....
  //-----------------------------------------------------------------------------
  if(AGV[num_agv]->test_mission_on() && into_path){
     //---------------------------------------------------------------
     // Testa se la posizione attuale dell' AGV Š compresa nel percorso
     //---------------------------------------------------------------
     if(AGV[num_agv]->test_position_into_path(all_mess)) return 1;
     //--------------------------------------------------------
     // Verifica che non vi siano dei nodi solo prenotati
     // tra la posizione attuale del TGV e i sucessivi
     // nodi occupati.
     //--------------------------------------------------------
     if(AGV[num_agv]->rioccupa_nodi(all_mess)) return 1;
     //--------------------------------------------------------
     // libera nodi già passati e occupa i successivi
     //--------------------------------------------------------
     if(AGV[num_agv]->free_nodi(all_mess)) return 1;
     agv_bloccante=0;
     if(AGV[num_agv]->occupa_nodi(all_mess, FALSE, &agv_bloccante)){
        AgvBloccati[num_agv]=agv_bloccante;
        return 1;
     }
     AgvBloccati[num_agv]=agv_bloccante;
     //--------------------------------------------------------
     // Il programma sul TGV viene compilato solo se la navetta
     // è in automatico
     //--------------------------------------------------------
     if(aut){
        //--------------------------------------------------------
        // Trasmetti parte o totale missione
        // Se AGV non Š su pos.finale e non ha attiva la missione
        // e ha le codizioni per ricevere nuovo programma
        //--------------------------------------------------------
        if(!AGV[num_agv]->mission.NoMissioniAlVolo){
           if(((pos != end) || gia_target) && (!AGV[num_agv]->stato.start) && okprog){
              NumNodo=0;
              if(AGV[num_agv]->compila_prg(all_mess, &NumNodo)) return 1;
           }
           if(((pos != end) || gia_target) && (AGV[num_agv]->mission.pintendtrasmesso!=intend)){
              if(AGV[num_agv]->trs_mission(all_mess)) {error_trasmit(num_agv, all_mess); return 0;}
           };
        }
        else{
           if(((pos != intend) && (pos != end) || gia_target) && (!AGV[num_agv]->stato.start) && okprog){
              NumNodo=0;
              if(AGV[num_agv]->compila_prg(all_mess, &NumNodo)) return 1;
              if(AGV[num_agv]->trs_mission(all_mess)) {error_trasmit(num_agv, all_mess); return 0;}
           };
        }
        //--------------------------------------------------------
        // Verifico se la posizione finale della missione
        // appartienere a una macchina:
        // - AUTOCLAVE
        // - DECESTONATORE
        // - RULLIERA
        //--------------------------------------------------------
        num_mag=0; num_mac=0; num_stz=0; num_baia=0;
        P.test_punto_presente_mac(all_mess, AGV[num_agv]->mission.pend, &num_mac, &num_stz);
        P.test_punto_presente_baia(all_mess, AGV[num_agv]->mission.pend, &num_baia);
        M.test_punto_presente_mag(all_mess, AGV[num_agv]->mission.pend, &num_mag, &num_stz);
        //--------------------------------------------------------
        // Conclusione Missione OK : aggiorna dati
        // In base al tipo di missione che è stato richiesto, il
        // PC setta il carico-scarico da forno-scelta, aggiorna
        // i magazzini ( praticamente si costruisce in RAM una
        // simulazione di quanto stà avvenendo sul campo )
        //--------------------------------------------------------
        if((pos==end) && end_mission){         // st 2
           //----------------------------------------------------------
           // Se ho un editor attivo allora evito la conclusione
           // della missione
           //----------------------------------------------------------
           if( FormTgv->EditorAgvAttivo ){
              //-------------------------------------------------
              // 18/11/2002
              // Elabora comunque se è attiva la pagina laser
              // e in corso un posizionamento semplice
              //-------------------------------------------------
              if(!FormMappa->Active            ) return 0;
              if(!AGV[num_agv]->mission.Typ==0 ) return 0;
           }
           //---------------------------------------------------
           // File di traccia
           //---------------------------------------------------
           // GB-06/02 Conteggio cestoni su macchina
           //history(num_agv, "END");           // Traccia operazioni TGV
           //---------------------------------------------------
           // Carico/Scarico dati (conclusione missione
           //---------------------------------------------------
           switch(AGV[num_agv]->mission.Typ){
                 //-----------------------------------------------------
                 // Posizionamento Semplice
                 //------------------------------------------------------
                 case MISS_SOLO_POSIZIONAMENTO:
                    break;
                 //-----------------------------------------------------
                 // (3-5) Carico da MACCHINA
                 //       Cestone Prodotto o Falda
                 //------------------------------------------------------
                 case MISS_CARICO_DA_MACCHINA:
                    if(EditorMacchineAttivo==num_mac                                                                                 ) return 0;
                    if(AbilitaSimulazione==false &&  TgvCarA==false                    && AGV[num_agv]->mission.TypPresa==N_FORCA_A  ) return 0;
                    if(AbilitaSimulazione==false &&  TgvCarB==false                    && AGV[num_agv]->mission.TypPresa==N_FORCA_B  ) return 0;
                    if(AbilitaSimulazione==false && (TgvCarA==false || TgvCarB==false) && AGV[num_agv]->mission.TypPresa==N_FORCA_AB ) return 0;
                    if(CaricoDaMacchina(num_agv, all_mess)) return 1;
                    break;
                 //-----------------------------------------------------
                 // (3-5) Carico da BAIA
                 //       Cestone Prodotto o Falda
                 //------------------------------------------------------
                 case MISS_CARICO_DA_BAIA:
                    if(EditorBaieAttivo==num_baia                                                                                    ) return 0;
                    if(AbilitaSimulazione==false &&  TgvCarA==false                    && AGV[num_agv]->mission.TypPresa==N_FORCA_A  ) return 0;
                    if(AbilitaSimulazione==false &&  TgvCarB==false                    && AGV[num_agv]->mission.TypPresa==N_FORCA_B  ) return 0;
                    if(AbilitaSimulazione==false && (TgvCarA==false || TgvCarB==false) && AGV[num_agv]->mission.TypPresa==N_FORCA_AB ) return 0;
                    if(CaricoDaBaia(num_agv, all_mess)) return 1;
                    break;
                 //-----------------------------------------------------
                 // (9) Carico da MAGAZZINO
                 //------------------------------------------------------
                 case MISS_CARICO_DA_MAGAZZINO:
                    if(EditorMagazzinoAttivo==num_mag                                                                                ) return 0;
                    if(AbilitaSimulazione==false &&  TgvCarA==false                    && AGV[num_agv]->mission.TypPresa==N_FORCA_A  ) return 0;
                    if(AbilitaSimulazione==false &&  TgvCarB==false                    && AGV[num_agv]->mission.TypPresa==N_FORCA_B  ) return 0;
                    if(AbilitaSimulazione==false && (TgvCarA==false || TgvCarB==false) && AGV[num_agv]->mission.TypPresa==N_FORCA_AB ) return 0;
                    if(CaricoDaMagazzino(num_agv, all_mess)) return 1;
                    break;
                 //-----------------------------------------------------
                 // (4-6) Scarico su CESTONATORE/DECESTONATORE
                 //       Cestone Prodotto o Falda
                 //------------------------------------------------------
                 case MISS_SCARICO_SU_MACCHINA:
                    if(EditorMacchineAttivo==num_mac                                                                                ) return 0;
                    if(AbilitaSimulazione==false && TgvCarA==true                     && AGV[num_agv]->mission.TypPresa==N_FORCA_A  ) return 0;
                    if(AbilitaSimulazione==false && TgvCarB==true                     && AGV[num_agv]->mission.TypPresa==N_FORCA_B  ) return 0;
                    if(AbilitaSimulazione==false && (TgvCarA==true || TgvCarB==true)  && AGV[num_agv]->mission.TypPresa==N_FORCA_AB ) return 0;
                    if(ScaricoSuMacchina(num_agv, all_mess)) return 1;
                    break;
                 //-----------------------------------------------------
                 // (4-6) Scarico su BAIA
                 //       Cestone Prodotto o Falda
                 //------------------------------------------------------
                 case MISS_SCARICO_SU_BAIA:
                    if(EditorBaieAttivo==num_baia                                                                                   ) return 0;
                    if(AbilitaSimulazione==false && TgvCarA==true                     && AGV[num_agv]->mission.TypPresa==N_FORCA_A  ) return 0;
                    if(AbilitaSimulazione==false && TgvCarB==true                     && AGV[num_agv]->mission.TypPresa==N_FORCA_B  ) return 0;
                    if(AbilitaSimulazione==false && (TgvCarA==true || TgvCarB==true)  && AGV[num_agv]->mission.TypPresa==N_FORCA_AB ) return 0;
                    if(ScaricoSuBaia(num_agv, all_mess)) return 1;
                    break;
                 //-----------------------------------------------------
                 // (9) Carico da MAGAZZINO
                 //------------------------------------------------------
                 case MISS_SCARICO_SU_MAGAZZINO:
                    if(EditorMagazzinoAttivo==num_mag                                                                               ) return 0;
                    if(AbilitaSimulazione==false && TgvCarA==true                     && AGV[num_agv]->mission.TypPresa==N_FORCA_A  ) return 0;
                    if(AbilitaSimulazione==false && TgvCarB==true                     && AGV[num_agv]->mission.TypPresa==N_FORCA_B  ) return 0;
                    if(AbilitaSimulazione==false && (TgvCarA==true || TgvCarB==true)  && AGV[num_agv]->mission.TypPresa==N_FORCA_AB ) return 0;
                    if(ScaricoSuMagazzino(num_agv, all_mess)) return 1;
                    break;
                 default:
                    break;
           };
           //---------------------------------------------------
           // File di traccia
           //---------------------------------------------------
           // GB-06/02 Conteggio cestoni su macchina
           history(num_agv, "END");           // Traccia operazioni TGV
           //--------------------------------------------------------
           // Azzeramento dei dati della missioni e trasmissione del
           // reset al PLC.
           //--------------------------------------------------------
           if(AGV[num_agv]->reset_mission(all_mess)) return 1;
           AGV[num_agv]->trasmetti_reset_AGV( all_mess );
           //---------------------------------------------------
           // AL-02/12/2019 Memorizzo data ora di ritorno a
           // riposo per gestione restart da ricarica dei TGV
           // fermi a più tempo
           //---------------------------------------------------
           if(AGV[num_agv]->stato.pos<51) AGV[num_agv]->mission.TimeInitAttesa=time(NULL);
           // --- reset missione già avvenuto ---
           reset_giafatto=1;
        };
        //--------------------------------------------------------
        // Se AGV arrivato al punto intermedio_end, allora esce e
        // mantiene attiva la missione in RAM , pur resettando il flag
        // di start dell'AGV. ( 2 -> 0 )
        //--------------------------------------------------------
        if((pos==intend) && (AGV[num_agv]->stato.start==2) && !reset_giafatto ){
           AGV[num_agv]->trasmetti_reset_AGV( all_mess );
           reset_giafatto=1;
           return 0;
        }
     }
  }
  //--------------------------------------------------------
  // 21/03/96 Conclusione Missione Anomala :
  //          START==2 senza missione in corso
  //          --> Trasmetti RESET AGV DM98=1
  //--------------------------------------------------------
  if( aut && (AGV[num_agv]->stato.start==2) && !reset_giafatto ){
     sprintf(all_mess, MESS[640], pos );
     error_trasmit(num_agv, all_mess);
     //------------------------------------------------------------
     // Eseguo la trasmissione del Reset della missione solo se in
     // seguito alla rilettura dello stato del TGV trovo ancora il
     // bit di START a "2".
     //------------------------------------------------------------
     err = AGV[num_agv]->load_stato(all_mess);
     if(!err && AGV[num_agv]->stato.start==2) AGV[num_agv]->trasmetti_reset_AGV(all_mess);
  }
  //------------------------------------------------------------
  // Conclusione Missione Anomala : azzera missione
  // - AGV non su posizione di end o intend e senza start e con
  //   una missione attiva.
  //   (si pu• verificare quando l'AGV viene messo in manuale)
  //------------------------------------------------------------
  if(((pos != intend) || (pos == end)) && (!AGV[num_agv]->stato.start) && AGV[num_agv]->test_mission_on() && okprog && aut){
     sprintf(all_mess, MESS[352], pos, AGV[num_agv]->stato.start, okprog);
     error_trasmit(num_agv, all_mess);
     //------------------------------------------------------------
     // Eseguo il Reset della missione in RAM solo se in seguito a
     // una rilettura dello stato TGV trovo ancora il bit di START
     // a "0".
     //------------------------------------------------------------
     err = AGV[num_agv]->load_stato(all_mess);
     if(!err && !AGV[num_agv]->stato.start){AGV[num_agv]->reset_mission(all_mess);}
  }
  //--------------------------------------------------------
  // Conclusione Missione Anomala : azzera missione
  // - AGV in MANUALE
  //--------------------------------------------------------
  if(!aut && AGV[num_agv]->test_mission_on() && okprog){
     sprintf(all_mess, MESS[353], pos );//"Fine misi¢n anormal POS=%d, AGV en MANUAL"
     error_trasmit(num_agv, all_mess);
  }
  return 0;
}/*** maintenance_mission() ***/


// -------------------------------
//    error_trasmit()
// -------------------------------
// Questa funzione lascia un messaggio di anomalia in TRSERR.DAT in caso
// di errore trasmissione con AGV
//
int process::error_trasmit(int num_agv, char *messaggio)
/******************************************************/
{
  char filename[81]="";
  char buffer[140]="";
  char NomeFile[81]="LogTrsErr.dat";
  short int punto=0;

  strcpy( filename, "TRSERR.DAT" );
  //-------------------------------------------------------
  // Gestione errore di sistema legato al TGV
  //-------------------------------------------------------
  if(num_agv>0 && num_agv<=MAXAGV){
     //------------------------------------
     // Recupero della posizione del TGV
     //------------------------------------
     punto = AGV[num_agv]->stato.pos;
     //------------------------------------
     // Gestione errore di comunicazione
     //------------------------------------
     if(errore_comunicazione[COM1][num_agv]==1 && AGV[num_agv]->mission.ErrCom==true) return 0;
  }
  //-------------------------------------------------------
  // Composizione messaggio
  //-------------------------------------------------------
  sprintf(buffer, "  %03d   %03d   %s", num_agv, punto, messaggio);
  file_cronologico_generico(filename, buffer, 90000L );
  sprintf(buffer, ";%03d;%03d;%s", num_agv, punto, messaggio);
  file_cronologico_generico(NomeFile, buffer, 90000L );

  return 0;
}

// -------------------------------
//    FileTraccia()
// -------------------------------
// Questa funzione lascia un messaggio in TRACCIA.DAT in caso
// di visualizzazione dati.
//
void FileTraccia(char *messaggio)
/******************************************************/
{
  char filename[81]="TRACCIA.DAT";
  file_cronologico_generico( filename, messaggio, 50000L );
  return;
}

// -------------------------------
//    FileLogOperatore()
// -------------------------------
// Questa funzione lascia un messaggio in LOGOP.DAT di tutte
// le operazioni eseguite da operatore.
//
void FileLogOperatore(char *messaggio)
/******************************************************/
{
  char buffer[201]="";
  char all_mess[101]="";
  char filename[81]="LOGOP.DAT";

  //-------------------------------------------------------
  // Recupero se il PC è Client o Server
  //-------------------------------------------------------
  strcpy(all_mess, "PC01");
  if(IdPC>1) sprintf(all_mess, "PC%02d", IdPC);
  //-------------------------------------------------------
  // Viene conservato in LOG di 50000 byte prevedendo:
  //  - 50 caratteri di media per ogni riga salvata.
  //  - 30 righe di media salvate in un giorno.
  // che coincide circa un mese di salvataggi.
  //-------------------------------------------------------
  sprintf(buffer, "%s %02d   %s", all_mess, NrOperatore, messaggio);
  //-------------------------------------------------------
  // CLIENT/SERVER
  // Se il PC è un Client il messaggio viene comunque
  // registrato sul Server.
  //-------------------------------------------------------
  if(IdPC>1) RemCreaFileComando("LOGOP", (BYTE *) buffer, strlen(buffer)+1, all_mess);
  else file_cronologico_generico(filename, buffer, 50000L);
  return;
}

// -------------------------------------
//     CicloSimulaAGV()
//  Routine di simulazione AGV
//
// -------------------------------------
// Funzione per la simulazione delle missioni
//
int process::CicloSimulazioneAGV()
/***********************/
{
  short int i, num_agv;
  short int rot, pos, pos1;
  int punto=0;
  int passo_prg;
  int cambio_rot=0;
  bool converti_grd;
  bool grd0, grd90, grd180, grd270;
  bool ind, movX, movY, movA, movI;
  double grd_ly, grd_ls;


  if( break_Com[COM1]==0 ){
     AbilitaSimulazione=false;
     ShowMessage("Error comunication is ON with TGVs!");
     return 0;
  }
  //----------------------------------------------
  // Sincronizzazione con RunAutomatico
  //----------------------------------------------
  if( RunAutomaticoEseguito==false ) return 0;
  RunAutomaticoEseguito=false;
  //----------------------------------
  // Simula tutti gli AGV
  //----------------------------------
  for( num_agv=1; num_agv<=MAXAGV; num_agv++){
     //-------------------------------------------
     // Con TGV escluso non faccio girare il
     // TGV in simulazione
     //-------------------------------------------
     if(AGV[num_agv]->mission.Esclusione==true) continue;
     //-------------------------------------------
     // Setta lo stato AGV
     //-------------------------------------------
     AGV[ num_agv ]->stato.s.bit.sunod = 1;   //Setta posizione su nodo
     AGV[ num_agv ]->asrv_stato.sbit1.su_nod = true;
     pos  = AGV[ num_agv ]->stato.pos;
     pos1 = AGV[ num_agv ]->stato.pos;        //Setta anche pos1 per evitare che rimanga sporco con la posiz. di un altro TGV
     ind  = AGV[ num_agv ]->stato.s.bit.ind;
     //-------------------------------------------
     // Verifico se c'è almeno un contenitore
     // a bordo del TGV
     //
     // GESTIONE FORCA SINGOLA CARICO DOPPIO
     //-------------------------------------------
     AGV[ num_agv ]->stato.s.bit.carA=0;
     AGV[ num_agv ]->stato.s.bit.carB=0;
     AGV[ num_agv ]->asrv_stato.sbit1.carico=false;
     if(GestioneDoppiaForcaTGV!=2){
        if(AGV[num_agv]->mission.PresCEST_A!=LIBERA) AGV[ num_agv ]->stato.s.bit.carA=1;
        if(AGV[num_agv]->mission.PresCEST_B!=LIBERA){AGV[ num_agv ]->stato.s.bit.carB=1; AGV[ num_agv ]->asrv_stato.sbit1.carico=true;}
     }
     else{
        if(AGV[num_agv]->mission.PresCEST_A!=LIBERA) AGV[ num_agv ]->stato.s.bit.carB=1;
        if(AGV[num_agv]->mission.PresCEST_B!=LIBERA){AGV[ num_agv ]->stato.s.bit.carB=1; AGV[ num_agv ]->asrv_stato.sbit1.carico=true;}
     }
     cambio_rot=0;
     //-------------------------------------------
     // Se fine missione forza START=0
     //-------------------------------------------
     if( AGV[ num_agv ]->stato.start==2 && !AGV[ num_agv ]->stato.s.bit.allarme && !AGV[ num_agv ]->allarme_interno){
        AGV[ num_agv ]->stato.start=0;
        AGV[ num_agv ]->asrv_stato.start=0;
        AGV[ num_agv ]->stato.s.bit.okprog=true;
        AGV[ num_agv ]->asrv_stato.sbit1.ok_prog=true;
     }
     if( AGV[ num_agv ]->test_mission_on()==0 ) continue;
     //----------------------------------
     // Missione ATTIVA
     //----------------------------------
     if( AGV[ num_agv ]->stato.start==1 ){
        AGV[ num_agv ]->stato.s.bit.okprog=false;
        //---------------------------------
        // GESTIONE_ASRV
        //---------------------------------
        AGV[ num_agv ]->asrv_stato.start=1;
        AGV[ num_agv ]->asrv_stato.sbit1.ok_prog=false;
        //---------------------------------
        // Estrai nodo sucessivo
        //---------------------------------
        for( i=0; i<MAXPERCORSI; i++){
           if( pos==AGV[ num_agv ]->mission.punto[ i ] ){
               pos1=AGV[ num_agv ]->mission.punto[ i+1 ];
               if(pos1!=0){
                  ind  = AGV[ num_agv ]->mission.dati_perc[i].dir;
                  rot  = AGV[ num_agv ]->rot_standard(i);
               }
               else{
                  ind  = AGV[ num_agv ]->stato.s.bit.ind;
                  if(AGV[ num_agv ]->stato.s.bit.grd0==true  ) rot=0;
                  if(AGV[ num_agv ]->stato.s.bit.grd90==true ) rot=1;
                  if(AGV[ num_agv ]->stato.s.bit.grd180==true) rot=2;
                  if(AGV[ num_agv ]->stato.s.bit.grd270==true) rot=3;
               }
               //----------------------------------------------
               // GESTIONE_ASRV
               //----------------------------------------------
               movX = false;
               movY = false;
               movA = false;
               movI = false;
               if(rot==0 && ind==false){movY=true; movA=true;}
               if(rot==2 && ind==true ){movY=true; movA=true;}
               if(rot==0 && ind==true ){movY=true; movI=true;}
               if(rot==2 && ind==false){movY=true; movI=true;}
               if(rot==1 && ind==false){movX=true; movI=true;}
               if(rot==3 && ind==true ){movX=true; movI=true;}
               if(rot==1 && ind==true ){movX=true; movA=true;}
               if(rot==3 && ind==false){movX=true; movA=true;}
               break;
           }
        }
        //-------------------------------
        //  Estrai rotazione
        //-------------------------------
        for( passo_prg=0; passo_prg<=MAXSTEP; passo_prg++ ){
           AGV[ num_agv ]->cbit = (struct comandi *) &AGV[ num_agv ]->mission.step[passo_prg];
           //----------------------------------------
           //  Cerca posizione nel programma
           //----------------------------------------
           if(!AGV[ num_agv ]->cbit->az &&
              !AGV[ num_agv ]->cbit->ral &&
              !AGV[ num_agv ]->cbit->end &&
              !AGV[ num_agv ]->cbit->avvic ){
              punto= AGV[ num_agv ]->mission.step[ passo_prg ];
              //-------------------------------
              // Aggiorna alcuni bit stato
              //-------------------------------
              if( punto==pos ){
                 //---------------------------------------------
                 // Direzione AGV
                 //---------------------------------------------
                 AGV[ num_agv ]->sbit->ind=ind;
                 AGV[ num_agv ]->sbit->av=!ind;
                 //---------------------------------------------
                 // Direzione ASRV
                 //---------------------------------------------
                 AGV[ num_agv ]->asrv_stato.sbit1.mov_X=movX;
                 AGV[ num_agv ]->asrv_stato.sbit1.mov_Y=movY;
                 AGV[ num_agv ]->asrv_stato.sbit1.mov_av=movA;
                 AGV[ num_agv ]->asrv_stato.sbit1.mov_ind=movI;
              }
           }
           grd0=(bool)(AGV[num_agv]->cbit->grd0);
           grd90=(bool)(AGV[num_agv]->cbit->grd90);
           grd180=(bool)(AGV[num_agv]->cbit->grd180);
           grd270=(bool)(AGV[num_agv]->cbit->grd270);
           //-----------------------------------
           // Estrai Rotazione
           //-----------------------------------
           if((AGV[ num_agv ]->cbit->az && !AGV[ num_agv ]->cbit->ral) && (grd0 || grd90 || grd180 || grd270)){
              cambio_rot = 0;
              //---------------------------------------------
              // Assegnazione gradi sul nodo
              //---------------------------------------------
              if( punto==pos ){
                 //---------------------------------------------
                 // GESTIONE_ASRV (le navette non ruotano)
                 //---------------------------------------------
                 #ifdef GESTIONE_ASRV
                    AGV[ num_agv ]->sbit->grd0   = 1;
                    AGV[ num_agv ]->sbit->grd90  = 0;
                    AGV[ num_agv ]->sbit->grd180 = 0;
                    AGV[ num_agv ]->sbit->grd270 = 0;
                    if(AGV[ num_agv ]->mission.GestioneLaser==false) AGV[ num_agv ]->stato.rot_calamite = 0;
                    else                                             AGV[ num_agv ]->stato.rot_laser    = 0;
                 #else
                 //---------------------------------------------
                 // GESTIONE_TGV
                 //---------------------------------------------
                    if( AGV[ num_agv ]->sbit->grd0!=(unsigned)(grd0) ){
                       cambio_rot=1;
                       AGV[ num_agv ]->sbit->grd0   = grd0;
                       AGV[ num_agv ]->sbit->grd90  = 0;
                       AGV[ num_agv ]->sbit->grd180 = 0;
                       AGV[ num_agv ]->sbit->grd270 = 0;
                       if(AGV[ num_agv ]->mission.GestioneLaser==false) AGV[ num_agv ]->stato.rot_calamite = 0;
                       else                                             AGV[ num_agv ]->stato.rot_laser    = 0;
                    }
                    if( AGV[ num_agv ]->sbit->grd90!=(unsigned)(grd90) ){
                       cambio_rot=1;
                       AGV[ num_agv ]->sbit->grd0   = 0;
                       AGV[ num_agv ]->sbit->grd90  = grd90;
                       AGV[ num_agv ]->sbit->grd180 = 0;
                       AGV[ num_agv ]->sbit->grd270 = 0;
                       if(AGV[ num_agv ]->mission.GestioneLaser==false) AGV[ num_agv ]->stato.rot_calamite = 90;
                       else                                             AGV[ num_agv ]->stato.rot_laser    = 90;
                    }
                    if( AGV[ num_agv ]->sbit->grd180!=(unsigned)(grd180) ){
                       cambio_rot=1;
                       AGV[ num_agv ]->sbit->grd0   = 0;
                       AGV[ num_agv ]->sbit->grd90  = 0;
                       AGV[ num_agv ]->sbit->grd180 = grd180;
                       AGV[ num_agv ]->sbit->grd270 = 0;
                       if(AGV[ num_agv ]->mission.GestioneLaser==false) AGV[ num_agv ]->stato.rot_calamite = 180;
                       else                                             AGV[ num_agv ]->stato.rot_laser    = 180;
                    }
                    if( AGV[ num_agv ]->sbit->grd270!=(unsigned)(grd270) ){
                       cambio_rot=1;
                       AGV[ num_agv ]->sbit->grd0   = 0;
                       AGV[ num_agv ]->sbit->grd90  = 0;
                       AGV[ num_agv ]->sbit->grd180 = 0;
                       AGV[ num_agv ]->sbit->grd270 = grd270;
                       if(AGV[ num_agv ]->mission.GestioneLaser==false) AGV[ num_agv ]->stato.rot_calamite = 270;
                       else                                             AGV[ num_agv ]->stato.rot_laser    = 270;
                    }
                 #endif;
              }
              if( punto==pos1 ) break;
           }
           //-----------------------------------------------------
           // Posiziona su percorso sucessivo se non è cambio ROT
           //-----------------------------------------------------
           if( cambio_rot ){cambio_rot=0; continue;}
        }
        if( AGV[ num_agv ]->mission.pintstart==0 ) continue;
        if(( pos1>0 ) && ( pos!= AGV[ num_agv ]->mission.pintend )){
           AGV[ num_agv ]->stato.pos = pos1;
           //----------------------------------------------------------
           // Recupero zona su cui si trova il TGV
           //----------------------------------------------------------
           if(pos1>0 && pos1<=MAXPUNTI){
              AGV[ num_agv ]->mission.Zona = N.n[pos1]->zp;
           }
        }
     }

     //----------------------------------
     // Missione ATTIVA
     //----------------------------------
     if( AGV[ num_agv ]->stato.start==1 ){
        //-----------------------------------------------------
        // Se raggiunto fine missione intermedia setta START=2
        //-----------------------------------------------------
        if( pos==AGV[ num_agv ]->mission.pend ){
           AGV[ num_agv ]->stato.start=2;
           AGV[ num_agv ]->asrv_stato.start=2;
        }
     }
  }
  return 0;
}

// -----------------------------
//    history()
// -----------------------------
// Lascia una traccia di tutte le missioni scatenate
//
int process::history(int num_agv, char *tipo)
/*******************************/
{
  char corpo[301]="";
  char all_mess[101]="";
  char filename[100]="HISTORY.DAT";
  char filename1[100]="";
  char forca[11]="";
  char start[11]="";
  char str_chiamata[31]="";
  char stringa[501]="";
  short int num_baia1, num_mac1, num_mag1, num_stz1;
  short int num_baia, num_mac, num_mag, num_stz;

  memset(&stringa[0], NULL, sizeof(stringa));
  sprintf(filename1, "HIST%03d.DAT", num_agv);
  //-----------------------------------------------------------------------
  // Nodo sconosciuto --> skip!!!
  //-----------------------------------------------------------------------
  if(AGV[num_agv]->mission.pend<0 || AGV[num_agv]->mission.pend>TOTPUNTI+1) return 1;
  //-------------------------------------------------------------------
  // Inserimento intestazione
  //-------------------------------------------------------------------
  LJ(tipo, start, 5);
  //-------------------------------------------------------------------
  // LOG MISSTGV (per avere il tempo trascorso tra lo start a vuoto
  // e la conclusione della missione con il deposito in locazione)
  // NOTA: non registro le missione di prelievo paletta da stoccaggo
  //       perchè la missione ha inizio con il prelievo del vuoto
  //       dalla posizione chiamante
  //-------------------------------------------------------------------
  strcpy(forca, "N" );
  if(AGV[num_agv]->mission.TypPresa==N_FORCA_A ) strcpy( forca, "A" );
  if(AGV[num_agv]->mission.TypPresa==N_FORCA_B ) strcpy( forca, "B" );
  if(AGV[num_agv]->mission.TypPresa==N_FORCA_AB) strcpy( forca, "A+B" );

  num_mac=0; num_stz=0; num_mag=0; num_baia=0;
  P.test_punto_presente_mac(all_mess, AGV[num_agv]->mission.pend, &num_mac, &num_stz);
  M.test_punto_presente_mag(all_mess, AGV[num_agv]->mission.pend, &num_mag, &num_stz);
  P.test_punto_presente_baia(all_mess, AGV[num_agv]->mission.pend, &num_baia);
  //------------------------------------------------------
  // Scompongo la chiamata per sapere la macchina
  // macchina chiamante
  //------------------------------------------------------
  strcpy(str_chiamata, "          ");
  if(AGV[num_agv]->mission.Chiamata!=0){
     num_baia1=num_mag1=num_mac1=num_stz1=0;
     P.TestChiamataNum(AGV[num_agv]->mission.Chiamata, &num_mag1, &num_baia1, &num_mac1, &num_stz1);
     if(num_mac1>0  && num_mac1<=TOTCHIAMATE){LJ(ch[num_mac1]->Sigla,  str_chiamata, 10); str_chiamata[10]=NULL;}
     if(num_baia1>0 && num_baia1<=TOTBAIE   ){LJ(ba[num_baia1]->Sigla, str_chiamata, 10); str_chiamata[10]=NULL;}
  }
  else{
     if(num_baia>0  && num_baia<=TOTBAIE    ){LJ(ba[num_baia]->Sigla, str_chiamata, 10); str_chiamata[10]=NULL;}
  }
  //------------------------------------------------------------------------
  // Compilazione stringa di missione
  //------------------------------------------------------------------------
  strcpy(corpo, "");
  if(AGV[num_agv]->mission.Typ==MISS_SOLO_POSIZIONAMENTO && AGV[num_agv]->mission.PIntForzato==0 ) sprintf(corpo, "SIMPLE MISSION                  |%4d| %3s", AGV[num_agv]->mission.pend, forca);
  if(AGV[num_agv]->mission.Typ==MISS_SOLO_POSIZIONAMENTO && AGV[num_agv]->mission.PIntForzato!=0 ) sprintf(corpo, "APPROACH MISSION                |%4d| %3s", AGV[num_agv]->mission.pend, forca);
  if(AGV[num_agv]->mission.Typ==MISS_CARICO_DA_MACCHINA                                          ) sprintf(corpo, "LOAD FROM MACHINE %02d            |%4d| %3s", num_mac, AGV[num_agv]->mission.pend, forca);
  if(AGV[num_agv]->mission.Typ==MISS_SCARICO_SU_MACCHINA                                         ) sprintf(corpo, "UNLOAD ON MACHINE %02d            |%4d| %3s", num_mac, AGV[num_agv]->mission.pend, forca);
  if(AGV[num_agv]->mission.Typ==MISS_CARICO_DA_MAGAZZINO                                         ) sprintf(corpo, "LOAD FROM STORE %02d              |%4d| %3s", num_mag, AGV[num_agv]->mission.pend, forca);
  if(AGV[num_agv]->mission.Typ==MISS_SCARICO_SU_MAGAZZINO                                        ) sprintf(corpo, "UNLOAD FORM STORE %02d            |%4d| %3s", num_mag, AGV[num_agv]->mission.pend, forca);
  if(AGV[num_agv]->mission.Typ==MISS_CARICO_DA_BAIA                                              ) sprintf(corpo, "LOAD FROM BAIA %04d             |%4d| %3s", num_mag, AGV[num_agv]->mission.pend, forca);
  if(AGV[num_agv]->mission.Typ==MISS_SCARICO_SU_BAIA                                             ) sprintf(corpo, "UNLOAD FORM BAIA %04d           |%4d| %3s", num_mag, AGV[num_agv]->mission.pend, forca);
  //------------------------------------------------------------------------
  // Composizione stringa finale
  //------------------------------------------------------------------------
  sprintf(stringa, "%s |%s| %s  |", start, str_chiamata, corpo);
  file_cronologico_generico(filename,  stringa, 90000L);
  file_cronologico_generico(filename1, stringa, 70000L);

  return 0;
}


//----------------------------------------
//   Converti Allarmi TGV memotrizzati
//   nel file cronoall.dat
//----------------------------------------
// Recupero gli allarmi memorizzati nel
// file TEMALLX.DAT dove X = Num.Tgv e li
// memorizzo nel cronologico allarmi.
//
int process::ConvertiTempCronoall(char *nome_file_source, char *nome_file_dest, short int NumAgv, char *all_mess)
/***************************************************************************/
{
  #define MAXLINEACRON   600
  char buffer[201]="";
  char buffer1[201]="";
  char Tempo[21]=  "";
  char linea[5]=  "";
  int  err=0;
  FILE *ptrfile, *ptrfile1;
  int  i, k, pt=0;
  long int TimeAllarme;
  long int TimeAuto, DiffTempo;
  long int len=0;
  struct ALLTGV Appoggio;

  //--------------------------------------------------------
  // Se troppo lungo oltre i 3Kbyte allora SCROLL
  //--------------------------------------------------------
  pt = open(nome_file_dest, O_BINARY, S_IREAD);
  if(pt >-1){
     len = filelength(pt);
  }
  close(pt);
  if(len > 200000L){
     err = scroll_file(nome_file_dest, 0, 50, buffer);
     if(err) return 0;
  }
  //--------------------------------------------------------
  // Apre il file origine in lettura e l'altro in scrittura
  //--------------------------------------------------------
  ptrfile  = fopen(nome_file_source, "rb");
  if (ptrfile==0) {
    AGV[NumAgv]->mission.TimeRipristino=0;
    sprintf(all_mess, MESS[112], nome_file_source );
    unlink(nome_file_source);
    return 0;
  }
  ptrfile1 = fopen(nome_file_dest, "ab");
  if (ptrfile==0) {
    AGV[NumAgv]->mission.TimeRipristino=0;
    sprintf(all_mess, MESS[112], nome_file_dest );
    unlink(nome_file_source);
    return 0;
  }

  //--------------------------------------------------------
  // Recupero la struttura dal file
  //--------------------------------------------------------
  TimeAuto=time(NULL);
  strcpy( all_mess, "" );
  for(k=0; k<=MAX_ALLARM; k++){
     memset(&Appoggio, 0, sizeof(ALLTGV));
     i= fread(&Appoggio, 1, sizeof(ALLTGV), ptrfile);
     if (i != sizeof(Appoggio))  err = 1;
     //----------------------------------------
     // Se c'è err allora chiudo i file e
     // cancello quello temporaneo. Err può
     // essere a 1 solo quando ho scorso in
     // tutto il file o la lunghezza della
     // struttura non è uguale a ALLTGV
     //----------------------------------------
     if(err){
        AGV[NumAgv]->mission.TimeRipristino=0;
        fclose(ptrfile );
        fclose(ptrfile1);
        unlink(nome_file_source);
        return 0;
     }
     //----------------------------------------
     // Passaggio da file temporaneo a
     // CRONOALL.DAT
     //----------------------------------------
     // Calcolo Secondi da Data e Ora.
     //----------------------------------------
     timechar_to_timeint(&TimeAllarme, &Appoggio.Date[0], &Appoggio.Time[0]);
     TimeAuto= time(NULL);
     //----------------------------------------
     // Linea su cui è il TGV
     //----------------------------------------
     strcpy(linea, "node");
/*
     if(N.n[Appoggio.Posizione]->zp==ZONA_1_DIFA104 ) strcpy(linea, "104");
     if(N.n[Appoggio.Posizione]->zp==ZONA_2_DIFA103 ) strcpy(linea, "103");
     if(N.n[Appoggio.Posizione]->zp==ZONA_3_DIFA102 ) strcpy(linea, "102");
     if(N.n[Appoggio.Posizione]->zp==ZONA_4_DIFA101 ) strcpy(linea, "101");
     if(N.n[Appoggio.Posizione]->zp==ZONA_5_DIFA105 ) strcpy(linea, "105");
     if(N.n[Appoggio.Posizione]->zp==ZONA_6_AGM     ) strcpy(linea, "agm");
     if(N.n[Appoggio.Posizione]->zp==ZONA_7_MAG_UP  ) strcpy(linea, "mag");
     if(N.n[Appoggio.Posizione]->zp==ZONA_8_MAG_DOWN) strcpy(linea, "mag");
*/
     //----------------------------------------
     // Calcolo secondi in allarme
     //----------------------------------------
     DiffTempo= TimeAuto-TimeAllarme;
     CalcolaTempo(DiffTempo, Tempo);
     LJ(&Appoggio.Allarme[0], buffer1, 81);
     sprintf(buffer, "%s %s   %03d   %04d  %s   %s %s", Appoggio.Date, Appoggio.Time, Appoggio.NumAgv, Appoggio.Posizione, buffer1, Tempo, linea);
     Write_Line(ptrfile1, buffer);
  }
  //----------------------------------------------
  // Scrittura del file avvenuta correttamente
  //----------------------------------------------
  AGV[NumAgv]->mission.TimeRipristino=0;
  fclose(ptrfile );
  fclose(ptrfile1);
  unlink(nome_file_source);
  return 0;
}
//---------------------------------------------------------------------------
//----------------------------------------
// Converti: tempo espresso in secondi
// in hh.mm.ss
//----------------------------------------
//
int process::CalcolaTempo(long int TempoSecondi, char *Tempo)
/***************************************************************************/
{
  int Ore;
  int Minuti;
  int Secondi;

  strcpy(Tempo, "");
  //--------------------------------------------
  // Per trovare le ore divido per 3600, i
  // restuanti secondi li divido per 60 e trovo
  // minuti ecc...
  //--------------------------------------------
  Ore=(int)(TempoSecondi/3600);
  TempoSecondi=(long int)(TempoSecondi-(Ore*3600));
  Minuti=(int)(TempoSecondi/60);
  TempoSecondi=(long int)(TempoSecondi-(Minuti*60));
  Secondi=(int)(TempoSecondi);

  sprintf(Tempo, "%02d.%02d.%02d", Ore, Minuti, Secondi);
  return 0;
}
//---------------------------------------------------------------------------






//------------------------------------------------------------------------------------------
// CAMBIO BATTERIE
// read_cbat(int num_cb, struct CAMBIOBAT *cbb, char *all_mess);
// load_file_cbat(char *all_mess);
// save_record_cbat(int numero, char *all_mess);
// save_file_cbat(char *all_mess);
// test_punto_presente_cbat(char *all_mess, short int punto, short int *num_cb);
// cerca_record_cbat(struct CAMBIOBAT *cbb, int numero);
// setta_consensi_cbat(char *all_mess);
// num_record_cbatt();
//------------------------------------------------------------------------------------------
// ----------------------------------------------
//       read_cbat()
// ----------------------------------------------
// Legge il record "cb" in base a "num_cb" passato come parametro
//
int process::read_cbat(int num_cb, CAMBIOBAT *cbb, char *all_mess)
/***********************************************/
{
  int  stato=0;

  stato = cerca_record_cbat(cbb, num_cb );
  if(stato){
     //---------------------------------
     // record non trovato ! -> errore
     //---------------------------------
     sprintf(all_mess, "Error in read cambio batteria %d (%s)", num_cb, "read_cbat()");
     return stato;
  }
  return 0;
}
//------------------------------------------------------------------------------------------
// ------------------------
//  load_file_cbat()
// ------------------------
// Dal file CAMBIOBAT.DAT compila i record in memoria precedentemente allocati
//
int process::load_file_cbat(char *all_mess)
{
  int  err=0;
  int  i, len;
  int  handle;

  strcpy( all_mess, "OK" );
  //------------------------------
  // Azzera records in memoria
  //------------------------------
  for( i=0; i<=MAXBATT; i++){
     memset( cb[i], 0, sizeof( CAMBIOBAT ));
  }
  //------------------------------
  // Apertura file CAMBIOBAT.DAT
  //------------------------------
  //--------------------------------------------------------------------------
  // Controllo se il File può essere Letto aprendolo in modalità Condivisione
  //--------------------------------------------------------------------------
  handle = sopen( cbat_name_sorg, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
      close( handle );
      sprintf(all_mess, MESS[112], cbat_name_sorg);
      return 1;
  }
  //--------------------------------
  // Lettura file CAMBIOBAT.DAT
  //--------------------------------
  for( i=0; i<=MAXBATT; i++){
     len = read( handle, cb[i], sizeof( CAMBIOBAT ) );
     if (len != sizeof( CAMBIOBAT ))  err = 1;
     if(err){
        memset( cb[i], 0, sizeof( CAMBIOBAT ));
        sprintf(all_mess, MESS[351], i, cbat_name_sorg);
        break;
     }
  }
  close( handle );

  return err;
}
//------------------------------------------------------------------------------------------
// ------------------------
//  save_record_cbat()
// ------------------------
// Salva sul file CAMBIOBAT.DAT i records di macchine in memoria
//
int process::save_record_cbat(int numero, char *all_mess)
{
  int  err=0;
  long curpos;
  int  len;
  unsigned long length;
  int  handle;

  strcpy( all_mess, "OK" );
  //------------------------------
  // Apertura file CAMBIOBAT.DAT
  //------------------------------
  //--------------------------------------------------------------------------
  // Controllo se il File può essere Letto aprendolo in modalità Condivisione
  //--------------------------------------------------------------------------
  handle = sopen( cbat_name_dest, O_RDWR | O_BINARY , SH_DENYNO );
  if( handle<0 ){
     close( handle );
     sprintf(all_mess, MESS[112], cbat_name_dest);
     return 1;
  }
  //------------------------------
  // Controllo lunghezza file
  //------------------------------
  length=0;
  if(handle >-1) length = filelength(handle);
  if( length < ((long)(numero)* sizeof( CAMBIOBAT ))){
     sprintf(all_mess, MESS[113], cbat_name_dest);
     close(handle);
     return 1;
  }
  //--------------------------------------
  // Calcolo dell'OFFSET e posizionamento
  //--------------------------------------
  curpos = (long ) numero * sizeof( CAMBIOBAT );
  if( lseek( handle, curpos, SEEK_SET )<1){  // pos. il puntatore a inzio record
     sprintf(all_mess, "errore puntamento al record %d del file %s...", numero, cbat_name_dest);
     close( handle );
     return 1;
  }
  //-------------------------------
  // Scrittura Record CAMBIOBAT.DAT
  //-------------------------------
  len= write( handle, cb[numero], sizeof( CAMBIOBAT ));
  if(len != sizeof( CAMBIOBAT )){
     err=1;
     sprintf(all_mess, "errore scrittura del record %d del file %s...", numero, cbat_name_dest);
  }
  close(handle);

  return err;
}
//------------------------------------------------------------------------------------------
// ------------------------
//    save_file_cbat()
// ------------------------
// Salva sul file CAMBIOBAT.DAT i records delle macchine in memoria
//
int process::save_file_cbat(char *all_mess)
{
  int err=0;
  int i, len;
  int handle;

  strcpy( all_mess, "OK" );
  //------------------------------
  // Apertura file CAMBIOBAT.DAT
  //------------------------------
  //--------------------------------------------------------------------------
  // Controllo se il File può essere Letto aprendolo in modalità condivisione
  //--------------------------------------------------------------------------
  handle = sopen( cbat_name_dest, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
  if( handle<0 ){
     close( handle );
     sprintf(all_mess, "Errore scrittura file: %s", cbat_name_dest);
     return 1;
  }
  //----------------------------------
  // Ciclo Scrittura file CAMBIOBAT.DAT
  //----------------------------------
  for( i=0; i<=MAXBATT; i++){
     len= write( handle, cb[i], sizeof( CAMBIOBAT ));
     if (len != sizeof( CAMBIOBAT ))  err = 1;
     if(err){
         memset( cb[i], 0, sizeof( CAMBIOBAT ));
         sprintf(all_mess, "errore scrittura del record %d del file %s...", i, cbat_name_dest);
         break;
     }
  }
  close( handle );
  return err;
}
//------------------------------------------------------------------------------------------
// ----------------------------------------------
//       test_punto_presente_cbat()
// ----------------------------------------------
// Verifica se  "punto" è un nodo di un CambioBatteria
// Se sì ritorna in "num_cb" il numero del CambioBatteria trovato
//
int  process::test_punto_presente_cbat(char *all_mess, short int punto, short int *num_cb)
{
  bool trovato;
  int  n_rec, i, stato;

  strcpy( all_mess, "OK");
  //--------------------------------
  // Azzeramento variabili
  //--------------------------------
  trovato=false;
  *num_cb =0;

  //--------------------------------
  // Nodo non valido
  //--------------------------------
  if(punto<1 || punto>TOTPUNTI){
     sprintf(all_mess, "(0) Error node %d does not good for battery archive...", punto);
     return 1;
  }
  //--------------------------------
  //  cerca il primo record
  //--------------------------------
  for( n_rec=1; n_rec<=MAXBATT; n_rec++){
     if( cb[ n_rec ]->Num==0 ) continue;
     if(n_rec>TOTBATT) break;
     trovato = false;
     //--------------------------------------
     // Controllo nodo principale
     //--------------------------------------
     if(punto==cb[ n_rec ]->Punto){
        trovato = true;
        *num_cb = cb[ n_rec ]->Num;
        break;
     }
     //--------------------------------------
     // Controllo anche nodi ausiliari
     //--------------------------------------
     for(i=0; i<5; i++){
        if(cb[ n_rec ]->PuntoAux[i]!=punto) continue;
        trovato = true;
        *num_cb = cb[ n_rec ]->Num;
        break;
     }
     if(trovato==true) break;
  }
  if(*num_cb==0){
     sprintf(all_mess, "Error search node %d in battery archive...", punto);
     return 1;
  }

  return 0;
}
//------------------------------------------------------------------------------------------
// ------------------------
//    cerca_record_cbat()
// ------------------------
// Cerca il record memorizzato in memoria e lo copia nella struttura
// ch, restituisce errore in caso di superamento dei limiti o
// record non inizializzato
//
int process::cerca_record_cbat(struct CAMBIOBAT *cbb, int numero)
{
  if( numero > MAXBATT      ) return 1;
  if( cb[ numero ]->Num==0  ) return 1;

  memcpy( cbb, cb[ numero ], sizeof( CAMBIOBAT ) );

  return 0;
}
//------------------------------------------------------------------------------------------
// ------------------------------------
//    setta_consensi_cbat( )
// ------------------------------------
int process::setta_consensi_cbat(char *all_mess)
{
  int  err;
  int  x1, x2, y1, y2;
  int  count, distX, distY;
  bool Ingombro;
  bool OkIngresso;
  bool RichiestaIngresso;
  bool forche_fuori_ing;
  short int i, Idx, NumAgv, CBat, Punto, PuntoPrecedente;
  double dist;
  bool SalvaCBat[MAXBATT+2];
  bool AlmenoUnoInIngombro;

  err=0;
  strcpy( all_mess, "" );
  memset(&SalvaCBat, false, sizeof(SalvaCBat));

  //-----------------------------------------------------
  // A) Verifica se c'è un AGV in ingombro alla macchina
  //-----------------------------------------------------
  for(CBat=1; CBat<=MAXBATT; CBat++){
     if(cb[CBat]->Num<1         ) continue;
     if(cb[CBat]->Num>TOTBATT   ) continue;
     if(cb[CBat]->Punto<1       ) continue;
     if(cb[CBat]->Punto>TOTPUNTI) continue;
     //-------------------------------------------------------------
     // Memorizzo se caricabatteira in ok ingresso
     //-------------------------------------------------------------
     Punto             = cb[CBat]->Punto;
     Ingombro          = false;
     RichiestaIngresso = false;
     OkIngresso        = cb[CBat]->OkIng;
     //-------------------------------------------------------------
     // PUNTI AUSILIARI CAMBIOBATTERIA
     // Verifico se esiste un TGV destinato a un punto AUX
     // e in quel caso...
     //-------------------------------------------------------------
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        for(i=0; i<5; i++){
           if(cb[CBat]->PuntoAux[i]<1       ) continue;
           if(cb[CBat]->PuntoAux[i]>TOTPUNTI) continue;
           //-------------------------------------------------------
           // Memorizzo PuntoAux come destinazine TGV se impegnato
           // o destinazione missione
           //-------------------------------------------------------
           if(AGV[NumAgv]->stato.pos==cb[CBat]->PuntoAux[i] || AGV[NumAgv]->mission.pend==cb[CBat]->PuntoAux[i]){
              Punto = cb[CBat]->PuntoAux[i];
           }
        }
     }
     //-------------------------------------------------------------
     // Verifica se è sul nodo stazione
     //-------------------------------------------------------------
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        if(AGV[NumAgv]->stato.pos!=Punto) continue;
        Ingombro=true;
        AGV[ NumAgv ]->mission.BaiaInIngombro=0;
        RichiestaIngresso=true;
        break;
     }
     //-------------------------------------------------------------
     // Non più su nodo macchina
     //-------------------------------------------------------------
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        if(AGV[NumAgv]->mission.BaiaInIngombro!=Punto) continue;
        if(N.nodo_busy[Punto]==NumAgv                ) continue;
        AGV[ NumAgv ]->mission.BaiaInIngombro=0;
     }
     //-------------------------------------------------------------
     // Verifica se AGV in transito verso INGOMBRO STAZ MACCHINA
     //-------------------------------------------------------------
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        if(AGV[ NumAgv ]->mission.BaiaInIngombro!=Punto) continue;
        Ingombro=true;
        RichiestaIngresso=true;
        break;
     }
     //-------------------------------------------------------------
     // Considero fuori ingombro la navetta quando è sul primo nodo
     // a una distanza superiore ai 250cm dalla stazione di start.
     //-------------------------------------------------------------
     for( NumAgv=1; NumAgv<=MAXAGV; NumAgv++ ){
        if(AGV[ NumAgv ]->stato.pos==Punto       ) continue;
        if(AGV[ NumAgv ]->mission.pstart!=Punto  ) continue;
        //----------------------------------
        // Recupero coordinate
        //----------------------------------
        x1 = Punti[AGV[ NumAgv ]->stato.pos][0];
        y1 = Punti[AGV[ NumAgv ]->stato.pos][1];
        x2 = Punti[Punto][0];
        y2 = Punti[Punto][1];
        //----------------------------------
        // Se i nodi si trovano in retta la
        // distanza viene calcolata con il
        // teorema di pitagora.
        //----------------------------------
        distX = abs(x1-x2);
        distY = abs(y1-y2);
        //----------------------------------
        // Punti troppo lontani
        //----------------------------------
        if(distX>30000 || distX>30000) continue;
        //----------------------------------
        // Calcolo distanza dalla stazione
        //----------------------------------
        dist = (double)(sqrt_algo((distX*distX)+(distY*distY)));
        //----------------------------------
        // Se la distanza<250 sono ancora
        // in ingombro
        //----------------------------------
        if(dist<DIST_MANTIENI_ING_RULL) Ingombro=true;
        break;
     }
     //-------------------------------------------------------------
     // Verifica se è su nodo precedente in richiesta di accesso
     //-------------------------------------------------------------
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        //---------------------------------------------------------
        // RICHIESTA INGRESSO CON BIT ATTESA DA PLC TGV
        //---------------------------------------------------------
        if(AGV[NumAgv]->mission.pend==Punto && N.nodo_busy[Punto]==NumAgv){
           if(AGV[ NumAgv ]->stato.s.bit.inattesa){
              //----------------------------------------------
              // Memorizza che l'AGV fa richiesta di ingresso
              //----------------------------------------------
              RichiestaIngresso=true;
              break;
           }
        }
        //---------------------------------------------------------
        // RICHIESTA INGRESSO CON PINT NODO PRECEDENTE DA PC
        //---------------------------------------------------------
        if(AbilitaPIntPerConsensiMacchina==true && AGV[NumAgv]->mission.pend==Punto){
           //--------------------------------------------
           // Verifico distanza da destinazione
           //--------------------------------------------
           for(i=0; i<MAXPERCORSI; i++){
              if(AGV[NumAgv]->mission.punto[i]==0                     ) break;
              if(AGV[NumAgv]->mission.punto[i]==AGV[NumAgv]->stato.pos) break;
           }
           count=0;
           PuntoPrecedente=0;
           for(;i<MAXPERCORSI; i++){
              if(AGV[NumAgv]->mission.punto[i]==0) break;
              //----------------------------------------------
              // Raggiunge la destinazione --> skip!
              //----------------------------------------------
              if(AGV[NumAgv]->mission.punto[i]==AGV[ NumAgv ]->mission.pend && AGV[NumAgv]->mission.punto[i+1]==0) break;
              //----------------------------------------------
              // Memorizzo punto precedente a destinazione
              //----------------------------------------------
              if(AGV[ NumAgv ]->mission.punto[i+1]==AGV[ NumAgv ]->mission.pend){
                 PuntoPrecedente = AGV[ NumAgv ]->mission.punto[i];
              }
              //----------------------------------------------
              // Conteggio nodi mancanti a destinazione
              //----------------------------------------------
              count++;
           }
           //-------------------------------------------------------------------------
           // Se TGV su nodo precedente o a una distanza <=ANTICIPO_RICH_INGRESSO_MAC
           // dalla destinazione scatena la richiesta ingresso
           //-------------------------------------------------------------------------
           if(AGV[NumAgv]->stato.pos==PuntoPrecedente || count<=ANTICIPO_RICH_INGRESSO_MAC){
              //----------------------------------------------
              // Memorizza che l'AGV fa richiesta di ingresso
              //----------------------------------------------
              RichiestaIngresso=true;
              //----------------------------------------------
              // IMPORTANTE: Se il TGV ha già occupato
              // la destinazione il PINT non è più forzabile
              // sul nodo precedente, quindi è necessario
              // alzare anche l'ingombro
              //----------------------------------------------
              if(N.nodo_busy[Punto]==NumAgv){
                 Ingombro=true;
                 AGV[NumAgv]->mission.BaiaInIngombro=Punto;
              }
              break;
           }
        }
     }
     //-------------------------------------------------------------
     // Verifica se è sul nodo precedente alla stazione
     //-------------------------------------------------------------
     if( OkIngresso ){
        for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
           for(i=1; i<MAXPERCORSI; i++){                         // IMPORTANTE: scarto il nodo di start missione perchè il controllo
              if(AGV[NumAgv]->mission.punto[i]==0     ) break;   // prevede di recuperare il punto_precedente con indice i-1.
              if(AGV[NumAgv]->mission.punto[i]==Punto ) break;
           }
           if(AGV[ NumAgv ]->mission.punto[i]==Punto){
              PuntoPrecedente=AGV[ NumAgv ]->mission.punto[i-1];
              if(AGV[NumAgv]->stato.pos!=PuntoPrecedente)  continue;
              //----------------------------------------------
              // Memorizza che l'AGV sta entrando in ingombro
              //----------------------------------------------
              Ingombro=true;
              AGV[NumAgv]->mission.BaiaInIngombro=Punto;
              RichiestaIngresso=true;
              break;
           }
        }
     }
     //-------------------------------------------------------------
     // Verifica se è su nodo precedente e sta entrando su stazione
     //-------------------------------------------------------------
     if( OkIngresso ){
        for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
           if(N.nodo_busy[Punto]!=NumAgv           ) continue;
           if(!AGV[ NumAgv ]->stato.s.bit.inattesa ) continue;
           //----------------------------------------------
           // Memorizza che l'AGV sta entrando in ingombro
           //----------------------------------------------
           Ingombro=true;
           AGV[NumAgv]->mission.BaiaInIngombro=Punto;
           RichiestaIngresso=true;
           break;
        }
     }
     //------------------------------------------------------------------
     // 27/11/2016 Su richiesta di BARGI ingombro solo quando è sul NODO
     //------------------------------------------------------------------
     AlmenoUnoInIngombro=false;
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        if(AGV[NumAgv]->stato.pos!=Punto) continue;
        AlmenoUnoInIngombro=true;
        break;
     }
     if(AlmenoUnoInIngombro==false) Ingombro=false;
     //-----------------------------------------------
     // Verifico se c'è una variazione di stato
     //-----------------------------------------------
     if(cb[CBat]->RichIng==false && RichiestaIngresso==true ) SalvaCBat[CBat]=true;
     if(cb[CBat]->TgvIng ==false && Ingombro==true          ) SalvaCBat[CBat]=true;
     if(cb[CBat]->RichIng==true  && RichiestaIngresso==false) SalvaCBat[CBat]=true;
     if(cb[CBat]->TgvIng ==true  && Ingombro==false         ) SalvaCBat[CBat]=true;
     //-----------------------------------------------
     // Compilazione consensi su struttura
     //-----------------------------------------------
     cb[CBat]->TgvIng  = Ingombro;
     cb[CBat]->RichIng = RichiestaIngresso;
     //-----------------------------------------------
     // E) Se Consenso Ok azzera DM BLOCCO AGV
     //-----------------------------------------------
     if(OkIngresso){
        for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
           if(!AGV[ NumAgv ]->stato.s.bit.inattesa) continue;
           if (AGV[ NumAgv ]->mission.pend!=Punto ) continue;
           //-----------------------------------------
           // Comando al TGV di proseguire DM89=0
           //-----------------------------------------
           if(AGV[ NumAgv ]->TrasmettiResetBloccoIngAGV(all_mess)) return 1;
        }
     }
  }
  //-----------------------------------------------
  // F) Setta Consenso al movimento (forche alte)
  //-----------------------------------------------
  for(CBat=1; CBat<=MAXBATT; CBat++){
     if(cb[CBat]->Num<1      ) continue;
     if(cb[CBat]->Num>TOTBATT) continue;
     //-----------------------------------------------------
     // PUNTI AUSILIARI CARIBATTERIA
     // Verifico se esiste un TGV destinato a un punto AUX
     // e in quel caso...
     //-----------------------------------------------------
     Punto = cb[CBat]->Punto;
     for(NumAgv=1; NumAgv<=MAXAGV; NumAgv++){
        for(i=0; i<5; i++){
           if(cb[CBat]->PuntoAux[i]<1       ) continue;
           if(cb[CBat]->PuntoAux[i]>TOTPUNTI) continue;
           //-------------------------------------------------------
           // Memorizzo PuntoAux come destinazine TGV se impegnato
           // o destinazione missione
           //-------------------------------------------------------
           if(AGV[NumAgv]->stato.pos==cb[CBat]->PuntoAux[i] || AGV[NumAgv]->mission.pend==cb[CBat]->PuntoAux[i]){
              Punto = cb[CBat]->PuntoAux[i];
           }
        }
     }
  }

  //-----------------------------------------------
  // Z) Salva record caricabatteria modificati
  //-----------------------------------------------
  for(CBat=1; CBat<=MAXBATT; CBat++){
     if(cb[CBat]->Num<1         ) continue;
     if(cb[CBat]->Num>TOTBATT   ) continue;
     if( SalvaCBat[CBat]==false ) continue;
     //----------------------------
     // Salva Record macchina
     //----------------------------
     err = save_record_cbat(CBat, all_mess);
     if(err!=0) break;
  }
  return err;
}
// ------------------------
//    num_record_cbatt()
// ------------------------
// Restituisce il numero di Record registrati, viene fatto un controllo
// sul numero del CambioBatterie (se diverso da zero)
//
int process::num_record_cbatt()
/**********************************/
{
  int numero_rec=0;
  int i;

  for( i=0; i<MAXBATT; i++){
     if( cb[i]->Num >0 ) numero_rec++;
  }

  return numero_rec;
}

// ----------------------------------
//   load_consensi_cbat_SPEC()
// ---------------------------------
//  Funzione che legge i consensi dal cambiobatteria
//
int process::load_consensi_cbat(int NumCBatt, char *all_mess)
{
  static int CicliLetturaCB=1;
  int  errPlc=0;
  int  i;
  int  NumPlc;
  bool SempreON=true;
  char buffer[101]="";
  short int CanaleIniziale, NumCanali;
  struct CAMBIOBAT OldCBat;
  static WORD RifrescoPerClient=0;
  WORD dm[351];
  //----------------------------------------------------------------------
  errPlc=0;
  strcpy(all_mess, "");
  memset(&dm[0], 0, sizeof(dm));
  //--------------------------------------------------
  // se break_plc allora SKIP !!!!
  //--------------------------------------------------
  if(break_Com[COM2]) return 0;
  //---------------------------------------------------------------------
  // Verifica se almeno un cambiobatterie è abilitato
  //---------------------------------------------------------------------
  if(NumCBatt<1 || NumCBatt>TOTBATT) return 0;
  if(P.cb[NumCBatt]->Num==0 || P.cb[NumCBatt]->Num>TOTBATT) return 0;
  if(P.cb[NumCBatt]->Punto==0) return 0;
  //---------------------------------------------------------------
  // Abilitazione ciclo di lettura
  // normalmente una volta ogni n cicli
  // se c'è un AGV in arrivo sul CB leggiamo di continuo
  //---------------------------------------------------------------
  P.cb[NumCBatt]->AbilitaReadWrite=false;

  if(CicliLetturaCB==1 ) P.cb[NumCBatt]->AbilitaReadWrite=true;
  if(CicliLetturaCB<=20) CicliLetturaCB++;
  else CicliLetturaCB=1;

  for(i=1;i<=MAXAGV;i++){
     if(AGV[i]->mission.pend==P.cb[NumCBatt]->Punto) P.cb[NumCBatt]->AbilitaReadWrite=true;
  }
  RifrescoPerClient++;
  //---------------------------------------------------------------
  // (A) Letttura da PLC
  //---------------------------------------------------------------
  if( P.cb[NumCBatt]->EsclComunic==false && P.cb[NumCBatt]->AbilitaReadWrite==true){
     //------------------------------------------------------
     NumPlc=P.cb[NumCBatt]->Plc;
     CanaleIniziale = 1100;
     NumCanali      = 100;
     errPlc = OM.long_ld_canale(&dm[0], NumPlc, "RD", CanaleIniziale, NumCanali, buffer);
     P.cb[NumCBatt]->PlcErr = errPlc;
     if(errPlc){
        sprintf(all_mess, "Err. (PLC Battery Change) Consent Read - %s", buffer);
        return errPlc;
     }
     memcpy(&OldCBat, P.cb[NumCBatt], sizeof(CAMBIOBAT));
     if(SempreON){
        // WatchDog
        P.cb[NumCBatt]->WatchDog = dm[0];
        OldCBat.WatchDog         = dm[0];

        // Compila Bit Generali Macchina
        P.cb[NumCBatt]->Automatico = TestBit((char *) &dm[1], 0);
        P.cb[NumCBatt]->Blocco     = TestBit((char *) &dm[1], 1);
        P.cb[NumCBatt]->OkIng = false;
        if(P.cb[NumCBatt]->Punto>0) P.cb[NumCBatt]->OkIng = TestBit((char *) &dm[1], 2);

        // Compila Bit Allarmi
        memset(&P.cb[NumCBatt]->BitAllarmi[0], 0, sizeof(P.cb[NumCBatt]->BitAllarmi));
        P.cb[NumCBatt]->BitAllarmi[0]= dm[60];
        P.cb[NumCBatt]->BitAllarmi[1]= dm[61];
        P.cb[NumCBatt]->BitAllarmi[2]= dm[62];
        P.cb[NumCBatt]->BitAllarmi[3]= dm[63];
     }
     //-----------------------------------------------------------------
     // (Z) Verifico se è variata la struttura e salvo su file
     //-----------------------------------------------------------------
     if((memcmp(P.cb[NumCBatt], &OldCBat, sizeof(CAMBIOBAT))!=0) || RifrescoPerClient>50){
        P.save_record_cbat(NumCBatt, all_mess);
        RifrescoPerClient=0;
     }
  }
  return errPlc;
}
//---------------------------------------------------------------------------
// ----------------------------------
//   write_consensi_cbat_SPEC()
// ---------------------------------
//  Funzione che scrive i consensi al cambiobatteria
//
int process::write_consensi_cbat(int NumCBatt, char *all_mess)
{
  int  errPlc=0;
  int  i;
  int  NumPlc;
  bool SempreON=true;
  char buffer[101]="";
  short int CanaleIniziale, NumCanali;
  struct CAMBIOBAT OldCBat;
  WORD dm[351];
  //----------------------------------------------------------------------
  errPlc=0;
  strcpy(all_mess, "");
  memset(&dm[0], 0, sizeof(dm));
  //--------------------------------------------------
  // se break_plc allora SKIP !!!!
  //--------------------------------------------------
  if(break_Com[COM2]) return 0;

  if(P.cb[NumCBatt]->PlcErr) return 0;
  // Eseguiamo il ciclo di scrittura solo se abbiamo fatto precedentemente una lettura
  if(P.cb[NumCBatt]->AbilitaReadWrite==false) return 0;
  //---------------------------------------------------------------------
  // Verifica se almeno un cambiobatterie è abilitato
  //---------------------------------------------------------------------
  if(NumCBatt<1 || NumCBatt>TOTBATT)                        return 0;
  if(P.cb[NumCBatt]->Num==0 || P.cb[NumCBatt]->Num>TOTBATT) return 0;
  if(P.cb[NumCBatt]->Punto==0)                              return 0;
  //---------------------------------------------------------------
  // (A) Letttura da PLC
  //---------------------------------------------------------------
  if( P.cb[NumCBatt]->EsclComunic==false ){
     //------------------------------------------------------
     memcpy(&OldCBat, P.cb[NumCBatt], sizeof(CAMBIOBAT));
     if(SempreON){

        // WatchDog
        dm[0] = P.cb[NumCBatt]->WatchDog + 1;

        // Compila Bit Generali Macchina
        if(P.cb[NumCBatt]->RichIng == true)  SetBit((char *) &dm[1], 0);
        if(P.cb[NumCBatt]->TgvIng  == true)  SetBit((char *) &dm[1], 1);
     }
     //-----------------------------------------------------------------
     // (Z) Verifico se è variata la struttura e salvo su file
     //-----------------------------------------------------------------
     if(memcmp(P.cb[NumCBatt], &OldCBat, sizeof(CAMBIOBAT))!=0){
        P.save_record_cbat(NumCBatt, all_mess);
     }

     if(break_Com[COM2]==false){
        NumPlc=P.cb[NumCBatt]->Plc;
        CanaleIniziale = 1000;
        NumCanali      = 10;
        errPlc = OM.wr_canale(&dm[0], NumPlc, "WD", CanaleIniziale, NumCanali, buffer);
        if(errPlc) sprintf(all_mess, "Err. (PLC Battery Change) Consent Write - %s", buffer);
     }
     P.cb[NumCBatt]->PlcErr = errPlc;
  }
  return errPlc;
}
//---------------------------------------------------------------------------
//------------------------------------------------------
//   LogMovimenti()
//------------------------------------------------------
// LOG_TRACCIA_MOVIMENTI
//
int process::LogMovimenti(char *Operazione, char *Coordinate, short int num_agv, struct BOX BoxA, struct BOX BoxB, short int TypPresa, int NMacc)
{
  int  i, err;
  char buffer[501]="";
  char buffer1[101]="";
  char NomeFile[201]="LogMovim.txt";
  char NomeFile1[201]="LogBaiaC.txt";
  char DescTipoBox[31]="";

  //--------------------------------------------------------------
  // Ammessi solo LOG DI MOVIMENTI
  //--------------------------------------------------------------
  if(TypPresa==N_NO_FORCHE) return 0;

  //--------------------------------
  if(NMacc!=0) sprintf(NomeFile1,"LogBaiaC%03d.txt",NMacc);

  err=0;
  //--------------------------------------------------------------
  // FORCA "A" - Compila la stringa da salvare nel LOG
  //--------------------------------------------------------------
  if(TypPresa==N_FORCA_A || TypPresa==N_FORCA_AB){
     memset(&buffer[0],  NULL, sizeof(buffer));
     memset(&buffer1[0], NULL, sizeof(buffer1));
     //----------------------------------------------------
     // Recupero descrizione TIPOPALLET
     //----------------------------------------------------
     sprintf( DescTipoBox, "%d-%s", BoxA.TipoBox,DESCTIPOBOX[BoxA.TipoBox] );
     //----------------------------------------------------
     // Compilazione prima parte con DATI GLOBALI BOX
     //----------------------------------------------------
     sprintf(buffer,";%s;%02d;%s;%d;%d;%s;%d;%d",
        Operazione,
        num_agv,
        Coordinate,
        BoxA.Codice,
        BoxA.Lotto,
        DescTipoBox,
        BoxA.CarWash,
        BoxA.Basso);
     //----------------------------------------------------
     // Ciclo per tutti gli UDC PREVISTI
     //----------------------------------------------------
     for(i=0; i<=MAXUDCBOX; i++){
      //sprintf(buffer1, ";%s;%s;%s;%d", BoxA.Prd[i].Udc, BoxA.Prd[i].batch, BoxA.Prd[i].state, BoxA.Prd[i].location);
        sprintf(buffer1, ";%s;%s;%s;%s;%d", BoxA.Prd[i].Udc, BoxA.Prd[i].batch, BoxA.Prd[i].shift, BoxA.Prd[i].state, BoxA.Prd[i].location);
        strcat(buffer, buffer1);
     }
     //----------------------------------------------------
     // Inserimento riga nel LOG
     //----------------------------------------------------
     err=file_cronologico_generico(NomeFile,buffer, 600000L);                // LogMovim.txt
     if(NMacc!=0) err=file_cronologico_generico(NomeFile1,buffer, 200000L);   // LogMacc%03d.txt
  }
  //--------------------------------------------------------------
  // FORCA "B" - Compila la stringa da salvare nel LOG
  //--------------------------------------------------------------
  if(TypPresa==N_FORCA_B || TypPresa==N_FORCA_AB){
     memset(&buffer[0],  NULL, sizeof(buffer));
     memset(&buffer1[0], NULL, sizeof(buffer1));
     //----------------------------------------------------
     // Recupero descrizione TIPOPALLET
     //----------------------------------------------------
     sprintf( DescTipoBox, "%d-%s", BoxB.TipoBox,DESCTIPOBOX[BoxB.TipoBox] );
     //----------------------------------------------------
     // Compilazione prima parte con DATI GLOBALI BOX
     //----------------------------------------------------
     sprintf(buffer,";%s;%02d;%s;%d;%d;%s;%d;%d",
        Operazione,
        num_agv,
        Coordinate,
        BoxB.Codice,
        BoxB.Lotto,
        DescTipoBox,
        BoxB.CarWash,
        BoxB.Basso);
     //----------------------------------------------------
     // Ciclo per tutti gli UDC PREVISTI
     //----------------------------------------------------
     for(i=0; i<MAXUDCBOX; i++){
      //sprintf(buffer1, ";%s;%s;%s;%d", BoxB.Prd[i].Udc, BoxB.Prd[i].batch, BoxB.Prd[i].state, BoxB.Prd[i].location);
        sprintf(buffer1, ";%s;%s;%s;%s;%d", BoxB.Prd[i].Udc, BoxB.Prd[i].batch, BoxB.Prd[i].shift, BoxB.Prd[i].state, BoxB.Prd[i].location);
        strcat(buffer, buffer1);
     }
     //----------------------------------------------------
     // Inserimento riga nel LOG
     //----------------------------------------------------
     err=file_cronologico_generico(NomeFile,buffer, 600000L);                // LogMovim.txt
     if(NMacc!=0) err=file_cronologico_generico(NomeFile1,buffer, 200000L);   // LogMacc%03d.txt
  }
  return err;
}
