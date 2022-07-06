// ------------------------------------------------------------------------
//                              file AGV.CPP
// ------------------------------------------------------------------------
//
//-------------------------------------------
// Elenco Funzioni incluse nel file
//-------------------------------------------
//      agv(int num_agv)
// int  crea_agv()
// int  load_stato(char *all_mess)
// int  AzzeraAllarmi(char *all_mess)
// int  load_mission(char *all_mess)
// int  save_mission(char *all_mess)
// int  delete_mission(char *all_mess)
// ...
//-------------------------------------------
#include <mem.h>
#include <string.h>
#include <math.h>
#include <share.h>
#include <sys\stat.h>

#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include "Main.h"
#include "SQLDB.h"
#include "OmronETH.h"
#include "Laser.h"
#include "Remoto.h"
#include "SrvTcpAndroid.h"
#include "Variabil.h"
#include "XYNodi.h"

//------------------------------------------------------
// Funzioni   locali
//------------------------------------------------------

int VerificaZonaFuoriIngombro( int NumAgv, int NodoConsiderato, int NodoInterruzionePercorso, int AgvPerInterruzionePercorso,
                               int & EsisteZonaFuoriIngombro );

int VerificaNodoOccupato(bool RioccupaNodo, int NumAgv, int NodoConsiderato,
                                int & NodoOccupabile, int & AgvAdiacente, int & satellite_occupato, int & NodoInterruzionePercorso, int ScartaAgv, int SoloAgv );

int VerificaNodoOccupatoRP(int NumAgv, int NodoConsiderato,
                           int & NodoOccupabile, int & AgvAdiacente, int & satellite_occupato, int & NodoInterruzionePercorso );

int VerificaNodoPrenotato(int NumAgv, int NodoConsiderato,
                          int & NodoOccupabile, int & AgvAdiacente, int & satellite_occupato, int & NodoInterruzionePercorso );

int VerificaRetteParallele(int NumAgv, bool *RetteParallele, int Nodo1, int Nodo2,
                          int & NodoOccupabile, int & AgvAdiacente, int & satellite_occupato, int & NodoInterruzionePercorso );

int VerificaNodoOccupatoIngombroAgv(bool RioccupaNodo, int NumAgv, int NodoConsiderato, int &NodoOccupabile, int &AgvAdiacente, int &satellite_occupato, int &NodoInterruzionePercorso, int ScartaAgv, int SoloAgv);

int VerificaNodoPrenotatoConFuoriIng(int NumAgv, int NodoConsiderato,
                          int & NodoOccupabile, int & AgvAdiacente, int & satellite_occupato, int & NodoInterruzionePercorso );

int VerificaNodoPrenotatoIngombroAgv(int NumAgv, int NodoConsiderato, int &NodoOccupabile, int &AgvAdiacente, int &satellite_occupato, int &NodoInterruzionePercorso );

// ---------------------------
//      da GRAPHVAR.CPP
// ---------------------------
extern int Punti[][5];
extern int BloccoRot[][3];
extern int Percorso[][2];
extern int PercorsoCurvaBezier[][7];

// --------------------------------
//        agv()
// --------------------------------
// Costruttore classe AGV, setta le variabili globali
//
agv::agv(short int num_agv)
/*******************/
{
  int err =0;
  char buffer[201]="";

  agv_num = num_agv;
  sprintf(mission_name_sorg, "MISSION%d.DAT", agv_num);
  sprintf(mission_name_dest, "MISSION%d.DAT", agv_num);

  //------------------------------------------------------
  // GESTIONE_ASRV
  //------------------------------------------------------
  sprintf(ASRV_file_stato_sorg, "ASRVSTAT%d.DAT", agv_num);
  sprintf(ASRV_file_stato_sorg, "ASRVSTAT%d.DAT", agv_num);

  sprintf(ASRV_file_mission_sorg, "ASRVMISS%d.DAT", agv_num);
  sprintf(ASRV_file_mission_dest, "ASRVMISS%d.DAT", agv_num);

  //------------------------------------------------------
  // Aggiornamento della struttura missioni
  //------------------------------------------------------
  err = load_mission(buffer);
  if( err ) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);

  //------------------------------------------------------
  // GESTIONE_ASRV
  // rilettura stato da file, solo nel costruttore
  //------------------------------------------------------
  #ifdef GESTIONE_ASRV
     err = ASRV_load_file_stato(buffer);
     if( err ) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  #endif;
  //------------------------------------------------------
  // Se al TGV non è stato assegnato nessun colore vengono
  // dati quelli di default:
  //------------------------------------------------------
  if(mission.Col==clBlack){
     switch(agv_num){
        case  1: mission.Col=clAqua;              break;
        case  2: mission.Col=clYellow;            break;
        case  3: mission.Col=clFuchsia;           break;
        case  4: mission.Col=(TColor)(0x00FF8080);break;
        case  5: mission.Col=clBlue;              break;
        case  6: mission.Col=clPurple;            break;
        case  7: mission.Col=clTeal;              break;
        case  8: mission.Col=clMaroon;            break;
        case  9: mission.Col=clRed;               break;
        case 10: mission.Col=clLime;              break;
        default: mission.Col=clWhite;             break;
     }
  }

  //------------------------------------------------------
  // Rap encoder non può essere nullo
  //------------------------------------------------------
  if(mission.RapEncoder==0) mission.RapEncoder = RAP_ENCODER;

  //------------------------------------------------------
  // Aggiornamento della struttura di "Zone Mapping"
  //------------------------------------------------------
  sprintf(filemapping, "MAPPA%02d.DAT", agv_num);
  if(mission.GestioneLaser){
     err = load_zone_mapping(buffer);
     if( err ) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }

  //------------------------------------------------------
  // Forzo il Punto di riposo del TGV se non esiste
  //------------------------------------------------------
  if(mission.PosRicarica==0) mission.PosRicarica=agv_num;

  //-------------------------------------------------------
  // Aggiornamento variabile TGV di gestione percorsi
  // speciali
  //-------------------------------------------------------
  PercorsiSpeciali = 0;

  //------------------------------------------------------
  // Azzeramento memoria di TGV in errore di comunicazione
  //------------------------------------------------------
  mission.ErrCom=false;

  //------------------------------------------------------
  // Inizializzazione DM88 di stop TGV
  //------------------------------------------------------
  dm88_stop_tgv           = 0;

  //------------------------------------------------------
  // Inizializzazione DM1000 versione pollicino/allarmi
  //------------------------------------------------------
  dm1000_versione_allarmi = 0;

  //------------------------------------------------------
  // Inizializzazione cicli attesa per TGV in cambio
  // missione
  //------------------------------------------------------
  CicliAttesaPerAgvInReset = CICLI_ATTESA_PER_AGV_IN_RESET;

  //------------------------------------------------------
  // Attivo GESTIONE LASER se attiva GESTIONE_CURVE
  //------------------------------------------------------
  #ifdef GESTIONE_CURVE
     mission.GestioneLaser=true;
  #endif;

  //------------------------------------------------------
  // Stato allarmi
  //------------------------------------------------------
  allarme_interno     = 0;
  old_allarme_interno = 0;

  //------------------------------------------------------
  // Stato TGV
  //------------------------------------------------------
  sbit = &stato.s.bit;    // stato AGV
  stato.pos   = mission.OldPos;
  if(mission.GestioneLaser==true) stato.rot_laser    = mission.OldRot;
  else                            stato.rot_calamite = mission.OldRot;
  stato.start = mission.OldStart;
  stato.s.val = mission.OldComandi;
}


// ------------------------
//        crea_agv()
// ------------------------
//  In realtà resetta la missione dell'AGV
//
int agv::crea_agv()
/******************/
{
  int err=0;
  char buffer[201]="";

  err = delete_mission(buffer);
  return err;
}



// ------------------------
//    load_stato()
// ------------------------
// legge lo stato attuale dell'AGV
//
int agv::load_stato(char *all_mess)
/*********************************/
{
  int  newpos;
  int  oldpos;
  int  totrot;
  int  i, err=0;
  WORD dm[51];
  WORD NumCanali;
  bool anomalia;
  bool converti_grd;
  double grd_ly, grd_ls;
  double ComponenteX, ComponenteY, ScostamentoPuntoLogico;
  long double grd, rad, sinA, cosA;
  def_stat dati;
  def_stat old_stato;
  def_stat new_stato;
  short int altro_agv=0;
  bool nodo_di_missione, no_aggiorna_stato;

  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  // Azzero l'errore di comunicazione legato al TGV
  //---------------------------------------------------------
  if(mission.EsclPLC || break_Com[COM1]){
     mission.ErrCom=false;
     errore_comunicazione[COM1][agv_num]=0;
     //----------------------------------------------------------
     // Recupero zona su cui si trova il TGV
     //----------------------------------------------------------
     if(stato.pos>0 && stato.pos<=TOTPUNTI){
        mission.Zona = N.n[stato.pos]->zp;
        if(mission.GestioneLaser==true && AbilitaSimulazione==true) stato.g_laser=1;  // per debug
     }
     return 0;
  }

  NumCanali=12;
  if(mission.GestioneLaser) NumCanali=45;
  //------------------------------------------------------
  // 18/01/2005 Se TypCom==3 (Etherneth-Seriale) e
  // Lettura Multi PLC abilitata
  // Recupera i DM dalla precedente lettura Multi PLC
  // controllando se c'è errore
  //------------------------------------------------------
  if((OmrPlc[agv_num].TypCom==3 || OmrPlc[agv_num].TypCom==4) && AbilitaLetturaMultiPLC && !OmrEth_MultiPlcEsclusionePLC[agv_num]){
     //--------------------------------------------
     // Se c'è errore -> skip!
     //--------------------------------------------
     if(OmrEth_ErrPlc[agv_num]!=0){sprintf(all_mess, MESS[114], agv_num); return 1;}
     //--------------------------------------------
     // Dati attendibili
     //--------------------------------------------
     memcpy(&dm[0], &OmrEth_DM_TxRx[ agv_num ][0] , NumCanali*sizeof(WORD));
  }
  else{
     //---------------------------------------------
     // Lettura diretta
     // DM 88 = dm di stop TGV per caduta consensi
     // DM 89 = dm di attesa ingresso macchina
     // DM 90 = posizione attuale AGV
     // DM 91 = start missione AGV ( 0,1,2 )
     // DM 92 = bit di stato AGV   ( HR 3  )
     // DM 93 = offset impulsi da nodo ( 11.02.94 )
     // DM 94 = bit di stato AGV   ( aggiuntivo )
     // DM 95 = Volt batterie
     // DM 96 = Peso lordo
     // DM 97 = gradi calamite
     //---------------------------------------------
     err = OM.ld_canale(&dm[0], (WORD)(agv_num), "RD", 88, NumCanali, all_mess);
     if(err){sprintf(all_mess, MESS[114], agv_num); return 1;}
  }

  //------------------------------------------------------------------
  // GESTIONE_ASRV
  //------------------------------------------------------------------
  // Recupero il tipo di comunicazione riservato
  // per il PLC TGV
  //---------------------------------------------
  if(OmrPlc[agv_num].TypCom==TYPCOM_BECKHOFF){
     err = ASRV_load_plc_stato(&dm[0], all_mess);
     if(err) return 1;
  }

  //------------------------------------------------------------------
  // Lettura dei DM per lo stato della Batteria e del peso
  //------------------------------------------------------------------
  dm88_stop_tgv          = dm[0];          // DM88
  mission.VoltBatteria   = dm[7] & 0x00ff; // Volt (parte bassa)
  mission.PercCaricaBatt = dm[7]>>8;       // Percentuale carica Batteria (parte alta)
  mission.PesoLordo      = dm[8];          // Peso lordo
  mission.Zona           = 0;              // inizializzazione Zona
  //------------------------------------------------------------------
  // DM 90 - deve contenere un numero di nodo ammesso
  // DM 91 - start missione (non sono ammessi valori maggiori di "2")
  // DM 92 - stato AGV (i bit devono essere attendibili)
  //------------------------------------------------------------------
  altro_agv= 0;
  anomalia = false;
  oldpos = newpos = 0;
  no_aggiorna_stato=false;
  dati.val  = (int)(dm[6]<<16)+dm[4];
  old_stato.val = stato.s.val;
  new_stato.val = dati.val;
  totrot        = dati.bit.grd0 + dati.bit.grd90 + dati.bit.grd180 + dati.bit.grd270;
  if(dm[3]>2                   ) anomalia=true;  // bit di start >=0 e <=2
  if(totrot!=1                 ) anomalia=true;  // ammessa una rotazione per volta
  if( dm[3] &&  dati.bit.okprog) anomalia=true;  // bit di start e okprog complementari
  if(!dm[3] && !dati.bit.okprog) anomalia=true;
  if( dm[3] && !dati.bit.funz  ) anomalia=true;  // con TGVin manuale bit di start =0
  if(dm[2]>TOTPUNTI            ) anomalia=true;  // punto inesistente
  //-----------------------------------------------------------------
  // TEST_INTERNO_TECNOFERRARI
  // Non vengono fatti i controlli di anomalia sul percorso perchè
  // è modificato dal software
  //-----------------------------------------------------------------
  if(dm[3]!=0 && dati.bit.funz && mission.pstart){
     for(i=0; i<MAXPERCORSI; i++){
        //-------------------------------------
        // dm90 non appartenente alla missione
        //-------------------------------------
        if(mission.punto_prenotato[i]==0) break;
        //-------------------------------------
        // La nuova pos fa parte dei possibili
        // punti del percorso TGV
        //-------------------------------------
        if(dm[2]==mission.punto_prenotato[i]){newpos=i+1; break;}
     }
     //---------------------------------------------------
     // Verifico se il nuovo nodo letto per il TGV è
     // sucessivo alla posizione precedente altrimenti
     // lascio traccia dell'anomalia.
     //---------------------------------------------------
     for(i=0; i<MAXPERCORSI; i++){
        if(mission.punto_prenotato[i]==0) break;
        //---------------------------------------
        // La vecchia pos fa parte dei possibili
        // punti del percorso TGV
        //---------------------------------------
        if(mission.punto_prenotato[i]==stato.pos){oldpos=i+1; break;}
     }
     if(!newpos || newpos<oldpos){
        sprintf(all_mess, MESS[345], dm[2], stato.pos);
        P.error_trasmit(agv_num, all_mess);
        anomalia=true;
     }
  }
  //----------------------------------------------------------
  // 21/07/2011 TGV IN AUTOMATICO SU TRAGITTO GIA' OCCUPATO
  // Se la navetta viene messa in automatico su un percorso
  // già impegnato da un altro TGV visualizzo lo stato di
  // manuale e alzo l'allarme su PLC
  //
  // AL-01/08/2017 Eliminato controllo su passaggio in
  //               automatico e lasciato più in generale
  //               per quando un TGV si trova sul un percorso
  //               occupato da altra navetta
  //----------------------------------------------------------
//if(dm[3]==0 && dati.bit.funz && !stato.s.bit.funz && dm[2]>0 && dm[2]<=TOTPUNTI){
  if(dati.bit.funz && !dati.bit.allarme && dm[2]>0 && dm[2]<=TOTPUNTI){
     if(N.nodo_busy[dm[2]]>0 && N.nodo_busy[dm[2]]<=MAXAGV && N.nodo_busy[dm[2]]!=agv_num){
        altro_agv = N.nodo_busy[dm[2]];
        if(AGV[altro_agv]->test_mission_on()){
           allarme_interno = ALL_INT_START_DA_OCC;
           no_aggiorna_stato=true;
        }
     }
  }
  //---------------------------------------------------------------
  // 22/05/2017 TGV IN AUTOMATICO SU NODO DESTINAZIONE di ALTROAGV
  // Se la navetta viene messa in automatico su un Nodo
  // già destinazione di un altro TGV visualizzo lo stato di
  // manuale e alzo l'allarme su PLC
  //---------------------------------------------------------------
  if(dm[3]==0 && dati.bit.funz && !stato.s.bit.funz && dm[2]>0 && dm[2]<=TOTPUNTI){
     for(altro_agv=1; altro_agv<=MAXAGV; altro_agv++){
        if(altro_agv==agv_num) continue;
        if(AGV[altro_agv]->mission.pend==dm[2]){
           if(AGV[altro_agv]->test_mission_on()){
              if(AGV[altro_agv]->stato.s.bit.funz){
                 allarme_interno = ALL_INT_START_DA_OCC;
                 no_aggiorna_stato=true;
              }
           }
        }
     }
  }
  //----------------------------------------------------------
  // Se è presente un'anomalia mantengo i valori vecchi
  //----------------------------------------------------------
  if(!anomalia){
     // aggiornamento DM91 (stato missione)
     stato.start = (short int)(dm[3]);
     // aggiornamento DM92 (bit di stato)
     if(no_aggiorna_stato==false){
      //stato.s.val = (short int)(dm[4]);
        stato.s.val = (int)(dm[6]<<16)+dm[4];
     }
     //----------------------------------------------------------
     // 09/12/2009 OCCUPAZIONE NODI IN MANUALE
     // Con la navetta in manuale occupo tutti i nodi
     // su cui mi sposto e libero le predenti posizioni
     //----------------------------------------------------------
     if(!stato.s.bit.funz && stato.start==0 && dm[2]>0 && dm[2]<=MAXPUNTI){
        if(dm[2]!=stato.pos){
           if(N.nodo_busy[stato.pos]==agv_num){
              //-------------------------------------------------
              // 21/07/2011 LIBERAZIONE NODO VECCHIO SOLO SE
              //            NON APPARTIENE ALLA STRUTTURA PUNTI
              //-------------------------------------------------
            //N.nodo_busy[stato.pos]=0;        // old posizione
              nodo_di_missione=false;
              for(i=0; i<MAXPERCORSI; i++){
                 if(mission.punto[i]==0        ) break;
                 if(mission.punto[i]!=stato.pos) continue;
                 nodo_di_missione=true;
                 break;
              }
              // libero il nodo vecchio se non appartiene alla missione originale
              if(nodo_di_missione==false){
                 N.nodo_busy[stato.pos]=0;     // old posizione
              }
           }
           //-------------------------------
           // Occupazione nuovo nodo solo
           // se libero
           //-------------------------------
           if(N.nodo_busy[dm[2]]==0) N.nodo_busy[dm[2]]=agv_num;  // new posizione
           //-------------------------------
           // Salvataggio su file
           //-------------------------------
           err = N.save_nodo_busy(all_mess);
           if(err) return 1;
        }
     }
     //----------------------------------------------------------
     // ASSEGNAZIONE NUOVA POSIZIONE TGV
     //----------------------------------------------------------
     // aggiornamento DM90 (posizione TGV)
     stato.pos = (short int)(dm[2]);
     //----------------------------------------------------------
     // 14/04/2014 OCCUPAZIONE NODI IN AUTOMATICO
     // Con la navetta in automatico e senza missione in corso
     // occupo il nodo su cui si trova il TGV liberando.
     //
     // IMPORTANTE: dopo aver assegnato la nuova posizione al
     // TGV perchè la funzione free_all_nodi() deve vedere
     // il TGV già in posizione
     //----------------------------------------------------------
     if(stato.s.bit.funz && stato.start==0 && stato.pos>0 && stato.pos<=TOTPUNTI && N.nodo_busy[stato.pos]==0){
        free_all_nodi(all_mess);
     }
     //----------------------------------------------------------
     // Recupero zona su cui si trova il TGV
     //----------------------------------------------------------
     if(stato.pos>0 && stato.pos<=MAXPUNTI){
        mission.Zona = N.n[stato.pos]->zp;
     }
     //----------------------------------------------------------
     // con la missione in corso e nessuna anomalia rilevata
     // azzero la variabile di allarme_interno
     //----------------------------------------------------------
     if(allarme_interno==ALL_INT_DATI_ANOMALI){
        allarme_interno     = 0;
        old_allarme_interno = 0;
     }
     //----------------------------------------------------------
     // con l'aggiornamento dei bit di stato da PLC abbasso
     // anche il relativo allarme interno.
     //----------------------------------------------------------
     if(allarme_interno==ALL_INT_START_DA_OCC && no_aggiorna_stato==false){
        allarme_interno     = 0;
        old_allarme_interno = 0;
     }
     //----------------------------------------------------------
     // AL-13/02 GESTIONE CONTROLLO ALLARME INTERNO CAR/SCAR TGV
     // Attivo tutte le volte che non c'è missione attiva per
     // garanzia di segnalazione all'operatore
     //----------------------------------------------------------
     if(stato.start==0 && (allarme_interno==0 || allarme_interno==ALL_INT_RESET_DATI || allarme_interno==ALL_INT_SCAR_MANUALE)){
        allarme_interno=0;
        //--------------------------------------------------
        // (1) GESTIONE FORCA DOPPIA  CON CARICO DOPPIO
        // (2) GESTIONE FORCA SINGOLA CON CARICO DOPPIO
        //--------------------------------------------------
        if(GestioneDoppiaForcaTGV!=2){
           //----------------------------------------------------
           // Errore mancanza cestone scaricato manualmente
           //----------------------------------------------------
           if(!stato.s.bit.carA && mission.PresCEST_A!=LIBERA) allarme_interno = ALL_INT_RESET_DATI;
           if(!stato.s.bit.carB && mission.PresCEST_B!=LIBERA) allarme_interno = ALL_INT_RESET_DATI;
           //----------------------------------------------------
           // Errore caricamento manuale di un cestone
           //----------------------------------------------------
           if(stato.s.bit.carA && mission.PresCEST_A==LIBERA ) allarme_interno = ALL_INT_SCAR_MANUALE;
           if(stato.s.bit.carB && mission.PresCEST_B==LIBERA ) allarme_interno = ALL_INT_SCAR_MANUALE;
        }
        else{
           //----------------------------------------------------
           // Errore mancanza cestone scaricato manualmente
           //----------------------------------------------------
           if(!stato.s.bit.carB && (mission.PresCEST_B!=LIBERA || mission.PresCEST_A!=LIBERA)) allarme_interno = ALL_INT_RESET_DATI;
           //----------------------------------------------------
           // Errore caricamento manuale di un cestone
           //----------------------------------------------------
           if( stato.s.bit.carB && (mission.PresCEST_B==LIBERA && mission.PresCEST_A==LIBERA)) allarme_interno = ALL_INT_SCAR_MANUALE;
        }
     }
  }
  else{
     sprintf(all_mess, MESS[346], dm[2], dm[3], dm[4]);
     P.error_trasmit(agv_num, all_mess);
     allarme_interno = ALL_INT_DATI_ANOMALI;                // mando il TGV in allarme interno
  }
  //------------- DM 93 - offset_da_nodo
  stato.offset_da_nodo = (short int)(dm[5]);
  //------------- DM 97 - offset_da_nodo
  stato.rot_calamite  = (short int)(dm[9]);
  //------------- DM 98 - reset missione da PC
  //------------- DM 99 - puntatore passi programma
  stato.ptr_step     = (short int)(dm[11]);
  //-----------------------------------------------
  // Gestione riservata ai TGV con guida laser
  //-----------------------------------------------
  if(mission.GestioneLaser){
     grd_ly=grd_ls=0;
     converti_grd=false;
     if(stato.s.bit.grd0  ){grd_ls=0;   converti_grd=true;}
     if(stato.s.bit.grd90 ){grd_ls=90;  converti_grd=true;}
     if(stato.s.bit.grd180){grd_ls=180; converti_grd=true;}
     if(stato.s.bit.grd270){grd_ls=270; converti_grd=true;}
     if(converti_grd){
        convertiGRD_laser_to_layout(grd_ls, &grd_ly);
        //-------------------------------------
        // Conversione delle rotazioni
        //-------------------------------------
        stato.s.bit.grd0=0;
        stato.s.bit.grd90=0;
        stato.s.bit.grd180=0;
        stato.s.bit.grd270=0;
        if(grd_ly==  0) stato.s.bit.grd0=1;
        if(grd_ly== 90) stato.s.bit.grd90=1;
        if(grd_ly==180) stato.s.bit.grd180=1;
        if(grd_ly==270) stato.s.bit.grd270=1;
     }
     //------------- DM 100/101 - Coordinata X passata dal laser
     stato.x_laser   = (int)(dm[13]<<16)+dm[12];
     //------------- DM 102/103 - Coordinata Y passata dal laser
     stato.y_laser   = (int)(dm[15]<<16)+dm[14];
     //------------- DM 104 - Rotazione navetta passata dal laser
     stato.rot_laser    = (short int)(dm[16]);
     //------------- DM 132 - Rotazione navetta passata dal laser
     stato.g_laser   = (short int)(dm[44]);
     //-----------------------------------------------------------------------
     // Calcolo dello scostamento tra "PUNTO LOGICO" e testa laser
     //-----------------------------------------------------------------------
     ScostamentoPuntoLogico = 1160;
     ScostamentoPuntoLogico = zone_mapping.ScostamentoPuntoLogico;
     grd = (long double)(stato.rot_laser);
     rad = DegToRad(grd);
     SinCos(rad, sinA, cosA);
     ComponenteX = ScostamentoPuntoLogico*sinA;
     ComponenteY = ScostamentoPuntoLogico*cosA;
     stato.x_laser = stato.x_laser - (int) ComponenteX;
     stato.y_laser = stato.y_laser + (int) ComponenteY;
  }

  return err;
}


// ---------------------------------
//    write_allarmi_interni_a_plc()
// ----------------------------------
//  Scrittura ALLARMI INTERNI A PLC PER FAR SUONARE LA SIRENA
//
int agv::write_allarmi_interni_a_plc(char *all_mess)
//*****************************************
{
  int  err;
  WORD dm[3];
  WORD NumPlc=0;
  WORD NumCanali=1;
  WORD CanaleIniziale=4999;

  NumPlc=agv_num;
  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------
  // Comunicazione con i TGV esclusa
  //---------------------------------------------
  if(break_Com[COM1]==true) return 0;
  //---------------------------------------------
  // Comunicazione con il TGV esclusa
  //---------------------------------------------
  if(mission.EsclPLC==true) return 0;
  //---------------------------------------------
  // TGV NON IN ALLARME INTERNO --> skip!
  //---------------------------------------------
  if(allarme_interno==0 && stato.s.bit.funz) return 0;
  //---------------------------------------------
  // AL-FPT 18/08/2014 No scrittura error com
  //---------------------------------------------
  if(allarme_interno==ALL_INT_ERR_COMUNIC) return 0;
  //---------------------------------------------
  // Scrittura a PLC TGV del DM4999
  //
  // AL-FPT 18/08/2014 scrivo nel d4999
  // direttamente il numero dell'allarme
  //---------------------------------------------
  dm[0] = allarme_interno;
  err = OM.wr_canale(&dm[0], NumPlc, "WD", CanaleIniziale, NumCanali, all_mess);

  return err;
}

// --------------------------------
//  stop_TGV_per_caduta_consensi()
// --------------------------------
// scrive il DM88 per fermare i TGV in ingresso su macchina
// che tolgono l'OK ACCESSO
// bit00 - macchina/baia
// bit01 - porte
// ...
// bit14 - punto intermedio in prossimità di TGV in allarme.
// bit15 - Incendio in corso.
//
int agv::stop_TGV_per_caduta_consensi(char *all_mess)
/*********************************/
{
  int err;
//bool Ch_ON;
  bool stop_per_porta, curva;
  bool tgv_su_nodo, watch_dog;
  bool tgv_su_nodo_precente;
  bool stop_per_altro_tgv_in_allarme;
  bool no_controllo_punto_precedente;
  short int i, j, k, Port, count;
  short int sotto_porta, count_da_pint;
  short int altro_agv, succ_sotto_porta, grd_rot;
  short int p1, p2, num_baia, num_mac, num_stz, num_cb;
  ingombroagv *IngombroAgv1, *IngombroAgv2;
  short int indice_pos, dirrot_altroagv, succ_altroagv, punto_altroagv;
  unsigned short int puntoAgv, prec_puntoAgv;
  short int rot1, rot2;
  bool agv_rot_180, altro_rot_180;
  int x1, y1, x2, y2, dir;
  double Dist, DistX, DistY;
  WORD dm[3];
  WORD CanaleIniziale=88;

  err=0;
  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------------------
  // Bit13 - WATCH-DOG di comunicazione PC <--> PLC
  //---------------------------------------------------------
  watch_dog = (bool)(TestBit((char *)&dm88_stop_tgv, 13));
  if(watch_dog==true) ClrBit((char *)&dm88_stop_tgv, 13);
  else                SetBit((char *)&dm88_stop_tgv, 13);
  //---------------------------------------------------------
  // Appoggio DM GLOBALE su DM LOCALE
  //---------------------------------------------------------
  dm[0] = dm88_stop_tgv;
  //---------------------------------------------------------
  // TGV SU BAIA DI CAMBIO BATTERIE AUTOMATICO
  //---------------------------------------------------------
  err=0;
  P.test_punto_presente_baia(all_mess, stato.pos, &num_baia);
  //---------------------------------------------------------
  // Mi assicuro che la navetta abbia la missione in corso
  //---------------------------------------------------------
  if(!test_mission_on() && !IncendioInCorso){
     //--------------------------------------------------------
     // Se la navetta non ha la missione in corso azzero il DM
     // per sicurezza
     //--------------------------------------------------------
     if((OmrPlc[agv_num].TypCom!=3 && OmrPlc[agv_num].TypCom!=4) || !AbilitaLetturaMultiPLC){
        if(mission.EsclPLC==false && break_Com[COM1]==false){
           //--------------------------------------------------------
           // TGV senza missione in corso, gestione solo WATCH-DOG
           //--------------------------------------------------------
           err = OM.wr_canale(&dm[0], (WORD)(agv_num), "WD", CanaleIniziale,  1, all_mess);
           if(err) sprintf(all_mess, "Error in set bit per stop TGV%02d on DM88...", agv_num);
        }
     }
  }
  //---------------------------------------------------------
  // Se il TGV ha la missione in corso rileggo il DM88
  // prima di modificarne lo stato
  //---------------------------------------------------------
  dm[0] = dm88_stop_tgv;
  if(!err && (test_mission_on() || IncendioInCorso==true)){
     //---------------------------------------------------------
     // Verifico a che stuttura appartiene la destinazione
     //---------------------------------------------------------
     tgv_su_nodo=false;
     tgv_su_nodo_precente=false;
     no_controllo_punto_precedente=false;
     if(test_mission_on()){
        num_baia=0; num_mac=0; num_stz=0; num_cb=0;
        P.test_punto_presente_mac(all_mess, mission.pend, &num_mac, &num_stz);
        if(num_mac==0) P.test_punto_presente_baia(all_mess, mission.pend, &num_baia);
        if(num_baia==0 && num_mac==0) P.test_punto_presente_cbat(all_mess, mission.pend, &num_cb);
        // MACCHINE (Chiamata attiva o TGV non in ingombro o Macchina in ok accesso = NO BLOCCO)
        if(num_mac>0 && num_mac<=TOTCHIAMATE){
         //Ch_ON=P.ch[num_mac]->Staz[num_stz].Richiesta | P.ch[num_mac]->Staz[num_stz].Espulsione;
           if(P.ch[num_mac]->Automatico==true && P.ch[num_mac]->Blocco==false){
              // AL-06/06/2017: controllo solo OK INGRESSO perchè la chiamata resta alta in alcuni casi
            //if(Ch_ON==true || P.ch[num_mac]->Staz[num_stz].OkIngresso==true || P.ch[num_mac]->Staz[num_stz].TgvInIngombro==false) num_mac=0;
              if(P.ch[num_mac]->Staz[num_stz].OkIngresso==true || P.ch[num_mac]->Staz[num_stz].TgvInIngombro==false) num_mac=0;
           }
           //----------------------------------------------
           // Comunicazione macchina --> ESCLUSA
           //----------------------------------------------
           if(P.ch[num_mac]->EsclComunic==true) num_mac=0;
        }
        // BAIE (Chiamata attiva o TGV non in ingombro o Macchina in ok accesso = NO BLOCCO)
        if(num_baia>0 && num_baia<=TOTBAIE){
           //--------------------------------------------------------
           // BAIA PRELIEVO o DEPOSITO
           // controllo condizione consensi per ingresso TGV
           //--------------------------------------------------------
           if(P.ba[num_baia]->Tipo==BA_PRELIEVO || P.ba[num_baia]->Tipo==BA_DEPOSITO){
              if(P.ba[num_baia]->Automatico==true && P.ba[num_baia]->Blocco==false){
                 if(P.ba[num_baia]->OkIng==true) num_baia=0;
              }
           }
           //----------------------------------------------
           // Comunicazione baia --> ESCLUSA
           //----------------------------------------------
           if(P.ba[num_baia]->EsclComunic==true) num_baia=0;
           if(P.ba[num_baia]->TgvIng==false    ) num_baia=0;
        }
        // CAMBIOBATTERIA (Chiamata attiva o TGV non in ingombro o Macchina in ok accesso = NO BLOCCO)
        if(num_cb>0 && num_cb<=TOTBATT){
           //--------------------------------------------------------
           // BAIA PRELIEVO o DEPOSITO
           // controllo condizione consensi per ingresso TGV
           //--------------------------------------------------------
           if(N.punti_notevoli[mission.pend]==true){
              if(P.cb[num_cb]->Automatico==true && P.cb[num_cb]->Blocco==false){
                 if(P.cb[num_cb]->OkIng==true || P.cb[num_cb]->TgvIng==false) num_cb=0;
              }
           }
           //----------------------------------------------
           // Comunicazione baia --> ESCLUSA
           //----------------------------------------------
           if(P.cb[num_cb]->EsclComunic==true) num_cb=0;
        }
        //---------------------------------------------------------
        // STOP PER CADUTA CONSENSI MACCHINE  (DM88 - bit0)
        // Setto il bit0 del DM88 se in ingresso a una macchina
        // cade la chiamata
        //---------------------------------------------------------
        if(num_mac!=0 || num_baia!=0 || num_cb!=0){
           for(i=MAXPERCORSI-2; i>0; i--){
              if(no_controllo_punto_precedente==true ) break;
              if(mission.punto[i+1]==mission.pend    ) break;
              if(mission.punto[i]==0                 ) continue;
              if(stato.pos!=mission.punto[i-1]       ) continue; //controllo di essere sul nodo precedente alla destinazione
              tgv_su_nodo_precente=true;
              break;
           }
           if(stato.pos==mission.pend)   tgv_su_nodo=true;
        }
     }
     //---------------------------------------------------------
     // Set/Reset in base alla posizione
     //---------------------------------------------------------
     if(tgv_su_nodo_precente==true || tgv_su_nodo==true) SetBit((char *) &dm[0], 0);
     else                                                ClrBit((char *) &dm[0], 0);
     //---------------------------------------------------------
     // STOP PER CADUTA CONSENSI PORTE  (DM88 - bit1)
     //---------------------------------------------------------
     stop_per_porta=false;
     for(Port=1; Port<=TOTPORTE; Port++){
        //----------------------------------------------------
        // OkAccesso alto --> skip
        //----------------------------------------------------
        if(PorteImp[Port].OkIngresso          ) continue;
        //----------------------------------------------------
        // Porta esclusa da PLC
        // (attraversabile)
        //----------------------------------------------------
        if(PorteImp[Port].EsclusionePLC==true ) continue;
        //----------------------------------------------------
        // Se il TGV non ? gi? in transito --> skip!
        //----------------------------------------------------
        if(mission.PorInIngombro[Port]==false) continue;
        //----------------------------------------------------
        // Recupero il numero del TGV sul nodo della porta
        //----------------------------------------------------
        for(i=0; i<11; i++){
           if(PorteImp[Port].Punto[i]<1        ) continue;
           if(PorteImp[Port].Punto[i]>MAXPUNTI ) continue;
           //----------------------------------------------------
           // Controllo se uno dei nodi della porta ? occupato
           // da TGV considerato
           //----------------------------------------------------
           if(N.nodo_busy[PorteImp[Port].Punto[i]]!=agv_num) continue;
           //----------------------------------------------------
           // Il TGV deve gi? essere sul nodo della porta
           //----------------------------------------------------
           if(stato.pos!=PorteImp[Port].Punto[i]           ) continue;
           //----------------------------------------------------
           // SOLO SE TGV IN ATTRAVERSAMENTO
           //     (Pugnaghi 28/04/2011)
           // anche il successivo deve essere in ingombro
           // alla porta
           //----------------------------------------------------
           succ_sotto_porta=0;
           for(j=0; j<MAXPERCORSI; j++){
              if(mission.punto[j]==0) break;
              if(mission.punto[j]!=stato.pos) continue;
              break;
           }
           if(mission.punto[j]==stato.pos && mission.punto[j+1]!=0){
              //-------------------------------------------------
              // controllo che anche il succ appartenga ai nodi
              // della stessa porta
              //-------------------------------------------------
              for(k=0; k<11; k++){
                 if(mission.punto[j+1]!=PorteImp[Port].Punto[k]) continue;
                 succ_sotto_porta = PorteImp[Port].Punto[k];
                 break;
              }
           }
           //----------------------------------------------------
           // Stop con TGV con successivo = porta
           //----------------------------------------------------
           if(succ_sotto_porta!=0){
              stop_per_porta=true;
           }
           //----------------------------------------------------
           // E' sufficente che un TGV sia in ingombro a una
           // qualunque porta su cui cade l'ok accesso per
           // fermare la navetta
           //
           // 28/04/2011
           // Pugnaghi ha chiesto la fermata solo dal nodo prima
           //
           //----------------------------------------------------
           //stop_per_porta=true;
           break;
        }
        if(stop_per_porta==true) break;
     }
     //---------------------------------------------------------
     // In caso di Incendio non sono ammesse fermate sotto
     // le porte
     //---------------------------------------------------------
     if(IncendioInCorso==false){
        if(stop_per_porta==true ) SetBit((char *) &dm[0], 1);
        if(stop_per_porta==false) ClrBit((char *) &dm[0], 1);
     }
     else{
        ClrBit((char *) &dm[0], 1);
     }
     //---------------------------------------------------------
     // STOP PER NODO INTERMEDIO PROSSIMO A TGV IN ALLARME
     // Questo bit serve per gestiere la riattivazione
     // dell'area di rallentamento se in prossimità c'è
     // un TGV in allarme o in manuale
     //---------------------------------------------------------
     if(test_mission_on()){
        //--------------------------------------------------------------
        // NOTA: se TGV già fermo su proprio punto intermedio
        //       evito di settare il bit per dare l'allarme e
        //       fermarlo con il PLC
        //--------------------------------------------------------------
        if(mission.pintend==stato.pos) stop_per_altro_tgv_in_allarme=false;
        else{
           stop_per_altro_tgv_in_allarme=false;
           //--------------------------------------------------------------
           // 1. ALTRO TGV IN MANUALE O IN ALLARME PER NODO IMPEGNATO
           //--------------------------------------------------------------
           for(j=1; j<=MAXAGV; j++){
              if(j==agv_num                ) continue;
              if(AGV[j]->stato.pos<1       ) continue;
              if(AGV[j]->stato.pos>TOTPUNTI) continue;
              //---------------------------------------------
              // Punti terminali --> continue
              //---------------------------------------------
              if(N.punti_notevoli[AGV[j]->stato.pos]==true &&
                 N.pt_in_ingombro[AGV[j]->stato.pos]==false) continue;
              //---------------------------------------------
              // SOLO PER TGV IN MANUALE O SU PERCORSI BUSY
              //---------------------------------------------
              if(!AGV[j]->stato.s.bit.funz || AGV[j]->allarme_interno==ALL_INT_START_DA_OCC){
                 //---------------------------------------------
                 // Verifico distanza
                 //---------------------------------------------
                 p1 = stato.pos;
                 p2 = AGV[j]->stato.pos;
                 x1 = Punti[ p1 ][0];
                 y1 = Punti[ p1 ][1];
                 x2 = Punti[ p2 ][0];
                 y2 = Punti[ p2 ][1];
                 DistX = abs(y1-y2);
                 DistY = abs(x1-x2);
                 if(DistX>30000 || DistY>30000) continue;
                 Dist = (double)(sqrt_algo((DistX*DistX)+(DistY*DistY)));
                 //---------------------------------------------
                 // Se la distanza mi garantisce la rotazione
                 // non ? necessario settare il bit
                 //---------------------------------------------
                 if(Dist<DIST_MIN_ROT_DOPPIA_MAX){
                    //---------------------------------------------
                    // TGV in allarme troppo vicino
                    //---------------------------------------------
                    stop_per_altro_tgv_in_allarme = true;
                    break;
                 }
                 //---------------------------------------------
                 // RIPARTENZA DA PINT
                 // in caso di ripartenza da PINT è nuovo
                 // PINT è < MIN_NODI_FUORI_INGOMBRO dalla
                 // posizione, non lo faccio nemmeno partire
                 //---------------------------------------------
                 if(AgvBloccati[agv_num]==j){
                    if(stato.pos==mission.pintstart && mission.pintstart!=mission.pintend && mission.pintend!=mission.pend){
                       count=0;
                       for(k=0; k<=MAXPERCORSI; k++){
                          if(mission.punto_prenotato[k]==0              ) break;
                          if(mission.punto_prenotato[k]==mission.pintend) break;
                          count++;
                       }
                       //---------------------------------------------
                       // Ripartenza ingiustificata per percorrere
                       // pochi nodi
                       //---------------------------------------------
                       if(count<=MIN_NODI_FUORI_INGOMBRO){
                          stop_per_altro_tgv_in_allarme = true;
                          break;
                       }
                    }
                 }
              }
           }
           //--------------------------------------------------------------
           // 2. TGV IN MANUALE O IN ALLARME SU TRAGITTO ALTRO TGV
           //--------------------------------------------------------------
           for(i=1; i<MAXPERCORSI; i++){
              if(stop_per_altro_tgv_in_allarme==true              ) break;
              if(mission.punto_prenotato[i]<1                     ) break;
              if(mission.punto_prenotato[i]>TOTPUNTI              ) break;
              if(N.nodo_busy[mission.punto_prenotato[i]]<1        ) continue;
              if(N.nodo_busy[mission.punto_prenotato[i]]>MAXAGV   ) continue;
              if(N.nodo_busy[mission.punto_prenotato[i]]==agv_num ) continue;
              //--------------------------------------------
              // Verifico se l'altro TGV ? in manuale o
              // in allarme
              //--------------------------------------------
              altro_agv = N.nodo_busy[mission.punto_prenotato[i]];
              if(!AGV[altro_agv]->stato.s.bit.funz || AGV[altro_agv]->allarme_interno==ALL_INT_START_DA_OCC){
                 //--------------------------------------------
                 // Verifico la distanza tra il nodo occupato
                 // da TGV in allarme e il pint del TGV
                 // considerato
                 //--------------------------------------------
                 Dist=DistX=DistY=0;
                 p1 = mission.punto_prenotato[i];
                 p2 = mission.pintend;
                 x1 = Punti[ p1 ][0];
                 y1 = Punti[ p1 ][1];
                 x2 = Punti[ p2 ][0];
                 y2 = Punti[ p2 ][1];
                 DistX = abs(y1-y2);
                 DistY = abs(x1-x2);
                 if(DistX>30000 || DistY>30000) continue;
                 Dist = (double)(sqrt_algo((DistX*DistX)+(DistY*DistY)));
                 //--------------------------------------------
                 // Se la distanza mi garantisce la rotazione
                 // non ? necessario settare il bit
                 //--------------------------------------------
                 if(Dist>=DIST_MIN_ROT_DOPPIA_MAX) continue;
                 //--------------------------------------------
                 // Stop per altro TGV fermo
                 //--------------------------------------------
                 stop_per_altro_tgv_in_allarme = true;
                 break;
              }
           }
           //--------------------------------------------------------------
           // 3. ING. INGOMBRO AGV
           //--------------------------------------------------------------
           if(AbilitaGestioneIngombroCurvePerStopTGV>0){
              IngombroAgv1 = new ingombroagv(agv_num);
              if(!ModificaCoordinateNodi){
                 IngombroAgvGrafico[agv_num]->AzzeraIngombroAgv();
                 FormXYNodi->CurvaPerIngombro[agv_num]= 999999;
              }
              for(i=1; i<MAXPERCORSI; i++){
                 puntoAgv=prec_puntoAgv=0;
                 if(stop_per_altro_tgv_in_allarme==true    ) break;
                 if(mission.punto[i]<1                     ) break;
                 if(mission.punto[i]>TOTPUNTI              ) break;
                 if(i<2) continue;
                 if(N.nodo_busy[mission.punto[i]]!=agv_num ) continue;
                 puntoAgv=mission.punto[i];
                 prec_puntoAgv=mission.punto[i-1];
                 if(puntoAgv<1      || puntoAgv>TOTPUNTI       ) continue;
                 if(prec_puntoAgv<1 || prec_puntoAgv>TOTPUNTI  ) continue;
                 if(N.nodo_busy[prec_puntoAgv]!=agv_num        ) continue;
                 curva=true;
                 dir=mission.dati_perc[i-1].dir;
                 if(mission.dati_perc[i-1].rot<=3 ) curva=false;
                 if(mission.dati_perc[i-1].rot>=40) curva=false;
                 //--------------------------------------------------------------
                 // ROTAZIONI DA FERMO
                 //--------------------------------------------------------------
                 agv_rot_180=false;
                 rot1=mission.dati_perc[i].rot;
                 rot2=mission.dati_perc[i-1].rot;
                 if(rot1<4 && rot2<4 && rot1!=rot2){
                    if(rot1==0 && rot2==2) agv_rot_180=true;
                    if(rot1==1 && rot2==3) agv_rot_180=true;
                    if(rot1==2 && rot2==0) agv_rot_180=true;
                    if(rot1==3 && rot2==1) agv_rot_180=true;
                 }
                 //--------------------------------------------------------------
                 // INGOMBRO AGV CONSIDERATO
                 //--------------------------------------------------------------
                 IngombroAgv1->AzzeraIngombroAgv();
                 IngombroAgv1->SettaIngombroSuTratto(prec_puntoAgv, puntoAgv, dir, all_mess);
                 //--------------------------------------------------------------
                 // Controllo interferenze con altri eventuali agv
                 //--------------------------------------------------------------
                 for(j=1; j<=MAXAGV; j++){
                    if(j==agv_num                ) continue;
                    if(AGV[j]->stato.pos<1       ) continue;
                    if(AGV[j]->stato.pos>TOTPUNTI) continue;
                    //---------------------------------------------
                    // Punti terminali --> continue
                    //---------------------------------------------
                    if(N.punti_notevoli[AGV[j]->stato.pos]==true &&
                       N.pt_in_ingombro[AGV[j]->stato.pos]==false) continue;
                    //---------------------------------------------
                    // Ingombro 2 tgv
                    //---------------------------------------------
                    altro_rot_180=false;
                    IngombroAgv2 = new ingombroagv(j);
                    //-----------------------------------------------------------------------------------
                    // AL-19/06/2020 - INGOMBRO AGV PER GESTIONE ROTAZIONE CALAMITE
                    //-----------------------------------------------------------------------------------
                    grd_rot=AGV[j]->stato.rot_calamite;
                    if(AGV[j]->mission.GestioneLaser==true) grd_rot=AGV[j]->stato.rot_laser;
                    // ALTRO TGV FERMO SUL NODO
                    if(!AGV[j]->mission.pstart || AGV[j]->mission.pintend==AGV[j]->stato.pos || AGV[j]->mission.pend==AGV[j]->stato.pos){
                       IngombroAgv2->AzzeraIngombroAgv();
                       IngombroAgv2->SettaIngombroSuTratto(AGV[j]->stato.pos, AGV[j]->stato.pos, grd_rot, all_mess);
                    }
                    // ALTRO AGV IN MOVIMENTO
                    if(AGV[j]->mission.pstart && AGV[j]->mission.pintend!=AGV[j]->stato.pos){
                       indice_pos=0;
                       for(k=0; k<MAXPERCORSI; k++){
                          if(AGV[j]->mission.punto[k]<1                 ) break;
                          if(AGV[j]->mission.punto[k]>TOTPUNTI          ) break;
                          if(AGV[j]->mission.punto[k]!=AGV[j]->stato.pos) continue;
                          indice_pos=k;
                          break;
                       }
                       for(k=indice_pos; k<MAXPERCORSI; k++){
                          if(AGV[j]->mission.punto[k+1]<1        ) break;
                          if(AGV[j]->mission.punto[k+1]>TOTPUNTI ) break;
                          //----------------------------------------
                          // Memorizzo nodo precendente
                          //----------------------------------------
                          punto_altroagv=AGV[j]->mission.punto[k];
                          dirrot_altroagv=AGV[j]->mission.dati_perc[k].dir;
                          succ_altroagv=AGV[j]->mission.punto[k+1];
                          IngombroAgv2->AzzeraIngombroAgv();
                          IngombroAgv2->SettaIngombroSuTratto(punto_altroagv, succ_altroagv, dirrot_altroagv, all_mess);
                          //--------------------------------------------
                          // ROTAZIONI 180 DA FERMO
                          //--------------------------------------------
                          if(indice_pos>0){
                             rot1 = AGV[j]->mission.dati_perc[k  ].rot;
                             rot2 = AGV[j]->mission.dati_perc[k-1].rot;
                             if(rot1<4 && rot2<4 && rot1!=rot2){
                                if(rot1==0 && rot2==2) altro_rot_180=true;
                                if(rot1==1 && rot2==3) altro_rot_180=true;
                                if(rot1==2 && rot2==0) altro_rot_180=true;
                                if(rot1==3 && rot2==1) altro_rot_180=true;
                             }
                          }
                          break;
                       }
                    }
                    //-------------------------------------------------
                    // VERIFICA INTERSEZIONE
                    //-------------------------------------------------
                    if(IngombroAgv1->VerificaIntersezioneAgv(IngombroAgv2, all_mess) || mission.pintend==AGV[j]->stato.pos){
                       //-------------------------------------------------
                       // Esiste intersezione
                       //-------------------------------------------------
                       if(AbilitaGestioneIngombroCurvePerStopTGV>1) stop_per_altro_tgv_in_allarme=true;
                       else{
                          // AL-27/03/2019 !!! DA TESTARE IN ALTERNATIVO AL SET DIRETTO !!!
                          // stop_per_altro_tgv_in_allarme=true;
                          //----------------------------------------------
                          // FERMO PER RIPARTENZA
                          // Se il secondo AGV è fermo per ripartenza
                          // devo fermare la navetta che sta arrivando
                          // immediatamente per lasciare anche più spazio
                          // in caso di ripartenza dal manuale
                          //----------------------------------------------
                          if(!AGV[j]->test_mission_on() || AGV[j]->stato.s.bit.allarme || !AGV[j]->stato.s.bit.funz || stato.pos==prec_puntoAgv){
                             stop_per_altro_tgv_in_allarme=true;
                          }
                          //----------------------------------------------
                          // TGV BLOCCANTE IN MOVIMENTO
                          // Se il TGV bloccante è in movimento e sulla
                          // posizione successiva non è in ingombro,
                          // fermo la navetta solo all'inizio del tratto
                          // incriminato
                          //----------------------------------------------
                          else{
                           //stop_per_altro_tgv_in_allarme=true;
                             stop_per_altro_tgv_in_allarme=false;
                             //---------------------------------------------
                             // Verifico distanza
                             //---------------------------------------------
                             p1 = stato.pos;
                             p2 = AGV[j]->stato.pos;
                             x1 = Punti[ p1 ][0];
                             y1 = Punti[ p1 ][1];
                             x2 = Punti[ p2 ][0];
                             y2 = Punti[ p2 ][1];
                             DistX = abs(y1-y2);
                             DistY = abs(x1-x2);
                             if(DistX>30000 || DistY>30000) continue;
                             Dist = (double)(sqrt_algo((DistX*DistX)+(DistY*DistY)));
                             //---------------------------------------------
                             // Se la distanza  NON garantisce la rotazione
                             // doppia massma setto il bit
                             //---------------------------------------------
                             if(Dist<DIST_MIN_ROT_DOPPIA_MAX) stop_per_altro_tgv_in_allarme=true;
                             else{
                                //---------------------------------------------
                                // Il TGV si deve fermare al primo nodo
                                // utile che garantisca la rotazione doppia
                                //---------------------------------------------
                                for(k=0;k<MAXPERCORSI;k++){
                                   if(mission.punto[k]!=stato.pos               ) continue;
                                   if(mission.punto[k+1]<1                      ) break;
                                   if(mission.punto[k+1]>TOTPUNTI               ) break;
                                   if(N.nodo_busy[mission.punto[k+1]]!=agv_num  ) break;
                                   if(mission.punto[k+1]==mission.pintend       ) break;
                                   p1 = mission.punto[k+1];
                                   x1 = Punti[ p1 ][0];
                                   y1 = Punti[ p1 ][1];
                                   x2 = Punti[ p2 ][0];
                                   y2 = Punti[ p2 ][1];
                                   DistX = abs(y1-y2);
                                   DistY = abs(x1-x2);
                                   if(DistX>30000 || DistY>30000) continue;
                                   Dist = (double)(sqrt_algo((DistX*DistX)+(DistY*DistY)));
                                   if(Dist<DIST_MIN_ROT_DOPPIA_MAX+30){ // +30 sicurezza
                                      //---------------------------------------------
                                      // TGV in allarme troppo vicino
                                      //---------------------------------------------
                                      stop_per_altro_tgv_in_allarme = true;
                                   }
                                   break;
                                }
                             }
                             //-----------------------------------------------------
                             // Verifico ingombro INGENIERE
                             //-----------------------------------------------------
                             for(k=0;k<MAXPERCORSI;k++){
                                if(AGV[j]->mission.punto[k+1]==0               ) break;
                                if(AGV[j]->mission.punto[k]<1                  ) continue;
                                if(AGV[j]->mission.punto[k]>TOTPUNTI           ) continue;
                                if(AGV[j]->mission.punto[k]!=AGV[j]->stato.pos ) continue;
                                if(N.nodo_busy[AGV[j]->mission.punto[k+1]]!=j  ) continue;
                                //----------------------------------------------------------
                                // Valido solo se c'è un rettilineo che unisca la
                                // posizione attuale e la successiva
                                //----------------------------------------------------------
                                if(AGV[j]->mission.dati_perc[k].rot!=AGV[j]->mission.dati_perc[k+1].rot) continue;
                                //----------------------------------------------------------
                                // Se il TGV è in movimento sul punto successivo, controllo
                                // l'ingombro del tratto
                                //----------------------------------------------------------
                                stop_per_altro_tgv_in_allarme=false;
                                if(AGV[j]->mission.punto[k+2]>0 && AGV[j]->mission.punto[k+2]<=TOTPUNTI){
                                   //-------------------------------------------------
                                   // Verifico ingombro tratto successivo a quello
                                   // considerato
                                   //-------------------------------------------------
                                   if(N.nodo_busy[AGV[j]->mission.punto[k+2]]==j){
                                      dir=AGV[j]->mission.dati_perc[k+1].dir;
                                      IngombroAgv2->AzzeraIngombroAgv();
                                      IngombroAgv2->SettaIngombroSuTratto(AGV[j]->mission.punto[k+1], AGV[j]->mission.punto[k+2], dir, all_mess);
                                   }
                                   //-------------------------------------------------
                                   // Fermo sul successivo uso stessa rotazione di
                                   // ingresso perchè controllo valido solo in retta
                                   // tra pos attuale e successiva
                                   //-------------------------------------------------
                                   else{
                                      IngombroAgv2->AzzeraIngombroAgv();
                                      IngombroAgv2->SettaIngombroSuTratto(AGV[j]->mission.punto[k+1], AGV[j]->mission.punto[k+1], grd_rot, all_mess);
                                   }
                                }
                                //-------------------------------------------------
                                // Fermo sul successivo uso stessa rotazione di
                                // ingresso perchè controllo valido solo in retta
                                // tra pos attuale e successiva
                                //-------------------------------------------------
                                else{
                                   IngombroAgv2->AzzeraIngombroAgv();
                                   IngombroAgv2->SettaIngombroSuTratto(AGV[j]->mission.punto[k+1], AGV[j]->mission.punto[k+1], grd_rot, all_mess);
                                }
                                //-------------------------------------------------
                                // Verifica intersezione
                                //-------------------------------------------------
                                if(IngombroAgv1->VerificaIntersezioneAgv(IngombroAgv2, all_mess)){
                                   stop_per_altro_tgv_in_allarme=true;
                                   break;
                                }
                             }
                          }
                       }
                       //---------------------------------------------------------------
                       // DISEGNA AREE DI INGOMBRO
                       //---------------------------------------------------------------
                       if(!ModificaCoordinateNodi && stop_per_altro_tgv_in_allarme==true){
                          FormXYNodi->CurvaPerIngombro[agv_num]= 999999;
                          FormXYNodi->SegmentoPerIngombro[agv_num]= 999999;
                          FormXYNodi->DirezionePerIngombro[agv_num] = dir;
                          IngombroAgvGrafico[agv_num]->AzzeraIngombroAgv();
                          //--------------------------------------------------------------------------------
                          // Imposto nella classe Layout la curva su cui disegnare l'ingombro della navetta
                          // che ha la curva già occupata ma che si trova un'altra navetta in mezzo
                          //--------------------------------------------------------------------------------
                          if(curva){
                             for(k=0;;k++){
                                if(PercorsoCurvaBezier[k][1]==0) break;
                                if(PercorsoCurvaBezier[k][1]==prec_puntoAgv && PercorsoCurvaBezier[k][2]==puntoAgv){
                                   FormXYNodi->CurvaPerIngombro[agv_num]=k;
                                   break;
                                }
                             }
                             for(k=0;;k++){
                                if(PercorsoCurvaBezier[k][1]==0) break;
                                if(PercorsoCurvaBezier[k][1]==puntoAgv && PercorsoCurvaBezier[k][2]==prec_puntoAgv){
                                   FormXYNodi->CurvaPerIngombro[agv_num]=k;
                                   //----------------------------------------------------------------------------------------
                                   // Se la struttura curve è compilata con prima il secondo punto di percorrenza e poi il
                                   // primo punto di percorrenza la funzione di disegno disegnerà gli ingombri come se
                                   // percorressi la curva nell'altro verso, quindi per avere il disegno cocnorde gli
                                   // giro la direzione di percorrenza nel disegno
                                   //----------------------------------------------------------------------------------------
                                   if(FormXYNodi->DirezionePerIngombro[agv_num]==0)   FormXYNodi->DirezionePerIngombro[agv_num]=1;
                                   else                                               FormXYNodi->DirezionePerIngombro[agv_num]=0;
                                   break;
                                }
                             }
                          }
                          //-----------------------------------------------------------------------------------
                          // Imposto nella classe Layout il segmento su cui disegnare l'ingombro della navetta
                          // che ha il tratto già occupato ma che si trova un'altra navetta in mezzo
                          //-----------------------------------------------------------------------------------
                          else{
                             for(k=0;;k++){
                                if(Percorso[k][1]==0) break;
                                if(Percorso[k][0]==prec_puntoAgv && Percorso[k][1]==puntoAgv){
                                   FormXYNodi->SegmentoPerIngombro[agv_num]=k;
                                   break;
                                }
                             }
                             for(k=0;;k++){
                                if(Percorso[k][1]==0) break;
                                if(Percorso[k][0]==puntoAgv && Percorso[k][1]==prec_puntoAgv){
                                   FormXYNodi->SegmentoPerIngombro[agv_num]=k;
                                   //----------------------------------------------------------------------------------------
                                   // Se la struttura percorsi è compilata con prima il secondo punto di percorrenza e poi il
                                   // primo punto di percorrenza la funzione di disegno disegnerà gli ingombri come se
                                   // percorressi la curva nell'altro verso, quindi per avere il disegno cocnorde gli
                                   // giro la direzione di percorrenza nel disegno
                                   //----------------------------------------------------------------------------------------
                                   if(FormXYNodi->DirezionePerIngombro[agv_num]==0)   FormXYNodi->DirezionePerIngombro[agv_num]=1;
                                   else                                               FormXYNodi->DirezionePerIngombro[agv_num]=0;
                                   break;
                                }
                             }
                          }
                          //-----------------------------------------------------------------------------------
                          // Imposto nella classe Layout il punto su cui disegnare l'ingombro della navetta
                          // con cui vado a collidere
                          //
                          // AL-19/06/2020 - INGOMBRO AGV PER GESTIONE ROTAZIONE CALAMITE
                          //-----------------------------------------------------------------------------------
                          grd_rot=AGV[j]->stato.rot_calamite;
                          if(AGV[j]->mission.GestioneLaser==true) grd_rot=AGV[j]->stato.rot_laser;
                          FormXYNodi->PuntoPerIngombro[j]   = AGV[j]->stato.pos;
                          FormXYNodi->RotazioneIngombroS[j] = grd_rot;
                          FormXYNodi->RotazioneIngombroE[j] = grd_rot;
                       }
                    }
                    //-----------------------------------------------------------------------------------
                    // !!! SICUREZZA (con AbilitaGestioneIngombroCurvePerStopTGV==2) !!!
                    // ROTAZIONI DA FERMO A 180°
                    //-----------------------------------------------------------------------------------
                    else{
                       //---------------------------------------------
                       // 180° DA FERMO
                       //---------------------------------------------
                       if(agv_rot_180==true || altro_rot_180==true){
                          x1 = Punti[ puntoAgv       ][0];
                          y1 = Punti[ puntoAgv       ][1];
                          x2 = Punti[ punto_altroagv ][0];
                          y2 = Punti[ punto_altroagv ][1];
                          DistX = (double) abs(x1-x2);
                          DistY = (double) abs(y1-y2);
                          if(DistX>30000 || DistY>30000) continue;
                          Dist = (double)sqrt_algo((DistX*DistX)+(DistY*DistY));
                          //--------------------------------------------------
                          // Distanza, maggiore rotazione doppia --> continue
                          //--------------------------------------------------
                          if(Dist<DIST_MIN_ROT_DOPPIA_MAX) stop_per_altro_tgv_in_allarme=true;
                       }
                    }
                    delete IngombroAgv2;
                    if(stop_per_altro_tgv_in_allarme==true) break;
                 }
              }
              delete IngombroAgv1;
           }
        }
        if(stop_per_altro_tgv_in_allarme==true ) SetBit((char *) &dm[0], 14);
        if(stop_per_altro_tgv_in_allarme==false) ClrBit((char *) &dm[0], 14);
     }
     //---------------------------------------------------------
     // STOP PER INCENDIO IN CORSO  (DM88 - bit15)
     // Devo essere fuori ingombro alle porte (Fontana F.)
     //---------------------------------------------------------
     if(IncendioInCorso==false) ClrBit((char *) &dm[0], 15);
     else{
        sotto_porta=0;
        //------------------------------------------------------
        // TGV SU NODI IN INGOMBRO A VIE DI FUGA
        // Controllo se navetta in vicino a punti in prossimità
        // di vie di fuga
        //-----------------------------------------------------
        if(N.nodi_in_ingombro_per_porte[stato.pos       ]==true) sotto_porta = N.nodi_in_ingombro_per_porte[stato.pos];
        if(N.nodi_in_ingombro_per_porte[mission.pintend ]==true) sotto_porta = N.nodi_in_ingombro_per_porte[mission.pintend];
        //-----------------------------------------------------
        // TGV SOTTO PORTE
        // Se il TGV ? lontano almeno 2 nodi da qualunque
        // nodo porta si deve fermare altrimenti deve
        // proseguire per liberare il passaggio
        //-----------------------------------------------------
        if(sotto_porta==0){
           for(Port=1; Port<=TOTPORTE; Port++){
              //----------------------------------------------------
              // Recupero il numero del TGV sul nodo della porta
              //----------------------------------------------------
              for(i=0; i<11; i++){
                 if(PorteImp[Port].Punto[i]<1                    ) continue;
                 if(PorteImp[Port].Punto[i]>MAXPUNTI             ) continue;
                 if(PorteImp[Port].Punto[i]!=mission.pintend &&
                    PorteImp[Port].Punto[i]!=stato.pos           ) continue;
                 //------------------------------------------------------
                 // TGV SOTTO PORTA
                 //------------------------------------------------------
                 sotto_porta=PorteImp[Port].Punto[i];
                 break;
              }
              if(sotto_porta!=0) break;
           }
        }
        //-----------------------------------------------------
        // Se ? alto il DM di incendio sul PLC TERRA
        // forzo anche il PINT per fermare il TGV
        //-----------------------------------------------------
        if(sotto_porta==0) SetBit((char *) &dm[0], 15);
     }
     //------------------------------------------------------
     // 10/04/2019 GESTIONE MULTI-PLC
     //------------------------------------------------------
     dm88_stop_tgv=dm[0];
     //------------------------------------------------------
     // GESTIONE ASRV (stop in emergenza)
     // D88.0  = stop baia/macchina in emergenza
     // D88.1  = stop porta/elevatore in emergenza
     // D88.14 = stop altro navetta troppo vicina
     // D88.15 = stop per incendicio in corso
     //------------------------------------------------------
     #ifdef GESTIONE_ASRV
        asrv_comandi.cbit.stop=false;
        if(TestBit((char *)&dm88_stop_tgv,  0)==true) asrv_comandi.cbit.stop=true;
        if(TestBit((char *)&dm88_stop_tgv,  1)==true) asrv_comandi.cbit.stop=true;
        if(TestBit((char *)&dm88_stop_tgv, 14)==true) asrv_comandi.cbit.stop=true;
        if(TestBit((char *)&dm88_stop_tgv, 15)==true) asrv_comandi.cbit.stop=true;
     #endif;
     //------------------------------------------------------
     // Scrittura DM88 con nuovo stato consensi
     //------------------------------------------------------
     if((OmrPlc[agv_num].TypCom!=3 && OmrPlc[agv_num].TypCom!=4) || !AbilitaLetturaMultiPLC){
        if(mission.EsclPLC==false && break_Com[COM1]==false){
           err = OM.wr_canale(&dm[0], (WORD)(agv_num), "WD", 88, 1, all_mess);
           if(err) sprintf(all_mess, "Error in set bit per stop TGV%02d on DM88...", agv_num);
        }
     }
  }
  return err;
}

// ------------------------
//    AzzeraAllarmi()
// ------------------------
// Azzera le aree di Memoria per gli Allarmi TGV
//
int agv::AzzeraAllarmi(char *all_mess)
/*********************************/
{
  int  err=0;
  WORD lr[3];
  WORD hr[3];

  memset(&lr , 0, sizeof(lr));
  memset(&hr , 0, sizeof(hr));
  //---------------------------------------------------------
  // Resetta Allarme Interno
  //---------------------------------------------------------
  allarme_interno = 0;
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV Š escluso
  //---------------------------------------------------------
  if(mission.EsclPLC) return 0;
  if(break_Com[COM1]) return 0;

  //-----------------------------------------
  // LR 6,7  = Aree di memoria con Allarmi
  // HR 0.15 = Allarme presente
  //-----------------------------------------
  //----------------
  // Azzero gli LR
  //----------------
  err = OM.wr_canale(lr, (WORD)(agv_num), "WL", 6, 2, all_mess);
  if(err){sprintf(all_mess, MESS[347], agv_num); return 1;}

  //----------------------------------------
  // Recupero il contenuto dell'HR0
  //----------------------------------------
  err = OM.ld_canale(&hr[0], (WORD)(agv_num), "RH", 0, 1, all_mess);
  if(err){sprintf(all_mess, MESS[347], agv_num); return 1;}

  //----------------------------------------
  // Setto solo il Bit15 dell'HR0
  //--------------------------------------
  SetBit((char *) &hr[0], 15);

  //----------------------------------------
  // Riscrivo l'HR0 modificato
  //----------------------------------------
  err = OM.wr_canale(&hr[0], (WORD)(agv_num), "WH", 0, 1, all_mess);
  if(err){sprintf(all_mess, MESS[347], agv_num); return 1;}

  return err;
}


// ------------------------
//    load_mission()
// ------------------------
// load Mission da file
//
int agv::load_mission(char *all_mess)
/***********************************/
{
  int  i,
       err =0;
  int handle;

  memset(&mission , 0, sizeof(mission));
  handle = sopen( mission_name_sorg, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
      close( handle );
      sprintf(all_mess, MESS[112], mission_name_sorg);
      return 1;
  }
  i = read( handle, &mission, sizeof( mission ) );
  close( handle );

  if (i != sizeof(mission))  err = 1;
  if(err) sprintf(all_mess, MESS[112], mission_name_sorg);


  //------------------------------------------------------
  // GESTIONE_ASRV
  //------------------------------------------------------
  #ifdef GESTIONE_ASRV
     if(!err) err = ASRV_load_file_mission(all_mess);
  #endif;

  return err;
}


// ------------------------
//     save_mission()
// ------------------------
// Salva missione AGV su file
//
int agv::save_mission(char *all_mess)
/***********************************/
{
  int  i,
       err = 0;
  int handle;

  mission.OldPos     = stato.pos;
  mission.OldStart   = stato.start;
  mission.OldComandi = stato.s.val;
  if(mission.GestioneLaser==true) mission.OldRot = stato.rot_laser;
  else                            mission.OldRot = stato.rot_calamite;
  memcpy(&mission.old_allarm[0], &allarme_esterno[0], sizeof(mission.old_allarm));
  mission.old_allarme_interno = old_allarme_interno;
  //----------------------------------------------------
  // Azzeramento del numero delle missioni giornaliere
  //----------------------------------------------------
  if(mission.NumMissioni<0 || mission.NumMissioni>9999) mission.NumMissioni=0;

  //---------------------------------------------
  handle = sopen( mission_name_dest, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );

  if( handle<0 ){
      close( handle );
      sprintf(all_mess, MESS[113], mission_name_dest);
      return 1;
  }
  i =  write( handle, &mission, sizeof( mission ) );
  close( handle );

  if (i != sizeof(mission))  err = 1;
  if(err) sprintf(all_mess, MESS[113], mission_name_dest);

  //------------------------------------------------------
  // GESTIONE_ASRV
  //------------------------------------------------------
  #ifdef GESTIONE_ASRV
     // stato ASRV
     if(!err) err = ASRV_save_file_stato(all_mess);
     // missione ASRV
     if(!err) err = ASRV_save_file_mission(all_mess);
  #endif;

  return err;
}

// ------------------------
//     delete_mission()
// ------------------------
// Cancella la missione su file
//
int agv::delete_mission(char *all_mess)
/**************************************/
{
  int err=0;

  memset(&mission , 0, sizeof(mission));
  err = save_mission(all_mess);
  return err;
}

// --------------------------------
//      reset_ram_mission()
// --------------------------------
// Resetta i dati della missione AGV relativi al percorso e al prg. esclu-
// sivamente in RAM
//
void agv::reset_ram_mission()
/*****************************/
{
  int  i;
  short int num_baia, num_mac, num_mag, num_pos, num_pin;

  memset(&mission.step,            0, sizeof(mission.step));
  memset(&mission.punto,           0, sizeof(mission.punto));
  memset(&mission.dati_perc,       0, sizeof(mission.dati_perc));
  memset(&mission.punto_prenotato, 0, sizeof(mission.punto_prenotato));
  mission.pstart           = 0;
  mission.pend             = 0;
  mission.pintstart        = 0;
  mission.pintend          = 0;
  mission.pintendtrasmesso = 0;
  mission.StopManuale      = false; // Forzatura dello Stop del TGV sul Punto Intermedio.
  //---------------------------------------------
  // RESET MISSIONI AL VOLO
  // Se sono abilitate le missioni al volo
  // e il TGV ha il PIntForzato devo resettare
  // anche la chiamata perchè si tratta di
  // una chiamata per reset missione al volo
  //---------------------------------------------
  if(mission.ResetAlVolo==true && mission.PIntForzato!=0){
     reset_chiamata_AGV();
     mission.ResetAlVolo=false;
  }
  mission.PIntForzato=0;
  //---------------------------------------------
  // AZZERAMENTO STRUTTURA NODI PRENOTATI
  //---------------------------------------------
  for(i=0; i<=TOTPUNTI; i++){
     nodi_prenotati[i][agv_num-1]=0;
  }
  //---------------------------------------------
  // MAGAZZINI MULTI NODO
  // Azzermento variabile che contiene il
  // nodo di ingresso/uscita nel caso di
  // impianti con magazzini multi nodo
  //---------------------------------------------
  mission.NodIngMag = 0;
  //---------------------------------------------
  // AL-27/04/2017
  // In caso di annullamento missione
  // azzero anche eventuali TGV bloccanti
  // perchè se la navetta poi viene esclusa
  // e non riparte, risulterebbe ancora bloccata
  // da qualche TGV e questo crea casini
  // di sblocchi dead-lock immotivati
  //---------------------------------------------
  AgvBloccati[agv_num]=0;
  //---------------------------------------------
  //  GESTIONE PRECEDENZE PER TGV IN RETTILINEO
  //---------------------------------------------
  for(i=0; i<=MAXPUNTI; i++){
     if(N.nodo_con_tgv_lanciato[i]!=agv_num) continue;
     N.nodo_con_tgv_lanciato[i]=0;
  }
  //---------------------------------------------
  //  GESTIONE VISUALIZZAZIONE GRADI
  //---------------------------------------------
  memset(&N.nodo_grd[agv_num][0], 0, sizeof(N.nodo_grd[agv_num]));
  //---------------------------------------------------------
  // Resetta Allarme Interno 12/07/2016
  //---------------------------------------------------------
  if(allarme_interno == ALL_INT_DEST_NON_VALIDA){
     allarme_interno = 0;
  }
  //---------------------------------------------------------
  // GESTIONE_ASRV
  //---------------------------------------------------------
  #ifdef GESTIONE_ASRV
     memset(&asrv_mission, 0, sizeof(ASRV_MISSION));
  #endif;
  return;
}

// --------------------------------
// reset_ram_mission_per_manuale()
// --------------------------------
//
// SOLO IN MANUALE
//
// Resetta i dati della missione AGV e del programma trasmesso al TGV
// lasciando le informazioni di percorso per mantenere lontano le altre
// navette
//
void agv::reset_ram_mission_per_manuale()
/***************************/
{
  int i;
  short int num_baia, num_mac, num_mag, num_pos, num_pin;


  memset(&mission.step, 0, sizeof(mission.step));
  mission.pstart           = 0;
  mission.pintstart        = 0;
  mission.pintendtrasmesso = 0;
  mission.StopManuale      = false; // Forzatura dello Stop del TGV sul Punto Intermedio.
  //---------------------------------------------
  // RESET MISSIONI AL VOLO
  // Se sono abilitate le missioni al volo
  // e il TGV ha il PIntForzato devo resettare
  // anche la chiamata perchè si tratta di
  // una chiamata per reset missione al volo
  //---------------------------------------------
  if(mission.ResetAlVolo==true && mission.PIntForzato!=0){
     reset_chiamata_AGV();
     mission.ResetAlVolo=false;
  }
  mission.PIntForzato=0;
  //---------------------------------------------
  // AL-21/10/2015 Azzero a tgv anche eventuali
  // AGV BLOCCANTI. Senza missione non possono
  // avere agv bloccanti
  //---------------------------------------------
  AgvBloccati[agv_num]     = 0;
  //---------------------------------------------
  //  GESTIONE PRECEDENZE PER TGV IN RETTILINEO
  //---------------------------------------------
  for(i=0; i<=MAXPUNTI; i++){
     if(N.nodo_con_tgv_lanciato[i]!=agv_num) continue;
     N.nodo_con_tgv_lanciato[i]=0;
  }
  //---------------------------------------------
  //  GESTIONE VISUALIZZAZIONE GRADI
  //---------------------------------------------
  memset(&N.nodo_grd[agv_num][0], 0, sizeof(N.nodo_grd[agv_num]));
  //---------------------------------------------------------
  // GESTIONE_ASRV
  //---------------------------------------------------------
  #ifdef GESTIONE_ASRV
     memset(&asrv_mission, 0, sizeof(ASRV_MISSION));
  #endif;
  return;
}

// --------------------------------
//      reset_chiamata_AGV()
// --------------------------------
// Resetta i dati della missione AGV relativi al percorso e al prg. esclu-
// sivamente in RAM
//
void agv::reset_chiamata_AGV()
/***************************/
{
  //--------------------------------------
  // Azzeramento vero e proprio della
  // chiamata TGV e Provenienza
  //--------------------------------------
  mission.Chiamata    = 0;
  //--------------------------------------
  // La provenienza viene azzerata solo
  // a foche vuote
  //--------------------------------------
  if(mission.PresCEST_A==LIBERA && mission.PresCEST_B==LIBERA){
     mission.Provenienza = 0;
  }
  return;
}


// --------------------------------
//      reset_mission()
//  Resetta i dati della missione
//  relativi al percorso e al prg.
// --------------------------------
// resetta i dati della missione AGV relativi al percorso e al programma
// sia in RAM che su file.
//
int agv::reset_mission(char *all_mess)
/************************************/
{
  int err=0;

  reset_ram_mission();
  //---------------------------------------------------------
  // Salvataggio del nuovo stato della missione
  //---------------------------------------------------------
  if(save_mission(all_mess)) return 1;
  //---------------------------------------------------------
  // libera tutti i nodi occupati dall'AGV tranne quello su
  // cui si trova attualmente
  //---------------------------------------------------------
  if(free_all_nodi(all_mess)) return 1;
  return err;
}/*** reset_mission() ***/


// -------------------------------
//    LiberaTgvInCambioMissione
// -------------------------------
// Azzera a tempo il flag di TGV in cambio missione
//
void agv::LiberaTgvInCambioMissione()
/******************************************/
{
  long int DiffTempo;
  long int TimeAttuale;

  //--------------------------------------------------------
  // TGV non in cambio missione --> skip!
  //--------------------------------------------------------
  if(mission.TGVInCambioMissione==false) return;

  //--------------------------------------------------------
  // Calcolo del tempo che il TGV è rimasto fermo.
  //--------------------------------------------------------
  TimeAttuale = time(NULL);
  DiffTempo   = TimeAttuale-mission.TimeInitAttesa;

  //--------------------------------------------------------
  // Aspetto max 3 sec. che un TGV prenda una nuova missione
  //--------------------------------------------------------
  if(DiffTempo>10){
     mission.TimeInitAttesa=0;
     mission.TGVInCambioMissione=false;
  }

  return;
}

// -------------------------------
//    SetResetPIntForzatoTGV
// -------------------------------
// Attiva ripartenza TGV fermi per diversi motivi
//
void agv::SetResetPIntForzatoTGV()
/******************************************/
{
  int  i, j, err;
  short int ptr_percorso;
  char all_mess[101]="";
  short int p1, p2;
  short int num_baia;
  short int altro_agv;
  short int ForzaPInt;
  short int count_nodi;
  bool ResetMissione;
  bool ResetChiamata;
  bool ResetPIntForzato;
  bool tgv_in_senso_opposto;
  bool pintforzato_gia_passato;
  bool last_pint_fuori_ingombro;
  bool DestPosRicarica, baia_not_ok;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  short int MinPercorsi, MinPercToStart, nodo_interr;
  WORD dm[11];

  ForzaPInt=0;
  ResetMissione=false;
  ResetChiamata=false;
  ResetPIntForzato=false;
  //-------------------------------------------------------------
  // Controlli validi solo per TGV in missione automatica
  //-------------------------------------------------------------
  if(mission.SemiAutomatico==true) return;
  if(mission.Esclusione==true    ) return;
  //-------------------------------------------------------------
  // GESTIONI CON TGV IN MISSIONE IN CORSO
  //-------------------------------------------------------------
  if(test_mission_on()){
     //-------------------------------------------------------------
     // Cerca indice iniziale del percorso
     //-------------------------------------------------------------
     for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
        if(mission.punto[ptr_percorso]==stato.pos  ) break;
        if(mission.punto[ptr_percorso]==0          ) break;
     }
     if(mission.punto[ptr_percorso]!=stato.pos) return;
     //-------------------------------------------------------------
     // 0. INCENDIO IN CORSO IN INGOMBRO ALLE PORTE
     //-------------------------------------------------------------
     if(IncendioInCorso==true && N.nodi_in_ingombro_per_porte[stato.pos]==true && stato.pos==mission.PIntForzato){
        ResetMissione=true;
     }
     //-------------------------------------------------------------
     // Libero TGV su PIntForzato
     //-------------------------------------------------------------
     if(stato.pos==mission.PIntForzato && mission.ResetAlVolo==true){
        ResetMissione=true;
        //------------------------------------------------------------------
        // RESET CHIAMATA
        //------------------------------------------------------------------
        if(AbilitaResetMissioniAlVolo==2) ResetChiamata=true;
        //------------------------------------------------------------------
        // file traccia
        //------------------------------------------------------------------
        if(ResetChiamata==true){
           sprintf(all_mess, "(3a) - TGV%02d END RESET SU PINT:%04d (OK RESET CHIAMATA)", agv_num, AGV[agv_num]->mission.PIntForzato);
        }
        else{
           sprintf(all_mess, "(3b) - TGV%02d END RESET SU PINT:%04d (NO RESET CHIAMATA)", agv_num, AGV[agv_num]->mission.PIntForzato);
        }
        file_cronologico_generico("LOGRESET.TXT", all_mess, 50000L);
     }
     //-------------------------------------------------------------
     // Se il TGV ho già oltrepassato il PINT FORZATO lo sposto
     // o lo resetto
     //-------------------------------------------------------------
     if(mission.PIntForzato!=0 && stato.pos!=mission.PIntForzato && mission.ResetAlVolo==true){
        pintforzato_gia_passato=true;
        for(i=ptr_percorso; i<MAXPERCORSI; i++){
           if(mission.punto[i]==0                  ) break;
           if(mission.punto[i]==mission.pend       ) break;
           if(mission.punto[i]!=mission.PIntForzato) continue;
           pintforzato_gia_passato=false;
           break;
        }
        //--------------------------------------------
        // PIntForzato già passato
        //--------------------------------------------
        if(pintforzato_gia_passato==true){
           //-----------------------------------------
           // SPOSTO PIntForzato
           //-----------------------------------------
           if(mission.pintend!=mission.pend){
              ResetPIntForzato=false;
              ForzaPInt=mission.pintend;
              sprintf(all_mess, "(4) - TGV%02d RINNOVO PINT FORZATO SU PINT:%04d", agv_num, ForzaPInt);
              file_cronologico_generico("LOGRESET.TXT", all_mess, 50000L);
           }
           //-----------------------------------------
           // RESETTO PIntForzato
           //-----------------------------------------
           else{
              ForzaPInt=0;
              ResetPIntForzato=true;
              sprintf(all_mess, "(5) - TGV%02d RESET PINT FORZATO", agv_num);
              file_cronologico_generico("LOGRESET.TXT", all_mess, 50000L);
           }
        }
     }
     //-------------------------------------------------------
     // Verifico se il PInt forzato è l'ultimo punto fuori
     // ingombro che il TGV incontra lungo la missione
     //-------------------------------------------------------
     last_pint_fuori_ingombro=false;
     if(mission.PIntForzato>0 && mission.PIntForzato<=TOTPUNTI){
        for(i=MAXPERCORSI-1; i>0; i--){
           if(mission.punto[i]==0) continue;
           //------------------------------------------------------------------
           // Se il TGV è già in posizione fuori ingombro --> skip!
           //------------------------------------------------------------------
           for(j=0; j<MAXNODIATTESA; j++){
              if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]<1                   ) continue;
              if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]>TOTPUNTI            ) continue;
              if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]!=mission.punto[i]   ) continue;
              if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]!=mission.PIntForzato) continue;
              last_pint_fuori_ingombro=true;
              break;
           }
           if(last_pint_fuori_ingombro==true) break;
        }
     }
     //-------------------------------------------------------------
     // AVVICINAMENTO A DESTINAZIONE
     // Reset PIntForzato per tgv in avvicinamento a baia che
     // va in chiamata
     //-------------------------------------------------------------
     if(AbilitaAvvicinamentoADest!=0 && mission.ResetAlVolo==false && last_pint_fuori_ingombro==true && ForzaPInt==0){
        //------------------------------------------------------------------
        // Recupero tipo di destinazione
        //------------------------------------------------------------------
        P.test_punto_presente_baia(all_mess, mission.pend, &num_baia);
        if(num_baia>0 && num_baia<=TOTBAIE){
           //-----------------------------------------------------
           // Baia in blocco
           //-----------------------------------------------------
           baia_not_ok=false;
           if(P.ba[num_baia]->Esclusione==true ) baia_not_ok=true;
           if(P.ba[num_baia]->Blocco==true     ) baia_not_ok=true;
           if(P.ba[num_baia]->Automatico==false) baia_not_ok=true;
           if(P.ba[num_baia]->Chiamata==false  ) baia_not_ok=true;
           if(P.ba[num_baia]->Tipo!=BA_PRELIEVO &&
              P.ba[num_baia]->Tipo!=BA_DEPOSITO) baia_not_ok=true;
           if(P.ba[num_baia]->Tipo==BA_PRELIEVO){
              if(P.ba[num_baia]->PresCEST_B==LIBERA) baia_not_ok=true;
              if(P.ba[num_baia]->StatoPres==0      ) baia_not_ok=true;
           }
           if(P.ba[num_baia]->Tipo==BA_DEPOSITO){
              if(P.ba[num_baia]->PresCEST_B!=LIBERA) baia_not_ok=true;
              if(P.ba[num_baia]->StatoPres!=0      ) baia_not_ok=true;
           }
           //-----------------------------------------------------
           // Verifico altro TGV con stessa destinazione
           // e nessun PIntForzato
           //-----------------------------------------------------
           altro_agv=0;
           for(i=1; i<=MAXAGV; i++){
              if(i==agv_num) continue;
              //------------------------------------------------------
              // Altro TGV su destinazione
              //------------------------------------------------------
              if(AGV[i]->stato.pos==mission.pend){altro_agv=i; break;}
              //------------------------------------------------------
              // Altro TGV destinato a pend senza PIntForzato
              //------------------------------------------------------
              if(AGV[i]->mission.pend==mission.pend && AGV[i]->mission.PIntForzato==0){altro_agv=i; break;}
           }
           //-----------------------------------------------------
           // BAIA OK:
           // Reset PIntForzato se percorso sgombro fino a dest
           // e nessun altro TGV con medesima destinazione nessun
           // PIntForzato
           //-----------------------------------------------------
           if(baia_not_ok==false && altro_agv==0){
              //-----------------------------------------------------
              // Conteggio nodi mancanti a PIntForzato per reset
              // PIntForzato
              //-----------------------------------------------------
              count_nodi=0;
              for(i=ptr_percorso; i<MAXPERCORSI; i++){
                 if(mission.punto[i]==0                  ) break;
                 if(mission.punto[i]==mission.pend       ) break;
                 if(mission.punto[i]==mission.PIntForzato) break;
                 count_nodi++;
              }
              if(stato.pos==mission.PIntForzato || (count_nodi!=0 && count_nodi<=NUM_NODI_PER_RESET_PINTFORZATO)){
                 //-----------------------------------------------------
                 // Recupero il percorso mancante alla destinazione
                 //-----------------------------------------------------
                 count_nodi=0;
                 memset(&nodi_perc[0], 0, sizeof(nodi_perc));
                 memset(&dati_perc[0], 0, sizeof(DATI_PERCORSO));
                 for(i=ptr_percorso; i<MAXPERCORSI; i++){
                    if(mission.punto[i]==0) break;
                    nodi_perc[count_nodi]=mission.punto[i];
                    memcpy(&dati_perc[count_nodi], &mission.dati_perc[i], sizeof(DATI_PERCORSO));
                    count_nodi++;
                 }
                 //-----------------------------------------------------
                 // Verifica se percorso mancante interamente
                 // disponibile
                 //-----------------------------------------------------
                 altro_agv=MinPercorsi=MinPercToStart=nodo_interr=0;
                 test_percorso_disponibile(&nodi_perc[0], &dati_perc[0], &altro_agv, &MinPercorsi, &MinPercToStart, &nodo_interr, all_mess);
                 if(MinPercorsi>=count_nodi){
                    ResetPIntForzato=true;
                 }                    
              }
           }
        }
     }
     //-------------------------------------------------------------
     // RITORNO IN RICARICA
     // GESTIONE PINT FORZATO SU PUNTO FUORI INGOMBRO
     // Sull'inizializzazione delle missione di ritorno in
     // ricarica, il TGV forza il PInt sull'ultimo punto fuori
     // ingombro che incontra, ma l'annullamento della missione
     // la esegue solo se non incontra nessun altro TGV in
     // ingombro
     //-------------------------------------------------------------
     if(PosSuFuoriIngombroPreRicarica==true && mission.ResetAlVolo==false && last_pint_fuori_ingombro==true && stato.pos==mission.PIntForzato && ForzaPInt==0){
        DestPosRicarica=false;
        if(mission.pend==mission.PosRicarica) DestPosRicarica=true;
        if(AbilitaRiposoFlottante==true){
           for(i=1; i<=MAXAGV; i++){
              if(mission.pend==AGV[i]->mission.PosRicarica) DestPosRicarica=true;
           }
        }
        if(DestPosRicarica==true){
           //----------------------------------------------------------
           // Verifico se c'è un altro TGV che viene in senso opposto
           //----------------------------------------------------------
           p1=p2=0;
           tgv_in_senso_opposto=false;
           for(i=0; i<MAXPERCORSI; i++){
              if(mission.punto[i+1]==0       ) break;
              if(mission.punto[i]!=stato.pos ) continue;
              p1=stato.pos;
              p2=mission.punto[i+1];
              break;
           }
           for(i=1; i<=MAXAGV; i++){
              if(p1==0 || p2==0             ) break;
              if(i==agv_num                 ) continue;
              if(!AGV[i]->test_mission_on() ) continue;
              for(j=0; j<MAXPERCORSI; j++){
                 if(AGV[i]->mission.punto[j+1]==0   ) break;
                 if(AGV[i]->mission.punto[j]==p2 && AGV[i]->mission.punto[j+1]==p1){
                    tgv_in_senso_opposto=true;
                    break;
                 }
              }
              if(tgv_in_senso_opposto==true) break;
           }
           //----------------------------------------------------
           // Se nessun TGV si muove in senso opposto posso
           // procedere al reset della missione altrimenti
           // procedo con la missione fino alla ricarica
           //----------------------------------------------------
           if(tgv_in_senso_opposto==false) ResetMissione=true;
           else                            ResetPIntForzato=true;
        }
     }
  }

  //------------------------------------------------------------------
  // IMPIANTI SPECIALI
  // Personalizzazioni legate agli impianti
  //------------------------------------------------------------------
  SetResetPIntForzatoTGV_IMPIANTI(agv_num, &ResetMissione, &ResetChiamata, &ResetPIntForzato, &ForzaPInt);

  //----------------------------------------------------------
  // PROCEDURA DI ANNULLAMENTO MISSIONE
  //
  // Se ci sono le condizioni per eseguire un Reset delle
  // Missioni al volo la prima cosa che faccio è trasmettere
  // il Reset al PLC (DM98) in modo da bloccare il reset
  // della missione in RAM in caso di errore di comunicazione
  //----------------------------------------------------------
  if(ResetMissione==true && !stato.s.bit.sunod           ) ResetMissione=false;
  if(ResetMissione==true && trasmetti_reset_AGV(all_mess)) ResetMissione=false;
  //----------------------------------------------------------
  // SICUREZZA
  // Invio due volte il comando di RESET MISSIONE TGV
  //----------------------------------------------------------
  if(ResetMissione==true && trasmetti_reset_AGV(all_mess)) ResetMissione=false;
  //----------------------------------------------------------
  // Verifico lo stato del DM91 sul TGV per vedere se
  // l'azzeramento è andato a buon fine.
  //----------------------------------------------------------
  if(ResetMissione==true && !break_Com[COM1]){
     memset(&dm[0], 0, sizeof(dm));
     err = OM.ld_canale(&dm[0], (WORD)(agv_num), "RD", 91, 2, all_mess);
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
     //---------------------------------------------------------------
     // Reset missione in RAM
     //---------------------------------------------------------------
     reset_mission(all_mess);
     //---------------------------------------------------------------
     // Aggiorno già lo stato del TGV senza attendere il load_stato
     // che avverrebbe il ciclo di scansione dopo
     //---------------------------------------------------------------
     stato.start=0;
     stato.s.bit.okprog=1;
     //-----------------------------------------------------------------
     // Reset anche della chiamata solo se previsto
     //-----------------------------------------------------------------
     if(ResetChiamata==true) reset_chiamata_AGV();
     //-----------------------------------------------------------------
     // Memorizzo che è stato fatto un reset missione al TGV
     // per cambio missione automatico e inizializzo la data/ora
     //-----------------------------------------------------------------
     mission.TGVInCambioMissione=true;
     mission.TimeInitAttesa=time(NULL);
  }
  //----------------------------------------------------------
  // INCENDIO IN CORSO - NON DEVE ESSERE AZZERATO IL PINT
  //----------------------------------------------------------
  if(IncendioInCorso==true) ResetPIntForzato=false;
  //----------------------------------------------------------
  // Forzature Punto Intermedio
  //----------------------------------------------------------
  if(ResetPIntForzato==false && ForzaPInt>0 && ForzaPInt<MAXPUNTI){
     mission.PIntForzato = ForzaPInt;
  }
  //----------------------------------------------------------
  // Reset Punto Intermedio
  //----------------------------------------------------------
  if(ResetPIntForzato==true && ForzaPInt==0){
     mission.PIntForzato = 0;
  }
}

// -------------------------------
//      SpostaTGVInAttesa
// -------------------------------
// Attiva ripartenza TGV fermi per diversi motivi
//
void agv::SpostaTGVInAttesa()
/******************************************/
{
  return;
}

// -------------------------------
//  TrasmettiResetBloccoIngAGV
// -------------------------------
// resetta il DM89 dando cos il consenso all'AGV ad entrare sul pallettizzatore
//
int agv::TrasmettiResetBloccoIngAGV(char *all_mess)
/******************************************/
{
  WORD dm[10];
  WORD NumCanali      = 1;
  WORD NumPlc         = (WORD)(agv_num);
  WORD CanaleIniziale = 89;

  sprintf(all_mess, "");
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV Š escluso
  //---------------------------------------------------------
  if(mission.EsclPLC) return 0;
  if(break_Com[COM1]) return 0;
  //-------------------------
  // DM 89 = 0
  //-------------------------
  dm[1]=0;
  if(OM.wr_canale(&dm[1], NumPlc, "WD", CanaleIniziale, NumCanali, all_mess)) return 1;
  return 0;
}

// ---------------------------------------------------
//      test_mission_on()
// ---------------------------------------------------
// Verifica se per l'AGV esiste una missione in esecuzione. restituisce 0
// in caso negativo
// Affinchè una missione sia attiva, Š necessario che abbia almeno un punto
// di percorso > 0 e un punto di partenza.
//
int agv::test_mission_on()
/************************/
{
  if(stato.s.bit.okprog==false             ) return 1; //19/02/2013 Per evitare rallentamenti in debug
  return(mission.pstart || (stato.start==1));
}

// ------------------------
//   load_zone_mapping()
// ------------------------
// Lettura dello stato del "Zone di mapping" da file
//
int agv::load_zone_mapping(char *all_mess)
/***********************************/
{
  FILE * ptrfile;
  int  i, err =0;

  memset(&zone_mapping, 0, sizeof(ZONEMAPPING));
  ptrfile = fopen(filemapping, "rb");
  if(ptrfile==0){
     sprintf(all_mess, MESS[112], filemapping);
     return 1;
  }
  i= fread(&zone_mapping, 1, sizeof(ZONEMAPPING), ptrfile);
  fclose(ptrfile);
  if (i != sizeof(ZONEMAPPING))  err = 1;
  if(err) sprintf(all_mess, MESS[112], filemapping);
  return err;
}


// ------------------------
//   save_zone_mapping()
// ------------------------
// Lettura dello stato delle "Zome di mapping" da file
//
int agv::save_zone_mapping(char *all_mess)
/***********************************/
{
  FILE * ptrfile;
  int  i, err = 0;

  ptrfile = fopen(filemapping, "wb");
  if (ptrfile==0) return 1;
  i= fwrite(&zone_mapping, 1, sizeof(ZONEMAPPING), ptrfile);
  fclose(ptrfile);
  if (i != sizeof(ZONEMAPPING))  err = 1;
  if(err) sprintf(all_mess, MESS[113], filemapping);
  return err;
}

// ------------------------
//   CalcoloNodoTgvLaser()
// ------------------------
// Viene calcolato il nodo di layout più vicino alle coordinate passate
// attualmente dal laser e questo valore viene trasmesso al TGV sul DM305
// per permettere al sistema di recuperare la propria posizione dopo
// un "fuori traccia"
//
// N.B. I controlli sulla posizione vengono fatti non sulla distanza diagonale
//      ma su una finestra in X e Y per ragioni di semplicità e di velocità
//
void agv::CalcoloNodoTgvLaser()
/***********************************/
{
  char all_mess[300];
  double Xly, Yly;
  int  lx, ly;
  int  i, j;
  int  Xtgv, Ytgv;
  int  PosTgv;
  int  Xgraf, Ygraf;
  int  X1, Y1;
  int  DistanzaMAX=50;    // 50 cm. !!!!
  int  Punto;
  int  err;
  int  dist, old_dist;
  short int zona;
  int  NumZona;
  bool nodo_non_valido;
  WORD dm[20];
  WORD NumCanali=1;
  WORD NumCanaleIniziale=305;

  //----------------------------------
  // Controlla condizioni per -->SKIP
  //----------------------------------
  if( break_Com[COM1]     ) return;
  if( mission.EsclPLC     ) return;
  if( mission.Esclusione  ) return;
  if( stato.s.bit.funz    ) return; //Automatico
  if(!stato.s.bit.okprog  ) return;
//if( stato.s.bit.allarme ) return; //02/12/2011 controllo eliminato su richiesta di Simone Guidetti

  //-----------------------------------------------------------------------
  // N.B. Le quote ricevute dal Laser sono in mm. --> quindi divido per 10
  // e converto
  //-----------------------------------------------------------------------
  convertiXY_laser_to_layout( stato.x_laser/10, stato.y_laser/10, &Xly, &Yly);
  Xtgv = (int) Xly;
  Ytgv = (int) Yly;
  PosTgv = stato.pos;
  //-----------------------------------------------------------------------
  // Se il numero di nodo sul DM90 recuperato dal PLC coincide con le
  // coordinate grafiche in tale posizione (circa 50 cm.)allora SKIP !!!
  //-----------------------------------------------------------------------
  Punto = PosTgv;
  if( Punto>0 && Punto<TOTPUNTI && N.n[Punto]->typ==TYP_NODO_RIENTRO ){
     Xgraf = Punti[Punto][0];
     Ygraf = Punti[Punto][1];
     lx = abs(Xtgv-Xgraf);
     ly = abs(Ytgv-Ygraf);
     //-----------------------------------------------
     // 27/11/2013
     // Se il TGV si trova in prossimità di un nodo
     // lo mando come nodo di rientro nel D305
     //
     // AL-23/10/2014
     // Anche in caso di "G" molto basso Simone
     // ha chiesto di passare come nodi di rientro
     // e zona quelle nel punto impostato su D90.
     //-----------------------------------------------
   //if ( lx<DistanzaMAX && ly<DistanzaMAX ) return;
     if ((lx<DistanzaMAX && ly<DistanzaMAX ) || stato.g_laser<1){
        NumCanali=1;
        NumCanaleIniziale=305;
        dm[0]= (WORD)(Punto);
        err = OM.wr_canale(&dm[0], (WORD)(agv_num), "WD", NumCanaleIniziale, NumCanali, all_mess);
        if(err) P.error_trasmit( agv_num, all_mess);
        if( !err && stato.s.bit.aux2 ){
           calcolo_zona_mapping((short int) Punto, &zona);
           NumZona = zona;
           if(!err) err = LaserInvioNumeroSpecchi(agv_num, zone_mapping.Z[NumZona].Minr, all_mess);
           if(!err) err = LaserInvioUltimaZonaMapping(agv_num, NumZona, all_mess);
           if(err) P.error_trasmit(agv_num, all_mess);
        }
        return;
     }
  }
  //-------------------------------------------------
  // Verifica le coordinate del punto più vicino
  // Tra quelli impostati come NODI DI RIENTRO.
  //-------------------------------------------------
  Punto=0;
  old_dist=9999;
  for(i=1; i<=MAXPUNTI; i++){
     if(Punti[i][4]==0                ) continue;
     if(N.n[i]->num<1                 ) continue;
     if(N.n[i]->num>TOTPUNTI          ) continue;
     if(N.n[i]->typ!=TYP_NODO_RIENTRO ) continue;
     //-----------------------------------------
     // Coordinate grafiche punto considerto
     //-----------------------------------------
     Xgraf = Punti[i][0];
     Ygraf = Punti[i][1];
     //-----------------------------------------
     // 06/06/2014 - NODO SICURO
     // Scarto i nodi di percorso troppo vicini
     // ad altri PINT di altri TGV
     //-----------------------------------------
     if(!N.punti_notevoli[i]){
        nodo_non_valido=false;
        for(j=1; j<=MAXAGV; j++){
           if(j==agv_num                                ) continue;
           if(!AGV[j]->test_mission_on()                ) continue;
           if(AGV[j]->mission.pintend<1                 ) continue;
           if(AGV[j]->mission.pintend>TOTPUNTI          ) continue;
           if(N.punti_notevoli[AGV[j]->mission.pintend] ) continue;
           X1 = Punti[AGV[j]->mission.pintend][0];
           Y1 = Punti[AGV[j]->mission.pintend][1];
           lx = abs(X1-Xgraf);
           ly = abs(Y1-Ygraf);
           if(lx>30000 || ly>30000) continue;
           dist = sqrt_algo((lx*lx)+(ly*ly));
           if(dist<DIST_MIN_ROT_DOPPIA_MAX){nodo_non_valido=true; break;}
        }
        if(nodo_non_valido==true) continue;
     }
     //-----------------------------------------
     // Verifico posizione grafica TGV
     //-----------------------------------------
     lx = abs(Xtgv-Xgraf);
     ly = abs(Ytgv-Ygraf);
     if(lx>30000 || ly>30000) continue;
     dist = sqrt_algo((lx*lx)+(ly*ly));
     if(dist<old_dist){
        Punto=i;
        old_dist=dist;
     }
  }
  //-------------------------------------------------
  // Trasmetto nuova posizione su DM305
  //-------------------------------------------------
  if( Punto>0 ){
     //err = trasmetti_new_pos_AGV( Punto, all_mess);
     NumCanali=1;
     NumCanaleIniziale=305;
     dm[0]= (WORD)(Punto);
     err = OM.wr_canale(&dm[0], (WORD)(agv_num), "WD", NumCanaleIniziale, NumCanali, all_mess);
     if(err) P.error_trasmit( agv_num, all_mess);
  }
  //-------------------------------------------------
  // Se cambio Nodo
  // Trasmetto anche la zona e il numero di specchi
  //-------------------------------------------------
  if( !err && Punto>0 && Punto!=PosTgv && stato.s.bit.aux2){
     calcolo_zona_mapping((short int) Punto, &zona);
     NumZona = zona;
     if(!err) err = LaserInvioNumeroSpecchi(agv_num, zone_mapping.Z[NumZona].Minr, all_mess);
     if(!err) err = LaserInvioUltimaZonaMapping(agv_num, NumZona, all_mess);
     if(err) P.error_trasmit(agv_num, all_mess);
  }
}

// --------------------------
//   calcolo_zona_mapping()
// --------------------------
// Dato un nodo e le sue coordinate calcolo la zona_mapping di appartenenza
//
void agv::calcolo_zona_mapping(short int Punto, short int *Zona)
/***********************************/
{
  int lx, ly, diff, old_diff;
  int i, xp, yp, rag1, rag2;
  double xz_ls, yz_ls, xz_ly, yz_ly;
  double QuadratoDiagonale;
  double QuadratoCatetoX;
  double QuadratoCatetoY;

  rag1=0;
  rag2=0;
  diff=0;
  *Zona=0;
  old_diff=999999;
  //---------------------------------------------------------
  // 18/04/2014 Abilitazione Abbinamento Zona Mapping al Nodo
  // Se ZonaMapping impostata sul NODO --> Privilegiamo
  // altrimenti calcolo standard
  //---------------------------------------------------------
  if(Laser_AbilitaZonaMapSuNodo){
     if(Punto<=TOTPUNTI){
        if(N.n[Punto]->ZoneMap>0){ // Se >0 allora forzatura
           if(N.n[Punto]->ZoneMap==255){                      // Se valore impostato=255 --> Forza Zona 0
              *Zona=0;
              return;
           }
           if(N.n[Punto]->ZoneMap< MAXZONEMAPPING){
              if(zone_mapping.Z[N.n[Punto]->ZoneMap].ZAbiltata!=0){   // Se valore valido --> Forza Zona n
                 *Zona=N.n[Punto]->ZoneMap ;
                 return;
              }
           }
        }
     }
  }

  //-------------------------------------------------
  // Recupero le coordinate in "mm" del punto
  // selezionato
  //-------------------------------------------------
  xp = Punti[Punto][0]*10;
  yp = Punti[Punto][1]*10;
  rag1=9999999;
  //-------------------------------------------------
  // Racupero l'area di raggio minore entro la quale
  // si trova il nodo selezionato.
  //-------------------------------------------------
  for(i=0; i<MAXZONEMAPPING; i++){
     if( zone_mapping.Z[i].ZAbiltata<1) continue;
     xz_ls = zone_mapping.Z[i].Xl;
     yz_ls = zone_mapping.Z[i].Yl;
     //rag1  = zone_mapping.Z[i].Rl*1000;
     //------------------------------
     // Conversione coordinate
     //------------------------------
     convertiXY_laser_to_layout(xz_ls, yz_ls, &xz_ly, &yz_ly);
     //------------------------------
     // Calcolo della distanza
     //------------------------------
     lx = abs(xp-xz_ly);
     ly = abs(yp-yz_ly);
     //------------------------------
     // Punti troppo lontani
     // GB-29/03 (1) Commentato perchè anche se il punto era fuori dai 30 mt
     //              deve comunque prendere la zon apiù vicina e NON la zona 0
     //------------------------------
     //if(lx>30000 || ly>30000) continue;
     //------------------------------
     // Calcolo raggio
     //------------------------------
     //rag2 = (int) sqrt_algo((lx*lx)+(ly*ly));
     QuadratoCatetoX= (double)abs(lx);
     QuadratoCatetoX= QuadratoCatetoX * QuadratoCatetoX;
     QuadratoCatetoY= (double)abs(ly);
     QuadratoCatetoY= QuadratoCatetoY * QuadratoCatetoY;

     QuadratoDiagonale=QuadratoCatetoX+QuadratoCatetoY;
     rag2 = (int) sqrt_algo(QuadratoDiagonale);
     //------------------------------
     // Verifico se <= RAGGIO
     //------------------------------
     if(rag2<=rag1){
        diff=rag2;
        if(diff<old_diff){
           *Zona=(short int)(i);
           old_diff=diff;
        }
     }
  }
}

// -------------------------------
//   convertiXY_layout_to_laser()
// -------------------------------
// Converte le coordinate in riferimento LAYOUT in riferimento LASER
//
void agv::convertiXY_layout_to_laser(double *Xls, double *Yls, double Xly, double Yly)
/***********************************/
{
  double X, Y;
  long double grd, rad, sinA, cosA;

  *Xls=0;
  *Yls=0;
  //-----------------------------------------------------------------------
  // Calcolo il SENO e il COSENO dell'angolo tra il rif.LASER e rif.LAYOUT
  //-----------------------------------------------------------------------
  //grd = (long double) 0;
  grd = (long double)(360-zone_mapping.GRD_offset);
  rad = DegToRad(grd);
  SinCos(rad, sinA, cosA);
  if( zone_mapping.GRD_offset==0 ) sinA=0; cosA=1;
  //-----------------------------------------------------------------------
  // Calcolo della traslazione del rif. laser rispetto al rif. layout
  //-----------------------------------------------------------------------
  // Viene corretto *10 e /10 per migliorare la precisione
  // è stato tolto anche la eventuale correzzione sull'offset X Y che in realtà
  // se così non sarebbe corretta --> sarebbe necessario introdurre 2 campi
  // di offset come è stato fatto per i GRADI
  //X  =  (Xly-(zone_mapping.X_0_layout-zone_mapping.X_0_laser));
  //Y  = -(Yly-(zone_mapping.Y_0_layout+zone_mapping.Y_0_laser));
  //*Xls = (X*cosA)-(Y*sinA);
  //*Yls = (X*sinA)+(Y*cosA);
  X  =   Xly*10;
  Y  =  -Yly*10;
  *Xls = ((X*cosA)-(Y*sinA))/10;
  *Yls = ((X*sinA)+(Y*cosA))/10;
}

// ------------------------------
//  convertiXY_laser_to_layout()
// ------------------------------
// Converte le coordinate in riferimento LASER in riferimento LAYOUT
//
void agv::convertiXY_laser_to_layout(double Xls, double Yls, double *Xly, double *Yly)
/***********************************/
{
  double X, Y;
  long double rad, sinA, cosA;

  *Xly=0;
  *Yly=0;
  //-----------------------------------------------------------------------
  // Calcolo il SENO e il COSENO dell'angolo tra il rif.LASER e rif.LAYOUT
  //-----------------------------------------------------------------------
  rad = 0;
  rad = DegToRad(zone_mapping.GRD_offset);

  SinCos(rad, sinA, cosA);
  if( zone_mapping.GRD_offset==0 ) sinA=0; cosA=1;
  //-----------------------------------------------------------------------
  // Calcolo della traslazione del rif. laser rispetto al rif. layout
  //-----------------------------------------------------------------------
  // Viene corretto *10 e /10 per migliorare la precisione
  // è stato tolto anche la eventuale correzzione sull'offset che in realtà
  // se così non sarebbe corretta --> sarebbe necessario introdurre 2 campi
  // di offset come è stato fatto per i GRADI
  //X =  ((Xls*cosA)-(Yls*sinA));
  //Y = -((Xls*sinA)+(Yls*cosA));
  //*Xly = (int)(X)+(zone_mapping.X_0_layout-zone_mapping.X_0_laser);
  //*Yly = (int)(Y)+(zone_mapping.Y_0_layout+zone_mapping.Y_0_laser);
  X =  ((Xls*cosA*10)-(Yls*sinA*10));
  Y = -((Xls*sinA*10)+(Yls*cosA*10));
  *Xly = (int)(X)/10;
  *Yly = (int)(Y)/10;
}

// -------------------------------
//  convertiGRD_layout_to_laser()
// -------------------------------
// Converte le rotazioni dal riferimento LAYOUT al riferimento LASER
//
void agv::convertiGRD_layout_to_laser(double *GRDls, double GRDly)
/***********************************/
{
  double grd;

  //grd=(double)( GRDly );
  grd=(double)( GRDly + zone_mapping.GRD_offset );

  if(grd<0   ) grd=360+grd;
  if(grd>=360) grd=360-grd;
  *GRDls=grd;
}

// ------------------------------
//  convertiGRD_laser_to_layout()
// ------------------------------
// Converte le rotazioni dal riferimento LASER al riferimento LAYOUT
//
void agv::convertiGRD_laser_to_layout(double GRDls, double *GRDly)
/***********************************/
{
  double grd;

  //grd=(double)( GRDls );
  grd=(double)( GRDls - zone_mapping.GRD_offset );

  if(grd<0   ) grd=360+grd;
  if(grd>=360) grd=360-grd;
  *GRDly=grd;
}


// ---------------------------------------------------
//      test_agv_escluso()
//   Verifica se l'AGV Š escluso restituisce 0 in
//   caso negativo.
// ---------------------------------------------------
// Verifica se l'AGV Š escluso. Restituisce 0 in caso negativo.
//
int agv::test_agv_escluso()
/**************************/
{
  // ----------------------------------------------------
  // Ritorna 1 se escluso o se in manuale
  // ----------------------------------------------------
  if(mission.Esclusione) return 1;
  if(!stato.s.bit.funz)  return 1;
  return 0;
}

// -----------------------------------------------------
//      distanza()
// -----------------------------------------------------
// Restituisce la distanza in "num_nodi" tra AGV e "target". Torna 1 in
// caso di errore
//
// tipo_percorso = 0 (TUTTI)
//               = 1 (SOLO STANDARD)
//               = 2 (SOLO VELOCE)
//
int agv::distanza(short int tipo_percorso, short int altro_punto, short int target, short int *nodi_perc, struct DATI_PERCORSO *dati_perc, char *all_mess, short int *num_nod, short int *num_rot)
/*********************************************************/
{
  int stat=0, trovato=0;
  int  x1, x2, y1, y2, offx, offy;
  bool avanzamento_in_rettaX, avanzamento_in_rettaY;
  short int count_rot, prec=0, i, j, source, dest;
  short int old_rotazione=0, old_av=0, old_ind=0, old_ral=0, NodoPrec, RotFinale;
  struct NOD nod;

  memset(&nod, 0, sizeof(NOD));
  //----------------- definisce Source (pos.AGV) e Dest
  if(altro_punto==0){
     source = stato.pos;
     //------------------------------------
     // Recupero rotazione attuale da
     // stato TGV
     //------------------------------------
     if(stato.s.bit.grd0==true  ) old_rotazione=0;
     if(stato.s.bit.grd90==true ) old_rotazione=1;
     if(stato.s.bit.grd180==true) old_rotazione=2;
     if(stato.s.bit.grd270==true) old_rotazione=3;
  }
  else source = altro_punto;

  //----------------------------------------------
  // Compilazione array punti missione
  //----------------------------------------------
  i=0;
  *num_nod=MAXPERCORSI+3;   // inizializzo a un valore ALTISSIMO per ritornare una distanza in caso di errore
  *num_rot=0;
  count_rot=0;
  dest = target;
  do{
     if( i>0 ){
        prec   = nod.num;      // punto precedente
        source = dati_path.ps; // punto successivo
     }
     stat = N.cerca_record(&nod, source );
     //----------------------------------
     //  Errore punto non trovato
     //----------------------------------
     if(stat){
        sprintf(all_mess, MESS[116] , agv_num, source, dest, prec);
        return 0;
     }
     //------------------------------------------------------------------
     //  14/02/2018B GB Compilazione di una rotazione precedente  0-1-2-3
     //  Simile a old_rotazione ma valuta anche la posizione TGV
     //------------------------------------------------------------------
     //trovato = N.cerca_path_into_nodo(tipo_percorso, PercorsiSpeciali, mission.pstart, dest, &nod, &dati_path);
     RotFinale=99;
     if(i==0)      NodoPrec=nod.num;    // punto attuale
     if(i>=1)      NodoPrec=prec;       // punto precedente
     N.CalcolaRotUscita(agv_num, i, NodoPrec, dati_path, &RotFinale);
     trovato = N.cerca_path_into_nodo(tipo_percorso, PercorsiSpeciali, mission.pstart, dest,  NodoPrec, RotFinale, &nod, &dati_path);
     //----------------------------------------------
     // RICERCA PERCORSO STANDARD
     // Verifico se il percorso trovato
     //----------------------------------------------
     if(tipo_percorso==STD_PATH && (source != dest) && (trovato<0 || trovato>PERC_ALTERNATIVO_VELOCE)){
        sprintf(all_mess, MESS[117], agv_num, source, dest, prec);
        return 0;
     }
     //----------------------------------------------
     // RICERCA PERCORSO VELOCE
     // Verifico se il percorso trovato
     //----------------------------------------------
     if(tipo_percorso==FAST_PATH && (source != dest) && trovato<0){
        sprintf(all_mess, MESS[117], agv_num, source, dest, prec);
        return 0;
     }
     //----------------------------------------------
     // Recupero coordinate dei punti attuale a succ
     //----------------------------------------------
     x1 = Punti[ nod.num      ][0];
     y1 = Punti[ nod.num      ][1];
     x2 = Punti[ dati_path.ps ][0];
     y2 = Punti[ dati_path.ps ][1];
     offx=abs(x1-x2);
     offy=abs(y1-y2);
     avanzamento_in_rettaX = false;
     avanzamento_in_rettaY = false;
     if((x1!=x2 && offy<=5) || (y1!=y2 && offx<=5)){
        if(offx<=5) avanzamento_in_rettaY=true;
        if(offy<=5) avanzamento_in_rettaX=true;
     }
     //----------------------------------------------
     //  Errore indicazioni target non presenti
     //  nŠ come PS, ZP o ZS
     //----------------------------------------------
  // if((trovato <0) && (source != dest)){
  //    sprintf(all_mess, MESS[117], agv_num, source , target, prec );
  //    return 0;
  // };
     //----------------------------------------------
     // Errore punto successivo = 0
     //----------------------------------------------
     if((dati_path.ps == 0) && (source != dest)){
        sprintf(all_mess, MESS[118], agv_num, source, dati_path.ps, target, prec );
        return 0;
     };
     //--------------------------------------------------------
     // Errore indicazioni punto sucessivo == posizione attuale
     //--------------------------------------------------------
     if((dati_path.av == dati_path.ind) && (source != dest)){
        sprintf(all_mess, MESS[119], agv_num, source, dest, prec);
        return 0;
     };
     //-----------------------------------------------------
     // Errore direzione opposta a punto precedente 10/07/95
     //-----------------------------------------------------
     if(!mission.GestioneLaser){
        if((dati_path.rot == old_rotazione) &&
           (dati_path.av  != old_av ) &&
           (dati_path.ind != old_ind) &&
           (i>0) &&
//         (source != dest)){
           (source != dest) && dati_path.ps==prec){  // necessario per salto nodi ma causa mancata
                                                     // segnalazione problemi rotazione
              sprintf(all_mess, MESS[120], agv_num, source, dest, prec);
              return 0;
        }
        //--------------------------------------------------------
        // Errore direzione opposta a punto precedente 03/12/2010
        //--------------------------------------------------------
        if((dati_path.rot != old_rotazione) &&
          ((dati_path.rot==0 && old_rotazione==2) ||
           (dati_path.rot==1 && old_rotazione==3) ||
           (dati_path.rot==2 && old_rotazione==0) ||
           (dati_path.rot==3 && old_rotazione==1)) &&
           (dati_path.av  == old_av ) &&
           (dati_path.ind == old_ind) &&
           (i>0) &&
//         (source != dest)){
           (source != dest) && dati_path.ps==prec){  // necessario per salto nodi ma causa mancata
                                                     // segnalazione problemi rotazione
              sprintf(all_mess, MESS[120], agv_num, source, dest, prec);
              return 0;
        }
     }
     //-----------------------------------------------------
     // Errore Rallentare prima di cambio rotazione 11/07/95
     //-----------------------------------------------------
/*
     if((dati_path.rot != old_rotazione) && (old_ral == 0) && (i>0)){
        sprintf(all_mess, MESS[121], agv_num, source, dest, prec);
        return 0;
     }
*/
     //-----------------------------------------------------
     // Errore distanza inferiore a 30cm
     //-----------------------------------------------------
     if(dati_path.dist<DIST_MIN_ING_CURVA && (source != dest)){
        sprintf(all_mess, MESS[129], agv_num, dati_path.dist, source, dati_path.ps, DIST_MIN_ING_CURVA);
        return 0;
     }
     //-----------------------------------------------------
     // Se un tratto risulta in retta verifica se le
     // rotazioni sono coerenti:
     // Retta X ==> 90° o 270°.
     // Retta Y ==>  0° o 180°.
     //-----------------------------------------------------
     if((avanzamento_in_rettaX==true && dati_path.rot!=1 && dati_path.rot!=3) ||
        (avanzamento_in_rettaY==true && dati_path.rot!=0 && dati_path.rot!=2)){
        if(avanzamento_in_rettaX==true) sprintf(all_mess, "TGV %d errore in nodo %d succ %d (dest=%d) retta X con rot %d", agv_num, nod.num, dati_path.ps, dest, dati_path.rot);
        if(avanzamento_in_rettaY==true) sprintf(all_mess, "TGV %d errore in nodo %d succ %d (dest=%d) retta Y con rot %d", agv_num, nod.num, dati_path.ps, dest, dati_path.rot);
        return 0;
     }
     //-----------------------------------------------------
     // Conteggio della variazione di rotazioni lungo il
     // percorso
     //-----------------------------------------------------
     if(dati_path.rot != old_rotazione) count_rot++;
     //-----------------------------------------------------
     // Memorizzazione delle informazioni di percorso
     //-----------------------------------------------------
     old_rotazione  = dati_path.rot;
     old_av         = dati_path.av;
     old_ind        = dati_path.ind;
     old_ral        = dati_path.ral;
     //-------------------------------------------------------------------
     // Err. punto RICORSIVO (esist. precedentemente nel percorso)
     //-------------------------------------------------------------------
     for(j=1; j<i; j++){
        if((nodi_perc[j] == dati_path.ps) && (source != dest)){
           sprintf(all_mess, MESS[122], agv_num, source, dati_path.ps, dest, prec );
           return 0;
        }
     }
     //----------------------------------------------
     // Errore percorso troppo lungo
     //----------------------------------------------
     if(i>MAXPERCORSI){
        sprintf(all_mess, MESS[125], agv_num, source, i, target);//AGV %d func. distanza interr.en el nodo %d recorrido demasiado largo: %d (dest=%d)
        return 0;
     };
     //----------------------------------------------------------
     // Compilazione di un array con tutti i nodi della missione
     //----------------------------------------------------------
     nodi_perc[i]=source;
     //----------------------------------------------------------
     // Gestione uscita su percorso per missione su stesso nodo
     //----------------------------------------------------------
     if(source == dest && i==0 && dati_path.pt==dest){
        source  = dati_path.ps;
        nod.num = dati_path.ps;
     }
     //----------------------------------------------------------
     // Recupero dei dati di percorso associati al nodo
     //----------------------------------------------------------
     dati_perc[i].ps            = dati_path.ps;
     dati_perc[i].dir           = dati_path.ind;
     dati_perc[i].rot           = dati_path.rot;
     dati_perc[i].ral           = dati_path.ral;
     dati_perc[i].dist          = dati_path.dist;
     dati_perc[i].salta_a_ps    = 0;  // salta a nodo per distanza bassa
     dati_perc[i].salta_a_dist  = 0;  // salta a distanza
     i++;                // incrementa conteggio Nodi
  }while(source != dest);
  *num_nod = (short int)(i-1);           // distanza come numero di nodi
  *num_rot = (short int)(count_rot-1);   // rotazioni eseguite nel percorso
  return 0;
}


// -----------------------------------
//         SelezioneTipoPath()
// -----------------------------------
// Verifica se ci sono le condizioni per intraprendere il percorso
// veloce (FAST_PATH) o se è meglio intraprendere il percorso
// ad anello (STD_PATH)
//
int agv::SelezioneTipoPath(bool deadlock, short int forza_tipo_path, short int pstart, short int pend, bool *forza_tutto, bool *tutto_libero)
{
  int err, i, j;
  char all_mess[101]="";
//short int p1, p2;
//int x1, y1, x2, y2;
//double DistToStart, Dist, DistX, DistY;
  short int altro_agv, MinPercorsi, MinPercToStart, nodo_interr;
  short int pintend, prec_pintend, succ_pintend, succ1_pintend, agv_bloccante, ultimo_nodo;
  short int dist, rot, count, rot_pintend;
  short int perc[MAXPERCORSI];
  struct DATI_PERCORSO dati[MAXPERCORSI];
  short int new_perc[MAXPERCORSI];
  short int app_perc[MAXPERCORSI];
  struct DATI_PERCORSO new_dati[MAXPERCORSI];
  short int magazzino, baia, num_pos;
  bool ok_sposta;
  bool blocco_a_vicenda;
  bool nodo_fuori_ingombro;
  bool tgv_in_senso_opposto;
  bool missione_con_path_veloce;

  err=0;
  *forza_tutto  = false;
  *tutto_libero = false;
  //-------------------------------------------------------------------
  // Disabilita gestione percorsi veloci
  //-------------------------------------------------------------------
  if(AbilitaGestionePercorsiVeloci==false) return STD_PATH;

  //-------------------------------------------------------------------
  // Missione di sblocco DEAD-LOCK sicuramente percorso STANDARD
  //-------------------------------------------------------------------
  if(deadlock==true) return STD_PATH;

  //-------------------------------------------------------------------
  // Forzatura tipo percorso da gestioni esterne e/o speciali
  //-------------------------------------------------------------------
  if(forza_tipo_path!=CALCOLA_TIPO_PATH) return forza_tipo_path;

  //-------------------------------------------------------------------
  // DEST FUORI INGOMBRO = STANDARD
  // Se il punto di destinazione è fuori ingombro, il percorso deve
  // essere ad anello
  //-------------------------------------------------------------------
  nodo_fuori_ingombro=false;
  for(i=0; i<MAXNODIATTESA; i++){
     if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]<1         ) continue;
     if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]>MAXPUNTI  ) continue;
     if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]!=pend     ) continue;
     nodo_fuori_ingombro=true;
     break;
  }
  if(nodo_fuori_ingombro==true) return STD_PATH;

  //-------------------------------------------------------------------
  // Recupero l'ipoterico percorso veloce per fare ulteriori valutaz.
  //-------------------------------------------------------------------
  dist=rot=0;
  memset(&perc, 0, sizeof(perc));
  memset(&dati, 0, sizeof(dati));
  memset(&app_perc, 0, sizeof(app_perc));
  memset(&new_perc, 0, sizeof(new_perc));
  memset(&new_dati, 0, sizeof(new_dati));
  err = distanza(FAST_PATH, pstart, pend, &perc[0], &dati[0], all_mess, &dist, &rot);
  if(err!=0 || dist>MAXPERCORSI) return STD_PATH;
  //-------------------------------------------------------------------
  // Verifico se nei punti di percorso esiste almeno una riga
  // abbinata a un precorso veloce
  //-------------------------------------------------------------------
  missione_con_path_veloce=false;
  for(i=0; i<MAXPERCORSI; i++){
     if(perc[i]<1       ) break;
     if(perc[i]>TOTPUNTI) break;
     for(j=0;j<MAXPUNTINODO;j++){
        if(N.n[perc[i]]->rec[j].ps==0                       ) break;
        if(N.n[perc[i]]->rec[j].ps!=dati[i].ps              ) continue;
        if(N.n[perc[i]]->rec[j].pt!=PERC_ALTERNATIVO_VELOCE &&
           N.n[perc[i]]->rec[j].zp!=PERC_ALTERNATIVO_VELOCE ) continue;
        //------------------------------------------
        // Missione con percorso veloce
        //------------------------------------------
        missione_con_path_veloce=true;
        break;
     }
     if(missione_con_path_veloce==true) break;
  }
  //-------------------------------------------------------------------
  // Non esiste percorso veloce nella misssione considerata
  //-------------------------------------------------------------------
  if(missione_con_path_veloce==false) return STD_PATH;
  //-------------------------------------------------------------------
  // VERIFICA PRESENZA CURVE
  // Verifica presenza di curve lungo il percorso
  //
  // AL-22/02 aggiorno anche i dati_perc con eventuali curve/deviazioni
  //          in questo modo l'occupa_nodi sarà attendibile
  //-------------------------------------------------------------------
  if(mission.GestioneLaser==true){
     err = P.ImpostazioneCurve(perc, dati, new_perc, new_dati, all_mess);
     if(err!=0) return STD_PATH;
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
  //
  // AL-22/02 aggiorno anche i dati_perc con eventuali curve/deviazioni
  //          in questo modo l'occupa_nodi sarà attendibile
  //-------------------------------------------------------------------
  memset(&new_perc, 0, sizeof(new_perc));
  memset(&new_dati, 0, sizeof(new_dati));
  err = P.ImpostazioneDeviazioni(agv_num, perc, dati, new_perc, new_dati, all_mess);
  if(err!=0) return STD_PATH;
  //-------------------------------------------------------------------
  // Modifico il percorso revisionato
  //-------------------------------------------------------------------
  if(memcmp(&new_perc[0], &app_perc[0], sizeof(app_perc))!=0){
     memcpy(&perc, &new_perc, sizeof(perc));
     memcpy(&dati, &new_dati, sizeof(dati));
  }

  //-------------------------------------------------------------------
  // Se il TGV deve compiere meno di 3 rotazioni verifico se è
  // occupabile fino a destinazione
  //-------------------------------------------------------------------
  pintend=agv_bloccante=0;
  //-------------------------------------------------------------------
  // Compilo temporaneamente i dati di missione
  //-------------------------------------------------------------------
  mission.pintstart = pstart;
  mission.pstart    = pstart;
  mission.pend      = pend;
  memcpy(&mission.punto[0],     &perc[0], sizeof(mission.punto));
  memcpy(&mission.dati_perc[0], &dati[0], sizeof(mission.dati_perc));
  //-------------------------------------------------------------------
  // Tento di occupare  i nodi fino a destinazione
  //-------------------------------------------------------------------
  occupa_nodi(all_mess, TRUE, &agv_bloccante);
  //-------------------------------------------------------------------
  // Memorizzo il PInt raggiunto e il precedente
  //-------------------------------------------------------------------
  pintend      = mission.pintend;
  prec_pintend = 0;
  succ_pintend = 0;
  succ1_pintend= 0;
  rot_pintend  = 99;  //inizializzazione a rot impossibile
  for(i=1; i<MAXPERCORSI; i++){
     if(mission.punto[i+1]==0      ) break;
     if(mission.punto[i]!=pintend  ) continue;
     prec_pintend  = mission.punto[i-1];
     succ_pintend  = mission.punto[i+1];
     succ1_pintend = mission.punto[i+2];
     rot_pintend   = rot_standard(i-1);
     break;
  }
  //-------------------------------------------------------------------
  // Resetto nuovamente la missione in RAM
  //-------------------------------------------------------------------
  reset_ram_mission();
  free_all_nodi(all_mess);
  //-------------------------------------------------------------------
  // MEMORIZZO PERCORSO LIBERO FINO A DESTINAZIONE
  //-------------------------------------------------------------------
  if(pintend==pend && agv_bloccante==0) *tutto_libero = true;
  //-------------------------------------------------------------------
  // PERCORSO VELOCE OCCUPABILE FINO A DESTINAZIONE
  // nessuna navetta sul percorso e pint che coincide con la
  // destinazione e poche rotazioni.
  // (le poche rotazioni per garantire rapidità nel raggiungimento
  //  della destinazione)
  //-------------------------------------------------------------------
  if(rot<3 && pintend==pend && agv_bloccante==0){
     //-------------------------------------------------------
     // Se è abilitata l'occupazione dei nodi sui PATH VELOCI
     // e la missione ne prevede uno, cerco di occupare fino
     // a destinazione
     //-------------------------------------------------------
     if(OccupaTuttoSuPathVeloci==true) *forza_tutto=true;
     return FAST_PATH;
  }
  //-------------------------------------------------------------------
  // GESTIONE TGV BLOCCANTE
  //
  // ATTENZIONE: aggiorno la struttura AgvBloccati[] solo se
  //             l'agv_bloccante ritorna dalla funzione occupa_nodi.
  //-------------------------------------------------------------------
  blocco_a_vicenda=false;
  if(agv_bloccante>0 && agv_bloccante<=MAXAGV){
     AgvBloccati[agv_num]=agv_bloccante;
     if(AgvBloccati[agv_num]==agv_bloccante && AgvBloccati[agv_bloccante]==agv_num) blocco_a_vicenda=true;
  }
  //-------------------------------------------------------------------
  // BLOCCO TGV A VICENDA
  // Obbligo di percorso standard da parte del TGV considerato
  //-------------------------------------------------------------------
  if(blocco_a_vicenda==true) return STD_PATH;
  //-------------------------------------------------------------------
  // Se occupa_nodi non ha rilevato nessun TGV
  // bloccante, ma il nodo successivo al pint
  // risulta occupato da un altro TGV senza missione
  // in corso memorizzo quello come bloccante
  //-------------------------------------------------------------------
  if(agv_bloccante==0 && succ_pintend!=0 && succ_pintend!=mission.pend && succ1_pintend!=mission.pend){
     if(N.nodo_busy[succ_pintend]!=0 && N.nodo_busy[succ_pintend]!=agv_num){
        agv_bloccante = N.nodo_busy[succ_pintend];
        //---------------------------------------------------
        // Se il tgv che occupa il nodo di fronte al PINT
        // ha la missione in corso o è ruotato nella direz
        // di arrivo del TGV considerato, non è ritenuto
        // bloccante
        //---------------------------------------------------
        if(agv_bloccante!=0 && AGV[agv_bloccante]->test_mission_on()                   ) agv_bloccante=0;
        if(agv_bloccante!=0 && rot_pintend==0 && AGV[agv_bloccante]->stato.s.bit.grd0  ) agv_bloccante=0;
        if(agv_bloccante!=0 && rot_pintend==1 && AGV[agv_bloccante]->stato.s.bit.grd90 ) agv_bloccante=0;
        if(agv_bloccante!=0 && rot_pintend==2 && AGV[agv_bloccante]->stato.s.bit.grd180) agv_bloccante=0;
        if(agv_bloccante!=0 && rot_pintend==3 && AGV[agv_bloccante]->stato.s.bit.grd270) agv_bloccante=0;
     }
     if(agv_bloccante==0 && N.nodo_busy[succ1_pintend]!=0 && N.nodo_busy[succ1_pintend]!=agv_num){
        agv_bloccante = N.nodo_busy[succ1_pintend];
        //---------------------------------------------------
        // Se il tgv che occupa il nodo di fronte al PINT
        // ha la missione in corso o è ruotato nella direz
        // di arrivo del TGV considerato, non è ritenuto
        // bloccante
        //---------------------------------------------------
        if(agv_bloccante!=0 && AGV[agv_bloccante]->test_mission_on()                   ) agv_bloccante=0;
        if(agv_bloccante!=0 && rot_pintend==0 && AGV[agv_bloccante]->stato.s.bit.grd0  ) agv_bloccante=0;
        if(agv_bloccante!=0 && rot_pintend==1 && AGV[agv_bloccante]->stato.s.bit.grd90 ) agv_bloccante=0;
        if(agv_bloccante!=0 && rot_pintend==2 && AGV[agv_bloccante]->stato.s.bit.grd180) agv_bloccante=0;
        if(agv_bloccante!=0 && rot_pintend==3 && AGV[agv_bloccante]->stato.s.bit.grd270) agv_bloccante=0;
     }
  }
  //-------------------------------------------------------------
  // AGV BLOCCANTE NON IN MISSIONE
  // Vedo se riesco a spostarlo, altrimenti STANDARD
  //-------------------------------------------------------------
  if(agv_bloccante>0 && agv_bloccante<=MAXAGV && !AGV[agv_bloccante]->test_mission_on()){
     //---------------------------------------
     // Non sposto i TGV in attesa sulla
     // macchine
     //---------------------------------------
     ok_sposta=true;
     if(N.punti_notevoli[AGV[agv_bloccante]->stato.pos]==true){
        ok_sposta=false;
        baia=magazzino=num_pos=0;
        M.test_punto_presente_mag(all_mess, AGV[agv_bloccante]->stato.pos, &magazzino, &num_pos);
        P.test_punto_presente_baia(all_mess, AGV[agv_bloccante]->stato.pos, &baia);
        if(magazzino>0 && magazzino<=TOTMAG    ) ok_sposta=true;
        if(baia>0      && baia<=TOTBAIE        ) ok_sposta=true;
     }
     //---------------------------------------
     // Se l'agv_bloccante è sul percorso
     // ruotato di 180° rispetto all'arrivo
     // della navetta considerata scelgo
     // il PATH_STANDARD
     //---------------------------------------
     else{
        if(rot_pintend==0 && AGV[agv_bloccante]->stato.s.bit.grd180) ok_sposta=false;
        if(rot_pintend==1 && AGV[agv_bloccante]->stato.s.bit.grd270) ok_sposta=false;
        if(rot_pintend==2 && AGV[agv_bloccante]->stato.s.bit.grd0  ) ok_sposta=false;
        if(rot_pintend==3 && AGV[agv_bloccante]->stato.s.bit.grd90 ) ok_sposta=false;
        //-----------------------------------------------------
        // Se la navetta bloccante è lungo il percorso ruotata
        // in senso opposto all'arrivo del TGV considerato
        // faccio scegliere al TGV considerato il PATH_STANDARD
        //-----------------------------------------------------
        if(ok_sposta==false) return STD_PATH;
     }
     //---------------------------------------
     // Tgv spostabile
     //---------------------------------------
     if(ok_sposta==true){
        AGV[agv_bloccante]->SpostaTGVInAttesa();
        //--------------------------------------------------
        // Se NON sono riuscito a spostare il TGV bloccante
        // obbligo il TGV considerato al percorso standard
        //--------------------------------------------------
        if(!AGV[agv_bloccante]->test_mission_on()) return STD_PATH;
     }
  }
  //-------------------------------------------------------------
  // AGV BLOCCANTE IN SENSO OPPOSTO A TGV SU PINT
  // 1 - Verifico se nella missione del TGV bloccante compare il
  //     PINT del tgv considerato in senso opposto al verso di
  //     percorrenza.
  // 2 - Se il TGV considerato non ha nemmeno le condizioni per
  //     uscire dal Punto in cui si trove, verifico se la
  //     destinazione del TGV bloccante è limitrofa, alla dest
  //     del TGV e in quel caso prendo il percorso standard,
  //     altrimenti aspetto sul PT di fare il percorso veloce
  //-------------------------------------------------------------
  if(agv_bloccante>0 && agv_bloccante<=MAXAGV && AGV[agv_bloccante]->test_mission_on()){
/*
     //----------------------------------------------------------
     // 1 - IL TGV CONSIDERATO e TGV BLOCCANTE AVREBBERO PINT.
     //     TROPPO VICINI
     //     Verifico se l'altro TGV ha come destinazione un
     //     punto in prossimità di dove si trova quello
     //     considerato
     //----------------------------------------------------------
     p1 = pintend;
     p2 = AGV[agv_bloccante]->mission.pintend;
     x1 = Punti[p1][0];
     y1 = Punti[p1][1];
     x2 = Punti[p2][0];
     y2 = Punti[p2][1];
     //----------------------------------------------------------
     // Calcolo delle distanze da punto a punto
     //----------------------------------------------------------
     DistX = abs(y1-y2);
     DistY = abs(x1-x2);
     if(DistX>30000 || DistY>30000) return STD_PATH;
     Dist = (double)(sqrt_algo((DistX*DistX)+(DistY*DistY)));
     //----------------------------------------------------------
     // Sicurezza controllo che la distanza tra la pos del TGV
     // considerato e quella del TGV bloccanto sia superiore a
     // DIST_MIN_ROT_DOPPIA_MAX.
     //----------------------------------------------------------
     if(Dist<DIST_MIN_ROT_DOPPIA_MAX) return STD_PATH;
*/
     //----------------------------------------------------------
     // 2 - PERCORSO UN MINIMO OCCUPABILE
     //----------------------------------------------------------
     altro_agv=MinPercorsi=MinPercToStart=nodo_interr=0;
     test_percorso_disponibile(&perc[0], &dati[0], &altro_agv, &MinPercorsi, &MinPercToStart, &nodo_interr, all_mess);
     //----------------------------------------------------------
     // 3 - IL TGV CONSIDERATO SI FERMA LUNGO IL PERCORSO.
     //     controllo che le 2 navette non si vengano incontro
     //----------------------------------------------------------
     if(prec_pintend!=0){
        tgv_in_senso_opposto=false;
        for(i=1; i<MAXPERCORSI-1; i++){
           if(AGV[agv_bloccante]->mission.punto_prenotato[i]==0                                                                         ) break;
           if(AGV[agv_bloccante]->mission.punto_prenotato[i]!=prec_pintend && AGV[agv_bloccante]->mission.punto_prenotato[i-1]!=pintend ) continue;
           tgv_in_senso_opposto=true;
           break;
        }
     }
     //----------------------------------------------------------
     // Se il TGV non riesce a occupare un minimo di percorsi
     // e la navetta bloccante viene in senso opposto --> STD
     //----------------------------------------------------------
     if(MinPercorsi<MIN_NODI_VERIFICA_PATH_VELOCE && tgv_in_senso_opposto==true ) return STD_PATH;
     //----------------------------------------------------------
     // 21/11/2019 GB-Se AGV NON riesce a occupare nessun NODO
     //            Allora tenta percorso STANDARD
     //----------------------------------------------------------
     if(pintend>0 && pintend==pstart) return STD_PATH;
  }
  //-------------------------------------------------------------
  // Se non si sono verificate le condizioni di scelta del
  // percorso standard imbocco quello rapido
  //-------------------------------------------------------------
  return FAST_PATH;
}

// -----------------------------------
//   AvvicinamentoADestinazione()
// -----------------------------------
// Cerca un punto di attesa tra una sorgente e una destinazione
//
void agv::AvvicinamentoADestinazione(short int tipo_percorso, short int dest, short int passa_da, short int *pint)
{
  int err, i, j;
  char all_mess[101]="";
  short int dist, rot;
  short int perc[MAXPERCORSI];
  struct DATI_PERCORSO dati[MAXPERCORSI];
  short int perc1[MAXPERCORSI];
  struct DATI_PERCORSO dati1[MAXPERCORSI];
  short int perc2[MAXPERCORSI];
  struct DATI_PERCORSO dati2[MAXPERCORSI];
  bool no_passa_da_altro_punto;

  err=0;
  *pint=0;
  //-------------------------------------------------
  // Recupero l'ipoterico percorso veloce per
  // fare ulteriori valutazioni
  //-------------------------------------------------
  dist=rot=0;
  no_passa_da_altro_punto=true;
  memset(&perc,  0, sizeof(perc));
  memset(&dati,  0, sizeof(dati));
  memset(&perc1, 0, sizeof(perc1));
  memset(&dati1, 0, sizeof(dati1));
  memset(&perc2, 0, sizeof(perc2));
  memset(&dati2, 0, sizeof(dati2));
  //-------------------------------------------------------------------
  // PERCORSO STANDARD DA SORGENTE A DESTINAZIONE
  //-------------------------------------------------------------------
  if(passa_da<1 || passa_da>TOTPUNTI){
     err = distanza(tipo_percorso, 0, dest, &perc[0], &dati[0], all_mess, &dist, &rot);
     if(err!=0 || dist>MAXPERCORSI) return;
  }
  //-------------------------------------------------------------------
  // !!! PASSAGGIO DA NODO !!!
  // solo se non è previsto nodo di attesa
  //-------------------------------------------------------------------
  else{
     no_passa_da_altro_punto=false;
     //-----------------------------------------------
     // recupero percorso da sorg a "passa_da"
     //-----------------------------------------------
     err = distanza(tipo_percorso, 0, passa_da, &perc1[0], &dati1[0], all_mess, &dist, &rot);
     if(err!=0 || dist>MAXPERCORSI) no_passa_da_altro_punto=true;
     //-----------------------------------------------
     // recupero percorso da "passa_da" a dest
     //-----------------------------------------------
     err = distanza(tipo_percorso, passa_da, dest, &perc2[0], &dati2[0], all_mess, &dist, &rot);
     if(err!=0 || dist>MAXPERCORSI) no_passa_da_altro_punto=true;
     //-----------------------------------------------
     // Compilazione percorso con passaggio da
     // altro punto
     //-----------------------------------------------
     if(no_passa_da_altro_punto==false){
        j=0;
        // (1) - PERCORSO FINO A "ALTRO NODO"
        for(i=0; i<MAXPERCORSI; i++){
           if(perc1[i+1]==0) break;   // prendo tutti i nodi a parte l'ultimo
           perc[j] = perc1[i];
           memcpy(&dati[j], &dati1[i], sizeof(DATI_PERCORSO));
           j++;
        }
        // (2) - PERCORSO DA "ALTRO NODO" A DEST
        for(i=0; i<MAXPERCORSI; i++){
           if(perc2[i]==0) continue;   // prendo tutti i nodi
           perc[j] = perc2[i];
           memcpy(&dati[j], &dati2[i], sizeof(DATI_PERCORSO));
           j++;
        }
     }
  }

  //---------------------------------------------------
  // 24/04/2015 Se avvicinamento NON abilitato -->SKIP
  //---------------------------------------------------
  if(AbilitaAvvicinamentoADest==0) return;
  //---------------------------------------------------
  // Verfico qual'è l'ultimo nodo fuori ingombro
  // che incontro lungo il percorso fino a dest
  //---------------------------------------------------
  for(i=MAXPERCORSI-1; i>0; i--){
     if(perc[i]==0) continue;
     //------------------------------------------------------------------
     // Se il TGV è già in posizione fuori ingombro --> skip!
     //------------------------------------------------------------------
     for(j=0; j<MAXNODIATTESA; j++){
        if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]<1                   ) continue;
        if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]>MAXPUNTI            ) continue;
        if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]!=perc[i]            ) continue;
        *pint=perc[i];
        break;
     }
     if(*pint!=0) break;
  }
  //-------------------------------------------------
  // Posizionamento davanti a STAZIONE
  //-------------------------------------------------
  AvvicinamentoADestinazione_IMPIANTI(dest, &perc[0], &dati[0], pint);

  return;
}
// -----------------------------------
//         Rallentamenti()
// -----------------------------------
// Inserimento dei rallentamenti in automatico.
//
void agv::Rallentamenti()
{
  short int i;
  bool  NodoFinale;
  bool  inversione_rotazione;
  short int ral1, rot1, rot2, rot3;
  struct DATI_PERCORSO d1;
  struct DATI_PERCORSO d2;
  struct DATI_PERCORSO d3;

  ral1=rot1=rot2=rot3=0;
  memset(&d1, 0, sizeof(DATI_PERCORSO));
  memset(&d2, 0, sizeof(DATI_PERCORSO));
  memset(&d3, 0, sizeof(DATI_PERCORSO));
  //----------------------------------------
  // GESTIONE_ASRV
  // no calcolo rallentamente automatici
  //----------------------------------------
  #ifdef GESTIONE_ASRV
     return;
  #endif;
  //----------------------------------------
  // Analisi di tutto il percorso del TGV
  //----------------------------------------
  for(i=0; i<MAXPERCORSI; i++){
     if(mission.punto[i+1]==0) break;
     NodoFinale=false;
     memcpy(&d1, &mission.dati_perc[i],   sizeof(DATI_PERCORSO));
     memcpy(&d2, &mission.dati_perc[i+1], sizeof(DATI_PERCORSO));
     if(i<MAXPERCORSI-2) memcpy(&d3, &mission.dati_perc[i+2], sizeof(DATI_PERCORSO));
     //-------------------------------------------------
     // Stato rotazioni:
     // (1) PUNTO CONSIDERATO
     // (2) PUNTO SUCESSIVO
     // (3) PUNTO SUCESSIVO SUCESSIVO
     //-------------------------------------------------
     ral1    = d1.ral;
     rot1    = d1.rot;
     rot2    = d2.rot;
     rot3    = d3.rot;
     //----------------------------------------------------
     // GESTIONE DEVIAZIONI
     // Sul punto considerato devo gestire la rotazione
     // come già compiuta dal TGV
     //----------------------------------------------------
     // (1) PUNTO CONSIDERATO
     switch(d1.rot){
        case  0: rot1 = 0;
                 break;
        case  1: rot1 = 1;
                 break;
        case  2: rot1 = 2;
                 break;
        case  3: rot1 = 3;
                 break;
        case 50: rot1 = 0;
                 break;
        case 51: rot1 = 1;
                 break;
        case 52: rot1 = 2;
                 break;
        case 53: rot1 = 3;
                 break;
        case 54: rot1 = 0;
                 break;
        case 55: rot1 = 1;
                 break;
        case 56: rot1 = 2;
                 break;
        case 57: rot1 = 3;
                 break;
        case 60: rot1 = 0;
                 break;
        case 61: rot1 = 1;
                 break;
        case 62: rot1 = 2;
                 break;
        case 63: rot1 = 3;
                 break;
        case 64: rot1 = 0;
                 break;
        case 65: rot1 = 1;
                 break;
        case 66: rot1 = 2;
                 break;
        case 67: rot1 = 3;
                 break;
        default: break;
     }
     //-------------------------------------------------
     // Nodo Finale
     //-------------------------------------------------
     if(i<MAXPERCORSI-2 && mission.punto[i+2]==0) NodoFinale=true;
     //-------------------------------------------------
     // Nodo Finale
     //-------------------------------------------------
     if(i<MAXPERCORSI-1 && mission.punto[i+1]==0) NodoFinale=true;
     //-------------------------------------------------
     // Gestione rallentamenti:
     // + 10: inversione solo rotazioni standard
     // + 20: inversione solo deviazioni
     // + 30: inversione deviazioni + rotazioni
     //-------------------------------------------------
     if(ral1>9) d1.ral=d1.ral-10;

     //----------------------------------------------------
     // Se la linea di percorso ha i rallentamenti
     // predisposti per essere calcolati in automatico
     // nodo sucessivo.
     // RALLENTAMENTO=3  --> Piano Obliquo.
     // RALLENTAMENTO=4  --> Tratto retto con rallentamento
     //                      obligatorio.
     // RALLENTAMENTO=5  --> Percorso Curva NON in Ingombro
     //
     // RALLENTAMENTO>9  --> Inversione ROTAZIONE
     //                      oraria      --> antioraria
     //                      anti-oraria --> oraria
     //----------------------------------------------------
     // d1.ral==RALL_CURVA_NON_IN_INGOMBRO // 07/01/2016 GB
     if(d1.ral<3 || d1.ral==9){
        //-------------------------------------------------
        // Recupero lo stato delle rotazioni dei nodi e
        // dei loro sucessivi.
        //-------------------------------------------------
        d1.ral = 0;
        //-------------------------------------------------
        // Se un nodo di percorso e il sucessivo hanno
        // rotazioni differenti inserisco "ral = 1".
        //-------------------------------------------------
        if(rot1!=rot2    ) d1.ral = 1;
        //-------------------------------------------------
        // Se c'è un cambio di direzione "ral = 1".
        //-------------------------------------------------
        if(d1.dir!=d2.dir) d1.ral = 1;
        //-------------------------------------------------
        // SOLO PER GUIDA A MAGNETI
        // Se nel nodo sucess_sucess ho una rotazione a una
        // distanza inferiore di 150cm devo rallentate al
        // nodo prima.
        //-------------------------------------------------
        if(mission.GestioneLaser==false){
           if(rot3!=rot2 && d2.dist<DIST_MAX_PER_RAL_ANTICIPATO) d1.ral = 1;
           //-------------------------------------------------
           // Se il nodo sucess_sucess è il PInt e ho una
           // distanza inferiore di 150cm devo rallentate al
           // nodo prima.
           //-------------------------------------------------
           if(mission.punto[i+2]==mission.pintend && d2.dist<DIST_MAX_PER_RAL_ANTICIPATO) d1.ral = 1;
        }   
        //-------------------------------------------------
        // Se il punto di missione sucessivo è un punto
        // terminale inserisco "ral = 2".
        //-------------------------------------------------
        if(NodoFinale==true) d1.ral = 2;
     }
     //-----------------------------------------------------
     // Se è stato impostato Rallentamento "4" dell'editor
     // dei Nodi significa che deve essere forzato un Ral-
     // lentamento indipendentemente dalle rotazioni.
     //-----------------------------------------------------
     if(d1.ral==4) d1.ral=1;
     //-----------------------------------------------------
     // ROTAZIONE INVERTITA
     // devo memorizzare che in caso di rotazione su un
     // punto deve essere fatta una rotazione invertita
     //-----------------------------------------------------
     if(ral1>9){
        d1.ral = 10 + d1.ral;
     }
     //-----------------------------------------------------
     // Associo i nuovi dati con i rallentamenti modificati
     // alla struttura della navetta.
     //-----------------------------------------------------
     memcpy(&mission.dati_perc[i], &d1, sizeof(DATI_PERCORSO));
  }
  return;
}

// -----------------------------------
//     AttivaPercorsoSpeciale()
// -----------------------------------
// Modifica la direzione di movimento dei TGV
//
void agv::AttivaPercorsoSpeciale()
{
  short int i, Port;
  bool tgv_su_nodo_porta;

  //------------------------------------------------------------------------------------
  // Valido solo per TGV senza missione in corso
  //------------------------------------------------------------------------------------
  if(test_mission_on()==true) return;
  //------------------------------------------------------------------------------------
  // Inizializzazione percorso standard
  // PercorsoSpeciale = 0            (standard)
  // PercorsoSpeciale = 1...TOTPORTE (porta in apertura parziale = percorso altenativo)
  // PercorsoSpeciale = 5...         (condizioni aux             = percorso altenativo)
  //------------------------------------------------------------------------------------
  PercorsiSpeciali=0;
  //------------------------------------------------------------------------------------
  // Verifico se il TGV si trova in ingombro a una porta in apertura parziale
  //------------------------------------------------------------------------------------
  for(Port=1; Port<=TOTPORTE; Port++){
     //----------------------------------------------------
     // Porta esclusa da PLC - (attraversabile)
     //----------------------------------------------------
     if(PorteImp[Port].EsclusionePLC==true ) continue;
     //----------------------------------------------------
     // Apertura totale o porta chiusa --> skip!
     //----------------------------------------------------
     if(!PorteImp[Port].AperturaParziale   ) continue;
     //----------------------------------------------------
     // Se il TGV non è già in transito --> skip!
     //----------------------------------------------------
   //if(mission.PorInIngombro[Port]==false) continue;
     //----------------------------------------------------
     // Verifico che il TGV si trovi già su un nodo della
     // porta
     //----------------------------------------------------
     tgv_su_nodo_porta=false;
     for(i=0; i<11; i++){
        if(PorteImp[Port].Punto[i]<1           ) continue;
        if(PorteImp[Port].Punto[i]>MAXPUNTI    ) continue;
        if(PorteImp[Port].Punto[i]!=stato.pos  ) continue;
        tgv_su_nodo_porta=true;
        break;
     }
     if(tgv_su_nodo_porta==false) continue;
     //----------------------------------------------------
     // - APERTURA PARZIALE = percorso speciale
     //----------------------------------------------------
     PercorsiSpeciali = Port;
     break;
  }

  return;
}

// -----------------------------------
//         CambiaDirezione()
// -----------------------------------
// Modifica la direzione di movimento dei TGV
//
void agv::CambiaDirezione()
{
//short int i;
//short int count_circostanti;
//bool   tratto_deviato;
//struct DATI_PERCORSO dati;
//struct DATI_PERCORSO dati_prec;
//struct DATI_PERCORSO dati_prec_prec;
//struct DATI_PERCORSO dati_succ;
//struct DATI_PERCORSO dati_succ_succ;
  struct DATI_PERCORSO new_dati[MAXPERCORSI];

//memset(&dati,           0, sizeof(DATI_PERCORSO));
//memset(&dati_prec,      0, sizeof(DATI_PERCORSO));
//memset(&dati_prec_prec, 0, sizeof(DATI_PERCORSO));
//memset(&dati_succ,      0, sizeof(DATI_PERCORSO));
//memset(&dati_succ_succ, 0, sizeof(DATI_PERCORSO));
  //-------------------------------------------------------
  // Reset struttura temporanea missione
  //-------------------------------------------------------
  memset(&new_dati, 0, sizeof(new_dati));
  //-------------------------------------------------------
  // Appoggio la struttura originale su una temporanea
  //-------------------------------------------------------
  memcpy(&new_dati, &mission.dati_perc, sizeof(new_dati));
//
//  TRATTANDOSI DI NAVETTA PALLET NON PREVEDO L'OTTIMIZZAZIONE DEI 180°
//
/*
  //-------------------------------------------------------
  // DEVIAZIONI
  // Se è impostato il movimento dinamico e il percorso
  // presenta tratti deviati, viene gestito come standard
  //-------------------------------------------------------
  tratto_deviato=false;
  for(i=0; i<MAXPERCORSI; i++){
     if(mission.punto[i]==0                                         ) break;
     if(mission.dati_perc[i].ral!=5 && mission.dati_perc[i].ral!=6  ) continue;
     tratto_deviato=true;
     break;
  }
  //-------------------------------------------------------
  // VERIFICA ROTAZIONI A 180°
  // Se il TGV si muove in maniera "standard" o "dinamica"
  // cerco di evito le rotazioni a 180°
  // Scorro la missione al contrario e aggiusto le direzioni
  // per evitare i 180°
  //--------------------------------------------------------
  for(i=MAXPERCORSI-1; i>0; i--){
     if(mission.punto[i]==0) continue;
     //-----------------------------------------------------
     // Se il punto di partenza è un punto terminale non
     // sono ammessi i 180°
     // (non più necessario per inserimento controllo sotto)
     //-----------------------------------------------------
   //if(i==1 && N.punti_notevoli[mission.punto[i-1]]) break;
     //-----------------------------------------------------
     // Se il percorso presenta un tratto deviato
     // la direzione non può essere calcolata dinamicamente
     //-----------------------------------------------------
     if(tratto_deviato==true) break;
     //-----------------------------------------------------
     // 08/09/2009
     // Se l'ultimo punto della missione non è un PT non
     // viene utilizzato come nodo di riferimento per
     // l'aggiustamento delle rotazioni
     //
     //-----------------------------------------------------
     if(i<MAXPERCORSI && mission.punto[i+1]==0 && N.punti_notevoli[mission.punto[i]]==false) continue;
     //-----------------------------------------------------
     // Sui punti terminali non sono ammessi i 180°
     //-----------------------------------------------------
     if(i>0 && N.punti_notevoli[mission.punto[i-1]]) continue;
     //-----------------------------------------------------
     // Partenza su nodi di rettilinea non sono ammessi 180°
     // UN NODO PER NON ESSERE SU UN RETTILINEO DEVE AVERE
     // ALMENO 3 NODI CIRCOSTANTI
     //-----------------------------------------------------
     count_circostanti=0;
     if(i>0 && stato.pos==mission.punto[i-1]){
        if(N.nodo_circostante[mission.punto[i-1]][0]!=0) count_circostanti++;
        if(N.nodo_circostante[mission.punto[i-1]][1]!=0) count_circostanti++;
        if(N.nodo_circostante[mission.punto[i-1]][2]!=0) count_circostanti++;
        if(N.nodo_circostante[mission.punto[i-1]][3]!=0) count_circostanti++;
        if(count_circostanti<3) continue;
     }
     //-----------------------------------------------------
     // PUNTO PRECENDENTE PRECEDENTE
     //-----------------------------------------------------
     if(i>1){
        memcpy(&dati_prec_prec, &new_dati[i-2], sizeof(DATI_PERCORSO));
     }
     //-----------------------------------------------------
     // PUNTO PRECENDENTE
     //-----------------------------------------------------
     memcpy(&dati_prec, &new_dati[i-1], sizeof(DATI_PERCORSO));
     //-----------------------------------------------------
     // PUNTO CONSIDERATO
     //-----------------------------------------------------
     memcpy(&dati, &new_dati[i], sizeof(DATI_PERCORSO));
     //-----------------------------------------------------
     // verifico se c'è una rotazione a 180°
     //-----------------------------------------------------
     if(dati_prec.rot==0 && dati.rot==2 ||
        dati_prec.rot==1 && dati.rot==3 ||
        dati_prec.rot==2 && dati.rot==0 ||
        dati_prec.rot==3 && dati.rot==1){
        new_dati[i-1].dir=new_dati[i].dir;
        new_dati[i-1].rot=new_dati[i].rot;
     }
     //-----------------------------------------------------
     // 18/03/2010
     // verifico se c'è una rotazione a 90°
     //-----------------------------------------------------
     if(dati_prec.rot!=dati.rot){
        //-------------------------------------------
        // in caso di cambio direzione sul punto
        // considerato controllo se c'è stata una
        // rot a 180° nel nodo prima e la correggo
        // (CONDIZIONE POSSIBILE SE IL NODO PREC PREC
        //  E UN PUNTO TERMINALE)
        //-------------------------------------------
        if(dati_prec_prec.rot==0 && dati_prec.rot==2 ||
           dati_prec_prec.rot==1 && dati_prec.rot==3 ||
           dati_prec_prec.rot==2 && dati_prec.rot==0 ||
           dati_prec_prec.rot==3 && dati_prec.rot==1){
           //------------------------------------------
           // mantengo direzione e rotazione di 2 nodi
           // prima per evitare un 180° aspettando di
           // arrivare sulla naturale rotazione a 90°
           // del precorso
           //------------------------------------------
           new_dati[i-1].dir=new_dati[i-2].dir;
           new_dati[i-1].rot=new_dati[i-2].rot;
        }
     }
  }
*/
  //-------------------------------------------------------
  // copio la struttura percorsi temporanea modificata
  // sulla struttura globale
  //-------------------------------------------------------
  memcpy(&mission.dati_perc, &new_dati, sizeof(mission.dati_perc));
  return;
}

// -----------------------------------
//         BloccoRotazioni180()
// -----------------------------------
// Verifico se sono presenti rotazioni a 180° su tratti dove non è
// possibile farle.
//
int agv::BloccoRotazioni180()
{
  bool rot_su_primo_nodo;
  bool tratto_deviato;
  bool nodo_fuori_ingombro;
  short int i, j, k;
  short int rot1, rot2, punto;
  short int count_notevoli, count, count1;
  double CalcolaRad, CatetoX, CatetoY;
  int Dist, x1, y1, x2, y2;

  //-------------------------------------------------------------------
  // Scorro tutta la missione del TGV
  //-------------------------------------------------------------------
  for(i=0; i<MAXPERCORSI-1; i++){
     punto = mission.punto[i];
     if(punto==0) break;
     //-------------------------------------------------------------------
     // Recupero lo stato di rotazione dei TGV nei vari punti
     //-------------------------------------------------------------------
     rot1 = mission.dati_perc[i].rot;
     //-------------------------------------------------------------------
     // Verifico se sul nodo precedente a quello considerato è
     // programmata una DEVIAZIONE
     //-------------------------------------------------------------------
     tratto_deviato=false;
     if(i>0 && N.nodo_grd[agv_num][punto]!=0) tratto_deviato=true;
     //-------------------------------------------------------------------
     // FUORI INGOMBRO
     // ammessi anche i 180°
     //-------------------------------------------------------------------
     nodo_fuori_ingombro=false;
     for(j=0; j<MAXNODIATTESA; j++){
        if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]<1         ) continue;
        if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]>MAXPUNTI  ) continue;
        if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]!=punto    ) continue;
        nodo_fuori_ingombro=true;
        break;
     }
     //-------------------------------------------------------------------
     // se sto analizzando il primo punto di percorso recupero lo stato
     //-------------------------------------------------------------------
     if(i==0){
        if(stato.s.bit.grd0  ) rot2 = 0;
        if(stato.s.bit.grd90 ) rot2 = 1;
        if(stato.s.bit.grd180) rot2 = 2;
        if(stato.s.bit.grd270) rot2 = 3;
     }
     //-------------------------------------------------------------------
     // recupero la rotazione che assumerò sul nodo successivo
     //-------------------------------------------------------------------
   //else rot2 = mission.dati_perc[i+1].rot;
     //-------------------------------------------------------------------
     // recupero la rotazione che avevo nel nodo precedente (se c'è una
     // variazione significa che in quel nodo devo compiere una rotazione)
     //-------------------------------------------------------------------
     else rot2 = mission.dati_perc[i-1].rot;
     //-------------------------------------------------------------------
     // BLOCCHI ROTAZIONE 180° SU TUTTI I PUNTI DI UN RETTILINEO
     //-------------------------------------------------------------------
     if(!AbilitaRotazioneSuiRettilinei && !N.punti_notevoli[punto] && !tratto_deviato && !nodo_fuori_ingombro){
        Dist=x1=y1=x2=y2=0;
        count_notevoli=count=0;
        CalcolaRad=CatetoX=CatetoY=0;
        //------------------------------------------------------
        // Verifico che la missione possa fare una rotazione
        //------------------------------------------------------
        for(j=0;;j++){
           if(!Percorso[j][0] && !Percorso[j][1]) break;
           //---------------------------------------------------
           // Conto le presenze del nodo intermedio nella
           // struttura dei percorsi
           //---------------------------------------------------
           if(Percorso[j][0]==punto){
              count++;
              if(N.punti_notevoli[Percorso[j][1]] && (N.nodo_busy[Percorso[j][1]]!=0 && N.nodo_busy[Percorso[j][1]]!=agv_num)){
                 //-------------------------------------------------------------------
                 // Recupero la distanza tra la punto è il punto notevole occupato
                 //-------------------------------------------------------------------
                 x1 = Punti[ punto ][0];
                 y1 = Punti[ punto ][1];
                 x2 = Punti[ Percorso[j][1] ][0];
                 y2 = Punti[ Percorso[j][1] ][1];
                 CatetoX = (double) abs(x2-x1);
                 CatetoY = (double) abs(y2-y1);
                 CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
                 if(CatetoX<30000 && CatetoY<30000){
                    Dist = (int) sqrt_algo( CalcolaRad );
                    if( Dist<DIST_MIN_ROT_SINGOLA ) count_notevoli++;
                 }
              }
              //----------------------------------------------------------------------
              // Se il punto a cui sono collegato direttamente è un incrocio,
              // ammetto la rotazione
              //----------------------------------------------------------------------
              count1=0;
              for(k=0;;k++){
                 if(!Percorso[k][0] && !Percorso[k][1]) break;
                 if(N.punti_notevoli[Percorso[k][0]] ||
                    N.punti_notevoli[Percorso[k][1]]  ) continue;  // non sono considerati i PT
                 //-----------------------------------------
                 // Conto a quanti nodi sono collegato
                 //-----------------------------------------
                 if(Percorso[k][0]!=Percorso[j][1] &&
                    Percorso[k][1]!=Percorso[j][1]    ) continue;
                 count1++;
                 if(count1>2){
                    count++;
                 }
              }
           }
           if(Percorso[j][1]==punto){
              count++;
              if(N.punti_notevoli[Percorso[j][0]] && (N.nodo_busy[Percorso[j][0]]!=0 && N.nodo_busy[Percorso[j][0]]!=agv_num)){
                 //-------------------------------------------------------------------
                 // Recupero la distanza tra la punto è il punto notevole occupato
                 //-------------------------------------------------------------------
                 x1 = Punti[ punto ][0];
                 y1 = Punti[ punto ][1];
                 x2 = Punti[ Percorso[j][0] ][0];
                 y2 = Punti[ Percorso[j][0] ][1];
                 CatetoX = (double) abs(x2-x1);
                 CatetoY = (double) abs(y2-y1);
                 CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
                 if(CatetoX<30000 && CatetoY<30000){
                    Dist = (int) sqrt_algo( CalcolaRad );
                    if( Dist<DIST_MIN_ROT_SINGOLA ) count_notevoli++;
                 }
              }
              //----------------------------------------------------------------------
              // Se il punto a cui sono collegato direttamente è un incrocio,
              // ammetto la rotazione
              //----------------------------------------------------------------------
              count1=0;
              for(k=0;;k++){
                 if(!Percorso[k][0] && !Percorso[k][1]) break;
                 if(N.punti_notevoli[Percorso[k][0]] ||
                    N.punti_notevoli[Percorso[k][1]]  ) continue;  // non sono considerati i PT
                 //-----------------------------------------
                 // Conto a quanti nodi sono collegato
                 //-----------------------------------------
                 if(Percorso[k][0]!=Percorso[j][0] &&
                    Percorso[k][1]!=Percorso[j][0]    ) continue;
                 count1++;
                 if(count1>2){
                    count++;
                 }
              }
           }
        }
        //---------------------------------------------------------------
        // Verifico che la missione non inizi con una rotazione di 180°.
        //---------------------------------------------------------------
        if((rot1==0 && rot2==2) || (rot1==1 && rot2==3) || (rot1==2 && rot2==0) || (rot1==3 && rot2==1)){
           //--------------------------------------------------------------------------
           // Nodo collegato a meno di 3 nodi (non è un incrocio) --> Skip!!!
           //--------------------------------------------------------------------------
           if(count_notevoli==0 && count<3) return punto;    // sicuramente tratto rettilineo
           //--------------------------------------------------------------------------
           // Nodo su incrocio con punto terminale occupato o in ingombro --> Skip!!!
           //--------------------------------------------------------------------------
           if(count_notevoli>0) return punto;                // sicuramente tratto rettilineo
        }
     }
     //-------------------------------------------------------------------
     // BLOCCHI ROTAZIONE 180° SU TUTTI I PUNTI IMPOSTATI DALL'OPERATORE
     //
     // NOTA:
     // Se il punto è finale della missione può essere mandato il TGV
     // perchè eventualmente entra in gioco il controllo sulla missione
     // successiva
     //-------------------------------------------------------------------
     if(N.n[punto]->blocco_rot==1 && punto!=mission.pend){
        if((rot1==0 && rot2==2) || (rot1==1 && rot2==3) || (rot1==2 && rot2==0) || (rot1==3 && rot2==1)) return punto;
     }
     //-------------------------------------------------------------------
     // (Sicurezza)
     // BLOCCHI ROTAZIONE 180° SU TUTTI I PUNTI TERMINALI
     //-------------------------------------------------------------------
     if(N.punti_notevoli[punto]==true){
        if((rot1==0 && rot2==2) || (rot1==1 && rot2==3) || (rot1==2 && rot2==0) || (rot1==3 && rot2==1)) return punto;
     }
  }
  //-------------------------------------------------------------------
  // 31/08/2011 PARTENZA CON 180° E ALTRO TGV IN INGOMBRO
  //-------------------------------------------------------------------
  if(N.n[stato.pos]->blocco_rot==false && mission.punto[0]!=0){
     rot_su_primo_nodo=false;
     if(stato.s.bit.grd0   && mission.dati_perc[0].rot==2) rot_su_primo_nodo=true;
     if(stato.s.bit.grd90  && mission.dati_perc[0].rot==3) rot_su_primo_nodo=true;
     if(stato.s.bit.grd180 && mission.dati_perc[0].rot==0) rot_su_primo_nodo=true;
     if(stato.s.bit.grd270 && mission.dati_perc[0].rot==1) rot_su_primo_nodo=true;
     //----------------------------------------------
     // Se il TGV deve compiere un 180° sul primo
     // nodo mi assicuro che non ci siano altri TGV
     // in ingombro
     //----------------------------------------------
     for(punto=1; punto<=TOTPUNTI; punto++){
        if(rot_su_primo_nodo==false   ) break;
        if(N.nodo_busy[punto]==0      ) continue;
        if(N.nodo_busy[punto]==agv_num) continue;
        //---------------------------------------------------------------------
        // GB-15/04/2015 Nel caso di Punto termimnale NON in ingombro -->SKIP
        //---------------------------------------------------------------------
        if(N.punti_notevoli[punto] && N.pt_in_ingombro[punto]==0){
           continue;
        }
        //-------------------------------------
        // Calcolo la distanza tra il nodo
        // occupato e la posizione del TGV
        //-------------------------------------
        x1 = Punti[ stato.pos ][0];
        y1 = Punti[ stato.pos ][1];
        x2 = Punti[ punto     ][0];
        y2 = Punti[ punto     ][1];
        CatetoX = (double) abs(x2-x1);
        CatetoY = (double) abs(y2-y1);
        CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
        if(CatetoX<30000 && CatetoY<30000){
           Dist = (int) sqrt_algo( CalcolaRad );
           if( Dist<DIST_MIN_ROT_DOPPIA_MAX ) return stato.pos;
        }
     }
  }

  return 0;
}

// -----------------------------------
//         BloccoSuDeviazione()
// -----------------------------------
// Verifico se la navetta si trova su un nodo
// sul quale partire con una deviazione.
//
int agv::BloccoSuDeviazione()
{
  int x1, y1, x2, y2;
  int rot1, rot2, punto1, punto2;

  //-------------------------------------------------------------------
  // Verifico se la navetta deve partire con una rotazione
  //-------------------------------------------------------------------
  punto1 = stato.pos;
  punto2 = mission.punto[1];
  if(stato.s.bit.grd0   ) rot1=0;
  if(stato.s.bit.grd90  ) rot1=1;
  if(stato.s.bit.grd180 ) rot1=2;
  if(stato.s.bit.grd270 ) rot1=3;
  rot2 = rot_standard(0);  // rotazione da assumere sul nodo considerato
  //-------------------------------------------------------------------
  // Solo per navette a magneti
  //-------------------------------------------------------------------
  if(mission.GestioneLaser==true) return 0;
  //-------------------------------------------------------------------
  // Se è ammesso il PInt su DEVIAZIONE deve essere possibile anche
  // lo start missione
  //-------------------------------------------------------------------
  if(NoPIntSuStartDeviazione==0 ) return 0;
  //-------------------------------------------------------------------
  // Se la missione parte senza rotazioni non è importante che il TGV
  // si trovi su un nodo di inizio deviazione
  //-------------------------------------------------------------------
  if(rot1==rot2                 ) return 0;
  //-------------------------------------------------------------------
  // Recupero le coordinate dei punti di start e successivo allo start
  //-------------------------------------------------------------------
  x1 = Punti[punto1][0];
  y1 = Punti[punto1][1];
  x2 = Punti[punto2][0];
  y2 = Punti[punto2][1];
  //-------------------------------------------------------------------
  // Verifico se si tratta di un tratto inclinato
  //-------------------------------------------------------------------
  if(x1==x2        ) return 0;
  if(y1==y2        ) return 0;
  if(abs(x1-x2)<5  ) return 0;
  if(abs(y1-y2)<5  ) return 0;
  //-------------------------------------------------------------------
  // Ritorno il nodo di partenza come nodo di inizio di una deviazione
  //-------------------------------------------------------------------
  return punto1;
}
// -----------------------------------------------------
//      test_percorso_disponibile()
// -----------------------------------------------------
// Verifica se dato un array di nodi "nodi_perc", è possibile occupare il percorso fino alla fine
//
int agv::test_percorso_disponibile(short int *nodi_perc, struct DATI_PERCORSO *dati_perc, short int *altro_agv, short int *MinPercorsi, short int *MinPercorsiToStart,  short int *nodo_di_interruzione, char *all_mess)
/*********************************************************/
{
  bool ragginta_dest;
  int i, punto, MinimoPercorsi;
  int NodoOccupabile, AgvPerInterruzionePercorso;
  int satellite_occupato, NodoInterruzionePercorso;
  short int tot_nodi_occupabili;
  short int app_nodi[MAXPERCORSI];
  struct DATI_PERCORSO app_dati[MAXPERCORSI];

  //---------------------------------------------
  // Inizializzazione varaibili
  //---------------------------------------------
  *altro_agv=0;
  *MinPercorsi=0;
  *MinPercorsiToStart=0;
  *nodo_di_interruzione=0;
  memset(&app_nodi[0], 0, sizeof(app_nodi));
  memset(&app_dati[0], 0, sizeof(app_dati));
  //---------------------------------------------
  // Valuto se esiste il percorso disponibile
  //---------------------------------------------
  MinimoPercorsi=MINPERCORSI;
  if(N.nodo_minpercorsi[stato.pos]!=0         ) MinimoPercorsi=N.nodo_minpercorsi[stato.pos];
  if(MinimoPercorsi<MIN_NODI_SBLOCCO_DEADLOCK ) MinimoPercorsi=MIN_NODI_SBLOCCO_DEADLOCK; // Minimo dei liberi nodi nella direzione di sblocco
  if(MinimoPercorsi==99                       ) MinimoPercorsi=MIN_NODI_SBLOCCO_DEADLOCK; // 23/11/2015 Se impostazione di default forza comunque MIN_NODI_SBLOCCO_DEADLOCK
  //---------------------------------------------
  // Compilazione TEMPORANEA struttura TGV
  // con possible nuovo percorso per calcolo
  // disponibilità occupazione nodi
  //---------------------------------------------
  memcpy(&app_nodi[0], &mission.punto[0],     sizeof(app_nodi));
  memcpy(&app_dati[0], &mission.dati_perc[0], sizeof(app_dati));
  memcpy(&mission.punto[0],     &nodi_perc[0],  sizeof(mission.punto));
  memcpy(&mission.dati_perc[0], &dati_perc[0],  sizeof(mission.dati_perc));
  //---------------------------------------------
  // Controllo solo gli "N" nodi di minpercorsi
  //---------------------------------------------
  ragginta_dest=false;
  tot_nodi_occupabili=0;
  punto=0;
  for(i=0; i<MAXPERCORSI; i++){
     if(mission.punto[i]<1){ragginta_dest=true; break;}
     punto=mission.punto[i];
     NodoOccupabile=AgvPerInterruzionePercorso=satellite_occupato=NodoInterruzionePercorso=0;
     VerificaNodoOccupato(TRUE, agv_num, punto, NodoOccupabile, AgvPerInterruzionePercorso, satellite_occupato, NodoInterruzionePercorso, 0, 0);
     if(NodoOccupabile==0) break;
     tot_nodi_occupabili++;
  }
  //----------------------------------------------
  // RICOMPILAZIONE STRUTTURA REALE
  //---------------------------------------------
  memcpy(&mission.punto[0],     &app_nodi[0], sizeof(mission.punto));
  memcpy(&mission.dati_perc[0], &app_dati[0], sizeof(mission.dati_perc));
  //---------------------------------------------
  // Verifico se il nodo successivo da occupare
  // è occupabile
  //---------------------------------------------
  *MinPercorsi          = tot_nodi_occupabili;
  *MinPercorsiToStart   = MinimoPercorsi;
  *nodo_di_interruzione = punto;
  if(ragginta_dest==true && tot_nodi_occupabili<MinimoPercorsi) *MinPercorsi=MinimoPercorsi;
  if(ragginta_dest==false){
     sprintf(all_mess, "NO test_percorso_disponibile() per TGV%d: nodo %d impeganto da ALTRO%d (Min:%d).", agv_num, satellite_occupato, AgvPerInterruzionePercorso, MinimoPercorsi);
     *altro_agv = (short int)(AgvPerInterruzionePercorso);
     return 1;
  }
  return 0;
}
// ---------------------------------------------------
//      free_nodi()
//  Libera tutti i nodi occupati in precedenza dal
//  punto di start alla posizione attuale
// ---------------------------------------------------
// Libera tutti i nodi occupati in precedenza da un AGV dal punto di
// start missione al punto su cui si trova attualmente l'AGV
//
int agv::free_nodi(char *all_mess)
/*********************************/
{
  int err=0;
  bool modificato;
  bool nodo_reincontrato, nodo_reincontrato2;
  short int ptr_percorso;
  short int ptr_nodo, ptr_nodo1;
  short int source;
  short int i, dest;


  modificato=false;
  sprintf(all_mess, MESS[126], agv_num);//Error gen‚rico en la funci¢n Free_nodi() AGV %d
  source = mission.pintstart;
  dest = mission.pend;
  if(source == 0) return 0;
  if(dest == 0) return 0;
  //----------------------------------------------------------
  // AL-24/01/2014 NON LIBERA NODI CON TGV IN MANUALE
  // Nemmeno quelli già percorsi da quando è stato messo in
  // automatico
  //----------------------------------------------------------
  if(ManualeConNodiOccupati==true && N.punti_notevoli[stato.pos]==false && !stato.s.bit.funz) return 0;
  //----------------------------------------------------------
  // AGV non si Š mosso dalla posizione precedente
  //----------------------------------------------------------
//if(stato.pos==source){
//   sprintf(all_mess, "OK");
//   return 0;
//}
  if(test_position_into_path(all_mess)) return 1;
  //----------------------------------------------------------
  // LIBERA NODI
  //----------------------------------------------------------
  for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
   //ptr_nodo = mission.punto_prenotato[ptr_percorso];
     ptr_nodo = mission.punto[ptr_percorso];            // !!!IMPORTANTE: Utilizzata struttura punti per liberari i nodi precedenti
     if(ptr_nodo <= 0        ) break;
     if(ptr_nodo > MAXPUNTI  ) break;
     if(ptr_nodo == stato.pos) break;
     //--------------------------------------------------------------
     // ECCEZZIONE:
     // Se un nodo ricompare all'interno della missione del TGV non
     // deve essere liberato.
     //
     // AL-01/07/2014: Utilizzato per evitare che un nodo terminale
     // venga liberato uscendo e rientrando, ma per i percorsi deve
     // essere liberato
     //--------------------------------------------------------------
     if(N.punti_notevoli[ptr_nodo]==true){
        nodo_reincontrato=false;
        for(i=(short int)(ptr_percorso+1); i<MAXPERCORSI; i++){
         //ptr_nodo1 = mission.punto_prenotato[i];
           ptr_nodo1 = mission.punto[i];                   // !!!IMPORTANTE: Utilizzata struttura punti per liberari i nodi precedenti
           if(ptr_nodo1 <= 0      ) break;
           if(ptr_nodo1 > MAXPUNTI) break;
           if(ptr_nodo1!=ptr_nodo ) continue;
           nodo_reincontrato=true;
           break;
        }
        if(nodo_reincontrato==true) break;
     }
     //--------------------------------------------------------------
     // Verifica se il nodo è occupato da questo AGV
     //--------------------------------------------------------------
     if(N.nodo_busy[ptr_nodo] == agv_num){
        N.nodo_busy[ptr_nodo]         =0;
        N.nodo_grd[agv_num][ptr_nodo] =0;
        modificato = true;
     }
     //--------------------------------------------------------------
     // GESTIONE PRECEDENZE PER TGV IN RETTILINEO
     //--------------------------------------------------------------
     if(N.nodo_con_tgv_lanciato[ptr_nodo] == agv_num){
        N.nodo_con_tgv_lanciato[ptr_nodo] = 0;
     }
  }
  mission.pintstart = stato.pos;        // Aggiorna la pos. intermedia di start

  //------------------------------------------------------------------
  // 14/02/97 S.Valentino inibito salvataggio su file
  //          dei nodi occupati in modo da accelerare l'applicazione
  // 11/05/97 Riabilitato
  //------------------------------------------------------------------
  if(modificato==true){
     err = N.save_nodo_busy(all_mess);
     if(!err) err = N.save_nodo_grd(all_mess);
  }
  if(err) return 1;
  return 0;
} /*** free_nodi() ***/

// ---------------------------------------------------
//      free_all_nodi()
// ---------------------------------------------------
// Libera tutti i nodi riservati in precedenza all'AGV tranne quello
// attualmente occupato fisicamente. Ritorna 1 in caso di errore
//
int agv::free_all_nodi(char *all_mess)
/************************************/
{
  short int ptr_nodo;
  bool modificato=false;
  int err=0;
  //----------------------------------------------------------
  // LIBERA NODI
  //----------------------------------------------------------
  for(ptr_nodo = 0; ptr_nodo<MAXPUNTI; ptr_nodo++){
     //------------------------------------------------------
     // Se il nodo su cui si trova il TGV è già occupato dal
     // TGV NON LO LIBERO.
     //------------------------------------------------------
     if((N.nodo_busy[ptr_nodo] == agv_num) && (ptr_nodo != stato.pos)){
        N.nodo_busy[ptr_nodo]    =0;
        N.nodo_grd[agv_num][ptr_nodo] =0;
        modificato =true;
     }
     //------------------------------------------------------
     // Se il nodo su cui si trova il TGV non è occupato dal
     // TGV LO OCCUPO.
     //------------------------------------------------------
     if((N.nodo_busy[ptr_nodo] == 0) && (ptr_nodo == stato.pos)){
        N.nodo_busy[ptr_nodo] = agv_num;
        modificato =true;
     }

     //------------------------------------------------------
     // Se il nodo è occupato da TGV > MAXAGV allora libera
     //------------------------------------------------------
     if( N.nodo_busy[ptr_nodo] > MAXAGV ){
        N.nodo_busy[ptr_nodo]    =0;
        modificato =true;
     }

  }
  if(modificato==true){
     err = N.save_nodo_busy(all_mess);
     if(!err) err = N.save_nodo_grd(all_mess);
  }
  if(err) return 1;
  return 0;
}/*** free_all_nodi() ***/

// ---------------------------------------------------
//      rot_standard()
// ---------------------------------------------------
// Dato un valore di rotazione > 3 (CURVA o DEVIAZIONE)
// restituisce il corrispondente valore di rotazione (0°, 90°, 180°, 270°);
//
//
int agv::rot_standard(int ptr_percorso)
/*************************************************/
{
  int rot_standard=0;
  int rotazione_prec=0;
  int avanti;
  int rotazione;
//int cambio_rot_a_inizio_curva = CAMBIO_ROTAZIONE_SU_START_CURVA;

//  if(ptr_percorso<0          ) return 0;
//  if(ptr_percorso>MAXPERCORSI) return 0;

  //----------------------------------------------------
  // Recupero direzione e rotazione TGV su nodo
  //----------------------------------------------------
  avanti         =!mission.dati_perc[ptr_percorso].dir;
  rotazione      = mission.dati_perc[ptr_percorso].rot;
  rot_standard   = rotazione;
  //----------------------------------------------------
  // Memorizzo anche rotazione precendente
  //----------------------------------------------------
  if(ptr_percorso>0){
     rotazione_prec = mission.dati_perc[ptr_percorso-1].rot;
     //----------------------------------------------------
     // TGV su posizione finale (rotazione da stato TGV)
     //
     // (sull'ultimo nodo la rotazione è quella del
     //  passo precedente di percorso)
     //----------------------------------------------------
     if(mission.punto[ptr_percorso]==mission.pend){
        rotazione    = rotazione_prec;
        rot_standard = rotazione_prec;
     }
  }
  //----------------------------------------------------
  // Puntatore percorso sconosciuto
  // (rotazione da stato TGV
  //----------------------------------------------------
  if(ptr_percorso<0 && ptr_percorso>MAXPERCORSI){
     if( stato.s.bit.grd0  ) rot_standard=0;
     if( stato.s.bit.grd90 ) rot_standard=1;
     if( stato.s.bit.grd180) rot_standard=2;
     if( stato.s.bit.grd270) rot_standard=3;
     return rot_standard;
  }
  //----------------------------------------------------
  // TGV su nodo senza missione
  //----------------------------------------------------
  // GB-15/02 (2) Se è su nodo di partenza valuta rotazione TGV
  // AL-13/03 (1) Se è su nodo di partenza valuta rotazione TGV (senza missione in corso)
  if(ptr_percorso==0 && !test_mission_on()){
     if( stato.s.bit.grd0  ) rot_standard=0;
     if( stato.s.bit.grd90 ) rot_standard=1;
     if( stato.s.bit.grd180) rot_standard=2;
     if( stato.s.bit.grd270) rot_standard=3;
     return rot_standard;
  }
  //----------------------------------------------------
  // GESTIONE LASER
  // In caso di curve, è possibile decidere se assegnare
  // la nuova rotazione al TGV a inizio o a fine curva
  //----------------------------------------------------
  if(mission.GestioneLaser==true && ptr_percorso>0 && rotazione>3 && rotazione<40){
     //--------------------------------------------------
     // Se la rotazione è impostato che debba essere
     // presa a fine curva, a ogni inizio curva recupero
     // la rotazione che aveva il TGV al nodo precedente
     //
     // AL-02/09/2015 in caso di 2 rotazioni consecutive
     // a inizio della seconda ho ancora la rot di
     // uscita della prima 
     //--------------------------------------------------
   //if(cambio_rot_a_inizio_curva==0 || (rotazione_prec>3 && rotazione_prec<40)) rotazione = rotazione_prec;
     rotazione = rotazione_prec;
  }
  //----------------------------------------------------
  // GESTIONE DEVIAZIONI + GESTIONE CURVE
  // Sul punto considerato devo gestire la rotazione
  // come già compiuta dal TGV
  //----------------------------------------------------
  switch(rotazione){
     case  0: rot_standard = 0;
              break;
     case  1: rot_standard = 1;
              break;
     case  2: rot_standard = 2;
              break;
     case  3: rot_standard = 3;
              break;
     case  4: if(avanti ) rot_standard=3;
              if(!avanti) rot_standard=1;
              break;
     case  5: if(avanti ) rot_standard=2;
              if(!avanti) rot_standard=0;
              break;
     case  6: if(avanti ) rot_standard=0;
              if(!avanti) rot_standard=2;
              break;
     case  7: if(avanti ) rot_standard=3;
              if(!avanti) rot_standard=1;
              break;
     case  8: if(avanti ) rot_standard=2;
              if(!avanti) rot_standard=0;
              break;
     case  9: if(avanti ) rot_standard=1;
              if(!avanti) rot_standard=3;
              break;
     case 10: if(avanti ) rot_standard=1;
              if(!avanti) rot_standard=3;
              break;
     case 11: if(avanti ) rot_standard=0;
              if(!avanti) rot_standard=2;
              break;
     case 12: if(avanti ) rot_standard=2;
              if(!avanti) rot_standard=0;
              break;
     case 13: if(avanti ) rot_standard=3;
              if(!avanti) rot_standard=1;
              break;
     case 14: if(avanti ) rot_standard=3;
              if(!avanti) rot_standard=1;
              break;
     case 15: if(avanti ) rot_standard=0;
              if(!avanti) rot_standard=2;
              break;
     case 16: if(avanti ) rot_standard=1;
              if(!avanti) rot_standard=3;
              break;
     case 17: if(avanti ) rot_standard=2;
              if(!avanti) rot_standard=0;
              break;
     case 18: if(avanti ) rot_standard=0;
              if(!avanti) rot_standard=2;
              break;
     case 19: if(avanti ) rot_standard=1;
              if(!avanti) rot_standard=3;
              break;
     case 20: rot_standard = 0;
              break;
     case 21: rot_standard = 1;
              break;
     case 22: rot_standard = 2;
              break;
     case 23: rot_standard = 3;
              break;
     case 30: rot_standard = 0;
              break;
     case 31: rot_standard = 1;
              break;
     case 32: rot_standard = 2;
              break;
     case 33: rot_standard = 3;
              break;
     case 50: rot_standard = 0;
              break;
     case 51: rot_standard = 1;
              break;
     case 52: rot_standard = 2;
              break;
     case 53: rot_standard = 3;
              break;
     case 54: rot_standard = 0;
              break;
     case 55: rot_standard = 1;
              break;
     case 56: rot_standard = 2;
              break;
     case 57: rot_standard = 3;
              break;
     case 60: rot_standard = 0;
              break;
     case 61: rot_standard = 1;
              break;
     case 62: rot_standard = 2;
              break;
     case 63: rot_standard = 3;
              break;
     case 64: rot_standard = 0;
              break;
     case 65: rot_standard = 1;
              break;
     case 66: rot_standard = 2;
              break;
     case 67: rot_standard = 3;
              break;
     default: break;
  }
  return rot_standard;
}

// ---------------------------------------------------
//      rioccupa_nodi()
// ---------------------------------------------------
// Questa funzione ha il compito di rimediare a una condizione
// anomala. Occupa i nodi tra la posizione attaule del TGV e
// il primo nodo occupato.
//
int agv::rioccupa_nodi(char *all_mess)
/************************************/
{
  bool AlmenoUnoOccupato;
  short int ptr_percorso, ptr_nodo;
  short int nodi_non_occupati[MAXPERCORSI];
  short int aux[MAXPERCORSI];

  AlmenoUnoOccupato=false;
  memset(&aux, 0, sizeof(aux));
  memset(&nodi_non_occupati, 0, sizeof(nodi_non_occupati));
  //----------------------------------------------------------
  // Verifico se tra la posizione attuale del TGV e il primo
  // nodo occupato ci sono dei nodi solo prenotati.
  //----------------------------------------------------------
  for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
     ptr_nodo = mission.punto_prenotato[ptr_percorso];
     if(ptr_nodo==0                ) break;
     if(N.nodo_busy[ptr_nodo]!=0   ) continue;            // nodo già occupato da altro tgv
     if(N.nodo_busy[ptr_nodo]==agv_num){
        AlmenoUnoOccupato=true;
        break;
     }
     nodi_non_occupati[ptr_percorso]=ptr_nodo;
  }
  //----------------------------------------------------------
  // Se tra tutti i nodi del percorso no ce n'è almeno uno
  // occupato -->Skipp!
  //----------------------------------------------------------
  if(AlmenoUnoOccupato==false && N.nodo_busy[stato.pos]==0){
     //----------------------------------------------------------
     // Forzatura dell'occupazione del nodo sul quale si trova
     // il TGV.
     //----------------------------------------------------------
     N.nodo_busy[stato.pos]=agv_num;
     //----------------------------------------------------------
     // Salvataggio della nuova situazione dei nodi occupati
     //----------------------------------------------------------
     if(N.save_nodo_busy(all_mess)) return 1;
     return 0;
  }
  //----------------------------------------------------------
  // Se non c'è nessun altro nodo occupato da TGV lungo il
  // percorso --> Skip!
  //----------------------------------------------------------
  if(AlmenoUnoOccupato==false) return 0;
  //----------------------------------------------------------
  // Se il percorso è stato impegnato in maniera corretta
  // -->Skipp!!
  //----------------------------------------------------------
  if(memcmp(&nodi_non_occupati, &aux, sizeof(aux))==0) return 0;
  //----------------------------------------------------------
  // Occupazione dei nodi che non sono stati occupati
  //----------------------------------------------------------
  for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
     if(nodi_non_occupati[ptr_percorso]==0) break;
     if(N.nodo_busy[nodi_non_occupati[ptr_percorso]]!=0) continue;    // nodo già occupato da altro tgv
     N.nodo_busy[nodi_non_occupati[ptr_percorso]]=agv_num;
  }
  //----------------------------------------------------------
  // Salvataggio della nuova situazione dei nodi occupati
  //----------------------------------------------------------
  if(N.save_nodo_busy(all_mess)) return 1;
  return 0;
}/*** rioccupa_nodi() ***/

// ---------------------------------------------------
//      occupa_nodi()
// ---------------------------------------------------
// Occupa tutti i nodi dalla posizione attuale AGV fino alla destinazione
// (target) o sino al punto impegnato da un 'altro AGV. Restituisce in
// "num_nodo" l'ultimo nodo occupato. La funzione ritorna 1 in caso di
// errore.
//
int agv::occupa_nodi(char *all_mess, bool tutto, short int *agv_bloccante)
/*************************************************/
{
  bool RioccupaNodo;
  int ptr_percorso, modificato =0, err=0;
  int x1, y1, rot1, rot2;
  int i, j, k, z, indice;
  int satellite_occupato=0;
  int count_da_interruzione, count_da_porta, count_da_macc, count_nodi_porta;
  int percorso_libero;
  int numero_nodo;
  int agv_adiacente=0;
  int nodo_vicino;
  int MinNodiPIntSuccessivo = MINNODIPINTSUCC;
  int MinNodFuoriIngombro = MIN_NODI_FUORI_INGOMBRO;
  short int ing_macchina;
  short int salto_a_nodo;
  short int nodi_da_saltare[21];
  short int AgvBloccante;
  short int AgvInIngombro;
  short int max_metri;
  short int punto_arrivo;
  short int altro_agv, metri;
  short int nodi_da_occupare, nodi_occupati;
  short int source, ptr_nodo, ptr_nodo1, dest, last_nodo;
  short int succ_sotto_porta, sotto_porta, rot_su_nodo_porta;
  short int semaforo_porta;
  short int pintend;
  short int num_baia, num_mac, num_stz, num_cbat;
//short int id_last_nodo;
//short int id_pintend;
  short int altro_agv_su_pt_in_ingombro;

  int Vicino,
      Dist, DistX, DistY,
      xv, yv, PuntoConsiderato;
  bool rot_not_ok;
  bool tgv_si_incontrano;
  bool pintend_passato;
  bool pint_obbligato, gruppo;
  bool blocco_manuale, blocco_tgv_lanciato, zona_fuori_ingombro_manuale;
  bool porta_in_ok_accesso;
  bool macch_in_ok_accesso;
  bool no_attraverso_porta;
  bool no_stop_per_deviazione;
  bool RetteParallele, AgvSuAngolo;
  bool altro_agv_fermo_su_fuori_ingombro;
  double CalcolaRad, CatetoX, CatetoY;
  int PrecInterruzionePercorso;
  int EsisteZonaFuoriIngombro, NodoInterruzionePercorso, AgvPerInterruzionePercorso;
  int satellite_occupato1, NodoOccupabile1, AgvPerInterruzionePercorso1, NodoInterruzionePercorso1;
  int UltimoNodoDiPercorsoPrimaDellaDest;

  int NodoOccupabile;
  int NodoConsiderato;
  int SempreON=1;

//int  jj=0;
//bool NodoDiAttesa=0;
  int  IndicePercorsoIngOutCrvDev;                   // Nodo di percorso in ingresso o uscita curva
  int  DistIngombro, DistIngombroSucc;               // Distanza dal nodo adiacente Nodo ingombro
  int  NodoPercorsoIngombroPt;                       // Nodo di percorso in Ingombro da PT
  int  NodoPercorsoIngombroAnticipatoPt;             // Nodo di percorso in Ingombro anticipato da PT (rotazione doppia)
  int  NodoPercorsoIngombroAnticipatoCrvDev;         // Nodo di percorso in Ingombro anticipato per CURVA o DEVIAZ. (su primo tratto rettilineo disponibile)
  int  NodoPercorsoIngombroAnticipatoAltroAgvFermo;  // Nodo di percorso in Ingombro anticipato per altro TGV fermo senza missione
  int  NumTgvPerAnticipo;                            // Numero del TGV che ha causato l'anticipo nodo
  int  NodoIngombroPercorsoAnticipato;               // Nodo di ingombro percorso anticipato causa PTinIng, Curva o Deviaz.
  int    PSucc[MAXPERCORSI+1];
  double DSucc[MAXPERCORSI+1];

  rot_su_nodo_porta = 0;
//id_pintend        = 0;
//id_last_nodo      = 0;
  sotto_porta       = 0;
  succ_sotto_porta  = 0;
  semaforo_porta    = 0;
  punto_arrivo      = 0;
  metri             = 0;
  nodi_da_occupare  = 0;
  *agv_bloccante    = 0;
  source            = stato.pos;       // ** source = posizione attuale AGV
  dest              = mission.pend;    // ** dest   = target della missione AGV
  last_nodo         = source;
  pintend           = mission.pintend;   // AL-22/02 ultimo pintend;
  sprintf(all_mess, "Errore in occupa_nodi() per il TGV %d!", agv_num);
  if((source==0) || (dest==0)){strcpy(all_mess, ""); return 0;}
  if(test_position_into_path(all_mess)) return 1;

  //-------------------------------------------------------------
  // OCCUPAZIONE NODI CON INGOMBRO AGV
  //-------------------------------------------------------------
  if(AbilitaGestioneIngombroCurvePerStopTGV==3){
     err=occupa_nodi_ingombro_agv(all_mess, tutto, agv_bloccante);
     return err;
  }

  //-------------------------------------------------------------
  // (A) Cerca punto del percorso dove iniziare a occupare nodi
  //-------------------------------------------------------------
  for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
     if(mission.punto[ptr_percorso] == source) break;
     if(mission.punto[ptr_percorso] == 0) break;
  }
  if(mission.punto[ptr_percorso] != source){
     sprintf(all_mess, "Errore nodo fuori percorso in occupa_nodi() per il TGV %d!", agv_num);
     return 1;
  }
  //-------------------------------------------------------------
  // (B) Compila array punto_prenotato[] con i nodi restanti
  //     dalla posizione attuale all'obbiettivo
  //-------------------------------------------------------------
  memset(&mission.punto_prenotato , 0, sizeof(mission.punto_prenotato));
  for(i=0; i<MAXPERCORSI; i++){
     mission.punto_prenotato[ i ] = mission.punto[ ptr_percorso+i ];
     if(mission.punto_prenotato[ i ] == 0) break;
  }
  //-----------------------------------------------------------------
  // Se i nodi sono già tutti occupati fino a destinazione --> Skip!
  //-----------------------------------------------------------------
  if(mission.pend==mission.pintend){
     //-----------------------------------------------------------
     // Verifico se il nodo precedente all'ultimo è già occupato
     // occupati --> skip
     //-----------------------------------------------------------
     if(i>1 && N.nodo_busy[mission.punto_prenotato[i-2]]==agv_num) return 0;
  }
  //-------------------------------------------------------------
  // Recupero indice posizione TGV
  //-------------------------------------------------------------
  indice=0;
  for(i=0; i<MAXPERCORSI; i++){
     if(mission.punto[i]==0     ) break;
     if(mission.punto[i]!=source) continue;
     indice=i;
     break;
  }

  //-------------------------------------------------------------
  // CONSENSI PORTA
  // - Verifico se il TGV è in passaggio sotto una porta
  //-------------------------------------------------------------
  count_da_porta=0;
  for(i=indice; i<MAXPERCORSI; i++){
     if(mission.punto[i]==0) break;
     //------------------------------------------
     // Se il TGV passa sotto la porta setto il
     // nodo intermedio
     //------------------------------------------
     for(j=1; j<=MAXPORTEIMP; j++){
        sotto_porta=0;
        //-----------------------------------
        // Porta esclusa da PLC
        // (attraversabile)
        //-----------------------------------
        if(PorteImp[j].EsclusionePLC==true) continue;
        //-----------------------------------
        // Ciclo per tutti i nodi
        //-----------------------------------
        for(k=0; k<11; k++){
           if(PorteImp[j].Punto[k]==0               ) continue;
           if(mission.punto[i]!=PorteImp[j].Punto[k]) continue;
           //-------------------------------------------------------
           // Recupero la rotazione che il TGV ha sul nodo in
           // ingombro alla porta
           //-------------------------------------------------------
           rot_su_nodo_porta = rot_standard(i);
           //-------------------------------------------------------
           // Memorizzo se il nodo successivo della missione è
           // ancora in ingombro alla porta
           //-------------------------------------------------------
           succ_sotto_porta=0;
           for(z=0; z<11; z++){
              if(PorteImp[j].Punto[z]==0                 ) continue;
              if(mission.punto[i+1]!=PorteImp[j].Punto[z]) continue;
              succ_sotto_porta = PorteImp[j].Punto[z];
              break;
           }
           //-------------------------------------------------------
           // 05/08/2011
           // Se non esiste il successivo sotto porta, mi
           // assicuro che la porta abbia più nodi assegnati
           //-------------------------------------------------------
           if(succ_sotto_porta==0){
              count_nodi_porta=0;
              for(z=0; z<11; z++){
                 if(PorteImp[j].Punto[z]==0) continue;
                 count_nodi_porta++;
              }
              if(count_nodi_porta<2) succ_sotto_porta=PorteImp[j].Punto[k];
           }
           //-------------------------------------------------------
           // Verifico se il TGV si move nella direzione impostata
           //-------------------------------------------------------
           rot_not_ok=true;
           if(mission.dati_perc[i].dir==1){
              if(rot_su_nodo_porta==0 && PorteImp[j].DirDown!=true ) rot_not_ok=false;
              if(rot_su_nodo_porta==1 && PorteImp[j].DirRight!=true) rot_not_ok=false;
              if(rot_su_nodo_porta==2 && PorteImp[j].DirUp!=true   ) rot_not_ok=false;
              if(rot_su_nodo_porta==3 && PorteImp[j].DirLeft!=true ) rot_not_ok=false;
           }
           if(mission.dati_perc[i].dir==0){
              if(rot_su_nodo_porta==0 && PorteImp[j].DirUp!=true   ) rot_not_ok=false;
              if(rot_su_nodo_porta==1 && PorteImp[j].DirLeft!=true ) rot_not_ok=false;
              if(rot_su_nodo_porta==2 && PorteImp[j].DirDown!=true ) rot_not_ok=false;
              if(rot_su_nodo_porta==3 && PorteImp[j].DirRight!=true) rot_not_ok=false;
           }
           //-------------------------------------------------------
           // La rotazione è ammessa ma il punto successivo è
           // di deviazione e non è ammesso il PInt su deviazione
           // abilito l'apertura con un nodo di anticipo
           //-------------------------------------------------------
           if(rot_not_ok==false){
              // DEVIAZIONE = ROT>49 && ROT<70
              if(NoPIntSuStartDeviazione==true){
                 if(mission.dati_perc[i+1].rot>49 && mission.dati_perc[i+1].rot<70) rot_not_ok=true;
              }
              // CURVA = ROT>3 && ROT<40
              if(NoPIntSuStartCurva==true){
                 if(mission.dati_perc[i+1].rot> 3 && mission.dati_perc[i+1].rot<40) rot_not_ok=true;
              }
           }
           //-------------------------------------------------------
           // Se la rotazione è ammessa non viene considerato sotto
           // porta
           //-------------------------------------------------------
           if(rot_not_ok==false) continue;
           //-------------------------------------------------------
           // Nodo porta trovato
           //-------------------------------------------------------
           sotto_porta=PorteImp[j].Punto[k];
           break;
        }
        //------------------------------------------------
        // PORTE CORRIDOIO
        // (FORZO IL PINT SOLO IN ATTRAVERSAMENTO)
        // Solo sulle porte di corridoio controllo che
        // anche il nodo successivo della missione sia
        // in ingombro alla porta altrimenti NON forzo
        // il punto intermedio
        //------------------------------------------------
        if(sotto_porta!=0 && succ_sotto_porta==0) sotto_porta=0;
        //------------------------------------------------
        // TGV GIA' IN TRANSITO SOTTO LA PORTA
        // Se ho individuato un nodo sotto porta nel
        // percorso verifico se è azzerabile
        //------------------------------------------------
        if(sotto_porta){
           //--------------------------------------------------------
           // Se la porta è in OK INGRESSO e il TGV è in prossimità
           // proseguo nell'occupazione dei nodi
           //--------------------------------------------------------
           porta_in_ok_accesso = PorteImp[j].OkIngresso;
           //----------------------------------------------------------
           // Verifico se il TGV ha il carrello a bordo e in quel
           // caso mi accerto che la porta non sia aperta parzialmente
           //----------------------------------------------------------
         //if((stato.s.bit.carA || stato.s.bit.carB) && PorteImp[j].AperturaParziale==true) porta_in_ok_accesso=false;
           //----------------------------------------------------------
           // GESTIONE PERCORSI ALTERNATIVI (con apertura parziale)
           // Utilizzo dell'apetura parziale per la gestione dei
           // percorsi alternativi sotto alle porte
           //
           // AL-25/01 (1)
           // Se il TGV ha già imboccato il percorso speciale e la
           // navetta Kraft si sposta in fuori ingombro mantengo il
           // percorso già intrapreso
           //----------------------------------------------------------
           if(porta_in_ok_accesso==true){
              if(PercorsiSpeciali==0 && PorteImp[j].AperturaParziale==true ) porta_in_ok_accesso=false;
            //if(PercorsiSpeciali==j && PorteImp[j].AperturaParziale==false) porta_in_ok_accesso=false;
           }
           //----------------------------------------------------------
           // Se la porta è in OK INGRESSO verifico se ho le condizioni
           // per procedere all'occupazione dei nodi
           //----------------------------------------------------------
           if(porta_in_ok_accesso==true){
              //-----------------------------------------------------
              // Proseguo l'occupazione del nodo
              //-----------------------------------------------------
              if(N.nodo_busy[sotto_porta]==agv_num && count_da_porta<=PorteImp[j].AnticipoRichiesta){
                 sotto_porta=0;
              }
              //-----------------------------------------------------
              // 25/02/2011
              // Se è già stato alzato l'ingombro alla porta non
              // viene mantenuto il punto intermedio
              //-----------------------------------------------------
              if(PorteImp[j].TgvInIngombro) sotto_porta=0;
           }
        }
        if(sotto_porta) break;
     }
     if(sotto_porta) break;
     //------------------------------------
     // 24/02/2011
     // Incremento conteggio nodi da porta
     //------------------------------------
     count_da_porta++;                                                          // conteggio nodi da pos tgv.
  }

  //-------------------------------------------------------------
  // SEMAFORI PORTA
  // - Verifico se il TGV incontra un punto di semaforo
  //   di una porta bloccata
  //-------------------------------------------------------------
  for(i=0; i<MAXPERCORSI; i++){
     if(mission.punto_prenotato[ i ]==0) break;
     //----------------------------------------
     // controllo tutte le porte dell'impianto
     // e considero solo quelle che il TGV
     // deve attraversare
     //----------------------------------------
     for(j=1; j<=MAXPORTEIMP; j++){
        //---------------------------------------
        // Scarto tutte le porte non bloccate
        //---------------------------------------
        if(PorteImp[j].PortaInBlocco==false) continue;
        //-----------------------------------
        // Porta esclusa da PLC
        // (attraversabile)
        //-----------------------------------
        if(PorteImp[j].EsclusionePLC==true ) continue;
        //---------------------------------------
        // Scarto tutte le porte che non devo
        // attraversare.
        //---------------------------------------
        no_attraverso_porta=true;
        for(k=0; k<11; k++){
           if(PorteImp[j].Punto[k]==0                          ) continue;
           if(PorteImp[j].Punto[k]!=mission.punto_prenotato[i] ) continue;
           //---------------------------------------------------
           // Per avere la condizione di attraversamento
           // almeno 2 nodi devono essere nodi in ingombro
           //---------------------------------------------------
           succ_sotto_porta=0;
           for(z=0; z<11; z++){
              if(PorteImp[j].Punto[z]==0                            ) continue;
              if(PorteImp[j].Punto[z]!=mission.punto_prenotato[i+1] ) continue;
              succ_sotto_porta=PorteImp[j].Punto[z];
              break;
           }
           if(succ_sotto_porta==0) continue;
           //---------------------------------------------------
           // TGV IN ATTRAVERSAMENTO
           //---------------------------------------------------
           no_attraverso_porta=false;
           break;
        }
        if(no_attraverso_porta==true) continue;
        //-------------------------------------------
        // Memorizzo il primo nodo di semaforo che
        // incontro lungo il percorso
        //-------------------------------------------
        semaforo_porta=0;
        for(z=0; z<MAXPERCORSI; z++){
           if(mission.punto_prenotato[ z ]==0) break;
           //------------------------------
           // Cerco il primo nodo
           //------------------------------
           for(k=0; k<11; k++){
              if(PorteImp[j].PuntoSemaforo[k]==0                          ) continue;
              if(PorteImp[j].PuntoSemaforo[k]!=mission.punto_prenotato[z] ) continue;
              semaforo_porta = PorteImp[j].PuntoSemaforo[k];
              break;
           }
           if(semaforo_porta!=0) break;
        }
        //-------------------------------------------
        // semaforo trovato --> skip!
        //-------------------------------------------
        if(semaforo_porta!=0) break;
     }
     if(semaforo_porta!=0) break;
  }
  //-------------------------------------------------------------
  // CONSENSI MACCHINA/BAIE e COMPATTABILI
  // Se la destinazione missione è una macchina o una baia
  // con comunicazione attiva (e quindi scambio consensi)
  // è necessario forzare il PINT sul nodo precedente
  //-------------------------------------------------------------
  ing_macchina=0;
  if(AbilitaPIntPerConsensiMacchina==true && stato.pos!=mission.pend){
     num_baia=num_mac=num_stz=num_cbat=0;
     //-------------------------------------------------------------------------------
     // ACCESSO a BAIE/MACCHINE
     //-------------------------------------------------------------------------------
     P.test_punto_presente_baia(all_mess, mission.pend, &num_baia);
     if(num_baia==0) P.test_punto_presente_mac(all_mess, mission.pend, &num_mac, &num_stz);
     if(num_baia==0 && num_mac==0) P.test_punto_presente_cbat(all_mess, mission.pend, &num_cbat);
     if((num_baia>0 && num_baia<=TOTBAIE    && P.ba[num_baia]->EsclComunic == false) ||
        (num_mac >0 && num_mac<=TOTCHIAMATE && P.ch[num_mac]->EsclComunic  == false) ||
        (num_cbat>0 && num_cbat<=TOTBATT    && P.cb[num_cbat]->EsclComunic == false)){
        count_da_macc=0;
        macch_in_ok_accesso = false;
        if(num_baia>0 && num_baia<=TOTBAIE    && P.ba[num_baia]->OkIng==true) macch_in_ok_accesso=true;
        if(num_cbat>0 && num_cbat<=TOTBATT    && P.cb[num_cbat]->OkIng==true) macch_in_ok_accesso=true;
        if(num_mac >0 && num_mac<=TOTCHIAMATE && (P.ch[num_mac]->OkIngMac==true || P.ch[num_mac]->Staz[num_stz].OkIngresso==true)) macch_in_ok_accesso=true;
        for(i=indice; i<MAXPERCORSI; i++){
           if(mission.punto[i]==0) break;
           //----------------------------------------------
           // Raggiune la destinazione --> skip!
           //----------------------------------------------
           if(mission.punto[i]==mission.pend && mission.punto[i+1]==0) break;
           //------------------------------------------
           // SUCCESSIVO NODO MACCHINA
           //------------------------------------------
           if(mission.punto[i+1]==mission.pend){
              ing_macchina=mission.punto[i];
              //--------------------------------------------------------
              // Se la macchina/baia è in ok ingresso e il TGV
              // ha già occupato il nodo precedente ed è a un distanza
              // <=ANTICIPO_RICH_INGRESSO_MAC non fermo il TGV
              //--------------------------------------------------------
              if(macch_in_ok_accesso==true && N.nodo_busy[ing_macchina]==agv_num && count_da_macc<=ANTICIPO_RICH_INGRESSO_MAC){
                 ing_macchina=0;
              }
           }
           if(ing_macchina!=0) break;
           //------------------------------------
           // Incremento conteggio nodi da porta
           //------------------------------------
           count_da_macc++;
        }
     }
  }
  //-------------------------------------------------------------
  // INGOMBRO PORTA
  // Se il TGV è già su un nodo di ingombro alla porta e non ha
  // l'OkAccesso non può occupare altri nodi
  //-------------------------------------------------------------
  if(sotto_porta!=0 && mission.pintend==sotto_porta) return 0;
  //-------------------------------------------------------------
  // SEMAFORO PORTA
  // Se il TGV è già su un nodo di ingombro alla porta e non ha
  // l'OkAccesso non può occupare altri nodi
  //-------------------------------------------------------------
  if(semaforo_porta!=0 && mission.pintend==semaforo_porta) return 0;
  //-------------------------------------------------------------
  // RICHIESTA INGRESSO PORTA
  // Se il TGV è su un nodo precedente a una macchina forzo
  // il Pint per la richiesta ingresso
  //-------------------------------------------------------------
  if(ing_macchina!=0 && mission.pintend==ing_macchina) return 0;
  //-------------------------------------------------------------
  // (B2) Non cambio la situazione dei nodi occupati per i TGV
  //      che hanno la missione che attraversa il nodo sul quale
  //      si trova un altro TGV IN RESET MISSIONE.
  //
  // AL-22/05/2015: Solo nel caso il TGV considerato sia in
  // missione in corso e senza missione ma su un pt_in_ingombro
  //-------------------------------------------------------------
  if(CicliAttesaPerAgvInReset>0){
     AgvInIngombro=0;
     if(stato.start==1 || (stato.start==0 && N.punti_notevoli[stato.pos]==true && N.pt_in_ingombro[stato.pos]==false)){
        for(i=1; i<=MAXAGV; i++){
           if(AgvInIngombro!=0) break;
           //-------------------------------------------------------------
           // Verifico se c'è una navetta in RESET
           //-------------------------------------------------------------
           if(i==agv_num                          ) continue;
           if(AGV[i]->test_mission_on()           ) continue;
           if(AGV[i]->mission.SemiAutomatico==true) continue;
           if(AGV[i]->mission.Esclusione==true    ) continue;
           if(AGV[i]->stato.s.bit.funz==false     ) continue;
           if(AGV[i]->stato.s.bit.allarme==true   ) continue;
           if(AGV[i]->allarme_interno!=0          ) continue;  // AL-FPT 22/09/2014 Anche l'allarme interno è da considerarsi allarme che non fa muovere il TGV
           if(AGV[i]->mission.TimeInitAttesa!=0   ) continue;  // AL-FPT 26/09/2014 Il TGV in attesa voluta, non è da utilizzare
           //-------------------------------------------------------------
           // TGV SU PUNTO TERMINALE
           //-------------------------------------------------------------
           if(N.punti_notevoli[AGV[i]->stato.pos]==true) continue;  // AL-IVC 18/04/2017 scarto i TGV sui punti TERMINALI
           //-------------------------------------------------------------
           // Verifico se altro AGV su nodo_fuori_ingombro, allora
           // non è ritenuto in cambio missione
           //-------------------------------------------------------------
           altro_agv_fermo_su_fuori_ingombro=false;
           for(gruppo=GRUPPO_FUORI_INGOMBRO; gruppo<=MAXGRUPPIIMP; gruppo++){
              for(j=0; j<MAXNODIATTESA; j++){
                 if(N.nodi_attesa[gruppo][j]<1                 ) continue;
                 if(N.nodi_attesa[gruppo][j]>TOTPUNTI          ) continue;
                 if(N.nodi_attesa[gruppo][j]!=AGV[i]->stato.pos) continue;
                 altro_agv_fermo_su_fuori_ingombro=true;
                 break;
              }
              if(altro_agv_fermo_su_fuori_ingombro==true) break;
           }
           if(altro_agv_fermo_su_fuori_ingombro==true) continue;
           //-------------------------------------------------------------
           // (1) Verifico se la missione del TGV attraversa la posizione
           //     di una navetta in RESET MISSIONE
           //-------------------------------------------------------------
           for(j=0; j<MAXPERCORSI; j++){
              if(AgvInIngombro!=0           ) break;
              if(!mission.punto_prenotato[j]) break;
              if(AGV[i]->stato.pos!=mission.punto_prenotato[j]) continue;
              //---------------------------------------------
              // TGV in inbombro
              //---------------------------------------------
              AgvInIngombro=i;
              break;
           }
           //-------------------------------------------------------------
           // (2) Verifico se la missione del TGV affinca la posizione
           //     di una navetta in RESET MISSIONE
           //-------------------------------------------------------------
           for(j=0; j<MAXPERCORSI; j++){
              if(AgvInIngombro!=0           ) break;
              if(!mission.punto_prenotato[j]) break;
              ptr_nodo = mission.punto_prenotato[j];
              x1 = Punti[ ptr_nodo ][0];
              y1 = Punti[ ptr_nodo ][1];
              for(Vicino=10; Vicino<=TOTPUNTI; Vicino++){
                 if(ptr_nodo == Vicino              ) continue;
                 if(AGV[i]->stato.pos!=Vicino       ) continue;
                 if(N.punti_notevoli[ Vicino ]==true) continue;
                 if(N.n[ Vicino ]->num ==0          ) continue;
                 //---------------------------------------------
                 // Estrai le coordinate del nodo VICINO e se
                 // DISTANZA > distanza minima --> CONTINUA
                 //---------------------------------------------
                 xv = Punti[ Vicino ][0];
                 yv = Punti[ Vicino ][1];
                 CatetoX = (double) abs(x1-xv);
                 CatetoY = (double) abs(y1-yv);
                 if(CatetoX>30000 || CatetoY>30000) continue;
                 CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
                 Dist = (int) sqrt_algo( CalcolaRad );
                 if( Dist> DIST_MIN_ROT_DOPPIA_MAX ) continue;
                 //---------------------------------------------
                 // Verifico se il ha almeno 2 nodi liberi
                 // occupabili di fronte a se.
                 //---------------------------------------------
                 if(j<MAXPERCORSI-2){
                    if((!N.nodo_busy[mission.punto_prenotato[j+1]] || N.nodo_busy[mission.punto_prenotato[j+1]]==agv_num) &&
                       (!N.nodo_busy[mission.punto_prenotato[j+2]] || N.nodo_busy[mission.punto_prenotato[j+2]]==agv_num)) continue;
                 }
                 //---------------------------------------------
                 // TGV in inbombro
                 //---------------------------------------------
                 AgvInIngombro=i;
                 break;
              }
           }
        }
     }
     CicliAttesaPerAgvInReset--;
     if(AgvInIngombro>0 && AgvInIngombro<=MAXAGV){
        *agv_bloccante = AgvInIngombro;
        return 0;
     }
  }
  CicliAttesaPerAgvInReset = CICLI_ATTESA_PER_AGV_IN_RESET;
  //----------------------------------------------------
  // (C) Calcola il numero massimo di nodi da occupare
  //----------------------------------------------------
  max_metri = MaxMetriOccupabili;
  if(tutto==true) max_metri=999;  // occupo 999metri = tutto il percorso
  if(occupa_metri(source, dest, max_metri, &punto_arrivo, &nodi_da_occupare, &metri, all_mess)) return 1;

  //----------------------------------------------------
  // (C1)
  // Se il punto di arrivo coincide con un punto
  // notevole mi accerto di occupare solo fino al
  // precedente
  //----------------------------------------------------
  if(stato.start!=0 && punto_arrivo!=0 && N.punti_notevoli[punto_arrivo]==true && N.nodo_in_ingombro[punto_arrivo]!=0){
     nodi_da_occupare--;
  }
  //---------------------------------------------------------------------------------------
  // (C1-A) 02/12/2015 GB
  // Se il punto di arrivo coincide con il punto di partenza
  // e posizione AGV (con nodi occupabili>0) mi accerto di occupare solo fino al precedente
  // N.B. Risolve alcuni casi particolari come la ripartenza da magazzini multinodo.. 
  //---------------------------------------------------------------------------------------
  if(stato.start!=0 && punto_arrivo!=0 && stato.pos==punto_arrivo && nodi_da_occupare>1){
     nodi_da_occupare--;
  }
  //------------------------------------------------------------------
  // (D) 11/12/98 La funzione "occupa_nodi()" viene richiamata
  // solo se START==0 (l'AGV ha completato la mssione parz. o tot.)
  //------------------------------------------------------------------
  if(mission.NoMissioniAlVolo==true){
     if(stato.start != 0) return 0;
  }

  //----------------------------------------------------------
  // (E) Verifica se il percorso è libero (nessun nodo occupato)
  //     sino alla destinazione
  //
  // Ritorna :
  //    percorso_libero
  //    satellite_occupato
  //    NodoInterruzionePercorso
  //
  //----------------------------------------------------------
  NodoConsiderato=0;
  PrecInterruzionePercorso=0;
  NodoInterruzionePercorso=0;
  AgvPerInterruzionePercorso=0;
  UltimoNodoDiPercorsoPrimaDellaDest=0;
  if( SempreON ){
     percorso_libero    = 0;
     agv_adiacente      = 0;
     satellite_occupato = 0;
     for( i=ptr_percorso; i<MAXPERCORSI; i++){
        NodoConsiderato = mission.punto[ i ];
        if( NodoConsiderato == 0    ) break;
        VerificaNodoOccupato(FALSE, agv_num, NodoConsiderato, NodoOccupabile, AgvPerInterruzionePercorso, satellite_occupato, NodoInterruzionePercorso, 0, 0);
        //-------------------------------------
        // Memorizzo l'ultimo nodo occupabile
        // prima dell'interruzione
        //-------------------------------------
        if(i>0 && NodoInterruzionePercorso!=0) PrecInterruzionePercorso = mission.punto[ i-1 ];
        //-------------------------------------
        // AL-05/11/2019
        // Raggiunti i metri massimi mi fermo
        //-------------------------------------
        if(NodoConsiderato==punto_arrivo) break;
        //-------------------------------------
        // Stop a dastinazione o a nodo non
        // occupabile
        //-------------------------------------
        if( NodoOccupabile  == 0                                 ) break;
        if( NodoConsiderato == dest && mission.punto[ i+1 ]==0   ) break;
     }
     if(  NodoConsiderato==dest && NodoOccupabile==1 ) percorso_libero=1;
  }
  //----------------------------------------------------------------------
  // AL-12/02/2020  GESTIONE OCCUPAZIONE NODO CON "INGOMBRO AGV"
  // Se non trovato nessun AgvInInteruzione controllo specifico
  // per TGV che occupano PT in ingombro
  //----------------------------------------------------------------------
  if(AbilitaGestioneIngombroCurvePerStopTGV==2 && AgvPerInterruzionePercorso==0){
     for(i=1; i<=MAXAGV; i++){
        if(i==agv_num                                ) continue;
        if(AGV[i]->stato.pos<1                       ) continue;
        if(AGV[i]->stato.pos>TOTPUNTI                ) continue;
        if(N.punti_notevoli[AGV[i]->stato.pos]==false) continue;
        if(N.pt_in_ingombro[AGV[i]->stato.pos]==false) continue;
        //-----------------------------------------------
        // Memorizzo pt_in_ingombro
        //-----------------------------------------------
        numero_nodo = AGV[i]->stato.pos;
        if(N.n[numero_nodo]->NodoMaster>0 && N.n[numero_nodo]->NodoMaster<=TOTPUNTI){
           if(N.punti_notevoli[N.n[numero_nodo]->NodoMaster]==true && N.pt_in_ingombro[N.n[numero_nodo]->NodoMaster]==true){
              numero_nodo = N.n[numero_nodo]->NodoMaster;
           }
        }
        //-----------------------------------------------
        // coordinate pt_in_ingombrto
        //-----------------------------------------------
        x1 = Punti[numero_nodo][0];
        y1 = Punti[numero_nodo][1];
        //-----------------------------------------------
        // solo dal punto di arrivo, gli altri sono
        // già stati analizzati
        //-----------------------------------------------
        k=MAXPERCORSI+1;
        for( j=0; j<MAXPERCORSI; j++){
           if(mission.punto_prenotato[j]<1            ) break;
           if(mission.punto_prenotato[j]>TOTPUNTI     ) break;
           if(mission.punto_prenotato[j]!=punto_arrivo) continue;
           k=j;
           break;
        }
        //-----------------------------------------------
        // Scorro i nodi ancora prenotati del TGV
        // considerato
        //-----------------------------------------------
        for( j=k+1; j<MAXPERCORSI; j++){
           if(mission.punto_prenotato[j]<1        ) break;
           if(mission.punto_prenotato[j]>TOTPUNTI ) break;
           //---------------------------------------------
           // Estrai le coordinate del nodo VICINO e se
           // DISTANZA > distanza minima --> CONTINUA
           //---------------------------------------------
           xv = Punti[mission.punto_prenotato[j]][0];
           yv = Punti[mission.punto_prenotato[j]][1];
           CatetoX = (double) abs(x1-xv);
           CatetoY = (double) abs(y1-yv);
           if(CatetoX>30000 || CatetoY>30000) continue;
           CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
           Dist = (int) sqrt_algo( CalcolaRad );
           if( Dist> DIST_MIN_ROT_DOPPIA_MAX ) continue;
           //-------------------------------------
           // Memorizzo l'ultimo nodo occupabile
           // prima dell'interruzione
           //-------------------------------------
           satellite_occupato=numero_nodo;
           AgvPerInterruzionePercorso=i;
           NodoInterruzionePercorso=mission.punto_prenotato[j];
           if(j>0) PrecInterruzionePercorso = mission.punto_prenotato[j-1];
           break;
        }
        if(AgvPerInterruzionePercorso!=0) break;
     }
  }
  //----------------------------------------------------------------------
  // Se il nodo di interruzione percorso è a soli "n" nodi di distanza dal
  // PInt appena calcolato non mi sposto nemmeno
  //----------------------------------------------------------------------
  if(MinNodiPIntSuccessivo!=0 && stato.start==1 && NodoInterruzionePercorso!=0 && NodoInterruzionePercorso!=mission.pend){
     for( i=0; i<MAXPERCORSI; i++){
        if(mission.punto_prenotato[i]<1        ) break;
        if(mission.punto_prenotato[i]>TOTPUNTI ) break;
        if(mission.punto_prenotato[i]!=pintend ) continue;
        indice=i;
        break;
     }
     //-------------------------------------------------------------
     // Conteggio nodi da punto interruzione
     //-------------------------------------------------------------
     count_da_interruzione=0;
     for(i=indice+1; i<MAXPERCORSI; i++){
        if(mission.punto_prenotato[i]<1                         ) break;
        if(mission.punto_prenotato[i]>TOTPUNTI                  ) break;
        if(mission.punto_prenotato[i]==NodoInterruzionePercorso ) break;
        count_da_interruzione++;
     }
     //-------------------------------------------------------------
     // Numero se non raggiungo il numero nodi minimo per il
     // prossimo PINT... skip!
     //-------------------------------------------------------------
     if(count_da_interruzione!=0 && count_da_interruzione<MinNodiPIntSuccessivo) return 0;
  }

  //----------------------------------------------------------------------
  // Ultimo nodo di percorso
  //----------------------------------------------------------------------
  for( i=MAXPERCORSI-1; i>ptr_percorso; i--){
     if(mission.punto[ i ]<1                      ) continue;
     if(N.punti_notevoli[mission.punto[ i ]]==true) continue;
     UltimoNodoDiPercorsoPrimaDellaDest=mission.punto[ i ];
     break;
  }
  //----------------------------------------------------------------------
  // (E1) 02/01/2013 Controllo se percorso interrotto per PT in ingombro
  //      05/02/2013 Evita che il PINT capiti a inizio/fine curva/deviaz.
  //      06/02/2013 Fermata a DIST_ROT_MAX se AgvPerInterruzionePercorso
  //                 è senza missione in corso
  // Ritorna :
  //
  //----------------------------------------------------------------------
  if( SempreON ){
     //---------------------------------------------------------------------------------
     // -Il percorso si può interrompere per una rot. anche sucessiva al PT
     // -Memorizza il nodo di percorso Adiacente al PT (Nodo di uscita da PT)
     // -Verifica se appartiene al percorso e interrompo in anticipo
     //
     // GESTIONE CURVE/DEVIAZIONI
     // -Verifica se il nodo di interruzione è in ing/out curva
     // -Interrompo in anticipo su primo nodo in rettilineo
     //
     // ALTRO TGV SENZA MISSIONE IN CORSO
     // -Se l'altro TGV è fermo senza missione in corso lungo il percorso devo
     //  fermarmi a una DIST_MIN_ROT_DOPPIA_MAX per garantire che questo possa
     //  ripartire in tutte le direzioni
     //---------------------------------------------------------------------------------
     DistIngombro=0;
     NumTgvPerAnticipo=0;                            // Numero del TGV che ha causato l'anticipo del Punto intermedio.
     NodoIngombroPercorsoAnticipato=0;               // Nodo di percorso anticipato causa PTinIng, Curva o Deviaz.
     NodoPercorsoIngombroPt=0;
     IndicePercorsoIngOutCrvDev=0;
     NodoPercorsoIngombroAnticipatoPt=0;
     NodoPercorsoIngombroAnticipatoCrvDev=0;         // Nodo di percorso in Ingombro anticipato per CURVA o DEVIAZ. (su primo tratto rettilineo disponibile)
     NodoPercorsoIngombroAnticipatoAltroAgvFermo=0;  // Nodo di percorso in Ingombro anticipato per altro TGV fermo senza missione
     if( satellite_occupato>0 && satellite_occupato< TOTPUNTI ){
        //-------------------------------------------------------
        // Il satellite occupato è un PT in ingombro
        //-------------------------------------------------------
        if(N.punti_notevoli[ satellite_occupato ]){
           if( N.pt_in_ingombro[ satellite_occupato ]==true){
            //NodoPercorsoIngombroPt = N.n[ satellite_occupato ]->rec[0].ps;
              if(AGV[AgvPerInterruzionePercorso]->stato.pos>0 && AGV[AgvPerInterruzionePercorso]->stato.pos<=TOTPUNTI){
                 //----------------------------------------------------
                 // AL-16/01/2014 PROBLEMA SALTO NODI
                 // Se il TGV adiacente è già sul satellite_occupato
                 // lo setto come NodoPercorsoIngombroPt come primo
                 // dei possibili nodi di uscita in caso contrario
                 // è la ricerca sotto che deve determinare il
                 // "NodoPercorsoIngombroPt"
                 //----------------------------------------------------
                 if(AGV[AgvPerInterruzionePercorso]->stato.pos==satellite_occupato){
                    NodoPercorsoIngombroPt = N.n[ satellite_occupato ]->rec[0].ps;
                    //---------------------------------------------------------
                    // AL-21/03/2019 ATTESA DA PERCORSO ANCHE PER MULTI-NODO
                    // Se il satellite_occupato ho un nodo master, il
                    // punto successivo di percorso viene recuparato dal nodo
                    // master
                    //---------------------------------------------------------
                    if(N.n[ satellite_occupato ]->NodoMaster>0 && N.n[ satellite_occupato ]->NodoMaster<=TOTPUNTI){
                       NodoPercorsoIngombroPt = N.n[ N.n[ satellite_occupato ]->NodoMaster ]->rec[0].ps;
                    }
                    //---------------------------------------------------------
                    // Il nuovo satellite occupato a cui fare riferimento per
                    // la distanza diventerà il nodo di uscita
                    //---------------------------------------------------------
                    satellite_occupato = NodoPercorsoIngombroPt;
                 }
                 //------------------------------------------------------
                 // AL-21/01/2014 Gestione NODI MASTER IN INGOMBRO
                 // Se il satellite occupato è Nodo Master della pos del
                 // TGV bloccante, è da considerarsi cmq pt_in_ingombro
                 //------------------------------------------------------
                 if(N.n[AGV[AgvPerInterruzionePercorso]->stato.pos]->NodoMaster==satellite_occupato){
                    NodoPercorsoIngombroPt = N.n[ satellite_occupato ]->rec[0].ps;
                    //---------------------------------------------------------
                    // Il nuovo satellite occupato a cui fare riferimento per
                    // la distanza diventerà il nodo di uscita
                    //---------------------------------------------------------
                    satellite_occupato = NodoPercorsoIngombroPt;
                 }
              }
           }
        }
        //-------------------------------------------------------
        // 22/01/2013 anche se il satellite occupato è precedente
        // a un PT in ingobro devo memorizzarlo per garantire una
        // fermata anticipata che permetta poi alla navetta di
        // uscire
        //-------------------------------------------------------
        if(NodoPercorsoIngombroPt==0 && N.pt_in_ingombro[ AGV[AgvPerInterruzionePercorso]->mission.pend ]==true){
           for( i=MAXPERCORSI-1; i>=0;  i--){
              if( AGV[AgvPerInterruzionePercorso]->mission.punto[ i ]==0 ) continue;
              //------------------------------------------------------------
              // Satellite occupato precedente a un PT in ingobro
              //------------------------------------------------------------
              if(AGV[AgvPerInterruzionePercorso]->mission.punto[ i ]==AGV[AgvPerInterruzionePercorso]->mission.pend){
               //if(satellite_occupato==AGV[AgvPerInterruzionePercorso]->mission.punto[ i-1 ]){
               //   NodoPercorsoIngombroPt = satellite_occupato;
               //}
                 //-----------------------------------------------------------------
                 // AL-16/01/2014 PROBLEMA SALTO NODI
                 // In ogni caso il satellite occupato e il NodoPercorsoIngombroPt
                 // devono essere il nodo precedente alla destinazione per avere
                 // garanzia di uscita
                 //-----------------------------------------------------------------
                 if(satellite_occupato==AGV[AgvPerInterruzionePercorso]->mission.punto[ i-1 ] || satellite_occupato==AGV[AgvPerInterruzionePercorso]->mission.punto[ i ]){
                    NodoPercorsoIngombroPt = AGV[AgvPerInterruzionePercorso]->mission.punto[i-1];
                    //---------------------------------------------------------
                    // Il nuovo satellite occupato a cui fare riferimento per
                    // la distanza diventerà il nodo di uscita
                    //---------------------------------------------------------
                    satellite_occupato = NodoPercorsoIngombroPt;
                 }
              }
              break;
           }
           //-------------------------------------------------------
           // AL-21/01/2014 Gestione NODI MASTER IN INGOMBRO
           // Controllo nodo precedente anticipato a Nodo Master
           //-------------------------------------------------------
           if(N.n[AGV[AgvPerInterruzionePercorso]->mission.pend]->NodoMaster!=0){
              for( i=MAXPERCORSI-1; i>=0;  i--){
                 if( AGV[AgvPerInterruzionePercorso]->mission.punto[ i ]==0 ) continue;
                 //------------------------------------------------------------
                 // Satellite occupato precedente a un PT in ingobro
                 //------------------------------------------------------------
                 if(AGV[AgvPerInterruzionePercorso]->mission.punto[ i ]==N.n[AGV[AgvPerInterruzionePercorso]->mission.pend]->NodoMaster){
                    if(satellite_occupato==AGV[AgvPerInterruzionePercorso]->mission.punto[ i-1 ] || satellite_occupato==AGV[AgvPerInterruzionePercorso]->mission.punto[ i ]){
                       NodoPercorsoIngombroPt = AGV[AgvPerInterruzionePercorso]->mission.punto[i-1];
                       //---------------------------------------------------------
                       // Il nuovo satellite occupato a cui fare riferimento per
                       // la distanza diventerà il nodo di uscita
                       //---------------------------------------------------------
                       satellite_occupato = NodoPercorsoIngombroPt;
                    }
                    break;
                 }
              }
           }
        }
        //-------------------------------------------------------
        // 05/02/2013 SATELLITE OCCUPATO IN ING/OUT CURVA
        // Verifico che il nodo di interruzione percorso
        // non sia in ingresso o uscita curva.
        //-------------------------------------------------------
        if(NodoInterruzionePercorso>0 && NodoInterruzionePercorso<=TOTPUNTI && NodoInterruzionePercorso!=mission.pend){
           //----------------------------------------------------------------
           // Punto all'indice di percorso su cui trovo il
           // NodoInterruzionePercorso per nodo occupato
           //----------------------------------------------------------------
           for( i=MAXPERCORSI-1; i>ptr_percorso;  i--){
              if( mission.punto[ i ]!=NodoInterruzionePercorso ) continue;
              break;
           }
           //----------------------------------------------------------------
           // Considero come reale nodo di interruzione percorso il primo
           // nodo a una distanza DIST_MIN_ROT_DOPPIA_MAX dal satellite_occ
           // per avere la garanzia di raggiungerlo
           //----------------------------------------------------------------
           for(; i>ptr_percorso;  i--){
              x1 = Punti[ mission.punto[ i ] ][0];
              y1 = Punti[ mission.punto[ i ] ][1];
              xv = Punti[ satellite_occupato ][0];
              yv = Punti[ satellite_occupato ][1];
              CatetoX = (double) abs(x1-xv);
              CatetoY = (double) abs(y1-yv);
              if(CatetoX>30000 || CatetoY>30000) continue;
              CalcolaRad   = (CatetoX*CatetoX) + (CatetoY*CatetoY);
              DistIngombro = (int) sqrt_algo( CalcolaRad );
              rot1         = mission.dati_perc[i].rot;
            //if( DistIngombro<DIST_MIN_ROT_DOPPIA_MAX) continue;
              if( DistIngombro<DIST_MIN_ROT_DOPPIA_MAX || rot1>3 ){  // se punto di inzio curva o distanza inferiore a ROT_DOPPIA_MAX
                 //--------------------------------------------------------
                 // 26/10/2013 (NECESSARIO PER NAVETTE LASER)
                 // Verifico se i TGV sono già ruotati su rette parallele
                 //--------------------------------------------------------
                 if(mission.GestioneLaser==true || AGV[AgvPerInterruzionePercorso]->mission.GestioneLaser==true){
                    AgvInIngombro=true;
                    if(i>0 && i<MAXPERCORSI && AGV[AgvPerInterruzionePercorso]->test_mission_on()){
                       rot2 = 99;
                       for(j=1; j<MAXPERCORSI-1; j++){   // scarto primo e ultimo nodo
                          if(AGV[AgvPerInterruzionePercorso]->mission.punto[j]==0) break;
                          if(AGV[AgvPerInterruzionePercorso]->mission.punto[j]!=satellite_occupato) continue;
                          rot2 = AGV[AgvPerInterruzionePercorso]->mission.dati_perc[j].rot;
                          break;
                       }
                       //---------------------------------------------------------------
                       // Verifico se i TGV si trovano su rette parallele
                       //---------------------------------------------------------------
                       RetteParallele=false;
                       // rette parallele in "X"
                       if(CatetoX<=DIST_MIN_ING_CURVA && CatetoY>=DIST_MIN_RETTE_PARALLELE && ((rot1==1 || rot1==3) && (rot2==1 || rot2==3))) RetteParallele=true;
                       // rette parallele in "Y"
                       if(CatetoY<=DIST_MIN_ING_CURVA && CatetoX>=DIST_MIN_RETTE_PARALLELE && ((rot1==0 || rot1==2) && (rot2==0 || rot2==2))) RetteParallele=true;
                       //---------------------------------------------------------------
                       // Se le navette risultano su tratti paralleli devono essere
                       // già ruotate
                       //---------------------------------------------------------------
                       if(RetteParallele==true){
                          //---------------------------------------------------------------
                          // Anche le rotazioni precedenti e successive di entrambi i TGV
                          // devono essere le stesse
                          //---------------------------------------------------------------
                          if(mission.dati_perc[i-1].rot==rot1 && mission.dati_perc[i+1].rot==rot1 &&
                             AGV[AgvPerInterruzionePercorso]->mission.dati_perc[j-1].rot==rot2 && AGV[AgvPerInterruzionePercorso]->mission.dati_perc[j+1].rot==rot2){
                                AgvInIngombro=false;
                          }
                       }
                    }
                    //----------------------------------------------------
                    // Se la distanza è inferiore DIST_MIN_ROT_DOPPIA_MAX
                    // e le rette non sono parallele
                    //----------------------------------------------------
                    if(AgvInIngombro==true) continue;
                 }
              }
              //-------------------------------------------------------
              // Raggiunto un nodo di interruzione che sia sicuramente
              // raggiungibile, se coincide con un ing/out curva, ne
              // memorizzo l'indice
              //
              // AL-28/03/2013 Solo se abilitata gestione no stop
              //               su start deviazione
              //
              // AL-15/01/2014 Solo se abilitata gestione no stop
              //               su start curva
              //
              // AL-23/01/2014 A seconda del flag attivo controllo
              //               lo stato rotazione
              //-------------------------------------------------------
              if(NoPIntSuStartDeviazione==true || (NoPIntSuStartCurva==true && mission.GestioneLaser==true)){
               //if(IndicePercorsoIngOutCrvDev==0 && mission.dati_perc[i  ].rot>3) IndicePercorsoIngOutCrvDev=i;
               //if(IndicePercorsoIngOutCrvDev==0 && mission.dati_perc[i-1].rot>3) IndicePercorsoIngOutCrvDev=i-1;
                 // DEVIAZIONE = ROT>49 && ROT<70
                 if(NoPIntSuStartDeviazione==true){
                    if(IndicePercorsoIngOutCrvDev==0 && mission.dati_perc[i  ].rot>49 && mission.dati_perc[i  ].rot<70) IndicePercorsoIngOutCrvDev=i;
                    if(IndicePercorsoIngOutCrvDev==0 && mission.dati_perc[i-1].rot>49 && mission.dati_perc[i-1].rot<70) IndicePercorsoIngOutCrvDev=i-1;
                 }
                 // CURVA = ROT>3 && ROT<40
                 if(NoPIntSuStartCurva==true){
                    if(IndicePercorsoIngOutCrvDev==0 && mission.dati_perc[i  ].rot>3 && mission.dati_perc[i  ].rot<40) IndicePercorsoIngOutCrvDev=i;
                    if(IndicePercorsoIngOutCrvDev==0 && mission.dati_perc[i-1].rot>3 && mission.dati_perc[i-1].rot<40) IndicePercorsoIngOutCrvDev=i-1;
                 }
              }
              //-------------------------------------------------------
              // Raggiunto un nodo di interruzione che sia sicuramente
              // raggiungibile, se il TGV di interruzione è senza
              // missione in corso memorizzo il nodo in modo che
              // il TGV considerato si fermi a una distanza di
              // sicurezza che permetta all'altra navetta di ripartire
              //-------------------------------------------------------
              if(ForzaPIntPerNavettaInAttesa==true && AgvPerInterruzionePercorso>0 && AgvPerInterruzionePercorso<=MAXAGV && (AGV[AgvPerInterruzionePercorso]->stato.start==0 || AGV[AgvPerInterruzionePercorso]->mission.pstart==0)){
                 if(N.punti_notevoli[ AGV[AgvPerInterruzionePercorso]->stato.pos ]==false && N.verifica_nodo_fuori_ingombro(AGV[AgvPerInterruzionePercorso]->stato.pos)==false){
                    NodoPercorsoIngombroAnticipatoAltroAgvFermo = mission.punto[ i ];
                 }
              }
              break;
           }
        }
     }
     //-------------------------------------------------------------------------------
     // AL-17/01/2014 NESSUNA INTERRUZIONE FINO A DESTINAZIONE PT_IN_INGOMBRO
     // Se il TGV non ha nessuna interruzione fino al pt_in_ingombro, ma un altro TGV
     // deve transitare a una distanza < DIST_ROT_DOPPIA_MAX dal pt_in_ingombro e ha
     // già occupato un nodo a una distanza < DIST_ROT_DOPPIA_MAX dal pt_in_ingombro
     // (quindi non si può fermare) --> DEVO FERMARE IL TGV CONSIDERATO
     //-------------------------------------------------------------------------------
     if(NodoPercorsoIngombroPt==0 && IndicePercorsoIngOutCrvDev==0 && NodoPercorsoIngombroAnticipatoAltroAgvFermo==0){
        //---------------------------------------------------------
        // CASO 1: il TGV considerato ha il percorso libero fino
        //         a destinazione e questa è un pt_in_ingombro
        //---------------------------------------------------------
        if((percorso_libero==1 || NodoInterruzionePercorso==mission.pend) && UltimoNodoDiPercorsoPrimaDellaDest!=0 && N.punti_notevoli[ mission.pend ]==true && N.pt_in_ingombro[ mission.pend ]==true){
           //-------------------------------------------
           // Verifico se c'è un altro TGV con un PINT
           // a una distanza < DIST_ROT_DOPPIA_MAX
           // (che quindi non si può fermare)
           //-------------------------------------------
           x1 = Punti[ UltimoNodoDiPercorsoPrimaDellaDest ][0];
           y1 = Punti[ UltimoNodoDiPercorsoPrimaDellaDest ][1];
           for(i=1; i<=MAXAGV; i++){
              if(i==agv_num                       ) continue;
              if(AGV[i]->mission.pintend<1        ) continue;
              if(AGV[i]->mission.pintend>TOTPUNTI ) continue;
              xv = Punti[ AGV[i]->mission.pintend ][0];
              yv = Punti[ AGV[i]->mission.pintend ][1];
              CatetoX = (double) abs(x1-xv);
              CatetoY = (double) abs(y1-yv);
              if(CatetoX>30000 || CatetoY>30000) continue;
              CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
              DistIngombro = (int) sqrt_algo( CalcolaRad );
              if(DistIngombro>=DIST_MIN_ROT_DOPPIA_MAX) continue;
              //------------------------------------------------------
              // Se il TGV continua ad avvicinarsi:
              // cotrollo il successivo al PINT già occupato se è
              // ancora più vicino al PT_IN_INGOMBRO
              // (l'unico controllo possibile che funziona in
              //  in previsione di SALTI NODO)
              //------------------------------------------------------
              for(j=MAXPERCORSI-1; j>=0; j--){
                 if(AGV[i]->mission.punto[j]!=AGV[i]->mission.pintend) continue;
                 //-------------------------------------------------------
                 // Se successivo esiste verifico se più vicino
                 //-------------------------------------------------------
                 if(AGV[i]->mission.punto[j+1]<1        ) break;
                 if(AGV[i]->mission.punto[j+1]>TOTPUNTI ) break;
                 xv = Punti[ AGV[i]->mission.punto[j+1] ][0];
                 yv = Punti[ AGV[i]->mission.punto[j+1] ][1];
                 CatetoX = (double) abs(x1-xv);
                 CatetoY = (double) abs(y1-yv);
                 if(CatetoX>30000 || CatetoY>30000) continue;
                 CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
                 DistIngombroSucc = (int) sqrt_algo( CalcolaRad );
                 //-------------------------------------------------------
                 // Allontanamento da PT_IN_INGOMBRO
                 //-------------------------------------------------------
                 if(DistIngombroSucc>DistIngombro) continue;
                 //-------------------------------------------------------
                 // ALTROTGV in avvicinamento a PT_IN_INGOMBRO con
                 // TGV considerato in dest = PT_IN_INGOMBRO
                 //
                 // Setto come "NodoInterruzionePercorso" il pint del
                 // tgv adiacente e come "NodoInterruzionePercorso"  il
                 // nodo prima del pt_in_ingombro in modo da garantire
                 // una fermata del tgv considerato sufficenemente distante
                 // da far transitare il tgv adiacente
                 //-------------------------------------------------------
                 AgvPerInterruzionePercorso = i;
                 NodoPercorsoIngombroPt     = AGV[i]->mission.pintend;      // !!! IMPORTANTE
                 NodoInterruzionePercorso   = UltimoNodoDiPercorsoPrimaDellaDest;
                 //---------------------------------------------------------
                 // Il nuovo satellite occupato a cui fare riferimento per
                 // la distanza diventerà il nodo di uscita
                 //---------------------------------------------------------
                 satellite_occupato         = AGV[i]->mission.pintend;
                 break;
              }
              if(NodoPercorsoIngombroPt!=0) break;
           }
        }
        //---------------------------------------------------------
        // CASO 2: il TGV considerato ha un interruzione di perc
        //         ma il nodo precedente all'interruzione è un
        //         pt_in_ingombro.
        //---------------------------------------------------------
        if(NodoPercorsoIngombroPt==0){
           if(percorso_libero==0 && NodoInterruzionePercorso!=0 && NodoInterruzionePercorso!=mission.pend && PrecInterruzionePercorso!=0 && N.punti_notevoli[ PrecInterruzionePercorso ]==true && N.pt_in_ingombro[ PrecInterruzionePercorso ]==true){
              //-------------------------------------------
              // Verifico se c'è un altro TGV con un PINT
              // a una distanza < DIST_ROT_DOPPIA_MAX
              // (che quindi non si può fermare)
              //-------------------------------------------
              x1 = Punti[ NodoInterruzionePercorso ][0];
              y1 = Punti[ NodoInterruzionePercorso ][1];
              for(i=1; i<=MAXAGV; i++){
                 if(i==agv_num                       ) continue;
                 if(AGV[i]->mission.pintend<1        ) continue;
                 if(AGV[i]->mission.pintend>TOTPUNTI ) continue;
                 xv = Punti[ AGV[i]->mission.pintend ][0];
                 yv = Punti[ AGV[i]->mission.pintend ][1];
                 CatetoX = (double) abs(x1-xv);
                 CatetoY = (double) abs(y1-yv);
                 if(CatetoX>30000 || CatetoY>30000) continue;
                 CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
                 DistIngombro = (int) sqrt_algo( CalcolaRad );
                 if(DistIngombro>=DIST_MIN_ROT_DOPPIA_MAX) continue;
                 //------------------------------------------------------
                 // Se il TGV continua ad avvicinarsi:
                 // cotrollo il successivo al PINT già occupato se è
                 // ancora più vicino al PT_IN_INGOMBRO
                 // (l'unico controllo possibile che funziona in
                 //  in previsione di SALTI NODO)
                 //------------------------------------------------------
                 for(j=MAXPERCORSI-1; j>=0; j--){
                    if(AGV[i]->mission.punto[j]!=AGV[i]->mission.pintend) continue;
                    //-------------------------------------------------------
                    // Se successivo esiste verifico se più vicino
                    //-------------------------------------------------------
                    if(AGV[i]->mission.punto[j+1]<1        ) break;
                    if(AGV[i]->mission.punto[j+1]>TOTPUNTI ) break;
                    xv = Punti[ AGV[i]->mission.punto[j+1] ][0];
                    yv = Punti[ AGV[i]->mission.punto[j+1] ][1];
                    CatetoX = (double) abs(x1-xv);
                    CatetoY = (double) abs(y1-yv);
                    if(CatetoX>30000 || CatetoY>30000) continue;
                    CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
                    DistIngombroSucc = (int) sqrt_algo( CalcolaRad );
                    //-------------------------------------------------------
                    // Allontanamento da PT_IN_INGOMBRO
                    //-------------------------------------------------------
                    if(DistIngombroSucc>DistIngombro) continue;
                    //-------------------------------------------------------
                    // ALTROTGV in avvicinamento a PT_IN_INGOMBRO con
                    // TGV considerato transita su PT_IN_INGOMBRO come
                    // percorso.
                    //
                    // Setto come "NodoPercorsoIngombroPt" il
                    // NodoDiInteruzione del tgv considerato, questo
                    // garantisce di fermarsi molto prima e di far
                    // transitare il tgv adiacente
                    //-------------------------------------------------------
                    NodoPercorsoIngombroPt = NodoInterruzionePercorso;
                    //---------------------------------------------------------
                    // Il nuovo satellite occupato a cui fare riferimento per
                    // la distanza diventerà il nodo di uscita
                    //---------------------------------------------------------
                    satellite_occupato     = AGV[i]->mission.pintend;
                    break;
                 }
                 if(NodoPercorsoIngombroPt!=0) break;
              }
           }
        }           
     }
     //------------------------------------------------------
     // Calcola punto di Stop anticipato  ANDANDO a RITROSO
     //------------------------------------------------------
     if( NodoPercorsoIngombroPt || IndicePercorsoIngOutCrvDev || NodoPercorsoIngombroAnticipatoAltroAgvFermo){
        //-------------------------------------------------------
        // (1) ANTICIPO PER PT IN INGOBRO
        // Mi devo fermare sul primo nodo a una distanza pari a
        // DIST_MIN_ROT_DOPPIA_MAX
        //-------------------------------------------------------
        if(NodoPercorsoIngombroPt!=0){
           for( i=MAXPERCORSI-1; i>ptr_percorso;  i--){
              if( mission.punto[ i ]==0 ) continue;
              //---------------------------------------------------------
              // 05/02/2013 valuto solo i nodi di rettilineo
              //
              // AL-22/02 non devo scartare i nodi di deviazione
              //---------------------------------------------------------
            //if(mission.dati_perc[i  ].rot>3 ) continue;
            //if(mission.dati_perc[i-1].rot>3 ) continue;

              NodoConsiderato = mission.punto[ i ];
              //--------------------------------------------------------------------------------------------
              // Verifico il nodo di interruzione o quello di ingombro
              //--------------------------------------------------------------------------------------------
              if( NodoConsiderato==NodoPercorsoIngombroPt || NodoConsiderato==NodoInterruzionePercorso){
                 NodoPercorsoIngombroAnticipatoPt=NodoConsiderato;
              }
              //--------------------------------------------------------------------------------------------
              // La distanza deve essere DIST_MIN_ROT_DOPPIA_MAX --> TGV deve potere uscire e ruotare
              //--------------------------------------------------------------------------------------------
              if( NodoPercorsoIngombroAnticipatoPt>0 && NodoConsiderato!=NodoPercorsoIngombroPt){
                 x1 = Punti[ NodoConsiderato ][0];
                 y1 = Punti[ NodoConsiderato ][1];
                 xv = Punti[ NodoPercorsoIngombroPt ][0];
                 yv = Punti[ NodoPercorsoIngombroPt ][1];
                 CatetoX = (double) abs(x1-xv);
                 CatetoY = (double) abs(y1-yv);
                 if(CatetoX>30000 || CatetoY>30000) continue;
                 CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
                 DistIngombro = (int) sqrt_algo( CalcolaRad );
                 //--------------------------------------------------------------------------------------
                 // AL-22/02 Memorizzo l'ultimo nodo a una distanza < DIST_MIN_ROT_DOPPIA_MAX o che
                 //          si trova su un tratto inclinato
                 //--------------------------------------------------------------------------------------
                 if(NoPIntSuStartDeviazione==true || (NoPIntSuStartCurva==true && mission.GestioneLaser==true)){
                    //-----------------------------------------------------
                    // AL-23/01/2014 A seconda del flag attivo controllo
                    //               lo stato rotazione
                    //-----------------------------------------------------
                    rot_not_ok=false;
                    if(NoPIntSuStartDeviazione==true){
                       if(mission.dati_perc[i  ].rot>49 && mission.dati_perc[i  ].rot<70) rot_not_ok=true;
                       if(mission.dati_perc[i-1].rot>49 && mission.dati_perc[i-1].rot<70) rot_not_ok=true;
                    }
                    // CURVA = ROT>3 && ROT<40
                    if(NoPIntSuStartCurva==true){
                       if(mission.dati_perc[i  ].rot> 3 && mission.dati_perc[i  ].rot<40) rot_not_ok=true;
                       if(mission.dati_perc[i-1].rot> 3 && mission.dati_perc[i-1].rot<40) rot_not_ok=true;
                    }
                  //if( DistIngombro<=DIST_MIN_ROT_DOPPIA_MAX || mission.dati_perc[i].rot>3 || mission.dati_perc[i-1].rot>3){
                    if( DistIngombro<=DIST_MIN_ROT_DOPPIA_MAX || rot_not_ok==true){
                       NodoPercorsoIngombroAnticipatoPt=NodoConsiderato;
                    }else{
                       break;
                    }
                 }
                 //--------------------------------------------------------------------------------------
                 // AL-17/01/2014 Se è abilitato lo stop su Start/End DEVIAZIONE e CURVA memorizzo solo
                 // l'ultimo nodo a distanza < DIST_MIN_ROT_DOPPIA_MAX
                 //--------------------------------------------------------------------------------------
                 else{
                    if( DistIngombro<=DIST_MIN_ROT_DOPPIA_MAX){
                       NodoPercorsoIngombroAnticipatoPt=NodoConsiderato;
                    }else{
                       break;
                    }
                 }
              }
           }
        }
        //-------------------------------------------------------
        // (2) ANTICIPO PER ING/OUT CURVA o DEVIAZIONE
        // Mi devo fermare sul primo nodo di rettilineo
        // cominciando a scorrere il percorso a ritroso
        //-------------------------------------------------------
        if(IndicePercorsoIngOutCrvDev>0){
           for( i=IndicePercorsoIngOutCrvDev; i>ptr_percorso;  i--){
              if( mission.punto[ i ]==0       ) continue;
              //---------------------------------------------------------
              // Memorizzo l'ultimo nodo che presenta una curva/deviaz.
              //---------------------------------------------------------
              if(mission.dati_perc[i  ].rot>3 ){
                 NodoPercorsoIngombroAnticipatoCrvDev = mission.punto[ i ];
              }else{
                 break;
              }
           }
        }
        //-------------------------------------------------------
        // (3) Tra i 3 nodi di anticipo eventualmente trovati
        // scelgo quello PIU' ANTICIPATO
        //-------------------------------------------------------
        for( i=ptr_percorso; i<MAXPERCORSI; i++){
           if( mission.punto[ i ]==0 ) break;
           if( mission.punto[ i ]!=NodoPercorsoIngombroAnticipatoPt &&
               mission.punto[ i ]!=NodoPercorsoIngombroAnticipatoCrvDev &&
               mission.punto[ i ]!=NodoPercorsoIngombroAnticipatoAltroAgvFermo) continue;
           //------------------------------------------------------------------------------
           // Scelgo il primo dei 2 nodi anticipati che trovo nell'esecuzione del percorso
           //------------------------------------------------------------------------------
           NodoIngombroPercorsoAnticipato = mission.punto[ i ];
           NumTgvPerAnticipo              = AgvPerInterruzionePercorso;
           break;
        }
     }
  }

  //---------------------------------------------------------------
  // (F) 18/10/95  Compila Array nodi prenotati da tutti gli AGV
  //         Indice 1 = Numero nodo
  //         Indice 2 = Numero AGV che ha prenotato ( 0 = NO)
  //---------------------------------------------------------------
  memset(&nodi_prenotati , 0, sizeof(nodi_prenotati));
  for( altro_agv=1; altro_agv<=MAXAGV; altro_agv++){
     for( i=0; i<MAXPERCORSI; i++){
        numero_nodo = AGV[ altro_agv ]->mission.punto_prenotato[i];
        if( numero_nodo==0 ) break;
        if( numero_nodo > TOTPUNTI ) break;
        nodi_prenotati[ numero_nodo ][ altro_agv-1 ] = altro_agv;
     }
  }

  //----------------------------------------------------------
  // (G) Valuta se OCCUPARE  NODO
  //----------------------------------------------------------
  salto_a_nodo       = 0;
  nodi_occupati      = 0;
  satellite_occupato = 0;
  *agv_bloccante     = 0;
  AgvBloccante       = 0;
  //if(stato.start == 0){
  for( ; ptr_percorso<MAXPERCORSI; ptr_percorso++){
     RioccupaNodo = false;
     ptr_nodo = mission.punto[ptr_percorso];
     if(ptr_nodo > TOTPUNTI+10) break;
     //-------------------------------------------------------
     // GESTIONE SALTA NODI
     // Non posso assolutamente forzare come PINTEND un nodo
     // che ho previsto di saltare.
     //-------------------------------------------------------
     salto_a_nodo = mission.dati_perc[ptr_percorso].salta_a_ps;
     satellite_occupato1 = 0;
     memset(&nodi_da_saltare[0], 0, sizeof(nodi_da_saltare));
     if(salto_a_nodo!=0){
        //------------------------------------------------------
        // Verifico se tutti i nodi fina a quello a cui saltare
        // sono occupabili e li occupo
        //------------------------------------------------------
        indice=0;
        for(i=ptr_percorso; i<MAXPERCORSI; i++){
           if(mission.punto[i]==0           ) break;
           if(mission.punto[i]==salto_a_nodo) break;
           ptr_nodo1 = mission.punto[i];
           //------------------------------------------------
           // Verifico se il nodo considerato è già occupato
           //------------------------------------------------
           if(N.nodo_busy[ptr_nodo1]!=0 && N.nodo_busy[ptr_nodo1]!=agv_num){
              satellite_occupato1 = ptr_nodo1;
              break;
           }
           //-----------------------------------------------------------------------
           // 02/01/2013 Controllo se percorso interrotto per PT in ingombro
           // Interromi SALTO NODI
           //-----------------------------------------------------------------------
           if(ptr_nodo1==NodoIngombroPercorsoAnticipato){
              satellite_occupato1 = ptr_nodo1;
              break;
           }
           //------------------------------------------------
           // Verifico se i nodi vicini sono TUTTI fuori
           // ingombro
           //------------------------------------------------
           NodoOccupabile1=AgvPerInterruzionePercorso1=NodoInterruzionePercorso1=0;
           VerificaNodoOccupato(TRUE, agv_num, ptr_nodo1, NodoOccupabile1, AgvPerInterruzionePercorso1, satellite_occupato1, NodoInterruzionePercorso1, 0, 0);
           if(NodoOccupabile1==0){
              satellite_occupato1 = ptr_nodo1;
              break;
           }
           //----------------------------------------------
           // Se il nodo considerato è disponibile
           // compilo la struttura dei nodi da saltare
           //----------------------------------------------
           nodi_da_saltare[indice] = ptr_nodo1;
           indice++;
        }
        //-----------------------------------------------
        // Se nessuno tra nodi da saltare è impegnato
        // memorizzo l'indice a cui saltare nel ciclo
        // di occupazioni nodo successivo.
        //-----------------------------------------------
        if(satellite_occupato1==0){
           ptr_percorso = i;
           ptr_nodo     = salto_a_nodo;
        }
     }
     //---------------------------------------------------------
     // (G1) Se il NODO è libero verifica se quelli circostanti
     //      siano altrettanto liberi
     //---------------------------------------------------------
   //if(N.nodo_busy[ptr_nodo]==0){
     j=1;
     PuntoConsiderato = mission.punto[ptr_percorso];
     memset(&PSucc[0], 0, sizeof(PSucc));
     memset(&DSucc[0], 0, sizeof(DSucc));
     PSucc[0] = PuntoConsiderato;
     for(i=ptr_percorso+1; i<MAXPERCORSI; i++){
        if(mission.punto[i]<1             ) break;
        if(mission.punto[i]>TOTPUNTI      ) break;
        if(mission.punto[i]==mission.pend ) break;
        PSucc[j] = mission.punto[i];
        N.VerificaDistanzaPunti(PSucc[0], PSucc[j], &DistX, &DistY, &DSucc[j]);
        if(DSucc[j]>=DIST_MIN_ROT_DOPPIA_MAX) break;
        j++;
     }
     //---------------------------------------------------------
     // AL-06/02/2014 Nello start da Punto di percorso
     // è necessario valutare anche se ci sono le condizioni
     // per rioccupare il nodo su cui il TGV si trova per
     // poterlo conservare come PINTEND
     //---------------------------------------------------------
     if(stato.start==0 && N.punti_notevoli[stato.pos]==false && ptr_nodo==stato.pos){
        last_nodo=pintend=0;
        RioccupaNodo = true;
     }
     //---------------------------------------------------------
     // (G3) Se il NODO è libero verifica se quelli circostanti
     //      siano altrettanto liberi
     //---------------------------------------------------------
     if(N.nodo_busy[ptr_nodo]==0 || RioccupaNodo==true){
        //-------------------------------------------------------
        // Estrai le coordinate del nodo da occupare
        // (x1,y1) Attuale (x2,y2) Precedente (x3, y3) Sucessivo
        //-------------------------------------------------------
        x1 = Punti[ ptr_nodo ][0];
        y1 = Punti[ ptr_nodo ][1];
        //-------------------------------------------------------------------------------
        // Verifico se il TGV deve percorrere un ANGOLO RETTO
        //-------------------------------------------------------------------------------
        rot1 = rot_standard(ptr_percorso  );
        rot2 = rot_standard(ptr_percorso+2);
        if(rot1!=rot2) AgvSuAngolo=true;
        //-------------------------------------------------------------------------------
        // (G3) Altrimenti verifica se nodo SATELLITE OCCUPATO: DIST<DIST_MIN_ROT_DOPPIA_MAX
        //
        // Ritorna :
        //
        //    NodoOccupabile           = 1 -> OK
        //    agv_adiacente            = xx
        //    satellite_occupato       = yy
        //    NodoInterruzionePercorso = zz
        //
        //-------------------------------------------------------------------------------
        VerificaNodoOccupato(RioccupaNodo, agv_num, PuntoConsiderato, NodoOccupabile, agv_adiacente, satellite_occupato, NodoInterruzionePercorso, 0, 0);
        //-----------------------------------------------------------------------
        // 02/01/2013 Controllo se percorso interrotto per PT in ingombro
        // a) Percorso Standard
        // b) Percorso Con Salto Nodi
        //
        // AL-22/02 setto il satellite_occupato!=0 solo se esiste il nodo
        //          anticipato (SICUREZZA)
        //-----------------------------------------------------------------------
        if(NodoIngombroPercorsoAnticipato!=0){
         //if(PuntoConsiderato==NodoIngombroPercorsoAnticipato || (mission.pintend!=0 && mission.pintend==NodoIngombroPercorsoAnticipato)){
           if((PuntoConsiderato==NodoIngombroPercorsoAnticipato && NumTgvPerAnticipo!=agv_num) || (mission.pintend!=0 && mission.pintend==NodoIngombroPercorsoAnticipato)){
              agv_adiacente=NumTgvPerAnticipo;
              NodoOccupabile=0;
              satellite_occupato=NodoIngombroPercorsoAnticipato;
           }
        }
        //-------------------------------------------------------------------------------
        // Se il nodo risulta occupabile verifico di non essere nella condizione di
        // rette parallele
        //-------------------------------------------------------------------------------
        RetteParallele=false;
        if( NodoOccupabile!=0 ){
           VerificaRetteParallele( agv_num, &RetteParallele, PSucc[0], PSucc[1], NodoOccupabile, agv_adiacente, satellite_occupato, NodoInterruzionePercorso );
        }
        if( NodoOccupabile!=0 ){
           VerificaRetteParallele( agv_num, &RetteParallele, PSucc[2], PSucc[3], NodoOccupabile, agv_adiacente, satellite_occupato, NodoInterruzionePercorso );
        }
        if( NodoOccupabile==0 ){
           *agv_bloccante= (short int)(agv_adiacente);
           AgvBloccante  = (short int)(agv_adiacente);
        }
        //--------------------------------------------------------------
        // (G4) N.B. TOLTO da ciclo FOR di G3 ---> MI SEMBRA UN ERRORE
        //--------------------------------------------------------------
        if( SempreON && RioccupaNodo==false ){
           //---------------------------------------------------------------
           // (G4.01) 18/10/95  Se il percorso non è completamente libero
           //                   occupa comunque fino al primo nodo prenotato
           //                   da altro AGV
           //
           // Ritorna :
           //
           //    NodoOccupabile           = 1 -> OK
           //    agv_adiacente            = xx
           //    satellite_occupato       = yy
           //    NodoInterruzionePercorso = zz
           //---------------------------------------------------------------
           //------------------------------------------------------
           // AL-03/09/2018
           // !!! ZONE FUORI INGOMBRO AUTOMATICHE STANDARD !!!
           //------------------------------------------------------
           if(AbilitaVerificaZonaFuoriIngombro==true){
              if( percorso_libero==0 && satellite_occupato==0 && (RetteParallele==false || (RetteParallele==true && NodoOccupabile!=0))){
                 VerificaNodoPrenotato( agv_num, PSucc[0], NodoOccupabile, agv_adiacente, satellite_occupato, NodoInterruzionePercorso );
                 if( NodoOccupabile ){
                    VerificaNodoPrenotato( agv_num, PSucc[1], NodoOccupabile, agv_adiacente, satellite_occupato, NodoInterruzionePercorso );
                 }
                 //---------------------------------------------------------------
                 // CONTROLLO PUNTI PRENOTATI SPECIALE
                 // Se l'applicazione richiede una zona fuori ingombro molto
                 // ampia è necessario rinforzare il controllo anche sui nodi
                 // prenotati in caso la zona non esista.
                 // (MOLTO PROBABILMENTE SONO PRENSENTI MOLTI SALTI NODO)
                 //---------------------------------------------------------------
                 if(MinNodFuoriIngombro>3){
                    if( NodoOccupabile ){
                       VerificaNodoPrenotato( agv_num, PSucc[2], NodoOccupabile, agv_adiacente, satellite_occupato, NodoInterruzionePercorso );
                    }
                    if( NodoOccupabile ){
                       VerificaNodoPrenotato( agv_num, PSucc[3], NodoOccupabile, agv_adiacente, satellite_occupato, NodoInterruzionePercorso );
                    }
                 }
                 //---------------------------------------
                 // Controllo Punti prenotati standard
                 //---------------------------------------
                 else{
                    if( NodoOccupabile && (DSucc[2]<DIST_MIN_ROT_DOPPIA_MAX || AgvSuAngolo) ){
                       VerificaNodoPrenotato( agv_num, PSucc[2], NodoOccupabile, agv_adiacente, satellite_occupato, NodoInterruzionePercorso );
                       //---------------------------------------------------------------------
                       // Se il TGV adiacente ha già occupato anche altri punti di missione
                       // NodoOccupabile = 1;
                       //---------------------------------------------------------------------
                       if(NodoOccupabile==0 && DSucc[2]>=DIST_MIN_ROT_DOPPIA_MAX && agv_adiacente>0 && agv_adiacente<=MAXAGV){
                          //--------------------------------------------------
                          // Scorro la missione del TGV ADIACENTE e verifico
                          // se il punto successivo a quello considerato è
                          // occupato
                          //--------------------------------------------------
                          for( i=0; i<MAXPERCORSI; i++){
                             if(AGV[agv_adiacente]->mission.punto_prenotato[i]==0               ) break;
                             if(AGV[agv_adiacente]->mission.punto_prenotato[i]!=PSucc[2]        ) continue;
                             if(AGV[agv_adiacente]->mission.punto_prenotato[i+1]==0             ) continue;
                             if(N.nodo_busy[AGV[agv_adiacente]->mission.punto_prenotato[i+1]]!=agv_adiacente) continue;
                             NodoOccupabile=1;
                             satellite_occupato=0;
                             break;
                          }
                       }
                    }
                    //---------------------------------------------------------------------
                    // 23/09/2008
                    // Nel controllo del "PuntoSucessivoSucessSucess" è stato tolto
                    // la verifica se il TGV è in rotazione perchè ritenuta sbagliata in
                    // seguito a backup di ASCOT DOM.
                    //---------------------------------------------------------------------
                    if( NodoOccupabile && DSucc[3]<DIST_MIN_ROT_DOPPIA_MAX ){
                       VerificaNodoPrenotato( agv_num, PSucc[3], NodoOccupabile, agv_adiacente, satellite_occupato, NodoInterruzionePercorso );
                    }
                 }
              }
           }
           //------------------------------------------------------
           // AL-03/09/2018
           // !!! NO GESTIONE ZONE FUORI INGOMBRO AUTOMATICHE !!!
           //------------------------------------------------------
           else{
              if( percorso_libero==0 && satellite_occupato==0 && (RetteParallele==false || (RetteParallele==true && NodoOccupabile!=0))){
                 for(i=0; i<MAXPERCORSI; i++){
                    if(PSucc[i]<1        ) break;
                    if(PSucc[i]>TOTPUNTI ) break;
                    VerificaNodoPrenotato( agv_num, PSucc[i], NodoOccupabile, agv_adiacente, satellite_occupato, NodoInterruzionePercorso );
                    if(NodoOccupabile==0                                          ) break;
                    if(NodoOccupabile>1 && NodoOccupabile>=MIN_NODI_FUORI_INGOMBRO) break;  // AL-10/10/2018 Sospendo verifica nodi prenotati per esistenza FUORI INGOMBRO
                 }
              }
           }
           if(NodoOccupabile==0){
              //---------------------------------------------------------------
              // (G4.00) 5/01/2005 Spostata qui il controllo di una eventuale
              //                  "ZONA FUORI INGOMBRO"
              //
              // Ritorna :
              //
              //    EsisteZonaFuoriIngombro  = 1 -> OK
              //    NodoInterruzionePercorso = zz
              //---------------------------------------------------------------
              EsisteZonaFuoriIngombro=0;
              if(AbilitaVerificaZonaFuoriIngombro==true){
                 VerificaZonaFuoriIngombro(agv_num, PuntoConsiderato, NodoInterruzionePercorso, AgvPerInterruzionePercorso, EsisteZonaFuoriIngombro);
              }
              //---------------------------------------------------------------
              // AL-28/07/2014 - Gestione zone fuori ingombro manuali
              // Viene richiamata una funzione che gestisce una struttura
              // del tutto simile a quella per i blocchi manuali, ma in
              // questo caso se ritorna TRUE, gestisce le condizioni per
              // ritenere una zona FUORI INGOMBRO
              //---------------------------------------------------------------
              if( EsisteZonaFuoriIngombro==0 ){
                 if(agv_adiacente==0) agv_adiacente=AgvPerInterruzionePercorso;
                 zona_fuori_ingombro_manuale = N.verifica_zona_fuori_ingombro_manuale(agv_num, ptr_nodo, agv_adiacente);
                 if(zona_fuori_ingombro_manuale==true){
                    EsisteZonaFuoriIngombro = MIN_NODI_FUORI_INGOMBRO+1;
                    NodoOccupabile          = 1;
                    satellite_occupato      = 0;
                 }
              }
              //------------------------------------------------
              // Se non esiste nemmeno una zona fuori ing.
              // --> skip!
              //------------------------------------------------
              if( EsisteZonaFuoriIngombro==0 ){
                 *agv_bloccante=(short int)(agv_adiacente);
                 AgvBloccante  =(short int)(agv_adiacente);
              }
              else{
                 NodoOccupabile     = 1;
                 satellite_occupato = 0;
              }
           }
        }
        //--------------------------------------------------------------
        // (G5) 13/09/95 Se il nodo è quello iniziale (source) o quello
        //               finale (dest) allora occupa comunque
        //--------------------------------------------------------------
/*
        //---------------------------------------------------------
        // 20/06/2011 G.B. Evita il bypass se sono punti di attesa
        //---------------------------------------------------------
        NodoDiAttesa=false;
        if( SempreON ){
           //-------------------------------------
           // Controllo se punto fuori ingombro
           //-------------------------------------
           for(jj=0; jj<MAXNODIATTESA; jj++){
              if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][jj]<1        ) continue;
              if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][jj]>MAXPUNTI ) continue;
              if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][jj]!=ptr_nodo) continue;
              NodoDiAttesa=true;
              break;
           }
        }
        if( NodoDiAttesa==false ){
           if(ptr_nodo == source) satellite_occupato = 0;
           if(ptr_nodo == dest  ) satellite_occupato = 0;
        }
*/
        //-----------------------------------------------------------------
        // AL-06/02/2014 Il nodo iniziale (source) e il nodo finale (dest)
        //               vengono occupati comunque se non fanno parte di
        //               quelli "da rioccupare".
        //               Questa gestione ingloba anche i nodi di attesa
        //               perchè nodi di percorso, quindi è stata
        //               commentata la gestione di "20/06/2011 G.B"
        // AL-27/01/2015 Se la destinazione è un nodo di percorso devono
        //               valere i normali controlli legati alle precedenze
        //               verranno utilizzare eventualmente le
        //               zone_fuori_ing
        //-----------------------------------------------------------------
        if(RioccupaNodo==false){
           if(ptr_nodo == source) satellite_occupato = 0;
          //f(ptr_nodo == dest  ) satellite_occupato = 0;
           if(ptr_nodo == dest && N.punti_notevoli[dest]) satellite_occupato = 0;
        }
        //-----------------------------------------------------------------
        // RIOCCUPA NODO
        // Se il nodo non è occupabile verifico se si tratta di
        // una zona fuori ingombro manuale
        //-----------------------------------------------------------------
        else{
           if((ptr_nodo == source || ptr_nodo == dest) && satellite_occupato!=0){
              zona_fuori_ingombro_manuale = N.verifica_zona_fuori_ingombro_manuale(agv_num, ptr_nodo, 0);
              if(zona_fuori_ingombro_manuale==true) satellite_occupato = 0;
           }
        }
        //--------------------------------------------------------------
        // Se il percorso è libero Resetto per sicurezza il flag di
        // Agv Bloccante.
        //--------------------------------------------------------------
        if(satellite_occupato==0){
           *agv_bloccante = 0;
        }
        //--------------------------------------------------------------
        // (G6) 12/11/96 Se il nodo attuale, è nodo di destinazione, ha
        //      un nodo adiacente impostato come "nodo_in_ingombro"
        //      occupato da un altro AGV, interrompi..(nodi che non
        //      possono essere occupati da 2 AGVs contemporaneamente
        //      per ragioni di spazio).
        //--------------------------------------------------------------
        if( ptr_nodo==dest ){
           nodo_vicino = N.nodo_in_ingombro[ dest ];
           if( nodo_vicino>0 ){
              if( N.nodo_busy[ nodo_vicino ]>0 ){
                 if( N.nodo_busy[ nodo_vicino ]!=agv_num ){
                    satellite_occupato = 1;
                    *agv_bloccante =(short int)(N.nodo_busy[ nodo_vicino ]);
                    AgvBloccante   =(short int)(N.nodo_busy[ nodo_vicino ]);

                 }
              }
           }
        }

        //--------------------------------------------------------------
        // (G6a) 10/12/2004 ALBERDI
        //      Se il nodo attuale,  ha  un nodo adiacente
        //      impostato come "nodo_in_ingombro"
        //      un nodo adiacente impostato come "nodo_in_ingombro"
        //      occupato da un altro AGV, interrompi..(nodi che non
        //      possono essere occupati da 2 AGVs contemporaneamente
        //      per ragioni di spazio).
        //--------------------------------------------------------------
        //if( ptr_nodo==dest ){
        if( N.nodo_in_ingombro[ PSucc[0] ]>0 ){
           nodo_vicino = N.nodo_in_ingombro[ PSucc[0] ];
           if( nodo_vicino>0 ){
              if( N.nodo_busy[ nodo_vicino ]>0 ){
                 if( N.nodo_busy[ nodo_vicino ]!=agv_num ){
                    satellite_occupato = 1;
                    *agv_bloccante =(short int)(N.nodo_busy[ nodo_vicino ]);
                    AgvBloccante   =(short int)(N.nodo_busy[ nodo_vicino ]);
                 }
              }
           }
           nodo_vicino = N.nodo_in_ingombro[ PSucc[1] ];
           if( nodo_vicino>0 ){
              if( N.nodo_busy[ nodo_vicino ]>0 ){
                 if( N.nodo_busy[ nodo_vicino ]!=agv_num ){
                    satellite_occupato = 1;
                    *agv_bloccante =(short int)(N.nodo_busy[ nodo_vicino ]);
                    AgvBloccante   =(short int)(N.nodo_busy[ nodo_vicino ]);
                 }
              }
           }
           nodo_vicino = N.nodo_in_ingombro[ PSucc[2] ];
           if( nodo_vicino>0 ){
              if( N.nodo_busy[ nodo_vicino ]>0 ){
                 if( N.nodo_busy[ nodo_vicino ]!=agv_num ){
                    satellite_occupato = 1;
                    *agv_bloccante =(short int)(N.nodo_busy[ nodo_vicino ]);
                    AgvBloccante   =(short int)(N.nodo_busy[ nodo_vicino ]);
                 }
              }
           }
           nodo_vicino = N.nodo_in_ingombro[ PSucc[3] ];
           if( nodo_vicino>0 ){
              if( N.nodo_busy[ nodo_vicino ]>0 ){
                 if( N.nodo_busy[ nodo_vicino ]!=agv_num ){
                    satellite_occupato = 1;
                    *agv_bloccante =(short int)(N.nodo_busy[ nodo_vicino ]);
                    AgvBloccante   =(short int)(N.nodo_busy[ nodo_vicino ]);
                 }
              }
           }
        }


        //-------------------------------------------------------
        // (G7) Occupa il nodo
        //-------------------------------------------------------
        if( satellite_occupato == 0) {
           N.nodo_busy[ptr_nodo] = agv_num;
           *agv_bloccante =0;
           modificato     =1;
        }
     }


     //-------------------------------------------------------
     // Se il nodo attuale, o quello sucessivo oppure uno dei
     // nodi circostanti è occupato da altro AGV, interrompi.
     // perché occorrono almeno 2 nodi di distanza tra 2 AGVs
     // per evitare la collisione !!!
     //
     // AL-06/02/2014 Se si tratta di un nodo da rioccupare
     // e satellite_occupato!=0 --> skip! il TGV deve
     // riforzare il pint
     //-------------------------------------------------------
     if(N.nodo_busy[ptr_nodo]!=agv_num || (RioccupaNodo==true && satellite_occupato!=0)){
        //----------------------------------------------------------
        // Se non è stato occupato nessun nodo allora la condizione
        // è di blocco.
        //----------------------------------------------------------
        if(modificato==0 && N.nodo_busy[ptr_nodo]!=0 && N.nodo_busy[ptr_nodo]!=agv_num){   // 11/04/2018 GB Reintrodotto controllo su nodo_busy[] altrimenti il controllo fallisce (da ITALGRES)
           //----------------------------------------------
           // AL-15/06/2017 solo se AgvBloccante non è
           // già stato calcolato
           //----------------------------------------------
           if(AgvBloccante==0){
              *agv_bloccante = N.nodo_busy[ptr_nodo];
               AgvBloccante  = N.nodo_busy[ptr_nodo];
           }
        }
        //----------------------------------------------------------
        // Rioccupazione del nodo con PINT
        // Per sicurezza mi assicuro che il successivo non sia
        // già occupato da me stesso
        //----------------------------------------------------------
        if(RioccupaNodo==true && last_nodo!=ptr_nodo && N.nodo_busy[ptr_nodo]==agv_num && N.nodo_busy[PSucc[1]]!=agv_num && N.nodo_busy[PSucc[1]]!=0){
           last_nodo = ptr_nodo;
           //----------------------------------------------
           // AL-15/06/2017 solo se AgvBloccante non è
           // già stato calcolato
           //----------------------------------------------
           if(AgvBloccante==0){
              *agv_bloccante = N.nodo_busy[PSucc[1]];
              AgvBloccante   = N.nodo_busy[PSucc[1]];
           }
        }
        break;
     }
     //----------------------------------------------
     // Memorizzo l'ultimo nodo occupato
     //----------------------------------------------
     last_nodo    = ptr_nodo;
   //id_last_nodo = ptr_percorso;
     //----------------------------------------------
     // GESTIONE SALTO NODO
     // Se ho la certezza che il nodo a cui ho               
     // saltato possa essere occupato -> impegno
     // anche tutti i nodi saltati
     //----------------------------------------------
     if(salto_a_nodo!=0 && salto_a_nodo==last_nodo && satellite_occupato1==0){
        //------------------------------------------------------
        // Verifico se tutti i nodi fina a quello a cui saltare
        // sono occupabili e li occupo
        //------------------------------------------------------
        for(i=0; i<20; i++){
           if(nodi_da_saltare[i]==0) break;
           N.nodo_busy[nodi_da_saltare[i]]=agv_num;
           modificato=1;
        }
        //------------------------------------------------------
        // Incremento il contatore dei nodi occupati anche per
        // il numero dei nodi saltati
        //------------------------------------------------------
        nodi_occupati=(short int)(nodi_occupati+(i-1));
        //------------------------------------------------------
        // Decremento il puntatore ai percorsi per accertarmi
        // di verificare anche sul nodo a cui sono saltato se
        // c'è un ulteriore salto impostato
        //------------------------------------------------------
        ptr_percorso--;
     }
     //----------------------------------------------
     // Se il numero di percorsi gi… occupati supera
     // il numero MAX per una missione SKIP !!!
     //----------------------------------------------
     if( nodi_occupati >= nodi_da_occupare ) break;
     nodi_occupati++;
     //----------------------------------------------
     // Condizione di stop per punto intermedio
     //----------------------------------------------
     if(ptr_nodo==dest && mission.punto[ptr_percorso+1]==0     ) break; // Destinazione raggiunta
     if(ptr_nodo==mission.pintend && mission.StopManuale==true ) break; // Stop su pint MANUALE
     if(sotto_porta!=0  && ptr_nodo==sotto_porta               ) break; // Stop su Punto di APERTURA PORTA PERCORSO
     if(semaforo_porta!=0 && ptr_nodo==semaforo_porta          ) break; // Stop su Punto di SEMAFORO PORTA PERCORSO
     if(ing_macchina!=0 && ptr_nodo==ing_macchina              ) break; // Stop su Punto di RICHIESTA INGRESSO MACCHINA/BAIA
     if(salto_a_nodo!=0 && satellite_occupato1!=0              ) break; // Stop su Punto per salto NON FATTIBILE
     //----------------------------------------------
     // PUNTO INTERMEDIO FORZATO
     // Se il TGV ha un pint forzato dalla missione
     //----------------------------------------------
     if(ptr_nodo==mission.PIntForzato) break;
     //----------------------------------------------
     // PUNTO INTERMEDIO OBBLIGATO
     // (es. per rotazioni da fermo)
     //----------------------------------------------
     pint_obbligato=false;
     for(i=0; i<100; i++){
        //----------------------------------------------
        // Se il TGV è già sul nodo -- skip!
        //----------------------------------------------
        if(stato.pos==ptr_nodo) break;
        //----------------------------------------------
        // Fine pint obbligati
        //----------------------------------------------
        if(N.pint_obbligato[i].pintend==0) break;
        //----------------------------------------------
        // se ptr_nodo == pintobbligato
        //----------------------------------------------
        if(ptr_nodo!=N.pint_obbligato[i].pintend) continue;
        //----------------------------------------------
        // Controllo successivo e precedente
        //----------------------------------------------
        if(mission.punto[ptr_percorso+1]!=0){
           if(mission.punto[ptr_percorso+1]!=N.pint_obbligato[i].succ) continue;
        }
        if(ptr_percorso>0 && mission.punto[ptr_percorso-1]!=0){
           if(mission.punto[ptr_percorso-1]!=N.pint_obbligato[i].prec) continue;
        }
        //----------------------------------------------
        // PINT OBBLIGATO
        //----------------------------------------------
        pint_obbligato=true;
        break;
     }
     if(pint_obbligato) break;
     //----------------------------------------------
     // GESTIONE PRECEDENZE PER TGV IN RETTILINEO
     // Lascio la precedenza alle navette lanciate
     // in rettilineo
     //----------------------------------------------
     blocco_tgv_lanciato = N.blocco_per_tgv_lanciato_su_rettilineo(agv_num, ptr_nodo, &altro_agv);
     if(blocco_tgv_lanciato==true){
        if(altro_agv>0 && altro_agv<=MAXAGV) *agv_bloccante=altro_agv;
        break;
     }
     //----------------------------------------------
     // BLOCCHI MANUALI OCCUPAZIONE NODI
     // Se ho individuato un blocco manuale
     // nella gestione percorsi -> skip!
     //----------------------------------------------
     blocco_manuale = N.verifica_blocco_manuale(agv_num, ptr_nodo, &altro_agv);
     if(blocco_manuale==true){
        if(altro_agv>0 && altro_agv<=MAXAGV) *agv_bloccante=altro_agv;
        break;
     }
     //----------------------------------------------
     // PINT SUI NODI DI ROTAZIONE
     //----------------------------------------------
     if(AbilitaPuntoIntermedioSuRotaz==true && tutto==false && (stato.start!=0 || (stato.start==0 && N.nodo_minpercorsi[stato.pos]==0))){
        if(stato.pos!=ptr_nodo && ptr_percorso>0 && mission.punto[ptr_percorso+1]!=mission.pend){
           if(mission.dati_perc[ptr_percorso].rot<4 && mission.dati_perc[ptr_percorso-1].rot<4 && mission.dati_perc[ptr_percorso].rot!=mission.dati_perc[ptr_percorso-1].rot) break;
        }
     }
  }
  //----------------------------------------------
  // memorizzo il Pint raggiunto
  //----------------------------------------------
  pintend    = last_nodo;
//id_pintend = id_last_nodo;
  //-------------------------------------------------------------------------------
  // Aggiorna il punto intermedio di END solo se START = 0
  // In caso di MISSIONI_AL_VOLO aggiornare sempre punto intermedio di STOP
  //-------------------------------------------------------------------------------
  if(!mission.NoMissioniAlVolo){
     //------------------------------------------------------------------
     // Prima di assegnare il nuovo punto intermedio mi accerto che
     // sia sucessivo al precedente durante l'esecuzione della missione
     //------------------------------------------------------------------
     ptr_percorso=0;
     for(i=0; i<MAXPERCORSI; i++){
        if(mission.pintend==0               ) break;
        if(mission.punto[i]==0              ) break;
        if(mission.punto[i]!=mission.pintend) continue;
        ptr_percorso=i;
        break;
     }
     if(ptr_percorso!=0){
        for(i=ptr_percorso+1; i<MAXPERCORSI; i++){
           if(mission.punto[i]==0        ) break;
           if(mission.punto[i]!=pintend  ) continue;
           mission.pintend = pintend;
           break;
        }
     }
     else{
        mission.pintend = pintend;
     }
  }
  else{
     if(stato.start == 0){
        mission.pintend = pintend;
     }
  }
  //------------------------------------------------------------------
  // 14/02/97 S.Valentino inibito salvataggio su file
  //          dei nodi occupati in modo da accelerare l'applicazione
  // 11/05/97 Riabilitato
  //------------------------------------------------------------------
  err=0;
  N.nodo_busy[mission.pintend]=agv_num;           // SICUREZZA: occupo il nodo fissato come pintend  
  if(modificato) err = N.save_nodo_busy(all_mess);
  if(err) return 1;

  return 0;
}
// ------------------------------------------------------------------------
// ---------------------------------------------------
//      occupa_nodi_ingombro_agv()
// ---------------------------------------------------
// Occupa tutti i nodi dalla posizione attuale AGV fino alla destinazione
// (target) o sino al punto impegnato da un 'altro AGV. Restituisce in
// "num_nodo" l'ultimo nodo occupato. La funzione ritorna 1 in caso di
// errore.
//
int agv::occupa_nodi_ingombro_agv(char *all_mess, bool tutto, short int *agv_bloccante)
/*************************************************/
{
  bool RioccupaNodo;
  int ptr_percorso, modificato =0, err=0;
  int x1, y1, rot1, rot2;
  int i, j, k, z, indice;
  int satellite_occupato=0;
  int count_start_missione, count_da_interruzione;
  int count_da_porta, count_da_macc, count_nodi_porta;
  int percorso_libero;
  int numero_nodo;
  int agv_adiacente=0;
  int MinPercorsi = MINPERCORSI;
  short int ing_macchina;
  short int salto_a_nodo;
  short int nodi_da_saltare[21];
  short int AgvBloccante;
  short int AgvInIngombro;
  short int max_metri;
  short int punto_arrivo;
  short int altro_agv, metri;
  short int nodi_da_occupare, nodi_occupati;
  short int source, ptr_nodo, ptr_nodo1, dest, last_nodo;
  short int succ_sotto_porta, sotto_porta, rot_su_nodo_porta;
  short int semaforo_porta;
  short int pintend;
  short int num_baia, num_mac, num_stz, num_cbat;
  short int altro_agv_su_pt_in_ingombro;

  int Vicino,
      Dist, DistX, DistY,
      xv, yv, PuntoConsiderato;
  bool rot_not_ok;
  bool RetteParallele;
  bool tgv_si_incontrano;
  bool pintend_passato;
  bool pint_obbligato, gruppo;
  bool blocco_manuale, blocco_tgv_lanciato, zona_fuori_ingombro_manuale;
  bool porta_in_ok_accesso;
  bool macch_in_ok_accesso;
  bool no_attraverso_porta;
  bool no_stop_per_deviazione;
  bool altro_agv_fermo_su_fuori_ingombro;
  double CalcolaRad, CatetoX, CatetoY;
  int PrecInterruzionePercorso;
  int EsisteZonaFuoriIngombro, NodoInterruzionePercorso, AgvPerInterruzionePercorso;
  int satellite_occupato1, NodoOccupabile1, AgvPerInterruzionePercorso1, NodoInterruzionePercorso1;
  int UltimoNodoDiPercorsoPrimaDellaDest;

  int NodoOccupabile;
  int NodoConsiderato;
  int SempreON=1;

  int  IndicePercorsoIngOutCrvDev;                   // Nodo di percorso in ingresso o uscita curva
  int  DistIngombro, DistIngombroSucc;               // Distanza dal nodo adiacente Nodo ingombro
  int  NodoPercorsoIngombroPt;                       // Nodo di percorso in Ingombro da PT
  int  NodoPercorsoIngombroAnticipatoPt;             // Nodo di percorso in Ingombro anticipato da PT (rotazione doppia)
  int  NodoPercorsoIngombroAnticipatoCrvDev;         // Nodo di percorso in Ingombro anticipato per CURVA o DEVIAZ. (su primo tratto rettilineo disponibile)
  int  NodoPercorsoIngombroAnticipatoAltroAgvFermo;  // Nodo di percorso in Ingombro anticipato per altro TGV fermo senza missione
  int  NumTgvPerAnticipo;                            // Numero del TGV che ha causato l'anticipo nodo
  int  NodoIngombroPercorsoAnticipato;               // Nodo di ingombro percorso anticipato causa PTinIng, Curva o Deviaz.
  short int num_comp, num_corsia;

  rot_su_nodo_porta = 0;
  sotto_porta       = 0;
  succ_sotto_porta  = 0;
  semaforo_porta    = 0;
  punto_arrivo      = 0;
  metri             = 0;
  nodi_da_occupare  = 0;
  *agv_bloccante    = 0;
  RetteParallele    = false;
  source            = stato.pos;       // ** source = posizione attuale AGV
  dest              = mission.pend;    // ** dest   = target della missione AGV
  last_nodo         = source;
  pintend           = mission.pintend;   // AL-22/02 ultimo pintend;
  sprintf(all_mess, "Errore in occupa_nodi() per il TGV %d!", agv_num);
  if((source==0) || (dest==0)){strcpy(all_mess, ""); return 0;}
  if(test_position_into_path(all_mess)) return 1;

  //-------------------------------------------------------------
  // (A) Cerca punto del percorso dove iniziare a occupare nodi
  //-------------------------------------------------------------
  for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
     if(mission.punto[ptr_percorso] == source) break;
     if(mission.punto[ptr_percorso] == 0) break;
  }
  if(mission.punto[ptr_percorso] != source){
     sprintf(all_mess, "Errore nodo fuori percorso in occupa_nodi() per il TGV %d!", agv_num);
     return 1;
  }
  //-------------------------------------------------------------
  // (B) Compila array punto_prenotato[] con i nodi restanti
  //     dalla posizione attuale all'obbiettivo
  //-------------------------------------------------------------
  memset(&mission.punto_prenotato , 0, sizeof(mission.punto_prenotato));
  for(i=0; i<MAXPERCORSI; i++){
     mission.punto_prenotato[ i ] = mission.punto[ ptr_percorso+i ];
     if(mission.punto_prenotato[ i ] == 0) break;
  }
  //-----------------------------------------------------------------
  // Se i nodi sono già tutti occupati fino a destinazione --> Skip!
  //-----------------------------------------------------------------
  if(mission.pend==mission.pintend && stato.pos!=mission.pintend){
     //-----------------------------------------------------------
     // Verifico se il nodo precedente all'ultimo è già occupato
     // occupati --> skip
     //
     // AL-21/12/2020 solo se ultimo nodo occupato per gestione
     //               anello
     //-----------------------------------------------------------
   //if(i>1 && N.nodo_busy[mission.punto_prenotato[i-2]]==agv_num) return 0;
     if(i>0 && N.nodo_busy[mission.punto_prenotato[i-1]]==agv_num) return 0;
  }
  //-------------------------------------------------------------
  // Recupero indice posizione TGV
  //-------------------------------------------------------------
  indice=0;
  for(i=0; i<MAXPERCORSI; i++){
     if(mission.punto[i]==0     ) break;
     if(mission.punto[i]!=source) continue;
     indice=i;
     break;
  }

  //-------------------------------------------------------------
  // CONSENSI PORTA
  // - Verifico se il TGV è in passaggio sotto una porta
  //-------------------------------------------------------------
  count_da_porta=0;
  for(i=indice; i<MAXPERCORSI; i++){
     if(mission.punto[i]==0) break;
     //------------------------------------------
     // Se il TGV passa sotto la porta setto il
     // nodo intermedio
     //------------------------------------------
     for(j=1; j<=MAXPORTEIMP; j++){
        sotto_porta=0;
        //-----------------------------------
        // Porta esclusa da PLC
        // (attraversabile)
        //-----------------------------------
        if(PorteImp[j].EsclusionePLC==true) continue;
        //-----------------------------------
        // Ciclo per tutti i nodi
        //-----------------------------------
        for(k=0; k<11; k++){
           if(PorteImp[j].Punto[k]==0               ) continue;
           if(mission.punto[i]!=PorteImp[j].Punto[k]) continue;
           //-------------------------------------------------------
           // Recupero la rotazione che il TGV ha sul nodo in
           // ingombro alla porta
           //-------------------------------------------------------
           rot_su_nodo_porta = rot_standard(i);
           //-------------------------------------------------------
           // Memorizzo se il nodo successivo della missione è
           // ancora in ingombro alla porta
           //-------------------------------------------------------
           succ_sotto_porta=0;
           for(z=0; z<11; z++){
              if(PorteImp[j].Punto[z]==0                 ) continue;
              if(mission.punto[i+1]!=PorteImp[j].Punto[z]) continue;
              succ_sotto_porta = PorteImp[j].Punto[z];
              break;
           }
           //-------------------------------------------------------
           // 05/08/2011
           // Se non esiste il successivo sotto porta, mi
           // assicuro che la porta abbia più nodi assegnati
           //-------------------------------------------------------
           if(succ_sotto_porta==0){
              count_nodi_porta=0;
              for(z=0; z<11; z++){
                 if(PorteImp[j].Punto[z]==0) continue;
                 count_nodi_porta++;
              }
              if(count_nodi_porta<2) succ_sotto_porta=PorteImp[j].Punto[k];
           }
           //-------------------------------------------------------
           // Verifico se il TGV si move nella direzione impostata
           //-------------------------------------------------------
           rot_not_ok=true;
           if(mission.dati_perc[i].dir==1){
              if(rot_su_nodo_porta==0 && PorteImp[j].DirDown!=true ) rot_not_ok=false;
              if(rot_su_nodo_porta==1 && PorteImp[j].DirRight!=true) rot_not_ok=false;
              if(rot_su_nodo_porta==2 && PorteImp[j].DirUp!=true   ) rot_not_ok=false;
              if(rot_su_nodo_porta==3 && PorteImp[j].DirLeft!=true ) rot_not_ok=false;
           }
           if(mission.dati_perc[i].dir==0){
              if(rot_su_nodo_porta==0 && PorteImp[j].DirUp!=true   ) rot_not_ok=false;
              if(rot_su_nodo_porta==1 && PorteImp[j].DirLeft!=true ) rot_not_ok=false;
              if(rot_su_nodo_porta==2 && PorteImp[j].DirDown!=true ) rot_not_ok=false;
              if(rot_su_nodo_porta==3 && PorteImp[j].DirRight!=true) rot_not_ok=false;
           }
           //-------------------------------------------------------
           // La rotazione è ammessa ma il punto successivo è
           // di deviazione e non è ammesso il PInt su deviazione
           // abilito l'apertura con un nodo di anticipo
           //-------------------------------------------------------
           if(rot_not_ok==false){
              // DEVIAZIONE = ROT>49 && ROT<70
              if(NoPIntSuStartDeviazione==true){
                 if(mission.dati_perc[i+1].rot>49 && mission.dati_perc[i+1].rot<70) rot_not_ok=true;
              }
              // CURVA = ROT>3 && ROT<40
              if(NoPIntSuStartCurva==true){
                 if(mission.dati_perc[i+1].rot> 3 && mission.dati_perc[i+1].rot<40) rot_not_ok=true;
              }
           }
           //-------------------------------------------------------
           // Se la rotazione è ammessa non viene considerato sotto
           // porta
           //-------------------------------------------------------
           if(rot_not_ok==false) continue;
           //-------------------------------------------------------
           // Nodo porta trovato
           //-------------------------------------------------------
           sotto_porta=PorteImp[j].Punto[k];
           break;
        }
        //------------------------------------------------
        // PORTE CORRIDOIO
        // (FORZO IL PINT SOLO IN ATTRAVERSAMENTO)
        // Solo sulle porte di corridoio controllo che
        // anche il nodo successivo della missione sia
        // in ingombro alla porta altrimenti NON forzo
        // il punto intermedio
        //------------------------------------------------
        if(sotto_porta!=0 && succ_sotto_porta==0) sotto_porta=0;
        //------------------------------------------------
        // TGV GIA' IN TRANSITO SOTTO LA PORTA
        // Se ho individuato un nodo sotto porta nel
        // percorso verifico se è azzerabile
        //------------------------------------------------
        if(sotto_porta){
           //--------------------------------------------------------
           // Se la porta è in OK INGRESSO e il TGV è in prossimità
           // proseguo nell'occupazione dei nodi
           //--------------------------------------------------------
           porta_in_ok_accesso = PorteImp[j].OkIngresso;
           //----------------------------------------------------------
           // Verifico se il TGV ha il carrello a bordo e in quel
           // caso mi accerto che la porta non sia aperta parzialmente
           //----------------------------------------------------------
         //if((stato.s.bit.carA || stato.s.bit.carB) && PorteImp[j].AperturaParziale==true) porta_in_ok_accesso=false;
           //----------------------------------------------------------
           // GESTIONE PERCORSI ALTERNATIVI (con apertura parziale)
           // Utilizzo dell'apetura parziale per la gestione dei
           // percorsi alternativi sotto alle porte
           //
           // AL-25/01 (1)
           // Se il TGV ha già imboccato il percorso speciale e la
           // navetta Kraft si sposta in fuori ingombro mantengo il
           // percorso già intrapreso
           //----------------------------------------------------------
           if(porta_in_ok_accesso==true){
              if(PercorsiSpeciali==0 && PorteImp[j].AperturaParziale==true ) porta_in_ok_accesso=false;
            //if(PercorsiSpeciali==j && PorteImp[j].AperturaParziale==false) porta_in_ok_accesso=false;
           }
           //----------------------------------------------------------
           // Se la porta è in OK INGRESSO verifico se ho le condizioni
           // per procedere all'occupazione dei nodi
           //----------------------------------------------------------
           if(porta_in_ok_accesso==true){
              //-----------------------------------------------------
              // Proseguo l'occupazione del nodo
              //-----------------------------------------------------
              if(N.nodo_busy[sotto_porta]==agv_num && count_da_porta<=PorteImp[j].AnticipoRichiesta){
                 sotto_porta=0;
              }
              //-----------------------------------------------------
              // 25/02/2011
              // Se è già stato alzato l'ingombro alla porta non
              // viene mantenuto il punto intermedio
              //-----------------------------------------------------
              if(PorteImp[j].TgvInIngombro) sotto_porta=0;
           }
        }
        if(sotto_porta) break;
     }
     if(sotto_porta) break;
     //------------------------------------
     // 24/02/2011
     // Incremento conteggio nodi da porta
     //------------------------------------
     count_da_porta++;                                                          // conteggio nodi da pos tgv.
  }

  //-------------------------------------------------------------
  // SEMAFORI PORTA
  // - Verifico se il TGV incontra un punto di semaforo
  //   di una porta bloccata
  //-------------------------------------------------------------
  for(i=0; i<MAXPERCORSI; i++){
     if(mission.punto_prenotato[ i ]==0) break;
     //----------------------------------------
     // controllo tutte le porte dell'impianto
     // e considero solo quelle che il TGV
     // deve attraversare
     //----------------------------------------
     for(j=1; j<=MAXPORTEIMP; j++){
        //---------------------------------------
        // Scarto tutte le porte non bloccate
        //---------------------------------------
        if(PorteImp[j].PortaInBlocco==false) continue;
        //-----------------------------------
        // Porta esclusa da PLC
        // (attraversabile)
        //-----------------------------------
        if(PorteImp[j].EsclusionePLC==true ) continue;
        //---------------------------------------
        // Scarto tutte le porte che non devo
        // attraversare.
        //---------------------------------------
        no_attraverso_porta=true;
        for(k=0; k<11; k++){
           if(PorteImp[j].Punto[k]==0                          ) continue;
           if(PorteImp[j].Punto[k]!=mission.punto_prenotato[i] ) continue;
           //---------------------------------------------------
           // Per avere la condizione di attraversamento
           // almeno 2 nodi devono essere nodi in ingombro
           //---------------------------------------------------
           succ_sotto_porta=0;
           for(z=0; z<11; z++){
              if(PorteImp[j].Punto[z]==0                            ) continue;
              if(PorteImp[j].Punto[z]!=mission.punto_prenotato[i+1] ) continue;
              succ_sotto_porta=PorteImp[j].Punto[z];
              break;
           }
           if(succ_sotto_porta==0) continue;
           //---------------------------------------------------
           // TGV IN ATTRAVERSAMENTO
           //---------------------------------------------------
           no_attraverso_porta=false;
           break;
        }
        if(no_attraverso_porta==true) continue;
        //-------------------------------------------
        // Memorizzo il primo nodo di semaforo che
        // incontro lungo il percorso
        //-------------------------------------------
        semaforo_porta=0;
        for(z=0; z<MAXPERCORSI; z++){
           if(mission.punto_prenotato[ z ]==0) break;
           //------------------------------
           // Cerco il primo nodo
           //------------------------------
           for(k=0; k<11; k++){
              if(PorteImp[j].PuntoSemaforo[k]==0                          ) continue;
              if(PorteImp[j].PuntoSemaforo[k]!=mission.punto_prenotato[z] ) continue;
              semaforo_porta = PorteImp[j].PuntoSemaforo[k];
              break;
           }
           if(semaforo_porta!=0) break;
        }
        //-------------------------------------------
        // semaforo trovato --> skip!
        //-------------------------------------------
        if(semaforo_porta!=0) break;
     }
     if(semaforo_porta!=0) break;
  }
  //-------------------------------------------------------------
  // CONSENSI MACCHINA/BAIA
  // Se la destinazione missione è una macchina o una baia
  // con comunicazione attiva (e quindi scambio consensi)
  // è necessario forzare il PINT sul nodo precedente
  //-------------------------------------------------------------
  ing_macchina=0;
  if(AbilitaPIntPerConsensiMacchina==true && stato.pos!=mission.pend){
     num_baia=num_mac=num_stz=num_cbat=0;
     //-------------------------------------------------------------------------------
     // ACCESSO a BAIE/MACCHINE
     //-------------------------------------------------------------------------------
     P.test_punto_presente_baia(all_mess, mission.pend, &num_baia);
     if(num_baia==0) P.test_punto_presente_mac(all_mess, mission.pend, &num_mac, &num_stz);
     if(num_baia==0 && num_mac==0) P.test_punto_presente_cbat(all_mess, mission.pend, &num_cbat);
     if((num_baia>0 && num_baia<=TOTBAIE    && P.ba[num_baia]->EsclComunic == false) ||
        (num_mac >0 && num_mac<=TOTCHIAMATE && P.ch[num_mac]->EsclComunic  == false) ||
        (num_cbat>0 && num_cbat<=TOTBATT    && P.cb[num_cbat]->EsclComunic == false)){
        count_da_macc=0;
        macch_in_ok_accesso = false;
        if(num_baia>0 && num_baia<=TOTBAIE    && P.ba[num_baia]->OkIng==true) macch_in_ok_accesso=true;
        if(num_cbat>0 && num_cbat<=TOTBATT    && P.cb[num_cbat]->OkIng==true) macch_in_ok_accesso=true;
        if(num_mac >0 && num_mac<=TOTCHIAMATE && (P.ch[num_mac]->OkIngMac==true || P.ch[num_mac]->Staz[num_stz].OkIngresso==true)) macch_in_ok_accesso=true;
        for(i=indice; i<MAXPERCORSI; i++){
           if(mission.punto[i]==0) break;
           //----------------------------------------------
           // Raggiune la destinazione --> skip!
           //----------------------------------------------
           if(mission.punto[i]==mission.pend && mission.punto[i+1]==0) break;
           //------------------------------------------
           // SUCCESSIVO NODO MACCHINA
           //------------------------------------------
           if(mission.punto[i+1]==mission.pend){
              ing_macchina=mission.punto[i];
              //--------------------------------------------------------
              // Se la macchina/baia è in ok ingresso e il TGV
              // ha già occupato il nodo precedente ed è a un distanza
              // <=ANTICIPO_RICH_INGRESSO_MAC non fermo il TGV
              //--------------------------------------------------------
              if(macch_in_ok_accesso==true && N.nodo_busy[ing_macchina]==agv_num && count_da_macc<=ANTICIPO_RICH_INGRESSO_MAC){
                 ing_macchina=0;
              }
           }
           if(ing_macchina!=0) break;
           //------------------------------------
           // Incremento conteggio nodi da porta
           //------------------------------------
           count_da_macc++;
        }
     }
  }
  //-------------------------------------------------------------
  // INGOMBRO PORTA
  // Se il TGV è già su un nodo di ingombro alla porta e non ha
  // l'OkAccesso non può occupare altri nodi
  //-------------------------------------------------------------
  if(sotto_porta!=0 && mission.pintend==sotto_porta) return 0;
  //-------------------------------------------------------------
  // SEMAFORO PORTA
  // Se il TGV è già su un nodo di ingombro alla porta e non ha
  // l'OkAccesso non può occupare altri nodi
  //-------------------------------------------------------------
  if(semaforo_porta!=0 && mission.pintend==semaforo_porta) return 0;
  //-------------------------------------------------------------
  // RICHIESTA INGRESSO PORTA
  // Se il TGV è su un nodo precedente a una macchina forzo
  // il Pint per la richiesta ingresso
  //-------------------------------------------------------------
  if(ing_macchina!=0 && mission.pintend==ing_macchina) return 0;
  //-------------------------------------------------------------
  // (B2) Non cambio la situazione dei nodi occupati per i TGV
  //      che hanno la missione che attraversa il nodo sul quale
  //      si trova un altro TGV IN RESET MISSIONE.
  //
  // AL-22/05/2015: Solo nel caso il TGV considerato sia in
  // missione in corso e senza missione ma su un pt_in_ingombro
  //-------------------------------------------------------------
  if(CicliAttesaPerAgvInReset>0){
     AgvInIngombro=0;
     if(stato.start==1 || (stato.start==0 && N.punti_notevoli[stato.pos]==true && N.pt_in_ingombro[stato.pos]==false)){
        for(i=1; i<=MAXAGV; i++){
           if(AgvInIngombro!=0) break;
           //-------------------------------------------------------------
           // Verifico se c'è una navetta in RESET
           //-------------------------------------------------------------
           if(i==agv_num                          ) continue;
           if(AGV[i]->test_mission_on()           ) continue;
           if(AGV[i]->mission.SemiAutomatico==true) continue;
           if(AGV[i]->mission.Esclusione==true    ) continue;
           if(AGV[i]->stato.s.bit.funz==false     ) continue;
           if(AGV[i]->stato.s.bit.allarme==true   ) continue;
           if(AGV[i]->allarme_interno!=0          ) continue;  // AL-FPT 22/09/2014 Anche l'allarme interno è da considerarsi allarme che non fa muovere il TGV
           if(AGV[i]->mission.TimeInitAttesa!=0   ) continue;  // AL-FPT 26/09/2014 Il TGV in attesa voluta, non è da utilizzare
           //-------------------------------------------------------------
           // TGV SU PUNTO TERMINALE
           //-------------------------------------------------------------
           if(N.punti_notevoli[AGV[i]->stato.pos]==true) continue;  // AL-IVC 18/04/2017 scarto i TGV sui punti TERMINALI
           //-------------------------------------------------------------
           // Verifico se altro AGV su nodo_fuori_ingombro, allora
           // non è ritenuto in cambio missione
           //-------------------------------------------------------------
           altro_agv_fermo_su_fuori_ingombro=false;
           for(gruppo=GRUPPO_FUORI_INGOMBRO; gruppo<=MAXGRUPPIIMP; gruppo++){
              for(j=0; j<MAXNODIATTESA; j++){
                 if(N.nodi_attesa[gruppo][j]<1                 ) continue;
                 if(N.nodi_attesa[gruppo][j]>TOTPUNTI          ) continue;
                 if(N.nodi_attesa[gruppo][j]!=AGV[i]->stato.pos) continue;
                 altro_agv_fermo_su_fuori_ingombro=true;
                 break;
              }
              if(altro_agv_fermo_su_fuori_ingombro==true) break;
           }
           if(altro_agv_fermo_su_fuori_ingombro==true) continue;
           //-------------------------------------------------------------
           // (1) Verifico se la missione del TGV attraversa la posizione
           //     di una navetta in RESET MISSIONE
           //-------------------------------------------------------------
           for(j=0; j<MAXPERCORSI; j++){
              if(AgvInIngombro!=0           ) break;
              if(!mission.punto_prenotato[j]) break;
              if(AGV[i]->stato.pos!=mission.punto_prenotato[j]) continue;
              //---------------------------------------------
              // TGV in inbombro
              //---------------------------------------------
              AgvInIngombro=i;
              break;
           }
           //-------------------------------------------------------------
           // (2) Verifico se la missione del TGV affinca la posizione
           //     di una navetta in RESET MISSIONE
           //-------------------------------------------------------------
           for(j=0; j<MAXPERCORSI; j++){
              if(AgvInIngombro!=0           ) break;
              if(!mission.punto_prenotato[j]) break;
              ptr_nodo = mission.punto_prenotato[j];
              x1 = Punti[ ptr_nodo ][0];
              y1 = Punti[ ptr_nodo ][1];
              for(Vicino=10; Vicino<=TOTPUNTI; Vicino++){
                 if(ptr_nodo == Vicino              ) continue;
                 if(AGV[i]->stato.pos!=Vicino       ) continue;
                 if(N.punti_notevoli[ Vicino ]==true) continue;
                 if(N.n[ Vicino ]->num ==0          ) continue;
                 //---------------------------------------------
                 // Estrai le coordinate del nodo VICINO e se
                 // DISTANZA > distanza minima --> CONTINUA
                 //---------------------------------------------
                 xv = Punti[ Vicino ][0];
                 yv = Punti[ Vicino ][1];
                 CatetoX = (double) abs(x1-xv);
                 CatetoY = (double) abs(y1-yv);
                 if(CatetoX>30000 || CatetoY>30000) continue;
                 CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
                 Dist = (int) sqrt_algo( CalcolaRad );
                 if( Dist> DIST_MIN_ROT_DOPPIA_MAX ) continue;
                 //---------------------------------------------
                 // Verifico se il ha almeno 2 nodi liberi
                 // occupabili di fronte a se.
                 //---------------------------------------------
                 if(j<MAXPERCORSI-2){
                    if((!N.nodo_busy[mission.punto_prenotato[j+1]] || N.nodo_busy[mission.punto_prenotato[j+1]]==agv_num) &&
                       (!N.nodo_busy[mission.punto_prenotato[j+2]] || N.nodo_busy[mission.punto_prenotato[j+2]]==agv_num)) continue;
                 }
                 //---------------------------------------------
                 // TGV in inbombro
                 //---------------------------------------------
                 AgvInIngombro=i;
                 break;
              }
           }
        }
     }
     CicliAttesaPerAgvInReset--;
     if(AgvInIngombro>0 && AgvInIngombro<=MAXAGV){
        *agv_bloccante = AgvInIngombro;
        return 0;
     }
  }
  CicliAttesaPerAgvInReset = CICLI_ATTESA_PER_AGV_IN_RESET;
  //----------------------------------------------------
  // (C) Calcola il numero massimo di nodi da occupare
  //----------------------------------------------------
  max_metri = MaxMetriOccupabili;
  if(tutto==true) max_metri=999;  // occupo 999metri = tutto il percorso
  if(occupa_metri(source, dest, max_metri, &punto_arrivo, &nodi_da_occupare, &metri, all_mess)) return 1;

  //----------------------------------------------------
  // (C1)
  // Se il punto di arrivo coincide con un punto
  // notevole mi accerto di occupare solo fino al
  // precedente
  //----------------------------------------------------
  if(stato.start!=0 && punto_arrivo!=0 && N.punti_notevoli[punto_arrivo]==true && N.nodo_in_ingombro[punto_arrivo]!=0){
     nodi_da_occupare--;
  }
  //---------------------------------------------------------------------------------------
  // (C1-A) 02/12/2015 GB
  // Se il punto di arrivo coincide con il punto di partenza
  // e posizione AGV (con nodi occupabili>0) mi accerto di occupare solo fino al precedente
  // N.B. Risolve alcuni casi particolari come la ripartenza da magazzini multinodo..
  //---------------------------------------------------------------------------------------
  if(stato.start!=0 && punto_arrivo!=0 && stato.pos==punto_arrivo && nodi_da_occupare>1){
     nodi_da_occupare--;
  }
  //------------------------------------------------------------------
  // (D) 11/12/98 La funzione "occupa_nodi()" viene richiamata
  // solo se START==0 (l'AGV ha completato la mssione parz. o tot.)
  //------------------------------------------------------------------
  if(mission.NoMissioniAlVolo==true){
     if(stato.start != 0) return 0;
  }

  //----------------------------------------------------------
  // (E) Verifica se il percorso è libero (nessun nodo occupato)
  //     sino alla destinazione
  //
  // Ritorna :
  //    percorso_libero
  //    satellite_occupato
  //    NodoInterruzionePercorso
  //
  //----------------------------------------------------------
  NodoConsiderato=0;
  count_start_missione=0;
  PrecInterruzionePercorso=0;
  NodoInterruzionePercorso=0;
  AgvPerInterruzionePercorso=0;
  UltimoNodoDiPercorsoPrimaDellaDest=0;
  if( SempreON ){
     percorso_libero    = 0;
     agv_adiacente      = 0;
     satellite_occupato = 0;
     for( i=ptr_percorso; i<MAXPERCORSI; i++){
        NodoConsiderato = mission.punto[ i ];
        if( NodoConsiderato == 0    ) break;
        VerificaNodoOccupatoIngombroAgv(FALSE, agv_num, NodoConsiderato, NodoOccupabile, AgvPerInterruzionePercorso, satellite_occupato, NodoInterruzionePercorso, 0, 0);
        //-------------------------------------
        // Memorizzo l'ultimo nodo occupabile
        // prima dell'interruzione
        //-------------------------------------
        if(i>0 && NodoInterruzionePercorso!=0) PrecInterruzionePercorso = mission.punto[ i-1 ];
        //-------------------------------------
        // AL-05/11/2019
        // Raggiunti i metri massimi mi fermo
        //-------------------------------------
        if(NodoConsiderato==punto_arrivo) break;
        //-------------------------------------
        // AL-15/06/2020 count nodi start
        // missione
        //-------------------------------------
        if(stato.start==0 && NodoConsiderato!=stato.pos) count_start_missione++;
        //-------------------------------------
        // Stop a dastinazione o a nodo non
        // occupabile
        //-------------------------------------
        if( NodoOccupabile  == 0                                 ) break;
        if( NodoConsiderato == dest && mission.punto[ i+1 ]==0   ) break;
     }
     if(  NodoConsiderato==dest && NodoOccupabile==1 ) percorso_libero=1;
  }
  //----------------------------------------------------------------------
  // GESTIONE OCCUPAZIONE NODO CON "INGOMBRO AGV"
  //
  // (1) AL-12/02/2020  Se non trovato nessun AgvPerInteruzione controllo
  //                    specifico per TGV che occupano PT in ingombro
  //
  // (2) AL-15/06/2020  Se start missione e trovato AgvPerInteruzione con
  //                    NodoInterruzione sotto a minpercorsi start non
  //                    proseguo nemmeno con l'occupazione nodi
  //----------------------------------------------------------------------
  // (1)
  if(AgvPerInterruzionePercorso==0){
     for(i=1; i<=MAXAGV; i++){
        if(i==agv_num                                ) continue;
        if(AGV[i]->stato.pos<1                       ) continue;
        if(AGV[i]->stato.pos>TOTPUNTI                ) continue;
        if(N.punti_notevoli[AGV[i]->stato.pos]==false) continue;
        if(N.pt_in_ingombro[AGV[i]->stato.pos]==false) continue;
        //-----------------------------------------------
        // Memorizzo pt_in_ingombro
        //-----------------------------------------------
        numero_nodo = AGV[i]->stato.pos;
        if(N.n[numero_nodo]->NodoMaster>0 && N.n[numero_nodo]->NodoMaster<=TOTPUNTI){
           if(N.punti_notevoli[N.n[numero_nodo]->NodoMaster]==true && N.pt_in_ingombro[N.n[numero_nodo]->NodoMaster]==true){
              numero_nodo = N.n[numero_nodo]->NodoMaster;
           }
        }
        //-----------------------------------------------
        // coordinate pt_in_ingombrto
        //-----------------------------------------------
        x1 = Punti[numero_nodo][0];
        y1 = Punti[numero_nodo][1];
        //-----------------------------------------------
        // solo dal punto di arrivo, gli altri sono
        // già stati analizzati
        //-----------------------------------------------
        k=MAXPERCORSI+1;
        for( j=0; j<MAXPERCORSI; j++){
           if(mission.punto_prenotato[j]<1            ) break;
           if(mission.punto_prenotato[j]>TOTPUNTI     ) break;
           if(mission.punto_prenotato[j]!=punto_arrivo) continue;
           k=j;
           break;
        }
        //-----------------------------------------------
        // Scorro i nodi ancora prenotati del TGV
        // considerato
        //-----------------------------------------------
        for( j=k+1; j<MAXPERCORSI; j++){
           if(mission.punto_prenotato[j]<1        ) break;
           if(mission.punto_prenotato[j]>TOTPUNTI ) break;
           //---------------------------------------------
           // Estrai le coordinate del nodo VICINO e se
           // DISTANZA > distanza minima --> CONTINUA
           //---------------------------------------------
           xv = Punti[mission.punto_prenotato[j]][0];
           yv = Punti[mission.punto_prenotato[j]][1];
           CatetoX = (double) abs(x1-xv);
           CatetoY = (double) abs(y1-yv);
           if(CatetoX>30000 || CatetoY>30000) continue;
           CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
           Dist = (int) sqrt_algo( CalcolaRad );
           if( Dist> DIST_MIN_ROT_DOPPIA_MAX ) continue;
           //-------------------------------------
           // Memorizzo l'ultimo nodo occupabile
           // prima dell'interruzione
           //-------------------------------------
           satellite_occupato=numero_nodo;
           AgvPerInterruzionePercorso=i;
           NodoInterruzionePercorso=mission.punto_prenotato[j];
           if(j>0) PrecInterruzionePercorso = mission.punto_prenotato[j-1];
           break;
        }
        if(AgvPerInterruzionePercorso!=0) break;
     }
  }
  // (2)
  if(AgvPerInterruzionePercorso!=0 && NodoInterruzionePercorso!=0 && stato.start==0){
     if(N.nodo_minpercorsi[stato.pos]!=99) MinPercorsi=N.nodo_minpercorsi[stato.pos];
     if(count_start_missione<MinPercorsi ){*agv_bloccante=AgvPerInterruzionePercorso; return 0;}
  }

  //----------------------------------------------------------------------
  // Ultimo nodo di percorso
  //----------------------------------------------------------------------
  for( i=MAXPERCORSI-1; i>ptr_percorso; i--){
     if(mission.punto[ i ]<1                      ) continue;
     if(N.punti_notevoli[mission.punto[ i ]]==true) continue;
     UltimoNodoDiPercorsoPrimaDellaDest=mission.punto[ i ];
     break;
  }
  //----------------------------------------------------------------------
  // (E1) 02/01/2013 Controllo se percorso interrotto per PT in ingombro
  //      05/02/2013 Evita che il PINT capiti a inizio/fine curva/deviaz.
  //      06/02/2013 Fermata a DIST_ROT_MAX se AgvPerInterruzionePercorso
  //                 è senza missione in corso
  // Ritorna :
  //
  //----------------------------------------------------------------------
  if( SempreON ){
     //---------------------------------------------------------------------------------
     // -Il percorso si può interrompere per una rot. anche sucessiva al PT
     // -Memorizza il nodo di percorso Adiacente al PT (Nodo di uscita da PT)
     // -Verifica se appartiene al percorso e interrompo in anticipo
     //
     // GESTIONE CURVE/DEVIAZIONI
     // -Verifica se il nodo di interruzione è in ing/out curva
     // -Interrompo in anticipo su primo nodo in rettilineo
     //
     // ALTRO TGV SENZA MISSIONE IN CORSO
     // -Se l'altro TGV è fermo senza missione in corso lungo il percorso devo
     //  fermarmi a una DIST_MIN_ROT_DOPPIA_MAX per garantire che questo possa
     //  ripartire in tutte le direzioni
     //---------------------------------------------------------------------------------
     DistIngombro=0;
     NumTgvPerAnticipo=0;                            // Numero del TGV che ha causato l'anticipo del Punto intermedio.
     NodoIngombroPercorsoAnticipato=0;               // Nodo di percorso anticipato causa PTinIng, Curva o Deviaz.
     NodoPercorsoIngombroPt=0;
     IndicePercorsoIngOutCrvDev=0;
     NodoPercorsoIngombroAnticipatoPt=0;
     NodoPercorsoIngombroAnticipatoCrvDev=0;         // Nodo di percorso in Ingombro anticipato per CURVA o DEVIAZ. (su primo tratto rettilineo disponibile)
     NodoPercorsoIngombroAnticipatoAltroAgvFermo=0;  // Nodo di percorso in Ingombro anticipato per altro TGV fermo senza missione
     if( satellite_occupato>0 && satellite_occupato< TOTPUNTI ){
        //-------------------------------------------------------
        // Priorità AGV INTERRUZIONE su PT_IN_INGOMBRO
        //-------------------------------------------------------
        if(NodoInterruzionePercorso>0 && NodoInterruzionePercorso<=TOTPUNTI){
           for( i=1; i<=MAXAGV; i++ ){
              if(i==AgvPerInterruzionePercorso       ) continue;
              if(AGV[i]->stato.pos<1                 ) continue;
              if(AGV[i]->stato.pos>TOTPUNTI          ) continue;
              if(!N.punti_notevoli[AGV[i]->stato.pos]) continue;
              if(!N.pt_in_ingombro[AGV[i]->stato.pos]) continue;
              //-------------------------------------------------------
              // Calcolo la distanza tra punto successivo al TGV su
              // pt_in_ingombro e nodo interruzione agv
              //-------------------------------------------------------
              CalcolaRad=0;
              DistX=Dist=0;
              N.VerificaDistanzaPunti(N.n[AGV[i]->stato.pos]->rec[0].ps, NodoInterruzionePercorso, &DistX, &DistY, &CalcolaRad);
              if(CalcolaRad>=DIST_MIN_ROT_DOPPIA_MAX) continue;
              satellite_occupato = AGV[i]->stato.pos;
              AgvPerInterruzionePercorso = i;
              break;
           }
        }
        //-------------------------------------------------------
        // Il satellite occupato è un PT in ingombro
        //-------------------------------------------------------
        if(N.punti_notevoli[ satellite_occupato ]){
           if( N.pt_in_ingombro[ satellite_occupato ]==true){
            //NodoPercorsoIngombroPt = N.n[ satellite_occupato ]->rec[0].ps;
              if(AGV[AgvPerInterruzionePercorso]->stato.pos>0 && AGV[AgvPerInterruzionePercorso]->stato.pos<=TOTPUNTI){
                 //----------------------------------------------------
                 // AL-16/01/2014 PROBLEMA SALTO NODI
                 // Se il TGV adiacente è già sul satellite_occupato
                 // lo setto come NodoPercorsoIngombroPt come primo
                 // dei possibili nodi di uscita in caso contrario
                 // è la ricerca sotto che deve determinare il
                 // "NodoPercorsoIngombroPt"
                 //----------------------------------------------------
                 if(AGV[AgvPerInterruzionePercorso]->stato.pos==satellite_occupato){
                    NodoPercorsoIngombroPt = N.n[ satellite_occupato ]->rec[0].ps;
                    //---------------------------------------------------------
                    // AL-21/03/2019 ATTESA DA PERCORSO ANCHE PER MULTI-NODO
                    // Se il satellite_occupato ho un nodo master, il
                    // punto successivo di percorso viene recuparato dal nodo
                    // master
                    //---------------------------------------------------------
                    if(N.n[ satellite_occupato ]->NodoMaster>0 && N.n[ satellite_occupato ]->NodoMaster<=TOTPUNTI){
                       NodoPercorsoIngombroPt = N.n[ N.n[ satellite_occupato ]->NodoMaster ]->rec[0].ps;
                    }
                    //---------------------------------------------------------
                    // Il nuovo satellite occupato a cui fare riferimento per
                    // la distanza diventerà il nodo di uscita
                    //---------------------------------------------------------
                    satellite_occupato = NodoPercorsoIngombroPt;
                 }
                 //------------------------------------------------------
                 // AL-21/01/2014 Gestione NODI MASTER IN INGOMBRO
                 // Se il satellite occupato è Nodo Master della pos del
                 // TGV bloccante, è da considerarsi cmq pt_in_ingombro
                 //------------------------------------------------------
                 if(N.n[AGV[AgvPerInterruzionePercorso]->stato.pos]->NodoMaster==satellite_occupato){
                    NodoPercorsoIngombroPt = N.n[ satellite_occupato ]->rec[0].ps;
                    //---------------------------------------------------------
                    // Il nuovo satellite occupato a cui fare riferimento per
                    // la distanza diventerà il nodo di uscita
                    //---------------------------------------------------------
                    satellite_occupato = NodoPercorsoIngombroPt;
                 }
              }
           }
        }
        //-------------------------------------------------------
        // 22/01/2013 anche se il satellite occupato è precedente
        // a un PT in ingobro devo memorizzarlo per garantire una
        // fermata anticipata che permetta poi alla navetta di
        // uscire
        //-------------------------------------------------------
        if(NodoPercorsoIngombroPt==0 && N.pt_in_ingombro[ AGV[AgvPerInterruzionePercorso]->mission.pend ]==true){
           for( i=MAXPERCORSI-1; i>=0;  i--){
              if( AGV[AgvPerInterruzionePercorso]->mission.punto[ i ]==0 ) continue;
              //------------------------------------------------------------
              // Satellite occupato precedente a un PT in ingobro
              //------------------------------------------------------------
              if(AGV[AgvPerInterruzionePercorso]->mission.punto[ i ]==AGV[AgvPerInterruzionePercorso]->mission.pend){
               //if(satellite_occupato==AGV[AgvPerInterruzionePercorso]->mission.punto[ i-1 ]){
               //   NodoPercorsoIngombroPt = satellite_occupato;
               //}
                 //-----------------------------------------------------------------
                 // AL-16/01/2014 PROBLEMA SALTO NODI
                 // In ogni caso il satellite occupato e il NodoPercorsoIngombroPt
                 // devono essere il nodo precedente alla destinazione per avere
                 // garanzia di uscita
                 //-----------------------------------------------------------------
                 if(satellite_occupato==AGV[AgvPerInterruzionePercorso]->mission.punto[ i-1 ] || satellite_occupato==AGV[AgvPerInterruzionePercorso]->mission.punto[ i ]){
                    NodoPercorsoIngombroPt = AGV[AgvPerInterruzionePercorso]->mission.punto[i-1];
                    //---------------------------------------------------------
                    // Il nuovo satellite occupato a cui fare riferimento per
                    // la distanza diventerà il nodo di uscita
                    //---------------------------------------------------------
                    satellite_occupato = NodoPercorsoIngombroPt;
                 }
              }
              break;
           }
           //-------------------------------------------------------
           // AL-21/01/2014 Gestione NODI MASTER IN INGOMBRO
           // Controllo nodo precedente anticipato a Nodo Master
           //-------------------------------------------------------
           if(N.n[AGV[AgvPerInterruzionePercorso]->mission.pend]->NodoMaster!=0){
              for( i=MAXPERCORSI-1; i>=0;  i--){
                 if( AGV[AgvPerInterruzionePercorso]->mission.punto[ i ]==0 ) continue;
                 //------------------------------------------------------------
                 // Satellite occupato precedente a un PT in ingobro
                 //------------------------------------------------------------
                 if(AGV[AgvPerInterruzionePercorso]->mission.punto[ i ]==N.n[AGV[AgvPerInterruzionePercorso]->mission.pend]->NodoMaster){
                    if(satellite_occupato==AGV[AgvPerInterruzionePercorso]->mission.punto[ i-1 ] || satellite_occupato==AGV[AgvPerInterruzionePercorso]->mission.punto[ i ]){
                       NodoPercorsoIngombroPt = AGV[AgvPerInterruzionePercorso]->mission.punto[i-1];
                       //---------------------------------------------------------
                       // Il nuovo satellite occupato a cui fare riferimento per
                       // la distanza diventerà il nodo di uscita
                       //---------------------------------------------------------
                       satellite_occupato = NodoPercorsoIngombroPt;
                    }
                    break;
                 }
              }
           }
        }
        //-------------------------------------------------------
        // 05/02/2013 SATELLITE OCCUPATO IN ING/OUT CURVA
        // Verifico che il nodo di interruzione percorso
        // non sia in ingresso o uscita curva.
        //-------------------------------------------------------
        if(NodoInterruzionePercorso>0 && NodoInterruzionePercorso<=TOTPUNTI && NodoInterruzionePercorso!=mission.pend){
           //----------------------------------------------------------------
           // Punto all'indice di percorso su cui trovo il
           // NodoInterruzionePercorso per nodo occupato
           //----------------------------------------------------------------
           for( i=MAXPERCORSI-1; i>ptr_percorso;  i--){
              if( mission.punto[ i ]!=NodoInterruzionePercorso ) continue;
              break;
           }
           //----------------------------------------------------------------
           // Considero come reale nodo di interruzione percorso il primo
           // nodo a una distanza DIST_MIN_ROT_DOPPIA_MAX dal satellite_occ
           // per avere la garanzia di raggiungerlo
           //----------------------------------------------------------------
           for(; i>ptr_percorso;  i--){
              x1 = Punti[ mission.punto[ i ] ][0];
              y1 = Punti[ mission.punto[ i ] ][1];
              xv = Punti[ satellite_occupato ][0];
              yv = Punti[ satellite_occupato ][1];
              CatetoX = (double) abs(x1-xv);
              CatetoY = (double) abs(y1-yv);
              if(CatetoX>30000 || CatetoY>30000) continue;
              CalcolaRad   = (CatetoX*CatetoX) + (CatetoY*CatetoY);
              DistIngombro = (int) sqrt_algo( CalcolaRad );
              rot1         = mission.dati_perc[i].rot;
            //if( DistIngombro<DIST_MIN_ROT_DOPPIA_MAX) continue;
              if( DistIngombro<DIST_MIN_ROT_DOPPIA_MAX || rot1>3 ){  // se punto di inzio curva o distanza inferiore a ROT_DOPPIA_MAX
                 //--------------------------------------------------------
                 // 26/10/2013 (NECESSARIO PER NAVETTE LASER)
                 // Verifico se i TGV sono già ruotati su rette parallele
                 //--------------------------------------------------------
                 if(mission.GestioneLaser==true || AGV[AgvPerInterruzionePercorso]->mission.GestioneLaser==true){
                    AgvInIngombro=true;
                    if(i>0 && i<MAXPERCORSI && AGV[AgvPerInterruzionePercorso]->test_mission_on()){
                       rot2 = 99;
                       for(j=1; j<MAXPERCORSI-1; j++){   // scarto primo e ultimo nodo
                          if(AGV[AgvPerInterruzionePercorso]->mission.punto[j]==0) break;
                          if(AGV[AgvPerInterruzionePercorso]->mission.punto[j]!=satellite_occupato) continue;
                          rot2 = AGV[AgvPerInterruzionePercorso]->mission.dati_perc[j].rot;
                          break;
                       }
                       //---------------------------------------------------------------
                       // Verifico se i TGV si trovano su rette parallele
                       //---------------------------------------------------------------
                       RetteParallele=false;
                       // rette parallele in "X"
                       if(CatetoX<=DIST_MIN_ING_CURVA && CatetoY>=DIST_MIN_RETTE_PARALLELE && ((rot1==1 || rot1==3) && (rot2==1 || rot2==3))) RetteParallele=true;
                       // rette parallele in "Y"
                       if(CatetoY<=DIST_MIN_ING_CURVA && CatetoX>=DIST_MIN_RETTE_PARALLELE && ((rot1==0 || rot1==2) && (rot2==0 || rot2==2))) RetteParallele=true;
                       //---------------------------------------------------------------
                       // Se le navette risultano su tratti paralleli devono essere
                       // già ruotate
                       //---------------------------------------------------------------
                       if(RetteParallele==true){
                          //---------------------------------------------------------------
                          // Anche le rotazioni precedenti e successive di entrambi i TGV
                          // devono essere le stesse
                          //---------------------------------------------------------------
                          if(mission.dati_perc[i-1].rot==rot1 && mission.dati_perc[i+1].rot==rot1 &&
                             AGV[AgvPerInterruzionePercorso]->mission.dati_perc[j-1].rot==rot2 && AGV[AgvPerInterruzionePercorso]->mission.dati_perc[j+1].rot==rot2){
                                AgvInIngombro=false;
                          }
                       }
                    }
                    //----------------------------------------------------
                    // Se la distanza è inferiore DIST_MIN_ROT_DOPPIA_MAX
                    // e le rette non sono parallele
                    //----------------------------------------------------
                    if(AgvInIngombro==true) continue;
                 }
              }
              //-------------------------------------------------------
              // Raggiunto un nodo di interruzione che sia sicuramente
              // raggiungibile, se coincide con un ing/out curva, ne
              // memorizzo l'indice
              //
              // AL-28/03/2013 Solo se abilitata gestione no stop
              //               su start deviazione
              //
              // AL-15/01/2014 Solo se abilitata gestione no stop
              //               su start curva
              //
              // AL-23/01/2014 A seconda del flag attivo controllo
              //               lo stato rotazione
              //-------------------------------------------------------
              if(NoPIntSuStartDeviazione==true || (NoPIntSuStartCurva==true && mission.GestioneLaser==true)){
               //if(IndicePercorsoIngOutCrvDev==0 && mission.dati_perc[i  ].rot>3) IndicePercorsoIngOutCrvDev=i;
               //if(IndicePercorsoIngOutCrvDev==0 && mission.dati_perc[i-1].rot>3) IndicePercorsoIngOutCrvDev=i-1;
                 // DEVIAZIONE = ROT>49 && ROT<70
                 if(NoPIntSuStartDeviazione==true){
                    if(IndicePercorsoIngOutCrvDev==0 && mission.dati_perc[i  ].rot>49 && mission.dati_perc[i  ].rot<70) IndicePercorsoIngOutCrvDev=i;
                    if(IndicePercorsoIngOutCrvDev==0 && mission.dati_perc[i-1].rot>49 && mission.dati_perc[i-1].rot<70) IndicePercorsoIngOutCrvDev=i-1;
                 }
                 // CURVA = ROT>3 && ROT<40
                 if(NoPIntSuStartCurva==true){
                    if(IndicePercorsoIngOutCrvDev==0 && mission.dati_perc[i  ].rot>3 && mission.dati_perc[i  ].rot<40) IndicePercorsoIngOutCrvDev=i;
                    if(IndicePercorsoIngOutCrvDev==0 && mission.dati_perc[i-1].rot>3 && mission.dati_perc[i-1].rot<40) IndicePercorsoIngOutCrvDev=i-1;
                 }
              }
              //-------------------------------------------------------
              // Raggiunto un nodo di interruzione che sia sicuramente
              // raggiungibile, se il TGV di interruzione è senza
              // missione in corso memorizzo il nodo in modo che
              // il TGV considerato si fermi a una distanza di
              // sicurezza che permetta all'altra navetta di ripartire
              //-------------------------------------------------------
              if(ForzaPIntPerNavettaInAttesa==true && AgvPerInterruzionePercorso>0 && AgvPerInterruzionePercorso<=MAXAGV && (AGV[AgvPerInterruzionePercorso]->stato.start==0 || AGV[AgvPerInterruzionePercorso]->mission.pstart==0)){
                 if(N.punti_notevoli[ AGV[AgvPerInterruzionePercorso]->stato.pos ]==false && N.verifica_nodo_fuori_ingombro(AGV[AgvPerInterruzionePercorso]->stato.pos)==false){
                    NodoPercorsoIngombroAnticipatoAltroAgvFermo = mission.punto[ i ];
                 }
              }
              break;
           }
        }
     }
     //-------------------------------------------------------------------------------
     // AL-17/01/2014 NESSUNA INTERRUZIONE FINO A DESTINAZIONE PT_IN_INGOMBRO
     // Se il TGV non ha nessuna interruzione fino al pt_in_ingombro, ma un altro TGV
     // deve transitare a una distanza < DIST_ROT_DOPPIA_MAX dal pt_in_ingombro e ha
     // già occupato un nodo a una distanza < DIST_ROT_DOPPIA_MAX dal pt_in_ingombro
     // (quindi non si può fermare) --> DEVO FERMARE IL TGV CONSIDERATO
     //-------------------------------------------------------------------------------
     if(NodoPercorsoIngombroPt==0 && IndicePercorsoIngOutCrvDev==0 && NodoPercorsoIngombroAnticipatoAltroAgvFermo==0){
        //---------------------------------------------------------
        // CASO 1: il TGV considerato ha il percorso libero fino
        //         a destinazione e questa è un pt_in_ingombro
        //---------------------------------------------------------
        if((percorso_libero==1 || NodoInterruzionePercorso==mission.pend) && UltimoNodoDiPercorsoPrimaDellaDest!=0 && N.punti_notevoli[ mission.pend ]==true && N.pt_in_ingombro[ mission.pend ]==true){
           //-------------------------------------------
           // Verifico se c'è un altro TGV con un PINT
           // a una distanza < DIST_ROT_DOPPIA_MAX
           // (che quindi non si può fermare)
           //-------------------------------------------
           x1 = Punti[ UltimoNodoDiPercorsoPrimaDellaDest ][0];
           y1 = Punti[ UltimoNodoDiPercorsoPrimaDellaDest ][1];
           for(i=1; i<=MAXAGV; i++){
              if(i==agv_num                       ) continue;
              if(AGV[i]->mission.pintend<1        ) continue;
              if(AGV[i]->mission.pintend>TOTPUNTI ) continue;
              xv = Punti[ AGV[i]->mission.pintend ][0];
              yv = Punti[ AGV[i]->mission.pintend ][1];
              CatetoX = (double) abs(x1-xv);
              CatetoY = (double) abs(y1-yv);
              if(CatetoX>30000 || CatetoY>30000) continue;
              CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
              DistIngombro = (int) sqrt_algo( CalcolaRad );
              if(DistIngombro>=DIST_MIN_ROT_DOPPIA_MAX) continue;
              //------------------------------------------------------
              // Se il TGV continua ad avvicinarsi:
              // cotrollo il successivo al PINT già occupato se è
              // ancora più vicino al PT_IN_INGOMBRO
              // (l'unico controllo possibile che funziona in
              //  in previsione di SALTI NODO)
              //------------------------------------------------------
              for(j=MAXPERCORSI-1; j>=0; j--){
                 if(AGV[i]->mission.punto[j]!=AGV[i]->mission.pintend) continue;
                 //-------------------------------------------------------
                 // Se successivo esiste verifico se più vicino
                 //-------------------------------------------------------
                 if(AGV[i]->mission.punto[j+1]<1        ) break;
                 if(AGV[i]->mission.punto[j+1]>TOTPUNTI ) break;
                 xv = Punti[ AGV[i]->mission.punto[j+1] ][0];
                 yv = Punti[ AGV[i]->mission.punto[j+1] ][1];
                 CatetoX = (double) abs(x1-xv);
                 CatetoY = (double) abs(y1-yv);
                 if(CatetoX>30000 || CatetoY>30000) continue;
                 CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
                 DistIngombroSucc = (int) sqrt_algo( CalcolaRad );
                 //-------------------------------------------------------
                 // Allontanamento da PT_IN_INGOMBRO
                 //-------------------------------------------------------
                 if(DistIngombroSucc>DistIngombro) continue;
                 //-------------------------------------------------------
                 // ALTROTGV in avvicinamento a PT_IN_INGOMBRO con
                 // TGV considerato in dest = PT_IN_INGOMBRO
                 //
                 // Setto come "NodoInterruzionePercorso" il pint del
                 // tgv adiacente e come "NodoInterruzionePercorso"  il
                 // nodo prima del pt_in_ingombro in modo da garantire
                 // una fermata del tgv considerato sufficenemente distante
                 // da far transitare il tgv adiacente
                 //-------------------------------------------------------
                 AgvPerInterruzionePercorso = i;
                 NodoPercorsoIngombroPt     = AGV[i]->mission.pintend;      // !!! IMPORTANTE
                 NodoInterruzionePercorso   = UltimoNodoDiPercorsoPrimaDellaDest;
                 //---------------------------------------------------------
                 // Il nuovo satellite occupato a cui fare riferimento per
                 // la distanza diventerà il nodo di uscita
                 //---------------------------------------------------------
                 satellite_occupato         = AGV[i]->mission.pintend;
                 break;
              }
              if(NodoPercorsoIngombroPt!=0) break;
           }
        }
        //---------------------------------------------------------
        // CASO 2: il TGV considerato ha un interruzione di perc
        //         ma il nodo precedente all'interruzione è un
        //         pt_in_ingombro.
        //---------------------------------------------------------
        if(NodoPercorsoIngombroPt==0){
           if(percorso_libero==0 && NodoInterruzionePercorso!=0 && NodoInterruzionePercorso!=mission.pend && PrecInterruzionePercorso!=0 && N.punti_notevoli[ PrecInterruzionePercorso ]==true && N.pt_in_ingombro[ PrecInterruzionePercorso ]==true){
              //-------------------------------------------
              // Verifico se c'è un altro TGV con un PINT
              // a una distanza < DIST_ROT_DOPPIA_MAX
              // (che quindi non si può fermare)
              //-------------------------------------------
              x1 = Punti[ NodoInterruzionePercorso ][0];
              y1 = Punti[ NodoInterruzionePercorso ][1];
              for(i=1; i<=MAXAGV; i++){
                 if(i==agv_num                       ) continue;
                 if(AGV[i]->mission.pintend<1        ) continue;
                 if(AGV[i]->mission.pintend>TOTPUNTI ) continue;
                 xv = Punti[ AGV[i]->mission.pintend ][0];
                 yv = Punti[ AGV[i]->mission.pintend ][1];
                 CatetoX = (double) abs(x1-xv);
                 CatetoY = (double) abs(y1-yv);
                 if(CatetoX>30000 || CatetoY>30000) continue;
                 CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
                 DistIngombro = (int) sqrt_algo( CalcolaRad );
                 if(DistIngombro>=DIST_MIN_ROT_DOPPIA_MAX) continue;
                 //------------------------------------------------------
                 // Se il TGV continua ad avvicinarsi:
                 // cotrollo il successivo al PINT già occupato se è
                 // ancora più vicino al PT_IN_INGOMBRO
                 // (l'unico controllo possibile che funziona in
                 //  in previsione di SALTI NODO)
                 //------------------------------------------------------
                 for(j=MAXPERCORSI-1; j>=0; j--){
                    if(AGV[i]->mission.punto[j]!=AGV[i]->mission.pintend) continue;
                    //-------------------------------------------------------
                    // Se successivo esiste verifico se più vicino
                    //-------------------------------------------------------
                    if(AGV[i]->mission.punto[j+1]<1        ) break;
                    if(AGV[i]->mission.punto[j+1]>TOTPUNTI ) break;
                    xv = Punti[ AGV[i]->mission.punto[j+1] ][0];
                    yv = Punti[ AGV[i]->mission.punto[j+1] ][1];
                    CatetoX = (double) abs(x1-xv);
                    CatetoY = (double) abs(y1-yv);
                    if(CatetoX>30000 || CatetoY>30000) continue;
                    CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
                    DistIngombroSucc = (int) sqrt_algo( CalcolaRad );
                    //-------------------------------------------------------
                    // Allontanamento da PT_IN_INGOMBRO
                    //-------------------------------------------------------
                    if(DistIngombroSucc>DistIngombro) continue;
                    //-------------------------------------------------------
                    // ALTROTGV in avvicinamento a PT_IN_INGOMBRO con
                    // TGV considerato transita su PT_IN_INGOMBRO come
                    // percorso.
                    //
                    // Setto come "NodoPercorsoIngombroPt" il
                    // NodoDiInteruzione del tgv considerato, questo
                    // garantisce di fermarsi molto prima e di far
                    // transitare il tgv adiacente
                    //-------------------------------------------------------
                    NodoPercorsoIngombroPt = NodoInterruzionePercorso;
                    //---------------------------------------------------------
                    // Il nuovo satellite occupato a cui fare riferimento per
                    // la distanza diventerà il nodo di uscita
                    //---------------------------------------------------------
                    satellite_occupato     = AGV[i]->mission.pintend;
                    break;
                 }
                 if(NodoPercorsoIngombroPt!=0) break;
              }
           }
        }           
     }
     //------------------------------------------------------
     // Calcola punto di Stop anticipato  ANDANDO a RITROSO
     //------------------------------------------------------
     if( NodoPercorsoIngombroPt || IndicePercorsoIngOutCrvDev || NodoPercorsoIngombroAnticipatoAltroAgvFermo){
        //-------------------------------------------------------
        // (1) ANTICIPO PER PT IN INGOBRO
        // Mi devo fermare sul primo nodo a una distanza pari a
        // DIST_MIN_ROT_DOPPIA_MAX
        //-------------------------------------------------------
        if(NodoPercorsoIngombroPt!=0){
           for( i=MAXPERCORSI-1; i>ptr_percorso;  i--){
              if( mission.punto[ i ]==0 ) continue;
              //---------------------------------------------------------
              // 05/02/2013 valuto solo i nodi di rettilineo
              //
              // AL-22/02 non devo scartare i nodi di deviazione
              //---------------------------------------------------------
            //if(mission.dati_perc[i  ].rot>3 ) continue;
            //if(mission.dati_perc[i-1].rot>3 ) continue;

              NodoConsiderato = mission.punto[ i ];
              //--------------------------------------------------------------------------------------------
              // Verifico il nodo di interruzione o quello di ingombro
              //--------------------------------------------------------------------------------------------
              if( NodoConsiderato==NodoPercorsoIngombroPt || NodoConsiderato==NodoInterruzionePercorso){
                 NodoPercorsoIngombroAnticipatoPt=NodoConsiderato;
              }
              //--------------------------------------------------------------------------------------------
              // La distanza deve essere DIST_MIN_ROT_DOPPIA_MAX --> TGV deve potere uscire e ruotare
              //--------------------------------------------------------------------------------------------
              if( NodoPercorsoIngombroAnticipatoPt>0 && NodoConsiderato!=NodoPercorsoIngombroPt){
                 x1 = Punti[ NodoConsiderato ][0];
                 y1 = Punti[ NodoConsiderato ][1];
                 xv = Punti[ NodoPercorsoIngombroPt ][0];
                 yv = Punti[ NodoPercorsoIngombroPt ][1];
                 CatetoX = (double) abs(x1-xv);
                 CatetoY = (double) abs(y1-yv);
                 if(CatetoX>30000 || CatetoY>30000) continue;
                 CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
                 DistIngombro = (int) sqrt_algo( CalcolaRad );
                 //--------------------------------------------------------------------------------------
                 // AL-22/02 Memorizzo l'ultimo nodo a una distanza < DIST_MIN_ROT_DOPPIA_MAX o che
                 //          si trova su un tratto inclinato
                 //--------------------------------------------------------------------------------------
                 if(NoPIntSuStartDeviazione==true || (NoPIntSuStartCurva==true && mission.GestioneLaser==true)){
                    //-----------------------------------------------------
                    // AL-23/01/2014 A seconda del flag attivo controllo
                    //               lo stato rotazione
                    //-----------------------------------------------------
                    rot_not_ok=false;
                    if(NoPIntSuStartDeviazione==true){
                       if(mission.dati_perc[i  ].rot>49 && mission.dati_perc[i  ].rot<70) rot_not_ok=true;
                       if(mission.dati_perc[i-1].rot>49 && mission.dati_perc[i-1].rot<70) rot_not_ok=true;
                    }
                    // CURVA = ROT>3 && ROT<40
                    if(NoPIntSuStartCurva==true){
                       if(mission.dati_perc[i  ].rot> 3 && mission.dati_perc[i  ].rot<40) rot_not_ok=true;
                       if(mission.dati_perc[i-1].rot> 3 && mission.dati_perc[i-1].rot<40) rot_not_ok=true;
                    }
                  //if( DistIngombro<=DIST_MIN_ROT_DOPPIA_MAX || mission.dati_perc[i].rot>3 || mission.dati_perc[i-1].rot>3){
                    if( DistIngombro<=DIST_MIN_ROT_DOPPIA_MAX || rot_not_ok==true){
                       NodoPercorsoIngombroAnticipatoPt=NodoConsiderato;
                    }else{
                       break;
                    }
                 }
                 //--------------------------------------------------------------------------------------
                 // AL-17/01/2014 Se è abilitato lo stop su Start/End DEVIAZIONE e CURVA memorizzo solo
                 // l'ultimo nodo a distanza < DIST_MIN_ROT_DOPPIA_MAX
                 //--------------------------------------------------------------------------------------
                 else{
                    if( DistIngombro<=DIST_MIN_ROT_DOPPIA_MAX){
                       NodoPercorsoIngombroAnticipatoPt=NodoConsiderato;
                    }else{
                       break;
                    }
                 }
              }
           }
        }
        //-------------------------------------------------------
        // (2) ANTICIPO PER ING/OUT CURVA o DEVIAZIONE
        // Mi devo fermare sul primo nodo di rettilineo
        // cominciando a scorrere il percorso a ritroso
        //-------------------------------------------------------
        if(IndicePercorsoIngOutCrvDev>0){
           for( i=IndicePercorsoIngOutCrvDev; i>ptr_percorso;  i--){
              if( mission.punto[ i ]==0       ) continue;
              //---------------------------------------------------------
              // Memorizzo l'ultimo nodo che presenta una curva/deviaz.
              //---------------------------------------------------------
              if(mission.dati_perc[i  ].rot>3 ){
                 NodoPercorsoIngombroAnticipatoCrvDev = mission.punto[ i ];
              }else{
                 break;
              }
           }
        }
        //-------------------------------------------------------
        // (3) Tra i 3 nodi di anticipo eventualmente trovati
        // scelgo quello PIU' ANTICIPATO
        //-------------------------------------------------------
        for( i=ptr_percorso; i<MAXPERCORSI; i++){
           if( mission.punto[ i ]==0 ) break;
           if( mission.punto[ i ]!=NodoPercorsoIngombroAnticipatoPt &&
               mission.punto[ i ]!=NodoPercorsoIngombroAnticipatoCrvDev &&
               mission.punto[ i ]!=NodoPercorsoIngombroAnticipatoAltroAgvFermo) continue;
           //------------------------------------------------------------------------------
           // Scelgo il primo dei 2 nodi anticipati che trovo nell'esecuzione del percorso
           //------------------------------------------------------------------------------
           NodoIngombroPercorsoAnticipato = mission.punto[ i ];
           NumTgvPerAnticipo              = AgvPerInterruzionePercorso;
           break;
        }
     }
  }

  //---------------------------------------------------------------
  // (F) 18/10/95  Compila Array nodi prenotati da tutti gli AGV
  //         Indice 1 = Numero nodo
  //         Indice 2 = Numero AGV che ha prenotato ( 0 = NO)
  //---------------------------------------------------------------
  memset(&nodi_prenotati , 0, sizeof(nodi_prenotati));
  for( altro_agv=1; altro_agv<=MAXAGV; altro_agv++){
     for( i=0; i<MAXPERCORSI; i++){
        numero_nodo = AGV[ altro_agv ]->mission.punto_prenotato[i];
        if( numero_nodo==0 ) break;
        if( numero_nodo > TOTPUNTI ) break;
        nodi_prenotati[ numero_nodo ][ altro_agv-1 ] = altro_agv;
     }
  }

  //----------------------------------------------------------
  // (G) Valuta se OCCUPARE  NODO
  //----------------------------------------------------------
  salto_a_nodo       = 0;
  nodi_occupati      = 0;
  satellite_occupato = 0;
  *agv_bloccante     = 0;
  AgvBloccante       = 0;
  //if(stato.start == 0){
  for( ; ptr_percorso<MAXPERCORSI; ptr_percorso++){
     RioccupaNodo = false;
     ptr_nodo = mission.punto[ptr_percorso];
     if(ptr_nodo > TOTPUNTI+10) break;
     //-------------------------------------------------------
     // GESTIONE SALTA NODI
     // Non posso assolutamente forzare come PINTEND un nodo
     // che ho previsto di saltare.
     //-------------------------------------------------------
     salto_a_nodo = mission.dati_perc[ptr_percorso].salta_a_ps;
     satellite_occupato1 = 0;
     memset(&nodi_da_saltare[0], 0, sizeof(nodi_da_saltare));
     if(salto_a_nodo!=0){
        //------------------------------------------------------
        // Verifico se tutti i nodi fina a quello a cui saltare
        // sono occupabili e li occupo
        //------------------------------------------------------
        indice=0;
        for(i=ptr_percorso; i<MAXPERCORSI; i++){
           if(mission.punto[i]==0           ) break;
           if(mission.punto[i]==salto_a_nodo) break;
           ptr_nodo1 = mission.punto[i];
           //------------------------------------------------
           // Verifico se il nodo considerato è già occupato
           //------------------------------------------------
           if(N.nodo_busy[ptr_nodo1]!=0 && N.nodo_busy[ptr_nodo1]!=agv_num){
              satellite_occupato1 = ptr_nodo1;
              break;
           }
           //-----------------------------------------------------------------------
           // 02/01/2013 Controllo se percorso interrotto per PT in ingombro
           // Interromi SALTO NODI
           //-----------------------------------------------------------------------
           if(ptr_nodo1==NodoIngombroPercorsoAnticipato){
              satellite_occupato1 = ptr_nodo1;
              break;
           }
           //------------------------------------------------
           // Verifico se i nodi vicini sono TUTTI fuori
           // ingombro
           //------------------------------------------------
           NodoOccupabile1=AgvPerInterruzionePercorso1=NodoInterruzionePercorso1=0;
           VerificaNodoOccupatoIngombroAgv(TRUE, agv_num, ptr_nodo1, NodoOccupabile1, AgvPerInterruzionePercorso1, satellite_occupato1, NodoInterruzionePercorso1, 0, 0);
           if(NodoOccupabile1==0){
              satellite_occupato1 = ptr_nodo1;
              break;
           }
           //----------------------------------------------
           // Se il nodo considerato è disponibile
           // compilo la struttura dei nodi da saltare
           //----------------------------------------------
           nodi_da_saltare[indice] = ptr_nodo1;
           indice++;
        }
        //-----------------------------------------------
        // Se nessuno tra nodi da saltare è impegnato
        // memorizzo l'indice a cui saltare nel ciclo
        // di occupazioni nodo successivo.
        //-----------------------------------------------
        if(satellite_occupato1==0){
           ptr_percorso = i;
           ptr_nodo     = salto_a_nodo;
        }
     }
     //---------------------------------------------------------
     // (G1) Se il NODO è libero verifica se quelli circostanti
     //      siano altrettanto liberi
     //---------------------------------------------------------
     PuntoConsiderato = mission.punto[ptr_percorso];
     //---------------------------------------------------------
     // AL-06/02/2014 Nello start da Punto di percorso
     // è necessario valutare anche se ci sono le condizioni
     // per rioccupare il nodo su cui il TGV si trova per
     // poterlo conservare come PINTEND
     //---------------------------------------------------------
     if(stato.start==0 && N.punti_notevoli[stato.pos]==false && ptr_nodo==stato.pos){
        last_nodo=pintend=0;
        RioccupaNodo = true;
     }
     //---------------------------------------------------------
     // (G3) Se il NODO è libero verifica se quelli circostanti
     //      siano altrettanto liberi
     //---------------------------------------------------------
     if(N.nodo_busy[ptr_nodo]==0 || RioccupaNodo==true){
        //-------------------------------------------------------
        // Estrai le coordinate del nodo da occupare
        // (x1,y1) Attuale (x2,y2) Precedente (x3, y3) Sucessivo
        //-------------------------------------------------------
        x1 = Punti[ ptr_nodo ][0];
        y1 = Punti[ ptr_nodo ][1];
        //-------------------------------------------------------------------------------
        // (G3) Altrimenti verifica se nodo SATELLITE OCCUPATO: DIST<DIST_MIN_ROT_DOPPIA_MAX
        //
        // Ritorna :
        //
        //    NodoOccupabile           = 1 -> OK
        //    agv_adiacente            = xx
        //    satellite_occupato       = yy
        //    NodoInterruzionePercorso = zz
        //
        //-------------------------------------------------------------------------------
        VerificaNodoOccupatoIngombroAgv(RioccupaNodo, agv_num, PuntoConsiderato, NodoOccupabile, agv_adiacente, satellite_occupato, NodoInterruzionePercorso, 0, 0);
        //-----------------------------------------------------------------------
        // 02/01/2013 Controllo se percorso interrotto per PT in ingombro
        // a) Percorso Standard
        // b) Percorso Con Salto Nodi
        //
        // AL-22/02 setto il satellite_occupato!=0 solo se esiste il nodo
        //          anticipato (SICUREZZA)
        //-----------------------------------------------------------------------
        if(NodoIngombroPercorsoAnticipato!=0){
           if((PuntoConsiderato==NodoIngombroPercorsoAnticipato && NumTgvPerAnticipo!=agv_num) || (mission.pintend!=0 && mission.pintend==NodoIngombroPercorsoAnticipato)){
              agv_adiacente=NumTgvPerAnticipo;
              NodoOccupabile=0;
              satellite_occupato=NodoIngombroPercorsoAnticipato;
           }
        }
        //--------------------------------------------------------------
        // (G4) N.B. TOLTO da ciclo FOR di G3 ---> MI SEMBRA UN ERRORE
        //--------------------------------------------------------------
        if( SempreON && RioccupaNodo==false ){
           //---------------------------------------------------------------
           // (G4.01) 18/10/95  Se il percorso non è completamente libero
           //                   occupa comunque fino al primo nodo prenotato
           //                   da altro AGV
           //
           // Ritorna :
           //
           //    NodoOccupabile           = 1 -> OK
           //    agv_adiacente            = xx
           //    satellite_occupato       = yy
           //    NodoInterruzionePercorso = zz
           //---------------------------------------------------------------
           //
           // AL-13/09/2021 - controllo indipendente dal flag Rette parallele non valorizzato 
           //
         //if( percorso_libero==0 && satellite_occupato==0 && (RetteParallele==false || (RetteParallele==true && NodoOccupabile!=0))){
           if( percorso_libero==0 && satellite_occupato==0 && NodoOccupabile!=0 ){
              VerificaNodoPrenotatoIngombroAgv( agv_num, PuntoConsiderato, NodoOccupabile, agv_adiacente, satellite_occupato, NodoInterruzionePercorso );
           }
           if(NodoOccupabile==0){
              EsisteZonaFuoriIngombro=0;
              if(agv_adiacente==0) agv_adiacente=AgvPerInterruzionePercorso;
              zona_fuori_ingombro_manuale = N.verifica_zona_fuori_ingombro_manuale(agv_num, ptr_nodo, agv_adiacente);
              if(zona_fuori_ingombro_manuale==true){
                 EsisteZonaFuoriIngombro = MIN_NODI_FUORI_INGOMBRO+1;
                 NodoOccupabile          = 1;
                 satellite_occupato      = 0;
              }
              //------------------------------------------------
              // Se non esiste nemmeno una zona fuori ing.
              // --> skip!
              //------------------------------------------------
              if( EsisteZonaFuoriIngombro==0 ){
                 *agv_bloccante=(short int)(agv_adiacente);
                 AgvBloccante  =(short int)(agv_adiacente);
              }
              else{
                 NodoOccupabile     = 1;
                 satellite_occupato = 0;
              }
           }
        }
        //-----------------------------------------------------------------
        // AL-06/02/2014 Il nodo iniziale (source) e il nodo finale (dest)
        //               vengono occupati comunque se non fanno parte di
        //               quelli "da rioccupare".
        //               Questa gestione ingloba anche i nodi di attesa
        //               perchè nodi di percorso, quindi è stata
        //               commentata la gestione di "20/06/2011 G.B"
        // AL-27/01/2015 Se la destinazione è un nodo di percorso devono
        //               valere i normali controlli legati alle precedenze
        //               verranno utilizzare eventualmente le
        //               zone_fuori_ing
        //-----------------------------------------------------------------
        if(RioccupaNodo==false){
           if(ptr_nodo == source) satellite_occupato = 0;
           if(ptr_nodo == dest && N.punti_notevoli[dest]) satellite_occupato = 0;
        }
        //-----------------------------------------------------------------
        // RIOCCUPA NODO
        // Se il nodo non è occupabile verifico se si tratta di
        // una zona fuori ingombro manuale
        //-----------------------------------------------------------------
        else{
           if((ptr_nodo == source || ptr_nodo == dest) && satellite_occupato!=0){
              zona_fuori_ingombro_manuale = N.verifica_zona_fuori_ingombro_manuale(agv_num, ptr_nodo, 0);
              if(zona_fuori_ingombro_manuale==true) satellite_occupato = 0;
           }
        }
        //--------------------------------------------------------------
        // Se il percorso è libero Resetto per sicurezza il flag di
        // Agv Bloccante.
        //--------------------------------------------------------------
        if(satellite_occupato==0){
           *agv_bloccante = 0;
        }
        //-------------------------------------------------------
        // (G7) Occupa il nodo
        //-------------------------------------------------------
        if( satellite_occupato == 0) {
           N.nodo_busy[ptr_nodo] = agv_num;
           *agv_bloccante =0;
           modificato     =1;
        }
     }

     //-------------------------------------------------------
     // Se il nodo attuale, o quello sucessivo oppure uno dei
     // nodi circostanti è occupato da altro AGV, interrompi.
     // perché occorrono almeno 2 nodi di distanza tra 2 AGVs
     // per evitare la collisione !!!
     //
     // AL-06/02/2014 Se si tratta di un nodo da rioccupare
     // e satellite_occupato!=0 --> skip! il TGV deve
     // riforzare il pint
     //-------------------------------------------------------
     if(N.nodo_busy[ptr_nodo]!=agv_num || (RioccupaNodo==true && satellite_occupato!=0)){
        //----------------------------------------------------------
        // Se non è stato occupato nessun nodo allora la condizione
        // è di blocco.
        //----------------------------------------------------------
        if(modificato==0 && N.nodo_busy[ptr_nodo]!=0 && N.nodo_busy[ptr_nodo]!=agv_num){   // 11/04/2018 GB Reintrodotto controllo su nodo_busy[] altrimenti il controllo fallisce (da ITALGRES)
           //----------------------------------------------
           // AL-15/06/2017 solo se AgvBloccante non è
           // già stato calcolato
           //----------------------------------------------
           if(AgvBloccante==0){
              *agv_bloccante = N.nodo_busy[ptr_nodo];
               AgvBloccante  = N.nodo_busy[ptr_nodo];
           }
        }
        break;
     }
     //----------------------------------------------
     // Memorizzo l'ultimo nodo occupato
     //----------------------------------------------
     last_nodo    = ptr_nodo;
     //----------------------------------------------
     // GESTIONE SALTO NODO
     // Se ho la certezza che il nodo a cui ho
     // saltato possa essere occupato -> impegno
     // anche tutti i nodi saltati
     //----------------------------------------------
     if(salto_a_nodo!=0 && salto_a_nodo==last_nodo && satellite_occupato1==0){
        //------------------------------------------------------
        // Verifico se tutti i nodi fina a quello a cui saltare
        // sono occupabili e li occupo
        //------------------------------------------------------
        for(i=0; i<20; i++){
           if(nodi_da_saltare[i]==0) break;
           N.nodo_busy[nodi_da_saltare[i]]=agv_num;
           modificato=1;
        }
        //------------------------------------------------------
        // Incremento il contatore dei nodi occupati anche per
        // il numero dei nodi saltati
        //------------------------------------------------------
        nodi_occupati=(short int)(nodi_occupati+(i-1));
        //------------------------------------------------------
        // Decremento il puntatore ai percorsi per accertarmi
        // di verificare anche sul nodo a cui sono saltato se
        // c'è un ulteriore salto impostato
        //------------------------------------------------------
        ptr_percorso--;
     }
     //----------------------------------------------
     // Se il numero di percorsi gi… occupati supera
     // il numero MAX per una missione SKIP !!!
     //----------------------------------------------
     if( nodi_occupati >= nodi_da_occupare ) break;
     nodi_occupati++;
     //----------------------------------------------
     // Condizione di stop per punto intermedio
     //----------------------------------------------
     if(ptr_nodo==dest && mission.punto[ptr_percorso+1]==0     ) break; // Destinazione raggiunta
     if(ptr_nodo==mission.pintend && mission.StopManuale==true ) break; // Stop su pint MANUALE
     if(sotto_porta!=0  && ptr_nodo==sotto_porta               ) break; // Stop su Punto di APERTURA PORTA PERCORSO
     if(semaforo_porta!=0 && ptr_nodo==semaforo_porta          ) break; // Stop su Punto di SEMAFORO PORTA PERCORSO
     if(ing_macchina!=0 && ptr_nodo==ing_macchina              ) break; // Stop su Punto di RICHIESTA INGRESSO MACCHINA/BAIA
     if(salto_a_nodo!=0 && satellite_occupato1!=0              ) break; // Stop su Punto per salto NON FATTIBILE
     //----------------------------------------------
     // PUNTO INTERMEDIO FORZATO
     // Se il TGV ha un pint forzato dalla missione
     //----------------------------------------------
     if(ptr_nodo==mission.PIntForzato) break;
     //----------------------------------------------
     // GESTIONE PRECEDENZE PER TGV IN RETTILINEO
     // Lascio la precedenza alle navette lanciate
     // in rettilineo
     //----------------------------------------------
     blocco_tgv_lanciato = N.blocco_per_tgv_lanciato_su_rettilineo(agv_num, ptr_nodo, &altro_agv);
     if(blocco_tgv_lanciato==true){
        if(altro_agv>0 && altro_agv<=MAXAGV) *agv_bloccante=altro_agv;
        break;
     }
     //----------------------------------------------
     // BLOCCHI MANUALI OCCUPAZIONE NODI
     // Se ho individuato un blocco manuale
     // nella gestione percorsi -> skip!
     //----------------------------------------------
     blocco_manuale = N.verifica_blocco_manuale(agv_num, ptr_nodo, &altro_agv);
     if(blocco_manuale==true){
        if(altro_agv>0 && altro_agv<=MAXAGV) *agv_bloccante=altro_agv;
        break;
     }
     //----------------------------------------------
     // PINT SUI NODI DI ROTAZIONE
     //----------------------------------------------
     if(AbilitaPuntoIntermedioSuRotaz==true && tutto==false && (stato.start!=0 || (stato.start==0 && N.nodo_minpercorsi[stato.pos]==0))){
        if(stato.pos!=ptr_nodo && ptr_percorso>0 && mission.punto[ptr_percorso+1]!=mission.pend){
           if(mission.dati_perc[ptr_percorso].rot<4 && mission.dati_perc[ptr_percorso-1].rot<4 && mission.dati_perc[ptr_percorso].rot!=mission.dati_perc[ptr_percorso-1].rot) break;
        }
     }
  }
  //----------------------------------------------
  // memorizzo il Pint raggiunto
  //----------------------------------------------
  pintend    = last_nodo;
  //-------------------------------------------------------------------------------
  // Aggiorna il punto intermedio di END solo se START = 0
  // In caso di MISSIONI_AL_VOLO aggiornare sempre punto intermedio di STOP
  //-------------------------------------------------------------------------------
  if(!mission.NoMissioniAlVolo){
     //------------------------------------------------------------------
     // Prima di assegnare il nuovo punto intermedio mi accerto che
     // sia sucessivo al precedente durante l'esecuzione della missione
     //------------------------------------------------------------------
     ptr_percorso=0;
     for(i=0; i<MAXPERCORSI; i++){
        if(mission.pintend==0               ) break;
        if(mission.punto[i]==0              ) break;
        if(mission.punto[i]!=mission.pintend) continue;
        ptr_percorso=i;
        break;
     }
     if(ptr_percorso!=0){
        for(i=ptr_percorso+1; i<MAXPERCORSI; i++){
           if(mission.punto[i]==0        ) break;
           if(mission.punto[i]!=pintend  ) continue;
           mission.pintend = pintend;
           break;
        }
     }
     else{
        mission.pintend = pintend;
     }
  }
  else{
     if(stato.start == 0){
        mission.pintend = pintend;
     }
  }
  //------------------------------------------------------------------
  // 14/02/97 S.Valentino inibito salvataggio su file
  //          dei nodi occupati in modo da accelerare l'applicazione
  // 11/05/97 Riabilitato
  //------------------------------------------------------------------
  err=0;
  N.nodo_busy[mission.pintend]=agv_num;           // SICUREZZA: occupo il nodo fissato come pintend
  if(modificato) err = N.save_nodo_busy(all_mess);
  if(err) return 1;

  return 0;
}
// ------------------------------------------------------------------------
//------------------------------------------------------------------
//  VerificaZonaFuoriIngombro( )
//------------------------------------------------------------------
//  Restituisce :
//
//      EsisteZonaFuoriIngombro           -> Nodo dove inizia la zona fuori ingombro
//
//
int VerificaZonaFuoriIngombro( int NumAgv, int NodoConsiderato, int NodoInterruzionePercorso, int AgvPerInterruzionePercorso, int & EsisteZonaFuoriIngombro)
{
  int   SempreON=1;
  int   PuntoSucessivo, RotProx, RotProxPiu1;
  int   w, z, k, NodProx, NodProxPiu1, NumNodiLiberi,
        DiversoAgv, NodoGiaPrenotato;
  int   dest;
  int   ptr_percorso;
  bool  AlmenoUnTgvBloccante;
  bool  agv_bloccante, salto_nodi;
  bool  zona_fuori_ingombro_manuale;
//int   id_pint;
  int    x1, y1, x2, y2, x3, y3;
  double Dist, DistX, DistY;
  int    NodoCosiderato1, NodoOccupabile1, AgvPerInterruzionePercorso1, satellite_occupato1, NodoInterruzionePercorso1;


  //-----------------------------------------------
  // Inizializza Variabili
  //-----------------------------------------------
  AlmenoUnTgvBloccante=false;
  EsisteZonaFuoriIngombro=0;
  
  if( NodoConsiderato <= 0       ) return 1;
  if( NodoConsiderato > TOTPUNTI ) return 1;
  dest = AGV[ NumAgv ]->mission.pend;

  //---------------------------------------------------------------------------
  // 28/07/2011 SE NON C'E' AGV BLOCCANTE LA ZONA FUORI INGOMBRO    ESISTE
  //---------------------------------------------------------------------------
  if(AgvPerInterruzionePercorso==0){
     EsisteZonaFuoriIngombro=1;
     return 0;
  }

  //--------------------------------------------------------------
  // (G4) Se il percorso non è completamente libero
  //      occupa comunque fino al primo nodo prenotato
  //      da altro AGVN.B.
  //--------------------------------------------------------------
  if( SempreON ){
     //--------------------------------------------------------------------------------
     // Cerca punto del percorso all'interno della missione
     // se NON presente --> SKIP
     //--------------------------------------------------------------------------------
     for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
        if(AGV[ NumAgv ]->mission.punto[ptr_percorso] == NodoConsiderato) break;
        if(AGV[ NumAgv ]->mission.punto[ptr_percorso] == 0) break;
     }
     if(AGV[ NumAgv ]->mission.punto[ptr_percorso] == 0) return 1;
     PuntoSucessivo = AGV[ NumAgv ]->mission.punto[ptr_percorso+1];
     //-------------------------------------------------------------
     // AL13/03 Verifico la distanza tra nodo considerato e
     // successivo se supero il DIST_MIN_ROT_DOPPIA_MAX il
     // punto successivo non è da considerare nella distanziamento
     //-------------------------------------------------------------
     x1 = Punti[NodoConsiderato][0];
     y1 = Punti[NodoConsiderato][1];
     x2 = Punti[PuntoSucessivo ][0];
     y2 = Punti[PuntoSucessivo ][1];
     //-------------------------------------------------------------
     // Calcolo delle distanze da punto a punto
     //-------------------------------------------------------------
     DistX = abs(y1-y2);
     DistY = abs(x1-x2);
     if(DistX<30000 && DistY<30000){
        Dist = (double)(sqrt_algo((DistX*DistX)+(DistY*DistY)));
        //-------------------------------------------------
        // START DA PT NON IN INGOMBRO --> skip!
        //-------------------------------------------------
        if(Dist>=DIST_MIN_ROT_DOPPIA_MAX) PuntoSucessivo=0;
     }
     //-------------------------------------------------------------
     // 15/04/99
     // In caso il percorso dell'AGV non sia completamente libero
     // sino a destinazione (percorso_libero==0), prima di fermarmi
     // al primo nodo prenotato in senso opposto verifico se
     // sucessivamente c'è una "Zona di FUORI INGOMBRO" cioè un
     // tratto di percorso dove l'AGV si possa muovere senza causare
     // problemi.
     //-------------------------------------------------------------
     NumNodiLiberi=0;
     for( w=ptr_percorso; w<MAXPERCORSI; w++ ){
        NodoGiaPrenotato=0;
        NodProx     = AGV[ NumAgv ]->mission.punto[ w   ];
        NodProxPiu1 = AGV[ NumAgv ]->mission.punto[ w+1 ];
        RotProx     = AGV[ NumAgv ]->mission.dati_perc[ w   ].rot;
        RotProxPiu1 = AGV[ NumAgv ]->mission.dati_perc[ w+1 ].rot;
        //        if( satellite_occupato==0             ) break;
        if( NodProx==dest                     ) break;
        if( NodProx==0                        ) break;
        if( NodProx>TOTPUNTI                  ) break;
        if( N.nodo_busy[ NodProx ]!=0         ) break;
        if( NodProx==NodoInterruzionePercorso ) break;

        //------------------------------------------------
        // Controlla se il nodo è prenotato da altro AGV
        //------------------------------------------------
        for( DiversoAgv=1; DiversoAgv<=MAXAGV; DiversoAgv++ ){
           if( DiversoAgv==NumAgv                                            ) continue;
           if(!AGV[DiversoAgv]->test_mission_on()                            ) continue; // AL-IVC 18/04/2017 (!!! PER VELOCIZZARE !!!) scarto i TGV senza missione in corso
           if(AGV[DiversoAgv]->stato.pos==AGV[DiversoAgv]->mission.pintend   ) continue; // AL-IVC 18/04/2017 (!!! PER VELOCIZZARE !!!) scarto i TGV fermi sul proprio PINT
           agv_bloccante=false;
           //-----------------------------------------------------
           // Il TGV deve avere almeno un nodo occupato da altro
           // TGV e questo nodo deve corrispondere al pintermedio.
           // NOTA: non controllo i nodi che non sono punti
           //       intermedi perchè su questi nodi, comunque il
           //       TGV sarebbe in movimento
           //-----------------------------------------------------
           for( z=ptr_percorso; z<MAXPERCORSI; z++ ){
              if(AGV[ NumAgv ]->mission.punto[z]==0                               ) break;
              if(N.nodo_busy[AGV[ NumAgv ]->mission.punto[z]]!=DiversoAgv         ) continue;
              if(AGV[ DiversoAgv ]->mission.pintend!=AGV[NumAgv]->mission.punto[z]) continue;
              agv_bloccante=true;
              break;
           }
           //-----------------------------------------------------
           // Verifico se il TGV di interruzione percorso
           // ha il pint a una distanza di rotazione doppia
           //
           // 26/03/2012 - controllo esteso a tutti i TGV
           //-----------------------------------------------------
           for( z=ptr_percorso; z<MAXPERCORSI; z++ ){
              if(agv_bloccante==true              ) break;
              if(AGV[NumAgv]->mission.punto[z]==0 ) break;
              //---------------------------------------------
              // Verifico occupabilità
              //---------------------------------------------
              NodoCosiderato1=AGV[NumAgv]->mission.punto_prenotato[z];
              NodoOccupabile1=AgvPerInterruzionePercorso1=satellite_occupato1=NodoInterruzionePercorso1=0;
              VerificaNodoOccupato(TRUE, NumAgv, NodoCosiderato1, NodoOccupabile1, AgvPerInterruzionePercorso1, satellite_occupato1, NodoInterruzionePercorso1, 0, 0);
              if(NodoOccupabile1==0){
                 //-------------------------------------------
                 // Controllo se si tratta di una zona
                 // fuori ingombro manuale
                 //-------------------------------------------
                 zona_fuori_ingombro_manuale = N.verifica_zona_fuori_ingombro_manuale(NumAgv, NodoCosiderato1, (short int)(AgvPerInterruzionePercorso1));
                 if(zona_fuori_ingombro_manuale==false){ // NODO NON OCCUPABILE
                    agv_bloccante=true;
                 }
              }
           }
           //-----------------------------------------------------
           // Controllo sovrapposizione nodi prenotati
           //-----------------------------------------------------
           if(agv_bloccante==true){
              AlmenoUnTgvBloccante=true;
              if( nodi_prenotati[NodProx][DiversoAgv-1]!=0 ){
                 //-----------------------------------------------------
                 // Allora controllo se l'atro AGV ha lameno 2 nodi
                 // prenotati nello stesso verso del primo
                 //-----------------------------------------------------
                 for( z=0; z<MAXPERCORSI; z++ ){
                    if( AGV[ DiversoAgv ]->mission.punto[ z ]==NodProx ) break;
                 }
                 if( z>=MAXPERCORSI ) NodoGiaPrenotato=1;
                 if( AGV[ DiversoAgv ]->mission.punto[ z   ]!=NodProx     ) NodoGiaPrenotato=1;
                 if( AGV[ DiversoAgv ]->mission.punto[ z+1 ]!=NodProxPiu1 &&
                    ( nodi_prenotati[NodProxPiu1][DiversoAgv-1]!=0 || NumNodiLiberi==0 ) ) NodoGiaPrenotato=1;
              }
           }
        }
        //-----------------------------------------------------------
        // Modifica del 02/02/2000
        //
        // AL13/03 - Se non è stato individuato il TGV bloccante
        //           proseguo nel conteggio dei nodi liberi
        //-----------------------------------------------------------
        if((NodProx!=NodoConsiderato && NodProx!=PuntoSucessivo && NodoGiaPrenotato==0) || AlmenoUnTgvBloccante==false){
           //-----------------------------------------------------
           // SALTO NODI
           // Se NodProx e NodProxPiu1 sono su una stessa retta
           // non deve esistere nessun Nodo in mezzo ai 2 già
           // prenotato a una Dist<DISASSAMENTO_LIMITROFO_MIN
           // da antrambi
           //-----------------------------------------------------
           salto_nodi=false;
           if(RotProx==RotProxPiu1){
              x1 = Punti[NodProx    ][0];
              y1 = Punti[NodProx    ][1];
              x2 = Punti[NodProxPiu1][0];
              y2 = Punti[NodProxPiu1][1];
              //-------------------------------------------------
              // Verifico condizioni di salto nodi
              //-------------------------------------------------
              for(k=1; k<=TOTPUNTI; k++){
                 if(N.n[k]->num<1                    ) continue;
                 if(N.n[k]->num>TOTPUNTI             ) continue;
                 if(N.punti_notevoli[N.n[k]->num]!=0 ) continue;
                 if( N.n[k]->num==NodProx            ) continue;
                 if( N.n[k]->num==NodProxPiu1        ) continue;
                 for( DiversoAgv=1; DiversoAgv<=MAXAGV; DiversoAgv++ ){
                    if( DiversoAgv==NumAgv                           ) continue;
                    if( nodi_prenotati[N.n[k]->num][DiversoAgv-1]==0 ) continue;
                    x3 = Punti[N.n[k]->num][0];
                    y3 = Punti[N.n[k]->num][1];
                    if(abs(x1-x2)<=5 && abs(x1-x3)<=5 && abs(x2-x3)<=5 && abs(y1-y3)<DIST_MAX_SALTO_NODI && abs(y2-y3)<DIST_MAX_SALTO_NODI) salto_nodi=true;
                    if(abs(y1-y2)<=5 && abs(y1-y3)<=5 && abs(y2-y3)<=5 && abs(x1-x3)<DIST_MAX_SALTO_NODI && abs(x2-x3)<DIST_MAX_SALTO_NODI) salto_nodi=true;
                    if(salto_nodi==true) break;
                 }
                 if(salto_nodi==true) break;
              }
           }
           if(salto_nodi==false) NumNodiLiberi++;
        }
        if( NumNodiLiberi >= MIN_NODI_FUORI_INGOMBRO ){
           EsisteZonaFuoriIngombro=NodProx; //EsisteZonaFuoriIngombro=1;
           break;
        }
     }
  }
  return 0;
}


// ------------------------------------------------------------------------
//------------------------------------------------------------------
//  VerificaNodoOccupato( )
//------------------------------------------------------------------
//  Restituisce :
//
//     NodoOccupabile           -> 1 OK
//     satellite_occupato       -> Ha significato solo locale perchè poi viene reinizializzato in occupa_nodi()
//     NodoInterruzionePercorso -> 0...xx
//
//
int VerificaNodoOccupato(bool RioccupaNodo, int NumAgv, int NodoConsiderato,
                           int & NodoOccupabile, int & AgvAdiacente, int & satellite_occupato, int & NodoInterruzionePercorso, int ScartaAgv, int SoloAgv )
{
  int agv_adiacente=0;
  int x1, y1;
  int x2, y2;
  int i, j;

  int x1_aux, x2_aux;
  int y1_aux, y2_aux;
  int Dist_aux, DistX_aux, DistY_aux;

  int Vicino,
      Dist,
      DistX,  DistY,
      DistX2, DistY2,
      xv1, yv1, xv2, yv2;

  double CalcolaRad, CatetoX, CatetoY;

  int         SempreON=1;

  int         rot1, rot2, err;
  int         rot1_adiacente, rot2_adiacente;
  int         rot1_considerato, rot2_considerato;
  int         offset_da_nodo;
  int         ptr_percorso=0;
  int         AgvConsiderato_RotAttuale=0;    // 0=0° 1=90° 2=180° 3=270°
  int         AgvConsiderato_InRot=0;         // 0 Nessun cambio
  bool        CambioDirConsiderato=false;
  bool        CambioDirAdiacente=false;
  bool        AgvAdiacente_In180=false;
  bool        AgvConsiderato_In180=false;
  bool        SenzaMaiIncontrarsi=false;
  int         AgvConsideratoRettaX=0;
  int         AgvConsideratoRettaY=0;
  int         DirAgvConsiderato=0;            // 0 - sconosciuta (fermo)
                                              // 1 - indietro
                                              // 2 - avanti

  int         PuntoSuccessivo;
  int         AgvAdicentePuntoSuccessivo;
  int         AgvAdiacente_ptr_percorso=0;
  int         AgvAdiacente_RotAttuale=0;      // 0=0° 1=90° 2=180° 3=270°
  int         AgvAdiacente_InRot=0;           // 0 Nessun cambio
  int         AgvAdiacenteInRettaX=0;
  int         AgvAdiacenteInRettaY=0;
  int         DirAgvAdiacente=0;              // 0 - sconosciuta (fermo)
                                              // 1 - indietro
                                              // 2 - avanti

  int         rotazione_in_curva;
  int         DirAllontanamento;
  bool        NoStartAgvConsiderato;
  bool        rot_in_corso;
  bool        RotOpposteInX=false;
  bool        RotOpposteInY=false;
  bool        AgvInIseguimento=false;               // I 2 TGV sono sulla stessa retta e si inseguono
  bool        AgvInAllontanamento=false;
  bool        AgvAdiacentaGiaRuotato=false;
  int         AdiacenteInCurva=0;
  int         ConsideratoInCurva=0;
  int         AgvAdiacenteSuCurvaStretta=0;
  int         AgvConsideratoSuCurvaStretta=0;
  bool        vicino_fuori_ingombro, considerato_fuori_ingombro;
  int         DistRetteParallele   = DIST_MIN_RETTE_PARALLELE;
  int         DistRotazioneSingola = DIST_MIN_ROT_SINGOLA;
  int         DistRotazDoppiaMax   = DIST_MIN_ROT_DOPPIA_MAX;
  int         DistRotazDoppiaMin   = DIST_MIN_ROT_DOPPIA_MIN;
  int         DistAllontanamento   = DIST_MIN_ALLONTANAMENTO;

  long double AngoloRadA, AngoloRadB, AngoloRadC, TangA, TangB, QuadranteA, QuadranteB;
  int  DistDev, DistXDev,  DistYDev, DistXCrv, DistYCrv;

  //--------------------------------------------------------------------------
  // Inizializza Variabili
  //--------------------------------------------------------------------------
  if( NodoConsiderato <= 0       ) return 1;
  if( NodoConsiderato > TOTPUNTI ) return 1;

  //--------------------------------------------------------------------------
  // GESTIONE OCCUPAZIONE NODO CON "INGOMBRO AGV"
  //--------------------------------------------------------------------------
  err=0;
  if(AbilitaGestioneIngombroCurvePerStopTGV>1){
     err = VerificaNodoOccupatoIngombroAgv(RioccupaNodo, NumAgv, NodoConsiderato, NodoOccupabile, AgvAdiacente, satellite_occupato, NodoInterruzionePercorso, ScartaAgv, SoloAgv);
     return err;
  }

  //--------------------------------------------------------------------------
  // Verifico se il TGV considerato è su un nodo fuori ingombro
  //--------------------------------------------------------------------------
  considerato_fuori_ingombro=N.verifica_nodo_fuori_ingombro(NodoConsiderato);
  //--------------------------------------------------------------------------
  // (A) - Verifica situazione AGV sul punto considerato
  //--------------------------------------------------------------------------
  if( SempreON ){
     AgvConsideratoRettaX  = 0;
     AgvConsideratoRettaY  = 0;
     AgvConsiderato_InRot  = 0;
     DirAgvConsiderato     = 0;
     //--------------------------------------------------------------------------------
     // Cerca punto del percorso all'interno della missione
     // se NON presente --> SKIP
     //--------------------------------------------------------------------------------
     for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
        if(AGV[ NumAgv ]->mission.punto[ptr_percorso] == NodoConsiderato) break;
        if(AGV[ NumAgv ]->mission.punto[ptr_percorso] == 0) break;
     }
     if(AGV[ NumAgv ]->mission.punto[ptr_percorso] == 0) return 1;
     //--------------------------------------------------------------------------------
     // Valuta Rotazione ATTUALE AGV e eventaule  cambio di rotazione
     //--------------------------------------------------------------------------------
     PuntoSuccessivo = AGV[ NumAgv ]->mission.punto[ptr_percorso+1];
     rot1_considerato = AGV[ NumAgv ]->rot_standard(ptr_percorso);
     if(ptr_percorso>0) rot2_considerato = AGV[ NumAgv ]->rot_standard(ptr_percorso-1);
     else               rot2_considerato = rot1_considerato;
     if((RioccupaNodo==true || AGV[ NumAgv ]->stato.start==0) && NodoConsiderato==AGV[ NumAgv ]->stato.pos ){
        if( AGV[ NumAgv ]->stato.s.bit.grd0   ) rot2_considerato = 0;
        if( AGV[ NumAgv ]->stato.s.bit.grd90  ) rot2_considerato = 1;
        if( AGV[ NumAgv ]->stato.s.bit.grd180 ) rot2_considerato = 2;
        if( AGV[ NumAgv ]->stato.s.bit.grd270 ) rot2_considerato = 3;
        //----------------------------------------------------------
        // In caso di nuova missione il TGV il cambio di rotazione è
        // dato dalla variazione tra lo stato del TGV e il primo
        // passo di programma
        // Memorizziamo 10..13 per rotazioni in uscita da nodo 0..3
        //----------------------------------------------------------
        if(rot1_considerato!=rot2_considerato){
           AgvConsiderato_InRot = rot1_considerato+10;
           //----------------------------------------------------------
           // Memorizzo se il TGV considerato sul nodo considerato
           // deve compiere un 180°
           //----------------------------------------------------------
           AgvConsiderato_In180=false;
           if(rot1_considerato==0 && rot2_considerato==2) AgvConsiderato_In180=true;
           if(rot1_considerato==1 && rot2_considerato==3) AgvConsiderato_In180=true;
           if(rot1_considerato==2 && rot2_considerato==0) AgvConsiderato_In180=true;
           if(rot1_considerato==3 && rot2_considerato==1) AgvConsiderato_In180=true;
        }
     }
     AgvConsiderato_RotAttuale = rot1_considerato;
     //-------------------------------------------------------
     // Memorizzo direzione movimento
     //-------------------------------------------------------
     if(AGV[ NumAgv ]->mission.dati_perc[ptr_percorso].dir) DirAgvConsiderato=1;    // Indietro
     else                                                   DirAgvConsiderato=2;    // Avanti
     //-------------------------------------------------------
     // Controlli per AGV in movimento
     //-------------------------------------------------------
     if(ptr_percorso>0){
        //-------------------------------------------------------
        // Memorizzo un cambio direzione di movimento
        //-------------------------------------------------------
        CambioDirConsiderato=false;
        if(AGV[ NumAgv ]->mission.dati_perc[ptr_percorso].dir!=AGV[ NumAgv ]->mission.dati_perc[ptr_percorso-1].dir){
           CambioDirConsiderato=true;
        }
        //-------------------------------------------------------
        // Cambio rotazione
        //
        // AL-06/02/2014 VALIDO SOLO PER LE ROTAZIONI/DEVIAZIONI
        // per le curve è stato identificiato in rotazione con
        // +20
        //-------------------------------------------------------
        if((AGV[ NumAgv ]->mission.dati_perc[ptr_percorso  ].rot<4 || AGV[ NumAgv ]->mission.dati_perc[ptr_percorso  ].rot>39) &&
           (AGV[ NumAgv ]->mission.dati_perc[ptr_percorso-1].rot<4 || AGV[ NumAgv ]->mission.dati_perc[ptr_percorso-1].rot>39)){
           if(rot1_considerato!=rot2_considerato){
              //-------------------------------------------------------
              // Memorizziamo 10..13 per rotazioni in uscita da nodo 0..3
              //----------------------------------------------------------
              AgvConsiderato_InRot = rot1_considerato+10;
              //----------------------------------------------------------
              // Memorizzo se il TGV considerato sul nodo considerato
              // deve compiere un 180°
              //----------------------------------------------------------
              AgvConsiderato_In180=false;
              if(rot1_considerato==0 && rot2_considerato==2) AgvConsiderato_In180=true;
              if(rot1_considerato==1 && rot2_considerato==3) AgvConsiderato_In180=true;
              if(rot1_considerato==2 && rot2_considerato==0) AgvConsiderato_In180=true;
              if(rot1_considerato==3 && rot2_considerato==1) AgvConsiderato_In180=true;
           }
           //--------------------------------------
           // AL-13/03/2014
           // No cambio rotazione ma DEVIAZIONE
           //--------------------------------------
           else{
              //-----------------------------------------------------
              // Il TGV su un nodo di deviazione anche se compilto
              // senza cambio rotazione rispetto al nodo precedente
              // si metterà inclinato, è quindi da considerarsi in
              // Rotazione
              //-----------------------------------------------------
              if(AGV[ NumAgv ]->mission.dati_perc[ptr_percorso].rot>49 && AGV[ NumAgv ]->mission.dati_perc[ptr_percorso].rot<70){
                 //----------------------------------------------------------
                 // Memorizziamo 10..13 per rotazioni in uscita da nodo 0..3
                 //----------------------------------------------------------
               //AgvConsiderato_InRot = rot1+10;
                 //--------------------------------------------------------
                 // AL-07/07/2014 anche se c'è una deviazione, al di
                 // sotto di certi gradi, non si considera una variazione
                 // di inclinazione del TGV
                 //--------------------------------------------------------
                 DistXDev=0;
                 DistYDev=0;
                 x1_aux=x2_aux=0;
                 y1_aux=y2_aux=0;
                 if(PuntoSuccessivo!=0){
                    x1_aux = Punti[ NodoConsiderato ][0];
                    y1_aux = Punti[ NodoConsiderato ][1];
                    x2_aux = Punti[ PuntoSuccessivo ][0];
                    y2_aux = Punti[ PuntoSuccessivo ][1];
                    DistXDev = abs(x1_aux-x2_aux);
                    DistYDev = abs(y1_aux-y2_aux);
                    if(DistXDev>OFFSET_MAX_COORD_TRATTO_INCLINATO && DistXDev>OFFSET_MAX_COORD_TRATTO_INCLINATO){
                       //----------------------------------------------------------
                       // Memorizziamo 10..13 per rotazioni in uscita da nodo 0..3
                       //----------------------------------------------------------
                       AgvConsiderato_InRot = rot1_considerato+10;
                    }
                 }
              }
           }
        }
     }
     //--------------------------------------------------------------------------------
     // Valuta Rotazione ATTUALE AGV e eventaule cambio di rotazione
     //--------------------------------------------------------------------------------
     if((( AgvConsiderato_RotAttuale==0 ) || ( AgvConsiderato_RotAttuale==2 )) && ( AgvConsiderato_InRot==0 )) AgvConsideratoRettaY=1;
     if((( AgvConsiderato_RotAttuale==1 ) || ( AgvConsiderato_RotAttuale==3 )) && ( AgvConsiderato_InRot==0 )) AgvConsideratoRettaX=1;
     //--------------------------------------------------------------------------------
     // Solo per TGV LASER
     // A fine curva viene considerato il TGV già in Retta
     //
     // NOTA: Restano comunque alti i flag di "AgvAdiacenteInRetta" perchè sia sul nodo
     //       di inizio curva che di fine curva il TGV effettivamente è in retta, ma
     //       viene mantenuto AgvAdiacente_InRot per sapere che percorrerà una CURVA
     //--------------------------------------------------------------------------------
     if(AGV[NumAgv]->mission.GestioneLaser==true){
        //---------------------------------------------------
        // TGV IN USCITA CURVA
        //---------------------------------------------------
        ConsideratoInCurva=0;
        AgvConsideratoSuCurvaStretta=0;
        // - NODO INGRESSO CURVA
        if(PuntoSuccessivo!=0){
           if(AGV[ NumAgv ]->mission.dati_perc[ptr_percorso].rot>3 && AGV[ NumAgv ]->mission.dati_perc[ptr_percorso].rot<40){
              //-------------------------------------------------------
              // ConsideratoInCurva:
              // 1 --> IN INGRESSO CURVA
              // 2 --> IN USCITA CURVA
              // 3 --> IN ING/OUT CURVA
              //-------------------------------------------------------
              ConsideratoInCurva = ConsideratoInCurva+1;
              //-------------------------------------------------------
              // GESTIONE CURVA STRETTA
              // Recupero la distanza in X e Y dei punti della curva
              //-------------------------------------------------------
              x1_aux = Punti[ NodoConsiderato ][0];
              y1_aux = Punti[ NodoConsiderato ][1];
              x2_aux = Punti[ PuntoSuccessivo ][0];
              y2_aux = Punti[ PuntoSuccessivo ][1];
              DistXCrv = abs(x1_aux-x2_aux);
              DistYCrv = abs(y1_aux-y2_aux);
              if(DistXCrv<DIST_MIN_PER_CURVA_STRETTA || DistYCrv<DIST_MIN_PER_CURVA_STRETTA){AgvConsideratoSuCurvaStretta=1;}
           }
        }
        // - NODO USCITA CURVA
        if(ptr_percorso>0){
           if(AGV[ NumAgv ]->mission.dati_perc[ptr_percorso-1].rot>3 && AGV[ NumAgv ]->mission.dati_perc[ptr_percorso-1].rot<40){
              if(AgvConsiderato_RotAttuale==0 || AgvConsiderato_RotAttuale==2) AgvConsideratoRettaY=1;
              if(AgvConsiderato_RotAttuale==1 || AgvConsiderato_RotAttuale==3) AgvConsideratoRettaX=1;
              //-------------------------------------------------------
              // ConsideratoInCurva:
              // 1 --> IN INGRESSO CURVA
              // 2 --> IN USCITA CURVA
              // 3 --> IN ING/OUT CURVA
              //-------------------------------------------------------
              ConsideratoInCurva = ConsideratoInCurva+2;
              //-------------------------------------------------------
              // GESTIONE CURVA STRETTA
              // Recupero la distanza in X e Y dei punti della curva
              //-------------------------------------------------------
              x1_aux = Punti[ NodoConsiderato ][0];
              y1_aux = Punti[ NodoConsiderato ][1];
              x2_aux = Punti[ AGV[ NumAgv ]->mission.punto[ptr_percorso-1] ][0];
              y2_aux = Punti[ AGV[ NumAgv ]->mission.punto[ptr_percorso-1] ][1];
              DistXCrv = abs(x1_aux-x2_aux);
              DistYCrv = abs(y1_aux-y2_aux);
              if(DistXCrv<DIST_MIN_PER_CURVA_STRETTA || DistYCrv<DIST_MIN_PER_CURVA_STRETTA){AgvConsideratoSuCurvaStretta=2;}
           }
        }
        //---------------------------------------------------
        // TGV IN ROTAZIONE SU OGNI INIZIO CURVA
        //---------------------------------------------------
        rotazione_in_curva = AGV[ NumAgv ]->mission.dati_perc[ptr_percorso].rot;
        //---------------------------------------------------
        // se non c'è curva sul nodo considerato verifico
        // quello precedente
        //---------------------------------------------------
        if((rotazione_in_curva<4 || rotazione_in_curva>39) || AgvConsideratoSuCurvaStretta==2){
           rotazione_in_curva= AGV[ NumAgv ]->mission.dati_perc[ptr_percorso-1].rot;
        }
        //------------------------------------------------------
        // Se sul punto considerato non c'è una curva controllo
        // sul successivo se è molto vicino
        //------------------------------------------------------
        if((rotazione_in_curva<4 || rotazione_in_curva>39) && AGV[ NumAgv ]->mission.dati_perc[ptr_percorso].dist<=DIST_MIN_ING_CURVA+20){
           if(AGV[ NumAgv ]->mission.dati_perc[ptr_percorso+1].rot>3 && AGV[ NumAgv ]->mission.dati_perc[ptr_percorso+1].rot<40){
              rotazione_in_curva = AGV[ NumAgv ]->mission.dati_perc[ptr_percorso+1].rot;
           }
        }
        //----------------------------------------------------------
        // AL-20/03/2019: un TGV in uscita da una curva e ingresso
        // ad un altra non deve essere considerato in nè in retta
        // X nè in retta Y per rientrare nella ROTAZIONE DOPPIA
        // (es. curvette Bargi)
        //----------------------------------------------------------
        if(ConsideratoInCurva==3){
           AgvConsideratoRettaX=false;
           AgvConsideratoRettaY=false;
        }
        //------------------------------------------------------
        // Memorizziamo 20..23 per curve in uscita da nodo 0..3
        //------------------------------------------------------
        if(rotazione_in_curva>3 && rotazione_in_curva<40){
           switch(rotazione_in_curva){
              case  4: if(DirAgvConsiderato==2){AgvConsiderato_InRot = 20+3;}
                       if(DirAgvConsiderato==1){AgvConsiderato_InRot = 20+1;}
                       break;
              case  5: if(DirAgvConsiderato==2){AgvConsiderato_InRot = 20+2;}
                       if(DirAgvConsiderato==1){AgvConsiderato_InRot = 20+0;}
                       break;
              case  6: if(DirAgvConsiderato==2){AgvConsiderato_InRot = 20+0;}
                       if(DirAgvConsiderato==1){AgvConsiderato_InRot = 20+2;}
                       break;
              case  7: if(DirAgvConsiderato==2){AgvConsiderato_InRot = 20+3;}
                       if(DirAgvConsiderato==1){AgvConsiderato_InRot = 20+1;}
                       break;
              case  8: if(DirAgvConsiderato==2){AgvConsiderato_InRot = 20+2;}
                       if(DirAgvConsiderato==1){AgvConsiderato_InRot = 20+0;}
                       break;
              case  9: if(DirAgvConsiderato==2){AgvConsiderato_InRot = 20+1;}
                       if(DirAgvConsiderato==1){AgvConsiderato_InRot = 20+3;}
                       break;
              case 10: if(DirAgvConsiderato==2){AgvConsiderato_InRot = 20+1;}
                       if(DirAgvConsiderato==1){AgvConsiderato_InRot = 20+3;}
                       break;
              case 11: if(DirAgvConsiderato==2){AgvConsiderato_InRot = 20+0;}
                       if(DirAgvConsiderato==1){AgvConsiderato_InRot = 20+2;}
                       break;
              case 12: if(DirAgvConsiderato==2){AgvConsiderato_InRot = 20+2;}
                       if(DirAgvConsiderato==1){AgvConsiderato_InRot = 20+0;}
                       break;
              case 13: if(DirAgvConsiderato==2){AgvConsiderato_InRot = 20+3;}
                       if(DirAgvConsiderato==1){AgvConsiderato_InRot = 20+1;}
                       break;
              case 15: if(DirAgvConsiderato==2){AgvConsiderato_InRot = 20+0;}
                       if(DirAgvConsiderato==1){AgvConsiderato_InRot = 20+2;}
                       break;
              case 16: if(DirAgvConsiderato==2){AgvConsiderato_InRot = 20+1;}
                       if(DirAgvConsiderato==1){AgvConsiderato_InRot = 20+3;}
                       break;
              case 17: if(DirAgvConsiderato==2){AgvConsiderato_InRot = 20+2;}
                       if(DirAgvConsiderato==1){AgvConsiderato_InRot = 20+0;}
                       break;
              case 18: if(DirAgvConsiderato==2){AgvConsiderato_InRot = 20+0;}
                       if(DirAgvConsiderato==1){AgvConsiderato_InRot = 20+2;}
                       break;
              case 19: if(DirAgvConsiderato==2){AgvConsiderato_InRot = 20+1;}
                       if(DirAgvConsiderato==1){AgvConsiderato_InRot = 20+3;}
                       break;
              case 20: AgvConsiderato_InRot = 20+0;
                       break;
              case 21: AgvConsiderato_InRot = 20+1;
                       break;
              case 22: AgvConsiderato_InRot = 20+2;
                       break;
              case 23: AgvConsiderato_InRot = 20+3;
                       break;
              case 30: AgvConsiderato_InRot = 20+0;
                       break;
              case 31: AgvConsiderato_InRot = 20+1;
                       break;
              case 32: AgvConsiderato_InRot = 20+2;
                       break;
              case 33: AgvConsiderato_InRot = 20+3;
                       break;
           }
        }
     }
  }
  //----------------------------
  // (G3) INIZIO
  //----------------------------
  if( SempreON ){
     satellite_occupato = 0;
     //---------------------------------------------------------
     // (G3) Se il NODO è libero verifica se quelli circostanti
     //      siano altrettanto liberi
     //---------------------------------------------------------
     if(N.nodo_busy[NodoConsiderato]==0 || (RioccupaNodo==true && considerato_fuori_ingombro==false)){
        //-------------------------------------------------------
        // (G3_a) Estrai le coordinate del nodo da occupare (x1,y1)
        //-------------------------------------------------------
        x1 = Punti[ NodoConsiderato ][0];
        y1 = Punti[ NodoConsiderato ][1];
        x2 = 0;
        y2 = 0;
        if(PuntoSuccessivo!=0){
           x2 = Punti[ PuntoSuccessivo ][0];
           y2 = Punti[ PuntoSuccessivo ][1];
        }
        //---------------------------------------------
        // (G3_b) Valuta i nodi adiacenti
        //---------------------------------------------
        for( Vicino=10; Vicino<=TOTPUNTI ; Vicino++){
           if(NodoConsiderato == Vicino                                            ) continue;
           if(N.n[ Vicino ]->num ==0                                               ) continue;
           if(N.pt_in_ingombro[ Vicino ]==false && N.punti_notevoli[ Vicino ]==true) continue;   // Punto terminale non a rischio
           if(N.n[NodoConsiderato]->piano!=N.n[Vicino]->piano                      ) continue;   // x/x/2014 Punto terminale non a rischio perchè su altro Piano
           //--------------------------------------------------------------------------
           // Verifico se il nodo Vicino è su un nodo fuori ingombro
           //--------------------------------------------------------------------------
           vicino_fuori_ingombro=N.verifica_nodo_fuori_ingombro(Vicino);
           //------------------------------------------------------------------------------
           // Estrai le coordinate del nodo VICINO e se DISTANZA > distanza minima
           // --> CONTINUA
           //------------------------------------------------------------------------------
           xv1 = Punti[ Vicino ][0];
           yv1 = Punti[ Vicino ][1];
           CatetoX = DistX  = (double) abs(x1-xv1);
           CatetoY = DistY  = (double) abs(y1-yv1);
           DistX2  = DistY2 = 0;
           if(CatetoX>600 || CatetoY>600) continue;
           CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
           Dist = (int) sqrt_algo( CalcolaRad );
           //--------------------------------------------------
           // Distanza, maggiore rotazione doppia --> continue
           //--------------------------------------------------
           if( Dist>DIST_MIN_ROT_DOPPIA_MAX ) continue;   // nodo sicuramente occupabile
           //------------------------------------------------
           // Altrimenti verifica se nodo SATELLITE OCCUPATO
           //------------------------------------------------
           if(N.nodo_busy[Vicino]!=0 && N.nodo_busy[Vicino]!=NumAgv && N.nodo_busy[Vicino]<=MAXAGV){
              //--------------------------------------------------------------------------
              // Di default --> SATELLITE OCCUPATO
              //--------------------------------------------------------------------------
              if( N.nodo_busy[Vicino] < 1         ) continue;
              if( N.nodo_busy[Vicino] > MAXAGV    ) continue;
              //--------------------------------------------------------------------------
              // Scarto un dato TGV (PER ZONE FUORI INGOMBRO)
              //--------------------------------------------------------------------------
              if( N.nodo_busy[Vicino]==ScartaAgv  ) continue;
              //--------------------------------------------------------------------------
              // Considero solo un DATO TGV
              //--------------------------------------------------------------------------
              if(SoloAgv>0 && SoloAgv<=MAXAGV){
                 if( N.nodo_busy[Vicino]!=SoloAgv ) continue;
              }
              //--------------------------------------------------------------------------
              // Setta Vicino come satellite_occupato e agv_adiacente
              //--------------------------------------------------------------------------
              satellite_occupato = Vicino;
              agv_adiacente      = N.nodo_busy[Vicino];
              //--------------------------------------------------------------------------
              // Se il TGV sul nodo interessato non ha la missione in corso
              //--------------------------------------------------------------------------
              // (G3_c)Controlla AGV ADIACENTE
              //--------------------------------------------------------------------------
              if(SempreON ){
                 //----------------------------------------
                 // Valuta percorso AGV ADIACENTE
                 //----------------------------------------
                 AgvAdiacenteInRettaX      = 0;
                 AgvAdiacenteInRettaY      = 0;
                 AgvAdiacente_InRot        = 0;
                 AgvAdiacente_ptr_percorso = 0;
                 DirAgvAdiacente           = 0;
                 for(j=0;j<MAXPERCORSI;j++){
                    if( Vicino==AGV[ agv_adiacente ]->mission.punto[j]){
                       AgvAdiacente_ptr_percorso=j;
                       break;
                    }
                 }
                 if(AgvAdiacente_ptr_percorso >=MAXPERCORSI ) continue;
                 //--------------------------------------------------------
                 // Memorizzo direzione modiviemento TGV ADIACENTE
                 //--------------------------------------------------------
                 if(AGV[ agv_adiacente ]->mission.pstart!=0){
                    if(AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso].dir) DirAgvAdiacente=1;    // Indietro
                    else                                                                       DirAgvAdiacente=2;    // Avanti
                 }
                 //-------------------------------------------------------
                 // Memorizzo un cambio direzione di movimento
                 //-------------------------------------------------------
                 CambioDirAdiacente=false;
                 if(AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso].dir!=AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso-1].dir){
                    CambioDirAdiacente=true;
                 }
                 //--------------------------------------------------------
                 // Recupero coordinate del punto successivo a quello
                 // vicino
                 //--------------------------------------------------------
                 xv2 = 0;
                 yv2 = 0;
                 AgvAdicentePuntoSuccessivo = AGV[ agv_adiacente ]->mission.punto[AgvAdiacente_ptr_percorso+1];
                 if(AgvAdicentePuntoSuccessivo>0){
                    xv2 = Punti[ AgvAdicentePuntoSuccessivo ][0];
                    yv2 = Punti[ AgvAdicentePuntoSuccessivo ][1];
                 }
                 //--------------------------------------------------------
                 // Recupero rotazioni TGV adicente
                 //--------------------------------------------------------
                 rot1_adiacente = AGV[ agv_adiacente ]->rot_standard(AgvAdiacente_ptr_percorso);
                 if(AgvAdiacente_ptr_percorso>0) rot2_adiacente = AGV[ agv_adiacente ]->rot_standard(AgvAdiacente_ptr_percorso-1);
                 else                            rot2_adiacente = rot1_adiacente;
                 //--------------------------------------------------------
                 // Se AGV senza missione valuta la sua rotazione attuale
                 //--------------------------------------------------------
                 AgvAdiacentaGiaRuotato=false;
                 if( AGV[ agv_adiacente ]->mission.pstart==0 ){
                    if( AGV[ agv_adiacente ]->stato.s.bit.grd0   ) rot1_adiacente = 0;
                    if( AGV[ agv_adiacente ]->stato.s.bit.grd90  ) rot1_adiacente = 1;
                    if( AGV[ agv_adiacente ]->stato.s.bit.grd180 ) rot1_adiacente = 2;
                    if( AGV[ agv_adiacente ]->stato.s.bit.grd270 ) rot1_adiacente = 3;
                    if( Vicino==AGV[ agv_adiacente ]->stato.pos  ) AgvAdiacentaGiaRuotato=true;
                    else AgvAdiacente_InRot=AgvAdiacente_RotAttuale+10; // Sui nodi al di fuori di dove si
                                                                        // trova il TGV ADIACENTE setto
                                                                        // InRot per manetenerlo a distanza
                 }
                 //--------------------------------------------------------
                 // 22/06/2011 Anche se il TGV ha inizializzato la missione
                 //            ma non si muove perchè il pint conicide con
                 //            quello di partenza lo considero già ruotato
                 //--------------------------------------------------------
                 if( AGV[ agv_adiacente ]->mission.pstart>0 && AGV[ agv_adiacente ]->mission.pstart==AGV[ agv_adiacente ]->mission.pintend){
                    if( AGV[ agv_adiacente ]->stato.s.bit.grd0   ){ rot1_adiacente = 0; AgvAdiacentaGiaRuotato=true;}
                    if( AGV[ agv_adiacente ]->stato.s.bit.grd90  ){ rot1_adiacente = 1; AgvAdiacentaGiaRuotato=true;}
                    if( AGV[ agv_adiacente ]->stato.s.bit.grd180 ){ rot1_adiacente = 2; AgvAdiacentaGiaRuotato=true;}
                    if( AGV[ agv_adiacente ]->stato.s.bit.grd270 ){ rot1_adiacente = 3; AgvAdiacentaGiaRuotato=true;}
                 }
                 //-------------------------------------------------------
                 // Rotazione attuale TGV adiacente
                 //--------------------------------------------------------
                 AgvAdiacente_RotAttuale = rot1_adiacente;             // Rotazione attuale
                 //--------------------------------------------------------
                 // Se AGV con missione valuta se deve ruotare
                 //
                 // 22/06/2011 modificato controllo su TGV in movimento
                 //            vedi controllo precedente
                 //--------------------------------------------------------
               //if( AGV[ agv_adiacente ]->mission.pstart>0){
                 if( AGV[ agv_adiacente ]->mission.pstart>0 && AGV[ agv_adiacente ]->mission.pstart!=AGV[ agv_adiacente ]->mission.pintend){
                    //------------------------------------------------------------------
                    // Se l'agv_adiacente si trova già sul nodo Vicino verifico se ha
                    // a già eseguito la rotazione o se è ancora in fase di rotazione
                    //------------------------------------------------------------------
                    if( AGV[ agv_adiacente ]->stato.pos==Vicino ){
                       if(AgvAdiacente_RotAttuale==0 && AGV[ agv_adiacente ]->stato.s.bit.grd0   ) AgvAdiacentaGiaRuotato=true;
                       if(AgvAdiacente_RotAttuale==1 && AGV[ agv_adiacente ]->stato.s.bit.grd90  ) AgvAdiacentaGiaRuotato=true;
                       if(AgvAdiacente_RotAttuale==2 && AGV[ agv_adiacente ]->stato.s.bit.grd180 ) AgvAdiacentaGiaRuotato=true;
                       if(AgvAdiacente_RotAttuale==3 && AGV[ agv_adiacente ]->stato.s.bit.grd270 ) AgvAdiacentaGiaRuotato=true;
                    }
                    if( AgvAdiacente_ptr_percorso>0){
                       //------------------------------------------------------------------
                       // se l'agv_adiacente è in prossimità di una rotazione setto
                       // un flag AgvAdiacente_InRot
                       //------------------------------------------------------------------
                       if( AgvAdiacentaGiaRuotato==false ){
                          //-------------------------------------------------------
                          // Con missione in corso AgvAdiacentaGiaRuotato=true
                          // tranne che in caso di rotazione ortogonale
                          // Curve escluse perchè sul nodo iniziale e finale il
                          // TGV è già ruotato
                          //-------------------------------------------------------
                          AgvAdiacentaGiaRuotato=true;
                          //-------------------------------------------------------
                          // Cambio rotazione
                          //
                          // AL-06/02/2014 VALIDO SOLO PER LE ROTAZIONI/DEVIAZIONI
                          // per le curve è stato identificiato in rotazione con
                          // +20
                          //-------------------------------------------------------
                          if((AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso  ].rot<4 || AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso  ].rot>39) &&
                             (AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso-1].rot<4 || AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso-1].rot>39)){
                             if(rot1_adiacente!=rot2_adiacente){
                                //AgvAdiacente_InRot = 1;
                                //----------------------------------------------------------
                                // Memorizziamo 10..13 per rotazioni in uscita da nodo 0..3
                                //----------------------------------------------------------
                                AgvAdiacente_InRot = rot1_adiacente+10;
                                //----------------------------------------------------------
                                // Memorizzo se il TGV considerato sul nodo considerato
                                // deve compiere un 180°
                                //----------------------------------------------------------
                                AgvAdiacente_In180=false;
                                if(rot1_adiacente==0 && rot2_adiacente==2) AgvAdiacente_In180=true;
                                if(rot1_adiacente==1 && rot2_adiacente==3) AgvAdiacente_In180=true;
                                if(rot1_adiacente==2 && rot2_adiacente==0) AgvAdiacente_In180=true;
                                if(rot1_adiacente==3 && rot2_adiacente==1) AgvAdiacente_In180=true;
                                //----------------------------------------------------------
                                // Con rotazione o deviazione SICURAMENTE AGV NON RUOTATO
                                //----------------------------------------------------------
                                AgvAdiacentaGiaRuotato=false;
                             }
                             //--------------------------------------
                             // AL-13/03/2014
                             // No cambio rotazione ma deviazione
                             //--------------------------------------
                             else{
                                //-----------------------------------------------------
                                // Il TGV su un nodo di deviazione anche se compilto
                                // senza cambio rotazione rispetto al nodo precedente
                                // si metterà inclinato, è quindi da considerarsi in
                                // Rotazione
                                //-----------------------------------------------------
                                if(AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso].rot>49 && AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso].rot<70){
                                   //--------------------------------------------------------
                                   // AL-07/07/2014 anche se c'è una deviazione, al di
                                   // sotto di certi gradi, non si considera una variazione
                                   // di inclinazione del TGV
                                   //--------------------------------------------------------
                                   DistXDev=0;
                                   DistYDev=0;
                                   x1_aux=x2_aux=0;
                                   y1_aux=y2_aux=0;
                                   if(AgvAdicentePuntoSuccessivo!=0){
                                      x1_aux   = Punti[ Vicino ][0];
                                      y1_aux   = Punti[ Vicino ][1];
                                      x2_aux   = Punti[ AgvAdicentePuntoSuccessivo ][0];
                                      y2_aux   = Punti[ AgvAdicentePuntoSuccessivo ][1];
                                      DistXDev = abs(x1_aux-x2_aux);
                                      DistYDev = abs(y1_aux-y2_aux);
                                      if(DistXDev>OFFSET_MAX_COORD_TRATTO_INCLINATO && DistYDev>OFFSET_MAX_COORD_TRATTO_INCLINATO){
                                         //----------------------------------------------------------
                                         // Memorizziamo 10..13 per rotazioni in uscita da nodo 0..3
                                         //----------------------------------------------------------
                                         AgvAdiacente_InRot = rot1_adiacente+10;
                                         //----------------------------------------------------------
                                         // Con rotazione o deviazione SICURAMENTE AGV NON RUOTATO
                                         //----------------------------------------------------------
                                         AgvAdiacentaGiaRuotato=false;
                                      }
                                   }
                                }
                             }
                          }
                       }
                    }
                    //----------------------------------------------------------------------------------------------------------------
                    // 05/07/2016 - GB/SC In caso in cui l'AGV adiacente parta dal primo nodo di percorso valuto se deve effettuare
                    // la rotazione in base alla stato del tgv rispetto alla rotazione da fare sul primo nodo
                    // (senza questo controllo non veniva considerata la rotazione di partenza per esempio nel caso la navetta venisse
                    //  fatta ripartire da un nodo di percorso)
                    //----------------------------------------------------------------------------------------------------------------
                    else{
                       if(AgvAdiacentaGiaRuotato==false){
                          if(AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso  ].rot<4){
                             AgvAdiacente_InRot = rot1_adiacente+10;
                          }
                       }
                    }
                 }
                 //--------------------------------------------------------------------------------
                 // Valuta Rotazione ATTUALE AGV e eventaule cambio di rotazione
                 //--------------------------------------------------------------------------------
                 if((( AgvAdiacente_RotAttuale==0 ) || ( AgvAdiacente_RotAttuale==2 )) && ( AgvAdiacente_InRot==0 )) AgvAdiacenteInRettaY=1;
                 if((( AgvAdiacente_RotAttuale==1 ) || ( AgvAdiacente_RotAttuale==3 )) && ( AgvAdiacente_InRot==0 )) AgvAdiacenteInRettaX=1;
                 //----------------------------------------------------------------------------------------
                 // Solo per TGV LASER
                 // A fine curva viene considerato il TGV già in Retta
                 //
                 // NOTA: Restano comunque alti i flag di "AgvAdiacentaGiaRuotato" e "AgvAdiacenteInRetta"
                 //       perchè sia sul nodo di inizio curva che di fine curva il TGV effettivamente è
                 //       in retta, ma viene mantenuto AgvAdiacente_InRot per sapere che percorrerà una
                 //       CURVA
                 //----------------------------------------------------------------------------------------
                 if(AGV[agv_adiacente]->mission.GestioneLaser==true){
                    //---------------------------------------------------
                    // TGV IN USCITA CURVA
                    //---------------------------------------------------
                    AdiacenteInCurva=0;
                    AgvAdiacenteSuCurvaStretta=0;
                    // - NODO INGRESSO CURVA
                    if(AgvAdicentePuntoSuccessivo!=0){
                       if(AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso].rot>3 && AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso].rot<40){
                          //-------------------------------------------------------
                          // AdiacenteInCurva:
                          // 1 --> IN INGRESSO CURVA
                          // 2 --> IN USCITA CURVA
                          // 3 --> IN ING/OUT CURVA
                          //-------------------------------------------------------
                          AdiacenteInCurva = AdiacenteInCurva+1;
                          //-------------------------------------------------------
                          // GESTIONE CURVA STRETTA
                          // Recupero la distanza in X e Y dei punti della curva
                          //-------------------------------------------------------
                          x1_aux = Punti[ Vicino ][0];
                          y1_aux = Punti[ Vicino ][1];
                          x2_aux = Punti[ AgvAdicentePuntoSuccessivo ][0];
                          y2_aux = Punti[ AgvAdicentePuntoSuccessivo ][1];
                          DistXCrv = abs(x1_aux-x2_aux);
                          DistYCrv = abs(y1_aux-y2_aux);
                          if(DistXCrv<DIST_MIN_PER_CURVA_STRETTA || DistYCrv<DIST_MIN_PER_CURVA_STRETTA){AgvAdiacenteSuCurvaStretta=1;}
                       }
                    }
                    // - NODO USCITA CURVA
                    if(AgvAdiacente_ptr_percorso>0){
                       if(AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso-1].rot>3 && AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso-1].rot<40){
                          if(AgvAdiacente_RotAttuale==0 || AgvAdiacente_RotAttuale==2) AgvAdiacenteInRettaY=1;
                          if(AgvAdiacente_RotAttuale==1 || AgvAdiacente_RotAttuale==3) AgvAdiacenteInRettaX=1;
                          //-------------------------------------------------------
                          // AdiacenteInCurva:
                          // 1 --> IN INGRESSO CURVA
                          // 2 --> IN USCITA CURVA
                          // 3 --> IN ING/OUT CURVA
                          //-------------------------------------------------------
                          AdiacenteInCurva = AdiacenteInCurva+2;
                          //-------------------------------------------------------
                          // GESTIONE CURVA STRETTA
                          // Recupero la distanza in X e Y dei punti della curva
                          //-------------------------------------------------------
                          x1_aux = Punti[ Vicino ][0];
                          y1_aux = Punti[ Vicino ][1];
                          x2_aux = Punti[ AGV[ agv_adiacente ]->mission.punto[AgvAdiacente_ptr_percorso-1] ][0];
                          y2_aux = Punti[ AGV[ agv_adiacente ]->mission.punto[AgvAdiacente_ptr_percorso-1] ][1];
                          DistXCrv = abs(x1_aux-x2_aux);
                          DistYCrv = abs(y1_aux-y2_aux);
                          if(DistXCrv<DIST_MIN_PER_CURVA_STRETTA || DistYCrv<DIST_MIN_PER_CURVA_STRETTA) AgvAdiacenteSuCurvaStretta=2;
                       }
                    }
                    //---------------------------------------------------
                    // TGV IN ROTAZIONE SU OGNI INIZIO CURVA
                    //---------------------------------------------------
                    rotazione_in_curva = AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso].rot;
                    //---------------------------------------------------
                    // se non c'è curva sul nodo considerato verifico
                    // quello precedente
                    //---------------------------------------------------
                    if((rotazione_in_curva<4 || rotazione_in_curva>39) || AgvAdiacenteSuCurvaStretta==2){
                       rotazione_in_curva= AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso-1].rot;
                    }
                    //---------------------------------------------------
                    // Se sul punto considerato non c'è una curva
                    // controllo sul successivo se è molto vicino
                    //---------------------------------------------------
                    if((rotazione_in_curva<4 || rotazione_in_curva>39) && AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso].dist<=DIST_MIN_ING_CURVA+20){
                       if(AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso+1].rot>3 && AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso+1].rot<40){
                          rotazione_in_curva = AGV[ agv_adiacente ]->mission.dati_perc[AgvAdiacente_ptr_percorso+1].rot;
                       }
                    }
                    //------------------------------------------------------
                    // Memorizziamo 20..23 per curve in uscita da nodo 0..3
                    //------------------------------------------------------
                    if(rotazione_in_curva>3 && rotazione_in_curva<40){
                       //----------------------------------------------------------
                       // Con rotazione o deviazione SICURAMENTE AGV NON RUOTATO
                       //----------------------------------------------------------
                       AgvAdiacentaGiaRuotato=false;
                       //-------------------------------------------------------------
                       // AL-31/01/2017
                       // Se il TGV è già fermo su suo PINT (non occuparà altri nodi)
                       // e ha già acquisito la rotazione finale prevista sul nodo
                       // viene considerato "GiaRuotato" in tutti gli altri carci NO
                       //-------------------------------------------------------------
                       if(AGV[ agv_adiacente ]->stato.pos==AGV[ agv_adiacente ]->mission.pintend){
                          if(AgvAdiacente_RotAttuale==0 && AGV[ agv_adiacente ]->stato.s.bit.grd0   ) AgvAdiacentaGiaRuotato=true;
                          if(AgvAdiacente_RotAttuale==1 && AGV[ agv_adiacente ]->stato.s.bit.grd90  ) AgvAdiacentaGiaRuotato=true;
                          if(AgvAdiacente_RotAttuale==2 && AGV[ agv_adiacente ]->stato.s.bit.grd180 ) AgvAdiacentaGiaRuotato=true;
                          if(AgvAdiacente_RotAttuale==3 && AGV[ agv_adiacente ]->stato.s.bit.grd270 ) AgvAdiacentaGiaRuotato=true;
                       }
                       //----------------------------------------------------------
                       // AL-20/03/2019: un TGV in uscita da una curva e ingresso
                       // ad un altra non deve essere considerato in nè in retta
                       // X nè in retta Y per rientrare nella ROTAZIONE DOPPIA
                       // (es. curvette Bargi)
                       //----------------------------------------------------------
                       if(AdiacenteInCurva==3){
                          AgvAdiacenteInRettaX=false;
                          AgvAdiacenteInRettaY=false;
                          AgvAdiacentaGiaRuotato=false;
                       }
                       //----------------------------------------------------------
                       // Verifica tipo di curva in corso
                       //----------------------------------------------------------
                       switch(rotazione_in_curva){
                          case  4: if(DirAgvAdiacente==2){AgvAdiacente_InRot = 20+3;}
                                   if(DirAgvAdiacente==1){AgvAdiacente_InRot = 20+1;}
                                   break;
                          case  5: if(DirAgvAdiacente==2){AgvAdiacente_InRot = 20+2;}
                                   if(DirAgvAdiacente==1){AgvAdiacente_InRot = 20+0;}
                                   break;
                          case  6: if(DirAgvAdiacente==2){AgvAdiacente_InRot = 20+0;}
                                   if(DirAgvAdiacente==1){AgvAdiacente_InRot = 20+2;}
                                   break;
                          case  7: if(DirAgvAdiacente==2){AgvAdiacente_InRot = 20+3;}
                                   if(DirAgvAdiacente==1){AgvAdiacente_InRot = 20+1;}
                                   break;
                          case  8: if(DirAgvAdiacente==2){AgvAdiacente_InRot = 20+2;}
                                   if(DirAgvAdiacente==1){AgvAdiacente_InRot = 20+0;}
                                   break;
                          case  9: if(DirAgvAdiacente==2){AgvAdiacente_InRot = 20+1;}
                                   if(DirAgvAdiacente==1){AgvAdiacente_InRot = 20+3;}
                                   break;
                          case 10: if(DirAgvAdiacente==2){AgvAdiacente_InRot = 20+1;}
                                   if(DirAgvAdiacente==1){AgvAdiacente_InRot = 20+3;}
                                   break;
                          case 11: if(DirAgvAdiacente==2){AgvAdiacente_InRot = 20+0;}
                                   if(DirAgvAdiacente==1){AgvAdiacente_InRot = 20+2;}
                                   break;
                          case 12: if(DirAgvAdiacente==2){AgvAdiacente_InRot = 20+2;}
                                   if(DirAgvAdiacente==1){AgvAdiacente_InRot = 20+0;}
                                   break;
                          case 13: if(DirAgvAdiacente==2){AgvAdiacente_InRot = 20+3;}
                                   if(DirAgvAdiacente==1){AgvAdiacente_InRot = 20+1;}
                                   break;
                          case 15: if(DirAgvAdiacente==2){AgvAdiacente_InRot = 20+0;}
                                   if(DirAgvAdiacente==1){AgvAdiacente_InRot = 20+2;}
                                   break;
                          case 16: if(DirAgvAdiacente==2){AgvAdiacente_InRot = 20+1;}
                                   if(DirAgvAdiacente==1){AgvAdiacente_InRot = 20+3;}
                                   break;
                          case 17: if(DirAgvAdiacente==2){AgvAdiacente_InRot = 20+2;}
                                   if(DirAgvAdiacente==1){AgvAdiacente_InRot = 20+0;}
                                   break;
                          case 18: if(DirAgvAdiacente==2){AgvAdiacente_InRot = 20+0;}
                                   if(DirAgvAdiacente==1){AgvAdiacente_InRot = 20+2;}
                                   break;
                          case 19: if(DirAgvAdiacente==2){AgvAdiacente_InRot = 20+1;}
                                   if(DirAgvAdiacente==1){AgvAdiacente_InRot = 20+3;}
                                   break;
                          case 20: AgvAdiacente_InRot = 20+0;
                                   break;
                          case 21: AgvAdiacente_InRot = 20+1;
                                   break;
                          case 22: AgvAdiacente_InRot = 20+2;
                                   break;
                          case 23: AgvAdiacente_InRot = 20+3;
                                   break;
                          case 30: AgvAdiacente_InRot = 20+0;
                                   break;
                          case 31: AgvAdiacente_InRot = 20+1;
                                   break;
                          case 32: AgvAdiacente_InRot = 20+2;
                                   break;
                          case 33: AgvAdiacente_InRot = 20+3;
                                   break;
                       }
                    }
                 }
                 //---------------------------------------------------------------
                 // PUNTI TERMINALI IN INGOMBRO
                 // 1 - Se l'AGV adicente si trova su un PT non posso considerare
                 //     le navette in inseguimento per garantire al TGV che deve
                 //     uscire dal PT, una distanza di rotazione sufficente per
                 //     andarsene
                 // 2 - Ricalcolo delle DISTX, DISTY e DIST in base alla rotazione
                 //     e all'offset da nodo.
                 //---------------------------------------------------------------
                 //if( AgvAdiacentaGiaRuotato==true && N.punti_notevoli[ Vicino ]==true){
                 if(AgvAdiacentaGiaRuotato==true && AGV[agv_adiacente]->stato.pos==Vicino && !AGV[agv_adiacente]->stato.s.bit.sunod && N.punti_notevoli[Vicino]==true){
                    //-------------------------------------------------------------
                    // Recupero le nuove distanze in X o Y a seconda della
                    // rotazione TGV
                    //
                    // N.B. Viene sommato l'offset solo se il nodo prevede una
                    //      direzione di carico/scarico
                    //-------------------------------------------------------------
                    offset_da_nodo = 0;
                    if(N.n[Vicino]->direz!=0) offset_da_nodo = AGV[agv_adiacente]->stato.offset_da_nodo;
                    if(AgvAdiacente_RotAttuale==0 || AgvAdiacente_RotAttuale==2){
                       DistY   = DistY + (offset_da_nodo * AGV[agv_adiacente]->mission.RapEncoder);
                       CatetoY = abs(DistY);
                    }
                    if(AgvAdiacente_RotAttuale==1 || AgvAdiacente_RotAttuale==3){
                       DistX   = DistX + (offset_da_nodo * AGV[agv_adiacente]->mission.RapEncoder);
                       CatetoX = abs(DistX);
                    }
                    //------------------------------------------
                    // Ricalcolo della distanza
                    //------------------------------------------
                    if(CatetoX<600 && CatetoY<600){
                       CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
                       Dist = (int) sqrt_algo( CalcolaRad );
                    }
                 }
                 //-----------------------------------------------------
                 // Verifico se le navette si stanno inseguento
                 //-----------------------------------------------------
                 SenzaMaiIncontrarsi         = false;
                 AgvInIseguimento            = true;
                 AgvInAllontanamento         = false;
                 RotOpposteInX=RotOpposteInY = false;
                 DirAllontanamento           = 0;
                 //---------------------------------------------------------------
                 // Recupero se sono rotazioni opposte
                 //---------------------------------------------------------------
                 rot1 = AGV[ NumAgv        ]->rot_standard(ptr_percorso);
                 rot2 = AGV[ agv_adiacente ]->rot_standard(AgvAdiacente_ptr_percorso);
                 if(rot1==0 && rot2==2) RotOpposteInY=true;
                 if(rot1==1 && rot2==3) RotOpposteInX=true;
                 if(rot1==2 && rot2==0) RotOpposteInY=true;
                 if(rot1==3 && rot2==1) RotOpposteInX=true;
                 //---------------------------------------------------------------
                 // CONDIZIONI DI ALLONTANAMENTO (CON TGV FERMO SU PT_IN_INGOMBRO)
                 // Se il TGV adiacente è fermo senza missione, le 2 navette
                 // vengono considerate in allontanamento se la distanza di
                 // tutti i punti di percorso del TGV considerato è
                 // >=DIST_MIN_ALLONTANAMENTO dal Vicino considerato.
                 //---------------------------------------------------------------
                 if(PuntoSuccessivo>0 && AgvAdicentePuntoSuccessivo==0 && Vicino==AGV[agv_adiacente]->stato.pos){
                    // (1) I 2 TGV non si incontrano
                    SenzaMaiIncontrarsi=true;
                    x1_aux=x2_aux=0;
                    y1_aux=y2_aux=0;
                    Dist_aux=DistX_aux=DistY_aux=0;
                    for(i=1; i<MAXPERCORSI; i++){  // !!!IMPORTANTE: scarto indice "0" perchè è già dove si trova il tGV e quindi già occupato
                       if(AGV[NumAgv]->mission.punto_prenotato[i+1]==0) break;
                       x1_aux = Punti[ AGV[NumAgv]->mission.punto_prenotato[i] ][0];
                       y1_aux = Punti[ AGV[NumAgv]->mission.punto_prenotato[i] ][1];
                       x2_aux     = Punti[ Vicino ][0];
                       y2_aux     = Punti[ Vicino ][1];
                       DistX_aux  = (int) abs(x1_aux-x2_aux);
                       DistY_aux  = (int) abs(y1_aux-y2_aux);
                       Dist_aux   = (int) sqrt_algo((double)(DistX_aux*DistX_aux) + (double)(DistY_aux*DistY_aux));
                       //-------------------------------------------------------------
                       // Tutti i nodi sono a una distanza sufficente di allontanam.
                       //-------------------------------------------------------------
                       if(Dist_aux>=DIST_MIN_ALLONTANAMENTO) continue;
                       SenzaMaiIncontrarsi=false;
                       break;
                    }
                 }
                 //---------------------------------------------------------------
                 // CONDIZIONI DI ALLONTANAMENTO (CON TGV ADIACENTE IN MISSIONE)
                 // Tgv considerato e adiacente con rotazioni opposte sul
                 // NodoConsiderato e sul nodo Vicino e le distanze che aumentano
                 //---------------------------------------------------------------
                 if(PuntoSuccessivo>0 && AgvAdicentePuntoSuccessivo>0){
                    DistX2 = (double) abs(x2-xv2);
                    DistY2 = (double) abs(y2-yv2);
                    //---------------------------------------------------------------
                    // CONDIZIONI DI ALLONTANAMENTO CERTO:
                    // 1. Il TGV non incontra mai l'adiacente nella sua missione.
                    // 2. I 2 TGV hanno stessa direzione, ma rotazioni opposte.
                    //    e la distanza tra i due deve aumentare.
                    // 3. Nel punto considerato e nel Vicino i 2 TGV hanno rotazioni
                    //    diverse, e la dist tra i 2 nel nodo successivo aumenta sia
                    //    in X che in Y.
                    //---------------------------------------------------------------
                    // (1) I 2 TGV non si incontrano
                    SenzaMaiIncontrarsi=true;
                    for(i=1; i<MAXPERCORSI; i++){  // !!!IMPORTANTE: scarto indice "0" perchè è già dove si trova il tGV e quindi già occupato
                       if(AGV[NumAgv]->mission.punto_prenotato[i]==0) break;
                       for(j=0; j<MAXPERCORSI; j++){
                          if(AGV[agv_adiacente]->mission.punto_prenotato[j]==0                                      ) break;
                          if(AGV[agv_adiacente]->mission.punto_prenotato[j]!=AGV[NumAgv]->mission.punto_prenotato[i]) continue;
                          SenzaMaiIncontrarsi=false;
                          break;
                       }
                       if(SenzaMaiIncontrarsi==false) break;
                    }
                    //---------------------------------------------------------------
                    // GESTIONE LASER
                    // Se i TGV apparentemente non si incontrano mai, ma tra i
                    // prenotati in alcuni punti c'è una distanza molto bassa
                    // in realtà c'è un probabile salto nodi e i TGV SI INCONTRANO
                    //---------------------------------------------------------------
                    if(SenzaMaiIncontrarsi==true && AGV[NumAgv]->mission.GestioneLaser==true){
                       x1_aux=x2_aux=0;
                       y1_aux=y2_aux=0;
                       Dist_aux=DistX_aux=DistY_aux=0;
                       for(i=1; i<MAXPERCORSI; i++){  // !!!IMPORTANTE: scarto indice "0" perchè è già dove si trova il tGV e quindi già occupato
                          if(AGV[NumAgv]->mission.punto_prenotato[i]==0) break;
                          x1_aux = Punti[ AGV[NumAgv]->mission.punto_prenotato[i] ][0];
                          y1_aux = Punti[ AGV[NumAgv]->mission.punto_prenotato[i] ][1];
                          for(j=0; j<MAXPERCORSI; j++){
                             if(AGV[agv_adiacente]->mission.punto_prenotato[j]==0) break;
                             x2_aux     = Punti[ AGV[agv_adiacente]->mission.punto_prenotato[j] ][0];
                             y2_aux     = Punti[ AGV[agv_adiacente]->mission.punto_prenotato[j] ][1];
                             DistX_aux  = (int) abs(x1_aux-x2_aux);
                             DistY_aux  = (int) abs(y1_aux-y2_aux);
                             Dist_aux   = (int) sqrt_algo((double)(DistX_aux*DistX_aux) + (double)(DistY_aux*DistY_aux));
                             //-------------------------------------------------------------
                             // E' stata scelta la DIST_MIN_ING_CURVA+20 il riferimento per
                             // fissare un probabile salto nodi nei LASER
                             //-------------------------------------------------------------
                             if(Dist_aux>=DIST_MIN_ING_CURVA+20) continue;
                             //-------------------------------------------------------------
                             // TGV si incontrano.
                             //-------------------------------------------------------------
                             SenzaMaiIncontrarsi=false;
                             break;
                          }
                          if(SenzaMaiIncontrarsi==false) break;
                       }
                    }
                    //---------------------------------------------------------------
                    // AL-05/02/2014
                    // Due tgv che no si incortano mai perchè non hanno punti
                    // prenotati in comune, devono essere considerati in ingombro
                    // nei punti di rotazione se in un intorno DIST_MIN_ROT_SINGOLA
                    // c'è un nodo occupato da un altro TGV.
                    //
                    // IMPORTANTE:
                    // Controllare importante DIST_MIN_ROT_SINGOLA xchè
                    // la gestione dell'allontanamante prevede già che al max una
                    // navetta sia in rotazione
                    //---------------------------------------------------------------
                    if(SenzaMaiIncontrarsi==true){
                       for(i=ptr_percorso; i<MAXPERCORSI; i++){
                          if(AGV[NumAgv]->mission.punto[i]==0) break;
                          rot_in_corso=false;
                          rot1 = AGV[NumAgv]->mission.dati_perc[i].rot;
                          rot2 = rot1; // inizializzo le stesse rotazioni
                          //----------------------------------------------------
                          // Se TGV su start curva o devizione --> rot_in_corso
                          //----------------------------------------------------
                          if(rot1>3) rot_in_corso=true;
                          //----------------------------------------------------
                          // Se curva a magneti: verifico variazione rot
                          //----------------------------------------------------
                          else{
                             if(ptr_percorso>0) rot2 = AGV[NumAgv]->mission.dati_perc[i-1].rot;
                             if(rot1!=rot2    ) rot_in_corso=true;
                          }
                          //----------------------------------------------------
                          // Sui punti di rotazione controllo di non avere un
                          // modo occupato in un intorno DIST_MIN_ROT_SINGOLA
                          //----------------------------------------------------
                          if(rot_in_corso==true){
                             x1_aux = Punti[ AGV[NumAgv]->mission.punto[i] ][0];
                             y1_aux = Punti[ AGV[NumAgv]->mission.punto[i] ][1];
                             for(j=0; j<MAXPERCORSI; j++){
                                if(AGV[agv_adiacente]->mission.punto_prenotato[j]==0) break;
                                //----------------------------------------------------------
                                // !!! Solo nodi occupati: da verificare !!!
                                //----------------------------------------------------------
                                if(N.nodo_busy[AGV[agv_adiacente]->mission.punto_prenotato[j]]==0       ) continue;
                                if(N.nodo_busy[AGV[agv_adiacente]->mission.punto_prenotato[j]]==NumAgv  ) continue;
                                //----------------------------------------------------------
                                // Recupero distanza tra i nodi
                                //----------------------------------------------------------
                                x2_aux     = Punti[ AGV[agv_adiacente]->mission.punto_prenotato[j] ][0];
                                y2_aux     = Punti[ AGV[agv_adiacente]->mission.punto_prenotato[j] ][1];
                                DistX_aux  = (int) abs(x1_aux-x2_aux);
                                DistY_aux  = (int) abs(y1_aux-y2_aux);
                                Dist_aux   = (int) sqrt_algo((double)(DistX_aux*DistX_aux) + (double)(DistY_aux*DistY_aux));
                                //-------------------------------------------------------------
                                // !!! Dist>DIST_MIN_ROT_SINGOLA !!!
                                //-------------------------------------------------------------
                                if(Dist_aux>=DIST_MIN_ROT_SINGOLA) continue;
                                //-------------------------------------------------------------
                                // TGV si incontrano.
                                //-------------------------------------------------------------
                                SenzaMaiIncontrarsi=false;
                                break;
                             }
                             if(SenzaMaiIncontrarsi==false) break;
                          }
                       }
                    }
                    //---------------------------------------------------------------
                    // Se TGV CONSIDERATO e ADIACENTE devono compiere una curva
                    // dei rispettivi NodoConsiderato e Vicino abbasso di default
                    // il flag SenzaMaiIncontrarsi
                    //---------------------------------------------------------------
                    if(AgvConsiderato_InRot>19 && AgvAdiacente_InRot>19) SenzaMaiIncontrarsi=false;
                    //---------------------------------------------------------------
                    // CONDIZIONI DI ALLONTANAMENTO:
                    // (1) - I TGV non si incontrano MAI
                    // (2) - I TGV aumentano la distanza tra loro con rotazioni
                    //       opposte e stessa dirazione <--1 2-->
                    //---------------------------------------------------------------
                    // (1)
                    if(SenzaMaiIncontrarsi==true) AgvInAllontanamento=true;
                    // (2) I 2 TGV in stessa direzione, ma rotazioni opposte su tracce differenti
                    if(RotOpposteInX || RotOpposteInY){
                       if(RotOpposteInX==true){
                          if(DistX2>DistX && DirAgvConsiderato!=0 && DirAgvAdiacente!=0 && DirAgvConsiderato==DirAgvAdiacente) AgvInAllontanamento=true;
                          else{
                             if(DistY<DIST_MIN_RETTE_PARALLELE) AgvInAllontanamento=false;
                          }
                       }
                       if(RotOpposteInY==true){
                          if(DistY2>DistY && DirAgvConsiderato!=0 && DirAgvAdiacente!=0 && DirAgvConsiderato==DirAgvAdiacente) AgvInAllontanamento=true;
                          else{
                             if(DistX<DIST_MIN_RETTE_PARALLELE) AgvInAllontanamento=false;
                          }
                       }
                    }
                    //------------------------------------------------------------------
                    // Una volta accertato l'allontanamento, recupero la direzione
                    // dei TGV nei nodi considerati
                    //------------------------------------------------------------------
                    if( AgvInAllontanamento==true ){
                       //---------------------------------------------------------------
                       // Memorizzo la direzione di allontanemento
                       // DirAllontanamento==0 (non in allontanemento)
                       // DirAllontanamento==1 (entrambi indietro)
                       // DirAllontanamento==2 (entrambi avanti)
                       // DirAllontanamento==3 (uno dei due indietro e l'altro avanti)
                       //---------------------------------------------------------------
                       DirAllontanamento=3;
                       if(DirAgvConsiderato==1 && DirAgvAdiacente==1) DirAllontanamento=1;   // indietro
                       if(DirAgvConsiderato==2 && DirAgvAdiacente==2) DirAllontanamento=2;   // avanti
                    }
                 }
                 //---------------------------------------------------------------
                 // AL-17/01/2014 (AgvInIseguimento=false)
                 // TGV in ALLONTANAMENTO NON POSSONO ESSERE IN INSEGUIMENTO
                 //---------------------------------------------------------------
                 if(AgvInAllontanamento==true) AgvInIseguimento=false;
                 //---------------------------------------------------------------
                 // AL-17/01/2014 (AgvInIseguimento=false)
                 // TGV in DIREZIONI OPPOSTE O UNA DELLE 2 SCONOSCIUTA
                 //---------------------------------------------------------------
                 if(DirAgvConsiderato!=0 && DirAgvAdiacente!=0 && DirAgvAdiacente!=DirAgvConsiderato) AgvInIseguimento=false;
                 //---------------------------------------------------------------
                 // !!! SICUREZZA !!!
                 // AL-31/01/2014 (AgvInAllontanamento=false)
                 // Verifico se il TGV considerato sul nodo deve fare una 180°
                 // sicuramente le navette non si stanno inseguendo
                 //---------------------------------------------------------------
                 if(AgvInAllontanamento==true && (AgvConsiderato_In180==true || AgvAdiacente_In180==true)) AgvInAllontanamento=false;
                 //---------------------------------------------------------------
                 // !!! SICUREZZA !!!
                 // 07/09/2011
                 // Verifico se il TGV considerato sul nodo deve fare una 180°
                 // sicuramente le navette non si stanno inseguendo
                 //---------------------------------------------------------------
                 if(AgvInIseguimento==true && (AgvConsiderato_In180==true || AgvAdiacente_In180==true)) AgvInIseguimento=false;
                 //---------------------------------------------------------------
                 // 01/03/2011 AGV ADIACENTE GIA' RUOTATO                D
                 // Se la navetta adicente è già ruotata controllo che il suo
                 // stato di rotazione coincida con quella che ha già il TGV
                 // considerato o che assumerà sul nodo considerato
                 //---------------------------------------------------------------
                 if( AgvAdiacentaGiaRuotato==true ){
                    if(AgvAdiacente_RotAttuale==0 && AgvConsiderato_RotAttuale!=0 && AgvConsiderato_InRot!=10 && AgvConsiderato_InRot!=20) AgvInIseguimento = false;
                    if(AgvAdiacente_RotAttuale==1 && AgvConsiderato_RotAttuale!=1 && AgvConsiderato_InRot!=11 && AgvConsiderato_InRot!=21) AgvInIseguimento = false;
                    if(AgvAdiacente_RotAttuale==2 && AgvConsiderato_RotAttuale!=2 && AgvConsiderato_InRot!=12 && AgvConsiderato_InRot!=22) AgvInIseguimento = false;
                    if(AgvAdiacente_RotAttuale==3 && AgvConsiderato_RotAttuale!=3 && AgvConsiderato_InRot!=13 && AgvConsiderato_InRot!=23) AgvInIseguimento = false;
                 }
                 //----------------------------------------
                 // (NEW) Verifica se SATELLITE occupato
                 //----------------------------------------
                 if(SempreON ){
                    DistRetteParallele   = DIST_MIN_RETTE_PARALLELE;
                    DistRotazioneSingola = DIST_MIN_ROT_SINGOLA;
                    DistRotazDoppiaMax   = DIST_MIN_ROT_DOPPIA_MAX;
                    DistRotazDoppiaMin   = DIST_MIN_ROT_DOPPIA_MIN;
                    DistAllontanamento   = DIST_MIN_ALLONTANAMENTO;
                    //----------------------------------------------------------------------
                    // 15/11/2006
                    // Se entrambi i TGV hanno un BOX a bordo la DIST_MIN_RETTE_PARALLELE
                    // deve essere maggiorata di 30cm per un fatto di sicurezza
                    // (vale per i sogliola non per i pallet)
                    //----------------------------------------------------------------------
                  //if(AGV[NumAgv]->stato.s.bit.car && AGV[agv_adiacente]->stato.s.bit.car) DistRetteParallele = DIST_MIN_RETTE_PARALLELE + INGOMBRO_BOX_A_BORDO;
                    //----------------------------------------------------------------------
                    // 19/08/2008
                    // Se anche un solo TGV ha un box a bordo deve essere maggiorata
                    // di 30cm per un fatto di sicurezza
                    // (vale per i sogliola non per i pallet)
                    //----------------------------------------------------------------------
                  //if(AGV[NumAgv]->stato.s.bit.car || AGV[agv_adiacente]->stato.s.bit.car) DistRotazioneSingola = DIST_MIN_ROT_SINGOLA + INGOMBRO_BOX_A_BORDO;
                    //----------------------------------------------------------------------
                    // (1) Se DIST < DIST_MIN_RETTE_PARALLELE --> SAT=OCCUPATO
                    //----------------------------------------------------------------------
                    if( Dist<DistRetteParallele ) break;
                    //-----------------------------------------------------------------------------------
                    // 25/02/2014 GB Controllo aggiuntivo per RETTE PARALLELE INCLINATE
                    //
                    // - Abilitare solo se informazioni di percorso indicano TGVs in retta
                    // - 1) Calcolo della inclinazione del TGV Considerato in occupazione
                    // - 2) Calcolo della posizione del TGV adiacenete rispetto al primo
                    // - 3) Calcolo dell'angolo e ipotenusa per la triangolazione
                    // - 4) Calcolo dellr proiezioni sulla traiettoria del primo TGV
                    // - 5) Sovrascrittura delle componenti DistX e DistY utilizzate nella funzione
                    //      Eventuale inversioni delle proiezioni in base alla direzione di avanzamento
                    //-----------------------------------------------------------------------------------
                    if(AbilitaControlloRetteParalleleInclinate){
                       if(AgvConsiderato_InRot==0 && AgvAdiacente_InRot==0 &&
                          ((AgvConsideratoRettaX && AgvAdiacenteInRettaX) ||
                           (AgvConsideratoRettaY && AgvAdiacenteInRettaY))){
                          DistXDev = 0;
                          DistYDev = 0;
                          //---------------------------------------------
                          // 1) Calcolo Angolo A TGV in occupazione
                          //    Pendenza P1-P2
                          //---------------------------------------------
                          QuadranteA=1;
                          if((x2>x1) && (y2<y1)) QuadranteA=1;
                          if((x2<x1) && (y2<y1)) QuadranteA=2;
                          if((x2<x1) && (y2>y1)) QuadranteA=3;
                          if((x2>x1) && (y2>y1)) QuadranteA=4;
                          if( SempreON ){
                             //--------------------------------------------------
                             // 1A) 12/10/2015 Se NON esiste un punto sucessivo
                             //    Calcolo Pendenza P1-P2 da posizione attuale
                             //--------------------------------------------------
                             if(PuntoSuccessivo==0 && x2==0 && y2==0){
                                if(AGV[ NumAgv ]->stato.pos>0 && AGV[ NumAgv ]->stato.pos<=TOTPUNTI){
                                   if(NodoConsiderato!=AGV[ NumAgv ]->stato.pos){
                                      if(ptr_percorso>0){
                                         x2 = Punti[ AGV[ NumAgv ]->mission.punto[ptr_percorso-1]][0];
                                         y2 = Punti[ AGV[ NumAgv ]->mission.punto[ptr_percorso-1]][1];
                                         if((x1>x2) && (y1<y2)) QuadranteA=1;
                                         if((x1<x2) && (y1<y2)) QuadranteA=2;
                                         if((x1<x2) && (y1>y2)) QuadranteA=3;
                                         if((x1>x2) && (y1>y2)) QuadranteA=4;
                                      }
                                   }
                                }
                             }
                          }
                          DistXDev = (double) abs(x1-x2);
                          DistYDev = (double) abs(y1-y2);
                          if(DistXDev>OFFSET_MAX_COORD_TRATTO_INCLINATO && DistYDev>OFFSET_MAX_COORD_TRATTO_INCLINATO ){
                             TangA       = (long double)(DistYDev)/(long double)(DistXDev);
                             AngoloRadA  = (long double)(atanl(TangA));
                          }else{
                             //-------------------------------------------------------
                             // Si sta muovendo parallelamente agli assi cartesiani
                             //-------------------------------------------------------
                             if(DistXDev<50 ){ AngoloRadA=M_PI/2;}
                             if(DistYDev<50 ){ AngoloRadA=0;     }
                          }
                          if(QuadranteA==2) AngoloRadA=M_PI  -AngoloRadA;
                          if(QuadranteA==3) AngoloRadA=M_PI  +AngoloRadA;
                          if(QuadranteA==4) AngoloRadA=M_PI*2-AngoloRadA;
                          //--------------------------------------------------------
                          // Elabora solo se traccia veramente inclinata altrimenti
                          // si sta muovendo parallelamente agli assi cartesiani
                          // e valgono le elaborazioni standard
                          //--------------------------------------------------------
                          if(DistXDev>OFFSET_MAX_COORD_TRATTO_INCLINATO && DistYDev>OFFSET_MAX_COORD_TRATTO_INCLINATO){
                             //---------------------------------------------
                             // 2) Calcolo Angolo B Vicino rispetto a asse X
                             //    Pendenza P1-Pv1
                             //---------------------------------------------
                             QuadranteB=1;
                             if((xv1>x1) && (yv1<y1)) QuadranteB=1;
                             if((xv1<x1) && (yv1<y1)) QuadranteB=2;
                             if((xv1<x1) && (yv1>y1)) QuadranteB=3;
                             if((xv1>x1) && (yv1>y1)) QuadranteB=4;
                             DistXDev = (double) abs(x1-xv1);
                             DistYDev = (double) abs(y1-yv1);
                             if(DistXDev>OFFSET_MAX_COORD_TRATTO_INCLINATO && DistYDev>OFFSET_MAX_COORD_TRATTO_INCLINATO ){
                                TangB       = (long double)(DistYDev)/(long double)(DistXDev);
                                AngoloRadB  = (long double)(atanl(TangB));
                             }else{
                                if(DistXDev<50 ){ AngoloRadB=M_PI/2;}
                                if(DistYDev<50 ){ AngoloRadB=0;     }
                             }

                             if(QuadranteB==2) AngoloRadB=M_PI  -AngoloRadB;
                             if(QuadranteB==3) AngoloRadB=M_PI  +AngoloRadB;
                             if(QuadranteB==4) AngoloRadB=M_PI*2-AngoloRadB;
                             //---------------------------------------------
                             // 3) Calcolo Angolo e Ipotenusa del Triangolo
                             //    con altro punto
                             //---------------------------------------------
                             AngoloRadC=AngoloRadA-AngoloRadB;
                             if(AngoloRadA<AngoloRadB) AngoloRadC=AngoloRadB-AngoloRadA;
                             if(AngoloRadC>M_PI) AngoloRadC=(M_PI*2)-AngoloRadC;
                             DistXDev = (double) abs(x1-xv1);
                             DistYDev = (double) abs(y1-yv1);
                             if(DistXDev<1000 && DistXDev<1000){
                                CalcolaRad = (DistXDev*DistXDev) + (DistYDev*DistYDev);
                                DistDev = (int) sqrt_algo( CalcolaRad );
                             }
                             //------------------------------------------
                             // 4) Calcolo della proiezione in X e Y
                             //------------------------------------------
                             DistXDev = DistDev* cosl(AngoloRadC);
                             DistYDev = DistDev* sinl(AngoloRadC);
                             //-------------------------------------------------------------------
                             // 5) Sovrascrivi DistX e DistY
                             //    DistXDev in realtà è la componente nel verso di avanzamento
                             //    DistYDev in realtà è la componente ortogonale all'avanzamento
                             //
                             //    Quindi se AgvConsideratoRettaX --> OK coincidono
                             //    Quindi se AgvConsideratoRettaY --> INVERTIRE
                             //
                             //-------------------------------------------------------------------
                             DistX = DistXDev;
                             DistY = DistYDev;
                             if(AgvConsideratoRettaY){
                                DistY = DistXDev;
                                DistX = DistYDev;
                             }
                          }
                       }
                    }
                    //----------------------------------------------------------------------
                    // PUNTI TERMINALI IN INGOMBRO
                    // Se il TGV adiacente è fermo su un punto terminale il controllo di
                    // rotazione singola viene fatto in base allo stato del TGV adiacente
                    //----------------------------------------------------------------------
                    if(AgvAdiacentaGiaRuotato==true && N.punti_notevoli[ Vicino ]==true){
                       //----------------------------------------------------------------------
                       // AL-17/01/2014 NON START TGV SE VICINO IN INGOMBRO
                       // Se il TGV considerato non ha ancora la missione e si trova su
                       // un punto terminale NON IN INGOBRO, MA IL VICINO E' LO E' NON PARTO
                       //
                       // N.B. per controllare se il TGV non ha la missione uso solo lo start
                       //      e non pstart perchè è valorizzato da init_mission()
                       //----------------------------------------------------------------------
                       NoStartAgvConsiderato=false;
                       if(AGV[NumAgv]->stato.start==0 && N.punti_notevoli[AGV[NumAgv]->stato.pos]==true && N.pt_in_ingombro[AGV[NumAgv]->stato.pos]==false){
                          NoStartAgvConsiderato=true;
                       }
                       //----------------------------------------------------------------------
                       // AL-17/01/2014 NON START TGV SE VICINO IN INGOMBRO
                       // Solo se TGV Considerato è già in missione o parte anch'esso da un
                       // pt_in_ingombro
                       //----------------------------------------------------------------------
                       if(NoStartAgvConsiderato==false){
                          //----------------------------------------------------------------------
                          // (2) Se i TGV si muovono su rette parallele nella stessa direzione
                          //     skip!
                          //
                          //     29/02/2012 modificati controlli di accesso a rette parallele
                          //----------------------------------------------------------------------
                          if(DistX>=DistRetteParallele && AgvAdiacentaGiaRuotato && AgvAdiacenteInRettaY){
                             if( AgvConsideratoRettaY && AgvConsiderato_InRot==0             ) { satellite_occupato=0; continue; }
                             else{
                                //-----------------------------------------------------------
                                // Se i TGV sono su rette parallele ma il considerato
                                // deve compiere una rotazione, posso occupare comunque
                                // se la distanza complessiva è > DIST_ROT_DOPPIA_MAX o se
                                // la distanza nel verso possosto alle terre parallele
                                // peremette comunque una rotazione SINGOLA con il TGV che
                                // andrà in inseguimento a quello adiacente
                                //-----------------------------------------------------------
                                if(Dist>=DistRotazDoppiaMax                                  ) { satellite_occupato=0; continue; }
                                if(AgvInIseguimento==true && DistY>=DistRotazioneSingola){
                                   if((AgvConsiderato_InRot==10 || AgvConsiderato_InRot==12) ) { satellite_occupato=0; continue; }
                                   if((AgvConsiderato_InRot==20 || AgvConsiderato_InRot==22) ) { satellite_occupato=0; continue; }
                                }
                                //-----------------------------------------------------------
                                // AL-01/07/2014 Con i TGV in allontanamento e 2 TGV
                                // su rette parallele di cui l'adiacente già ruotato,
                                // l'altro può compiere la rotazione singole se la Dist
                                // è sufficiente
                                //-----------------------------------------------------------
                                if(AgvInAllontanamento==true && DistY>=DistRotazioneSingola  ) { satellite_occupato=0; continue; }
                                //-------------------------------------------------------------------
                                // AL-06/02/2014 Nel caso di TGV considerato in AgvConsideratoRetta
                                //               ma in inizio curva il nodo è occupabile perchè
                                //               sullo start curva il TGV non ruota
                                // 1. Se i TGV partono da una condizione di rotazione identiche
                                //    è necessario che NON ci sia un cambio di direzione, questo
                                //    garantisce che tra ingresso e uscita curva ci sia un
                                //    allontanamento tra i due
                                // 2. Se i TGV partono da una condizione di rotazione opposte
                                //    è necessario che ci sia un cambio di direzione, questo
                                //    garantisce che tra ingresso e uscita curva ci sia un
                                //    allontanamento tra i due
                                //-------------------------------------------------------------------
                                if(AgvConsideratoRettaY==true && AgvConsiderato_InRot>19 && AgvConsideratoSuCurvaStretta==0){
                                   //-------------------------------------------------------------
                                   // 1. Se entrambi non cambiano direzione la rotazione
                                   //-------------------------------------------------------------
                                   if(CambioDirConsiderato==false && CambioDirAdiacente==false){
                                      if(AgvConsiderato_RotAttuale==AgvAdiacente_RotAttuale){ satellite_occupato=0; continue; }
                                      //----------------------------------------------------------------------------------
                                      // STEFANO - 18/01/2017 (Se va)
                                      // ALBERTO - 18/01/2017 (Se non va)
                                      // $$$$$$$$$$$ - Rimane da valutare a modo
                                      // Se dall'inizio alla fine della curva la rotazione dell'Agv rimane inalterata
                                      // vuol dire che è una curva che non provoca una rotazione (se non molto blanda)
                                      // dell'agv quindi è come se prodeguissi dritto
                                      //----------------------------------------------------------------------------------
                                      if(AgvConsiderato_InRot==20 || AgvConsiderato_InRot==22){
                                         if(AgvConsiderato_RotAttuale==0 && AgvConsiderato_InRot==20){ satellite_occupato=0; continue; }
                                         if(AgvConsiderato_RotAttuale==2 && AgvConsiderato_InRot==22){ satellite_occupato=0; continue; }
                                      }
                                   }
                                   else{
                                      //----------------------------------------------------------
                                      // 2a. Rotazioni opposte e solo uno cambia direzione
                                      //----------------------------------------------------------
                                      if(CambioDirConsiderato==true && CambioDirAdiacente==false){
                                         if(AgvConsiderato_InRot==20 || AgvConsiderato_InRot==22){
                                            if(AgvConsiderato_RotAttuale==0 && AgvConsiderato_InRot==20){ satellite_occupato=0; continue; }
                                            if(AgvConsiderato_RotAttuale==2 && AgvConsiderato_InRot==22){ satellite_occupato=0; continue; }
                                         }
                                      }
                                      //----------------------------------------------------------
                                      // 2b. Rotazioni opposte e solo uno cambia direzione
                                      //----------------------------------------------------------
                                      if(CambioDirConsiderato==false && CambioDirAdiacente==true){
                                         if(AgvAdiacente_InRot==20 || AgvAdiacente_InRot==22){
                                            if(AgvAdiacente_RotAttuale==0 && AgvAdiacente_InRot==20){ satellite_occupato=0; continue; }
                                            if(AgvAdiacente_RotAttuale==2 && AgvAdiacente_InRot==22){ satellite_occupato=0; continue; }
                                         }
                                      }
                                   }
                                }
                             }
                          }
                          if(DistY>=DistRetteParallele && AgvAdiacentaGiaRuotato && AgvAdiacenteInRettaX){
                             if( AgvConsideratoRettaX && AgvConsiderato_InRot==0             ) { satellite_occupato=0; continue; }
                             else{
                                //-----------------------------------------------------------
                                // Se i TGV sono su rette parallele ma il considerato
                                // deve compiere una rotazione, posso occupare comunque
                                // se la distanza complessiva è > DIST_ROT_DOPPIA_MAX o se
                                // la distanza nel verso possosto alle terre parallele
                                // peremette comunque una rotazione SINGOLA con il TGV che
                                // andrà in inseguimento a quello adiacente
                                //-----------------------------------------------------------
                                if(Dist>=DistRotazDoppiaMax                                  ) { satellite_occupato=0; continue; }
                                if(AgvInIseguimento==true && DistX>=DistRotazioneSingola){
                                   if((AgvConsiderato_InRot==11 || AgvConsiderato_InRot==13) ) { satellite_occupato=0; continue; }
                                   if((AgvConsiderato_InRot==21 || AgvConsiderato_InRot==23) ) { satellite_occupato=0; continue; }
                                }
                                //-----------------------------------------------------------
                                // AL-01/07/2014 Con i TGV in allontanamento e 2 TGV
                                // su rette parallele di cui l'adiacente già ruotato,
                                // l'altro può compiere la rotazione singole se la Dist
                                // è sufficiente
                                //-----------------------------------------------------------
                                if(AgvInAllontanamento==true && DistX>=DistRotazioneSingola  ) { satellite_occupato=0; continue; }
                                //-------------------------------------------------------------------
                                // AL-06/02/2014 Nel caso di TGV considerato in AgvConsideratoRetta
                                //               ma in inizio curva il nodo è occupabile perchè
                                //               sullo start curva il TGV non ruota
                                // 1. Se i TGV partono da una condizione di rotazione identiche
                                //    è necessario che NON ci sia un cambio di direzione, questo
                                //    garantisce che tra ingresso e uscita curva ci sia un
                                //    allontanamento tra i due
                                // 2. Se i TGV partono da una condizione di rotazione opposte
                                //    è necessario che ci sia un cambio di direzione, questo
                                //    garantisce che tra ingresso e uscita curva ci sia un
                                //    allontanamento tra i due
                                //-------------------------------------------------------------------
                                if(AgvConsideratoRettaX==true && AgvConsiderato_InRot>19 && AgvConsideratoSuCurvaStretta==0){
                                   //-------------------------------------------------------------
                                   // 1. Se entrambi non cambiano direzione la rotazione
                                   //-------------------------------------------------------------
                                   if(CambioDirConsiderato==false && CambioDirAdiacente==false){
                                      if(AgvConsiderato_RotAttuale==AgvAdiacente_RotAttuale){ satellite_occupato=0; continue; }
                                      //----------------------------------------------------------------------------------
                                      // STEFANO - 18/01/2017 (Se va)
                                      // ALBERTO - 18/01/2017 (Se non va)
                                      // $$$$$$$$$$$ - Rimane da valutare a modo
                                      // Se dall'inizio alla fine della curva la rotazione dell'Agv rimane inalterata
                                      // vuol dire che è una curva che non provoca una rotazione (se non molto blanda)
                                      // dell'agv quindi è come se prodeguissi dritto
                                      //----------------------------------------------------------------------------------
                                      if(AgvConsiderato_InRot==21 || AgvConsiderato_InRot==23){
                                         if(AgvConsiderato_RotAttuale==1 && AgvConsiderato_InRot==21){ satellite_occupato=0; continue; }
                                         if(AgvConsiderato_RotAttuale==3 && AgvConsiderato_InRot==23){ satellite_occupato=0; continue; }
                                      }
                                   }
                                   else{
                                      //----------------------------------------------------------
                                      // 2a. Rotazioni opposte e solo uno cambia direzione
                                      //----------------------------------------------------------
                                      if(CambioDirConsiderato==true && CambioDirAdiacente==false){
                                         if(AgvConsiderato_InRot==21 || AgvConsiderato_InRot==23){
                                            if(AgvConsiderato_RotAttuale==1 && AgvConsiderato_InRot==21){ satellite_occupato=0; continue; }
                                            if(AgvConsiderato_RotAttuale==3 && AgvConsiderato_InRot==23){ satellite_occupato=0; continue; }
                                         }
                                      }
                                      //----------------------------------------------------------
                                      // 2b. Rotazioni opposte e solo uno cambia direzione
                                      //----------------------------------------------------------
                                      if(CambioDirConsiderato==false && CambioDirAdiacente==true){
                                         if(AgvAdiacente_InRot==21 || AgvAdiacente_InRot==23){
                                            if(AgvAdiacente_RotAttuale==1 && AgvAdiacente_InRot==21){ satellite_occupato=0; continue; }
                                            if(AgvAdiacente_RotAttuale==3 && AgvAdiacente_InRot==23){ satellite_occupato=0; continue; }
                                         }
                                      }
                                   }
                                }
                             }
                          }
                          //----------------------------------------------------------------------
                          // (3) Se DIST > DIST_MIN_ROT_SINGOLA (STANDARD PER TGV IN MISSIONE)
                          //     Verifica se almeno UNO  è orientato nel verso di minore ingombro
                          //     allora --> OK
                          //
                          // AL-20/02 controllo ridotto a TGV sulla stessa traccia (1)
                          // AL-02/05 controllo per TGV in rot singola di allontanemento (2)
                          // AL-05/11/15 riprintino controllo RotSingola esteso (3)
                          //
                          //----------------------------------------------------------------------
                          // -- (1)
                        //if( DistX >= DistRotazioneSingola && DistY<=5 ){
                          // -- (3)
                          if( DistX >= DistRotazioneSingola ){
                             if( AgvAdiacenteInRettaY && !AgvAdiacente_InRot  ) { satellite_occupato=0; continue; }
                             if( AgvConsideratoRettaY && !AgvConsiderato_InRot) { satellite_occupato=0; continue; }
                             //-----------------------------------------------------
                             // CURVE: Il TGV sul nodo curva risulta ancora in
                             //        retta (valido su stessa traccia)
                             //-----------------------------------------------------
                             if(AgvConsideratoRettaY && AgvConsiderato_InRot>19 && AgvAdiacente_InRot==0){
                                if(AgvConsiderato_RotAttuale==0 || AgvConsiderato_RotAttuale==2) { satellite_occupato=0; continue; }
                             }
                             //----------------------------------------------------------------------------------
                             // STEFANO - 17/01/2017 (Se va)
                             // ALBERTO - 17/01/2017 (Se non va)
                             // Se la dall'inizio alla fine della curva la rotazione dell'Agv rimane inalterata
                             // vuol dire che è una curva che non provoca una rotazione (se non molto blanda)
                             // dell'agv quindi è come se prodeguissi dritto
                             //----------------------------------------------------------------------------------
                             if(AgvConsideratoRettaY && (AgvConsiderato_InRot==20 || AgvConsiderato_InRot==22)){
                                if(AgvConsiderato_RotAttuale==0 && AgvConsiderato_InRot==20){ satellite_occupato=0; continue; }
                                if(AgvConsiderato_RotAttuale==2 && AgvConsiderato_InRot==22){ satellite_occupato=0; continue; }
                             }
                             if(AgvAdiacenteInRettaY && (AgvAdiacente_InRot==20 || AgvAdiacente_InRot==22)){
                                if(AgvAdiacente_RotAttuale==0 && AgvAdiacente_InRot==20){ satellite_occupato=0; continue; }
                                if(AgvAdiacente_RotAttuale==2 && AgvAdiacente_InRot==22){ satellite_occupato=0; continue; }
                             }
                          }
                          // -- (1)
                        //if( DistY >= DistRotazioneSingola && DistX<=5 ){
                          // -- (3)
                          if( DistY >= DistRotazioneSingola ){
                             if( AgvAdiacenteInRettaX && !AgvAdiacente_InRot  ) { satellite_occupato=0; continue; }
                             if( AgvConsideratoRettaX && !AgvConsiderato_InRot) { satellite_occupato=0; continue; }
                             //-----------------------------------------------------
                             // CURVE: Il TGV sul nodo curva risulta ancora in
                             //        retta (valido su stessa traccia)
                             //-----------------------------------------------------
                             if(AgvConsideratoRettaX && AgvConsiderato_InRot>19 && AgvAdiacente_InRot==0){
                                if(AgvConsiderato_RotAttuale==1 || AgvConsiderato_RotAttuale==3) { satellite_occupato=0; continue; }
                             }
                             //----------------------------------------------------------------------------------
                             // STEFANO - 17/01/2017 (Se va)
                             // ALBERTO - 17/01/2017 (Se non va)
                             // Se la dall'inizio alla fine della curva la rotazione dell'Agv rimane inalterata
                             // vuol dire che è una curva che non provoca una rotazione (se non molto blanda)
                             // dell'agv quindi è come se prodeguissi dritto
                             //----------------------------------------------------------------------------------
                             if(AgvConsideratoRettaX && (AgvConsiderato_InRot==21 || AgvConsiderato_InRot==23)){
                                if(AgvConsiderato_RotAttuale==1 && AgvConsiderato_InRot==21){ satellite_occupato=0; continue; }
                                if(AgvConsiderato_RotAttuale==3 && AgvConsiderato_InRot==23){ satellite_occupato=0; continue; }
                             }
                             if(AgvAdiacenteInRettaX && (AgvAdiacente_InRot==21 || AgvAdiacente_InRot==23)){
                                if(AgvAdiacente_RotAttuale==1 && AgvAdiacente_InRot==21){ satellite_occupato=0; continue; }
                                if(AgvAdiacente_RotAttuale==3 && AgvAdiacente_InRot==23){ satellite_occupato=0; continue; }
                             }
                          }
                          // -- (2)
                          if( DistX >= DistRotazioneSingola && AgvInAllontanamento==true ){
                             if( AgvAdiacenteInRettaY ) { satellite_occupato=0; continue; }
                             if( AgvConsideratoRettaY ) { satellite_occupato=0; continue; }
                          }
                          if( DistY >= DistRotazioneSingola && AgvInAllontanamento==true ){
                             if( AgvAdiacenteInRettaX ) { satellite_occupato=0; continue; }
                             if( AgvConsideratoRettaX ) { satellite_occupato=0; continue; }
                          }
                       }
                    }
                    //------------------------------------------------------------------------
                    // CONTROLLI STANDARD PER TGV CON MISSIONI IN CORSO O SU NODI DI PERCORSO
                    //------------------------------------------------------------------------
                    else{
                       //----------------------------------------------------------------------
                       // (2) Se i TGV si muovono su rette parallele nella stessa direzione
                       //     skip!
                       //
                       //     29/02/2012 modificati controlli di accesso a rette parallele
                       //----------------------------------------------------------------------
                       if(DistX>=DistRetteParallele && AgvAdiacentaGiaRuotato && AgvAdiacenteInRettaY){
                          if( AgvConsideratoRettaY && AgvConsiderato_InRot==0             ) { satellite_occupato=0; continue; }
                          else{
                             //-----------------------------------------------------------
                             // Se i TGV sono su rette parallele ma il considerato
                             // deve compiere una rotazione, posso occupare comunque
                             // se la distanza complessiva è > DIST_ROT_DOPPIA_MAX o se
                             // la distanza nel verso possosto alle terre parallele
                             // peremette comunque una rotazione SINGOLA con il TGV che
                             // andrà in inseguimento a quello adiacente
                             //-----------------------------------------------------------
                             if(Dist>=DistRotazDoppiaMax                                  ) { satellite_occupato=0; continue; }
                             if(AgvInIseguimento==true && DistY>=DistRotazioneSingola){
                                if((AgvConsiderato_InRot==10 || AgvConsiderato_InRot==12) ) { satellite_occupato=0; continue; }
                                if((AgvConsiderato_InRot==20 || AgvConsiderato_InRot==22) ) { satellite_occupato=0; continue; }
                             }
                             //-----------------------------------------------------------
                             // AL-01/07/2014 Con i TGV in allontanamento e 2 TGV
                             // su rette parallele di cui l'adiacente già ruotato,
                             // l'altro può compiere la rotazione singole se la Dist
                             // è sufficiente
                             //-----------------------------------------------------------
                             if(AgvInAllontanamento==true && DistY>=DistRotazioneSingola  ) { satellite_occupato=0; continue; }
                             //-------------------------------------------------------------------
                             // AL-06/02/2014 Nel caso di TGV considerato in AgvConsideratoRetta
                             //               ma in inizio curva il nodo è occupabile perchè
                             //               sullo start curva il TGV non ruota
                             // 1. Se i TGV partono da una condizione di rotazione identiche
                             //    è necessario che NON ci sia un cambio di direzione, questo
                             //    garantisce che tra ingresso e uscita curva ci sia un
                             //    allontanamento tra i due
                             // 2. Se i TGV partono da una condizione di rotazione opposte
                             //    è necessario che ci sia un cambio di direzione, questo
                             //    garantisce che tra ingresso e uscita curva ci sia un
                             //    allontanamento tra i due
                             //-------------------------------------------------------------------
                             if(AgvConsideratoRettaY==true && AgvConsiderato_InRot>19 && AgvConsideratoSuCurvaStretta==0){
                                //-------------------------------------------------------------
                                // 1. Se entrambi non cambiano direzione la rotazione
                                //-------------------------------------------------------------
                                if(CambioDirConsiderato==false && CambioDirAdiacente==false){
                                   if(AgvConsiderato_RotAttuale==AgvAdiacente_RotAttuale){ satellite_occupato=0; continue; }
                                   //----------------------------------------------------------------------------------
                                   // 07/01/2016 GB. Se AGV adiacente NON in missione valgono anche rotazioni Speculari
                                   // 31/01/2017 AL  Se AGV adiacente FERMO su pint valgono anche rotazioni Speculari
                                   //----------------------------------------------------------------------------------
                                 //if(AGV[ agv_adiacente ]->mission.pstart==0){
                                   if(AGV[ agv_adiacente ]->mission.pstart==0 || AGV[ agv_adiacente ]->stato.pos==AGV[ agv_adiacente ]->mission.pintend){
                                      if(AgvConsiderato_RotAttuale==0 && AgvAdiacente_RotAttuale==2 ){ satellite_occupato=0; continue; }
                                      if(AgvConsiderato_RotAttuale==2 && AgvAdiacente_RotAttuale==0 ){ satellite_occupato=0; continue; }
                                   }
                                   //-----------------------------------------------------------------------------------------------
                                   // 07/01/2016 GB. Se impostato Rallentamento==5 si presuppone che la curva non sia in ingombro
                                   //                Quindi anche in questo caso valgono rotazioni speculari
                                   //                tra TGV considerato e adiacente (quando si muovono su rette parallele)
                                   //-----------------------------------------------------------------------------------------------
                                   if(ptr_percorso>1){
                                      if(AGV[ NumAgv ]->mission.dati_perc[ptr_percorso-1].ral==RALL_CURVA_NON_IN_INGOMBRO ){
                                         if(AgvConsiderato_RotAttuale==0 && AgvAdiacente_RotAttuale==2 ){ satellite_occupato=0; continue; }
                                         if(AgvConsiderato_RotAttuale==2 && AgvAdiacente_RotAttuale==0 ){ satellite_occupato=0; continue; }
                                      }
                                   }
                                   //----------------------------------------------------------------------------------
                                   // STEFANO - 18/01/2017 (Se va)
                                   // ALBERTO - 18/01/2017 (Se non va)
                                   // $$$$$$$$$$$ - Rimane da valutare a modo
                                   // Se dall'inizio alla fine della curva la rotazione dell'Agv rimane inalterata
                                   // vuol dire che è una curva che non provoca una rotazione (se non molto blanda)
                                   // dell'agv quindi è come se prodeguissi dritto
                                   //----------------------------------------------------------------------------------
                                   if(AgvConsiderato_InRot==20 || AgvConsiderato_InRot==22){
                                      if(AgvConsiderato_RotAttuale==0 && AgvConsiderato_InRot==20){ satellite_occupato=0; continue; }
                                      if(AgvConsiderato_RotAttuale==2 && AgvConsiderato_InRot==22){ satellite_occupato=0; continue; }
                                   }
                                }
                                else{
                                   //----------------------------------------------------------
                                   // 2a. Rotazioni opposte e solo uno cambia direzione
                                   //----------------------------------------------------------
                                   if(CambioDirConsiderato==true && CambioDirAdiacente==false){
                                      if(AgvConsiderato_InRot==20 || AgvConsiderato_InRot==22){
                                         if(AgvConsiderato_RotAttuale==0 && AgvConsiderato_InRot==20){ satellite_occupato=0; continue; }
                                         if(AgvConsiderato_RotAttuale==2 && AgvConsiderato_InRot==22){ satellite_occupato=0; continue; }
                                      }
                                   }
                                   //----------------------------------------------------------
                                   // 2b. Rotazioni opposte e solo uno cambia direzione
                                   //----------------------------------------------------------
                                   if(CambioDirConsiderato==false && CambioDirAdiacente==true){
                                      if(AgvAdiacente_InRot==20 || AgvAdiacente_InRot==22){
                                         if(AgvAdiacente_RotAttuale==0 && AgvAdiacente_InRot==20){ satellite_occupato=0; continue; }
                                         if(AgvAdiacente_RotAttuale==2 && AgvAdiacente_InRot==22){ satellite_occupato=0; continue; }
                                      }
                                   }
                                }
                             }
                          }
                       }
                       if(DistY>=DistRetteParallele && AgvAdiacentaGiaRuotato && AgvAdiacenteInRettaX){
                          if( AgvConsideratoRettaX && AgvConsiderato_InRot==0             ) { satellite_occupato=0; continue; }
                          else{
                             //-----------------------------------------------------------
                             // Se i TGV sono su rette parallele ma il considerato
                             // deve compiere una rotazione, posso occupare comunque
                             // se la distanza complessiva è > DIST_ROT_DOPPIA_MAX o se
                             // la distanza nel verso possosto alle terre parallele
                             // peremette comunque una rotazione SINGOLA con il TGV che
                             // andrà in inseguimento a quello adiacente
                             //-----------------------------------------------------------
                             if(Dist>=DistRotazDoppiaMax                                  ) { satellite_occupato=0; continue; }
                             if(AgvInIseguimento==true && DistX>=DistRotazioneSingola){
                                if((AgvConsiderato_InRot==11 || AgvConsiderato_InRot==13) ) { satellite_occupato=0; continue; }
                                if((AgvConsiderato_InRot==21 || AgvConsiderato_InRot==23) ) { satellite_occupato=0; continue; }
                             }
                             //-----------------------------------------------------------
                             // AL-01/07/2014 Con i TGV in allontanamento e 2 TGV
                             // su rette parallele di cui l'adiacente già ruotato,
                             // l'altro può compiere la rotazione singole se la Dist
                             // è sufficiente
                             //-----------------------------------------------------------
                             if(AgvInAllontanamento==true && DistX>=DistRotazioneSingola  ) { satellite_occupato=0; continue; }
                             //-------------------------------------------------------------------
                             // AL-06/02/2014 Nel caso di TGV considerato in AgvConsideratoRetta
                             //               ma in inizio curva il nodo è occupabile perchè
                             //               sullo start curva il TGV non ruota
                             // 1. Se i TGV partono da una condizione di rotazione identiche
                             //    è necessario che NON ci sia un cambio di direzione, questo
                             //    garantisce che tra ingresso e uscita curva ci sia un
                             //    allontanamento tra i due
                             // 2. Se i TGV partono da una condizione di rotazione opposte
                             //    è necessario che ci sia un cambio di direzione, questo
                             //    garantisce che tra ingresso e uscita curva ci sia un
                             //    allontanamento tra i due
                             //-------------------------------------------------------------------
                             if(AgvConsideratoRettaX==true && AgvConsiderato_InRot>19 && AgvConsideratoSuCurvaStretta==0){
                                //-------------------------------------------------------------
                                // 1. Se entrambi non cambiano direzione la rotazione
                                //-------------------------------------------------------------
                                if(CambioDirConsiderato==false && CambioDirAdiacente==false){
                                   if(AgvConsiderato_RotAttuale==AgvAdiacente_RotAttuale){ satellite_occupato=0; continue; }
                                   //----------------------------------------------------------------------------------
                                   // 07/01/2016 GB. Se AGV adiacente NON in missione valgono anche rotazioni Speculari
                                   // 31/01/2017 AL  Se AGV adiacente FERMO su pint valgono anche rotazioni Speculari
                                   //----------------------------------------------------------------------------------
                                 //if(AGV[ agv_adiacente ]->mission.pstart==0){
                                   if(AGV[ agv_adiacente ]->mission.pstart==0 || AGV[ agv_adiacente ]->stato.pos==AGV[ agv_adiacente ]->mission.pintend){
                                      if(AgvConsiderato_RotAttuale==1 && AgvAdiacente_RotAttuale==3 ){ satellite_occupato=0; continue; }
                                      if(AgvConsiderato_RotAttuale==3 && AgvAdiacente_RotAttuale==1 ){ satellite_occupato=0; continue; }
                                   }
                                   //-----------------------------------------------------------------------------------------------
                                   // 07/01/2016 GB. Se impostato Rallentamento==5 si presuppone che la curva non sia in ingombro
                                   //                Quindi anche in questo caso valgono rotazioni speculari
                                   //                tra TGV considerato e adiacente (quando si muovono su rette parallele)
                                   //-----------------------------------------------------------------------------------------------
                                   if(ptr_percorso>1){
                                      if(AGV[ NumAgv ]->mission.dati_perc[ptr_percorso-1].ral==RALL_CURVA_NON_IN_INGOMBRO ){
                                         if(AgvConsiderato_RotAttuale==1 && AgvAdiacente_RotAttuale==3 ){ satellite_occupato=0; continue; }
                                         if(AgvConsiderato_RotAttuale==3 && AgvAdiacente_RotAttuale==1 ){ satellite_occupato=0; continue; }
                                      }
                                   }
                                   //----------------------------------------------------------------------------------
                                   // STEFANO - 18/01/2017 (Se va)
                                   // ALBERTO - 18/01/2017 (Se non va)
                                   // $$$$$$$$$$$ - Rimane da valutare a modo
                                   // Se dall'inizio alla fine della curva la rotazione dell'Agv rimane inalterata
                                   // vuol dire che è una curva che non provoca una rotazione (se non molto blanda)
                                   // dell'agv quindi è come se prodeguissi dritto
                                   //----------------------------------------------------------------------------------
                                   if(AgvConsiderato_InRot==21 || AgvConsiderato_InRot==23){
                                      if(AgvConsiderato_RotAttuale==1 && AgvConsiderato_InRot==21){ satellite_occupato=0; continue; }
                                      if(AgvConsiderato_RotAttuale==3 && AgvConsiderato_InRot==23){ satellite_occupato=0; continue; }
                                   }
                                }
                                else{
                                   //----------------------------------------------------------
                                   // 2a. Rotazioni opposte e solo uno cambia direzione
                                   //----------------------------------------------------------
                                   if(CambioDirConsiderato==true && CambioDirAdiacente==false){
                                      if(AgvConsiderato_InRot==21 || AgvConsiderato_InRot==23){
                                         if(AgvConsiderato_RotAttuale==1 && AgvConsiderato_InRot==21){ satellite_occupato=0; continue; }
                                         if(AgvConsiderato_RotAttuale==3 && AgvConsiderato_InRot==23){ satellite_occupato=0; continue; }
                                      }
                                   }
                                   //----------------------------------------------------------
                                   // 2b. Rotazioni opposte e solo uno cambia direzione
                                   //----------------------------------------------------------
                                   if(CambioDirConsiderato==false && CambioDirAdiacente==true){
                                      if(AgvAdiacente_InRot==21 || AgvAdiacente_InRot==23){
                                         if(AgvAdiacente_RotAttuale==1 && AgvAdiacente_InRot==21){ satellite_occupato=0; continue; }
                                         if(AgvAdiacente_RotAttuale==3 && AgvAdiacente_InRot==23){ satellite_occupato=0; continue; }
                                      }
                                   }
                                }
                             }
                          }
                       }
                       //----------------------------------------------------------------------
                       // (3) Se DIST > DIST_MIN_ROT_SINGOLA (STANDARD PER TGV IN MISSIONE)
                       //     Verifica se almeno UNO  è orientato nel verso di minore ingombro
                       //     allora --> OK
                       //
                       // AL-20/02 controllo ridotto a TGV sulla stessa traccia (1)
                       // AL-02/05 controllo per TGV in rot singola di allontanemento (2)
                       // AL-05/11/15 riprintino controllo RotSingola esteso (3)
                       //
                       //----------------------------------------------------------------------
                       // -- (1)
                       //if( DistX >= DistRotazioneSingola && DistY<=5 ){
                       // -- (3)
                       if( DistX >= DistRotazioneSingola ){
                          if( AgvAdiacenteInRettaY && !AgvAdiacente_InRot  ) { satellite_occupato=0; continue; }
                          if( AgvConsideratoRettaY && !AgvConsiderato_InRot) { satellite_occupato=0; continue; }
                          //-----------------------------------------------------
                          // CURVE: Il TGV sul nodo curva risulta ancora in
                          //        retta (valido su stessa traccia)
                          //-----------------------------------------------------
                          if(AgvConsideratoRettaY && AgvConsiderato_InRot>19 && AgvAdiacente_InRot==0){
                             if(AgvConsiderato_RotAttuale==0 || AgvConsiderato_RotAttuale==2) { satellite_occupato=0; continue; }
                          }
                          //----------------------------------------------------------------------------------
                          // STEFANO - 17/01/2017 (Se va)
                          // ALBERTO - 17/01/2017 (Se non va)
                          // Se la dall'inizio alla fine della curva la rotazione dell'Agv rimane inalterata
                          // vuol dire che è una curva che non provoca una rotazione (se non molto blanda)
                          // dell'agv quindi è come se prodeguissi dritto
                          //----------------------------------------------------------------------------------
                          if(AgvConsideratoRettaY && (AgvConsiderato_InRot==20 || AgvConsiderato_InRot==22)){
                             if(AgvConsiderato_RotAttuale==0 && AgvConsiderato_InRot==20){ satellite_occupato=0; continue; }
                             if(AgvConsiderato_RotAttuale==2 && AgvConsiderato_InRot==22){ satellite_occupato=0; continue; }
                          }
                          if(AgvAdiacenteInRettaY && (AgvAdiacente_InRot==20 || AgvAdiacente_InRot==22)){
                             if(AgvAdiacente_RotAttuale==0 && AgvAdiacente_InRot==20){ satellite_occupato=0; continue; }
                             if(AgvAdiacente_RotAttuale==2 && AgvAdiacente_InRot==22){ satellite_occupato=0; continue; }
                          }
                       }
                       // -- (1)
                       //if( DistY >= DistRotazioneSingola && DistX<=5 ){
                       // -- (3)
                       if( DistY >= DistRotazioneSingola ){
                          if( AgvAdiacenteInRettaX && !AgvAdiacente_InRot  ) { satellite_occupato=0; continue; }
                          if( AgvConsideratoRettaX && !AgvConsiderato_InRot) { satellite_occupato=0; continue; }
                          //-----------------------------------------------------
                          // CURVE: Il TGV sul nodo curva risulta ancora in
                          //        retta (valido su stessa traccia)
                          //-----------------------------------------------------
                          if(AgvConsideratoRettaX && AgvConsiderato_InRot>19 && AgvAdiacente_InRot==0){
                             if(AgvConsiderato_RotAttuale==1 || AgvConsiderato_RotAttuale==3) { satellite_occupato=0; continue; }
                          }
                          //----------------------------------------------------------------------------------
                          // STEFANO - 17/01/2017 (Se va)
                          // ALBERTO - 17/01/2017 (Se non va)
                          // Se la dall'inizio alla fine della curva la rotazione dell'Agv rimane inalterata
                          // vuol dire che è una curva che non provoca una rotazione (se non molto blanda)
                          // dell'agv quindi è come se prodeguissi dritto
                          //----------------------------------------------------------------------------------
                          if(AgvConsideratoRettaX && (AgvConsiderato_InRot==21 || AgvConsiderato_InRot==23)){
                             if(AgvConsiderato_RotAttuale==1 && AgvConsiderato_InRot==21){ satellite_occupato=0; continue; }
                             if(AgvConsiderato_RotAttuale==3 && AgvConsiderato_InRot==23){ satellite_occupato=0; continue; }
                          }
                          if(AgvAdiacenteInRettaX && (AgvAdiacente_InRot==21 || AgvAdiacente_InRot==23)){
                             if(AgvAdiacente_RotAttuale==1 && AgvAdiacente_InRot==21){ satellite_occupato=0; continue; }
                             if(AgvAdiacente_RotAttuale==3 && AgvAdiacente_InRot==23){ satellite_occupato=0; continue; }
                          }
                       }
                       // -- (2)
                       if( DistX >= DistRotazioneSingola && AgvInAllontanamento==true ){
                          if( AgvAdiacenteInRettaY ) { satellite_occupato=0; continue; }
                          if( AgvConsideratoRettaY ) { satellite_occupato=0; continue; }
                       }
                       if( DistY >= DistRotazioneSingola && AgvInAllontanamento==true ){
                          if( AgvAdiacenteInRettaX ) { satellite_occupato=0; continue; }
                          if( AgvConsideratoRettaX ) { satellite_occupato=0; continue; }
                       }
                    }
                    //-----------------------------------------------------------------------------
                    // AL-06/02/2014 - NUOVE CONDIZIONI DI INSEGUIMENTO
                    // L'inseguimento è verificato solo con i TGV già ruotati o nella medesima
                    // condizione di rotazione che fa presumere che si stanno mettendo in
                    // inseguimento.
                    // I controllo dovrebbero essere certi dal momento che il flag AgvInIseguimento
                    // non si alza mai se i TGV non si muovo nella stessa direzione
                    //-----------------------------------------------------------------------------
                    if( AgvInIseguimento && DistX>=DistRotazDoppiaMin ){
                       //----------------------------------------------------------
                       // TGVs già in orrizzontale
                       //----------------------------------------------------------
                       if((AgvConsideratoRettaX && !AgvConsiderato_InRot) &&
                          (AgvAdiacenteInRettaX && !AgvAdiacente_InRot)) { satellite_occupato=0; continue; }
                       //----------------------------------------------------------
                       // Verifico se i TGV ESCONO o ENTRANO in una condizione
                       // di inseguimento
                       //
                       // NOTA: Controllo su rette non parallele per quelle
                       //       parallele c'è già il cotrollo specifico prima.
                       //----------------------------------------------------------
                       if(DistY<DistRetteParallele){
                          //----------------------------------------------------------
                          // USCITA DA CONDIZIONE DI INSEGUIMENTO
                          // Verifico che i TGV siano già con la stessa rotazione
                          // e solo uno dei due debba ruotare/curvare
                          //----------------------------------------------------------
                          if(AgvConsideratoRettaX==true || AgvAdiacenteInRettaX==true){
                             if(AgvAdiacenteInRettaX==true && AgvAdiacente_InRot==0){
                                if(AgvConsiderato_RotAttuale==AgvAdiacente_RotAttuale) { satellite_occupato=0; continue; }
                             }
                             if(AgvConsideratoRettaX==true && AgvConsiderato_InRot==0){
                                if(AgvConsiderato_RotAttuale==AgvAdiacente_RotAttuale) { satellite_occupato=0; continue; }
                             }
                          }
                          //----------------------------------------------------------
                          // INGRESSO IN CONDIZIONE DI INSEGUIMENTO (SOLO ROTAZIONI)
                          // Verifico che entrambi i TGV si mettano in orrizzontale
                          //
                          // !!! CURVE NON AMMESSE NEL CONTROLLO !!!
                          // Infatti l'inseguimento può essere con "sorpasso" e
                          // cioè il TGV considerato si mette davanti al tgv adiacente,
                          // ma in questo caso l'ingombro in curva è maggiore.
                          //----------------------------------------------------------
                          if(AgvConsiderato_InRot==11 || AgvConsiderato_InRot==13){
                             //------------------------------------------------------------
                             // Entrambi in rotazione da fermi che si posizionano nello
                             // stesso modo
                             //------------------------------------------------------------
                             if(AgvConsiderato_InRot==AgvAdiacente_InRot) { satellite_occupato=0; continue; }
                             //------------------------------------------------------------
                             // Uno solo in rotazione e l'altro già in retta con la
                             // rotazione nello stesso verso che assumerà il primo
                             //------------------------------------------------------------
                             if(AgvConsideratoRettaX==true && AgvConsiderato_InRot==0){
                                if(AgvConsiderato_RotAttuale==1 && AgvAdiacente_InRot==11) { satellite_occupato=0; continue; }
                                if(AgvConsiderato_RotAttuale==3 && AgvAdiacente_InRot==13) { satellite_occupato=0; continue; }
                             }
                             if(AgvAdiacenteInRettaX==true && AgvAdiacente_InRot==0){
                                if(AgvAdiacente_RotAttuale==1 && AgvConsiderato_InRot==11) { satellite_occupato=0; continue; }
                                if(AgvAdiacente_RotAttuale==3 && AgvConsiderato_InRot==13) { satellite_occupato=0; continue; }
                             }
                          }
                          //----------------------------------------------------------
                          // INGRESSO IN CONDIZIONE DI INSEGUIMENTO (SOLO CURVE)
                          // Solo il TGV considerato entra in curva e l'altro
                          // deve essere già ruotato
                          //----------------------------------------------------------
                          if(AgvConsiderato_InRot==21 || AgvConsiderato_InRot==23){
                             //------------------------------------------------------------
                             // Il TGV adiacente deve essere già ruotato come il TGV
                             // considerato a fine curva
                             //------------------------------------------------------------
                             if(AgvAdiacenteInRettaX==true && AgvAdiacente_InRot==0){
                                if(AgvAdiacente_RotAttuale==1 && AgvConsiderato_InRot==21) { satellite_occupato=0; continue; }
                                if(AgvAdiacente_RotAttuale==3 && AgvConsiderato_InRot==23) { satellite_occupato=0; continue; }
                             }
                          }
                       }
                    }
                    if( AgvInIseguimento && DistY>=DistRotazDoppiaMin ){
                       //----------------------------------------------------------
                       // TGVs già in orrizzontale
                       //----------------------------------------------------------
                       if((AgvConsideratoRettaY && !AgvConsiderato_InRot) &&
                          (AgvAdiacenteInRettaY && !AgvAdiacente_InRot)) { satellite_occupato=0; continue; }
                       //----------------------------------------------------------
                       // Verifico se i TGV ESCONO o ENTRANO in una condizione
                       // di inseguimento
                       //
                       // NOTA: Controllo su rette non parallele per quelle
                       //       parallele c'è già il cotrollo specifico prima.
                       //----------------------------------------------------------
                       if(DistX<DistRetteParallele){
                          //----------------------------------------------------------
                          // USCITA DA CONDIZIONE DI INSEGUIMENTO
                          // Verifico che i TGV siano già con la stessa rotazione
                          // e solo uno dei due debba ruotare/curvare
                          //----------------------------------------------------------
                          if(AgvConsideratoRettaY==true || AgvAdiacenteInRettaY==true){
                             if(AgvAdiacenteInRettaY==true && AgvAdiacente_InRot==0){
                                if(AgvConsiderato_RotAttuale==AgvAdiacente_RotAttuale) { satellite_occupato=0; continue; }
                             }
                             if(AgvConsideratoRettaY==true && AgvConsiderato_InRot==0){
                                if(AgvConsiderato_RotAttuale==AgvAdiacente_RotAttuale) { satellite_occupato=0; continue; }
                             }
                          }
                          //----------------------------------------------------------
                          // INGRESSO IN CONDIZIONE DI INSEGUIMENTO
                          // Verifico che entrambi i TGV si mettano in orrizzontale
                          //
                          // !!! CURVE NON AMMESSE NEL CONTROLLO !!!
                          // Infatti l'inseguimento può essere con "sorpasso" e
                          // cioè il TGV considerato si mette davanti al tgv adiacente,
                          // ma in questo caso l'ingombro in curva è maggiore.
                          //----------------------------------------------------------
                          if(AgvConsiderato_InRot==10 || AgvConsiderato_InRot==12){
                             //------------------------------------------------------------
                             // Entrambi in rotazione da fermi che si posizionano nello
                             // stesso modo
                             //------------------------------------------------------------
                             if(AgvConsiderato_InRot==AgvAdiacente_InRot) { satellite_occupato=0; continue; }
                             //------------------------------------------------------------
                             // Uno solo in rotazione e l'eltro già in retta con la
                             // rotazione nello stesso verso che assumerà il primo
                             //------------------------------------------------------------
                             if(AgvConsideratoRettaY==true && AgvConsiderato_InRot==0){
                                if(AgvConsiderato_RotAttuale==0 && AgvAdiacente_InRot==10) { satellite_occupato=0; continue; }
                                if(AgvConsiderato_RotAttuale==2 && AgvAdiacente_InRot==12) { satellite_occupato=0; continue; }
                             }
                             if(AgvAdiacenteInRettaY==true && AgvAdiacente_InRot==0){
                                if(AgvAdiacente_RotAttuale==0 && AgvConsiderato_InRot==10) { satellite_occupato=0; continue; }
                                if(AgvAdiacente_RotAttuale==2 && AgvConsiderato_InRot==12) { satellite_occupato=0; continue; }
                             }
                          }
                          //----------------------------------------------------------
                          // INGRESSO IN CONDIZIONE DI INSEGUIMENTO (SOLO CURVE)
                          // Solo il TGV considerato entra in curva e l'altro
                          // deve essere già ruotato
                          //----------------------------------------------------------
                          if(AgvConsiderato_InRot==20 || AgvConsiderato_InRot==22){
                             //------------------------------------------------------------
                             // Il TGV adiacente deve essere già ruotato come il TGV
                             // considerato a fine curva
                             //------------------------------------------------------------
                             if(AgvAdiacenteInRettaY==true && AgvAdiacente_InRot==0){
                                if(AgvAdiacente_RotAttuale==0 && AgvConsiderato_InRot==20) { satellite_occupato=0; continue; }
                                if(AgvAdiacente_RotAttuale==2 && AgvConsiderato_InRot==22) { satellite_occupato=0; continue; }
                             }
                          }
                       }
                    }
                    //-----------------------------------------------------------------------------
                    // (6) Se DIST > DIST_MIN_ALLONTANAMENTO (STANDARD PER TGV IN MISSIONE)
                    //     Verifica se i TGV si stanno allontanando
                    //     allora --> OK
                    //
                    // VALIDA SOLO SULL'INDIETRO PER NAVETTE TIPO PALLET:
                    //
                    // (a) 13/12/2012  NEGLI IMPIANTI PALLET, L'ALLONTANAMENTO E DA CONSIDERARSI
                    //                 SOLO SULL'IDIETRO, perchè 2 tgv che si allontanano in
                    //                 direzione avanti hanno un ingombro di ROTAZIONE_MAX
                    //
                    // (b) AL-12/06/13 con gli agv in allontanamento ma solo uno in dir indietro
                    //                 la distanza da controllare è quella di RotDoppiaMin e
                    //                 l'agv_adicente deve essere in retta
                    //                 N.B.  (24/09/2013 solo su nodi fuori ingombro)
                    //
                    // (c1) AL-09/01/14 Se i TGV sono in allontanamento e non si incotreranno
                    //                  MAI e uno solo dei 2 compie rotazioni, se la distanza
                    //                  conplessiva è > DIST_MIN_ALLONTANAMENTO (su INDIETRO)
                    //
                    // (c2) AL-09/01/14 Se i TGV sono in allontanamento e non si incotreranno
                    //                  MAI e TUTTI E 2 non compiono rotazioni, se la distanza
                    //                  conplessiva è > DIST_MIN_ALLONTANAMENTO (su AVANTI)
                    //                  (!!! DA VERIFICARE: devono essere 2 tracce diverse a
                    //                                      una distanza almeno di rette parallele)
                    //
                    //-----------------------------------------------------------------------------
                    if(AgvInAllontanamento==true){
                       // SOLO SU INDIETRO
                       if(DirAllontanamento==1){
                          // (a)
                          if( DistX >= DistAllontanamento){
                             if(!AgvAdiacenteInRettaY && !AgvConsideratoRettaY && RotOpposteInX==true && DistY<=5 ) { satellite_occupato=0; continue; }
                          }
                          // (a)
                          if( DistY >= DistAllontanamento){
                             if(!AgvAdiacenteInRettaX && !AgvConsideratoRettaX && RotOpposteInY==true && DistX<=5 ) { satellite_occupato=0; continue; }
                          }
                          // (c1)
                          if(Dist>=DistAllontanamento && SenzaMaiIncontrarsi==true){
                             if((AgvAdiacenteInRettaX || AgvConsideratoRettaX) && RotOpposteInX==false) { satellite_occupato=0; continue; }
                             if((AgvAdiacenteInRettaY || AgvConsideratoRettaY) && RotOpposteInY==false) { satellite_occupato=0; continue; }
                          }
                       }
                       // SOLO UNO INDIETRO (con gestione nodi_fuori_ingombro)
                       if(DirAllontanamento==3){
                          // (b)
                          if(vicino_fuori_ingombro==true || considerato_fuori_ingombro==true){
                             // è sufficente verificare che l'altro_agv non debba ruotare che la dist in obliquo sia > DistRotazDoppiaMin
                             if( Dist >= DistRotazDoppiaMin && AgvAdiacente_InRot==0) { satellite_occupato=0; continue; }
                          }
                          // (c2)
                          if(Dist>=DistAllontanamento && SenzaMaiIncontrarsi==true){
                             if(DistY>=DistRetteParallele && (AgvAdiacenteInRettaX || AgvConsideratoRettaX)){
                                if(AgvAdiacenteInRettaY || AgvConsideratoRettaY) { satellite_occupato=0; continue; }
                             }
                             if(DistX>=DistRetteParallele && (AgvAdiacenteInRettaY || AgvConsideratoRettaY)){
                                if(AgvAdiacenteInRettaX || AgvConsideratoRettaX) { satellite_occupato=0; continue; }
                             }
                          }
                       }
                    }
                    //--------------------------------------------------------------------------
                    // (7) Se DIST < DIST_MIN_ROT_DOPPIA_MAX
                    //     E i 2 TGV si inseguono e uno ruota allora comunque --> SAT OCCUPATO
                    //--------------------------------------------------------------------------
                    if( DistX<DistRotazDoppiaMax ){
                       if( !AgvConsideratoRettaY || !AgvAdiacenteInRettaY ) break;
                    }
                    if( DistY<DistRotazDoppiaMax ){
                       if( !AgvConsideratoRettaX || !AgvAdiacenteInRettaX ) break;
                    }
                 }
              }
           }
        }
     }
  }
  //----------------------------------------
  // Se NODO o SATELLITE occupato --> BREAK
  //----------------------------------------
  NodoOccupabile  = 1;
  if((N.nodo_busy[NodoConsiderato] >0) && (N.nodo_busy[NodoConsiderato]!=NumAgv)){
     satellite_occupato = NodoConsiderato;
     agv_adiacente      = N.nodo_busy[NodoConsiderato];
  }
  //----------------------------------------
  // Nodo non occupabile
  //----------------------------------------
  if( satellite_occupato >0 ) NodoOccupabile = 0;
  if( NodoOccupabile == 0   ){
     NodoInterruzionePercorso   = NodoConsiderato;
     AgvAdiacente               = agv_adiacente ;
     //-----------------------------------------------
     // GESTIONE PER PT IN INGOMBRO
     // (ma forse è giusta sempre!!!!)
     // Se al nodo di interruzzione non corrisponde
     // un agv_adiacente lo setto controllando se
     // i 2 punti successivi al nodo di interruzione
     // li trovo occupati
     //-----------------------------------------------
     if(AnticipoPintPerPtInIngombro==true && agv_adiacente==0 && NodoInterruzionePercorso!=0){
        //------------------------------------------------------------
        // Scorro tutta la missione fino al NodoInterruzionePercorso
        //------------------------------------------------------------
        for(j=0; j<MAXPERCORSI; j++){
           if(AGV[ NumAgv ]->mission.punto[j]==0                       ) break;
           if(AGV[ NumAgv ]->mission.punto[j]!=NodoInterruzionePercorso) continue;
           //------------------------------------------------------------
           // agv_adicente su nodo successivo a quello di interruzione
           //------------------------------------------------------------
           if(N.nodo_busy[AGV[ NumAgv ]->mission.punto[j+1]]!=0 && N.nodo_busy[AGV[ NumAgv ]->mission.punto[j+1]]!=NumAgv){
              agv_adiacente = N.nodo_busy[AGV[ NumAgv ]->mission.punto[j+1]];
              break;
           }
           //------------------------------------------------------------
           // agv_adicente su nodo succ, succ a quello di interruzione
           //------------------------------------------------------------
           if(N.nodo_busy[AGV[ NumAgv ]->mission.punto[j+2]]!=0 && N.nodo_busy[AGV[ NumAgv ]->mission.punto[j+2]]!=NumAgv){
              agv_adiacente = N.nodo_busy[AGV[ NumAgv ]->mission.punto[j+2]];
              break;
           }
        }
        //------------------------------------------------------------
        // Setto come TGV bloccante quello che occupa i successivi
        // al nodo di interruzione
        //------------------------------------------------------------
        if(agv_adiacente!=0){
           AgvAdiacente = agv_adiacente;
        }
     }
  }
  return 0;
}
// ------------------------------------------------------------------------
//------------------------------------------------------------------
//  VerificaNodoOccupatoIngombroAgv( )
//------------------------------------------------------------------
//  Restituisce :
//
//     NodoOccupabile           -> 1 OK
//     satellite_occupato       -> Ha significato solo locale perchè poi viene reinizializzato in occupa_nodi()
//     NodoInterruzionePercorso -> 0...xx
//
//
int VerificaNodoOccupatoIngombroAgv(bool RioccupaNodo, int NumAgv, int NodoConsiderato, int &NodoOccupabile, int &AgvAdiacente, int &satellite_occupato, int &NodoInterruzionePercorso, int ScartaAgv, int SoloAgv)
{
  #define LAST_CICLO 2

  char all_mess[101]="";
  short int rot1, rot2, grd_altroagv;
  double Dist, DistX, DistY;
  int  x1, x2, y1, y2, ciclo;
  bool verifica_ingombro;
  bool deviazione_numagv;
  bool deviazione_altroagv;
  bool altroagv_su_percorso;
  bool agv_rot_180, altro_rot_180;
  short int rot_da_fermo_numagv, rot_da_fermo_altroagv;
  short int i, j, indice_pos, dirrot_numagv, prec_numagv;
  short int dirsucc_numagv, succ_numagv;
  short int dirrot_altroagv, punto_altroagv;
  short int prec_altroagv, succ_altro_agv;
  ingombroagv *IngombroAgv1, *IngombroAgv2;

  //--------------------------------------------------------------
  // Inizializzazione variabili
  //--------------------------------------------------------------
  AgvAdiacente=0;
  NodoOccupabile=1;
  satellite_occupato=0;
  agv_rot_180=altro_rot_180=false;
  NodoInterruzionePercorso=NodoConsiderato;
  deviazione_numagv=deviazione_altroagv=false;
  rot_da_fermo_numagv=rot_da_fermo_altroagv=999;
  //--------------------------------------------------------------
  // Scarto Nodi già occupati se non richiesto
  //--------------------------------------------------------------
  if(RioccupaNodo==false && N.nodo_busy[NodoConsiderato]==NumAgv) return 0;
  //--------------------------------------------------------------
  // TGV CONSIDERATO
  //--------------------------------------------------------------
  IngombroAgv1 = new ingombroagv(NumAgv);
  dirrot_numagv=0;
  prec_numagv=NodoConsiderato;
  dirsucc_numagv=succ_numagv=0;
  if(!AGV[NumAgv]->test_mission_on() || NodoConsiderato==AGV[NumAgv]->stato.pos){
     //-----------------------------------------------------------------------------------
     // AL-19/06/2020 - INGOMBRO AGV PER GESTIONE ROTAZIONE CALAMITE
     //-----------------------------------------------------------------------------------
     dirrot_numagv=AGV[NumAgv]->stato.rot_calamite;
     if(AGV[NumAgv]->mission.GestioneLaser==true) dirrot_numagv=AGV[NumAgv]->stato.rot_laser;
  }
  if(AGV[NumAgv]->test_mission_on() && NodoConsiderato!=AGV[NumAgv]->stato.pos){
     for(i=1; i<MAXPERCORSI; i++){
        if(AGV[NumAgv]->mission.punto[i]<1                ) break;
        if(AGV[NumAgv]->mission.punto[i]>TOTPUNTI         ) break;
        if(AGV[NumAgv]->mission.punto[i]!=NodoConsiderato ) continue;
        //----------------------------------------
        // Memorizzo nodo precendente
        //----------------------------------------
        deviazione_numagv=false;
        rot_da_fermo_numagv=999;
        dirsucc_numagv=succ_numagv=0;
        rot1=AGV[NumAgv]->mission.dati_perc[i].rot;
        if(AGV[NumAgv]->mission.punto[i-1]>0 && AGV[NumAgv]->mission.punto[i-1]<=TOTPUNTI){
           prec_numagv=AGV[NumAgv]->mission.punto[i-1];
           dirrot_numagv=AGV[NumAgv]->mission.dati_perc[i-1].dir;
           rot2=AGV[NumAgv]->mission.dati_perc[i-1].rot;
           //--------------------------------------------
           // AL-23/10/2020 DEVIAZIONE PRECEDENTE
           // Verifico rotazione standard di arrivo
           // per valutare rotazione da fermo
           //--------------------------------------------
           if(rot2>49) rot2=AGV[NumAgv]->rot_standard(i-1);
           //--------------------------------------------
           // NODO CONSIDERATO = DEVIAZIONE
           //--------------------------------------------
           if(rot1>49 && AGV[NumAgv]->mission.punto[i+1]>0 && AGV[NumAgv]->mission.punto[i+1]<=TOTPUNTI){
              deviazione_numagv=true;
              succ_numagv=AGV[NumAgv]->mission.dati_perc[i].ps;
              dirsucc_numagv=AGV[NumAgv]->mission.dati_perc[i].dir;
           }
           //--------------------------------------------
           // ROTAZIONI DA FERMO
           //--------------------------------------------
           if(rot1<4 && rot2<4 && rot1!=rot2){
              //--------------------------------------------
              // 180°
              //--------------------------------------------
              if(rot1==0 && rot2==2) agv_rot_180=true;
              if(rot1==1 && rot2==3) agv_rot_180=true;
              if(rot1==2 && rot2==0) agv_rot_180=true;
              if(rot1==3 && rot2==1) agv_rot_180=true;
              //--------------------------------------------
              // Rotazione finale TGV su punto considerato
              //--------------------------------------------
              if(rot1==0) rot_da_fermo_numagv=0;
              if(rot1==1) rot_da_fermo_numagv=90;
              if(rot1==2) rot_da_fermo_numagv=180;
              if(rot1==3) rot_da_fermo_numagv=270;
           }
        }
        break;
     }
  }
  //--------------------------------------------------------------
  // CICLI CONTROLLO INGOMBRO AGV
  //--------------------------------------------------------------
  ciclo=0;
  do{
     //--------------------------------------------------------------
     // (0) - TGV CONSIDERATO SU TRATTO o FERMO POSIZIONE
     //       (valuta rotazione a cui si trova il tgvo con cui
     //        arriva alla fine del tratto)
     //--------------------------------------------------------------
     if(ciclo==0){
        IngombroAgv1->AzzeraIngombroAgv();
        IngombroAgv1->SettaIngombroSuTratto(prec_numagv, NodoConsiderato, dirrot_numagv, all_mess);
     }
     //--------------------------------------------------------------
     // (1) - TGV CONSIDERATO IN ROTAZIONE/DEVIAZIONE DA FERMO
     //       (1a): nel caso di rotazione da fermo valuta
     //             rotazione finale
     //       (1b): nel caso di deviazione valuta anche tratto
     //             successivo
     //--------------------------------------------------------------
     if(ciclo==1){
        // (1a) - ROTAZIONE DA FERMO
        if(rot_da_fermo_numagv!=999){
           IngombroAgv1->AzzeraIngombroAgv();
           IngombroAgv1->SettaIngombroSuTratto(NodoConsiderato, NodoConsiderato, rot_da_fermo_numagv, all_mess);
        }
        // (1b) - DEVIAZIONE DA FERMO
        if(deviazione_numagv==true){
           IngombroAgv1->AzzeraIngombroAgv();
           IngombroAgv1->SettaIngombroSuTratto(NodoConsiderato, succ_numagv, dirsucc_numagv, all_mess);
        }
     }
     //--------------------------------------------------------------
     // TUTTI ALTRI TGV
     //--------------------------------------------------------------
     for(j=1; j<=MAXAGV; j++){
        if(j==NumAgv) continue;
        //--------------------------------------------------
        // Verifica occupabilità per SPECIFICO TGV
        //--------------------------------------------------
        if(SoloAgv>0 && SoloAgv<=MAXAGV){
           if(j!=SoloAgv) continue;
        }
        //--------------------------------------------------
        // Verifica occupabilità per tutti i TGV tranne uno
        //--------------------------------------------------
        if(ScartaAgv>0 && ScartaAgv<=MAXAGV){
           if(j==ScartaAgv) continue;
        }
        //--------------------------------------------------
        // Costruttore classe Ingombro altro Agv
        //--------------------------------------------------
        IngombroAgv2 = new ingombroagv(j);
        dirrot_altroagv=0;
        // ALTRO AGV FERMO
        //----------------------------------------------------------------------
        // AL-31/03/2022 Controllo il Pend per sapere se il TGV è in movimento
        //               perchè in manuale con nodi occupati il Pstart viene
        //               azzerato, ma l'altro_tgv deve restare lontano
        //----------------------------------------------------------------------
      //if(!AGV[j]->mission.pstart || AGV[j]->mission.pintend==AGV[j]->stato.pos || AGV[j]->mission.pend==AGV[j]->stato.pos){
        if(AGV[j]->mission.pend==0 || AGV[j]->mission.pintend==AGV[j]->stato.pos || AGV[j]->mission.pend==AGV[j]->stato.pos){
           //-----------------------------------------------------------------------------------
           // AL-19/06/2020 - INGOMBRO AGV PER GESTIONE ROTAZIONE CALAMITE
           //-----------------------------------------------------------------------------------
           dirrot_altroagv=AGV[j]->stato.rot_calamite;
           if(AGV[j]->mission.GestioneLaser==true) dirrot_altroagv=AGV[j]->stato.rot_laser;
           prec_altroagv=AGV[j]->stato.pos;
           punto_altroagv=AGV[j]->stato.pos;
           //-----------------------------------------------------------------------------------
           // AL-07/08/2020 - ALTRO AGV CONSIDERATO SU PERCORSO ANCHE SU PUNTI NOTEVOLI
           //                 se pt_in_ingombro o se PINTEND che non coincide con PEND o PSTART
           //                 per gestire multi-nodo (es. celle e/o magazzini)
           //-----------------------------------------------------------------------------------
           altroagv_su_percorso=false;
         //if(!N.punti_notevoli[punto_altroagv] || (N.punti_notevoli[punto_altroagv]==true && N.pt_in_ingombro[punto_altroagv]==true)) altroagv_su_percorso=true
           if(!N.punti_notevoli[punto_altroagv]                                                ) altroagv_su_percorso=true;
           if(N.punti_notevoli[punto_altroagv]==true && N.pt_in_ingombro[punto_altroagv]==true ) altroagv_su_percorso=true;
           if(N.punti_notevoli[punto_altroagv]==true && AGV[j]->mission.pintend==punto_altroagv &&
                                                        AGV[j]->mission.pintend!=AGV[j]->mission.pend &&
                                                        AGV[j]->mission.pintend!=AGV[j]->mission.pstart) altroagv_su_percorso=true;
           if(altroagv_su_percorso==true){
              IngombroAgv2->AzzeraIngombroAgv();
              IngombroAgv2->SettaIngombroSuTratto(prec_altroagv, punto_altroagv, dirrot_altroagv, all_mess);
              //-------------------------------------------------------
              // Verifica intersezione TGV
              //-------------------------------------------------------
              if(IngombroAgv1->VerificaIntersezioneAgv(IngombroAgv2, all_mess)){
                  AgvAdiacente=j;
                  NodoOccupabile=0;
                  satellite_occupato=punto_altroagv;
                  NodoInterruzionePercorso=NodoConsiderato;
              }
              else{
                 //-------------------------------------------------------
                 // TGV CONSIDERATO IN 180° DA FERMO (!! SICUREZZA !!)
                 // Ci deve essere la DIST_MIN_ROT_DOPPIA_MAX perchè
                 // sia a inizio che a fine rotazione IngobroAgv potrebbe
                 // non esserci, ma esserci durante.
                 //-------------------------------------------------------
                 if(agv_rot_180==true){
                    x1 = Punti[ NodoConsiderato ][0];
                    y1 = Punti[ NodoConsiderato ][1];
                    x2 = Punti[ punto_altroagv ][0];
                    y2 = Punti[ punto_altroagv ][1];
                    DistX = (double) abs(x1-x2);
                    DistY = (double) abs(y1-y2);
                    if(DistX>30000 || DistY>30000) continue;
                    Dist = (double)sqrt_algo((DistX*DistX)+(DistY*DistY));
                    //--------------------------------------------------
                    // Distanza, maggiore rotazione doppia --> continue
                    //--------------------------------------------------
                    if(Dist<DIST_MIN_ROT_DOPPIA_MAX){
                        AgvAdiacente=j;
                        NodoOccupabile=0;
                        satellite_occupato=punto_altroagv;
                        NodoInterruzionePercorso=NodoConsiderato;
                    }
                 }
              }
           }
        }
        // ALTRO AGV IN MOVIMENTO
      //if(AGV[j]->mission.pstart && AGV[j]->mission.pintend!=AGV[j]->stato.pos){
        //----------------------------------------------------------------------
        // AL-21/07/2020 TGV considerato in movimento se:
        // 1. punto successivo alla posizione attuale già occupato
        // 2. su punto intermedio, prossimo nodo non occupato, ma senza bit di
        //    tgv su nodo (rotazione in corso tranne che se pint=pend)
        //
        // AL-31/03/2022 Controllo il Pend per sapere se il TGV è in movimento
        //               perchè in manuale con nodi occupati il Pstart viene
        //               azzerato, ma l'altro_tgv deve restare lontano
        //----------------------------------------------------------------------
      //if(AGV[j]->mission.pstart){
        if(AGV[j]->mission.pend){
           indice_pos=0;
           for(i=0; i<MAXPERCORSI; i++){
              if(AGV[j]->mission.punto[i]<1                 ) break;
              if(AGV[j]->mission.punto[i]>TOTPUNTI          ) break;
              if(AGV[j]->mission.punto[i]!=AGV[j]->stato.pos) continue;
              indice_pos=i;
              break;
           }
           if(N.nodo_busy[AGV[j]->mission.punto[indice_pos+1]]==j || (AGV[j]->stato.pos==AGV[j]->mission.pintend && AGV[j]->mission.pintend!=AGV[j]->mission.pend && !AGV[j]->stato.s.bit.sunod)){
              for(i=indice_pos; i<MAXPERCORSI; i++){
                 if(AGV[j]->mission.punto[i]<1        ) break;
                 if(AGV[j]->mission.punto[i]>TOTPUNTI ) break;
                 //--------------------------------------------------------------------
                 // AL-25/01/2022 fermo il controllo
                 // all'ultimo tratto con almeno un
                 // nodo occupatoù
                 // LAMMO - 27/04/2022 (3 giorni prima del mio compleanno)
                 // Controllo che non ci siano comunque altri nodi occupati dopo
                 // quello intermedio
                 //-------------------------------------------------------------------
                 //if(i>1 && AGV[j]->mission.punto[i-1]==AGV[j]->mission.pintend) break;
                 if(i>1 && AGV[j]->mission.punto[i-1]==AGV[j]->mission.pintend && N.nodo_busy[AGV[j]->mission.punto[i]]!=j) break;
                 //----------------------------------------
                 // Memorizzo nodo precendente
                 //----------------------------------------
                 prec_altroagv=0;
                 succ_altro_agv=0;
                 altro_rot_180=false;
                 deviazione_altroagv=false;
                 rot_da_fermo_altroagv=999;
                 punto_altroagv=AGV[j]->mission.punto[i];
                 dirrot_altroagv=AGV[j]->mission.dati_perc[i].dir;
                 if(i<MAXPERCORSI-1) succ_altro_agv=AGV[j]->mission.punto[i+1];
                 rot1 = AGV[j]->mission.dati_perc[i].rot;
                 //----------------------------------------
                 // Gradi altra navetta
                 //----------------------------------------
                 grd_altroagv=AGV[j]->stato.rot_calamite;
                 if(AGV[j]->mission.GestioneLaser==true) grd_altroagv=AGV[j]->stato.rot_laser;
                 //----------------------------------------
                 // Lungo perocorso
                 //----------------------------------------
                 if(i>0){
                    prec_altroagv=AGV[j]->mission.punto[i-1];
                    dirrot_altroagv=AGV[j]->mission.dati_perc[i-1].dir;
                    rot2 = AGV[j]->mission.dati_perc[i-1].rot;
                    //--------------------------------------------
                    // AL-23/10/2020 DEVIAZIONE PRECEDENTE
                    // Verifico rotazione standard di arrivo
                    // per valutare rotazione da fermo
                    //--------------------------------------------
                    if(rot2>49) rot2=AGV[j]->rot_standard(i-1);
                    //--------------------------------------------
                    // DEVIAZIONE DA FERMO SU NODO
                    //--------------------------------------------
                    if(rot1>49 && AGV[j]->mission.pintend==punto_altroagv && N.nodo_busy[succ_altro_agv]!=j ) deviazione_altroagv=true;
                    if(rot1>49 && AGV[j]->mission.pintend==AGV[j]->stato.pos && !AGV[j]->stato.s.bit.sunod  ) deviazione_altroagv=true;
                    //--------------------------------------------
                    // AL-09/03/2022
                    // Uso anche l'off_da_nodo==0 per sapere che
                    // la navetta è ferma su nodo di deviazione
                    //--------------------------------------------
                    if(rot1>49 && AGV[j]->stato.offset_da_nodo==0 && !AGV[j]->stato.s.bit.sunod             ) deviazione_altroagv=true;
                    //--------------------------------------------
                    // ROTAZIONI DA FERMO
                    //--------------------------------------------
                    if(rot1<4 && rot2<4 && rot1!=rot2){
                       //--------------------------------------------
                       // 180°
                       //--------------------------------------------
                       if(rot1==0 && rot2==2) altro_rot_180=true;
                       if(rot1==1 && rot2==3) altro_rot_180=true;
                       if(rot1==2 && rot2==0) altro_rot_180=true;
                       if(rot1==3 && rot2==1) altro_rot_180=true;
                       //--------------------------------------------
                       // Rotaz. finale ALTRO AGV punto considerato
                       //--------------------------------------------
                       if(rot1==0) rot_da_fermo_altroagv=0;
                       if(rot1==1) rot_da_fermo_altroagv=90;
                       if(rot1==2) rot_da_fermo_altroagv=180;
                       if(rot1==3) rot_da_fermo_altroagv=270;
                    }
                 }
                 //----------------------------------------
                 // Verifica ingombro ROTAZIONI
                 //----------------------------------------
                 if(prec_altroagv>0 && prec_altroagv<=TOTPUNTI){
                    //-------------------------------------------------------
                    // 180° DA FERMO + ALTRO AGV IN DEVIAZIONE
                    // Se uno delle 2 navette copie una rotazione a 180°
                    // ci deve essere la DIST_MIN_ROT_DOPPIA_MAX perchè
                    // sia a inizio che a fine rotazione IngobroAgv potrebbe
                    // non esserci, ma esserci durante.
                    //-------------------------------------------------------
                    if(agv_rot_180==true || altro_rot_180==true || deviazione_altroagv==true){
                       x1 = Punti[ NodoConsiderato ][0];
                       y1 = Punti[ NodoConsiderato ][1];
                       x2 = Punti[ punto_altroagv ][0];
                       y2 = Punti[ punto_altroagv ][1];
                       DistX = (double) abs(x1-x2);
                       DistY = (double) abs(y1-y2);
                       if(DistX>30000 || DistY>30000) continue;
                       Dist = (double)sqrt_algo((DistX*DistX)+(DistY*DistY));
                       //--------------------------------------------------
                       // Distanza, maggiore rotazione doppia --> continue
                       //--------------------------------------------------
                       if(Dist<DIST_MIN_ROT_DOPPIA_MAX){
                           AgvAdiacente=j;
                           NodoOccupabile=0;
                           satellite_occupato=punto_altroagv;
                           NodoInterruzionePercorso=NodoConsiderato;
                           break;
                       }
                    }
                    //-------------------------------------------------------
                    // ALTRO TGV IN ROTAZIONE DA FERMO (ma non 180°)
                    // controllo sia inizio che fine rotazione
                    //-------------------------------------------------------
                    if(rot_da_fermo_altroagv!=999){
                       IngombroAgv2->AzzeraIngombroAgv();
                       IngombroAgv2->SettaIngombroSuTratto(punto_altroagv, punto_altroagv, rot_da_fermo_altroagv, all_mess);
                       //-------------------------------------------------------------------
                       // Verifico interferenza navette
                       //-------------------------------------------------------------------
                       if(IngombroAgv1->VerificaIntersezioneAgv(IngombroAgv2, all_mess)){
                           AgvAdiacente=j;
                           NodoOccupabile=0;
                           satellite_occupato=punto_altroagv;
                           NodoInterruzionePercorso=NodoConsiderato;
                           break;
                       }
                    }
                 }
                 //-------------------------------------------------------
                 // INGOMBRI AGV SU TRATTO
                 //-------------------------------------------------------
                 verifica_ingombro=false;
                 IngombroAgv2->AzzeraIngombroAgv();
                 if(punto_altroagv==AGV[j]->stato.pos && (rot_da_fermo_altroagv==999 || AGV[j]->stato.s.bit.sunod || (rot_da_fermo_altroagv!=999 && rot_da_fermo_altroagv==grd_altroagv))){
                    //----------------------------------------
                    // Nodo di percorso o pt_in_ingombro
                    //----------------------------------------
                    if(!N.punti_notevoli[punto_altroagv] || (N.punti_notevoli[punto_altroagv]==true && N.pt_in_ingombro[punto_altroagv]==true)){
                       //----------------------------------------
                       // Posizione attuale del TGV
                       // 1. TGV fermo
                       // 2. TGV in movimento
                       //----------------------------------------
                       if(N.nodo_busy[succ_altro_agv]!=j){
                          //-----------------------------------------------------------------------------------
                          // AL-19/06/2020 - INGOMBRO AGV PER GESTIONE ROTAZIONE CALAMITE
                          //-----------------------------------------------------------------------------------
                          dirrot_altroagv=AGV[j]->stato.rot_calamite;
                          if(AGV[j]->mission.GestioneLaser==true) dirrot_altroagv=AGV[j]->stato.rot_laser;
                          succ_altro_agv=AGV[j]->stato.pos;
                          punto_altroagv=AGV[j]->stato.pos;
                       }
                       IngombroAgv2->SettaIngombroSuTratto(punto_altroagv, succ_altro_agv, dirrot_altroagv, all_mess);
                       verifica_ingombro=true;
                    }
                 }
                 else{
                    IngombroAgv2->SettaIngombroSuTratto(prec_altroagv, punto_altroagv, dirrot_altroagv, all_mess);
                    verifica_ingombro=true;
                 }
                 //-------------------------------------------------------
                 // Verifica intersezione TGV
                 //-------------------------------------------------------
                 if(verifica_ingombro==true && IngombroAgv1->VerificaIntersezioneAgv(IngombroAgv2, all_mess)){
                     AgvAdiacente=j;
                     NodoOccupabile=0;
                     satellite_occupato=punto_altroagv;
                     NodoInterruzionePercorso=NodoConsiderato;
                     break;
                 }
                 //-------------------------------------------------------
                 // Interrompo il ciclo al PINT
                 //
                 // AL-08/07/2020 controllo anche che il successivo non
                 // sia già occupato per gestire uscita e rientro da mag
                 //-------------------------------------------------------
               //if(AGV[j]->mission.punto[i]==AGV[j]->mission.pintend) break;
                 if(AGV[j]->mission.punto[i]==AGV[j]->mission.pintend && (succ_altro_agv==0 || succ_altro_agv==AGV[j]->stato.pos || N.nodo_busy[succ_altro_agv]!=j)) break;
              }
           }
        }
        delete IngombroAgv2;
        if(NodoOccupabile==0) break;
     }
     //---------------------------------------------
     // Non è prevista rotazione da FERMO -> skip
     //---------------------------------------------
     ciclo++;
     if(rot_da_fermo_numagv==999 && deviazione_numagv==false) ciclo=LAST_CICLO;
  }while(NodoOccupabile==1 && ciclo<LAST_CICLO);
  delete IngombroAgv1;
  return 0;
}
// ------------------------------------------------------------------------
//------------------------------------------------------------------
//  VerificaNodoOccupatoRP( )
//------------------------------------------------------------------
//  Verifica NODO Occupato e nodo vicino x rette parallele
//
//  Restituisce :
//
//     NodoOccupabile           -> 1 OK
//     satellite_occupato       -> Ha significato solo locale perchè poi viene reinizializzato in occupa_nodi()
//     NodoInterruzionePercorso -> 0...xx
//
//
int VerificaNodoOccupatoRP(int NumAgv, int NodoConsiderato,
                           int & NodoOccupabile, int & AgvAdiacente, int & satellite_occupato, int & NodoInterruzionePercorso )
{
  int agv_adiacente;
  int x1, y1;

  int Vicino,
      Dist;
  int i, xv, yv;

  bool AgvAdiacenteInMovimento;
  double CalcolaRad, CatetoX, CatetoY;

  int         SempreON=1;

  //-----------------------------------------------
  // Inizializza Variabili
  //-----------------------------------------------
  if( NodoConsiderato <= 0       ) return 1;
  if( NodoConsiderato > TOTPUNTI ) return 1;
//  dest = AGV[ NumAgv ]->mission.pend;

  //----------------------------
  // (G3) INIZIO
  //----------------------------
  if( SempreON ){
     agv_adiacente=0;
     satellite_occupato=0;
     //---------------------------------------------------------
     // (G3) Se il NODO è libero verifica se quelli circostanti
     //      siano altrettanto liberi
     //---------------------------------------------------------
     if(N.nodo_busy[NodoConsiderato] == 0){
        //-------------------------------------------------------
        // (G3_a) Estrai le coordinate del nodo da occupare (x1,y1)
        //-------------------------------------------------------
        x1 = Punti[ NodoConsiderato ][0];
        y1 = Punti[ NodoConsiderato ][1];
        //---------------------------------------------
        // (G3_b) Valuta i nodi adiacenti
        //---------------------------------------------
        for( Vicino=10; Vicino<=TOTPUNTI ; Vicino++){
           if(NodoConsiderato == Vicino        ) continue;
           if(N.n[ Vicino ]->num ==0           ) continue;
           if(N.punti_notevoli[ Vicino ]==true ) continue;   // Punto terminale non a rischio
           //------------------------------------------------------------------------------
           // Estrai le coordinate del nodo VICINO e se DISTANZA > 300
           // --> CONTINUA
           //------------------------------------------------------------------------------
           xv = Punti[ Vicino ][0];
           yv = Punti[ Vicino ][1];
           CatetoX = (double) abs(x1-xv);
           CatetoY = (double) abs(y1-yv);
           if(CatetoX>600 || CatetoY>600) continue;
           CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
           Dist = (int) sqrt_algo( CalcolaRad );
           if( Dist> 300 ) continue;
           //------------------------------------------------
           // Altrimenti verifica se nodo SATELLITE OCCUPATO
           //------------------------------------------------
           if((N.nodo_busy[Vicino]!=0) && (N.nodo_busy[Vicino]!=NumAgv) && (N.nodo_busy[Vicino]<=MAXAGV)){
              agv_adiacente = N.nodo_busy[Vicino];
              //--------------------------------------------------------------------------
              // Se l'AGV ADIACENTE HA ALTRI NODI OCCUPATI OLTRE AL VICINO vuol dire che
              // cambierà posizione e non lo considero effettivamente in ingombro
              //--------------------------------------------------------------------------
              AgvAdiacenteInMovimento=false;
              for(i=0; i<MAXPERCORSI; i++){
                 if(AGV[agv_adiacente]->mission.punto[i]==0     ) break;
                 if(AGV[agv_adiacente]->mission.punto[i]!=Vicino) continue;
                 //----------------------------------------------------------
                 // Nodo successivo al VICINO occupato dal AGV ADIACENTE
                 //----------------------------------------------------------
                 if(N.nodo_busy[AGV[agv_adiacente]->mission.punto[i+1]]==agv_adiacente){
                    //-------------------------------------------------------------------------------
                    // 4/05/2005 G. Back19 Il TGV adiacente deve già essere già sul nodo considerato
                    //-------------------------------------------------------------------------------
                    if( AGV[agv_adiacente]->stato.pos == Vicino){
                       AgvAdiacenteInMovimento=true;
                       //break;
                    }
                 }
                 //---------------------------------------------------------------------------
                 // 4/05/2005 G. Back19 Nodo successivo al VICINO occupato dal AGV ADIACENTE
                 //---------------------------------------------------------------------------
                 if(N.nodo_busy[AGV[agv_adiacente]->mission.punto[i+2]]==agv_adiacente){
                    //-------------------------------------------------------------------------------
                    // 4/05/2005 G. Back19 Il TGV adiacente deve già essere già sul nodo considerato
                    //-------------------------------------------------------------------------------
                    if( AGV[agv_adiacente]->stato.pos == AGV[agv_adiacente]->mission.punto[i+1]){
                       AgvAdiacenteInMovimento=true;
                       //break;
                    }
                 }
              }
              if(AgvAdiacenteInMovimento==true) continue;
              //--------------------------------------------------------------------------
              // Controlla AGV ADIACENTE se sta entrando nodo considerato
              // (nodo considerato = prenotato)
              //--------------------------------------------------------------------------
            //if( nodi_prenotati[ NodoConsiderato ][agv_adiacente-1]!=agv_adiacente) continue;
              //--------------------------------------------------------------------------
              // Di default --> SATELLITE OCCUPATO
              //--------------------------------------------------------------------------
              satellite_occupato = Vicino;
              break;
           }
        }
     }
  }
  //-----------------------------------------------
  // Se NODO o SATELLITE occupato --> BREAK
  //-----------------------------------------------
  NodoOccupabile  = 1;
  if((N.nodo_busy[NodoConsiderato] >0) && (N.nodo_busy[NodoConsiderato]!=NumAgv)){
     agv_adiacente      = N.nodo_busy[NodoConsiderato];
     //--------------------------------------------------------------------------
     // Se l'AGV ADIACENTE HA ALTRI NODI OCCUPATI OLTRE AL VICINO vuol dire che
     // cambierà posizione e non lo considero effettivamente in ingombro
     //--------------------------------------------------------------------------
     AgvAdiacenteInMovimento=false;
     for(i=0; i<MAXPERCORSI; i++){
        if(AGV[agv_adiacente]->mission.punto[i]==0) break;
        if(AGV[agv_adiacente]->mission.punto[i]!=NodoConsiderato) continue;
        //----------------------------------------------------------
        // Nodo successivo al VICINO occupato dal AGV ADIACENTE
        //----------------------------------------------------------
        if(N.nodo_busy[AGV[agv_adiacente]->mission.punto[i+1]]==agv_adiacente){
           //-------------------------------------------------------------------------------
           // 4/05/2005 G. Back19 Il TGV adiacente deve già essere già sul nodo considerato
           //-------------------------------------------------------------------------------
           if( AGV[agv_adiacente]->stato.pos == NodoConsiderato){
              AgvAdiacenteInMovimento=true;
              break;
           }
        }
        //---------------------------------------------------------------------------
        // 4/05/2005 G. Back19 Nodo successivo al VICINO occupato dal AGV ADIACENTE
        //---------------------------------------------------------------------------
        if(N.nodo_busy[AGV[agv_adiacente]->mission.punto[i+2]]==agv_adiacente){
           //-------------------------------------------------------------------------------
           // 4/05/2005 G. Back19 Il TGV adiacente deve già essere già sul nodo considerato
           //-------------------------------------------------------------------------------
           if( AGV[agv_adiacente]->stato.pos == AGV[agv_adiacente]->mission.punto[i+1]){
              AgvAdiacenteInMovimento=true;
              break;
           }
        }
     }
     if(AgvAdiacenteInMovimento==false) satellite_occupato = NodoConsiderato;
  }
  //----------------------------------------
  // Nodo non occupabile
  //----------------------------------------
  if( satellite_occupato >0 ) NodoOccupabile = 0;
  if( NodoOccupabile == 0   ){
     NodoInterruzionePercorso = NodoConsiderato;
     AgvAdiacente             = agv_adiacente ;
  }

  return 0;
}
// ------------------------------------------------------------------------
//------------------------------------------------------------------
//  VerificaNodoPrenotato( )
//------------------------------------------------------------------
//  Restituisce :
//
//     NodoOccupabile           -> 1 OK
//     satellite_occupato       -> Ha significato solo locale perchè poi viene reinizializzato in occupa_nodi()
//     NodoInterruzionePercorso -> 0...xx
//
//
int VerificaNodoPrenotato(int NumAgv, int NodoConsiderato, int &NodoOccupabile, int &AgvAdiacente, int &satellite_occupato, int &NodoInterruzionePercorso )
{
  int    DistX, DistY;
  double Dist;
  int    PuntoPrenotato;
  bool   zona_fuori_ingombro_manuale;
  int    PuntoSucessivo, PuntoSuccSucc;//, PuntoSucessivoSucessSucess;
  bool   StessaDirezione, altro_tgv_rot180, mantieni_dist_sicurezza;
  double Dist1, Dist2, Dist3, Dist4;
  double DistMAX=5;    // distanza ammessa tra 2 nodi per essere considerati sovrapposti
  int    altro_agv;
  int    AgvConsideratoInRot, AltroAgvInRot;
  int    SempreON=1;
  int    Vicino, j, k;
  int    ptr_percorso, ptrpath_adiacente, rot1, rot2;
  int    satellite_occupato1, NodoInterruzionePercorso1, ptr_percorso1;
  int    NodoCosiderato1, NodoOccupabile1, AgvPerInterruzionePercorso1;
//int    AltroAgv_NodoOccupato;


  //--------------------------------------------------------------
  // Inizializza Variabili
  //--------------------------------------------------------------
//NodoOccupabile    = 0;
//AgvAdiacente      = 0;
//satellite_occupato= 0;
  if( NodoConsiderato <= 0       ) return 1;
  if( NodoConsiderato > TOTPUNTI ) return 1;


  //--------------------------------------------------------------
  // VERSIOONE VERIFICA NODI PRENOTATI CON FUORI INGOMBRO
  //--------------------------------------------------------------
  if(AbilitaVerificaZonaFuoriIngombro==false){
     VerificaNodoPrenotatoConFuoriIng(NumAgv, NodoConsiderato, NodoOccupabile, AgvAdiacente, satellite_occupato, NodoInterruzionePercorso );
     return 0;
  }

  //--------------------------------------------------------------
  // Nodo destinazione (salto controllo prenotati) -> occupabile
  //--------------------------------------------------------------
  if(NodoConsiderato==AGV[NumAgv]->mission.pend && NodoConsiderato==NodoInterruzionePercorso){
     NodoOccupabile=1;
     return 0;
  }

  //--------------------------------------------------------------
  // GESTIONE SALTO NODO TGV LASER
  // Se si tratta di una navetta laser i salti nodi possono
  // essere di diversi cm per esigenze di guida, quindi i nodi
  // vengono ritenuti sovrapposti con tolleranze maggioni.
  //--------------------------------------------------------------
  if(AGV[NumAgv]->mission.GestioneLaser==true) DistMAX = DIST_MAX_SALTO_NODI+20;
  //--------------------------------------------------------------
  // (G4) Se il percorso non è completamente libero
  //      occupa comunque fino al primo nodo prenotato
  //      da altro AGVN.B.
  //--------------------------------------------------------------
  if( SempreON ){
     //--------------------------------------------------------------------------------
     // Cerca punto del percorso all'interno della missione
     // se NON presente --> SKIP
     //--------------------------------------------------------------------------------
     for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
        if(AGV[ NumAgv ]->mission.punto[ptr_percorso]==NodoConsiderato) break;
        if(AGV[ NumAgv ]->mission.punto[ptr_percorso]==0) break;
     }
     if(AGV[ NumAgv ]->mission.punto[ptr_percorso]==0) return 1;
     PuntoSucessivo = AGV[ NumAgv ]->mission.punto[ptr_percorso+1];
     PuntoSuccSucc  = AGV[ NumAgv ]->mission.punto[ptr_percorso+2];
     //------------------------------------------------------
     // Verifica se esiste un nodo prenotato
     //------------------------------------------------------
     for(altro_agv=1; altro_agv<=MAXAGV; altro_agv++){
        if(altro_agv==NumAgv) continue;
        if(!AGV[altro_agv]->test_mission_on()) continue;
        //---------------------------------------------------
        // Indice punto intermedio ALTRO TGV
        //---------------------------------------------------
        StessaDirezione=true;
        ptrpath_adiacente=0;
        for(j=1; j<MAXPERCORSI; j++){
           if(AGV[altro_agv]->mission.punto[j]==0                               ) break;
           if(AGV[altro_agv]->mission.punto[j]==AGV[altro_agv]->mission.pend    ) break;
           if(AGV[altro_agv]->mission.punto[j]!=AGV[altro_agv]->mission.pintend ) continue;
           ptrpath_adiacente=j;
           break;
        }
        //---------------------------------------------------
        // Se il TGV NON incontra almeno 2 NODI percorsi in
        // senso opposto --> CONTINUE
        // P.S. 1 solo nodo può essere un AGV che attraversa
        //      perpendicolarmente 1 incrocio
        //---------------------------------------------------
        for(j=0; j<MAXPERCORSI; j++){
           if(StessaDirezione==false                       ) break;
           if(AGV[altro_agv]->mission.punto_prenotato[j]==0) break;
           //-------------------------------------------------------------------
           // Calcolo distanza tra nodi prenotati
           //-------------------------------------------------------------------
           Dist1=0;
           if(N.VerificaDistanzaPunti(PuntoSucessivo,  AGV[altro_agv]->mission.punto_prenotato[j  ], &DistX, &DistY, &Dist1)) Dist1=9999;
           Dist2=0;
           if(N.VerificaDistanzaPunti(NodoConsiderato, AGV[altro_agv]->mission.punto_prenotato[j+1], &DistX, &DistY, &Dist2)) Dist2=9999;
           Dist3=0;
           if(N.VerificaDistanzaPunti(PuntoSuccSucc,   AGV[altro_agv]->mission.punto_prenotato[j  ], &DistX, &DistY, &Dist3)) Dist3=9999;
           Dist4=0;
           if(N.VerificaDistanzaPunti(PuntoSucessivo,  AGV[altro_agv]->mission.punto_prenotato[j+1], &DistX, &DistY, &Dist4)) Dist4=9999;
         // AL-03/09/2015 Ammessa tolleranza
         //if((AGV[altro_agv]->mission.punto_prenotato[j]==PuntoSucessivo && AGV[altro_agv]->mission.punto_prenotato[j+1]==NodoConsiderato) ||
         //   (AGV[altro_agv]->mission.punto_prenotato[j]==PuntoSuccSucc  && AGV[altro_agv]->mission.punto_prenotato[j+1]==PuntoSucessivo)){
           if((Dist1<=DistMAX && Dist2<=DistMAX) || (Dist3<=DistMAX && Dist4<=DistMAX)){
              //---------------------------------------------------------------------------
              // AL-18/06 RITARDO PARTENZA TGV DA PT
              // Aumento i controlli in partenza dai punti terminali
              // (nessun nodo prenotato in senso opposto)
              //---------------------------------------------------------------------------
              #ifdef RITARDO_PARTENZA_TGV_DA_PT
                 if(N.punti_notevoli[AGV[NumAgv]->stato.pos]==true && AGV[NumAgv]->stato.start==0) StessaDirezione=false;
              #endif;
              //---------------------------------------------------------------------------
              // Se il punto sucessivo o quello considerato sono già occupati da altro_tgv
              // allora l'ingombro è sicuro
              //---------------------------------------------------------------------------
              if(N.nodo_busy[PuntoSucessivo]==altro_agv || N.nodo_busy[NodoConsiderato]==altro_agv){
                 StessaDirezione=false;
              }
              //---------------------------------------------------------------------------
              // Se proseguendo nella missione del TGV trovo almeno un nodo occupato
              // da alto_agv allora le due missioni si intralciano altrimenti NO.
              //---------------------------------------------------------------------------
              for(k=ptr_percorso; k<MAXPERCORSI; k++){
                 if(AGV[NumAgv]->mission.punto[k]==0) break;
                 PuntoPrenotato = AGV[NumAgv]->mission.punto[k];
                 if(N.nodo_busy[PuntoPrenotato]==altro_agv) StessaDirezione=false;
                 else{
                    //----------------------------------------------------------------------
                    // Verifico che nemmeno tutti i nodi Vicini a quello considerato
                    //----------------------------------------------------------------------
                    for(Vicino=10; Vicino<=TOTPUNTI; Vicino++){
                       if(PuntoPrenotato==Vicino        ) continue;
                       if(N.punti_notevoli[Vicino]==true) continue;
                       if(N.n[Vicino]->num==0           ) continue;
                       if(N.nodo_busy[Vicino]!=altro_agv) continue;
                       //-------------------------------------------
                       // Se la distanza e superiore a quella di
                       // ROTAZIONE_DOPPIA_MAX --> Skip!
                       //-------------------------------------------
                       if(N.VerificaDistanzaPunti(PuntoPrenotato, Vicino, &DistX, &DistY, &Dist)) continue;
                       if(Dist>=DIST_MIN_ROT_DOPPIA_MAX) continue;
                       StessaDirezione=false;
                       break;
                    }
                 }
                 if(StessaDirezione==false) break;
              }
              if(StessaDirezione==false) break;
           }
        }
        //---------------------------------------------------
        // AL-21/05/2013
        // Se il punto successivo risulta di interruzione,
        // è di rotazione e prenotato da altro TGV
        // --> non occupabile
        //---------------------------------------------------
      //if(PuntoSucessivo==NodoInterruzionePercorso && AGV[NumAgv]->mission.dati_perc[ptr_percorso].rot!=AGV[NumAgv]->mission.dati_perc[ptr_percorso+1].rot){
        AgvConsideratoInRot=AltroAgvInRot=0;
        //----------------------------------------------------------------
        // TGV in rotazione sul punto considerato
        //
        // AL-25/05/2017 modificato controllo con nodo precedente per
        // sapere se su cosiderato c'è la rotazione (controllare il
        // successivo è sbagliato
        //----------------------------------------------------------------
      //if(AGV[NumAgv]->mission.dati_perc[ptr_percorso].rot!=AGV[NumAgv]->mission.dati_perc[ptr_percorso+1].rot) AgvConsideratoInRot=1;
        if(AGV[NumAgv]->mission.dati_perc[ptr_percorso].rot!=AGV[NumAgv]->mission.dati_perc[ptr_percorso-1].rot) AgvConsideratoInRot=1;
        //----------------------------------------------------------------
        // ALTROAGV in rotazione sul suo Punto intermedio con punto
        // successivo coincidente con il punto successivo del TGV
        //----------------------------------------------------------------
        if(AGV[altro_agv]->mission.punto[ptrpath_adiacente+1]==PuntoSucessivo && AGV[altro_agv]->mission.dati_perc[ptrpath_adiacente-1].rot!=AGV[altro_agv]->mission.dati_perc[ptrpath_adiacente].rot){
           AltroAgvInRot=1;
        }
        //----------------------------------------------------------------
        // Punto di interruzione coincide con punto successivo
        //----------------------------------------------------------------
        if(PuntoSucessivo==NodoInterruzionePercorso && (AgvConsideratoInRot==1 || AltroAgvInRot==1)){
           for(j=0; j<MAXPERCORSI; j++){
              if(StessaDirezione==false                                    ) break;
              if(AGV[altro_agv]->mission.punto_prenotato[j]==0             ) break;
              if(AGV[altro_agv]->mission.punto_prenotato[j]!=PuntoSucessivo) continue;   // considero solo i TGV che prenotano il successivo al considerato
              //---------------------------------------------------------
              // Se distanza inferiore a ROT_DOPPIA -> possibile stallo
              //---------------------------------------------------------
              if(!N.VerificaDistanzaPunti(PuntoSucessivo, AGV[altro_agv]->mission.pintend, &DistX, &DistY, &Dist)){
                 if(Dist<DIST_MIN_ROT_DOPPIA_MAX){
                    StessaDirezione=false;
                 }
                 break;
              }
           }
           //-----------------------------------------------------------
           // AL-29/01/2015 SE TGV IN DOPPIA DEVIAZIONE (49<rot<70)
           // Il successivo deve essere a una ROT_DOPPIA
           // -> possibile stallo
           //-----------------------------------------------------------
           if(AGV[NumAgv]->mission.dati_perc[ptr_percorso  ].rot>49 && AGV[NumAgv]->mission.dati_perc[ptr_percorso  ].rot<70 &&
              AGV[NumAgv]->mission.dati_perc[ptr_percorso+1].rot>49 && AGV[NumAgv]->mission.dati_perc[ptr_percorso+1].rot<70){
              //---------------------------------------------------------
              // Recupero rotazione altro TGV su PINT
              //---------------------------------------------------------
              mantieni_dist_sicurezza=false;
              rot1 = AGV[NumAgv]->rot_standard(ptr_percorso+1);
              for(j=1; j<MAXPERCORSI; j++){
                 if(AGV[altro_agv]->mission.punto[j]==0                               ) break;
                 if(AGV[altro_agv]->mission.punto[j]!=AGV[altro_agv]->mission.pintend ) continue;
                 rot2 = AGV[altro_agv]->rot_standard(j);
                 if((rot1==0 || rot1==2) && (rot2==1 || rot2==3)) mantieni_dist_sicurezza=true;
                 if((rot1==1 || rot1==3) && (rot2==0 || rot2==2)) mantieni_dist_sicurezza=true;
              }
              //---------------------------------------------------------
              // Se distanza inferiore a ROT_DOPPIA -> possibile stallo
              //---------------------------------------------------------
              if(mantieni_dist_sicurezza==true){
                 if(!N.VerificaDistanzaPunti(PuntoSucessivo, AGV[altro_agv]->mission.pintend, &DistX, &DistY, &Dist)){
                    if(Dist<DIST_MIN_ROT_DOPPIA_MAX) StessaDirezione=false;
                 }
              }
           }
        }
        //-------------------------------------------------------
        // AL-03/02/2015
        // Condizione particolare per risolvere stallo
        //-------------------------------------------------------
        if(NodoConsiderato==NodoInterruzionePercorso){
           //-------------------------------------------------------
           // Verifico se altro tgv in rotazione su PINT
           //-------------------------------------------------------
           for(j=1; j<MAXPERCORSI; j++){  // scarto posizione di start
              if(StessaDirezione==false                                            ) break;
              if(AGV[altro_agv]->mission.punto[j+1]==0                             ) break;      // nodo considerato destinazione
              if(AGV[altro_agv]->mission.punto[j]!=AGV[altro_agv]->mission.pintend ) continue;   // considero solo i TGV che prenotano il successivo al considerato
              //---------------------------------------------------------
              // Verifico se il TGV sul nodo successivo al PINT debba
              // compiere una rotazione a 180°
              //---------------------------------------------------------
              altro_tgv_rot180=false;
              rot1 = AGV[altro_agv]->rot_standard(j  );
              rot2 = AGV[altro_agv]->rot_standard(j+1);
              if(rot1==0 && rot2==2) altro_tgv_rot180=true;
              if(rot1==1 && rot2==3) altro_tgv_rot180=true;
              if(rot1==2 && rot2==0) altro_tgv_rot180=true;
              if(rot1==3 && rot2==1) altro_tgv_rot180=true;
              //---------------------------------------------------------
              // Se distanza inferiore a ROT_DOPPIA -> possibile stallo
              //---------------------------------------------------------
              if(altro_tgv_rot180==true){
                 if(!N.VerificaDistanzaPunti(NodoConsiderato, AGV[altro_agv]->mission.pintend, &DistX, &DistY, &Dist)){
                    if(Dist<DIST_MIN_ROT_DOPPIA_MAX){
                       StessaDirezione=false;
                    }
                    break;
                 }
              }
              break; // esco comunque perchè interssa solo indice PINT
           }
        }
        //---------------------------------------------------
        // TGV non in stessa direzione
        //---------------------------------------------------
        if( StessaDirezione ) continue;
        //----------------------------------------------------------------
        // AL-15/07/2016 controllo se il TGV che prenota il
        // percorso è effettivamente prima o poi bloccante
        //
        // GB-11/04/2018: Sostituito   ptr_percorso --> ptr_percorso1
        // perchè altrimenti la variabile non è più valida per altro_agv
        // e il controllo dell'08/06/2017 fallisce
        //----------------------------------------------------------------
        NodoOccupabile1=AgvPerInterruzionePercorso1=satellite_occupato1=NodoInterruzionePercorso1=0;
        ptr_percorso1=ptr_percorso;
        for(; ptr_percorso1<MAXPERCORSI; ptr_percorso1++){
           if(AGV[ NumAgv ]->mission.punto[ptr_percorso1+1]==0) break;
           if(AGV[ NumAgv ]->mission.punto[ptr_percorso1]<1       ) continue;
           if(AGV[ NumAgv ]->mission.punto[ptr_percorso1]>TOTPUNTI) continue;
           NodoCosiderato1=AGV[ NumAgv ]->mission.punto[ptr_percorso1];
           //----------------------------------------------------------------------
           // !!! IMPORTANTE !!!
           // Interrompo se i nodi che non risultano prenotati dal
           // TGV BLOCCANTE considerato
           //
           // AL-08/06/2017: Se arrivo a un nodo non prenotato
           // dall'tgv blocccante considerato e tutti quelli prima
           // sono risultati prenotabili anche questo lo sarà di
           // certo.
           //
           // GB-01/06/2018: Disabilitata definitivamente la modifica del 8/06/2017
           //                perchè creava comunque stalli
           //----------------------------------------------------------------------
           //if(nodi_prenotati[NodoCosiderato1][altro_agv-1]!=altro_agv){
           //   NodoOccupabile1=1;
           //   break;
           //}
           if(nodi_prenotati[NodoCosiderato1][altro_agv-1]!=altro_agv) break;
           //-----------------------------------------------------
           // Verifico occupabilità di tutto il percorso
           //-----------------------------------------------------
           VerificaNodoOccupato(FALSE, NumAgv, NodoCosiderato1, NodoOccupabile1, AgvPerInterruzionePercorso1, satellite_occupato1, NodoInterruzionePercorso1, 0, altro_agv);
           //-----------------------------------------------------
           // Se anche solo un nodo non è occupabile il percorso
           // è da ritenersi prenotato
           //-----------------------------------------------------
           if(NodoOccupabile1==0){
              //-------------------------------------------
              // Controllo se si tratta di una zona
              // fuori ingombro manuale
              //-------------------------------------------
              zona_fuori_ingombro_manuale = N.verifica_zona_fuori_ingombro_manuale(NumAgv, NodoCosiderato1, (short int)(AgvPerInterruzionePercorso1));
              if(zona_fuori_ingombro_manuale==false) break; // NODO NON OCCUPABILE
           }
        }
        if(NodoOccupabile1==1) continue;
        //---------------------------------------------------------
        // Se Invece 2 NODI prenotati in senso inverso --> ERROR
        //---------------------------------------------------------
        satellite_occupato = NodoConsiderato;
        AgvAdiacente       = altro_agv;
        break;
     }
  }

  //---------------------------------------------------------
  // Recupero l'informazione di Nodo Occupabile
  //---------------------------------------------------------
  NodoOccupabile=1;
  if( satellite_occupato>0 ) NodoOccupabile=0;

  return 0;
}

//------------------------------------------------------------------
//  VerificaNodoPrenotatoConFuoriIng( )
//------------------------------------------------------------------
//  Restituisce :
//
//     NodoOccupabile           -> 1 OK
//     satellite_occupato       -> Ha significato solo locale perchè poi viene reinizializzato in occupa_nodi()
//     NodoInterruzionePercorso -> 0...xx
//
//
int VerificaNodoPrenotatoConFuoriIng(int NumAgv, int NodoConsiderato, int &NodoOccupabile, int &AgvAdiacente, int &satellite_occupato, int &NodoInterruzionePercorso )
{
  double Dist;
  int    DistX, DistY;
  int    PuntoPrenotato;
  bool   almeno_un_nodo_occupato;
  bool   zona_fuori_ingombro_manuale;
  bool   agv_in_rettaY, agv_in_rettaX;
  bool   altro_agv_in_rettaY, altro_agv_in_rettaX;
  int    PuntoSucessivo, PuntoSuccSucc;//, PuntoSucessivoSucessSucess;
  bool   StessaDirezione, altro_tgv_rot180, mantieni_dist_sicurezza;
  bool   stop_fuori_ing;
  double Dist1, Dist2, Dist3, Dist4;
  double DistMAX=5;    // distanza ammessa tra 2 nodi per essere considerati sovrapposti
  int    altro_agv;
  int    TotFuoriIng;
  int    AgvConsideratoInRot, AltroAgvInRot;
  int    SempreON=1;
  int    Vicino, j, k, z, x1, y1, x2, y2, rot1, rot2, count_fuori_ing;
  int    ptr_percorso, ptr_pos_percorso;
  int    ptrpath_pint_adiacente, ptrpath_pos_adiacente;
  int    satellite_occupato1, NodoInterruzionePercorso1, ptr_percorso1;
  int    NodoCosiderato1, NodoOccupabile1, AgvPerInterruzionePercorso1;
  int    rotp1, rotp2, xp1, yp1, xp2, yp2;

  int    count_nodi_allontanamento;
  int    count_nodi_inseguimento, count_nodi_cuscino;
  int    ptr_start_insegui, ptr_start_insegui_adiacente;
  int    ptr_start_cuscino;

//int    AltroAgv_NodoOccupato;


  //--------------------------------------------------------------
  // Inizializza Variabili
  //--------------------------------------------------------------
//NodoOccupabile    = 0;
//AgvAdiacente      = 0;
//satellite_occupato= 0;
  if( NodoConsiderato <= 0       ) return 1;
  if( NodoConsiderato > TOTPUNTI ) return 1;


  //--------------------------------------------------------------
  // Nodo destinazione (salto controllo prenotati) -> occupabile
  //--------------------------------------------------------------
  if(NodoConsiderato==AGV[NumAgv]->mission.pend && NodoConsiderato==NodoInterruzionePercorso){
     NodoOccupabile=1;
     return 0;
  }

  //--------------------------------------------------------------
  // GESTIONE SALTO NODO TGV LASER
  // Se si tratta di una navetta laser i salti nodi possono
  // essere di diversi cm per esigenze di guida, quindi i nodi
  // vengono ritenuti sovrapposti con tolleranze maggioni.
  //--------------------------------------------------------------
  if(AGV[NumAgv]->mission.GestioneLaser==true) DistMAX = DIST_MAX_SALTO_NODI+20;
  //--------------------------------------------------------------
  // (G4) Se il percorso non è completamente libero
  //      occupa comunque fino al primo nodo prenotato
  //      da altro AGVN.B.
  //--------------------------------------------------------------
  if( SempreON ){
     //--------------------------------------------------------------------------------
     // Cerca punto del percorso all'interno della missione
     // se NON presente --> SKIP
     //--------------------------------------------------------------------------------
     ptr_pos_percorso=0;
     for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
        //------------------------------------------------
        // Memorizzo l'indice di percorso della posizione
        // tgv
        //------------------------------------------------
        if(AGV[ NumAgv ]->mission.punto[ptr_percorso]==AGV[ NumAgv ]->stato.pos) ptr_pos_percorso=ptr_percorso;
        //------------------------------------------------
        // Interrompo indice di percorso del nodo
        // considerato
        //------------------------------------------------
        if(AGV[ NumAgv ]->mission.punto[ptr_percorso]==NodoConsiderato) break;
        if(AGV[ NumAgv ]->mission.punto[ptr_percorso]==0) break;
     }
     if(AGV[ NumAgv ]->mission.punto[ptr_percorso]==0) return 1;
     PuntoSucessivo = AGV[ NumAgv ]->mission.punto[ptr_percorso+1];
     PuntoSuccSucc  = AGV[ NumAgv ]->mission.punto[ptr_percorso+2];
     //-------------------------------------------------------------------
     // Verifico se ALTRO AGV in RETTA
     //-------------------------------------------------------------------
     x1 = Punti[ NodoConsiderato ][0];
     y1 = Punti[ NodoConsiderato ][1];
     agv_in_rettaY=agv_in_rettaX=false;
     rot1 = AGV[NumAgv]->mission.dati_perc[ptr_percorso+0].rot;
     rot2 = AGV[NumAgv]->mission.dati_perc[ptr_percorso+1].rot;
     if(rot1<10 && rot2<10 && rot1==rot2){
        if(rot1==0 || rot1==2) agv_in_rettaY=true;
        if(rot1==1 || rot1==3) agv_in_rettaX=true;
     }
     //------------------------------------------------------
     // Verifica se esiste un nodo prenotato
     //------------------------------------------------------
     TotFuoriIng=0;
     for(altro_agv=1; altro_agv<=MAXAGV; altro_agv++){
        if(altro_agv==NumAgv) continue;
        if(!AGV[altro_agv]->test_mission_on()) continue;
        StessaDirezione=true;
        //---------------------------------------------------
        // Indice posizione ALTRO AGV
        //---------------------------------------------------
        ptrpath_pos_adiacente=0;
        for(j=1; j<MAXPERCORSI; j++){
           if(AGV[altro_agv]->mission.punto[j]==0                            ) break;
           if(AGV[altro_agv]->mission.punto[j]==AGV[altro_agv]->mission.pend ) break;
           if(AGV[altro_agv]->mission.punto[j]!=AGV[altro_agv]->stato.pos    ) continue;
           ptrpath_pos_adiacente=j;
           break;
        }
        //---------------------------------------------------
        // Indice punto intermedio ALTRO TGV
        //---------------------------------------------------
        ptrpath_pint_adiacente=0;
        for(j=1; j<MAXPERCORSI; j++){
           if(AGV[altro_agv]->mission.punto[j]==0                               ) break;
           if(AGV[altro_agv]->mission.punto[j]==AGV[altro_agv]->mission.pend    ) break;
           if(AGV[altro_agv]->mission.punto[j]!=AGV[altro_agv]->mission.pintend ) continue;
           ptrpath_pint_adiacente=j;
           break;
        }
        //---------------------------------------------------------------------------
        // GESTIONE INSEGUIMENTI E ALLONTANAMENTI
        //---------------------------------------------------------------------------
        count_nodi_cuscino=0;
        count_nodi_inseguimento=0;
        count_nodi_allontanamento=0;
        ptr_start_cuscino=MAXPERCORSI+1;
        ptr_start_insegui=MAXPERCORSI+1;
        ptr_start_insegui_adiacente=MAXPERCORSI+1;
        if(AbilitaPrenotatiConCompattaTGV==true){
           //-----------------------------------------------
           // Verifico se il primo nodo prenotato in comune
           // tra tgv considerato e altro_tgv è già BUSY
           //-----------------------------------------------
           for(k=ptr_pos_percorso+1; k<MAXPERCORSI; k++){
              if(AGV[NumAgv]->mission.punto[k+1]==0                           ) break;
              if(nodi_prenotati[AGV[NumAgv]->mission.punto[k]][altro_agv-1]==0) continue;
              //----------------------------------------------
              // INSEGUIMENTO
              // Il primo nodo prenotato dall'altro TGV
              // deve essere anche occupato
              //----------------------------------------------
              if(N.nodo_busy[AGV[NumAgv]->mission.punto[k]]==altro_agv){
                 ptr_start_insegui=k;
                 for(z=ptrpath_pos_adiacente; z<MAXPERCORSI; z++){
                    if(AGV[altro_agv]->mission.punto[z+1]==0                          ) break;
                    if(AGV[altro_agv]->mission.punto[z]!=AGV[NumAgv]->mission.punto[k]) continue;
                    ptr_start_insegui_adiacente=z;
                    break;
                 }
              }
              //----------------------------------------------
              // ZONA CUSCINETTO
              // Se il primo nodo prenotato dall'altro TGV
              // non è occupato deve essere l'unico poi deve
              // iniziare una zona cuscitetto senza più
              // incontrarlo
              //----------------------------------------------
              for(z=ptrpath_pint_adiacente; z<MAXPERCORSI; z++){
                 if(AGV[altro_agv]->mission.punto[z+1]==0                          ) break;
                 if(nodi_prenotati[AGV[altro_agv]->mission.punto[z]][NumAgv-1]==0  ) continue;
                 if(AGV[altro_agv]->mission.punto[z]==AGV[NumAgv]->mission.punto[k]){
                    ptr_start_cuscino=k;
                    break;
                 }
                 break;
              }
              //----------------------------------------------
              // Al primo prenotato/occupato --> skip!
              //----------------------------------------------
              break;
           }
           //-------------------------------------------------------
           // INSEGUIMENTO E ALLONTANAMENTO
           // condizione: primo nodo in comune tra i TGV
           // deve già essere occupato da altro_agv
           //-------------------------------------------------------
           if(ptr_start_insegui<MAXPERCORSI && ptr_start_insegui_adiacente<MAXPERCORSI){
              //-------------------------------------------------------
              // VERIFICA INSEGUIMENTO
              // I TGV proseguono nella stessa direzione con nodi
              // in comune e nessuna inversione di marcia
              //-------------------------------------------------------
              z=ptr_start_insegui_adiacente;
              for(k=ptr_start_insegui; k<MAXPERCORSI; k++){
                 if(AGV[NumAgv]->mission.punto[k+1]==0) break;
                 //-------------------------------------
                 // Nodi non in comune --> skip!!
                 //-------------------------------------
                 if(AGV[NumAgv]->mission.punto[k]!=AGV[altro_agv]->mission.punto[z]) break;
                 //-------------------------------------
                 // Punti in comune
                 //-------------------------------------
                 if(AGV[NumAgv]->mission.punto[k]==AGV[altro_agv]->mission.punto[z]){
                    //-------------------------------------
                    // Stessa rotazione sul punto
                    //-------------------------------------
                    if(AGV[NumAgv]->rot_standard(k)==AGV[altro_agv]->rot_standard(z)){
                       //-------------------------------------
                       // Inseguimento (stessa direzione)
                       //-------------------------------------
                       if(AGV[NumAgv]->mission.dati_perc[k].dir==AGV[altro_agv]->mission.dati_perc[z].dir){
                          count_nodi_inseguimento++;
                       }
                       //-------------------------------------
                       // Inversione (direzione inversa)
                       //
                       // AL-11/06/2020 - l'inversione deve
                       // essere fatta dall'altro TGV non dal
                       // considerato
                       //-------------------------------------
                       if(z>0 && AGV[altro_agv]->mission.dati_perc[z-1].dir!=AGV[altro_agv]->mission.dati_perc[z].dir){
                          if(AGV[NumAgv]->mission.dati_perc[k].dir!=AGV[altro_agv]->mission.dati_perc[z].dir){
                             count_nodi_inseguimento=0;
                             break;
                          }
                       }
                    }
                 }
                 z++;
              }
              //-------------------------------------------------------
              // VERIFICA ALLONTANAMENTO
              // Se il primo nodo prenotato in comune è occupato e ne
              // seguono altri occupati dall'altro_agv allora siamo
              // in condizione certa di allontanamento.
              //-------------------------------------------------------
              Dist1=0;
              if(N.VerificaDistanzaPunti(NodoConsiderato, AGV[altro_agv]->mission.pintend, &DistX, &DistY, &Dist1)) Dist1=9999;
              Dist2=0;
              if(N.VerificaDistanzaPunti(NodoConsiderato, AGV[altro_agv]->mission.punto[ptr_start_insegui_adiacente], &DistX, &DistY, &Dist2)) Dist2=9999;
              if(Dist1!=9999 && Dist2!=9999 && Dist1>=Dist2){
                 z=ptr_start_insegui;
                 for(k=ptr_start_insegui_adiacente+1; k<MAXPERCORSI; k++){
                    if(AGV[altro_agv]->mission.punto[k+1]==0                   ) break;
                    if(N.nodo_busy[AGV[altro_agv]->mission.punto[k]]!=altro_agv) break;  // mi fermo al primo nodo non occupato.
                    //--------------------------------------------------
                    // L'altro agv viene consiederato in allontanamento
                    // se i nodi occupati non sono anche prenotati
                    // dal TGV considerato
                    //--------------------------------------------------
                    if(nodi_prenotati[AGV[altro_agv]->mission.punto[k]][NumAgv-1]==NumAgv) continue;
                    count_nodi_allontanamento++;
                 }
              }
           }
           //-------------------------------------------------------
           // ZONA CUSCINETTO
           // condizione: primo nodo in comune tra i TGV non è
           // necessario sia occupato, ma deve essere l'unico
           // poi deve seguire una zona cuscinetto non prenotata
           // da altro agv e occupabile.
           //-------------------------------------------------------
           if(ptr_start_cuscino<MAXPERCORSI){
              for(k=ptr_start_cuscino+1; k<MAXPERCORSI; k++){
                 if(AGV[NumAgv]->mission.punto[k+1]==0                                   ) break;
                 if(nodi_prenotati[AGV[NumAgv]->mission.punto[k]][altro_agv-1]==altro_agv) break;
                 //-------------------------------------------------------
                 // Conteggio nodi occupabili in zona cuscino usando
                 // come riferimento solo i TGV considerato
                 //-------------------------------------------------------
                 NodoOccupabile1=AgvPerInterruzionePercorso1=satellite_occupato1=NodoInterruzionePercorso1=0;
                 VerificaNodoOccupato(FALSE, NumAgv, AGV[NumAgv]->mission.punto[k], NodoOccupabile1, AgvPerInterruzionePercorso1, satellite_occupato1, NodoInterruzionePercorso1, 0, altro_agv);
                 if(NodoOccupabile1==0) continue;
                 count_nodi_cuscino++;
              }
           }
        }
        //-------------------------------------------------------
        // Se esiste condizione di INSEGUIMENTO o ALLONTANAMENTO
        // o una ZONA CUSCINETTO, altro_agv può essere scartato
        //-------------------------------------------------------
//      if(count_nodi_inseguimento>0 || count_nodi_cuscino>0) continue;
        if(count_nodi_inseguimento>0 || count_nodi_allontanamento>0 || count_nodi_cuscino>0) continue;
        //---------------------------------------------------
        // Se il TGV NON incontra almeno 2 NODI percorsi in
        // senso opposto --> CONTINUE
        // P.S. 1 solo nodo può essere un AGV che attraversa
        //      perpendicolarmente 1 incrocio
        //---------------------------------------------------
        for(j=ptrpath_pos_adiacente; j<MAXPERCORSI; j++){
           if(StessaDirezione==false               ) break;
           if(AGV[altro_agv]->mission.punto[j+1]==0) break;
           //-------------------------------------------------------------------
           // Verifico se ALTRO AGV in RETTA
           //-------------------------------------------------------------------
           x2 = Punti[ AGV[altro_agv]->mission.punto[j] ][0];
           y2 = Punti[ AGV[altro_agv]->mission.punto[j] ][1];
           altro_agv_in_rettaY=altro_agv_in_rettaX=false;
           rot1 = AGV[altro_agv]->mission.dati_perc[j+0].rot;
           rot2 = AGV[altro_agv]->mission.dati_perc[j+1].rot;
           if(rot1<10 && rot2<10 && rot1==rot2){
              if(rot1==0 || rot1==2) altro_agv_in_rettaY=true;
              if(rot1==1 || rot1==3) altro_agv_in_rettaX=true;
           }
           //-------------------------------------------------------------------
           // Verifico se TGV su due percorsi separati in RETTE PARALLELE
           //-------------------------------------------------------------------
           if(altro_agv_in_rettaY==true && agv_in_rettaY==true){
              if(abs(x1-x2)>=DIST_MIN_RETTE_PARALLELE) continue;
           }
           if(altro_agv_in_rettaX==true && agv_in_rettaX==true){
              if(abs(y1-y2)>=DIST_MIN_RETTE_PARALLELE) continue;
           }
           //-------------------------------------------------------------------
           // Calcolo distanza tra nodi prenotati
           //-------------------------------------------------------------------
           Dist1=0;
           if(N.VerificaDistanzaPunti(PuntoSucessivo,  AGV[altro_agv]->mission.punto[j+0], &DistX, &DistY, &Dist1)) Dist1=9999;
           Dist2=0;
           if(N.VerificaDistanzaPunti(NodoConsiderato, AGV[altro_agv]->mission.punto[j+1], &DistX, &DistY, &Dist2)) Dist2=9999;
           Dist3=0;
           if(N.VerificaDistanzaPunti(PuntoSuccSucc,   AGV[altro_agv]->mission.punto[j+0], &DistX, &DistY, &Dist3)) Dist3=9999;
           Dist4=0;
           if(N.VerificaDistanzaPunti(PuntoSucessivo,  AGV[altro_agv]->mission.punto[j+1], &DistX, &DistY, &Dist4)) Dist4=9999;
           if((Dist1<=DistMAX && Dist2<=DistMAX) || (Dist3<=DistMAX && Dist4<=DistMAX)){
              //---------------------------------------------------------------------------
              // AL-18/06 RITARDO PARTENZA TGV DA PT
              // Aumento i controlli in partenza dai punti terminali
              // (nessun nodo prenotato in senso opposto)
              //---------------------------------------------------------------------------
              #ifdef RITARDO_PARTENZA_TGV_DA_PT
                 if(N.punti_notevoli[AGV[NumAgv]->stato.pos]==true && AGV[NumAgv]->stato.start==0) StessaDirezione=false;
              #endif;
              //---------------------------------------------------------------------------
              // Se il punto sucessivo o quello considerato sono già occupati da altro_tgv
              // allora l'ingombro è sicuro
              //---------------------------------------------------------------------------
            //if(N.nodo_busy[PuntoSucessivo]==altro_agv || N.nodo_busy[NodoConsiderato]==altro_agv){
            //   StessaDirezione=false;
            //}
              if(N.nodo_busy[NodoConsiderato]==altro_agv){
                 StessaDirezione=false;
              }
              //--------------------------------------------------------------------------
              // AL-08/06/2020 - GESTIONE OCCUPAZIONE NODO CON "INGOMBRO AGV"
              // Se attiva la gestione Occupa nodi con IngombroAgv uso la funzione
              // VerificaNodoOccupato() per determinare se almeno un nodo è occupato
              // perchè più precisa anche se più lenta
              // PER TRACCE MOLTO VICINE TRA LORO E PARALLERE
              //--------------------------------------------------------------------------
              if(AbilitaGestioneIngombroCurvePerStopTGV==2) almeno_un_nodo_occupato=true;
              else{
                 //---------------------------------------------------------------------------
                 // Se proseguendo nella missione del TGV trovo almeno un nodo occupato
                 // da alto_agv allora le due missioni si intralciano altrimenti NO.
                 //---------------------------------------------------------------------------
                 almeno_un_nodo_occupato=false;
                 for(k=ptr_percorso; k<MAXPERCORSI; k++){
                    if(AGV[NumAgv]->mission.punto[k+1]==0) break;
                    PuntoPrenotato = AGV[NumAgv]->mission.punto[k];
                    //-----------------------------------------------
                    // Nodo occupato da altro agv su stessa traccia
                    //-----------------------------------------------
                    if(N.nodo_busy[PuntoPrenotato]==altro_agv){
                       almeno_un_nodo_occupato=true;
                       break;
                    }
                 }
              }
              //-----------------------------------------------
              // VERIFICA ZONA FUORI INGOMBRO
              //-----------------------------------------------
              if(almeno_un_nodo_occupato==true){
                 count_fuori_ing=0;
                 stop_fuori_ing=false;
                 NodoOccupabile1=AgvPerInterruzionePercorso1=satellite_occupato1=NodoInterruzionePercorso1=0;
                 for(k=ptr_percorso; k<MAXPERCORSI; k++){
                    if(StessaDirezione==false          ) break;
                    if(AGV[NumAgv]->mission.punto[k]==0) break;
                    PuntoPrenotato = AGV[NumAgv]->mission.punto[k];
                    //-----------------------------------------------
                    // Nodo occupato da altro agv
                    //-----------------------------------------------
                    if(N.nodo_busy[PuntoPrenotato]==altro_agv){
                       StessaDirezione=false;
                       break;
                    }
                    else{
                       //-----------------------------------------------------
                       // Verifica se nodo occupabile
                       //
                       // 08/06/2020 solo altro_agv considerato, gli altri
                       // hanno il loro ciclo (PER VELOCIZZARE)
                       //-----------------------------------------------------
                     //VerificaNodoOccupato(FALSE, NumAgv, PuntoPrenotato, NodoOccupabile1, AgvPerInterruzionePercorso1, satellite_occupato1, NodoInterruzionePercorso1, 0, 0);
                       VerificaNodoOccupato(FALSE, NumAgv, PuntoPrenotato, NodoOccupabile1, AgvPerInterruzionePercorso1, satellite_occupato1, NodoInterruzionePercorso1, 0, altro_agv);
                       //-----------------------------------------------------
                       // CONTEGGIO NODI FUORI INGOMBRO
                       // il nodo deve essere occupabile e non prenotato
                       // da altro TGV considerato
                       //
                       // AL-18/05/2020 interrompo la verifica del fuori
                       // ingombro solo se l'agv interruzione è quello
                       // considerato
                       //-----------------------------------------------------
                     //if(NodoOccupabile1==0) stop_fuori_ing=true;
                       if(NodoOccupabile1==0 && AgvPerInterruzionePercorso1==altro_agv) stop_fuori_ing=true;
                       if(NodoOccupabile1==1 && stop_fuori_ing==false){
                          //-----------------------------------------------------------
                          // Verifico se il nodo prenotato considerato è oltre la
                          // distanza di rotazione doppia in modo da conteggiare solo
                          // quelli come nodi fuori ingombro
                          //-----------------------------------------------------------
                          Dist=0;
                          if(N.VerificaDistanzaPunti(NodoConsiderato, PuntoPrenotato, &DistX, &DistY, &Dist)) Dist=9999;
                          //-----------------------------------------------------------
                          // Nodo occupabile e non prenotato da altro agv considerato
                          //-----------------------------------------------------------
                          if(Dist>=DIST_MIN_ROT_DOPPIA_MAX){
                             //-----------------------------------------------------------
                             // Nodo occupabile e non prenotato da altro agv considerato
                             //-----------------------------------------------------------
                             if(AgvPerInterruzionePercorso1==0 && nodi_prenotati[PuntoPrenotato][altro_agv-1]==0){
                                count_fuori_ing++;
                                if(count_fuori_ing>=MIN_NODI_FUORI_INGOMBRO){TotFuoriIng=count_fuori_ing; break;}
                             }
                          }
                       }
                       //-----------------------------------------------------
                       // AL-08/06/2020
                       // Se nodo non occupabile causa altro agv considerato
                       // StessaDirezione=FALSE
                       //-----------------------------------------------------
                       if(NodoOccupabile1==0 && AgvPerInterruzionePercorso1==altro_agv){
                          StessaDirezione=false;
                          break;
                       }
                    }
                 }
              }
              //--------------------------------------------------------------------------
              // AL-08/06/2020 - GESTIONE OCCUPAZIONE NODO CON "INGOMBRO AGV"
              // Se attiva la gestione Occupa nodi con IngombroAgv uso la funzione
              // VerificaNodoOccupato() per determinare se almeno un nodo è occupato
              // perchè più precisa anche se più lenta
              // PER TRACCE MOLTO VICINE TRA LORO E PARALLERE
              //--------------------------------------------------------------------------
              if(AbilitaGestioneIngombroCurvePerStopTGV==2) almeno_un_nodo_occupato=true;
              else{
                 //---------------------------------------------------------------------------
                 // Tutti i nodi prenotati non devono incontrare un nodo occupato a
                 // una distanza rotazione doppia dal TGV bloccante
                 //---------------------------------------------------------------------------
                 almeno_un_nodo_occupato=false;
                 for(k=ptr_percorso; k<MAXPERCORSI; k++){
                    if(StessaDirezione==false              ) break;
                    if(AGV[NumAgv]->mission.punto[k+1]==0  ) break;
                    PuntoPrenotato = AGV[NumAgv]->mission.punto[k];
                    //----------------------------------------------------------------------
                    // Verifico che nemmeno tutti i nodi Vicini a quello considerato
                    //----------------------------------------------------------------------
                    for(Vicino=10; Vicino<=TOTPUNTI; Vicino++){
                       if(PuntoPrenotato==Vicino           ) continue;
                       if(N.pt_in_ingombro[Vicino]==false &&
                          N.punti_notevoli[Vicino]==true   ) continue;   // Punto terminale non a rischio
                       if(N.n[Vicino]->num==0              ) continue;
                       if(N.nodo_busy[Vicino]!=altro_agv   ) continue;
                       //-------------------------------------------
                       // Se la distanza e superiore a quella di
                       // ROTAZIONE_DOPPIA_MAX --> Skip!
                       //-------------------------------------------
                       if(N.VerificaDistanzaPunti(PuntoPrenotato, Vicino, &DistX, &DistY, &Dist)) continue;
                       if(Dist>=DIST_MIN_ROT_DOPPIA_MAX) continue;
                       almeno_un_nodo_occupato=true;
                       break;
                    }
                    if(almeno_un_nodo_occupato) break;
                 }
              }
              //-----------------------------------------------
              // VERIFICA ZONA FUORI INGOMBRO
              //-----------------------------------------------
              if(almeno_un_nodo_occupato==true){
                 count_fuori_ing=0;
                 stop_fuori_ing=false;
                 for(k=ptr_percorso; k<MAXPERCORSI; k++){
                    if(StessaDirezione==false              ) break;
                    if(AGV[NumAgv]->mission.punto[k+1]==0  ) break;
                    PuntoPrenotato = AGV[NumAgv]->mission.punto[k];
                    xp1 = Punti[ PuntoPrenotato ][0];
                    yp1 = Punti[ PuntoPrenotato ][1];
                    agv_in_rettaY=agv_in_rettaX=false;
                    rotp1 = AGV[NumAgv]->mission.dati_perc[k+0].rot;
                    rotp2 = AGV[NumAgv]->mission.dati_perc[k+1].rot;
                    if(rotp1<10 && rotp2<10 && rotp1==rotp2){
                       if(rotp1==0 || rotp1==2) agv_in_rettaY=true;
                       if(rotp1==1 || rotp1==3) agv_in_rettaX=true;
                    }
                    //----------------------------------------------------------------------
                    // Verifico che nemmeno tutti i nodi Vicini a quello considerato
                    //----------------------------------------------------------------------
                    NodoOccupabile1=1;
                    AgvPerInterruzionePercorso1=satellite_occupato1=NodoInterruzionePercorso1=0;
                    for(Vicino=10; Vicino<=TOTPUNTI; Vicino++){
                       if(PuntoPrenotato==Vicino           ) continue;
                       if(N.pt_in_ingombro[Vicino]==false &&
                          N.punti_notevoli[Vicino]==true   ) continue;   // Punto terminale non a rischio
                       if(N.n[Vicino]->num==0              ) continue;
                       if(N.nodo_busy[Vicino]!=altro_agv   ) continue;
                       //-------------------------------------------
                       // Se la distanza e superiore a quella di
                       // ROTAZIONE_DOPPIA_MAX --> Skip!
                       //-------------------------------------------
                       if(N.VerificaDistanzaPunti(PuntoPrenotato, Vicino, &DistX, &DistY, &Dist)) continue;
                       if(Dist>=DIST_MIN_ROT_DOPPIA_MAX) continue;
                     //StessaDirezione=false;
                     //break;
                       //-------------------------------------------
                       // Verifico come si muove il ALTRO TGV
                       // sul VICINO
                       //-------------------------------------------
                       altro_agv_in_rettaY=altro_agv_in_rettaX=false;
                       for(z=1; z<MAXPERCORSI; z++){
                          if(AGV[altro_agv]->mission.punto[z+1]==0    ) break;
                          if(AGV[altro_agv]->mission.punto[z]!=Vicino ) continue;
                          xp2 = Punti[ Vicino ][0];
                          yp2 = Punti[ Vicino ][1];
                          altro_agv_in_rettaY=altro_agv_in_rettaX=false;
                          rotp1 = AGV[altro_agv]->mission.dati_perc[z+0].rot;
                          rotp2 = AGV[altro_agv]->mission.dati_perc[z+1].rot;
                          if(rotp1<10 && rotp2<10 && rotp1==rotp2){
                             if(rotp1==0 || rotp1==2) altro_agv_in_rettaY=true;
                             if(rotp1==1 || rotp1==3) altro_agv_in_rettaX=true;
                          }
                          break;
                       }
                       //-------------------------------------------------------------------
                       // Verifico se TGV su due percorsi separati in RETTE PARALLELE
                       //-------------------------------------------------------------------
                       if(altro_agv_in_rettaY==true && agv_in_rettaY==true){
                          if(abs(xp1-xp2)>=DIST_MIN_RETTE_PARALLELE) continue;
                       }
                       if(altro_agv_in_rettaX==true && agv_in_rettaX==true){
                          if(abs(yp1-yp2)>=DIST_MIN_RETTE_PARALLELE) continue;
                       }
                       //-----------------------------------------------------
                       // Verifica se nodo occupabile
                       //
                       // 08/06/2020 solo altro_agv considerato, gli altri
                       // hanno il loro ciclo (PER VELOCIZZARE)
                       //-----------------------------------------------------
                     //VerificaNodoOccupato(FALSE, NumAgv, PuntoPrenotato, NodoOccupabile1, AgvPerInterruzionePercorso1, satellite_occupato1, NodoInterruzionePercorso1, 0, 0);
                       VerificaNodoOccupato(FALSE, NumAgv, PuntoPrenotato, NodoOccupabile1, AgvPerInterruzionePercorso1, satellite_occupato1, NodoInterruzionePercorso1, 0, altro_agv);
                       //-----------------------------------------------------
                       // Se anche solo un nodo non è occupabile il percorso
                       // è da ritenersi prenotato
                       //-----------------------------------------------------
                       if(NodoOccupabile1==0){
                          zona_fuori_ingombro_manuale = N.verifica_zona_fuori_ingombro_manuale(NumAgv, PuntoPrenotato, (short int)(AgvPerInterruzionePercorso1));
                          if(zona_fuori_ingombro_manuale==false) break; // NODO NON OCCUPABILE
                          NodoOccupabile1=1;
                       }
                    }
                    //-----------------------------------------------------
                    // CONTEGGIO NODI FUORI INGOMBRO
                    // il nodo deve essere occupabile e non prenotato
                    // da altro TGV considerato
                    //-----------------------------------------------------
                    if(NodoOccupabile1==0) stop_fuori_ing=true;
                    if(NodoOccupabile1==1 && stop_fuori_ing==false){
                       //-----------------------------------------------------------
                       // Nodo occupabile e non prenotato da altro agv considerato
                       //-----------------------------------------------------------
                       if(AgvPerInterruzionePercorso1==0 && nodi_prenotati[PuntoPrenotato][altro_agv-1]==0){
                          count_fuori_ing++;
                          if(count_fuori_ing>=MIN_NODI_FUORI_INGOMBRO){TotFuoriIng=count_fuori_ing; break;}
                       }
                    }
                    //-----------------------------------------------------
                    // Se nodo non occupabile causa altro agv considerato
                    // StessaDirezione=FALSE
                    //-----------------------------------------------------
                    if(NodoOccupabile1==0 && AgvPerInterruzionePercorso1==altro_agv){
                       StessaDirezione=false;
                       break;
                    }
                 }
              }
           }
        }
        //---------------------------------------------------
        // AL-21/05/2013
        // Se il punto successivo risulta di interruzione,
        // è di rotazione e prenotato da altro TGV
        // --> non occupabile
        //---------------------------------------------------
      //if(PuntoSucessivo==NodoInterruzionePercorso && AGV[NumAgv]->mission.dati_perc[ptr_percorso].rot!=AGV[NumAgv]->mission.dati_perc[ptr_percorso+1].rot){
        AgvConsideratoInRot=AltroAgvInRot=0;
        //----------------------------------------------------------------
        // TGV in rotazione sul punto considerato
        //
        // AL-25/05/2017 modificato controllo con nodo precedente per
        // sapere se su cosiderato c'è la rotazione (controllare il
        // successivo è sbagliato
        //----------------------------------------------------------------
      //if(AGV[NumAgv]->mission.dati_perc[ptr_percorso].rot!=AGV[NumAgv]->mission.dati_perc[ptr_percorso+1].rot) AgvConsideratoInRot=1;
        if(AGV[NumAgv]->mission.dati_perc[ptr_percorso].rot!=AGV[NumAgv]->mission.dati_perc[ptr_percorso-1].rot) AgvConsideratoInRot=1;
        //----------------------------------------------------------------
        // ALTROAGV in rotazione sul suo Punto intermedio con punto
        // successivo coincidente con il punto successivo del TGV
        //----------------------------------------------------------------
        if(AGV[altro_agv]->mission.punto[ptrpath_pint_adiacente+1]==PuntoSucessivo && AGV[altro_agv]->mission.dati_perc[ptrpath_pint_adiacente-1].rot!=AGV[altro_agv]->mission.dati_perc[ptrpath_pint_adiacente].rot){
           AltroAgvInRot=1;
        }
        //----------------------------------------------------------------
        // Punto di interruzione coincide con punto successivo
        //----------------------------------------------------------------
        if(PuntoSucessivo==NodoInterruzionePercorso && (AgvConsideratoInRot==1 || AltroAgvInRot==1)){
           for(j=0; j<MAXPERCORSI; j++){
              if(StessaDirezione==false                                    ) break;
              if(AGV[altro_agv]->mission.punto_prenotato[j]==0             ) break;
              if(AGV[altro_agv]->mission.punto_prenotato[j]!=PuntoSucessivo) continue;   // considero solo i TGV che prenotano il successivo al considerato
              //---------------------------------------------------------
              // Se distanza inferiore a ROT_DOPPIA -> possibile stallo
              //---------------------------------------------------------
              if(!N.VerificaDistanzaPunti(PuntoSucessivo, AGV[altro_agv]->mission.pintend, &DistX, &DistY, &Dist)){
                 if(Dist<DIST_MIN_ROT_DOPPIA_MAX){
                    StessaDirezione=false;
                 }
                 break;
              }
           }
           //-----------------------------------------------------------
           // AL-29/01/2015 SE TGV IN DOPPIA DEVIAZIONE (49<rot<70)
           // Il successivo deve essere a una ROT_DOPPIA
           // -> possibile stallo
           //-----------------------------------------------------------
           if(AGV[NumAgv]->mission.dati_perc[ptr_percorso  ].rot>49 && AGV[NumAgv]->mission.dati_perc[ptr_percorso  ].rot<70 &&
              AGV[NumAgv]->mission.dati_perc[ptr_percorso+1].rot>49 && AGV[NumAgv]->mission.dati_perc[ptr_percorso+1].rot<70){
              //---------------------------------------------------------
              // Recupero rotazione altro TGV su PINT
              //---------------------------------------------------------
              mantieni_dist_sicurezza=false;
              rot1 = AGV[NumAgv]->rot_standard(ptr_percorso+1);
              for(j=1; j<MAXPERCORSI; j++){
                 if(AGV[altro_agv]->mission.punto[j]==0                               ) break;
                 if(AGV[altro_agv]->mission.punto[j]!=AGV[altro_agv]->mission.pintend ) continue;
                 rot2 = AGV[altro_agv]->rot_standard(j);
                 if((rot1==0 || rot1==2) && (rot2==1 || rot2==3)) mantieni_dist_sicurezza=true;
                 if((rot1==1 || rot1==3) && (rot2==0 || rot2==2)) mantieni_dist_sicurezza=true;
              }
              //---------------------------------------------------------
              // Se distanza inferiore a ROT_DOPPIA -> possibile stallo
              //---------------------------------------------------------
              if(mantieni_dist_sicurezza==true){
                 if(!N.VerificaDistanzaPunti(PuntoSucessivo, AGV[altro_agv]->mission.pintend, &DistX, &DistY, &Dist)){
                    if(Dist<DIST_MIN_ROT_DOPPIA_MAX){
                       StessaDirezione=false;
                    }
                 }
              }
           }
        }
        //-------------------------------------------------------
        // AL-03/02/2015
        // Condizione particolare per risolvere stallo
        //-------------------------------------------------------
        if(NodoConsiderato==NodoInterruzionePercorso){
           //-------------------------------------------------------
           // Verifico se altro tgv in rotazione su PINT
           //-------------------------------------------------------
           for(j=1; j<MAXPERCORSI; j++){  // scarto posizione di start
              if(StessaDirezione==false                                            ) break;
              if(AGV[altro_agv]->mission.punto[j+1]==0                             ) break;      // nodo considerato destinazione
              if(AGV[altro_agv]->mission.punto[j]!=AGV[altro_agv]->mission.pintend ) continue;   // considero solo i TGV che prenotano il successivo al considerato
              //---------------------------------------------------------
              // Verifico se il TGV sul nodo successivo al PINT debba
              // compiere una rotazione a 180°
              //---------------------------------------------------------
              altro_tgv_rot180=false;
              rot1 = AGV[altro_agv]->rot_standard(j  );
              rot2 = AGV[altro_agv]->rot_standard(j+1);
              if(rot1==0 && rot2==2) altro_tgv_rot180=true;
              if(rot1==1 && rot2==3) altro_tgv_rot180=true;
              if(rot1==2 && rot2==0) altro_tgv_rot180=true;
              if(rot1==3 && rot2==1) altro_tgv_rot180=true;
              //---------------------------------------------------------
              // Se distanza inferiore a ROT_DOPPIA -> possibile stallo
              //---------------------------------------------------------
              if(altro_tgv_rot180==true){
                 if(!N.VerificaDistanzaPunti(NodoConsiderato, AGV[altro_agv]->mission.pintend, &DistX, &DistY, &Dist)){
                    if(Dist<DIST_MIN_ROT_DOPPIA_MAX){
                       StessaDirezione=false;
                    }
                    break;
                 }
              }
              break; // esco comunque perchè interssa solo indice PINT
           }
        }
        //---------------------------------------------------
        // TGV non in stessa direzione
        //---------------------------------------------------
        if( StessaDirezione ) continue;
        //----------------------------------------------------------------
        // RESET FUORI INGOMBRO PER MANCATA "STESSA DIREZIONE" = FALSE
        //----------------------------------------------------------------
        TotFuoriIng=0;
        //----------------------------------------------------------------
        // AL-15/07/2016 controllo se il TGV che prenota il
        // percorso è effettivamente prima o poi bloccante
        //
        // GB-11/04/2018: Sostituito   ptr_percorso --> ptr_percorso1
        // perchè altrimenti la variabile non è più valida per altro_agv
        // e il controllo dell'08/06/2017 fallisce
        //----------------------------------------------------------------
        ptr_percorso1=ptr_percorso;
        NodoOccupabile1=AgvPerInterruzionePercorso1=satellite_occupato1=NodoInterruzionePercorso1=0;
        for(; ptr_percorso1<MAXPERCORSI; ptr_percorso1++){
           if(AGV[ NumAgv ]->mission.punto[ptr_percorso1+1]==0) break;
           if(AGV[ NumAgv ]->mission.punto[ptr_percorso1]<1       ) continue;
           if(AGV[ NumAgv ]->mission.punto[ptr_percorso1]>TOTPUNTI) continue;
           NodoCosiderato1=AGV[ NumAgv ]->mission.punto[ptr_percorso1];
           //----------------------------------------------------------------------
           // !!! IMPORTANTE !!!
           // Interrompo se i nodi che non risultano prenotati dal
           // TGV BLOCCANTE considerato
           //
           // AL-08/06/2017: Se arrivo a un nodo non prenotato
           // dall'tgv blocccante considerato e tutti quelli prima
           // sono risultati prenotabili anche questo lo sarà di
           // certo.
           //
           // GB-01/06/2018: Disabilitata definitivamente la modifica del 8/06/2017
           //                perchè creava comunque stalli
           //----------------------------------------------------------------------
           //if(nodi_prenotati[NodoCosiderato1][altro_agv-1]!=altro_agv){
           //   NodoOccupabile1=1;
           //   break;
           //}
           if(nodi_prenotati[NodoCosiderato1][altro_agv-1]!=altro_agv) break;
           //-----------------------------------------------------
           // Verifico occupabilità di tutto il percorso
           //-----------------------------------------------------
           VerificaNodoOccupato(FALSE, NumAgv, NodoCosiderato1, NodoOccupabile1, AgvPerInterruzionePercorso1, satellite_occupato1, NodoInterruzionePercorso1, 0, altro_agv);
           //-----------------------------------------------------
           // Se anche solo un nodo non è occupabile il percorso
           // è da ritenersi prenotato
           //-----------------------------------------------------
           if(NodoOccupabile1==0){
              //-------------------------------------------
              // Controllo se si tratta di una zona
              // fuori ingombro manuale
              //-------------------------------------------
              zona_fuori_ingombro_manuale = N.verifica_zona_fuori_ingombro_manuale(NumAgv, NodoCosiderato1, (short int)(AgvPerInterruzionePercorso1));
              if(zona_fuori_ingombro_manuale==false) break; // NODO NON OCCUPABILE
           }
        }
        if(NodoOccupabile1==1) continue;
        //---------------------------------------------------------
        // Se Invece 2 NODI prenotati in senso inverso --> ERROR
        //---------------------------------------------------------
        satellite_occupato = NodoConsiderato;
        AgvAdiacente       = altro_agv;
        break;
     }
  }

  //---------------------------------------------------------
  // Recupero l'informazione di Nodo Occupabile
  //---------------------------------------------------------
  NodoOccupabile=1;
  if( TotFuoriIng >= MIN_NODI_FUORI_INGOMBRO ) NodoOccupabile=TotFuoriIng;
  if( satellite_occupato>0                   ) NodoOccupabile=0;

  return 0;
}

//------------------------------------------------------------------
//  VerificaNodoPrenotatoIngombroAgv()
//------------------------------------------------------------------
//  Restituisce :
//
//     NodoOccupabile           -> 1 OK
//     satellite_occupato       -> Ha significato solo locale perchè poi viene reinizializzato in occupa_nodi()
//     NodoInterruzionePercorso -> 0...xx
//
//
int VerificaNodoPrenotatoIngombroAgv(int NumAgv, int NodoConsiderato, int &NodoOccupabile, int &AgvAdiacente, int &satellite_occupato, int &NodoInterruzionePercorso )
{
  double Dist;
  double Dist1, Dist2;
  int    DistX, DistY, j, k, z;
  bool   nodo_cuscinetto;
  bool   tgv_lontani_anni_luce;
  bool   zona_fuori_ingombro_manuale;
  bool   agv_in_rettaY, agv_in_rettaX;
  bool   altro_agv_in_rettaY, altro_agv_in_rettaX;
  int    altro_agv;
  int    SempreON=1;
  int    rot1, rot2;
  int    ptr_percorso, ptr_percorso1, ptr_pos_percorso;
  int    ptrpath_pint_adiacente, ptrpath_pos_adiacente;
  int    satellite_occupato1, NodoInterruzionePercorso1;
  int    NodoCosiderato1, NodoOccupabile1, AgvPerInterruzionePercorso1;

  int    count_nodi_cuscinetto;
  int    count_nodi_inseguimento;
  int    count_nodi_allontanamento;
  int    ptr_start_insegui, ptr_start_insegui_adiacente;
  int    ptr_satellite_occupato;
//int    AltroAgv_NodoOccupato;


  //--------------------------------------------------------------
  // Inizializza Variabili
  //--------------------------------------------------------------
//NodoOccupabile    = 0;
//AgvAdiacente      = 0;
//satellite_occupato= 0;
  if( NodoConsiderato <= 0       ) return 1;
  if( NodoConsiderato > TOTPUNTI ) return 1;


  //--------------------------------------------------------------
  // Nodo destinazione (salto controllo prenotati) -> occupabile
  //--------------------------------------------------------------
  if(NodoConsiderato==AGV[NumAgv]->mission.pend && NodoConsiderato==NodoInterruzionePercorso){
     NodoOccupabile=1;
     return 0;
  }

  //--------------------------------------------------------------
  // (G4) Se il percorso non è completamente libero
  //      occupa comunque fino al primo nodo prenotato
  //      da altro AGVN.B.
  //--------------------------------------------------------------
  if( SempreON ){
     //--------------------------------------------------------------------------------
     // Cerca punto del percorso all'interno della missione
     // se NON presente --> SKIP
     //--------------------------------------------------------------------------------
     ptr_pos_percorso=0;
     for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
        //------------------------------------------------
        // Memorizzo l'indice di percorso della posizione
        // tgv
        //------------------------------------------------
        if(AGV[ NumAgv ]->mission.punto[ptr_percorso]==AGV[ NumAgv ]->stato.pos) ptr_pos_percorso=ptr_percorso;
        //------------------------------------------------
        // Interrompo indice di percorso del nodo
        // considerato
        //------------------------------------------------
        if(AGV[ NumAgv ]->mission.punto[ptr_percorso]==NodoConsiderato) break;
        if(AGV[ NumAgv ]->mission.punto[ptr_percorso]==0) break;
     }
     if(AGV[ NumAgv ]->mission.punto[ptr_percorso]==0) return 1;
     //------------------------------------------------------
     // Verifica se esiste un nodo prenotato
     //------------------------------------------------------
     for(altro_agv=1; altro_agv<=MAXAGV; altro_agv++){
        if(altro_agv==NumAgv) continue;
        if(!AGV[altro_agv]->test_mission_on()) continue;
        //---------------------------------------------------
        // Nodo occupato fino a destinazione
        //---------------------------------------------------
        if(AGV[altro_agv]->mission.pend==AGV[altro_agv]->mission.pintend) continue;
        //---------------------------------------------------
        // Indice punto intermedio ALTRO TGV
        //---------------------------------------------------
        for(j=1; j<MAXPERCORSI; j++){
           if(AGV[altro_agv]->mission.punto[j]==0                               ) break;
           if(AGV[altro_agv]->mission.punto[j]==AGV[altro_agv]->mission.pend    ) break;
           if(AGV[altro_agv]->mission.punto[j]!=AGV[altro_agv]->mission.pintend ) continue;
           ptrpath_pint_adiacente=j;
           break;
        }
        //---------------------------------------------------
        // Indice posizione ALTRO AGV
        //---------------------------------------------------
        ptrpath_pos_adiacente=0;
        for(j=1; j<MAXPERCORSI; j++){
           if(AGV[altro_agv]->mission.punto[j]==0                            ) break;
           if(AGV[altro_agv]->mission.punto[j]==AGV[altro_agv]->mission.pend ) break;
           if(AGV[altro_agv]->mission.punto[j]!=AGV[altro_agv]->stato.pos    ) continue;
           ptrpath_pos_adiacente=j;
           break;
        }
        //---------------------------------------------------
        // Indice punto intermedio ALTRO TGV
        //---------------------------------------------------
        ptrpath_pint_adiacente=0;
        for(j=1; j<MAXPERCORSI; j++){
           if(AGV[altro_agv]->mission.punto[j]==0                               ) break;
           if(AGV[altro_agv]->mission.punto[j]==AGV[altro_agv]->mission.pend    ) break;
           if(AGV[altro_agv]->mission.punto[j]!=AGV[altro_agv]->mission.pintend ) continue;
           ptrpath_pint_adiacente=j;
           break;
        }
        //-------------------------------------------------
        // GESTIONE INSEGUIMENTI E ALLONTANAMENTI
        //-------------------------------------------------
        count_nodi_cuscinetto=0;
        count_nodi_inseguimento=0;
        count_nodi_allontanamento=0;
        ptr_start_insegui=MAXPERCORSI+1;
        ptr_start_insegui_adiacente=MAXPERCORSI+1;
        //-----------------------------------------------
        // Verifico se il primo nodo prenotato in comune
        // tra tgv considerato e altro_tgv è già BUSY
        //-----------------------------------------------
        for(k=ptr_pos_percorso+1; k<MAXPERCORSI; k++){
           if(AGV[NumAgv]->mission.punto[k+1]==0                           ) break;
           if(nodi_prenotati[AGV[NumAgv]->mission.punto[k]][altro_agv-1]==0) continue;
           //----------------------------------------------
           // INSEGUIMENTO
           // Il primo nodo prenotato dall'altro TGV
           // deve essere anche occupato
           //----------------------------------------------
           if(N.nodo_busy[AGV[NumAgv]->mission.punto[k]]==altro_agv){
              ptr_start_insegui=k;
              for(z=ptrpath_pos_adiacente; z<MAXPERCORSI; z++){
                 if(AGV[altro_agv]->mission.punto[z+1]==0                          ) break;
                 if(AGV[altro_agv]->mission.punto[z]!=AGV[NumAgv]->mission.punto[k]) continue;
                 ptr_start_insegui_adiacente=z;
                 break;
              }
           }
           //----------------------------------------------
           // Al primo prenotato/occupato --> skip!
           //----------------------------------------------
           break;
        }
        //-------------------------------------------------------
        // INSEGUIMENTO E ALLONTANAMENTO
        // condizione: primo nodo in comune tra i TGV
        // deve già essere occupato da altro_agv
        //-------------------------------------------------------
        if(ptr_start_insegui<MAXPERCORSI && ptr_start_insegui_adiacente<MAXPERCORSI){
           //-------------------------------------------------------
           // AL-10/03/2022 Memorizzo distanza tra nodo considerato
           // e punto di inizio inseguimento
           //-------------------------------------------------------
           Dist1=0;
           if(N.VerificaDistanzaPunti(NodoConsiderato, AGV[NumAgv]->mission.punto[ptr_start_insegui], &DistX, &DistY, &Dist1)) Dist1=9999;
           //-------------------------------------------------------
           // VERIFICA INSEGUIMENTO
           // I TGV proseguono nella stessa direzione con nodi
           // in comune e nessuna inversione di marcia
           //-------------------------------------------------------
           z=ptr_start_insegui_adiacente;
           for(k=ptr_start_insegui; k<MAXPERCORSI; k++){
              if(AGV[NumAgv]->mission.punto[k+1]==0) break;
              //-------------------------------------
              // Nodi non in comune --> skip!!
              //-------------------------------------
              if(AGV[NumAgv]->mission.punto[k]!=AGV[altro_agv]->mission.punto[z]) break;
              //-------------------------------------
              // Punti in comune
              //-------------------------------------
              if(AGV[NumAgv]->mission.punto[k]==AGV[altro_agv]->mission.punto[z]){
                 //-------------------------------------
                 // Stessa rotazione sul punto
                 //-------------------------------------
                 if(AGV[NumAgv]->rot_standard(k)==AGV[altro_agv]->rot_standard(z)){
                    //-------------------------------------
                    // Inseguimento (stessa direzione)
                    //-------------------------------------
                    if(AGV[NumAgv]->mission.dati_perc[k].dir==AGV[altro_agv]->mission.dati_perc[z].dir){
                       count_nodi_inseguimento++;
                    }
                    //-------------------------------------
                    // Inversione (direzione inversa)
                    //
                    // AL-11/06/2020 - l'inversione deve
                    // essere fatta dall'altro TGV non dal
                    // considerato
                    //
                    // AL-10/03/2022 - viene considerata
                    // solo se TGV considerato già vicino
                    // ad altro_tgv in inversione
                    //-------------------------------------
                  //if(z>0 && AGV[altro_agv]->mission.dati_perc[z-1].dir!=AGV[altro_agv]->mission.dati_perc[z].dir){
                    if(Dist1<DIST_MIN_ROT_DOPPIA_MAX && z>0 && AGV[altro_agv]->mission.dati_perc[z-1].dir!=AGV[altro_agv]->mission.dati_perc[z].dir){
                       if(AGV[NumAgv]->mission.dati_perc[k].dir!=AGV[altro_agv]->mission.dati_perc[z].dir){
                          count_nodi_inseguimento=0;
                          break;
                       }
                    }
                 }
              }
              z++;
           }
        }
        if(count_nodi_inseguimento>0) continue;
        //----------------------------------------------------------------
        // TGV LONTANI ANNI LUCE
        //----------------------------------------------------------------
        tgv_lontani_anni_luce=true;
        ptr_percorso1=ptr_percorso;
        for(; ptr_percorso1<MAXPERCORSI; ptr_percorso1++){
           if(AGV[ NumAgv ]->mission.punto[ptr_percorso1+1]==0    ) break;
           if(AGV[ NumAgv ]->mission.punto[ptr_percorso1]<1       ) continue;
           if(AGV[ NumAgv ]->mission.punto[ptr_percorso1]>TOTPUNTI) continue;
           NodoCosiderato1=AGV[ NumAgv ]->mission.punto[ptr_percorso1];
           for(k=0; k<MAXPERCORSI; k++){
              if(AGV[altro_agv]->mission.punto_prenotato[k+1]==0) break;
              Dist1=0;
              if(N.VerificaDistanzaPunti(NodoCosiderato1, AGV[altro_agv]->mission.punto_prenotato[k], &DistX, &DistY, &Dist1)) Dist1=9999;
              if(Dist1>=DIST_MIN_ROT_DOPPIA_MAX) continue;
              tgv_lontani_anni_luce=false;
              break;
           }
           if(tgv_lontani_anni_luce==false) continue;
        }
        if(tgv_lontani_anni_luce==true) continue;
        //----------------------------------------------------------------
        // Verifico la condizione di occupabilità dei nodi di percorso
        // rispetto ai tutti gli altri TGV.
        //----------------------------------------------------------------
        ptr_percorso1=ptr_percorso;
        NodoOccupabile1=AgvPerInterruzionePercorso1=satellite_occupato1=NodoInterruzionePercorso1=0;
        for(; ptr_percorso1<MAXPERCORSI; ptr_percorso1++){
           if(AGV[ NumAgv ]->mission.punto[ptr_percorso1+1]==0    ) break;
           if(AGV[ NumAgv ]->mission.punto[ptr_percorso1]<1       ) continue;
           if(AGV[ NumAgv ]->mission.punto[ptr_percorso1]>TOTPUNTI) continue;
           //-----------------------------------------------------
           // Verifico occupabilità di tutto il percorso
           //-----------------------------------------------------
           NodoCosiderato1=AGV[ NumAgv ]->mission.punto[ptr_percorso1];
           VerificaNodoOccupatoIngombroAgv(FALSE, NumAgv, NodoCosiderato1, NodoOccupabile1, AgvPerInterruzionePercorso1, satellite_occupato1, NodoInterruzionePercorso1, 0, altro_agv);
           //--------------------------------------------------
           // STATO ROTAZIONE AGV CONSIDERATO
           //--------------------------------------------------
           agv_in_rettaY=agv_in_rettaX=false;
           rot1 = AGV[NumAgv]->mission.dati_perc[ptr_percorso1+0].rot;
           rot2 = AGV[NumAgv]->mission.dati_perc[ptr_percorso1+1].rot;
           if(ptr_percorso1>0){
              rot1 = AGV[NumAgv]->mission.dati_perc[ptr_percorso1-1].rot;
              rot2 = AGV[NumAgv]->mission.dati_perc[ptr_percorso1+0].rot;
           }
           if(rot1<4 && rot2<4 && rot1==rot2){
              if(rot1==0 || rot1==2) agv_in_rettaY=true;
              if(rot1==1 || rot1==3) agv_in_rettaX=true;
           }
           //--------------------------------------------------
           // VERIFICO ZONA CUSCINETTO
           // Se il nodo oltre a essere occupabile è anche
           // a DIST_ROT_DOPPIA_MAX da tutti i nodi_prenotati
           // dall'altra navetta -> NODO CUSCINETTO
           //--------------------------------------------------
           if(NodoOccupabile1!=0){
              nodo_cuscinetto=true;
              for(j=ptrpath_pint_adiacente; j<MAXPERCORSI; j++){
                 if(AGV[altro_agv]->mission.punto[j+1]==0) break;
                 Dist=0;
                 if(N.VerificaDistanzaPunti(NodoCosiderato1, AGV[altro_agv]->mission.punto[j], &DistX, &DistY, &Dist)) Dist=9999;
                 if(Dist>=DIST_MIN_ROT_DOPPIA_MAX) continue;
                 //--------------------------------------------------
                 // STATO ROTAZIONE ALTRO AGV
                 //--------------------------------------------------
                 altro_agv_in_rettaY=altro_agv_in_rettaX=false;
                 //--------------------------------------------------
                 // ALTRO TGV IN MOVIMENTO
                 //--------------------------------------------------
                 if(AGV[altro_agv]->stato.pos!=AGV[altro_agv]->mission.pintend){
                    rot1 = AGV[altro_agv]->mission.dati_perc[j+0].rot;
                    rot2 = AGV[altro_agv]->mission.dati_perc[j+1].rot;
                    if(j>0){
                       rot1 = AGV[altro_agv]->mission.dati_perc[j-1].rot;
                       rot2 = AGV[altro_agv]->mission.dati_perc[j+0].rot;
                    }
                    if(rot1<4 && rot2<4 && rot1==rot2){
                       if(rot1==0 || rot1==2) altro_agv_in_rettaY=true;
                       if(rot1==1 || rot1==3) altro_agv_in_rettaX=true;
                    }
                    //--------------------------------------------------
                    // RETTE PARALLELE
                    //--------------------------------------------------
                    if(agv_in_rettaY==true && altro_agv_in_rettaY==true){
                       if(DistX>=DIST_MIN_RETTE_PARALLELE) continue;
                    }
                    if(agv_in_rettaX==true && altro_agv_in_rettaX==true){
                       if(DistY>=DIST_MIN_RETTE_PARALLELE) continue;
                    }
                 }
                 //--------------------------------------------------
                 // AL-10/03/2022 - ALTRO TGV FERMO SU NODO
                 // determino agv in retta da stato rotazione
                 // e in caso di distanza rette parallele posso
                 // procedere perchè altra navetta ferma
                 //--------------------------------------------------
                 else{
                    if(AGV[altro_agv]->stato.s.bit.grd0  ) altro_agv_in_rettaY=true;
                    if(AGV[altro_agv]->stato.s.bit.grd90 ) altro_agv_in_rettaX=true;
                    if(AGV[altro_agv]->stato.s.bit.grd180) altro_agv_in_rettaY=true;
                    if(AGV[altro_agv]->stato.s.bit.grd270) altro_agv_in_rettaX=true;
                    //--------------------------------------------------
                    // RETTE PARALLELE
                    //--------------------------------------------------
                    if(agv_in_rettaY==true && altro_agv_in_rettaY==true){
                       if(DistX>=DIST_MIN_RETTE_PARALLELE) break;
                    }
                    if(agv_in_rettaX==true && altro_agv_in_rettaX==true){
                       if(DistY>=DIST_MIN_RETTE_PARALLELE) break;
                    }
                 }
                 //--------------------------------------------------
                 // No zona cuscinetto
                 //--------------------------------------------------
                 nodo_cuscinetto=false;
                 break;
              }
              //---------------------------------------------
              // Conteggio nodi occupabili a una distanza
              // di SICUREZZA da tutti i nodi dell'altro agv
              //---------------------------------------------
              if(nodo_cuscinetto==true) count_nodi_cuscinetto++;
              //---------------------------------------------------------
              // Se incotro un nodo non occupabile, ma ho trovato
              // un'area coscinetto --> posso proseguire!!!
              //---------------------------------------------------------
              if(count_nodi_cuscinetto!=0 && count_nodi_cuscinetto>=MIN_NODI_FUORI_INGOMBRO) break;
           }
           //-----------------------------------------------------
           // Se anche solo un nodo non è occupabile il percorso
           // è da ritenersi prenotato
           //-----------------------------------------------------
           if(NodoOccupabile1==0){
              //-------------------------------------------
              // Controllo se si tratta di una zona
              // fuori ingombro manuale
              //-------------------------------------------
              zona_fuori_ingombro_manuale = N.verifica_zona_fuori_ingombro_manuale(NumAgv, NodoCosiderato1, (short int)(AgvPerInterruzionePercorso1));
              if(zona_fuori_ingombro_manuale==true){NodoOccupabile1=1; break;}
              else{
                 count_nodi_inseguimento=0;
                 count_nodi_allontanamento=0;
                 ptr_satellite_occupato=ptrpath_pos_adiacente;
                 for(j=ptrpath_pos_adiacente; j<MAXPERCORSI; j++){
                    if(AGV[altro_agv]->mission.punto[j+1]==0                ) break;
                    if(AGV[altro_agv]->mission.punto[j]!=satellite_occupato1) continue;
                    ptr_satellite_occupato=j;
                    break;
                 }
                 //-------------------------------------------------------
                 // VERIFICA INSEGUIMENTO
                 //-------------------------------------------------------
                 ptr_start_insegui=MAXPERCORSI+1;
                 ptr_start_insegui_adiacente=MAXPERCORSI+1;
                 for(k=ptr_percorso1; k<MAXPERCORSI; k++){
                    if(AGV[NumAgv]->mission.punto[k+1]==0                             ) break;
                    if(N.nodo_busy[AGV[NumAgv]->mission.punto[k]]!=altro_agv          ) continue;
                    for(j=ptr_satellite_occupato+1; j<MAXPERCORSI; j++){
                       if(AGV[altro_agv]->mission.punto[j+1]==0) break;
                       if(AGV[NumAgv]->mission.punto[k]!=AGV[altro_agv]->mission.punto[j]) continue;
                       ptr_start_insegui=k;
                       ptr_start_insegui_adiacente=j;
                       break;
                    }
                    if(ptr_start_insegui<MAXPERCORSI && ptr_start_insegui_adiacente<MAXPERCORSI) break;
                 }
                 if(ptr_start_insegui<MAXPERCORSI && ptr_start_insegui_adiacente<MAXPERCORSI){
                    z=ptr_start_insegui_adiacente;
                    for(k=ptr_start_insegui; k<MAXPERCORSI; k++){
                       if(AGV[NumAgv]->mission.punto[k+1]==0) break;
                       //-------------------------------------
                       // Nodi non in comune --> skip!!
                       //-------------------------------------
                       if(AGV[NumAgv]->mission.punto[k]!=AGV[altro_agv]->mission.punto[z]) break;
                       //-------------------------------------
                       // Punti in comune
                       //-------------------------------------
                       if(AGV[NumAgv]->mission.punto[k]==AGV[altro_agv]->mission.punto[z]){
                          //-------------------------------------
                          // Stessa rotazione sul punto
                          //-------------------------------------
                          if(AGV[NumAgv]->rot_standard(k)==AGV[altro_agv]->rot_standard(z)){
                             //-------------------------------------
                             // Inseguimento (stessa direzione)
                             //-------------------------------------
                             if(AGV[NumAgv]->mission.dati_perc[k].dir==AGV[altro_agv]->mission.dati_perc[z].dir){
                                count_nodi_inseguimento++;
                             }
                          }
                       }
                    }
                 }
                 if(count_nodi_inseguimento>0){NodoOccupabile1=1; break;}
                 //-------------------------------------------------------
                 // VERIFICA ALLONTANAMENTO
                 //-------------------------------------------------------
                 for(j=ptr_satellite_occupato; j<MAXPERCORSI; j++){
                    if(AGV[altro_agv]->mission.punto[j+1]==0) break;
                    Dist1=0;
                    if(N.VerificaDistanzaPunti(NodoConsiderato, AGV[altro_agv]->mission.punto[j], &DistX, &DistY, &Dist1)) Dist1=9999;
                    Dist2=0;
                    if(N.VerificaDistanzaPunti(NodoConsiderato, AGV[altro_agv]->mission.punto[j+1], &DistX, &DistY, &Dist2)) Dist2=9999;
                    if(Dist1<DIST_MIN_ROT_DOPPIA_MAX || Dist2<DIST_MIN_ROT_DOPPIA_MAX){
                       if(Dist2<Dist1){count_nodi_allontanamento=0; break;}
                    }
                    //--------------------------------------------------
                    // INCREMENTO ALLONTANAMENTO
                    //--------------------------------------------------
                    count_nodi_allontanamento++;
                 }
                 if(count_nodi_allontanamento>0){NodoOccupabile1=1; break;}
                 //-------------------------------------------------------
                 // in ogni caso (Occupabile o No) --> Skip!
                 //-------------------------------------------------------
                 break;
              }
           }
        }
        //---------------------------------------------
        // Nodo comunque occupabile --> continue
        //---------------------------------------------
        if(NodoOccupabile1!=0) continue;
        //---------------------------------------------------------
        // NODO NON OCCUPABILE!!! --> break;
        //---------------------------------------------------------
        NodoOccupabile     = 0;
        satellite_occupato = satellite_occupato1;
        AgvAdiacente       = altro_agv;
        break;
     }
  }
  return 0;
}

//------------------------------------------------------------------
//  VerificaRetteParallele( )
//------------------------------------------------------------------
//  Restituisce :
//
//     NodoOccupabile           -> 1 OK
//     satellite_occupato       -> Ha significato solo locale perchè poi viene reinizializzato in occupa_nodi()
//     NodoInterruzionePercorso -> 0...xx
//
//
int VerificaRetteParallele(int NumAgv, bool *RetteParallele, int Nodo1, int Nodo2,
                           int & NodoOccupabile, int & AgvAdiacente, int & satellite_occupato, int & NodoInterruzionePercorso )
{
  int  i, x0, y0, x1, y1, x2, y2;
  int  xv1, yv1, xv2, yv2;
  int  Vicino1, Vicino2;
  double CatetoX, CatetoY, CalcolaRad;
  int  Dist;
  int  DistX_A, DistY_A;   // distanze tra NODO1 e NODO2
  int  DistX_B, DistY_B;   // distanze tra NODO1 e VICINO1
  int  DistX_C, DistY_C;   // distanze tra NODO2 e VICINO2
  int  DistX_D, DistY_D;   // distanze tra VICINO1 e VICINO2
  int  rot1, rot2;
  bool RettaInNodo1;
  bool RettaInNodo2;
  bool nodo_libero;
  bool tgv_adicente_gia_fuori;
  bool rotazione_in_curva;

  int  SempreON=1;

  //-----------------------------------------------
  // Inizializza Variabili
  //-----------------------------------------------
  if( Nodo1 <= 0       ) return 1;
  if( Nodo1 > TOTPUNTI ) return 1;
  if( Nodo2 <= 0       ) return 1;
  if( Nodo2 > TOTPUNTI ) return 1;
  //-----------------------------------------------
  // 07/03/2012
  // Scarto il nodo di start se è un PT IN IGOMBRO
  // perchè l'altro TGV potrebbe essere fermo per
  // me stesso
  //-----------------------------------------------
  if(N.punti_notevoli[AGV[NumAgv]->stato.pos]==true && N.pt_in_ingombro[AGV[NumAgv]->stato.pos]==true){
     return 0;
  }
  //-----------------------------------------------
  // VERIFICO PRESENZA DI UN QUADRATO CRITICO
  //-----------------------------------------------
  if( SempreON ){
//   *RetteParallele=false;
     //---------------------------------------------------------
     // (G3) Se il NODO è libero verifica se quelli circostanti
     //      siano altrettanto liberi
     //---------------------------------------------------------
     x1 = Punti[ Nodo1 ][0];
     y1 = Punti[ Nodo1 ][1];
     x2 = Punti[ Nodo2 ][0];
     y2 = Punti[ Nodo2 ][1];
     DistX_A = abs(x1-x2);
     DistY_A = abs(y1-y2);
     //---------------------------------------------------------
     // Verifico che i punti siano in retta
     //---------------------------------------------------------
     if((DistX_A<10 || DistY_A<10) && N.nodo_busy[Nodo1]==0){
        //--------------------------------------------------------------------------
        // (G3) Ricerca di altri due nodi in un intorno di DIST_MIN_RETTE_PARALLELE
        //      prenotati da un altro TGV
        //--------------------------------------------------------------------------
        for( Vicino1=10; Vicino1<=TOTPUNTI; Vicino1++){
           if(Nodo1 == Vicino1                ) continue;
           if(Nodo2 == Vicino1                ) continue;
           if(N.n[ Vicino1 ]->num ==0         ) continue;
           if(N.punti_notevoli[Vicino1]==true ) continue;   // Punto terminale non a rischio
           //------------------------------------------------------------------------------
           // Un quadrato viene considerato critico se i nodi che ne fanno parte sono
           // almeno prenotati da un TGV
           //------------------------------------------------------------------------------
           nodo_libero=true;
           for(i=1; i<=MAXAGV; i++){
              if(nodi_prenotati[Vicino1][i-1]==0) continue;
              //----------------------------------------------
              // 24/08/2011
              // Se il TGV che prenota il nodo è fermo devo
              // controllare la distanza tra la posizione
              // del TGV è quella del NODO1 e se supera
              // la distanza DIST_MIN_ROT_DOPPIA_MAX non
              // lo considero
              //----------------------------------------------
              if(AGV[i]->stato.pos==AGV[i]->mission.pintend && N.nodo_busy[Vicino1]==0){
                 x0 = Punti[ AGV[i]->stato.pos ][0];
                 y0 = Punti[ AGV[i]->stato.pos ][1];
                 CatetoX = (double) abs(x1-x0);
                 CatetoY = (double) abs(y1-y0);
                 if(CatetoX>600 || CatetoY>600) continue;
                 CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
                 Dist = (int) sqrt_algo( CalcolaRad );
                 if(Dist>=DIST_MIN_ROT_DOPPIA_MAX) continue;
              }
              //----------------------------------------------
              // Nodo impegnato
              //----------------------------------------------
              nodo_libero=false;
              break;
           }
           if(nodo_libero==true) continue;
           //------------------------------------------------------------------------------
           // Verifico se il VICINO1 di trova in retta con il NODO1
           //------------------------------------------------------------------------------
           xv1 = Punti[ Vicino1 ][0];
           yv1 = Punti[ Vicino1 ][1];
           DistX_B = (int) abs(x1-xv1);
           DistY_B = (int) abs(y1-yv1);
           if(DistX_B>10 && DistY_B>10  ) continue;
           if(DistX_B>530 || DistY_B>530) continue;
           //------------------------------------------------------------------------------
           // La retta tra NODO1 e NODO2 deve formare un angolo retto con la retta tra
           // NODO1 e VICINO1
           //------------------------------------------------------------------------------
           if(DistX_A<=10 && DistY_B>10) continue;
           if(DistY_A<=10 && DistX_B>10) continue;
           //------------------------------------------------------------------------------
           // Se la distanza tra il NODO1 e VICINO1 e > DIST_MIN_RETTE_PARALLELE -> Skip!
           //------------------------------------------------------------------------------
         //if(DistX_B<=10 && DistY_B>DIST_MIN_RETTE_PARALLELE) continue;
         //if(DistY_B<=10 && DistX_B>DIST_MIN_RETTE_PARALLELE) continue;
           if(DistX_B<=10 && DistY_B>DIST_MIN_RETTE_PARALLELE && DistX_A>DIST_MIN_RETTE_PARALLELE) continue;
           if(DistY_B<=10 && DistX_B>DIST_MIN_RETTE_PARALLELE && DistY_A>DIST_MIN_RETTE_PARALLELE) continue;
           //-------------------------------------------------------------------------------
           // Verifico se il VICINO2 in prossimità di NODO2 e che forma un quadrato critico
           // di percorso.
           //-------------------------------------------------------------------------------
           for(Vicino2=10; Vicino2<=TOTPUNTI; Vicino2++){
              if(Nodo1==Vicino2                 ) continue;
              if(Nodo2==Vicino2                 ) continue;
              if(N.n[Vicino2]->num ==0          ) continue;
              if(N.punti_notevoli[Vicino2]==true) continue;   // Punto terminale non a rischio
              //------------------------------------------------------------------------------
              // Un quadrato viene considerato critico se i nodi che ne fanno parte sono
              // almeno prenotati da un TGV
              //------------------------------------------------------------------------------
              nodo_libero=true;
              for(i=1; i<=MAXAGV; i++){
                 if(nodi_prenotati[Vicino2][i-1]==0) continue;
                 //----------------------------------------------
                 // 24/08/2011
                 // Se il TGV che prenota il nodo è fermo devo
                 // controllare la distanza tra la posizione
                 // del TGV è quella del NODO2 e se supera
                 // la distanza DIST_MIN_ROT_DOPPIA_MAX non
                 // lo considero
                 //----------------------------------------------
                 if(AGV[i]->stato.pos==AGV[i]->mission.pintend && N.nodo_busy[Vicino2]==0){
                    x0 = Punti[ AGV[i]->stato.pos ][0];
                    y0 = Punti[ AGV[i]->stato.pos ][1];
                    CatetoX = (double) abs(x2-x0);
                    CatetoY = (double) abs(y2-y0);
                    if(CatetoX>600 || CatetoY>600) continue;
                    CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
                    Dist = (int) sqrt_algo( CalcolaRad );
                    if(Dist>=DIST_MIN_ROT_DOPPIA_MAX) continue;
                 }
                 //----------------------------------------------
                 // Nodo impegnato
                 //----------------------------------------------
                 nodo_libero=false;
                 break;
              }
              if(nodo_libero==true) continue;
              //------------------------------------------------------------------------------
              // Nel caso il VICINO sia prenotato verifico se il suo precedente è in retta
              // con il NODO2
              //------------------------------------------------------------------------------
              xv2 = Punti[ Vicino2 ][0];
              yv2 = Punti[ Vicino2 ][1];
              DistX_C = (int) abs(x2-xv2);
              DistY_C = (int) abs(y2-yv2);
              if(DistX_C>10 && DistY_C>10  ) continue;
              if(DistX_C>530 || DistY_C>530) continue;
              //------------------------------------------------------------------------------
              // La retta tra NODO1 e NODO2 deve formare un angolo retto con la retta tra
              // NODO2 e VICINO2
              //------------------------------------------------------------------------------
              if(DistX_A<=10 && DistY_C>10) continue;
              if(DistY_A<=10 && DistX_C>10) continue;
              //------------------------------------------------------------------------------
              // Se la distanza tra il NODO2 e VICINO2 e > DIST_MIN_RETTE_PARALLELE -> Skip!
              //------------------------------------------------------------------------------
            //if(DistX_C<=10 && DistY_C>DIST_MIN_RETTE_PARALLELE) continue;
            //if(DistY_C<=10 && DistX_C>DIST_MIN_RETTE_PARALLELE) continue;
              if(DistX_C<=10 && DistY_C>DIST_MIN_RETTE_PARALLELE && DistX_A>DIST_MIN_RETTE_PARALLELE) continue;
              if(DistY_C<=10 && DistX_C>DIST_MIN_RETTE_PARALLELE && DistY_A>DIST_MIN_RETTE_PARALLELE) continue;
              //------------------------------------------------------------------------------
              // Verifico che il VICINO2 sia in linea con VICINO1 per chiudere il quadrato
              // critico
              //------------------------------------------------------------------------------
              DistX_D = (int) abs(xv2-xv1);
              DistY_D = (int) abs(yv2-yv1);
              if(DistX_B<=10 && DistY_D>10) continue;      // VICINO1 e VICINO2 non in linea
              if(DistY_B<=10 && DistX_D>10) continue;      // VICINO1 e VICINO2 non in linea
              //------------------------------------------------------------------------------
              // Se il TGV è già nel quadrato crititico ormai non ha più senso verificare la
              // condizione di rette parallele
              //------------------------------------------------------------------------------
              if(N.nodo_busy[Nodo1]==NumAgv    ) break;
              if(N.nodo_busy[Nodo2]==NumAgv    ) break;
              if(N.nodo_busy[Vicino1]==NumAgv  ) break;
              if(N.nodo_busy[Vicino2]==NumAgv  ) break;
              //------------------------------------------------------------------------------
              // AL-05/09/15 TGV CONSIDERATO TROPPO IN PROSSIMITA' QUADRATO CRITICO
              // Se il tgv considerato non si trova almeno a DIST_MIN_ROT_SINGOLA tanto vale
              // occupare il nodo pechè l'altro TGV non avrebbe comunque la possibilità di
              // passare
              //------------------------------------------------------------------------------
              x0 = Punti[ AGV[NumAgv]->mission.pintend ][0];
              y0 = Punti[ AGV[NumAgv]->mission.pintend ][1];
              CatetoX = (double) abs(x1-x0);
              CatetoY = (double) abs(y1-y0);
              if(CatetoX>600 || CatetoY>600) continue;
              CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
              Dist = (int) sqrt_algo( CalcolaRad );
              if(Dist<DIST_MIN_ROT_SINGOLA) break;
              //------------------------------------------------------------------------------
              // Se il TGV deve proseguire dritto senza fare rotazioni nel quadrato critico
              // allora non setto la condizione di rette parallele.
              // - verifico se il TGV deve ruotare in NODO1.
              // - verifico se il TGV deve ruotare in NODO2.
              // - verifico se il SUCESSIVO al NODO1 è un PUNTO NOTEVOLE.
              // - verifico se il SUCESSIVO al NODO2 è un PUNTO NOTEVOLE.
              //------------------------------------------------------------------------------
              // --- NODO1
              RettaInNodo1=false;
              for(i=0; i<MAXPERCORSI; i++){
                 if(AGV[NumAgv]->mission.punto[i]==Nodo1 ) break;
                 if(AGV[NumAgv]->mission.punto[i]==0     ) break;
              }
              if(AGV[NumAgv]->mission.punto[i]==0) break;
              if(i>0){
                 rot1 = AGV[NumAgv]->rot_standard(i  );
                 rot2 = AGV[NumAgv]->rot_standard(i-1);
                 //-----------------------------------------------------------
                 // Verifico se TGV a inizio curva
                 //-----------------------------------------------------------
                 rotazione_in_curva=false;
                 if(AGV[NumAgv]->mission.dati_perc[i].rot>3 && AGV[NumAgv]->mission.dati_perc[i].rot<40) rotazione_in_curva=true;  // a inizio curva il TGV è ancora in retta.
                 //-----------------------------------------------------------
                 // No cambio rotazione o inizio curva
                 //-----------------------------------------------------------
                 if(rot1==rot2 || rotazione_in_curva==true) RettaInNodo1=true;
                 //-----------------------------------------------------------
                 // Se il TGV è in rotazione verifico se ci sono le distanze
                 // per eseguire comunque la rotazione
                 //-----------------------------------------------------------
                 else{
                    if(rot2==0 && rot1==1 && DistY_B>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo1=true;
                    if(rot2==0 && rot1==3 && DistY_B>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo1=true;
                    if(rot2==1 && rot1==0 && DistX_B>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo1=true;
                    if(rot2==1 && rot1==2 && DistX_B>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo1=true;
                    if(rot2==2 && rot1==1 && DistY_B>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo1=true;
                    if(rot2==2 && rot1==3 && DistY_B>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo1=true;
                    if(rot2==3 && rot1==0 && DistX_B>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo1=true;
                    if(rot2==3 && rot1==2 && DistX_B>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo1=true;
                 }
              }
              // punto notevole
              if(N.punti_notevoli[AGV[NumAgv]->mission.punto[i+1]]) RettaInNodo1=true;
              // --- NODO2
              RettaInNodo2=false;
              for(i=0; i<MAXPERCORSI; i++){
                 if(AGV[NumAgv]->mission.punto[i]==Nodo2 ) break;
                 if(AGV[NumAgv]->mission.punto[i]==0     ) break;
              }
              if(AGV[NumAgv]->mission.punto[i]==0) break;
              if(i>0){
                 rot1 = AGV[NumAgv]->rot_standard(i  );
                 rot2 = AGV[NumAgv]->rot_standard(i-1);
                 //-----------------------------------------------------------
                 // Verifico se TGV a inizio curva
                 //-----------------------------------------------------------
                 rotazione_in_curva=false;
                 if(AGV[NumAgv]->mission.dati_perc[i].rot>3 && AGV[NumAgv]->mission.dati_perc[i].rot<40) rotazione_in_curva=true;  // a inizio curva il TGV è ancora in retta.
                 //-----------------------------------------------------------
                 // No cambio rotazione o inizio curva
                 //-----------------------------------------------------------
                 if(rot1==rot2 || rotazione_in_curva==true) RettaInNodo2=true;
                 //-----------------------------------------------------------
                 // Se il TGV è in rotazione verifico se ci sono le distanze
                 // per eseguire comunque la rotazione
                 //-----------------------------------------------------------
                 else{
                    if(rot2==0 && rot1==1 && DistY_C>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo2=true;
                    if(rot2==0 && rot1==3 && DistY_C>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo2=true;
                    if(rot2==1 && rot1==0 && DistX_C>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo2=true;
                    if(rot2==1 && rot1==2 && DistX_C>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo2=true;
                    if(rot2==2 && rot1==1 && DistY_C>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo2=true;
                    if(rot2==2 && rot1==3 && DistY_C>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo2=true;
                    if(rot2==3 && rot1==0 && DistX_C>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo2=true;
                    if(rot2==3 && rot1==2 && DistX_C>=DIST_MIN_ROT_DOPPIA_MAX) RettaInNodo2=true;
                 }
              }
              // punto notevole
              if(N.punti_notevoli[AGV[NumAgv]->mission.punto[i+1]]) RettaInNodo2=true;
              //------------------------------------------------------------------------------
              // Se il TGV prosegue diritto in entrambi i NODO1 e NODO2 --> skip!
              //------------------------------------------------------------------------------
              if(RettaInNodo1==true && RettaInNodo2==true) break;
              //------------------------------------------------------------------------------
              // Verifico se sono in presenza di un QUADRATO CRITICO CON RETTE PARALLELE
              //------------------------------------------------------------------------------
              *RetteParallele=true;
              //------------------------------------------------------------------------------
              // Verifico se i nodi del quadrato critico sono impegnati da un altro TGV
              //------------------------------------------------------------------------------
              VerificaNodoOccupatoRP( NumAgv, Nodo2, NodoOccupabile, AgvAdiacente, satellite_occupato, NodoInterruzionePercorso );
              if(NodoOccupabile!=0){
                 VerificaNodoPrenotato( NumAgv, Nodo2, NodoOccupabile, AgvAdiacente, satellite_occupato, NodoInterruzionePercorso );
              }
              if(NodoOccupabile!=0){
                 VerificaNodoOccupatoRP( NumAgv, Vicino1, NodoOccupabile, AgvAdiacente, satellite_occupato, NodoInterruzionePercorso );
              }
              if(NodoOccupabile!=0){
                 VerificaNodoPrenotato( NumAgv, Vicino1, NodoOccupabile, AgvAdiacente, satellite_occupato, NodoInterruzionePercorso );
              }
              if(NodoOccupabile!=0){
                 VerificaNodoOccupatoRP( NumAgv, Vicino2, NodoOccupabile, AgvAdiacente, satellite_occupato, NodoInterruzionePercorso );
              }
              if(NodoOccupabile!=0){
                 VerificaNodoPrenotato( NumAgv, Vicino2, NodoOccupabile, AgvAdiacente, satellite_occupato, NodoInterruzionePercorso );
              }
              //--------------------------------------------------------------------------------
              // Se non ho più la condizione di "NodoOccupabile" verifico che il TGV BLOCCANTE
              //--------------------------------------------------------------------------------
              if(NodoOccupabile==0 && AgvAdiacente!=0){
                 //---------------------------------------------------------------
                 // AL-16/05/13 TGV ADICENTE GIA' FUORI DA QUADRATO CRITICO
                 // Se il TGV adicente è già fuori da quadrato critico
                 // non deve essere ritenuto bloccante.
                 // Perchè sia verificata questa condizione, nessuno dei nodi
                 // del TGV deve appartenere al quadrato critico stesso.
                 //---------------------------------------------------------------
                 tgv_adicente_gia_fuori=true;
                 for(i=0; i<MAXPERCORSI; i++){
                    if(AGV[AgvAdiacente]->mission.punto_prenotato[i]==0       ) break;
                    if(AGV[AgvAdiacente]->mission.punto_prenotato[i]!=Nodo1 &&
                       AGV[AgvAdiacente]->mission.punto_prenotato[i]!=Nodo2 &&
                       AGV[AgvAdiacente]->mission.punto_prenotato[i]!=Vicino1 &&
                       AGV[AgvAdiacente]->mission.punto_prenotato[i]!=Vicino2 ) continue;
                       tgv_adicente_gia_fuori=false;
                       break;
                 }
                 //--------------------------------------------------
                 // Se il TGV BLOCCANTE non è nel quadrato critico
                 // verifico comunque se prosegue diritto e in quel
                 // caso non intralcia l'occupazione dei nodi
                 //
                 // Se invece risulta già fuori, sicuramente non
                 // è bloccante
                 //--------------------------------------------------
                 if(tgv_adicente_gia_fuori==false){
                    if(AGV[AgvAdiacente]->stato.pos!=Nodo1 &&
                       AGV[AgvAdiacente]->stato.pos!=Nodo2 &&
                       AGV[AgvAdiacente]->stato.pos!=Vicino1 &&
                       AGV[AgvAdiacente]->stato.pos!=Vicino2) break;
                    //--------------------------------------------------
                    // Verifico se nei 3 nodi sucessivi in cui si deve
                    // muovere il TGV sono liberi e senza rotazioni
                    //--------------------------------------------------
                    for(i=0; i<MAXPERCORSI; i++){
                       if(AGV[AgvAdiacente]->mission.punto[i]==AGV[AgvAdiacente]->stato.pos) break;
                       if(AGV[AgvAdiacente]->mission.punto[i]==0) break;
                    }
                    if(AGV[AgvAdiacente]->mission.punto[i]==0) break;
                    //--------------------------------------------------
                    // Verifico che il TGV proceda diritto
                    //--------------------------------------------------
                    RettaInNodo1=false;
                    rot1 = AGV[AgvAdiacente]->rot_standard(i  );
                    rot2 = AGV[AgvAdiacente]->rot_standard(i+1);
                    if(i>0 && rot1==rot2) RettaInNodo1=true;
                    if(RettaInNodo1==false) break;
                    //---------------------------------------------------------------------------------------
                    // 4/05/2005 G. Back28  ITALGRES
                    //          Se il TGV adiacente viene incontro sulla retta parallela Vicino2-->Vicino1
                    //          con VICINO1 sul lato CORTO del rettangolo --> annulla RettaInNodo
                    //---------------------------------------------------------------------------------------
                    if( i>0 && AGV[AgvAdiacente]->mission.punto[ i   ]==Vicino2 ){
                       if(     AGV[AgvAdiacente]->mission.punto[ i+1 ]==Vicino1 ){
                          if(( DistX_B< DIST_MIN_RETTE_PARALLELE) && ( DistY_B<DIST_MIN_RETTE_PARALLELE )){
                             RettaInNodo1=false;
                          }
                       }
                    }
                    if(RettaInNodo1==false) break;
                 }
                 //--------------------------------------------------
                 // Se l'AGV_ADIACENTE prosegue diritto non azzero
                 // il nodo è ritenuto occuopabile
                 //--------------------------------------------------
                 NodoOccupabile     = 1;
                 AgvAdiacente       = 0;
                 satellite_occupato = 0;
              }
              break;
           }
           if(NodoOccupabile==0) break;
        }
     }
  }
  return 0;
}
// ------------------------------------------------------------------------

// ---------------------------------------------------
//               occupa_metri()
// ---------------------------------------------------
// Data la missione del TGV e il numero massimo di metri occupabili
// questa funzione restituisce le seguenti informazioni:
// - Numero del nodo che si arriva ad occupare dai quei metri a disposizione.
// - Totale dei nodi occupabili dati quei metri a disposizione.
// - Metri realmente occupabili (visto gli arrotondamenti possibili).
//
int agv::occupa_metri(short int pstart, short int pend, short int metri_max, short int *punto, short int *tot_nodi, short int *metri, char *all_mess)
{
  int x1, y1, x2, y2;
  short int MinPercorsi;
  bool continua, punto_finale;
  bool nodo_curva_o_deviaz;
  double Dist, DistX, DistY;
  double DistDaNodo, DistTot;
  short int M, count, p1, p2;
  short int ptr_percorso;
  int AbilitaOffSetDaNodo = OCCUPA_METRI_CON_OFFSET_DA_NODO;

  *punto      = 0;
  *metri      = 0;
  *tot_nodi   = 0;
  DistTot     = 0;
  DistDaNodo  = 0;
  count       = 0;
  MinPercorsi = 2;        // lungo il percorso devo occupare almeno 2 nodi avanti
  continua            = true;
  nodo_curva_o_deviaz = false;
  //-------------------------------------------------------------
  // In caso di inizializzazione della missione devo occupare
  // un minino di nodi
  //-------------------------------------------------------------
  if(stato.start==0){
     MinPercorsi = MINPERCORSI;
     if(N.nodo_minpercorsi[stato.pos]!=99){
        MinPercorsi = N.nodo_minpercorsi[stato.pos];
     }
  }
  //-------------------------------------------------------------
  // Recupero della parte di missione interessata
  //-------------------------------------------------------------
  for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
     if(mission.punto[ptr_percorso] == pstart) break;
     if(mission.punto[ptr_percorso] == 0     ) break;
  }
  if(mission.punto[ptr_percorso] != pstart){
     sprintf(all_mess, "Errore nodo fuori percorso in distanza_metri() per il TGV %d!", agv_num);
     return 1;
  }
  //-------------------------------------------------------------
  // Scorrimento di tutta la missione restante e calcolo della
  // metratura.
  //-------------------------------------------------------------
  do{
     Dist=0;
     p1 = mission.punto[ptr_percorso];
     x1 = Punti[p1][0];
     y1 = Punti[p1][1];
     p2 = mission.punto[ptr_percorso+1];
     x2 = Punti[p2][0];
     y2 = Punti[p2][1];
     //-------------------------------------------------------------
     // Calcolo delle distanze da punto a punto
     //-------------------------------------------------------------
     DistX = abs(y1-y2);
     DistY = abs(x1-x2);
     if(DistX>30000 || DistY>30000) break;
     Dist = (double)(sqrt_algo((DistX*DistX)+(DistY*DistY)));
     //-------------------------------------------------------------
     // Calcolo della distanza totale in cm
     //-------------------------------------------------------------
     DistTot = DistTot + Dist;
     //-------------------------------------------------------------
     // Calcolo i metri già percorsi dall'ultimo nodo occupato.
     //-------------------------------------------------------------
     DistDaNodo=0;
     if(AbilitaOffSetDaNodo==1){
        DistDaNodo = (double)(stato.offset_da_nodo*mission.RapEncoder);
     }
     //-------------------------------------------------------------
     // Calcolo dei Metri
     //-------------------------------------------------------------
     M = (short int)((DistTot/100)-(DistDaNodo/100));
     if(M<0) M=0;
     //-------------------------------------------------------------
     // Verifico se il punto sucessivo e il punto finale della
     // missione
     //-------------------------------------------------------------
     punto_finale=false;
     if(mission.punto[ptr_percorso+2]==0) punto_finale=true;
     //-------------------------------------------------------------
     // Memorizzo l'ultimo punto della missione considerato.
     // (almeno un nodo lo occupo)
     // N.B. Per richiesta di Fabrizio Fontana se l'occupazione
     //      dei metri impostati dal cliente non raggiunge almeno
     //      i 2 nodi vengono forzati
     //-------------------------------------------------------------
     if(count<MinPercorsi || M<metri_max || punto_finale || nodo_curva_o_deviaz){
        //-----------------------------------------
        // Punto intermedio calcolato
        //-----------------------------------------
        *punto = p2;
        if(punto_finale) *punto=0;
        //-----------------------------------------
        // GESTIONE DEVIAZIONI O CURVE LASER
        // Verifico se il nodo considerato
        // appartiene a una CURVA O UNA DEVIAZIONE
        //-----------------------------------------
        nodo_curva_o_deviaz=false;                                              // nodo di percorso
        if(mission.dati_perc[ptr_percorso+1].rot>3) nodo_curva_o_deviaz=true;   // nodo attuale inizio curva LASER
        if(mission.dati_perc[ptr_percorso  ].rot>3) nodo_curva_o_deviaz=true;   // nodo attuale fine curva LASER
        //-----------------------------------------
        // AL-FPT 13/08/2014: abilitazione a
        // fermate su start inizio fine deviazione
        //-----------------------------------------
        if(NoPIntSuStartDeviazione==false){
           if(mission.dati_perc[ptr_percorso  ].rot>49 && mission.dati_perc[ptr_percorso  ].rot<70) nodo_curva_o_deviaz=false;
           if(mission.dati_perc[ptr_percorso-1].rot>49 && mission.dati_perc[ptr_percorso-1].rot<70) nodo_curva_o_deviaz=false;
        }
        if(NoPIntSuStartCurva==false){
           if(mission.dati_perc[ptr_percorso  ].rot> 3 && mission.dati_perc[ptr_percorso  ].rot<40) nodo_curva_o_deviaz=false;
           if(mission.dati_perc[ptr_percorso-1].rot> 3 && mission.dati_perc[ptr_percorso-1].rot<40) nodo_curva_o_deviaz=false;
        }
        //-----------------------------------------
        // Calcolo Totale punti scorsi
        //-----------------------------------------
        count++;
        //-----------------------------------------
        // Metri da occupare
        //-----------------------------------------
        *metri = M;
     }
     //-----------------------------------------
     // Condizioni di uscita dal ciclo
     //-----------------------------------------
     if(punto_finale                                                ) continua=false;
     if(p2==pend && mission.pend!=pend                              ) continua=false;
     if(M>=metri_max && count>=MinPercorsi && !nodo_curva_o_deviaz  ) continua=false;
     ptr_percorso++;
  }while(continua);
  //---------------------------------------
  // Calcolo dei Nodi scorsi
  //---------------------------------------
  *tot_nodi = count;
  return 0;
}

// -----------------------------------
//         SaltaNodi()
// -----------------------------------
// Gestione del salto nodi causa dist<DistSaltaNodoInCm.
//
void agv::SaltaNodi()
{
  short int DistSaltaNodoInCm = DIST_SALTO_NODI;
  short int i, j, ptr_percorso;
  struct DATI_PERCORSO d1;
  struct DATI_PERCORSO d2;
  struct DATI_PERCORSO d3;
  struct DATI_PERCORSO d4;
  struct DATI_PERCORSO d5;
  struct DATI_PERCORSO d6;
  bool stop_retro_salta_a_nodo;

  i=j=0;
  stop_retro_salta_a_nodo=false;
  memset(&d1, 0, sizeof(DATI_PERCORSO));
  memset(&d2, 0, sizeof(DATI_PERCORSO));
  memset(&d3, 0, sizeof(DATI_PERCORSO));
  memset(&d4, 0, sizeof(DATI_PERCORSO));
  memset(&d5, 0, sizeof(DATI_PERCORSO));
  memset(&d6, 0, sizeof(DATI_PERCORSO));
  //----------------------------------------
  // Se la distanza minima di salto non è
  // impostata o troppo alta --> skip!
  //----------------------------------------
  if(DistSaltaNodoInCm<1 || DistSaltaNodoInCm>200) return;
  //----------------------------------------
  // Recupero l'indice finale della
  // missione
  //----------------------------------------
  for(i=0; i<MAXPERCORSI; i++){
     if(mission.punto[i]==0) break;
     ptr_percorso=i;
  }
  ptr_percorso--;  // ultimo nodo prima della dest
  //----------------------------------------
  // Analisi di tutto il percorso del TGV
  //----------------------------------------
  do{
     j=ptr_percorso;
     memcpy(&d1, &mission.dati_perc[ptr_percorso  ], sizeof(DATI_PERCORSO));
     if(ptr_percorso>0) memcpy(&d2, &mission.dati_perc[ptr_percorso-1], sizeof(DATI_PERCORSO));
     if(ptr_percorso>1) memcpy(&d3, &mission.dati_perc[ptr_percorso-2], sizeof(DATI_PERCORSO));
     if(ptr_percorso>2) memcpy(&d4, &mission.dati_perc[ptr_percorso-3], sizeof(DATI_PERCORSO));
     if(ptr_percorso>3) memcpy(&d5, &mission.dati_perc[ptr_percorso-4], sizeof(DATI_PERCORSO));
     if(ptr_percorso>4) memcpy(&d6, &mission.dati_perc[ptr_percorso-5], sizeof(DATI_PERCORSO));
     //----------------------------------------------------
     // SALTA NODI INDIETRO
     // (sul nodo prec non ci deve essere un rallentamento)
     // n.b. sul nodo considerato controllo sia la dist
     //      attuale che quella precedente.
     //----------------------------------------------------
     if(j>0 && d2.ral==0 && (d2.dist<=DistSaltaNodoInCm || d1.dist<=DistSaltaNodoInCm)){
        //-------------------------------
        // indietro di nr.1 nodi
        // (sul nodo prec non ci deve
        //  essere un rallentamento)
        //-------------------------------
        j--;
        d2.salta_a_ps   = d1.ps;
        d2.salta_a_dist = (short int)(d2.dist+d1.dist);
        d2.ral          = d1.ral;
        d1.salta_a_ps   = 0;
        d1.salta_a_dist = 0;
        //-------------------------------
        // indietro di nr.2 nodi
        // (sul nodo prec non ci deve
        //  essere un rallentamento)
        //-------------------------------
        if(!stop_retro_salta_a_nodo){
           stop_retro_salta_a_nodo=true;
           if(j>2 && d3.ral==0 && d2.salta_a_dist<=DistSaltaNodoInCm){
              j--;
              d3.salta_a_ps   = d2.salta_a_ps;
              d3.salta_a_dist = (short int)(d2.salta_a_dist+d3.dist);
              d3.ral          = d2.ral;
              d2.salta_a_ps   = 0;
              d2.salta_a_dist = 0;
              stop_retro_salta_a_nodo=false;
           }
        }
        //-------------------------------
        // indietro di nr.3 nodi
        // (sul nodo prec non ci deve
        //  essere un rallentamento)
        //-------------------------------
        if(!stop_retro_salta_a_nodo){
           stop_retro_salta_a_nodo=true;
           if(j>3 && d4.ral==0 && d3.salta_a_dist<=DistSaltaNodoInCm){
              j--;
              d4.salta_a_ps   = d3.salta_a_ps;
              d4.salta_a_dist = (short int)(d3.salta_a_dist+d4.dist);
              d4.ral          = d3.ral;
              d3.salta_a_ps   = 0;
              d3.salta_a_dist = 0;
              stop_retro_salta_a_nodo=false;
           }
        }
        //-------------------------------
        // indietro di nr.4 nodi
        // (sul nodo prec non ci deve
        //  essere un rallentamento)
        //-------------------------------
        if(!stop_retro_salta_a_nodo){
           stop_retro_salta_a_nodo=true;
           if(j>4 && d5.ral==0 && d4.salta_a_dist<=DistSaltaNodoInCm){
              j--;
              d5.salta_a_ps   = d4.salta_a_ps;
              d5.salta_a_dist = (short int)(d4.salta_a_dist+d5.dist);
              d5.ral          = d4.ral;
              d4.salta_a_ps   = 0;
              d4.salta_a_dist = 0;
              stop_retro_salta_a_nodo=false;
           }
        }
        //-------------------------------
        // indietro di nr.5 nodi
        // (sul nodo prec non ci deve
        //  essere un rallentamento)
        //-------------------------------
        if(!stop_retro_salta_a_nodo){
           stop_retro_salta_a_nodo=true;
           if(j>5 && d6.ral==0 && d5.salta_a_dist<=DistSaltaNodoInCm){
              j--;
              d6.salta_a_ps   = d5.salta_a_ps;
              d6.salta_a_dist = (short int)(d5.salta_a_dist+d6.dist);
              d6.ral          = d5.ral;
              d5.salta_a_ps   = 0;
              d5.salta_a_dist = 0;
              stop_retro_salta_a_nodo=false;
           }
        }
     }
     //-----------------------------------------------------
     // Associo i nuovi dati con i rallentamenti modificati
     // alla struttura della navetta.
     //-----------------------------------------------------
     if(j==ptr_percorso  ) memcpy(&mission.dati_perc[j], &d1, sizeof(DATI_PERCORSO));
     if(j==ptr_percorso-1) memcpy(&mission.dati_perc[j], &d2, sizeof(DATI_PERCORSO));
     if(j==ptr_percorso-2) memcpy(&mission.dati_perc[j], &d3, sizeof(DATI_PERCORSO));
     if(j==ptr_percorso-3) memcpy(&mission.dati_perc[j], &d4, sizeof(DATI_PERCORSO));
     if(j==ptr_percorso-4) memcpy(&mission.dati_perc[j], &d5, sizeof(DATI_PERCORSO));
     if(j==ptr_percorso-5) memcpy(&mission.dati_perc[j], &d6, sizeof(DATI_PERCORSO));
     ptr_percorso=(short int)(j-1); // decremento indice di percorso
  }while(ptr_percorso>0);
  return;
}

// ---------------------------------------------------
//      compila_prg()
// ---------------------------------------------------
// Compila il programma (steps) relativo alla missione da inviare all'AGV
//
int agv::compila_prg(char *all_mess, int *NumNodo)
/**********************************/
{
  bool Stop;
  int stato=0, err;
  short int source, dest;
  struct NOD nod;
  struct DATI_PERCORSO dati_perc;
  short int falso_obbiettivo;

  //------------------------------------------------
  // Evita di fare una rotazione iniziale dell'AGV
  // se questo Š gi… ruotato correttamente
  //------------------------------------------------
  if( AGV[ agv_num ]->stato.s.bit.grd0   ) old_rot = 0; // test bit AGV a 0ø
  if( AGV[ agv_num ]->stato.s.bit.grd90  ) old_rot = 1; // test bit AGV a 90ø
  if( AGV[ agv_num ]->stato.s.bit.grd180 ) old_rot = 2; // test bit AGV a 180ø
  if( AGV[ agv_num ]->stato.s.bit.grd270 ) old_rot = 3; // test bit AGV a 270ø

  *NumNodo=0;
  //------------------------------
  // punti notevoli missione
  //------------------------------
  source  = mission.pstart;
  dest    = mission.pend;
  if(mission.NoMissioniAlVolo) falso_obbiettivo=mission.pintend;
  //----- azzeramento strutture RAM
  memset(&nod, 0, sizeof(NOD));
  setmem(&dati_perc,   sizeof(DATI_PERCORSO), 0);
  setmem(mission.step, sizeof(mission.step),  0);
  //------------------------------------
  // Posiziona il puntatore al percorso
  //------------------------------------
  for(ptr_path=0; ptr_path < MAXPERCORSI; ptr_path++){
     if(mission.punto[ptr_path] == mission.pintstart) break;
     if(mission.punto[ptr_path] == 0) break;
  };
  ptr_step=0;
  //--------------------------------
  // Ciclo compila steps programma
  //--------------------------------
  if(!mission.NoMissioniAlVolo){
     do{
        Stop   = false;
        source = mission.punto[ptr_path];
        memcpy(&dati_perc, &mission.dati_perc[ptr_path], sizeof(DATI_PERCORSO));
        //----------------------------------------------------
        // Verifica della validità del numero del nodo
        //----------------------------------------------------
        memset(&nod, 0, sizeof(NOD));
        stato = N.cerca_record(&nod, source );
        //----------------------------------------------------
        // Errore non trovato nodo
        //----------------------------------------------------
        if(stato){
           sprintf(all_mess, MESS[109], agv_num, source);
           return stato;
        }
        //-----------------------------------------------------------------
        // Compilazione dei dati del percorso
        //-----------------------------------------------------------------
        //cerca_path_into_nodo(mission.pstart, dest, &dati_path);
        dati_path.pt   = dest;
        dati_path.zs   = 0;
        dati_path.zp   = 0;
        dati_path.ps   = dati_perc.ps;
        dati_path.av   = !dati_perc.dir;
        dati_path.ind  = dati_perc.dir;
        dati_path.rot  = dati_perc.rot;
        dati_path.ral  = dati_perc.ral;
        dati_path.dist = dati_perc.dist;
        //-----------------------------------------------------------------
        // GESTIONE SALTO NODI AUTOMATICO
        //-----------------------------------------------------------------
        if(dati_perc.salta_a_ps!=0  ) dati_path.ps   = dati_perc.salta_a_ps;
        if(dati_perc.salta_a_dist!=0) dati_path.dist = dati_perc.salta_a_dist;
//      sprintf(all_mess, "Nodo:%d\nPt:%d, Zp:%d, Zs:%d, Pt:%d, Av:%d, Ind:%d, Rot:%d, Ral:%d, Dist:%d", source,
//                                                                                                       dati_path.pt,
//                                                                                                       dati_path.zp,
//                                                                                                       dati_path.zs,
//                                                                                                       dati_path.ps,
//                                                                                                       dati_path.av,
//                                                                                                       dati_path.ind,
//                                                                                                       dati_path.rot,
//                                                                                                       dati_path.ral,
//                                                                                                       dati_path.dist);
//      ShowMessage(all_mess);

        //-----------------------------------------------------------------
        // Compilazione dei passi di programma
        //-----------------------------------------------------------------
        err = compila_passo_prg(&nod, all_mess);              // ** compila step programma
        //---------------------------------------------------------
        // Esco dal Ciclo solo se il punto analizzato coincide con
        // la destinazione e se il nodo sucessivo è 0.
        //---------------------------------------------------------
        if(ptr_path>=MAXPERCORSI || (source==dest && mission.punto[ptr_path+1]==0)){
           Stop = true;
        }
        // 13/03/97 Inserito ritorno con errore
        if( err ) return err;
        //-----------------------------------------------------------------
        // GESTIONE SALTO NODI
        // Incremento i passi di programma fino mission.punto non coincide
        // con il punto sucessivo dei dati_path appena compilati
        //-----------------------------------------------------------------
      //ptr_path++;
        do{
           ptr_path++;
        }while(mission.punto[ptr_path]!=dati_path.ps && mission.punto[ptr_path]!=0);
        //--------------------------------
        // Errore troppi steps programma
        //--------------------------------
        if(ptr_step >= (MAXSTEP-5)){
           sprintf(all_mess, MESS[108], agv_num, source, MAXSTEP);
           *NumNodo=source;
           return 1;
        }
     //-------------------------------------------------------------------------------
     // Compila tutti i passi di programma del percorso
     //-------------------------------------------------------------------------------
     }while(!Stop);
  }
  else{
     do{
        Stop   = false;
        source = mission.punto[ptr_path];
        memcpy(&dati_perc, &mission.dati_perc[ptr_path], sizeof(DATI_PERCORSO));
        //----------------------------------------------------
        // Verifica della validità del numero del nodo
        //----------------------------------------------------
        stato = N.cerca_record(&nod, source );
        //----------------------------------------------------
        // Errore non trovato nodo
        //----------------------------------------------------
        if(stato){
           sprintf(all_mess, MESS[109], agv_num, source);
           return stato;
        }
        //-----------------------------------------------------------------
        // Compilazione dei dati del percorso
        //-----------------------------------------------------------------
        //cerca_path_into_nodo(mission.pstart, dest, &dati_path);
        dati_path.pt   = dest;
        dati_path.zs   = 0;
        dati_path.zp   = 0;
        dati_path.ps   = dati_perc.ps;
        dati_path.av   = !dati_perc.dir;
        dati_path.ind  = dati_perc.dir;
        dati_path.rot  = dati_perc.rot;
        dati_path.ral  = dati_perc.ral;
        dati_path.dist = dati_perc.dist;
        //-----------------------------------------------------------------
        // Compilazione dei passi di programma
        //-----------------------------------------------------------------
        err = compila_passo_prg(&nod, all_mess);              // ** compila step programma
        //---------------------------------------------------------
        // Esco dal Ciclo solo se il punto analizzato coincide con
        // la destinazione e se il nodo sucessivo è 0.
        //---------------------------------------------------------
        if(ptr_path>=MAXPERCORSI || (source==dest && mission.punto[ptr_path+1]==0)){
           Stop = true;
        }
        // 13/03/97 Inserito ritorno con errore
        if( err ) return err;
        ptr_path++;
        //--------------------------------
        // Errore troppi steps programma
        //--------------------------------
        if(ptr_step >= (MAXSTEP-5)){
           sprintf(all_mess, MESS[108], agv_num, source, MAXSTEP);
           *NumNodo=source;
           return 1;
        }
     //-------------------------------------------------------------------------------
     // Compila tutti i passi di programma del percorso
     //-------------------------------------------------------------------------------
     }while((source!=falso_obbiettivo) && (ptr_path<MAXPERCORSI));
  }
  return 0;
}

// ---------------------------------------------------
//      compila_passo_prg()
// ---------------------------------------------------
// Legge i dati contenuti in un nodo e compila il programma di uno step
//
// --------------------------------------------------------------------
//  CARICO DA MAGAZZINO:
//- La navetta si posiziona su magazzino (es: p.8), quindi si ha la seguente
//  sequenza di comandi:
//  a) Azione + Ind/Av ==( dipende dal campo "avvicinamento" relativo al nodo
//         magazzino.
//  b) Av.Mag.S. == permette all'AGV di abbandonare l'ingresso magazzino alla
//     ricerca del primo cestone, in direzione specificata al punto a).
//  c) Carico == Esegue la manovra di carico cestone.
//
//  SCARICO SU MAGAZZINO:
//- Come per il carico da magazzino, cambia solo il punto b) dove
//  Av.Mag.S. viene sostituito da Av.Mag.C.
//
//  CARICO DA MACCHINA:
//- La navetta si posiziona sulla macchina (es: p.5), quindi si ha la
//  seguente sequenza di comandi:
//  a) Azione + Ind/Av ==( dipende dal campo "avvicinamento" relativo al nodo
//     macchina )
//      b) Av.Mac.C. == permette all'AGV di abbandonare il punto macchina alla
//     ricerca del cestone da caricare , in direzione specificata al punto a).
//  c) Carico == Esegue la manovra di carico cestone.
// --------------------------------------------------------------------
//  NB: entra con "ptr_path" aggiornato e attuale
//
int agv::compila_passo_prg(struct NOD *nod, char *all_mess)
/****************************************/
{
  bool ErrorPos;
  bool cambio_dir;
  bool ForcheAlte;
  bool tratto_inclinato;
  bool antioraria;
  bool antioraria_su_start_curva;
  bool inversione_rotazione;
  bool ingresso_uscita_curva_da_retta;
  short int stop=0, stato_rot=0, stato_ral=0;
  short int end=0, proxend =0, cambio_rotazione=0;
  short int curva_laser, deviaz_calamite;
  short int rot_su_start_curva, stato_rot_su_start_curva;
  short int punto0=0;
  short int punto1=0;
  short int punto2=0;
  short int punto3=0;
  short int zona=0;
  long double grd;
  long double impulsi_deviaz;
  bool converti_grd;
  int  id_curva, arco, raggio;
  int  id_curva_tmp, arco_tmp, raggio_tmp;
  double grd_ls, grd_ly;
  double Xv1, Xv2, Yv1, Yv2;
  short int Riflettori;
  short int dir_car_scar;
  int  typ_curva;
  int  x1, x2, y1, y2;
  int  CurvaNonPossibile;
  TPoint points[5];
  //------------------------------------
  // Variabil.h -> Disattivazione curve
  //------------------------------------
  bool NoGestioneCurve=false;
  #ifndef GESTIONE_CURVE
     NoGestioneCurve=true;
  #endif;

  //-----------------------------------------------------------
  // FORCHE ALTE
  // Il bit di "forche alte" non ha più il significato di
  // piano sovrapposto inquanto esiste un DM apposito per
  // questo scopo.
  // Viene utilizzato per gestire l'info di carico con
  // FORCHE FUORI INGOMBRO DA MACCHINE E BAIE
  //-----------------------------------------------------------
  ForcheAlte=false;
  if(mission.TypPresa==N_FORCA_A) ForcheAlte=true;
  //-----------------------------------------------------------
  // Recupero punto considera e successivo
  //-----------------------------------------------------------
  cambio_dir = false;
  if(ptr_path>0){
     punto0 = mission.punto[ptr_path-1];
     //-------------------------------------------
     // Verifico cambio direzione tra precedente
     // e nodo considerato
     //-------------------------------------------
     if(mission.dati_perc[ptr_path-1].dir!=mission.dati_perc[ptr_path].dir) cambio_dir=true;
  }
  punto1 = mission.punto[ptr_path];
  punto2 = mission.punto[ptr_path+1];
  punto3 = mission.punto[ptr_path+2];
  //-----------------------------------------------------------
  // Verifico se il TGV si trova a 0°, 90, 180 o 270
  // IN QUESTO CASO NON SONO SU UN TRATTO INCLINATO
  //-----------------------------------------------------------
  x1 = Punti[ punto1 ][0];
  y1 = Punti[ punto1 ][1];
  x2 = Punti[ punto2 ][0];
  y2 = Punti[ punto2 ][1];
  tratto_inclinato=true;
  if(punto2<1       ) tratto_inclinato=false;
  if(punto2>TOTPUNTI) tratto_inclinato=false;
  if(tratto_inclinato==true){
     if(abs(x1-x2)<5) tratto_inclinato=false;
     if(abs(y1-y2)<5) tratto_inclinato=false;
  }
  //-----------------------------------------------------------
  // verifica se step = fine missione o fine intermedia
  //-----------------------------------------------------------
  if(mission.NoMissioniAlVolo) if(mission.pintend==mission.punto[ptr_path]) stop=1;
  if(mission.pend==mission.punto[ptr_path] && mission.punto[ptr_path+1]==0) end = 1;
  if((mission.pintend == mission.punto[ptr_path+1]) && (mission.pend != mission.punto[ptr_path+1]) ) proxend = 1;
  if(!mission.NoMissioniAlVolo) proxend=0;
  //-----------------------------------------------------------
  // 21/02/96 Modifica per anticipo rotazione miss. intermedie
  //-----------------------------------------------------------
  //if(!stop){
  curva_laser=0;
  impulsi_deviaz=0;
  deviaz_calamite=0;
  stato_rot=0;
  stato_ral=0;
  ErrorPos=false;
  antioraria=false;
  cambio_rotazione=0;
  rot_su_start_curva=0;
  stato_rot_su_start_curva=0;
  antioraria_su_start_curva=false;
  //-----------------------------------------------------------
  // Scomposizione dell'informazione di rotazione per risalire
  // al tipo di curva/retta
  //
  // ROTAZIONE
  //  0... 3 - RETTILINEI
  //  4...19 - ARCHI DI CIRCONFERENZA
  // 20...23 - CURVE DI BEZIER QUADRATICHE
  // 30...33 - CURVE DI BEZIER CUBICHE
  // ....... - free
  // 50...70 - DEVIAZIONI RETTILINEE
  //-----------------------------------------------------------
  if(!end){
     switch(dati_path.rot){
        case  0: stato_rot = 0;
                 break;
        case  1: stato_rot = 1;
                 break;
        case  2: stato_rot = 2;
                 break;
        case  3: stato_rot = 3;
                 break;
        case  4: if( dati_path.av){curva_laser=2; stato_rot=3;}
                 if(!dati_path.av){curva_laser=2; stato_rot=1;}
                 break;
        case  5: if( dati_path.av){curva_laser=1; stato_rot=2;}
                 if(!dati_path.av){curva_laser=1; stato_rot=0;}
                 break;
        case  6: if( dati_path.av){curva_laser=2; stato_rot=0;}
                 if(!dati_path.av){curva_laser=2; stato_rot=2;}
                 break;
        case  7: if( dati_path.av){curva_laser=1; stato_rot=3;}
                 if(!dati_path.av){curva_laser=1; stato_rot=1;}
                 break;
        case  8: if( dati_path.av){curva_laser=2; stato_rot=2;}
                 if(!dati_path.av){curva_laser=2; stato_rot=0;}
                 break;
        case  9: if( dati_path.av){curva_laser=1; stato_rot=1;}
                 if(!dati_path.av){curva_laser=1; stato_rot=3;}
                 break;
        case 10: if( dati_path.av){curva_laser=2; stato_rot=1;}
                 if(!dati_path.av){curva_laser=2; stato_rot=3;}
                 break;
        case 11: if( dati_path.av){curva_laser=1; stato_rot=0;}
                 if(!dati_path.av){curva_laser=1; stato_rot=2;}
                 break;
        case 12: if( dati_path.av){curva_laser=2; stato_rot=2;}
                 if(!dati_path.av){curva_laser=2; stato_rot=0;}
                 break;
        case 13: if( dati_path.av){curva_laser=1; stato_rot=3;}
                 if(!dati_path.av){curva_laser=1; stato_rot=1;}
                 break;
        case 14: if( dati_path.av){curva_laser=2; stato_rot=3;}
                 if(!dati_path.av){curva_laser=2; stato_rot=1;}
                 break;
        case 15: if( dati_path.av){curva_laser=1; stato_rot=0;}
                 if(!dati_path.av){curva_laser=1; stato_rot=2;}
                 break;
        case 16: if( dati_path.av){curva_laser=2; stato_rot=1;}
                 if(!dati_path.av){curva_laser=2; stato_rot=3;}
                 break;
        case 17: if( dati_path.av){curva_laser=1; stato_rot=2;}
                 if(!dati_path.av){curva_laser=1; stato_rot=0;}
                 break;
        case 18: if( dati_path.av){curva_laser=2; stato_rot=0;}
                 if(!dati_path.av){curva_laser=2; stato_rot=2;}
                 break;
        case 19: if( dati_path.av){curva_laser=1; stato_rot=1;}
                 if(!dati_path.av){curva_laser=1; stato_rot=3;}
                 break;
        case 20: stato_rot   = 0;
                 curva_laser = 3;
                 break;
        case 21: stato_rot   = 1;
                 curva_laser = 3;
                 break;
        case 22: stato_rot   = 2;
                 curva_laser = 3;
                 break;
        case 23: stato_rot   = 3;
                 curva_laser = 3;
                 break;
        case 30: stato_rot   = 0;
                 curva_laser = 4;
                 break;
        case 31: stato_rot   = 1;
                 curva_laser = 4;
                 break;
        case 32: stato_rot   = 2;
                 curva_laser = 4;
                 break;
        case 33: stato_rot   = 3;
                 curva_laser = 4;
                 break;
        case 50: stato_rot=0;
                 if(old_rot==1 && stato_rot==0) deviaz_calamite=1;
                 if(old_rot==2 && stato_rot==0) deviaz_calamite=2;  //180° scegliamo le curve anti-orarie
                 if(old_rot==3 && stato_rot==0) deviaz_calamite=2;
                 break;
        case 51: stato_rot=1;
                 if(old_rot==0 && stato_rot==1) deviaz_calamite=2;
                 if(old_rot==2 && stato_rot==1) deviaz_calamite=1;
                 if(old_rot==3 && stato_rot==1) deviaz_calamite=2;  //180° scegliamo le curve anti-orarie
                 break;
        case 52: stato_rot=2;
                 if(old_rot==0 && stato_rot==2) deviaz_calamite=2;  //180° scegliamo le curve anti-orarie
                 if(old_rot==1 && stato_rot==2) deviaz_calamite=2;
                 if(old_rot==3 && stato_rot==2) deviaz_calamite=1;
                 break;
        case 53: stato_rot=3;
                 if(old_rot==0 && stato_rot==3) deviaz_calamite=1;
                 if(old_rot==1 && stato_rot==3) deviaz_calamite=2;  //180° scegliamo le curve anti-orarie
                 if(old_rot==2 && stato_rot==3) deviaz_calamite=2;
                 break;
        case 54: stato_rot=0;
                 if(old_rot==1 && stato_rot==0) deviaz_calamite=1;
                 if(old_rot==2 && stato_rot==0) deviaz_calamite=2;  //180° scegliamo le curve anti-orarie
                 if(old_rot==3 && stato_rot==0) deviaz_calamite=2;
                 break;
        case 55: stato_rot=1;
                 if(old_rot==0 && stato_rot==1) deviaz_calamite=2;
                 if(old_rot==2 && stato_rot==1) deviaz_calamite=1;
                 if(old_rot==3 && stato_rot==1) deviaz_calamite=2;  //180° scegliamo le curve anti-orarie
                 break;
        case 56: stato_rot=2;
                 if(old_rot==0 && stato_rot==2) deviaz_calamite=2;  //180° scegliamo le curve anti-orarie
                 if(old_rot==1 && stato_rot==2) deviaz_calamite=2;
                 if(old_rot==3 && stato_rot==2) deviaz_calamite=1;
                 break;
        case 57: stato_rot=3;
                 if(old_rot==0 && stato_rot==3) deviaz_calamite=1;
                 if(old_rot==1 && stato_rot==3) deviaz_calamite=2;  //180° scegliamo le curve anti-orarie
                 if(old_rot==2 && stato_rot==3) deviaz_calamite=2;
                 break;
        case 60: stato_rot=0;
                 deviaz_calamite=2;  // tratti deviati senza variazione di rotazione (antiorario)
                 break;
        case 61: stato_rot=1;
                 deviaz_calamite=2;  // tratti deviati senza variazione di rotazione (antiorario)
                 break;
        case 62: stato_rot=2;
                 deviaz_calamite=2;  // tratti deviati senza variazione di rotazione (antiorario)
                 break;
        case 63: stato_rot=3;
                 deviaz_calamite=2;  // tratti deviati senza variazione di rotazione (antiorario)
                 break;
        case 64: stato_rot=0;
                 deviaz_calamite=1;  // tratti deviati senza variazione di rotazione (orario)
                 break;
        case 65: stato_rot=1;
                 deviaz_calamite=1;  // tratti deviati senza variazione di rotazione (orario)
                 break;
        case 66: stato_rot=2;
                 deviaz_calamite=1;  // tratti deviati senza variazione di rotazione (orario)
                 break;
        case 67: stato_rot=3;
                 deviaz_calamite=1;  // tratti deviati senza variazione di rotazione (orario)
                 break;
        default: break;
     }
     //----------------------------------------------------
     // MEMORIZZO LO STATO DI RALLENTAMENTO SUL NODO
     // e se si tratta di un INVERSIONE ROTAZIONE
     //----------------------------------------------------
     stato_ral = dati_path.ral;
     inversione_rotazione=false;
     if(ptr_path>0 && mission.dati_perc[ptr_path-1].ral>9){
        inversione_rotazione = true; // inversione deviazione su prossimo nodo
     }
     if(stato_ral>9) stato_ral = stato_ral-10;    // rotazione su nodo considerato
     //-------------------------------------------------------------
     // INVERSIONE 180° SU START MISSIONE
     // Se sono sul punto di start, ed è prevista una rotazione a
     // 180° setto l'inversione
     //-------------------------------------------------------------
     if(ptr_path==0 && punto1>0 && punto1<=TOTPUNTI){
        if(N.n[punto1]->inverti_180==true){
           if(old_rot==0 && stato_rot==2) inversione_rotazione=true;
           if(old_rot==2 && stato_rot==0) inversione_rotazione=true;
           if(old_rot==1 && stato_rot==3) inversione_rotazione=true;
           if(old_rot==3 && stato_rot==1) inversione_rotazione=true;
        }
     }
     //-------------------------------------------------------------
     // 15/11/2013 CURVA SUL PRIMO NODO
     // Verifico se i gradi del TGV coincido (+/- un offset di 5°)
     // con lo stato di rotazione previsto in ingresso curva,
     // altrimenti viene inserita una rotazione calcolando di
     // quanti gradi compierla
     //-------------------------------------------------------------
     if(curva_laser!=0 || (ptr_path==0 && mission.GestioneLaser==true && NoGestioneCurve==false)){
        rot_su_start_curva = (short int)(N.CalcolaGrdRotazioneSuStartTGV(agv_num, ptr_path, &stato_rot_su_start_curva, &antioraria_su_start_curva));
     }
     //-----------------------------------------------------------
     // CURVE AD ARCO DI CIRCONFERENZA
     // Verifico se il TGV arriva ed esce su una retta e recupero
     //-----------------------------------------------------------
     ingresso_uscita_curva_da_retta=false;
     if(mission.GestioneLaser==true && (curva_laser==1 || curva_laser==2)){
        ingresso_uscita_curva_da_retta=true;
        if(punto0==0 || punto1==0 || punto2==0 || punto3==0) ingresso_uscita_curva_da_retta=false;
        //----------------------------------------------------------------
        // Verifico se il punto considerato e il successivo appartengono
        // a una curva e ne memorizzo raggio e arco (da DWG)
        //----------------------------------------------------------------
        id_curva=arco=raggio=0;
        if(N.CercaCurva(TRUE, punto1, punto2, &raggio, &arco, &id_curva)) ingresso_uscita_curva_da_retta=false;
        //----------------------------------------------------------------
        // Verifico se TGV proviene da una retta
        //----------------------------------------------------------------
        if(ingresso_uscita_curva_da_retta==true){
           id_curva_tmp=arco_tmp=raggio_tmp=0;
           if(!N.CercaCurva(TRUE, punto0, punto1, &raggio_tmp, &arco_tmp, &id_curva_tmp)) ingresso_uscita_curva_da_retta=false;
        }
        //----------------------------------------------------------------
        // Verifico se TGV esce su una retta
        //----------------------------------------------------------------
        if(ingresso_uscita_curva_da_retta==true){
           id_curva_tmp=arco_tmp=raggio_tmp=0;
           if(!N.CercaCurva(TRUE, punto2, punto3, &raggio_tmp, &arco_tmp, &id_curva_tmp)) ingresso_uscita_curva_da_retta=false;
        }
        //----------------------------------------------------------------
        // Se il TGV entra e esce da una retta eseguo un ricalcolo
        // di raggio e arco, nel caso l'operatore abbia spostato
        // graficamente i punti.
        //
        // NOTA: La funzione da esisto positivo solo se le rette di ing e
        //       uscita sono tangenti all'arco, altrimenti ritorna i
        //       valori passati dal DWG
        //----------------------------------------------------------------
        if(ingresso_uscita_curva_da_retta==true){
           N.CalcoloRaggioArcoTangente(punto0, punto1, punto2, punto3, &raggio, &arco);
        }
     }
     //-----------------------------------------------------------
     // CURVE DI BEZIER
     // Recupero le coordinate dei vertici di controllo
     //-----------------------------------------------------------
     if(mission.GestioneLaser==true && (curva_laser==3 || curva_laser==4)){
        if(curva_laser==3) typ_curva=2;  // QUADRATICA
        if(curva_laser==4) typ_curva=3;  // CUBICA
        if(N.CercaCurvaBezier(FALSE, punto1, punto2, &typ_curva, &points[0])) typ_curva=0;
        //------------------------------------------------
        // Se ho trovato la curva, riporto le coordinate
        // da riferimento LAYOUT a LASER
        //------------------------------------------------
        Xv1=Xv2=Yv1=Yv2=0;
        if(typ_curva!=0){
           // QUADRATICA (singolo vertice)
           convertiXY_layout_to_laser(&Xv1, &Yv1, (double)(points[1].x), (double)(points[1].y));
           // CUBICA (doppio vertice)
           if(typ_curva==3) convertiXY_layout_to_laser(&Xv2, &Yv2, (double)(points[2].x), (double)(points[2].y));
        }
     }
     //----------------------------------------------------
     // TRATTI INCLINATI
     // vefico sul nodo prec a quello considerato
     // se è stata attivata la procedura di deviazione
     // oraria o antioraria.
     //----------------------------------------------------
     if(curva_laser==0){
        if(deviaz_calamite!=0){
           antioraria=false;
           if(deviaz_calamite==2) antioraria=true;
           //----------------------------------------------------
           // Calcolo degli imp/grd per DEVIAZIONE
           //----------------------------------------------------
           impulsi_deviaz=grd=0;
           if(calcolo_grd_calamite(ptr_path, antioraria, &grd, &impulsi_deviaz)){
              sprintf( all_mess, "deviazione sconosciuta %d -> %d", mission.punto[ptr_path-1], mission.punto[ptr_path] );
              return 1;
           }
           //----------------------------------------------------
           // tratti inclinati sotto i 5° sono da ritenersi
           // rettilinei
           //----------------------------------------------------
           if(impulsi_deviaz<=GrdMinPerAttivaDeviazioni) deviaz_calamite=0;
        }
        //-----------------------------------------------------------
        // AGGIUSTAMENTO GRADI PER DEVIAZIONI INVERSE O ROT STANDARD
        //-----------------------------------------------------------
        if(deviaz_calamite!=0){
           if( inversione_rotazione==true ){
              //--------------------------------------------
              // Inverisione deviazioni:
              // gradi = 360 - gradi calcolati
              //--------------------------------------------
              impulsi_deviaz              = (abs)(360 - impulsi_deviaz);
              N.nodo_grd[agv_num][punto1] = impulsi_deviaz;
              //--------------------------------------------
              // Inversione bit di ORARIA/ANTIORARIA
              //--------------------------------------------
              if(deviaz_calamite==2) antioraria=false;
              if(deviaz_calamite==1) antioraria=true;
           }
        }
        //-----------------------------------------------------------
        // ROTAZIONE NORMALE
        // Scelgo se fare le rotazioni orarie o anti-orarie e gli
        // imp/gradi sono dati dallo stato di rotazione finale.
        //-----------------------------------------------------------
        else{
           impulsi_deviaz=0;
           if(old_rot==1 && stato_rot==0){antioraria=false; impulsi_deviaz=90;}
           if(old_rot==2 && stato_rot==0){antioraria=true;  impulsi_deviaz=180;}  //180° scegliamo le rotazioni anti-orarie
           if(old_rot==3 && stato_rot==0){antioraria=true;  impulsi_deviaz=90;}
           if(old_rot==0 && stato_rot==1){antioraria=true;  impulsi_deviaz=90;}
           if(old_rot==2 && stato_rot==1){antioraria=false; impulsi_deviaz=90;}
           if(old_rot==3 && stato_rot==1){antioraria=true;  impulsi_deviaz=180;}  //180° scegliamo le rotazioni anti-orarie
           if(old_rot==0 && stato_rot==2){antioraria=true;  impulsi_deviaz=180;}  //180° scegliamo le rotazioni anti-orarie
           if(old_rot==1 && stato_rot==2){antioraria=true;  impulsi_deviaz=90;}
           if(old_rot==3 && stato_rot==2){antioraria=false; impulsi_deviaz=90;}
           if(old_rot==0 && stato_rot==3){antioraria=false; impulsi_deviaz=90;}
           if(old_rot==1 && stato_rot==3){antioraria=true;  impulsi_deviaz=180;}  //180° scegliamo le rotazioni anti-orarie
           if(old_rot==2 && stato_rot==3){antioraria=true;  impulsi_deviaz=90;}
           //--------------------------------------------
           // ROTAZIONE CONTRARIA
           // Se sul nodo impostato RAL=10+ral significa
           // che si desidera inverire la rotazione
           // ORARIA     --> ANTIORARIA
           // ANTIORARIA --> ORARIA
           //--------------------------------------------
           if( inversione_rotazione==true ){
              if(old_rot==1 && stato_rot==0){antioraria=true; impulsi_deviaz=270;}
              if(old_rot==2 && stato_rot==0){antioraria=false;impulsi_deviaz=180;}  //180° scegliamo le rotazioni orarie
              if(old_rot==3 && stato_rot==0){antioraria=false;impulsi_deviaz=270;}
              if(old_rot==0 && stato_rot==1){antioraria=false;impulsi_deviaz=270;}
              if(old_rot==2 && stato_rot==1){antioraria=true; impulsi_deviaz=270;}
              if(old_rot==3 && stato_rot==1){antioraria=false;impulsi_deviaz=180;}  //180° scegliamo le rotazioni orarie
              if(old_rot==0 && stato_rot==2){antioraria=false;impulsi_deviaz=180;}  //180° scegliamo le rotazioni orarie
              if(old_rot==1 && stato_rot==2){antioraria=false;impulsi_deviaz=270;}
              if(old_rot==3 && stato_rot==2){antioraria=true; impulsi_deviaz=270;}
              if(old_rot==0 && stato_rot==3){antioraria=true; impulsi_deviaz=270;}
              if(old_rot==1 && stato_rot==3){antioraria=false;impulsi_deviaz=180;}  //180° scegliamo le rotazioni orarie
              if(old_rot==2 && stato_rot==3){antioraria=false;impulsi_deviaz=270;}
           }
        }
     }
     //-----------------------------------------------------------
     // 17/02/97 Controllo del corretto orientamento dell'AGV nei
     //          punti notevoli  per evitare rotazioni indesiderate
     //          sotto macchine, magazzini ecc.. ->SKIP
     //-----------------------------------------------------------
     if(ptr_path==0){
        //--------------------------------------
        // PERCORSO RETTO
        //--------------------------------------
        if(curva_laser==0){
           if(old_rot!=stato_rot && N.punti_notevoli[stato.pos]==true){
              ErrorPos=true;
           }
        }
        //--------------------------------------
        // PERCORSO CURVA
        //--------------------------------------
        if(curva_laser!=0){
           if(((old_rot==0 && stato_rot==2) ||
               (old_rot==1 && stato_rot==3) ||
               (old_rot==2 && stato_rot==0) ||
               (old_rot==3 && stato_rot==1)) && N.punti_notevoli[ stato.pos ]==true){
               ErrorPos=true;
           }
        }
        //-------------------------------------
        // Rotazione di partenza non corretta!
        //-------------------------------------
        if(ErrorPos){
           allarme_interno=ALL_INT_POS_NOT_OK;
           //---------------------------------------------
           // AL-26/05/2017 reset missione perchè il TGV
           // non si muove ma tiene bloccata chiamata e
           // percorso
           //---------------------------------------------
           reset_chiamata_AGV();
           free_all_nodi(all_mess);
           reset_ram_mission();
           //---------------------------------------------
           // Messaggio su errori di sistema
           //---------------------------------------------
           strcpy( all_mess, MESS[219] );
           return 1;
        }
     }
     //--------------------------
     // NO CURVE
     // Rotazione:
     // rot==0 ->   0°
     // rot==1 ->  90°
     // rot==2 -> 180°
     // rot==3 -> 270°
     // o DEVIAZIONI
     //--------------------------
     if((old_rot!=stato_rot && curva_laser==0) || (deviaz_calamite!=0) || (rot_su_start_curva!=0 && old_rot!=stato_rot_su_start_curva)){
        cbit = (struct comandi *) &mission.step[ptr_step];
        cbit->az     = 1;
        cbit->grd0   = 0;
        cbit->grd90  = 0;
        cbit->grd180 = 0;
        cbit->grd270 = 0;
        //---------------------------------------------------------
        // Gestione riservata ai TGV SENZA guida laser
        //---------------------------------------------------------
        if(!mission.GestioneLaser){
           //--------------------------------------
           // GESTIONE CURVA A CALAMITE
           // Solo senza deviazioni e inversione
           // rotazioni
           //--------------------------------------
           if(stato_rot>3 || inversione_rotazione==true || deviaz_calamite!=0) CurvaNonPossibile=1;
           //--------------------------------------
           // CAMBI DIREZIONE E 180°
           // Con un cambio direzione non sono
           // ammesse curvette FONTANA
           //--------------------------------------
           if(cambio_dir==true || stato_rot!=old_rot){
              if(cambio_dir==true           ) CurvaNonPossibile=1;  // cambio direzione
              if(stato_rot==0 && old_rot==2 ) CurvaNonPossibile=1;  // 180°
              if(stato_rot==1 && old_rot==3 ) CurvaNonPossibile=1;  // 180°
              if(stato_rot==2 && old_rot==0 ) CurvaNonPossibile=1;  // 180°
              if(stato_rot==3 && old_rot==1 ) CurvaNonPossibile=1;  // 180°
           }
           //--------------------------------------
           // CURVETTA FONTATA POSSIBILE
           // alzo il bit9 nel comando di rotazione
           //--------------------------------------
           if( CurvaNonPossibile==0){
              cbit->forchealte=1; //(1) Gestione Curve AGV Calamita
           }
           //--------------------------------------
           // Gestione rotazioni riferimento LAYOUT
           //--------------------------------------
           switch(stato_rot){
              case  0: cbit->grd0   =1; break;
              case  1: cbit->grd90  =1; break;
              case  2: cbit->grd180 =1; break;
              case  3: cbit->grd270 =1; break;
              default: break;
           }
        }
        //---------------------------------------------------------
        // Gestione riservata ai TGV CON guida laser
        //---------------------------------------------------------
        else{
           grd_ly=grd_ls=0;
           converti_grd=false;
           //--------------------------------------
           // Gestione rotazioni riferimento LASER
           //--------------------------------------
           switch(stato_rot){
              case  0: grd_ly=0;   converti_grd=true; break;
              case  1: grd_ly=90;  converti_grd=true; break;
              case  2: grd_ly=180; converti_grd=true; break;
              case  3: grd_ly=270; converti_grd=true; break;
              default: break;
           }
           if(converti_grd){
              convertiGRD_layout_to_laser(&grd_ls, grd_ly);
              //-------------------------------------
              // Conversione delle rotazioni
              //-------------------------------------
              if(grd_ls==  0) cbit->grd0=1;
              if(grd_ls== 90) cbit->grd90=1;
              if(grd_ls==180) cbit->grd180=1;
              if(grd_ls==270) cbit->grd270=1;
           }
        }
        //--------------------------------------------
        // VERSO ROTAZIONI/DEVIAZIONI
        //--------------------------------------------
        cbit->antiora = (unsigned)(antioraria);
        //--------------------------------------------
        // CURVA SU START TGV
        // recupera il bit di orarai/antioraria da
        // apposita funzione
        //--------------------------------------------
        if(rot_su_start_curva!=0 && inversione_rotazione==false){
           cbit->antiora  = (unsigned )(antioraria_su_start_curva);
           impulsi_deviaz = (short int)(rot_su_start_curva);
           cbit->grd0   = 0;
           cbit->grd90  = 0;
           cbit->grd180 = 0;
           cbit->grd270 = 0;
           switch(stato_rot_su_start_curva){
              case  0: cbit->grd0   =1; break;
              case  1: cbit->grd90  =1; break;
              case  2: cbit->grd180 =1; break;
              case  3: cbit->grd270 =1; break;
              default: break;
           }
        }
        //--------------------------------------------
        // DEVIAZIONI MODALITA' ANNOVI
        // Tutte le rotazioni sono gestite come delle
        // deviazioni quindi dopo il comando di
        // rotazione vengono trasmessi i GRADI
        //--------------------------------------------
        if(GrdMinPerAttivaDeviazioni<360){
           // MAGNETI + LASER
           ptr_step++; // incremento
           cbit = (struct comandi *) &mission.step[ptr_step];
           mission.step[ptr_step] = (short int)((abs)(impulsi_deviaz));
           cbit->ral = 1;
        }
        //--------------------------------------------
        // Memorizzo il cambio di rotazione/deviazione
        //--------------------------------------------
        old_rot = stato_rot;
        cambio_rotazione=1;
        ptr_step++;
     }
     //-------------------------------------------------------------------------------
     // 25/02/99
     // In caso di MISSIONI_AL_VOLO elaborare anche se punto intermedio di STOP
     //-------------------------------------------------------------------------------
     if(!mission.NoMissioniAlVolo){
        if(!end || cambio_rotazione){
           //--------------------------
           // Avanti, Indietro, Veloce
           //--------------------------
           cbit = (struct comandi *) &mission.step[ptr_step];
           cbit->az  = 1;
           cbit->av  = dati_path.av;
           cbit->ind = dati_path.ind;

           cbit->vel = (!stato_ral) && (!proxend); // Toglie la veloce in
                                                   // Rallentamento impostato
                                                   // o di missione intermedia
           cbit->grd0   = 0;
           cbit->grd90  = 0;
           cbit->grd180 = 0;
           cbit->grd270 = 0;
           //------------------------------------------
           // Gestione delle CURVE LASER
           // selezione ARCO DI CRF / BEZIER
           //------------------------------------------
           if(mission.GestioneLaser==true){
              switch(curva_laser){
                 case  1: cbit->curva   = 1;   // ARCO DI CRF ORARIO
                          cbit->antiora = 0;
                          break;
                 case  2: cbit->curva   = 1;   // ARCO DI CRF ANTI-ORARIO
                          cbit->antiora = 1;
                          break;
                 case  3: cbit->curva   = 1;   // CURVE DI BEZIER QUADRATICA
                          cbit->antiora = 0;
                          break;
                 case  4: cbit->curva   = 1;   // CURVE DI BEZIER CUBICA
                          cbit->antiora = 1;
                          break;
                 default: break;
              }
           }
           //------------------------------------------
           // GESTIONE MAGNETI
           // Il bit di "curva" abbinato
           // all'azione di direzione
           // "AV" o "IND" indica tratto
           // rettilineo per rifasamento
           // DM97 di gradi TGV.
           //------------------------------------------
           if(mission.GestioneLaser==false && GrdMinPerAttivaDeviazioni<360){
              if(curva_laser==0 && tratto_inclinato==false){
                 cbit->curva=1;
              }
           }
           //------------------------------------------
           // CURVE DI BEZIER  (09/04/2013)
           // Inserimento bit di stato rotazione
           // nel'informazione di curva
           //------------------------------------------
           if(curva_laser==3 || curva_laser==4){
              cbit->grd0   = 0;
              cbit->grd90  = 0;
              cbit->grd180 = 0;
              cbit->grd270 = 0;
              //--------------------------------------
              // Gestione rotazioni riferimento LAYOUT
              //--------------------------------------
              switch(stato_rot){
                 case  0: cbit->grd0   = 1; break;
                 case  1: cbit->grd90  = 1; break;
                 case  2: cbit->grd180 = 1; break;
                 case  3: cbit->grd270 = 1; break;
                 default: break;
              }
           }
           //------------------------------------------
           // Memorizzo ultima rotazione
           //------------------------------------------
           old_rot = stato_rot;
           ptr_step++;     // *** incremento step programma
        }
     }
     else{
        if(!stop || cambio_rotazione){
           //--------------------------
           // Avanti, Indietro, Veloce
           //--------------------------
           cbit = (struct comandi *) &mission.step[ptr_step];
           cbit->az  = 1;
           cbit->av  = dati_path.av;
           cbit->ind = dati_path.ind;

           cbit->vel = (!stato_ral) && (!proxend); // Toglie la veloce in
                                                   // Rallentamento impostato
                                                   // o di missione intermedia
           cbit->grd0   = 0;
           cbit->grd90  = 0;
           cbit->grd180 = 0;
           cbit->grd270 = 0;
           //--------------------------------------
           // Gestione rotazioni riferimento LAYOUT
           //--------------------------------------
           switch(stato_rot){
              case  0: cbit->grd0   = 1; break;
              case  1: cbit->grd90  = 1; break;
              case  2: cbit->grd180 = 1; break;
              case  3: cbit->grd270 = 1; break;
              default: break;
           }
           old_rot = stato_rot;
           ptr_step++;     // *** incremento step programma
        }
     }
  }
  //-------------------------------------------------------------------------------
  // 25/02/99
  // In caso di MISSIONI_AL_VOLO elaborare anche se punto intermedio di STOP
  //-------------------------------------------------------------------------------
  if(!mission.NoMissioniAlVolo){
     if( !end ){
        cbit = (struct comandi *) &mission.step[ptr_step];
        //-----------------------------------------------------------
        // CURVE DI BEZIER
        // Viene sostituita la distanza da percorrere sulla curva
        // con la coordinata X del vertice di controllo
        //-----------------------------------------------------------
        if(curva_laser==3 || curva_laser==4){
           mission.step[ptr_step] = Xv1;
           ptr_step++;     // *** incremento step programma
        }
        else{
           //-----------------------------------------------------------
           // 26/11/96 Valore di distanza dati_path.dist espresso in cm
           //-----------------------------------------------------------
           if(curva_laser==0) mission.step[ptr_step] = dati_path.dist/mission.RapEncoder;
           else               mission.step[ptr_step] = arco/mission.RapEncoder;
           cbit->ral = 1;
           //-----------------------------------------------------------
           // In caso di punto terminale Ral==2 viene informato l'AGV
           // settando contemporaneamente anche il bit di END
           //-----------------------------------------------------------
           if( dati_path.ral==2 ){
              cbit->end =1;
           }
           //-----------------------------------------------------------
           // In caso di Ral==3 viene informato l'AGV che si tratta di
           // una TRACCIA OBLIQUA (viene settato il bit HR12 = avvic).
           //-----------------------------------------------------------
           if( dati_path.ral==3 ){
              cbit->avvic =1;
           }
           ptr_step++;     // *** incremento step programma
        }
     }
  }
  else{
     if(!stop){
        cbit = (struct comandi *) &mission.step[ptr_step];
        //-----------------------------------------------------------
        // CURVE DI BEZIER
        // Viene sostituita la distanza da percorrere sulla curva
        // con la coordinata X del vertice di controllo
        //-----------------------------------------------------------
        if(curva_laser==3 || curva_laser==4){
           mission.step[ptr_step] = Xv1;
           ptr_step++;     // *** incremento step programma
        }
        else{
           //-----------------------------------------------------------
           // 26/11/96 Valore di distanza dati_path.dist espresso in cm
           //-----------------------------------------------------------
           if(curva_laser==0) mission.step[ptr_step] = dati_path.dist/mission.RapEncoder;
           else               mission.step[ptr_step] = arco/mission.RapEncoder;
           cbit->ral = 1;
           //----------------------------------------------
           // In caso di punto terminale Ral==2 viene
           // informato l'AGV settando contemporaneamente
           // anche il bit di END
           //----------------------------------------------
           if( dati_path.ral==2 ){
              cbit->end =1;
           }
           //----------------------------------------------
           // In caso di Ral==3 viene informato l'AGV
           // che si tratta di una TRACCIA OBLIQUA (viene
           // settato il bit HR12 = avvic).
           //----------------------------------------------
           if( dati_path.ral==3 ){
              cbit->avvic =1;
           }
           ptr_step++;     // *** incremento step programma
        }
     }
  }
  //-----------------------------------
  // Punto sucessivo
  //-----------------------------------
  if(!mission.NoMissioniAlVolo){
     if(!end){
        cbit = (struct comandi *) &mission.step[ptr_step];
        mission.step[ptr_step] = dati_path.ps;
        ptr_step++;     // *** incremento step programma
     }
  }
  else{
     if(!stop){
        cbit = (struct comandi *) &mission.step[ptr_step];
        mission.step[ptr_step] = dati_path.ps;
        ptr_step++;     // *** incremento step programma
     }
  }
  //-----------------------------------------------
  // Gestione riservata ai TGV con guida laser
  //-----------------------------------------------
  if(mission.GestioneLaser){
     //-----------------------------------
     // Calcolo della ZONE DI MAPPING
     //-----------------------------------
     if(!mission.NoMissioniAlVolo){
        if( !end ){
           cbit = (struct comandi *) &mission.step[ptr_step];
           //-------------------------------------------
           // Recupero la zona mapping di apparteneza
           // del nodo da raggiungere.
           // e numero riflettori da gestire
           //-------------------------------------------
           calcolo_zona_mapping(dati_path.ps, &zona);
           //-------------------------------------------
           // Scrittura della zona
           //-------------------------------------------
           Riflettori=3;
           if( zona>=0 && zona< MAXZONEMAPPING ){
              if( zone_mapping.Z[zona].Minr<=9 ){
                 Riflettori = (short int) zone_mapping.Z[zona].Minr;
              }
           }
           mission.step[ptr_step] =  (short int) (zona+ (Riflettori<<8));
           cbit->ral = 1;
           cbit->az  = 1;
           ptr_step++;     // *** incremento step programma
           //--------------------------------------------
           // Trasmissione del Raggio della curva
           //--------------------------------------------
           if(curva_laser){
              cbit = (struct comandi *) &mission.step[ptr_step];
              //-----------------------------------------------------------
              // CURVE DI BEZIER
              // Viene sostituita il raggio della curva con la coordinata
              // Y del vertice di controllo
              //-----------------------------------------------------------
              if(curva_laser==3 || curva_laser==4){
                 mission.step[ptr_step] = Yv1;
                 ptr_step++;     // *** incremento step programma
              }
              //-----------------------------------------------------------
              // CURVE ARCHI DI CIRCONFERENZA
              // Trasmissione raggio curva
              //-----------------------------------------------------------
              else{
                 mission.step[ptr_step] = (short int)(raggio);
                 cbit->ral = 1;
                 cbit->az  = 1;
                 cbit->end = 1;
                 ptr_step++;     // *** incremento step programma
              }
           }
        }
     }
  }
  //-----------------------------------
  // Raggiungimento falso obiettivo
  //-----------------------------------
  if(mission.NoMissioniAlVolo){
     if((stop) && (!end)){
        cbit = (struct comandi *) &mission.step[ptr_step];
        cbit->end = 1;
        ptr_step++;     // *** incremento step programma
     }
  }
  //-----------------------------------
  // Fine programma movimentazione
  //-----------------------------------
  if(end){
     dir_car_scar = nod->direz;
     switch(mission.Typ){
        //---------------------------------------
        // ( 1) CARICO DA MACCHINA/BAIA
        //---------------------------------------
        case MISS_CARICO_DA_MACCHINA:
        case MISS_CARICO_DA_BAIA:
             //------ comando 1: azione + direz.
              // 26/10/95 Solo se direzione > 0
              if(dir_car_scar>0){
                 cbit = (struct comandi *) &mission.step[ptr_step];
                 cbit->az  = 1;
                 if( dir_car_scar == 1) cbit->av  =1;
                 if( dir_car_scar == 2) cbit->ind =1;
                 //------ gestione rifasamento gradi per magneti
                 if(mission.GestioneLaser==false && GrdMinPerAttivaDeviazioni<360){
                    if(tratto_inclinato==false) cbit->curva=1;
                 }
                 ptr_step++;     // *** incremento step programma
                 //------ comando 2: avvicin.x carico da macch.
                 cbit = (struct comandi *) &mission.step[ptr_step];
                 cbit->avvic = 1; // avvicinamento x carico
                 cbit->vel   = 1; // discrimina tipo di avv.
                 ptr_step++;     // *** incremento step programma
              }
              //------ comando 3: azione + carico
              cbit = (struct comandi *) &mission.step[ptr_step];
              cbit->az     = 1;
              cbit->carico = 1;
              if( ForcheAlte ) cbit->forchealte = 1;                           // Forse NON serve
              ptr_step++;     // *** incremento step programma
              break;
        //---------------------------------------
        // ( 9) CARICO DA MAGAZZINO
        //---------------------------------------
        case MISS_CARICO_DA_MAGAZZINO:
              //------ comando 1: azione + direz.
              // 26/10/95 Solo se direzione > 0
              if(dir_car_scar>0){
                 cbit = (struct comandi *) &mission.step[ptr_step];
                 cbit->az  = 1;
                 if( dir_car_scar == 1) cbit->av  =1;
                 if( dir_car_scar == 2) cbit->ind =1;
                 //------ gestione rifasamento gradi per magneti
                 if(mission.GestioneLaser==false && GrdMinPerAttivaDeviazioni<360){
                    if(tratto_inclinato==false) cbit->curva=1;
                 }
                 ptr_step++;     // *** incremento step programma
                 //------ comando 2: avvicin.x carico da Magazzino
                 cbit = (struct comandi *) &mission.step[ptr_step];
                 cbit->avvic = 1; // avvicinamento x carico
                 cbit->vel   = 1; // discrimina tipo di avv.
                 ptr_step++;     // *** incremento step programma
              }
              //------ comando 3: azione + carico
              cbit = (struct comandi *) &mission.step[ptr_step];
              cbit->az     = 1;
              cbit->carico = 1;
            //if( ForcheAlte ) cbit->forchealte = 1;                           // Forse NON serve vecchio scopo
              ptr_step++;     // *** incremento step programma
              break;
        //---------------------------------------
        // ( 4) SCARICO SU MACCHINA/BAIA
        //---------------------------------------
        case MISS_SCARICO_SU_MACCHINA:
        case MISS_SCARICO_SU_BAIA:
              //------ comando 1: azione + direz.
              // 26/10/95 Solo se direzione > 0
              if(dir_car_scar>0){
                 //------ comando 1: azione + direz.
                 cbit = (struct comandi *) &mission.step[ptr_step];
                 cbit->az  = 1;
                 if( dir_car_scar == 1) cbit->av  =1;
                 if( dir_car_scar == 2) cbit->ind =1;
                 //------ gestione rifasamento gradi per magneti
                 if(mission.GestioneLaser==false && GrdMinPerAttivaDeviazioni<360){
                    if(tratto_inclinato==false) cbit->curva=1;
                 }
                 ptr_step++;     // *** incremento step programma
                 //------ comando 2: avvicin.x scarico da macch.
                 cbit = (struct comandi *) &mission.step[ptr_step];
                 cbit->avvic   = 1; // avvicinamento x carico
                 cbit->av      = 1; // discrimina tipo di avv.
                 cbit->scarico = 1; // discrimina tipo di avv.
                 ptr_step++;     // *** incremento step programma
              }
              //------ comando 3: azione + carico
              cbit = (struct comandi *) &mission.step[ptr_step];
              cbit->az      = 1;
              cbit->scarico = 1;
            //if( ForcheAlte ) cbit->forchealte = 1;                           // NON serve vecchio scopo
              ptr_step++;     // *** incremento step programma
              break;
        //---------------------------------------
        // (10) SCARICO SU MAGAZZINO
        //---------------------------------------
        case MISS_SCARICO_SU_MAGAZZINO:
              //------ comando 1: azione + direz.
              // 26/10/95 Solo se direzione > 0
              // !!! IMPORTANTE !!!
              // Con i magazzini multi nodo il deposito deve essere sul nodo
              // solo al prelievo è ammesso l'avvicinamento per la ricerca
              // del pallet
              if(dir_car_scar>0 && MagazziniMultiNodo==false){
                 cbit = (struct comandi *) &mission.step[ptr_step];
                 cbit->az  = 1;
                 if( dir_car_scar == 1) cbit->av  =1;
                 if( dir_car_scar == 2) cbit->ind =1;
                 //------ gestione rifasamento gradi per magneti
                 if(mission.GestioneLaser==false && GrdMinPerAttivaDeviazioni<360){
                    if(tratto_inclinato==false) cbit->curva=1;
                 }
                 ptr_step++;     // *** incremento step programma
                 //------ comando 2: avvicin.x carico da Magazzino
                 cbit = (struct comandi *) &mission.step[ptr_step];
                 cbit->avvic = 1; // avvicinamento x carico
                 cbit->av    = 1; // discrimina tipo di avv.
                 ptr_step++;     // *** incremento step programma
              }
              //------ comando 3: azione + carico
              cbit = (struct comandi *) &mission.step[ptr_step];
              cbit->az      = 1;
              cbit->scarico = 1;
            //if( ForcheAlte ) cbit->forchealte = 1;                           // NON serve vecchio scopo
              ptr_step++;     // *** incremento step programma
              break;

     }

     //----------------
     // fine programma
     //----------------
     cbit = (struct comandi *) &mission.step[ptr_step];
     cbit->end = 1;
     ptr_step++;         // *** incremento step programma
  }
  strcpy(all_mess, "OK");
  return 0;
}

// ---------------------------------------------------
//      calcolo_grd_calamite()
// ---------------------------------------------------
// Recupero i gradi interni o esterni tra 2 tracce
//
int agv::calcolo_grd_calamite(short int ptr_path, bool anti_oraria, long double *gradi, long double *impulsi)
/****************************************/
{
  short int j, k;
  long double grd;
  long double radA, grdA;
  long double radB, grdB;
  double CatetoX, CatetoY;
  int x0, y0, x1, y1, x2, y2;
  short int prec, nodo, succ;
  bool ing_tratto_rettoX;
  bool ing_tratto_rettoY;
  bool out_tratto_rettoX;
  bool out_tratto_rettoY;
  bool tgv_ortogonale;
  short int rot_prec, dir_prec;
  short int stato_grd, stato_rot;

  *gradi=0;
  *impulsi=0;
  prec=nodo=succ=0;
  CatetoX=CatetoY=0;
  x0=y0=x1=y1=x2=y2=0;
  ing_tratto_rettoX=false;
  ing_tratto_rettoY=false;
  out_tratto_rettoX=false;
  out_tratto_rettoY=false;
  radB=grdB=radA=grdA=grd=0;
  rot_prec=stato_rot=stato_grd=dir_prec=0;
  //---------------------------------------------------
  // Recupero le coordinate dei punti interessati
  //---------------------------------------------------
  nodo = mission.punto[ptr_path  ];
  succ = mission.punto[ptr_path+1];
  if(ptr_path>0){
     prec = mission.punto[ptr_path-1];
  }
  //---------------------------------------------------
  // DEVIAZIONE SU PRIMO NODO DI PERCORSO
  // In caso di partenza da punti di percorso verifico
  // la rotazione attuale del TGV
  //---------------------------------------------------
  if(ptr_path==0 && N.punti_notevoli[nodo]==0){
/*
     if(stato.s.bit.grd0  ) rot_prec=0;
     if(stato.s.bit.grd90 ) rot_prec=1;
     if(stato.s.bit.grd180) rot_prec=2;
     if(stato.s.bit.grd270) rot_prec=3;
     //-----------------------------------------------
     // Simulo il nodo precedente recuperando tra i
     // circostanti alla posizione TGV, quello che
     // ha come PS, quello dove si trova la navetta
     // con la medesima rotazione di arrivo
     //-----------------------------------------------
     for(j=1; j<MAXPUNTI; j++){
        if(N.n[j]->num<1        ) continue;
        if(N.n[j]->num>TOTPUNTI ) continue;
        for(k=0; k<MAXPUNTINODO; k++){
           if(N.n[j]->rec[k].ps==0         ) break;
           if(N.n[j]->rec[k].ps!=nodo      ) continue;
           if(N.n[j]->rec[k].rot!=rot_prec ) continue;
           prec     = N.n[j]->num;
           break;
        }
        if(prec!=0) break;
     }
*/
     //-----------------------------------------------
     // Recupero lo stato di rotazione dallo stato
     // navetta
     //-----------------------------------------------
     if(stato.s.bit.grd0  ){rot_prec=0; stato_rot=  0;}
     if(stato.s.bit.grd90 ){rot_prec=1; stato_rot= 90;}
     if(stato.s.bit.grd180){rot_prec=2; stato_rot=180;}
     if(stato.s.bit.grd270){rot_prec=3; stato_rot=270;}
     if(stato.s.bit.ind   ) dir_prec=1;
     if(stato.s.bit.av    ) dir_prec=0;
     //-----------------------------------------------
     // Recupero gradi di rotazione
     //-----------------------------------------------
     stato_grd = stato.rot_calamite;
     if(mission.GestioneLaser==true) stato_grd = stato.rot_laser;
     //-----------------------------------------------
     // Partenza TGV molto inclinata rispetto a
     // bit di rotazione
     //
     // AL-17/06/2020 TGV passe 360 anzichè 0°
     //-----------------------------------------------
     tgv_ortogonale=true;
     if((360-stato_grd)<OFFSET_MIN_COORD_TRATTO_INCLINATO ) stato_grd=0;
     if(abs(stato_grd-stato_rot)>GrdMinPerAttivaDeviazioni) tgv_ortogonale=false;
     //-----------------------------------------------
     // Simulo il nodo precedente recuperando tra i
     // circostanti alla posizione TGV, quello che
     // ha come PS, quello dove si trova la navetta
     // con la medesima rotazione di arrivo
     //
     // AL-08/01/2014
     //
     // 1. prima verifico se ci sono dei precedenti
     //    su tratti ortogonali
     //
     // 2. ricerca tra tutti i nodi
     //
     // AL-13/05/2020
     //
     // 1a. se il TGV parte molto inclinato cerco se
     //     ci sono nodi precenti su tratti inclinati
     //
     // 1b. prima verifico se ci sono dei precedenti
     //     su tratti ortogonali
     //
     // 2. ricerca tra tutti i nodi
     //
     //-----------------------------------------------
     // 1a
     if(tgv_ortogonale==false){
        for(j=1; j<MAXPUNTI; j++){
           if(N.n[j]->num<1        ) continue;
           if(N.n[j]->num>TOTPUNTI ) continue;
           for(k=0; k<MAXPUNTINODO; k++){
              if(N.n[j]->rec[k].ps==0                    ) break;
              if(N.n[j]->rec[k].ps!=nodo                 ) continue;
              if(N.n[j]->rec[k].rot!=rot_prec            ) continue;
              if(dir_prec==1 && N.n[j]->rec[k].ind==false) continue;
              if(dir_prec==0 && N.n[j]->rec[k].ind==true ) continue;
              //----------------------------------------
              // Ricerca preferenziale di precedenti su
              // tratti ortogonali
              //----------------------------------------
              x0 = Punti[N.n[j]->num][0];
              y0 = Punti[N.n[j]->num][1];
              x1 = Punti[nodo][0];
              y1 = Punti[nodo][1];
              if(abs(x0-x1)<=5 || abs(y0-y1)<=5) continue;
              prec = N.n[j]->num;
              break;
           }
           if(prec!=0) break;
        }
     }
     // 1b
     for(j=1; j<MAXPUNTI; j++){
        if(prec!=0              ) break;    // nodo già trovato
        if(N.n[j]->num<1        ) continue;
        if(N.n[j]->num>TOTPUNTI ) continue;
        for(k=0; k<MAXPUNTINODO; k++){
           if(N.n[j]->rec[k].ps==0                    ) break;
           if(N.n[j]->rec[k].ps!=nodo                 ) continue;
           if(N.n[j]->rec[k].rot!=rot_prec            ) continue;
           if(dir_prec==1 && N.n[j]->rec[k].ind==false) continue;
           if(dir_prec==0 && N.n[j]->rec[k].ind==true ) continue;
           //----------------------------------------
           // Ricerca preferenziale di precedenti su
           // tratti ortogonali
           //----------------------------------------
           x0 = Punti[N.n[j]->num][0];
           y0 = Punti[N.n[j]->num][1];
           x1 = Punti[nodo][0];
           y1 = Punti[nodo][1];
           if(abs(x0-x1)>5 && abs(y0-y1)>5) continue;
           prec = N.n[j]->num;
           break;
        }
        if(prec!=0) break;
     }
     // 2.
     for(j=1; j<MAXPUNTI; j++){
        if(prec!=0              ) break;    // nodo già trovato
        if(N.n[j]->num<1        ) continue;
        if(N.n[j]->num>TOTPUNTI ) continue;
        for(k=0; k<MAXPUNTINODO; k++){
           if(N.n[j]->rec[k].ps==0         ) break;
           if(N.n[j]->rec[k].ps!=nodo      ) continue;
           if(N.n[j]->rec[k].rot!=rot_prec ) continue;
           prec = N.n[j]->num;
           break;
        }
        if(prec!=0) break;
     }
     if(prec==0) return 0;
  }
  //---------------------------------------------------
  // Recupero coordinate punti
  //---------------------------------------------------
  x0 = Punti[ prec ][0];
  y0 = Punti[ prec ][1];
  x1 = Punti[ nodo ][0];
  y1 = Punti[ nodo ][1];
  x2 = Punti[ succ ][0];
  y2 = Punti[ succ ][1];
  //---------------------------------------------------
  // Calocolo le rette interessate dalla deviazione
  //---------------------------------------------------
  CatetoX = (double)(x1-x0);
  CatetoY = (double)(y1-y0);
  if(CatetoX==0) out_tratto_rettoY=true;
  if(CatetoY==0) out_tratto_rettoX=true;
  if(CatetoX && CatetoY){
     radA = (long double)(atanl( CatetoY / CatetoX  ));
     grdA = RadToDeg(radA);
  }
  CatetoX = (double)(x2-x1);
  CatetoY = (double)(y2-y1);
  if(CatetoX==0) ing_tratto_rettoY=true;
  if(CatetoY==0) ing_tratto_rettoX=true;
  if(CatetoX && CatetoY){
     radB = (long double)(atanl( CatetoY / CatetoX  ));
     grdB = RadToDeg(radB);
  }
  //-----------------------------------------------------
  // Recupero i gradi in base al tipo di rotazione
  //-----------------------------------------------------
  if(out_tratto_rettoX && grdB>0){
     if(anti_oraria==false) grd =   0 + grdB;
     if(anti_oraria==true ) grd = 180 - grdB;
  }
  if(out_tratto_rettoY && grdB>0){
     if(anti_oraria==false) grd =  90 + grdB;
     if(anti_oraria==true ) grd =  90 - grdB;
  }
  if(out_tratto_rettoX && grdB<0){
     if(anti_oraria==false) grd = 180 + grdB;
     if(anti_oraria==true ) grd =   0 - grdB;
  }
  if(out_tratto_rettoY && grdB<0){
     if(anti_oraria==false) grd =  90 + grdB;
     if(anti_oraria==true ) grd =  90 - grdB;
  }

  if(ing_tratto_rettoX && grdA>0){
     if(anti_oraria==false) grd = 180 - grdA;
     if(anti_oraria==true ) grd =   0 + grdA;
  }
  if(ing_tratto_rettoY && grdA>0){
     if(anti_oraria==false) grd =  90 - grdA;
     if(anti_oraria==true ) grd =  90 + grdA;
  }
  if(ing_tratto_rettoX && grdA<0){
     if(anti_oraria==false) grd =   0 - grdA;
     if(anti_oraria==true ) grd = 180 + grdA;
  }
  if(ing_tratto_rettoY && grdA<0){
     if(anti_oraria==false) grd =  90 - grdA;
     if(anti_oraria==true ) grd =  90 + grdA;
  }

  if(!ing_tratto_rettoX && !ing_tratto_rettoX && !out_tratto_rettoX && !out_tratto_rettoY){
     if(grdB>0 && grdA>0){
        if(anti_oraria==false) grd = (90-grdA) - (90-grdB);
        if(anti_oraria==true ) grd = ( 0+grdA) - ( 0+grdB);
     }
     if(grdB>0 && grdA<0){
        if(anti_oraria==false) grd = ( 0-grdA) + ( 0+grdB);
        if(anti_oraria==true ) grd = (90+grdA) + (90-grdB);
     }
     if(grdB<0 && grdA>0){
        if(anti_oraria==false) grd = (90-grdA) + (90+grdB);
        if(anti_oraria==true ) grd = ( 0+grdA) + ( 0-grdB);
     }
     if(grdB<0 && grdA<0){
        if(anti_oraria==false) grd = ( 0-grdA) - ( 0-grdB);
        if(anti_oraria==true ) grd = (90+grdA) - (90+grdB);
     }
     //------------------------------------------
     // ???????? però FUNZIONA!!!
     //
     // Aggiustamento per avere i gradi positivi
     //------------------------------------------
     if(grd<0) grd=180+grd;
  }

  //-----------------------------------------------------
  // calcolo dei gradi su rotazioni orarie o anti-orarie
  //-----------------------------------------------------
  *gradi   = grd;
//*impulsi = (long double)(grd)*(long double)(mission.RapEncDev);
  //-----------------------------------------------------
  // Aggiorno struttura grafica di visualizzazione gradi
  // trasmessi al TGV
  //-----------------------------------------------------
  N.nodo_grd[agv_num][nodo] = grd;
  // --- arrotondamento per eccesso sui decimali > 0.5
  grd = (long double)(grd) - (long double)(N.nodo_grd[agv_num][nodo]);
  if(grd>0.5) N.nodo_grd[agv_num][nodo]++;
  //-----------------------------------------------------
  // ANNOVI
  // Anzichè gli impulsi, viene passata alla navetta i
  // gradi calcolati e visualizzati nell'esecuzione
  // missione
  //-----------------------------------------------------
  *impulsi = N.nodo_grd[agv_num][nodo];
  return 0;
}

// -------------------------------
//    test_position_into_path()
// -------------------------------
// Testa se la posizione attuale dell'AGV Š compresa nel percorso della
// missione. Torna 1 se posizione 0 o fuori da percorso missione.
//
int agv::test_position_into_path(char *all_mess)
/**********************************************/
{
  sprintf(all_mess, "");
  if(stato.pos == 0){     // ** posizione attuale = 0
     sprintf(all_mess, MESS[638], agv_num);
     return 1;
  };
  for(ptr_path=0; ptr_path < MAXPERCORSI; ptr_path++){
     if(mission.punto[ptr_path] == stato.pos) break;
     if(mission.punto[ptr_path] == 0) break;
  }
  if(mission.punto[ptr_path] != stato.pos){
     sprintf(all_mess, MESS[636], agv_num, stato.pos);
     return 1;
  }
  return 0;
}


// ------------------------
//    trs_mission()
// ------------------------
// Trasmissione missione ad AGV
//
int agv::trs_mission(char *all_mess)
/**********************************/
{
  int  err;
  WORD dm[20];
  WORD dm_start[101];
  WORD DMIniziale;
  WORD TypInforco;
  WORD LatoInforco;
  char filename[21]="";
  char str_miss[21]="";
  double a, b;
  double rap_imp;
  short int count_nodi_prenotati;
  short int TypCom, NumStep, ptr_percorso;
  short int NumStruttura, TotBoxInTrasporto;
  short int i, j, k, lato_box, tot_pos, num_pos;
  short int TipoDest, DistUltimoDep, CaricoScarico, DistBoxInDep,
            ProfonditaMax, TipoBox, AltezzaBaia, Sollevamento;
  short int Piano, Posizione, PosizioneTgv;
  short int PianoP, PosizioneP, PosizioneTgvP;
  short int num_baia, num_mac, num_mag, num_stz;
  int  SovrapCarWash = SOVRAPPOSIZIONE_CARWASH;
  bool Presa[POSINPRESA+1][PIANIINPRESA+1];              // Matrice di configurazione presa
  bool Basso, MissScar, Pieno, PresaSovrap;
  bool lato_ingresso, lato_uscita;
  bool aggiorna_pintendtrasmesso;
  bool percorso_occupato_fino_a_dest;

  i=0;
  aggiorna_pintendtrasmesso=true;
  memset(&dm[0],       0, sizeof(dm));
  memset(&dm_start[0], 0, sizeof(dm_start));
  //---------------------------------------------
  // Memorizzio "1" come rapporto di divisione
  // nel caso delle navette lasert, in modo da
  // gestire le distanze in cm anzichè in mm
  //---------------------------------------------
  rap_imp = mission.RapEncoder;
  if(mission.GestioneLaser==true) rap_imp=1;
  //---------------------------------------------
  // Recupero il tipo di comunicazione riservato
  // per il PLC TGV
  //---------------------------------------------
  TypCom = OmrPlc[agv_num].TypCom;
  if(TypCom==TYPCOM_BECKHOFF){
     err = ASRV_trs_mission(all_mess);
     if(err) return err;
  }
  //---------------------------------------------
  // compila passi programma ( missione.step[] )
  // fino al bit di end
  //---------------------------------------------
  do{
     cbit = (struct comandi *) &mission.step[i];
     //----------------------------------------
     // Nessun altro step
     //----------------------------------------
     if(mission.step[i]==0) break;
     //----------------------------------------
     // 14/11/2013
     // Deve esserci solo il bit di END
     //----------------------------------------
     if( cbit->end &&
        !cbit->grd0 &&
        !cbit->grd90 &&
        !cbit->vel &&
        !cbit->av &&
        !cbit->ind &&
        !cbit->carico &&
        !cbit->scarico &&
        !cbit->grd180 &&
        !cbit->grd270 &&
        !cbit->forchealte &&
        !cbit->curva &&
        !cbit->antiora &&
        !cbit->avvic &&
        !cbit->ral &&
        !cbit->az) break;
     i++;
  }while(1);

  //----------------------------------------------------
  // AL-FPT 21/08/2014
  // conto nodi prenotati
  //
  // !!! IMPORTANTE !!!
  // salto il nodo iniziale e nodo finale perchè in
  // caso di missione senza anello ma manovra sul posto
  // darebbe un falso conteggio.
  //----------------------------------------------------
  count_nodi_prenotati=0;
  for(j=1; j<MAXPERCORSI; j++){
     if(mission.punto_prenotato[j+1]==0    ) break;
     if(mission.punto_prenotato[j]<1       ) continue;
     if(mission.punto_prenotato[j]>TOTPUNTI) continue;
     count_nodi_prenotati++;
  }

  NumStep = i;
  //-------------------------------------------------------------------------------
  // 25/02/99
  // In caso di MISSIONI_AL_VOLO....
  //-------------------------------------------------------------------------------
  if(!mission.NoMissioniAlVolo){
     //-------------------------------------------------------------------------------
     // Se START==0 Trasmetti tutto
     //-------------------------------------------------------------------------------
     if( stato.start==0 ){
        i++;
        //------------------------------------------------------
        // Recupero info a bordo
        //------------------------------------------------------
        TipoBox = 0;
        Pieno=PresaSovrap=false;
        memset(&Presa[0][0], false, sizeof(Presa));
        // MISSIONI DI SCARICO (recupero le info dal box che devo scaricare)
        switch(mission.Typ){
           case MISS_SCARICO_SU_MACCHINA:
           case MISS_SCARICO_SU_BAIA:
           case MISS_SCARICO_SU_MAGAZZINO:  MissScar=true;
                                            break;
           default:                         MissScar=false;
                                            break;
        }
        //------------------------------------------------------
        // Nelle missioni di scarico recupero le info in
        // base al tipo presa
        //
        // AL-20/06/2013 LE PRESE SONO TUTTE SOVRAPPONIBILI
        // tranne quelle con il flag di Car-Wash.
        // Il bit di "sovrapposizione" è importante per il
        // controllo dello stato della presa su cui sovraporre
        // che ovviamente deve essere ON
        //------------------------------------------------------
        if(MissScar==true){
           PresaSovrap=true;
           if(mission.TypPresa==N_NO_FORCHE){Pieno=false; TipoBox = 0; PresaSovrap=false;}
           // FORCA "A" IMPEGNATA
           if(mission.TypPresa==N_FORCA_A  ){
              Pieno       = true;
              if(mission.PresCEST_A==PRES_VUOTO) Pieno=false;
              TipoBox     = mission.BoxA.TipoBox;
              Basso       = mission.BoxA.Basso;
              if(SovrapCarWash==0 && mission.BoxA.CarWash!=0) PresaSovrap = false;
              memcpy(&Presa[0][0], &mission.BoxA.Presa[0][0], sizeof(Presa));
           }
           // FORCA "B" IMPEGNATA
           if(mission.TypPresa==N_FORCA_B  ){
              Pieno       = true;
              if(mission.PresCEST_B==PRES_VUOTO) Pieno=false;
              TipoBox     = mission.BoxB.TipoBox;
              Basso       = mission.BoxB.Basso;
              if(SovrapCarWash==0 && mission.BoxB.CarWash!=0) PresaSovrap = false;
              memcpy(&Presa[0][0], &mission.BoxB.Presa[0][0], sizeof(Presa));
           }
           // FORCA "A+B" IMPEGNATA
           if(mission.TypPresa==N_FORCA_AB ){
              Pieno       = true;
              if(mission.PresCEST_B==PRES_VUOTO) Pieno=false;
              TipoBox     = mission.BoxB.TipoBox;
              Basso       = mission.BoxB.Basso;
              if(SovrapCarWash==0 && mission.BoxB.CarWash!=0) PresaSovrap = false;
              memcpy(&Presa[0][0], &mission.BoxB.Presa[0][0], sizeof(Presa));
           }
        }
        //------------------------------------------------------
        // Inizializzazione variabili:
        //------------------------------------------------------
        TypInforco=0;
        LatoInforco=0;
        AltezzaBaia=0;
        Sollevamento=0;
        DistBoxInDep=0;
        CaricoScarico=0;
        ProfonditaMax=0;
        DistUltimoDep=0;
        TipoDest=DEST_TIPO_NODO_SEMPLICE;
        Piano=Posizione=PosizioneTgv=0;
        PianoP=PosizioneP=PosizioneTgvP=0;
        num_baia=num_mac=num_mag=num_stz=num_pos=0;
        P.test_punto_presente_mac(all_mess, mission.pend, &num_mac, &num_stz);
        P.test_punto_presente_baia(all_mess, mission.pend, &num_baia);
        M.test_punto_presente_mag(all_mess, mission.pend, &num_mag, &num_pos);
        //------------------------------------------------------
        // TIPO DESTINAZIONE:
        // 0 - Nodo no struttura
        // 1 - Macchina,
        // 2 - Magazzino,
        // 3 - Baia
        // 4 - Baia/Scaffale
        // 5 - Magazzino MULTI-NODO
        //------------------------------------------------------
        if(num_mac==0 && num_baia==0 && num_mag==0) TipoDest = DEST_TIPO_NODO_SEMPLICE;
        else{
           // macchine
           if(num_mac>0 && num_mac<=TOTCHIAMATE) TipoDest = DEST_TIPO_NODO_MACCHINA;
           // magazzini
           if(num_mag>0 && num_mag<=TOTMAG     ){
              TipoDest = DEST_TIPO_NODO_MAGAZZINO;
              //--------------------------------------------
              // MAGAZZINI MULTI NODO
              // Verifico se si tratta di un magazzino
              // multi-nodo
              //--------------------------------------------
              if(num_stz!=0 && (M.m[num_mag]->Pos[num_stz].NodoIng!=0 || M.m[num_mag]->Pos[num_stz].NodoOut!=0)){
                 TipoDest = DEST_TIPO_NODO_MAG_MULTI_NOD;
              }
           }
           // baie
           if(num_baia>0 && num_baia<=TOTBAIE  ){
              TipoDest = DEST_TIPO_NODO_BAIA;
           }
        }
        // BAIA
        if(num_baia>0 && num_baia<=TOTBAIE){
           NumStruttura = num_baia;
           AltezzaBaia  = P.ba[num_baia]->BaiaRialzata;
           Sollevamento = P.ba[num_baia]->Sollevamento;
           //--------------------------------------------------------------------
           // MISSIONI DI PRELIEVO DA BAIA
           //--------------------------------------------------------------------
           if(mission.Typ==MISS_CARICO_DA_BAIA || (mission.Typ==MISS_SOLO_POSIZIONAMENTO && (stato.s.bit.carB==false && stato.s.bit.carA==false))){
              //-----------------------------------------------------------------
              // In fase di prelievo il TIPO BOX viene recuperato dalla baia
              //-----------------------------------------------------------------
              TipoBox = P.ba[num_baia]->TipoBox[0];
              if(P.ba[num_baia]->TipoCod[0]!=LIBERA && P.ba[num_baia]->TipoCod[0]!=ST_BOX_VUOTI) Pieno=true;
              //--------------------------------------------------------------------
              // Memorizzo stato presa di carico
              //--------------------------------------------------------------------
              memcpy(&Presa[0][0], &P.ba[num_baia]->BoxB.Presa, sizeof(Presa));
              if(mission.TypPresa==N_FORCA_A){
                 memcpy(&Presa[0][0], &P.ba[num_baia]->BoxA.Presa, sizeof(Presa));
              }
           }
        }
        // MACCHINA
        if(num_mac>0  && num_mac<=TOTCHIAMATE ){
           NumStruttura  = num_mac;
           lato_box      = M.TipiBox[TipoBox].Lung;
           AltezzaBaia   = P.ch[num_mac]->Staz[num_stz].BaiaRialzata;
           DistBoxInDep  = (short int)(P.ch[num_mac]->dist_box_in_mac/rap_imp);
           tot_pos       = P.ch[num_mac]->Staz[num_stz].Npt-1;   // calcolo effettivo palette a terra - 1
           //--------------------------------------------------------------------
           // Calcolo profondità massima in base alle pos paletta
           //--------------------------------------------------------------------
           a = (double)((tot_pos*(lato_box+DistBoxInDep))/rap_imp);
           b = (double)(a) - (int)(a);
           if(b>0.5)  a = (a - b) + 1; // arrotondo all'impulso/cm successivo
           ProfonditaMax = (short int)(a);
           //--------------------------------------------------------------------
           // MISSIONI DI PRELIEVO DA MACCHINA
           //--------------------------------------------------------------------
           if(mission.Typ==MISS_CARICO_DA_MACCHINA || (mission.Typ==MISS_SOLO_POSIZIONAMENTO && (stato.s.bit.carB==false && stato.s.bit.carA==false))){
              lato_ingresso=false;
              if(P.ch[num_mac]->Staz[num_stz].PuntoIng!=P.ch[num_mac]->Staz[num_stz].PuntoOut && mission.pend==P.ch[num_mac]->Staz[num_stz].PuntoIng) lato_ingresso=true;
              //--------------------------------------------------------------------
              // RECUPERO PIANO/POSIZIONE DI CARICO
              //--------------------------------------------------------------------
              if(P.VerificaTipoPrelievo(lato_ingresso, num_mac, num_stz, &Posizione, &PosizioneTgv, all_mess)) return 1;
              //--------------------------------------------------------------------
              // In fase di prelievo il TIPO BOX viene recuperato dalla macchina
              //--------------------------------------------------------------------
              TipoBox     = P.ch[num_mac]->Staz[num_stz].TipoBox;
              if(P.ch[num_mac]->Staz[num_stz].StatoLav!=LIBERA && P.ch[num_mac]->Staz[num_stz].StatoLav!=ST_BOX_VUOTI) Pieno=true;
              //--------------------------------------------------------------------
              // Memorizzo stato presa di carico
              //--------------------------------------------------------------------
              memcpy(&Presa[0][0], &P.ch[num_mac]->Staz[num_stz].Pos[0].BoxB.Presa, sizeof(Presa));
              if(mission.TypPresa==N_FORCA_A){
                 memcpy(&Presa[0][0], &P.ch[num_mac]->Staz[num_stz].Pos[0].BoxA.Presa, sizeof(Presa));
              }
           }
           //--------------------------------------------------------------------
           // MISSIONI DI DEPOSITO SU MACCHINA
           //--------------------------------------------------------------------
           if(mission.Typ==MISS_SCARICO_SU_MACCHINA){
              lato_uscita=false;
              if(P.ch[num_mac]->Staz[num_stz].PuntoOut!=P.ch[num_mac]->Staz[num_stz].PuntoIng){
                 if(mission.pend==P.ch[num_mac]->Staz[num_stz].PuntoOut) lato_uscita=true;
              }
              //--------------------------------------------------------------------
              // RECUPERO PIANO/POSIZIONE DI SCARICO
              //--------------------------------------------------------------------
              if(P.VerificaTipoDeposito(lato_uscita, num_mac, num_stz, &Posizione, &PosizioneTgv, all_mess)) return 1;
           }
        }
        // MAGAZZINO
        if(num_mag>0  && num_mag<=TOTMAG ){
           lato_uscita   = false;
           lato_ingresso = false;
           NumStruttura  = num_mag;
           lato_box      = M.TipiBox[TipoBox].Lung;
           AltezzaBaia   = M.m[num_mag]->BaiaRialzata;
           Sollevamento  = M.m[num_mag]->Sollevamento;
           DistBoxInDep  = (short int)(M.TipiBox[TipoBox].dist_box_in_magaz/rap_imp);
         //ProfonditaMax = (short int)(M.m[num_mag]->Profondita/rap_imp);
           tot_pos       = M.m[num_mag]->Npt-1;   // calcolo effettivo palette a terra - 1
           //--------------------------------------------------------------------
           // GESTIONE DINAMICA MAGAZZINI
           // Calcolo delle posizioni a terra dalla superficie abilitata per il
           // deposito
           //--------------------------------------------------------------------
           if(GestioneDinamicaMagazzini==1){
              a = (double)(M.m[num_mag]->Profondita)/(double)(lato_box+M.TipiBox[TipoBox].dist_box_in_magaz);
              b = (double)(a) - (int)(a);
              if(b>M.TipiBox[M.EditMag.TipoBox].eccesso_count_pos_mag) a = (a - b) + 1;      // tolgo i decimali al valore calcolato e aggingo "1".
              tot_pos = (short int)(a);           // calcolo effettivo palette a terra - 1
           }
           //--------------------------------------------------------------------
           // Calcolo profondità massima in base alle pos paletta
           //--------------------------------------------------------------------
           a = (double)(M.m[num_mag]->OffSetDaNodo/rap_imp)+(double)(M.TipiBox[TipoBox].offset_tgv_da_nodo_mag/rap_imp)+(double)((tot_pos*(lato_box+M.TipiBox[TipoBox].dist_box_in_magaz))/rap_imp);
           b = (double)(a) - (int)(a);
           if(b>0.5)  a = (a - b) + 1; // arrotondo all'impulso/cm successivo
           ProfonditaMax = (short int)(a);
           //--------------------------------------------------------------------
           // MISSIONI DI PRELIEVO DA MAGAZZINO
           //--------------------------------------------------------------------
           if(mission.Typ==MISS_CARICO_DA_MAGAZZINO || (mission.Typ==MISS_SOLO_POSIZIONAMENTO && (stato.s.bit.carB==false && stato.s.bit.carA==false))){
              if(M.m[num_mag]->Fifo==false || (M.m[num_mag]->Fifo==true && mission.NodIngMag==M.m[num_mag]->PuntoIng)) lato_ingresso=true;
              else                                                                                                     lato_uscita=true;
              //--------------------------------------------------------------------
              // RECUPERO PIANO/POSIZIONE DI CARICO
              //--------------------------------------------------------------------
              if(!M.VerificaTipoPrelievo(lato_ingresso, num_mag, mission.TypPresa, &Posizione, &Piano, &PosizioneTgv, all_mess)){
                 //--------------------------------------------------------------------
                 // Memorizzo stato presa di carico
                 //--------------------------------------------------------------------
                 memcpy(&Presa[0][0], &M.m[num_mag]->Pos[Posizione].Box[Piano].Presa[0][0], sizeof(Presa));
                 //--------------------------------------------------------------------
                 // In fase di prelievo il TIPO BOX viene recuperato dal magazzino
                 //--------------------------------------------------------------------
                 TipoBox = M.m[num_mag]->Pos[Posizione].Box[Piano].TipoBox;
                 if(M.m[num_mag]->Pos[Posizione].Box[Piano].StatoLav!=ST_BOX_VUOTI) Pieno=true;
                 //--------------------------------------------------------------------
                 // QUOTA ULTIMO DEPOSITO
                 // Viene compilata solo per i magazzini Lifo dove c'è la certezza che
                 // il prelievo e il deposito sono avvenuti dallo stesso lato
                 //--------------------------------------------------------------------
                 if(M.m[num_mag]->PuntoIng==M.m[num_mag]->PuntoOut){
                    DistUltimoDep = M.m[num_mag]->Pos[Posizione].QuotaP[Piano];
                 }
                 AltezzaBaia   = M.m[num_mag]->Pos[Posizione].QuotaH[Piano];
              }
           }
           //--------------------------------------------------------------------
           // MISSIONI DI SCARICO SU MAGAZZINO
           //--------------------------------------------------------------------
           if(mission.Typ==MISS_SCARICO_SU_MAGAZZINO){
              if(M.m[num_mag]->Fifo==true && mission.NodIngMag==M.m[num_mag]->PuntoOut) lato_uscita=true;
              else                                                                      lato_ingresso=true;
              //--------------------------------------------------------------------
              // RECUPERO PIANO/POSIZIONE DI SCARICO
              //--------------------------------------------------------------------
              if(M.VerificaTipoDeposito(lato_uscita, num_mag, num_pos, mission.TypPresa, TipoBox, Presa, PresaSovrap, Basso, FALSE, &Posizione, &Piano, &PosizioneTgv, all_mess)) return 1;
              //--------------------------------------------------------------------
              // Recupero QUOTE ULTIMO DEPOSITO
              // Verifico a che quota dal nodo è stato fatto l'ultimo deposito
              // a TERRA in magazzino
              // N.b. Il magazzino deve essere di tipo Lifo per essere sicuri
              //      che il dato sia attendibilie
              //--------------------------------------------------------------------
              if(M.m[num_mag]->NumCestoni!=0 && M.m[num_mag]->PuntoIng==M.m[num_mag]->PuntoOut){
                 if(!M.VerificaTipoPrelievo(lato_ingresso, num_mag, mission.TypPresa, &PosizioneP, &PianoP, &PosizioneTgvP, all_mess) && PianoP==0){;
                    DistUltimoDep = M.m[num_mag]->Pos[PosizioneP].QuotaP[PianoP];
                 }
              }
              //--------------------------------------------------------------------
              // In deposito recupero la quota dalle altezza pallet sotto
              //--------------------------------------------------------------------
              for(j=0; j<Piano; j++){
                  AltezzaBaia = AltezzaBaia + M.m[num_mag]->Pos[Posizione].Box[j].Altezza;
              }
           }
           //------------------------------------------------------
           // MAGAZZINI MULTI-NODO
           // Posizione in profondità fissa "1" perchè il TGV
           // non deve andare in ricerca della posizione
           //------------------------------------------------------
           if(MagazziniMultiNodo==true) PosizioneTgv=1;
           //------------------------------------------------------
           // LATO INFORCO
           // gestione lato ingresso in magazzino
           //
           // FIFO: bit 0 - lato deposito
           //       bit 1 - lato prelievo
           //------------------------------------------------------
           if(M.m[num_mag]->Fifo==true){
              if(lato_ingresso==true  ) SetBit((char *)&LatoInforco, 0);
              if(lato_uscita==true    ) SetBit((char *)&LatoInforco, 1);
           }
        }
        //------------------------------------------------------
        // Conteggio BOX in trasporto
        //------------------------------------------------------
        TypInforco=0;
        TotBoxInTrasporto=0;
        for(k=0; k<POSINPRESA; k++){
           for(j=0; j<PIANIINPRESA; j++){
              if(Presa[k][j]==false) continue;
              //------------------------------------------------------
              // DM STATO INFOCO
              // bit0... 3 - Stato presenza al piano "0"
              // bit4... 7 - Stato presenza al piano "1"
              // bit8...11 - Stato presenza al piano "2"
              //------------------------------------------------------
              SetBit((char *)&TypInforco, k + (4*j));
              //------------------------------------------------------
              // Totalizzatore Pallet
              //------------------------------------------------------
              TotBoxInTrasporto++;
           }
        }
        //------------------------------------------------------
        // GESTIONE DINAMICA MAGAZZINI (Calcolata dinamic.)
        // D3000 = impulsi da nodo su deposito  1° posizione
        // D3001 = impulsi da nodo su deposito  2° posizione
        // D3002 = impulsi da nodo su deposito  3° posizione
        // D3003 = impulsi da nodo su deposito  4° posizione
        // D3004 = impulsi da nodo su deposito  5° posizione
        // D3005 = impulsi da nodo su deposito  6° posizione
        // D3006 = impulsi da nodo su deposito  7° posizione
        // D3007 = impulsi da nodo su deposito  8° posizione
        // D3008 = impulsi da nodo su deposito  9° posizione
        // D3009 = impulsi da nodo su deposito 10° posizione
        // ...
        // D3029 = impulsi da nodo su deposito 30° posizione
        //------------------------------------------------------
        if(num_mag>0 && num_mag<=TOTMAG ){
           //---------------------------------------------------------
           // Recupero il lato box su cui eseguire il calcolo
           //---------------------------------------------------------
           lato_box = M.TipiBox[TipoBox].Lung;
           //---------------------------------------------------------
           // Calcolo capacita magazzin presunta in base al tipo box
           //---------------------------------------------------------
           for(j=0; j<MAXTERRAMAG; j++){
              a = (double)(M.m[num_mag]->OffSetDaNodo/rap_imp)+(double)(M.TipiBox[TipoBox].offset_tgv_da_nodo_mag/rap_imp)+(double)((j*(lato_box+M.TipiBox[TipoBox].dist_box_in_magaz))/rap_imp);
              b = (double)(a) - (int)(a);
              if(b>0.5)  a = (a - b) + 1; // arrotondo all'impulso successivo
              //-------------------------------------------
              // Compilo mappa delle posizioni di deposito
              //-------------------------------------------
              dm_start[j] = (WORD)(a);
           }
        }
        // MACCHINE
        if(num_mac>0 && num_mac<=TOTCHIAMATE ){
           //---------------------------------------------------------
           // Calcolo capacita magazzin presunta in base al tipo box
           //---------------------------------------------------------
           for(j=0; j<MAXPOSTISTAZ; j++){
              a = (double)((j*(lato_box+P.ch[num_mac]->dist_box_in_mac))/rap_imp);
              b = (double)(a) - (int)(a);
              if(b>0.5)  a = (a - b) + 1; // arrotondo all'impulso successivo
              //-------------------------------------------
              // Compilo mappa delle posizioni di deposito
              //-------------------------------------------
              dm_start[j] = (WORD)(a);
           }
        }
        //------------------------------------------------------
        // TRACCIA DATI AUSILIARI INVIATI A TGV
        //------------------------------------------------------
        switch(mission.Typ){
           case  MISS_SOLO_POSIZIONAMENTO:  strcpy(str_miss, "POSIZ");
                                            CaricoScarico=0;
                                            break;
           case  MISS_CARICO_DA_MACCHINA:   strcpy(str_miss, "C_MAC");
                                            CaricoScarico=1;
                                            break;
           case  MISS_SCARICO_SU_MACCHINA:  strcpy(str_miss, "S_MAC");
                                            CaricoScarico=2;
                                            break;
           case  MISS_CARICO_DA_MAGAZZINO:  strcpy(str_miss, "C_MAG");
                                            CaricoScarico=1;
                                            break;
           case  MISS_SCARICO_SU_MAGAZZINO: strcpy(str_miss, "S_MAG");
                                            CaricoScarico=2;
                                            break;
           case  MISS_CARICO_DA_BAIA:       strcpy(str_miss, "C_BAI");
                                            CaricoScarico=1;
                                            break;
           case  MISS_SCARICO_SU_BAIA:      strcpy(str_miss, "S_BAI");
                                            CaricoScarico=2;
                                            break;
           default:                         strcpy(str_miss, "NONE ");
        }
        //------------------------------------------------------
        // D3030 - PIANO DI PRELIEVO DEPOSITO
        // D3031 - TIPO PRESA (0 - per i posizion. semplici)
        //          1 = Forca Bassa
        //          2 = Forca Alta
        //         ... altri valori per impianti con forche
        //             speciali
        // D3032 - CONTENITORE VUOTO (utile per le velocità)
        //          0 - vuoto
        //          1 - pieno
        // D3033 - TIPO PALLET/BOX IN TRASPORTO
        //          (da definire a seconda dell'impianto)
        // D3034 - POSIZIONE DI PRELIEVO\DEPOSITO
        // D3035 - INTERASSE TRA PALETTE
        // D3036 - PROFONDITA MASSIMA (0 - non gestita)
        // D3037 - ALTEZZA BAIE IN IMP.
        // D3038 - DESTINAZIONE MISSIONE
        // D3039 - TIPO MISSIONE
        //          0 - Posizionamento semplice
        //          1 - Carico
        //          2 - Scarico
        // D3040 - DISTANZA ULTIMO DEPOSITO IN MAGAZZINO
        // D3041 - TIPO DESTINAZIONE
        //          0 - Nodo percorso/Ricarica
        //          1 - Macchina
        //          2 - Magazzino
        //          3 - Baia
        //          4 - Baia scaffale
        //          5 - Magazzino a multi nodo
        // D3042 - SOLLEVAMENTO CARICO/SCARICO.
        // D3043 - Altezza BOX/BOBINA in trasporto
        // D3044 - Diametro/Larghezza PRESA in trasporto
        // D3045 - Numero BOX in trasporto
        // D3046 - Apertura Forche rispetto al centro (LARGHEZZA/2)
        //         (riferita a una solo forca rispetto al centro)
        // D3047 - Apertura Forche INTERNE /2 (SOLO PREVISTA)
        //         (riferita a una solo forca rispetto al centro)
        // D3048 - Disassamento montante (SX = "+", DX = "-")
        // D3049 - TIPO INFORCO       (schema di inforco)
        // D3050 - LATO INFORCO       (lato ingresso in magazzino)
        // D3051 - AVANZAMENTO FORCHE (significativo solo per TRILATERALI)
        // D3052 - NUMERO STRUTTURA   (Numero Macchina, Scaffale, Baia...ecc)
        //------------------------------------------------------
        dm_start[30]=Piano;
        dm_start[31]=mission.TypPresa;
        dm_start[32]=Pieno;
        dm_start[33]=TipoBox;
        dm_start[34]=PosizioneTgv;
        dm_start[35]=DistBoxInDep;
        dm_start[36]=ProfonditaMax;
        dm_start[37]=AltezzaBaia;
        dm_start[38]=mission.pend;
        dm_start[39]=CaricoScarico;
        dm_start[40]=DistUltimoDep;
        dm_start[41]=TipoDest;
        dm_start[42]=Sollevamento;
        dm_start[43]=0;
        dm_start[44]=0;
        dm_start[45]=TotBoxInTrasporto;
        dm_start[46]=0;
        dm_start[47]=0;
        dm_start[48]=0;
        dm_start[49]=TypInforco;
        dm_start[50]=LatoInforco;
        dm_start[51]=0;
        dm_start[52]=NumStruttura;
        //------------------------------------------------------
        // ADEGUAMENTO SPECIALIZZATO PER SINGOLO IMPIANTO
        //------------------------------------------------------
        AdjustDMStartMission_IMPIANTO(agv_num, &dm_start[0]);
        //------------------------------------------------------
        // Composizione stringa
        //------------------------------------------------------
        sprintf(all_mess, "Pend:%04d, %s, Presa:%02d, Pos:%02d, Pin:%02d, P%05d, H:%04d, S:%04d, Pieno:%d, TipoBox:%02d",
                mission.pend,
                str_miss,
                mission.TypPresa,
                PosizioneTgv,
                Piano,
                ProfonditaMax,
                AltezzaBaia,
                Sollevamento,
                Pieno,
                TipoBox);
        //------------------------------------------------------
        // Scrittura di un file per ogni TGV
        //------------------------------------------------------
        sprintf(filename, "LogDm%02d.txt", agv_num);
        file_cronologico_generico(filename, all_mess, 80000L);
        //------------------------------------------------------
        // SIMULAZIONE
        // Salto la scrittura a PLC e metto il START=1
        //------------------------------------------------------
        if(mission.EsclPLC || break_Com[COM1]){
           if( stato.start==0 ) stato.start = 1;
           return 0;
        }
        //------------------------------------------------------
        // RILETTURA KM PERCORSI
        // Lettura dei KM percorsi dal TGV
        //------------------------------------------------------
        if(!OM.ld_canale(&dm[0], (WORD)(agv_num), "RD", 304, 1, all_mess)){
           mission.KmPercorsi = (short int)(dm[0]);
        }
        //------------------------------------------------------
        // Scrittura a PLC
        // Tot 60 canali DM STATICI MISSIONE
        //------------------------------------------------------
        if(OM.wr_canale(&dm_start[0], (WORD)(agv_num), "WD", 3000, 61, all_mess)) return 1;
        //-------------------------------------------
        // A) Trasmetti Passi programma DM1401..1800
        // HOST LINK - Max 25 DM per volta
        //-------------------------------------------
        if(TypCom==0 || TypCom==3){
           for( DMIniziale=(WORD)(1401); DMIniziale<(WORD)(i+1401); DMIniziale+=(WORD)(25) ){
              if(((i+1401) - DMIniziale ) <25 ){
                 if(OM.wr_canale(&mission.step[DMIniziale-1401], (WORD)(agv_num), "WD", DMIniziale, (WORD)(i+1401-DMIniziale), all_mess)) return 1;
              }
              else{
                 if(OM.wr_canale(&mission.step[DMIniziale-1401], (WORD)(agv_num), "WD", DMIniziale, 25, all_mess)) return 1;
              }
           }
        }
        //--------------------------------------------------
        // FINS PROTOCOL / TCP-IP / ALGO COMUNICATION
        // Max 450 DM per volta
        //--------------------------------------------------
        if(TypCom==1 || TypCom==4 || TypCom==2){
           for( DMIniziale=(WORD)(1401); DMIniziale<(WORD)(i+1401); DMIniziale+=(WORD)(450) ){
              if(((i+1401) - DMIniziale ) <450 ){
                 if(OM.long_wr_canale(&mission.step[DMIniziale-1401], (WORD)(agv_num), "WD", DMIniziale, (WORD)(i+1401-DMIniziale), all_mess)) return 1;
              }
              else{
                 if(OM.long_wr_canale(&mission.step[DMIniziale-1401], (WORD)(agv_num), "WD", DMIniziale, 450, all_mess)) return 1;
              }
           }
        }
        //--------------------------------------------------
        // B) Trasmetti il nodo intermedio di STOP DM1400
        //    se coincide con nodo finale della missione
        //    allora trasmetti  "0" (NESSUN BLOCCO)
        //--------------------------------------------------
        dm[0]=mission.pintend;
        //--------------------------------------------------
        // AL28/03 il DM del PINT viene messo a "0" solo
        //         se la destinazione è un punto notevole
        //
        // AL-FPT 21/08/2014 azzero il PINT sono se sono
        // solo se non ci sono nodi prenotati e quindi
        // il percorso è occupato fino a destinazione
        //--------------------------------------------------
      //if( mission.pintend == mission.pend && N.punti_notevoli[mission.pend]==true) dm[ 0 ]=0;
        if(mission.pintend==mission.pend && count_nodi_prenotati==0) dm[ 0 ]=0;
        //--------------------------------------------------
        // GESTIONE_ASRV (trasmissione pintend - D1400)
        //--------------------------------------------------
        #ifdef GESTIONE_ASRV
           asrv_comandi.pintend = dm[0];
        #endif;
        //--------------------------------------------------
        // Trasferimento a PLC
        //--------------------------------------------------
        if(OM.wr_canale(&dm[ 0 ], (WORD)(agv_num), "WD", 1400,  1, all_mess)) return 1;
        //------------------------------------------------------
        // 15/10/2012 Spostati 1998, 1999 -> 2998, 2999 per
        //            aumento step programma ammessi
        //
        // DM2998
        // ex TIPO BOX (ora nessun significato)
        // DM2999
        // Numero totale Step di programma
        //------------------------------------------------------
        dm[0]=0;
        dm[1]=(WORD)(NumStep);
        TraduciPCtoPLC(&dm[1]);
        if(OM.wr_canale(&dm[0], (WORD)(agv_num), "WD", 2998,  2, all_mess)) return 1;
        //--------------------------------------------------
        // SERVER ALGODROID
        // Scrittura DM DI SICUREZZA di controllo
        //
        // Nota: se la comunicazione ALGODROIN non è attiva
        //       forzo "0" nei DM di controllo, questo
        //       garantirà la movimentazione manuale
        //--------------------------------------------------
        if(break_Com[COM9]==false){
           dm[0] = (WORD)(COMM_ALGOSISTEMI);
           dm[1] = (WORD)(ABILITA_OPZIONI_ANDROID);  // Tipo impianto
           dm[1] = (WORD)((dm[1]<<8) | agv_num);     // Numero TGV
           if(OM.wr_canale(&dm[0], (WORD)(agv_num), "WD", 1001,  2, all_mess)) return 1;
        }
        //-----------------------------------
        // C) Setta lo start mission (DM 91)
        //-----------------------------------
        dm[0]=1;
        if(OM.wr_canale(&dm[ 0 ], (WORD)(agv_num), "WD", 91,  1, all_mess)) return 1;
        stato.start = 1;
        //--------------------------------------------------
        // GESTIONE_ASRV (trasmissione start mission D91)
        //--------------------------------------------------
        #ifdef GESTIONE_ASRV
           asrv_comandi.start = dm[0];
        #endif;
        return 0;
     }
     //--------------------------------------------------------
     // Se START==1 Trasmetti solo nodo intermedio di STOP
     //--------------------------------------------------------
     if( stato.start==1 ){
        //--------------------------------------------------
        // B1) Trasmetti il nodo intermedio di STOP DM1400
        //     se coincide con nodo finale della missione
        //     allora trasmetti  "0" (NESSUN BLOCCO)
        //--------------------------------------------------
        dm[0]=mission.pintend;
        if(mission.pintend==mission.pend){
           percorso_occupato_fino_a_dest=true;
           //---------------------------------------------
           // ANELLO (AL-13/04/2018)
           // il percorso è considerato occupato fino
           // a destinazione solo se non ci sono altri
           // nodi prenotati
           //---------------------------------------------
           if(mission.pstart==mission.pend && mission.pintend==mission.pend){
              if(count_nodi_prenotati!=0) percorso_occupato_fino_a_dest=false;
           }
           //---------------------------------------------
           // 12/09/2006
           // Prima di azzerare il DM1400 mi accerto che
           // tutti i punti fino a destinazione siano
           // occupati dal TGV
           //---------------------------------------------
           if(percorso_occupato_fino_a_dest==true){
              ptr_percorso=0;
              for(j=0; j<MAXPERCORSI; j++){
                 if(mission.punto[j]==0        ) break;
                 if(mission.punto[j]!=stato.pos) continue;
                 ptr_percorso=j;
                 break;
              }
              if(ptr_percorso){
                 //-----------------------------------------------------------
                 // Verifico se i nodi dalla posizione attuale del TGV a dest
                 // sono occupati
                 //-----------------------------------------------------------
                 for(j=ptr_percorso; j<MAXPERCORSI; j++){
                    if(mission.punto[j]==0                   ) break;
                    if(N.nodo_busy[mission.punto[j]]==agv_num) continue;
                    percorso_occupato_fino_a_dest=false;
                    break;
                 }
              }
           }
           //---------------------------------------------
           // Se il percorso è occupato fino a dest
           // trasmtto "0"
           //---------------------------------------------
           if(percorso_occupato_fino_a_dest==true) dm[0]=0;
           else{
              //-------------------------------------------------
              // Se il percorso non è occupato fino a dest
              // interrompo l'aggiornamento di pintendtrasmesso
              // per accertarmi la ri-trasmissione
              //-------------------------------------------------
              aggiorna_pintendtrasmesso=false;
           }
        }
        //------------------------------------------------------
        // GESTIONE_ASRV (trasmissione pintend - D1400)
        //------------------------------------------------------
        #ifdef GESTIONE_ASRV
           asrv_comandi.pintend = dm[0];
        #endif;
        //------------------------------------------------------
        // SIMULAZIONE
        //------------------------------------------------------
        if(mission.EsclPLC==false && break_Com[COM1]==false){
           if(OM.wr_canale(&dm[ 0 ], (WORD)(agv_num), "WD", 1400,  1, all_mess)) return 1;
        }
     }
     //-------------------------------------------------------------------
     // Aggiornamento di "pintendtrasmesso"
     //-------------------------------------------------------------------
     if(aggiorna_pintendtrasmesso==true){
        mission.pintendtrasmesso = mission.pintend;
     }
  }
  else{
     //-------------------------
     // Trasmetti programma
     //-------------------------
     i++;
     if(i<25){if(OM.wr_canale(&mission.step[0], (WORD)(agv_num), "WD", 100,  i, all_mess)) return 1;}
        else if(OM.wr_canale(&mission.step[0], (WORD)(agv_num), "WD", 100, 25, all_mess)) return 1;
     if(i>=25){
        if(OM.wr_canale(&mission.step[25], (WORD)(agv_num), "WD", 125, 25, all_mess)) return 1;}
     if(i>=50){
        if(OM.wr_canale(&mission.step[50], (WORD)(agv_num), "WD", 150, 25, all_mess)) return 1;}
     if(i>=75)
        if(OM.wr_canale(&mission.step[75], (WORD)(agv_num), "WD", 175, 25, all_mess)) return 1;
     if(i>=100)
        if(OM.wr_canale(&mission.step[100], (WORD)(agv_num), "WD", 200, 25, all_mess)) return 1;
     if(i>=125)
        if(OM.wr_canale(&mission.step[125], (WORD)(agv_num), "WD", 225, 25, all_mess)) return 1;
     if(i>=150)
        if(OM.wr_canale(&mission.step[150], (WORD)(agv_num), "WD", 250, 25, all_mess)) return 1;
     if(i>=175)
        if(OM.wr_canale(&mission.step[175], (WORD)(agv_num), "WD", 275, 25, all_mess)) return 1;
     //--------------------------------
     // Setta lo start mission (DM 91)
     //--------------------------------
     stato.start = 1;
     if(OM.wr_canale((unsigned short int*) &stato.start, (WORD)(agv_num), "WD", 91, 1, all_mess)) return 1;
  }
  sprintf(all_mess, "OK");
  return 0;
}

// ------------------------
//    load_prg()
// ------------------------
// Lettura programma da AGV
//
int agv::load_prg(WORD *prg, char *all_mess)
/***************************************************/
{
  int i;

  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV Š escluso
  //---------------------------------------------------------
  if(break_Com[COM1]) return 0;
  if(mission.EsclPLC) return 0;

  if(!mission.NoMissioniAlVolo){
     for( i=0;i<MAXSTEP; i=i+25 ){
        if(OM.ld_canale(prg+i , (WORD)(agv_num), "RD", (WORD)(1401+i), 25, all_mess)) return 1;
        if( *(prg+i)==0 )break; // verifica fine programma
     }
  }
  else{
     for( i=0;i<200; i=i+25 ){
        if(OM.ld_canale(prg+i , (WORD)(agv_num), "RD", (WORD)(100+i), 25, all_mess)) return 1;
        if( *(prg+i)==0 )break; // verifica fine programma
     }
  }
  sprintf(all_mess, "OK");
  return 0;
}

// ------------------------
//  trasmetti_reset_AGV
// ------------------------
// resetta e quindi ripristina l'AGV ( DM 98 = 1 )
//
int agv::trasmetti_reset_AGV(char *all_mess)
/******************************************/
{
  WORD reset_AGV =1;

  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV Š escluso
  //---------------------------------------------------------
  if(break_Com[COM1]) return 0;
  if(mission.EsclPLC) return 0;

  //---------------------------------------------------------
  // DM 98 = 1
  //---------------------------------------------------------
  if(OM.wr_canale(&reset_AGV, (WORD)(agv_num), "WD", 98, 1, all_mess)) return 1;
  //---------------------------------------------------------
  // GESTIONE ASRV (fine missione a navetta - D98)
  //---------------------------------------------------------
  #ifdef GESTIONE_ASRV
     asrv_comandi.end=1;
  #endif;
  return 0;
}

// ------------------------
//  trasmetti_new_pos_AGV
// ------------------------
// Modifica i DM reltivi alla posizione dell'AGV su PLC
//
int agv::trasmetti_new_pos_AGV(int pos, char *all_mess)
/******************************************/
{
  WORD dm[5];

  memset(&dm, 0, sizeof(dm));
  dm[0] = (WORD)(pos);

  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV Š escluso
  //---------------------------------------------------------
  if(break_Com[COM1]) return 0;
  if(mission.EsclPLC) return 0;

  //--------------------------
  // DM 302 modifica posizione
  //--------------------------
  if(OM.wr_canale(&dm[0], (WORD)(agv_num), "WD", 302, 1, all_mess)) return 1;
  //---------------------------------------------------------
  // GESTIONE ASRV (nuova posizione a navetta - D302)
  //---------------------------------------------------------
  #ifdef GESTIONE_ASRV
     asrv_comandi.new_pos=pos;
  #endif;
  return 0;
}


// -------------------------------
//    test_percorso_to_target()
// -------------------------------
// Restituisce il numero di nodi restanti per arrivare alla
// posizione intermedia di end.
// Restituisce 1000 in caso la posizione intermedia coincida con la
// posizione finale.
//
int agv::test_percorso_to_target(short int *metri)
/*********************************/
{
  char all_mess[101]="";
  double DistTot, Dist;
  int i, j, DistX, DistY, x1, x2, y1, y2;
  int count_nodi_perc, count_nodi_occu;
  short int p1, p2, numero_nodi=0;
  short int num_baia, num_mac, num_stz, num_cbat, count_da_macc;

  *metri=0;
  //---------------------------------------------
  // Errore nella posizione attuale
  //---------------------------------------------
  if(stato.pos == 0){     // ** posizione attuale = 0
     return 0;
  };

  //---------------------------------------------
  // Conteggio del numero di nodi che compongono
  // la missione
  //---------------------------------------------
  count_nodi_perc=0;
  count_nodi_occu=0;
  for(i=0; i<MAXPERCORSI; i++){
     if(mission.punto[i]==0 ) break;
     count_nodi_perc++;
     if(N.nodo_busy[mission.punto[i]]==agv_num) count_nodi_occu++;
  }

  //---------------------------------------------
  // Se nodo finale coincide con nodo intermedio
  // ritorna 1000
  //---------------------------------------------
//if(mission.pintend==mission.pend){
  if(mission.pintend==mission.pend && (count_nodi_perc==count_nodi_occu)){
     numero_nodi = 1000;
     *metri      = 1000;
     return numero_nodi;
  }

  //-------------------------------------------------------------------
  // AL-27/04/2018
  // START CON PINT su punto di start per prossimo nodo RICHIESTA
  // punto successivo destinazione
  //-------------------------------------------------------------------
  if(mission.punto[1]==mission.pend && mission.punto[2]==0){
     numero_nodi = 1000;
     *metri      = 1000;
     return numero_nodi;
  }

  //---------------------------------------------
  // Il PInt intermedio a un nodo dalla dest
  //---------------------------------------------
  for(i=0; i<MAXPERCORSI; i++){
     if(mission.punto[i  ]==0               ) break;
     if(mission.punto[i  ]!=mission.pintend ) continue;
     if(mission.punto[i+1]!=mission.pend    ) continue;
     numero_nodi = 1000;
     *metri      = 1000;
     return numero_nodi;
  }

  //-----------------------------------------------------------
  // 08/01/2012 Il PInt intermedio è un Nodo di FUORI INGOMBRO
  // --> return 1000
  //-----------------------------------------------------------
  for(i=0; i<MAXNODIATTESA; i++){
     if(mission.pintend==0) break;
     if(mission.pintend!=N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]                ) continue;
     if(mission.pstart==mission.pintend && N.nodo_minpercorsi[mission.pstart]!=0) continue;
     numero_nodi = 1000;
     *metri      = 1000;
     return numero_nodi;
  }

  //----------------------------------------------------------
  // Pint coincide con pint forzato
  //----------------------------------------------------------
  if(N.verifica_pint_forzato(mission.pintend)==true){
     numero_nodi = 1000;
     return numero_nodi;
  }

  //---------------------------------------------
  // Punto intermedio a PIntForzato
  //---------------------------------------------
  if(mission.pintend==mission.PIntForzato){
     numero_nodi = 1000;
     *metri      = 1000;
     return numero_nodi;
  }

  //---------------------------------------------
  // Se il Punto intermedio di END corrisponde
  // al punto di accesso a una porte
  // --> return 1000
  //---------------------------------------------
  for(i=1; i<=MAXPORTEIMP; i++){
     for(j=0; j<11; j++){
        if(mission.pintend!=PorteImp[i].Punto[j]) continue;
        numero_nodi = 1000;
        *metri      = 1000;
        return numero_nodi;
     }
  }

  //---------------------------------------------
  // Recupera posizione attuale nel percorso
  //---------------------------------------------
  for(ptr_path=0; ptr_path < MAXPERCORSI; ptr_path++){
     if(mission.punto[ptr_path] == stato.pos) break;
     if(mission.punto[ptr_path] == 0        ) break;
  }
  if(ptr_path >= MAXPERCORSI ){
     numero_nodi = 1000;
     *metri      = 1000;
     return numero_nodi;
  }
  //---------------------------------------------
  // Se il punti intermedio è di rotazione
  // parto comunque
  //---------------------------------------------
  i=ptr_path;
  for(i; i<MAXPERCORSI; i++){
     if(AbilitaPuntoIntermedioSuRotaz==false                ) break;
     //--------------------------------------------------------------
     // MINIMO PUNTI INTERMEDI PER PARTENZA TGV
     // Se è stata compilata la struttura N.nodo_minpercorsi[]
     // ha priorità sulla gestione dei Punti Intermedi su rotazione
     //--------------------------------------------------------------
     if(N.nodo_minpercorsi[stato.pos]!=99                   ) break;
     //--------------------------------------------------------------
     // GESTIONE PRECEDENZE PER TGV IN RETTILINEO
     // interrompo il conteggio dei nodi se incontro una navetta
     // lanciata in rettilineo
     //--------------------------------------------------------------
   //p1 = mission.punto[i];
   //if(N.nodo_con_tgv_lanciato[p1]!=0 && N.nodo_con_tgv_lanciato[p1]!=agv_num) break;
     //--------------------------------------------------------------
     // Verifico se il punto è di rotazione
     //--------------------------------------------------------------
     if(i==0                                                ) continue;
     if(mission.punto[i]!=mission.pintend                   ) continue;
     if(mission.dati_perc[i].rot==mission.dati_perc[i-1].rot) continue;
     numero_nodi = 1000;
     *metri      = 1000;
     return numero_nodi;
  }
  //-------------------------------------------------------------
  // RICHIESTA INGRESSO A MACCHINE O BAIE o CAMBIOBATTERIE
  // In caso che la destinazione sia una macchina o una baia
  // deve essere ammesso anche il punto precedente
  //-------------------------------------------------------------
  num_baia=num_mac=num_stz=num_cbat=0;
  P.test_punto_presente_baia(all_mess, mission.pend, &num_baia);
  P.test_punto_presente_cbat(all_mess, mission.pend, &num_cbat);
  P.test_punto_presente_mac (all_mess, mission.pend, &num_mac, &num_stz);
  //-------------------------------------------------------------
  // Destinazione macchina
  //-------------------------------------------------------------
  if((num_baia>0 && num_baia<=TOTBAIE) || (num_mac>0 && num_mac<=TOTCHIAMATE) || (num_cbat>0 && num_cbat<=TOTBATT) ){
     //-------------------------------------------------------------
     // Conto i nodi che separano la destinazione dal PINT
     // o baia
     //-------------------------------------------------------------
     count_da_macc=0;
     for(i=MAXPERCORSI-1; i>ptr_path; i--){
        if(mission.punto[i]==0              ) continue;
        //----------------------------------------------
        // Conto solo fino al PINT
        //----------------------------------------------
        if(mission.punto[i]==mission.pintend) break;
        //------------------------------------
        // Incremento conteggio nodi da porta
        //------------------------------------
        count_da_macc++;
     }
     //-------------------------------------------------------------
     // Se sono nella "finestra" di RICHIESTA INGRESSO parto
     // anche sempre.
     //-------------------------------------------------------------
     if(count_da_macc!=0 && count_da_macc<=ANTICIPO_RICH_INGRESSO_MAC){
        numero_nodi = 1000;
        *metri      = 1000;
        return numero_nodi;
     }
  }
  //---------------------------------------------
  // Conta quanti nodi mancano
  //---------------------------------------------
  DistTot=0;
  i=ptr_path;
  for(i; i<MAXPERCORSI; i++){
   //if(mission.punto[i]==mission.pintend) break;
     if(N.nodo_busy[mission.punto[i]]!=agv_num) break;
     p1 = mission.punto[i];
     if(i<MAXPERCORSI-1) p2 = mission.punto[i+1];
     //--------------------------------------------------------------
     // GESTIONE PRECEDENZE PER TGV IN RETTILINEO
     // interrompo il conteggio dei nodi se incontro una navetta
     // lanciata in rettilineo
     //--------------------------------------------------------------
   //if(N.nodo_con_tgv_lanciato[p1]!=0 && N.nodo_con_tgv_lanciato[p1]!=agv_num) break; 
     //--------------------------------------------------------------
     // Calcolo dei metri
     //--------------------------------------------------------------
     x1 = Punti[p1][0];
     y1 = Punti[p1][1];
     x2 = Punti[p2][0];
     y2 = Punti[p2][1];
     //--------------------------------------------------------------
     // Calcolo delle distanze da punto a punto
     //--------------------------------------------------------------
     DistX = abs(y1-y2);
     DistY = abs(x1-x2);
     if(DistX>30000 || DistY>30000) break;
     Dist  = (double)(sqrt_algo((DistX*DistX)+(DistY*DistY)));
     //--------------------------------------------------------------
     // Calcolo della distanza totale in cm
     //--------------------------------------------------------------
     DistTot = DistTot + Dist;
     p1 = p2;
     //--------------------------------------------------------------
     // Incremento indice nodi occupati
     //--------------------------------------------------------------
     numero_nodi++;
  }
  *metri = (short int)(DistTot/100);
  return numero_nodi-1;
}


// ----------------------------------------------
//       history_allarmi()
// ----------------------------------------------
//  Stampa su file cronologico eventuali allarmi AGV
//
int agv::history_allarmi()
/************************/
{
  int  err;
  char filename[101]="";
  struct ALLTGV Appoggio;
  unsigned short int i;
  short int variato;
  short int TotAllarmi;
  WORD dm1000;
  WORD dm4999;
  WORD dm[MAXLRALLARMI+31];
  WORD dmAllLr[MAXLRALLARMI+MAXAGV+31];
  WORD NumCanali=MAXLRALLARMI;
  WORD NumPlc=PLCTERRA;
  char all_mess[201];
  char buffer[2011]="";
  char buffer1[11]="";
  char data[11]="";
  char time[11]="";
  char messaggio[201]="";
  bool AllarmiTgvVariati;
  static bool TgvInAllarme[MAXAGV+1];
  static bool OldManuale[MAXAGV+1];

  err=0;
  variato=0;
  TotAllarmi=0;
  AllarmiTgvVariati=false;
  memset(&Appoggio, 0, sizeof(ALLTGV));
  //-------------------------------------------------------------
  // Azzera allarme interno se START > 0
  //
  // N.B. "allarme_interno==7 e "allarme_interno==8" possono
  //      essere in corso anche su navette in movimento.
  //-------------------------------------------------------------
  if(stato.start==1 && allarme_interno!=ALL_INT_DATI_ANOMALI && allarme_interno!=ALL_INT_ERR_COMUNIC){
     allarme_interno  = 0;
     old_allarme_interno = 0; //18/02/2013 Altrimenti sul Client NON si azzera mai
  }

  //-------------------------------------------------------------
  // 14/09/2011 NESSUN ALLARME INTERNO VIENE ARCHIVIATO
  // Per evitare che venga scritto nel cronologico e per
  // viene rialzato dal ciclo di scansione se l'allarme persiste
  //
  // 26/09/2011 AGGIORNAMENTO ALLARME INTERNO PER CLIENT
  //
  // 13/02/2011 RIPRISTINO ARCHIVIAZIONE ALLARMI INTERNI (SPAL)
  //-------------------------------------------------------------
//old_allarme_interno = allarme_interno;
//if(allarme_interno!=0){
//   allarme_interno = 0;
//}

  //-------------------------------------------------------------
  // Verifica presenza ALLARMI
  //-------------------------------------------------------------
  if(break_Com[COM1] || mission.EsclPLC || mission.Esclusione) return 0;

  //-------------------------------------------------------------
  // Se il TGV non è in allarme
  //-------------------------------------------------------------
  if(!stato.s.bit.allarme && !allarme_interno && !stato.s.bit.ris) TgvInAllarme[agv_num]=false;
  //-------------------------------------------------------------
  // Se il TGV in automatico
  //-------------------------------------------------------------
  if(stato.s.bit.funz) OldManuale[agv_num]=false;
  //-------------------------------------------------------------
  // Se non ho nessun tipo di allarme --> Skipp!!!
  //-------------------------------------------------------------
  if(!stato.s.bit.allarme && !allarme_interno && stato.s.bit.funz && !stato.s.bit.ris){
     memset(&old_allarm[0],      0, sizeof(old_allarm));
     memset(&allarme_esterno[0], 0, sizeof(allarme_esterno));
     return 0;
  }
  //-------------------------------------------------------------
  // Verifico quanti canali leggere in base al valore della
  // variabile "CanaleCronoAllarmiTGV" per assicurare la lettura
  // sia degli LR6,7,8 (allarmi attuali) che LR16,17,18
  //
  // DM1000 - Lettura versione POLLICINO
  //-------------------------------------------------------------
  if(CanaleCronoAllarmiTGV!=6) NumCanali = (WORD)(NumCanali + (CanaleCronoAllarmiTGV-6));
  if(NumCanali>25            ) NumCanali = 25;
  //-------------------------------------------------------------
  // Normalmente gli allarmi da archiviare vengono letti negli
  // LR 6, 7, 8 ma su richiesta di TecnoFerrari vengono letti gli
  // LR16,17,18.
  //-------------------------------------------------------------
  memset(&dm[0], 0, sizeof(dm));
  if( stato.s.bit.allarme || stato.s.bit.ris ){
     if(OM.ld_canale(&dm[0], (WORD)(agv_num), "RL", 6, NumCanali, all_mess)) return 1;
     //-------------------------------------------------------------
     // 07/10/2010
     // Lettura DM1000 di Release Pollicino
     //-------------------------------------------------------------
     dm1000=0;
     if(OM.ld_canale(&dm1000, (WORD)(agv_num), "RD", 1000, 1, all_mess)) return 1;
     //-------------------------------------------------------------
     // Aggiorno la WORD globale con il valore del D1000 solo se
     // la lettura è andata a buon fine
     //-------------------------------------------------------------
     dm1000_versione_allarmi = dm1000;
  }
  //-------------------------------------------------------------
  // Memorizzo il valore degli LR di allarme.
  //-------------------------------------------------------------
  for(i=0; i<MAXLRALLARMI; i++){
     allarme_esterno[i] = dm[i];         // LR6
  }
  //-------------------------------------------------------------
  // Verifica se allarmi VARIATI
  // puntamented ai canali di "CanaleCronoAllarmiTGV"
  //-------------------------------------------------------------
  for(i=0;i<MAXLRALLARMI;i++){
     if(dm[i+(CanaleCronoAllarmiTGV-6)]!=0 && dm[i+(CanaleCronoAllarmiTGV-6)]!=old_allarm[i]){variato=1; AllarmiTgvVariati=true;}
  }
  if(allarme_interno!=old_allarme_interno) variato=1;
  //-------------------------------------------------------------
  // Se il TGV è in manuale non segnalo le variazioni di errore.
  //-------------------------------------------------------------
  if(!stato.s.bit.funz && OldManuale[agv_num]==true) variato=0;
  if(!stato.s.bit.funz && variato                  ) OldManuale[agv_num]=true;
  //-------------------------------------------------------------
  // Archiviazione degli ALLARMI
  //-------------------------------------------------------------
  if(variato){
     //-------------------------------------------------------------
     // RESET D4999
     // Auto-reset per allarme interno sparito
     //-------------------------------------------------------------
     if(allarme_interno==0){
        dm4999=0;
        if(OM.wr_canale(&dm4999, (WORD)(agv_num), "WD", 4999, 0, all_mess)) return 1;
     }
     //-------------------------------------------------------------
     // Recupero data e ora dell'allarme
     //-------------------------------------------------------------
     OraData(time, data);
     //-------------------------------------------------------------
     // 07/10/2010
     // Rilettura del file allarmi in base alla versione della
     // Pollicino
     //-------------------------------------------------------------
     err = ReloadALLARMI(all_mess);
     if(err) return 1;
     //-------------------------------------------------------------
     // Aggiornamento cronologico allarmi
     //-------------------------------------------------------------
     for( i=0; i<(16*MAXLRALLARMI); i++){
        if( TestBit((char *)&dm[0+(CanaleCronoAllarmiTGV-6)], i) && (!TestBit((char *) old_allarm, i))){
           Copy(ALLARM[i], 1, 101, buffer);
           //---------------------------------------------------
           // Verifica il bit dell'allarme
           //---------------------------------------------------
           Copy(ALLARM[i], 5,   5, buffer1);
           if(strcmp(buffer1, "     ")==0) continue;
           //---------------------------------------------------
           // Memorizzazione del messaggio
           //---------------------------------------------------
           sprintf(messaggio, "  %03d   %04d   %s", agv_num, stato.pos, buffer);
           //---------------------------------------------------
           // "BACKALL.DAT" Cronologico allarmi per backup
           //---------------------------------------------------
           err = file_cronologico_generico("BACKALL.DAT", messaggio, 30000L);
           if(err){
              sprintf(all_mess, MESS[113], "BACKALL.DAT");
              P.error_trasmit(agv_num, all_mess);
           }
           //---------------------------------------------------
           // Alzo un bit che mi ricordo che il Tgv è in allarme
           // e salvo allarmi in un file di appoggio
           //---------------------------------------------------
           mission.TimeRipristino=1;
           strcpy(Appoggio.Time, time);
           strcpy(Appoggio.Date, data);
           Appoggio.Posizione = stato.pos;
           Appoggio.NumAgv    = agv_num;
           strcpy(Appoggio.Allarme, buffer);
           err = SaveAllTgv(agv_num, &Appoggio, all_mess);
           if(err){
              sprintf(all_mess, MESS[113], filename);
              P.error_trasmit(agv_num, all_mess);
              return err;
           }
           TotAllarmi= (short int)(TotAllarmi+1);
           if(TotAllarmi==9) break;
        }
     }
     for( i=0; i<MAXLRALLARMI; i++){
        old_allarm[i] = dm[i+(CanaleCronoAllarmiTGV-6)];
     }
     //-------------------------------------------------------------
     // Stampa Messaggio BOX caricato/scaricato manualmente
     //-------------------------------------------------------------
     if( old_allarme_interno != allarme_interno && allarme_interno!=0){
        if( allarme_interno == ALL_INT_RESET_DATI          ) strcpy(buffer, MESS[360]);
        if( allarme_interno == ALL_INT_SCAR_MANUALE        ) strcpy(buffer, MESS[361]);
        if( allarme_interno == ALL_INT_POS_NOT_OK          ) strcpy(buffer, MESS[362]);
        if( allarme_interno == ALL_INT_STOP_TGV            ) sprintf(buffer, "stop TGV! view in menù: (%s)", MESS[241]);
        if( allarme_interno == ALL_INT_DATI_ANOMALI        ) sprintf(buffer, "error in DM90, DM91, DM92! view in menù: (%s)", MESS[241]);
        if( allarme_interno == ALL_INT_ERR_COMUNIC         ) sprintf(buffer, "persistent comunication error! view in menù: (%s)", MESS[241]);
        if( allarme_interno == ALL_INT_RICARICA_OCC        ) strcpy(buffer, MESS[340]);
        if( allarme_interno == ALL_INT_START_DA_OCC        ) strcpy(buffer, MESS[379]);
        if( allarme_interno == ALL_INT_ID_NOT_OK           ) strcpy(buffer,  "Id not ok!");
        if( allarme_interno == ALL_INT_DEST_IMPEGNATA      ) strcpy(buffer, MESS[639]);
        if( allarme_interno == ALL_INT_DATI_BOX_NON_VALIDI ) strcpy(buffer, "Box Data not valid");
        if( allarme_interno == ALL_INT_UDC_DUPLICATO       ) strcpy(buffer, "Duplicated UDC/SSCC");
        if( allarme_interno == ALL_INT_DEST_NON_VALIDA     ) strcpy(buffer, "Destination not valid");
        sprintf(messaggio, "  %03d   %03d   %s", agv_num, stato.pos, buffer);
        //---------------------------------------------------
        // "BACKALL.DAT" Cronologico allarmi per backup
        //---------------------------------------------------
        err = file_cronologico_generico("BACKALL.DAT", messaggio, 30000L);
        if(err){
           sprintf(all_mess, MESS[113], filename);
           P.error_trasmit(agv_num, all_mess);
        }
        //---------------------------------------------------
        // Alzo un bit che mi ricordo che il Tgv è in allarme
        // e salvo allarmi in un file di appoggio
        //---------------------------------------------------
        mission.TimeRipristino=1;
        strcpy(Appoggio.Time, time);
        strcpy(Appoggio.Date, data);
        Appoggio.Posizione = stato.pos;
        Appoggio.NumAgv    = agv_num;
        strcpy(Appoggio.Allarme, buffer);
        err = SaveAllTgv(agv_num, &Appoggio, all_mess);
        if(err){
           sprintf(all_mess, MESS[113], filename);
           P.error_trasmit(agv_num, all_mess);
           return err;
        }
     }
     //-------------------------------------------------------------
     // Memorizzo la vecchia situazione degli allarmi interni
     //-------------------------------------------------------------
     old_allarme_interno=allarme_interno;
  }
  //-------------------------------------------------------------
  // Se sono variati gli allarmi del Tgv li segnalo sul PLCTERRA
  //-------------------------------------------------------------
  if(AllarmiTgvVariati == true && break_Com[COM2]==false && (ComunicazioneDirettaPLC==0 || NumPlc!=PLCTERRA)){
     for(i=0; i<MAXLRALLARMI; i++){
        dmAllLr[i] = allarme_esterno[i];
     }
     dmAllLr[MAXLRALLARMI] = stato.pos;
     if(OM.wr_canale(&dmAllLr[0], 0, "WD", (WORD)(350+((agv_num-1)*(MAXLRALLARMI+1))), (WORD)(MAXLRALLARMI+1), all_mess)) return 1;
  }
  return 0;
}

// ----------------------------------------------
//       ReloadALLARMI()
// ----------------------------------------------
//  Rilettura file allarmi TGV in base alla versione della
//  scheda POLLICINO
//
int agv::ReloadALLARMI(char *all_mess)
/************************/
{
  int  err;
  char file_all1[31]="";
  char file_all2[31]="";
  short int pollicino;

  //-----------------------------------------
  // Selezione nome file in base alla lingua
  // e alla versione della SCHEDA POLLICINO
  //-----------------------------------------
  err=0;
  pollicino=dm1000_versione_allarmi;
  strcpy(file_all1, "ALLARM.ITA");
  strcpy(file_all2, "ALLARM.ITA");

  if(pollicino!=0) sprintf(file_all1, "ALLARM%d.%s", pollicino, LinguaSistema);
  else             sprintf(file_all1, "ALLARM.%s"             , LinguaSistema);
  strcpy(file_all2, "ALLARM.ITA");
  //-----------------------------------------
  // ALLARM[]
  //-----------------------------------------
  err = load_ALLARM(file_all1);
  if(err){
     err = load_ALLARM(file_all2);
     if(err) sprintf(all_mess, MESS[112], file_all1);
  }
  return err;
}


// ---------------------------
//  CompilaPercorsoGrafico()
// ---------------------------
// Compila il file PERCTGV.DAT con le informazioni di percorso
// del TGV residenti in RAM o scaricate su PC.
//
int agv::CompilaPercorsoGrafico( char *Intestazione )
/******************************************/
{
  FILE   *ptrfile;
  char   stringa[101]="";
  short  int i=0, Nodo=0;
  char   filename[21]="PERCTGV.DAT";

  //-----------------------------------------------------
  // Carica nel buffer dell'agv il programma letto da PLC
  //-----------------------------------------------------
  sprintf(Intestazione, MESS[349],
                                   AGV[agv_num]->mission.pstart,
                                   AGV[agv_num]->mission.pend,
                                   AGV[agv_num]->mission.pintstart,
                                   AGV[agv_num]->mission.pintend);

  //---------------------------------------
  // Crea il file di visualizzazione
  //---------------------------------------
  ptrfile = fopen(filename, "wb");
  if (ptrfile==0) return 1;

  //---------------------------------------
  // Leggo tutti gli Step di programma
  //---------------------------------------
  for(i=0; i<MAXPERCORSI; i++){
     Nodo = AGV[agv_num]->mission.punto[i];
     if(Nodo==0) break;
     if(AGV[agv_num]->mission.punto[i+1]!=0){
        sprintf(stringa, "%03d -> node %4d (ps: %4d, ind: %d, rot: %d, ral: %d, dist: %d)", i, Nodo, AGV[agv_num]->mission.dati_perc[i].ps, AGV[agv_num]->mission.dati_perc[i].dir, AGV[agv_num]->mission.dati_perc[i].rot, AGV[agv_num]->mission.dati_perc[i].ral, AGV[agv_num]->mission.dati_perc[i].dist);
     }
     else{
        sprintf(stringa, "%03d -> node %4d", i, Nodo, AGV[agv_num]->mission.dati_perc[i].ps, AGV[agv_num]->mission.dati_perc[i].dir, AGV[agv_num]->mission.dati_perc[i].rot, AGV[agv_num]->mission.dati_perc[i].ral, AGV[agv_num]->mission.dati_perc[i].dist);
     }
     Write_Line(ptrfile, stringa);
     Write_Line(ptrfile, "--------------------------------------------------------------------------------");
  }
  fclose(ptrfile);
  return 0;
}

// ---------------------------
//  CompilaProgrammaGrafico()
// ---------------------------
// Compila il file PROGTGV.DAT con le informazioni di percorso
// del TGV residenti in RAM o scaricate su PC.
//
int agv::CompilaProgrammaGrafico( bool Plc )
/******************************************/
{
  int    err=0;
  FILE   *ptrfile;
  short int i, j, cmd_curva;
  char   stringa[101]="";
  char   buffer[201]="";
  char   filename[21]="PROGTGV.DAT";
  struct comandi *cbit;
  bool   distanza_grd;
  bool   CmdDistanza, CmdPrecDistanza;
  Canale Ch;

  //-----------------------------------------------------
  // GESTIONE_ASRV
  //-----------------------------------------------------
  #ifdef GESTIONE_ASRV
     err = ASRV_compila_programma_grafico(Plc);
     return err;
  #endif;
  //-----------------------------------------------------
  // GESTIONE_TGV
  //-----------------------------------------------------
  cmd_curva=0;
  distanza_grd=false;
  memset(&Ch, false, sizeof(Canale));
  //-----------------------------------------------------
  // Carica nel buffer dell'agv il programma letto da PLC
  //-----------------------------------------------------
  memcpy(AGV[agv_num]->buf_prg, AGV[agv_num]->mission.step, sizeof(AGV[agv_num]->mission.step));
  if(Plc==true){
     err=AGV[agv_num]->load_prg( AGV[agv_num]->buf_prg, buffer);
  }
  if(err){
     sprintf(buffer, "Error in load program on PLC TGV%d...", agv_num);
     P.error_trasmit(agv_num, buffer);
     return 1;
  }

  //---------------------------------------
  // Crea il file di visualizzazione
  //---------------------------------------
  ptrfile = fopen(filename, "wb");
  if (ptrfile==0) return 1;

  //---------------------------------------
  // Leggo tutti gli Step di programma
  //---------------------------------------
  CmdDistanza=false; CmdPrecDistanza=false;
  for(i=0;;i++){
     CmdDistanza=false;
     //-------------------------------------------------
     // cbit = struttura bit di comando (step programma)
     //-------------------------------------------------
     cbit = (struct comandi *) &AGV[agv_num]->buf_prg[i];
     strcpy(buffer, "");
     //------- L'istruzione è un Nodo ?
     // GB-01/04/2015 Ampliamento del range Nodi 4096 --> 8192 (usando anche bit avvicinamento)
     // mascheramento se precedente Step Distanza
     //if(cmd_curva!=0 || ((!cbit->az) && (!cbit->ral) && (!cbit->end) && (!cbit->avvic))){
     if(cmd_curva!=0 || ((!cbit->az) && (!cbit->ral) && (!cbit->end) && (!cbit->avvic || CmdPrecDistanza==true))){
        sprintf(buffer, MESS[149], AGV[agv_num]->buf_prg[i]);
        //---------------------------------------------
        // CURVE DI BEZIER
        // Visualizzazione coordinate vertice
        //---------------------------------------------
        if(cmd_curva==1){
           sprintf(buffer, "X vertice %d", (signed short int)(AGV[agv_num]->buf_prg[i])); cmd_curva++;
        }
        else{
           if(cmd_curva==2 || cmd_curva==3){
              cmd_curva++;
           }
           else{
              if(cmd_curva==4){
                 sprintf(buffer, "Y vertice %d", (signed short int)(AGV[agv_num]->buf_prg[i])); cmd_curva++;
              }
           }
        }
     }

     //------- L'istruzione è una distanza (con indicazioni di Rallentamento) ?
     if(!cbit->az && cbit->ral){
        sprintf(buffer, MESS[148], (AGV[agv_num]->buf_prg[i] & 0x0fff));
        if(distanza_grd==true) sprintf(buffer, "Grd  %03d", (AGV[agv_num]->buf_prg[i] & 0x0fff));
        distanza_grd=false;
        CmdDistanza=true;
     }
     //------- L'istruzione è di Zona di mapping
     if(cbit->az && cbit->ral && !cbit->end && !cbit->avvic){
        sprintf(buffer, MESS[132], (AGV[agv_num]->buf_prg[i] & 0x00ff),(AGV[agv_num]->buf_prg[i] & 0x0f00)>>8 ); // Zona di mapping
        if(distanza_grd==true) distanza_grd=false;
     }
     //------- L'istruzione è di Raggio
     if(cbit->az && cbit->ral && cbit->end && !cbit->avvic){
        sprintf(buffer, MESS[133], (AGV[agv_num]->buf_prg[i] & 0x0fff)); // Raggio della curva
     }
     //--------------------------------------------------------------------------------------------
     // GB-01/04/2015 Ampliamento del range Nodi 4096 --> 8192 (usando anche bit avvicinamento)
     // mascheramento se precedente Step Distanza
     //--------------------------------------------------------------------------------------------
     if(CmdPrecDistanza==false){
        //-------- L'istruzione è un Avvicinamento Magazzino x carico ?
        if(cmd_curva==0 && (cbit->avvic)&&(cbit->grd0)&&(!cbit->ral)){
           sprintf(buffer, MESS[147]);//"Ac.Alm.C."
        }
        //-------- L'istruzione è un Avvicinamento Magazzino x scarico ?
        if(cmd_curva==0 && (cbit->avvic)&&(cbit->av)&&(!cbit->scarico)&&(!cbit->ral)){
           sprintf(buffer, MESS[146]);//"Ac.Alm.S."
        }
        //-------- L'istruzione è un Avvicinamento Macchina x carico ?
        if(cmd_curva==0 && (cbit->avvic)&&(cbit->vel)&&(!cbit->ral)){
           sprintf(buffer, MESS[145]);//"Ac.Maq.C."
        }
        //-------- L'istruzione è un Avvicinamento Macchina x scarico ?
        if(cmd_curva==0 && (cbit->avvic)&&(cbit->av)&&(cbit->scarico)&&(!cbit->ral)){
           sprintf(buffer, MESS[153]);//"Ac.Alm.S."
        }
        //----- L'istruzione è un Ritorno Ingresso Magazzino dopo carico-scarico?
        if(cmd_curva==0 && (cbit->avvic)&&(cbit->grd90)&&(!cbit->ral)){
           sprintf(buffer, MESS[144]);//"Ret.entr."
        }
     }
     //-------- L'istruzione è una Rotazione 0ø ?
     if((cbit->az)&&(!cbit->av)&&(!cbit->ind)&&(cbit->grd0)&&(!cbit->ral)){
        sprintf(buffer, MESS[143]);//"Rot 0ø   "
        if(cbit->antiora) strcat( buffer, "+anti-oraria" );
        else              strcat( buffer, "+oraria" );
        if(cbit->curva  ) strcat( buffer, "+rac." );          // RACCORDO FONTANA
        if(GrdMinPerAttivaDeviazioni<360) distanza_grd=true;
     }
     //-------- L'istruzione è una Rotazione 90ø ?
     if((cbit->az)&&(!cbit->av)&&(!cbit->ind)&&(cbit->grd90)&&(!cbit->ral)){
        sprintf(buffer, MESS[142]);//"Rot 90ø  "
        if(cbit->antiora) strcat( buffer, "+anti-oraria" );
        else              strcat( buffer, "+oraria" );
        if(cbit->curva  ) strcat( buffer, "+rac." );          // RACCORDO FONTANA
        if(GrdMinPerAttivaDeviazioni<360) distanza_grd=true;
     }
     //-------- L'istruzione è una Rotazione 180ø?
     if((cbit->az)&&(!cbit->av)&&(!cbit->ind)&&(cbit->grd180)&&(!cbit->ral)){
        sprintf(buffer, MESS[141]);
        if(cbit->antiora) strcat( buffer, "+anti-oraria" );
        else              strcat( buffer, "+oraria" );
        if(cbit->curva  ) strcat( buffer, "+rac." );          // RACCORDO FONTANA
        if(GrdMinPerAttivaDeviazioni<360) distanza_grd=true;
     }
     //-------- L'istruzione è una Rotazione 270ø?
     if((cbit->az)&&(!cbit->av)&&(!cbit->ind)&&(cbit->grd270)&&(!cbit->ral)){
        sprintf(buffer, MESS[140]);
        if(cbit->antiora) strcat( buffer, "+anti-oraria" );
        else              strcat( buffer, "+oraria" );
        if(cbit->curva  ) strcat( buffer, "+rac." );          // RACCORDO FONTANA
        if(GrdMinPerAttivaDeviazioni<360) distanza_grd=true;
     }
     //------- L'istruzione è un Carico ?
     if((cbit->az)&&(!cbit->curva)&&(cbit->carico)&&(!cbit->scarico)&&(!cbit->ral)){
        sprintf(buffer, MESS[139]);//"Carga   "
     }
     //------- L'istruzione è uno Scarico ?
     if((cbit->az)&&(!cbit->curva)&&(!cbit->carico)&&(cbit->scarico)&&(!cbit->ral)){
        sprintf(buffer, MESS[138]);//"Descarga"
     }
     //------- L'istruzione è uno Carico/Scarico ?
     if((cbit->az)&&(!cbit->curva)&&(cbit->carico)&&(cbit->scarico)&&(!cbit->ral)){
        sprintf(buffer, MESS[134]);//"Descarga"
     }
     //---------------------------------------------
     // GESTIONE CURVE E DIREZIONE LASER
     //---------------------------------------------
     if(AGV[agv_num]->mission.GestioneLaser==true){
        //------- L'istruzione è un Avanti ?
        if((cbit->az)&&(!cbit->curva)&&(cbit->av)&&(!cbit->ral)){
           sprintf(buffer, MESS[137]);//"Adelante"
        }
        //------- L'istruzione è un Indietro ?
        if((cbit->az)&&(!cbit->curva)&&(cbit->ind)&&(!cbit->ral)){
           sprintf(buffer, MESS[136]);//"Atr s   "
        }
        //-------- Curva Anti-Oraria
        if((cbit->az)&&(cbit->curva)&&(cbit->antiora)&&(!cbit->ral)){
           sprintf(buffer, MESS[131]);
           //------------------------------------------------
           // Memorizzo che si tratta di una curva di Bezier
           //------------------------------------------------
           if(CurveDiBezier!=0){
              cmd_curva=1;
              sprintf(buffer, "BEZIER CUBICA");
           }
        }
        //-------- Curva Oraria
        if((cbit->az)&&(cbit->curva)&&(!cbit->antiora)&&(!cbit->ral)){
           sprintf(buffer, MESS[130]);
           //------------------------------------------------
           // Memorizzo che si tratta di una curva di Bezier
           //------------------------------------------------
           if(CurveDiBezier!=0){
              cmd_curva=1;
              sprintf(buffer, "BEZIER QUADRATICA");
           }
        }
     }
     //---------------------------------------------
     // GESTIONE DIREZIONE RETTA/INCLINATE MAGNETI
     //---------------------------------------------
     else{
        //------- L'istruzione è un Avanti ?
        if((cbit->az)&&(cbit->av)&&(!cbit->ral)){
           sprintf(buffer, MESS[137]);//"Adelante"
           if(GrdMinPerAttivaDeviazioni<360 && !cbit->curva) strcat(buffer, " INCLINATA");
        }
        //------- L'istruzione è un Indietro ?
        if((cbit->az)&&(cbit->ind)&&(!cbit->ral)){
           sprintf(buffer, MESS[136]);//"Adelante"
           if(GrdMinPerAttivaDeviazioni<360 && !cbit->curva) strcat(buffer, " INCLINATA");
        }
     }

     //------- L'istruzione è Fine Programma ?
   //if((cbit->end)&&(!cbit->curva)&&(!cbit->ral)){
     if((cbit->end)&&(!cbit->grd0)&&(!cbit->grd90)&&(!cbit->vel)&&(!cbit->av)&&(!cbit->ind)&&(!cbit->carico)&&(!cbit->grd180)&&(!cbit->grd270)&&(!cbit->forchealte)&&(!cbit->curva)&&(!cbit->antiora)&&(!cbit->avvic)&&(!cbit->ral)&&(!cbit->az)){
        sprintf(buffer, MESS[135]);            //"Fin   "
     }
     //---------------------------------------------------
     // stampa i bit 0 e 1 sulla linea di visualizzazione
     //---------------------------------------------------
     IntCanale(AGV[agv_num]->buf_prg[i], &Ch);
     for(j=0; j<=15; j++){
        if(!strcmp(buffer, "") && Ch.Bit[j]==true) strcpy(buffer, "??????????");
     }

     Write_Line(ptrfile, "                                               |A|B|C|D|E|F|G|H|I|L|M|N|O|P|Q|R|");
     //------------------------------------
     // Compilazione PASSO e INTESTAZIONE
     //------------------------------------
     sprintf(stringa, "  %03d  %35s     |", i, buffer);
     //------------------------------------
     // Compilazione CODIFICA
     //------------------------------------
     for(j=15; j>=0; j--){
        if(Ch.Bit[j]==true) strcat(stringa, "*|");
        else                strcat(stringa, " |");
     }
     Write_Line(ptrfile, stringa);

     // --- finito il ciclo per le CURVE DI BEZIER resetto il contatore fino al prossimo
     if(cmd_curva>4){
        cmd_curva=0;
     }
     //--- Interrompo se la codifica e NULLA ---
     if(AGV[agv_num]->buf_prg[i]==0 || i>MAXSTEP-1) break;
     //----------------------------------------------
     // Memorizzo Comando Precedente distanza
     //----------------------------------------------
     CmdPrecDistanza=CmdDistanza;
  }
  fclose(ptrfile);
  return 0;
}


// ---------------------------
//  CompilaAllarmiAttuali()
// ---------------------------
// Compila il file ALLARM*.DAT con gli Allarmi attuali
//
void agv::CompilaAllarmiAttuali()
/******************************************/
{
  FILE  *ptrfile;
  char  buffer[201]="";
  char  buffer1[11]="";
  char  stringa[101]="";
  short int i=0, n=0, j=0, variato=0;
  char  filename[81]="";
  WORD  dm[MAXLRALLARMI+11];
  WORD  old_dm[MAXLRALLARMI+11];
  int   err;

  //-----------------------------------------------------
  // Compilazione Nome del file di allarm*.dat
  //-----------------------------------------------------
  sprintf(filename, "ALLTGV%02d.DAT", agv_num);
  //---------------------------------------
  // Crea il file di visualizzazione
  //---------------------------------------
  ptrfile = fopen(filename, "wb");
  if(ptrfile==0){
     sprintf(buffer, MESS[113], filename);
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  err=0;
  memset(&dm, 0, sizeof( dm ));
  memset(&old_dm, 0, sizeof( old_dm ));
  //--------------------------------------------
  // GESTIONE_ASRV
  // Lettura allarmi da struttura
  //--------------------------------------------
  #ifdef GESTIONE_ASRV
     for( i=0;i<MAXLRALLARMI;i++){
        dm[i] = allarme_esterno[i];
     }
  #else
     // --- SERVER ---
     if( IdPC<2 ){
        if(!break_Com[COM1] && !mission.EsclPLC){
           err= OM.ld_canale(&dm[0], (WORD)(agv_num), "RL", 6, MAXLRALLARMI, buffer);
        }
     }
     // --- CLIENT ---
     if( IdPC>1 ) {
        for( i=0;i<MAXLRALLARMI;i++){
           dm[i] = allarme_esterno[i];
        }
     }
  #endif;

  if( err==0 ){
     n=0;
     variato=0;
     for( i=0;i<MAXLRALLARMI;i++){
        if( dm[i] != old_dm[i] ) variato = 1;
        if( allarme_interno    ) variato = 1;
     }
     for( i=0;i<MAXLRALLARMI;i++){
        old_dm[i]=dm[i];
     }
     if( variato == 1 ){
        //---------------------------------
        // Visualizzazione allarmi presenti
        //---------------------------------
        for( i=0;i<MAXLRALLARMI;i++){
           for( j=0;j<16;j++){
              if(((dm[i]>>j)&1) == 1 ){
                 strcpy(stringa, ALLARM[n]);
                 //---------------------------------------------------
                 // Verifica il bit dell'allarme
                 //---------------------------------------------------
                 Copy(ALLARM[n], 5,   5, buffer1);
                 if(strcmp(buffer1, "     ")==0) continue;
                 //---------------------------------------------------
                 // Registrazione della stringa
                 //---------------------------------------------------
                 Write_Line(ptrfile, stringa);
//               Write_Line(ptrfile, "--------------------------------------------------------------------------------");
              }
              n++;
           }
        }
        //-----------------------------------------------------
        // Stampa Messaggio allarme interno es: BOX caricato/
        // scaricato manualmente ..
        //-----------------------------------------------------
        if( allarme_interno == ALL_INT_RESET_DATI          ) strcpy(stringa, MESS[360]);
        if( allarme_interno == ALL_INT_SCAR_MANUALE        ) strcpy(stringa, MESS[361]);
        if( allarme_interno == ALL_INT_POS_NOT_OK          ) strcpy(stringa, MESS[362]);
        if( allarme_interno == ALL_INT_STOP_TGV            ) sprintf(stringa, "stop TGV! view in menù: (%s)", MESS[42]);
        if( allarme_interno == ALL_INT_DATI_ANOMALI        ) sprintf(stringa, "error in DM90, DM91, DM92! view in menù: (%s)", MESS[42]);
        if( allarme_interno == ALL_INT_ERR_COMUNIC         ) sprintf(stringa, "persistent comunication error! view in menù: (%s)", MESS[42]);
        if( allarme_interno == ALL_INT_RICARICA_OCC        ) strcpy(stringa, MESS[340]);
        if( allarme_interno == ALL_INT_START_DA_OCC        ) strcpy(stringa, MESS[379]);
        if( allarme_interno == ALL_INT_ID_NOT_OK           ) strcpy(stringa, "ID not OK");
        if( allarme_interno == ALL_INT_DEST_IMPEGNATA      ) strcpy(stringa, MESS[639]);
        if( allarme_interno == ALL_INT_DATI_BOX_NON_VALIDI ) strcpy(stringa, "Box Data not valid");
        if( allarme_interno == ALL_INT_UDC_DUPLICATO       ) strcpy(stringa, "Duplicated UDC/SSCC");
        if( allarme_interno == ALL_INT_DEST_NON_VALIDA     ) strcpy(stringa, "Destination not valid");
        if( allarme_interno ){
           Write_Line(ptrfile, stringa);
//         Write_Line(ptrfile, "--------------------------------------------------------------------------------");
        }
     }
  }
  else{
     sprintf(buffer, MESS[152], agv_num);
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  fclose( ptrfile );
  return;
}
// ------------------------------------------------------------------------
// ------------------------
//    SaveAllTgv()
// ------------------------
// Salva un archivio di BOX depositati sulla Macchina
//
int agv::SaveAllTgv(short int NumTgv, struct ALLTGV *Dati, char *all_mess)
/**********************************/
{
  FILE *ptrfile;
  int  i, err = 0;
  char filename[90]="";
  struct ALLTGV Appoggio;

  strcpy( all_mess, "" );
  sprintf( filename, "TEMALL%d.DAT", NumTgv);
  //----------------------------------------
  // Passaggio da struttura ESTERNA a LOCALE
  //----------------------------------------
  memcpy(&Appoggio, Dati, sizeof(ALLTGV));
  //----------------------------------------
  // Apertura del file in scittura
  //----------------------------------------
  ptrfile = fopen( filename, "ab");
  if (ptrfile==0) {
     sprintf(all_mess, MESS[113], filename );
     return 1;
  }
  i= fwrite(&Appoggio, 1, sizeof(ALLTGV), ptrfile);
  fclose(ptrfile);

  if (i != sizeof(Appoggio))  err = 1;
  if(err) sprintf(all_mess, MESS[113], filename );
  return err;

}
// -----------------------------------------------------------------------

