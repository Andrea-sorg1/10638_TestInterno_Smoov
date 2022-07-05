//-----------------------------------------------------------------
//                  file     ICAM.CPP
//-----------------------------------------------------------------
//
// Questo file contiene tutte le funzioni specialòi create appositamente
// per i vari Impianti.
//---------------------------------------------------------------------------

#include "ICAM.H"
#include "Main.h"
#include "SQLDB.h"

//@@@
//#include <iostream>
//#include <windows.h>
//#include <conio.h>
//#include <cstdio.h>
#include "TcAdsDEF.h"
#include "TcAdsAPI.h"
//@@@


//  ---------------------
//   STRUTTURA NODI
//  ---------------------
//  0,1 = x,y posizione centrale nodo
//  2 = numero nodo
//  3 = occupazione agv ( numero AGV oppure libero = 0 )
//-------------------------------------------------------
extern int Punti[][5];

//---------------------------------------------------------------------------
// ----------------------------------------------
//       disegna_speciali()
//       disegni specializzati per l'impianto
// ----------------------------------------------
void disegna_speciali_ICAM()
{
  int x, y;

  // VERSI MAGAZZINI SUPERIORI
  x=Punti[119][0]-0; y=Punti[119][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[129][0]-0; y=Punti[129][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[139][0]-0; y=Punti[139][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[149][0]-0; y=Punti[149][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[159][0]-0; y=Punti[159][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[169][0]-0; y=Punti[169][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[179][0]-0; y=Punti[179][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[189][0]-0; y=Punti[189][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[199][0]-0; y=Punti[199][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[209][0]-0; y=Punti[209][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[219][0]-0; y=Punti[219][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[229][0]-0; y=Punti[229][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[239][0]-0; y=Punti[239][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  // VERSI MAGAZZINI CENTRALI
  x=Punti[116][0]-0; y=Punti[116][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[126][0]-0; y=Punti[126][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[136][0]-0; y=Punti[136][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[136][0]-0; y=Punti[136][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[146][0]-0; y=Punti[146][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[156][0]-0; y=Punti[156][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[166][0]-0; y=Punti[166][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[176][0]-0; y=Punti[176][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[186][0]-0; y=Punti[186][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[196][0]-0; y=Punti[196][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[206][0]-0; y=Punti[206][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[216][0]-0; y=Punti[216][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  x=Punti[226][0]-0; y=Punti[226][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);

  // VERSI MAGAZZINI INFERIORI
  x=Punti[172][0]-0; y=Punti[172][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40,   0, clWhite);

  x=Punti[182][0]-0; y=Punti[182][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40,   0, clWhite);

  x=Punti[192][0]-0; y=Punti[192][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40,   0, clWhite);

  x=Punti[202][0]-0; y=Punti[202][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40,   0, clWhite);

  x=Punti[212][0]-0; y=Punti[212][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40,   0, clWhite);

  x=Punti[222][0]-0; y=Punti[222][1]-0;
  FormLayout->disegna_freccia2(    x,      y, 60, 40,   0, clWhite);

  // BAIA
  x=Punti[143][0]-30; y=Punti[143][1]-3;
  FormLayout->disegna_freccia2(    x,      y, 60, 40, 180, clWhite);
  x=Punti[143][0]+30; y=Punti[143][1]+3;
  FormLayout->disegna_freccia2(    x,      y, 60, 40,   0, clWhite);
}
//---------------------------------------------------------------------------
//-------------------------------
//   ReadIni_SPEC()
//-------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN MAIN.CPP  (ReadIniProject())
//---------------------------------------------------------------------------
// Rilettura flag riservati all'impianto
//
//
void ReadIni_ICAM()
{
  int i;
  TIniFile  *IniFile;
  AnsiString Buff;
  AnsiString FIniFileName;

  FIniFileName = PathProject + PlantName + ".ini";
  IniFile      = new TIniFile(FIniFileName);
/*
  AbilitaAttesaLaserUscitaMag_ADELAIDE   = IniFile->ReadInteger("CONFIGURAZIONE",  "AbilitaAttesaLaserUscitaMag", 0);
  AbilitaLetturaDatiDaPlc_ADELAIDE       = IniFile->ReadInteger("CONFIGURAZIONE", "AbilitaLetturaDatiDaPlc",      2);
  DESTSQLdaTecno_ADELAIDE                = IniFile->ReadBool("CONFIGURAZIONE", "DESTSQLdaTecno", true);
  NoAttesaDaRicarica_ADELAIDE            = IniFile->ReadBool("CONFIGURAZIONE", "NoAttesaDaRicarica", true);
  AbilitaDMAltezzaSuScaricoAMag_ADELAIDE = IniFile->ReadBool("CONFIGURAZIONE", "AbilitaDMAltezzaSuScaricoAMag", false);
  AbilitaLetturaBassoDaPlc_ADELAIDE      = IniFile->ReadBool("CONFIGURAZIONE", "AbilitaLetturaBassoDaPlc", false);

  TimeAttesaBaiePieni_ADELAIDE           = IniFile->ReadInteger("BAIE PIENI", "TimeAttesaBaiePieni", 120);
  TimeAttesaBaiaRORO_ADELAIDE            = IniFile->ReadInteger("BAIE PIENI", "TimeAttesaBaiaRORO",  300);
  AbilitaCarWashPerMagSaturo_ADELAIDE    = IniFile->ReadBool("BAIE PIENI",    "AbilitaCarWashPerMagSaturo", false);
*/

  delete IniFile;
  return;
}
//---------------------------------------------------------------------------
//-------------------------------
//   WriteIni_SPEC()
//-------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN MAIN.CPP  (Close())
//---------------------------------------------------------------------------
// Scrittura flag riservati all'impianto
//
void WriteIni_ICAM()
{
  int i;
  TIniFile *IniFile;
  AnsiString Buff;
  AnsiString FIniFileName;

  FIniFileName = PathProject + PlantName + ".ini";
  IniFile      = new TIniFile(FIniFileName);

/*
  IniFile->WriteInteger("CONFIGURAZIONE", "AbilitaAttesaLaserUscitaMag", AbilitaAttesaLaserUscitaMag_ADELAIDE);
  IniFile->WriteInteger("CONFIGURAZIONE", "AbilitaLetturaDatiDaPlc",     AbilitaLetturaDatiDaPlc_ADELAIDE);
  IniFile->WriteBool("CONFIGURAZIONE", "DESTSQLdaTecno",                 DESTSQLdaTecno_ADELAIDE);
  IniFile->WriteBool("CONFIGURAZIONE", "NoAttesaDaRicarica",             NoAttesaDaRicarica_ADELAIDE);
  IniFile->WriteBool("CONFIGURAZIONE", "AbilitaDMAltezzaSuScaricoAMag",  AbilitaDMAltezzaSuScaricoAMag_ADELAIDE);
  IniFile->WriteBool("CONFIGURAZIONE", "AbilitaLetturaBassoDaPlc",       AbilitaLetturaBassoDaPlc_ADELAIDE);

  IniFile->WriteInteger("BAIE PIENI",     "TimeAttesaBaiePieni",         TimeAttesaBaiePieni_ADELAIDE);
  IniFile->WriteInteger("BAIE PIENI",     "TimeAttesaBaiaRORO",          TimeAttesaBaiaRORO_ADELAIDE);
  IniFile->WriteInteger("BAIE PIENI",     "AbilitaCarWashPerMagSaturo",  AbilitaCarWashPerMagSaturo_ADELAIDE);
*/
  delete IniFile;
  return;
}
//---------------------------------------------------------------------------
//-------------------------------
//   VerificaInputMAG_ICAM()
//-------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN MAGWIN  (Salva())
//---------------------------------------------------------------------------
// Forzature editor magazzini legate ai singoli impianti ALL_GLASS
//
void VerificaInputMAG_ICAM(struct MAG *mag)
{
/*
  int i, j, k;
  struct NOD nod;
  char buffer[200]="";
  int  NodoOut, NodoPos, av,ind;
  bool almeno_un_pal_spostato;
  short int Tgv, Mac, Mag, Bai, Stz, Pos, Pin, Forc, NPal;

  //-------------------------------------------
  // TUTTI I MAGAZZINI SONO PER PRODOTTO
  // e non è prevista nè gestione emergenza
  //-------------------------------------------
  if(mag->Tipo!=MG_ESCLUSO && mag->Tipo!=MG_TEMPORANEO) mag->Tipo = MG_PROD_TUTTI;
  //----------------------------------------------------
  // Ammessi come magazzini temporani solo quelli
  // che contengono pallet con loc_sorg diversa da
  // quella attuale, ma appartenete allo stesso gruppo
  //----------------------------------------------------
  if(mag->Tipo==MG_TEMPORANEO){
     almeno_un_pal_spostato=false;
     if(mag->NumCestoni!=0){
        for(i=1; i<=MAXTERRAMAG; i++){
           for(j=0; j<MAXPIANIMAG; j++){
              if(mag->Pos[i].Presenza[j]==false) continue;
              for(k=0; k<=MAXUDCBOX; k++){
                 if(mag->Pos[i].Box[j].Prd[k].loc_prov==0) continue;
                 //------------------------------------------------------------
                 // Verifico se la locazione di provenienza appartiene allo
                 // stesso gruppo del magazzino considerato
                 //------------------------------------------------------------
                 Tgv=Mac=Mag=Bai=Stz=Pos=Pin=Forc=NPal=0;
                 B.LocationToStruct(mag->Pos[i].Box[j].Prd[k].loc_prov, &Tgv, &Mac, &Mag, &Bai, &Stz, &Pos, &Pin, &Forc, &NPal);
                 if(Mag<1                         ) continue;
                 if(Mag>TOTMAG                    ) continue;
                 if(Mag==mag->Num                 ) continue;
                 if(mag->Gruppo!=M.m[Mag]->Gruppo ) continue;  // spostato da altro gruppo
                 almeno_un_pal_spostato=true;
                 break;
              }
              if(almeno_un_pal_spostato==true) break;
           }
           if(almeno_un_pal_spostato==true) break;
        }
     }
     if(almeno_un_pal_spostato==false) mag->Tipo = MG_PROD_TUTTI;
  }
*/

  return;
}
//---------------------------------------------------------------------------
//-------------------------------
//   VerificaInputBOX_ICAM()
//-------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN BOXWIN  (Salva())
//---------------------------------------------------------------------------
// Forzature editor box legate ai singoli impianti ALL_GLASS
//
void VerificaInputBOX_ICAM(struct BOX *box)
{
/*
  //-------------------------------------------------
  // In ADELAIDE SOLO PALLE BASSI E PIENI
  //-------------------------------------------------
  box->Basso=true;
  if(box->StatoLav==ST_BOX_VUOTI || box->Codice==0){
     Application->MessageBox("Please put a correct code assignment\n...Empty pallet or code 0 not admitted!", MESS[99], MB_OK | MB_ICONINFORMATION);
  }
  if(box->GruppoDest==NN) box->GruppoDest=GRP_MAG_TEMP_ADELAIDE;
*/
  return;
}
//---------------------------------------------------------------------------
//---------------------------------
// AdjustDMStartMission_ICAM()
//---------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN AGV.CPP  (trs_mission())
//---------------------------------------------------------------------------
// Aggiusta il DM di start missione (D3000...D3060) con gestioni specializzate
// per singolo impianto
//
void AdjustDMStartMission_ICAM(short int num_agv, WORD *dm)
{
/*
  char all_mess[101]="";
  short int piano;
  short int num_mag, num_pos;
  short int altezza, tipo_pal;

  //-------------------------------------------
  // Solo per TGV con missione programmata
  //-------------------------------------------
  if(AGV[num_agv]->mission.pend<1        ) return;
  if(AGV[num_agv]->mission.pend>TOTPUNTI ) return;

  //-------------------------------------------
  // CALCOLO ALTEZZA DI CARICO CATASTE PALLET
  //-------------------------------------------
  if(AGV[num_agv]->mission.Typ==MISS_CARICO_DA_MAGAZZINO){
     num_mag=num_pos=0;
     M.test_punto_presente_mag(all_mess, AGV[num_agv]->mission.pend, &num_mag, &num_pos);
     //-----------------------------------------------------
     // Verifico se è previsto il prelievo al piano alto
     //-----------------------------------------------------
     piano = dm[30];  // DM3030 - Piano di prelievo/deposito
     //-----------------------------------------------------
     // Verifico che il carico sia da un magazzino di vuoti
     //-----------------------------------------------------
     if(num_mag>0 && num_mag<=TOTMAG && piano>0 && M.m[num_mag]->Stato==MG_ST_VUOTI){
        //-----------------------------------------------------
        // In base al tipo di pallet impostato in magazzino
        // recupero il parametro di altezza del singolo pal
        //-----------------------------------------------------
        tipo_pal = M.m[num_mag]->TipoBox;
        //-----------------------------------------------------
        // Calcolo l'altezza reale a cui deve andare il TGV
        //-----------------------------------------------------
        altezza  = (M.TipiBox[tipo_pal].Altez*(NumPalletDaDecatastare_REIMS-1))*piano;
        //-----------------------------------------------------
        // Aggiusto il DM di altezza TGV (DM3037)
        //-----------------------------------------------------
        dm[37] = altezza;
     }
  }
*/
  return;
}
//---------------------------------------------------------------------------
// -----------------------------------
//  load_chiamate_baie_ICAM()
// -----------------------------------
//  Personalizzazione per l'impianto OI-REIMS (ALLGLASS)
//---------------------------------------------------------------------------
// UTILIZZATA IN PROCESS.CPP  (load_chiamate_baie())
//---------------------------------------------------------------------------
// E' una funzione che legge i segnali e i dati da PLC SIEMENS S71500 (ETH)
// con le DB di scambio defite per l'impianto REIMS
//
int load_chiamate_baie_ICAM(char *all_mess)
/********************************************/
{
  int i;
  AmsAddr  Addr;
  PAmsAddr pAddr = &Addr;
  unsigned char BeckhoffIP1[6];
  int PortBeckhoff1 = 851;   // 801
  int TimeOuttBeckhoff1 = 300;
  long nErr, nPort;
  long err_Plc;
  long handle;
  unsigned long lHdlVar;
  long openp=0;
  static int  pippo=101;
  char szVarSta[]={"Var_Globali.sTxSta"};
  char szVarCmd[]={"Var_Globali.sRxCmd"};
  char szVarMis[]={"Var_Globali.sRxMis"};

  //-------------------------------------
  // Comunicazione disattivata
  //-------------------------------------
  if(break_Com[COM2]==true) return 0;

  BeckhoffIP1[0]=172;
  BeckhoffIP1[1]=16;
  BeckhoffIP1[2]=36;
  BeckhoffIP1[3]=53;
  BeckhoffIP1[4]=1;
  BeckhoffIP1[5]=1;

  pAddr->port = PortBeckhoff1;
  pAddr->netId.b[0]=BeckhoffIP1[0];
  pAddr->netId.b[1]=BeckhoffIP1[1];
  pAddr->netId.b[2]=BeckhoffIP1[2];
  pAddr->netId.b[3]=BeckhoffIP1[3];
  pAddr->netId.b[4]=BeckhoffIP1[4];
  pAddr->netId.b[5]=BeckhoffIP1[5];
  AdsSyncSetTimeout(TimeOuttBeckhoff1);

  openp=AdsPortOpen();
  //--------------------------------------------------------
  // Leggo Stato ASRV <- Plc BECKHOFF
  //--------------------------------------------------------
  err_Plc = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, sizeof(szVarSta), szVarSta );
  err_Plc = AdsSyncReadReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(ASRV_STATO), &AGV[1]->asrv_stato);
  if(err_Plc){
     sprintf(all_mess, "(Stato_ASRV) Errore %ld lettura %s PLC BECKHOFF", err_Plc, szVarSta);
     //AdsPortClose();
     return 1;
  }
  //--------------------------------------------------------
  // Leggo Comandi ASRV <- Plc BECKHOFF
  //--------------------------------------------------------
  err_Plc = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, sizeof(szVarCmd), szVarCmd );
  err_Plc = AdsSyncReadReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(ASRV_COMANDI), &AGV[1]->asrv_comandi);
  if(err_Plc){
     sprintf(all_mess, "(Comandi_PC) Errore %ld lettura %s PLC BECKHOFF", err_Plc, szVarCmd);
     //AdsPortClose();
     return 1;
  }
  //--------------------------------------------------------
  // Leggo Missione ASRV <- Plc BECKHOFF
  //--------------------------------------------------------
  err_Plc = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, sizeof(szVarMis), szVarMis );
  err_Plc = AdsSyncReadReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(ASRV_MISSION), &AGV[1]->asrv_mission);
  if(err_Plc){
     sprintf(all_mess, "(Missione_PC) Errore %ld lettura %s PLC BECKHOFF", err_Plc, szVarMis);
     //AdsPortClose();
     return 1;
  }




  //--------------------------------------------------------
  // Scrivo qualcosa su Stato ASRV
  //--------------------------------------------------------
  if( N.n[pippo]->num!=0){
     AGV[1]->asrv_stato.pos = pippo;
     AGV[1]->stato.pos = pippo;
  }
  pippo++;
  if(pippo>240) pippo=101;

  if( AGV[1]->asrv_comandi.wdog != AGV[1]->asrv_stato.wdog  ){
     AGV[1]->asrv_comandi.wdog = AGV[1]->asrv_stato.wdog;
  }
  AGV[1]->asrv_mission.aux1++;
  AGV[1]->asrv_mission.step[500].dist++;


  err_Plc = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, sizeof(szVarSta), szVarSta );
  err_Plc = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(ASRV_STATO), &AGV[1]->asrv_stato);
  if(err_Plc){
     sprintf(all_mess, "(Stato_ASRV) Errore %ld scrittura %s PLC BECKHOFF", err_Plc, szVarSta);
     //AdsPortClose();
     return 1;
  }

  err_Plc = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, sizeof(szVarCmd), szVarCmd );
  err_Plc = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(ASRV_COMANDI), &AGV[1]->asrv_comandi);
  if(err_Plc){
     sprintf(all_mess, "(Comandi_PC) Errore %ld scrittura %s PLC BECKHOFF", err_Plc, szVarCmd);
     //AdsPortClose();
     return 1;
  }

  err_Plc = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, sizeof(szVarMis), szVarMis );
  err_Plc = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(ASRV_MISSION), &AGV[1]->asrv_mission);
  if(err_Plc){
     sprintf(all_mess, "(Missione_PC) Errore %ld scrittura %s PLC BECKHOFF", err_Plc, szVarMis);
     //AdsPortClose();
     return 1;
  }
  AdsPortClose();





/*
  int  msec;
  int  r, r2, v;
  int  i, j, k, z;
  int  errsql, err;
  int  chiamata;
  WORD dm[501];
  WORD ds[101];
  BYTE B1, B2;
  char buffer[151]="";
  char buffer2[200]="";
  static WORD old_dm[501];
  WORD NumCanali      = 0;
  WORD NumPlc         = 0;
  WORD CanaleIniziale = 0;
  bool pres1, pres2;
  bool almeno_un_box_a_terra;
  bool Ok, cambio_stato;
  short int max_height;
  short int count_in_presa, sh;
  bool trovato_agv, Ch_On;
  bool reset_richiesta_svuotamento;
  char HUcode[30];     // HU + Batch
  char Batch[30], PalNum[30], Shift[30];
  struct DATI_PRD UdcTmp[MAXUDCIMPIANTO];
  unsigned int LocS, LocE;
  char UdcS[30], UdcE[30];
  short int num_mag, altro_agv;
  static int CntRichiesteSvuotamento=4;
  bool EseguiRichiesteSvuotamento=false;
  //###############################################################
  static bool PrimaScansione=true;
  static bool OldChiamata[TOTBAIE+1];
  static short int OldCodice[TOTBAIE+1];
  static char OldUdc[TOTBAIE+1][MAXUDCBOX+1][21];
  char filename[81]="";
  char messaggio[200]="";
  SYSTEMTIME Time1;
  SYSTEMTIME Time2;

  //---------------------------------------------------------------
  // Log Variazione Codice con chiamata attiva
  //---------------------------------------------------------------
  if( PrimaScansione==true ){
     PrimaScansione=false;
     memset( OldCodice,   0, sizeof(OldCodice) );
     memset( OldChiamata, 0, sizeof(OldChiamata) );
     memset( OldUdc,      0, sizeof(OldUdc)    );
  }
  for(i=1;i<=TOTBAIE;i++){
     //-----------------------------------------------
     // Con fronte chiamata memorizza codice
     //-----------------------------------------------
     if(P.ba[i]->Chiamata==true && OldChiamata[i]==false){
        OldCodice[i] = P.ba[i]->Codice[0];
     }
     //-----------------------------------------------------
     // Con chiamata attiva se varia Codice allora esegue Log
     //-----------------------------------------------------
     if(OldChiamata[i]==true && OldCodice[i] != P.ba[i]->Codice[0]){
        sprintf( messaggio, "Bay %d, Change Code from <%d> to <%d> having call active!", i, P.ba[i]->Codice[0], OldCodice[i] );
        file_cronologico_generico( "LOGSOURCE_ADELAIDE.TXT", messaggio, 50000L );
        OldCodice[i] = P.ba[i]->Codice[0];
     }
     //-----------------------------------------------
     // Aggiorna OldChiamata + azzera se chiamata=0
     //-----------------------------------------------
     if(P.ba[i]->Chiamata==false ){
        OldCodice[i]=0;
     }
     OldChiamata[i] = P.ba[i]->Chiamata;
  }
  //###############################################################


  //---------------------------------------------------------------
  // Se ho chiamata attiva azzero attesa chiamata
  // a prescindere dalla comunicazione in Break (per simulazione)
  //---------------------------------------------------------------
  for(i=1;i<=TOTBAIE;i++){
     if(P.ba[i]->Chiamata==true) AttesaBaieGiaFatta[i]=false;
     if(P.ba[i]->GruppoDest==NN) P.ba[i]->GruppoDest = GRP_MAG_TEMP_ADELAIDE;
  }

  //----------------------------------------------------------------
  // Comunicazione disabilitata --> skip!
  //	PLC Baie = COM2 (Omron), Indice=0, Baia P.1,2,3
  //----------------------------------------------------------------
  if(break_Com[COM2]==true ) return 0;

  err=errsql=0;
  cambio_stato=false;
  strcpy( all_mess, "" );
  //================================================================================
  // Rx BAIE PIENI (1,2,3) (D0 - D240)
  // Baia 1 = Only Emergency Pallet
  // D000     (d[0]            ) = Check Rete
  // D001.0   (d[1] [81] [161]) = Automatico
  // D001.1   (d[1] [81] [161]) = Blocco
  // D001.2   (d[1] [81] [161]) = Chiamata (presa doppia)
  // D001.3   (d[1] [81] [161]) = Ok Ingresso
  // D001.4   (d[1] [81] [161]) = Pres. Posizione 1
  // D001.5   (d[1] [81] [161]) = Pres. Posizione 2
  // D001.6   (d[1] [81] [161]) =
  // D001.7   (d[1] [81] [161]) = Doppio Pallet Pos.3 (overlapped)
  // D001.8   (d[1] [81] [161]) = Doppio Pallet Pos.4 (overlapped)
  //------------------ Pallet 1
  // D002-011 (d[2] [82] [162]) = HU Code (20 char)
  // D012     (d[12][92] [172]) = Source Line     (>>Baia 1<< sempre=99, Odd Pallet)
  //                                               (>>Baie 2/3<< 50,51,52,XX)
  // D013     (d[13][93] [173]) = Height mm
  // D014     (d[14][94] [174]) = Pallet Type (0=800x1200, 1=1000x1200..)
  // D015     (d[15][95] [175]) = Height Type (0=High, 1=Low..)
  // D016     (d[16][96] [176]) = Carwash (1=Not Ok)   (>>Baia 1<< sempre=1)
  //                                                    (>>Baia 2/3<< 0=Ok, 1=Non Ok, 2=Quarantine, 3=Resort)
  // D017     (d[17][97] [177]) = Overlap Enabled (1=Yes, 0=No) (>>Baia 1<< sempre=0, Odd Pallet)
  // D018     (d[18][98] [178]) = Shift (1=Day, 2=Afternoon, 3=Night)  //###
  // D019-021 (d[19][99] [179]) = Free
  //------------------ Pallet 2
  // D022-031 (d[22][102] [182]) = HU Code (20 char)
  // D032     (d[32][112] [192]) = Source Line     (>>Baia 1<< sempre=99, Odd Pallet)
  //                                               (>>Baie 2/3<< 50,51,52,XX)
  // D033     (d[33][113] [193]) = Height mm
  // D034     (d[34][114] [194]) = Pallet Type (0=800x1200, 1=1000x1200..)
  // D035     (d[35][115] [195]) = Height Type (0=High, 1=Low..)
  // D036     (d[36][116] [196]) = Carwash (1=Not Ok)   (>>Baia 1<< sempre=1)
  //                                                    (>>Baia 2/3<< 0=Ok, 1=Non Ok, 2=Quarantine, 3=Resort)
  // D037     (d[37][117] [197]) = Overlap Enabled (1=Yes, 0=No) (>>Baia 1<< sempre=0, Odd Pallet)
  // D038     (d[38][118] [198]) = Shift (1=Day, 2=Afternoon, 3=Night)  //###
  // D039-041 (d[39][119] [199]) = Free
  //------------------ Pallet 3
  // D042-051 (d[42][122] [202]) = HU Code (20 char)
  // D052     (d[52][132] [212]) = Source Line     (>>Baia 1<< sempre=99, Odd Pallet)
  //                                               (>>Baie 2/3<< 50,51,52,XX)
  // D053     (d[53][133] [213]) = Height mm
  // D054     (d[54][134] [214]) = Pallet Type (0=800x1200, 1=1000x1200..)
  // D055     (d[55][135] [215]) = Height Type (0=High, 1=Low..)
  // D056     (d[56][136] [216]) = Carwash (1=Not Ok)   (>>Baia 1<< sempre=1)
  //                                                    (>>Baia 2/3<< 0=Ok, 1=Non Ok, 2=Quarantine, 3=Resort)
  // D057     (d[57][137] [217]) = Overlap Enabled (1=Yes, 0=No) (>>Baia 1<< sempre=0, Odd Pallet)
  // D058     (d[58][138] [218]) = Shift (1=Day, 2=Afternoon, 3=Night)  //###
  // D059-061 (d[59][139] [219]) = Free
  //------------------ Pallet 4
  // D062-071 (d[62][142] [222]) = HU Code (20 char)
  // D072     (d[72][152] [232]) = Source Line     (>>Baia 1<< sempre=99, Odd Pallet)
  //                                               (>>Baie 2/3<< 50,51,52,XX)
  // D073     (d[73][153] [233]) = Height mm
  // D074     (d[74][154] [234]) = Pallet Type (0=800x1200, 1=1000x1200..)
  // D075     (d[75][155] [235]) = Height Type (0=High, 1=Low..)
  // D076     (d[76][156] [236]) = Carwash (1=Not Ok)   (>>Baia 1<< sempre=1)
  //                                                    (>>Baia 2/3<< 0=Ok, 1=Non Ok, 2=Quarantine, 3=Resort)
  // D077     (d[77][157] [237]) = Overlap Enabled (1=Yes, 0=No) (>>Baia 1<< sempre=0, Odd Pallet)
  // D078     (d[78][158] [238]) = Shift (1=Day, 2=Afternoon, 3=Night)  //###
  // D079-080 (d[79][159] [239]) = Free
  //
  //--------------------------------------------------------
  // ### Scomposizione HU Code ###
  // 1234567890
  // 1     = Source Line (0=AD50, 1=AD51, 2=AD52, 3=AD21 etc.)   -- OLD
  // 2345  = Pallet Number (always 4 digits)                     -- OLD
  // 67890 = Batch Number (always 5 digits)                      -- OLD
  //
  //----------------------------------------------------
  // Andrea 16/10/2018
  // 1234567890ABCDEFGHIL
  // 1234567890 = Pallet Number (HU=Udc)(10 digits)
  // ABCDEFGHIL = Batch Number          (10 digits)
  //----------------------------------------------------

  //--------------------------------------------------------
  // Gestione Svuotamento Mag da OP:
  // D4501..D4532: OP -> 1 (richiesta svuotamento), PC -> 0  (svuotamento completato)
  // D4501 = Svuotamento Mag. Shipment 1    --> Mag.1
  // D4502 = Svuotamento Mag. Shipment 2    --> Mag.2
  // D4518 = Svuotamento Mag. Shipment 18   --> Mag.18
  // D4521 = Svuotamento Mag. Resort 1      --> Mag.21
  // D4522 = Svuotamento Mag. Resort 2      --> Mag.22
  // D4528 = Svuotamento Mag. Resort 8      --> Mag.28
  //================================================================================
  //------------------------------------------------------
  // Verifico se la macchina considerata è abbinata a
  // un PLC OMRON
  // UNICO PLC PER TUTTE LE BAIE
  // (default uso quello della baia "1");
  //------------------------------------------------------
  for(i=1; i<=3; i++){
     NumPlc=P.ba[i]->Plc;
     if(NumPlc==PLCTERRA) continue;     // NumPlc "0" = PLC TERRA
     break;
  }
  //------------------------------------------------------
  // Lettura STATO
  //------------------------------------------------------
  memset(&dm, 0, sizeof(dm));
  CanaleIniziale = 4000;
  NumCanali      = 305;     // Dm4000..dm4305
  err = OM.ld_canale(&dm[0], NumPlc, "RD", CanaleIniziale, NumCanali, all_mess);

  //######################################################
  //------------------------------------------------------
  // Lettura Richieste Svuotamento (ogni 5 cicli)
  //------------------------------------------------------
  EseguiRichiesteSvuotamento=false;
  memset(&ds, 0, sizeof(ds));
  if(!err){
     CntRichiesteSvuotamento++;
     if( CntRichiesteSvuotamento>=5 ){
        CntRichiesteSvuotamento=0;
        EseguiRichiesteSvuotamento=true;
        //------------------------------
        CanaleIniziale = 4500;
        NumCanali      = 30;     // Dm4500..dm4529
        err = OM.ld_canale(&ds[0], NumPlc, "RD", CanaleIniziale, NumCanali, all_mess);
        //------------------------------
     }
  }
  //######################################################

  if( err ){
     sprintf(all_mess, "(PLC%02d) - %s", NumPlc, buffer);
  }
  else{
     //-------------------------------------------------------
     // recupero WatchDog poi azzero per vedere solo le
     // variazioni Baia
     //
     // WatchDog BAIE PRELIEVO ==> dm[  0] = D4000
     // WatchDog BAIE DEPOSITO ==> dm[300] = D4300
     //
     //-------------------------------------------------------
     WatchDog = (short int)(dm[0]);
     dm[  0]=0;
     dm[300]=0;
     //------------------------------------------------------
     // Verifico se c'è una variazione di stato stazione
     //------------------------------------------------------
     if(memcmp(&dm[0], &old_dm[0], sizeof(dm))!=0) cambio_stato=true;
     //------------------------------------------------------
     // Verifico se c'è una variazione di stato stazione
     //------------------------------------------------------
     for(i=0; i<TOTBAIE; i++){        // Baie 1,2,3
        //----------------------------------------------
        // Scarto le macchine senza PLC
        //----------------------------------------------
        if(P.ba[i+1]->EsclComunic==true     ) continue;
        if(P.ba[i+1]->Plc==0                ) continue;
        //----------------------------------------------
        // BAIA ASSEGNATA A SCAFFALE
        //----------------------------------------------
        if(P.ba[i+1]->Tipo==BA_NONE         ) continue;
        //----------------------------------------------
        // (1) - BAIE DI PRELIEVO (1...3)
        //----------------------------------------------
        if(P.ba[i+1]->Tipo==BA_PRELIEVO && P.ba[i+1]->Num>=1 && P.ba[i+1]->Num<=3){
           //----------------------------------------------
           // Chiamata + consenso all'ingresso
           //----------------------------------------------
           P.ba[i+1]->Automatico = TestBit((char*) &dm[1+(i*80)], 0);   // D001.00 / D81.00 / D161.00
           P.ba[i+1]->Blocco     = TestBit((char*) &dm[1+(i*80)], 1);   // D001.01 / D81.01 / D161.01
           P.ba[i+1]->OkIng      = TestBit((char*) &dm[1+(i*80)], 3);   // D001.03 / D81.03 / D161.03
           //------------------------------------------------------
           // GESTIONE TGV IN INGOMBRO
           // Non aggiorno le rulliere su cui è presente il TGV
           //------------------------------------------------------
           if(P.ba[i+1]->TgvIng && !P.ba[i+1]->OkForcheAlte) continue;
           //--------------------------------------------------
           // Memorizzo data/ora chiamata come data ora di
           // carico
           //--------------------------------------------------
           P.ba[i+1]->Chiamata = TestBit((char*) &dm[1+(i*80)], 2);   // D001.02 / D81.02 / D161.02
           //------------------------------------------------
           // Con la chiamata alta azzero la memoria di
           // TGV in attesa per baia
           //------------------------------------------------
           if(P.ba[i+1]->Chiamata==true) AttesaBaieGiaFatta[i+1]=false;
           //----------------------------------------------
           // Composizione presa
           //----------------------------------------------
           P.ba[i+1]->Presa[0][0][0]   = TestBit((char*) &dm[1+(i*80)], 4);   // D001.04 / D81.04 / D161.04 - pres.1
           P.ba[i+1]->Presa[0][1][0]   = TestBit((char*) &dm[1+(i*80)], 5);   // D001.05 / D81.05 / D161.05 - pres.2
           P.ba[i+1]->Presa[0][0][1]   = TestBit((char*) &dm[1+(i*80)], 7);   // D001.07 / D81.07 / D161.07 - pres.3
           P.ba[i+1]->Presa[0][1][1]   = TestBit((char*) &dm[1+(i*80)], 8);   // D001.08 / D81.08 / D161.08 - pres.4
           //----------------------------------------------
           // DATI PRODOTTO (4 aree) -> Prd[ MAXUDCBOX+1 ]
           // MAXUDCBOX = POSINPRESA*PIANIINPRESA (2*2=4)
           // Compilo [0][1], [2][3]
           //----------------------------------------------
           r=-1;
           max_height=0;
           for( r2=0; r2<MAXUDCBOX; r2++ ){
              r=r2;    // 0..3
              //----------------------------------------------------------
              // HUCode, stringa 20 char
              // Scomposizione HUCode: 0123456789
              // 0     = Source Line (0=AD50, 1=AD51, 2=AD52, 3=AD21 etc.)   -- OLD
              // 1234  = Pallet Number (always 4 digits)                     -- OLD
              // 56789 = Batch Number (always 5 digits)                      -- OLD
              //----------------------------------------------------
              // Andrea 16/10/2018
              // 1234567890ABCDEFGHIL       (20 digits = 10 word)
              // 1234567890 = Pallet Number (HU=Udc) (10 digits)
              // ABCDEFGHIL = Batch Number           (10 digits)
              //----------------------------------------------------------
              strcpy( HUcode, "" ); k=0;
              for(v=2;v<12;v++){
                 HUcode[k]= (char)((dm[v+(r*20)+(i*80)]>>8)&0x00ff);  // D002,D022,D042,D062.. / D82,D102,D122,D142..
                 if(HUcode[k]==0 ) break;
                 k++;
                 HUcode[k]= (char)((dm[v+(r*20)+(i*80)])&0x00ff);     // D002,D022,D042,D062.. / D82,D102,D122,D142..
                 if(HUcode[k]==0 ) break;
                 k++;
              }
              HUcode[k]=0;
              //----------------------------------------------------------
              // Pallet Number (Udc), ricavato da HUCode
              //----------------------------------------------------------
              strcpy( PalNum, "" );
              memcpy( &PalNum[0], &HUcode[0], 10 ); PalNum[10]=0;
              strcpy( P.ba[i+1]->BoxB.Prd[r2].Udc, PalNum );           // char   Udc[21];   ok
              //----------------------------------------------------------
              // Batch (batch), ricavato da HU Code
              //----------------------------------------------------------
              strcpy( Batch, "" );
              memcpy( &Batch[0], &HUcode[10], 10); Batch[10]=0;
              //----------------------------------------------------------
              // C.9207: HU composto da SKU anzichè lotto.
              //----------------------------------------------------------
              if( AbilitaLetturaDatiDaPlc_ADELAIDE==1 ){
                 strcpy( P.ba[i+1]->BoxB.Prd[r2].batch, Batch );          // char   batch[11];  ok
              }
              if( AbilitaLetturaDatiDaPlc_ADELAIDE==2 ){
                 strcpy( P.ba[i+1]->BoxB.Prd[r2].code,  Batch );          // char   batch[11];  ok
              }
              //----------------------------------------------------------
              // Source Line
              //----------------------------------------------------------
              P.ba[i+1]->BoxB.Prd[r2].source = (short int)(dm[12+(r*20)+(i*80)]);   // D012,D032,D052,D072.. / D82..
              //----------------------------------------------------------
              // LOG "HU" letti da PLC
              //----------------------------------------------------------
              if(P.ba[i+1]->Chiamata==true){
                 sprintf(filename, "LOGPLCB%d.TXT", P.ba[i+1]->Num);
                 sprintf(messaggio, "%02d (%s) ID: %s - SKU:%s - Lin:%02d", r2, HUcode, PalNum, Batch, P.ba[i+1]->BoxB.Prd[r2].source);
                 file_cronologico_generico(filename, messaggio, 90000L);
              }
              //----------------------------------------------------------
              // Shift
              // (1="DAY", 2="AFT", 3="NIGHT")
              //----------------------------------------------------------
              sh = (short int)(dm[18+(r*20)+(i*80)]);                  // D018,D038,D058,D078.. / D88..
              switch( sh ){
                 case 1:  strcpy( Shift, "DAY" ); break;
                 case 2:  strcpy( Shift, "AFT" ); break;
                 case 3:  strcpy( Shift, "NIGHT" ); break;
                 default: strcpy( Shift, "" ); break;
              }
              strcpy( P.ba[i+1]->BoxB.Prd[r2].shift, Shift );          // char   shift[11];  ok
              //----------------------------------------------------------
              // Height
              //----------------------------------------------------------
              P.ba[i+1]->BoxB.Prd[r2].height = (short int)(dm[13+(r*20)+(i*80)]);   // D013,D033,D053,D073.. / D83..
              //----------------------------------------------------------
              // Se abilitato SQL recupera Prd da Dbase
              //----------------------------------------------------------
              if(break_Com[COM3]==false && GestioneUdc>0 && atoi(PalNum)>0 && strcmp(OldUdc[i+1][r2], P.ba[i+1]->BoxB.Prd[r2].Udc)!=0){
                 errsql=0;
                 LocS=LocE=0;
                 strcpy( UdcS, PalNum );    // Udc = PalNum
                 strcpy( UdcE, "" );
                 memset( &UdcTmp[0], 0, sizeof( UdcTmp ));

                 msec=0;
                 GetLocalTime( &Time1 );

                 errsql = load_udc_from_SQL_ICAM(CMD_RD_SQL_ONE_UDC, &UdcTmp[0], LocS, LocE, UdcS, UdcE, all_mess);

                 GetLocalTime( &Time2 );
                 msec = (int) (Time2.wSecond - Time1.wSecond);
                 if( msec<0 ) msec+=60;
                 msec = msec * 1000;
                 msec = msec + (int) (Time2.wMilliseconds - Time1.wMilliseconds);
                 sprintf(all_mess, "UDC:%s, Time:%dmsc", UdcS, msec);
                 file_cronologico_generico("LOGSQLB.txt", all_mess, 50000L);

                 if(errsql==0){
                    memcpy( &P.ba[i+1]->BoxB.Prd[r2], &UdcTmp[0], sizeof( DATI_PRD ));
                    //----------------------------------------------------------------------
                    // Memorizzo dati udc letti
                    //----------------------------------------------------------------------
                    strcpy(OldUdc[i+1][r2],  P.ba[i+1]->BoxB.Prd[r2].Udc);
                    //----------------------------------------------------------------------
                    // Andrea 16/10/2018
                    // ripristina i dati letti da PLC: PalNum (UDC), Batch, Shift
                    //
                    // C.9207: HU composto da SKU anzichè lotto.
                    //
                    //----------------------------------------------------------------------
                    strcpy( P.ba[i+1]->BoxB.Prd[r2].Udc,   PalNum );         // char   Udc[21];  ok
                    strcpy( P.ba[i+1]->BoxB.Prd[r2].shift, Shift  );         // char   shift[11];  ok
                    if( AbilitaLetturaDatiDaPlc_ADELAIDE==1 ){
                       strcpy( P.ba[i+1]->BoxB.Prd[r2].batch, Batch );       // char   batch[11];  ok
                    }
                    if( AbilitaLetturaDatiDaPlc_ADELAIDE==2 ){
                       strcpy( P.ba[i+1]->BoxB.Prd[r2].code,  Batch );       // char   batch[11];  ok
                    }
                    //-----------------------------------------------------------------------
                    // Associazione time di carico
                    //-----------------------------------------------------------------------
                    P.ba[i+1]->BoxB.TimeC = P.ba[i+1]->BoxB.Prd[r2].time;
                    //-----------------------------------------------------------------------
                    // Set Locazione
                    //-----------------------------------------------------------------------
                    P.ba[i+1]->BoxB.Prd[r2].location = B.StructToLocation(0, 0, 0, i+1, 0, 0, 0, N_FORCA_B, r2);
                    errsql = B.StructToUdcTmp(CMD_UDC_SET_LOC, 0, 0, 0, P.ba[i+1]->Num, UDC_ALL_STZ, UDC_ALL_POS, UDC_ALL_LEV, N_FORCA_AB, all_mess);
                 }
                 if(errsql!=0){
                    sprintf( buffer2, "%s - Udc=<%s>", all_mess, HUcode );
                    P.error_trasmit(0, buffer2);
                    if(GestioneUdc==1) return 1;
                 }
              }
              //----------------------------------------------------------------------
              // GESTIONE ALTEZZA
              //----------------------------------------------------------------------
              if(P.ba[i+1]->BoxB.Prd[r2].height>max_height) max_height=P.ba[i+1]->BoxB.Prd[r2].height;
           }
           //-------------------------------------------------------------
           // I dati Generali li prendiamo dalla prima area
           //-------------------------------------------------------------
           P.ba[i+1]->Codice[0]  = (short int)(dm[12+(i*80)]);     // D012 / D92 / D172   (50,51,52,99)
           P.ba[i+1]->Altezza[0] = (short int)(dm[13+(i*80)]);     // D013 / D93 / D173   (mm)
           P.ba[i+1]->TipoBox[0] = (short int)(dm[14+(i*80)]);     // D014 / D94 / D174   (0=800x1200, 1=1000x1200..)
           //-------------------------------------------------------------
           // Se la presa ha almeno un pallet alto definisco l'altezza
           // massima come
           // il valore di altezza che viene passato al PLC TGV in fase di
           // scarico
           //-------------------------------------------------------------
           if(P.ba[i+1]->Presa[0][1][1]==true || P.ba[i+1]->Presa[0][0][1]==true){
              max_height=max_height*2;
           }
           //-------------------------------------------------------------
           // AL-24/10/2018 Bargi chiede di rendere abilitabile/disabilit.
           // il valore di altezza che viene passato al PLC TGV in fase di
           // scarico
           //-------------------------------------------------------------
           if(AbilitaDMAltezzaSuScaricoAMag_ADELAIDE==false) P.ba[i+1]->Altezza[0]=0;
           else{
              if(P.ba[i+1]->Altezza[0]<max_height) P.ba[i+1]->Altezza[0]=max_height;
           }
           //-------------------------------------------------------------
           // Andrea 20/09/2018
           // Se TipoBox fuori range assegna=0 altrimenti va in eccezione
           //-------------------------------------------------------------
           if( P.ba[i+1]->TipoBox[0]<0 || P.ba[i+1]->TipoBox[0]>MAXTIPOBOXIMP ){
              P.ba[i+1]->TipoBox[0]=0;
           }
           // AL-19/01/2018 L'impianto gestirà solo PALLET BASSI
           P.ba[i+1]->Basso[0]      = true;
           if(AbilitaLetturaBassoDaPlc_ADELAIDE==true){
              P.ba[i+1]->Basso[0]   = (short int)(dm[15+(i*80)]);  // D015 / D95 / D175   (0=High, 1=Low)
           }
           P.ba[i+1]->CarWash[0]    = (short int)(dm[16+(i*80)]);  // D016 / D96 / D176   (1=Not Ok, 0=Ok)
           P.ba[i+1]->Sovrap[0]     = (short int)(dm[17+(i*80)]);  // D017 / D97 / D177   (1=sovrapponibile)
           P.ba[i+1]->Lotto[0]      = 0;
           P.ba[i+1]->Ruotato180[0] = false;                       // sempre false
           //-----------------------------------------------------
           // DEFAULT DESTINAZIONE MAGAZZINO TEMPORANEO
           //-----------------------------------------------------
           P.ba[i+1]->GruppoDest = GRP_MAG_TEMP_ADELAIDE;
           //-----------------------------------------------------
           // GESTIONI BAIE DI PRELIEVO  (EMERGENZA)
           // Se la macchina è in mancanza MAGAZZINO DI DEPOSITO
           // alzo il car_wash e porto in emeregenza
           // NOTA: per evitare di tenere la rulliera bloccata
           //-----------------------------------------------------
           if(AbilitaCarWashPerMagSaturo_ADELAIDE==false){
              if(P.ba[i+1]->NoMagaz==true && P.ba[i+1]->CarWash[0]==false){
                 P.ba[i+1]->CarWash[0]=1;
               //P.ba[i+1]->Codice[0]=ASS_COD8; // CODICE EMERGENZA
              }
           }
           if(P.ba[i+1]->Codice[0]==ASS_COD1){P.ba[i+1]->TipoCod[0]=ST_BOX_LAV1; P.ba[i+1]->GruppoDest=GRP_MAG_TEMP_ADELAIDE;}
           if(P.ba[i+1]->Codice[0]==ASS_COD2){P.ba[i+1]->TipoCod[0]=ST_BOX_LAV2; P.ba[i+1]->GruppoDest=GRP_MAG_TEMP_ADELAIDE;}
           if(P.ba[i+1]->Codice[0]==ASS_COD3){P.ba[i+1]->TipoCod[0]=ST_BOX_LAV3; P.ba[i+1]->GruppoDest=GRP_MAG_TEMP_ADELAIDE;}
           if(P.ba[i+1]->Codice[0]==ASS_COD4){P.ba[i+1]->TipoCod[0]=ST_BOX_LAV4; P.ba[i+1]->GruppoDest=GRP_MAG_TEMP_ADELAIDE;}
           if(P.ba[i+1]->Codice[0]==ASS_COD5){P.ba[i+1]->TipoCod[0]=ST_BOX_LAV5; P.ba[i+1]->GruppoDest=GRP_MAG_TEMP_ADELAIDE;}
           if(P.ba[i+1]->Codice[0]==ASS_COD6){P.ba[i+1]->TipoCod[0]=ST_BOX_LAV6; P.ba[i+1]->GruppoDest=GRP_MAG_TEMP_ADELAIDE;}
           if(P.ba[i+1]->Codice[0]==ASS_COD7){P.ba[i+1]->TipoCod[0]=ST_BOX_LAV7; P.ba[i+1]->GruppoDest=GRP_MAG_TEMP_ADELAIDE;}
           if(P.ba[i+1]->Codice[0]==ASS_COD8){P.ba[i+1]->TipoCod[0]=ST_BOX_LAV8; P.ba[i+1]->GruppoDest=GRP_MAG_TEMP_ADELAIDE;}
           //---------------------------------------------------------
           // Destinazione in base a Carwash:
           // Carwash == 2 -> Destinazione Quatantena
           // Carwash == 3 -> Destinazione Resort 1
           // dopo aver assegnato Destinazione, azzera Gruppo Dest
           //---------------------------------------------------------
           if( P.ba[i+1]->CarWash[0]>0 ){
              //---------------------------------------------
              // GESTIONE CAR-WASH
              //---------------------------------------------
              P.ba[i+1]->GruppoDest=GRP_MAG_CARWASH_ADELAIDE;
              P.ba[i+1]->CarWash[0]=1;
           }
           //----------------------------------------------
           // Verifico se c'è almeno un pallet in presa
           //----------------------------------------------
           count_in_presa=0;
           almeno_un_box_a_terra=false;
           for(j=0; j<POSINPRESA; j++){
              for(k=0; k<PIANIINPRESA; k++){
                 if(P.ba[i+1]->Presa[0][j][k]==false) continue;
                 count_in_presa++;
                 if(k==0) almeno_un_box_a_terra=true;
              }
           }
           //----------------------------------------------
           // Assegnazione dati alla posizione di prelievo
           //----------------------------------------------
           if( almeno_un_box_a_terra==false || P.ba[i+1]->TipoCod[0]==LIBERA ){
              P.ba[i+1]->PresCEST_A = LIBERA;
              P.ba[i+1]->PresCEST_B = LIBERA;
              memset(&P.ba[i+1]->BoxA, 0, sizeof(BOX));
              memset(&P.ba[i+1]->BoxB, 0, sizeof(BOX));
           }
           // Numero pallet in presa bassa
           P.ba[i+1]->StatoPres = count_in_presa;
           if(almeno_un_box_a_terra==true){
              // BASSA
              if(P.ba[i+1]->TipoCod[0]!=LIBERA){
                 switch(P.ba[i+1]->TipoCod[0]){
                    case ST_BOX_LAV1: P.ba[i+1]->PresCEST_B = PRES_LAV1;
                                      break;
                    case ST_BOX_LAV2: P.ba[i+1]->PresCEST_B = PRES_LAV2;
                                      break;
                    case ST_BOX_LAV3: P.ba[i+1]->PresCEST_B = PRES_LAV3;
                                      break;
                    case ST_BOX_LAV4: P.ba[i+1]->PresCEST_B = PRES_LAV4;
                                      break;
                    case ST_BOX_LAV5: P.ba[i+1]->PresCEST_B = PRES_LAV5;
                                      break;
                    case ST_BOX_LAV6: P.ba[i+1]->PresCEST_B = PRES_LAV6;
                                      break;
                    case ST_BOX_LAV7: P.ba[i+1]->PresCEST_B = PRES_LAV7;
                                      break;
                    case ST_BOX_LAV8: P.ba[i+1]->PresCEST_B = PRES_LAV8;
                                      break;
                    case ST_BOX_VUOTI:P.ba[i+1]->PresCEST_B = PRES_VUOTO;
                                      break;
                             default: P.ba[i+1]->PresCEST_B = PRES_VUOTO;
                                      break;
                 }
                 P.ba[i+1]->BoxB.StatoLav    = P.ba[i+1]->TipoCod[0];
                 P.ba[i+1]->BoxB.TipoBox     = P.ba[i+1]->TipoBox[0];
                 P.ba[i+1]->BoxB.Codice      = P.ba[i+1]->Codice[0];
                 P.ba[i+1]->BoxB.Altezza     = P.ba[i+1]->Altezza[0];
                 P.ba[i+1]->BoxB.CarWash     = P.ba[i+1]->CarWash[0];
                 P.ba[i+1]->BoxB.Basso       = P.ba[i+1]->Basso[0];
                 P.ba[i+1]->BoxB.Sovrap      = P.ba[i+1]->Sovrap[0];
                 if(P.ba[i+1]->BoxB.CarWash==true) P.ba[i+1]->BoxB.Sovrap=false;
                 memcpy(&P.ba[i+1]->BoxB.Presa[0][0], &P.ba[i+1]->Presa[0][0][0], sizeof(P.ba[i+1]->BoxB.Presa));
                 P.ba[i+1]->BoxB.GruppoProv  = P.ba[i+1]->Gruppo;           // non utilizzate per ALL GLASS
                 P.ba[i+1]->BoxB.GruppoDest  = P.ba[i+1]->GruppoDest;
                 P.ba[i+1]->BoxB.Ruotato180  = P.ba[i+1]->Ruotato180[0];
                 P.ba[i+1]->BoxB.Lotto       = P.ba[i+1]->Lotto[0];
              }
           }
        }
        //----------------------------------------------
        // (2) - BAIE DI DEPOSITO (4 - RORO)
        //----------------------------------------------
        if(P.ba[i+1]->Tipo==BA_DEPOSITO && P.ba[i+1]->Num==4){
           //------------------------------------------------------
           // Chiamata + consenso all'ingresso
           //------------------------------------------------------
           P.ba[i+1]->Automatico = TestBit((char*) &dm[301], 0);   // D001.00
           P.ba[i+1]->Blocco     = TestBit((char*) &dm[301], 1);   // D001.01
           P.ba[i+1]->OkIng      = TestBit((char*) &dm[301], 3);   // D001.03
           P.ba[i+1]->TuttoPieno = TestBit((char*) &dm[301], 6);   // D001.06
           //------------------------------------------------------
           // GESTIONE TGV IN INGOMBRO
           // Non aggiorno le rulliere su cui è presente il TGV
           //------------------------------------------------------
           if(P.ba[i+1]->TgvIng && !P.ba[i+1]->OkForcheAlte) continue;
           //------------------------------------------------------
           // Chiamata solo senza navetta in ingombro
           //------------------------------------------------------
           P.ba[i+1]->Chiamata   = TestBit((char*) &dm[301], 2);   // D001.02
           //------------------------------------------------------
           // Con chiamata attiva:
           // 1 - resetto la memoria di reset missione già fatto
           //     su attesa baia.
           // 2 - il TuttoPieno se la macchina non ha già navette
           //     in asserimento perchè almeno una coppia di pal
           //     ci stanno
           //------------------------------------------------------
           if(P.ba[i+1]->Chiamata==true){
              AttesaBaieGiaFatta[i+1]=false;
              //--------------------------------------------------
              // Se chiamata non asservita resetto anche tutto
              // pieno per consentire ad almento una navetta di
              // partire.
              //--------------------------------------------------
              P.TestNumChiamata(&chiamata, P.ba[i+1]->Punto, P.ba[i+1]->Num);
              if(chiamata!=0){
                 altro_agv = (short int)(P.test_chiamata_asservita(chiamata));
                 if(altro_agv==0) P.ba[i+1]->TuttoPieno=false;
              }
           }
           //------------------------------------------------------
           // Composizione presa
           //------------------------------------------------------
           pres1 = TestBit((char*) &dm[301], 4);  // D001.04
           pres2 = TestBit((char*) &dm[301], 5);  // D001.05
           if(pres1==false && pres2==false){
              P.ba[i+1]->StatoPres      = 0;
              P.ba[i+1]->Codice[0]      = 0;
              P.ba[i+1]->Lotto[0]       = 0;
              P.ba[i+1]->Altezza[0]     = 0;
              P.ba[i+1]->TipoBox[0]     = 0;
              P.ba[i+1]->Sovrap[0]      = false;
              P.ba[i+1]->Basso[0]       = false;
              P.ba[i+1]->Ruotato180[0]  = false;
              P.ba[i+1]->Presa[0][0][0] = 0;
              P.ba[i+1]->Presa[0][1][0] = 0;
              P.ba[i+1]->Presa[0][0][1] = 0;
              P.ba[i+1]->Presa[0][1][1] = 0;
              P.ba[i+1]->PresCEST_A = LIBERA;
              P.ba[i+1]->PresCEST_B = LIBERA;
              memset(&P.ba[i+1]->BoxA, 0, sizeof(BOX));
              memset(&P.ba[i+1]->BoxB, 0, sizeof(BOX));
           }
        }
     }
     //##############################################################
     if( EseguiRichiesteSvuotamento==true ){
        //--------------------------------------------------------------
        // Gestione richiesta Svuotamento Magazzini da OP
        //--------------------------------------------------------------
        // D4501..4518 = Svuotamento Mag. Shipment 1..18  --> Mag.1..18
        // D4521..4528 = Svuotamento Mag. Resort 1..8     --> Mag.21..28
        //--------------------------------------------------------------
        reset_richiesta_svuotamento = false;
        for(z=1; z<=28; z++){
           num_mag = z;   // coincide con indice
           if(ds[z]>0){
              //------------------------------------------------
              // Reset della struttura
              //------------------------------------------------
              M.m[ num_mag ]->Stato      = MG_ST_FREE;
              M.m[ num_mag ]->NumCestoni = 0;
              M.m[ num_mag ]->Completo   = false;
              for(i=1; i<=MAXTERRAMAG; i++){
                 for(j=0; j<MAXPIANIMAG; j++){
                    M.m[ num_mag ]->Pos[i].Presenza[j] = false;
                    M.m[ num_mag ]->Pos[i].QuotaH[j]   = 0;
                    M.m[ num_mag ]->Pos[i].QuotaP[j]   = 0;
                    memset(&M.m[ num_mag ]->Pos[i].Box[j], 0, sizeof(BOX));
                 }
              }
              //----------------------------------------------------------------
              // Salvataggio su file di tutte le variazioni
              //----------------------------------------------------------------
              err = M.save_record_mag( num_mag, all_mess);
              reset_richiesta_svuotamento = true;
              ds[z]=0;
           }
        }
        if( reset_richiesta_svuotamento==true ){
           //------------------------------------------------------
           // Scrittura Richieste Svuotamento
           //------------------------------------------------------
           CanaleIniziale = 4500;
           NumCanali      = 30;     // Dm4500..dm4529
           err = OM.wr_canale(&ds[0], NumPlc, "WD", CanaleIniziale, NumCanali, all_mess);
           if( err ){
              sprintf(all_mess, "(PLC%02d) - %s", NumPlc, buffer);
           }
        }
     }
     //##############################################################
  }

  //------------------------------------------------------
  // Salvataggio su file
  //------------------------------------------------------
  if(cambio_stato==true){
     err =  P.save_file_baie(buffer);
     if(!err) memcpy(&old_dm[0], &dm[0], sizeof(dm));
  }
  //---------------------------------------------
  // Se c'è stato errore su PLC ritorna errore
  //---------------------------------------------
  return err;
*/
  return 0;

}
//---------------------------------------------------------------------------

// -----------------------------------
//  write_chiamate_baie_ICAM()
// -----------------------------------
//  Personalizzazione per l'impianto OI-REIMS (ALLGLASS)
//---------------------------------------------------------------------------
// UTILIZZATA IN PROCESS.CPP  (write_chiamate_baie())
//---------------------------------------------------------------------------
// E' una funzione che scrive i segnali e i dati da PLC SIEMENS S300 (ETH)
// con le DB di scambio defite per l'impianto REIMS
//
int write_chiamate_baie_ICAM(char *all_mess)
//*****************************************
{
/*
  int  i, j, k, r, v, z, err=0;
  WORD dm[501];
  WORD ds[201];
  char buffer[151]="";
  WORD NumCanali      = 0;
  WORD NumPlc         = 0;
  WORD CanaleIniziale = 0;
  short int id;
  short int num_mag, num_baia, num_pos, num_mac;

  strcpy( all_mess, "" );
  //----------------------------------------------------------------
  // Comunicazione disabilitata --> skip!
  //	PLC Baie = COM2 (Omron), Indice=0, Baia P.1,2,3
  //----------------------------------------------------------------
  if(break_Com[COM2]==true ) return 0;

  //================================================================================
  // Tx BAIE PIENI (1,2,3) (D1000 - D1030)
  // D1000       (d[0]          ) = Check Rete
  // D1001.0     (d[1] [11] [21]) = Richiesta Ingresso
  // D1001.1     (d[1] [11] [21]) = Tgv in ingombro
  // D1001.2     (d[1] [11] [21]) = Tgv Forche Alte
  // D1001.3     (d[1] [11] [21]) = Tgv in Allarme (su Baia)
  // D1001.4     (d[1] [11] [21]) = Tgv in Arrivo
  // D1002..1010 (d[2] [12] [22]) = free
  //-------------------------------------------
  // Tx Stato Tgv (D1101 - D1300)
  // D1051       (d[51])         = Stato Tgv 1
  // D1052       (d[52])         = Stato Tgv 2
  // D1053       (d[53])         = Stato Tgv 3
  // D1054       (d[54])         = Stato Tgv 4
  // D1055       (d[55])         = Stato Tgv 5
  //-------------------------------------------
  // D1061..1070 (d[61].. d[70])  = All. Tgv 1
  // D1071..1080 (d[71].. d[80])  = All. Tgv 2
  // D1081..1090 (d[81].. d[90])  = All. Tgv 3
  // D1091..1100 (d[91].. d[100]) = All. Tgv 4
  // D1101..1110 (d[101]..d[110]) = All. Tgv 5
  //
  //--------------------------------------------
  // Gestione Stato Mag per OP: Bit 0=pos.1 impegnata, Bit1=pos.2 ...
  // D5501..D5532:
  // D5501 = Stato Mag. Shipment 1    --> Mag.1
  // D5502 = Stato Mag. Shipment 2    --> Mag.2
  // D5518 = Stato Mag. Shipment 18   --> Mag.18
  // D5521 = Stato Mag. Resort 1      --> Mag.21
  // D5522 = Stato Mag. Resort 2      --> Mag.22
  // D5528 = Stato Mag. Resort 8      --> Mag.28
  //================================================================================
  //------------------------------------------------------
  // Verifico se la macchina considerata è abbinata a
  // un PLC OMRON
  // UNICO PLC PER TUTTE LE BAIE
  // (default uso quello della baia "1");
  //------------------------------------------------------
  for(i=1; i<=3; i++){
     NumPlc=P.ba[i]->Plc;
     if(NumPlc==PLCTERRA) continue;     // NumPlc "0" = PLC TERRA
     break;
  }
  err=0;
  strcpy(all_mess, "");
  memset(&dm, 0, sizeof(dm));
  //------------------------------------------------------
  // WATCH-DOG
  // Segnale per monitoraggio comunicazioni al PLC
  //------------------------------------------------------
  WatchDog++;
  if(WatchDog>254) WatchDog=1;
  dm[ 0] = WatchDog;  // PLC BAIE PRELIEVO
  dm[30] = WatchDog;  // PLC BAIE DEPOSITO (RORO)
  //------------------------------------------------------
  // Scorrimento di tutte le baie
  //------------------------------------------------------
  for(i=0; i<TOTBAIE; i++){        // Baie 1,2,3
     //------------------------------------------------------
     // Scarto le macchine senza PLC
     //------------------------------------------------------
     if(P.ba[i+1]->EsclComunic==true     ) continue;
     if(P.ba[i+1]->Plc==0                ) continue;       // plc non assegnato
     if(P.ba[i+1]->Tipo==BA_NONE         ) continue;       // plc non assegnato
     //------------------------------------------------------
     // Calcolo puntatore DM in base a tipo baia
     // BAIE PRELIEVO (1...3)
     // BAIE DEPOSITO (4) RORO
     //------------------------------------------------------
     id=0;
     if(P.ba[i+1]->Tipo==BA_PRELIEVO && P.ba[i+1]->Num>=1 && P.ba[i+1]->Num<=3) id=(short int)( 1+(i*10));
     if(P.ba[i+1]->Tipo==BA_DEPOSITO && P.ba[i+1]->Num==4                     ) id=(short int)(31);
     if(id==0) continue; // indice sconosciuto
     //------------------------------------------------------
     // Verifico se c'è una variazione di stato stazione
     //------------------------------------------------------
     if(P.ba[i+1]->RichIng               ) SetBit((char*) &dm[id], 0);    // D1001.00 / D1011.00 / D1021.00
     if(P.ba[i+1]->TgvIng                ) SetBit((char*) &dm[id], 1);    // D1001.01 / D1011.01 / D1021.01
     if(P.ba[i+1]->OkForcheAlte          ) SetBit((char*) &dm[id], 2);    // D1001.02 / D1011.02 / D1021.02
     //------------------------------------------------------
     // Segnalazione TGV in allarme solo se su rulliera
     //------------------------------------------------------
     if(P.ba[i+1]->TgvIng==true ){
        for(j=1; j<=MAXAGV; j++){
           if(AGV[j]->mission.BaiaInIngombro!=(i+1) && AGV[j]->stato.pos!=P.ba[i+1]->Punto) continue;
           if(AGV[j]->allarme_interno==0 && AGV[j]->stato.s.bit.allarme==0                ) continue;
           //-------------------------------------------
           // Setto il bit di TGV in allarme alla baia
           //-------------------------------------------
           SetBit((char*) &dm[id], 3);      // D1001.03 / D1011.03 / D1021.03
           break;
        }
     }
     //------------------------------------------------------
     // Segnalazione TGV in arrivo  (pend=nodo baia)
     //------------------------------------------------------
     for(j=1; j<=MAXAGV; j++){
        if(AGV[j]->mission.Esclusione==true) continue;
        //-------------------------------------------
        // Verifico se TGV abbinato alla chiamata
        //-------------------------------------------
        num_baia=num_mac=num_pos=0;
        P.TestChiamataNum(AGV[j]->mission.Chiamata, &num_mag, &num_baia, &num_mac, &num_pos);
        if(num_baia!=P.ba[i+1]->Num   ) continue;
        if(P.ba[i+1]->Tipo==BA_NONE   ) continue;
        if(P.ba[i+1]->Esclusione==true) continue;
        //--------------------------------------------------------------------------------
        // 1. SE TGV CON MISSIONE GIA' PER NODO BAIA:
        //    setto il TGV in arrivo già dalla partenza da magazzino
        //--------------------------------------------------------------------------------
        if(AGV[j]->mission.pend==P.ba[i+1]->Punto){
           //-------------------------------------------
           // Setto il bit di TGV in arrivo su baia
           //-------------------------------------------
           SetBit((char*) &dm[id], 4);      // D1001.04 / D1011.04 / D1021.04
           break;
        }
     }
  }
  //------------------------------------------------------
  // Scrittura Tgv Status
  // In ordine di Importanza:
  // Automatico      = 1
  // Missione attiva = 2
  // Allarme         = 3
  // Manuale         = 4
  //------------------------------------------------------
  for( k=1; k<=MAXAGV; k++ ){
     //dm[50+k] =  AGV[k]->stato.s.val;      // D1051..D1060
     dm[50+k]=0;
     if( AGV[k]->stato.s.bit.funz    ) dm[50+k]=1;  // Automatico
     if( AGV[k]->stato.start==1      ) dm[50+k]=2;  // Missione attiva
     if( AGV[k]->allarme_interno     ) dm[50+k]=3;  // Allarme
     if( AGV[k]->stato.s.bit.allarme ) dm[50+k]=3;  // Allarme
     if( !AGV[k]->stato.s.bit.funz   ) dm[50+k]=4;  // Manuale
  }
  //------------------------------------------------------
  // Scrittura Allarmi Tgv
  //------------------------------------------------------
  for( k=1; k<=MAXAGV; k++ ){
     for( j=0;j<MAXLRALLARMI;j++){
        v = (60+((k-1)*10)+j)+1;                // D1061..1070 / D1071..1080 /...
        dm[ v ] = AGV[k]->allarme_esterno[j];
     }
  }

  //##############################################################
  //--------------------------------------------------------------
  // Gestione Stato Magazzini per OP
  //--------------------------------------------------------------
  // D5501..5518 = Stato Mag. Shipment 1..18  --> Mag.1..18
  // D5521..5528 = Stato Mag. Resort 1..8     --> Mag.21..28
  //--------------------------------------------------------------
  memset(&ds, 0, sizeof(ds));
  for(z=1; z<=28; z++){
     num_mag = z;   // coincide con indice
     for(i=1; i<=MAXTERRAMAG; i++){
        //-----------------------------------------------
        // Setta 1 bit x ogni presenza a terra
        //-----------------------------------------------
        if( M.m[ num_mag ]->Pos[i].Presenza[0] == true ){
           SetBit((char*) &ds[z], i-1);
        }
     }
  }
  //##############################################################


  //------------------------------------------------------
  // Scrittura bit a PLC
  //------------------------------------------------------
  if(break_Com[COM2]==false){
     CanaleIniziale = 5000;
     NumCanali      = 121;     // Dm1000..dm1120
     err = OM.wr_canale(&dm[0], NumPlc, "WD", CanaleIniziale, (WORD)(NumCanali), all_mess);

     //######################################################
     //------------------------------------------------------
     // Scrittura Stato Magazzini
     //------------------------------------------------------
     if(!err){
        CanaleIniziale = 5500;
        NumCanali      = 30;     // Dm5500..dm5529
        err = OM.wr_canale(&ds[0], NumPlc, "WD", CanaleIniziale, NumCanali, all_mess);
     }
     //######################################################

     if( err ){
        sprintf(all_mess, "(PLC%02d) - %s", NumPlc, buffer);
     }
  }
  return err;
*/
  return 0;
}
//---------------------------------------------------------------------------
// --------------------------------------------
//  SetResetPIntForzatoTGV_ICAM()
// --------------------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN AGV.CPP  (SetResetPIntForzatoTGV_IMPIANTI())
//
//---------------------------------------------------------------------------
// Rielabora i flags di SetReset PIntForzato
//
void SetResetPIntForzatoTGV_ICAM(short int num_agv, bool *ResetMissione, bool *ResetChiamata, bool *ResetPIntForzato, short int *ForzaPInt)
{
/*
  char all_mess[201]="";
  short int altro_tgv;
  short int nodo_interr;
  short int ptr_percorso;
  short int agv_bloccante;
  short int nodo_prec, count;
  short int i, j, k, bb, pos_att_p;
  bool path_not_ok, baia_not_ok;
  bool attesa_tgv_fronte_baia;
  bool last_pint_fuori_ingombro;
  bool tgv_passa_da_nodo_attesa;
  bool mag_emerg_roro_disponibile;
  bool chiamata_abbinata, reset_chiamata;
  short int MinPercorsi, MinPercToStart;
  short int num_baia, num_mac, num_mag, num_pos;
  short int num_baia2, num_mac2, num_mag2, num_pos2;
  long int TimeAttuale, DiffTime;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];

  TimeAttuale=time(NULL);
  //--------------------------------------------------------------
  // Se il TGV è agganciato al laser e il PIntForzato coincide
  // con un nodo di rientro posso resettarlo.
  //--------------------------------------------------------------
  if(AGV[num_agv]->stato.g_laser!=0 && AGV[num_agv]->mission.PIntForzato!=0 && AGV[num_agv]->stato.pos==AGV[num_agv]->mission.PIntForzato){
     if(atoi(N.n[AGV[num_agv]->mission.PIntForzato]->typ)==1 && N.punti_notevoli[AGV[num_agv]->mission.PIntForzato]==true){
        *ResetPIntForzato=true;
        return;
     }
  }

  //--------------------------------------------------------------
  // PASSAGGUI DA MAGNETI A LASER
  // TGV in uscita da magazzino, attesa sul NODO MASTER
  //--------------------------------------------------------------
  M.test_punto_presente_mag(all_mess, AGV[num_agv]->stato.pos, &num_mag, &num_pos);
  if(num_mag>0 && num_mag<=TOTMAG && AbilitaAttesaLaserUscitaMag_ADELAIDE!=0){
     if(M.m[num_mag]->Gruppo>=GRP_MAG_TEMP_ADELAIDE && M.m[num_mag]->Gruppo<=GRP_MAG_SPED18_ADELAIDE){
        //-------------------------------------------------------------
        // Cerca indice iniziale del percorso
        //-------------------------------------------------------------
        count=0;
        if(AbilitaAttesaLaserUscitaMag_ADELAIDE>1){
           for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
              if(AGV[num_agv]->mission.punto[ptr_percorso]==AGV[num_agv]->stato.pos ) break;
              if(AGV[num_agv]->mission.punto[ptr_percorso]==0                       ) break;
           }
           //-------------------------------------------------------------
           // conteggio nodi dal PIntForzato
           //-------------------------------------------------------------
           if(AGV[num_agv]->mission.punto[ptr_percorso]==AGV[num_agv]->stato.pos){
              for(;ptr_percorso<MAXPERCORSI; ptr_percorso++){
                 if(AGV[num_agv]->mission.punto[ptr_percorso]==0                                ) break;
                 if(AGV[num_agv]->mission.punto[ptr_percorso]==AGV[num_agv]->mission.PIntForzato) break;
                 count++;
              }
           }
           //--------------------------------------------------------------
           // AL-19/02/2017 RESET PINT FORZATO AL VOLO
           // Conto quanti nodi mancano e resetto se sono nel valore
           // richiesto
           //--------------------------------------------------------------
           if(AGV[num_agv]->stato.g_laser!=0 && count<=AbilitaAttesaLaserUscitaMag_ADELAIDE){
              if(AGV[num_agv]->mission.PIntForzato!=0 && atoi(N.n[AGV[num_agv]->mission.PIntForzato]->typ)==1){*ResetPIntForzato=true; return;}
           }
        }
        //--------------------------------------------------------------
        // Set PINT FORZATO IN USCITA DAL MAGAZZINO
        //--------------------------------------------------------------
        if(AGV[num_agv]->stato.g_laser<50 && AGV[num_agv]->mission.PIntForzato==0 && N.punti_notevoli[AGV[num_agv]->stato.pos]==true){
           //---------------------------------------------------
           // Cerco il primo nodo di rientro tra quelli del TGV
           // della missione in corso del TGV
           //---------------------------------------------------
           // 1. partenza da interno scaffale
           if(N.n[AGV[num_agv]->stato.pos]->NodoMaster!=0){
              *ForzaPInt=N.n[AGV[num_agv]->stato.pos]->NodoMaster;
              return;
           }
           // 2. partenza da Nodo master scaffale
           else{
              //-------------------------------------------------
              // a. Se il TGV ha il "G" a "0" e il primo nodo
              //    di percorso non è occupato posso ancora
              //    forzare il PInt sul nodo su cui mi trovo
              //-------------------------------------------------
              if(AGV[num_agv]->stato.g_laser==0){
                 for(i=0; i<MAXPERCORSI; i++){
                    if(AGV[num_agv]->mission.punto[i]==0                      ) break;
                    if(AGV[num_agv]->mission.punto[i]!=AGV[num_agv]->stato.pos) continue;
                    //---------------------------------------------------
                    // Il nodo successivo alla posizione del TGV non è
                    // già occupato posso forzare il PINT sulla posizione
                    // del TGV altrimenti è troppo tardi o è già
                    // avvenuto un reset di PINTFORZATO
                    //---------------------------------------------------
                    if(N.nodo_busy[AGV[num_agv]->mission.punto[i+1]]==0){
                       *ForzaPInt=AGV[num_agv]->stato.pos;
                       return;
                    }
                    break;
                 }
              }
              //-------------------------------------------------
              // b. Se il TGV ha il "G" diverso da "0" forzo
              //    il PINTFORZATO solo se il TGV non ha ancora
              //    preso lo "start"
              //-------------------------------------------------
              else{
                 if(AGV[num_agv]->stato.start==0 && AGV[num_agv]->mission.pstart==AGV[num_agv]->stato.pos && AGV[num_agv]->mission.pintend==AGV[num_agv]->stato.pos){
                    *ForzaPInt=AGV[num_agv]->stato.pos;
                    return;
                 }
              }
           }
        }
     }
  }

  //-------------------------------------------------------------
  // Scompattazione chiamata
  //-------------------------------------------------------------
  num_baia=num_mac=num_pos=pos_att_p=0;
  if(AGV[num_agv]->mission.Chiamata!=0){
     P.TestChiamataNum(AGV[num_agv]->mission.Chiamata, &num_mag, &num_baia, &num_mac, &num_pos);
  }
  //-------------------------------------------------------------
  // GESTIONE ATTESA BAIE CHIAMANTI
  //-------------------------------------------------------------
  if(num_baia>0 && num_baia<=TOTBAIE){
     //----------------------------------------------------
     // (1) - ATTESA PER BAIE INGRESSO MATERIALE
     //----------------------------------------------------
     if(P.ba[num_baia]->Gruppo==GRP_BAIE_ADELAIDE){
        pos_att_p = N.nodi_attesa[GRP_BAIE_ADELAIDE][num_baia-1];
           //------------------------------------------------------------------
        // RESET CHIAMATA PER CAMBIO MISSIONE
        // AL-22/02/2018 IMPORTANTE:
        // con TGV in zona baie pieni e destinato all'attesa in una
        // baia non in chiamata, valuto il ResetChiamata per fare il
        // cambio missione al volo.
        //------------------------------------------------------------------
        if(pos_att_p>0 && AGV[num_agv]->stato.pos==pos_att_p && !AGV[num_agv]->test_mission_on()){
           //---------------------------------------------
           // TGV su nodo di attesa stazione
           //---------------------------------------------
           reset_chiamata = false;
           if(P.ba[num_baia]->Chiamata==false){
              for(bb=1; bb<=TOTBAIE; bb++){          // cerca chiamata "B"
                 if( P.ba[bb]->Num==num_baia   ) continue;   // "B"="A"
                 if( P.ba[bb]->Esclusione      ) continue;   // "B" esclusa
                 if( P.ba[bb]->Chiamata==false ) continue;   // "B" non in chiamata
                 if( P.ba[bb]->Blocco          ) continue;   // "B" Blocco
                 if( !P.ba[bb]->Automatico     ) continue;   // "B" non automatico
                 if( P.ba[bb]->NoMagaz         ) continue;   // "B" no magazzino
                 chiamata_abbinata = false;
                 for(k=1; k<=MAXAGV; k++){
                    if(k==num_agv) continue;
                    num_baia2=num_mac2=num_pos2=0;
                    //-----------------------------------------------------
                    // Verifico se altro TGV abbinato a chiamata "B"
                    //-----------------------------------------------------
                    if(AGV[k]->mission.Chiamata!=0){
                       P.TestChiamataNum(AGV[k]->mission.Chiamata, &num_mag2, &num_baia2, &num_mac2, &num_pos2);
                    }
                    if( num_baia2>0 && num_baia2==P.ba[bb]->Num ){
                       chiamata_abbinata=true;                // "B" asservita da altro Tgv
                       break;
                    }
                 }
                 if( chiamata_abbinata==true ) continue;      // "B" già asservita
                 //--------------------------------------------------------------------
                 // esiste un Baia "B" in chiamata e non asservita: reset chiamata "A"
                 //--------------------------------------------------------------------
                 reset_chiamata=true;
                 break;
              }
           }
           //------------------------------------------------------------------
           // Reset Chiamata AGV
           // Se ResetChiamata==false non uscire ma continua l'elaborazione per
           // portare il TGV su nodo di attesa
           //------------------------------------------------------------------
           if(reset_chiamata==true ){
              AGV[num_agv]->reset_chiamata_AGV();
              return;
           }
        }
        //-------------------------------------------------------------
        // AL-26/02/2018 RESET MISSIONE e CHIAMATA PER MANCATO OK
        //               INGRESSO DA RULLIERA
        //-------------------------------------------------------------
        if(AGV[num_agv]->stato.s.bit.sunod==true && N.nodo_busy[P.ba[num_baia]->Punto]==0 && AGV[num_agv]->test_mission_on() && AGV[num_agv]->mission.pend==P.ba[num_baia]->Punto){
           nodo_prec=0;
           for(i=1; i<=MAXPERCORSI; i++){
              if(AGV[num_agv]->mission.punto[i]<=0      ) break;
              if(AGV[num_agv]->mission.punto[i]>TOTPUNTI) break;
              //---------------------------------------------------
              // Punto successivo = destinazione
              //---------------------------------------------------
              if(AGV[num_agv]->mission.punto[i+1]!=AGV[num_agv]->mission.pend) continue;
              //---------------------------------------------------
              // Memorizzo nodo precedente a destinazione
              //---------------------------------------------------
              nodo_prec=AGV[num_agv]->mission.punto[i];
              break;
           }
           if(nodo_prec!=0 && AGV[num_agv]->stato.pos==nodo_prec){
              if(P.ba[num_baia]->RichIng==true && P.ba[num_baia]->OkIng==false && (P.ba[num_baia]->Chiamata==false || P.ba[num_baia]->Blocco==true || P.ba[num_baia]->Automatico==false)){
                 if(AGV[num_agv]->mission.TimeInitAttesa==0) AGV[num_agv]->mission.TimeInitAttesa=TimeAttuale;
                 DiffTime = TimeAttuale - AGV[num_agv]->mission.TimeInitAttesa;
                 //------------------------------------------------------------
                 // Scaduto il tempo di attesa RESETTO
                 //------------------------------------------------------------
                 if(DiffTime>=TimeAttesaBaiePieni_ADELAIDE){
                    *ResetChiamata=true;
                    *ResetMissione=true;
                 }
              }
           }
        }
     }
     //----------------------------------------------------
     // (2) - ATTESA PER BAIA "RORO" DI SPEDIZIONE
     //----------------------------------------------------
     if(P.ba[num_baia]->Gruppo==GRP_BAIA_RORO && AGV[num_agv]->mission.pend==P.ba[num_baia]->Punto){
        //-------------------------------------------------------------
        // Avvicinamento a NODO DI ATTESA
        //-------------------------------------------------------------
        nodo_interr=0;
        agv_bloccante=0;
        attesa_tgv_fronte_baia=false;
        tgv_passa_da_nodo_attesa=false;
        pos_att_p = N.nodi_attesa[GRP_BAIA_RORO][0];
        if(pos_att_p>0 && pos_att_p<=TOTPUNTI){
           //-----------------------------------------------------
           // Baia in blocco
           //-----------------------------------------------------
           baia_not_ok=false;
           if(P.ba[num_baia]->Esclusione==true  ) baia_not_ok=true;
           if(P.ba[num_baia]->Blocco==true      ) baia_not_ok=true;
           if(P.ba[num_baia]->Automatico==false ) baia_not_ok=true;
           if(P.ba[num_baia]->Chiamata==false   ) baia_not_ok=true;
           if(P.ba[num_baia]->Tipo!=BA_DEPOSITO ) baia_not_ok=true;
           if(P.ba[num_baia]->PresCEST_B!=LIBERA) baia_not_ok=true;
           if(P.ba[num_baia]->StatoPres!=0      ) baia_not_ok=true;
           //------------------------------------------------------------------
           // Libero il TGV già di fronte alla baia
           //------------------------------------------------------------------
           if(baia_not_ok==false && AGV[num_agv]->stato.pos==pos_att_p){
              *ResetPIntForzato=true;
              return;
           }
           //------------------------------------------------------------------
           // Memoria TGV in attesa di fronte alla baia
           //------------------------------------------------------------------
           if(baia_not_ok==true && AGV[num_agv]->stato.pos==pos_att_p){
              attesa_tgv_fronte_baia=true;
           }
           //------------------------------------------------------------------
           // Mando TGV in attesa di fronte alla baia
           //------------------------------------------------------------------
           if(baia_not_ok==true && attesa_tgv_fronte_baia==false){
              ptr_percorso=0;
              for(i=0; i<MAXPERCORSI; i++){
                 if(AGV[num_agv]->mission.punto[i]==0                        ) break;
                 if(AGV[num_agv]->mission.punto[i]!=AGV[num_agv]->stato.pos  ) continue;
                 ptr_percorso=i;
                 break;
              }
              j=0;
              memset(&nodi_perc[0], 0, sizeof(nodi_perc));
              memset(&dati_perc[0], 0, sizeof(dati_perc));
              for(i=ptr_percorso+1; i<MAXPERCORSI; i++){
                 if(AGV[num_agv]->mission.punto[i]==0) break;
                 nodi_perc[j]=AGV[num_agv]->mission.punto[i];
                 dati_perc[j]=AGV[num_agv]->mission.dati_perc[i];
                 j++;
              }
              //------------------------------------------------------------------
              // TGV PASSA DA NODO DI ATTESA
              //------------------------------------------------------------------
              for(i=0; i<MAXPERCORSI; i++){
                 if(nodi_perc[i]==0          ) break;
                 if(nodi_perc[i]!=pos_att_p  ) continue;
                 tgv_passa_da_nodo_attesa=true;
                 break;
              }
              //------------------------------------------------------------------
              // Conteggio nodi mancanti a punto intermedio
              //------------------------------------------------------------------
              if(tgv_passa_da_nodo_attesa==true && AGV[num_agv]->mission.PIntForzato!=0 && AGV[num_agv]->mission.PIntForzato!=pos_att_p && AGV[num_agv]->mission.PIntForzato==AGV[num_agv]->stato.pos){
                 //------------------------------------------------------------------
                 // Verifico se percorso non disponibile per tgv su pt_in_ingombro
                 //------------------------------------------------------------------
                 altro_tgv=0;
                 agv_bloccante=0;
                 MinPercorsi=MinPercToStart=nodo_interr=0;
                 if(AGV[num_agv]->test_percorso_disponibile(&nodi_perc[0], &dati_perc[0], &altro_tgv, &MinPercorsi, &MinPercToStart, &nodo_interr, all_mess)){
                    if(altro_tgv>0 && altro_tgv<=MAXAGV && altro_tgv!=num_agv) agv_bloccante=altro_tgv;
                 }
                 //------------------------------------------------------------------
                 // Verifico che non ci sia altro agv con PINT forzato = NODO ATTESA
                 //------------------------------------------------------------------
                 if(agv_bloccante==0){*ForzaPInt=pos_att_p; return;}
              }
           }
        }
        //-------------------------------------------------------
        // Verifico se il PInt forzato è l'ultimo punto fuori
        // ingombro che il TGV incontra lungo la missione
        //-------------------------------------------------------
        last_pint_fuori_ingombro=false;
        if(AGV[num_agv]->mission.PIntForzato>0 && AGV[num_agv]->mission.PIntForzato<=TOTPUNTI){
           if(AGV[num_agv]->stato.pos==AGV[num_agv]->mission.PIntForzato){
              for(i=MAXPERCORSI-1; i>0; i--){
                 if(AGV[num_agv]->mission.punto[i]==0) continue;
                 //------------------------------------------------------------------
                 // Se il TGV è già in posizione fuori ingombro --> skip!
                 //------------------------------------------------------------------
                 for(j=0; j<MAXNODIATTESA; j++){
                    if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]<1                                  ) continue;
                    if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]>TOTPUNTI                           ) continue;
                    if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]!=AGV[num_agv]->mission.punto[i]    ) continue;
                    if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]!=AGV[num_agv]->mission.PIntForzato ) continue;
                    last_pint_fuori_ingombro=true;
                    break;
                 }
                 if(last_pint_fuori_ingombro==true) break;
              }
           }
        }
        //-------------------------------------------------------
        // Verifica che ci sia almento una EMERGENZA RORO
        //-------------------------------------------------------
        mag_emerg_roro_disponibile=false;
        for(i=1; i<=TOTMAG; i++){
           if(M.m[i]->Num<1                          ) continue;
           if(M.m[i]->Num>TOTMAG                     ) continue;
           if(M.m[i]->PuntoIng<1                     ) continue;
           if(M.m[i]->PuntoIng>TOTPUNTI              ) continue;
           if(M.m[i]->Tipo!=MG_PROD_TUTTI            ) continue;
           if(M.m[i]->Gruppo!=GRP_MAG_EMERGENZA_RORO ) continue;
           if(M.m[i]->Completo==true                 ) continue;
           //------------------------------------------------
           // Non deve essere già impegnato da altra navetta
           //------------------------------------------------
           altro_tgv=0;
           if(P.test_dest_occupata(M.m[i]->PuntoIng, &altro_tgv) && altro_tgv!=0 && altro_tgv!=num_agv) continue;
           if(P.test_dest_riservata(M.m[i]->PuntoIng, &altro_tgv, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv) continue;
           //------------------------------------------------
           // Magazzino disponibile trovato
           //------------------------------------------------
           mag_emerg_roro_disponibile=true;
           break;
        }
        //-------------------------------------------------------
        // AGV su ultimo punto fuori ingombro per destinazione
        // baia, attivo timer di RESET CHIAMATA per ripostare
        // materiale a magazzino
        //-------------------------------------------------------
        if((last_pint_fuori_ingombro==true || attesa_tgv_fronte_baia==true) && TimeAttesaBaiaRORO_ADELAIDE!=0){
           //------------------------------------------------------
           // Con TGV in ingombro alla baia resetto l'attesa di
           // tutte le navette in coda
           //------------------------------------------------------
           if(P.ba[num_baia]->TgvIng==true) AGV[num_agv]->mission.TimeInitAttesa=0;
           //------------------------------------------------------
           // Conteggio sec attesa per baia non disponbile
           //------------------------------------------------------
           if(P.ba[num_baia]->TgvIng==false && (P.ba[num_baia]->Chiamata==false || P.ba[num_baia]->Blocco==true || P.ba[num_baia]->Automatico==false)){
              if(AGV[num_agv]->mission.TimeInitAttesa==0) AGV[num_agv]->mission.TimeInitAttesa=TimeAttuale;
              DiffTime = TimeAttuale - AGV[num_agv]->mission.TimeInitAttesa;
              //------------------------------------------------------------
              // Scaduto il tempo di attesa RESETTO
              //------------------------------------------------------------
              if(DiffTime>=TimeAttesaBaiaRORO_ADELAIDE){
                 //------------------------------------------------------------
                 // (1) - magazzino emergenza disponibile invio navette
                 //------------------------------------------------------------
                 if(mag_emerg_roro_disponibile==true){
                    *ResetChiamata=true;
                    *ResetMissione=true;
                    //------------------------------------------------------------
                    // Memorizzo attasa baia già fatta
                    //------------------------------------------------------------
                    AttesaBaieGiaFatta[num_baia]=true;
                    //------------------------------------------------------------
                    // Forzo il TuttoPieno alla baia che dovrà tornare giù
                    // alla prima chiamata effettiva per evitare che altri TGV
                    // partano a prelevare dal parcheggio
                    //------------------------------------------------------------
                    P.ba[num_baia]->TuttoPieno=true;
                    P.save_record_baie(num_baia, all_mess);
                    //------------------------------------------------------------
                    // Assegno destinazione emergenza
                    //------------------------------------------------------------
                    AGV[num_agv]->mission.BoxB.GruppoDest=GRP_MAG_EMERGENZA_RORO;
                 }
                 //------------------------------------------------------------
                 // (2) - magazzino emergenza NON disponibile continuo attesa
                 //       ma alzo allarme
                 //------------------------------------------------------------
                 else{
                    AGV[num_agv]->allarme_interno=ALL_INT_DEST_NON_VALIDA;
                 }
              }
           }
        }
     }
  }
*/
}
//---------------------------------------------------------------------------
// --------------------------------------------
//  AvvicinamentoADestinazione()
// --------------------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN AGV.CPP  (AvvicinamentoADestinazione())
//
//---------------------------------------------------------------------------
// Rielabora i flags di SetReset PIntForzato
//
void AvvicinamentoADestinazione_ICAM(short int dest, short int *perc, struct DATI_PERCORSO *dati, short int *pint)
{
/*
  char all_mess[101]="";
  short int i, num_baia;
  short int pos_att_p;

  //--------------------------------------------
  // Cerco PINT solo se la funzione standard
  // non lo ha trovato
  //--------------------------------------------
  if(*pint!=0) return;
  //--------------------------------------------
  // Verifico se destinazione è una baia
  //--------------------------------------------
  num_baia=0;
  if(P.test_punto_presente_baia(all_mess, dest, &num_baia)) return;
  if(num_baia<1 || num_baia>TOTBAIE) return;
  //----------------------------------------------------
  // (2) - ATTESA PER BAIA "RORO" DI SPEDIZIONE
  //----------------------------------------------------
  if(P.ba[num_baia]->Gruppo==GRP_BAIA_RORO){
     pos_att_p = N.nodi_attesa[GRP_BAIA_RORO][0];
     if(pos_att_p>0 && pos_att_p<=TOTPUNTI){
        for(i=MAXPERCORSI-1; i>0; i--){
           if(perc[i]==0        ) continue;
           if(perc[i]!=pos_att_p) continue;
           *pint=perc[i];
           break;
        }
     }
  }
*/
  return;
}
//---------------------------------------------------------------------------
// --------------------------------------------
//  PosizioneRiposo_ICAM()
// --------------------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN PRCHIAMA.CPP  (PosizioneRiposoTGV())
//
//---------------------------------------------------------------------------
//   Ritorna 1 se il posizionamento a RIPOSO deve essere lasciato alla funzione standard
//
int PosizioneRiposo_ICAM(short int num_agv, char *all_mess)
{
/*
  int err=0;
  bool Ok=false;
  int  chiamata;
  bool ResetChiamata;
  short int altro_agv;
  short int mac_in_blocco;
  short int num_baia, num_mac, num_pos, num_cb, num_mag, pos_att_p;;
  short int i, j, k, Dest, PInt, gruppo;
  short int MinPercorsi, MinPercToStart, nodo_interr;
  short int new_dist, num_rot;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  long int TimeAttuale, DiffTime;
  short int  agv_bloccante;
  short int  tgv_gia_a_riposo;
  static int count_sblocco[MAXAGV+1];
  short int num_baia2, num_mac2, num_pos2, num_mag2, bb;
  bool chiamata_abbinata;
  bool baia_esclusa;
  long int OldTimeInitAttesa;
  BYTE OldPercCaricaBatt;

  err=0;
  PInt=0;
  Dest=0;
  chiamata=0;
  altro_agv=0;
  OldPercCaricaBatt=0;
  TimeAttuale=time(NULL);
  OldTimeInitAttesa=time(NULL)+63244800L;          // Forza al valore di oggi + 2ANNO  memset(&elenco_mag, 0, sizeof(elenco_mag));
  //------------------------------------------------------------------
  // No gestione riposo SPECIALE allora faccio la gestione STANDARD
  //------------------------------------------------------------------
  if(!AGV[num_agv]->stato.s.bit.okprog         ) return 0;
  if(!AGV[num_agv]->stato.s.bit.funz           ) return 0;          // Test se AGV in Automatico
  if(AGV[num_agv]->mission.SemiAutomatico      ) return 0;          // Test se AGV in Semiautomatico
  if(AGV[num_agv]->stato.start                 ) return 0;
  if(AGV[num_agv]->mission.Esclusione          ) return 0;          // Test se escluso
  if(AGV[num_agv]->stato.pos<1                 ) return 0;
  if(AGV[num_agv]->stato.pos>TOTPUNTI          ) return 0;
  //------------------------------------------------------------------
  // ALL_INT_DEST_IMPEGNATA (carico a bordo senza dest. disponibile)
  // SOLO SE DESTINAZIONE MAGAZZINO TEMPORANEO O CARWASH
  // Se il sistema arriva a processare la funzione di ritorno a
  // riposo con navetta con carico a bordo.
  // alza l'allarme ALL_INT_DEST_IMPEGNATA dopo "n" cicli per
  // segnalazione all'operatore che non sa dove depositare
  //------------------------------------------------------------------
  if(AGV[num_agv]->mission.Chiamata==0 && AGV[num_agv]->mission.PresCEST_B!=LIBERA && AGV[num_agv]->mission.BoxB.GruppoDest!=GRP_BAIA_RORO){
     new_dist=num_rot=0;
     memset(&nodi_perc[0], 0, sizeof(nodi_perc));
     memset(&dati_perc[0], 0, sizeof(dati_perc));
     //-------------------------------------------------------
     // Simulo il ritorno a ricarica per avere una dest
     // da assegnare alla funzione distanza() per vedere se
     // il TGV ha il percorso disponibile per partire e che
     // non sia fermo per altra navetta in ingombro
     //-------------------------------------------------------
     err = AGV[num_agv]->distanza(STD_PATH, 0, AGV[num_agv]->mission.PosRicarica, &nodi_perc[0], &dati_perc[0], all_mess, &new_dist, &num_rot);
     if(new_dist<MAXPERCORSI && !err){
        //--------------------------------------------------------
        // Verifico Minimo percorsi disponibili
        //--------------------------------------------------------
        altro_agv=MinPercorsi=MinPercToStart=nodo_interr=0;
        AGV[num_agv]->test_percorso_disponibile(&nodi_perc[0], &dati_perc[0], &altro_agv, &MinPercorsi, &MinPercToStart, &nodo_interr, all_mess);
        if(MinPercorsi>=MinPercToStart && AGV[num_agv]->allarme_interno==0 && AGV[num_agv]->stato.s.bit.allarme==false){
           AGV[num_agv]->allarme_interno=ALL_INT_DEST_IMPEGNATA;
        }
     }
  }
  //------------------------------------------------------------------
  // Ricarica senza pallet a bordo
  //------------------------------------------------------------------
  if(AGV[num_agv]->mission.PresCEST_A!=LIBERA  ) return 0;
  if(AGV[num_agv]->mission.PresCEST_B!=LIBERA  ) return 0;
  if(AGV[num_agv]->stato.s.bit.carA==true      ) return 0;
  if(AGV[num_agv]->stato.s.bit.carB==true      ) return 0;
  //------------------------------------------------------------------
  // NO POSIZIONAMENTO A NODO DI ATTESA CON TGV IN RICARICA
  //------------------------------------------------------------------
//if(NoAttesaDaRicarica_ADELAIDE==true){
//   if(AGV[num_agv]->stato.pos==AGV[num_agv]->mission.PosRicarica) return 0;
//}
  //------------------------------------------------------------------
  // ATTESA SU NODO PER PROSSIMA CHIAMATA A TEMPO
  //------------------------------------------------------------------
//if(AGV[num_agv]->mission.Chiamata!=0){
  if(AGV[num_agv]->mission.Chiamata!=0 && !AGV[num_agv]->test_mission_on() ){
     num_baia=num_mac=num_pos=0;
     P.TestChiamataNum(AGV[num_agv]->mission.Chiamata, &num_mag, &num_baia, &num_mac, &num_pos);
     if(num_baia>0 && num_baia<=TOTBAIE && P.ba[num_baia]->Gruppo==GRP_BAIE_ADELAIDE && P.ba[num_baia]->Chiamata==false){
        //---------------------------------------------
        // TGV su nodo di attesa stazione
        //---------------------------------------------
        ResetChiamata = false;
        pos_att_p = N.nodi_attesa[GRP_BAIE_ADELAIDE][num_baia-1];
        if(AGV[num_agv]->stato.pos==pos_att_p && pos_att_p>0 && pos_att_p<=TOTPUNTI){
           //-----------------------------------------------------------------
           // Postazione non di prelievo --> Skip!
           //------------------------------------------------------------
           if(P.ba[num_baia]->Tipo!=BA_PRELIEVO ) ResetChiamata=true;
           //------------------------------------------------------------
           // Se la macchina è esclusa/blocco --> Skip!
           //------------------------------------------------------------
           if(P.ba[num_baia]->Esclusione        ) ResetChiamata=true;
           //------------------------------------------------------------
           // Se la macchina in blocco --> Skip!
           //------------------------------------------------------------
         //if(P.ba[num_baia]->Blocco==true      ) ResetChiamata=true;
         //if(P.ba[num_baia]->Automatico==false ) ResetChiamata=true;
           //------------------------------------------------------------
           // Attesa già fatta
           //------------------------------------------------------------
         //if(AttesaBaieGiaFatta[num_baia]==true) ResetChiamata=true;
           //------------------------------------------------------------
           // ATTESA PER UN TEMPO MASSIMO
           //------------------------------------------------------------
           if(AGV[num_agv]->mission.TimeInitAttesa==0) AGV[num_agv]->mission.TimeInitAttesa=TimeAttuale;
           DiffTime = TimeAttuale - AGV[num_agv]->mission.TimeInitAttesa;
           //------------------------------------------------------------
           // Scaduto il tempo di attesa RESETTO e dirigo in RICARICA,
           // per i CAR WASH con deposito a terra non attendo nemmeno il
           // tempo, ma resetto per ricalcolare la destinazione visto che
           // sono posizioni in profondità
           //------------------------------------------------------------
           if(DiffTime>=TimeAttesaBaiePieni_ADELAIDE) ResetChiamata=true;
        }
        //------------------------------------------------------------
        // TGV Senza niente a bordo su percorso e start a "0"
        //------------------------------------------------------------
        if(AGV[num_agv]->stato.pos!=pos_att_p && pos_att_p>0 && pos_att_p<=TOTPUNTI){
           //------------------------------------------------------------
           // (1) RESET PER TGV BLOCCANTE
           // Verifico se TGV bloccante per altre navette dopo 'n' cicli
           // Resett
           //------------------------------------------------------------
           agv_bloccante=false;
           for(i=1; i<=MAXAGV; i++){
              if(i==num_agv              ) continue;
              if(AgvBloccati[i]!=num_agv ) continue;
              agv_bloccante=true;
           }
           if(agv_bloccante==true){
              if(count_sblocco[num_agv]>NumCicliPerSbloccoDeadLock){
                 ResetChiamata=true;
              }
              else count_sblocco[num_agv]++;
           }
           //------------------------------------------------------------
           // (2) RESET PER NODO OCCUPATO, MA STAZIONE NON IN CHIAMATA
           //------------------------------------------------------------
           if(N.nodo_busy[P.ba[num_baia]->Punto]==num_agv){
              mac_in_blocco=0;
              if(mac_in_blocco==0 && P.ba[num_baia]->Esclusione        ) mac_in_blocco=1;
              if(mac_in_blocco==0 && P.ba[num_baia]->Blocco==true      ) mac_in_blocco=1;
              if(mac_in_blocco==0 && P.ba[num_baia]->Automatico==false ) mac_in_blocco=1;
              if(mac_in_blocco==0 && AttesaBaieGiaFatta[num_baia]      ) mac_in_blocco=1;
              if(mac_in_blocco!=0) ResetChiamata=true;
           }
           //------------------------------------------------------------
           // (3) RESET PER TGV SU NODO DI ATTESA DI ALTRA BAIA ESCLUSA
           //------------------------------------------------------------
           for(i=1; i<=TOTBAIE; i++){
              if(P.ba[i]->Num<1                                                 ) continue;
              if(P.ba[i]->Num>TOTBAIE                                           ) continue;
              if(P.ba[i]->Num==num_baia                                         ) continue;
              if(P.ba[i]->Punto<1                                               ) continue;
              if(P.ba[i]->Punto>TOTPUNTI                                        ) continue;
              if(P.ba[i]->Gruppo!=GRP_BAIE_ADELAIDE                             ) continue;
              if(AGV[num_agv]->stato.pos!=N.nodi_attesa[GRP_BAIE_ADELAIDE][i-1] ) continue;
              //------------------------------------------------------------
              // TGV su nodo di attesa di altra baia in blocco o esclusa
              //------------------------------------------------------------
              mac_in_blocco=0;
              if(mac_in_blocco==0 && P.ba[i]->Esclusione        ) mac_in_blocco=1;
              if(mac_in_blocco==0 && P.ba[i]->Blocco==true      ) mac_in_blocco=1;
              if(mac_in_blocco==0 && P.ba[i]->Automatico==false ) mac_in_blocco=1;
              if(mac_in_blocco==0 && AttesaBaieGiaFatta[i]      ) mac_in_blocco=1;
              if(mac_in_blocco!=0) ResetChiamata=true;
              break;
           }
        }
        //---------------------------------------------------------------------------------
        // Se Tgv associato a chiamata "A", Baia "A" però non è in chiamata
        // oppure "A" è in chiamata (ma non carica perchè altro Tgv sta occupando il mag.di dest.)
        // ed invece si attiva un'altra chiamata "B" non asservita allora reset chiamata "A"
        // (Se arrivo fino a qui allora significa che non ha potuto lanciare la missione per "A" pur
        // avendo la chiamata attiva perchè, probabilmente, mancava la disponibilità del deposito)
        //---------------------------------------------------------------------------------
      //if(P.ba[num_baia]->Chiamata==false ){     // chiamata "A" (=Tgv) non attiva
        if( (P.ba[num_baia]->Chiamata==false)    ||      // chiamata "A" (=Tgv) non attiva
            (AGV[num_agv]->stato.pos==pos_att_p) ){      // chiamata "A" attiva e Tgv in attesa davanti "A" (senza missione)

           for(bb=1; bb<=TOTBAIE; bb++){          // cerca chiamata "B"
              if( P.ba[bb]->Num==num_baia )    continue;   // "B"="A"
              if( P.ba[bb]->Esclusione )       continue;   // "B" esclusa
              if( P.ba[bb]->Chiamata==false )  continue;   // "B" non in chiamata
              if( P.ba[bb]->Blocco     )       continue;   // "B" Blocco
              if( !P.ba[bb]->Automatico )      continue;   // "B" non automatico
              if( P.ba[bb]->NoMagaz    )       continue;   // "B" no magazzino
              chiamata_abbinata = false;
              for(k=1; k<=MAXAGV; k++){
                 if(k==num_agv) continue;
                 num_baia2=num_mac2=num_pos2=0;
                 //-----------------------------------------------------
                 // Verifico se altro TGV abbinato a chiamata "B"
                 //-----------------------------------------------------
                 if(AGV[k]->mission.Chiamata!=0){
                    P.TestChiamataNum(AGV[k]->mission.Chiamata, &num_mag2, &num_baia2, &num_mac2, &num_pos2);
                 }
                 if( num_baia2>0 && num_baia2==P.ba[bb]->Num ){
                    chiamata_abbinata=true;                // "B" asservita da altro Tgv
                    break;
                 }
              }
              if( chiamata_abbinata==true ) continue;      // "B" già asservita
              //--------------------------------------------------------------------
              // esiste un Baia "B" in chiamata e non asservita: reset chiamata "A"
              //--------------------------------------------------------------------
              ResetChiamata=true;
              break;
           }
        }
        //------------------------------------------------------------------
        // Reset Chiamata AGV
        // Se ResetChiamata==false non uscire ma continua l'elaborazione per
        // portare il TGV su nodo di attesa
        //------------------------------------------------------------------
        if(ResetChiamata==true ){
           AGV[num_agv]->reset_chiamata_AGV();
           //-----------------------------------------------------
           // In caso di RESET a attesa per una qualunque delle
           // due baie PIENI sospendo anche l'altra
           //-----------------------------------------------------
           AttesaBaieGiaFatta[num_baia]=true;
        }
     }
  }
  tgv_gia_a_riposo=NN;
  count_sblocco[num_agv]=0;
  //------------------------------------------------------------------
  // Se tgv non in riserva è già su un nodo di attesa e non in
  // riserva --> skip!
  //------------------------------------------------------------------
  if(AGV[num_agv]->stato.s.bit.ris==false){
     //--------------------------------------------------------------
     // TGV già su uno dei nodi di riposo possibili
     //--------------------------------------------------------------
     if(tgv_gia_a_riposo==NN){
        for(i=0; i<MAXNODIATTESA; i++){
           if(N.nodi_attesa[GRP_BAIE_ADELAIDE][i]<1                         ) continue;
           if(N.nodi_attesa[GRP_BAIE_ADELAIDE][i]>TOTPUNTI                  ) continue;
           if(AGV[num_agv]->stato.pos!=N.nodi_attesa[GRP_BAIE_ADELAIDE][i]  ) continue;
           tgv_gia_a_riposo=N.nodi_attesa[GRP_BAIE_ADELAIDE][i];
           break;
        }
     }
     if(tgv_gia_a_riposo>0 && tgv_gia_a_riposo<=TOTPUNTI){
      //if(AGV[num_agv]->mission.PosRicarica==num_agv && AGV[num_agv]->mission.Chiamata==0){
        if(AGV[num_agv]->mission.PosRicarica<21 && AGV[num_agv]->mission.Chiamata==0){
           Dest=AGV[num_agv]->mission.PosRicarica;
        }
        else return 0;
     }
     //--------------------------------------------------------------
     // VERIFICO SE TGV abbinato a una BAIA PIENI
     //--------------------------------------------------------------
     for(j=0; j<MAXGRUPPIIMP; j++){
        if(Dest!=0) break;
        if(AGV[num_agv]->mission.AsservBaie[j]!=GRP_BAIE_ADELAIDE) continue;
        //-------------------------------------------------
        // Verifico se almeno una baia attiva appartenete
        // al gruppo
        //-------------------------------------------------
        for(i=1; i<=TOTBAIE; i++){
           if(P.ba[i]->Num<1                         ) continue;
           if(P.ba[i]->Num>TOTBAIE                   ) continue;
           if(P.ba[i]->Punto<1                       ) continue;
           if(P.ba[i]->Punto>TOTPUNTI                ) continue;
           if(P.ba[i]->Gruppo!=GRP_BAIE_ADELAIDE     ) continue;
           if(P.ba[i]->Esclusione                    ) continue;
           if(P.ba[i]->Blocco                        ) continue;
           if(!P.ba[i]->Automatico                   ) continue;
           //--------------------------------------------------------
           // AL-19/10/2019 OTTIMIZZAZIONE CARICA BATTERIE
           // Dalla ricarica posso andare in attesa solo con la
           // chiamata per
           //--------------------------------------------------------
           if(NoAttesaDaRicarica_ADELAIDE==true && AGV[num_agv]->stato.pos==AGV[num_agv]->mission.PosRicarica){
              if(P.ba[i]->Chiamata==false) continue;
           }
           //--------------------------------------------------------
           // Attesa già fatta per BAIA
           //--------------------------------------------------------
           if(AttesaBaieGiaFatta[i]==true) continue;
           //--------------------------------------------------------
           // Verifico se nodo di attesa della baia considerata è
           // già occupato
           //--------------------------------------------------------
           err= P.test_dest_occupata(N.nodi_attesa[GRP_BAIE_ADELAIDE][i-1], &altro_agv);
           if((err!=0) && (num_agv!=altro_agv)) continue;
           //--------------------------------------------------------
           // AL-30/10/2019
           // pertenza da carwash solo se almeno un minimo nodi
           // disponibili
           //--------------------------------------------------------
           if(N.pt_in_ingombro[AGV[num_agv]->stato.pos]==true && N.nodo_minpercorsi[AGV[num_agv]->stato.pos]!=99){
              new_dist=num_rot=0;
              memset(&nodi_perc[0], 0, sizeof(nodi_perc));
              memset(&dati_perc[0], 0, sizeof(dati_perc));
              err = AGV[num_agv]->distanza(STD_PATH, 0, N.nodi_attesa[GRP_BAIE_ADELAIDE][i-1], &nodi_perc[0], &dati_perc[0], all_mess, &new_dist, &num_rot);
              if(new_dist>=MAXPERCORSI || err) continue;
              //--------------------------------------------------------
              // Verifico Minimo percorsi disponibili
              //--------------------------------------------------------
              altro_agv=MinPercorsi=MinPercToStart=nodo_interr=0;
              AGV[num_agv]->test_percorso_disponibile(&nodi_perc[0], &dati_perc[0], &altro_agv, &MinPercorsi, &MinPercToStart, &nodo_interr, all_mess);
              if(MinPercorsi<N.nodo_minpercorsi[AGV[num_agv]->stato.pos]) continue;
           }
           //--------------------------------------------------------
           // Controllo altro tgv con stessa destinazione.
           // non uso test_dest_riservata() perchè non lavora
           // con destinazioni che non siano Punti terminali
           //--------------------------------------------------------
           altro_agv=0;
           for(k=1; k<=MAXAGV; k++){
              if(k==num_agv) continue;
              //-----------------------------------------------------
              // Verifico se altro TGV abbinato a chiamata
              //-----------------------------------------------------
              num_baia=num_mac=num_pos=0;
              if(AGV[k]->mission.Chiamata!=0){
                 P.TestChiamataNum(AGV[k]->mission.Chiamata, &num_mag, &num_baia, &num_mac, &num_pos);
              }
              //-----------------------------------------------------
              // Altro TGV con destinazione nodo di attesa
              //-----------------------------------------------------
              if(AGV[k]->mission.pend!=N.nodi_attesa[GRP_BAIE_ADELAIDE][i-1] && num_baia!=i) continue;
              altro_agv=k;
              break;
           }
           if(altro_agv!=0) continue;
           //--------------------------------------------------------
           // Start da posizione di ricarica SPECIALIZZATA
           //--------------------------------------------------------
           if(CercaAgvLiberoInRicarica>0 && AGV[num_agv]->stato.pos<51){
              altro_agv=0;
              for(k=1; k<=MAXAGV; k++){
                 if(AGV[k]->test_mission_on()     ) continue;
                 if(AGV[k]->mission.SemiAutomatico) continue;
                 if(AGV[k]->mission.Esclusione    ) continue;
                 if(AGV[k]->stato.pos>50          ) continue;
                 //------------------------------------------------------
                 // (1) RICERCA TGV IN RICARICA DA PIU' TEMPO
                 //------------------------------------------------------
                 if(CercaAgvLiberoInRicarica==1 && AGV[k]->mission.TimeInitAttesa!=0){
                    //---------------------------------------------
                    // trova la distanza pi— breve se pi— di 1 AGV
                    //---------------------------------------------
                    if(AGV[k]->mission.TimeInitAttesa<OldTimeInitAttesa){
                       altro_agv = k;
                       OldTimeInitAttesa=AGV[k]->mission.TimeInitAttesa;
                    }
                 }
                 //------------------------------------------------------
                 // (2) RICERCA TGV IN RICARICA CON PERCENTUALE BATTERIA
                 //     MAGGIORE
                 //------------------------------------------------------
                 if(CercaAgvLiberoInRicarica==2 && (AGV[k]->mission.PercCaricaBatt!=0 || AGV[k]->mission.TimeInitAttesa!=0)){
                    //---------------------------------------------
                    // trova la distanza pi— breve se pi— di 1 AGV
                    //---------------------------------------------
                    if(AGV[k]->mission.PercCaricaBatt!=0){
                       if(AGV[k]->mission.PercCaricaBatt>OldPercCaricaBatt){
                          altro_agv = k;
                          OldPercCaricaBatt=AGV[k]->mission.PercCaricaBatt;
                       }
                    }
                    else{
                       if(AGV[k]->mission.TimeInitAttesa<OldTimeInitAttesa){
                          altro_agv = k;
                          OldTimeInitAttesa=AGV[k]->mission.TimeInitAttesa;
                       }
                    }
                 }
              }
              if(altro_agv!=0 && altro_agv!=num_agv) continue;
           }
           //--------------------------------------------------------
           // Se il TGV è abbinato alla chiamata
           //--------------------------------------------------------
           Dest = N.nodi_attesa[GRP_BAIE_ADELAIDE][i-1];
           //--------------------------------------------------------
           // Se il TGV va in attesa per baia, memorizzo la chiamata
           // per la gestione dell'attesa a TEMPO
           //--------------------------------------------------------
           P.TestNumChiamata(&chiamata, P.ba[i]->Punto, P.ba[i]->Num);
           break;
        }
     }
     if( Dest==0 ){
        Dest=AGV[num_agv]->mission.PosRicarica;
     }
  }
  //------------------------------------------------------------------
  // Se TGV in batteria scarica --> CAMBIO BATTERIE
  //------------------------------------------------------------------
  if(AGV[num_agv]->stato.s.bit.ris==true || AGV[num_agv]->stato.s.bit.allarme==true){
     Dest=AGV[num_agv]->mission.PosRicarica;
  }
  //--------------------------------------------------------
  // Verifica Destinazione disponibile
  //--------------------------------------------------------
  if( Dest>0 ){
     if(P.test_dest_riservata(Dest, &altro_agv, num_agv)) Dest=0;
     err= P.test_dest_occupata(Dest, &altro_agv);
     if((err!=0) && (num_agv!=altro_agv)) Dest=0;
  }
  //--------------------------------------------------------
  // Nessuna posizione riposo trovata
  //--------------------------------------------------------
  if(Dest==0){
     AGV[num_agv]->allarme_interno = ALL_INT_RICARICA_OCC;
     return 0;
  }
  //--------------------------------------------------------
  // Destinazione calcolata = POSIZIONE AGV
  //--------------------------------------------------------
  if(Dest==AGV[num_agv]->stato.pos) return 0;
  //--------------------------------------------------------
  // Inizializzazione missione
  //--------------------------------------------------------
  if(new_init_mission(FALSE, CALCOLA_TIPO_PATH, num_agv, chiamata, Dest, 0, PInt, MISS_SOLO_POSIZIONAMENTO, 0, &Ok, all_mess)) return 1;
*/
  return 0;
}
//---------------------------------------------------------------------------

// -----------------------------------
//   load_udc_from_SQL()
// -----------------------------------
// --- READ ---
int load_udc_from_SQL_ICAM(int cmd, struct DATI_PRD *UdcTmp, unsigned int locS, unsigned int locE, char *UdcS, char *UdcE, char *all_mess)
{
/*
  int err=0;
  int idx=0;
  int tmpCmd;
  char SourceLine[10]="";
  char Destination[10]="";
  bool NoLockQuery=true;
  struct DATI_PRD UdcSQL[MAXUDCIMPIANTO];
  TDateTime tmpTime;
  AnsiString CmdWhereSql="";
  AnsiString StringaSQL="";
  AnsiString NoLock;
  TADOQuery  *ADOQueryTab;
  //--------------------------------------
  strcpy(all_mess, "");

  //--------------------------------------
//if(break_Com[COM3]==true){
//   strcpy(all_mess, "SQL not enabled");
//   return 1;
//}
  //--------------------------------------
  switch(cmd){
     case CMD_RD_SQL_ALL_CHANGE:   CmdWhereSql=" WHERE CustomerChange <> 0 ";                                                break; // 1 tutti gli UDC con una CustomerChange
     case CMD_RD_SQL_ALL_UDC:      CmdWhereSql=" WHERE CustomerChange <> 0 OR Location <> 0";                                break; // 2 tutti gli UDC con una locazione o CustomerChange
     case CMD_RD_SQL_ALL_LOC:      CmdWhereSql=" WHERE Location <> 0 ";                                                      break; // 3 tutti gli UDC con una locazione
     case CMD_RD_SQL_RANGE_UDC:    CmdWhereSql=" WHERE HU BETWEEN '" + AnsiString(UdcS) + "' AND '" + AnsiString(UdcE) + "'";break; // 4 tutti gli UDC in un range di UDC
     case CMD_RD_SQL_RANGE_LOC:    CmdWhereSql=" WHERE Location BETWEEN " + AnsiString(locS) + " AND " + AnsiString(locE);   break; // 5 tutti gli UDC in un range di locazioni
     case CMD_RD_SQL_ONE_UDC:      CmdWhereSql=" WHERE HU ='" + AnsiString(UdcS) + "'";                                      break; // 6 tutti gli UDC in una locazione
     case CMD_RD_SQL_ONE_LOC:      CmdWhereSql=" WHERE Location = " + AnsiString(locS);                                      break; // 7 tutti gli UDC con una locazione
     default: strcpy(all_mess, "cmd not valid");  return 1;
  }

  //------------------------------------------------------------
  // Attivazione procedura di NO LOCK per velocizzare accessi
  //------------------------------------------------------------
  NoLock = "";
  if(NoLockQuery==true) NoLock = " WITH (nolock) ";
  //------------------------------------------------------------
  try{
     //-----------------------------------------------
     // Inizializzazione query di aggiornamento
     //-----------------------------------------------
     ADOQueryTab = new TADOQuery(FormSQLDB);
     //FormMain->ADOQuery1->Connection=FormMain->ADOConnSQL;
     //ADOQueryTab->Connection = FormMain->ADOConnSQL;

     ADOQueryTab->ConnectionString  = "Provider="+ ProviderADO + "; Password=" + DBasePsw + "; Persist Security Info=True; User ID=" + DBaseUser + "; Initial Catalog=" + NomeDBase + "; Data Source=" + NomeSRVSQL + ";";
     ADOQueryTab->CommandTimeout    = SecComandoTimeOutSQL;

     StringaSQL = "SELECT * FROM " + NomeTabA + NoLock + CmdWhereSql;

     ADOQueryTab->SQL->Clear();
     ADOQueryTab->SQL->Add(StringaSQL);
     ADOQueryTab->Open();   // apertura data set
     //-----------------------------------------------
     if(ADOQueryTab->RecordCount<=0){
        ADOQueryTab->Close();   // chiusura data set
        delete ADOQueryTab;
        strcpy(all_mess,"No record Found");
        return 0;
     }
     //-----------------------------------------------
     idx=0;
     memset(&UdcSQL, 0, sizeof(UdcSQL));
     do{
        if(idx>=MAXUDCIMPIANTO) break;

        UdcSQL[idx].id_record; // ??
        strcpy( UdcSQL[idx].Udc ,        ADOQueryTab->FieldByName("HU"          )->AsString.c_str()); // HU
        strcpy( UdcSQL[idx].code,        ADOQueryTab->FieldByName("Product"     )->AsString.c_str()); // Product
        strcpy( UdcSQL[idx].description, ADOQueryTab->FieldByName("Description" )->AsString.c_str()); // Description
        //strcpy( UdcTmp->size,          ADOQueryTab->FieldByName(""            )->AsString.c_str()); // ??
        strcpy( UdcSQL[idx].batch,       ADOQueryTab->FieldByName("Batch"       )->AsString.c_str()); // Batch
        strcpy( UdcSQL[idx].state,       ADOQueryTab->FieldByName("Status"      )->AsString.c_str()); // Status
        strcpy( UdcSQL[idx].shift,       ADOQueryTab->FieldByName("Shift"       )->AsString.c_str()); // Shift   //### Andrea 14/9

        strcpy(SourceLine,               ADOQueryTab->FieldByName("SourceLine"  )->AsString.c_str()); // SourceLine è una stringa!!
        SourceLine[0]=SourceLine[2];
        SourceLine[1]=SourceLine[3];
        SourceLine[2]=NULL;
        SourceLine[3]=NULL;
        UdcSQL[idx].source      = (unsigned int) atoi(SourceLine);

        strcpy( Destination, ADOQueryTab->FieldByName("Destination" )->AsString.c_str());             // Destination
        UdcSQL[idx].destination = (unsigned int) atoi(Destination);

        UdcSQL[idx].location    = (unsigned int) ADOQueryTab->FieldByName("Location"    )->AsInteger; // Location
        UdcSQL[idx].weight      = (short int)    ADOQueryTab->FieldByName("Weight"      )->AsInteger; // Weight
        UdcSQL[idx].height      = (short int)    ADOQueryTab->FieldByName("Height"      )->AsInteger; // Height
        UdcSQL[idx].quantity    = (short int)    ADOQueryTab->FieldByName("Quantity"    )->AsInteger; // Quantity

        TDateTime tmpTime = ADOQueryTab->FieldByName("CreationData")->AsDateTime;
        UdcSQL[idx].time = FormSQLDB->Algo_DateTime_To_TimeInt(tmpTime);
        //----------------------------------------------
        idx++;
        ADOQueryTab->Next();
     }while(ADOQueryTab->Eof==false);
     //----------------------------------------------
     ADOQueryTab->Close();   // chiusura data set
     delete ADOQueryTab;
     //-----------------------------------------------
     // Compilo struttura in uscita funzione
     //-----------------------------------------------
     memcpy(UdcTmp, &UdcSQL, sizeof(UdcSQL));
     //-----------------------------------------------
     // Dopo la lettura azzera il CustomerChange
     // per i comandi che lo leggono
     //-----------------------------------------------
   //if(cmd==CMD_RD_SQL_ALL_CHANGE || cmd==CMD_RD_SQL_ALL_UDC){
   //   tmpCmd=CMD_WR_SQL_UDC_RST_CUCH;
   //   err=write_udc_to_SQL_ICAM(tmpCmd, &UdcSQL[0], all_mess);
   //}
     return err;
  }
  catch(Exception &exception){
     errore_comunicazione[COM3][DB_DBASE]=ERR_SQL_RD_TAB;
     sprintf(all_mess, "Query Execution Error on %s \n[ %s ]", NomeTabA, NomeDBase);
     ADOQueryTab->Close();   // chiusura data set
     //----------------------------------------------------------------
     char messaggio[501]="";
     char FileLogExceptionSQL[101]="";
     sprintf(FileLogExceptionSQL, "%sSQL\\LogExceptionSQL.txt", PathProject.c_str());
     sprintf(messaggio,"load_udc_from_SQL_ALLGLASS;%s;%s", all_mess, exception.Message.c_str() );
     messaggio[498]=NULL;    // per garantire il corretto scroll
     file_cronologico_generico(FileLogExceptionSQL, messaggio, 50000L);
     strcpy(messaggio, StringaSQL.c_str());
     messaggio[498]=NULL;    // per garantire il corretto scroll
     file_cronologico_generico(FileLogExceptionSQL, messaggio, 50000L);
     //----------------------------------------------------------------
     delete ADOQueryTab;
     return 9999;
  }
*/
  return 0;
}
//---------------------------------------------------------------------------
// -----------------------------------
//    write_udc_to_SQL()
// -----------------------------------
// --- WRITE ---
int write_udc_to_SQL_ICAM(int cmd, struct DATI_PRD *UdcTmp, char *all_mess)
{
/*
  int err=0;
  int idx;
  short int gruppo_dest;
  struct DATI_PRD UdcSQL[MAXUDCIMPIANTO];
  short int Tgv, Mac, Mag, Bai, Stz, Pos, Pin, Forc, NPal;

  AnsiString CmdSetSql="";
  AnsiString StringaSQL="";
  TADOQuery  *ADOQueryTab;
  //--------------------------------------
  strcpy(all_mess, "");
  //--------------------------------------
//if( break_Com[COM3]==true ){
//   strcpy(all_mess, "SQL not enabled");
//   return 1;
//}
  switch(cmd){
     case CMD_WR_SQL_UDC_ALL:      break; // 1 aggiornamento di tutti i dati previsti (Stato + Locazione)
     case CMD_WR_SQL_UDC_SET_LOC:  break; // 2 settaggio solo della LOCAZIONE
     case CMD_WR_SQL_UDC_RST_LOC:  break; // 3 reset della LOCAZIONE
     case CMD_WR_SQL_UDC_RST_CUCH: break; // 4 reset del Customer Change
     default: strcpy(all_mess, "cmd not valid");  return 1;
  }
  //--------------------------------------
  idx=0;
  memcpy(&UdcSQL, UdcTmp,sizeof(UdcSQL));

  try{
     //-----------------------------------------------
     // Inizializzazione query di aggiornamento
     //-----------------------------------------------
     ADOQueryTab = new TADOQuery(FormSQLDB);

     ADOQueryTab->ConnectionString = "Provider="+ ProviderADO + "; Password=" + DBasePsw + "; Persist Security Info=True; User ID=" + DBaseUser + "; Initial Catalog=" + NomeDBase + "; Data Source=" + NomeSRVSQL + ";";
     ADOQueryTab->CommandTimeout   = SecComandoTimeOutSQL;

     for(idx=0; idx<MAXUDCIMPIANTO; idx++){
        //---------------------------------------------
        // Andrea 13/9/2018
        // molti record hanno UDC=""  --> Vedere!!
        //---------------------------------------------
        if(strcmp(UdcSQL[idx].Udc,"")==0) break;
        //--------------------------------------
        // ADELAIDE
        // scrittura gruppo di destinazione
        // su SQL
        //--------------------------------------
        if(DESTSQLdaTecno_ADELAIDE==true){
           //------------------------------------------------------------
           // Descripatazione locazione
           //------------------------------------------------------------
           Tgv=Mac=Mag=Bai=Stz=Pos=Pin=Forc=NPal=0;
           B.LocationToStruct(UdcSQL[idx].location, &Tgv, &Mac, &Mag, &Bai, &Stz, &Pos, &Pin, &Forc, &NPal);
           gruppo_dest=NN;
           //------------------------------------------------------------
           // UDC a bordo TGV
           //------------------------------------------------------------
           if(Tgv>0 && Tgv<=MAXAGV){
              if(Forc==N_FORCA_A) gruppo_dest = AGV[Tgv]->mission.BoxA.GruppoDest;
              if(Forc==N_FORCA_B) gruppo_dest = AGV[Tgv]->mission.BoxB.GruppoDest;
           }
           //------------------------------------------------------------
           // UDC in Baia
           //------------------------------------------------------------
           if(Bai>0 && Bai<=TOTBAIE){
              if(Forc==N_FORCA_A) gruppo_dest = P.ba[Bai]->BoxA.GruppoDest;
              if(Forc==N_FORCA_B) gruppo_dest = P.ba[Bai]->BoxB.GruppoDest;
           }
           //------------------------------------------------------------
           // UDC in Magazzino
           //------------------------------------------------------------
           if(Mag>0 && Mag<=TOTMAG){
              gruppo_dest = M.m[Mag]->Pos[Pos].Box[Pin].GruppoDest;
           }
        }
        //--------------------------------------
        // SETTAGGIO ALL DATA
        //--------------------------------------
      //if(cmd==CMD_WR_SQL_UDC_ALL     ) CmdSetSql = " SET TecnoChange = 1, Location = " + AnsiString(UdcSQL[idx].location) + ", Status='" + AnsiString(UdcSQL[idx].state) + "'";
        //--------------------------------------
        // SETTAGGIO LOCAZIONE
        //
        // ADELAIDE: ammesso aggiornamento
        //           solo della locazione
        //--------------------------------------
        if(cmd==CMD_WR_SQL_UDC_SET_LOC || cmd==CMD_WR_SQL_UDC_ALL){
           CmdSetSql = " SET TecnoChange = 1, Location = " + AnsiString(UdcSQL[idx].location);
           if(DESTSQLdaTecno_ADELAIDE==true){
              CmdSetSql = CmdSetSql + ", Destination = " + AnsiString(gruppo_dest);
           }
        }
        //--------------------------------------
        // RESET LOCAZIONE
        //--------------------------------------
        if(cmd==CMD_WR_SQL_UDC_RST_LOC ){
           CmdSetSql = " SET TecnoChange = 1, Location = 0 ";
           if(DESTSQLdaTecno_ADELAIDE==true){
              CmdSetSql = CmdSetSql + ", Destination = 0" + AnsiString(gruppo_dest);
           }
        }
        //--------------------------------------
        // RESET CUSTOMER CHANGE
        //--------------------------------------
        if(cmd==CMD_WR_SQL_UDC_RST_CUCH) CmdSetSql = " SET CustomerChange = 0 ";
        //--------------------------------------
        // UPDEAT TABELLA
        //--------------------------------------
        StringaSQL = "UPDATE " + NomeTabA + CmdSetSql + " WHERE HU='" + AnsiString(UdcSQL[idx].Udc) + "'";
        if(cmd==CMD_WR_SQL_UDC_RST_CUCH){
           StringaSQL += " AND CustomerChange <> 0";
        }

        ADOQueryTab->SQL->Clear();
        ADOQueryTab->SQL->Add(StringaSQL);
        ADOQueryTab->ExecSQL(); // apertura data set

     }
     ADOQueryTab->Close();      // chiusura data set
     delete ADOQueryTab;
  }
  catch(Exception &exception){
     errore_comunicazione[COM3][DB_DBASE]=ERR_SQL_RD_TAB;
     sprintf(all_mess, "Query Execution Error on %s \n[ %s ]", NomeTabA, NomeDBase);
     ADOQueryTab->Close();   // chiusura data set
     //----------------------------------------------------------------
     char messaggio[501]="";
     char FileLogExceptionSQL[101]="";
     sprintf(FileLogExceptionSQL, "%sSQL\\LogExceptionSQL.txt", PathProject.c_str());
     sprintf(messaggio,"write_udc_to_SQL_ALLGLASS;%s;%s", all_mess, exception.Message.c_str() );
     messaggio[498]=NULL;    // per garantire il corretto scroll
     file_cronologico_generico(FileLogExceptionSQL, messaggio, 50000L);
     strcpy(messaggio, StringaSQL.c_str());
     messaggio[498]=NULL;    // per garantire il corretto scroll
     file_cronologico_generico(FileLogExceptionSQL, messaggio, 50000L);
     //----------------------------------------------------------------
     delete ADOQueryTab;
     return 9999;
  }
  return err;
*/
  return 0;
}
//---------------------------------------------------------------------------

