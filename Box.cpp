// ------------------------------------------------------------------------
//             file        BOX.CPP
// ------------------------------------------------------------------------
//
#include <stdio.h>
#include <io.h>
#include <mem.h>
#include <string.h>
#include <conio.h>

#include <fcntl.h>
#include <errno.h>
#include <share.h>
#include <sys\stat.h>

#include "Main.h"
#include "SQLDB.h"
#include "Tecnofer.h"
#include "Libreria.h"

// ---------------------------
//       box()
//   costruttore, setta le
//   variabili globali
// ---------------------------
// Costruttore classe box : setta le variabili globali
//
box::box()
/*********************/
{
  // free
}


// -----------------------------------
//    StructToLocation()
// -----------------------------------
// Ottiene codice di locazione dalla struttura in cui si trova il box
//
unsigned int box::StructToLocation(short int Tgv, short int Mac, short int Mag, short int Bai, short int Stz, short int Pos, short int Pin, short int Forc, short int NPal)
/**********************************/
{
  unsigned int val;
  unsigned int location;

  location=0;
  //------------------------------------------
  // Nessun struttura --> locazione "0"
  //------------------------------------------
  if(Tgv==0 && Mac==0 && Mag==0 && Bai==0) return location;
  //------------------------------------------
  // TGV (location = 0TTT000FN)
  // T = Tgv
  // F = Forca (A o B)
  // N = Num pal in presa
  //------------------------------------------
  if(Tgv>0 && Tgv<=MAXAGV){
     location = (int)((100000*Tgv)+(Forc*10)+NPal);
     return location;
  }
  //------------------------------------------
  // MACCHINE (location = 1MMSPP0FN)
  // 1 = PROV_MACCHINE
  // M = Macchina 1..99
  // S = Stazione 0...9
  // P = Posizione in profondità
  // F = Forca (A o B)
  // N = Num pal in presa
  //------------------------------------------
  if(Mac>0 && Mac<=TOTCHIAMATE){
     val = (unsigned int)(PROV_MACCHINE*100000000);
     val = val + (unsigned int)(Mac*1000000);
     val = val + (unsigned int)(Stz*100000);
     val = val + (unsigned int)(Pos*1000);
     val = val + (unsigned int)(Forc*10);
     val = val + (unsigned int)(NPal);
     location = val;
     return location;
  }
  //------------------------------------------
  // MAGAZZINI (location = 2MMMPPL0N)
  // 2 = PROV_MAGAZZINO
  // M = Magazzino 1..999
  // P = Posizione in profondità
  // L = Livello/Piano in altezza
  // N = Num pal in presa
  //------------------------------------------
  if(Mag>0 && Mag<=TOTMAG){
     val = (unsigned int)(PROV_MAGAZZINO*100000000);
     val = val + (unsigned int)(Mag*100000);
     val = val + (unsigned int)(Pos*1000);
     val = val + (unsigned int)(Pin*100);
     val = val + (unsigned int)(NPal);
     location = val;
     return location;
  }
  //------------------------------------------
  // BAIE (location = 3BBB000FN)
  // 3 = PROV_BAIE
  // B = Baia 1..999
  // F = Forca (A o B)
  // N = Num pal in presa
  //------------------------------------------
  if(Bai>0 && Bai<=TOTBAIE){
     val = (unsigned int)(PROV_BAIE*100000000);
     val = val + (unsigned int)(Bai*100000);
     val = val + (unsigned int)(Forc*10);
     val = val + (unsigned int)(NPal);
     location = val;
     return location;
  }
  return 0;
}

// -----------------------------------
//    LocationToStruct()
// -----------------------------------
// Ottiene codice di locazione dalla struttura in cui si trova il box
//
int box::LocationToStruct(unsigned int Loc, short int *Tgv, short int *Mac, short int *Mag, short int *Bai, short int *Stz, short int *Pos, short int *Pin, short int *Forc, short int *NPal)
/**********************************/
{
  unsigned int val;
  short int tipo, num_tgv, num_mac, num_mag, num_bai;
  short int num_stz, num_pos, num_pin, num_forc, num_pal;

  //------------------------------------------
  // Azzeramento variabili di struttura
  //------------------------------------------
  tipo=num_tgv=num_mac=num_mag=num_bai=0;
  num_stz=num_pos=num_pin=num_forc=num_pal=0;
  *Tgv=*Mac=*Mag=*Bai=*Stz=*Pos=*Pin=*Forc=*NPal=0;
  //------------------------------------------
  // Nessun struttura --> locazione "0"
  //------------------------------------------
  if(Loc==0) return 1;
  //------------------------------------------
  // Recupero il TIPO DI STRUTTURA
  //------------------------------------------
  tipo = (short int)(Loc/100000000);
  if(tipo>0) Loc = (unsigned int)(Loc - (tipo*100000000));
  //------------------------------------------
  // TGV (location = 0TTT000FN)
  // T = Tgv
  // F = Forca (A o B)
  // N = Num pal in presa
  //------------------------------------------
  if(tipo==0){
     num_tgv  = (short int)(Loc/100000);
     val      = Loc - (num_tgv*100000);
     num_forc = (short int)(val/10);
     num_pal  = Loc - ((num_tgv*100000)+(num_forc*10));
     if(num_tgv<1         ) return 1;
     if(num_tgv>MAXAGV    ) return 1;
     if(num_pal>MAXUDCBOX ) return 1;
  }
  //------------------------------------------
  // MACCHINE (location = 1MMSPP0FN)
  // 1 = PROV_MACCHINE
  // M = Macchina 1..99
  // S = Stazione 0...9
  // P = Posizione in profondità
  // F = Forca (A o B)
  // N = Num pal in presa
  //------------------------------------------
  if(tipo==PROV_MACCHINE){
     num_mac  = (short int)(Loc/1000000);
     val      = Loc - (num_mac*1000000);
     num_stz  = (short int)(val/100000);
     val      = Loc - ((num_mac*1000000)+(num_stz*100000));
     num_pos  = (short int)(val/1000);
     val      = Loc - ((num_mac*1000000)+(num_stz*100000)+(num_pos*1000));
     num_forc = (short int)(val/10);
     num_pal  = Loc - ((num_mac*1000000)+(num_stz*100000)+(num_pos*1000)+(num_forc*10));
     if(num_mac<1            ) return 1;
     if(num_mac>TOTCHIAMATE  ) return 1;
     if(num_pal>MAXUDCBOX    ) return 1;
  }
  //------------------------------------------
  // MAGAZZINI (location = 2MMMPPL0N)
  // 2 = PROV_MAGAZZINO
  // M = Magazzino 1..999
  // P = Posizione in profondità
  // L = Livello/Piano in altezza
  // N = Num pal in presa
  //------------------------------------------
  if(tipo==PROV_MAGAZZINO){
     num_mag  = (short int)(Loc/100000);
     val      = Loc - (num_mag*100000);
     num_pos  = (short int)(val/1000);
     val      = Loc - ((num_mag*100000)+(num_pos*1000)); //!!!
     num_pin  = (short int)(val/100);
     num_pal  = Loc - ((num_mag*100000)+(num_pos*1000)+(num_pin*100)); //!!!
     if(num_mag<1         ) return 1;
     if(num_mag>TOTMAG    ) return 1;
     if(num_pal>MAXUDCBOX ) return 1;
  }
  //------------------------------------------
  // BAIE (location = 3BBB000FN)
  // 3 = PROV_BAIE
  // B = Baia 1..999
  // F = Forca (A o B)
  // N = Num pal in presa
  //------------------------------------------
  if(tipo==PROV_BAIE){
     num_bai  = (short int)(Loc/100000);
     val      = Loc - (num_bai*100000);
     num_forc = (short int)(val/10);
     num_pal  = Loc - ((num_bai*1000000)+(num_forc*100));
     if(num_bai<1         ) return 1;
     if(num_bai>TOTBAIE   ) return 1;
     if(num_pal>MAXUDCBOX ) return 1;
  }
  //------------------------------------------
  // Compilazione valori per l'esterno
  //------------------------------------------
  *Tgv=num_tgv;
  *Mac=num_mac;
  *Mag=num_mag;
  *Bai=num_bai;
  *Stz=num_stz;
  *Pos=num_pos;
  *Pin=num_pin;
  *Forc=num_forc;
  *NPal=num_pal;
  return 0;
}

// -----------------------------
//  StructToUdcTmp()
// -----------------------------
// Da struttura software ad array temporaneo di UDC
// silos, restituisce errore in caso di superamento dei limiti o
// record non inizializzato
//
int box::StructToUdcTmp(short int cmd, short int Tgv, short int Mac, short int Mag, short int Bai, short int Stz, short int Pos, short int Pin, short int Forc, char *all_mess)
/**********************************/
{
  int idx, err;
  bool trovato;
  unsigned int location;
  short int j, k, z, w, n, cmd_sql;
  struct DATI_PRD UdcTmp[MAXUDCIMPIANTO];

  //------------------------------------------------------------
  // Nessuna struttura passata alla funzione
  //------------------------------------------------------------
  if(Tgv==0 && Mac==0 && Bai==0 && Mag==0) return 0;
  //------------------------------------------------------------
  // Reset struttura UDC TEMPORANEA
  //------------------------------------------------------------
  idx=0;
  memset(&UdcTmp[0], 0, sizeof(UdcTmp));
  //============================================================
  // 1. CMD_UDC_REFRESH o CMD_UDC_SET_LOC
  // Compilo la struttura UdcTmp con gli udc contenuti nella
  // struttura passata alla funzione
  //============================================================
  if(cmd==CMD_UDC_REFRESH || cmd==CMD_UDC_SET_LOC){
     //------------------------------------------------------------
     // MACCHINE
     //------------------------------------------------------------
     if(Mac>0 && Mac<=TOTCHIAMATE){
        if(P.ch[Mac]->Num<1          ) return 0;
        if(P.ch[Mac]->Num>TOTCHIAMATE) return 0;
        for(j=0; j<MAXSTAZMAC; j++){
           if(Stz!=UDC_ALL_STZ && j!=Stz          ) continue;
           if(P.ch[Mac]->Staz[j].PuntoIng<1       ) continue;
           if(P.ch[Mac]->Staz[j].PuntoIng>TOTPUNTI) continue;
           if(P.ch[Mac]->Staz[j].PuntoOut<1       ) continue;
           if(P.ch[Mac]->Staz[j].PuntoOut>TOTPUNTI) continue;
           for(k=0; k<P.ch[Mac]->Staz[j].Npt; k++){
              if(Pos!=UDC_ALL_POS && k!=Pos) continue;
              if(Forc==N_FORCA_AB){
                 // FORCA "A"
                 if(P.ch[Mac]->Staz[j].Pos[k].PresCEST_A!=LIBERA){
                    for(z=0; z<POSINPRESA; z++){
                       for(w=0; w<PIANIINPRESA; w++){
                          if(P.ch[Mac]->Staz[j].Pos[k].BoxA.Presa[z][w]==false) continue;
                          //-------------------------------------------
                          // Verifico se UDC duplicato
                          //-------------------------------------------
                          n=z+(w*POSINPRESA);
                          if(n>MAXUDCBOX) break;
                          location = StructToLocation(0, Mac, 0, 0, j, k, 0, N_FORCA_A, n);
                          memcpy(&UdcTmp[idx], &P.ch[Mac]->Staz[j].Pos[k].BoxA.Prd[n], sizeof(DATI_PRD));
                          UdcTmp[idx].location=location;
                          idx++;
                       }
                    }
                 }
                 // FORCA "B"
                 if(P.ch[Mac]->Staz[j].Pos[k].PresCEST_B!=LIBERA){
                    for(z=0; z<POSINPRESA; z++){
                       for(w=0; w<PIANIINPRESA; w++){
                          if(P.ch[Mac]->Staz[j].Pos[k].BoxB.Presa[z][w]==false) continue;
                          //-------------------------------------------
                          // Verifico se UDC duplicato
                          //-------------------------------------------
                          n=z+(w*POSINPRESA);
                          if(n>MAXUDCBOX) break;
                          location = StructToLocation(0, Mac, 0, 0, j, k, 0, N_FORCA_B, n);
                          memcpy(&UdcTmp[idx], &P.ch[Mac]->Staz[j].Pos[k].BoxB.Prd[n], sizeof(DATI_PRD));
                          UdcTmp[idx].location=location;
                          idx++;
                       }
                    }
                 }
              }
              if(Forc==N_FORCA_A){
                 // FORCA "A"
                 if(P.ch[Mac]->Staz[j].Pos[k].PresCEST_A!=LIBERA){
                    for(z=0; z<POSINPRESA; z++){
                       for(w=0; w<PIANIINPRESA; w++){
                          if(P.ch[Mac]->Staz[j].Pos[k].BoxA.Presa[z][w]==false) continue;
                          //-------------------------------------------
                          // Verifico se UDC duplicato
                          //-------------------------------------------
                          n=z+(w*POSINPRESA);
                          if(n>MAXUDCBOX) break;
                          location = StructToLocation(0, Mac, 0, 0, j, k, 0, N_FORCA_A, n);
                          memcpy(&UdcTmp[idx], &P.ch[Mac]->Staz[j].Pos[k].BoxA.Prd[n], sizeof(DATI_PRD));
                          UdcTmp[idx].location=location;
                          idx++;
                       }
                    }
                 }
              }
              if(Forc==N_FORCA_B){
                 // FORCA "B"
                 if(P.ch[Mac]->Staz[j].Pos[k].PresCEST_B!=LIBERA){
                    for(z=0; z<POSINPRESA; z++){
                       for(w=0; w<PIANIINPRESA; w++){
                          if(P.ch[Mac]->Staz[j].Pos[k].BoxB.Presa[z][w]==false) continue;
                          //-------------------------------------------
                          // Verifico se UDC duplicato
                          //-------------------------------------------
                          n=z+(w*POSINPRESA);
                          if(n>MAXUDCBOX) break;
                          location = StructToLocation(0, Mac, 0, 0, j, k, 0, N_FORCA_B, n);
                          memcpy(&UdcTmp[idx], &P.ch[Mac]->Staz[j].Pos[k].BoxB.Prd[n], sizeof(DATI_PRD));
                          UdcTmp[idx].location=location;
                          idx++;
                       }
                    }
                 }
              }
           }
        }
     }
     //------------------------------------------------------------
     // BAIE
     //------------------------------------------------------------
     if(Bai>0 && Bai<=TOTBAIE){
        if(P.ba[Bai]->Num<1       ) return 0;
        if(P.ba[Bai]->Num>TOTBAIE ) return 0;
        if(Forc==N_FORCA_AB){
           // FORCA "A"
           if(P.ba[Bai]->PresCEST_A!=LIBERA){
              for(z=0; z<POSINPRESA; z++){
                 for(w=0; w<PIANIINPRESA; w++){
                    if(P.ba[Bai]->BoxA.Presa[z][w]==false) continue;
                    //-------------------------------------------
                    // Verifico se UDC duplicato
                    //-------------------------------------------
                    n=z+(w*POSINPRESA);
                    if(n>MAXUDCBOX) break;
                    location = StructToLocation(0, 0, 0, Bai, 0, 0, 0, N_FORCA_A, n);
                    memcpy(&UdcTmp[idx], &P.ba[Bai]->BoxA.Prd[n], sizeof(DATI_PRD));
                    UdcTmp[idx].location=location;
                    idx++;
                 }
              }
           }
           // FORCA "B"
           if(P.ba[Bai]->PresCEST_B!=LIBERA){
              for(z=0; z<POSINPRESA; z++){
                 for(w=0; w<PIANIINPRESA; w++){
                    if(P.ba[Bai]->BoxB.Presa[z][w]==false) continue;
                    //-------------------------------------------
                    // Verifico se UDC duplicato
                    //-------------------------------------------
                    n=z+(w*POSINPRESA);
                    if(n>MAXUDCBOX) break;
                    location = StructToLocation(0, 0, 0, Bai, 0, 0, 0, N_FORCA_B, n);
                    memcpy(&UdcTmp[idx], &P.ba[Bai]->BoxB.Prd[n], sizeof(DATI_PRD));
                    UdcTmp[idx].location=location;
                    idx++;
                 }
              }
           }
        }
        if(Forc==N_FORCA_A){
           // FORCA "A"
           if(P.ba[Bai]->PresCEST_A!=LIBERA){
              for(z=0; z<POSINPRESA; z++){
                 for(w=0; w<PIANIINPRESA; w++){
                    if(P.ba[Bai]->BoxA.Presa[z][w]==false) continue;
                    //-------------------------------------------
                    // Verifico se UDC duplicato
                    //-------------------------------------------
                    n=z+(w*POSINPRESA);
                    if(n>MAXUDCBOX) break;
                    location = StructToLocation(0, 0, 0, Bai, 0, 0, 0, N_FORCA_A, n);
                    memcpy(&UdcTmp[idx], &P.ba[Bai]->BoxA.Prd[n], sizeof(DATI_PRD));
                    UdcTmp[idx].location=location;
                    idx++;
                 }
              }
           }
        }
        if(Forc==N_FORCA_B){
           // FORCA "B"
           if(P.ba[Bai]->PresCEST_B!=LIBERA){
              for(z=0; z<POSINPRESA; z++){
                 for(w=0; w<PIANIINPRESA; w++){
                    if(P.ba[Bai]->BoxB.Presa[z][w]==false) continue;
                    //-------------------------------------------
                    // Verifico se UDC duplicato
                    //-------------------------------------------
                    n=z+(w*POSINPRESA);
                    if(n>MAXUDCBOX) break;
                    location = StructToLocation(0, 0, 0, Bai, 0, 0, 0, N_FORCA_B, n);
                    memcpy(&UdcTmp[idx], &P.ba[Bai]->BoxB.Prd[n], sizeof(DATI_PRD));
                    UdcTmp[idx].location=location;
                    idx++;
                 }
              }
           }
        }
     }
     //------------------------------------------------------------
     // MAGAZZINI
     //------------------------------------------------------------
     if(Mag>0 && Mag<=TOTMAG){
        if(M.m[Mag]->Num<1      ) return 0;
        if(M.m[Mag]->Num>TOTMAG ) return 0;
        for(j=1; j<=M.m[Mag]->Npt; j++){
           if(Pos!=UDC_ALL_POS && Pos!=j) continue;
           for(k=0; k<MAXPIANIMAG; k++){
              if(Pin!=UDC_ALL_LEV && Pin!=k          ) continue;
              if(M.m[Mag]->Pos[j].Presenza[k]==false ) continue;
              for(z=0; z<POSINPRESA; z++){
                 for(w=0; w<PIANIINPRESA; w++){
                    if(M.m[Mag]->Pos[j].Box[k].Presa[z][w]==false) continue;
                    //-------------------------------------------
                    // Verifico se UDC duplicato
                    //-------------------------------------------
                    n=z+(w*POSINPRESA);
                    if(n>MAXUDCBOX) break;
                    location = StructToLocation(0, 0, Mag, 0, 0, j, k, N_FORCA_B, n);
                    memcpy(&UdcTmp[idx], &M.m[Mag]->Pos[j].Box[k].Prd[n], sizeof(DATI_PRD));
                    UdcTmp[idx].location=location;
                    idx++;
                 }
              }
           }
        }
     }
     //------------------------------------------------------------
     // TGV
     //------------------------------------------------------------
     if(Tgv>0 && Tgv<=MAXAGV){
        if(Forc==N_FORCA_AB){
           // FORCA "A"
           if(AGV[Tgv]->mission.PresCEST_A!=LIBERA){
              for(z=0; z<POSINPRESA; z++){
                 for(w=0; w<PIANIINPRESA; w++){
                    if(AGV[Tgv]->mission.BoxA.Presa[z][w]==false) continue;
                    //-------------------------------------------
                    // Verifico se UDC duplicato
                    //-------------------------------------------
                    n=z+(w*POSINPRESA);
                    if(n>MAXUDCBOX) break;
                    location = StructToLocation(Tgv, 0, 0, 0, 0, 0, 0, N_FORCA_A, n);
                    memcpy(&UdcTmp[idx], &AGV[Tgv]->mission.BoxA.Prd[n], sizeof(DATI_PRD));
                    UdcTmp[idx].location=location;
                    idx++;
                 }
              }
           }
           // FORCA "B"
           if(AGV[Tgv]->mission.PresCEST_B!=LIBERA){
              for(z=0; z<POSINPRESA; z++){
                 for(w=0; w<PIANIINPRESA; w++){
                    if(AGV[Tgv]->mission.BoxB.Presa[z][w]==false) continue;
                    //-------------------------------------------
                    // Verifico se UDC duplicato
                    //-------------------------------------------
                    n=z+(w*POSINPRESA);
                    if(n>MAXUDCBOX) break;
                    location = StructToLocation(Tgv, 0, 0, 0, 0, 0, 0, N_FORCA_B, n);
                    memcpy(&UdcTmp[idx], &AGV[Tgv]->mission.BoxB.Prd[n], sizeof(DATI_PRD));
                    UdcTmp[idx].location=location;
                    idx++;
                 }
              }
           }
        }
        if(Forc==N_FORCA_A){
           // FORCA "A"
           if(AGV[Tgv]->mission.PresCEST_A!=LIBERA){
              for(z=0; z<POSINPRESA; z++){
                 for(w=0; w<PIANIINPRESA; w++){
                    if(AGV[Tgv]->mission.BoxA.Presa[z][w]==false) continue;
                    //-------------------------------------------
                    // Verifico se UDC duplicato
                    //-------------------------------------------
                    n=z+(w*POSINPRESA);
                    if(n>MAXUDCBOX) break;
                    location = StructToLocation(Tgv, 0, 0, 0, 0, 0, 0, N_FORCA_A, n);
                    memcpy(&UdcTmp[idx], &AGV[Tgv]->mission.BoxA.Prd[n], sizeof(DATI_PRD));
                    UdcTmp[idx].location=location;
                    idx++;
                 }
              }
           }
        }
        if(Forc==N_FORCA_B){
           // FORCA "B"
           if(AGV[Tgv]->mission.PresCEST_B!=LIBERA){
              for(z=0; z<POSINPRESA; z++){
                 for(w=0; w<PIANIINPRESA; w++){
                    if(AGV[Tgv]->mission.BoxB.Presa[z][w]==false) continue;
                    //-------------------------------------------
                    // Verifico se UDC duplicato
                    //-------------------------------------------
                    n=z+(w*POSINPRESA);
                    if(n>MAXUDCBOX) break;
                    location = StructToLocation(Tgv, 0, 0, 0, 0, 0, 0, N_FORCA_B, n);
                    memcpy(&UdcTmp[idx], &AGV[Tgv]->mission.BoxB.Prd[n], sizeof(DATI_PRD));
                    UdcTmp[idx].location=location;
                    idx++;
                 }
              }
           }
        }
     }
     //-----------------------------------------------------
     // Comando SQL
     //-----------------------------------------------------
     switch(cmd){
        case  CMD_UDC_REFRESH:  cmd_sql=CMD_WR_SQL_UDC_ALL;
                                break;
        case  CMD_UDC_SET_LOC:  cmd_sql=CMD_WR_SQL_UDC_SET_LOC;
                                break;
        case  CMD_UDC_RST_LOC:  cmd_sql=CMD_WR_SQL_UDC_RST_LOC;
                                break;
                      default:  cmd_sql=CMD_NO_SQL;
                                break;
     }        
     //-----------------------------------------------------
     // Funzione di scrittura SQL in base al comando
     //-----------------------------------------------------
     err = FormSQLDB->write_udc_to_SQL(cmd_sql, &UdcTmp[0], all_mess);
     if(err) return 1;
  }
  //============================================================
  // 2. CMD_UDC_RESET_LOC
  // Compilo nell'array UdcTmp le locazioni vuote nella
  // struttura passata alla funzione
  //============================================================
  if(cmd==CMD_UDC_RST_LOC){
     //------------------------------------------------------------
     // MACCHINE
     //------------------------------------------------------------
     if(Mac>0 && Mac<=TOTCHIAMATE){
        if(P.ch[Mac]->Num<1          ) return 0;
        if(P.ch[Mac]->Num>TOTCHIAMATE) return 0;
        for(j=0; j<MAXSTAZMAC; j++){
           if(Stz!=UDC_ALL_STZ && j!=Stz          ) continue;
           if(P.ch[Mac]->Staz[j].PuntoIng<1       ) continue;
           if(P.ch[Mac]->Staz[j].PuntoIng>TOTPUNTI) continue;
           if(P.ch[Mac]->Staz[j].PuntoOut<1       ) continue;
           if(P.ch[Mac]->Staz[j].PuntoOut>TOTPUNTI) continue;
           for(k=0; k<P.ch[Mac]->Staz[j].Npt; k++){
              if(Pos!=UDC_ALL_POS && k!=Pos) continue;
              if(Forc==N_FORCA_AB){
                 // FORCA "A"
                 for(z=0; z<POSINPRESA; z++){
                    for(w=0; w<PIANIINPRESA; w++){
                       if(P.ch[Mac]->Staz[j].Pos[k].BoxA.Presa[z][w]==true) continue;
                       //-------------------------------------------
                       // Verifico se UDC duplicato
                       //-------------------------------------------
                       n=z+(w*POSINPRESA);
                       if(n>MAXUDCBOX) break;
                       location = StructToLocation(0, Mac, 0, 0, j, k, 0, N_FORCA_A, n);
                       UdcTmp[idx].location=location;
                       idx++;
                    }
                 }
                 // FORCA "B"
                 for(z=0; z<POSINPRESA; z++){
                    for(w=0; w<PIANIINPRESA; w++){
                       if(P.ch[Mac]->Staz[j].Pos[k].BoxB.Presa[z][w]==true) continue;
                       //-------------------------------------------
                       // Verifico se UDC duplicato
                       //-------------------------------------------
                       n=z+(w*POSINPRESA);
                       location = StructToLocation(0, Mac, 0, 0, j, k, 0, N_FORCA_B, n);
                       UdcTmp[idx].location=location;
                       idx++;
                    }
                 }
              }
              if(Forc==N_FORCA_A){
                 // FORCA "A"
                 for(z=0; z<POSINPRESA; z++){
                    for(w=0; w<PIANIINPRESA; w++){
                       if(P.ch[Mac]->Staz[j].Pos[k].BoxA.Presa[z][w]==true) continue;
                       //-------------------------------------------
                       // Verifico se UDC duplicato
                       //-------------------------------------------
                       n=z+(w*POSINPRESA);
                       location = StructToLocation(0, Mac, 0, 0, j, k, 0, N_FORCA_A, n);
                       UdcTmp[idx].location=location;
                       idx++;
                    }
                 }
              }
              if(Forc==N_FORCA_B){
                 // FORCA "B"
                 for(z=0; z<POSINPRESA; z++){
                    for(w=0; w<PIANIINPRESA; w++){
                       if(P.ch[Mac]->Staz[j].Pos[k].BoxB.Presa[z][w]==true) continue;
                       //-------------------------------------------
                       // Verifico se UDC duplicato
                       //-------------------------------------------
                       n=z+(w*POSINPRESA);
                       location = StructToLocation(0, Mac, 0, 0, j, k, 0, N_FORCA_B, n);
                       UdcTmp[idx].location=location;
                       idx++;
                    }
                 }
              }
           }
        }
     }
     //------------------------------------------------------------
     // BAIE
     //------------------------------------------------------------
     if(Bai>0 && Bai<=TOTBAIE){
        if(P.ba[Bai]->Num<1       ) return 0;
        if(P.ba[Bai]->Num>TOTBAIE ) return 0;
        if(Forc==N_FORCA_AB){
           // FORCA "A"
           for(z=0; z<POSINPRESA; z++){
              for(w=0; w<PIANIINPRESA; w++){
                 if(P.ba[Bai]->BoxA.Presa[z][w]==true) continue;
                 //-------------------------------------------
                 // Verifico se UDC duplicato
                 //-------------------------------------------
                 n=z+(w*POSINPRESA);
                 location = StructToLocation(0, 0, 0, Bai, 0, 0, 0, N_FORCA_A, n);
                 UdcTmp[idx].location=location;
                 idx++;
              }
           }
           // FORCA "B"
           for(z=0; z<POSINPRESA; z++){
              for(w=0; w<PIANIINPRESA; w++){
                 if(P.ba[Bai]->BoxB.Presa[z][w]==true) continue;
                 //-------------------------------------------
                 // Verifico se UDC duplicato
                 //-------------------------------------------
                 n=z+(w*POSINPRESA);
                 location = StructToLocation(0, 0, 0, Bai, 0, 0, 0, N_FORCA_B, n);
                 UdcTmp[idx].location=location;
                 idx++;
              }
           }
        }
        if(Forc==N_FORCA_A){
           // FORCA "A"
           for(z=0; z<POSINPRESA; z++){
              for(w=0; w<PIANIINPRESA; w++){
                 if(P.ba[Bai]->BoxA.Presa[z][w]==true) continue;
                 //-------------------------------------------
                 // Verifico se UDC duplicato
                 //-------------------------------------------
                 n=z+(w*POSINPRESA);
                 location = StructToLocation(0, 0, 0, Bai, 0, 0, 0, N_FORCA_A, n);
                 UdcTmp[idx].location=location;
                 idx++;
              }
           }
        }
        if(Forc==N_FORCA_B){
           // FORCA "B"
           for(z=0; z<POSINPRESA; z++){
              for(w=0; w<PIANIINPRESA; w++){
                 if(P.ba[Bai]->BoxB.Presa[z][w]==true) continue;
                 //-------------------------------------------
                 // Verifico se UDC duplicato
                 //-------------------------------------------
                 n=z+(w*POSINPRESA);
                 location = StructToLocation(0, 0, 0, Bai, 0, 0, 0, N_FORCA_B, n);
                 UdcTmp[idx].location=location;
                 idx++;
              }
           }
        }
     }
     //------------------------------------------------------------
     // MAGAZZINI
     //------------------------------------------------------------
     if(Mag>0 && Mag<=TOTMAG){
        if(M.m[Mag]->Num<1      ) return 0;
        if(M.m[Mag]->Num>TOTMAG ) return 0;
        for(j=1; j<=M.m[Mag]->Npt; j++){
           if(Pos!=UDC_ALL_POS && Pos!=j) continue;
           for(k=0; k<MAXPIANIMAG; k++){
              if(Pin!=UDC_ALL_LEV && Pin!=k) continue;
              for(z=0; z<POSINPRESA; z++){
                 for(w=0; w<PIANIINPRESA; w++){
                    if(M.m[Mag]->Pos[j].Box[k].Presa[z][w]==true) continue;
                    //-------------------------------------------
                    // Verifico se UDC duplicato
                    //-------------------------------------------
                    n=z+(w*POSINPRESA);
                    location = StructToLocation(0, 0, Mag, 0, j, k, k, N_FORCA_B, n);
                    UdcTmp[idx].location=location;
                    idx++;
                 }
              }
           }
        }
     }
     //------------------------------------------------------------
     // TGV
     //------------------------------------------------------------
     if(Tgv>0 && Tgv<=MAXAGV){
        if(Forc==N_FORCA_AB){
           // FORCA "A"
           for(z=0; z<POSINPRESA; z++){
              for(w=0; w<PIANIINPRESA; w++){
                 if(AGV[Tgv]->mission.BoxA.Presa[z][w]==true) continue;
                 //-------------------------------------------
                 // Verifico se UDC duplicato
                 //-------------------------------------------
                 n=z+(w*POSINPRESA);
                 location = StructToLocation(Tgv, 0, 0, 0, 0, 0, 0, N_FORCA_A, n);
                 UdcTmp[idx].location=location;
                 idx++;
              }
           }
           // FORCA "B"
           for(z=0; z<POSINPRESA; z++){
              for(w=0; w<PIANIINPRESA; w++){
                 if(AGV[Tgv]->mission.BoxB.Presa[z][w]==true) continue;
                 //-------------------------------------------
                 // Verifico se UDC duplicato
                 //-------------------------------------------
                 n=z+(w*POSINPRESA);
                 location = StructToLocation(Tgv, 0, 0, 0, 0, 0, 0, N_FORCA_B, n);
                 UdcTmp[idx].location=location;
                 idx++;
              }
           }
        }
        if(Forc==N_FORCA_A){
           // FORCA "A"
           for(z=0; z<POSINPRESA; z++){
              for(w=0; w<PIANIINPRESA; w++){
                 if(AGV[Tgv]->mission.BoxA.Presa[z][w]==true) continue;
                 //-------------------------------------------
                 // Verifico se UDC duplicato
                 //-------------------------------------------
                 n=z+(w*POSINPRESA);
                 location = StructToLocation(Tgv, 0, 0, 0, 0, 0, 0, N_FORCA_A, n);
                 UdcTmp[idx].location=location;
                 idx++;
              }
           }
        }
        if(Forc==N_FORCA_B){
           // FORCA "B"
           for(z=0; z<POSINPRESA; z++){
              for(w=0; w<PIANIINPRESA; w++){
                 if(AGV[Tgv]->mission.BoxB.Presa[z][w]==true) continue;
                 //-------------------------------------------
                 // Verifico se UDC duplicato
                 //-------------------------------------------
                 n=z+(w*POSINPRESA);
                 location = StructToLocation(Tgv, 0, 0, 0, 0, 0, 0, N_FORCA_B, n);
                 UdcTmp[idx].location=location;
                 idx++;
              }
           }
        }
     }
     //-----------------------------------------------------
     // Comando SQL
     //-----------------------------------------------------
     switch(cmd){
        case  CMD_UDC_REFRESH:  cmd_sql=CMD_WR_SQL_UDC_ALL;
                                break;
        case  CMD_UDC_SET_LOC:  cmd_sql=CMD_WR_SQL_UDC_SET_LOC;
                                break;
        case  CMD_UDC_RST_LOC:  cmd_sql=CMD_WR_SQL_UDC_RST_LOC;
                                break;
                      default:  cmd_sql=CMD_NO_SQL;
                                break;
     }
     //-----------------------------------------------------
     // Funzione di scrittura SQL in base al comando
     //-----------------------------------------------------
     err = FormSQLDB->write_udc_to_SQL(cmd_sql, &UdcTmp[0], all_mess);
     if(err) return 1;
  }
  return 0;
}

// -----------------------------
//   UdcTmpToStruct()
// -----------------------------
// Da struttura UDC temporanea a struttura supervisore
//
// !!! IMPORTANTE !!!
// controllo l'UDC e non la locazione perchè potrebbe non
// essere ancora compilata
//
int box::UdcTmpToStruct(struct DATI_PRD *UdcTmp, char *all_mess)
/**********************************/
{
  int err, idx;
  bool trovato;
  short int trovato_mac;
  short int trovato_bai;
  short int trovato_mag;
  short int trovato_tgv;
  unsigned int location;
  short int j, k, z, w, n;
  short int Mac, Mag, Bai, Tgv;

  //-----------------------------------------------------
  // Scorrimento tutta la stuttura TEMPORANEA UDC
  //-----------------------------------------------------
  err=0;
  trovato_mac=0;
  trovato_bai=0;
  trovato_mag=0;
  trovato_tgv=0;
  for(idx=0; idx<MAXUDCIMPIANTO; idx++){
     if(strcmp(UdcTmp[idx].Udc, "")==0) break;  // al primo non compilato skip!
     //----------------------------------------------
     // Inizializzo variabili di ricerca
     //----------------------------------------------
     trovato=false;
     //----------------------------------------------
     // MACCHINE
     //----------------------------------------------
     for(Mac=1; Mac<=MAXCHIAMATE; Mac++){
        if(P.ch[Mac]->Num<1          ) continue;
        if(P.ch[Mac]->Num>TOTCHIAMATE) continue;
        for(j=0; j<MAXSTAZMAC; j++){
           if(P.ch[Mac]->Staz[j].PuntoIng<1       ) continue;
           if(P.ch[Mac]->Staz[j].PuntoIng>TOTPUNTI) continue;
           if(P.ch[Mac]->Staz[j].PuntoOut<1       ) continue;
           if(P.ch[Mac]->Staz[j].PuntoOut>TOTPUNTI) continue;
           for(k=0; k<P.ch[Mac]->Staz[j].Npt; k++){
              // FORCA "A"
              if(P.ch[Mac]->Staz[j].Pos[k].PresCEST_A!=LIBERA){
                 for(z=0; z<POSINPRESA; z++){
                    for(w=0; w<PIANIINPRESA; w++){
                       if(P.ch[Mac]->Staz[j].Pos[k].BoxA.Presa[z][w]==false) continue;
                       //-------------------------------------------
                       // Controllo se UDC CORRISPONDE
                       //-------------------------------------------
                       n=z+(w*POSINPRESA);
                       if(n>MAXUDCBOX) break;
                       if(strcmp(P.ch[Mac]->Staz[j].Pos[k].BoxA.Prd[n].Udc, UdcTmp[idx].Udc)!=0) continue;
                       //-------------------------------------------
                       // Calcolo locazione e assegnazione dati
                       // a struttura
                       //-------------------------------------------
                       location = StructToLocation(0, Mac, 0, 0, j, k, 0, N_FORCA_A, n);
                       memcpy(&P.ch[Mac]->Staz[j].Pos[k].BoxA.Prd[n], &UdcTmp[idx], sizeof(DATI_PRD));
                       P.ch[Mac]->Staz[j].Pos[k].BoxA.Prd[n].location=location;
                       trovato=true;
                       trovato_mac++;
                       break;
                    }
                    if(trovato==true) break;
                 }
              }
              // FORCA "B"
              if(P.ch[Mac]->Staz[j].Pos[k].PresCEST_B!=LIBERA){
                 for(z=0; z<POSINPRESA; z++){
                    for(w=0; w<PIANIINPRESA; w++){
                       if(P.ch[Mac]->Staz[j].Pos[k].BoxB.Presa[z][w]==false) continue;
                       //-------------------------------------------
                       // Controllo se UDC CORRISPONDE
                       //-------------------------------------------
                       n=z+(w*POSINPRESA);
                       if(n>MAXUDCBOX) break;
                       if(strcmp(P.ch[Mac]->Staz[j].Pos[k].BoxB.Prd[n].Udc, UdcTmp[idx].Udc)!=0) continue;
                       //-------------------------------------------
                       // Calcolo locazione e assegnazione dati
                       // a struttura
                       //-------------------------------------------
                       location = StructToLocation(0, Mac, 0, 0, j, k, 0, N_FORCA_B, n);
                       memcpy(&P.ch[Mac]->Staz[j].Pos[k].BoxB.Prd[n], &UdcTmp[idx], sizeof(DATI_PRD));
                       P.ch[Mac]->Staz[j].Pos[k].BoxB.Prd[n].location=location;
                       trovato=true;
                       trovato_mac++;
                       break;
                    }
                    if(trovato==true) break;
                 }
              }
              if(trovato==true) break;
           }
           if(trovato==true) break;
        }
       if(trovato==true) break;
     }
     //----------------------------------------------
     // Se già trovato non controllo altre strutture
     //----------------------------------------------
     if(trovato==true) continue;
     //----------------------------------------------
     // BAIE
     //----------------------------------------------
     for(Bai=1; Bai<=MAXBAIE; Bai++){
        if(P.ba[Bai]->Num<1       ) return 0;
        if(P.ba[Bai]->Num>TOTBAIE ) return 0;
        // FORCA "A"
        if(P.ba[Bai]->PresCEST_A!=LIBERA){
           for(z=0; z<POSINPRESA; z++){
              for(w=0; w<PIANIINPRESA; w++){
                 if(P.ba[Bai]->BoxA.Presa[z][w]==false) continue;
                 //-------------------------------------------
                 // Controllo se UDC CORRISPONDE
                 //-------------------------------------------
                 n=z+(w*POSINPRESA);
                 if(n>MAXUDCBOX) break;
                 if(strcmp(P.ba[Bai]->BoxA.Prd[n].Udc, UdcTmp[idx].Udc)!=0) continue;
                 //-------------------------------------------
                 // Calcolo locazione e assegnazione dati
                 // a struttura
                 //-------------------------------------------
                 memcpy(&P.ba[Bai]->BoxA.Prd[n], &UdcTmp[idx], sizeof(DATI_PRD));
                 location = StructToLocation(0, 0, 0, Bai, 0, 0, 0, N_FORCA_A, n);
                 P.ba[Bai]->BoxA.Prd[n].location=location;
                 trovato=true;
                 trovato_bai++;
                 break;
              }
              if(trovato==true) break;
           }
        }
        // FORCA "B"
        if(P.ba[Bai]->PresCEST_B!=LIBERA){
           for(z=0; z<POSINPRESA; z++){
              for(w=0; w<PIANIINPRESA; w++){
                 if(P.ba[Bai]->BoxB.Presa[z][w]==false) continue;
                 //-------------------------------------------
                 // Controllo se UDC CORRISPONDE
                 //-------------------------------------------
                 n=z+(w*POSINPRESA);
                 if(n>MAXUDCBOX) break;
                 if(strcmp(P.ba[Bai]->BoxB.Prd[n].Udc, UdcTmp[idx].Udc)!=0) continue;
                 //-------------------------------------------
                 // Calcolo locazione e assegnazione dati
                 // a struttura
                 //-------------------------------------------
                 memcpy(&P.ba[Bai]->BoxB.Prd[n], &UdcTmp[idx], sizeof(DATI_PRD));
                 location = StructToLocation(0, 0, 0, Bai, 0, 0, 0, N_FORCA_B, n);
                 P.ba[Bai]->BoxA.Prd[n].location=location;
                 trovato=true;
                 trovato_bai++;
                 break;
              }
              if(trovato==true) break;
           }
           if(trovato==true) break;
        }
     }
     //----------------------------------------------
     // Se già trovato non controllo altre strutture
     //----------------------------------------------
     if(trovato==true) continue;
     //----------------------------------------------
     // MAGAZZINI
     //----------------------------------------------
     for(Mag=1; Mag<=TOTMAG; Mag++){
        if(M.m[Mag]->Num<1      ) return 0;
        if(M.m[Mag]->Num>TOTMAG ) return 0;
        for(j=1; j<=M.m[Mag]->Npt; j++){
           for(k=0; k<MAXPIANIMAG; k++){
              if(M.m[Mag]->Pos[j].Presenza[k]==false) continue;
              for(z=0; z<POSINPRESA; z++){
                 for(w=0; w<PIANIINPRESA; w++){
                    if(M.m[Mag]->Pos[j].Box[k].Presa[z][w]==false) continue;
                    //-------------------------------------------
                    // Controllo se UDC CORRISPONDE
                    //-------------------------------------------
                    n=z+(w*POSINPRESA);
                    if(n>MAXUDCBOX) break;
                    if(strcmp(P.ba[Bai]->BoxB.Prd[n].Udc, UdcTmp[idx].Udc)!=0) continue;
                    //-------------------------------------------
                    // Calcolo locazione e assegnazione dati
                    // a struttura
                    //-------------------------------------------
                    location = StructToLocation(0, 0, Mag, 0, j, k, k, N_FORCA_B, n);
                    memcpy(&M.m[Mag]->Pos[j].Box[k].Prd[n], &UdcTmp[idx], sizeof(DATI_PRD));
                    M.m[Mag]->Pos[j].Box[k].Prd[n].location=location;
                    trovato=true;
                    trovato_mag++;
                    break;
                 }
                 if(trovato==true) break;
              }
              if(trovato==true) break;
           }
           if(trovato==true) break;
        }
        if(trovato==true) break;
     }
     //----------------------------------------------
     // Se già trovato non controllo altre strutture
     //----------------------------------------------
     if(trovato==true) continue;
     //----------------------------------------------
     // TGV
     //----------------------------------------------
     for(Tgv=1; Tgv<=MAXAGV; Tgv++){
        // FORCA "A"
        if(AGV[Tgv]->mission.PresCEST_A!=LIBERA){
           for(z=0; z<POSINPRESA; z++){
              for(w=0; w<PIANIINPRESA; w++){
                 if(AGV[Tgv]->mission.BoxA.Presa[z][w]==false) continue;
                 //-------------------------------------------
                 // Controllo se UDC CORRISPONDE
                 //-------------------------------------------
                 n=z+(w*POSINPRESA);
                 if(n>MAXUDCBOX) break;
                 if(strcmp(AGV[Tgv]->mission.BoxA.Prd[n].Udc, UdcTmp[idx].Udc)!=0) continue;
                 //-------------------------------------------
                 // Calcolo locazione e assegnazione dati
                 // a struttura
                 //-------------------------------------------
                 location = StructToLocation(Tgv, 0, 0, 0, 0, 0, 0, N_FORCA_A, n);
                 memcpy(&AGV[Tgv]->mission.BoxA.Prd[n], &UdcTmp[idx], sizeof(DATI_PRD));
                 AGV[Tgv]->mission.BoxA.Prd[n].location=location;
                 trovato=true;
                 trovato_tgv++;
                 break;
              }
              if(trovato==true) break;
           }
           if(trovato==true) break;
        }
        // FORCA "B"
        if(AGV[Tgv]->mission.PresCEST_B!=LIBERA){
           for(z=0; z<POSINPRESA; z++){
              for(w=0; w<PIANIINPRESA; w++){
                 if(AGV[Tgv]->mission.BoxB.Presa[z][w]==false) continue;
                 //-------------------------------------------
                 // Controllo se UDC CORRISPONDE
                 //-------------------------------------------
                 n=z+(w*POSINPRESA);
                 if(n>MAXUDCBOX) break;
                 if(strcmp(AGV[Tgv]->mission.BoxB.Prd[n].Udc, UdcTmp[idx].Udc)!=0) continue;
                 //-------------------------------------------
                 // Calcolo locazione e assegnazione dati
                 // a struttura
                 //-------------------------------------------
                 location = StructToLocation(Tgv, 0, 0, 0, 0, 0, 0, N_FORCA_B, n);
                 memcpy(&AGV[Tgv]->mission.BoxB.Prd[n], &UdcTmp[idx], sizeof(DATI_PRD));
                 AGV[Tgv]->mission.BoxB.Prd[n].location=location;
                 trovato=true;
                 trovato_tgv++;
                 break;
              }
              if(trovato==true) break;
           }
           if(trovato==true) break;
        }
     }
  }
  //-----------------------------------------------------
  // Salvataggio stutture interessate dalle modifiche
  //-----------------------------------------------------
  if(!err && trovato_mac>0) err=P.save_file_chiamate(all_mess);
  if(!err && trovato_bai>0) err=P.save_file_baie(all_mess);
  if(!err && trovato_mag>0) err=M.save_file_mag(all_mess);
  if(!err && trovato_tgv>0){
     for(Tgv=1; Tgv<=MAXAGV; Tgv++){
        err = AGV[Tgv]->save_mission(all_mess);
        if(err) break;
     }
  }
  return err;
}

// -----------------------------
//  VerificaUDCBoxDuplicati()
// -----------------------------
// Verifica se dato il numero in memoria e lo copia nella struttura
// silos, restituisce errore in caso di superamento dei limiti o
// record non inizializzato
//
int box::VerificaUDCBoxDuplicati(char *Udc, unsigned int location, char *all_mess)
/**********************************/
{
  bool trovato;
  short int i, j, k, z;
  short int Tgv, Mac, Mag, Bai, Stz, Pos, Pin, Forc, NPal;

  //------------------------------------------------------------
  // Nessun UDC passato alla funzione
  //------------------------------------------------------------
  if(strcmp(Udc, "")==0) return 0;
  //------------------------------------------------------------
  // Descripatazione locazione
  //------------------------------------------------------------
  trovato=false;
  Tgv=Mac=Mag=Bai=Stz=Pos=Pin=Forc=NPal=0;
  LocationToStruct(location, &Tgv, &Mac, &Mag, &Bai, &Stz, &Pos, &Pin, &Forc, &NPal);
  //------------------------------------------------------------
  // Verifica se il BOX è già segnalato sotto una macchina
  //------------------------------------------------------------
  for(i=1; i<=MAXCHIAMATE; i++){
     if(P.ch[i]->Num<1          ) continue;
     if(P.ch[i]->Num>TOTCHIAMATE) continue;
     for(j=0; j<MAXSTAZMAC; j++){
        if(P.ch[i]->Staz[j].PuntoIng<1       ) continue;
        if(P.ch[i]->Staz[j].PuntoIng>TOTPUNTI) continue;
        if(P.ch[i]->Staz[j].PuntoOut<1       ) continue;
        if(P.ch[i]->Staz[j].PuntoOut>TOTPUNTI) continue;
        for(k=0; k<P.ch[i]->Staz[j].Npt; k++){
           if(Forc==N_FORCA_B && P.ch[i]->Staz[j].Pos[k].PresCEST_B!=LIBERA){
              // FORCA "A" (analizzata tutta)
              for(z=0; z<MAXUDCBOX; z++){
                 if(strcmp(P.ch[i]->Staz[j].Pos[k].BoxA.Prd[z].Udc, Udc)!=0) continue;
                 trovato=true;
                 break;
              }
              // FORCA "B" (tranne pallet considerato)
              if(trovato==false){
                 for(z=0; z<MAXUDCBOX; z++){
                    if(i==Mac && j==Stz && k==Pos && z==NPal) continue;
                    if(strcmp(P.ch[i]->Staz[j].Pos[k].BoxB.Prd[z].Udc, Udc)!=0) continue;
                    trovato=true;
                    break;
                 }
              }
           }
           if(Forc==N_FORCA_A && P.ch[i]->Staz[j].Pos[k].PresCEST_A!=LIBERA){
              // FORCA "B" (analizzata tutta)
              for(z=0; z<MAXUDCBOX; z++){
                 if(strcmp(P.ch[i]->Staz[j].Pos[k].BoxB.Prd[z].Udc, Udc)!=0) continue;
                 trovato=true;
                 break;
              }
              // FORCA "A" (tranne pallet considerato)
              if(trovato==false){
                 for(z=0; z<MAXUDCBOX; z++){
                    if(i==Mac && j==Stz && k==Pos && z==NPal) continue;
                    if(strcmp(P.ch[i]->Staz[j].Pos[k].BoxA.Prd[z].Udc, Udc)!=0) continue;
                    trovato=true;
                    break;
                 }
              }
              if(trovato==true) break;
           }
           if(trovato==true){
              sprintf(all_mess, "UDC '%s' already exist in machine %d, station %d, pos %d!", Udc, i, j+1, k+1);
              return 1;
           }
        }
     }
  }

  //------------------------------------------------------------
  // Verifica se il BOX è già segnalato sotto una baia
  //------------------------------------------------------------
  for(i=1; i<=MAXBAIE; i++){
     if(P.ba[i]->Num<1       ) continue;
     if(P.ba[i]->Num>TOTBAIE ) continue;
     if(Forc==N_FORCA_B && P.ba[i]->PresCEST_B!=LIBERA){
        // FORCA "A" (analizzata tutta)
        for(z=0; z<MAXUDCBOX; z++){
           if(strcmp(P.ba[i]->BoxA.Prd[z].Udc, Udc)!=0) continue;
           trovato=true;
           break;
        }
        // FORCA "B" (tranne pallet considerato)
        if(trovato==false){
           for(z=0; z<MAXUDCBOX; z++){
              if(P.ba[i]->Num==Bai && z==NPal            ) continue;
              if(strcmp(P.ba[i]->BoxB.Prd[z].Udc, Udc)!=0) continue;
              trovato=true;
              break;
           }
        }
     }
     if(Forc==N_FORCA_A && P.ba[i]->PresCEST_A!=LIBERA){
        // FORCA "B" (analizzata tutta)
        for(z=0; z<MAXUDCBOX; z++){
           if(strcmp(P.ba[i]->BoxB.Prd[z].Udc, Udc)!=0) continue;
           trovato=true;
           break;
        }
        // FORCA "A" (tranne pallet considerato)
        if(trovato==false){
           for(z=0; z<MAXUDCBOX; z++){
              if(P.ba[i]->Num==Bai && z==NPal            ) continue;
              if(strcmp(P.ba[i]->BoxA.Prd[z].Udc, Udc)!=0) continue;
              trovato=true;
              break;
           }
        }
     }
     if(trovato==true){
        sprintf(all_mess, "UDC '%s' already exist in bay %d!", Udc, i);
        return 1;
     }
  }
  //------------------------------------------------------------
  // Verifica se il BOX è già segnalato sotto un magazzino
  //------------------------------------------------------------
  for(i=1; i<=MAXMAG; i++){
     if(M.m[i]->Num<1     ) continue;
     if(M.m[i]->Num>TOTMAG) continue;
     for(j=1; j<=M.m[i]->Npt; j++){
        for(k=0; k<MAXPIANIMAG; k++){
           if(M.m[i]->Pos[j].Presenza[k]==false) continue;
           for(z=0; z<MAXUDCBOX; z++){
              if(i==Mag && j==Pos && k==Pin && z==NPal) continue;
              if(strcmp(M.m[i]->Pos[j].Box[k].Prd[z].Udc, Udc)!=0) continue;
              trovato=true;
              break;
           }
           if(trovato==true) break;
        }
        if(trovato==true) break;
     }
     if(trovato==true){
        sprintf(all_mess, "UDC '%s' already exist in storage %d, pos %d, level %d!", Udc, i, j, k);
        return 1;
     }
  }
  //------------------------------------------------------------
  // Verifica se il Carrello è già segnalato su un TGV
  //------------------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(i==Tgv ) continue;
     if(Forc==N_FORCA_B && AGV[i]->mission.PresCEST_B!=LIBERA){
        // FORCA "A" (analizzata tutta)
        for(z=0; z<MAXUDCBOX; z++){
           if(strcmp(AGV[i]->mission.BoxA.Prd[z].Udc, Udc)!=0) continue;
           trovato=true;
           break;
        }
        // FORCA "B" (tranne pallet considerato)
        if(trovato==false){
           for(z=0; z<MAXUDCBOX; z++){
              if(z==NPal) continue;
              if(strcmp(AGV[i]->mission.BoxB.Prd[z].Udc, Udc)!=0) continue;
              trovato=true;
              break;
           }
        }
     }
     if(Forc==N_FORCA_A && AGV[i]->mission.PresCEST_A!=LIBERA){
        // FORCA "B" (analizzata tutta)
        for(z=0; z<MAXUDCBOX; z++){
           if(strcmp(AGV[i]->mission.BoxB.Prd[z].Udc, Udc)!=0) continue;
           trovato=true;
           break;
        }
        // FORCA "A" (tranne pallet considerato)
        if(trovato==false){
           for(z=0; z<MAXUDCBOX; z++){
              if(z==NPal) continue;
              if(strcmp(AGV[i]->mission.BoxA.Prd[z].Udc, Udc)!=0) continue;
              trovato=true;
              break;
           }
        }
     }
     if(trovato==true){
        sprintf(all_mess, "UDC '%s' already exist on tgv %d!", Udc, i);
        return 1;
     }
  }
  return 0;
}
