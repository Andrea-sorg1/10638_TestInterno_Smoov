// ------------------------------------------------------------------------
//                              file ASRV.CPP
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
#include <dir.h>

#include "Tecnofer.h"
#include "TcAdsDEF.h"
#include "TcAdsAPI.h"

// -----------------------------
//     STRUTTURE GRAFICHE
//      da GRAPHVAR.CPP
// -----------------------------
extern int Punti[][5];
extern int Percorso[][2];

// -----------------------------
//   ASRV_load_file_stato()
// -----------------------------
// load Stato da file
//
int agv::ASRV_load_plc_stato(WORD *dm, char *all_mess)
/***********************************/
{
  AmsAddr  Addr;
  PAmsAddr pAddr = &Addr;
  AnsiString Buff;
  int i, pos, err;
  unsigned char NetId1, NetId2;
  unsigned char BeckhoffIP1[4];
  long handle;
  WORD Port;
  int  Tipo;
  unsigned long lHdlVar;
  char szVarSta[]={"Var_Globali.sTxSta"};

  err=0;
  memset(&BeckhoffIP1[0], NULL, sizeof(BeckhoffIP1));
  //---------------------------------------------
  // Recupero i parametri di comunicazione:
  // 1 - Tipo di comunicazione  (HOST/FINS)
  // 2 - Porta di comunicazione (se Host)
  // 3 - Indice del Plc
  //---------------------------------------------
  Tipo   = OmrPlc[agv_num].TypCom;
  Port   = OmrPlc[agv_num].RemotePort;
  NetId1 = OmrPlc[agv_num].DNA;
  NetId2 = OmrPlc[agv_num].DA2;
  //-------------------------------------
  // Comunicazione disattivata
  //-------------------------------------
  if(break_Com[COM1]==true) return 0;
  //-------------------------------------
  // Solo per comunicazione tipo BECKOFF
  //-------------------------------------
  if(Tipo!=TYPCOM_BECKHOFF) return 0;
  //-------------------------------------
  // Porta Backhoff non valida
  //-------------------------------------
  if(Port!=BECKHOFF_PORT  ) return 0;
  //-------------------------------------
  // Scompattazione indirizzo IP
  //-------------------------------------
  Buff = OmrPlc[agv_num].RemoteHost;
  try{
     pos=i=0;
     do{
        pos = Buff.Pos(".");
        if(pos!=0){
           BeckhoffIP1[i]=StrToInt(Buff.SubString(1, pos-1));
           Buff = Buff.SubString(pos+1, Buff.Length());
           i++;
        }
        else{
           BeckhoffIP1[3]=StrToInt(Buff);
        }
     }while(pos);
  }
  catch(...){
     sprintf(all_mess, "(Stato_ASRV%02d) Error in IP string format PLC BECKHOFF", agv_num);
     return 1;
  }

  //###
  //---------------------------
  // Verifica Ping con PLC ASRV
  //---------------------------
  int   MsecRisp;
  int   err_ping=0;
  err_ping = AlgoPing( OmrPlc[ agv_num ].RemoteHost.c_str(), 500, &MsecRisp, all_mess );
  if(err_ping){
     sprintf(all_mess, "(Stato_ASRV%02d) PLC BECKHOFF non raggiungibile", agv_num );
     errore_comunicazione[COM1][agv_num]=1;
     return err_ping;
  }
  //###

  //--------------------------------------------------------
  // Parametrizzazione comunicazione Beckoff
  //--------------------------------------------------------
  pAddr->port = Port;
  pAddr->netId.b[0]=BeckhoffIP1[0];
  pAddr->netId.b[1]=BeckhoffIP1[1];
  pAddr->netId.b[2]=BeckhoffIP1[2];
  pAddr->netId.b[3]=BeckhoffIP1[3];
  pAddr->netId.b[4]=NetId1;
  pAddr->netId.b[5]=NetId2;
  AdsSyncSetTimeout(TimeOutSocketTCP);
  //--------------------------------------------------------
  // Apertura porta di comunicazione
  //--------------------------------------------------------
  AdsPortOpen();
  //--------------------------------------------------------
  // Leggo Stato ASRV <- Plc BECKHOFF
  //--------------------------------------------------------
  err = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, sizeof(szVarSta), szVarSta);
  if(!err) err = AdsSyncReadReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(ASRV_STATO), &asrv_stato);
  //--------------------------------------------------------
  // associazione strutture beckoff ASRV a dm omron TGV
  // dm[ 0] = D88 - stop tgv
  // dm[ 1] = D89 - reset attesa agv
  // dm[ 2] = D90 - posizione tgv
  // dm[ 3] = D91 - start TGV
  // dm[ 4] = D92 - stato1 AGV
  // dm[ 5] = D93 - offset da nodo
  // dm[ 6] = D94 - stato2 AGV
  // dm[ 7] = D95 - batteria
  // dm[ 8] = D96 - peso
  // dm[ 9] = D97 - gradi rotazione
  // dm[10] = D98 - reset missione da PC
  // dm[11] = D99 - puntatore passi programma
  //--------------------------------------------------------
  if(!err){
     dm[ 0] = asrv_comandi.cbit.stop;
     dm[ 1] = 0;
     dm[ 2] = asrv_stato.pos;
     dm[ 3] = asrv_stato.start;
     dm[ 4] = 0;
     dm[ 5] = asrv_stato.offset_da_nodo;
     dm[ 6] = 0;
     dm[ 7] = (WORD)((asrv_stato.batteria<<8) + 0);
     dm[ 9] = 0; //sembre visualizzazione a 0°
     dm[ 8] = asrv_stato.peso;
     dm[10] = 0;
     dm[11] = asrv_stato.ptr_step;
     //------------------------------------------
     // ASRV in riserva: percentuale <30%
     //------------------------------------------
     if(asrv_stato.batteria<RISERVA_ASRV){
        SetBit((char *) &dm[4],  9);
     }
     //------------------------------------------
     // ASRV: fermo  = AGV: 0° nessuna direzione
     //------------------------------------------
     if(asrv_stato.sbit1.mov_X==false && asrv_stato.sbit1.mov_Y==false){
        // 0°
        SetBit((char *) &dm[4],  0);
        // nessuna direzione
     }
     //------------------------------------------
     // ASRV: X + Av = AGV: 90° + Ind
     //------------------------------------------
     if(asrv_stato.sbit1.mov_X==true && asrv_stato.sbit1.mov_av==true){
        // 90°
        SetBit((char *) &dm[4],  1);
        // INDIETRO
        SetBit((char *) &dm[4],  3);
        // IN MOVIMENTO
        SetBit((char *) &dm[4], 15);
     }
     //------------------------------------------
     // ASRV: Y + Av = AGV: 0° + Av
     //------------------------------------------
     if(asrv_stato.sbit1.mov_Y==true && asrv_stato.sbit1.mov_av==true){
        // 0°
        SetBit((char *) &dm[4],  0);
        // AVANTI
        SetBit((char *) &dm[4],  2);
        // IN MOVIMENTO
        SetBit((char *) &dm[4], 15);
     }
     //------------------------------------------
     // ASRV: X + Ind = AGV: 90° + Av
     //------------------------------------------
     if(asrv_stato.sbit1.mov_X==true && asrv_stato.sbit1.mov_ind==true){
        // 90°
        SetBit((char *) &dm[4],  1);
        // AVANTI
        SetBit((char *) &dm[4],  2);
        // IN MOVIMENTO
        SetBit((char *) &dm[4], 15);
     }
     //------------------------------------------
     // ASRV: Y + Ind = AGV: 0° + Ind
     //------------------------------------------
     if(asrv_stato.sbit1.mov_Y==true && asrv_stato.sbit1.mov_ind==true){
        // 0°
        SetBit((char *) &dm[4],  0);
        // INDIETRO
        SetBit((char *) &dm[4],  3);
        // IN MOVIMENTO
        SetBit((char *) &dm[4], 15);
     }
     //------------------------------------------
     // Bit STATO
     //------------------------------------------
     if(asrv_stato.sbit1.allarme) SetBit((char *) &dm[4],  4);
     if(asrv_stato.sbit1.ok_prog) SetBit((char *) &dm[4],  5);
     if(asrv_stato.sbit1.in_auto && !asrv_stato.sbit1.in_man) SetBit((char *) &dm[4], 7);
     if(asrv_stato.sbit1.su_nod ) SetBit((char *) &dm[4],  8);
     if(asrv_stato.sbit1.carico ) SetBit((char *) &dm[4], 13);
     //------------------------------------------
     // ALLARMI
     //------------------------------------------
     for(i=0; i<MAXLRALLARMI; i++){
        allarme_esterno[i] = asrv_stato.allarmi[i];
     }
  }
  if(err){
     sprintf(all_mess, "(Stato_ASRV%02d) Error %d read %s PLC BECKHOFF", agv_num, err, szVarSta);
     errore_comunicazione[COM1][agv_num]=1;
  }
  else{
     errore_comunicazione[COM1][agv_num]=0;
  }
  AdsPortClose();
  return err;
}

// -----------------------------
//   ASRV_load_prg_mission()
// -----------------------------
// load MISSION da PLC BECKHOFF ASRV
//
int agv::ASRV_load_prg_mission(struct ASRV_MISSION *asrv_buffmiss, char *all_mess)
/***********************************/
{
  AmsAddr  Addr;
  PAmsAddr pAddr = &Addr;
  AnsiString Buff;
  int i, pos, err;
  unsigned char NetId1, NetId2;
  unsigned char BeckhoffIP1[4];
  long handle;
  WORD Port;
  int  Tipo;
  unsigned long lHdlVar;
  char szVarMis[]={"Var_Globali.sRxMis"};

  err=0;
  memset(&BeckhoffIP1[0], NULL, sizeof(BeckhoffIP1));
  //---------------------------------------------
  // Inizializzo struttua con valore in RAM
  //---------------------------------------------
  memcpy(asrv_buffmiss, &asrv_mission, sizeof(ASRV_MISSION));
  //---------------------------------------------
  // Recupero i parametri di comunicazione:
  // 1 - Tipo di comunicazione  (HOST/FINS)
  // 2 - Porta di comunicazione (se Host)
  // 3 - Indice del Plc
  //---------------------------------------------
  Tipo   = OmrPlc[agv_num].TypCom;
  Port   = OmrPlc[agv_num].RemotePort;
  NetId1 = OmrPlc[agv_num].DNA;
  NetId2 = OmrPlc[agv_num].DA2;
  //-------------------------------------
  // Comunicazione disattivata
  //-------------------------------------
  if(break_Com[COM1]==true) return 0;
  //-------------------------------------
  // Solo per comunicazione tipo BECKOFF
  //-------------------------------------
  if(Tipo!=TYPCOM_BECKHOFF) return 0;
  //-------------------------------------
  // Porta Backhoff non valida
  //-------------------------------------
  if(Port!=BECKHOFF_PORT  ) return 0;
  //-------------------------------------
  // Scompattazione indirizzo IP
  //-------------------------------------
  Buff = OmrPlc[agv_num].RemoteHost;
  try{
     pos=i=0;
     do{
        pos = Buff.Pos(".");
        if(pos!=0){
           BeckhoffIP1[i]=StrToInt(Buff.SubString(1, pos-1));
           Buff = Buff.SubString(pos+1, Buff.Length());
           i++;
        }
        else{
           BeckhoffIP1[3]=StrToInt(Buff);
        }
     }while(pos);
  }
  catch(...){
     sprintf(all_mess, "(Mission_ASRV%02d) Error in IP string format PLC BECKHOFF", agv_num);
     return 1;
  }
  //--------------------------------------------------------
  // Parametrizzazione comunicazione Beckoff
  //--------------------------------------------------------
  pAddr->port = Port;
  pAddr->netId.b[0]=BeckhoffIP1[0];
  pAddr->netId.b[1]=BeckhoffIP1[1];
  pAddr->netId.b[2]=BeckhoffIP1[2];
  pAddr->netId.b[3]=BeckhoffIP1[3];
  pAddr->netId.b[4]=NetId1;
  pAddr->netId.b[5]=NetId2;
  AdsSyncSetTimeout(TimeOutSocketTCP);
  //--------------------------------------------------------
  // Apertura porta di comunicazione
  //--------------------------------------------------------
  AdsPortOpen();
  //--------------------------------------------------------
  // Leggo Stato ASRV <- Plc BECKHOFF
  //--------------------------------------------------------
  err = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, sizeof(szVarMis), szVarMis);
  if(!err) err = AdsSyncReadReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(ASRV_MISSION), asrv_buffmiss);
  if(err ){
     sprintf(all_mess, "(Mission_ASRV%02d) Error %d read %s PLC BECKHOFF", agv_num, err, szVarMis);
     errore_comunicazione[COM1][agv_num]=1;
  }
  else{
     errore_comunicazione[COM1][agv_num]=0;
  }
  AdsPortClose();
  return err;
}

// -----------------------------
//   ASRV_trs_mission()
// -----------------------------
// Trasmissione missione
//
int agv::ASRV_trs_mission(char *all_mess)
/***********************************/
{
  int err=0;
  int pos, i;
  int count_step;
  AnsiString Buff;
  AmsAddr  Addr;
  PAmsAddr pAddr = &Addr;
  unsigned char NetId1, NetId2;
  unsigned char BeckhoffIP1[4];
  long handle;
  WORD Port;
  int  Tipo;
  unsigned long lHdlVar;
  short int p0, ps, t_miss;
  short int rot, ral, dist;
  bool ind, movX, movY, movA, movI;
  char szVarMis[]={"Var_Globali.sRxMis"};

  err=0;
  memset(&BeckhoffIP1[0], NULL, sizeof(BeckhoffIP1));
  //--------------------------------------------------------
  // Recupero i parametri di comunicazione:
  // 1 - Tipo di comunicazione  (HOST/FINS)
  // 2 - Porta di comunicazione (se Host)
  // 3 - Indice del Plc
  //--------------------------------------------------------
  Tipo   = OmrPlc[agv_num].TypCom;
  Port   = OmrPlc[agv_num].RemotePort;
  NetId1 = OmrPlc[agv_num].DNA;
  NetId2 = OmrPlc[agv_num].DA2;
  //--------------------------------------------------------
  // Scompattazione indirizzo IP
  //--------------------------------------------------------
  Buff = OmrPlc[agv_num].RemoteHost;
  try{
     pos=i=0;
     do{
        pos = Buff.Pos(".");
        if(pos!=0){
           BeckhoffIP1[i]=StrToInt(Buff.SubString(1, pos-1));
           Buff = Buff.SubString(pos+1, Buff.Length());
           i++;
        }
        else{
           BeckhoffIP1[3]=StrToInt(Buff);
        }
     }while(pos);
  }
  catch(...){
     sprintf(all_mess, "(Mission_ASRV%02d) Error in IP string format PLC BECKHOFF", agv_num);
     return 1;
  }

  //###
  //---------------------------
  // Verifica Ping con PLC ASRV
  //---------------------------
  int   MsecRisp;
  int   err_ping=0;
  err_ping = AlgoPing( OmrPlc[ agv_num ].RemoteHost.c_str(), 500, &MsecRisp, all_mess );
  if(err_ping){
     sprintf(all_mess, "(Mission_ASRV%02d) PLC BECKHOFF non raggiungibile", agv_num );
     errore_comunicazione[COM1][agv_num]=1;
     return err_ping;
  }
  //###

  //--------------------------------------------------------
  // Parametrizzazione comunicazione Beckoff
  //--------------------------------------------------------
  pAddr->port = Port;
  pAddr->netId.b[0]=BeckhoffIP1[0];
  pAddr->netId.b[1]=BeckhoffIP1[1];
  pAddr->netId.b[2]=BeckhoffIP1[2];
  pAddr->netId.b[3]=BeckhoffIP1[3];
  pAddr->netId.b[4]=NetId1;
  pAddr->netId.b[5]=NetId2;
  AdsSyncSetTimeout(TimeOutSocketTCP);
  //--------------------------------------------------------
  // COMPILAZIONE MISSIONE stat==0
  //--------------------------------------------------------
  if(stato.start==0 && stato.s.bit.okprog==true){
     //-----------------------------------
     // Passi programma
     //-----------------------------------
     count_step=1;
     memset(&asrv_mission, 0, sizeof(asrv_mission));
     for(i=0; i<MAXPERCORSI; i++){
        if(mission.punto[i]==0) break;
        //-----------------------------------
        // PASSI PROGRAMMA
        //-----------------------------------
        p0 = mission.punto[i];
        ps = mission.punto[i+1];
        //-----------------------------------
        // Punto successivo missione
        //-----------------------------------
        if(ps!=0){
           rot  = rot_standard(i);
           ind  = mission.dati_perc[i].dir;
           ral  = mission.dati_perc[i].ral;
           dist = mission.dati_perc[i].dist;
           asrv_mission.step[count_step].ps=ps;
           asrv_mission.step[count_step].dist=dist;
           asrv_mission.step[count_step].rall=ral;
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
           asrv_mission.step[count_step].cmd.mov_X  = movX;
           asrv_mission.step[count_step].cmd.mov_Y  = movY;
           asrv_mission.step[count_step].cmd.mov_av = movA;
           asrv_mission.step[count_step].cmd.mov_ind= movI;
           //----------------------------------------------------
           // Bit di lenta, cambi corsia o rallentamento forzato
           //----------------------------------------------------
           if(ral!=0 || N.n[p0]->corsia!=N.n[ps]->corsia) asrv_mission.step[count_step].cmd.lenta=true;
           count_step++;
        }
        //-----------------------------------
        // ULTIMO STEP
        //-----------------------------------
        else{
           asrv_mission.step[count_step].rall=0;
           //---------------------------------
           // CARICO/SCARICO
           //---------------------------------
           if(mission.Typ!=MISS_SOLO_POSIZIONAMENTO){
              //---------------------------------
              // Carico/Scarico
              //---------------------------------
              switch(mission.Typ){
                 case  MISS_CARICO_DA_BAIA:
                 case  MISS_CARICO_DA_MACCHINA:
                 case  MISS_CARICO_DA_MAGAZZINO:  asrv_mission.step[count_step].cmd.sollev0=true;
                                                  asrv_mission.step[count_step].cmd.sollev1=false;
                                                  break;
                 case  MISS_SCARICO_SU_BAIA:
                 case  MISS_SCARICO_SU_MACCHINA:
                 case  MISS_SCARICO_SU_MAGAZZINO: asrv_mission.step[count_step].cmd.scarica=true;
                                                  break;
              }
              //---------------------------------
              // CARICO/SCARICO SUL NODO
              //---------------------------------
              if(N.n[p0]->direz==0){
                 asrv_mission.step[count_step].cmd.mov_X  = false;
                 asrv_mission.step[count_step].cmd.mov_Y  = false;
                 asrv_mission.step[count_step].cmd.mov_av = false;
                 asrv_mission.step[count_step].cmd.mov_ind= false;
                 asrv_mission.step[count_step].ps=p0;
                 asrv_mission.step[count_step].dist=0;
                 count_step++;
              }
              //---------------------------------
              // DIREZIONE CARICO/SCARICO
              //---------------------------------
              else{
                 rot = rot_standard(i);
                 if(N.n[p0]->direz==1) ind=false;
                 if(N.n[p0]->direz==2) ind=true;
                 if(rot==0 && ind==false){movY=true; movA=true;}
                 if(rot==2 && ind==true ){movY=true; movA=true;}
                 if(rot==0 && ind==true ){movY=true; movI=true;}
                 if(rot==2 && ind==false){movY=true; movI=true;}
                 if(rot==1 && ind==false){movX=true; movI=true;}
                 if(rot==3 && ind==true ){movX=true; movI=true;}
                 if(rot==1 && ind==true ){movX=true; movA=true;}
                 if(rot==3 && ind==false){movX=true; movA=true;}
                 asrv_mission.step[count_step].cmd.mov_X  = movX;
                 asrv_mission.step[count_step].cmd.mov_Y  = movY;
                 asrv_mission.step[count_step].cmd.mov_av = movA;
                 asrv_mission.step[count_step].cmd.mov_ind= movI;
                 asrv_mission.step[count_step].ps=p0;
                 asrv_mission.step[count_step].dist=0;   // (!!! VALUTARE !!!) offset per profondità dinamica
                 count_step++;
              }
           }
           //-----------------------------------------
           // FINE MISSIONE
           //-----------------------------------------
           asrv_mission.step[count_step].cmd.end=true;
           asrv_mission.step[count_step].ps=p0;
           count_step++;
        }
     }
     //---------------------------------------------
     // Parametri di missione
     //---------------------------------------------
     asrv_mission.p_start=mission.pstart;
     asrv_mission.p_end=mission.pend;
     asrv_mission.tot_step=count_step-1;
     asrv_mission.typ_car=0;   // (!!! VALUTARE !!!) stabilità del carico
     //---------------------------------------------
     // Comunicazione disattivata
     //---------------------------------------------
     if(break_Com[COM1]==true ) return 0;
     //---------------------------------------------
     // Solo per comunicazione tipo BECKOFF
     //---------------------------------------------
     if(Tipo!=TYPCOM_BECKHOFF ) return 0;
     //---------------------------------------------
     // Porta Backhoff non valida
     //---------------------------------------------
     if(Port!=BECKHOFF_PORT   ) return 0;
     //--------------------------------------------------------
     // Apertura porta di comunicazione
     //--------------------------------------------------------
     AdsPortOpen();
     //--------------------------------------------------------
     // Leggo Stato ASRV <- Plc BECKHOFF
     //--------------------------------------------------------
     err = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, sizeof(szVarMis), szVarMis);
     if(!err) err = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(ASRV_MISSION), &asrv_mission);
     //--------------------------------------------------------
     // Errore scrittura
     //--------------------------------------------------------
     if(err){
        sprintf(all_mess, "(Mission_ASRV%02d) Error %d write %s PLC BECKHOFF", agv_num, err, szVarMis);
        errore_comunicazione[COM1][agv_num]=1;
     }
     else{
        errore_comunicazione[COM1][agv_num]=0;
     }
     AdsPortClose();
  }

  return err;
}

// -----------------------------
//   ASRV_trs_command()
// -----------------------------
// Trasmssione comandi ASRV
//
int agv::ASRV_trs_command(char *all_mess)
/***********************************/
{
  int err=0;
  int i, pos;
  AnsiString Buff;
  AmsAddr  Addr;
  PAmsAddr pAddr = &Addr;
  unsigned char NetId1, NetId2;
  unsigned char BeckhoffIP1[4];
  long handle;
  WORD Port;
  int  Tipo;
  unsigned long lHdlVar;
  char szVarCmd[]={"Var_Globali.sRxCmd"};

  err=0;
  memset(&BeckhoffIP1[0], NULL, sizeof(BeckhoffIP1));
  //---------------------------------------------
  // Recupero i parametri di comunicazione:
  // 1 - Tipo di comunicazione  (HOST/FINS)
  // 2 - Porta di comunicazione (se Host)
  // 3 - Indice del Plc
  //---------------------------------------------
  Tipo   = OmrPlc[agv_num].TypCom;
  Port   = OmrPlc[agv_num].RemotePort;
  NetId1 = OmrPlc[agv_num].DNA;
  NetId2 = OmrPlc[agv_num].DA2;
  //---------------------------------------------
  // Scompattazione indirizzo IP
  //---------------------------------------------
  Buff = OmrPlc[agv_num].RemoteHost;
  try{
     pos=i=0;
     do{
        pos = Buff.Pos(".");
        if(pos!=0){
           BeckhoffIP1[i]=StrToInt(Buff.SubString(1, pos-1));
           Buff = Buff.SubString(pos+1, Buff.Length());
           i++;
        }
        else{
           BeckhoffIP1[3]=StrToInt(Buff);
        }
     }while(pos);
  }
  catch(...){
     sprintf(all_mess, "(Command_ASRV%02d) Error in IP string format PLC BECKHOFF", agv_num);
     return 1;
  }

  //###
  //---------------------------
  // Verifica Ping con PLC ASRV
  //---------------------------
  int   MsecRisp;
  int   err_ping=0;
  err_ping = AlgoPing( OmrPlc[ agv_num ].RemoteHost.c_str(), 500, &MsecRisp, all_mess );
  if(err_ping){
     sprintf(all_mess, "(Command_ASRV%02d) PLC BECKHOFF non raggiungibile", agv_num );
     errore_comunicazione[COM1][agv_num]=1;
     return err_ping;
  }
  //###

  //---------------------------------------------
  // Parametrizzazione comunicazione Beckoff
  //---------------------------------------------
  pAddr->port = Port;
  pAddr->netId.b[0]=BeckhoffIP1[0];
  pAddr->netId.b[1]=BeckhoffIP1[1];
  pAddr->netId.b[2]=BeckhoffIP1[2];
  pAddr->netId.b[3]=BeckhoffIP1[3];
  pAddr->netId.b[4]=NetId1;
  pAddr->netId.b[5]=NetId2;
  AdsSyncSetTimeout(TimeOutSocketTCP);
  //---------------------------------------------
  // Reset comando di start con missione in corso
  //---------------------------------------------
  if(asrv_stato.start>0  && asrv_comandi.start>0 ) asrv_comandi.start=0;
  if(asrv_comandi.end>0                          ) asrv_comandi.start=0;
  //---------------------------------------------
  // Reset new_pos se aggiornato o missione on
  //---------------------------------------------
  if(asrv_stato.pos==asrv_comandi.new_pos        ) asrv_comandi.new_pos=0;
  if(asrv_stato.start>0  || asrv_comandi.start>0 ) asrv_comandi.new_pos=0;
  //---------------------------------------------
  // Reset END mission se missione già azzerata
  //---------------------------------------------
  if(asrv_stato.start==0  && asrv_comandi.end>0  ){asrv_comandi.end=0; asrv_comandi.pintend=0;}
  //---------------------------------------------
  // WATCH-DOG (incrementa valore letto in stato)
  //---------------------------------------------
  asrv_comandi.wdog=asrv_stato.wdog+1;
  if(asrv_stato.wdog>30000) asrv_comandi.wdog=1;
  //---------------------------------------------
  // Comunicazione disattivata
  //---------------------------------------------
  if(break_Com[COM1]==true ) return 0;
  //---------------------------------------------
  // Solo per comunicazione tipo BECKOFF
  //---------------------------------------------
  if(Tipo!=TYPCOM_BECKHOFF ) return 0;
  //---------------------------------------------
  // Porta Backhoff non valida
  //---------------------------------------------
  if(Port!=BECKHOFF_PORT   ) return 0;
  //---------------------------------------------
  // Apertura porta di comunicazione
  //---------------------------------------------
  AdsPortOpen();
  //---------------------------------------------
  // Leggo Stato ASRV <- Plc BECKHOFF
  //---------------------------------------------
  err = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, sizeof(szVarCmd), szVarCmd);
  if(!err) err = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(ASRV_COMANDI), &asrv_comandi);
  //---------------------------------------------
  // Errore scrittura
  //---------------------------------------------
  if(err){
     sprintf(all_mess, "(Command_ASRV%02d) Error %d write %s PLC BECKHOFF", agv_num, err, szVarCmd);
     errore_comunicazione[COM1][agv_num]=1;
  }
  else{
     errore_comunicazione[COM1][agv_num]=0;
  }
  AdsPortClose();

  return err;
}

// ---------------------------
//  CompilaProgrammaGrafico()
// ---------------------------
// Compila il file PROGTGV.DAT con le informazioni di percorso
// del TGV residenti in RAM o scaricate su PC.
//
int agv::ASRV_compila_programma_grafico( bool Plc )
/******************************************/
{
  int    err=0;
  FILE   *ptrfile;
  short int i, j, cmd_curva;
  char   stringa[101]="";
  char   buffer[201]="";
  char   filename[21]="PROGTGV.DAT";
  struct ASRV_MISSION asrv_buffmiss;

  memset(&asrv_buffmiss, 0, sizeof(ASRV_MISSION));
  //-----------------------------------------------------
  // Carica nel buffer dell'agv il programma letto da PLC
  //-----------------------------------------------------
  if(Plc==true){
     err = ASRV_load_prg_mission(&asrv_buffmiss, buffer);
  }
  else{
     memcpy(&asrv_buffmiss, &asrv_mission, sizeof(ASRV_MISSION));
  }
  if(err){
     sprintf(buffer, "Error in load program on PLC ASRV%d...", agv_num);
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
  if(asrv_stato.start!=0){
     sprintf(stringa, "asrv_miss.pstart: %d",  asrv_buffmiss.p_start);
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "asrv_miss.pend: %d",    asrv_buffmiss.p_end);
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "asrv_miss.totstep: %d", asrv_buffmiss.tot_step);
     Write_Line(ptrfile, stringa);
     sprintf(stringa, "asrv_miss.typ_car: %d", asrv_buffmiss.typ_car);
     Write_Line(ptrfile, stringa);
     Write_Line(ptrfile, "================================================");
     for(i=1; i<MAXSTEP_ASRV; i++){
        sprintf(stringa, "step[%03d].PSuc: %d", i, asrv_buffmiss.step[i].ps);
        Write_Line(ptrfile, stringa);
        sprintf(stringa, "step[%03d].Dist: %d", i, asrv_buffmiss.step[i].dist);
        Write_Line(ptrfile, stringa);
        sprintf(stringa, "step[%03d].Rall: %d", i, asrv_buffmiss.step[i].rall);
        Write_Line(ptrfile, stringa);
        sprintf(stringa, "step[%03d].MovX: %d", i, asrv_buffmiss.step[i].cmd.mov_X);
        Write_Line(ptrfile, stringa);
        sprintf(stringa, "step[%03d].MovY: %d", i, asrv_buffmiss.step[i].cmd.mov_Y);
        Write_Line(ptrfile, stringa);
        sprintf(stringa, "step[%03d].MovA: %d", i, asrv_buffmiss.step[i].cmd.mov_av);
        Write_Line(ptrfile, stringa);
        sprintf(stringa, "step[%03d].MovI: %d", i, asrv_buffmiss.step[i].cmd.mov_ind);
        Write_Line(ptrfile, stringa);
        sprintf(stringa, "step[%03d].Sol0: %d", i, asrv_buffmiss.step[i].cmd.sollev0);
        Write_Line(ptrfile, stringa);
        sprintf(stringa, "step[%03d].Sol1: %d", i, asrv_buffmiss.step[i].cmd.sollev1);
        Write_Line(ptrfile, stringa);
        sprintf(stringa, "step[%03d].Scar: %d", i, asrv_buffmiss.step[i].cmd.scarica);
        Write_Line(ptrfile, stringa);
        sprintf(stringa, "step[%03d].Lent: %d", i, asrv_buffmiss.step[i].cmd.lenta);
        Write_Line(ptrfile, stringa);
        sprintf(stringa, "step[%03d].Fine: %d", i, asrv_buffmiss.step[i].cmd.end);
        Write_Line(ptrfile, stringa);
        Write_Line(ptrfile, "================================================");
        //-----------------------------------
        // End mission
        //-----------------------------------
        if(asrv_buffmiss.step[i].cmd.end==true) break;
     }
  }
  //---------------------------------------
  // Inserimento anche comandi per debug
  //---------------------------------------
  sprintf(stringa, "asrv_cmd.wdog: %d",    asrv_comandi.wdog);
  Write_Line(ptrfile, stringa);
  sprintf(stringa, "asrv_cmd.start: %d",   asrv_comandi.start);
  Write_Line(ptrfile, stringa);
  sprintf(stringa, "asrv_cmd.pint: %d",    asrv_comandi.pintend);
  Write_Line(ptrfile, stringa);
  sprintf(stringa, "asrv_cmd.end: %d",     asrv_comandi.end);
  Write_Line(ptrfile, stringa);
  sprintf(stringa, "asrv_cmd.stop: %d",    asrv_comandi.cbit.stop);
  Write_Line(ptrfile, stringa);
  sprintf(stringa, "asrv_cmd.new_pos: %d", asrv_comandi.new_pos);
  Write_Line(ptrfile, stringa);
  Write_Line(ptrfile, "================================================");
  //---------------------------------------
  // Chiusura file
  //---------------------------------------
  fclose(ptrfile);
  return 0;
}

// -----------------------------
//   ASRV_load_file_stato()
// -----------------------------
// load Stato da file
//
int agv::ASRV_load_file_stato(char *all_mess)
/***********************************/
{
  int  i,
       err =0;
  int handle;

  memset(&asrv_stato ,  0, sizeof(ASRV_STATO));
  memset(&asrv_comandi, 0, sizeof(ASRV_COMANDI));  // reset anche struttura comandi
  handle = sopen( ASRV_file_stato_sorg, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
      close( handle );
      sprintf(all_mess, MESS[112], ASRV_file_stato_sorg);
      return 1;
  }
  i = read( handle, &asrv_stato, sizeof( ASRV_STATO ) );
  close( handle );

  if (i != sizeof(ASRV_STATO))  err = 1;
  if(err) sprintf(all_mess, MESS[112], ASRV_file_stato_sorg);

  return err;
}


// -----------------------------
//   ASRV_save_file_stato()
// -----------------------------
// Salva stato AGV su file
//
int agv::ASRV_save_file_stato(char *all_mess)
/***********************************/
{
  int  i,
       err = 0;
  int handle;


  //---------------------------------------------
  handle = sopen( ASRV_file_stato_dest, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );

  if( handle<0 ){
      close( handle );
      sprintf(all_mess, MESS[113], ASRV_file_stato_dest);
      return 1;
  }
  i =  write( handle, &asrv_stato, sizeof( ASRV_STATO ) );
  close( handle );

  if (i != sizeof(ASRV_STATO))  err = 1;
  if(err) sprintf(all_mess, MESS[113], ASRV_file_stato_dest);

  return err;
}

// -----------------------------
//   ASRV_load_file_mission()
// -----------------------------
// load Mission da file
//
int agv::ASRV_load_file_mission(char *all_mess)
/***********************************/
{
  int  i,
       err =0;
  int handle;

  memset(&asrv_mission , 0, sizeof(ASRV_MISSION));
  handle = sopen( ASRV_file_mission_sorg, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
      close( handle );
      sprintf(all_mess, MESS[112], ASRV_file_mission_sorg);
      return 1;
  }
  i = read( handle, &asrv_mission, sizeof( ASRV_MISSION ) );
  close( handle );

  if (i != sizeof(ASRV_MISSION))  err = 1;
  if(err) sprintf(all_mess, MESS[112], ASRV_file_mission_sorg);

  return err;
}


// -----------------------------
//   ASRV_save_file_mission()
// -----------------------------
// Salva missione AGV su file
//
int agv::ASRV_save_file_mission(char *all_mess)
/***********************************/
{
  int  i,
       err = 0;
  int handle;


  //---------------------------------------------
  handle = sopen( ASRV_file_mission_dest, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );

  if( handle<0 ){
      close( handle );
      sprintf(all_mess, MESS[113], ASRV_file_mission_dest);
      return 1;
  }
  i =  write( handle, &asrv_mission, sizeof( ASRV_MISSION ) );
  close( handle );

  if (i != sizeof(ASRV_MISSION))  err = 1;
  if(err) sprintf(all_mess, MESS[113], ASRV_file_mission_dest);

  return err;
}
//===========================================================================
//
//                     GESTIONE OTTIMIZZAZIONE TRAGITTI ASRV
//
//===========================================================================
// -----------------------------------
//   ASRV_ottimizza_path()
// -----------------------------------
// Gestione attraversamento corsie ASRV
//
void agv::ASRV_select_path(short int dest, short int *perc_asrv, struct DATI_PERCORSO *dati_asrv)
{
  int  i, err=0;
  char buffer[101]="";
  bool ok_path_inverso;
  short int count1, count2;
  short int n_dist, n_rot;
  short int min_to_start1;
  short int min_to_start2;
  short int n_interr_perc1;
  short int n_interr_perc2;
  short int min_nodi_perc1;
  short int min_nodi_perc2;
  short int agv_bloccante_path1;
  short int agv_bloccante_path2;
  short int perc1[MAXPERCORSI];
  struct DATI_PERCORSO dati1[MAXPERCORSI];
  short int perc2[MAXPERCORSI];
  struct DATI_PERCORSO dati2[MAXPERCORSI];

  //--------------------------------------------------------
  // reset strutture locali
  //--------------------------------------------------------
  // PERCORSO DIRETTO
  memset(&perc1[0], 0, sizeof(perc1));
  memset(&dati1[0], 0, sizeof(dati1));
  //--------------------------------------------------------
  // PERCORSO DIRETTO ORIGINALE
  //--------------------------------------------------------
  memcpy(&perc1[0], perc_asrv, sizeof(perc1));
  memcpy(&dati1[0], dati_asrv, sizeof(dati1));
  //--------------------------------------------------------
  // PERCORSO DIRETTO RIELABORATO PER ATTRAVERAMENTO CORSIE
  //--------------------------------------------------------
  ASRV_attraversa_corsie(&perc1[0], &dati1[0]);
  //--------------------------------------------------------
  // Verifico se percorso standard è disponibile
  //--------------------------------------------------------
  agv_bloccante_path1=min_nodi_perc1=min_to_start1=n_interr_perc1=0;
  test_percorso_disponibile(&perc1[0], &dati1[0], &agv_bloccante_path1, &min_nodi_perc1, &min_to_start1, &n_interr_perc1, buffer);
  //--------------------------------------------------------
  // PERCORSO INVERSO
  //--------------------------------------------------------
  n_dist=n_rot=0;
  ok_path_inverso=true;
  memset(&perc2[0], 0, sizeof(perc2));
  memset(&dati2[0], 0, sizeof(dati2));
  err = distanza(STD_PATH, dest, stato.pos, &perc2[0], &dati2[0], buffer, &n_dist, &n_rot);
  if(err!=0 || n_dist>MAXPERCORSI) ok_path_inverso=false;
  //--------------------------------------------------------
  // Se il percorso inverso è valido, verifico se supera
  // i metri massimi ammessi
  //--------------------------------------------------------
  if(ok_path_inverso==true){
     //--------------------------------------------------------
     // PERCORSO DIRETTO RIELABORATO PER ATTRAVERAMENTO CORSIE
     //--------------------------------------------------------
     ASRV_attraversa_corsie(&perc2[0], &dati2[0]);
     //--------------------------------------------------------
     // Verifico se percorso standard è disponibile
     //--------------------------------------------------------
     agv_bloccante_path2=min_nodi_perc2=min_to_start2=n_interr_perc2=0;
     test_percorso_disponibile(&perc2[0], &dati2[0], &agv_bloccante_path2, &min_nodi_perc2, &min_to_start2, &n_interr_perc2, buffer);
     if(min_nodi_perc2>ASRV_MinNodiLiberiPercorsoInverso) ok_path_inverso=false;
  }
  //--------------------------------------------------------
  // Se il percorso inverso ha un numero di nodi occupabili
  // sufficente per essere scelto, faccio un conteggio
  // dei cambi di corsia.
  // NOTA: più cambi di corsia ci sono più lente deve fare
  //       la navetta
  //--------------------------------------------------------
  if(ok_path_inverso==true){
     //--------------------------------------------------------------
     // PERCORSO STANDARD (conteggio cambi di corsia)
     //--------------------------------------------------------------
     count1=0;
     for(i=0; i<MAXPERCORSI; i++){
        if(perc1[i+1]==0) break;
        //-------------------------
        // PERCORSO STANDARD
        // Conteggi cambi corsia
        //-------------------------
        if(N.n[perc1[i]]->corsia==N.n[perc1[i+1]]->corsia) continue;
        count1++;
     }
     //--------------------------------------------------------------
     // PERCORSO INVERSO (conteggio cambi di corsia)
     //--------------------------------------------------------------
     count2=0;
     for(i=0; i<MAXPERCORSI; i++){
        if(perc2[i+1]==0) break;
        if(N.n[perc2[i]]->corsia==N.n[perc2[i+1]]->corsia) continue;
        count2++;
     }
     //--------------------------------------------------------------
     // se il percorso inverso ha troppo cambi di corsi --> skip!!
     //--------------------------------------------------------------
     if(count2>count1) ok_path_inverso=false;
     //--------------------------------------------------------------
     // se l'inverso e lo standard hanno lo stesso numero di cambi
     // corsia, scelgo quello più traggitto disponibile
     //--------------------------------------------------------------
     if(count2==count1 && min_nodi_perc2<=min_nodi_perc1) ok_path_inverso=false;
  }
  //--------------------------------------------------------
  // RITORNO IL PERCORSO IL PERCORSO FINALE SCELTO
  //--------------------------------------------------------
  // 1. scelto PERCORSO DIRETTO
  if(ok_path_inverso==false){
     memcpy(perc_asrv, &perc1[0], sizeof(perc1));
     memcpy(dati_asrv, &dati1[0], sizeof(dati1));
  }
  // 2. scelto PERCORSO INVERSO
  else{
     memcpy(perc_asrv, &perc2[0], sizeof(perc2));
     memcpy(dati_asrv, &dati2[0], sizeof(dati2));
  }
}

// -----------------------------------
//   ASRV_attraversa_corsie()
// -----------------------------------
// Gestione attraversamento corsie ASRV
//
void agv::ASRV_attraversa_corsie(short int *perc, struct DATI_PERCORSO *dati)
{
  int err;
  char buffer[101]="";
  bool path_taglio_trovato;
  bool ok_ordine;
  bool in_X, in_Y;
  int xm, ym;
  int xd, yd;
  int x1, y1;
  int x2, y2;
  int xn1, yn1;
  int xn2, yn2;
  short int id_taglio;
  short int altro_agv;
  short int i, j, k, id;
  short int n0, n1, n2;
  short int end_taglio;
  short int p1, p2, n_mag;
  short int n_dist, n_rot;
  short int num_mag, num_pos;
  short int rot_taglio, dir_taglio;
  short int ral_taglio, dist_taglio;
  short int n_taglio[MAXPERCORSI];
  struct DATI_PERCORSO d_taglio[MAXPERCORSI];
  short int new_perc[MAXPERCORSI];
  struct DATI_PERCORSO new_dati[MAXPERCORSI];

  //--------------------------------------------------
  // Reset struttura percorso rielaborato
  //--------------------------------------------------
  memset(&new_perc[0], 0, sizeof(new_perc));
  memset(&new_dati[0], 0, sizeof(new_dati));
  //--------------------------------------------------
  // Scorro tutta la missione del TGV per individuare
  // eventuali passaggi sotto magazzini FIFO
  //--------------------------------------------------
  id=0;
  for(i=0; i<MAXPERCORSI-1; i++){
     //--------------------------------------------------
     // Recupero numero del nodo e i dati del percorso.
     //--------------------------------------------------
     new_perc[id] = perc[i];
     memcpy(&new_dati[id], &dati[i], sizeof(DATI_PERCORSO));
     //--------------------------------------------------
     // Mi fermo alla destinazione
     //--------------------------------------------------
     if(perc[i+1]==0) break;
     //--------------------------------------------------
     // Verifico se il nodo considerato appartiene a un
     // magazzino
     //--------------------------------------------------
     p1  = perc[i];
     p2  = 0;
     end_taglio = i;
     dir_taglio = 99;
     rot_taglio = 99;
     ral_taglio = 99;
     dist_taglio= 0;
     //--------------------------------------------------
     // Verifico se nodo appartiene a corridoio di
     // servizio
     //--------------------------------------------------
     if(N.n[p1]->corsia!=ASRV_NODO_SERVIZIO){id++; continue;}
     //--------------------------------------------------
     // 0. Attraversamento corsie disattivato
     //--------------------------------------------------
     if(ASRV_AbilitaPassaggioSottoAccumoli==false){id++; continue;}
     //--------------------------------------------------
     // Memorizzo coordinate punto attraveramento
     //--------------------------------------------------
     x1 = Punti[p1][0];
     y1 = (Punti[p1][1]*(-1));
     //--------------------------------------------------
     // 1. VERIFICO CONDIZIONE ATTRAVERAMENTO CORRIDOIO
     // Verifico tutti i punti successivi associati
     // al magazzino considerato
     //--------------------------------------------------
     path_taglio_trovato=false;
     for(j=0; j<=MAXPUNTINODO-1; j++){
        if(N.n[p1]->rec[j].ps==0                ) break;
        if(!N.punti_notevoli[N.n[p1]->rec[j].ps]) continue;
        //--------------------------------------------------
        // Attraversamento solo corridoi di accumulo
        //--------------------------------------------------
        n_mag = N.n[p1]->rec[j].ps;
        if(N.n[n_mag]->corsia!=ASRV_NODO_ACCUMULO) continue;
        if(N.n[n_mag]->typ!=2                    ) continue;  // abilitazione attraversamento
        //--------------------------------------------------
        // Verifico se appartiene a magazzino FIFO
        //--------------------------------------------------
        in_X=in_Y=false;
        num_mag=num_pos=0;
        if(M.test_punto_presente_mag(buffer, n_mag, &num_mag, &num_pos)) continue;
        //--------------------------------------------------
        // Magazzino non valido
        //--------------------------------------------------
        if(num_mag<1                      ) continue;
        if(num_mag>TOTMAG                 ) continue;
        if(M.m[num_mag]->Num<1            ) continue;
        if(M.m[num_mag]->Num>TOTMAG       ) continue;
        if(M.m[num_mag]->Fifo==false      ) continue;
        if(M.m[num_mag]->Tipo==MG_ESCLUSO ) continue;
        if(M.m[num_mag]->PuntoIng<1       ) continue;
        if(M.m[num_mag]->PuntoIng>TOTPUNTI) continue;
        if(M.m[num_mag]->PuntoOut<1       ) continue;
        if(M.m[num_mag]->PuntoOut>TOTPUNTI) continue;
        //--------------------------------------------------
        // Controllo stato magazzino (se ASRV carico)
        //--------------------------------------------------
        if(asrv_stato.sbit1.carico && M.m[num_mag]->NumCestoni!=0) continue;
        //--------------------------------------------------
        // Verifico che nodo del magazzino non sia
        // occupato o riservato
        //--------------------------------------------------
        altro_agv=0;
        if(P.test_dest_riservata(n_mag, &altro_agv, agv_num) && altro_agv!=0 && altro_agv!=agv_num) continue;
        if(P.test_dest_occupata(n_mag, &altro_agv) && altro_agv!=0 && altro_agv!=agv_num) continue;
        //--------------------------------------------------
        // Memorizzo le coordinate della corsia considerata
        //--------------------------------------------------
        xm = Punti[n_mag][0];
        ym = (Punti[n_mag][1]*(-1));
        //--------------------------------------------------
        // Controllo verso di percorrenza
        //--------------------------------------------------
        // 1. ingresso nel verso del magazzino
        if(n_mag==M.m[num_mag]->PuntoIng){
           //--------------------------------------------------
           // controllo se punto successivo all'uscita
           // appartiene al percorso originale dell'ASRV
           //--------------------------------------------------
           if(N.n[M.m[num_mag]->PuntoOut]->rec[0].ps<1       ) continue;
           if(N.n[M.m[num_mag]->PuntoOut]->rec[0].ps>TOTPUNTI) continue;
           //--------------------------------------------------
           // Recupero la rotazione di attraveramento
           //--------------------------------------------------
           rot_taglio=N.n[M.m[num_mag]->PuntoOut]->rec[0].rot;
           dir_taglio=N.n[M.m[num_mag]->PuntoOut]->rec[0].ind;
           switch(rot_taglio){
              case   0:
              case 180:in_Y=true;
                       break;
              case  90:
              case 270:in_X=true;
                       break;
           }
        }
        // 2. ingresso dal lato di uscita del magazzino
        if(n_mag==M.m[num_mag]->PuntoOut){
           //--------------------------------------------------
           // Memorizzo verso di ingresso invertito in maga
           //--------------------------------------------------
           rot_taglio=N.n[p1]->rec[j].rot;
           dir_taglio=N.n[p1]->rec[j].ind;
           switch(rot_taglio){
              case   0:
              case 180:in_Y=true;
                       break;
              case  90:
              case 270:in_X=true;
                       break;
           }
        }
        //--------------------------------------------------
        // Verifico esistenza nodo di percorso su
        // coordinata di attraversamento.
        //--------------------------------------------------
        for(k=i+1; k<MAXPERCORSI-1; k++){
           if(in_Y==false && in_X==false     ) break;
           if(perc[k]==0                     ) break;
           //--------------------------------------------------
           // Interrompo su cambio piano
           //--------------------------------------------------
           if(N.n[p1]->piano!=N.n[perc[k]]->piano) break;
           //--------------------------------------------------
           // Scarto punti notevoli
           //--------------------------------------------------
           if(N.punti_notevoli[perc[k]]==true) continue;
           //--------------------------------------------------
           // Recupero coordinate punti percorso
           //--------------------------------------------------
           x2 = Punti[perc[k]][0];
           y2 = (Punti[perc[k]][1]*(-1));
           //--------------------------------------------------
           // Y- (nodo mag deve essere compreso tra p1 e p2)
           //--------------------------------------------------
           if(in_Y==true && y2<ym && ym>y1) continue;
           //--------------------------------------------------
           // Y+ (nodo mag deve essere compreso tra p1 e p2)
           //--------------------------------------------------
           if(in_Y==true && y2>y1 && ym<y1) continue;
           //--------------------------------------------------
           // X- (nodo mag deve essere compreso tra p1 e p2)
           //--------------------------------------------------
           if(in_X==true && x2<x1 && xm>x1) continue;
           //--------------------------------------------------
           // X+ (nodo mag deve essere compreso tra p1 e p2)
           //--------------------------------------------------
           if(in_X==true && x2>x1 && xm>x1) continue;
           //--------------------------------------------------
           // Verifico se punto in ortogonale a corsia di
           // attraversamento
           //--------------------------------------------------
           if(in_X==true && abs(ym-y2)>5) continue;
           if(in_Y==true && abs(xm-x2)>5) continue;
           //--------------------------------------------------
           // trovato altro punto su percorso attraveramento
           //--------------------------------------------------
           p2 = perc[k];
           end_taglio=k-1;
           break;
        }
        if(p2!=0) break;
     }
     //--------------------------------------------------
     // 2. DETERMINO PERCORSO DI TAGLIO
     // Trovati i punti di attraversamento analizzo
     // struttrua percorsi e determino il percorso di
     // taglio con nodi sulla stessa coordinata in base
     // a verso di percorrenza del magazzino
     //--------------------------------------------------
     if(p1!=0 && p2!=0){
        x2 = Punti[p2][0];
        y2 = (Punti[p2][1]*(-1));
        switch(rot_taglio){
           case   0:
           case 180: in_Y=true;
                     break;
           case  90:
           case 270: in_X=true;
                     break;
        }
        //--------------------------------------------
        // compilo elenco nodi percorso di taglio
        // in sequenza di percorrenza
        //--------------------------------------------
        memset(&n_taglio[0], 0, sizeof(n_taglio));
        memset(&d_taglio[0], 0, sizeof(d_taglio));
        //--------------------------------------------
        // Y-
        //--------------------------------------------
        if(in_Y==true && y2<y1){
           k=0;
           for(j=1; j<=MAXPUNTI; j++){
              if(k>=MAXPERCORSI) break;
              if(Punti[j][2]==0) break;
              //-----------------------------------------
              // Solo nodi esistenti
              //-----------------------------------------
              if(N.n[j]->num<1        ) continue;
              if(N.n[j]->num>TOTPUNTI ) continue;
              //--------------------------------------------------
              // Scarto nodi su altri piani
              //--------------------------------------------------
              if(N.n[p1]->piano!=N.n[N.n[j]->num]->piano) continue;
              //-----------------------------------------
              // Se nodo di magazzino verifico se
              // percorribile
              //-----------------------------------------
              n1=N.n[j]->num;
              if(N.punti_notevoli[n1]==true){
                 num_mag=num_pos=0;
                 if(M.test_punto_presente_mag(buffer, n1, &num_mag, &num_pos)) continue;
                 //--------------------------------------------------
                 // Magazzino non valido
                 //--------------------------------------------------
                 if(num_mag<1                      ) continue;
                 if(num_mag>TOTMAG                 ) continue;
                 if(M.m[num_mag]->Num<1            ) continue;
                 if(M.m[num_mag]->Num>TOTMAG       ) continue;
                 if(M.m[num_mag]->Fifo==false      ) continue;
                 if(M.m[num_mag]->Tipo==MG_ESCLUSO ) continue;
                 if(M.m[num_mag]->PuntoIng<1       ) continue;
                 if(M.m[num_mag]->PuntoIng>TOTPUNTI) continue;
                 if(M.m[num_mag]->PuntoOut<1       ) continue;
                 if(M.m[num_mag]->PuntoOut>TOTPUNTI) continue;
                 //--------------------------------------------------
                 // Controllo stato magazzino (se ASRV carico)
                 //--------------------------------------------------
                 if(asrv_stato.sbit1.carico && M.m[num_mag]->NumCestoni!=0) continue;
                 //--------------------------------------------------
                 // Verifico che nodo del magazzino non sia
                 // occupato o riservato
                 //--------------------------------------------------
                 altro_agv=0;
                 if(P.test_dest_riservata(n1, &altro_agv, agv_num) && altro_agv!=0 && altro_agv!=agv_num) continue;
                 if(P.test_dest_occupata(n1, &altro_agv) && altro_agv!=0 && altro_agv!=agv_num) continue;
              }
              //-----------------------------------------
              // recupero coordinate
              //-----------------------------------------
              xn1 = Punti[n1][0];
              yn1 = (Punti[n1][1]*(-1));
              //-----------------------------------------
              // Scarto punti non allineati
              //-----------------------------------------
              if(abs(x1-xn1)>5) continue;
              //-----------------------------------------
              // Scarto punti fuori range di coordinate
              //-----------------------------------------
              if(yn1<y2 || yn1>y1) continue;
              path_taglio_trovato=true;
              n_taglio[k]=n1;
              k++;
           }
           //-----------------------------------------
           // Ordinamento in base al verso
           //-----------------------------------------
           do{
              ok_ordine=true;
              for(j=1; j<MAXPERCORSI; j++){
                 if(n_taglio[j]==0) break;
                 n1  = n_taglio[j-1];
                 xn1 = Punti[n1][0];
                 yn1 = (Punti[n1][1]*(-1));
                 n2  = n_taglio[j];
                 xn2 = Punti[n2][0];
                 yn2 = (Punti[n2][1]*(-1));
                 if(yn2<yn1) continue; // ordine giusto
                 //---------------------------
                 // Inversione ordine nodi
                 //---------------------------
                 n0 = n_taglio[j-1];
                 n_taglio[j-1] = n_taglio[j];
                 n_taglio[j  ] = n0;
                 ok_ordine=false;
                 break;
              }
           }while(!ok_ordine);
        }
        //--------------------------------------------
        // Y+
        //--------------------------------------------
        if(in_Y==true && y2>y1){
           k=0;
           for(j=1; j<=MAXPUNTI; j++){
              if(k>=MAXPERCORSI) break;
              if(Punti[j][2]==0) break;
              //-----------------------------------------
              // Solo nodi esistenti
              //-----------------------------------------
              if(N.n[j]->num<1        ) continue;
              if(N.n[j]->num>TOTPUNTI ) continue;
              //--------------------------------------------------
              // Scarto nodi su altri piani
              //--------------------------------------------------
              if(N.n[p1]->piano!=N.n[N.n[j]->num]->piano) continue;
              //-----------------------------------------
              // Se nodo di magazzino verifico se
              // percorribile
              //-----------------------------------------
              n1=N.n[j]->num;
              if(N.punti_notevoli[n1]==true){
                 num_mag=num_pos=0;
                 if(M.test_punto_presente_mag(buffer, n1, &num_mag, &num_pos)) continue;
                 //--------------------------------------------------
                 // Magazzino non valido
                 //--------------------------------------------------
                 if(num_mag<1                      ) continue;
                 if(num_mag>TOTMAG                 ) continue;
                 if(M.m[num_mag]->Num<1            ) continue;
                 if(M.m[num_mag]->Num>TOTMAG       ) continue;
                 if(M.m[num_mag]->Fifo==false      ) continue;
                 if(M.m[num_mag]->Tipo==MG_ESCLUSO ) continue;
                 if(M.m[num_mag]->PuntoIng<1       ) continue;
                 if(M.m[num_mag]->PuntoIng>TOTPUNTI) continue;
                 if(M.m[num_mag]->PuntoOut<1       ) continue;
                 if(M.m[num_mag]->PuntoOut>TOTPUNTI) continue;
                 //--------------------------------------------------
                 // Controllo stato magazzino (se ASRV carico)
                 //--------------------------------------------------
                 if(asrv_stato.sbit1.carico && M.m[num_mag]->NumCestoni!=0) continue;
                 //--------------------------------------------------
                 // Verifico che nodo del magazzino non sia
                 // occupato o riservato
                 //--------------------------------------------------
                 altro_agv=0;
                 if(P.test_dest_riservata(n1, &altro_agv, agv_num) && altro_agv!=0 && altro_agv!=agv_num) continue;
                 if(P.test_dest_occupata(n1, &altro_agv) && altro_agv!=0 && altro_agv!=agv_num) continue;
              }
              //-----------------------------------------
              // recupero coordinate
              //-----------------------------------------
              xn1 = Punti[n1][0];
              yn1 = (Punti[n1][1]*(-1));
              //-----------------------------------------
              // Scarto punti non allineati
              //-----------------------------------------
              if(abs(x1-xn1)>5) continue;
              //-----------------------------------------
              // Scarto punti fuori range di coordinate
              //-----------------------------------------
              if(yn1>y2 || yn1<y1) continue;
              path_taglio_trovato=true;
              n_taglio[k]=n1;
              k++;
           }
           //-----------------------------------------
           // Ordinamento in base al verso
           //-----------------------------------------
           do{
              ok_ordine=true;
              for(j=1; j<MAXPERCORSI; j++){
                 if(n_taglio[j]==0) break;
                 n1  = n_taglio[j-1];
                 xn1 = Punti[n1][0];
                 yn1 = (Punti[n1][1]*(-1));
                 n2  = n_taglio[j];
                 xn2 = Punti[n2][0];
                 yn2 = (Punti[n2][1]*(-1));
                 if(yn2>yn1) continue; // ordine giusto
                 //---------------------------
                 // Inversione ordine nodi
                 //---------------------------
                 n0 = n_taglio[j-1];
                 n_taglio[j-1] = n_taglio[j];
                 n_taglio[j  ] = n0;
                 ok_ordine=false;
                 break;
              }
           }while(!ok_ordine);
        }
        //--------------------------------------------
        // X-
        //--------------------------------------------
        if(in_X==true && x2<x1){
           k=0;
           for(j=1; j<=MAXPUNTI; j++){
              if(k>=MAXPERCORSI) break;
              if(Punti[j][2]==0) break;
              //-----------------------------------------
              // Solo nodi esistenti
              //-----------------------------------------
              if(N.n[j]->num<1        ) continue;
              if(N.n[j]->num>TOTPUNTI ) continue;
              //--------------------------------------------------
              // Scarto nodi su altri piani
              //--------------------------------------------------
              if(N.n[p1]->piano!=N.n[N.n[j]->num]->piano) continue;
              //-----------------------------------------
              // Se nodo di magazzino verifico se
              // percorribile
              //-----------------------------------------
              n1=N.n[j]->num;
              if(N.punti_notevoli[n1]==true){
                 num_mag=num_pos=0;
                 if(M.test_punto_presente_mag(buffer, n1, &num_mag, &num_pos)) continue;
                 //--------------------------------------------------
                 // Magazzino non valido
                 //--------------------------------------------------
                 if(num_mag<1                      ) continue;
                 if(num_mag>TOTMAG                 ) continue;
                 if(M.m[num_mag]->Num<1            ) continue;
                 if(M.m[num_mag]->Num>TOTMAG       ) continue;
                 if(M.m[num_mag]->Fifo==false      ) continue;
                 if(M.m[num_mag]->Tipo==MG_ESCLUSO ) continue;
                 if(M.m[num_mag]->PuntoIng<1       ) continue;
                 if(M.m[num_mag]->PuntoIng>TOTPUNTI) continue;
                 if(M.m[num_mag]->PuntoOut<1       ) continue;
                 if(M.m[num_mag]->PuntoOut>TOTPUNTI) continue;
                 //--------------------------------------------------
                 // Controllo stato magazzino (se ASRV carico)
                 //--------------------------------------------------
                 if(asrv_stato.sbit1.carico && M.m[num_mag]->NumCestoni!=0) continue;
                 //--------------------------------------------------
                 // Verifico che nodo del magazzino non sia
                 // occupato o riservato
                 //--------------------------------------------------
                 altro_agv=0;
                 if(P.test_dest_riservata(n1, &altro_agv, agv_num) && altro_agv!=0 && altro_agv!=agv_num) continue;
                 if(P.test_dest_occupata(n1, &altro_agv) && altro_agv!=0 && altro_agv!=agv_num) continue;
              }
              //-----------------------------------------
              // recupero coordinate
              //-----------------------------------------
              xn1 = Punti[n1][0];
              yn1 = (Punti[n1][1]*(-1));
              //-----------------------------------------
              // Scarto punti non allineati
              //-----------------------------------------
              if(abs(y1-yn1)>5) continue;
              //-----------------------------------------
              // Scarto punti fuori range di coordinate
              //-----------------------------------------
              if(xn1<x2 || xn1>x1) continue;
              path_taglio_trovato=true;
              n_taglio[k]=n1;
              k++;
           }
           //-----------------------------------------
           // Ordinamento in base al verso
           //-----------------------------------------
           do{
              ok_ordine=true;
              for(j=1; j<MAXPERCORSI; j++){
                 if(n_taglio[j]==0) break;
                 n1  = n_taglio[j-1];
                 xn1 = Punti[n1][0];
                 yn1 = (Punti[n1][1]*(-1));
                 n2  = n_taglio[j];
                 xn2 = Punti[n2][0];
                 yn2 = (Punti[n2][1]*(-1));
                 if(xn2<xn1) continue; // ordine giusto
                 //---------------------------
                 // Inversione ordine nodi
                 //---------------------------
                 n0 = n_taglio[j-1];
                 n_taglio[j-1] = n_taglio[j];
                 n_taglio[j  ] = n0;
                 ok_ordine=false;
                 break;
              }
           }while(!ok_ordine);
        }
        //--------------------------------------------
        // X+
        //--------------------------------------------
        if(in_X==true && x2>x1){
           k=0;
           for(j=1; j<=MAXPUNTI; j++){
              if(k>=MAXPERCORSI) break;
              if(Punti[j][2]==0) break;
              //-----------------------------------------
              // Solo nodi esistenti
              //-----------------------------------------
              if(N.n[j]->num<1        ) continue;
              if(N.n[j]->num>TOTPUNTI ) continue;
              //--------------------------------------------------
              // Scarto nodi su altri piani
              //--------------------------------------------------
              if(N.n[p1]->piano!=N.n[N.n[j]->num]->piano) continue;
              //-----------------------------------------
              // Se nodo di magazzino verifico se
              // percorribile
              //-----------------------------------------
              n1=N.n[j]->num;
              if(N.punti_notevoli[n1]==true){
                 num_mag=num_pos=0;
                 if(M.test_punto_presente_mag(buffer, n1, &num_mag, &num_pos)) continue;
                 //--------------------------------------------------
                 // Magazzino non valido
                 //--------------------------------------------------
                 if(num_mag<1                      ) continue;
                 if(num_mag>TOTMAG                 ) continue;
                 if(M.m[num_mag]->Num<1            ) continue;
                 if(M.m[num_mag]->Num>TOTMAG       ) continue;
                 if(M.m[num_mag]->Fifo==false      ) continue;
                 if(M.m[num_mag]->Tipo==MG_ESCLUSO ) continue;
                 if(M.m[num_mag]->PuntoIng<1       ) continue;
                 if(M.m[num_mag]->PuntoIng>TOTPUNTI) continue;
                 if(M.m[num_mag]->PuntoOut<1       ) continue;
                 if(M.m[num_mag]->PuntoOut>TOTPUNTI) continue;
                 //--------------------------------------------------
                 // Controllo stato magazzino (se ASRV carico)
                 //--------------------------------------------------
                 if(asrv_stato.sbit1.carico && M.m[num_mag]->NumCestoni!=0) continue;
                 //--------------------------------------------------
                 // Verifico che nodo del magazzino non sia
                 // occupato o riservato
                 //--------------------------------------------------
                 altro_agv=0;
                 if(P.test_dest_riservata(n1, &altro_agv, agv_num) && altro_agv!=0 && altro_agv!=agv_num) continue;
                 if(P.test_dest_occupata(n1, &altro_agv) && altro_agv!=0 && altro_agv!=agv_num) continue;
              }
              //-----------------------------------------
              // recupero coordinate
              //-----------------------------------------
              xn1 = Punti[n1][0];
              yn1 = (Punti[n1][1]*(-1));
              //-----------------------------------------
              // Scarto punti non allineati
              //-----------------------------------------
              if(abs(y1-yn1)>5) continue;
              //-----------------------------------------
              // Scarto punti fuori range di coordinate
              //-----------------------------------------
              if(xn1>x2 || xn1<x1) continue;
              path_taglio_trovato=true;
              n_taglio[k]=n1;
              k++;
           }
           //-----------------------------------------
           // Ordinamento in base al verso
           //-----------------------------------------
           do{
              ok_ordine=true;
              for(j=1; j<MAXPERCORSI; j++){
                 if(n_taglio[j]==0) break;
                 n1  = n_taglio[j-1];
                 xn1 = Punti[n1][0];
                 yn1 = (Punti[n1][1]*(-1));
                 n2  = n_taglio[j];
                 xn2 = Punti[n2][0];
                 yn2 = (Punti[n2][1]*(-1));
                 if(xn2>xn1) continue; // ordine giusto
                 //---------------------------
                 // Inversione ordine nodi
                 //---------------------------
                 n0 = n_taglio[j-1];
                 n_taglio[j-1] = n_taglio[j];
                 n_taglio[j  ] = n0;
                 ok_ordine=false;
                 break;
              }
           }while(!ok_ordine);
        }
     }
     //---------------------------------------------
     // 4. TROVATO IL PERCORSO DI TAGLIO
     // Controllo che i nodi di cui ne fanno parte
     // siano collegati tra loro nella struttura
     // Percorsi[][]
     //---------------------------------------------
     if(path_taglio_trovato==true){
        for(j=1; j<MAXPERCORSI; j++){
           if(n_taglio[j]==0) break;
           n1 = n_taglio[j-1];
           n2 = n_taglio[j  ];
           //----------------------------------------------------------
           // Verifico se nodi collegati
           //----------------------------------------------------------
           ok_ordine=false;
           for(k=0;;k++){
              if(Percorso[k][0]==0) break;
              if(Percorso[k][1]==0) break;
              if(Percorso[k][0]==n1 && Percorso[k][1]==n2){ok_ordine=true; break;}
              if(Percorso[k][0]==n2 && Percorso[k][1]==n1){ok_ordine=true; break;}
           }
           //----------------------------------------------------------
           // Se anche una sola coppia di nodi del percorso di taglio
           // non è collegata, allora scarto tutto il percorso
           //----------------------------------------------------------
           if(ok_ordine==false){path_taglio_trovato=false; break;}
        }
     }
     //---------------------------------------------
     // 5. COMPILO LE INFO DI PERCORSO TAGLIO
     // Se percorso di taglio valido, compilo i
     // dati di percorrenza
     //---------------------------------------------
     if(path_taglio_trovato==true){
        for(j=0; j<MAXPERCORSI; j++){
           if(n_taglio[j+1]==0) break;
           //---------------------------------------------------
           // Recupero distanza e rallentamento da editor nodi
           //---------------------------------------------------
           dist_taglio=ral_taglio=0;
           // 1. ricerca nel verso di percorrenza
           for(k=0; k<=MAXPUNTINODO-1; k++){
              if(N.n[n_taglio[j]]->rec[k].ps==0            ) break;
              if(N.n[n_taglio[j]]->rec[k].ps!=n_taglio[j+1]) continue;
              ral_taglio=N.n[n_taglio[j]]->rec[k].ral;
              dist_taglio=N.n[n_taglio[j]]->rec[k].dist;
              break;
           }
           // 2. ricerca nel verso opposto a quello di percorrenza
           if(dist_taglio==0){
              for(k=0; k<=MAXPUNTINODO-1; k++){
                 if(N.n[n_taglio[j+1]]->rec[k].ps==0          ) break;
                 if(N.n[n_taglio[j+1]]->rec[k].ps!=n_taglio[j]) continue;
                 ral_taglio=N.n[n_taglio[j+1]]->rec[k].ral;
                 dist_taglio=N.n[n_taglio[j+1]]->rec[k].dist;
                 break;
              }
           }
           //---------------------------------------------------
           // Se non ho trovato i dati di percorrenza mancanti
           // il percorso non è valido
           //---------------------------------------------------
           if(dist_taglio==0){path_taglio_trovato=false; break;}
           d_taglio[j].ps  = n_taglio[j+1];
           d_taglio[j].dir = dir_taglio;
           d_taglio[j].rot = rot_taglio;
           d_taglio[j].ral = ral_taglio;
           d_taglio[j].dist= dist_taglio;
        }
     }
     //--------------------------------------------------
     // 6. INSERISCO IL PERCORSO DI TAGLIO
     // Se anche i dati di percorrenza sono stati
     // calcolati correttamente vado a inserire il
     // percorso di taglio nel path originale.
     //--------------------------------------------------
     if(path_taglio_trovato==true){
        for(j=0; j<MAXPERCORSI; j++){
           if(n_taglio[j+1]==0) break;
           new_perc[id] = n_taglio[j];
           memcpy(&new_dati[id], &d_taglio[j], sizeof(DATI_PERCORSO));
           id++;
        }
        i=end_taglio;
     }
     //--------------------------------------------------
     // 7. PERCORSO DI TAGLIO NON VALIDO
     // Continuo l'analisi del percorso originale
     // incrementando l'indice
     //--------------------------------------------------
     else id++;
  }
  //--------------------------------------------------
  // Ritorno il percorso rielaborato
  //--------------------------------------------------
  memcpy(perc, &new_perc[0], sizeof(new_perc));
  memcpy(dati, &new_dati[0], sizeof(new_dati));
  return;
}


