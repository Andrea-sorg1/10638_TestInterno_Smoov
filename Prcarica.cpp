// ------------------------------------------------------------------------
//             file     PRCARICA.CPP
// ------------------------------------------------------------------------
// Funzione di carico e scarico dei DATI cestone da/verso AGV e da/verso
// MACCHINE MAGAZZINI BAIE 
//
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <mem.h>
#include <math.h>

#include "Tecnofer.h"
#include "SQLDB.h"

//  ---------------------
//   STRUTTURA NODI
//  ---------------------
//  0,1 = x,y posizione centrale nodo
//  2 = numero nodo
//  3 = occupazione agv ( numero AGV oppure libero = 0 )
//-------------------------------------------------------
extern int Punti[][5];

// ---------------------------
//   STRUTTURA GRAFICHE
// ---------------------------
//
//---------------------------------------------
extern int Baie[][4];
extern int Macch[][5];

// ------------------------------
//   CaricoDaMacchina()
// ------------------------------
// Questa funzione setta il cestone caricato su AGV
//
int process::CaricoDaMacchina(short int num_agv, char *all_mess)
/********************************************************/
{
/*
  int   i, j, err=0;
  bool  lato_ingresso;
  short int pos_trovata;
  short int num_mac, num_stz;
  short int TypPresa;
  short int Posizione, PosizioneTgv;
  short int ID;
  struct    CHIAMATE mac;

  num_mac=num_stz=0;
  memset(&mac, 0, sizeof(CHIAMATE));
  //--------------------------------------------------------------------
  // Verifica se la destinazione AGV  un punto chiamante
  // se si, compila "num_ch" con la coppia KY = macchina,stazione.
  //--------------------------------------------------------------------
  if(test_punto_presente_mac(all_mess, AGV[num_agv]->mission.pend, 0, num_agv, &num_mac, &num_stz)) return 1;
  if(num_mac==0) return 1;
  //--------------------------------------------------------------------
  // Verifico attendibilità della MAC
  //--------------------------------------------------------------------
  if(read_chiamate( num_mac, &mac, all_mess)) return 1;

  //--------------------------------------------------------------------
  // Recupero la posizione di deposito dal nodo di destinazione
  //--------------------------------------------------------------------
  lato_ingresso=false;
  if(mac.Staz[num_stz].PuntoIng!=mac.Staz[num_stz].PuntoOut && AGV[num_agv]->mission.pend==mac.Staz[num_stz].PuntoIng) lato_ingresso=true;

  //---------------------------------------------------
  // Compilazione dati struttura TGV cestone a bordo
  //
  // Controllando il TypPresa:
  // 1 = ForcaB(Bassa)
  // 2 = ForcaA(Alta)
  // 3 = ForcaAB(Alta e Bassa)
  //---------------------------------------------------
  TypPresa =  AGV[num_agv]->mission.TypPresa;

  //---------------------------------------------------
  // Postazione non in disponibile --> skip!!
  // N.B. Le forche B possono prel/dep solo in basso
  //---------------------------------------------------
  if(P.VerificaTipoPrelievo(lato_ingresso, num_mac, num_stz, &Posizione, &PosizioneTgv, all_mess)) return 1;

  AGV[num_agv]->mission.Provenienza  = (short int)((100*PROV_MACCHINE)+num_mac);
  //------------------------
  // Forca A
  //------------------------
  if( TypPresa==N_FORCA_A ){
     //--------------------------------------
     // MACCHINE DI CARICO
     // Recupero i dati dalla macchina
     //--------------------------------------
     if(mac.Tipo==MC_AVM || mac.Tipo==MC_ADS || mac.Tipo==MC_ADM){
        memset(&AGV[num_agv]->mission.BoxA[0], 0, sizeof(BOX));
        AGV[num_agv]->mission.BoxA[0].ID             = mac.Staz[num_stz].Pos[Posizione].Box.ID;
        AGV[num_agv]->mission.BoxA[0].StatoLav       = ST_BOX_CRUDO;
        AGV[num_agv]->mission.BoxA[0].Provenienza    = PR_BOX_CA;
        AGV[num_agv]->mission.BoxA[0].StatoBox       = mac.Staz[num_stz].StatoBox;
        AGV[num_agv]->mission.BoxA[0].TipoMateriale  = mac.Staz[num_stz].TipoMateriale;
        AGV[num_agv]->mission.BoxA[0].TipoBox        = mac.Staz[num_stz].TipoBoxVuoto;
        AGV[num_agv]->mission.BoxA[0].IdEssiccazione = mac.Staz[num_stz].IDEssiccazione;
        AGV[num_agv]->mission.BoxA[0].TempoInMag     = mac.Staz[num_stz].TempoInMag;
        AGV[num_agv]->mission.BoxA[0].OverTime       = 0;
        AGV[num_agv]->mission.BoxA[0].NonConforme    = mac.Staz[num_stz].NonConforme;
        AGV[num_agv]->mission.BoxA[0].TimeCR         = mac.Staz[num_stz].TimeChiamata;
     }
     //--------------------------------------
     // MACCHINE ESSICCATOIO
     // Lascio i dati del box invariati
     //--------------------------------------
     if(mac.Tipo==MC_ESSICCATOIO){
        memcpy(&AGV[num_agv]->mission.BoxA[0], &mac.Staz[num_stz].Pos[Posizione].Box, sizeof(BOX));
        AGV[num_agv]->mission.BoxA[0].StatoLav    = ST_BOX_ESSICC;
        AGV[num_agv]->mission.BoxA[0].Provenienza = PR_BOX_ES;
        AGV[num_agv]->mission.BoxA[0].TimeES      = mac.Staz[num_stz].TimeChiamata;
     }
     if(mac.Tipo==MC_SMALTERIA){
        memset(&AGV[num_agv]->mission.BoxA[0], 0, sizeof(BOX));
        AGV[num_agv]->mission.BoxA[0].ID             = mac.Staz[num_stz].Pos[Posizione].Box.ID;
        AGV[num_agv]->mission.BoxA[0].StatoLav       = ST_BOX_VUOTO;
        AGV[num_agv]->mission.BoxA[0].Provenienza    = PR_BOX_SC;
        AGV[num_agv]->mission.BoxA[0].StatoBox       = mac.Staz[num_stz].StatoBox;
        AGV[num_agv]->mission.BoxA[0].TipoMateriale  = 0;
        AGV[num_agv]->mission.BoxA[0].TipoBox        = mac.Staz[num_stz].TipoBoxVuoto;
        AGV[num_agv]->mission.BoxA[0].IdEssiccazione = 0;
        AGV[num_agv]->mission.BoxA[0].TempoInMag     = 0;
     }
     //---------------------------------------------------------
     // Posizione box a bordo TGV
     //---------------------------------------------------------
     AGV[num_agv]->mission.BoxA[0].Pos = num_agv;
     if(AGV[num_agv]->mission.BoxA[0].StatoLav==ST_BOX_VUOTO  ) AGV[num_agv]->mission.PresCEST_A=PRES_VUOTO;
     if(AGV[num_agv]->mission.BoxA[0].StatoLav==ST_BOX_CRUDO  ) AGV[num_agv]->mission.PresCEST_A=PRES_PIENO;
     if(AGV[num_agv]->mission.BoxA[0].StatoLav==ST_BOX_ESSICC ) AGV[num_agv]->mission.PresCEST_A=PRES_PIENO_ES;
     //---------------------------------------------------------
     // Azzeramento  dati stazione macchina
     //---------------------------------------------------------
     memset(&mac.Staz[num_stz].Pos[Posizione].Box, 0, sizeof(BOX));
     //---------------------------------------------------------
     // Azzeramento presenza e decremento postazioni occupate
     //---------------------------------------------------------
     mac.Staz[num_stz].Pos[Posizione].Presenza = false;
     mac.Staz[num_stz].NumCestoni--;
  }
  //------------------------
  // Forca B
  //------------------------
  if( TypPresa== N_FORCA_B ){
     //--------------------------------------
     // MACCHINE DI CARICO
     // Recupero i dati dalla macchina
     //--------------------------------------
     if(mac.Tipo==MC_AVM || mac.Tipo==MC_ADS || mac.Tipo==MC_ADM){
        memset(&AGV[num_agv]->mission.BoxB[0], 0, sizeof(BOX));
        AGV[num_agv]->mission.BoxB[0].ID             = mac.Staz[num_stz].Pos[Posizione].Box.ID;
        AGV[num_agv]->mission.BoxB[0].StatoLav       = ST_BOX_CRUDO;
        AGV[num_agv]->mission.BoxB[0].Provenienza    = PR_BOX_CA;
        AGV[num_agv]->mission.BoxB[0].StatoBox       = mac.Staz[num_stz].StatoBox;
        AGV[num_agv]->mission.BoxB[0].TipoMateriale  = mac.Staz[num_stz].TipoMateriale;
        AGV[num_agv]->mission.BoxB[0].TipoBox        = mac.Staz[num_stz].TipoBoxVuoto;
        AGV[num_agv]->mission.BoxB[0].IdEssiccazione = mac.Staz[num_stz].IDEssiccazione;
        AGV[num_agv]->mission.BoxB[0].TempoInMag     = mac.Staz[num_stz].TempoInMag;
        AGV[num_agv]->mission.BoxB[0].OverTime       = 0;
        AGV[num_agv]->mission.BoxB[0].NonConforme    = mac.Staz[num_stz].NonConforme;
        AGV[num_agv]->mission.BoxB[0].TimeCR         = mac.Staz[num_stz].TimeChiamata;
     }
     //--------------------------------------
     // MACCHINE ESSICCATOIO
     // Lascio i dati del box invariati
     //--------------------------------------
     if(mac.Tipo==MC_ESSICCATOIO){
        memcpy(&AGV[num_agv]->mission.BoxB[0], &mac.Staz[num_stz].Pos[Posizione].Box, sizeof(BOX));
        AGV[num_agv]->mission.BoxB[0].StatoLav    = ST_BOX_ESSICC;
        AGV[num_agv]->mission.BoxB[0].Provenienza = PR_BOX_ES;
        AGV[num_agv]->mission.BoxB[0].TimeES      = mac.Staz[num_stz].TimeChiamata;
     }
     if(mac.Tipo==MC_SMALTERIA){
        memset(&AGV[num_agv]->mission.BoxB[0], 0, sizeof(BOX));
        AGV[num_agv]->mission.BoxB[0].ID             = mac.Staz[num_stz].Pos[Posizione].Box.ID;
        AGV[num_agv]->mission.BoxB[0].StatoLav       = ST_BOX_VUOTO;
        AGV[num_agv]->mission.BoxB[0].Provenienza    = PR_BOX_SC;
        AGV[num_agv]->mission.BoxB[0].StatoBox       = mac.Staz[num_stz].StatoBox;
        AGV[num_agv]->mission.BoxB[0].TipoMateriale  = 0;
        AGV[num_agv]->mission.BoxB[0].TipoBox        = mac.Staz[num_stz].TipoBoxVuoto;
        AGV[num_agv]->mission.BoxB[0].IdEssiccazione = 0;
        AGV[num_agv]->mission.BoxB[0].TempoInMag     = 0;
     }
     //---------------------------------------------------------
     // Posizione box a bordo TGV
     //---------------------------------------------------------
     AGV[num_agv]->mission.BoxB[0].Pos = num_agv;
     if(AGV[num_agv]->mission.BoxB[0].StatoLav==ST_BOX_VUOTO  ) AGV[num_agv]->mission.PresCEST_B=PRES_VUOTO;
     if(AGV[num_agv]->mission.BoxB[0].StatoLav==ST_BOX_CRUDO  ) AGV[num_agv]->mission.PresCEST_B=PRES_PIENO;
     if(AGV[num_agv]->mission.BoxB[0].StatoLav==ST_BOX_ESSICC ) AGV[num_agv]->mission.PresCEST_B=PRES_PIENO_ES;
     //---------------------------------------------------------
     // Azzeramento  dati stazione macchina
     //---------------------------------------------------------
     memset(&mac.Staz[num_stz].Pos[Posizione].Box, 0, sizeof(BOX));
     //---------------------------------------------------------
     // Azzeramento presenza e decremento postazioni occupate
     //---------------------------------------------------------
     mac.Staz[num_stz].Pos[Posizione].Presenza = false;
     mac.Staz[num_stz].NumCestoni--;
  }

  //-------------------------------
  // Forca A+B (!!!IMPOSSIBILE!!!)
  //-------------------------------
  if( TypPresa== N_FORCA_AB ){
     sprintf(all_mess, "Not possible load with two fork on machine nr.%d!", mac.Num);
     return 1;
  }

  //--------------------------------------------------------------------
  // Azzeramento chiamata su TGV se completamente scarico
  //--------------------------------------------------------------------
  AGV[num_agv]->mission.Chiamata = 0;

  //--------------------------------------------------------------------
  // Azzeramento chiamata su stazione se svuotata + azzeramento ID
  // Essicazione max
  //--------------------------------------------------------------------
  mac.Staz[num_stz].TimeChiamata = time(NULL);
  if(mac.Staz[num_stz].NumCestoni==0){
     mac.Staz[num_stz].Espulsione = false;
     mac.IDEssicMacMax=0;      // Azzeramento ultimo ID ESSICAZIONE MACCHINA
  }
  //--------------------------------------------------------------------
  // Memorizza FIne Operazione da resettare
  //--------------------------------------------------------------------
  mac.Staz[num_stz].NumCicliResetFineOpraz= NUM_CICLI_RESET_FINEOP;

  //--------------------------------------------------------------------
  // Reset del flag di start CICLO ESSICCAZIONE
  //--------------------------------------------------------------------
  mac.RichCicloEs=false;

  //--------------------------------------------------------------------
  // salvataggio struttura TGV
  //--------------------------------------------------------------------
  if(!err){
     err = AGV[num_agv]->save_mission(all_mess);
  }
  //--------------------------------------------------------------------
  // salvataggio struttura macchine
  //--------------------------------------------------------------------
  if(!err){
     memcpy( ch[mac.Num], &mac, sizeof(CHIAMATE));
     err = save_record_chiamate(mac.Num, all_mess);
  }
  //----------------------------------------------------------------
  // GESTIONE UDC
  // da stuttura software ad array UDC per aggiornamento SQL
  //
  // !!! IMPORTANTE !!!
  // dopo aver aggiornato la struttura globale del software
  //----------------------------------------------------------------
  if(GestioneUdc==1){
     // -- SET LOCAZIONI SU UDC A BORDO TGV
     if(!err) err = B.StructToUdcTmp(CMD_WR_SQL_UDC_SET_LOC, NumTgv, 0, 0, 0, UDC_ALL_STZ, UDC_ALL_POS, UDC_ALL_LEV, TipoPesa, all_mess);
  }
  return err;

*/
  return 0;
}
// ------------------------------
//   CaricoDaBaia()
// ------------------------------
// Questa funzione setta il cestone caricato su AGV
//
int process::CaricoDaBaia(short int num_agv, char *all_mess)
/********************************************************/
{
  int   i, j, k, err=0;
  int   PosInPresa=POSINPRESA;
  short int num_baia;
  short int TypPresa;
  //short int ID;
  short int z, n, w;
  short int tot_palA, tot_palB;
  long int TimeAttuale;
  struct BAIE bb;
  WORD dm[3];
  WORD stato_presa_da_tgvB;
  WORD stato_presa_da_tgvA;
  bool abilita_sovrapposizione_prese_incomplete;
  char Coordinate[101]="";
  char Operazione[101]="Bay Pickup";

  num_baia=0;
  memset(&bb, 0, sizeof(BAIE));
  //--------------------------------------------------------------------
  // SOVRAPPOSIZIONE_PRESE_INCOMPLETE
  // da #define per ogni singolo impianto
  //--------------------------------------------------------------------
  abilita_sovrapposizione_prese_incomplete=false;
  #ifdef SOVRAPPOSIZIONE_PRESE_INCOMPLETE
     abilita_sovrapposizione_prese_incomplete=true;
  #endif;
  //--------------------------------------------------------------------
  // Verifica se la destinazione AGV  un punto chiamante
  // se si, compila "num_ch" con la coppia KY = macchina,stazione.
  //--------------------------------------------------------------------
  if(P.test_punto_presente_baia(all_mess, AGV[num_agv]->mission.pend, &num_baia)) return 1;
  if(num_baia<1 || num_baia>TOTBAIE) return 1;

  //---------------------------------------------------
  // Verifico attendibilità del BAIA
  //---------------------------------------------------
  if(P.read_baie( num_baia, &bb, all_mess)) return 1;

  //---------------------------------------------------
  // Compilazione dati struttura TGV cestone a bordo
  //
  // Controllando il TypPresa:
  // 1 = ForcaB(Bassa)
  // 2 = ForcaA(Alta)
  // 3 = ForcaAB(Alta e Bassa)
  //---------------------------------------------------
  TypPresa =  AGV[num_agv]->mission.TypPresa;

  //---------------------------------------------------
  // Assegnazione provenienza
  //---------------------------------------------------
  AGV[num_agv]->mission.Provenienza  = (short int)((10000*PROV_BAIE)+num_baia);

  //---------------------------------------------------
  // Lettura DM con stato fotocellule presenza prese
  // complete
  //---------------------------------------------------
  stato_presa_da_tgvB=0xffff;
  stato_presa_da_tgvA=0xffff;
  if(AbilitaControlloPresaSuTGV==true && break_Com[COM1]==false && !AGV[num_agv]->mission.EsclPLC && PosInPresa>1){
     memset(&dm[0], 0, sizeof(dm));
     err = OM.ld_canale(&dm[0], (WORD)(num_agv), "RD", 78, 2, all_mess);
     if(err) return 1;
     //-------------------------------------------------------------
     // A lettura avvenuta, salvo lo stato di presa letto dalle
     // fotocellule a bordo TGV
     //-------------------------------------------------------------
     stato_presa_da_tgvB = dm[0];
     stato_presa_da_tgvA = dm[1];

     //-------------------------------------------------------------
     // tacci DM presenza forche
     //-------------------------------------------------------------
     sprintf(all_mess, "D78:%d - D79:%d", dm[0], dm[1]);
     file_cronologico_generico("pres_forche.txt", all_mess, 30000L);

     //-------------------------------------------------------------
     // GESTIONE DOPPIA FORCA
     // Se non è un impianto a doppia forca reale, viene simulata
     // la presenza su tutta la forca alta
     //-------------------------------------------------------------
     if(GestioneDoppiaForcaTGV!=1) stato_presa_da_tgvA=0xffff;
  }

  //------------------------
  // Forca A
  //------------------------
  if( TypPresa== N_FORCA_A ){
     memset(&AGV[num_agv]->mission.BoxA, 0, sizeof(BOX));
     switch(bb.TipoCod[0]){
        case ST_BOX_LAV1: AGV[num_agv]->mission.PresCEST_A = PRES_LAV1;   break;
        case ST_BOX_LAV2: AGV[num_agv]->mission.PresCEST_A = PRES_LAV2;   break;
        case ST_BOX_LAV3: AGV[num_agv]->mission.PresCEST_A = PRES_LAV3;   break;
        case ST_BOX_LAV4: AGV[num_agv]->mission.PresCEST_A = PRES_LAV4;   break;
        case ST_BOX_LAV5: AGV[num_agv]->mission.PresCEST_A = PRES_LAV5;   break;
        case ST_BOX_LAV6: AGV[num_agv]->mission.PresCEST_A = PRES_LAV6;   break;
        case ST_BOX_LAV7: AGV[num_agv]->mission.PresCEST_A = PRES_LAV7;   break;
        case ST_BOX_LAV8: AGV[num_agv]->mission.PresCEST_A = PRES_LAV8;   break;
        case ST_BOX_VUOTI:AGV[num_agv]->mission.PresCEST_A = PRES_VUOTO;  break;
                 default: AGV[num_agv]->mission.PresCEST_A = LIBERA;      break;
     }
     AGV[num_agv]->mission.BoxA.StatoLav    = bb.TipoCod[0];
     AGV[num_agv]->mission.BoxA.TipoBox     = bb.TipoBox[0];
     AGV[num_agv]->mission.BoxA.Codice      = bb.Codice[0];
     AGV[num_agv]->mission.BoxA.Lotto       = bb.Lotto[0];
     AGV[num_agv]->mission.BoxA.Basso       = bb.Basso[0];
     AGV[num_agv]->mission.BoxA.Altezza     = bb.Altezza[0];
     AGV[num_agv]->mission.BoxA.CarWash     = bb.CarWash[0];
     AGV[num_agv]->mission.BoxA.Sovrap      = bb.Sovrap[0];
     AGV[num_agv]->mission.BoxA.Ruotato180  = bb.Ruotato180[0];
     AGV[num_agv]->mission.BoxA.Pos         = num_agv;
     AGV[num_agv]->mission.BoxA.ProvBox     = AGV[num_agv]->mission.Provenienza;
     AGV[num_agv]->mission.BoxA.GruppoProv  = bb.Gruppo;
     AGV[num_agv]->mission.BoxA.GruppoDest  = bb.GruppoDest;
     memcpy(&AGV[num_agv]->mission.BoxA.Presa[0][0], &bb.Presa[0][0][0], sizeof(AGV[num_agv]->mission.BoxA.Presa));
     //----------------------------------------------------------
     // §§§ Andrea 12/03/2108 §§§
     // Trasferisce anche dati produzione Prd[]
     //----------------------------------------------------------
     memcpy(&AGV[num_agv]->mission.BoxA.Prd[0], &bb.BoxA.Prd[0], sizeof(AGV[num_agv]->mission.BoxA.Prd));
     //----------------------------------------------------------
     // Data/Ora di carico
     //----------------------------------------------------------
     if(bb.BoxA.TimeC>0)  AGV[num_agv]->mission.BoxA.TimeC = bb.BoxA.TimeC;
     else AGV[num_agv]->mission.BoxA.TimeC = time(NULL);
     //----------------------------------------------------------
     // RICALCOLO LOCAZIONE
     //----------------------------------------------------------
     for(z=0; z<POSINPRESA; z++){
        for(w=0; w<PIANIINPRESA; w++){
           if(AGV[num_agv]->mission.BoxA.Presa[z][w]==false) continue;
           n=z+(w*POSINPRESA);
           if(n>MAXUDCBOX   ) break;
           AGV[num_agv]->mission.BoxA.Prd[n].loc_prov = AGV[num_agv]->mission.BoxA.Prd[n].location;
           AGV[num_agv]->mission.BoxA.Prd[n].location = B.StructToLocation(0, 0, 0, bb.Num, 0, 0, 0, N_FORCA_A, n);
        }
     }
     //---------------------------------------------------------
     // FORZATURA BLOCCO SOVRAPPOSIZIONE SU RULLIERA
     // I pallet bassi sono sovrapponibili solo in rulliera
     // quindi se viene bloccata la sovrapposizione da Editor
     // metto a OFF l'abilitazione alla sovrapposizione
     // dell'intero pallet, per evitare poi di sovrapporre
     // a magazzino  (esigenza di OI-MEZZACORONA)
     //---------------------------------------------------------
   //if(bb.BloccoSovrap==true && AGV[num_agv]->mission.BoxA.Basso==true){
   //   AGV[num_agv]->mission.BoxA.Sovrap=false;
   //}
     //---------------------------------------------------------
     // Azzeramento  dati magazzino
     //---------------------------------------------------------
     memset(&bb.BoxA, 0, sizeof(BOX));
     //---------------------------------------------------------
     // Azzeramento presenza e decremento postazioni occupate
     //---------------------------------------------------------
     bb.PresCEST_A = LIBERA;
     //---------------------------------------------------------
     // GESTIONE FORCA SINGOLA CON CARICO DOPPIO
     // Anche quando il TGV arriva in presa forca alta in
     // realtà viene caricato il carro basso sulla rulliera
     //---------------------------------------------------------
     if(GestioneDoppiaForcaTGV==2){
        memset(&bb.BoxB, 0, sizeof(BOX));
        bb.PresCEST_B = LIBERA;
     }
  }
  //------------------------
  // Forca B
  //------------------------
  if( TypPresa== N_FORCA_B ){
     memset(&AGV[num_agv]->mission.BoxB, 0, sizeof(BOX));
     switch(bb.TipoCod[0]){
        case ST_BOX_LAV1: AGV[num_agv]->mission.PresCEST_B = PRES_LAV1;   break;
        case ST_BOX_LAV2: AGV[num_agv]->mission.PresCEST_B = PRES_LAV2;   break;
        case ST_BOX_LAV3: AGV[num_agv]->mission.PresCEST_B = PRES_LAV3;   break;
        case ST_BOX_LAV4: AGV[num_agv]->mission.PresCEST_B = PRES_LAV4;   break;
        case ST_BOX_LAV5: AGV[num_agv]->mission.PresCEST_B = PRES_LAV5;   break;
        case ST_BOX_LAV6: AGV[num_agv]->mission.PresCEST_B = PRES_LAV6;   break;
        case ST_BOX_LAV7: AGV[num_agv]->mission.PresCEST_B = PRES_LAV7;   break;
        case ST_BOX_LAV8: AGV[num_agv]->mission.PresCEST_B = PRES_LAV8;   break;
        case ST_BOX_VUOTI:AGV[num_agv]->mission.PresCEST_B = PRES_VUOTO;  break;
                 default: AGV[num_agv]->mission.PresCEST_B = LIBERA;      break;
     }
     AGV[num_agv]->mission.BoxB.StatoLav    = bb.TipoCod[0];
     AGV[num_agv]->mission.BoxB.TipoBox     = bb.TipoBox[0];
     AGV[num_agv]->mission.BoxB.Codice      = bb.Codice[0];
     AGV[num_agv]->mission.BoxB.Lotto       = bb.Lotto[0];
     AGV[num_agv]->mission.BoxB.Altezza     = bb.Altezza[0];
     AGV[num_agv]->mission.BoxB.CarWash     = bb.CarWash[0];
     AGV[num_agv]->mission.BoxB.Basso       = bb.Basso[0];
     AGV[num_agv]->mission.BoxB.Sovrap      = bb.Sovrap[0];
     AGV[num_agv]->mission.BoxB.Ruotato180  = bb.Ruotato180[0];
     AGV[num_agv]->mission.BoxB.Pos         = num_agv;
     AGV[num_agv]->mission.BoxB.ProvBox     = AGV[num_agv]->mission.Provenienza;
     AGV[num_agv]->mission.BoxB.GruppoProv  = bb.Gruppo;
     AGV[num_agv]->mission.BoxB.GruppoDest  = bb.GruppoDest;
     memcpy(&AGV[num_agv]->mission.BoxB.Presa[0][0], &bb.Presa[0][0][0], sizeof(AGV[num_agv]->mission.BoxB.Presa));
     //----------------------------------------------------------
     // §§§ Andrea 12/03/2108 §§§
     // Trasferisce anche dati produzione Prd[]
     //----------------------------------------------------------
     memcpy(&AGV[num_agv]->mission.BoxB.Prd[0], &bb.BoxB.Prd[0], sizeof(AGV[num_agv]->mission.BoxB.Prd));
     //----------------------------------------------------------
     // Data/Ora di carico
     //----------------------------------------------------------
     if(bb.BoxB.TimeC>0)  AGV[num_agv]->mission.BoxB.TimeC = bb.BoxB.TimeC;
     else AGV[num_agv]->mission.BoxB.TimeC = time(NULL);
     //----------------------------------------------------------
     // RICALCOLO LOCAZIONE
     //----------------------------------------------------------
     for(z=0; z<POSINPRESA; z++){
        for(w=0; w<PIANIINPRESA; w++){
           if(AGV[num_agv]->mission.BoxB.Presa[z][w]==false) continue;
           n=z+(w*POSINPRESA);
           if(n>MAXUDCBOX   ) break;
           AGV[num_agv]->mission.BoxB.Prd[n].loc_prov = AGV[num_agv]->mission.BoxB.Prd[n].location;
           AGV[num_agv]->mission.BoxB.Prd[n].location = B.StructToLocation(0, 0, 0, bb.Num, 0, 0, 0, N_FORCA_B, n);
        }
     }
     //---------------------------------------------------------
     // FORZATURA BLOCCO SOVRAPPOSIZIONE SU RULLIERA
     // I pallet bassi sono sovrapponibili solo in rulliera
     // quindi se viene bloccata la sovrapposizione da Editor
     // metto a OFF l'abilitazione alla sovrapposizione
     // dell'intero pallet, per evitare poi di sovrapporre
     // a magazzino (esigenza di OI-MEZZACORONA)
     //---------------------------------------------------------
   //if(bb.BloccoSovrap==true && AGV[num_agv]->mission.BoxB.Basso==true){
   //   AGV[num_agv]->mission.BoxB.Sovrap=false;
   //}
     //---------------------------------------------------------
     // Azzeramento  dati magazzino
     //---------------------------------------------------------
     memset(&bb.BoxB, 0, sizeof(BOX));
     //---------------------------------------------------------
     // Azzeramento presenza e decremento postazioni occupate
     //---------------------------------------------------------
     bb.PresCEST_B = LIBERA;
  }
  //------------------------
  // Forca A+B
  //------------------------
  if( TypPresa== N_FORCA_AB ){
     // FORCA "A" (presa da baia ALTA baie)
     memcpy(&AGV[num_agv]->mission.BoxA, &bb.BoxA, sizeof(BOX));
     switch(bb.BoxA.StatoLav){
        case ST_BOX_LAV1: AGV[num_agv]->mission.PresCEST_A = PRES_LAV1;   break;
        case ST_BOX_LAV2: AGV[num_agv]->mission.PresCEST_A = PRES_LAV2;   break;
        case ST_BOX_LAV3: AGV[num_agv]->mission.PresCEST_A = PRES_LAV3;   break;
        case ST_BOX_LAV4: AGV[num_agv]->mission.PresCEST_A = PRES_LAV4;   break;
        case ST_BOX_LAV5: AGV[num_agv]->mission.PresCEST_A = PRES_LAV5;   break;
        case ST_BOX_LAV6: AGV[num_agv]->mission.PresCEST_A = PRES_LAV6;   break;
        case ST_BOX_LAV7: AGV[num_agv]->mission.PresCEST_A = PRES_LAV7;   break;
        case ST_BOX_LAV8: AGV[num_agv]->mission.PresCEST_A = PRES_LAV8;   break;
        case ST_BOX_VUOTI:AGV[num_agv]->mission.PresCEST_A = PRES_VUOTO;  break;
                 default: AGV[num_agv]->mission.PresCEST_A = LIBERA;      break;
     }
     AGV[num_agv]->mission.BoxA.Pos        = num_agv;
     AGV[num_agv]->mission.BoxA.ProvBox    = AGV[num_agv]->mission.Provenienza;
     AGV[num_agv]->mission.BoxA.GruppoProv = bb.Gruppo;
     AGV[num_agv]->mission.BoxA.GruppoDest = bb.GruppoDest;
     //----------------------------------------------------------
     // Data/Ora di carico
     //----------------------------------------------------------
     if(bb.BoxA.TimeC>0)  AGV[num_agv]->mission.BoxA.TimeC = bb.BoxA.TimeC;
     else AGV[num_agv]->mission.BoxA.TimeC = time(NULL);
     //----------------------------------------------------------
     // RICALCOLO LOCAZIONE
     //----------------------------------------------------------
     for(z=0; z<POSINPRESA; z++){
        for(w=0; w<PIANIINPRESA; w++){
           if(AGV[num_agv]->mission.BoxA.Presa[z][w]==false) continue;
           n=z+(w*POSINPRESA);
           if(n>MAXUDCBOX) break;
           AGV[num_agv]->mission.BoxA.Prd[n].loc_prov = AGV[num_agv]->mission.BoxA.Prd[n].location;
           AGV[num_agv]->mission.BoxA.Prd[n].location = B.StructToLocation(0, 0, 0, bb.Num, 0, 0, 0, N_FORCA_A, n);
        }
     }
     // FORCA "B" (presa da baia globale)
     memset(&AGV[num_agv]->mission.BoxB, 0, sizeof(BOX));
     switch(bb.TipoCod[0]){
        case ST_BOX_LAV1: AGV[num_agv]->mission.PresCEST_B = PRES_LAV1;   break;
        case ST_BOX_LAV2: AGV[num_agv]->mission.PresCEST_B = PRES_LAV2;   break;
        case ST_BOX_LAV3: AGV[num_agv]->mission.PresCEST_B = PRES_LAV3;   break;
        case ST_BOX_LAV4: AGV[num_agv]->mission.PresCEST_B = PRES_LAV4;   break;
        case ST_BOX_LAV5: AGV[num_agv]->mission.PresCEST_B = PRES_LAV5;   break;
        case ST_BOX_LAV6: AGV[num_agv]->mission.PresCEST_B = PRES_LAV6;   break;
        case ST_BOX_LAV7: AGV[num_agv]->mission.PresCEST_B = PRES_LAV7;   break;
        case ST_BOX_LAV8: AGV[num_agv]->mission.PresCEST_B = PRES_LAV8;   break;
        case ST_BOX_VUOTI:AGV[num_agv]->mission.PresCEST_B = PRES_VUOTO;  break;
                 default: AGV[num_agv]->mission.PresCEST_B = LIBERA;      break;
     }
     AGV[num_agv]->mission.BoxB.StatoLav    = bb.TipoCod[0];
     AGV[num_agv]->mission.BoxB.TipoBox     = bb.TipoBox[0];
     AGV[num_agv]->mission.BoxB.Codice      = bb.Codice[0];
     AGV[num_agv]->mission.BoxB.Lotto       = bb.Lotto[0];
     AGV[num_agv]->mission.BoxB.Altezza     = bb.Altezza[0];
     AGV[num_agv]->mission.BoxB.CarWash     = bb.CarWash[0];
     AGV[num_agv]->mission.BoxB.Basso       = bb.Basso[0];
     AGV[num_agv]->mission.BoxB.Sovrap      = bb.Sovrap[0];
     AGV[num_agv]->mission.BoxB.Ruotato180  = bb.Ruotato180[0];
     AGV[num_agv]->mission.BoxB.Pos         = num_agv;
     AGV[num_agv]->mission.BoxB.ProvBox     = AGV[num_agv]->mission.Provenienza;
     AGV[num_agv]->mission.BoxB.GruppoProv  = bb.Gruppo;
     AGV[num_agv]->mission.BoxB.GruppoDest  = bb.GruppoDest;
     memcpy(&AGV[num_agv]->mission.BoxB.Presa[0][0], &bb.Presa[0][0][0], sizeof(AGV[num_agv]->mission.BoxB.Presa));
     //----------------------------------------------------------
     // Data/Ora di carico
     //----------------------------------------------------------
     if(bb.BoxB.TimeC>0)  AGV[num_agv]->mission.BoxB.TimeC = bb.BoxB.TimeC;
     else AGV[num_agv]->mission.BoxB.TimeC = time(NULL);
     //----------------------------------------------------------
     // RICALCOLO LOCAZIONE
     //----------------------------------------------------------
     for(z=0; z<POSINPRESA; z++){
        for(w=0; w<PIANIINPRESA; w++){
           if(AGV[num_agv]->mission.BoxB.Presa[z][w]==false) continue;
           n=z+(w*POSINPRESA);
           if(n>MAXUDCBOX   ) break;
           AGV[num_agv]->mission.BoxB.Prd[n].loc_prov = AGV[num_agv]->mission.BoxB.Prd[n].location;
           AGV[num_agv]->mission.BoxB.Prd[n].location = B.StructToLocation(0, 0, 0, bb.Num, 0, 0, 0, N_FORCA_B, n);
        }
     }
     //---------------------------------------------------------
     // Azzeramento  dati magazzino
     //---------------------------------------------------------
     memset(&bb.BoxA, 0, sizeof(BOX));
     memset(&bb.BoxB, 0, sizeof(BOX));
     //---------------------------------------------------------
     // Azzeramento presenza e decremento postazioni occupate
     //---------------------------------------------------------
     bb.PresCEST_A = LIBERA;
     bb.PresCEST_B = LIBERA;
  }
  //---------------------------------------------------------
  // AGGIORNAMENTO PRESENZE PRESA IN BASE A DM STATO PRESA
  //---------------------------------------------------------
  k=0;
  for(i=0; i<PIANIINPRESA; i++){
     for(j=0; j<POSINPRESA; j++){
        // FORCA "B"
        if(TestBit((char *) &stato_presa_da_tgvB, k)==false){   // controlli per piano
           //------------------------------------------------
           // Reset presenza in base a DM di stato presa
           //------------------------------------------------
           AGV[num_agv]->mission.BoxB.Presa[j][i]=false;
        }
        // FORCA "A"
        if(TestBit((char *) &stato_presa_da_tgvA, k)==false){   // controlli per piano
           //------------------------------------------------
           // Reset presenza in base a DM di stato presa
           //------------------------------------------------
           AGV[num_agv]->mission.BoxA.Presa[j][i]=false;
        }
        k++;
     }
  }
  //-----------------------------------------------------------
  // SICUREZZA: Azzeramento dei piani successivi a piani
  //            con presenza nulla
  //-----------------------------------------------------------
  // FORCA "B"
  for(i=0; i<POSINPRESA; i++){
     for(j=1; j<PIANIINPRESA; j++){          // ciclo da 1° piano
        if(AGV[num_agv]->mission.BoxB.Presa[i][j]==false) continue;
        //------------------------------------------
        // Se il corrispondente al piano precedente
        // è presente --> continue!
        //------------------------------------------
        if(AGV[num_agv]->mission.BoxB.Presa[i][j-1]==true) continue;
        //------------------------------------------
        // metto a "0" le presenze ai piani alti se
        // manca il corrispondente al piano basso
        //------------------------------------------
        AGV[num_agv]->mission.BoxB.Presa[i][j]=false;
     }
  }
  // FORCA "A"
  for(i=0; i<POSINPRESA; i++){
     for(j=1; j<PIANIINPRESA; j++){          // ciclo da 1° piano
        if(AGV[num_agv]->mission.BoxA.Presa[i][j]==false) continue;
        //------------------------------------------
        // Se il corrispondente al piano precedente
        // è presente --> continue!
        //------------------------------------------
        if(AGV[num_agv]->mission.BoxA.Presa[i][j-1]==true) continue;
        //------------------------------------------
        // metto a "0" le presenze ai piani alti se
        // manca il corrispondente al piano basso
        //------------------------------------------
        AGV[num_agv]->mission.BoxA.Presa[i][j]=false;
     }
  }
  //---------------------------------------------------------
  // CONDIZIONI DI RESET ABILITAZIONE SOVRAPPOSIZIONE
  // 1 - Tutti i carichi con CAR-WASH.
  // 2 - Tutte le prese non complete (da bit della baia)
  // 3 - Tutti i piani non completi  (da bit TGV)
  //---------------------------------------------------------
  // (1)
  if(AGV[num_agv]->mission.BoxB.CarWash!=0) AGV[num_agv]->mission.BoxB.Sovrap=false;
  if(AGV[num_agv]->mission.BoxA.CarWash!=0) AGV[num_agv]->mission.BoxA.Sovrap=false;
  // gestione sovrapposizione solo PRESE COMPLETE
  if(abilita_sovrapposizione_prese_incomplete==false){
     // (2)
     for(i=0; i<Baie[bb.Num][2]; i++){
        // FROCA "B"
        if(AGV[num_agv]->mission.BoxB.Presa[i][0]==false) AGV[num_agv]->mission.BoxB.Sovrap=false;
        // FROCA "A"
        if(AGV[num_agv]->mission.BoxA.Presa[i][0]==false) AGV[num_agv]->mission.BoxA.Sovrap=false;
     }
     // (3)
     for(i=0; i<PIANIINPRESA; i++){
        tot_palA=0;
        tot_palB=0;
        for(j=0; j<POSINPRESA; j++){
           // FORCA "B"
           if(AGV[num_agv]->mission.BoxB.Presa[j][i]==true) tot_palB++;  // conto le posizioni in fila
           // FORCA "A"
           if(AGV[num_agv]->mission.BoxA.Presa[j][i]==true) tot_palA++;  // conto le posizioni in fila
        }
        //--------------------------------------------------
        // Se il piano ha almeno una presenza però non è
        // completo azzerlo il bit di "sovrapponibile"
        //--------------------------------------------------
        if(tot_palB!=0 && tot_palB!=POSINPRESA) AGV[num_agv]->mission.BoxB.Sovrap=false;
        if(tot_palA!=0 && tot_palA!=POSINPRESA) AGV[num_agv]->mission.BoxA.Sovrap=false;
     }
  }

  //-----------------------------------------------------------
  // Azzerame4nto della struttura baia
  //-----------------------------------------------------------
  bb.Chiamata       = false;
  bb.TipoBox[0]     = TIPOBOX_1;
  bb.TipoCod[0]     = LIBERA;
  bb.Codice[0]      = 0;
  bb.Lotto[0]       = 0;
  bb.Altezza[0]     = 0;
  bb.CarWash[0]     = 0;
  bb.Basso[0]       = false;
  bb.Sovrap[0]      = false;
  bb.GruppoDest     = 0;
  bb.StatoPres      = LIBERA;
  bb.Ruotato180[0]  = false;
  memset(&bb.Presa[0][0][0], false, sizeof(bb.Presa[0]));

  //-------------------------------------------------
  // RESET CHIAMATA SU TGV:
  //
  // singola forca e singolo carico:
  // (GestioneDoppiaForcaTGV=0) se N_FORCA_B.
  //
  // doppia forca e doppio carico:
  // (GestioneDoppiaForcaTGV=1) se N_FORCA_B.
  //                            se N_FORCA_AB.
  // singola forca e doppio carico:
  // (GestioneDoppiaForcaTGV=2) se N_FORCA_B.
  //                            se N_FORCA_AB.
  //-------------------------------------------------
  if(GestioneDoppiaForcaTGV==0 && TypPresa== N_FORCA_B                          ) AGV[num_agv]->mission.Chiamata = 0;
  if(GestioneDoppiaForcaTGV!=0 &&(TypPresa== N_FORCA_B || TypPresa== N_FORCA_AB)) AGV[num_agv]->mission.Chiamata = 0;
  //-------------------------------------------------
  // BLOCCO ALLA SOVRAPPOSIZIONE IN RULLIERA
  // Azzero la chiamata anche con il prel. Forca "A"
  // per garantire la ripartenza TGV
  //-------------------------------------------------
  if(bb.BloccoSovrap==true && TypPresa==N_FORCA_A) AGV[num_agv]->mission.Chiamata = 0;

  //-------------------------------------------------
  // salvataggio struttura TGV
  //-------------------------------------------------
  if(!err){
     err = AGV[num_agv]->save_mission(all_mess);
  }

  //-------------------------------------------
  // salvataggio struttura Magazzino
  //-------------------------------------------
  if(!err){
     memcpy( P.ba[bb.Num], &bb, sizeof(BAIE));
     err = P.save_record_baie(bb.Num, all_mess);
  }

  //----------------------------------------------------------------
  // GESTIONE UDC
  // da stuttura software ad array UDC per aggiornamento SQL
  //
  // !!! IMPORTANTE !!!
  // dopo aver aggiornato la struttura globale del software
  //----------------------------------------------------------------
  if(GestioneUdc>0){
     // -- SET LOCAZIONI SU UDC A BORDO TGV
     if(!err){
        err = B.StructToUdcTmp(CMD_WR_SQL_UDC_SET_LOC, num_agv, 0, 0, 0, UDC_ALL_STZ, UDC_ALL_POS, UDC_ALL_LEV, TypPresa, all_mess);
        if(GestioneUdc==2 && err!=0) err=0;
     }
  }
  //-----------------------------------------------------------
  // Log Traccia Movimenti
  //-----------------------------------------------------------
  sprintf(Coordinate,"Bay %02d", num_baia);
  LogMovimenti(Operazione, Coordinate, num_agv, AGV[num_agv]->mission.BoxA, AGV[num_agv]->mission.BoxB, TypPresa, 0);

  return err;
}

// ------------------------------
//   CaricoDaMagazzino()
// ------------------------------
// Questa funzione setta il cestone caricato su AGV
//
int process::CaricoDaMagazzino(short int num_agv, char *all_mess)
/********************************************************/
{
  int   i, j, err=0;
  double a, b;
  double RapImp;
  double Xly, Yly;
  double CatetoX, CatetoY, CalcolaRad;
  int    x1, y1, x2, y2;
  bool   lato_ingresso, spostamento;
  short int pos_trovata, n, z, w;
  short int num_mag, num_pos, TipoBox;
  short int lato_box, traccia_mag;
  short int TypPresa, TypPresaPLC;
  short int Posizione, PosizionePLC;
  short int Piano, PianoPLC;
  short int ID, PosizioneTgv;
  short int bai_ch, mac_ch, mag_ch, pos_ch;
  WORD  QuotaH_A, QuotaH_B, QuotaP;
  struct MAG mag;
  WORD dm[3];
  char Coordinate[101]="";
  char Operazione[101]="Storage Pickup";

  RapImp=1;    // gestione (cm)
  num_mag=0;
  num_pos=0;
  Xly=Yly=0;
  memset(&mag,    0, sizeof(MAG));
  memset(&dm[0],  0, sizeof(dm));
  //--------------------------------------------------------------------
  // Verifica se la destinazione AGV  un punto chiamante
  // se si, compila "num_ch" con la coppia KY = macchina,stazione.
  //--------------------------------------------------------------------
  if(M.test_punto_presente_mag(all_mess, AGV[num_agv]->mission.pend, &num_mag, &num_pos)) return 1;
  if(num_mag==0) return 1;
  //---------------------------------------------------
  // Verifico attendibilità del MAG
  //---------------------------------------------------
  if(M.read_mag( num_mag, &mag, all_mess)) return 1;
  //---------------------------------------------------
  // Compilazione dati struttura TGV cestone a bordo
  //
  // Controllando il TypPresa:
  // 1 = ForcaB(Bassa)
  // 2 = ForcaA(Alta)
  // 3 = ForcaAB(Alta e Bassa)
  //---------------------------------------------------
  TypPresa =  AGV[num_agv]->mission.TypPresa;
  //---------------------------------------------------
  // In carico, il TIPOBOX è quello impostato sul MAG
  //---------------------------------------------------
  TipoBox  =  mag.TipoBox;
  //---------------------------------------------------
  // Postazione non in disponibile --> skip!!
  // N.B. Le forche B possono prel/dep solo in basso
  //---------------------------------------------------
  lato_ingresso=false;
  if(mag.Fifo==false || (mag.Fifo==true && AGV[num_agv]->mission.NodIngMag==mag.PuntoIng)) lato_ingresso=true;
  if(M.VerificaTipoPrelievo(lato_ingresso, num_mag, TypPresa, &Posizione, &Piano, &PosizioneTgv, all_mess)) return 1;

  //--------------------------------------------------------------------
  // GESTIONE POSIZIONE REALE
  // Recupero dal PLC le reali quote di deposito sia in profondità
  // che in altezza
  //
  // DM51 = Quato deposito in ALTEZZA FORCA "B"
  // DM52 = Quato deposito in ALTEZZA FORCA "A"
  //--------------------------------------------------------------------
  QuotaH_B=QuotaH_A=QuotaP=0;
  // QUOTA IN ALTEZZA
  if(break_Com[COM1]==false && !AGV[num_agv]->mission.EsclPLC){
     err = OM.ld_canale(&dm[0], (WORD)(num_agv), "RD", 51, 2, all_mess);
     if(err) return 1;
     QuotaH_B = dm[0];                                   // altezza "B"
     QuotaH_A = dm[0];                                   // altezza "A"
     //-------------------------------------------------------------
     // GESTIONE FORCHE DOPPIE
     // solo per quando sono realmente esistenti 2 forche sul TGV
     // ha valore il DM52
     //-------------------------------------------------------------
     if(GestioneDoppiaForcaTGV==1) QuotaH_A = dm[1];
  }


  //====================================================================
  // Per TEST (INSERIMENTO QUOTE IPOTETICHE PASSATE DA PLC TGV)
  //
  //
//int pos;
//AnsiString InputStringa;

//InputStringa = InputBox("Introduzione manuale coordinate TGV", "formato richiesto (xxxx,yyyy):", "");
//if(InputStringa.IsEmpty()==false){
//   pos = InputStringa.Pos(",");
//   if(pos!=0){
//      Xly = StrToInt(InputStringa.SubString(0,                    pos-1 ));
//      Yly = StrToInt(InputStringa.SubString(pos+1, InputStringa.Length()));
//   }
//}
  //
  //====================================================================

  // QUOTA IN PROFONDITA' (DM93 per magneti o coordinate laser)
  if(break_Com[COM1]==false && !AGV[num_agv]->mission.EsclPLC){
     if(AGV[num_agv]->mission.GestioneLaser==true){
        AGV[num_agv]->convertiXY_laser_to_layout(AGV[num_agv]->stato.x_laser/10, AGV[num_agv]->stato.y_laser/10, &Xly, &Yly);
        x1 = (int) Xly;                               // coordinata X posizione TGV nell'impianto
        y1 = (int) Yly;                               // coordinata Y posizione TGV nell'impianto
        x2 = Punti[ AGV[num_agv]->stato.pos ][0];     // coordinata X posizione nodo occupato
        y2 = Punti[ AGV[num_agv]->stato.pos ][1];     // coordinata Y posizione nodo occupato
        //----------------------------------------
        // Calcolo distanza dal nodo
        //----------------------------------------
        CatetoX = (double) ((abs)(x1-x2));
        CatetoY = (double) ((abs)(y1-y2));
        QuotaP  = 0;
        RapImp  = 1;  // divisore "1" = cm
        if(CatetoX<30000 && CatetoY<30000){
           CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
           QuotaP = (int) sqrt( CalcolaRad );            // profondità (CM)
        }
     }
     // magneti
     else{
        QuotaP = AGV[num_agv]->stato.offset_da_nodo;     // profondità (IMPULSI)
        RapImp = AGV[num_agv]->mission.RapEncoder;
     }
     //--------------------------------------------------------------------
     // Conversione QUOTE in POS/PIANI
     //--------------------------------------------------------------------
     if(M.ConvertiQuotePLC(!lato_ingresso, num_mag, RapImp, TipoBox, QuotaP, QuotaH_A, QuotaH_B, TypPresa, Posizione, Piano, &TypPresaPLC, &PosizionePLC, &PianoPLC, all_mess)) return 1;
     //--------------------------------------------------------------------
     // Aggiorno dati rielaborati
     //--------------------------------------------------------------------
     TypPresa  = TypPresaPLC;
     Posizione = PosizionePLC;
     Piano     = PianoPLC;
  }


  //--------------------------------------------------------------------
  // Recupero provenienza
  //--------------------------------------------------------------------
  AGV[num_agv]->mission.Provenienza = (short int)((10000*PROV_MAGAZZINO)+num_mag);

  //------------------------
  // Forca A
  //------------------------
  if( TypPresa== N_FORCA_A ){
     memcpy(&AGV[num_agv]->mission.BoxA, &mag.Pos[Posizione].Box[Piano ], sizeof(BOX));
     //---------------------------------------------------------
     // Assegnazione TIPO DI CARICO a navetta
     //---------------------------------------------------------
     switch(AGV[num_agv]->mission.BoxA.StatoLav){
        case  ST_BOX_LAV1: AGV[num_agv]->mission.PresCEST_A = PRES_LAV1;  break;
        case  ST_BOX_LAV2: AGV[num_agv]->mission.PresCEST_A = PRES_LAV2;  break;
        case  ST_BOX_LAV3: AGV[num_agv]->mission.PresCEST_A = PRES_LAV3;  break;
        case  ST_BOX_LAV4: AGV[num_agv]->mission.PresCEST_A = PRES_LAV4;  break;
        case  ST_BOX_LAV5: AGV[num_agv]->mission.PresCEST_A = PRES_LAV5;  break;
        case  ST_BOX_LAV6: AGV[num_agv]->mission.PresCEST_A = PRES_LAV6;  break;
        case  ST_BOX_LAV7: AGV[num_agv]->mission.PresCEST_A = PRES_LAV7;  break;
        case  ST_BOX_LAV8: AGV[num_agv]->mission.PresCEST_A = PRES_LAV8;  break;
        case  ST_BOX_VUOTI:AGV[num_agv]->mission.PresCEST_A = PRES_VUOTO; break;
        default:           AGV[num_agv]->mission.PresCEST_A = LIBERA;     break;
     }
     //---------------------------------------------------------
     // Posizione e provenienza
     //---------------------------------------------------------
     AGV[num_agv]->mission.BoxA.Pos = num_agv;
     //----------------------------------------------------------
     // In caso di MAGAZZINI TEMPORANEI non viene modificata
     // la provenienza per mantenere quella originale del box
     //----------------------------------------------------------
     if(mag.Tipo!=MG_TEMPORANEO){
        AGV[num_agv]->mission.BoxA.ProvBox = AGV[num_agv]->mission.Provenienza;
     }
     else{
        AGV[num_agv]->mission.BoxA.ProvBox = mag.Pos[Posizione].Box[Piano].ProvBox;
     }
     //----------------------------------------------------------
     // RICALCOLO ROTAZIONE
     // Se prelievo è avvenuto dal lato di uscita di un
     // magazzino Fifo il pallet sarà RUOTATO
     //----------------------------------------------------------
     if(mag.Fifo==true && lato_ingresso==false){
        AGV[num_agv]->mission.BoxA.Ruotato180=!AGV[num_agv]->mission.BoxA.Ruotato180;
     }
     //----------------------------------------------------------
     // RICALCOLO LOCAZIONE
     //----------------------------------------------------------
     for(z=0; z<POSINPRESA; z++){
        for(w=0; w<PIANIINPRESA; w++){
           if(AGV[num_agv]->mission.BoxA.Presa[z][w]==false) continue;
           n=z+(w*POSINPRESA);
           if(n>MAXUDCBOX   ) break;
           AGV[num_agv]->mission.BoxA.Prd[n].loc_prov = AGV[num_agv]->mission.BoxA.Prd[n].location;
           AGV[num_agv]->mission.BoxA.Prd[n].location = B.StructToLocation(0, 0, mag.Num, 0, 0, Posizione, Piano, N_FORCA_A, n);
        }
     }
     //---------------------------------------------------------
     // Azzeramento  dati magazzino
     //---------------------------------------------------------
     memset( &mag.Pos[Posizione].Box[Piano  ], 0, sizeof(BOX));
     //---------------------------------------------------------
     // Azzeramento presenza e decremento postazioni occupate
     //---------------------------------------------------------
     mag.Pos[Posizione].Presenza[Piano] = false;
     mag.Pos[Posizione].QuotaH[Piano ]  = 0;
     mag.Pos[Posizione].QuotaP[Piano ]  = 0;
     mag.NumCestoni--;
     //---------------------------------------------------------
     // SICUREZZA
     // Azzeramento di tutti i piani superiori a quello da cui
     // ho depositato
     //---------------------------------------------------------
     for(j=Piano+1; j<MAXPIANIMAG; j++){
        mag.Pos[Posizione].Presenza[j] = false;
        mag.Pos[Posizione].QuotaH[j]   = 0;
        mag.Pos[Posizione].QuotaP[j]   = 0;
        memset(&mag.Pos[Posizione].Box[j], 0, sizeof(BOX));
     }
  }
  //------------------------
  // Forca B
  //------------------------
  if( TypPresa== N_FORCA_B ){
     memcpy(&AGV[num_agv]->mission.BoxB, &mag.Pos[Posizione].Box[Piano ], sizeof(BOX));
     //---------------------------------------------------------
     // Assegnazione TIPO DI CARICO a navetta
     //---------------------------------------------------------
     switch(AGV[num_agv]->mission.BoxB.StatoLav){
        case  ST_BOX_LAV1: AGV[num_agv]->mission.PresCEST_B = PRES_LAV1;  break;
        case  ST_BOX_LAV2: AGV[num_agv]->mission.PresCEST_B = PRES_LAV2;  break;
        case  ST_BOX_LAV3: AGV[num_agv]->mission.PresCEST_B = PRES_LAV3;  break;
        case  ST_BOX_LAV4: AGV[num_agv]->mission.PresCEST_B = PRES_LAV4;  break;
        case  ST_BOX_LAV5: AGV[num_agv]->mission.PresCEST_B = PRES_LAV5;  break;
        case  ST_BOX_LAV6: AGV[num_agv]->mission.PresCEST_B = PRES_LAV6;  break;
        case  ST_BOX_LAV7: AGV[num_agv]->mission.PresCEST_B = PRES_LAV7;  break;
        case  ST_BOX_LAV8: AGV[num_agv]->mission.PresCEST_B = PRES_LAV8;  break;
        case  ST_BOX_VUOTI:AGV[num_agv]->mission.PresCEST_B = PRES_VUOTO; break;
        default:           AGV[num_agv]->mission.PresCEST_B = LIBERA;     break;
     }
     //---------------------------------------------------------
     // Posizione e provenienza
     //---------------------------------------------------------
     AGV[num_agv]->mission.BoxB.Pos = num_agv;
     //----------------------------------------------------------
     // In caso di MAGAZZINI TEMPORANEI non viene modificata
     // la provenienza per mantenere quella originale del box
     //----------------------------------------------------------
     if(mag.Tipo!=MG_TEMPORANEO){
        AGV[num_agv]->mission.BoxB.ProvBox = AGV[num_agv]->mission.Provenienza;
     }
     else{
        AGV[num_agv]->mission.BoxB.ProvBox = mag.Pos[Posizione].Box[Piano].ProvBox;
     }
     //----------------------------------------------------------
     // RICALCOLO ROTAZIONE
     // Se prelievo è avvenuto dal lato di uscita di un
     // magazzino Fifo il pallet sarà RUOTATO
     //----------------------------------------------------------
     if(mag.Fifo==true && lato_ingresso==false){
        AGV[num_agv]->mission.BoxB.Ruotato180=!AGV[num_agv]->mission.BoxB.Ruotato180;
     }
     //----------------------------------------------------------
     // RICALCOLO LOCAZIONE
     //----------------------------------------------------------
     for(z=0; z<POSINPRESA; z++){
        for(w=0; w<PIANIINPRESA; w++){
           if(AGV[num_agv]->mission.BoxB.Presa[z][w]==false) continue;
           n=z+(w*POSINPRESA);
           if(n>MAXUDCBOX) break;
           AGV[num_agv]->mission.BoxB.Prd[n].loc_prov = AGV[num_agv]->mission.BoxB.Prd[n].location;
           AGV[num_agv]->mission.BoxB.Prd[n].location = B.StructToLocation(0, 0, mag.Num, 0, 0, Posizione, Piano, N_FORCA_B, n);
        }
     }
     //---------------------------------------------------------
     // Azzeramento  dati magazzino
     //---------------------------------------------------------
     memset(&mag.Pos[Posizione].Box[Piano  ], 0, sizeof(BOX));
     //---------------------------------------------------------
     // Azzeramento presenza e decremento postazioni occupate
     //---------------------------------------------------------
     mag.Pos[Posizione].Presenza[Piano] = false;
     mag.Pos[Posizione].QuotaH[Piano ]  = 0;
     mag.Pos[Posizione].QuotaP[Piano ]  = 0;
     mag.NumCestoni--;
     //---------------------------------------------------------
     // SICUREZZA
     // Azzeramento di tutti i piani superiori a quello da cui
     // ho depositato
     //---------------------------------------------------------
     for(j=Piano+1; j<MAXPIANIMAG; j++){
        mag.Pos[Posizione].Presenza[j] = false;
        mag.Pos[Posizione].QuotaH[j]   = 0;
        mag.Pos[Posizione].QuotaP[j]   = 0;
        memset(&mag.Pos[Posizione].Box[j], 0, sizeof(BOX));
     }
  }
  //------------------------
  // Forca A+B
  //------------------------
  if( TypPresa== N_FORCA_AB ){
     memcpy(&AGV[num_agv]->mission.BoxA, &mag.Pos[Posizione].Box[Piano+1], sizeof(BOX));
     memcpy(&AGV[num_agv]->mission.BoxB, &mag.Pos[Posizione].Box[Piano  ], sizeof(BOX));
     //---------------------------------------------------------
     // Assegnazione TIPO DI CARICO a navetta
     //---------------------------------------------------------
     switch(AGV[num_agv]->mission.BoxA.StatoLav){
        case  ST_BOX_LAV1: AGV[num_agv]->mission.PresCEST_A = PRES_LAV1;  break;
        case  ST_BOX_LAV2: AGV[num_agv]->mission.PresCEST_A = PRES_LAV2;  break;
        case  ST_BOX_LAV3: AGV[num_agv]->mission.PresCEST_A = PRES_LAV3;  break;
        case  ST_BOX_LAV4: AGV[num_agv]->mission.PresCEST_A = PRES_LAV4;  break;
        case  ST_BOX_LAV5: AGV[num_agv]->mission.PresCEST_A = PRES_LAV5;  break;
        case  ST_BOX_LAV6: AGV[num_agv]->mission.PresCEST_A = PRES_LAV6;  break;
        case  ST_BOX_LAV7: AGV[num_agv]->mission.PresCEST_A = PRES_LAV7;  break;
        case  ST_BOX_LAV8: AGV[num_agv]->mission.PresCEST_A = PRES_LAV8;  break;
        case  ST_BOX_VUOTI:AGV[num_agv]->mission.PresCEST_A = PRES_VUOTO; break;
        default:           AGV[num_agv]->mission.PresCEST_A = LIBERA;     break;
     }
     switch(AGV[num_agv]->mission.BoxB.StatoLav){
        case  ST_BOX_LAV1: AGV[num_agv]->mission.PresCEST_B = PRES_LAV1;  break;
        case  ST_BOX_LAV2: AGV[num_agv]->mission.PresCEST_B = PRES_LAV2;  break;
        case  ST_BOX_LAV3: AGV[num_agv]->mission.PresCEST_B = PRES_LAV3;  break;
        case  ST_BOX_LAV4: AGV[num_agv]->mission.PresCEST_B = PRES_LAV4;  break;
        case  ST_BOX_LAV5: AGV[num_agv]->mission.PresCEST_B = PRES_LAV5;  break;
        case  ST_BOX_LAV6: AGV[num_agv]->mission.PresCEST_B = PRES_LAV6;  break;
        case  ST_BOX_LAV7: AGV[num_agv]->mission.PresCEST_B = PRES_LAV7;  break;
        case  ST_BOX_LAV8: AGV[num_agv]->mission.PresCEST_B = PRES_LAV8;  break;
        case  ST_BOX_VUOTI:AGV[num_agv]->mission.PresCEST_B = PRES_VUOTO; break;
        default:           AGV[num_agv]->mission.PresCEST_B = LIBERA;     break;
     }
     //---------------------------------------------------------
     // Posizione e provenienza
     //---------------------------------------------------------
     AGV[num_agv]->mission.BoxA.Pos = num_agv;
     AGV[num_agv]->mission.BoxB.Pos = num_agv;
     //----------------------------------------------------------
     // In caso di MAGAZZINI TEMPORANEI non viene modificata
     // la provenienza per mantenere quella originale del box
     //----------------------------------------------------------
     if(mag.Tipo!=MG_TEMPORANEO){
        AGV[num_agv]->mission.BoxA.ProvBox = AGV[num_agv]->mission.Provenienza;
        AGV[num_agv]->mission.BoxB.ProvBox = AGV[num_agv]->mission.Provenienza;
     }
     else{
        AGV[num_agv]->mission.BoxA.ProvBox = mag.Pos[Posizione].Box[Piano+1].ProvBox;
        AGV[num_agv]->mission.BoxB.ProvBox = mag.Pos[Posizione].Box[Piano  ].ProvBox;
     }
     //----------------------------------------------------------
     // RICALCOLO ROTAZIONE
     // Se prelievo è avvenuto dal lato di uscita di un
     // magazzino Fifo il pallet sarà RUOTATO
     //----------------------------------------------------------
     if(mag.Fifo==true && lato_ingresso==false){
        AGV[num_agv]->mission.BoxA.Ruotato180=!AGV[num_agv]->mission.BoxA.Ruotato180;
        AGV[num_agv]->mission.BoxB.Ruotato180=!AGV[num_agv]->mission.BoxB.Ruotato180;
     }
     //----------------------------------------------------------
     // RICALCOLO LOCAZIONE  "FORCA A"
     //----------------------------------------------------------
     for(z=0; z<POSINPRESA; z++){
        for(w=0; w<PIANIINPRESA; w++){
           if(AGV[num_agv]->mission.BoxA.Presa[z][w]==false) continue;
           n=z+(w*POSINPRESA);
           if(n>MAXUDCBOX) break;
           AGV[num_agv]->mission.BoxA.Prd[n].loc_prov = AGV[num_agv]->mission.BoxA.Prd[n].location;
           AGV[num_agv]->mission.BoxA.Prd[n].location = B.StructToLocation(0, 0, mag.Num, 0, 0, Posizione, Piano+1, N_FORCA_A, n);
        }
     }
     //----------------------------------------------------------
     // RICALCOLO LOCAZIONE  "FORCA B"
     //----------------------------------------------------------
     for(z=0; z<POSINPRESA; z++){
        for(w=0; w<PIANIINPRESA; w++){
           if(AGV[num_agv]->mission.BoxB.Presa[z][w]==false) continue;
           n=z+(w*POSINPRESA);
           if(n>MAXUDCBOX) break;
           AGV[num_agv]->mission.BoxB.Prd[n].loc_prov = AGV[num_agv]->mission.BoxB.Prd[n].location;
           AGV[num_agv]->mission.BoxB.Prd[n].location = B.StructToLocation(0, 0, mag.Num, 0, 0, Posizione, Piano, N_FORCA_B, n);
        }
     }
     //---------------------------------------------------------
     // Azzeramento  dati magazzino
     //---------------------------------------------------------
     memset(&mag.Pos[Posizione].Box[Piano+1], 0, sizeof(BOX));
     memset(&mag.Pos[Posizione].Box[Piano  ], 0, sizeof(BOX));
     //---------------------------------------------------------
     // Azzeramento presenza e decremento postazioni occupate
     //---------------------------------------------------------
     mag.Pos[Posizione].Presenza[Piano+1] = false;
     mag.Pos[Posizione].Presenza[Piano  ] = false;
     mag.Pos[Posizione].QuotaH[Piano+1]   = 0;
     mag.Pos[Posizione].QuotaH[Piano  ]   = 0;
     mag.Pos[Posizione].QuotaP[Piano+1]   = 0;
     mag.Pos[Posizione].QuotaP[Piano  ]   = 0;
     mag.NumCestoni--;
     mag.NumCestoni--;
     //---------------------------------------------------------
     // SICUREZZA
     // Azzeramento di tutti i piani superiori a quello da cui
     // ho depositato
     //---------------------------------------------------------
     for(j=Piano+2; j<MAXPIANIMAG; j++){
        mag.Pos[Posizione].Presenza[j] = false;
        mag.Pos[Posizione].QuotaH[j]   = 0;
        mag.Pos[Posizione].QuotaP[j]   = 0;
        memset(&mag.Pos[Posizione].Box[j], 0, sizeof(BOX));
     }
  }


  //-------------------------------------------
  // Verifico se TGV in chiamata di
  // spostamento
  //-------------------------------------------
  spostamento=false;
  if(AbilitaSvuotamentoManualeMag>1 && AGV[num_agv]->mission.Chiamata!=0){
     bai_ch=mac_ch=mag_ch=pos_ch=0;
     P.TestChiamataNum(AGV[num_agv]->mission.Chiamata, &mag_ch, &bai_ch, &mac_ch, &pos_ch);
     if(mag_ch>0 && mag_ch<=TOTMAG && mag_ch!=mag.Num) spostamento=true;
  }

  //-------------------------------------------
  // GESTIONE COMPLETO
  // tutte le posizioni sono impegnate e si
  // resetta appena una posizione è libera.
  //
  // Nota: Nei magazzini LIFO di pieni viene
  //       mantenuto per lo svuotamento e se
  //       non è settato lo setto
  //
  // AL-25/02/2020 se si tratta di uno
  // SPOSTAMENTO, non viene marcato il
  // completo perchè prelevato il pallet
  // interessato può essere riutilizzato
  //-------------------------------------------
  if(mag.Npt!=0 && mag.NumCestoni>=(mag.Npt*(mag.Sovrapposizione+1))) mag.Completo=true;
  else{
     if(spostamento==true) mag.Completo=false;
     else{
        if(mag.Stato!=MG_ST_VUOTI                                       ) mag.Completo=true;  // Se comincio a svuotare un mag di pieni setto il completo.
        if(mag.Fifo==true || (mag.Fifo==false && mag.Stato==MG_ST_VUOTI)) mag.Completo=false;
     }
  }

  //-------------------------------------------
  // Magazzino svuotato
  //-------------------------------------------
  if(mag.NumCestoni==0){
     mag.Stato    = MG_ST_FREE;
     mag.Completo = false;
     mag.Time     = 0;               // Azzeramento data/ora primo inserimento
     //--------------------------------------------------------------------
     // MAGAZZINI TEMPORANEI
     // A svuotamento completato e gestione spotamento con ritorno attiva,
     // i magazzini devono ritornare per tutti i tipi di prodotto
     //--------------------------------------------------------------------
     if(AbilitaSvuotamentoManualeMag==3 && mag.Tipo==MG_TEMPORANEO){
        mag.Tipo=MG_PROD_TUTTI;
     }
  }

  //-------------------------------------------
  // SICUREZZA
  // Aggiorno lo stato del magazzino in base
  // al prossimo tipo di box in prelievo
  //-------------------------------------------
  if(mag.NumCestoni>0){
     pos_trovata=99;
     if( mag.Fifo==false ){
        for(i=1; i<=MAXTERRAMAG; i++){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(mag.Pos[i].Presenza[j]==false) continue;
              pos_trovata=j;
              break;
           }
           if(pos_trovata!=99) break;
        }
     }
     else{
        for(i=MAXTERRAMAG; i>0; i--){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(mag.Pos[i].Presenza[j]==false) continue;
              pos_trovata=j;
              break;
           }
           if(pos_trovata!=99) break;
        }
     }
     if(pos_trovata!=99 && mag.Pos[i].Presenza[pos_trovata]==true){
        //------------------------------------------
        // Setto il tipo di magazzino in base
        // a l'unico box depositato
        //------------------------------------------
        switch(mag.Pos[i].Box[pos_trovata].StatoLav){
           case  ST_BOX_LAV1: mag.Stato = MG_ST_PIENI;  break;
           case  ST_BOX_LAV2: mag.Stato = MG_ST_PIENI;  break;
           case  ST_BOX_LAV3: mag.Stato = MG_ST_PIENI;  break;
           case  ST_BOX_LAV4: mag.Stato = MG_ST_PIENI;  break;
           case  ST_BOX_LAV5: mag.Stato = MG_ST_PIENI;  break;
           case  ST_BOX_LAV6: mag.Stato = MG_ST_PIENI;  break;
           case  ST_BOX_LAV7: mag.Stato = MG_ST_PIENI;  break;
           case  ST_BOX_LAV8: mag.Stato = MG_ST_PIENI;  break;
           case  ST_BOX_VUOTI:mag.Stato = MG_ST_VUOTI;  break;
        }
     }
     //----------------------------------------------------------------
     // Controllo che il Tipo di magazzino ammetta i box in esso
     // inseriti
     //----------------------------------------------------------------
     if((mag.Stato==MG_ST_VUOTI && mag.Tipo!=MG_VUOTI) || (mag.Stato==MG_ST_PIENI && mag.Tipo==MG_VUOTI)){
        strcpy(all_mess, MESS[324]);
        return 1;
     }
  }

  //-------------------------------------------
  // salvataggio struttura TGV
  //-------------------------------------------
  if(!err){
     err = AGV[num_agv]->save_mission(all_mess);
  }

  //-------------------------------------------
  // salvataggio struttura Magazzino
  //-------------------------------------------
  if(!err){
     memcpy( M.m[mag.Num], &mag, sizeof(MAG));
     err = M.save_record_mag(mag.Num, all_mess);
  }

  //----------------------------------------------------------------
  // GESTIONE UDC
  // da stuttura software ad array UDC per aggiornamento SQL
  //
  // !!! IMPORTANTE !!!
  // dopo aver aggiornato la struttura globale del software
  //----------------------------------------------------------------
  if(GestioneUdc>0){
     // -- SET LOCAZIONI SU UDC A BORDO TGV
     if(!err){
        err = B.StructToUdcTmp(CMD_WR_SQL_UDC_SET_LOC, num_agv, 0, 0, 0, UDC_ALL_STZ, UDC_ALL_POS, UDC_ALL_LEV, TypPresa, all_mess);
        if(GestioneUdc==2 && err!=0) err=0;
     }
  }
  //-----------------------------------------------------------
  // Log Traccia Movimenti
  //-----------------------------------------------------------
  if(TypPresa==N_FORCA_AB) sprintf(Coordinate,"Storage %02d Pos.%02d Layer %02d,%02d", num_mag, Posizione, Piano, Piano+1);
  else                     sprintf(Coordinate,"Storage %02d Pos.%02d Layer %02d",      num_mag, Posizione, Piano);
  LogMovimenti(Operazione, Coordinate, num_agv, AGV[num_agv]->mission.BoxA, AGV[num_agv]->mission.BoxB, TypPresa, 0);

  //-----------------------------------------------------------
  return err;
}
// ------------------------------
//   ScaricoSuMacchina()
// ------------------------------
// Questa funzione setta il cestone scaricato da AGV
//
int process::ScaricoSuMacchina(short int num_agv, char *all_mess)
/********************************************************/
{
/*
  int       i, j, err=0;
  bool      lato_uscita;
  bool      macc_completa;
  bool      nessuna_chiamata_attiva;
  short int ID, num_mac, num_stz;
  short int TypPresa, Posizione, PosizioneTgv;
  struct    CHIAMATE mac;

  num_mac=num_stz=0;
  memset(&mac, 0, sizeof(CHIAMATE));
  //--------------------------------------------------------------------
  // Verifica se la destinazione AGV  un punto chiamante
  // se si, compila "num_ch" con la coppia KY = macchina,stazione.
  //--------------------------------------------------------------------
  if(test_punto_presente_mac(all_mess, AGV[num_agv]->mission.pend, 0, num_agv, &num_mac, &num_stz)) return 1;
  if(num_mac==0) return 1;
  //--------------------------------------------------------------------
  // Verifico attendibilità della MAC
  //--------------------------------------------------------------------
  if(read_chiamate( num_mac, &mac, all_mess)) return 1;

  //--------------------------------------------------------------------
  // Recupero la posizione di deposito dal nodo di destinazione
  //--------------------------------------------------------------------
  lato_uscita=false;
  if(mac.Staz[num_stz].PuntoOut!=mac.Staz[num_stz].PuntoIng){
     if(AGV[num_agv]->mission.pend==mac.Staz[num_stz].PuntoOut) lato_uscita=true;
  }

  //--------------------------------------------------------------------
  // Recupero posizione di deposito
  //--------------------------------------------------------------------
  if(VerificaTipoDeposito(lato_uscita, num_mac, num_stz, &Posizione, &PosizioneTgv, all_mess)) return 1;

  //--------------------------------------------------------------------
  // Azzera dati struttura TGV cestone a bordo
  //
  // Controllando il TypPresa:
  // 1 = ForcaB(Bassa)
  // 2 = ForcaA(Alta)
  // 3 = ForcaAB(Alta e Bassa)
  //--------------------------------------------------------------------
  TypPresa =  AGV[num_agv]->mission.TypPresa;

  //--------------------------------------------------------------------
  // Forca A
  //--------------------------------------------------------------------
  if( TypPresa== N_FORCA_A || TypPresa== N_FORCA_AB ){
     //-----------------------------------------------------------
     // Scarico dati su macchina
     //-----------------------------------------------------------
     mac.Staz[num_stz].Pos[Posizione].Presenza=true;
     memcpy(&mac.Staz[num_stz].Pos[Posizione].Box, &AGV[num_agv]->mission.BoxA[0], sizeof(BOX));
     mac.Staz[num_stz].Pos[Posizione].Box.Pos = AGV[num_agv]->mission.pend;
     ID = mac.Staz[num_stz].Pos[Posizione].Box.ID;
     //--------------------------------------
     // Incremento cestoni in macchina
     //--------------------------------------
     mac.Staz[num_stz].NumCestoni++;
     //--------------------------------------
     // Azzeramento su TGV
     //--------------------------------------
     AGV[num_agv]->mission.PresCEST_A = LIBERA;
     memset( &AGV[num_agv]->mission.BoxA[0], 0, sizeof(BOX));
  }
  //--------------------------------------------------------------------
  // Forca B
  //--------------------------------------------------------------------
  if( TypPresa== N_FORCA_B || TypPresa== N_FORCA_AB ){
     //--------------------------------------
     // Scarico dati su macchina
     //--------------------------------------
     mac.Staz[num_stz].Pos[Posizione].Presenza=true;
     memcpy(&mac.Staz[num_stz].Pos[Posizione].Box, &AGV[num_agv]->mission.BoxB[0], sizeof(BOX));
     mac.Staz[num_stz].Pos[Posizione].Box.Pos = AGV[num_agv]->mission.pend;
     ID = mac.Staz[num_stz].Pos[Posizione].Box.ID;
     //--------------------------------------
     // Incremento cestoni in macchina
     //--------------------------------------
     mac.Staz[num_stz].NumCestoni++;
     //--------------------------------------
     // Azzeramento su TGV
     //--------------------------------------
     AGV[num_agv]->mission.PresCEST_B = LIBERA;
     memset( &AGV[num_agv]->mission.BoxB[0], 0, sizeof(BOX));
  }

  //--------------------------------------------------------------------
  // CANCELLAZIONE BOX DALL'ELENCO DI QUELLI DEPOSITATI
  //--------------------------------------------------------------------
  for(i=0; i<((MAXSTAZMAC*MAXPOSTISTAZ)-1); i++){
     if(mac.IDRic[i]!=ID) continue;
     //----------------------------------------------------
     // Elimino l'identificativo dalla lista e la compatto
     //----------------------------------------------------
     mac.IDRic[i]=0;
     for(j=i+1; j<MAXSTAZMAC*MAXPOSTISTAZ; j++){
        mac.IDRic[j-1] = mac.IDRic[j];
        mac.IDRic[j]   = 0;
     }
  }

  //--------------------------------------------------------------------
  // Azzeramento chiamata su TGV se completamente scarico
  //--------------------------------------------------------------------
  if( AGV[num_agv]->mission.PresCEST_A==LIBERA && AGV[num_agv]->mission.PresCEST_B==LIBERA ){
     AGV[num_agv]->mission.Chiamata = 0;
  }

  //--------------------------------------------------------------------
  // ESSICCATOI
  // Recupero ID essicazione maggiore tra quelli inseriti e scrittura
  // alla macchina
  //--------------------------------------------------------------------
  mac.IDEssicMacMax=0;
  if(mac.Tipo==MC_ESSICCATOIO){
     //-----------------------------------------
     // CALCOLO ID ESSICAZIONE MACCHINA
     // Scroll di tutte le postazioni impegnate
     //-----------------------------------------
     for(i=0; i<MAXSTAZMAC; i++){
        if(mac.Staz[num_stz].PuntoIng<1        ) continue;
        if(mac.Staz[num_stz].PuntoIng>TOTPUNTI ) continue;
        if(mac.Staz[num_stz].PuntoOut<1        ) continue;
        if(mac.Staz[num_stz].PuntoOut>TOTPUNTI ) continue;
        for(j=0; j<MAXPOSTISTAZ; j++){
           if(mac.Staz[num_stz].Pos[j].Presenza==false                       ) continue;
           if(mac.Staz[num_stz].Pos[j].Box.IdEssiccazione<mac.IDEssicMacMax  ) continue;
           mac.IDEssicMacMax = mac.Staz[num_stz].Pos[j].Box.IdEssiccazione;
        }
     }
     //-----------------------------------------
     // Controllo range ammesso
     //-----------------------------------------
     if(mac.IDEssicMacMax<1) mac.IDEssicMacMax=1;
     if(mac.IDEssicMacMax>3) mac.IDEssicMacMax=3;
     //-----------------------------------------
     // START ESSICAZIONE MACCHINA
     // Verifica condizione di start essicazione
     // = essiccatoio completo
     //-----------------------------------------
     macc_completa=true;
     for(i=0; i<MAXSTAZMAC; i++){
        if(mac.Staz[num_stz].PuntoIng<1        ) continue;
        if(mac.Staz[num_stz].PuntoIng>TOTPUNTI ) continue;
        if(mac.Staz[num_stz].PuntoOut<1        ) continue;
        if(mac.Staz[num_stz].PuntoOut>TOTPUNTI ) continue;
        for(j=0; j<mac.Staz[num_stz].Npt; j++){
           if(mac.Staz[num_stz].Pos[j].Presenza==true) continue;
           macc_completa=false;
           break;
        }
        if(macc_completa==false) break;
     }
     //-----------------------------------------
     // Se la macchina è piena do lo start ciclo
     //-----------------------------------------
     //if(macc_completa==true) mac.RichCicloEs=true;
     //-----------------------------------------
     // SCRITTURA A PLC
     //-----------------------------------------
     //
     //....
     //
  }

  //--------------------------------------------------------------------
  // AZZERAMENTO CHIAMATA SU SINGOLA STAZIONE SE COMPLETA
  //
  // Anche il tempo di chiamata lo azzero solo quando azzero la chiamata
  // questo garantirà la cronologia sugli essicatoi fino al riempimento
  //--------------------------------------------------------------------
  if(mac.Staz[num_stz].NumCestoni>=mac.Staz[num_stz].Npt){
     mac.Staz[num_stz].Richiesta    = false;
     mac.Staz[num_stz].TimeChiamata = time(NULL);
  }

  //--------------------------------------------------------------------
  // Memorizza FIne Operazione da resettare
  //--------------------------------------------------------------------
  mac.Staz[num_stz].NumCicliResetFineOpraz= NUM_CICLI_RESET_FINEOP;

  //--------------------------------------------------------------------
  // AZZERAMENTO ID RICHIESTI
  // Azzeramento dell'elenco degli ID BOX richiesti ogni
  // volta che la macchina è senza richiesta cestoni
  //--------------------------------------------------------------------
  nessuna_chiamata_attiva=true;
  for(i=0; i<MAXSTAZMAC; i++){
     if(mac.Staz[i].PuntoIng<1        ) continue;
     if(mac.Staz[i].PuntoIng>TOTPUNTI ) continue;
     if(mac.Staz[i].PuntoOut<1        ) continue;
     if(mac.Staz[i].PuntoOut>TOTPUNTI ) continue;
     if(mac.Staz[i].Richiesta==false  ) continue;
     nessuna_chiamata_attiva=false;
     break;
  }
  if(nessuna_chiamata_attiva==true){
     memset(&mac.IDRic[0], 0, sizeof(mac.IDRic));
  }


  //----------------------------------------------------------------------
  //12/02/2013 Incremento missione su scarichi reali per
  //           conteggiare le movimentazioni
  //----------------------------------------------------------------------
  AGV[num_agv]->mission.NumMissioni++;
  //----------------------------------------------------------------------
  // salvataggio struttura TGV
  //----------------------------------------------------------------------
  if(!err){
     err = AGV[num_agv]->save_mission(all_mess);
  }
  //----------------------------------------------------------------------
  // salvataggio struttura macchine
  //----------------------------------------------------------------------
  if(!err){
     memcpy( ch[mac.Num], &mac, sizeof(CHIAMATE));
     err = save_record_chiamate(mac.Num, all_mess);
  }
  return err;

  //-----------------------------------------------------------
*/
  return 0;
}

// ------------------------------
//   ScaricoSuBaia()
// ------------------------------
// Questa funzione setta il cestone scaricato da AGV
//
int process::ScaricoSuBaia(short int num_agv, char *all_mess)
/********************************************************/
{
  int   i, j, k, err=0;
  short int num_baia;
  short int count_in_presa, n, z, w;
  short int piano_di_deposito, piano_libero;
  //short int ID;
  int  piani = PIANIINPRESA;
  short int TypPresa;
  struct BAIE bb;
  bool NoBoxRealeSuForcaA=false;
  char Coordinate[101]="";
  char Operazione[101]="Bay Dropoff";

//ID=0;
  num_baia=0;
  memset(&bb, 0, sizeof(BAIE));
  //--------------------------------------------------------------------
  // NO_BOX_REALE_SU_FORCA_A
  // Disattiva l'aggiornamento box reale con i depositi Forca "A", ma
  // vengono aggiornati solo i piani del box in presenza bassa
  //--------------------------------------------------------------------
  #ifdef NO_BOX_REALE_SU_DEP_FORCA_A_IN_BAIA
     NoBoxRealeSuForcaA=true;
  #endif;
  //--------------------------------------------------------------------
  // Verifica se la destinazione AGV  un punto chiamante
  // se si, compila "num_ch" con la coppia KY = macchina,stazione.
  //--------------------------------------------------------------------
  if(P.test_punto_presente_baia(all_mess, AGV[num_agv]->mission.pend, &num_baia)) return 1;
  if(num_baia==0) return 1;
  //---------------------------------------------------
  // Verifico attendibilità del MAG
  //---------------------------------------------------
  if(P.read_baie( num_baia, &bb, all_mess)) return 1;

  //---------------------------------------------------
  // Compilazione dati struttura TGV cestone a bordo
  //
  // Controllando il TypPresa:
  // 1 = ForcaB(Bassa)
  // 2 = ForcaA(Alta)
  // 3 = ForcaAB(Alta e Bassa)
  //---------------------------------------------------
  TypPresa =  AGV[num_agv]->mission.TypPresa;

  //------------------------
  // Forca A
  //------------------------
  if( TypPresa== N_FORCA_A ){
     if(NoBoxRealeSuForcaA==false && (GestioneDoppiaForcaTGV==1 || (GestioneDoppiaForcaTGV==2 && bb.PresCEST_B!=LIBERA))){
        memcpy(&bb.BoxA, &AGV[num_agv]->mission.BoxA, sizeof(BOX));
        bb.BoxA.Pos = AGV[num_agv]->mission.pend;
        //---------------------------------------------------------
        // presenza e incremento postazioni occupate
        //---------------------------------------------------------
        bb.PresCEST_A = AGV[num_agv]->mission.PresCEST_A;
     }
     else{
        //----------------------------------------------------
        // Se la postazione ha BASSA è completamente libera
        // aggiorno tutti i dati dalla FORCA "A" nella
        // posizione "B" della baia
        //----------------------------------------------------
        if(bb.PresCEST_B==LIBERA || piani<2){
           memcpy(&bb.BoxB, &AGV[num_agv]->mission.BoxA, sizeof(BOX));
           bb.BoxB.Pos = AGV[num_agv]->mission.pend;
           //---------------------------------------------------------
           // presenza e incremento postazioni occupate
           //---------------------------------------------------------
           bb.PresCEST_B = AGV[num_agv]->mission.PresCEST_A;
           //---------------------------------------------------------
           // Aggiornamento dati postazione di deposito
           //---------------------------------------------------------
           bb.TipoCod[0] = AGV[num_agv]->mission.BoxA.StatoLav;
           bb.TipoBox[0] = AGV[num_agv]->mission.BoxA.TipoBox;
           bb.Codice[0]  = AGV[num_agv]->mission.BoxA.Codice;
           bb.Lotto[0]   = AGV[num_agv]->mission.BoxA.Lotto;
           bb.Altezza[0] = AGV[num_agv]->mission.BoxA.Altezza;
           bb.CarWash[0] = AGV[num_agv]->mission.BoxA.CarWash;
           bb.Basso[0]   = AGV[num_agv]->mission.BoxA.Basso;
           bb.Sovrap[0]  = AGV[num_agv]->mission.BoxA.Sovrap;
           bb.Ruotato180[0] = AGV[num_agv]->mission.BoxA.Ruotato180;
           memcpy(&bb.Presa[0][0][0], &AGV[num_agv]->mission.BoxA.Presa[0][0], sizeof(bb.Presa[0]));
           //----------------------------------------------------------
           // RICALCOLO LOCAZIONE
           //----------------------------------------------------------
           for(z=0; z<POSINPRESA; z++){
              for(w=0; w<PIANIINPRESA; w++){
                 if(bb.BoxB.Presa[z][w]==false) continue;
                 n=z+(w*POSINPRESA);
                 if(n>MAXUDCBOX) break;
                 bb.BoxB.Prd[n].loc_prov = AGV[num_agv]->mission.BoxA.Prd[n].location;
                 bb.BoxB.Prd[n].location = B.StructToLocation(0, 0, 0, bb.Num, 0, 0, 0, N_FORCA_B, n);
              }
           }
        }
        //----------------------------------------------------
        // Se la postazione bassa è già compilata aggiorno
        // solo la presenza ai piani superiori
        //----------------------------------------------------
        else{
           //-----------------------------------------------
           // Cerco il primo piano libero in presa
           //-----------------------------------------------
           piano_di_deposito=99;
           for(i=0; i<piani; i++){
              //------------------------------------
              // Cerco il primo piano completamente
              // libero.
              //------------------------------------
              piano_libero=0;
              for(j=0; j<POSINPRESA; j++){
                 if(bb.BoxB.Presa[j][i]==true) break;
                 piano_libero++;
              }
              //------------------------------------
              // Il primo piano completamente libero
              // è il piano di deposito
              //------------------------------------
              if(piano_libero>=POSINPRESA){
                 piano_di_deposito = i;
                 break;
              }
           }
           //-----------------------------------------------
           // Piano di deposito non trovato --> skip!
           //-----------------------------------------------
           if(piano_di_deposito==99){
              sprintf(all_mess, "Manca piano di deposito su baia nr.%d", bb.Num);
              return 1;
           }
           //-----------------------------------------------
           // Aggiorno solo le presenze dal piano libero
           //-----------------------------------------------
           k=0;
           for(i=piano_di_deposito; i<PIANIINPRESA; i++){
              for(j=0; j<POSINPRESA; j++){
                 bb.BoxB.Presa[j][i] = AGV[num_agv]->mission.BoxA.Presa[j][k];  // struttura "B"
              }
              k++;
           }
           //-----------------------------------------------
           // Aggiornamento bit di presenza globali
           // prendendoli dalla presenza "B" appena
           // aggiornata dal TGV in scarico
           //-----------------------------------------------
           memcpy(&bb.Presa[0][0][0], &bb.BoxB.Presa[0][0], sizeof(bb.Presa[0]));
        }
     }
     //---------------------------------------------------------
     // Azzeramento dati su TGV
     //---------------------------------------------------------
     AGV[num_agv]->mission.PresCEST_A=LIBERA;
     memset(&AGV[num_agv]->mission.BoxA, 0, sizeof(BOX));
  }
  //------------------------
  // Forca B
  //------------------------
  if( TypPresa== N_FORCA_B ){
     memcpy(&bb.BoxB, &AGV[num_agv]->mission.BoxB, sizeof(BOX));
     bb.BoxB.Pos = AGV[num_agv]->mission.pend;
     //---------------------------------------------------------
     // presenza e incremento postazioni occupate
     //---------------------------------------------------------
     bb.PresCEST_B = AGV[num_agv]->mission.PresCEST_B;
     //---------------------------------------------------------
     // Aggiornamento dati postazione di deposito
     //---------------------------------------------------------
     bb.TipoCod[0] = AGV[num_agv]->mission.BoxB.StatoLav;
     bb.TipoBox[0] = AGV[num_agv]->mission.BoxB.TipoBox;
     bb.Codice[0]  = AGV[num_agv]->mission.BoxB.Codice;
     bb.Lotto[0]   = AGV[num_agv]->mission.BoxB.Lotto;
     bb.Altezza[0] = AGV[num_agv]->mission.BoxB.Altezza;
     bb.CarWash[0] = AGV[num_agv]->mission.BoxB.CarWash;
     bb.Basso[0]   = AGV[num_agv]->mission.BoxB.Basso;
     bb.Sovrap[0]  = AGV[num_agv]->mission.BoxB.Sovrap;
     bb.Ruotato180[0] = AGV[num_agv]->mission.BoxB.Ruotato180;
     memcpy(&bb.Presa[0][0][0], &AGV[num_agv]->mission.BoxB.Presa[0][0], sizeof(bb.Presa[0]));
     //----------------------------------------------------------
     // RICALCOLO LOCAZIONE
     //----------------------------------------------------------
     for(z=0; z<POSINPRESA; z++){
        for(w=0; w<PIANIINPRESA; w++){
           if(bb.BoxB.Presa[z][w]==false) continue;
           n=z+(w*POSINPRESA);
           if(n>MAXUDCBOX) break;
           bb.BoxB.Prd[n].loc_prov = AGV[num_agv]->mission.BoxB.Prd[n].location;
           bb.BoxB.Prd[n].location = B.StructToLocation(0, 0, 0, bb.Num, 0, 0, 0, N_FORCA_B, n);
        }
     }
     //---------------------------------------------------------
     // Azzeramento dati su TGV
     //---------------------------------------------------------
     AGV[num_agv]->mission.PresCEST_B=LIBERA;
     memset(&AGV[num_agv]->mission.BoxB, 0, sizeof(BOX));
  }
  //------------------------
  // Forca A+B
  //------------------------
  if( TypPresa== N_FORCA_AB ){
     memcpy(&bb.BoxA, &AGV[num_agv]->mission.BoxA, sizeof(BOX));
     memcpy(&bb.BoxB, &AGV[num_agv]->mission.BoxB, sizeof(BOX));
     bb.BoxA.Pos = AGV[num_agv]->mission.pend;
     bb.BoxB.Pos = AGV[num_agv]->mission.pend;
     //---------------------------------------------------------
     // presenza e incremento postazioni occupate
     //---------------------------------------------------------
     bb.PresCEST_A = AGV[num_agv]->mission.PresCEST_A;
     bb.PresCEST_B = AGV[num_agv]->mission.PresCEST_B;
     //---------------------------------------------------------
     // Aggiornamento dati postazione di deposito
     //---------------------------------------------------------
     bb.TipoCod[0] = AGV[num_agv]->mission.BoxB.StatoLav;
     bb.TipoBox[0] = AGV[num_agv]->mission.BoxB.TipoBox;
     bb.Codice[0]  = AGV[num_agv]->mission.BoxB.Codice;
     bb.Lotto[0]   = AGV[num_agv]->mission.BoxB.Lotto;
     bb.Altezza[0] = AGV[num_agv]->mission.BoxB.Altezza;
     bb.CarWash[0] = AGV[num_agv]->mission.BoxB.CarWash;
     bb.Basso[0]   = AGV[num_agv]->mission.BoxB.Basso;
     bb.Sovrap[0]  = AGV[num_agv]->mission.BoxB.Sovrap;
     bb.Ruotato180[0]  = AGV[num_agv]->mission.BoxB.Ruotato180;
     memcpy(&bb.Presa[0][0][0], &AGV[num_agv]->mission.BoxB.Presa[0][0], sizeof(bb.Presa[0]));
     //---------------------------------------------------------
     // Azzeramento dati su TGV
     //---------------------------------------------------------
     AGV[num_agv]->mission.PresCEST_A=LIBERA;
     AGV[num_agv]->mission.PresCEST_B=LIBERA;
     memset(&AGV[num_agv]->mission.BoxA, 0, sizeof(BOX));
     memset(&AGV[num_agv]->mission.BoxB, 0, sizeof(BOX));
     //----------------------------------------------------------
     // RICALCOLO LOCAZIONE "FORCA A"
     //----------------------------------------------------------
     for(z=0; z<POSINPRESA; z++){
        for(w=0; w<PIANIINPRESA; w++){
           if(bb.BoxA.Presa[z][w]==false) continue;
           n=z+(w*POSINPRESA);
           if(n>MAXUDCBOX) break;
           bb.BoxA.Prd[n].loc_prov = AGV[num_agv]->mission.BoxA.Prd[n].location;
           bb.BoxA.Prd[n].location = B.StructToLocation(0, 0, 0, bb.Num, 0, 0, 0, N_FORCA_A, n);
        }
     }
     //----------------------------------------------------------
     // RICALCOLO LOCAZIONE "FORCA B"
     //----------------------------------------------------------
     for(z=0; z<POSINPRESA; z++){
        for(w=0; w<PIANIINPRESA; w++){
           if(bb.BoxB.Presa[z][w]==false) continue;
           n=z+(w*POSINPRESA);
           if(n>MAXUDCBOX) break;
           bb.BoxB.Prd[n].loc_prov = AGV[num_agv]->mission.BoxB.Prd[n].location;
           bb.BoxB.Prd[n].location = B.StructToLocation(0, 0, 0, bb.Num, 0, 0, 0, N_FORCA_B, n);
        }
     }
  }
  //-------------------------------------------------
  // Aggiorno presenza pallet in baia
  //-------------------------------------------------
  count_in_presa=0;
  for(i=0; i<POSINPRESA; i++){
     for(j=0; j<PIANIINPRESA; j++){
        if(bb.Presa[0][i][j]==false) continue;
        count_in_presa++;
     }
  }
  bb.StatoPres = count_in_presa;
  //-------------------------------------------------
  //
  // RESET CHIAMATA SU BAIA E SU TGV:
  //
  // singola forca e singolo carico:
  // (GestioneDoppiaForcaTGV=0) se N_FORCA_B.
  //
  // doppia forca e doppio carico:
  // (GestioneDoppiaForcaTGV=1) se N_FORCA_B.
  //                            se N_FORCA_AB.
  // singola forca e doppio carico:
  // (GestioneDoppiaForcaTGV=2) se N_FORCA_B.
  //                            se N_FORCA_AB.
  //-------------------------------------------------
  if(GestioneDoppiaForcaTGV==0 && TypPresa== N_FORCA_B                          ){bb.Chiamata = false; AGV[num_agv]->mission.Chiamata = 0;}
  if(GestioneDoppiaForcaTGV!=0 &&(TypPresa== N_FORCA_B || TypPresa== N_FORCA_AB)){bb.Chiamata = false; AGV[num_agv]->mission.Chiamata = 0;}

  //-------------------------------------------------
  // 12/02/2013 Incremento missione su scarichi
  // reali per conteggiare le movimentazioni
  //-------------------------------------------------
  AGV[num_agv]->mission.NumMissioni++;

  //-------------------------------------------------
  // salvataggio struttura TGV
  //-------------------------------------------------
  if(!err){
     err = AGV[num_agv]->save_mission(all_mess);
  }

  //-------------------------------------------------
  // salvataggio struttura Baie
  //-------------------------------------------------
  if(!err){
     memcpy( P.ba[bb.Num], &bb, sizeof(BAIE));
     err = P.save_record_baie(bb.Num, all_mess);
  }

  //----------------------------------------------------------------
  // GESTIONE UDC
  // da stuttura software ad array UDC per aggiornamento SQL
  //
  // !!! IMPORTANTE !!!
  // dopo aver aggiornato la struttura globale del software
  //----------------------------------------------------------------
  if(GestioneUdc>0){
     // -- SET LOCAZIONI SU UDC A BORDO TGV
     if(!err){
        err = B.StructToUdcTmp(CMD_WR_SQL_UDC_SET_LOC, 0, 0, 0, bb.Num, UDC_ALL_STZ, UDC_ALL_POS, UDC_ALL_LEV, TypPresa, all_mess);
        if(GestioneUdc==2 && err!=0) err=0;
     }
  }

  //-----------------------------------------------------------
  // Log Traccia Movimenti
  //-----------------------------------------------------------
  sprintf(Coordinate,"Bay %02d", num_baia);
  LogMovimenti(Operazione, Coordinate, num_agv, bb.BoxA, bb.BoxB, TypPresa, 0);

  return err;
}

// ------------------------------
//   ScaricoSuMagazzino()
// ------------------------------
// Questa funzione setta il cestone scaricato da AGV
//
int process::ScaricoSuMagazzino(short int num_agv, char *all_mess)
/********************************************************/
{
  int   i, j, err=0;
  double a, b;
  double RapImp;
  double Xly, Yly;
  double CatetoX, CatetoY, CalcolaRad;
  int    SovrapCarWash = SOVRAPPOSIZIONE_CARWASH;
  int    x1, y1, x2, y2;
  bool  Basso;
  bool  lato_uscita;
  bool  primo_inserimento;
  bool  PresaSovrap, reset_presenze_dinamico;
  WORD  QuotaH_A, QuotaH_B, QuotaP;
  short int bai_ch, mac_ch, mag_ch, pos_ch;
  short int pos_trovata, z, w, n;
  short int num_mag, num_pos, TipoBox;
  short int lato_box, traccia_mag;
  short int TypPresa, TypPresaPLC;
  short int Posizione, PosizionePLC;
  short int Piano, PianoPLC;
  short int ID, PosizioneTgv;
  long int  TimeBox;
  short int tipo_prov_box, mac_prov_box, bai_prov_box, mag_prov_box;
  struct MAG mag;
  WORD dm[3];
  bool Presa[POSINPRESA+1][PIANIINPRESA+1];
  char Coordinate[101]="";
  char Operazione[101]="Storage Dropoff";


  RapImp=1;    // gestione (cm)
  num_mag=0;
  num_pos=0;
  Xly=Yly=0;
  memset(&mag,    0, sizeof(MAG));
  memset(&dm[0],  0, sizeof(dm));
  //--------------------------------------------------------------------
  // Verifica se la destinazione AGV  un punto chiamante
  // se si, compila "num_ch" con la coppia KY = macchina,stazione.
  //--------------------------------------------------------------------
  if(M.test_punto_presente_mag(all_mess, AGV[num_agv]->mission.pend, &num_mag, &num_pos)) return 1;
  if(num_mag==0) return 1;
  //--------------------------------------------------------------------
  // Verifico attendibilità del MAG
  //--------------------------------------------------------------------
  if(M.read_mag( num_mag, &mag, all_mess)) return 1;
  //--------------------------------------------------------------------
  // Compilazione dati struttura TGV cestone a bordo
  //
  // Controllando il TypPresa:
  // 1 = ForcaB(Bassa)
  // 2 = ForcaA(Alta)
  // 3 = ForcaAB(Alta e Bassa)
  //
  // AL-20/06/2013 LE PRESE SONO TUTTE SOVRAPPONIBILI
  // tranne quelle con il flag di Car-Wash.
  // Il bit di "sovrapposizione" è importante per il
  // controllo dello stato della presa su cui sovraporre
  // che ovviamente deve essere ON
  //--------------------------------------------------------------------
  TypPresa    = AGV[num_agv]->mission.TypPresa;
  TipoBox     = TIPOBOX_1;
  PresaSovrap = true;
  //--------------------------------------------------------------------
  // Recupero dati in base a forca in lavoro
  //--------------------------------------------------------------------
  memset(&Presa[0][0], false, sizeof(Presa));
  if( TypPresa==N_FORCA_A  ){
     TipoBox = AGV[num_agv]->mission.BoxA.TipoBox;
     Basso   = AGV[num_agv]->mission.BoxA.Basso;
     //---------------------------------------------------------
     // AL-05/04/2020 DEPOSITO IN MAGAZZINI SOLO PER EMERGENZA
     //               Forzatura CARWASH
     //---------------------------------------------------------
   //if(mag.PerEmergenza==1 && AGV[num_agv]->mission.BoxA.CarWash==0) AGV[num_agv]->mission.BoxA.CarWash=1;
     //---------------------------------------------------------
     // Pallet in CAR-WASH non sovrapponibili
     //---------------------------------------------------------
     if(SovrapCarWash==0 && AGV[num_agv]->mission.BoxA.CarWash!=0) PresaSovrap=false;
     memcpy(&Presa[0][0], &AGV[num_agv]->mission.BoxA.Presa[0][0], sizeof(Presa));
  }
  if( TypPresa==N_FORCA_B  ){
     TipoBox = AGV[num_agv]->mission.BoxB.TipoBox;
     Basso   = AGV[num_agv]->mission.BoxB.Basso;
     //---------------------------------------------------------
     // AL-05/04/2020 DEPOSITO IN MAGAZZINI SOLO PER EMERGENZA
     //               Forzatura CARWASH
     //---------------------------------------------------------
   //if(mag.PerEmergenza==1 && AGV[num_agv]->mission.BoxB.CarWash==0) AGV[num_agv]->mission.BoxB.CarWash=1;
     //---------------------------------------------------------
     // Pallet in CAR-WASH non sovrapponibili
     //---------------------------------------------------------
     if(SovrapCarWash==0 && AGV[num_agv]->mission.BoxB.CarWash!=0) PresaSovrap=false;
     memcpy(&Presa[0][0], &AGV[num_agv]->mission.BoxB.Presa[0][0], sizeof(Presa));
  }
  if( TypPresa==N_FORCA_AB ){
     TipoBox = AGV[num_agv]->mission.BoxB.TipoBox;
     Basso   = AGV[num_agv]->mission.BoxB.Basso;
     //---------------------------------------------------------
     // AL-05/04/2020 DEPOSITO IN MAGAZZINI SOLO PER EMERGENZA
     //               Forzatura CARWASH
     //---------------------------------------------------------
   //if(mag.PerEmergenza==1){
   //   if(AGV[num_agv]->mission.BoxA.CarWash==0) AGV[num_agv]->mission.BoxA.CarWash=1;
   //   if(AGV[num_agv]->mission.BoxB.CarWash==0) AGV[num_agv]->mission.BoxB.CarWash=1;
   //}
     //---------------------------------------------------------
     // Pallet in CAR-WASH non sovrapponibili
     //---------------------------------------------------------
     if(SovrapCarWash==0 && AGV[num_agv]->mission.BoxB.CarWash!=0) PresaSovrap=false;
     memcpy(&Presa[0][0], &AGV[num_agv]->mission.BoxB.Presa[0][0], sizeof(Presa));
  }
  //--------------------------------------------------------------------
  // Memorizzo che si tratterà del primo inserimento
  //--------------------------------------------------------------------
  primo_inserimento=false;
  if(mag.NumCestoni==0) primo_inserimento=true;

  //--------------------------------------------------------------------
  // Recupero la posizione di deposito dal nodo di destinazione
  //--------------------------------------------------------------------
  lato_uscita=false;
  if(mag.Fifo==true && AGV[num_agv]->mission.NodIngMag==mag.PuntoOut) lato_uscita=true;
  if(M.VerificaTipoDeposito(lato_uscita, num_mag, num_pos, TypPresa, TipoBox, Presa, PresaSovrap, Basso, FALSE, &Posizione, &Piano, &PosizioneTgv, all_mess)) return 1;

  //--------------------------------------------------------------------
  // GESTIONE POSIZIONE REALE
  // Recupero dal PLC le reali quote di deposito sia in profondità
  // che in altezza
  //
  // DM51 = Quato deposito in ALTEZZA FORCA "B"
  // DM52 = Quato deposito in ALTEZZA FORCA "A"
  //--------------------------------------------------------------------
  QuotaH_B=QuotaH_A=QuotaP=0;
  // QUOTA IN ALTEZZA
  if(break_Com[COM1]==false && !AGV[num_agv]->mission.EsclPLC){
     err = OM.ld_canale(&dm[0], (WORD)(num_agv), "RD", 51, 2, all_mess);
     if(err) return 1;
     QuotaH_B = dm[0];                                   // altezza "B"
     QuotaH_A = dm[0];                                   // altezza "A"
     //-------------------------------------------------------------
     // GESTIONE FORCHE DOPPIE
     // solo per quando sono realmente esistenti 2 forche sul TGV
     // ha valore il DM52
     //-------------------------------------------------------------
     if(GestioneDoppiaForcaTGV==1) QuotaH_A = dm[1];
  }


  //====================================================================
  // Per TEST (INSERIMENTO QUOTE IPOTETICHE PASSATE DA PLC TGV)
  //
  //
//int pos;
//AnsiString InputStringa;

//InputStringa = InputBox("Introduzione manuale coordinate TGV", "formato richiesto (xxxx,yyyy):", "");
//if(InputStringa.IsEmpty()==false){
//   pos = InputStringa.Pos(",");
//   if(pos!=0){
//      Xly = StrToInt(InputStringa.SubString(0,                    pos-1 ));
//      Yly = StrToInt(InputStringa.SubString(pos+1, InputStringa.Length()));
//   }
//}
  //
  //====================================================================

  // QUOTA IN PROFONDITA' (DM93 per magneti o coordinate laser)
  if(break_Com[COM1]==false && !AGV[num_agv]->mission.EsclPLC){
     if(AGV[num_agv]->mission.GestioneLaser==true){
        AGV[num_agv]->convertiXY_laser_to_layout(AGV[num_agv]->stato.x_laser/10, AGV[num_agv]->stato.y_laser/10, &Xly, &Yly);
        x1 = (int) Xly;                               // coordinata X posizione TGV nell'impianto
        y1 = (int) Yly;                               // coordinata Y posizione TGV nell'impianto
        x2 = Punti[ AGV[num_agv]->stato.pos ][0];     // coordinata X posizione nodo occupato
        y2 = Punti[ AGV[num_agv]->stato.pos ][1];     // coordinata Y posizione nodo occupato
        //----------------------------------------
        // Calcolo distanza dal nodo
        //----------------------------------------
        CatetoX = (double) ((abs)(x1-x2));
        CatetoY = (double) ((abs)(y1-y2));
        QuotaP  = 0;
        RapImp  = 1;  // divisore "1" = cm
        if(CatetoX<30000 && CatetoY<30000){
           CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
           QuotaP = (int) sqrt( CalcolaRad );            // profondità (CM)
        }
     }
     // magneti
     else{
        QuotaP = AGV[num_agv]->stato.offset_da_nodo;     // profondità (IMPULSI)
        RapImp = AGV[num_agv]->mission.RapEncoder;
     }
  }

  //--------------------------------------------------------------------
  // GESTIONE_DINAMICA_MAG
  // La capacità viene forzata dal TGV in base al tipo box depositato
  //--------------------------------------------------------------------
  if(GestioneDinamicaMagazzini==1){
     //--------------------------------------------------------------------
     // Conversione QUOTE in POS/PIANI
     //--------------------------------------------------------------------
     if(M.ConvertiQuotePLC(lato_uscita, num_mag, RapImp, TipoBox, QuotaP, QuotaH_A, QuotaH_B, TypPresa, Posizione, Piano, &TypPresaPLC, &PosizionePLC, &PianoPLC, all_mess)) return 1;
     //--------------------------------------------------------------------
     // Aggiorno dati rielaborati
     //--------------------------------------------------------------------
     TypPresa  = TypPresaPLC;
     Posizione = PosizionePLC;
     Piano     = PianoPLC;
     //---------------------------------------------------------
     // Recupero il lato box su cui eseguire il calcolo
     //---------------------------------------------------------
     lato_box = M.TipiBox[TipoBox].Lung;
     //---------------------------------------------
     // Calcolo postazioni a terra
     //---------------------------------------------
   //mag.Npt = (short int)(mag.Profondita/(lato_box+DIST_CEST_IN_MAG))+1;
     a = (double)(mag.Profondita)/(double)(lato_box+M.TipiBox[TipoBox].dist_box_in_magaz);
     b = (double)(a) - (int)(a);
     if(b>M.TipiBox[TipoBox].eccesso_count_pos_mag) a = (a - b) + 1;      // tolgo i decimali al valore calcolato e aggingo "1".
     mag.Npt = (short int)(a)+1;     // calcolo effettivo palette a terra
  }
  //--------------------------------------------------------------------
  // Forca A
  //--------------------------------------------------------------------
  if( TypPresa== N_FORCA_A ){
     memcpy(&mag.Pos[Posizione].Box[Piano ], &AGV[num_agv]->mission.BoxA, sizeof(BOX));
     mag.Pos[Posizione].Box[Piano ].Pos        = AGV[num_agv]->mission.pend;
     mag.Pos[Posizione].Box[Piano ].GruppoDest = mag.Gruppo;
     mag.Pos[Posizione].QuotaH[Piano ]         = QuotaH_A;
     mag.Pos[Posizione].QuotaP[Piano ]         = QuotaP;
     //---------------------------------------------------------
     // Memorizzo data/ora di carico box
     //---------------------------------------------------------
     TimeBox = AGV[num_agv]->mission.BoxA.TimeC;
     //---------------------------------------------------------
     // Memorizzo struttura di provenienza
     //---------------------------------------------------------
     tipo_prov_box=mac_prov_box=bai_prov_box=mag_prov_box=0;
     P.TestProvenienzaBox(AGV[num_agv]->mission.BoxA.ProvBox, &tipo_prov_box, &mac_prov_box, &bai_prov_box, &mag_prov_box);
     //---------------------------------------------------------
     // presenza e incremento postazioni occupate
     //---------------------------------------------------------
     mag.Pos[Posizione].Presenza[Piano] = true;
     mag.NumCestoni++;
     //---------------------------------------------------------
     // Azzeramento di tutti i piani superiori a quello su cui
     // ho depositato
     //---------------------------------------------------------
     for(j=Piano+1; j<MAXPIANIMAG; j++){
        mag.Pos[Posizione].Presenza[j] = false;
        mag.Pos[Posizione].QuotaH[j]   = 0;
        mag.Pos[Posizione].QuotaP[j]   = 0;
        memset(&mag.Pos[Posizione].Box[j], 0, sizeof(BOX));
     }
     //----------------------------------------------------------
     // RICALCOLO LOCAZIONE
     //----------------------------------------------------------
     for(z=0; z<POSINPRESA; z++){
        for(w=0; w<PIANIINPRESA; w++){
           if(mag.Pos[Posizione].Box[Piano].Presa[z][w]==false) continue;
           n=z+(w*POSINPRESA);
           if(n>MAXUDCBOX) break;
           mag.Pos[Posizione].Box[Piano].Prd[n].loc_prov = AGV[num_agv]->mission.BoxA.Prd[n].location;
           mag.Pos[Posizione].Box[Piano].Prd[n].location = B.StructToLocation(0, 0, mag.Num, 0, 0, Posizione, Piano, N_FORCA_A, n);
        }
     }
     //---------------------------------------------------------
     // Azzeramento dati su TGV
     //---------------------------------------------------------
     AGV[num_agv]->mission.PresCEST_A=LIBERA;
     memset(&AGV[num_agv]->mission.BoxA, 0, sizeof(BOX));
  }
  //--------------------------------------------------------------------
  // Forca B
  //--------------------------------------------------------------------
  if( TypPresa== N_FORCA_B ){
     memcpy(&mag.Pos[Posizione].Box[Piano ], &AGV[num_agv]->mission.BoxB, sizeof(BOX));
     mag.Pos[Posizione].Box[Piano ].Pos        = AGV[num_agv]->mission.pend;
     mag.Pos[Posizione].Box[Piano ].GruppoDest = mag.Gruppo;
     mag.Pos[Posizione].QuotaH[Piano ]         = QuotaH_B;
     mag.Pos[Posizione].QuotaP[Piano ]         = QuotaP;
     //---------------------------------------------------------
     // Memorizzo data/ora di carico box
     //---------------------------------------------------------
     TimeBox = AGV[num_agv]->mission.BoxB.TimeC;
     //---------------------------------------------------------
     // Memorizzo struttura di provenienza
     //---------------------------------------------------------
     tipo_prov_box=mac_prov_box=bai_prov_box=mag_prov_box=0;
     P.TestProvenienzaBox(AGV[num_agv]->mission.BoxB.ProvBox, &tipo_prov_box, &mac_prov_box, &bai_prov_box, &mag_prov_box);
     //---------------------------------------------------------
     // presenza e incremento postazioni occupate
     //---------------------------------------------------------
     mag.Pos[Posizione].Presenza[Piano] = true;
     mag.NumCestoni++;
     //---------------------------------------------------------
     // Azzeramento di tutti i piani superiori a quello su cui
     // ho depositato
     //---------------------------------------------------------
     for(j=Piano+1; j<MAXPIANIMAG; j++){
        mag.Pos[Posizione].Presenza[j] = false;
        mag.Pos[Posizione].QuotaH[j]   = 0;
        mag.Pos[Posizione].QuotaP[j]   = 0;
        memset(&mag.Pos[Posizione].Box[j], 0, sizeof(BOX));
     }
     //----------------------------------------------------------
     // RICALCOLO LOCAZIONE
     //----------------------------------------------------------
     for(z=0; z<POSINPRESA; z++){
        for(w=0; w<PIANIINPRESA; w++){
           if(mag.Pos[Posizione].Box[Piano].Presa[z][w]==false) continue;
           n=z+(w*POSINPRESA);
           if(n>MAXUDCBOX) break;
           mag.Pos[Posizione].Box[Piano].Prd[n].loc_prov = AGV[num_agv]->mission.BoxB.Prd[n].location;
           mag.Pos[Posizione].Box[Piano].Prd[n].location = B.StructToLocation(0, 0, mag.Num, 0, 0, Posizione, Piano, N_FORCA_B, n);
        }
     }
     //---------------------------------------------------------
     // Azzeramento dati su TGV
     //---------------------------------------------------------
     AGV[num_agv]->mission.PresCEST_B=LIBERA;
     memset(&AGV[num_agv]->mission.BoxB, 0, sizeof(BOX));
  }
  //--------------------------------------------------------------------
  // Forca A+B
  //--------------------------------------------------------------------
  if( TypPresa== N_FORCA_AB ){
     memcpy(&mag.Pos[Posizione].Box[Piano+1], &AGV[num_agv]->mission.BoxA, sizeof(BOX));
     memcpy(&mag.Pos[Posizione].Box[Piano  ], &AGV[num_agv]->mission.BoxB, sizeof(BOX));
     mag.Pos[Posizione].Box[Piano+1].Pos        = AGV[num_agv]->mission.pend;
     mag.Pos[Posizione].Box[Piano+1].GruppoDest = mag.Gruppo;
     mag.Pos[Posizione].Box[Piano  ].Pos        = AGV[num_agv]->mission.pend;
     mag.Pos[Posizione].Box[Piano  ].GruppoDest = mag.Gruppo;
     mag.Pos[Posizione].QuotaH[Piano+1] = QuotaH_A;
     mag.Pos[Posizione].QuotaH[Piano  ] = QuotaH_B;
     mag.Pos[Posizione].QuotaP[Piano+1] = QuotaP;
     mag.Pos[Posizione].QuotaP[Piano  ] = QuotaP;
     //---------------------------------------------------------
     // Memorizzo data/ora di carico box
     //---------------------------------------------------------
     TimeBox = AGV[num_agv]->mission.BoxB.TimeC;
     //---------------------------------------------------------
     // Memorizzo struttura di provenienza
     //---------------------------------------------------------
     tipo_prov_box=mac_prov_box=bai_prov_box=mag_prov_box=0;
     P.TestProvenienzaBox(AGV[num_agv]->mission.BoxB.ProvBox, &tipo_prov_box, &mac_prov_box, &bai_prov_box, &mag_prov_box);
     //---------------------------------------------------------
     // presenza e incremento postazioni occupate
     //---------------------------------------------------------
     mag.Pos[Posizione].Presenza[Piano+1] = true;
     mag.Pos[Posizione].Presenza[Piano  ] = true;
     mag.NumCestoni++;
     mag.NumCestoni++;
     //---------------------------------------------------------
     // Azzeramento di tutti i piani superiori a quello su cui
     // ho depositato
     //---------------------------------------------------------
     for(j=Piano+2; j<MAXPIANIMAG; j++){
        mag.Pos[Posizione].Presenza[j] = false;
        mag.Pos[Posizione].QuotaH[j]   = 0;
        mag.Pos[Posizione].QuotaP[j]   = 0;
        memset(&mag.Pos[Posizione].Box[j], 0, sizeof(BOX));
     }
     //----------------------------------------------------------
     // RICALCOLO LOCAZIONE "FORCA A"
     //----------------------------------------------------------
     for(z=0; z<POSINPRESA; z++){
        for(w=0; w<PIANIINPRESA; w++){
           if(mag.Pos[Posizione].Box[Piano+1].Presa[z][w]==false) continue;
           n=z+(w*POSINPRESA);
           if(n>MAXUDCBOX) break;
           mag.Pos[Posizione].Box[Piano+1].Prd[n].loc_prov = AGV[num_agv]->mission.BoxA.Prd[n].location;
           mag.Pos[Posizione].Box[Piano+1].Prd[n].location = B.StructToLocation(0, 0, mag.Num, 0, 0, Posizione, Piano+1, N_FORCA_A, n);
        }
     }
     //----------------------------------------------------------
     // RICALCOLO LOCAZIONE "FORCA B"
     //----------------------------------------------------------
     for(z=0; z<POSINPRESA; z++){
        for(w=0; w<PIANIINPRESA; w++){
           if(mag.Pos[Posizione].Box[Piano].Presa[z][w]==false) continue;
           n=z+(w*POSINPRESA);
           if(n>MAXUDCBOX) break;
           mag.Pos[Posizione].Box[Piano].Prd[n].loc_prov = AGV[num_agv]->mission.BoxB.Prd[n].location;
           mag.Pos[Posizione].Box[Piano].Prd[n].location = B.StructToLocation(0, 0, mag.Num, 0, 0, Posizione, Piano, N_FORCA_B, n);
        }
     }
     //---------------------------------------------------------
     // Azzeramento dati su TGV
     //---------------------------------------------------------
     AGV[num_agv]->mission.PresCEST_A=LIBERA;
     memset(&AGV[num_agv]->mission.BoxA, 0, sizeof(BOX));
     AGV[num_agv]->mission.PresCEST_B=LIBERA;
     memset(&AGV[num_agv]->mission.BoxB, 0, sizeof(BOX));
  }

  //--------------------------------------------------------------------
  // GESTIONE DINAMICA MAGAZZINO
  // Cancellazione automatica di tutte le posizioni a terra prima di
  // quella di deposito
  //--------------------------------------------------------------------
  if(GestioneDinamicaMagazzini==1){
     reset_presenze_dinamico=false;
     for(i=1; i<Posizione; i++){
        for(j=0; j<MAXPIANIMAG; j++){
           if(mag.Pos[i].Presenza[j]==false) continue;
           reset_presenze_dinamico=true;
           mag.Pos[i].Presenza[j] = false;
           mag.Pos[i].QuotaH[j]   = 0;
           mag.Pos[i].QuotaP[j]   = 0;
           memset(&mag.Pos[i].Box[j], 0, sizeof(BOX));
        }
     }
     //------------------------------------------------
     // Se è avvenuto un azzeramento di presenze
     // per il deposito dinamico del TGV, è necessario
     // riconteggiare l'esatto numero di cestoni
     // presenti in magazzino
     //------------------------------------------------
     if(reset_presenze_dinamico==true){
        mag.NumCestoni=0;
        mag.Completo=false;
        for(i=1; i<=mag.Npt; i++){
           for(j=0; j<MAXPIANIMAG; j++){
              if(mag.Pos[i].Presenza[j]==false) continue;
              mag.NumCestoni++;
           }
        }
     }
  }

  //--------------------------------------------------------------------
  // GESTIONE COMPLETO
  // tutte le posizioni sono impegnate e siresetta appena una posizione
  // è libera.
  //
  // Nota: Nei magazzini LIFO di pieni viene mantenuto per lo
  //       svuotamento
  //--------------------------------------------------------------------
  if(mag.Npt!=0 && mag.NumCestoni>=(mag.Npt*(mag.Sovrapposizione+1))) mag.Completo=true;
  else{
     if(mag.Fifo==true || (mag.Fifo==false && mag.Stato==MG_ST_VUOTI)) mag.Completo=false;
  }

  //--------------------------------------------------------------------
  // SICUREZZA
  // Aggiorno lo stato del magazzino in base al prossimo tipo di box
  // in prelievo
  //--------------------------------------------------------------------
  if(mag.NumCestoni>0){
     pos_trovata=99;
     if( mag.Fifo==false ){
        for(i=1; i<=MAXTERRAMAG; i++){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(mag.Pos[i].Presenza[j]==false) continue;
              pos_trovata=j;
              break;
           }
           if(pos_trovata!=99) break;
        }
     }
     else{
        for(i=MAXTERRAMAG; i>0; i--){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(mag.Pos[i].Presenza[j]==false) continue;
              pos_trovata=j;
              break;
           }
           if(pos_trovata!=99) break;
        }
     }
     if(pos_trovata!=99 && mag.Pos[i].Presenza[pos_trovata]==true){
        //------------------------------------------
        // Setto il tipo di magazzino in base
        // a l'unico box depositato
        //------------------------------------------
        mag.TipoBox = mag.Pos[i].Box[pos_trovata].TipoBox;
        if(mag.Pos[i].Box[pos_trovata].StatoLav!=ST_BOX_VUOTI) mag.Stato=MG_ST_PIENI;
        if(mag.Pos[i].Box[pos_trovata].StatoLav==ST_BOX_VUOTI) mag.Stato=MG_ST_VUOTI;
     }
     //----------------------------------------------------------------
     // Controllo che il Tipo di magazzino ammetta i box in esso
     // inseriti
     //----------------------------------------------------------------
     if((mag.Tipo==MG_VUOTI && mag.Stato!=MG_ST_VUOTI) || (mag.Tipo!=MG_VUOTI && mag.Stato==MG_ST_VUOTI)){
        strcpy(all_mess, MESS[324]);
        return 1;
     }
  }

  //--------------------------------------------------------------------
  // SICUREZZA
  // Azzero i dati di tutte le postazioni oltre alla capacità magazzino
  //--------------------------------------------------------------------
  for(i=mag.Npt+1; i<=MAXTERRAMAG; i++){
     for(j=0; j<MAXPIANIMAG; j++){
        mag.Pos[i].Presenza[j] = false;
        mag.Pos[i].QuotaH[j]   = 0;
        mag.Pos[i].QuotaP[j]   = 0;
        memset(&mag.Pos[i].Box[j], 0, sizeof(BOX));
     }
  }

  //--------------------------------------------------------------------
  // SOLO PER I PIENI
  // Se si è trattato di un primo inserimento assegno data/ora al
  // magazzino
  //--------------------------------------------------------------------
  if(mag.Stato==MG_ST_VUOTI || mag.Stato==MG_ST_FREE) mag.Time=0;
  else{
     if(primo_inserimento==true){
        mag.Time=time(NULL);
        //----------------------------------------------------------------------
        // Assegno a magazzino data/ora box solo se valida
        //----------------------------------------------------------------------
        if(TimeBox>0) mag.Time=TimeBox;
        //----------------------------------------------------------------------
        // SPOSTAMENTO PALLET CON RITORNO
        // in caso di gestione spostamento pallet con ritorno al mag di orgine,
        // devo memorizzare che il magazzino è temporaneo.
        //----------------------------------------------------------------------
        if(AbilitaSvuotamentoManualeMag>1 && mag_prov_box>0 && mag_prov_box<=TOTMAG && mag_prov_box!=mag.Num){
           bai_ch=mac_ch=mag_ch=pos_ch=0;
           P.TestChiamataNum(AGV[num_agv]->mission.Chiamata, &mag_ch, &bai_ch, &mac_ch, &pos_ch);
           if(mag_ch>0 && mag_ch<=TOTMAG){
              //-------------------------------------------------------------
              // Autosetto il magazzino come temporaneo se il gruppo di
              // destinazione è lo stesso del magazzino sorgente
              //-------------------------------------------------------------
              if(M.m[mag_ch]->Gruppo==mag.Gruppo) mag.Tipo = MG_TEMPORANEO;
           }
        }
     }
  }

  //-------------------------------------------
  // RESET CHIAMATA TGV
  //-------------------------------------------
  AGV[num_agv]->mission.Chiamata=0;

  //-------------------------------------------
  // 12/02/2013 Incremento missione su scarichi
  // reali per conteggiare le movimentazioni
  //-------------------------------------------
  AGV[num_agv]->mission.NumMissioni++;

  //--------------------------------------------------------------------
  // salvataggio struttura TGV
  //--------------------------------------------------------------------
  if(!err){
     err = AGV[num_agv]->save_mission(all_mess);
  }

  //--------------------------------------------------------------------
  // salvataggio struttura Magazzino
  //--------------------------------------------------------------------
  if(!err){
     memcpy( M.m[mag.Num], &mag, sizeof(MAG));
     err = M.save_record_mag(mag.Num, all_mess);
  }

  //----------------------------------------------------------------
  // GESTIONE UDC
  // da stuttura software ad array UDC per aggiornamento SQL
  //
  // !!! IMPORTANTE !!!
  // dopo aver aggiornato la struttura globale del software
  //----------------------------------------------------------------
  if(GestioneUdc>0){
     // -- SET LOCAZIONI SU UDC A BORDO TGV
     if(!err){
        err = B.StructToUdcTmp(CMD_WR_SQL_UDC_SET_LOC, 0, 0, mag.Num, 0, UDC_ALL_STZ, Posizione, Piano, TypPresa, all_mess);
        if(GestioneUdc==2 && err!=0) err=0;
     }
  }
  //-----------------------------------------------------------
  // Log Traccia Movimenti
  //-----------------------------------------------------------
  if(TypPresa==N_FORCA_AB) sprintf(Coordinate,"Storage %02d Pos.%02d Layer %02d,%02d", num_mag, Posizione, Piano, Piano+1);
  else                     sprintf(Coordinate,"Storage %02d Pos.%02d Layer %02d", num_mag, Posizione, Piano);
  LogMovimenti(Operazione, Coordinate, num_agv, mag.Pos[Posizione].Box[Piano+1], mag.Pos[Posizione].Box[Piano], TypPresa, 0);

  return err;
}



