// ------------------------------------------------------------------------
//             file     PRCHIAMA.CPP
// ------------------------------------------------------------------------
// Elenco funzioni di chiamata AGVs
//
// ...
//-------------------------------------------
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <mem.h>
#include <math.h>

#include "tecnofer.h"
#include "libreria.h"
#include "SQLDB.h"

extern int VerificaNodoOccupato(bool RioccupaNodo, int NumAgv, int NodoConsiderato,
                                    int & NodoOccupabile, int & AgvAdiacente, int & satellite_occupato, int & NodoInterruzionePercorso, int ScartaAgv, int SoloAgv );


// ---------------------------
//     STRUTTURE GRAFICHE
//      da GRAPHVAR.CPP
// ---------------------------
extern int Macch[][5];
extern int Baie[][4];
extern int Punti[][5];
extern int BloccoRot[][3];
extern int AccoppiaMagaz[][2];

// ---------------------------
//   STRUTTURA PERCORSO
// ---------------------------
//  0 = Numero nodo partenza
//  1 = Numero nodo arrivo
//-------------------------------------------------------
extern int Percorso[][2];

// ---------------------------
//   new_init_mission()
// ---------------------------
// Questa funzione è il reindirizzamento di init_mission()
// Gli passa gli stessi parametri  ma esegue ulteriori controlli.
// Per lo più questi controlli sono legati alla gestione dei
// PERCORSI ALTERNATIVI.
//
int new_init_mission(bool deadlock, short int forza_tipo_path, short int num_agv, int chiamata, short int dest, short int pos_dest, short int pint, short int tipo_mission, short int TypPresa, bool *Ok, char *all_mess)
{
  int  err=0;
  bool tutto_path_libero;
  bool forza_occupa_tutto;
  bool avvicinamento;
  bool dest_interbloccato;
  bool succ_interbloccato;
  int  okprog, oldagv, aut;
  short int tipo_path;
  short int NodoImpegnato;
  short int pos_tgv, PuntoOccupato, PuntoRot, rot1, rot2;
  short int id, i, j, k, z, punto, num_mag, num_pos, altro_agv, limitrofo, dist, rot;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  short int elenco_interblocchi[TOTPUNTI+1];


  *Ok=false;
  rot=dist=0;
  limitrofo=0;
  tipo_path=FAST_PATH;
  tutto_path_libero=false;
  forza_occupa_tutto=false;
  memset(&nodi_perc, 0, sizeof(nodi_perc));
  memset(&dati_perc, 0, sizeof(dati_perc));
  pos_tgv = AGV[ num_agv ]->stato.pos;
  //---------------------------------------------------------------------
  // PUNTO INTERMEDIO PER AVVICINAMENTO
  //---------------------------------------------------------------------
  avvicinamento=false;
  if(pint!=0) avvicinamento=true;
  //---------------------------------------------------------------------
  // (A) - Verifica condizioni di MISSIONE (duplicato di init_mission())
  //---------------------------------------------------------------------
  //---------------------------------------------------------------------
  // Verifica se AGV in posizione di ricarica con destinazione ricarica
  // -> SKIP
  //---------------------------------------------------------------------
  if(tipo_mission==MISS_SOLO_POSIZIONAMENTO){
     if(pos_tgv==dest && AGV[num_agv]->stato.pos==AGV[num_agv]->mission.PosRicarica) return 0;
  }
  //---------------------------------------------------------------------
  // verifica se AGV pronto a ricevere programma
  //---------------------------------------------------------------------
  okprog = AGV[num_agv]->stato.s.bit.okprog;   // AGV pronto aricevere nuovo prg.
  if(!okprog) return 0;
  //---------------------------------------------------------------------
  // verifica se AGV in automatico
  //---------------------------------------------------------------------
  aut = AGV[num_agv]->stato.s.bit.funz;        // AGV in automatico
  if(!aut) return 0;                           // Test se AGV in automatico
  //---------------------------------------------------------------------
  // verifica se AGV escluso o in riserva con
  // destinazione >< da posizone di ricarica
  //---------------------------------------------------------------------
  if(AGV[num_agv]->test_agv_escluso() ) return 0;
  if(AGV[num_agv]->stato.start        ) return 0;      // Test start >< 0
  if(AGV[num_agv]->mission.pstart     ) return 0;      // missione in corso
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
  //----------------------------------------------------------------------
  // (C) - Verifica se la destinazione ha come nodo contrapposto un nodo
  //       che se occupato da un altro TGV provocherebbe una collisione.
  //
  //
  // 24/06/2011 GESTIONE AVVICINAMENTI
  // In fase di avvicinamento a stoccaggio o avvicinamento per riposo
  // non controllo i nodi contrapposti, perchè comunque il TGV sul
  // PIntForzato modificheranno la loro destinazione
  //
  // 20/07/2011 GESTIONE CONTRAPPOSTI SOLO SU PUNTI NOTEVOLI
  // Se il nodo contrapposto è un nodo di percorso non è considerato
  // in fase di inizializzazione missione. Non vengono controllati i
  // nodi di percorso perchè i TGV sui nodi di percorso si spostano.
  //----------------------------------------------------------------------
  altro_agv=0;
  NodoImpegnato=0;
  punto=N.nodo_contrapposto[dest];
  if(punto!=0 && N.punti_notevoli[punto]==true && avvicinamento==false && pint==0){
     //---------------------------------------------------------------
     // Verifico se il nodo contrapposto è riservato da un altro TGV
     //---------------------------------------------------------------
     if(P.test_dest_riservata(punto, &altro_agv, num_agv) && altro_agv!=0 && altro_agv!=num_agv) NodoImpegnato=altro_agv;
     //---------------------------------------------------------------
     // Verifico se il nodo contrapposto è occupato da un altro TGV
     //---------------------------------------------------------------
     if(P.test_dest_occupata(punto, &altro_agv) && altro_agv!=0 && altro_agv!=num_agv ) NodoImpegnato=altro_agv;
  }
  if(NodoImpegnato) return 0;

  //-------------------------------------------------------
  // NODI INTERBLOCCATI
  // Se il nodo considerato appartiene alla struttura dei
  // nodi interbloccati non ha senso valutarne la
  // "limitroficità" con altri nodi
  //-------------------------------------------------------
  id=0;
  dest_interbloccato=false;
  memset(&elenco_interblocchi, 0, sizeof(elenco_interblocchi));
  for(i=0; i<50; i++){
     for(j=0; j<50; j++){
        if(N.nodi_interbloccati[i][j]!=dest) continue;
        dest_interbloccato = true;
        //--------------------------------------------
        // Compilazione elenco nodi interbloccati
        //--------------------------------------------
        for(k=0; k<50; k++){
           if(N.nodi_interbloccati[i][k]<1        ) continue;
           if(N.nodi_interbloccati[i][k]>TOTPUNTI ) continue;
           elenco_interblocchi[id] = N.nodi_interbloccati[i][k];
           id++;
        }
        break;
     }
     if(dest_interbloccato==true) break;
  }

  //--------------------------------------------------------
  // MAGAZZINI MULTI NODO
  // Se attiva la gestione multi nodo dei magazzini e
  // basta che uno dei nodi sia occupato.
  //--------------------------------------------------------
  if(MagazziniMultiNodo==true){
     num_mag=num_pos=0;
     M.test_punto_presente_mag(all_mess, dest, &num_mag, &num_pos);
     if(num_mag>0 && num_mag<=TOTMAG){
        //-----------------------------------------
        // Aggiungo agli interbloccati tutti i nodi
        // delle posizioni magazzino
        //-----------------------------------------
        for(i=1; i<=MAXTERRAMAG; i++){
           if(M.m[num_mag]->Pos[i].NodoIng<1       ) continue;
           if(M.m[num_mag]->Pos[i].NodoIng>TOTPUNTI) continue;
           dest_interbloccato      = true;
           elenco_interblocchi[id] = M.m[num_mag]->Pos[i].NodoIng;
           id++;
        }
        for(i=1; i<=MAXTERRAMAG; i++){
           if(M.m[num_mag]->Pos[i].NodoOut<1       ) continue;
           if(M.m[num_mag]->Pos[i].NodoOut>TOTPUNTI) continue;
           dest_interbloccato      = true;
           elenco_interblocchi[id] = M.m[num_mag]->Pos[i].NodoOut;
           id++;
        }
     }
  }

  //----------------------------------------------------------------------
  // (D) - Verifico se ci sono dei punti in cui sono costretto a bloccare
  //       la rotazione per evitare la collisione con altre navette.
  //----------------------------------------------------------------------
  for(i=0;;i++){
     if(BloccoRot[i][0]==0) break;
     //---------------------------------------------------------------
     // Inizializzazione variabili
     //---------------------------------------------------------------
     altro_agv=0;
     NodoImpegnato=0;
     PuntoOccupato = (short int)(BloccoRot[i][0]);
     //---------------------------------------------------------------
     // GESTIONE AVVICINAMENTI
     // Il controllo del blocco rotazione deve essere fatto solo
     // per rotazioni di fronte a PT occupati e non quando il
     // punto occupato coincide con la destinazione del TGV
     //---------------------------------------------------------------
     if(PuntoOccupato==dest) continue;
     //---------------------------------------------------------------
     // Nodi limitrofi
     //---------------------------------------------------------------
     if(P.test_dest_limitrofe(PuntoOccupato, &altro_agv, &limitrofo, num_agv) && altro_agv!=0 && altro_agv!=num_agv){
        // --- solo se l'altro AGV ha già la destinazione sicura
        if(AGV[altro_agv]->mission.PIntForzato==0) NodoImpegnato=altro_agv;
     }
     //---------------------------------------------------------------
     // Verifico se il nodo di blocco è riservato da un altro TGV
     //---------------------------------------------------------------
     if(P.test_dest_riservata(PuntoOccupato, &altro_agv, num_agv) && altro_agv!=0 && altro_agv!=num_agv){
        // --- solo se l'altro AGV ha già la destinazione sicura
        if(AGV[altro_agv]->mission.PIntForzato==0) NodoImpegnato=altro_agv;
     }
     //---------------------------------------------------------------
     // Verifico se il nodo di blocco è occupato da un altro TGV
     //---------------------------------------------------------------
     if(N.nodo_busy[PuntoOccupato]!=0 && N.nodo_busy[PuntoOccupato]!=num_agv){
        NodoImpegnato=N.nodo_busy[PuntoOccupato];
     }
     if(NodoImpegnato){
        //----------------------------------------------------------------------
        // Controllo se nel percorso della navetta compare uno dei punti in
        // cui non è possibile ruotare
        //----------------------------------------------------------------------
        for(j=1; j<3; j++){
           if(BloccoRot[i][j]==0) break;
           PuntoRot = (short int)(BloccoRot[i][j]);
           //-------------------------------------------------------------------
           // SELEZIONE DEL TIPO PERCORSO DA FARE
           //-------------------------------------------------------------------
           tipo_path = (short int)(AGV[num_agv]->SelezioneTipoPath(deadlock, forza_tipo_path, AGV[num_agv]->stato.pos, dest, &forza_occupa_tutto, &tutto_path_libero));
           AGV[num_agv]->distanza(tipo_path, 0, dest, nodi_perc, dati_perc, all_mess, &dist, &rot);
           for(k=0; k<MAXPERCORSI; k++){
              if(nodi_perc[k]!=PuntoRot) continue;
              //---------------------------------------------------------------
              // Recupero stato rotazioni sui vari punti
              // NOTA: se mi trovo sul punto di partenza la rotazione
              //       precedente è data dallo stato rotazione TGV
              //---------------------------------------------------------------
              if(k==0){
                 if(AGV[num_agv]->stato.s.bit.grd0  ) rot1 = 0;
                 if(AGV[num_agv]->stato.s.bit.grd90 ) rot1 = 1;
                 if(AGV[num_agv]->stato.s.bit.grd180) rot1 = 2;
                 if(AGV[num_agv]->stato.s.bit.grd270) rot1 = 3;
              }
              else{
                 rot1 = dati_perc[k-1].rot;
              }
              rot2 = dati_perc[k].rot;
              //---------------------------------------------------------------
              // 06/07/2011 GESTIONE AVVICINAMENTI
              // In fase di avvicinamento a stoccaggio non eseguo il cotrollo
              // solo se il punto di blocco rotazione è diverso da quello
              // precedente la destinazione...perchè comunque il TGV si dovrà
              // fermare sul PIntForzato per cambiare la destinazione e quindi
              // non arriverà mai a target
              //---------------------------------------------------------------
              if(avvicinamento==true){
                 //--------------------------------------------------------
                 // Successivo == destinazione -> skip!
                 //--------------------------------------------------------
                 if(nodi_perc[k+1]==dest) continue;
                 //--------------------------------------------------------
                 // INTERBLOCCO NODI
                 // Verifico se la destinazione è parte di un gruppo di
                 // nodi interbloccati
                 //--------------------------------------------------------
                 if(dest_interbloccato==true){
                    succ_interbloccato=false;
                    for(z=0; z<TOTPUNTI; z++){
                       if(elenco_interblocchi[z]<1               ) break;
                       if(elenco_interblocchi[z]>TOTPUNTI        ) break;
                       if(elenco_interblocchi[z]!=nodi_perc[k+1] ) continue;
                       succ_interbloccato=true;
                       break;
                    }
                    //--------------------------------------------------------
                    // il successivo fa parte di uno dei nodi interbloccati
                    // con la destionazione --> skip!
                    //--------------------------------------------------------
                    if(succ_interbloccato==true) continue;
                 }
              }
              //---------------------------------------------------------------
              // Se il TGV deve fare una rotazione --> Skip!!!
              //---------------------------------------------------------------
              if(rot1!=rot2) return 0;
           }
        }
     }
  }
  //--------------------------------------------------------------------
  // (F) - INIZIALIZZAZIONE DELLA MISSIONE
  //--------------------------------------------------------------------
  err = P.init_mission(deadlock, forza_tipo_path, num_agv, chiamata, dest, pos_dest, pint, tipo_mission, TypPresa, Ok, all_mess);
  return err;
}

// -----------------------------------
//  SbloccoDeadLock()
// -----------------------------------
// Verifica le presenza di una condizione di "Dead Lock", ossia
// in cui più TGV si interbloccano tra loro e individua a quale
// dare una missione di sblocco che permetta anche agli altri di
// rimettersi in movimento.
//
void process::SbloccoDeadLock()
/********************************************/
{
  int  AgvBloccante;
  int  TerzoAgv;
  int x1, y1, x2, y2;
  int NumAll, CtrAll;
  char all_mess[151]="";
  bool Ok, TgvEscluso;
  bool AltriAgvVicini;
  bool no_variato;
  bool Trovato=false;
  bool NoGestioneCurve;
  bool almeno_un_nodo_occupato;
  bool Prosegui, stop_fuori_ing;
  bool zona_fuori_ingombro_manuale;
  bool agv_in_rettaY, agv_in_rettaX;
  bool altro_agv_in_rettaY, altro_agv_in_rettaX;
  bool AllBit, Interferenza;
  short int i, j, k, z, idx;
  short int altro_agv, num_agv;
  short int NodoBloccante, NodoSucc;
  short int Tappi[MAXAGV+1];
  short int p1, p2;
  double Dist;
  int    DistX, DistY;
  int    Vicino, count_fuori_ing;
  short int PuntoSucessivo, PuntoPrenotato;
  int    ptr_percorso, ptrpath_pos_adiacente;
  int    satellite_occupato1, NodoInterruzionePercorso1, ptr_percorso1;
  int    NodoCosiderato1, NodoOccupabile1, AgvPerInterruzionePercorso1;
  int    RotSuccDaFermo, rotp1, rotp2, xp1, yp1, xp2, yp2;
  short int dirrot_numagv, prec_numagv;
  short int dirrot_altroagv, punto_altroagv, prec_altroagv;
  ingombroagv *IngombroAgv1, *IngombroAgv2;

  //------------------------------------
  // Variabil.h -> Disattivazione curve
  //------------------------------------
  NoGestioneCurve=false;
  #ifndef GESTIONE_CURVE
     NoGestioneCurve=true;
  #endif;

  NodoSucc=0;
  memset(&Tappi, 0, sizeof(Tappi));
  //--------------------------------------------------
  // Prima di attivare lo sblocco verifico se
  // spostare TGV fermo
  //--------------------------------------------------
  for(i=1; i<=MAXAGV; i++) AGV[i]->SpostaTGVInAttesa();

  //--------------------------------------------------
  // SBLOCCO DEAD LOCK DISATTIVATO
  // NumCicliPerSbloccoDeadLock = 9999
  //--------------------------------------------------
  if(NumCicliPerSbloccoDeadLock==9999) return;
  //--------------------------------------------------------------
  // Verifico se i TGV sono effettivamente bloccati
  // GB-06/06/2014 Controllo aggiuntivo per TGV su PuntiNotevoli
  //               Verifico se i TGV sono effettivamente bloccati
  //--------------------------------------------------------------
  for(j=1; j<=MAXAGV; j++){
     //-----------------------------------------------
     // Se TGV bloccante su nodo terminale -> SBLOCCA
     //-----------------------------------------------
     if(AgvBloccati[AgvBloccati[j]]==0){
        AgvBloccante =AgvBloccati[j];
        //-------------------------------------------------------
        // Bloccanti anche TGV su punti notevoli in ingombro
        // - Punto Notevole
        // - Punto in ingombro
        // - Tgv fermo senza missione
        // - Nessun TGV a distanza < MAX_ROT_DOPPIA
        //-------------------------------------------------------
        AltriAgvVicini=false;
        if(AgvBloccante>0 && AgvBloccante<=MAXAGV){
           NodoBloccante=AGV[AgvBloccante]->stato.pos;
           if(AGV[AgvBloccante]->test_mission_on()==false && AGV[AgvBloccante]->stato.start==0 && AGV[AgvBloccante]->stato.s.bit.funz && AGV[AgvBloccante]->mission.SemiAutomatico==false ){
              if(N.punti_notevoli[NodoBloccante]>0 && N.pt_in_ingombro[NodoBloccante]>0){
                 //----------------------------------
                 // Verifica TGV VICINI
                 //----------------------------------
                 for(i=1; i<=MAXAGV; i++){
                    //---------------------------------------------------
                    // Verifico la distanza tra TGV e altri nei dintorni
                    //---------------------------------------------------
                    if(i==AgvBloccante) continue;
                    Dist=DistX=DistY=0;
                    p1 = NodoBloccante;
                    p2 = AGV[i]->stato.pos;
                    x1 = Punti[ p1 ][0];
                    y1 = Punti[ p1 ][1];
                    x2 = Punti[ p2 ][0];
                    y2 = Punti[ p2 ][1];
                    DistX = abs(y1-y2);
                    DistY = abs(x1-x2);
                    if(DistX>30000 || DistY>30000) continue;
                    Dist = (double)(sqrt((DistX*DistX)+(DistY*DistY)));
                    //--------------------------------------------
                    // Se la distanza mi garantisce la rotazione
                    //    non è necessario settare il bit
                    //--------------------------------------------
                    if(Dist>=DIST_MIN_ROT_DOPPIA_MAX) continue;
                    //--------------------------------------------
                    // Stop per altro TGV fermo
                    //--------------------------------------------
                    AltriAgvVicini=true;
                    break;
                 }
              }
              if(AltriAgvVicini) break;
           }
           else{
              //--------------------------------------------
              // Se TGV bloccante non bloccato ---> Skip!!!
              //--------------------------------------------
              if(AgvBloccati[AgvBloccati[j]]==0){
                 AgvBloccati[j]=0;
                 continue;
              }
           }              
        }
        if(AltriAgvVicini==false) continue;
        //--------------------------------------------
        // Se TGV bloccante non bloccato ---> Skip!!!
        //--------------------------------------------
        AgvBloccati[j]=0;
        continue;
     }
  }

  //-------------------------------------------------------
  // AL-28/06/2014 - TGV FERMI sul loro PINTFORZATO non
  // sono ritenuti bloccanti perchè in attesa di sblocco
  //-------------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(AgvBloccati[i]<1     ) continue;
     if(AgvBloccati[i]>MAXAGV) continue;
     AgvBloccante = AgvBloccati[i];
     //---------------------------------------------------
     // TGV BLOCCANTE ancora in movimento
     //---------------------------------------------------
     if(!AGV[AgvBloccante]->test_mission_on()) continue;  // AL-29/11/2019 controllo valido solo con TGV in missione
     if(AGV[AgvBloccante]->mission.PIntForzato!=0 && AGV[AgvBloccante]->mission.pintend==AGV[AgvBloccante]->mission.PIntForzato){
        if(AGV[AgvBloccante]->stato.pos==AGV[AgvBloccante]->mission.pintend) AgvBloccati[i]=0;
     }
  }

  //-------------------------------------------------------
  // Verifica che i TGV bloccati non abbiano altri nodi
  // occupati oltre a quello sul quale sono posizionati.
  // Questo è un ulteriore controllo di sicurezza per
  // verificare che i TGV siano effettivamente bloccati.
  //-------------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(AgvBloccati[i]==0          ) continue;
     if(!AGV[i]->test_mission_on() ) continue;  // AL-29/11/2019 controllo valido solo con TGV in missione
     //----------------------------------------
     // Recupera posizione attuale nel percorso
     //----------------------------------------
     for(j=0; j<MAXPERCORSI; j++){
        if(AGV[i]->mission.punto[j]==AGV[i]->stato.pos){
           //------------------------------------------------
           // Verifico se il punto sucessivo alla posizione
           // attuale del TGV è occupato da TGV stesso.
           //------------------------------------------------
           NodoSucc=AGV[i]->mission.punto[j+1];
           if(N.nodo_busy[NodoSucc]==i){
              //------------------------------------------------
              // Se il punto è occupato il TGV non si può
              // ritenere Bloccato.
              //------------------------------------------------
              AgvBloccati[i]=0;
              break;
           }
        }
     }
  }

  //------------------------------------------------------------
  // 05/06/2014 - Un TGV è ritenuto realmente bloccante se
  //              ha già raggiunto il suo pint e quindi è
  //              FERMO!!!
  //------------------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(AgvBloccati[i]<1     ) continue;
     if(AgvBloccati[i]>MAXAGV) continue;
     AgvBloccante = AgvBloccati[i];
     //---------------------------------------------------
     // TGV BLOCCANTE ancora in movimento
     //---------------------------------------------------
     if(!AGV[AgvBloccante]->test_mission_on()                           ) continue;  // AL-29/11/2019 controllo valido solo con TGV in missione
     if(AGV[AgvBloccante]->mission.pintend==0                           ) continue;
     if(AGV[AgvBloccante]->mission.pintend==AGV[AgvBloccante]->stato.pos) continue;
     AgvBloccati[i]=0;
  }
  //------------------------------------------------------------
  // 12/10/2004 Verifico che tra la posizione AGV BLOCCATO
  //            e un eventuale AGV BLOCCANTE 'X' ci siano dei
  //            nodi OCCUPATI da altri AGV in quel caso
  //            l'AGV "X" NON è bloccante -->SKIP
  //------------------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(AgvBloccati[i]==0) continue;
     AgvBloccante = AgvBloccati[i];
     //----------------------------------------
     // Recupera posizione attuale nel percorso
     //----------------------------------------
     for(j=0; j<MAXPERCORSI; j++){
        if(AGV[i]->mission.punto[j]==AGV[i]->stato.pos) break;
     }
     //---------------------------------------------------
     // Prosegui il controllo lungo il resto del percorso
     //---------------------------------------------------
     for( ; j<MAXPERCORSI; j++){
        NodoSucc = AGV[i]->mission.punto[j];
        if(AGV[i]->mission.punto[j]==0                  ) break;
        if( N.nodo_busy[NodoSucc]==AgvBloccante         ) break;
        if( nodi_prenotati[NodoSucc][AgvBloccante-1]==0 ) continue;
        //--------------------------------------------------
        // Se il punto è occupato da altro AGV
        // diverso da quello bloccante --> il TGV non si può
        // ritenere Bloccato.
        //--------------------------------------------------
        TerzoAgv  = N.nodo_busy[ NodoSucc ];
        if( TerzoAgv!=0 && TerzoAgv!=i && TerzoAgv!=AgvBloccante ){
           //----------------------------------------
           // Se Terzo incomodo occupa il percorso ed
           // è in movimento -->azzera Blocco
           //----------------------------------------
           for(k=0; k<MAXPERCORSI; k++){
              if(AGV[TerzoAgv]->mission.punto[k]==0) break;
              if(AGV[TerzoAgv]->mission.punto[k]!=AGV[TerzoAgv]->stato.pos ) continue;
              if(N.nodo_busy[ AGV[TerzoAgv]->mission.punto[k+1] ]!=TerzoAgv) continue;
              AgvBloccati[i]=0;
              break;
           }
        }
        if(AgvBloccati[i]==0) break;
     }
  }

  //--------------------------------------------------------------
  // Verifico se i TGV sono effettivamente bloccati
  // AL-09/09/2014 TGV in missione o non su punti_notevoli
  //               non sono ritenuti bloccanti se a loro volta
  //               non sono bloccati
  // AL-12/02/2016 Ciclo multiplo fino a che la struttura non
  //               varia
  // AL-14/06/2016 Questo ciclo multiplo va fatto per ultimo
  //               deve essere l'ultima pulizia della struttura
  //               AgvBloccati[]
  //--------------------------------------------------------------
  do{
     no_variato=true;
     for(j=1; j<=MAXAGV; j++){
        //-----------------------------------
        // Se TGV non bloccato ---> Skip!!!
        //-----------------------------------
        if(AgvBloccati[j]==0) continue;
        //--------------------------------------------
        // Se TGV bloccante non bloccato ---> Skip!!!
        // Se ha la missione in corso o è lungo un
        // percorso non è bloccante se a sua volta
        // non è bloccato
        //--------------------------------------------
        if(AgvBloccati[AgvBloccati[j]]==0){
           AgvBloccante =AgvBloccati[j];
           if(AgvBloccante>0 && AgvBloccante<=MAXAGV){
              if(AGV[AgvBloccante]->test_mission_on() || (!AGV[AgvBloccante]->test_mission_on() && N.punti_notevoli[AGV[AgvBloccante]->stato.pos]==false)){
                 AgvBloccati[j]=0;
                 no_variato=false;
                 continue;
              }
           }
        }
     }
  }while(no_variato==false);

  //-------------------------------------------------------
  // Se uno tra i TGV bloccati è in MANUALE o ESCLUSO
  // evito il Dead Lock perchè potrebbe essere lui la
  // causa del blocco.
  //-------------------------------------------------------
  TgvEscluso=false;
  for(i=1; i<=MAXAGV; i++){
     if(AgvBloccati[i]==0) continue;
     //------------------------------------------------------
     AllBit=AGV[AgvBloccati[i]]->stato.s.bit.allarme;
     //------------------------------------------------------
     // 15/03/2015 Rick Se c'è un solo allarme e siamo in batteria scarica
     // ignoro (è solo batteria)
     //------------------------------------------------------
     CtrAll=0;
     for(NumAll=0;NumAll<=(16*MAXLRALLARMI);NumAll++){
        if(TestBit((char *)&AGV[AgvBloccati[i]]->old_allarm[0], NumAll)==true){
           CtrAll++;
           if(CtrAll>=2) break;
        }
     }
     if(CtrAll<=1 && AGV[AgvBloccati[i]]->stato.s.bit.ris) AllBit=false;
     //------------------------------------------------------

     if(AGV[AgvBloccati[i]]->test_agv_escluso() || AllBit ){
        TgvEscluso=true;
        break;
     }
  }
  if(TgvEscluso) memset(&AgvBloccati, 0, sizeof(AgvBloccati));

  //-------------------------------------------------------
  // Verifica se la condizione di Blocco rimane inalterata
  // per 15 cicli di "RunAutomatico()" solo in quel caso
  // tento il reset della missione.
  // Se non sono in una condizione di Blocco --> Skip!!!
  //-------------------------------------------------------
  if(memcmp(&AgvBloccati, &Tappi, sizeof(AgvBloccati))==0){
     CountCondizioneDiBlocco =0;
     memset(&BloccoInvariato, 0, sizeof(BloccoInvariato));
     return;
  }
  //--------------------------------------------------------
  // Se la condizione di Blocco è variata rispetto a quella
  // verificata il ciclo precedente azzero il conteggio dei
  // cicli di "RunAutomatico()".
  //--------------------------------------------------------
  if(CountCondizioneDiBlocco<NumCicliPerSbloccoDeadLock){
     if(memcmp(&AgvBloccati, &BloccoInvariato, sizeof(BloccoInvariato))){
        CountCondizioneDiBlocco =0;
        memcpy(&BloccoInvariato, &AgvBloccati, sizeof(BloccoInvariato));
     }
     else{
        CountCondizioneDiBlocco++;
     }
     return;
  }
  //----------------------------------------------
  // GESTIONE PRECEDENZE PER TGV IN RETTILINEO
  // Se permane una condizione di stallo la
  // prima cosa da fare è assicurarsi che la
  // struttura dei nodi prenotati per tgv
  // lanciati in rettilineo venga azzerata
  //----------------------------------------------
  Trovato=false;
  for(i=1; i<=MAXAGV; i++){
     if(AgvBloccati[i]==0) continue;
     for(j=0; j<=MAXPUNTI; j++){
        if(N.nodo_con_tgv_lanciato[j]!=AgvBloccati[i]) continue;
        N.nodo_con_tgv_lanciato[j]=0;
        Trovato=true;
     }
  }
  //----------------------------------------------
  // Se è variata la situazione di nodi prenotati
  // dai TGV lanciati in rettilineo vale la pena
  // aspettare un altro ciclo prima di procedere
  // al DeadLock
  //----------------------------------------------
  if(Trovato==true) return;
  //----------------------------------------------
  // Reset struttura di stato navette bloccate
  //----------------------------------------------
  idx=0;
  CountCondizioneDiBlocco=0;
  memset(&BloccoInvariato, 0, sizeof(BloccoInvariato));
  //----------------------------------------------
  // Compilazione di una tabella contenente tutti
  // i TGV interessati nel BLOCCO nell'ordine con
  // il quale tenterò di muoverli allo scopo di
  // risolvere il "Dead Lock":
  // 1) I TGV che si BLOCCANO a vicenda.
  // 2) Tutti i TGV che sono sia BLOCCATI che
  //    BLOCCNATI.
  // 3) Tutti i TGV solo BLOCCATI.
  //---------------------------------------------
  //--- 1 ---
  for(i=1; i<=MAXAGV; i++){
     //--------------------------------------------
     // Se il TGV che sto considerando è già stato
     // inserito nella tabella Tappi[] --> Skip!!!
     //--------------------------------------------
     Trovato=false;
     for(j=1; j<=MAXAGV; j++){
        if(Tappi[j]==i){Trovato=true; break;}
     }
     if(Trovato) continue;
     //--------------------------------------------
     // Se TGV non bloccato ---> Skip!!!
     //--------------------------------------------
     if(AgvBloccati[i]==0) continue;
     //--------------------------------------------
     // I TGV che si BLOCCANO a vicenda.
     //--------------------------------------------
     altro_agv=AgvBloccati[i];
     if(AgvBloccati[altro_agv]==i){
        idx++;
        Tappi[idx]=i;
        idx++;
        Tappi[idx]=altro_agv;
        break;
     }
  }
  //--- 2 ---
  for(i=1; i<=MAXAGV; i++){
     //--------------------------------------------
     // Se il TGV che sto considerando è già stato
     // inserito nella tabella Tappi[] --> Skip!!!
     //--------------------------------------------
     Trovato=false;
     for(j=1; j<=MAXAGV; j++){
        if(Tappi[j]==i){Trovato=true; break;}
     }
     if(Trovato) continue;
     //--------------------------------------------
     // Se TGV non bloccato ---> Skip!!!
     //--------------------------------------------
     if(AgvBloccati[i]==0) continue;
     //--------------------------------------------
     // Verifica se TGV bloccato è anche bloccante
     // del TGV che a sua volta lo blocca.
     //--------------------------------------------
     for(j=1; j<=MAXAGV; j++){
        if(j==i) continue;
        //-----------------------------------
        // Se TGV non bloccato ---> Skip!!!
        //-----------------------------------
        if(AgvBloccati[j]==0) continue;
        //-----------------------------------
        // TAPPO TROVATO
        //-----------------------------------
        if(AgvBloccati[j]==i){
           idx++;
           Tappi[idx]=i;
           break;
        }
     }
  }
  //--- 3 ---
  for(i=1; i<=MAXAGV; i++){
     Trovato=false;
     for(j=1; j<=MAXAGV; j++){
        if(Tappi[j]==i){Trovato=true; break;}
     }
     if(Trovato) continue;
     //--------------------------------------------
     // Tgv Restanti        ---> Skip!!!
     //--------------------------------------------
     if(AgvBloccati[i]==0) continue;
     idx++;
     Tappi[idx]=i;
  }

  //---------------------------------------------
  // CONTINUA PER ZONA FUORI INGOMBRO
  // Inizializzazione di missioni fasulle per i
  // TGV ritenuti dei TAPPI
  //---------------------------------------------
  Prosegui=false;
  if(ProseguiPerSbloccoDeadLock==true){
     if(NoGestioneCurve==true || ZoneFuoriIngManConStopPrenotati==true || AbilitaGestioneIngombroCurvePerStopTGV==0){
        for(i=1; i<=MAXAGV; i++){
           if(Tappi[i]==0                          ) break;
           if(Prosegui==true                       ) break;
           if(AGV[Tappi[i]]->mission.SemiAutomatico) continue;
           //--------------------------------------------------------------------------------
           // Cerca punto del percorso all'interno della missione
           // se NON presente --> SKIP
           //--------------------------------------------------------------------------------
           Prosegui=false;
           PuntoSucessivo=0;
           num_agv  =Tappi[i];
           altro_agv=AgvBloccati[Tappi[i]];
           for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
              if(AGV[num_agv]->mission.punto[ptr_percorso]!=AGV[num_agv]->stato.pos     ) continue;
              if(AGV[num_agv]->mission.punto[ptr_percorso]==AGV[num_agv]->mission.pend  ) break;
              if(AGV[num_agv]->mission.punto[ptr_percorso]==0                           ) break;
              PuntoSucessivo = AGV[num_agv]->mission.punto[ptr_percorso+1];
              break;
           }
           //-------------------------------------------------------------------
           // Verifico se ALTRO AGV in RETTA
           //-------------------------------------------------------------------
           if(PuntoSucessivo!=0 && N.nodo_busy[PuntoSucessivo]==0 && AGV[altro_agv]->test_mission_on()){
              agv_in_rettaY=agv_in_rettaX=false;
              rotp1 = AGV[num_agv]->mission.dati_perc[ptr_percorso+0].rot;
              rotp2 = AGV[num_agv]->mission.dati_perc[ptr_percorso+1].rot;
              if(rotp1<10 && rotp2<10 && rotp1==rotp2){
                 if(rotp1==0 || rotp1==2) agv_in_rettaY=true;
                 if(rotp1==1 || rotp1==3) agv_in_rettaX=true;
              }
              //---------------------------------------------------------------------------
              // Se il TGV non è già in retta verifico se sta per entrare in un tratto
              // rettilineo di almeno MINPERCORSI_FUORI_INGOMBRO
              //---------------------------------------------------------------------------
              if(!agv_in_rettaY && !agv_in_rettaX){
                 count_fuori_ing=0;
                 rotp1 = AGV[num_agv]->mission.dati_perc[ptr_percorso+1].rot;
                 for(k=1; k<=MINPERCORSI_FUORI_INGOMBRO; k++){
                    if(AGV[num_agv]->mission.punto[ptr_percorso+1+k]==0) break;
                    //----------------------------------------------------
                    // conto i nodi con stessa rotazione
                    //----------------------------------------------------
                    rotp2 = AGV[num_agv]->mission.dati_perc[ptr_percorso+1+k].rot;
                    if(rotp2!=rotp1) break;
                    count_fuori_ing++;
                 }
                 if(count_fuori_ing>=MINPERCORSI_FUORI_INGOMBRO){
                    if(rotp1==0 || rotp1==2) agv_in_rettaY=true;
                    if(rotp1==1 || rotp1==3) agv_in_rettaX=true;
                 }                 
              }
              //---------------------------------------------------------------------------
              // TGV già in retta o in ingresso a una retta.
              //---------------------------------------------------------------------------
              if(agv_in_rettaY || agv_in_rettaX){
                 //---------------------------------------------------------------------------
                 // Se proseguendo nella missione del TGV trovo almeno un nodo occupato
                 // da alto_agv allora le due missioni si intralciano altrimenti NO.
                 //---------------------------------------------------------------------------
                 Prosegui=true;
                 count_fuori_ing=0;
                 stop_fuori_ing=false;
                 almeno_un_nodo_occupato=false;
                 NodoOccupabile1=AgvPerInterruzionePercorso1=satellite_occupato1=NodoInterruzionePercorso1=0;
                 for(k=ptr_percorso; k<MAXPERCORSI; k++){
                    if(AGV[num_agv]->mission.punto[k]==0) break;
                    PuntoPrenotato = AGV[num_agv]->mission.punto[k];
                    //-----------------------------------------------
                    // Nodo occupato da altro agv
                    //-----------------------------------------------
                    if(N.nodo_busy[PuntoPrenotato]==altro_agv){
                       almeno_un_nodo_occupato=true;
                       break;
                    }
                 }
                 //-----------------------------------------------
                 // VERIFICA ZONA FUORI INGOMBRO
                 //-----------------------------------------------
                 if(almeno_un_nodo_occupato==true){
                    for(k=ptr_percorso; k<MAXPERCORSI; k++){
                       if(Prosegui==false             ) break;
                       if(AGV[num_agv]->mission.punto[k]==0 ) break;
                       PuntoPrenotato = AGV[num_agv]->mission.punto[k];
                       //-----------------------------------------------
                       // Nodo occupato da altro agv
                       //-----------------------------------------------
                       if(N.nodo_busy[PuntoPrenotato]==altro_agv){
                          Prosegui=false;
                          break;
                       }
                       //-----------------------------------------------------
                       // Verifica se nodo occupabile
                       //-----------------------------------------------------
                       VerificaNodoOccupato(FALSE, num_agv, PuntoPrenotato, NodoOccupabile1, AgvPerInterruzionePercorso1, satellite_occupato1, NodoInterruzionePercorso1, 0, 0);
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
                             if(count_fuori_ing>=MIN_NODI_FUORI_INGOMBRO) break;
                          }
                       }
                    }
                 }
                 //---------------------------------------------------------------------------
                 // Tutti i nodi prenotati non devono incontrare un nodo occupato a
                 // una distanza rotazione doppia dal TGV bloccante
                 //---------------------------------------------------------------------------
                 count_fuori_ing=0;
                 stop_fuori_ing=false;
                 almeno_un_nodo_occupato=false;
                 for(k=ptr_percorso+1; k<MAXPERCORSI; k++){
                    if(Prosegui==false                      ) break;
                    if(AGV[num_agv]->mission.punto[k+1]==0  ) break;
                    PuntoPrenotato = AGV[num_agv]->mission.punto[k];
                    //----------------------------------------------------------------------
                    // Verifico che nemmeno tutti i nodi Vicini a quello considerato
                    //----------------------------------------------------------------------
                    for(Vicino=10; Vicino<=TOTPUNTI; Vicino++){
                       if(PuntoPrenotato==Vicino             ) continue;
                       if(N.pt_in_ingombro[Vicino]==false &&
                          N.punti_notevoli[Vicino]==true     ) continue;   // Punto terminale non a rischio
                       if(N.n[Vicino]->num==0                ) continue;
                       if(N.nodo_busy[Vicino]!=altro_agv     ) continue;
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
                 //-----------------------------------------------
                 // VERIFICA ZONA FUORI INGOMBRO
                 //-----------------------------------------------
                 if(almeno_un_nodo_occupato==true){
                    for(k=ptr_percorso+1; k<MAXPERCORSI; k++){
                       if(Prosegui==false                ) break;
                       if(AGV[num_agv]->mission.punto[k+1]==0  ) break;
                       PuntoPrenotato = AGV[num_agv]->mission.punto[k];
                       xp1 = Punti[ PuntoPrenotato ][0];
                       yp1 = Punti[ PuntoPrenotato ][1];
                       agv_in_rettaY=agv_in_rettaX=false;
                       rotp1 = AGV[num_agv]->mission.dati_perc[k+0].rot;
                       rotp2 = AGV[num_agv]->mission.dati_perc[k+1].rot;
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
                          //-------------------------------------------
                          // Verifica se nodo occupabile
                          //-------------------------------------------
                          VerificaNodoOccupato(FALSE, num_agv, PuntoPrenotato, NodoOccupabile1, AgvPerInterruzionePercorso1, satellite_occupato1, NodoInterruzionePercorso1, 0, 0);
                          //-----------------------------------------------------
                          // Se anche solo un nodo non è occupabile il percorso
                          // è da ritenersi prenotato
                          //-----------------------------------------------------
                          if(NodoOccupabile1==0){
                             zona_fuori_ingombro_manuale = N.verifica_zona_fuori_ingombro_manuale(num_agv, PuntoPrenotato, (short int)(AgvPerInterruzionePercorso1));
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
                             if(count_fuori_ing>=MIN_NODI_FUORI_INGOMBRO) break;
                          }
                       }
                       //-----------------------------------------------------
                       // Se nodo non occupabile causa altro agv considerato
                       // StessaDirezione=FALSE
                       //-----------------------------------------------------
                       if(NodoOccupabile1==0 && AgvPerInterruzionePercorso1==altro_agv){
                          Prosegui=false;
                          break;
                       }
                    }
                 }
              }
              //----------------------------------------------
              // Proseguimento ammesso
              //----------------------------------------------
              if(Prosegui==true){
                 N.nodo_busy[PuntoSucessivo] = num_agv;
                 N.save_nodo_busy(all_mess);
                 //-----------------------------------------------------------------
                 // Lascio una traccia su file di un avvenuta Dead Lock.
                 //-----------------------------------------------------------------
                 sprintf(all_mess, "(1) Dead Lock - (TGV:%d, BLOCCANTE:%d, PROSEGUI SU %d PER FUORI ING)", num_agv, altro_agv, PuntoSucessivo);
                 error_trasmit(num_agv, all_mess);
                 break;
              }
           }
        }
     }
     else{
        for(i=1; i<=MAXAGV; i++){
           if(Tappi[i]==0                          ) break;
           if(Prosegui==true                       ) break;
           if(AGV[Tappi[i]]->mission.SemiAutomatico) continue;
           //--------------------------------------------------------------------------------
           // Cerca punto del percorso all'interno della missione
           // se NON presente --> SKIP
           //--------------------------------------------------------------------------------
           Prosegui=false;
           PuntoSucessivo=0;
           num_agv  =Tappi[i];
           altro_agv=AgvBloccati[Tappi[i]];
           if(altro_agv<1       ) continue;
           if(altro_agv>MAXAGV  ) continue;
           //-------------------------------------------------------------------
           // Se bloccante su PT in in ingombro NO PROSEGUI
           //-------------------------------------------------------------------
           if(AGV[altro_agv]->mission.pstart==0 && AGV[altro_agv]->stato.pos>0 && AGV[altro_agv]->stato.pos<=TOTPUNTI){
              if(N.punti_notevoli[AGV[altro_agv]->stato.pos]==true && N.pt_in_ingombro[AGV[altro_agv]->stato.pos]==true) continue;
           }
           //-------------------------------------------------------------------
           // NAVETTA CONSIDERATA
           //-------------------------------------------------------------------
           dirrot_numagv=0;
           prec_numagv =AGV[num_agv]->stato.pos;
           //-------------------------------------------------------------------
           // gestione eccezione rotazione da fermo
           //-------------------------------------------------------------------
           RotSuccDaFermo=rotp1=rotp2=999;
           if(AGV[num_agv]->stato.s.bit.grd0  ) rotp1=0;
           if(AGV[num_agv]->stato.s.bit.grd90 ) rotp1=1;
           if(AGV[num_agv]->stato.s.bit.grd180) rotp1=2;
           if(AGV[num_agv]->stato.s.bit.grd270) rotp1=3;
           for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
              if(AGV[num_agv]->mission.punto[ptr_percorso]!=AGV[num_agv]->stato.pos     ) continue;
              if(AGV[num_agv]->mission.punto[ptr_percorso]==AGV[num_agv]->mission.pend  ) break;
              if(AGV[num_agv]->mission.punto[ptr_percorso]==0                           ) break;
              PuntoSucessivo = AGV[num_agv]->mission.punto[ptr_percorso+1];
              prec_numagv    = AGV[num_agv]->mission.punto[ptr_percorso];
              dirrot_numagv  = AGV[num_agv]->mission.dati_perc[ptr_percorso].dir;
              //-------------------------------------------------------------------
              // gestione eccezione rotazione da fermo
              //-------------------------------------------------------------------
              rotp2 = AGV[num_agv]->mission.dati_perc[ptr_percorso+1].rot;
              //--------------------------------------------
              // ROTAZIONI DA FERMO
              //--------------------------------------------
              if(rotp1<4 && rotp2<4 && rotp1!=rotp2){
                 if(rotp2==0) RotSuccDaFermo=0;
                 if(rotp2==1) RotSuccDaFermo=90;
                 if(rotp2==2) RotSuccDaFermo=180;
                 if(rotp2==3) RotSuccDaFermo=270;
              }
              break;
           }
           //-------------------------------------------------------------------
           // Verifico se ALTRO AGV in RETTA
           //-------------------------------------------------------------------
           if(PuntoSucessivo!=0){
              if(!AGV[altro_agv]->mission.pstart || AGV[altro_agv]->mission.pintend==AGV[altro_agv]->stato.pos || AGV[altro_agv]->mission.pend==AGV[altro_agv]->stato.pos){
                 Interferenza=false;
                 prec_altroagv=AGV[altro_agv]->stato.pos;
                 punto_altroagv=AGV[altro_agv]->stato.pos;
                 dirrot_altroagv=AGV[altro_agv]->stato.rot_laser;
                 //-------------------------------------------------------------------
                 // Ingombri altro AGV
                 //-------------------------------------------------------------------
                 IngombroAgv2 = new ingombroagv(altro_agv);
                 IngombroAgv2->AzzeraIngombroAgv();
                 IngombroAgv2->SettaIngombroSuTratto(prec_altroagv, punto_altroagv, dirrot_altroagv, all_mess);
                 //-------------------------------------------------------------------
                 // Verifico interferenza navette su ROTAZIONE DA FERMO
                 //-------------------------------------------------------------------
                 if(RotSuccDaFermo!=999){
                    //-------------------------------------------------------------------
                    // Ingombri AGV considerato
                    //-------------------------------------------------------------------
                    IngombroAgv1 = new ingombroagv(num_agv);
                    IngombroAgv1->AzzeraIngombroAgv();
                    IngombroAgv1->SettaIngombroSuTratto(PuntoSucessivo, PuntoSucessivo, RotSuccDaFermo, all_mess);
                    //-------------------------------------------------------------------
                    // Verifico interferenza navette
                    //-------------------------------------------------------------------
                    if(IngombroAgv1->VerificaIntersezioneAgv(IngombroAgv2, all_mess)) Interferenza=true;
                 }
                 //-------------------------------------------------------------------
                 // Verifico interferenza navette su TRATTO
                 //-------------------------------------------------------------------
                 if(Interferenza==false){
                    IngombroAgv1 = new ingombroagv(num_agv);
                    IngombroAgv1->AzzeraIngombroAgv();
                    IngombroAgv1->SettaIngombroSuTratto(prec_numagv, PuntoSucessivo, dirrot_numagv, all_mess);
                    //-------------------------------------------------------------------
                    // Verifico interferenza navette
                    //-------------------------------------------------------------------
                    if(IngombroAgv1->VerificaIntersezioneAgv(IngombroAgv2, all_mess)) Interferenza=true;
                 }
                 //-------------------------------------------------------
                 // NESSUNA INTERFERENZA
                 //-------------------------------------------------------
                 if(Interferenza==false){
                    //----------------------------------------------
                    // Proseguimento ammesso
                    //----------------------------------------------
                    Prosegui=true;
                    N.nodo_busy[PuntoSucessivo] = num_agv;
                    N.save_nodo_busy(all_mess);
                    //-----------------------------------------------------------------
                    // Lascio una traccia su file di un avvenuta Dead Lock.
                    //-----------------------------------------------------------------
                    sprintf(all_mess, "(2) Dead Lock - (TGV:%d, BLOCCANTE:%d, PROSEGUI SU %d PER FUORI ING)", num_agv, altro_agv, PuntoSucessivo);
                    error_trasmit(num_agv, all_mess);
                 }
                 delete IngombroAgv1;
                 delete IngombroAgv2;
                 if(Prosegui==true) break;
              }
           }
        }
     }
  }

  //---------------------------------------------
  // (1) SOLO TGV FERMI SUL PERCORSO A CAUSA
  //     DI BLOCCAGGIO ALTRO TGV
  // Inizializzazione di missioni fasulle per i
  // TGV ritenuti dei TAPPI
  //---------------------------------------------
  Ok=false;
  Trovato=false;
  for(i=1; i<=MAXAGV; i++){
     if(Prosegui==true                       ) break;
     if(Tappi[i]==0                          ) break;
     if(AGV[Tappi[i]]->mission.SemiAutomatico) continue;
     if(AGV[Tappi[i]]->test_mission_on()     ) continue;
     //-----------------------------------------------
     // richiamo funzione
     //-----------------------------------------------
     if(InizializzaMissioneFuoriIngombro(Tappi[i], &Ok, all_mess)){
        error_trasmit(i, all_mess);
     }
     //-----------------------------------------------
     // Se l'inizializzazione è riuscita ---> Skip!!!
     //-----------------------------------------------
     if(Ok==true && AGV[Tappi[i]]->mission.pintend!=AGV[Tappi[i]]->stato.pos){Trovato=true; break;}
  }
  //---------------------------------------------
  // Inizializzazione di missioni fasulle per i
  // TGV ritenuti dei TAPPI
  //---------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(Prosegui==true                       ) break;
     if(Trovato==true                        ) break;
     if(Tappi[i]==0                          ) break;
     if(AGV[Tappi[i]]->mission.SemiAutomatico) continue;
     //-----------------------------------------------
     // Se il TGV è su un ponto di blocco rotazione
     // non va spostato
     //
     // AL-20/12/2019  controllo spostato in
     // InizializzaMissioneFuoriIngombro() dove
     // verifica start senza alcuna rotazione
     //-----------------------------------------------
   //if(N.n[AGV[Tappi[i]]->stato.pos]->blocco_rot==true && N.punti_notevoli[AGV[Tappi[i]]->stato.pos]==false) continue;
     //-----------------------------------------------
     // Cerco di inizializzare una missione fuori
     // ingombro
     //-----------------------------------------------
     if(InizializzaMissioneFuoriIngombro(Tappi[i], &Ok, all_mess)){
        error_trasmit(i, all_mess);
     }
     //-----------------------------------------------
     // Se l'inizializzazione è riuscita ---> Skip!!!
     // o TGV non si è mosso
     //-----------------------------------------------
     if(Ok==true && AGV[Tappi[i]]->mission.pintend!=AGV[Tappi[i]]->stato.pos) break;
  }
  return;
}

// -----------------------------------
//   ImpostazioneCurve()
// -----------------------------------
// Controllo se il Tgv è predisposto per fare delle curve
// e in quel caso modifica il percorso per eseguire la curva
//
int process::ImpostazioneCurve(short int perc[], struct DATI_PERCORSO dati[], short int *new_perc, struct DATI_PERCORSO *new_dati, char *all_mess)
{
  short int i;
  short int NrCurva;
  short int attu;
  short int succ;
  short int prec;
  short int rot_attu;
  short int rot_succ;
  short int dir_prec;
  short int dir_attu;
  int  x0, y0, x1, y1, x2, y2;
  bool r0_r0;
  bool r1_r1;
  bool r2_r2;
  bool r3_r3;
  bool r0_r1;
  bool r0_r3;
  bool r1_r2;
  bool r1_r0;
  bool r2_r3;
  bool r2_r1;
  bool r3_r0;
  bool r3_r2;
  bool ind_av;
  bool av_ind;
  bool NoGestioneCurve;
  bool provenieza_retta;
  int  typ_curva;
  int  Id, Raggio, Circonferenza;
  TPoint points[5];
  
  //--------------------------------------------------
  // Scorro tutta la missione del TGV per individuare
  // delle CURVE.
  //--------------------------------------------------
  for(i=0; i<MAXPERCORSI-1; i++){
     NrCurva=0;
     attu=0;
     succ=0;
     prec=0;
     rot_attu=0;
     rot_succ=0;
     dir_prec=0;
     dir_attu=0;
     Id=Raggio=0;
     Circonferenza=0;
     NoGestioneCurve=false;
     //--------------------------------------------------
     // Recupero numero del nodo e i dati del percorso.
     //--------------------------------------------------
     new_perc[i] = perc[i];
     memcpy(&new_dati[i], &dati[i], sizeof(DATI_PERCORSO));
     //--------------------------------------------------
     // Se sto analizzando l'ultimo nodo della missione
     // --> Skip!!!
     //--------------------------------------------------
     if(perc[i+1]==0) break;
     //--------------------------------------------------
     // Individuo il tipo di curva
     //--------------------------------------------------
     attu = perc[i];
     succ = perc[i+1];
     if(i>0) prec = perc[i-1];
     rot_attu = dati[i].rot;
     dir_attu = dati[i].dir;
     rot_succ = dati[i+1].rot;
     if(i>0){
        dir_prec = dati[i-1].dir;
     }
     //--------------------------------------------------
     // Variabil.h -> Disattivazione curve
     //--------------------------------------------------
     #ifndef GESTIONE_CURVE
        NoGestioneCurve=true;
     #endif;
     //--------------------------------------------------
     // Coordinate dei punti
     //--------------------------------------------------
     x0 = Punti[prec][0];
     y0 = Punti[prec][1];
     x1 = Punti[attu][0];
     y1 = Punti[attu][1];
     x2 = Punti[succ][0];
     y2 = Punti[succ][1];
     //--------------------------------------------------
     // CURVE ARCHI DI CIRCONFERENZA
     // Selezione tipo di curva in base a quadrante e
     // direzione di ingresso/uscita curva
     //--------------------------------------------------
     if(CurveDiBezier==0){
        //--------------------------------------------------
        // Verifico se i nodi appartengono a una curva
        // (se la funzione restituisce "1" significa che
        //  la curva non appartiene alla struttura).
        //--------------------------------------------------
        if(N.CercaCurva(TRUE, attu, succ, &Raggio, &Circonferenza, &Id)) NoGestioneCurve=true;
     }
     //--------------------------------------------------
     // CURVE DI BEZIER
     // Selezione tipo di curva in base a impostazione
     // variabil.h e stato di rotazione di uscita
     //
     // CurveDiBezier = 1 --> TUTTE
     // CurveDiBezier = 2 --> SOLO QUADRATICHE
     // CurveDiBezier = 3 --> SOLO CUBICHE
     //--------------------------------------------------
     else{
        typ_curva = CurveDiBezier;   // la funzione ricerca tra tutti i tipi di curve
        if(N.CercaCurvaBezier(FALSE, attu, succ, &typ_curva, &points[0])) NoGestioneCurve=true;
     }
     //--------------------------------------------------
     // Se è disabilitata la gestione curve -->Skip!
     //--------------------------------------------------
     if(NoGestioneCurve) continue;
     //--------------------------------------------------
     // Se non c'è variavrione di entrambe le coordinate
     // dei punti --> Skip!!!
     //--------------------------------------------------
     if(x1==x2 || y1==y2) continue;
     r0_r0  = false;
     r1_r1  = false;
     r2_r2  = false;
     r3_r3  = false;
     r0_r1  = false;
     r0_r3  = false;
     r1_r2  = false;
     r1_r0  = false;
     r2_r3  = false;
     r2_r1  = false;
     r3_r0  = false;
     r3_r2  = false;
     ind_av = false;
     av_ind = false;
     //-------------------------------------------------------------
     //                     av0,ind2
     //                        |
     //                        |
     //                  Q4    |    Q3
     //                        |
     //        av1  ___________|___________ av3
     //        ind3            |            ind1
     //                        |
     //                  Q1    |    Q2
     //                        |
     //                        |
     //                     av2,ind0
     //
     // Individuo il tipo esatto di curva e lo associo
     // ai dati di rotazione:
     //
     // Individuo il tipo esatto di curva e lo associo
     // ai dati di rotazione:
     //
     // CURVE SU ANGOLI DI 90°
     //
     // Curva:  4 --> Rotazione anti-oraria 180°-270° (uscita AV )
     //               Rotazione anti-oraria   0°- 90° (uscita IND)
     // Curva:  5 --> Rotazione oraria      270°-180° (uscita AV )
     //               Rotazione oraria       90°-  0° (uscita IND)
     // Curva:  6 --> Rotazione anti-oraria 270°-  0° (uscita AV )
     //               Rotazione anti-oraria  90°-180° (uscita IND)
     // Curva:  7 --> Rotazione oraria        0°-270° (uscita AV )
     //               Rotazione oraria      180°- 90° (uscita IND)
     // Curva:  8 --> Rotazione anti-oraria  90°-180° (uscita AV )
     //               Rotazione anti-oraria 270°-  0° (uscita IND)
     // Curva:  9 --> Rotazione oraria      180°- 90° (uscita AV )
     //               Rotazione oraria        0°-270° (uscita IND)
     // Curva: 10 --> Rotazione anti-oraria   0°- 90° (uscita AV )
     //               Rotazione anti-oraria 180°-270° (uscita IND)
     // Curva: 11 --> Rotazione oraria       90°-  0° (uscita AV )
     //               Rotazione oraria      270°-180° (uscita IND)
     //
     // (23/09/2010) - CURVE SU ANGOLI DIVERSI DA 90° O CURVE CONSECUTIVE
     //
     // Curva: 12 --> Rotazione anti-oraria 180°-180° (uscita AV )
     //               Rotazione anti-oraria   0°-  0° (uscita IND)
     // Curva: 13 --> Rotazione oraria      270°-270° (uscita AV )
     //               Rotazione oraria       90°- 90° (uscita IND)
     // Curva: 14 --> Rotazione anti-oraria 270°-270° (uscita AV )
     //               Rotazione anti-oraria  90°- 90° (uscita IND)
     // Curva: 15 --> Rotazione oraria        0°-  0° (uscita AV )
     //               Rotazione oraria      180°-180° (uscita IND)
     // Curva: 16 --> Rotazione anti-oraria  90°- 90° (uscita AV )
     //               Rotazione anti-oraria 270°-270° (uscita IND)
     // Curva: 17 --> Rotazione oraria      180°-180° (uscita AV )
     //               Rotazione oraria        0°-  0° (uscita IND)
     // Curva: 18 --> Rotazione anti-oraria   0°-  0° (uscita AV )
     //               Rotazione anti-oraria 180°-180° (uscita IND)
     // Curva: 19 --> Rotazione oraria       90°- 90° (uscita AV )
     //               Rotazione oraria      270°-270° (uscita IND)
     //
     //-------------------------------------------------------------
     if(rot_attu==0 && rot_succ==0) r0_r0 = true;
     if(rot_attu==1 && rot_succ==1) r1_r1 = true;
     if(rot_attu==2 && rot_succ==2) r2_r2 = true;
     if(rot_attu==3 && rot_succ==3) r3_r3 = true;
     if(rot_attu==0 && rot_succ==1) r0_r1 = true;
     if(rot_attu==0 && rot_succ==3) r0_r3 = true;
     if(rot_attu==1 && rot_succ==2) r1_r2 = true;
     if(rot_attu==1 && rot_succ==0) r1_r0 = true;
     if(rot_attu==2 && rot_succ==3) r2_r3 = true;
     if(rot_attu==2 && rot_succ==1) r2_r1 = true;
     if(rot_attu==3 && rot_succ==0) r3_r0 = true;
     if(rot_attu==3 && rot_succ==2) r3_r2 = true;
     //--------------------------------------------------
     // Cambio di rotazione tra nodo precedente e nodo
     // attuale.
     //--------------------------------------------------
     if(i>0 && dir_attu==0 && dir_prec!=dir_attu) ind_av=true;
     if(i>0 && dir_attu==1 && dir_prec!=dir_attu) av_ind=true;
     //--------------------------------------------------
     // Verifico la curva è relativa a un anglo di 90°
     //--------------------------------------------------
     if(x1<x2 && y1<y2){
      //AL-29/09/13 INVERTITE CONTROLLI CAMBIO DIREZIONE
      //(probabilmente mai dato errore prima perchè sono molto
      // rare le inversioni di direzione)
      //if(r2_r3 || r0_r1 || av_ind) NrCurva=4;
      //if(r3_r2 || r1_r0 || ind_av) NrCurva=5;
        if(r2_r3 || r0_r1) NrCurva=4;
        if(r3_r2 || r1_r0) NrCurva=5;
        if((NrCurva!=4 && NrCurva!=5) && av_ind) NrCurva=4;
        if((NrCurva!=4 && NrCurva!=5) && ind_av) NrCurva=5;
     }
     if(x1<x2 && y1>y2){
      //AL-29/09/13 INVERTITE CONTROLLI CAMBIO DIREZIONE
      //(probabilmente mai dato errore prima perchè sono molto
      // rare le inversioni di direzione)
      //if(r3_r0 || r1_r2 || av_ind) NrCurva=6;
      //if(r0_r3 || r2_r1 || ind_av) NrCurva=7;
        if(r3_r0 || r1_r2) NrCurva=6;
        if(r0_r3 || r2_r1) NrCurva=7;
        if((NrCurva!=6 && NrCurva!=7) && av_ind) NrCurva=6;
        if((NrCurva!=6 && NrCurva!=7) && ind_av) NrCurva=7;
     }
     if(x1>x2 && y1<y2){
      //AL-29/09/13 INVERTITE CONTROLLI CAMBIO DIREZIONE
      //(probabilmente mai dato errore prima perchè sono molto
      // rare le inversioni di direzione)
      //if(r1_r2 || r3_r0 || av_ind) NrCurva=8;
      //if(r2_r1 || r0_r3 || ind_av) NrCurva=9;
        if(r1_r2 || r3_r0) NrCurva=8;
        if(r2_r1 || r0_r3) NrCurva=9;
        if((NrCurva!=8 && NrCurva!=9) && av_ind) NrCurva=8;
        if((NrCurva!=8 && NrCurva!=9) && ind_av) NrCurva=9;
     }
     if(x1>x2 && y1>y2){
      //AL-29/09/13 INVERTITE CONTROLLI CAMBIO DIREZIONE
      //(probabilmente mai dato errore prima perchè sono molto
      // rare le inversioni di direzione)
      //if(r0_r1 || r2_r3 || av_ind) NrCurva=10;
      //if(r1_r0 || r3_r2 || ind_av) NrCurva=11;
        if(r0_r1 || r2_r3) NrCurva=10;
        if(r1_r0 || r3_r2) NrCurva=11;
        if((NrCurva!=10 && NrCurva!=11) && av_ind) NrCurva=10;
        if((NrCurva!=10 && NrCurva!=11) && ind_av) NrCurva=11;
     }
     //--------------------------------------------------
     // 23/09/2010
     // Verifico se si tratta di una curva relativa a un
     // angolo diverso di 90°
     //--------------------------------------------------
     if(!NrCurva){
        //----------------------------------------
        // TGV PROVIENE DA UN TRATTO INCLINATO
        // O DU UN ALTRA CURVA SENZA PASSAGGIO
        // DA TRACCIA RETTA
        //----------------------------------------
        provenieza_retta=true;
        if(prec!=0 && x0!=x1 && y0!=y1) provenieza_retta=false;
        //----------------------------------------
        // provenienza tratto rettilineo
        //----------------------------------------
        if(x1<x2 && y1<y2 && provenieza_retta==true){
           if(r2_r2 || r0_r0) NrCurva=12;
           if(r3_r3 || r1_r1) NrCurva=13;
        }
        if(x1<x2 && y1>y2 && provenieza_retta==true){
           if(r3_r3 || r1_r1) NrCurva=14;
           if(r0_r0 || r2_r2) NrCurva=15;
        }
        if(x1>x2 && y1<y2 && provenieza_retta==true){
           if(r1_r1 || r3_r3) NrCurva=16;
           if(r2_r2 || r0_r0) NrCurva=17;
        }
        if(x1>x2 && y1>y2 && provenieza_retta==true){
           if(r0_r0 || r2_r2) NrCurva=18;
           if(r1_r1 || r3_r3) NrCurva=19;
        }
        //----------------------------------------
        // provenienza tratto obliquo o curve
        // consecutive
        //----------------------------------------
        if(x1<x2 && y1<y2 && provenieza_retta==false){
           if(r2_r2 || r0_r0) NrCurva=17;
           if(r3_r3 || r1_r1) NrCurva=16;
        }
        if(x1<x2 && y1>y2 && provenieza_retta==false){
           if(r3_r3 || r1_r1) NrCurva=19;
           if(r0_r0 || r2_r2) NrCurva=18;
        }
        if(x1>x2 && y1<y2 && provenieza_retta==false){
           if(r1_r1 || r3_r3) NrCurva=12;
           if(r2_r2 || r0_r0) NrCurva=17;
        }
        if(x1>x2 && y1>y2 && provenieza_retta==false){
           if(r0_r0 || r2_r2) NrCurva=15;
           if(r1_r1 || r3_r3) NrCurva=14;
        }
     }
     //--------------------------------------------------
     // Il primo tratto di una missione non può essere
     // una curva per problemi di gestione del TGV
     //
     // SOLO PER NAVETTE CON VECCHIA GESTIONE CURVE
     //
     // 04/12/2013 SIMONE G. HA DETTO CHE E' POSSIBILE
     // RIPARTIRE CON PRIMA ISTRUZIONE ROT SU TUTTI I
     // TIPI DI CURVA
     //--------------------------------------------------
   //if(i==0 && NrCurva && CurveDiBezier==0){
   //   sprintf(all_mess, "curva da %d a %d non ammessa come primo tratto di missione.", attu, succ);
   //   return 1;
   //}
     //--------------------------------------------------
     // ERRORE COMPILAZIONE NODI
     // Il tratto è una curva ma non è stata individuata
     // ma la combinazione delle coordinate nodi abbinata
     // alle direzioni e rotazioni dei TGV non la rende
     // percorribile
     //--------------------------------------------------
     if(!NrCurva){
        sprintf(all_mess, "curva da %d a %d non percorribile (errore rot. di in/out).", attu, succ);
        return 1;
     }
     //--------------------------------------------------
     // CURVE DI BEZIER
     // Il numero di curva da passare alla struttura
     // missione è dato da una combinazione del tipo
     // di curva e della rotazione di uscita
     //--------------------------------------------------
     if(CurveDiBezier!=0){
        //-------------------------------------------
        // Selezione curve in base al grado e alla
        // rotazione di uscita
        // Ammesse solo QUADRATICHE e CUBICHE
        //-------------------------------------------
        if(typ_curva==2 || typ_curva==3) NrCurva = (short int)((typ_curva*10)+rot_succ);
     }
     //--------------------------------------------------
     // Imposto il Tipo di curva nel campo di rotazione
     //--------------------------------------------------
     new_dati[i].rot=NrCurva;
  }
  return 0;
}

// -----------------------------------
//   ImpostazioneDeviazioni()
// -----------------------------------
// Controllo se il Tgv è predisposto per fare delle curve
// e in quel caso modifica il percorso per eseguire la curva
//
int process::ImpostazioneDeviazioni(short int num_agv, short int perc[], struct DATI_PERCORSO dati[], short int *new_perc, struct DATI_PERCORSO *new_dati, char *all_mess)
{
  short int i, j, k;
  short int attu;
  short int succ;
  short int prec;
  short int rot_attu;
  short int rot_prec;
  short int dir_attu;
  short int dir_prec;
  short int rot_standard;
  short int caso;
  short int stato_rot, stato_grd;
  short int tipo_rotazione;
  int   x0, y0, x1, y1, x2, y2;
  double A, B;
  double CatetoX_A, CatetoY_A;
  double CatetoX_B, CatetoY_B;
  long double grd;
  long double radA, grdA;
  long double radB, grdB;
  bool rotazione_90;
  bool provenienza_retta;
  bool anti_oraria;
  bool tgv_ortogonale;
  bool NoGestioneCurve;
  bool ing_tratto_rettoX;
  bool ing_tratto_rettoY;
  bool out_tratto_rettoX;
  bool out_tratto_rettoY;
#ifdef GESTIONE_CURVE
  int  typ_curva;
  int  RaggioDwg, ArcoDwg, IdCurva;
  TPoint points[5];
#endif;

  //--------------------------------------------------
  // Nodo iniziale invariato
  //--------------------------------------------------
//
/* DA SCOMMENTARE PER TOGLIERE DEVIAZIONE SU PRIMO NODO DI PERCORSO
//
  new_perc[0] = perc[0];
  memcpy(&new_dati[0], &dati[0], sizeof(DATI_PERCORSO));
  //--------------------------------------------------
  // Scorro tutta la missione del TGV per individuare
  // delle CURVE.
  //--------------------------------------------------
  for(i=1; i<MAXPERCORSI-1; i++){
*/
  for(i=0; i<MAXPERCORSI-1; i++){
     attu=0;
     prec=0;
     succ=0;
     rot_attu=0;
     rot_prec=0;
     NoGestioneCurve=false;
     //--------------------------------------------------
     // Recupero numero del nodo e i dati del percorso.
     //--------------------------------------------------
     new_perc[i] = perc[i];
     memcpy(&new_dati[i], &dati[i], sizeof(DATI_PERCORSO));
     //--------------------------------------------------
     // Se sto analizzando l'ultimo nodo della missione
     // --> Skip!!!
     //--------------------------------------------------
     if(perc[i+1]==0) break;
     //--------------------------------------------------
     // Individuo il tipo di curva
     //--------------------------------------------------
     attu = perc[i];
     succ = perc[i+1];
     dir_attu = dati[i].dir;
     rot_attu = dati[i].rot;
     //---------------------------------------
     // Normalizzazione della rotazione in
     // caso di CURVA precedente
     //---------------------------------------
     switch(rot_attu){
        case  0: rot_standard = 0;
                 break;
        case  1: rot_standard = 1;
                 break;
        case  2: rot_standard = 2;
                 break;
        case  3: rot_standard = 3;
                 break;
        case  4: if(!dir_attu) rot_standard=3;
                 if(dir_attu ) rot_standard=1;
                 break;
        case  5: if(!dir_attu) rot_standard=2;
                 if(dir_attu ) rot_standard=0;
                 break;
        case  6: if(!dir_attu) rot_standard=0;
                 if(dir_attu ) rot_standard=2;
                 break;
        case  7: if(!dir_attu) rot_standard=3;
                 if(dir_attu ) rot_standard=1;
                 break;
        case  8: if(!dir_attu) rot_standard=2;
                 if(dir_attu ) rot_standard=0;
                 break;
        case  9: if(!dir_attu) rot_standard=1;
                 if(dir_attu ) rot_standard=3;
                 break;
        case 10: if(!dir_attu) rot_standard=1;
                 if(dir_attu ) rot_standard=3;
                 break;
        case 11: if(!dir_attu) rot_standard=0;
                 if(dir_attu ) rot_standard=2;
                 break;
        case 12: if(!dir_attu) rot_standard=2;
                 if(dir_attu ) rot_standard=0;
                 break;
        case 13: if(!dir_attu) rot_standard=3;
                 if(dir_attu ) rot_standard=1;
                 break;
        case 14: if(!dir_attu) rot_standard=3;
                 if(dir_attu ) rot_standard=1;
                 break;
        case 15: if(!dir_attu) rot_standard=0;
                 if(dir_attu ) rot_standard=2;
                 break;
        case 16: if(!dir_attu) rot_standard=1;
                 if(dir_attu ) rot_standard=3;
                 break;
        case 17: if(!dir_attu) rot_standard=2;
                 if(dir_attu ) rot_standard=0;
                 break;
        case 18: if(!dir_attu) rot_standard=0;
                 if(dir_attu ) rot_standard=2;
                 break;
        case 19: if(!dir_attu) rot_standard=1;
                 if(dir_attu ) rot_standard=3;
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
        default: break;
     }
     rot_attu = rot_standard;
     //--------------------------------------------------
     // Recupero dati nodo precedente
     //--------------------------------------------------
     if(i>0){
        prec     = perc[i-1];
        dir_prec = dati[i-1].dir;
        rot_prec = dati[i-1].rot;
        //---------------------------------------
        // Normalizzazione della rotazione in
        // caso di CURVA precedente
        //---------------------------------------
        switch(rot_prec){
           case  0: rot_standard = 0;
                    break;
           case  1: rot_standard = 1;
                    break;
           case  2: rot_standard = 2;
                    break;
           case  3: rot_standard = 3;
                    break;
           case  4: if(!dir_prec) rot_standard=3;
                    if(dir_prec ) rot_standard=1;
                    break;
           case  5: if(!dir_prec) rot_standard=2;
                    if(dir_prec ) rot_standard=0;
                    break;
           case  6: if(!dir_prec) rot_standard=0;
                    if(dir_prec ) rot_standard=2;
                    break;
           case  7: if(!dir_prec) rot_standard=3;
                    if(dir_prec ) rot_standard=1;
                    break;
           case  8: if(!dir_prec) rot_standard=2;
                    if(dir_prec ) rot_standard=0;
                    break;
           case  9: if(!dir_prec) rot_standard=1;
                    if(dir_prec ) rot_standard=3;
                    break;
           case 10: if(!dir_prec) rot_standard=1;
                    if(dir_prec ) rot_standard=3;
                    break;
           case 11: if(!dir_prec) rot_standard=0;
                    if(dir_prec ) rot_standard=2;
                    break;
           case 12: if(!dir_prec) rot_standard=2;
                    if(dir_prec ) rot_standard=0;
                    break;
           case 13: if(!dir_prec) rot_standard=3;
                    if(dir_prec ) rot_standard=1;
                    break;
           case 14: if(!dir_prec) rot_standard=3;
                    if(dir_prec ) rot_standard=1;
                    break;
           case 15: if(!dir_prec) rot_standard=0;
                    if(dir_prec ) rot_standard=2;
                    break;
           case 16: if(!dir_prec) rot_standard=1;
                    if(dir_prec ) rot_standard=3;
                    break;
           case 17: if(!dir_prec) rot_standard=2;
                    if(dir_prec ) rot_standard=0;
                    break;
           case 18: if(!dir_prec) rot_standard=0;
                    if(dir_prec ) rot_standard=2;
                    break;
           case 19: if(!dir_prec) rot_standard=1;
                    if(dir_prec ) rot_standard=3;
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
           default: break;
        }
        rot_prec = rot_standard;
     }
     //--------------------------------------------------
     // DEVIAZIONE SU PRIMO NODO DI PERCORSO
     // In caso di partenza da punti di percorso verifico
     // la rotazione attuale del TGV
     //--------------------------------------------------
//
// inzio commento per togliere deviazione su primo nodo di percorso
//
     if(i==0 && N.punti_notevoli[attu]==0){
        //-----------------------------------------------
        // Recupero lo stato di rotazione dallo stato
        // navetta
        //-----------------------------------------------
        if(AGV[num_agv]->stato.s.bit.grd0  ){rot_prec=0; stato_rot=  0;}
        if(AGV[num_agv]->stato.s.bit.grd90 ){rot_prec=1; stato_rot= 90;}
        if(AGV[num_agv]->stato.s.bit.grd180){rot_prec=2; stato_rot=180;}
        if(AGV[num_agv]->stato.s.bit.grd270){rot_prec=3; stato_rot=270;}
        if(AGV[num_agv]->stato.s.bit.ind   ) dir_prec=1;
        if(AGV[num_agv]->stato.s.bit.av    ) dir_prec=0;
        //-----------------------------------------------
        // Recupero gradi di rotazione
        //-----------------------------------------------
        stato_grd = AGV[num_agv]->stato.rot_calamite;
        if(AGV[num_agv]->mission.GestioneLaser==true) stato_grd = AGV[num_agv]->stato.rot_laser;
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
                 if(N.n[j]->rec[k].ps!=attu                 ) continue;
                 if(N.n[j]->rec[k].rot!=rot_prec            ) continue;
                 if(dir_prec==1 && N.n[j]->rec[k].ind==false) continue;
                 if(dir_prec==0 && N.n[j]->rec[k].ind==true ) continue;
                 //----------------------------------------
                 // Ricerca preferenziale di precedenti su
                 // tratti ortogonali
                 //----------------------------------------
                 x0 = Punti[N.n[j]->num][0];
                 y0 = Punti[N.n[j]->num][1];
                 x1 = Punti[attu][0];
                 y1 = Punti[attu][1];
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
              if(N.n[j]->rec[k].ps!=attu                 ) continue;
              if(N.n[j]->rec[k].rot!=rot_prec            ) continue;
              if(dir_prec==1 && N.n[j]->rec[k].ind==false) continue;
              if(dir_prec==0 && N.n[j]->rec[k].ind==true ) continue;
              //----------------------------------------
              // Ricerca preferenziale di precedenti su
              // tratti ortogonali
              //----------------------------------------
              x0 = Punti[N.n[j]->num][0];
              y0 = Punti[N.n[j]->num][1];
              x1 = Punti[attu][0];
              y1 = Punti[attu][1];
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
              if(N.n[j]->rec[k].ps==0                    ) break;
              if(N.n[j]->rec[k].ps!=attu                 ) continue;
              if(N.n[j]->rec[k].rot!=rot_prec            ) continue;
              if(dir_prec==1 && N.n[j]->rec[k].ind==false) continue;
              if(dir_prec==0 && N.n[j]->rec[k].ind==true ) continue;
              prec = N.n[j]->num;
              break;
           }
           if(prec!=0) break;
        }
        if(prec==0) continue;
        //--------------------------------------------------
        // Coordinate dei punti
        //--------------------------------------------------
        x0 = Punti[prec][0];
        y0 = Punti[prec][1];
        x1 = Punti[attu][0];
        y1 = Punti[attu][1];
        x2 = Punti[succ][0];
        y2 = Punti[succ][1];
        //--------------------------------------------------
        // Se non c'è variazione di entrambe le coordinate
        // dei punti --> Skip!!!
        //--------------------------------------------------
        if((x1==x2 || y1==y2) && (x0==x1 || y0==y1)) continue;
        //--------------------------------------------------
        // Inizializzazione variabili
        //--------------------------------------------------
        ing_tratto_rettoX=false;
        ing_tratto_rettoY=false;
        out_tratto_rettoX=false;
        out_tratto_rettoY=false;
        //--------------------------------------------------
        // Verifico se mi trovo in partenza da un nodo
        // di uscita o ingresso deviazione
        //--------------------------------------------------
        CatetoX_A = (double)(x1-x0);
        CatetoY_A = (double)(y1-y0);
        if(CatetoX_A==0) out_tratto_rettoY=true;
        if(CatetoY_A==0) out_tratto_rettoX=true;
        CatetoX_B = (double)(x2-x1);
        CatetoY_B = (double)(y2-y1);
        if(CatetoX_B==0) ing_tratto_rettoY=true;
        if(CatetoY_B==0) ing_tratto_rettoX=true;
        //-----------------------------------------------
        // START DEVIAZIONE
        // vengono passati i gradi solo se il TGV ha
        // ancora una posizione ortogonale
        //-----------------------------------------------
      //if((ing_tratto_rettoY==false && ing_tratto_rettoX==false) && stato_grd!=stato_rot) continue;
        if((ing_tratto_rettoY==false && ing_tratto_rettoX==false) && abs(stato_grd-stato_rot)>GrdMinPerAttivaDeviazioni) continue;

        //-----------------------------------------------
        // END DEVIAZIONE
        // vengono passati i gradi solo se il TGV è
        // ancora inclinato con gradi non ortogonali
        //-----------------------------------------------
      //if((out_tratto_rettoY==false && out_tratto_rettoX==false) && stato_grd==stato_rot) continue;
        if((out_tratto_rettoY==false && out_tratto_rettoX==false) && abs(stato_grd-stato_rot)<=GrdMinPerAttivaDeviazioni) continue;
     }
     //--------------------------------------------------
     // Nodo di partenza = PT --> skip!
     //--------------------------------------------------
     if(i==0 && N.punti_notevoli[attu]!=0) continue;
//
// fine commento per togliere deviazione su primo nodo di percorso
//
     //--------------------------------------------------
     // IMPORTANTE PER GLI IMPIANTI FOOD
     // Verifico che i nodi appartengano allo stesso
     // piano
     //--------------------------------------------------
     if(N.n[prec]->piano!=0 && N.n[attu]->piano!=0 && N.n[succ]->piano!=0){
        if(N.n[prec]->piano!=N.n[attu]->piano || N.n[attu]->piano!=N.n[succ]->piano) continue;
     }
     //--------------------------------------------------
     // Variabil.h -> Disattivazione curve
     //--------------------------------------------------
     #ifndef GESTIONE_CURVE
        NoGestioneCurve=true;
     #else
        //--------------------------------------------------
        // GESTIONE CURVE
        // Verifico se i nodi appartengono a una curva
        // (se la funzione restituisce "1" significa che
        //  la curva non appartiene alla struttura).
        //
        // N.B. devo essere sicuro di non essere nè a inizio
        //      nè a fine curva
        //--------------------------------------------------
        // ARCHI DI CIRCONFERENZA
        if(CurveDiBezier==0){
           ArcoDwg=0;
           RaggioDwg=0;
           if(N.CercaCurva(TRUE, attu, succ, &RaggioDwg, &ArcoDwg, &IdCurva)) NoGestioneCurve=true;
           if(NoGestioneCurve==true && prec!=0){
              if(!N.CercaCurva(TRUE, prec, attu, &RaggioDwg, &ArcoDwg, &IdCurva)) NoGestioneCurve=false;
           }
        }
        // CURVE DI BEZIER
        else{
           NoGestioneCurve=true;
           typ_curva = CurveDiBezier;   // la funzione ricerca tra tutti i tipi di curve
           if(!N.CercaCurvaBezier(FALSE, attu, succ, &typ_curva, &points[0])) NoGestioneCurve=false;
           if(NoGestioneCurve==true && prec!=0){
              if(!N.CercaCurvaBezier(FALSE, prec, attu, &typ_curva, &points[0])) NoGestioneCurve=false;
           }              
        }
     #endif;
     //--------------------------------------------------
     // Se è abilitata la gestione curve -->Skip!
     //--------------------------------------------------
     if(NoGestioneCurve==false) continue;
     //--------------------------------------------------
     // Se non ho uno dei tre nodi per il calcolo delle
     // deviazioni --> skip!
     //--------------------------------------------------
     if(prec==0 || attu==0 || succ==0) continue;
     //--------------------------------------------------
     // Coordinate dei punti
     //--------------------------------------------------
     x0 = Punti[prec][0];
     y0 = Punti[prec][1];
     x1 = Punti[attu][0];
     y1 = Punti[attu][1];
     x2 = Punti[succ][0];
     y2 = Punti[succ][1];
     //--------------------------------------------------
     // Se non c'è variazione di entrambe le coordinate
     // dei punti --> Skip!!!
     //--------------------------------------------------
     if((x1==x2 || y1==y2) && (x0==x1 || y0==y1)) continue;
     //--------------------------------------------------
     // Inizializzazione variabili
     //--------------------------------------------------
     A=B=0;
     grdA=grdB=radA=radB=0;
     ing_tratto_rettoX=false;
     ing_tratto_rettoY=false;
     out_tratto_rettoX=false;
     out_tratto_rettoY=false;
     //--------------------------------------------------
     // Anche in caso di variazione di coordinate
     // vengono considerati rettilinei i tratti che
     // hanno lo stesso andamento con insignificanti
     // variazioni di gradi di inclinazione
     //--------------------------------------------------
     CatetoX_A = (double)(x1-x0);
     CatetoY_A = (double)(y1-y0);
     if(CatetoX_A==0) out_tratto_rettoY=true;
     if(CatetoY_A==0) out_tratto_rettoX=true;
     if(CatetoX_A && CatetoY_A){
        radA = (long double)(atanl( CatetoY_A / CatetoX_A ));
        grdA = RadToDeg(radA); A = (double)(grdA);
     }
     CatetoX_B = (double)(x2-x1);
     CatetoY_B = (double)(y2-y1);
     if(CatetoX_B==0) ing_tratto_rettoY=true;
     if(CatetoY_B==0) ing_tratto_rettoX=true;
     if(CatetoX_B && CatetoY_B){
        radB = (long double)(atanl( CatetoY_B / CatetoX_B ));
        grdB = RadToDeg(radB); B = (double)(grdB);
     }
     //-------------------------------------------------------------
     // Skip su tratti inclinati con stesso andamento e piccole
     // varizioni di gradi di inclinazione
     //-------------------------------------------------------------
     if(x0>x1 && y0<y1 && x1>x2 && y1<y2 && abs(A-B)<OFFSET_MIN_COORD_TRATTO_INCLINATO) continue;
     if(x0>x1 && y0>y1 && x1>x2 && y1>y2 && abs(A-B)<OFFSET_MIN_COORD_TRATTO_INCLINATO) continue;
     if(x0<x1 && y0<y1 && x1<x2 && y1<y2 && abs(A-B)<OFFSET_MIN_COORD_TRATTO_INCLINATO) continue;
     if(x0<x1 && y0>y1 && x1<x2 && y1>y2 && abs(A-B)<OFFSET_MIN_COORD_TRATTO_INCLINATO) continue;
     //-------------------------------------------------------------
     // Indipendentemente dalle coordinate dei punti, se non ci sono
     // variazioni di gradi, non sono tratti inclinati
     //-------------------------------------------------------------
     if(abs(A-B)==0) continue;
     //-------------------------------------------------------------
     // Controllo variazioni di rotazione tra nodo precedente e
     // nodo attuale
     // ROTAZIONE: 0 ==> nessuna
     // ROTAZIONE: 1 ==> 90°
     // ROTAZIONE: 2 ==> 180°
     //-------------------------------------------------------------
     rotazione_90=true;
     if(rot_prec==0 && rot_attu==0) rotazione_90=false;
     if(rot_prec==1 && rot_attu==1) rotazione_90=false;
     if(rot_prec==2 && rot_attu==2) rotazione_90=false;
     if(rot_prec==3 && rot_attu==3) rotazione_90=false;
     if(rot_prec==0 && rot_attu==2) rotazione_90=false;
     if(rot_prec==1 && rot_attu==3) rotazione_90=false;
     if(rot_prec==2 && rot_attu==0) rotazione_90=false;
     if(rot_prec==3 && rot_attu==1) rotazione_90=false;
     //-------------------------------------------------------------
     // Determino se la navetta proviene da una traccia retta
     //-------------------------------------------------------------
     provenienza_retta=true;
     if(x0!=x1 && y0!=y1) provenienza_retta=false;
     //-------------------------------------------------------------
     // provenienza tratto rettilineo (cambia solo una coordinata)
     //-------------------------------------------------------------
     if(provenienza_retta==true  && rotazione_90==true) new_dati[i].rot = (short int)(new_dati[i].rot+50);
     //-------------------------------------------------------------
     // provenienza tratto inclinato  (con rotazione di 90°)
     //-------------------------------------------------------------
     if(provenienza_retta==false && rotazione_90==true) new_dati[i].rot = (short int)(new_dati[i].rot+54);
     //-------------------------------------------------------------
     // Se non ho una rotazione a 90° dichiarata da editor nodi
     // determino se devo compiere una rotazione a ORARIA o
     // ANTI-ORARIA in base alle coordinate di ingresso e uscita.
     //-------------------------------------------------------------
     if(rotazione_90==false){
        caso=0;
        tipo_rotazione=0;  // 0 - nessuna
                           // 1 - oraria
                           // 2 - antioraria
        // Combinazioni "A"
        if(x0>x1  && y0<y1  && x1>=x2 && y1<=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A>B ){caso= 1; tipo_rotazione=2;}   //*
        if(x0>x1  && y0<y1  && x1>=x2 && y1<=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A>B ){caso= 2; tipo_rotazione=1;}   //*
        if(x0>x1  && y0<y1  && x1>=x2 && y1<=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A<B ){caso= 1; tipo_rotazione=1;}   //*
        if(x0>x1  && y0<y1  && x1>=x2 && y1<=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A<B ){caso= 2; tipo_rotazione=2;}   //*
        if(x0==x1 && y0<y1  && x1>=x2 && y1<=y2 && dir_prec==dir_attu && A!=B                ){caso= 1; tipo_rotazione=1;}   //*
        if(x0==x1 && y0<y1  && x1>=x2 && y1<=y2 && dir_prec!=dir_attu && A!=B                ){caso= 2; tipo_rotazione=2;}   //*
        if(x0>x1  && y0==y1 && x1>=x2 && y1<=y2 && dir_prec==dir_attu && A!=B                ){caso= 1; tipo_rotazione=2;}   //*
        if(x0>x1  && y0==y1 && x1>=x2 && y1<=y2 && dir_prec!=dir_attu && A!=B                ){caso= 2; tipo_rotazione=1;}   //*

        if(x0>x1  && y0<y1  && x1>=x2 && y1>=y2 && dir_prec==dir_attu && A!=B                ){caso= 3; tipo_rotazione=1;}
        if(x0>x1  && y0<y1  && x1>=x2 && y1>=y2 && dir_prec!=dir_attu && A!=B                ){caso= 4; tipo_rotazione=2;}

        if(x0>x1  && y0<y1  && x1<=x2 && y1<=y2 && dir_prec==dir_attu && A!=B                ){caso= 5; tipo_rotazione=2;}
        if(x0>x1  && y0<y1  && x1<=x2 && y1<=y2 && dir_prec!=dir_attu && A!=B                ){caso= 6; tipo_rotazione=1;}

        if(x0>x1  && y0<y1  && x1<=x2 && y1>=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A>B ){caso= 7; tipo_rotazione=1;}   //*
        if(x0>x1  && y0<y1  && x1<=x2 && y1>=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A>B ){caso= 8; tipo_rotazione=2;}   //*
        if(x0>x1  && y0<y1  && x1<=x2 && y1>=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A<B ){caso= 7; tipo_rotazione=2;}   //*
        if(x0>x1  && y0<y1  && x1<=x2 && y1>=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A<B ){caso= 8; tipo_rotazione=1;}   //*
        if(x0==x1 && y0<y1  && x1<=x2 && y1>=y2 && dir_prec==dir_attu && A!=B                ){caso= 7; tipo_rotazione=2;}   //*
        if(x0==x1 && y0<y1  && x1<=x2 && y1>=y2 && dir_prec!=dir_attu && A!=B                ){caso= 8; tipo_rotazione=1;}   //*
        if(x0>x1  && y0==y1 && x1<=x2 && y1>=y2 && dir_prec==dir_attu && A!=B                ){caso= 7; tipo_rotazione=1;}   //*
        if(x0>x1  && y0==y1 && x1<=x2 && y1>=y2 && dir_prec!=dir_attu && A!=B                ){caso= 8; tipo_rotazione=2;}   //*
        // Combinazioni "B"
        if(x0>x1  && y0>y1  && x1>=x2 && y1<=y2 && dir_prec==dir_attu && A!=B                ){caso= 9; tipo_rotazione=2;}
        if(x0>x1  && y0>y1  && x1>=x2 && y1<=y2 && dir_prec!=dir_attu && A!=B                ){caso=10; tipo_rotazione=1;}

        if(x0>x1  && y0>y1  && x1>=x2 && y1>=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A>B ){caso=11; tipo_rotazione=2;}   //*
        if(x0>x1  && y0>y1  && x1>=x2 && y1>=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A>B ){caso=12; tipo_rotazione=1;}   //*
        if(x0>x1  && y0>y1  && x1>=x2 && y1>=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A<B ){caso=11; tipo_rotazione=1;}   //*
        if(x0>x1  && y0>y1  && x1>=x2 && y1>=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A<B ){caso=12; tipo_rotazione=2;}   //*
        if(x0==x1 && y0>y1  && x1>=x2 && y1>=y2 && dir_prec==dir_attu && A!=B                ){caso=11; tipo_rotazione=2;}   //*
        if(x0==x1 && y0>y1  && x1>=x2 && y1>=y2 && dir_prec!=dir_attu && A!=B                ){caso=12; tipo_rotazione=1;}   //*
        if(x0>x1  && y0==y1 && x1>=x2 && y1>=y2 && dir_prec==dir_attu && A!=B                ){caso=11; tipo_rotazione=1;}   //*
        if(x0>x1  && y0==y1 && x1>=x2 && y1>=y2 && dir_prec!=dir_attu && A!=B                ){caso=12; tipo_rotazione=2;}   //*

        if(x0>x1  && y0>y1  && x1<=x2 && y1<=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A>B ){caso=13; tipo_rotazione=1;}   //*
        if(x0>x1  && y0>y1  && x1<=x2 && y1<=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A>B ){caso=14; tipo_rotazione=2;}   //*
        if(x0>x1  && y0>y1  && x1<=x2 && y1<=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A<B ){caso=13; tipo_rotazione=2;}   //*
        if(x0>x1  && y0>y1  && x1<=x2 && y1<=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A<B ){caso=14; tipo_rotazione=1;}   //*
        if(x0==x1 && y0>y1  && x1<=x2 && y1<=y2 && dir_prec==dir_attu && A!=B                ){caso=13; tipo_rotazione=1;}   //*
        if(x0==x1 && y0>y1  && x1<=x2 && y1<=y2 && dir_prec!=dir_attu && A!=B                ){caso=14; tipo_rotazione=2;}   //*
        if(x0>x1  && y0==y1 && x1<=x2 && y1<=y2 && dir_prec==dir_attu && A!=B                ){caso=13; tipo_rotazione=2;}   //*
        if(x0>x1  && y0==y1 && x1<=x2 && y1<=y2 && dir_prec!=dir_attu && A!=B                ){caso=14; tipo_rotazione=1;}   //*

        if(x0>x1  && y0>y1  && x1<=x2 && y1>=y2 && dir_prec==dir_attu && A!=B                ){caso=15; tipo_rotazione=1;}
        if(x0>x1  && y0>y1  && x1<=x2 && y1>=y2 && dir_prec!=dir_attu && A!=B                ){caso=16; tipo_rotazione=2;}

        // Combinazioni "C"
        if(x0<x1  && y0<y1  && x1>=x2 && y1<=y2 && dir_prec==dir_attu && A!=B                ){caso=17; tipo_rotazione=1;}
        if(x0<x1  && y0<y1  && x1>=x2 && y1<=y2 && dir_prec!=dir_attu && A!=B                ){caso=18; tipo_rotazione=2;}

        if(x0<x1  && y0<y1  && x1>=x2 && y1>=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A>B ){caso=19; tipo_rotazione=1;}   //*
        if(x0<x1  && y0<y1  && x1>=x2 && y1>=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A>B ){caso=20; tipo_rotazione=2;}   //*
        if(x0<x1  && y0<y1  && x1>=x2 && y1>=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A<B ){caso=19; tipo_rotazione=2;}   //*
        if(x0<x1  && y0<y1  && x1>=x2 && y1>=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A<B ){caso=20; tipo_rotazione=1;}   //*
        if(x0==x1 && y0<y1  && x1>=x2 && y1>=y2 && dir_prec==dir_attu && A!=B                ){caso=19; tipo_rotazione=1;}   //*
        if(x0==x1 && y0<y1  && x1>=x2 && y1>=y2 && dir_prec!=dir_attu && A!=B                ){caso=20; tipo_rotazione=2;}   //*
        if(x0<x1  && y0==y1 && x1>=x2 && y1>=y2 && dir_prec==dir_attu && A!=B                ){caso=19; tipo_rotazione=2;}   //*
        if(x0<x1  && y0==y1 && x1>=x2 && y1>=y2 && dir_prec!=dir_attu && A!=B                ){caso=20; tipo_rotazione=1;}   //*

        if(x0<x1  && y0<y1  && x1<=x2 && y1<=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A>B ){caso=21; tipo_rotazione=2;}   //*
        if(x0<x1  && y0<y1  && x1<=x2 && y1<=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A>B ){caso=22; tipo_rotazione=1;}   //*
        if(x0<x1  && y0<y1  && x1<=x2 && y1<=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A<B ){caso=21; tipo_rotazione=1;}   //*
        if(x0<x1  && y0<y1  && x1<=x2 && y1<=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A<B ){caso=22; tipo_rotazione=2;}   //*
        if(x0==x1 && y0<y1  && x1<=x2 && y1<=y2 && dir_prec==dir_attu && A!=B                ){caso=21; tipo_rotazione=2;}   //*
        if(x0==x1 && y0<y1  && x1<=x2 && y1<=y2 && dir_prec!=dir_attu && A!=B                ){caso=22; tipo_rotazione=1;}   //*
        if(x0<x1  && y0==y1 && x1<=x2 && y1<=y2 && dir_prec==dir_attu && A!=B                ){caso=21; tipo_rotazione=1;}   //*
        if(x0<x1  && y0==y1 && x1<=x2 && y1<=y2 && dir_prec!=dir_attu && A!=B                ){caso=22; tipo_rotazione=2;}   //*

        if(x0<x1  && y0<y1  && x1<=x2 && y1>=y2 && dir_prec==dir_attu && A!=B                ){caso=23; tipo_rotazione=2;}
        if(x0<x1  && y0<y1  && x1<=x2 && y1>=y2 && dir_prec!=dir_attu && A!=B                ){caso=24; tipo_rotazione=1;}

        // Combinazioni "D"
        if(x0<x1  && y0>y1  && x1>=x2 && y1<=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A>B ){caso=25; tipo_rotazione=1;}   //*
        if(x0<x1  && y0>y1  && x1>=x2 && y1<=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A>B ){caso=26; tipo_rotazione=2;}   //*
        if(x0<x1  && y0>y1  && x1>=x2 && y1<=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A<B ){caso=25; tipo_rotazione=2;}   //*
        if(x0<x1  && y0>y1  && x1>=x2 && y1<=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A<B ){caso=26; tipo_rotazione=1;}   //*
        if(x0==x1 && y0>y1  && x1>=x2 && y1<=y2 && dir_prec==dir_attu && A!=B                ){caso=25; tipo_rotazione=2;}   //*
        if(x0==x1 && y0>y1  && x1>=x2 && y1<=y2 && dir_prec!=dir_attu && A!=B                ){caso=26; tipo_rotazione=1;}   //*
        if(x0<x1  && y0==y1 && x1>=x2 && y1<=y2 && dir_prec==dir_attu && A!=B                ){caso=25; tipo_rotazione=1;}   //*
        if(x0<x1  && y0==y1 && x1>=x2 && y1<=y2 && dir_prec!=dir_attu && A!=B                ){caso=26; tipo_rotazione=2;}   //*

        if(x0<x1  && y0>y1  && x1>=x2 && y1>=y2 && dir_prec==dir_attu && A!=B                ){caso=27; tipo_rotazione=2;}
        if(x0<x1  && y0>y1  && x1>=x2 && y1>=y2 && dir_prec!=dir_attu && A!=B                ){caso=28; tipo_rotazione=1;}

        if(x0<x1  && y0>y1  && x1<=x2 && y1<=y2 && dir_prec==dir_attu && A!=B                ){caso=29; tipo_rotazione=1;}
        if(x0<x1  && y0>y1  && x1<=x2 && y1<=y2 && dir_prec!=dir_attu && A!=B                ){caso=30; tipo_rotazione=2;}

        if(x0<x1  && y0>y1  && x1<=x2 && y1>=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A>B ){caso=31; tipo_rotazione=2;}   //*
        if(x0<x1  && y0>y1  && x1<=x2 && y1>=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A>B ){caso=32; tipo_rotazione=1;}   //*
        if(x0<x1  && y0>y1  && x1<=x2 && y1>=y2 && dir_prec==dir_attu && A!=0 && B!=0 && A<B ){caso=31; tipo_rotazione=1;}   //*
        if(x0<x1  && y0>y1  && x1<=x2 && y1>=y2 && dir_prec!=dir_attu && A!=0 && B!=0 && A<B ){caso=32; tipo_rotazione=2;}   //*
        if(x0==x1 && y0>y1  && x1<=x2 && y1>=y2 && dir_prec==dir_attu && A!=B                ){caso=31; tipo_rotazione=1;}   //*
        if(x0==x1 && y0>y1  && x1<=x2 && y1>=y2 && dir_prec!=dir_attu && A!=B                ){caso=32; tipo_rotazione=2;}   //*
        if(x0<x1  && y0==y1 && x1<=x2 && y1>=y2 && dir_prec==dir_attu && A!=B                ){caso=31; tipo_rotazione=2;}   //*
        if(x0<x1  && y0==y1 && x1<=x2 && y1>=y2 && dir_prec!=dir_attu && A!=B                ){caso=32; tipo_rotazione=1;}   //*
        //----------------------------------------
        // Impostazione tipo rotazione
        //----------------------------------------
        if(tipo_rotazione==2) new_dati[i].rot = (short int)(new_dati[i].rot+60); //ROTAZIONE ANTI-ORARIA
        if(tipo_rotazione==1) new_dati[i].rot = (short int)(new_dati[i].rot+64); //ROTAZIONE ORARIA
        if(tipo_rotazione==0){sprintf(all_mess, "Deviazione %d -> %d sconosciuta....", attu, succ); return 1;}
     }
     //-----------------------------------------------------
     // Se ho determinato che si tratta di una deviazione
     // in base al tipo faccio un calcolo dei gradi di
     // deviazione presunti e se sono sotto al minimo di
     // attivazione DEVIAZIONI ripristino la rotazione
     // originale
     //-----------------------------------------------------
     if(new_dati[i].rot>3){
        grd=0;
        stato_rot=0;
        anti_oraria=false;
        switch(new_dati[i].rot){
           case 50: stato_rot=0;
                    if(rot_prec==1 && stato_rot==0) anti_oraria=false;
                    if(rot_prec==2 && stato_rot==0) anti_oraria=true;  //180° scegliamo le curve anti-orarie
                    if(rot_prec==3 && stato_rot==0) anti_oraria=true;
                    break;
           case 51: stato_rot=1;
                    if(rot_prec==0 && stato_rot==1) anti_oraria=true;
                    if(rot_prec==2 && stato_rot==1) anti_oraria=false;
                    if(rot_prec==3 && stato_rot==1) anti_oraria=true;  //180° scegliamo le curve anti-orarie
                    break;
           case 52: stato_rot=2;
                    if(rot_prec==0 && stato_rot==2) anti_oraria=true;  //180° scegliamo le curve anti-orarie
                    if(rot_prec==1 && stato_rot==2) anti_oraria=true;
                    if(rot_prec==3 && stato_rot==2) anti_oraria=false;
                    break;
           case 53: stato_rot=3;
                    if(rot_prec==0 && stato_rot==3) anti_oraria=false;
                    if(rot_prec==1 && stato_rot==3) anti_oraria=true;  //180° scegliamo le curve anti-orarie
                    if(rot_prec==2 && stato_rot==3) anti_oraria=true;
                    break;
           case 54: stato_rot=0;
                    if(rot_prec==1 && stato_rot==0) anti_oraria=false;
                    if(rot_prec==2 && stato_rot==0) anti_oraria=true;  //180° scegliamo le curve anti-orarie
                    if(rot_prec==3 && stato_rot==0) anti_oraria=true;
                    break;
           case 55: stato_rot=1;
                    if(rot_prec==0 && stato_rot==1) anti_oraria=true;
                    if(rot_prec==2 && stato_rot==1) anti_oraria=false;
                    if(rot_prec==3 && stato_rot==1) anti_oraria=true;  //180° scegliamo le curve anti-orarie
                    break;
           case 56: stato_rot=2;
                    if(rot_prec==0 && stato_rot==2) anti_oraria=true;  //180° scegliamo le curve anti-orarie
                    if(rot_prec==1 && stato_rot==2) anti_oraria=true;
                    if(rot_prec==3 && stato_rot==2) anti_oraria=false;
                    break;
           case 57: stato_rot=3;
                    if(rot_prec==0 && stato_rot==3) anti_oraria=false;
                    if(rot_prec==1 && stato_rot==3) anti_oraria=true;  //180° scegliamo le curve anti-orarie
                    if(rot_prec==2 && stato_rot==3) anti_oraria=true;
                    break;
           case 60: stato_rot=0;
                    anti_oraria=true;   // tratti deviati senza variazione di rotazione (antiorario)
                    break;
           case 61: stato_rot=1;
                    anti_oraria=true;   // tratti deviati senza variazione di rotazione (antiorario)
                    break;
           case 62: stato_rot=2;
                    anti_oraria=true;   // tratti deviati senza variazione di rotazione (antiorario)
                    break;
           case 63: stato_rot=3;
                    anti_oraria=true;   // tratti deviati senza variazione di rotazione (antiorario)
                    break;
           case 64: stato_rot=0;
                    anti_oraria=false;  // tratti deviati senza variazione di rotazione (orario)
                    break;
           case 65: stato_rot=1;
                    anti_oraria=false;  // tratti deviati senza variazione di rotazione (orario)
                    break;
           case 66: stato_rot=2;
                    anti_oraria=false;  // tratti deviati senza variazione di rotazione (orario)
                    break;
           case 67: stato_rot=3;
                    anti_oraria=false;  // tratti deviati senza variazione di rotazione (orario)
                    break;
           default: stato_rot = new_dati[i].rot;
                    {sprintf(all_mess, "Deviazione (ROT=%d) %d -> %d sconosciuta....", stato_rot, attu, succ); return 1;}
        }
        //-------------------------------------------
        // Recupero i gradi da compiere
        //-------------------------------------------
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
        //--------------------------------------------
        // Se i gradi di deviazione sono al di sotto
        // di quelli minimi di attivazione DEVIAZIONI
        // ripristino la ROTAZIONE ORIGINALE
        //--------------------------------------------
        if((int)(grd)<GrdMinPerAttivaDeviazioni) new_dati[i].rot = stato_rot;
     }
  }
  return 0;
}

// -----------------------------------
//   test_chiamata_asservita()
// -----------------------------------
// Ritrona 1 se la chiamata passata ha già un AGV che la stà serverndo.
// --------------------------------------------------------------------------
// NB:  Il numero di chiamata identifica fisicamente il numero di locazione!
// --------------------------------------------------------------------------
//
int process::test_chiamata_asservita(int chiamata)
/************************************************/
{
  int  i;
  char all_mess[101]="";
  short int num_baia, num_mac, num_mag, num_stz;
  short int num_baia1, num_mac1, num_mag1, num_stz1;

  num_baia=num_mac=num_mag=num_stz=0;
  //-----------------------------------------
  // Scomposizione della chiamata
  //-----------------------------------------
  TestChiamataNum(chiamata, &num_mag, &num_baia, &num_mac, &num_stz);
  //-----------------------------------------
  // (1)
  // Verifico se un altro TGV ha esattamente
  // la stessa chiamata in asservimento
  //-----------------------------------------
  for(i=1; i<=MAXAGV; i++){
     //-----------------------------------------
     // Chiamata asservita da altro agv
     //-----------------------------------------
     if(chiamata==AGV[i]->mission.Chiamata) return i;
  }
  //-----------------------------------------
  // SOLO UN TGV PER VOLTA SULLA MACCHINA
  // - nessun altro TGV con la chiamata
  //   abbinata a una stazione macchina
  // - nessun altro TGV sulla macchina.
  //-----------------------------------------
  if(num_mac>0 && num_mac<=TOTCHIAMATE){
     // ALTRO TGV CON CHIAMATA ABBINATA ALLA MACCHINA
     for(i=1; i<=MAXAGV; i++){
        if(AGV[i]->mission.Chiamata==0) continue;
        //-----------------------------------------
        // Chiamata asservita da altro agv
        //-----------------------------------------
        num_baia1=num_mac1=num_mag1=num_stz1=0;
        TestChiamataNum(AGV[i]->mission.Chiamata, &num_mag1, &num_baia1, &num_mac1, &num_stz1);
        // stessa macchina
        if(num_mac1>0 && num_mac1<=TOTCHIAMATE && num_mac==num_mac1) return i;
     }
     // ALTRO TGV SU NODO MACCHINA
     for(i=1; i<=MAXAGV; i++){
        //-----------------------------------------
        // AL-18/03 Solo nodi dell'impianto
        //-----------------------------------------
        if(AGV[i]->stato.pos<1                       ) continue;
        if(AGV[i]->stato.pos>TOTPUNTI                ) continue;
        if(N.punti_notevoli[AGV[i]->stato.pos]==false) continue;
        //-----------------------------------------
        // Chiamata asservita da altro agv
        //-----------------------------------------
        num_mac1=num_stz1=0;
        test_punto_presente_mac(all_mess, AGV[i]->stato.pos, &num_mac1, &num_stz1);
        // stessa macchina
        if(num_mac1>0 && num_mac1<=TOTCHIAMATE && num_mac==num_mac1) return i;
     }
  }
  //-----------------------------------------
  // SOLO UN TGV PER VOLTA SULLA BAIA
  // - nessun altro TGV sulla macchina.
  //-----------------------------------------
  if(num_baia>0 && num_baia<=TOTBAIE){
     // ALTRO TGV SU NODO BAIA
     for(i=1; i<=MAXAGV; i++){
        if(N.punti_notevoli[AGV[i]->stato.pos]==false) continue;
        //-----------------------------------------
        // Chiamata asservita da altro agv
        //-----------------------------------------
        num_baia1=0;
        test_punto_presente_baia(all_mess, AGV[i]->stato.pos, &num_baia1);
        // stessa macchina
        if(num_baia1>0 && num_baia1<=TOTBAIE && num_baia==num_baia1) return i;
     }
  }
  //-----------------------------------------
  // SOLO UN TGV PER VOLTA SULLA BAIA
  // - nessun altro TGV sulla macchina.
  //-----------------------------------------
/*
  if(num_mag>0 && num_mag<=TOTMAG){
     // ALTRO TGV SU NODO BAIA
     for(i=1; i<=MAXAGV; i++){
        if(N.punti_notevoli[AGV[i]->stato.pos]==false) continue;
        //-----------------------------------------
        // Chiamata asservita da altro agv
        //-----------------------------------------
        num_mag1=num_stz1=0;
        M.test_punto_presente_mag(all_mess, AGV[i]->stato.pos, &num_mag1, &num_stz1);
        // stessa macchina
        if(num_mag1>0 && num_mag1<=TOTMAG && num_mag==num_mag1) return i;
     }
  }
*/
  return 0;
}


// ---------------------------------
//   test_dest_occupata()
// -----------------------------------
// ritorna il numero  se la destinazione Š gi… occupata da un AGV
//
int process::test_dest_occupata(short int dest, short int *num_agv)
/*****************************************************/
{
  char buffer[101]="";
  int tgv_occupante;
  short int num_baia, num_mag, num_pos, i, j, k;

  *num_agv=0;
  //------------------------------
  // Nodo non attendibile
  //------------------------------
  if(dest<1       ) return 0;
  if(dest>TOTPUNTI) return 0;

  tgv_occupante = N.nodo_busy[dest];
  //--------------------------------------------------------
  // INTERBLOCCO NODI
  // Verifico se il nodo considerato appartiene alla
  // struttura dei nodi da interbloccare
  //--------------------------------------------------------
  for(i=0; i<50; i++){
     if(tgv_occupante!=0) break;
     for(j=0; j<50; j++){
        if(tgv_occupante!=0                ) break;
        if(N.nodi_interbloccati[i][j]!=dest) continue;
        //---------------------------------------------
        // Verifico se altri nodi appartenenti alla
        // stessa riga sono destinazione di qualche
        // TGV
        //---------------------------------------------
        for(k=0; k<50; k++){
           if(k==j                                      ) continue;
           if(N.nodi_interbloccati[i][k]<1              ) continue;
           if(N.nodi_interbloccati[i][k]>MAXPUNTI       ) continue;
           if(N.nodo_busy[N.nodi_interbloccati[i][k]]==0) continue;
           tgv_occupante = N.nodo_busy[N.nodi_interbloccati[i][k]];
           break;
        }
     }
  }

  //--------------------------------------------------------
  // MAGAZZINI MULTI NODO O FIFO CON NODI SEPARATI
  //--------------------------------------------------------
  if(tgv_occupante==0){
     num_mag=num_pos=0;
     M.test_punto_presente_mag(buffer, dest, &num_mag, &num_pos);
     if(num_mag>0 && num_mag<=TOTMAG){
        //--------------------------------------------------------
        // MAGAZZINI FIFO CON NODI DIFFERENTI INGRESSO/USCITA
        //--------------------------------------------------------
        if(M.m[num_mag]->PuntoIng!=M.m[num_mag]->PuntoOut && tgv_occupante==0){
           //--------------------------------------------------------
           // Controllo sia i nodi di ingresso che quelli di uscita
           //--------------------------------------------------------
           tgv_occupante = N.nodo_busy[M.m[num_mag]->PuntoIng];
           if(tgv_occupante==0) tgv_occupante = N.nodo_busy[M.m[num_mag]->PuntoOut];
        }
        //--------------------------------------------------------
        // MAGAZZINI MULTI NODO
        // Se attiva la gestione multi nodo dei magazzini e
        // basta che uno dei nodi sia occupato.
        //--------------------------------------------------------
        if(MagazziniMultiNodo==true && tgv_occupante==0){
           //-----------------------------------------
           // Scroll di tutti i nodi del magazzino
           // alla ricerca di uno occupato
           //-----------------------------------------
           for(i=1; i<=MAXTERRAMAG; i++){
              if(M.m[num_mag]->Pos[i].NodoIng<1              ) continue;
              if(M.m[num_mag]->Pos[i].NodoIng>TOTPUNTI       ) continue;
              if(N.nodo_busy[M.m[num_mag]->Pos[i].NodoIng]==0) continue;
              tgv_occupante = N.nodo_busy[M.m[num_mag]->Pos[i].NodoIng];
              break;
           }
           if(tgv_occupante==0){
              for(i=1; i<=MAXTERRAMAG; i++){
                 if(M.m[num_mag]->Pos[i].NodoOut<1              ) continue;
                 if(M.m[num_mag]->Pos[i].NodoOut>TOTPUNTI       ) continue;
                 if(N.nodo_busy[M.m[num_mag]->Pos[i].NodoOut]==0) continue;
                 tgv_occupante = N.nodo_busy[M.m[num_mag]->Pos[i].NodoOut];
                 break;
              }
           }
        }
     }
  }

  //--------------------------------------------------------
  // BAIE CON PUNTI "AUX"
  // Su baie con punti Aux impostati, basta uno dei nodi
  // occupati per ritenere occupata tutti i punti
  // fila, basta che uno dei nodi sia occupato.
  //--------------------------------------------------------
  if(tgv_occupante==0){
     num_baia=0;
     P.test_punto_presente_baia(buffer, dest, &num_baia);
     if(num_baia>0 && num_baia<=TOTBAIE){
        //-----------------------------------------
        // Aggiungo agli interbloccati tutti i nodi
        // delle posizioni magazzino
        //-----------------------------------------
        for(i=0; i<5; i++){
           if(P.ba[num_baia]->PuntoAux[i]<1                ) continue;
           if(P.ba[num_baia]->PuntoAux[i]>TOTPUNTI         ) continue;
           if(N.nodo_busy[P.ba[num_baia]->PuntoAux[i]]==0  ) continue;
           tgv_occupante = N.nodo_busy[P.ba[num_baia]->PuntoAux[i]];
           break;
        }
     }
  }

  //--------------------------------------------------------
  // Assegno TGV OCCUPANTE
  //--------------------------------------------------------
  *num_agv=tgv_occupante;

  return tgv_occupante;
}


// -----------------------------------
//   test_dest_riservata()
//  Ritorna 1 in caso di destinazione
//  già riservata.
// -----------------------------------
// ritorna 1 se la destinazione è già riservata ad un altro AGV
//
int process::test_dest_riservata(short int dest, short int *num_agv, short int mio_agv)
/*******************************************************/
{
  char buffer[201]="";
  bool PercorsoRiservato;
  short int tgv_in_ing, tgv_in_out;
  short int num_baia, num_mag, num_pos, i, j, k, z, nn;

  *num_agv=0;
  //------------------------------------
  // Nodo non attendibile
  //------------------------------------
  if(dest<1       ) return 0;
  if(dest>TOTPUNTI) return 0;

  //------------------------------------
  // SOLO PER I PUNTI TERMINALI
  //------------------------------------
  if(N.punti_notevoli[dest]==false) return 0;

  //------------------------------------
  // Verifico nodo riservato
  //------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(i==mio_agv) continue;
     //----------------------------------------------------------------
     // Verifico tutti i percorsi degli altri TGV (start escluso)
     //----------------------------------------------------------------
     PercorsoRiservato=false;
     for(j=1; j<MAXPERCORSI; j++){
        if(AGV[i]->mission.punto_prenotato[j]==0) break;
        if(dest==AGV[i]->mission.punto_prenotato[j]){
           PercorsoRiservato=true;
           break;
        }
     }
     //----------------------------------------------------------------
     // Considero una destinazione riservata se:
     // - Fa parte del percorso di un altro TGV (Percorsi Alternativi)
     // - Coincide con la destinazione di un altro TGV.
     //----------------------------------------------------------------
     if(PercorsoRiservato==true || dest==AGV[i]->mission.pend){
        *num_agv = i;
        return 1;
     };
  };

  //--------------------------------------------------------
  // INTERBLOCCO NODI
  // Verifico se il nodo considerato appartiene alla
  // struttura dei nodi da interbloccare
  //--------------------------------------------------------
  for(i=0; i<50; i++){
     for(j=0; j<50; j++){
        if(N.nodi_interbloccati[i][j]!=dest) continue;
        //---------------------------------------------
        // Verifico se altri nodi appartenenti alla
        // stessa riga sono destinazione di qualche
        // TGV
        //---------------------------------------------
        for(k=0; k<50; k++){
           if(k==j                               ) continue;
           if(N.nodi_interbloccati[i][k]<1       ) continue;
           if(N.nodi_interbloccati[i][k]>MAXPUNTI) continue;
           for(z=1; z<=MAXAGV; z++){
              if(z==mio_agv                                      ) continue;
              if(AGV[z]->mission.pend!=N.nodi_interbloccati[i][k]) continue;
              *num_agv = z;
              return 1;
           }
        }
     }
  }

  //--------------------------------------------------------
  // MAGAZZINI MULTI NODO O FIFO CON NODI SEPARATI
  //--------------------------------------------------------
  num_mag=num_pos=0;
  M.test_punto_presente_mag(buffer, dest, &num_mag, &num_pos);
  if(num_mag>0 && num_mag<=TOTMAG){
     //--------------------------------------------------------
     // MAGAZZINI CON NODI DIVERSI INGRESSO/USCITA
     //--------------------------------------------------------
     if(M.m[num_mag]->PuntoIng!=M.m[num_mag]->PuntoOut){
        //-----------------------------------------
        // Verifico destinazione di tutti i TGV
        //-----------------------------------------
        for(j=1; j<=MAXAGV; j++){
           if(j==mio_agv                                  ) continue;
           if(AGV[j]->mission.pend!=M.m[num_mag]->PuntoIng &&
              AGV[j]->mission.pend!=M.m[num_mag]->PuntoOut) continue;
           *num_agv = j;
           return 1;
        }
     }
     //--------------------------------------------------------
     // MAGAZZINI MULTI NODO
     // Se attiva la gestione multi nodo dei magazzini e
     // basta che uno dei nodi sia occupato.
     //--------------------------------------------------------
     if(MagazziniMultiNodo==true){
        //-----------------------------------------
        // CONTROLLO SE IL TGV è destinato
        // all'ingresso o all'uscita
        //-----------------------------------------
        tgv_in_ing=0;
        for(i=1; i<=MAXTERRAMAG; i++){
           if(M.m[num_mag]->Pos[i].NodoIng<1         ) continue;
           if(M.m[num_mag]->Pos[i].NodoIng>TOTPUNTI  ) continue;
           if(M.m[num_mag]->Pos[i].NodoIng!=dest     ) continue;
           tgv_in_ing=i;
           break;
        }
        tgv_in_out=0;
        for(i=1; i<=MAXTERRAMAG; i++){
           if(M.m[num_mag]->Pos[i].NodoOut<1         ) continue;
           if(M.m[num_mag]->Pos[i].NodoOut>TOTPUNTI  ) continue;
           if(M.m[num_mag]->Pos[i].NodoOut!=dest     ) continue;
           tgv_in_out=i;
           break;
        }
        //-----------------------------------------
        // TGV IN USCITA
        // controllo tutti i nodi di uscita
        // ma di ingresso, solo quello della pos
        // considerata
        //-----------------------------------------
        if(tgv_in_out!=0){
           //-------------------------------------------------
           // Contro tutti i nodi di ingresso
           //-------------------------------------------------
           for(i=1; i<=MAXTERRAMAG; i++){
              if(M.m[num_mag]->Pos[i].NodoOut<1         ) continue;
              if(M.m[num_mag]->Pos[i].NodoOut>TOTPUNTI  ) continue;
              //-----------------------------------------
              // Verifico destinazione di tutti i TGV
              //-----------------------------------------
              for(j=1; j<=MAXAGV; j++){
                 if(j==mio_agv                                        ) continue;
                 if(AGV[j]->mission.pend!=M.m[num_mag]->Pos[i].NodoOut) continue;
                 *num_agv = j;
                 return 1;
              }
           }
           //-------------------------------------------------
           // Controllo solo specifica posizione di uscita
           //-------------------------------------------------
           if(M.m[num_mag]->Pos[tgv_in_out].NodoOut>0 && M.m[num_mag]->Pos[tgv_in_out].NodoOut<=TOTPUNTI){
              //-----------------------------------------
              // Verifico destinazione di tutti i TGV
              //-----------------------------------------
              for(j=1; j<=MAXAGV; j++){
                 if(j==mio_agv) continue;
                 if(AGV[j]->mission.pend!=M.m[num_mag]->Pos[tgv_in_out].NodoOut) continue;
                 *num_agv = j;
                 return 1;
              }
           }
        }
        //-----------------------------------------
        // TGV IN INGRESSO
        // controllo tutti i nodi di ingresso
        // ma di uscita, solo quello della pos
        // considerata
        //-----------------------------------------
        if(tgv_in_ing!=0){
           //-------------------------------------------------
           // Contro tutti i nodi di ingresso
           //-------------------------------------------------
           for(i=1; i<=MAXTERRAMAG; i++){
              if(M.m[num_mag]->Pos[i].NodoIng<1         ) continue;
              if(M.m[num_mag]->Pos[i].NodoIng>TOTPUNTI  ) continue;
              //-----------------------------------------
              // Verifico destinazione di tutti i TGV
              //-----------------------------------------
              for(j=1; j<=MAXAGV; j++){
                 if(j==mio_agv                                        ) continue;
                 if(AGV[j]->mission.pend!=M.m[num_mag]->Pos[i].NodoIng) continue;
                 *num_agv = j;
                 return 1;
              }
           }
           //-------------------------------------------------
           // Controllo solo specifica posizione di uscita
           //-------------------------------------------------
           if(M.m[num_mag]->Pos[tgv_in_ing].NodoOut>0 && M.m[num_mag]->Pos[tgv_in_ing].NodoOut<=TOTPUNTI){
              //-----------------------------------------
              // Verifico destinazione di tutti i TGV
              //-----------------------------------------
              for(j=1; j<=MAXAGV; j++){
                 if(j==mio_agv) continue;
                 if(AGV[j]->mission.pend!=M.m[num_mag]->Pos[tgv_in_ing].NodoOut) continue;
                 *num_agv = j;
                 return 1;
              }
           }
        }
     }
  }


  //--------------------------------------------------------
  // BAIE CON PUNTI "AUX"
  // Su baie con punti Aux impostati, basta uno dei nodi
  // occupati per ritenere occupata tutti i punti
  // fila, basta che uno dei nodi sia occupato.
  //--------------------------------------------------------
  num_baia=0;
  P.test_punto_presente_baia(buffer, dest, &num_baia);
  if(num_baia>0 && num_baia<=TOTBAIE){
     //-----------------------------------------
     // Aggiungo agli interbloccati tutti i nodi
     // delle posizioni magazzino
     //-----------------------------------------
     for(i=0; i<5; i++){
        if(P.ba[num_baia]->PuntoAux[i]<1                ) continue;
        if(P.ba[num_baia]->PuntoAux[i]>TOTPUNTI         ) continue;
        //-----------------------------------------
        // Verifico destinazione di tutti i TGV
        //-----------------------------------------
        for(j=1; j<=MAXAGV; j++){
           if(j==mio_agv                                       ) continue;
           if(AGV[j]->mission.pend!=P.ba[num_baia]->PuntoAux[i]) continue;
           *num_agv = j;
           return 1;
        }
     }
  }

  return 0;
}

// -------------------------------------
//   test_dest_limitrofe()
//  Ritorna 1 in caso di destinazione
//  limitrofa già riservata o occupata
// -------------------------------------
// ritorna 1 se la destinazione è in limitrofa a un altro punto
// terminale già occupato o riservato da un altro AGV.
//
int process::test_dest_limitrofe(short int dest, short int *num_agv, short int *vicino, short int mio_agv)
/*******************************************************/
{
  char all_mess[200]="";
  bool destin_interbloccato;
  bool vicino_interbloccato;
  bool TrovatoVicinoLimitrofo;
  short int i, j, k, id, num_mag, num_pos, Vicino;
  short int RotDest=0;
  short int RotVicino=0;
  short int altro_agv, num_rot;
  short int elenco_interblocchi[TOTPUNTI+1];
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  short int DistanzaInNodi=0;
  long int  Dist, DistX, DistY;
  double CalcolaRad;
  int  err=0, x1, y1, xv, yv;

  *vicino=0;
  *num_agv=0;
  altro_agv=num_rot=0;
  memset(&nodi_perc, 0, sizeof(nodi_perc));
  memset(&dati_perc, 0, sizeof(dati_perc));
  memset(&elenco_interblocchi, 0, sizeof(elenco_interblocchi));
  //-------------------------------------------------------
  // 30/08/2011 TGV CONSIDERATO GIA' IN POSIZIONE
  // Se il TGV considerato di trova già sul nodo di
  // destinazione, tanto vale che completi la missione
  //-------------------------------------------------------
  if(mio_agv>0 && mio_agv<=MAXAGV && AGV[mio_agv]->stato.pos==dest) return 0;

  //-------------------------------------------------------
  // NODI INTERBLOCCATI
  // Se il nodo considerato appartiene alla struttura dei
  // nodi interbloccati non ha senso valutarne la
  // "limitroficità" con altri nodi
  //-------------------------------------------------------
  id=0;
  destin_interbloccato=false;
  for(i=0; i<50; i++){
     for(j=0; j<50; j++){
        if(N.nodi_interbloccati[i][j]!=dest) continue;
        destin_interbloccato = true;
        //--------------------------------------------
        // Compilazione elenco nodi interbloccati
        //--------------------------------------------
        for(k=0; k<50; k++){
           if(N.nodi_interbloccati[i][k]<1        ) continue;
           if(N.nodi_interbloccati[i][k]>TOTPUNTI ) continue;
           elenco_interblocchi[id] = N.nodi_interbloccati[i][k];
           id++;
        }
        break;
     }
     if(destin_interbloccato==true) break;
  }

  //--------------------------------------------------------
  // MAGAZZINI MULTI NODO
  // Se attiva la gestione multi nodo dei magazzini e
  // basta che uno dei nodi sia occupato.
  //--------------------------------------------------------
  if(MagazziniMultiNodo==true){
     num_mag=num_pos=0;
     M.test_punto_presente_mag(all_mess, dest, &num_mag, &num_pos);
     if(num_mag>0 && num_mag<=TOTMAG){
        //-----------------------------------------
        // Aggiungo agli interbloccati tutti i nodi
        // delle posizioni magazzino
        //-----------------------------------------
        for(i=1; i<=MAXTERRAMAG; i++){
           if(M.m[num_mag]->Pos[i].NodoIng<1       ) continue;
           if(M.m[num_mag]->Pos[i].NodoIng>TOTPUNTI) continue;
           destin_interbloccato    = true;
           elenco_interblocchi[id] = M.m[num_mag]->Pos[i].NodoIng;
           id++;
        }
        for(i=1; i<=MAXTERRAMAG; i++){
           if(M.m[num_mag]->Pos[i].NodoOut<1       ) continue;
           if(M.m[num_mag]->Pos[i].NodoOut>TOTPUNTI) continue;
           destin_interbloccato    = true;
           elenco_interblocchi[id] = M.m[num_mag]->Pos[i].NodoOut;
           id++;
        }
     }
  }

  //-------------------------------------------------------
  // (A) Valuta tutti i nodi TROPPO vicini a DEST
  //-------------------------------------------------------
  if(destin_interbloccato==false){
     RotDest = N.n[dest]->rec[0].rot;
     x1 = Punti[ dest ][0];
     y1 = Punti[ dest ][1];
     for( Vicino=21; Vicino<=TOTPUNTI ; Vicino++){     // escludo punti di ricarica
        if( N.punti_notevoli[Vicino]==0 ) continue;
        if( dest == Vicino              ) continue;
        if( N.n[ Vicino ]->num ==0      ) continue;

        //-------------------------------------------------------------------
        // Se il la distanza tra il VICINO e la DEST in numero di
        // nodi è inferiore a 15 ---> Skip!
        // 19/02/2013 Calcolo Distanza spostato dopo controllo coordinate
        //            --> su impianto SPAL migliorato di 200msec a chiamata
        //-------------------------------------------------------------------
        //err = AGV[mio_agv]->distanza(FAST_PATH, Vicino, dest, nodi_perc, dati_perc, all_mess, &DistanzaInNodi, &num_rot);
        //if(err || DistanzaInNodi>MAX_NODI_LIMITROFI || DistanzaInNodi==0) continue;

        TrovatoVicinoLimitrofo=false;
        //------------------------------------------------------------
        // Estrai le coordinate del nodo VICINO e se
        //------------------------------------------------------------
        RotVicino = N.n[Vicino]->rec[0].rot;
        xv = Punti[ Vicino ][0];
        yv = Punti[ Vicino ][1];
        DistX = (long int) abs( x1-xv );
        DistY = (long int) abs( y1-yv );
        CalcolaRad = ( (double) DistX * (double) DistX) + ( (double) DistY * (double) DistY );
        Dist = (long int) sqrt( CalcolaRad );
        if((RotDest==0 || RotDest==2) && (RotVicino==1 || RotVicino==3)) continue;
        if((RotDest==1 || RotDest==3) && (RotVicino==0 || RotVicino==2)) continue;
        if(Dist > DISTANZA_MINIMA_OBLIQUA   ) continue;
        //-------------------------------------------------------------------
        // Se il la distanza tra il VICINO e la DEST in numero di
        // nodi è inferiore a 15 ---> Skip!
        //
        // 19/02/2013 Calcolo Distanza spostato dopo controllo coordinate
        //            --> su impianto SPAL migliorato di 200msec a chiamata
        //-------------------------------------------------------------------
        err = AGV[mio_agv]->distanza(FAST_PATH, Vicino, dest, nodi_perc, dati_perc, all_mess, &DistanzaInNodi, &num_rot);
        if(err || DistanzaInNodi>MAX_NODI_LIMITROFI || DistanzaInNodi==0) continue;

        //------------------------------------------------------------
        //  Se la Distanza Minima è Rispettata e i punti sono
        //  sono sullo stesso Asse --> CONTINUA
        //------------------------------------------------------------
        if((DistX<130 || DistY<130) && (RotDest==RotVicino) && (Dist<DISTANZA_LIMITROFA_MINIMA)){
           TrovatoVicinoLimitrofo=true;
        }
        //-----------------------------------------------------
        //  Se la Distanza Minima Obliqua per Punti NON
        //  sullo stesso Asse è Rispettata --> CONTINUA
        //-----------------------------------------------------
        if((DistX>100) && (DistY>200) && (RotDest!=RotVicino) && (Dist<DISTANZA_MINIMA_OBLIQUA)){
           TrovatoVicinoLimitrofo=true;
        }
        if((DistY>100) && (DistX>200) && (RotDest!=RotVicino) && (Dist<DISTANZA_MINIMA_OBLIQUA)){
           TrovatoVicinoLimitrofo=true;
        }
        //-----------------------------------------------------
        //  Se la Distanza Minima Contrapposta per Punti quasi
        //  sullo stesso Asse è Rispettata --> CONTINUA
        //-----------------------------------------------------
        if((DistX<100) && ((RotDest!=RotVicino) && (RotDest==0 || RotDest==2)) && (Dist<DISTANZA_MINIMA_CONTRAPPOSTA)){
           TrovatoVicinoLimitrofo=true;
        }
        if((DistY<100) && ((RotDest!=RotVicino) && (RotDest==1 || RotDest==3)) && (Dist<DISTANZA_MINIMA_CONTRAPPOSTA)){
           TrovatoVicinoLimitrofo=true;
        }
        //----------------------------------------------------------
        // Verifiva se il nodo VICINO è occupato da altro AGV
        //----------------------------------------------------------
        if(TrovatoVicinoLimitrofo ){
           if((P.test_dest_riservata( Vicino, &altro_agv, mio_agv) || P.test_dest_occupata( Vicino, &altro_agv)) && altro_agv!=mio_agv){
              *num_agv = altro_agv;
              *vicino  = Vicino;
              return 1;
           }
        }
     }
  }
  //-----------------------------------------------
  // 24/06/2011 NODO INTERBLOCCATO
  // controllo se tutti i nodi del gruppo hanno
  // un limitrofo impegnato
  //-----------------------------------------------
  else{
     for(k=0; k<TOTPUNTI; k++){
        if(elenco_interblocchi[k]<1       ) break;
        if(elenco_interblocchi[k]>TOTPUNTI) break;
        dest    = elenco_interblocchi[k];
        RotDest = N.n[dest]->rec[0].rot;
        x1 = Punti[ dest ][0];
        y1 = Punti[ dest ][1];
        for( Vicino=1; Vicino<=TOTPUNTI ; Vicino++){
           if( N.punti_notevoli[Vicino]==0 ) continue;
           if( dest == Vicino              ) continue;
           if( N.n[ Vicino ]->num ==0      ) continue;
           //------------------------------------------------------------
           // Scarto tutti i nodi Vicini appartenenti alla stessa lista
           // di nodi interbloccati
           //------------------------------------------------------------
           vicino_interbloccato=false;
           for(j=0; j<TOTPUNTI; j++){
              if(elenco_interblocchi[j]<1       ) break;
              if(elenco_interblocchi[j]>TOTPUNTI) break;
              if(elenco_interblocchi[j]!=Vicino ) continue;
              vicino_interbloccato = true;
              break;
           }
           if(vicino_interbloccato==true) continue;
           //------------------------------------------------------------------
           // Se il la distanza tra il VICINO e la DEST in numero di
           // nodi è inferiore a 15 ---> Skip!
           //
           // 19/02/2013 Calcolo Distanza spostato dopo controllo coordinate
           //            --> su impianto SPAL migliorato di 200msec a chiamata
           //------------------------------------------------------------------
         //err = AGV[mio_agv]->distanza(FAST_PATH, Vicino, dest, nodi_perc, dati_perc, all_mess, &DistanzaInNodi, &num_rot);
         //if(err || DistanzaInNodi>MAX_NODI_LIMITROFI || DistanzaInNodi==0) continue;

           TrovatoVicinoLimitrofo=false;
           //------------------------------------------------------------
           // Estrai le coordinate del nodo VICINO e se
           //------------------------------------------------------------
           RotVicino = N.n[Vicino]->rec[0].rot;
           xv = Punti[ Vicino ][0];
           yv = Punti[ Vicino ][1];
           DistX = (long int) abs( x1-xv );
           DistY = (long int) abs( y1-yv );
           CalcolaRad = ( (double) DistX * (double) DistX) + ( (double) DistY * (double) DistY );
           Dist = (long int) sqrt( CalcolaRad );
           if((RotDest==0 || RotDest==2) && (RotVicino==1 || RotVicino==3)) continue;
           if((RotDest==1 || RotDest==3) && (RotVicino==0 || RotVicino==2)) continue;
           if(Dist > DISTANZA_MINIMA_OBLIQUA   ) continue;
           //------------------------------------------------------------------
           // Se il la distanza tra il VICINO e la DEST in numero di
           // nodi è inferiore a 15 ---> Skip!
           //
           // 19/02/2013 Calcolo Distanza spostato dopo controllo coordinate
           //            --> su impianto SPAL migliorato di 200msec a chiamata
           //------------------------------------------------------------------
           err = AGV[mio_agv]->distanza(FAST_PATH, Vicino, dest, nodi_perc, dati_perc, all_mess, &DistanzaInNodi, &num_rot);
           if(err || DistanzaInNodi>MAX_NODI_LIMITROFI || DistanzaInNodi==0) continue;
           //------------------------------------------------------------
           //  Se la Distanza Minima è Rispettata e i punti sono
           //  sono sullo stesso Asse --> CONTINUA
           //------------------------------------------------------------
           if((DistX<130 || DistY<130) && (RotDest==RotVicino) && (Dist<DISTANZA_LIMITROFA_MINIMA)){
              TrovatoVicinoLimitrofo=true;
           }
           //-----------------------------------------------------
           //  Se la Distanza Minima Obliqua per Punti NON
           //  sullo stesso Asse è Rispettata --> CONTINUA
           //-----------------------------------------------------
           if((DistX>100) && (DistY>200) && (RotDest!=RotVicino) && (Dist<DISTANZA_MINIMA_OBLIQUA)){
              TrovatoVicinoLimitrofo=true;
           }
           if((DistY>100) && (DistX>200) && (RotDest!=RotVicino) && (Dist<DISTANZA_MINIMA_OBLIQUA)){
              TrovatoVicinoLimitrofo=true;
           }
           //-----------------------------------------------------
           //  Se la Distanza Minima Contrapposta per Punti quasi
           //  sullo stesso Asse è Rispettata --> CONTINUA
           //-----------------------------------------------------
           if((DistX<100) && ((RotDest!=RotVicino) && (RotDest==0 || RotDest==2)) && (Dist<DISTANZA_MINIMA_CONTRAPPOSTA)){
              TrovatoVicinoLimitrofo=true;
           }
           if((DistY<100) && ((RotDest!=RotVicino) && (RotDest==1 || RotDest==3)) && (Dist<DISTANZA_MINIMA_CONTRAPPOSTA)){
              TrovatoVicinoLimitrofo=true;
           }
           //----------------------------------------------------------
           // Verifiva se il nodo VICINO è occupato da altro AGV
           //----------------------------------------------------------
           if(TrovatoVicinoLimitrofo ){
              if((P.test_dest_riservata( Vicino, &altro_agv, mio_agv) || P.test_dest_occupata( Vicino, &altro_agv)) && altro_agv!=mio_agv){
                 *num_agv = altro_agv;
                 *vicino  = Vicino;
                 return 1;
              }
           }
        }
     }
  }
  return 0;
}


// -----------------------------------
//   TestNumChiamata()
// -----------------------------------
// Ritorna Il numero di chiamata in base alla struttura chiamante
//
//   CHIAMATA = TTNNSS
//
//   TT   = tipo provenienza      (1 = macchina)
//   NN   = numero macchina
//   SS   = stazione asservita
//
//   oppure:
//
//   CHIAMATA = TTNNNN
//
//   TT   = tipo provenienza      (3 = baia)
//   NNNN = numero baia
//
void process::TestNumChiamata(int *chiamata, short int Dest, short int Num)
/****************************************************************************/
{
  char all_mess[101]="";
  short int num_mag, num_pos;
  short int num_baia, num_mac, num_stz;

  //------------------------------------------
  // Inizializzazione delle variabili
  //------------------------------------------
  *chiamata=0;
  num_baia=num_mac=num_stz=num_mag=num_pos=0;
  P.test_punto_presente_mac(all_mess, Dest, &num_mac, &num_stz);
  if(num_mac==0){
     P.test_punto_presente_baia(all_mess, Dest, &num_baia);
     if(num_baia==0){
        M.test_punto_presente_mag(all_mess, Dest, &num_mag, &num_pos);
     }
  }
  //------------------------------------------
  // Compilazione numero chiamata per MAC
  //------------------------------------------
  if(num_mac>0 && num_mac<=TOTCHIAMATE){
     *chiamata = (int)((10000*PROV_MACCHINE )+(100*num_mac)+(num_stz+1));
  }
  //------------------------------------------
  // Compilazione numero chiamata per BAIA
  //------------------------------------------
  if(num_baia>0 && num_baia<=TOTBAIE){
     *chiamata = (int)((10000*PROV_BAIE )+(num_baia));
  }

  //------------------------------------------
  // Compilazione numero chiamata per MAG
  //------------------------------------------
  if(num_mag>0 && num_mag<=TOTMAG){
     *chiamata = (int)((10000*PROV_MAGAZZINO )+(num_mag));
  }

  return;
}

// -----------------------------------
//   TestChiamataNum()
// -----------------------------------
// Ritorna Il numero macchina e stazione chiamante in base alla chiamata
//
//   CHIAMATA = TTNNSS
//
//   TT   = tipo provenienza      (1 = macchina)
//   NN   = numero macchina
//   SS   = stazione asservita
//
//   oppure:
//
//   CHIAMATA = TTNNNN
//
//   TT   = tipo provenienza      (3 = baia)
//   NNNN = numero baia
//
bool process::TestChiamataNum(int chiamata, short int *num_mag, short int *num_baia, short int *num_mac, short int *num_stz)
/****************************************************************************/
{
  bool trovato;
  short int tipo, num_ba, num_ch, num_pos;

  trovato=false;
  num_pos=num_ch=tipo=0;
  *num_baia=*num_mag=*num_mac=*num_stz=0;
  //------------------------------------------
  // Recupero il tipo di macchina chiamante
  //------------------------------------------
  tipo     = (short int)(chiamata/10000);
  num_ba   = (short int)(chiamata - (tipo*10000));
  num_ch   = (short int)((chiamata - (tipo*10000))/100);
  num_pos  = (short int)(chiamata  - (tipo*10000) - (num_ch*100));
  //------------------------------------------
  // MACCHINE
  //------------------------------------------
  if(tipo==PROV_MACCHINE && num_ch>0 && num_ch<=TOTCHIAMATE){
     *num_mac = num_ch;
     *num_stz = (short int)(num_pos-1);
     trovato  = true;
  }
  //------------------------------------------
  // BAIE
  //------------------------------------------
  if(tipo==PROV_BAIE && num_ba>0 && num_ba<=TOTBAIE){
     *num_baia = num_ba;
     trovato  = true;
  }
  //------------------------------------------
  // MAGAZZINI
  //------------------------------------------
  if(tipo==PROV_MAGAZZINO && num_ba>0 && num_ba<=TOTMAG){
     *num_mag = num_ba;
     trovato  = true;
  }

  return !trovato;
}



// -----------------------------------
//   TestProvenienzaBox()
// -----------------------------------
// Ritorna Il numero macchina e stazione chiamante in base alla codifica di chiamata.
//
//   CHIAMATA = TTNNNN
//
//   TT   = tipo provenienza      (1 = macchine, 2 = magazzini, 3 = baie)
//   NNNN = numero struttura in base a tipo provenienza
//
bool process::TestProvenienzaBox(int provenienza, short int *tipo, short int *num_mac, short int *num_baia, short int *num_mag)
/****************************************************************************/
{
  bool trovato;
  short int tipo_prov;
  short int num_prov;

  num_prov=0;
  trovato=false;
  *tipo=*num_baia=*num_mac=*num_mag=0;
  //------------------------------------------
  // Recupero il tipo di macchina chiamante
  //------------------------------------------
  tipo_prov= (short int)(provenienza/10000);
  num_prov = (short int)(provenienza - (tipo_prov*10000));
  //------------------------------------------
  // MACCHINE
  //------------------------------------------
  if(tipo_prov==PROV_MACCHINE){
     *tipo = PROV_MACCHINE;
     if(num_prov>0 && num_prov<=TOTCHIAMATE){
        *num_mac = num_prov;
        trovato  = true;
     }
  }
  //------------------------------------------
  // BAIE
  //------------------------------------------
  if(tipo_prov==PROV_BAIE){
     *tipo = PROV_BAIE;
     if(num_prov>0 && num_prov<=TOTBAIE){
        *num_baia = num_prov;
        trovato   = true;
     }
  }
  //------------------------------------------
  // MAGAZZINI
  //------------------------------------------
  if(tipo_prov==PROV_MAGAZZINO){
     *tipo = PROV_MAGAZZINO;
     if(num_prov>0 && num_prov<=TOTMAG){
        *num_mag = num_prov;
        trovato  = true;
     }
  }

  return !trovato;
}


// -----------------------------------
//        TestChiamata()
// -----------------------------------
// Ritorna 1 se non c'è nessuna chiamata sulla macchina
//
int process::TestChiamata(short int Dest, short int Num)
/********************************************/
{
  bool ChON;
  bool AgvInIng;
  short int i, num_baia, num_mac, num_stz;
  char all_mess[101]="";

  ChON=false;
  num_baia=num_mac=num_stz=0;
  //-----------------------------------------------------
  // Scomposizione della chiamata
  //-----------------------------------------------------
  P.test_punto_presente_mac(all_mess, Dest, &num_mac, &num_stz);
  if(num_mac==0) P.test_punto_presente_baia(all_mess, Dest, &num_baia);
  //-----------------------------------------------------
  // Test chiamata RICHIESTA/ESPULSIONE attiva
  //-----------------------------------------------------
  if(num_mac>0 && num_mac<=TOTCHIAMATE){
     // Verifico se chiamata attiva
     ChON=ch[num_mac]->Staz[num_stz].Richiesta | ch[num_mac]->Staz[num_stz].Espulsione;
     if(ChON==false){
        ch[num_mac]->Staz[num_stz].TimeChiamata = time( NULL );
        ch[num_mac]->Staz[num_stz].NoCodice     = false;
        ch[num_mac]->Staz[num_stz].NoMagaz      = false;
     }
     //-----------------------------------------------------
     // Verifico se c'è già un TGV in ingombro alla macchina
     //-----------------------------------------------------
     AgvInIng =false;
     for(i=1; i<=MAXAGV; i++){
        if(!AGV[i]->test_mission_on()) continue;
        //----------------------------------------------
        // Ciclo per tutti i possibili NODI abbinati
        // alla macchina.
        //----------------------------------------------
        if(AGV[i]->stato.pos!=ch[num_mac]->Staz[num_stz].PuntoIng && AGV[i]->stato.pos!=ch[num_mac]->Staz[num_stz].PuntoOut) continue;
        AgvInIng=true;
        break;
     }
     if( AgvInIng ) ChON=false;
  }
  //-----------------------------------------------------
  // Test chiamata RICHIESTA/ESPULSIONE attiva
  //-----------------------------------------------------
  if(num_baia>0 && num_baia<=TOTBAIE){
     // Verifico se chiamata attiva
     ChON=ba[num_baia]->Chiamata;
     if(ChON==false){
        ba[num_baia]->TimeChiamata = time( NULL );
        ba[num_baia]->NoMagaz    = false;
        ba[num_baia]->NoTgvDisp  = false;
     }
     //-----------------------------------------------------
     // Verifico se c'è già un TGV in ingombro alla macchina
     //-----------------------------------------------------
     AgvInIng =false;
     for(i=1; i<=MAXAGV; i++){
        if(!AGV[i]->test_mission_on()) continue;
        //----------------------------------------------
        // Ciclo per tutti i possibili NODI abbinati
        // alla macchina.
        //----------------------------------------------
        if(AGV[i]->stato.pos!=ba[num_baia]->Punto) continue;
        AgvInIng=true;
        break;
     }
     if( AgvInIng ) ChON=false;
  }

  return(!ChON);
}

// --------------------------------------------------------
//   AssegnaPriorita()
// --------------------------------------------------------
//   Ritorna 1 se c'è errore
//
void process::AssegnaPriorita()
/********************************************/
{
  int  i, j;
  int  PrioritaStz;
  bool chiamata_ok;
  bool almeno_una_chiamata;
  long int TimeDiff;
  long int TimeAttuale;

  TimeAttuale=time(NULL);
  //---------------------------------------------------------------
  // MACCHINE
  // Azzermento priorità di tutte le locazioni prima del ricalcolo
  //---------------------------------------------------------------
  for(i=1; i<=MAXCHIAMATE; i++){
     if(ch[i]->Num<1           ) continue;
     if(ch[i]->Num>MAXCHIAMATE ) continue;
     ch[i]->Priorita=0;
  }

  //---------------------------------------------------------------
  // (1) - Priorità in base ai minuti di chiamata
  //---------------------------------------------------------------
  for(i=1; i<=MAXCHIAMATE; i++){
     if(ch[i]->Num<1           ) continue;
     if(ch[i]->Num>MAXCHIAMATE ) continue;
     if(ch[i]->Esclusione      ) continue;
     //if(ch[i]->Blocco        ) continue;
     if(ch[i]->Tipo==NN        ) continue;
     //-----------------------------------------------
     // Scorrimento di tutte le stazioni
     //-----------------------------------------------
     PrioritaStz=0;
     almeno_una_chiamata=false;
     for(j=0; j<MAXSTAZMAC; j++){
        if(ch[i]->Staz[j].PuntoIng<1         ) continue;
        if(ch[i]->Staz[j].PuntoIng>MAXPUNTI  ) continue;
        if(ch[i]->Staz[j].PuntoOut<1         ) continue;
        if(ch[i]->Staz[j].PuntoOut>MAXPUNTI  ) continue;
        if(ch[i]->Staz[j].Esclusione         ) continue;
        //-----------------------------------------------
        // Funzione di aggiornamento secondi di chiamata
        //-----------------------------------------------
        TestChiamata(ch[i]->Staz[j].PuntoIng, ch[i]->Num);
        TimeDiff = (TimeAttuale - ch[i]->Staz[j].TimeChiamata)/60;
        if( TimeDiff <0     ) TimeDiff=0L;
        if( TimeDiff >99    ) TimeDiff=99;
        PrioritaStz = (int)(PrioritaStz+TimeDiff);
        if( PrioritaStz <0  ) PrioritaStz=0L;
        if( PrioritaStz >99 ) PrioritaStz=99;
        //-----------------------------------------------
        // Se la stazione è in chiamata non posso
        // avere priorità "0".
        //-----------------------------------------------
        if(ch[i]->Staz[j].Richiesta || ch[i]->Staz[j].Espulsione){
           almeno_una_chiamata=true;
           //---------------------------------
           // Incremento con urgenza stazione
           //---------------------------------
           PrioritaStz = PrioritaStz + ch[i]->Staz[j].Urgenza*100;
        }
     }
     ch[i]->Priorita = (int)(ch[i]->Priorita + PrioritaStz);
     if(almeno_una_chiamata==true && ch[i]->Priorita<1) ch[i]->Priorita=1;
  }

  //---------------------------------------------------------------
  // BAIE
  // Azzermento priorità di tutte le locazioni prima del ricalcolo
  //---------------------------------------------------------------
  for(i=1; i<=MAXBAIE; i++){
     if(ba[i]->Num<1       ) continue;
     if(ba[i]->Num>MAXBAIE ) continue;
     ba[i]->Priorita=0;
  }

  //---------------------------------------------------------------
  // (1) - Priorità in base ai minuti di chiamata
  //---------------------------------------------------------------
  for(i=1; i<=MAXBAIE; i++){
     if(ba[i]->Num<1         ) continue;
     if(ba[i]->Num>MAXBAIE   ) continue;
     if(ba[i]->Punto<1       ) continue;
     if(ba[i]->Punto>TOTPUNTI) continue;
     if(ba[i]->Esclusione    ) continue;
     if(ba[i]->Tipo==NN      ) continue;
     //-----------------------------------------------
     // Funzione di aggiornamento secondi di chiamata
     //-----------------------------------------------
     TestChiamata(ba[i]->Punto, ba[i]->Num);
     TimeDiff = (TimeAttuale - ba[i]->TimeChiamata)/60;
     if( TimeDiff <0     ) TimeDiff=0L;
     if( TimeDiff >99    ) TimeDiff=99;
     PrioritaStz = (int)(TimeDiff);
     if( PrioritaStz <0  ) PrioritaStz=0;
     if( PrioritaStz >99 ) PrioritaStz=99;
     //-----------------------------------------------
     // Se la stazione è in chiamata non posso
     // avere priorità "0".
     //-----------------------------------------------
     if(ba[i]->Chiamata){
        //---------------------------------
        // Incremento con urgenza stazione
        //---------------------------------
        PrioritaStz = PrioritaStz + ba[i]->Urgenza*100;
        //---------------------------------
        // Aggiornamento priorità baia
        //---------------------------------
        ba[i]->Priorita = (int)(PrioritaStz);
        if(ba[i]->Priorita<1) ba[i]->Priorita=1;
     }
  }

  return;
}

// --------------------------------------------------------
//   CercaIDRisevatoMac()
// --------------------------------------------------------
//   Verifica se un'IDBOX compare tra quelli richiesti
//   da qualche macchina impianto
//
bool process::CercaIDRisevatoMac(short int ID, short int *num_mac)
/********************************************/
{
  short int i, macchina;

  *num_mac=0;
  //----------------------------------------------------
  // Verifica validità ID BOX
  //----------------------------------------------------
  if(ID<1              ) return FALSE;
  if(ID>MAXBOXIMPIANTO ) return FALSE;

  //----------------------------------------------------
  // Scroll di tutte le macchine per verificare se l'ID
  // compare tra quelli richiesti dalla macchina
  //----------------------------------------------------
  for(macchina=1; macchina<=MAXCHIAMATE; macchina++){
     if(ch[macchina]->Num<1           ) continue;
     if(ch[macchina]->Num>TOTCHIAMATE ) continue;
     for(i=0; i<MAXSTAZMAC*MAXPOSTISTAZ; i++){
        if(ch[macchina]->IDRic[i]<1             ) continue;
        if(ch[macchina]->IDRic[i]>MAXBOXIMPIANTO) continue;
        if(ch[macchina]->IDRic[i]!=ID           ) continue;
        *num_mac=macchina;
        return TRUE;
     }
  }
  return FALSE;
}

// --------------------------------------------------------
//   CercaIDInMac()
// --------------------------------------------------------
//   Verifica se un'IDBOX compare tra quelli depositati
//   in qualche macchina impianto
//
bool process::CercaIDInMac(short int ID, short int *num_mac, short int *num_stz, short int *num_pos)
/********************************************/
{
  short int i, j, macchina;

  *num_mac=0;
  *num_stz=0;
  *num_pos=0;
  //----------------------------------------------------
  // Verifica validità ID BOX
  //----------------------------------------------------
  if(ID<1              ) return FALSE;
  if(ID>MAXBOXIMPIANTO ) return FALSE;

  //----------------------------------------------------
  // Scroll di tutte le macchine per verificare se l'ID
  // compare tra quelli richiesti dalla macchina
  //----------------------------------------------------
  for(macchina=1; macchina<=MAXCHIAMATE; macchina++){
     if(ch[macchina]->Num<1           ) continue;
     if(ch[macchina]->Num>TOTCHIAMATE ) continue;
     for(i=0; i<MAXSTAZMAC; i++){
        if(ch[macchina]->Staz[i].PuntoIng<1         ) continue;
        if(ch[macchina]->Staz[i].PuntoIng>TOTPUNTI  ) continue;
        if(ch[macchina]->Staz[i].PuntoOut<1         ) continue;
        if(ch[macchina]->Staz[i].PuntoOut>TOTPUNTI  ) continue;
        for(j=0; j<MAXPOSTISTAZ; j++){
           if(ch[macchina]->Staz[i].Pos[j].PresCEST_B==LIBERA && ch[macchina]->Staz[i].Pos[j].PresCEST_A==LIBERA) continue;
           if(ch[macchina]->Staz[i].Pos[j].BoxB.ID!=ID && ch[macchina]->Staz[i].Pos[j].BoxA.ID!=ID              ) continue;
           *num_mac=macchina;
           *num_stz=i;
           *num_pos=j;
           return TRUE;
        }
     }
  }
  return FALSE;
}

// --------------------------------------------------------
//   GestioneChiamateMac_Ric()
// --------------------------------------------------------
//   Ritorna 1 se c'è errore
//
int process::GestioneChiamateMac_Ric(int macchina, char *all_mess)
/********************************************/
{
  return 0;
}

// --------------------------------------------------------
//   GestioneChiamateMac_Esp()
// --------------------------------------------------------
//   Ritorna 1 se c'è errore
//
int process::GestioneChiamateMac_Esp(int macchina, char *all_mess)
/********************************************/
{
  return 0;
}

// --------------------------------------------------------
//   GestioneChiamateBaie_Ric()
// --------------------------------------------------------
//   Ritorna 1 se c'è errore
//
int process::GestioneChiamateBaie_Ric(int baia, char *all_mess)
/********************************************/
{
  bool Ok;
  bool tgv_in_uscita;
  bool lato_ingresso;
  bool num_agv_in_reset_al_volo;
  int  chiamata;
  int  stato, i, j, k, z, err;
  short int Punto, PInt, Dest;
  short int new_dist, num_rot;
  short int grp_dest, grp_baia;
  short int tipo_carico, tipo_box;
  short int count_altro_agv;
  short int num_mag_accoppiato;
  short int num_mag1, num_pin1, num_pos1;
  short int num_agv, num_agv1, num_mag, num_pos;
  short int num_pin, typ_presa;
  short int altro_tgv, limitrofo;
  short int altro_tgv_blocco_rot;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  short int MinPercorsi, MinPercToStart, punto_arrivo;
  short int altro_num_mag_sorg, altro_num_pos_sorg;
  short int altro_num_mag_dest, altro_num_pos_dest;
  struct BAIE bb;
  struct MAG mag;

  memset(&bb,  0, sizeof(BAIE));
  //-----------------------------------------------------------------
  // Verifica dell'attendibilità dei dati di ricerca e compilazione
  // della struttura di appoggio.
  //-----------------------------------------------------------------
  stato = read_baie(baia, &bb, all_mess);
  if(stato) return 0;
  //-----------------------------------------------------------------
  // Se la macchina sconosciuta --> Skip!
  //-----------------------------------------------------------------
  if(bb.Tipo==BA_NONE     ) return 0;
  //-----------------------------------------------------------------
  // Pastazione non di deposito --> Skip!
  //-----------------------------------------------------------------
  if(bb.Tipo!=BA_DEPOSITO ) return 0;
  //-----------------------------------------------------------------
  // Se la macchina è esclusa/blocco --> Skip!
  //-----------------------------------------------------------------
  if(bb.Esclusione        ) return 0;
  //-----------------------------------------------------------------
  // KO - GESTIONE AVVICINAMENTO BAIE RICHIESTA
  // Se la macchina non in chiamata --> Skip!
  //-----------------------------------------------------------------
  if(AbilitaAvvicinamentoADest!=2 && AbilitaAvvicinamentoADest!=3){
     if(bb.Chiamata==false   ) return 0;
     if(bb.PresCEST_B!=LIBERA) return 0;
  }
  //-----------------------------------------------------------------
  // Inizializzazione variabili locali
  //-----------------------------------------------------------------
  PInt=0;
  Dest=0;
  Punto=0;
  num_agv=0;
  num_agv1=0;
  num_mag=0;
  num_pos=0;
  num_pin=0;
  chiamata=0;
  altro_tgv=0;
  limitrofo=0;
  typ_presa=N_FORCA_AB;
  num_agv_in_reset_al_volo=false;
  //-----------------------------------------------------------------
  // PARAMETRI SENSIBILI PER RICERCA BOX IN BASE AL TIPO MACCHINA
  // Selezione tipo box richiesto/espulso dalla macchina
  //-----------------------------------------------------------------
  // posizione di prelievo
  tipo_carico =LIBERA;
  switch(bb.TipoCod[0]){
     case ST_BOX_LAV1: tipo_carico = PRES_LAV1;
                       break;
     case ST_BOX_LAV2: tipo_carico = PRES_LAV2;
                       break;
     case ST_BOX_LAV3: tipo_carico = PRES_LAV3;
                       break;
     case ST_BOX_LAV4: tipo_carico = PRES_LAV4;
                       break;
     case ST_BOX_LAV5: tipo_carico = PRES_LAV5;
                       break;
     case ST_BOX_LAV6: tipo_carico = PRES_LAV6;
                       break;
     case ST_BOX_LAV7: tipo_carico = PRES_LAV7;
                       break;
     case ST_BOX_LAV8: tipo_carico = PRES_LAV8;
                       break;
     case ST_BOX_VUOTI:tipo_carico = PRES_VUOTO;
                       break;
  }
  //-----------------------------------------------------------------
  // Recupero i parametri di lavoro baia (POSIZIONE DI DEPOSITO)
  //-----------------------------------------------------------------
  tipo_box = bb.TipoBox[0];
  grp_dest = bb.GruppoDest;
  grp_baia = bb.Gruppo;
  //-----------------------------------------------------------------
  // Prelievo sempre dal lato di uscita
  //-----------------------------------------------------------------
  lato_ingresso = false;
  //-----------------------------------------------------------------
  // Recupero il nodo della baia
  //-----------------------------------------------------------------
  Punto=bb.Punto;
  //-----------------------------------------------------------------
  // Composizione chiamata
  //-----------------------------------------------------------------
  TestNumChiamata(&chiamata, Punto, bb.Num);
  if(chiamata==0) return 0;     // chiamata sconosciuta
  //-----------------------------------------------------------------
  // RICHIESTA VUOTO  (cerco AGV con vuoto a bordo)
  //-----------------------------------------------------------------
  if(tipo_carico==PRES_VUOTO){
     //-----------------------------------------------------------------
     // Cerco un TGV con box a bordo abbinato alla chiamata
     //-----------------------------------------------------------------
     if(CercaAgvConCaricoVUOTO(TRUE, Punto, chiamata, PROV_MAGAZZINO, tipo_carico, tipo_box, &num_agv, all_mess)) return 0;
     if(num_agv==0) if(CercaAgvConCaricoVUOTO(FALSE, Punto, chiamata, PROV_MAGAZZINO, tipo_carico, tipo_box, &num_agv, all_mess)) return 0;
  }
  //-----------------------------------------------------------------
  // RICHIESTA PIENO  (cerco AGV con pieno a bordo)
  //-----------------------------------------------------------------
  else{
     //-----------------------------------------------------------------
     // Cerco un TGV con box a bordo abbinato alla chiamata
     //-----------------------------------------------------------------
     if(CercaAgvConCaricoPIENO(TRUE, Punto, chiamata, PROV_MAGAZZINO, grp_baia, &num_agv, all_mess)) return 0;
     if(num_agv==0) if(CercaAgvConCaricoPIENO(FALSE, Punto, chiamata, PROV_MAGAZZINO, grp_baia, &num_agv, all_mess)) return 0;
  }
  //-----------------------------------------------------------------
  // SCARICO SULLA RULLIERA
  // Trovato TGV con carico lo indirizzo direttamente allo scarico
  // sulla rulliera
  //-----------------------------------------------------------------
  if(num_agv!=0){
     //-----------------------------------------------------------------
     // Magazzino/Baia di deposito trovati
     //-----------------------------------------------------------------
     ba[baia]->NoMagaz=false;
     //-----------------------------------------------------------------
     // Procedo a prelevare
     //-----------------------------------------------------------------
     PInt = 0;
     Dest = Punto;
     //-----------------------------------------------------------------
     // Verifico se la destinazione è impegnata da un altro TGV
     //-----------------------------------------------------------------
     if(AbilitaAvvicinamentoADest!=2 && AbilitaAvvicinamentoADest!=3){
        //-----------------------------------------------------------------
        // verifica se la chiamata è già asservita da un AGV e l'altro AGV
        // è in missione attiva.
        //-----------------------------------------------------------------
        altro_tgv = (short int)(test_chiamata_asservita(chiamata));
        if( altro_tgv!=0 && altro_tgv!=num_agv ) return 0;
        //-----------------------------------------------------------------
        // Nodo disponibile
        //-----------------------------------------------------------------
        if(P.test_dest_limitrofe(Dest, &altro_tgv, &limitrofo, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv) return 0;
        if(test_dest_riservata(Dest, &altro_tgv, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv) return 0;
        err = test_dest_occupata(Dest, &altro_tgv);
        if((err!=0) && (num_agv!=altro_tgv)) return 0;
     }
     //-----------------------------------------------------------------
     // ABILITA AVVICINAMENTO (AbilitaAvvicinamentoADest)
     // 1 - a baie prelievo
     // 2 - a baie deposito
     // 3 - a baie prelievo + deposito
     // Verifico se ci sono le condizioni per inizializzare la
     // missione fino a destinazione o se è possibile eseguire
     // un avvicinamento al nodo_fuori_ingombro più vicino alla
     // destinazione finale
     //-----------------------------------------------------------------
     else{
        AGV[num_agv]->AvvicinamentoADestinazione(FAST_PATH, Dest, 0, &PInt);
        if(PInt==0){
           AGV[num_agv]->AvvicinamentoADestinazione(STD_PATH, Dest, 0, &PInt);
           //-----------------------------------------------
           // Nodo di attesa per avvicinamento non trovato:
           // verifico che la destinazione sia raggiungibile
           //
           // IMPORTANTE: è necessario controllare anche
           // che la chiamata sia attiva perchè non viene
           // fatto a inizio funzione dando per scontato
           // l'avvicinamento
           //-----------------------------------------------
           if(PInt==0){
              //--------------------------------------------
              // verifica se la chiamata è già asservita
              //--------------------------------------------
              altro_tgv = (short int)(test_chiamata_asservita(chiamata));
              if( altro_tgv!=0 && altro_tgv!=num_agv ) return 0;
              //--------------------------------------------
              // Verifica chiamata attiva
              //--------------------------------------------
              if(bb.Chiamata==false   ) return 0;
              if(bb.PresCEST_B!=LIBERA) return 0;
              //--------------------------------------------
              // Verifica disponibilità NODO
              //--------------------------------------------
              if(P.test_dest_limitrofe(Dest, &altro_tgv, &limitrofo, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv) return 0;
              if(test_dest_riservata(Dest, &altro_tgv, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv) return 0;
              err = test_dest_occupata(Dest, &altro_tgv);
              if((err!=0) && (num_agv!=altro_tgv)) return 0;
           }
        }
     }
     //-----------------------------------------------------------------
     // Ammesso lo scarico cassone solo con la forca bassa
     //-----------------------------------------------------------------
     typ_presa = N_NO_FORCHE;
     //--------------------------------------------------
     // (1) GESTIONE FORCA DOPPIA  CON CARICO DOPPIO
     // (2) GESTIONE FORCA SINGOLA CON CARICO DOPPIO
     //--------------------------------------------------
     if(GestioneDoppiaForcaTGV!=2){
        // forca bassa piena + forca alta vuota = deposito BASSO;
        // forca bassa piena + forca alta pieno = deposito DOPPIO;
        if(AGV[num_agv]->stato.s.bit.carB==true  && AGV[num_agv]->mission.PresCEST_B!=LIBERA){
           typ_presa=N_FORCA_B;
           if(AGV[num_agv]->stato.s.bit.carA==true && AGV[num_agv]->mission.PresCEST_A!=LIBERA) typ_presa=N_FORCA_AB;
        }
        // forca bassa vuota + forca alta pieno = deposito ALTO
        if(AGV[num_agv]->stato.s.bit.carB==false && AGV[num_agv]->mission.PresCEST_B!=LIBERA){
           if(AGV[num_agv]->stato.s.bit.carA==true && AGV[num_agv]->mission.PresCEST_A!=LIBERA) typ_presa=N_FORCA_A;
        }
     }
     else{
        //----------------------------------------------------
        // Carico doppio su singola forca
        //----------------------------------------------------
        if(AGV[num_agv]->stato.s.bit.carB==true && (AGV[num_agv]->mission.PresCEST_B!=LIBERA || AGV[num_agv]->mission.PresCEST_A!=LIBERA)){
           typ_presa=N_FORCA_B;
           if(AGV[num_agv]->mission.PresCEST_B==LIBERA && AGV[num_agv]->mission.PresCEST_A!=LIBERA) typ_presa=N_FORCA_A;
           if(AGV[num_agv]->mission.PresCEST_B!=LIBERA && AGV[num_agv]->mission.PresCEST_A!=LIBERA) typ_presa=N_FORCA_AB;
        }
     }
     // combinazione forche non trovata
     if(typ_presa==N_NO_FORCHE) return 0;
     //-----------------------------------------------------------------
     // Inizializzazione missione verso linea
     //-----------------------------------------------------------------
     if(new_init_mission(FALSE, CALCOLA_TIPO_PATH, num_agv, chiamata, Dest, 0, PInt, MISS_SCARICO_SU_BAIA, typ_presa, &Ok, all_mess)) return 1;
     return 0;
  }
  //-----------------------------------------------------------------
  // GESTIONE AVVICINAMENTO BAIE RICHIESTA
  // Anche con la gestione avvicinamento attiva se la baia ha il
  // tutto pieno non devo partire con altri tgv a caricare da mag
  //-----------------------------------------------------------------
  if(AbilitaAvvicinamentoADest==2 || AbilitaAvvicinamentoADest==3){
     if(bb.TuttoPieno==true) return 0;
  }
  //-----------------------------------------------------------------
  // RICERCA TGV LIBERO PER PRELIEVO IN MAGAZZINO
  // Cerco AGV libero disponibile ad asservire la chiamata
  // CERCO UN TGV GIA' ABBINATO ALLA CHIAMATA
  //
  // Nota: cerco un TGV completamente libero se non c'è nessuna
  //       navetta abbinata alla chiamata (nemmeno con la missione
  //       in corso)
  //-----------------------------------------------------------------
  if(GestioneDoppiaForcaTGV!=0){
     if(CercaAgvLibero(FALSE, Punto, chiamata, CH_RICHIESTA, ZONA_0_ALL, &num_agv, all_mess)) return 0;
  }
  if(num_agv==0){
     //-----------------------------------------------------------------
     // RICERCA DI UN TGV COMPLETAMENTE LIBERO
     // Senza niente a bordo sulle forche
     //-----------------------------------------------------------------
     if(CercaAgvLibero(TRUE, Punto, chiamata, CH_RICHIESTA, ZONA_0_ALL, &num_agv, all_mess)) return 0;
     //-----------------------------------------------------------------
     // CAMBIO MISSIONI AL VOLO (Solo per navette COMPLETAMENTE LIBERE)
     // Andrea L. 16/04/2018
     // Essendo che nella funzione viene asseganata anche la chiamata,
     // Cerco il TGV libero per reset solo se Baia non in Blocco + Aut.
     // + NoMagaz=false
     //-----------------------------------------------------------------
     if(num_agv==0 ){
        if(bb.Blocco==false && bb.Automatico==true && bb.NoMagaz==false && bb.Chiamata==true){
           if(CercaAgvLiberoPerReset(TRUE, Punto, chiamata, CH_RICHIESTA, ZONA_0_ALL, &num_agv, all_mess)) return 0;
           if(num_agv!=0) num_agv_in_reset_al_volo=true;
        }
     }
  }
  //-----------------------------------------------------------------
  // Nessun TGV trovato
  //-----------------------------------------------------------------
  if(num_agv==0) return 0;
  //-----------------------------------------------------------------
  // GESTIONE MACCHINA IN BLOCCO
  // Valida solo per i TGV liberi
  //-----------------------------------------------------------------
  if(AGV[num_agv]->mission.Chiamata==0){
     //-----------------------------------------------------------------
     // Se baia in blocco/manuale --> Skip!
     //-----------------------------------------------------------------
     if(bb.Blocco==true      ) return 0;
     if(bb.Automatico==false ) return 0;
  }
  //-----------------------------------------------------------------
  // KO - GESTIONE AVVICINAMENTO BAIE RICHIESTA
  // verifica se la chiamata è già asservita da un AGV
  //-----------------------------------------------------------------
  if(AbilitaAvvicinamentoADest!=2 && AbilitaAvvicinamentoADest!=3){
     altro_tgv = (short int)(test_chiamata_asservita(chiamata));
     if( altro_tgv!=0 && altro_tgv!=num_agv ) return 0;
  }
  //-----------------------------------------------------------------
  // OK - GESTIONE AVVICINAMENTO BAIE RICHIESTA
  // Controllo numero massimo TGV in asservimento chiamata
  //-----------------------------------------------------------------
  else{
     count_altro_agv=0;
     for(i=1; i<=MAXAGV; i++){
        if(i==num_agv                          ) continue;
        if(AGV[i]->mission.Esclusione          ) continue;
        if(AGV[i]->mission.SemiAutomatico      ) continue;
        if(AGV[i]->mission.Chiamata!=chiamata  ) continue;
        count_altro_agv++;
     }
     if(count_altro_agv>=MAX_AGV_IN_AVVICINAMENTO) return 0;
  }
  //-----------------------------------------------------------------
  // GESTIONE DOPPIA FORCA
  // Recupero il TIPO DI PRESA che il TGV è in grado di fare
  // E' previsto il carico del cestone vuoto con la forca bassa
  //-----------------------------------------------------------------
  typ_presa = N_NO_FORCHE;
  if(GestioneDoppiaForcaTGV!=1) typ_presa=N_FORCA_B;
  else{
     if(AGV[num_agv]->stato.s.bit.carB==false && AGV[num_agv]->stato.s.bit.carA==true  ) typ_presa=N_FORCA_B;
     if(AGV[num_agv]->stato.s.bit.carB==false && AGV[num_agv]->stato.s.bit.carA==false ) typ_presa=N_FORCA_AB;
  }
  if(typ_presa==N_NO_FORCHE) return 0;
  //-----------------------------------------------------------------
  // (1) CARICO DA MAGAZZINO
  //
  // Nella ricerce dei vuoti mi predispondo all'accesso invertito
  // in mazzino
  //-----------------------------------------------------------------
  //
  // === VUOTI ===
  //
  if(tipo_carico==PRES_VUOTO){
     if(M.CercaMagPrelievoVUOTO(TRUE, 0, AbilitaControlloNodoDest, lato_ingresso, grp_dest, &num_mag, &num_pos, &num_pin, num_agv, typ_presa, tipo_carico, tipo_box, all_mess)) return 0;
     if(num_mag==0) if(M.CercaMagPrelievoVUOTO(FALSE, 0, AbilitaControlloNodoDest, grp_dest, lato_ingresso, &num_mag, &num_pos, &num_pin, num_agv, typ_presa, tipo_carico, tipo_box, all_mess)) return 0;
     //-----------------------------------------------------------------
     // Se ho la doppia forca e non ha avuto esito la ricerca con
     // doppia presa, eseguo la ricerca a presa solo FORCA BASSA
     //-----------------------------------------------------------------
     if(GestioneDoppiaForcaTGV==1){
        if(num_mag==0 && typ_presa==N_FORCA_AB){
           typ_presa = N_FORCA_B;
           if(M.CercaMagPrelievoVUOTO(TRUE, 0, AbilitaControlloNodoDest, lato_ingresso, grp_dest, &num_mag, &num_pos, &num_pin, num_agv, typ_presa, tipo_carico, tipo_box, all_mess)) return 0;
           if(num_mag==0) if(M.CercaMagPrelievoVUOTO(FALSE, 0, AbilitaControlloNodoDest, lato_ingresso, grp_dest, &num_mag, &num_pos, &num_pin, num_agv, typ_presa, tipo_carico, tipo_box, all_mess)) return 0;
        }
     }
     //-----------------------------------------------------------------
     // 2. GESTIONE MAGAZZINI ACCOPPIATI
     // se trovo un magazzino verifico se è accoppiato ad un altro e
     // in caso che lo sia destino il TGV al magazzino che ha più
     // posizioni a terra impegnate per motivi di SICUREZZA
     //-----------------------------------------------------------------
     if(num_mag!=0){
        //-------------------------------------------------
        // Cerco magazzino accoppiato in struttura
        //-------------------------------------------------
        num_mag_accoppiato=0;
        for(i=0;;i++){
           if(AccoppiaMagaz[i][0]==0 && AccoppiaMagaz[i][1]==0            ) break;
           if(AccoppiaMagaz[i][0]!=num_mag && AccoppiaMagaz[i][1]!=num_mag) continue;
           if(AccoppiaMagaz[i][0]==num_mag){num_mag_accoppiato=AccoppiaMagaz[i][1]; break;}
           if(AccoppiaMagaz[i][1]==num_mag){num_mag_accoppiato=AccoppiaMagaz[i][0]; break;}
        }
        //-------------------------------------------------
        // Verifico se magazzino accoppiato è disponibile
        //-------------------------------------------------
        if(num_mag_accoppiato>0 && num_mag_accoppiato<=TOTMAG){
           num_mag1=num_pin1=num_pos1=0;
           if(M.CercaMagPrelievoVUOTO(FALSE, num_mag_accoppiato, AbilitaControlloNodoDest, lato_ingresso, grp_dest, &num_mag1, &num_pos1, &num_pin1, num_agv, typ_presa, tipo_carico, tipo_box, all_mess)) return 0;
           //-----------------------------------------------
           // Se magazzino accoppiato è disponibile scelgo
           // tra i due in coppia quello con meno postazioni
           // a terra impegnate
           //-----------------------------------------------
           num_mag_accoppiato=0;
           if(num_mag1!=0){
              if(num_pos1<num_pos || (num_pos1==num_pos && num_pin1<num_pin)){
                 num_mag=num_mag1;
                 num_pos=num_pos1;
                 num_pin=num_pin1;
                 num_mag_accoppiato=num_mag1;
              }
           }
        }
     }
  }
  //
  // === PIENI ===
  //
  else{
     if(M.CercaMagPrelievoPIENO(TRUE, 0, AbilitaControlloNodoDest, lato_ingresso, grp_dest, &num_mag, &num_pos, &num_pin, num_agv, typ_presa, grp_baia, all_mess)) return 0;
     if(num_mag==0) if(M.CercaMagPrelievoPIENO(FALSE, 0, AbilitaControlloNodoDest, lato_ingresso, grp_dest, &num_mag, &num_pos, &num_pin, num_agv, typ_presa, grp_baia, all_mess)) return 0;
     //-----------------------------------------------------------------
     // Se ho la doppia forca e non ha avuto esito la ricerca con
     // doppia presa, eseguo la ricerca a presa solo FORCA BASSA
     //-----------------------------------------------------------------
     if(GestioneDoppiaForcaTGV==1){
        if(num_mag==0 && typ_presa==N_FORCA_AB){
           typ_presa = N_FORCA_B;
           if(M.CercaMagPrelievoPIENO(TRUE, 0, AbilitaControlloNodoDest, lato_ingresso, grp_dest, &num_mag, &num_pos, &num_pin, num_agv, typ_presa, grp_baia, all_mess)) return 0;
           if(num_mag==0) if(M.CercaMagPrelievoPIENO(FALSE, 0, AbilitaControlloNodoDest, lato_ingresso, grp_dest, &num_mag, &num_pos, &num_pin, num_agv, typ_presa, grp_baia, all_mess)) return 0;
        }
     }
     //-----------------------------------------------------------------
     // 2. GESTIONE MAGAZZINI ACCOPPIATI
     // se trovo un magazzino verifico se è accoppiato ad un altro e
     // in caso che lo sia destino il TGV al magazzino che ha più
     // posizioni a terra impegnate per motivi di SICUREZZA
     //-----------------------------------------------------------------
     if(num_mag!=0){
        //-------------------------------------------------
        // Cerco magazzino accoppiato in struttura
        //-------------------------------------------------
        num_mag_accoppiato=0;
        for(i=0;;i++){
           if(AccoppiaMagaz[i][0]==0 && AccoppiaMagaz[i][1]==0            ) break;
           if(AccoppiaMagaz[i][0]!=num_mag && AccoppiaMagaz[i][1]!=num_mag) continue;
           if(AccoppiaMagaz[i][0]==num_mag){num_mag_accoppiato=AccoppiaMagaz[i][1]; break;}
           if(AccoppiaMagaz[i][1]==num_mag){num_mag_accoppiato=AccoppiaMagaz[i][0]; break;}
        }
        //-------------------------------------------------
        // Verifico se magazzino accoppiato è disponibile
        //-------------------------------------------------
        if(num_mag_accoppiato>0 && num_mag_accoppiato<=TOTMAG){
           num_mag1=num_pin1=num_pos1=0;
           if(M.CercaMagPrelievoPIENO(FALSE, num_mag_accoppiato, AbilitaControlloNodoDest, lato_ingresso, grp_dest, &num_mag, &num_pos, &num_pin, num_agv, typ_presa, grp_baia, all_mess)) return 0;
           //-----------------------------------------------
           // Se magazzino accoppiato è disponibile scelgo
           // tra i due in coppia quello con meno postazioni
           // a terra impegnate
           //-----------------------------------------------
           num_mag_accoppiato=0;
           if(num_mag1!=0){
              if(num_pos1<num_pos || (num_pos1==num_pos && num_pin1<num_pin)){
                 num_mag=num_mag1;
                 num_pos=num_pos1;
                 num_pin=num_pin1;
                 num_mag_accoppiato=num_mag1;
              }
           }
        }
     }
  }
  //----------------------------------------------------
  // Magazzino non trovato e chiamata non asservita
  //----------------------------------------------------
  if(num_mag==0){
     altro_tgv = (short int)(test_chiamata_asservita(chiamata));
     if( altro_tgv==0 ) ba[baia]->NoMagaz=true;
     return 0;
  }
  //----------------------------------------------------
  // Magazzino trovato
  //----------------------------------------------------
  ba[baia]->NoMagaz=false;
  //----------------------------------------------------
  // DESTINAZIONE MAGAZZINO
  // Recupero dati magazzini selezionato
  //----------------------------------------------------
  if(num_mag!=0){
     memset(&mag, 0, sizeof(MAG));
     if(M.read_mag(num_mag, &mag, all_mess)) return 1;
     //-----------------------------------------------------------------
     // Procedo a prelevare
     //
     // N.B.: Se il TGV non si trova più sulla posizione falde del
     //       decestonatore da asservire, eseguo il prelievo dai
     //       magazzini abbinati entrando dal lato del deposito.
     //-----------------------------------------------------------------
     PInt = 0;
     if(lato_ingresso==false) Dest=mag.PuntoOut;
     if(lato_ingresso==true ) Dest=mag.PuntoIng;
     //-----------------------------------------------------------------
     // RICERCA NUOVAMENTE TGV LIBERO PER PRELIEVO IN MAGAZZINO
     // Questa volta, la ricerca è fatta in base alla vicinanza al
     // punto di prelievo
     //
     // NOTA: Solo se il TGV trovato precedentemente è completamente
     //       liberto
     //-----------------------------------------------------------------
     if(num_agv_in_reset_al_volo==false){
        if(AGV[num_agv]->mission.Chiamata==0 && AGV[num_agv]->mission.PresCEST_A==LIBERO && AGV[num_agv]->mission.PresCEST_B==LIBERO){
           num_agv1=0;
           //-----------------------------------------------------------------
           // RICERCA DI UN TGV COMPLETAMENTE LIBERO
           // Senza niente a bordo sulle forche
           //-----------------------------------------------------------------
           if(CercaAgvLibero(TRUE, Dest, chiamata, CH_RICHIESTA, ZONA_0_ALL, &num_agv1, all_mess)) return 0;
           if(num_agv1!=0 && num_agv1!=num_agv) num_agv=num_agv1;
        }
     }
     //-----------------------------------------------------------------
     // AGV IN CAMBIO MISSIONE AL VOLO (AL-07/01/2020)
     // Verifico se il TGV cacolato per il cambio missione al volo
     // ha il percorso libero per la destinazione reale (magazzino)
     // altrimenti lo libero dalla programmazione di cambio al volo
     //-----------------------------------------------------------------
     else{
        //------------------------------------------------------------------
        // Verifico rotaz. su PRIMO NODO NUOVA MISSIONE
        //------------------------------------------------------------------
        memset(&nodi_perc, 0, sizeof(nodi_perc));
        memset(&dati_perc, 0, sizeof(dati_perc));
        err = AGV[num_agv]->distanza(FAST_PATH, AGV[num_agv]->mission.pintend, Dest, &nodi_perc[0], &dati_perc[0], all_mess, &new_dist, &num_rot);
        if(!err && new_dist<=MAXPERCORSI){
           //---------------------------------------------
           // Valuto se esiste il percorso disponibile
           //---------------------------------------------
           MinPercorsi=MinPercToStart=punto_arrivo=0;
           AGV[num_agv]->test_percorso_disponibile(&nodi_perc[0], &dati_perc[0], &altro_tgv, &MinPercorsi, &MinPercToStart, &punto_arrivo, all_mess);
           if(MinPercorsi<MinPercToStart){
              AGV[num_agv]->mission.PIntForzato = 0;
              AGV[num_agv]->mission.Chiamata    = 0;
              AGV[num_agv]->mission.ResetAlVolo = false;
              //------------------------------------------------------------------
              // file traccia
              //------------------------------------------------------------------
              sprintf(all_mess, "(6) - TGV%02d ANNULLA CAMBIO MISSIONE AL VOLO", num_agv);
              file_cronologico_generico("LOGRESET.TXT", all_mess, 50000L);
              return 0;
           }
        }
     }
     //-----------------------------------------------------------------
     // NO CONTROLLO OCCUPAZIONE NODO
     // in caso di chiamata da baia di deposito se è stato disattivato
     // il controllo nella ricerca del magazzino di prelievo, devo
     // farlo prima di inizializzare la missione di carico
     //-----------------------------------------------------------------
     if(AbilitaControlloNodoDest==false){
        //-----------------------------------------------------------------
        // Gestione NODO CONTRAPPOSTO
        //-----------------------------------------------------------------
        if(N.nodo_contrapposto[Dest]!=0 && N.nodo_busy[N.nodo_contrapposto[Dest]] && N.nodo_busy[N.nodo_contrapposto[Dest]]!=num_agv) return 0;
        //-----------------------------------------------------------------
        // Recupero la distanza dalla destinazione
        //-----------------------------------------------------------------
        new_dist=num_rot=0;
        memset(&nodi_perc[0], 0, sizeof(nodi_perc));
        memset(&dati_perc[0], 0, sizeof(dati_perc));
        err = AGV[num_agv]->distanza(FAST_PATH, 0, Dest, &nodi_perc[0], &dati_perc[0], all_mess, &new_dist, &num_rot);
        if(err || new_dist>MAXPERCORSI) return 0;
        //-----------------------------------------------------------------
        // GESTIONE BLOCCHI ROTAZIONE
        // Scarto i magazzini interbloccati da altri
        //-----------------------------------------------------------------
        altro_tgv_blocco_rot=0;
        for(z=1; z<MAXPERCORSI; z++){
           if(nodi_perc[z+1]==0) break;
           //---------------------------------------------------------------
           // scarto il punti dove il TGV non deve compiere rotazioni
           //---------------------------------------------------------------
           if(dati_perc[z-1].rot==dati_perc[z].rot) continue;
           //---------------------------------------------------------------
           // scorro la struttura di blocco rotazioni per capire se il
           // punto di rotazione è in ingobro a qualche nodo riservato
           //---------------------------------------------------------------
           for(j=0;;j++){
              if(BloccoRot[j][0]==0) break;
              //-----------------------------------------------------
              // Scarto le righe che non hanno il punto di rotazione
              // come ingombrante
              //-----------------------------------------------------
              for(k=1; k<3; k++){
                 if(BloccoRot[j][k]==0            ) break;
                 if(nodi_perc[z]!=BloccoRot[j][k] ) continue;
                 //-----------------------------------------------------
                 // Se il nodo di rotazione compare come bloccante
                 // verifico se il rispettivo punto in ingobro è
                 // impegnato
                 //-----------------------------------------------------
                 if(P.test_dest_riservata((short int)(BloccoRot[j][0]), &altro_tgv, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv){
                    altro_tgv_blocco_rot=altro_tgv;
                 }
                 //-----------------------------------------------------
                 // Verifico se il nodo di blocco è occupato da un TGV
                 //-----------------------------------------------------
                 if(P.test_dest_occupata((short int)(BloccoRot[j][0]), &altro_tgv) && altro_tgv!=0 && altro_tgv!=num_agv){
                    altro_tgv_blocco_rot=altro_tgv;
                 }
                 //-----------------------------------------------------
                 // Punto di rotazione bloccante --> skip!
                 //-----------------------------------------------------
                 if(altro_tgv_blocco_rot!=0) break;
              }
              if(altro_tgv_blocco_rot!=0) break;
           }
           if(altro_tgv_blocco_rot!=0) break;
        }
        //------------------------------------------------------------
        // Percorso verso magazzino in blocco rotazione --> skip!
        //------------------------------------------------------------
        if(altro_tgv_blocco_rot!=0) return 0;
        //-----------------------------------------------------------
        // Controllo se magazzino già impeganto da altro TGV
        //-----------------------------------------------------------
        if(P.test_dest_occupata(Dest, &altro_tgv) && altro_tgv!=0 && altro_tgv!=num_agv){
           //-------------------------------------------------------------
           // Se l'altro TGV è in uscita dal magazzino, il magazzino non
           // è da considerare occupato.
           //-------------------------------------------------------------
           tgv_in_uscita=false;
           altro_num_mag_sorg=altro_num_pos_sorg=0;
           M.test_punto_presente_mag(all_mess, AGV[altro_tgv]->mission.pstart, &altro_num_mag_sorg, &altro_num_pos_sorg);
           altro_num_mag_dest=altro_num_pos_dest=0;
           M.test_punto_presente_mag(all_mess, AGV[altro_tgv]->mission.pend, &altro_num_mag_dest, &altro_num_pos_dest);
           //-------------------------------------------------------
           // ALTRO AGV con partenza magazzino destinazione del
           // TGV considerato, e destinazione differente
           //-------------------------------------------------------
           if(altro_num_mag_sorg==mag.Num && altro_num_mag_dest!=mag.Num) tgv_in_uscita=true;
           if( tgv_in_uscita==false ) return 0;
        }
        //-----------------------------------------------------------
        // Destinazione riservata da altro TGV
        //-----------------------------------------------------------
        if(P.test_dest_riservata(Dest, &altro_tgv, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv){
           //-------------------------------------------------------------
           // Se l'altro TGV è in uscita dal magazzino, il magazzino non
           // è da considerare occupato.
           //-------------------------------------------------------------
           tgv_in_uscita=false;
           altro_num_mag_sorg=altro_num_pos_sorg=0;
           M.test_punto_presente_mag(all_mess, AGV[altro_tgv]->mission.pstart, &altro_num_mag_sorg, &altro_num_pos_sorg);
           altro_num_mag_dest=altro_num_pos_dest=0;
           M.test_punto_presente_mag(all_mess, AGV[altro_tgv]->mission.pend, &altro_num_mag_dest, &altro_num_pos_dest);
           //-------------------------------------------------------
           // ALTRO AGV con partenza magazzino destinazione del
           // TGV considerato, e destinazione differente
           //-------------------------------------------------------
           if(altro_num_mag_sorg==mag.Num && altro_num_mag_dest!=mag.Num) tgv_in_uscita=true;
           if( tgv_in_uscita==false ) return 0;
        }
        //-----------------------------------------------------------
        // TGV su nodo limitrofo
        //-----------------------------------------------------------
        if(P.test_dest_limitrofe(Dest, &altro_tgv, &limitrofo, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv ) return 0;
     }
     else{
        //-----------------------------------------------------------------
        // Verifico se la destinazione è impegnata da un altro TGV
        //-----------------------------------------------------------------
        if(P.test_dest_limitrofe(Dest, &altro_tgv, &limitrofo, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv) return 0;
        if(test_dest_riservata(Dest, &altro_tgv, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv) return 0;
        err = test_dest_occupata(Dest, &altro_tgv);
        if((err!=0) && (num_agv!=altro_tgv)) return 0;
     }
     //----------------------------------------------
     // Inizializzazione della missione
     //----------------------------------------------
     if(new_init_mission(FALSE, CALCOLA_TIPO_PATH, num_agv, chiamata, Dest, num_pos, PInt, MISS_CARICO_DA_MAGAZZINO, typ_presa, &Ok, all_mess)) return 1;
  }
  return 0;
}

// --------------------------------------------------------
//   GestioneChiamateBaie_Esp()
// --------------------------------------------------------
//   Ritorna 1 se c'è errore
//
int process::GestioneChiamateBaie_Esp(int baia, char *all_mess)
/********************************************/
{
  bool Ok;
  bool udc_trovato;
  bool lato_uscita;
  int  lotto;
  int  stato, err;
  int  posti, i, j, k;
  int  provenienza;
  int  chiamata, chiamata1, chiamata2;
  int  piani = PIANIINPRESA;
  int  SovrapCarWash = SOVRAPPOSIZIONE_CARWASH;
  short int PInt, Dest;
  bool codice_gia_in_movimento;
  bool deposito_nel_piu_vicino = (bool)(DEPOSITO_NEL_PIU_VICINO);
  short int Staz, Punto, num_mag_accoppiato;
  bool  sovrap_baia, disponib_sovrap_mag, presa_sovrapponibile;
  bool  basso, sovrap, rotaz, basso1, sovrap1, rotaz1, basso2, sovrap2, rotaz2;
  short int tipo_carico, tipo_box, codice, car_wash;
  short int tipo_carico1, tipo_box1, codice1, car_wash1;
  short int tipo_box2, codice2, car_wash2, grp_dest;
  int lotto1, lotto2;
  short int num_agv, typ_presa, altro_agv, altro_tgv2, limitrofo;
  short int num_mag, num_pin, num_pos;
  short int num_mag1, num_pin1, num_pos1;
  bool presa[POSINPRESA+1][PIANIINPRESA+1];
  bool presa1[POSINPRESA+1][PIANIINPRESA+1];
  struct BAIE bb;
  struct DATI_PRD Udc;

  memset(&bb,  0, sizeof(BAIE));
  //-----------------------------------------------------------------
  // Verifica dell'attendibilità dei dati di ricerca e compilazione
  // della struttura di appoggio.
  //-----------------------------------------------------------------
  stato = read_baie(baia, &bb, all_mess);
  if(stato) return 0;
  //-----------------------------------------------------------------
  // Se la macchina sconosciuta --> Skip!
  //-----------------------------------------------------------------
  if(bb.Tipo==BA_NONE     ) return 0;
  //-----------------------------------------------------------------
  // Pastazione non di prelievo --> Skip!
  //-----------------------------------------------------------------
  if(bb.Tipo!=BA_PRELIEVO ) return 0;
  //-----------------------------------------------------------------
  // Se la macchina è esclusa/blocco --> Skip!
  //-----------------------------------------------------------------
  if(bb.Esclusione        ) return 0;
  //-----------------------------------------------------------------
  // Se la macchina non in chiamata --> Skip!
  //-----------------------------------------------------------------
  if(bb.Chiamata==false   ) return 0;
  //-----------------------------------------------------------------
  // Se la macchina in blocco --> Skip!
  //-----------------------------------------------------------------
  if(bb.Blocco==true      ) return 0;
  if(bb.Automatico==false ) return 0;
  //-----------------------------------------------------------------
  // Nessuna presenza in presa --> skip!
  //-----------------------------------------------------------------
  if(bb.PresCEST_B==LIBERA) return 0;
  //-----------------------------------------------------------------
  // Inizializzazione variabili locali
  //-----------------------------------------------------------------
  PInt=0;
  Dest=0;
  Punto=0;
  num_agv=0;
  num_mag=0;
  num_pin=0;
  num_pos=0;
  chiamata=0;
  altro_agv=0;
  limitrofo=0;
  posti       = Baie[bb.Num][1];          // Massimo numero cestoni
  typ_presa   = N_FORCA_AB;
  provenienza = (short int)((10000*PROV_BAIE)+baia);
  memset(&presa[0][0],  false, sizeof(presa));
  memset(&presa1[0][0], false, sizeof(presa1));
  basso=sovrap=rotaz=basso1=sovrap1=rotaz1=basso2=sovrap2=rotaz2=false;
  tipo_box=codice=lotto=tipo_box1=codice1=lotto1=tipo_box2=codice2=lotto2=0;
  //-----------------------------------------------------------------
  // SICUREZZA = inizializzazione "car_wash" a "1"
  //-----------------------------------------------------------------
  car_wash=car_wash1=car_wash2=1;
  //-----------------------------------------------------------------
  // PARAMETRI SENSIBILI PER RICERCA BOX IN BASE AL TIPO MACCHINA
  // Selezione tipo box richiesto/espulso dalla macchina
  //-----------------------------------------------------------------
  // posizione di prelievo
  tipo_carico =LIBERA;
  switch(bb.TipoCod[0]){
     case ST_BOX_LAV1: tipo_carico = PRES_LAV1;
                       break;
     case ST_BOX_LAV2: tipo_carico = PRES_LAV2;
                       break;
     case ST_BOX_LAV3: tipo_carico = PRES_LAV3;
                       break;
     case ST_BOX_LAV4: tipo_carico = PRES_LAV4;
                       break;
     case ST_BOX_LAV5: tipo_carico = PRES_LAV5;
                       break;
     case ST_BOX_LAV6: tipo_carico = PRES_LAV6;
                       break;
     case ST_BOX_LAV7: tipo_carico = PRES_LAV7;
                       break;
     case ST_BOX_LAV8: tipo_carico = PRES_LAV8;
                       break;
     case ST_BOX_VUOTI:tipo_carico = PRES_VUOTO;
                       break;
              default: tipo_carico = PRES_VUOTO;
                       break;
  }
  tipo_box = bb.TipoBox[0];
  car_wash = bb.CarWash[0];
  codice   = bb.Codice[0];
  lotto    = bb.Lotto[0];
  basso    = bb.Basso[0];
  rotaz    = bb.Ruotato180[0];
  sovrap   = bb.Sovrap[0];
  grp_dest = bb.GruppoDest;
  memcpy(&presa[0][0], &bb.Presa[0][0][0], sizeof(presa));
  // posizione successiva in rulliera
  tipo_carico1=LIBERA;
  switch(bb.TipoCod[1]){
     case ST_BOX_LAV1: tipo_carico1 = PRES_LAV1;
                       break;
     case ST_BOX_LAV2: tipo_carico1 = PRES_LAV2;
                       break;
     case ST_BOX_LAV3: tipo_carico1 = PRES_LAV3;
                       break;
     case ST_BOX_LAV4: tipo_carico1 = PRES_LAV4;
                       break;
     case ST_BOX_LAV5: tipo_carico1 = PRES_LAV5;
                       break;
     case ST_BOX_LAV6: tipo_carico1 = PRES_LAV6;
                       break;
     case ST_BOX_LAV7: tipo_carico1 = PRES_LAV7;
                       break;
     case ST_BOX_LAV8: tipo_carico1 = PRES_LAV8;
                       break;
     case ST_BOX_VUOTI:tipo_carico1 = PRES_VUOTO;
                       break;
              default: tipo_carico1 = PRES_VUOTO;
                       break;
  }
  // SECONDA POSIZIONE
  if(posti>1){
     tipo_box1 = bb.TipoBox[1];
     car_wash1 = bb.CarWash[1];
     codice1   = bb.Codice[1];
     lotto1    = bb.Lotto[1];
     basso1    = bb.Basso[1];
     rotaz1    = bb.Ruotato180[1];
     sovrap1   = bb.Sovrap[1];
     memcpy(&presa1[0][0], &bb.Presa[1][0][0], sizeof(presa));
  }
  // TERZA POSIZIONE
  if(posti>2){
     tipo_box2 = bb.TipoBox[2];
     car_wash2 = bb.CarWash[2];
     codice2   = bb.Codice[2];
     lotto2    = bb.Lotto[2];
     basso2    = bb.Basso[2];
     rotaz2    = bb.Ruotato180[2];
     sovrap2   = bb.Sovrap[2];
  }
  //-----------------------------------------------------------------
  // Verifica se ci sono le condizioni di sovrapposizione in prelievo
  // 0. Se il sistema non è predisposto per sapere lo stato della 2°
  //    posizione, sicuramente non è possibile la sovrapposizione in
  //    baia.
  // 1. IN PRELIEVO LE PALETTE NON DEVONO GIA' ESSERE SOVRAPPOSTE
  // 2. nessuna paletta in seconda posizione
  // 3. entrambe le palette con la stessa linea di provenienza.
  // 4. entrambe le palette dello stesso tipo.
  // 5. entrambe le palette basse.
  // 6. entrambe le palette con il bit di sovrapposizione
  // 7. controllo struttura presa
  //-----------------------------------------------------------------
  if(posti<2) sovrap_baia=false;
  else{   // sovrapposizione in baia solo se conosco dati 2° posizione baia
     sovrap_baia=true;
     if(bb.PresCEST_A!=LIBERA          ) sovrap_baia=false;
     if(tipo_carico1==LIBERA           ) sovrap_baia=false;
     if(codice!=codice1                ) sovrap_baia=false;
     if(lotto!=lotto1                  ) sovrap_baia=false;
     if(car_wash!=car_wash1            ) sovrap_baia=false;
     if(rotaz!=rotaz1                  ) sovrap_baia=false;
     if(tipo_box!=tipo_box1            ) sovrap_baia=false;
     if(basso==false || basso1==false  ) sovrap_baia=false;
     if(sovrap==false || sovrap1==false) sovrap_baia=false;
     //--------------------------------------------------
     // Nel controllo della struttura della presa
     // verifico che a ogni paletta a terra in 1° pos.,
     // corrisponda la stessa paletta in 2° pos.
     //
     // NOTE:
     // 1. La 2°presa risulta quella che finirà "sotto"
     //    a seguito della sovrapposizione
     // 2. Basta che sia assente un pallet nella 2° presa
     //    e presente il pallet nella stessa poso della
     //    1° presa per rendere disattivare la
     //    sovrapposizione in baia.
     // 4. E' necessario verificare solo il piano basso
     //    di ogni presa.
     // 5. Se una delle 2 prese ha già una paletta
     //    al 2° piano non è sovrapponibile in rulliera
     //--------------------------------------------------
     for(i=0; i<POSINPRESA; i++){
        if(sovrap_baia==false) break;
        //------------------------------------------------
        // (4) - struttura a terra sovrapponibile
        //------------------------------------------------
        if(presa1[i][0]==false && presa[i][0]==true) sovrap_baia=false;
        //------------------------------------------------
        // (5) - piano alto già presente
        //------------------------------------------------
        if(piani>1 && sovrap_baia==true){
           for(j=1; j<piani; j++){
              if(presa [i][j]==true){sovrap_baia=false; break;}
              if(presa1[i][j]==true){sovrap_baia=false; break;}
           }
        }
     }
  }
  //-----------------------------------------------------------------
  // RULLIERA DISABILITATA ALLA SOVRAPPOSIZIONE
  //-----------------------------------------------------------------
  if(bb.BloccoSovrap==true) sovrap_baia=false;
  //-----------------------------------------------------------------
  // DISPONIBILITA' DEPOSITO ALTO IN MAGAZZINO
  // Se le palette sono sovrapposte in rulliera, deve estere almeno
  // un magazzino per quel codice, abilitato alla sovrapposizione
  //-----------------------------------------------------------------
  disponib_sovrap_mag=false;
  if(bb.PresCEST_A!=LIBERA) disponib_sovrap_mag=true;
  //-----------------------------------------------------------------
  // Recupero il nodo della baia
  //-----------------------------------------------------------------
  Punto=bb.Punto;
  //-----------------------------------------------------------------
  // ABBINA_NODO_PRELIEVO_BAIA_A_TIPOBOX
  // Selezione del PuntoAux in baia in base al TIPOBOX in prelievo
  //-----------------------------------------------------------------
  #ifdef ABBINA_NODO_PRELIEVO_BAIA_A_TIPOBOX
     if(bb.PuntoAux[tipo_box]>0 && bb.PuntoAux[tipo_box]<=TOTPUNTI) Punto = bb.PuntoAux[tipo_box];
  #endif;
  //-----------------------------------------------------------------
  // Composizione chiamata
  //-----------------------------------------------------------------
  TestNumChiamata(&chiamata, Punto, bb.Num);
  if(chiamata==0) return 0;     // chiamata sconosciuta
  //-----------------------------------------------------------------
  // Cerco AGV libero disponibile ad asservire la chiamata
  // 1 - CERCO UN TGV GIA' ABBINATO ALLA CHIAMATA
  // 2 - CERCO UN TGV COMPLETAMENTE LIBERO (devono esserci entrambe
  //     le presenze contemporaneamente)
  //-----------------------------------------------------------------
  if(CercaAgvLibero(FALSE, Punto, chiamata, CH_ESPULSIONE, ZONA_0_ALL, &num_agv, all_mess)) return 0;
  if(num_agv==0){
     if(CercaAgvLibero(TRUE, Punto, chiamata, CH_ESPULSIONE, ZONA_0_ALL, &num_agv, all_mess)) return 0;
  }
  //-----------------------------------------------------------------
  // CAMBIO MISSIONI AL VOLO (Solo per navette TOTALMENTE LIBERE)
  // Andrea L. 16/04/2018
  // Essendo che nella funzione viene asseganata anche la chiamata,
  // Cerco il TGV libero per reset solo se Baia non in Blocco + Aut.
  //-----------------------------------------------------------------
  if(num_agv==0){
     if(bb.Blocco==false && bb.Automatico==true && bb.Automatico==true ){       //###
        if(CercaAgvLiberoPerReset(TRUE, Punto, chiamata, CH_ESPULSIONE, ZONA_0_ALL, &num_agv, all_mess)) return 0;
     }
  }
  //-----------------------------------------------------------------
  // Nessun TGV trovato
  //-----------------------------------------------------------------
  if(num_agv==0){
     //-----------------------------------------------------------------
     // Se non c'è nessun TGV disponibile e nessuna navettà in
     // asserivmento memorizzo NO TGV DISPONIBILE
     //-----------------------------------------------------------------
     altro_agv = (short int)(test_chiamata_asservita(chiamata));
     if( altro_agv==0) ba[baia]->NoTgvDisp=true;
     return 0;  //--> skip! cmq per mancanza tgv
  }
  //-----------------------------------------------------------------
  // Tgv trovato
  //-----------------------------------------------------------------
  ba[baia]->NoTgvDisp=false;
  //-----------------------------------------------------------------
  // verifica se la chiamata è già asservita da un AGV e l'altro AGV
  // è in missione attiva.
  //-----------------------------------------------------------------
  altro_agv = (short int)(test_chiamata_asservita(chiamata));
  if( altro_agv!=0 && altro_agv!=num_agv ) return 0;

  //-----------------------------------------------------------------
  // SE IL TGV E' COMPLETAMENTE LIBERO
  // Verifico la disponibilità del magazzino, per poi inizializzare
  // la missione di carico dalla baia
  //-----------------------------------------------------------------
  if(AGV[num_agv]->mission.PresCEST_A==LIBERA && AGV[num_agv]->mission.PresCEST_B==LIBERA){
     //-----------------------------------------------------------------
     // PRELIEVO PALETTE BASSE CON SOVRAPPOSIZIONE IN RULLIERA
     // Se è abilitata la sovrapposizione delle palette in rulliera,
     // la paletta in evacuazione è bassa e sovrapponibile, ma la pos
     // successiva è vuota, mentre la 3° è impegnata con una paletta
     // bassa dello stesso tipo, aspetto che avanzi prima di prelevare.
     //
     // N.B. PER EVITARE DI ANDARE VIA CON PALETTA SINGOLA MENTRE SIAT
     //      STA FACENDO AVANZARE LA FIFO
     //-----------------------------------------------------------------
     if(bb.PresCEST_B!=LIBERA && bb.PresCEST_A==LIBERA){
        if(bb.BloccoSovrap==false && sovrap_baia==false && basso==true && sovrap==true && codice1==0 && codice2!=0){
           if(posti>2 && codice2==codice && lotto2==lotto && car_wash2==car_wash && basso2==basso && sovrap2==sovrap && rotaz2==rotaz && tipo_box2==tipo_box) return 0;
        }
     }
     //-----------------------------------------------------------------
     // Selezione del TIPO DI PRESA
     //
     // GestioneDoppiaForcaTGV=0: forca singola carico singolo
     //
     // GestioneDoppiaForcaTGV=1: doppia forca con singolo carico su
     //                           ogni forca
     // GestioneDoppiaForcaTGV=2: forca singola con doppio carico
     //-----------------------------------------------------------------
     typ_presa=N_NO_FORCHE;
     if(GestioneDoppiaForcaTGV==0 && AGV[num_agv]->stato.s.bit.carB==false) typ_presa=N_FORCA_B;
     if(GestioneDoppiaForcaTGV==1){
        if(AGV[num_agv]->stato.s.bit.carB==false && AGV[num_agv]->stato.s.bit.carA==true ) typ_presa=N_FORCA_B;
        if(AGV[num_agv]->stato.s.bit.carB==false && AGV[num_agv]->stato.s.bit.carA==false) typ_presa=N_FORCA_A;
     }
     if(GestioneDoppiaForcaTGV==2){
        if(AGV[num_agv]->stato.s.bit.carB==false && AGV[num_agv]->stato.s.bit.carA==false){
           //-----------------------------------------------------
           // Paletta singola su rulliera
           //-----------------------------------------------------
           if(bb.PresCEST_B!=LIBERA && bb.PresCEST_A==LIBERA){
              typ_presa=N_FORCA_B;
              if(sovrap_baia==true) typ_presa=N_FORCA_A;
           }
           //-----------------------------------------------------
           // Paletta già sovrapposta su rulliera
           //-----------------------------------------------------
           if(bb.PresCEST_B!=LIBERA && bb.PresCEST_A!=LIBERA){
              typ_presa=N_FORCA_AB;
           }
        }
     }
     //----------------------------------------------------------------
     // Manca tipo presa --> skip!
     //----------------------------------------------------------------
     if(typ_presa==N_NO_FORCHE ) return 0;
     //-----------------------------------------------------------------
     // AL-20/06/2013 LE PRESE SONO TUTTE SOVRAPPONIBILI
     // tranne quelle con il flag di Car-Wash.
     // Il bit di "sovrapposizione" è importante per il
     // controllo dello stato della presa su cui sovraporre
     // che ovviamente deve essere ON
     //-----------------------------------------------------------------
     presa_sovrapponibile=true;
     if(SovrapCarWash==0 && car_wash!=0) presa_sovrapponibile=false;
     //-----------------------------------------------------------------
     // GESTIONE UDC
     // Come dati prodotto vengono usati quelli del primo UDC in presa
     // bassa, perchè comunque tutti i pallet di una presa deveono
     // essere uguali
     //-----------------------------------------------------------------
     udc_trovato=false;
     memset(&Udc, 0, sizeof(DATI_PRD));
     if(GestioneUdc!=0){
        for(i=0; i<POSINPRESA; i++){
           for(j=0; j<PIANIINPRESA; j++){
              if(bb.BoxB.Presa[i][j]==false) continue;
              //-------------------------------------------
              // Verifico se UDC duplicato
              //-------------------------------------------
              k=i+(j*POSINPRESA);
              memcpy(&Udc, &bb.BoxB.Prd[k], sizeof(DATI_PRD));
              udc_trovato=true;
              break;
           }
           if(udc_trovato==true) break;
        }
     }
     //-----------------------------------------------------------------
     // GESTIONE RICERCA MAGAZZINO DI DEPOSITO
     // 1. MAGAZZINI INCOMPLETI
     // 2. MAGAZZINI ACCOPPIATI
     // 3. MAGAZZINI LIBERI
     // NOTA: In caso si prevista la sovrapposizione pallet sulla
     //       rulliera, il magazzino da cercare deve essere
     //       predisposto alla sovrapposizione
     //-----------------------------------------------------------------
     lato_uscita=false;   // tentativo di scarico sempre l'ingresso magazzino
     // 1. GESTIONE MAGAZZINI INCOMPLETI
     if(M.CercaMagDepositoPIENO(FALSE, 0, AbilitaControlloNodoDest, deposito_nel_piu_vicino, lato_uscita, disponib_sovrap_mag, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
     //-----------------------------------------------------------------
     // 2. GESTIONE MAGAZZINI ACCOPPIATI
     // se trovo un magazzino incompleto verifico se è accoppiato ad
     // un altro e in caso che lo sia destino il TGV al magazzino che
     // dei due ha meno posizioni a terra impegnate
     //-----------------------------------------------------------------
     if(num_mag!=0){
        //-------------------------------------------------
        // Cerco magazzino accoppiato in struttura
        //-------------------------------------------------
        num_mag_accoppiato=0;
        for(i=0;;i++){
           if(AccoppiaMagaz[i][0]==0 && AccoppiaMagaz[i][1]==0            ) break;
           if(AccoppiaMagaz[i][0]!=num_mag && AccoppiaMagaz[i][1]!=num_mag) continue;
           if(AccoppiaMagaz[i][0]==num_mag){num_mag_accoppiato=AccoppiaMagaz[i][1]; break;}
           if(AccoppiaMagaz[i][1]==num_mag){num_mag_accoppiato=AccoppiaMagaz[i][0]; break;}
        }
        //-------------------------------------------------
        // Verifico se magazzino accoppiato è disponibile
        //-------------------------------------------------
        if(num_mag_accoppiato>0 && num_mag_accoppiato<=TOTMAG){
           num_mag1=num_pin1=num_pos1=0;
           if(M.CercaMagDepositoPIENO(FALSE, num_mag_accoppiato, AbilitaControlloNodoDest, deposito_nel_piu_vicino, lato_uscita, disponib_sovrap_mag, grp_dest, &num_mag1, &num_pos1, &num_pin1, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
           //-----------------------------------------------
           // Se magazzino accoppiato è disponibile scelgo
           // tra i due in coppia quello con meno postazioni
           // a terra impegnate
           //-----------------------------------------------
           num_mag_accoppiato=0;
           if(num_mag1!=0){
              if(num_pos1>num_pos || (num_pos1==num_pos && num_pin1>num_pin)){
                 num_mag=num_mag1;
                 num_pos=num_pos1;
                 num_pin=num_pin1;
                 num_mag_accoppiato=num_mag1;
              }
           }
        }
     }
     // 2. GESTIONE MAGAZZINI LIBERI
     if(num_mag==0) if(M.CercaMagDepositoPIENO(TRUE, 0, AbilitaControlloNodoDest, deposito_nel_piu_vicino, lato_uscita, disponib_sovrap_mag, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
     //-----------------------------------------------------------------
     // Nessun magazzino libero --> skip!
     //-----------------------------------------------------------------
     if(num_mag==0){
        ba[baia]->NoMagaz=true;
        return 0;
     }
     //######
     //-------------------------------------------------------------------------
     // Andrea 02/05/2018
     // Se trovato Magazzino, verifica se è già associato come chiamata a TGV
     // (destinazione di spostamento Mag>Mag). Se si, non considera la chiamata
     //-------------------------------------------------------------------------
     if( num_mag>0 ){
        //-----------------------------------------------------------------
        // Composizione chiamata abbinata a magazzino
        //-----------------------------------------------------------------
        chiamata2=0;
        P.TestNumChiamata(&chiamata2, M.m[num_mag]->PuntoOut, num_mag);
        //if(chiamata2==0) continue;     // chiamata sconosciuta - già controllato in cerca Mag.
        //-----------------------------------------------------------------
        // verifica se la chiamata è già asservita da un AGV e l'altro AGV
        // è in missione attiva (Mag. destinazione di spostamento pallet)
        //-----------------------------------------------------------------
        altro_tgv2 = (short int)(P.test_chiamata_asservita(chiamata2));
        if( altro_tgv2!=0 && altro_tgv2!=num_agv ){
           ba[baia]->NoMagaz=true;
           return 0;
        }
     }
     //######
     //-----------------------------------------------------------------
     // EVITO TGV FERMI SOTTO BAIE DI CARICO
     // Se è disattivato il controllo del nodo di DEPOSITO evito la
     // missione di carico se altri TGV impegnati in missioni di
     // carico dello stesso codice e ci sono altre chiamate disponibili
     //-----------------------------------------------------------------
     if(AbilitaControlloNodoDest==false && num_mag_accoppiato==0 && num_mag>0 && num_mag<=TOTMAG){
        altro_agv=0;
        codice_gia_in_movimento=false;
        //-------------------------------------------------------------
        // Controllo se codice già in movimento verso MAGAZZINO
        //-------------------------------------------------------------
        test_dest_occupata(M.m[num_mag]->PuntoOut, &altro_agv);
        if(altro_agv==0) test_dest_riservata(M.m[num_mag]->PuntoOut, &altro_agv, num_agv);
        if(altro_agv>0 && altro_agv<=MAXAGV){
           if(AGV[altro_agv]->mission.PresCEST_B!=LIBERA && AGV[altro_agv]->mission.BoxB.Codice==codice) codice_gia_in_movimento=true;
           if(AGV[altro_agv]->mission.PresCEST_A!=LIBERA && AGV[altro_agv]->mission.BoxA.Codice==codice) codice_gia_in_movimento=true;
        }
        //-------------------------------------------------------------
        // Controllo se stesso codice già in carico da altra BAIA
        //-------------------------------------------------------------
        if(codice_gia_in_movimento==false){
           for(i=1; i<=TOTBAIE; i++){                // per tutte le baie
              //-------------------------------------------------------
              // Se la macchina non esiste --> skip!!
              //-------------------------------------------------------
              if(P.ba[i]->Num<1             ) continue;
              if(P.ba[i]->Num>TOTBAIE       ) continue;
              if(P.ba[i]->Tipo!=BA_PRELIEVO ) continue;
              if(P.ba[i]->Esclusione==true  ) continue;
              if(P.ba[i]->Blocco==true      ) continue;
              if(P.ba[i]->Automatico==false ) continue;
              if(P.ba[i]->Chiamata==false   ) continue;
              if(P.ba[i]->Codice[0]!=codice ) continue;
              //-------------------------------------------------------
              // Composizione chiamata
              //-------------------------------------------------------
              chiamata1=0;
              TestNumChiamata(&chiamata1, P.ba[i]->Punto, P.ba[i]->Num);
              if(chiamata1==0) continue;     // chiamata sconosciuta
              //-------------------------------------------------------
              // Verifico se CHIAMATA GIA' ASSERVITA
              //-------------------------------------------------------
              altro_agv = (short int)(test_chiamata_asservita(chiamata1));
              if(altro_agv==0 || altro_agv==num_agv) continue;
              //-------------------------------------------------------
              // Se l'altro AGV non è destiato al nodo baia, ma al
              // nodo di attesa -> skip!!
              //-------------------------------------------------------
              if(AGV[altro_agv]->mission.pend!=P.ba[i]->Punto) continue;
              //-------------------------------------------------------
              // CODICE GIA' IN MOVIMENTO PER TGV IN CARICO DA BAIA
              //-------------------------------------------------------
              codice_gia_in_movimento=true;
              break;
           }
        }
        //-----------------------------------------------------------------
        // TGV con pallet a bordo senza missione
        // (fase di inizializzazione o condizione di manuale)
        //-----------------------------------------------------------------
        if(codice_gia_in_movimento==false){
           for(i=1; i<=MAXAGV; i++){                // per tutte le baie
              if(AGV[i]->mission.Esclusione==true    ) continue;
              if(AGV[i]->mission.SemiAutomatico==true) continue;
              if(AGV[i]->mission.PresCEST_B==LIBERA && AGV[i]->mission.PresCEST_A==LIBERA) continue;
            //if(AGV[i]->mission.PresCEST_B!=LIBERA && AGV[num_agv]->stato.s.bit.carB==true && AGV[i]->mission.BoxB.Codice==codice){
              if(AGV[i]->mission.PresCEST_B!=LIBERA && AGV[i]->stato.s.bit.carB==true && AGV[i]->mission.BoxB.Codice==codice){
                 if(grp_dest==NN) {codice_gia_in_movimento=true; break;}
                 else{
                    if(AGV[i]->mission.BoxB.GruppoDest==NN      ){codice_gia_in_movimento=true; break;}
                    if(grp_dest==AGV[i]->mission.BoxB.GruppoDest){codice_gia_in_movimento=true; break;}
                 }
              }
            //if(AGV[i]->mission.PresCEST_A!=LIBERA && AGV[num_agv]->stato.s.bit.carA==true && AGV[i]->mission.BoxA.Codice==codice){
              if(AGV[i]->mission.PresCEST_A!=LIBERA && AGV[i]->stato.s.bit.carA==true && AGV[i]->mission.BoxA.Codice==codice){
                 if(grp_dest==NN) {codice_gia_in_movimento=true; break;}
                 else{
                    if(AGV[i]->mission.BoxA.GruppoDest==NN      ){codice_gia_in_movimento=true; break;}
                    if(grp_dest==AGV[i]->mission.BoxA.GruppoDest){codice_gia_in_movimento=true; break;}
                 }
              }
           }
        }
        //-----------------------------------------------------------------
        // Ammesso carico di carico di un solo codice pallet per volta
        //-----------------------------------------------------------------
        if(codice_gia_in_movimento==true) return 0;
     }
     //-----------------------------------------------------------------
     // Magazzino/Baia di deposito trovati
     //-----------------------------------------------------------------
     ba[baia]->NoMagaz=false;
     //-----------------------------------------------------------------
     // Procedo a prelevare
     //-----------------------------------------------------------------
     PInt = 0;
     Dest = Punto;
     //-----------------------------------------------------------------
     // Verifico se la destinazione è impegnata da un altro TGV
     //-----------------------------------------------------------------
     if(AbilitaAvvicinamentoADest!=1 && AbilitaAvvicinamentoADest!=3){
        if(P.test_dest_limitrofe(Dest, &altro_agv, &limitrofo, num_agv) && altro_agv!=0 && altro_agv!=num_agv) return 0;
        if(test_dest_riservata(Dest, &altro_agv, num_agv) && altro_agv!=0 && altro_agv!=num_agv) return 0;
        err = test_dest_occupata(Dest, &altro_agv);
        if((err!=0) && (num_agv!=altro_agv)) return 0;
     }
     //-----------------------------------------------------------------
     // ABILITA AVVICINAMENTO (AbilitaAvvicinamentoADest)
     // 1 - a baie prelievo
     // 2 - a baie deposito
     // 3 - a baie prelievo + deposito
     // Verifico se ci sono le condizioni per inizializzare la
     // missione fino a destinazione o se è possibile eseguire
     // un avvicinamento al nodo_fuori_ingombro più vicino alla
     // destinazione finale
     //-----------------------------------------------------------------
     else{
        AGV[num_agv]->AvvicinamentoADestinazione(FAST_PATH, Dest, 0, &PInt);
        if(PInt==0){
           AGV[num_agv]->AvvicinamentoADestinazione(STD_PATH, Dest, 0, &PInt);
           //-----------------------------------------------
           // Nodo di attesa per avvicinamento non trovato:
           // verifico che la destinazione sia raggiungibile
           //-----------------------------------------------
           if(PInt==0){
              if(P.test_dest_limitrofe(Dest, &altro_agv, &limitrofo, num_agv) && altro_agv!=0 && altro_agv!=num_agv) return 0;
              if(test_dest_riservata(Dest, &altro_agv, num_agv) && altro_agv!=0 && altro_agv!=num_agv) return 0;
              err = test_dest_occupata(Dest, &altro_agv);
              if((err!=0) && (num_agv!=altro_agv)) return 0;
           }
        }
     }
     //-----------------------------------------------------------------
     // Inizializzazione missione verso linea
     //-----------------------------------------------------------------
     if(new_init_mission(FALSE, CALCOLA_TIPO_PATH, num_agv, chiamata, Dest, 0, PInt, MISS_CARICO_DA_BAIA, typ_presa, &Ok, all_mess)) return 1;
  }
  //-----------------------------------------------------------------
  // SE IL TGV E' LIBERO SU UNA SOLA POSIZIONE (LA BASSA)
  // Devo depositare il carico sul cestone che sarà filtrato sotto
  // e rifare un carico doppi con una seconda missione
  //-----------------------------------------------------------------
  if((AGV[num_agv]->mission.PresCEST_A!=LIBERA && AGV[num_agv]->mission.PresCEST_B==LIBERA) || (AGV[num_agv]->mission.PresCEST_A==LIBERA && AGV[num_agv]->mission.PresCEST_B!=LIBERA)){
     //-----------------------------------------------------------------
     // Magazzino/Baia di deposito trovati
     //-----------------------------------------------------------------
     ba[baia]->NoMagaz=false;
     //-----------------------------------------------------------------
     // Procedo a prelevare
     //-----------------------------------------------------------------
     PInt = 0;
     Dest = Punto;
     //-----------------------------------------------------------------
     // Se la posizione di prelievo in baia è ancora libera aspetto
     //-----------------------------------------------------------------
     if(tipo_carico==LIBERA) return 0;
     //-----------------------------------------------------------------
     // Recupero i dati della paletta già a bordo TGV in base alla
     // forca impegnata
     //-----------------------------------------------------------------
     if(AGV[num_agv]->mission.PresCEST_A!=LIBERA){
        tipo_carico1 = AGV[num_agv]->mission.PresCEST_A;
        tipo_box1    = AGV[num_agv]->mission.BoxA.TipoBox;
        codice1      = AGV[num_agv]->mission.BoxA.Codice;
        lotto1       = AGV[num_agv]->mission.BoxA.Lotto;
        basso1       = AGV[num_agv]->mission.BoxA.Basso;
        sovrap1      = AGV[num_agv]->mission.BoxA.Sovrap;
        rotaz1       = AGV[num_agv]->mission.BoxA.Ruotato180;
        memcpy(&presa1[0][0], &AGV[num_agv]->mission.BoxA.Presa[0][0], sizeof(presa));
     }
     if(AGV[num_agv]->mission.PresCEST_B!=LIBERA){
        tipo_carico1 = AGV[num_agv]->mission.PresCEST_B;
        tipo_box1    = AGV[num_agv]->mission.BoxB.TipoBox;
        codice1      = AGV[num_agv]->mission.BoxB.Codice;
        lotto1       = AGV[num_agv]->mission.BoxB.Lotto;
        basso1       = AGV[num_agv]->mission.BoxB.Basso;
        sovrap1      = AGV[num_agv]->mission.BoxB.Sovrap;
        rotaz1       = AGV[num_agv]->mission.BoxB.Ruotato180;
        memcpy(&presa1[0][0], &AGV[num_agv]->mission.BoxB.Presa[0][0], sizeof(presa));
     }
     //-----------------------------------------------------------------
     // Verifico sovrapponibilità
     //-----------------------------------------------------------------
     sovrap_baia=true;
     if(codice!=codice1                 ) sovrap_baia=false;
     if(lotto!=lotto1                   ) sovrap_baia=false;
     if(tipo_box!=tipo_box1             ) sovrap_baia=false;
     if(rotaz!=rotaz1                   ) sovrap_baia=false;
     if(basso==false  || basso1==false  ) sovrap_baia=false;
     if(sovrap==false || sovrap1==false ) sovrap_baia=false;
     for(i=0; i<POSINPRESA; i++){
        if(sovrap_baia==true) break;
        //------------------------------------------------
        // struttura a terra sovrapponibile
        //------------------------------------------------
        if(presa[i][0]==false && presa1[i][0]==true) sovrap_baia=false;
        //------------------------------------------------
        // piano alto già presente
        //------------------------------------------------
        if(piani>1 && sovrap_baia==true){
           for(j=1; j<piani; j++){
              if(presa [i][j]==true){sovrap_baia=false; break;}
              if(presa1[i][j]==true){sovrap_baia=false; break;}
           }
        }
     }
     //-----------------------------------------------------------------
     // No sovrapponibilità azzero la chiamata a bordo TGV
     // per dirigerlo in magazzino
     //-----------------------------------------------------------------
     if(sovrap_baia==false){
        if(AGV[num_agv]->mission.Chiamata==chiamata) AGV[num_agv]->mission.Chiamata=0;
        return 0;
     }
     //-----------------------------------------------------------------
     // Recupero il tipo presa dallo stato delle presenze a bordo
     //-----------------------------------------------------------------
     typ_presa = N_NO_FORCHE;
     if(GestioneDoppiaForcaTGV==0 && AGV[num_agv]->stato.s.bit.carB==true) typ_presa=N_FORCA_B;
     if(GestioneDoppiaForcaTGV==1){
        if(AGV[num_agv]->stato.s.bit.carB==false && AGV[num_agv]->stato.s.bit.carA==true ) typ_presa=N_FORCA_A;
        if(AGV[num_agv]->stato.s.bit.carB==true  && AGV[num_agv]->stato.s.bit.carA==false) typ_presa=N_FORCA_B;
     }
     if(GestioneDoppiaForcaTGV==2){
        //-------------------------------------------------------
        // carico singolo, ma possibilità di presenza doppia
        //-------------------------------------------------------
        if(AGV[num_agv]->stato.s.bit.carB==true){
           if(AGV[num_agv]->mission.PresCEST_B!=LIBERA && AGV[num_agv]->mission.PresCEST_A==LIBERA) typ_presa=N_FORCA_B;
           if(AGV[num_agv]->mission.PresCEST_B==LIBERA && AGV[num_agv]->mission.PresCEST_A!=LIBERA) typ_presa=N_FORCA_A;
        }
     }
     //----------------------------------------------------------------
     // Manca tipo presa --> skip!
     //----------------------------------------------------------------
     if(typ_presa==N_NO_FORCHE ) return 0;
     //-----------------------------------------------------------------
     // Se il TGV ormai è fuori nodo, mando in magazzino per poi
     // eventualmente sovrapporre là
     //-----------------------------------------------------------------
     if(AGV[num_agv]->stato.pos!=Punto){
        if(AGV[num_agv]->mission.Chiamata==chiamata) AGV[num_agv]->mission.Chiamata=0;
        return 0;
     }
     //-----------------------------------------------------------------
     // Inizializzazione missione verso linea
     //-----------------------------------------------------------------
     if(new_init_mission(FALSE, CALCOLA_TIPO_PATH, num_agv, chiamata, Dest, 0, PInt, MISS_SCARICO_SU_BAIA, typ_presa, &Ok, all_mess)) return 1;
  }
  return 0;
}

// --------------------------------------------------------
//   GestioneSpostaPallet()
// --------------------------------------------------------
//   Ritorna 1 se c'è errore
//
int process::GestioneSpostaPallet(char *all_mess)
/********************************************/
{
  bool Ok;
  bool ordine_ok;
  bool udc_trovato;
  int  stato, err;
  int  i, j, k;
  int  chiamata, ciclo;
  int  provenienza, lotto;
  short int PInt, Dest, PosDest;
  int  SovrapCarWash = SOVRAPPOSIZIONE_CARWASH;
  bool cambia_rot, tgv_libero;
  bool lato_ing_al_prelivo;
  bool lato_out_al_deposito;
  bool basso, rotaz, sovrap;
  bool presa_sovrapponibile;
  bool pallet_non_raggiungibile;
  short int num_mag_accoppiato;
  short int num_mag1, num_pin1, num_pos1;
  short int bai_ch, mac_ch, mag_ch, pos_ch;
  short int Punto, gruppo, grp_dest, tipo_mag;
  short int typ_presa, altro_agv, limitrofo;
  short int tipo_prov_box, mac_prov_box, bai_prov_box, mag_prov_box;
  short int mag_prel, pos_prel, pin_prel, pos_spost, pin_spost;
  short int num_agv, num_mag, num_pin, num_pos, new_agv;
  short int tipo_carico, tipo_box, car_wash, codice;
  bool presa[POSINPRESA+1][PIANIINPRESA+1];
  short int OrdineGrp[MAXGRUPPIIMP+1];
  short int Grp, Grp1, Grp2;
  struct DATI_PRD Udc;

  //--------------------------------------------------------------------
  // Metto in ordine i gruppi
  //--------------------------------------------------------------------
  Grp=Grp1=Grp2=0;
  memset(&OrdineGrp[0], 0, sizeof(OrdineGrp));
  for(i=1; i<=MAXGRUPPIIMP; i++){
     if(strcmp(P.GruppiBaie[i].Nome, "")==0) continue;
     OrdineGrp[Grp]=i;
     Grp++;
  }
  //--------------------------------------------------------------------
  // Ordinamento GRUPPI
  //--------------------------------------------------------------------
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
  //--------------------------------------------------------------------
  // Cerca magazzino da SVUOTARE IN ORDINE PER PRIORITA' GRUPPO
  // CICLO 0 - gestione spostamento pallet per gruppo destionazione.
  // CICLO 1 - ripristino pallet spostati in magazzini TEMPORANEI
  //--------------------------------------------------------------------
  for(ciclo=0; ciclo<2; ciclo++){
     //--------------------------------------------
     // Selezione TIPO MAG CON PALLET DA SPOSTARE
     //--------------------------------------------
     switch(ciclo){
        case  0: tipo_mag=NN;
                 break;
        case  1: tipo_mag=MG_TEMPORANEO;
                 break;
        default: tipo_mag=NN;
                 break;
     }
     //--------------------------------------------
     // GESTIONE SPOSTAMENTI PER PRIORITA' GRUPPO
     //--------------------------------------------
     for(Grp=0; Grp<MAXGRUPPIIMP; Grp++){
        if(OrdineGrp[Grp]==0) break;
        gruppo = OrdineGrp[Grp];
        num_agv=0;
        tgv_libero=true;
        typ_presa=N_NO_FORCHE;
        //-----------------------------------------------------------------
        // SOLO PER SPOSTAMENTI DI MAGAZZINO
        // Scarta gruppi di baie chiamanti
        //-----------------------------------------------------------------
        if(P.GruppiBaie[gruppo].Tipo==GR_BAIE) continue;
        //-----------------------------------------------------------------
        // Verifico se non esiste un TGV libero verifico se esiste un TGV
        // senza missione in corso ma abbinato a una chiamata di spostam.
        //-----------------------------------------------------------------
        for(i=1; i<=MAXAGV; i++){
           if(AGV[i]->test_agv_escluso()          ) continue;
           if(AGV[i]->mission.SemiAutomatico==true) continue;
           if(AGV[i]->test_mission_on()           ) continue;
           if(AGV[i]->mission.Chiamata==0         ) continue;
           //----------------------------------------------------------
           // Solo per TGV liberi in ripartenza da manuale
           //----------------------------------------------------------
           if(AGV[i]->mission.PresCEST_B==true    ) continue;
           if(AGV[i]->stato.s.bit.carB==true      ) continue;
           if(AGV[i]->mission.PresCEST_A==true    ) continue;
           if(AGV[i]->stato.s.bit.carA==true      ) continue;
           //----------------------------------------------------------
           // Ammesse solo Chiamate di MAGAZZINI
           //----------------------------------------------------------
           bai_ch=mac_ch=mag_ch=pos_ch=0;
           P.TestChiamataNum(AGV[i]->mission.Chiamata, &mag_ch, &bai_ch, &mac_ch, &pos_ch);
           if(mag_ch<1     ) continue;
           if(mag_ch>TOTMAG) continue;
           //----------------------------------------------------------
           // Verifico se la chiamata appartiene al GRUPPO considerato
           //----------------------------------------------------------
           if(M.m[mag_ch]->Gruppo!=gruppo) continue;
           num_agv=i;
           tgv_libero=false;
           break;
        }
        //-----------------------------------------------------------------
        // Verifico se c'è un TGV abilitato all'asservimento del GRUPPO
        // Cerco AGV libero disponibile ad asservire la chiamata
        // 1 - CERCO UN TGV GIA' ABBINATO ALLA CHIAMATA
        // 2 - CERCO UN TGV COMPLETAMENTE LIBERO (devono esserci entrambe
        //     le presenze contemporaneamente)
        //-----------------------------------------------------------------
        if(num_agv==0){
           if(CercaAgvLibero(FALSE, 0, gruppo, CH_ESPULSIONE, ZONA_0_ALL, &num_agv, all_mess)) return 0;
        }
        if(num_agv==0){
           if(CercaAgvLibero(TRUE, 0, gruppo, CH_ESPULSIONE, ZONA_0_ALL, &num_agv, all_mess)) return 0;
        }
        if(num_agv==0) continue;
        //-----------------------------------------------------------------
        // SE IL TGV E' COMPLETAMENTE LIBERO
        // Verifico la disponibilità del magazzino, per poi inizializzare
        // la missione di carico dalla baia
        //-----------------------------------------------------------------
        typ_presa=N_FORCA_AB;
        if(AGV[num_agv]->mission.PresCEST_A==LIBERA && AGV[num_agv]->mission.PresCEST_B==LIBERA){
           //-----------------------------------------------------------------
           // Selezione del TIPO DI PRESA
           //
           // GestioneDoppiaForcaTGV=0: forca singola carico singolo
           //
           // GestioneDoppiaForcaTGV=1: doppia forca con singolo carico su
           //                           ogni forca
           // GestioneDoppiaForcaTGV=2: forca singola con doppio carico
           //-----------------------------------------------------------------
           if(GestioneDoppiaForcaTGV==0 && AGV[num_agv]->stato.s.bit.carB==false) typ_presa=N_FORCA_B;
           if(GestioneDoppiaForcaTGV==1){
              if(AGV[num_agv]->stato.s.bit.carB==false && AGV[num_agv]->stato.s.bit.carA==true ) typ_presa=N_FORCA_B;
              if(AGV[num_agv]->stato.s.bit.carB==false && AGV[num_agv]->stato.s.bit.carA==false) typ_presa=N_FORCA_A;
           }
           //-----------------------------------------------------------------
           // GestioneDoppiaForcaTGV=2: forca singola con doppio carico
           // TEMPORANEAMENTE NON AMMESSA IN ATTESA DI IMPIANTO SU CUI
           // IMPLEMENTARLA
           //-----------------------------------------------------------------
           if(GestioneDoppiaForcaTGV==2) continue;
        }
        //-----------------------------------------------------------------
        // CERCO MAGAZZINO DA SVUOTARE CON INGRESSO STANDARD
        //-----------------------------------------------------------------
        cambia_rot=false;
        pos_spost=pin_spost=0;
        lato_ing_al_prelivo=false;
        mag_prel=pos_prel=pin_prel=0;
        pallet_non_raggiungibile=false;
        tipo_prov_box=mac_prov_box=bai_prov_box=mag_prov_box=0;
        M.CercaMagDaSvuotarePIENO(TRUE, lato_ing_al_prelivo, FALSE, tipo_mag, gruppo, num_agv, typ_presa, &mag_prel, &pos_prel, &pin_prel, &pos_spost, &pin_spost, &cambia_rot);
        if(mag_prel==0) M.CercaMagDaSvuotarePIENO(FALSE, lato_ing_al_prelivo, FALSE, tipo_mag, gruppo, num_agv, typ_presa, &mag_prel, &pos_prel, &pin_prel, &pos_spost, &pin_spost, &cambia_rot);
        if(mag_prel==0 && tipo_mag!=MG_TEMPORANEO){
           //--------------------------------------------------------------
           // Verifico se esiste una magazzino da svuotare con un pallet
           // non raggiungibile
           //--------------------------------------------------------------
           M.CercaMagDaSvuotarePIENO(FALSE, lato_ing_al_prelivo, TRUE, NN, gruppo, num_agv, typ_presa, &mag_prel, &pos_prel, &pin_prel, &pos_spost, &pin_spost, &cambia_rot);
           //--------------------------------------------------------------
           // Setto memoria di pallet non raggiungibile
           //--------------------------------------------------------------
           if(mag_prel!=0) pallet_non_raggiungibile=true;
        }
        //-----------------------------------------------------------------
        // GESTIONE SPOSTA E RITORNA:
        // In caso di questa gestione in cui il pallet dopo essere stato
        // spostato deve anche ritornare al magazzino di origine, è
        // necessario prelevare i pallet bloccanti dal lato di deposito
        //-----------------------------------------------------------------
        if(mag_prel==0) continue;
        //-----------------------------------------------------------------
        // MAGAZZINI TEMPORANEI RECUPERO MAGAZZINO DI ORIGINE
        // Trattandosi di un magazzino temporaneo recupero dalla locazione
        // di provenienza il magazzino di destinazione
        //
        // N.B.: Recupero dai mag. temporanei solo se previsto il ritorno
        //       al mag. di origine (AbilitaSvuotamentoManualeMag==3)
        //-----------------------------------------------------------------
        if(M.m[mag_prel]->Tipo==MG_TEMPORANEO){
           //------------------------------------------------
           // NO RECUPERO DA MAG TEMPORANEI
           // (AbilitaSvuotamentoManualeMag==3)
           //------------------------------------------------
           if(AbilitaSvuotamentoManualeMag!=3) continue;
           //------------------------------------------------
           // Decodifica provenieza
           //------------------------------------------------
           P.TestProvenienzaBox(M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].ProvBox, &tipo_prov_box, &mac_prov_box, &bai_prov_box, &mag_prov_box);
           if(tipo_prov_box!=PROV_MAGAZZINO) continue;
           if(mag_prov_box<1               ) continue;
           if(mag_prov_box>TOTMAG          ) continue;
           if(mag_prov_box==mag_prel       ) continue;
        }
        //-----------------------------------------------------------------
        // SICUREZZA: per evitare spostamenti inutili di pallet bloccanti
        // allo scopo di liberare il pallet da spostare, prima di tutto
        // verifico la disponibilità di magazzino finale per il tipo di
        // pallet da spostare
        //-----------------------------------------------------------------
        if(M.m[mag_prel]->Tipo!=MG_TEMPORANEO && pos_spost!=0){
           PInt=0;
           Dest=0;
           Punto=0;
           PosDest=0;
           num_mag=0;
           num_pin=0;
           num_pos=0;
           chiamata=0;
           altro_agv=0;
           limitrofo=0;
           memset(&presa[0][0], false, sizeof(presa));
           provenienza = (short int)((10000*PROV_MAGAZZINO)+mag_prel);
           //-----------------------------------------------------------------
           // PARAMETRI SENSIBILI PER RICERCA MAGAZZINO DI DEPOSITO
           // Selezione tipo box richiesto/espulso dalla macchina
           //-----------------------------------------------------------------
           tipo_carico = M.m[mag_prel]->Pos[pos_spost].Box[pin_spost].StatoLav;
           tipo_box    = M.m[mag_prel]->Pos[pos_spost].Box[pin_spost].TipoBox;
           car_wash    = M.m[mag_prel]->Pos[pos_spost].Box[pin_spost].CarWash;
           codice      = M.m[mag_prel]->Pos[pos_spost].Box[pin_spost].Codice;
           lotto       = M.m[mag_prel]->Pos[pos_spost].Box[pin_spost].Lotto;
           basso       = M.m[mag_prel]->Pos[pos_spost].Box[pin_spost].Basso;
           sovrap      = M.m[mag_prel]->Pos[pos_spost].Box[pin_spost].Sovrap;
           grp_dest    = M.m[mag_prel]->Pos[pos_spost].Box[pin_spost].GruppoDest;
           rotaz       = M.m[mag_prel]->Pos[pos_spost].Box[pin_spost].Ruotato180;
           if(cambia_rot==true) rotaz=!rotaz;
           memcpy(&presa[0][0], &M.m[mag_prel]->Pos[pos_spost].Box[pin_spost].Presa[0][0], sizeof(presa));
           //-----------------------------------------------------------------
           // AL-20/06/2013 LE PRESE SONO TUTTE SOVRAPPONIBILI
           // tranne quelle con il flag di Car-Wash.
           // Il bit di "sovrapposizione" è importante per il
           // controllo dello stato della presa su cui sovraporre
           // che ovviamente deve essere ON
           //-----------------------------------------------------------------
           presa_sovrapponibile=sovrap;
           if(SovrapCarWash==0 && car_wash!=0) presa_sovrapponibile=false;
           //-----------------------------------------------------------------
           // GESTIONE UDC
           // Come dati prodotto vengono usati quelli del primo UDC in presa
           // bassa, perchè comunque tutti i pallet di una presa deveono
           // essere uguali
           //-----------------------------------------------------------------
           udc_trovato=false;
           memset(&Udc, 0, sizeof(DATI_PRD));
           if(GestioneUdc!=0){
              for(i=0; i<POSINPRESA; i++){
                 for(j=0; j<PIANIINPRESA; j++){
                    if(M.m[mag_prel]->Pos[pos_spost].Box[pin_spost].Presa[i][j]==false) continue;
                    //-------------------------------------------
                    // Verifico se UDC duplicato
                    //-------------------------------------------
                    k=i+(j*POSINPRESA);
                    memcpy(&Udc, &M.m[mag_prel]->Pos[pos_spost].Box[pin_spost].Prd[k], sizeof(DATI_PRD));
                    udc_trovato=true;
                    break;
                 }
                 if(udc_trovato==true) break;
              }
           }
           //-----------------------------------------------------------------
           // GESTIONE RICERCA BAIA DI DEPOSITO
           // Verifica disponibilità di almeno un magazzino completamente
           // vuoto con possibilità di deposito a terra
           //
           // NOTA: In caso si prevista la sovrapposizione pallet sulla
           //       rulliera, il magazzino da cercare deve essere
           //       predisposto alla sovrapposizione
           //-----------------------------------------------------------------
           lato_out_al_deposito=false;   // tentativo di scarico sempre l'ingresso magazzino
           if(M.CercaMagRiempirePIENO(FALSE, FALSE, lato_out_al_deposito, FALSE, mag_prov_box, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) continue;
           if(num_mag==0) if( M.CercaMagRiempirePIENO(TRUE, FALSE, lato_out_al_deposito, FALSE, mag_prov_box, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) continue;
           if(num_mag==0 && M.m[mag_prel]->Fifo==true && cambia_rot==true && lato_ing_al_prelivo==false){
              rotaz=!rotaz;
              if(M.CercaMagRiempirePIENO(FALSE, FALSE, lato_out_al_deposito, FALSE, mag_prov_box, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) continue;
              if(num_mag==0) if( M.CercaMagRiempirePIENO(TRUE, FALSE, lato_out_al_deposito, FALSE, mag_prov_box, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) continue;
           }
           //-----------------------------------------------------------------
           // SE NON ESISTE IL MAGAZZINO DI DEPOSITO PER IL PALLET DA SPOSTARE
           // NON VALUTO NEMMENO L'INGRESSO INVERTITO
           //-----------------------------------------------------------------
           if(num_mag==0) continue;
        }
        //-----------------------------------------------------------------
        // !!! OK SPOSTAMENTO POSSIBILE !!!
        //-----------------------------------------------------------------
        PInt=0;
        Dest=0;
        Punto=0;
        PosDest=0;
        num_mag=0;
        num_pin=0;
        num_pos=0;
        chiamata=0;
        altro_agv=0;
        limitrofo=0;
        memset(&presa[0][0], false, sizeof(presa));
        provenienza = (short int)((10000*PROV_MAGAZZINO)+mag_prel);
        //-----------------------------------------------------------------
        // PARAMETRI SENSIBILI PER RICERCA MAGAZZINO DI DEPOSITO
        // Selezione tipo box richiesto/espulso dalla macchina
        //-----------------------------------------------------------------
        tipo_carico = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].StatoLav;
        tipo_box    = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].TipoBox;
        car_wash    = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].CarWash;
        codice      = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Codice;
        lotto       = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Lotto;
        basso       = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Basso;
        sovrap      = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Sovrap;
        grp_dest    = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].GruppoDest;
        rotaz       = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Ruotato180;
        if(cambia_rot==true) rotaz=!rotaz;
        memcpy(&presa[0][0], &M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Presa[0][0], sizeof(presa));
        //-----------------------------------------------------------------
        // AL-20/06/2013 LE PRESE SONO TUTTE SOVRAPPONIBILI
        // tranne quelle con il flag di Car-Wash.
        // Il bit di "sovrapposizione" è importante per il
        // controllo dello stato della presa su cui sovraporre
        // che ovviamente deve essere ON
        //-----------------------------------------------------------------
        presa_sovrapponibile=sovrap;
        if(SovrapCarWash==0 && car_wash!=0) presa_sovrapponibile=false;
        //-----------------------------------------------------------------
        // Recupero il nodo del magazzino
        //-----------------------------------------------------------------
        Punto=M.m[mag_prel]->PuntoOut;
        if(lato_ing_al_prelivo==true) Punto=M.m[mag_prel]->PuntoIng;
        //-----------------------------------------------------------------
        // Composizione chiamata
        //-----------------------------------------------------------------
        if(tgv_libero==true){
           TestNumChiamata(&chiamata, Punto, mag_prel);
           if(chiamata==0) return 0;     // chiamata sconosciuta
           //-----------------------------------------------------------------
           // ORA CHE CONOSCO IL MAGAZZINO CERCO IL TGV PIU' VICINO
           // 1 - CERCO UN TGV GIA' ABBINATO ALLA CHIAMATA
           // 2 - CERCO UN TGV COMPLETAMENTE LIBERO (devono esserci entrambe
           //     le presenze contemporaneamente)
           //-----------------------------------------------------------------
           if(CercaAgvLibero(FALSE, Punto, chiamata, CH_ESPULSIONE, ZONA_0_ALL, &num_agv, all_mess)) return 0;
           if(num_agv==0){
              if(CercaAgvLibero(TRUE, Punto, chiamata, CH_ESPULSIONE, ZONA_0_ALL, &num_agv, all_mess)) return 0;
           }
        }
        //-----------------------------------------------------------------
        // Nessun TGV trovato
        //-----------------------------------------------------------------
        if(num_agv==0) continue;
        //-----------------------------------------------------------------
        // SE IL TGV E' COMPLETAMENTE LIBERO
        // Verifico la disponibilità del magazzino, per poi inizializzare
        // la missione di carico dalla baia
        //-----------------------------------------------------------------
        typ_presa=N_FORCA_AB;
        if(AGV[num_agv]->mission.PresCEST_A==LIBERA && AGV[num_agv]->mission.PresCEST_B==LIBERA){
           //-----------------------------------------------------------------
           // Selezione del TIPO DI PRESA
           //
           // GestioneDoppiaForcaTGV=0: forca singola carico singolo
           //
           // GestioneDoppiaForcaTGV=1: doppia forca con singolo carico su
           //                           ogni forca
           // GestioneDoppiaForcaTGV=2: forca singola con doppio carico
           //-----------------------------------------------------------------
           typ_presa=N_NO_FORCHE;
           if(GestioneDoppiaForcaTGV==0 && AGV[num_agv]->stato.s.bit.carB==false) typ_presa=N_FORCA_B;
           if(GestioneDoppiaForcaTGV==1){
              if(AGV[num_agv]->stato.s.bit.carB==false && AGV[num_agv]->stato.s.bit.carA==true ) typ_presa=N_FORCA_B;
              if(AGV[num_agv]->stato.s.bit.carB==false && AGV[num_agv]->stato.s.bit.carA==false) typ_presa=N_FORCA_A;
           }
           //-----------------------------------------------------------------
           // GestioneDoppiaForcaTGV=2: forca singola con doppio carico
           // TEMPORANEAMENTE NON AMMESSA IN ATTESA DI IMPIANTO SU CUI
           // IMPLEMENTARLA
           //-----------------------------------------------------------------
           if(GestioneDoppiaForcaTGV==2) continue;
        }
        //----------------------------------------------------------------
        // Manca tipo presa --> skip!
        //----------------------------------------------------------------
        if(typ_presa==N_NO_FORCHE   ) continue;
        //-----------------------------------------------------------------
        // GESTIONE UDC
        // Come dati prodotto vengono usati quelli del primo UDC in presa
        // bassa, perchè comunque tutti i pallet di una presa deveono
        // essere uguali
        //-----------------------------------------------------------------
        udc_trovato=false;
        memset(&Udc, 0, sizeof(DATI_PRD));
        if(GestioneUdc!=0){
           for(i=0; i<POSINPRESA; i++){
              for(j=0; j<PIANIINPRESA; j++){
                 if(M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Presa[i][j]==false) continue;
                 //-------------------------------------------
                 // Verifico se UDC duplicato
                 //-------------------------------------------
                 k=i+(j*POSINPRESA);
                 memcpy(&Udc, &M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Prd[k], sizeof(DATI_PRD));
                 udc_trovato=true;
                 break;
              }
              if(udc_trovato==true) break;
           }
        }
        //-----------------------------------------------------------------
        // GESTIONE RICERCA BAIA DI DEPOSITO
        // Verifica disponibilità di almeno un magazzino completamente
        // vuoto con possibilità di deposito a terra
        //
        // NOTA: In caso si prevista la sovrapposizione pallet sulla
        //       rulliera, il magazzino da cercare deve essere
        //       predisposto alla sovrapposizione
        //-----------------------------------------------------------------
        lato_out_al_deposito=false;   // tentativo di scarico sempre l'ingresso magazzino
        //-----------------------------------------------------------------
        // Pallet da spostare raggiungibile cerco nel GRUPPO PREVISTO
        //-----------------------------------------------------------------
        if(pallet_non_raggiungibile==false){
           if(M.CercaMagRiempirePIENO(FALSE, FALSE, lato_out_al_deposito, FALSE, mag_prov_box, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) continue;
           if(num_mag==0) if( M.CercaMagRiempirePIENO(TRUE, FALSE, lato_out_al_deposito, FALSE, mag_prov_box, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) continue;
           //-----------------------------------------------------------------
           // MAGAZZINO DI DEPOSITO TROVATO
           //-----------------------------------------------------------------
           if(num_mag>0 && num_mag<=TOTMAG){
              //-----------------------------------------------------------------
              // Ricalcolo della chiamata in base al magazzino di destinazione
              // per assicurarsi che nessun altro TGV lo utilizzi
              //-----------------------------------------------------------------
              chiamata=0;
              TestNumChiamata(&chiamata, M.m[num_mag]->PuntoIng, num_mag);
              if(chiamata==0) return 0;     // chiamata sconosciuta
              //-----------------------------------------------------------------
              // verifica se la chiamata è già asservita da un AGV e l'altro AGV
              // è in missione attiva.
              //-----------------------------------------------------------------
              altro_agv = (short int)(test_chiamata_asservita(chiamata));
              if( altro_agv!=0 && altro_agv!=num_agv) continue;
           }
        }
        //-----------------------------------------------------------------
        // Per lo spostamento di pallet davanti a quelli non
        // raggiungibili viene spostato il pallet in file con la
        // logica di provenienza dalle baie
        //-----------------------------------------------------------------
        if(pallet_non_raggiungibile==true){
           //-----------------------------------------------------------------
           // SPOSTAMENTO CON RITORNO - AbilitaSvuotamentoManualeMag==3
           //
           // Ricerca magazzino temporaneo in attesa di riportare il pallet
           // bloccanti al magazzino sorgente
           //-----------------------------------------------------------------
           // 1. GESTIONE MAGAZZINI TEMPORANEI INCOMPLETI
           if(M.CercaMagTempDeposito(FALSE, 0, AbilitaControlloNodoDest, lato_out_al_deposito, FALSE, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
           //-----------------------------------------------------------------
           // 2. GESTIONE MAGAZZINI ACCOPPIATI
           // se trovo un magazzino incompleto verifico se è accoppiato ad
           // un altro e in caso che lo sia destino il TGV al magazzino che
           // dei due ha meno posizioni a terra impegnate
           //-----------------------------------------------------------------
           if(num_mag!=0){
              //-------------------------------------------------
              // Cerco magazzino accoppiato in struttura
              //-------------------------------------------------
              num_mag_accoppiato=0;
              for(i=0;;i++){
                 if(AccoppiaMagaz[i][0]==0 && AccoppiaMagaz[i][1]==0            ) break;
                 if(AccoppiaMagaz[i][0]!=num_mag && AccoppiaMagaz[i][1]!=num_mag) continue;
                 if(AccoppiaMagaz[i][0]==num_mag){num_mag_accoppiato=AccoppiaMagaz[i][1]; break;}
                 if(AccoppiaMagaz[i][1]==num_mag){num_mag_accoppiato=AccoppiaMagaz[i][0]; break;}
              }
              //-------------------------------------------------
              // Verifico se magazzino accoppiato è disponibile
              //-------------------------------------------------
              if(num_mag_accoppiato>0 && num_mag_accoppiato<=TOTMAG){
                 num_mag1=num_pin1=num_pos1=0;
                 if(M.CercaMagTempDeposito(FALSE, num_mag_accoppiato, AbilitaControlloNodoDest, lato_out_al_deposito, FALSE, grp_dest, &num_mag1, &num_pos1, &num_pin1, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
                 //-----------------------------------------------
                 // Se magazzino accoppiato è disponibile scelgo
                 // tra i due in coppia quello con meno postazioni
                 // a terra impegnate
                 //-----------------------------------------------
                 num_mag_accoppiato=0;
                 if(num_mag1!=0){
                    if(num_pos1>num_pos || (num_pos1==num_pos && num_pin1>num_pin)){
                       num_mag=num_mag1;
                       num_pos=num_pos1;
                       num_pin=num_pin1;
                       num_mag_accoppiato=num_mag1;
                    }
                 }
              }
           }
           // 2. GESTIONE MAGAZZINI TEMPORANEI LIBERI
           if(num_mag==0) if(M.CercaMagTempDeposito(TRUE, 0, AbilitaControlloNodoDest, lato_out_al_deposito, FALSE, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
           // 3. GESTIONE MAGAZZINI LIBERI
           if(num_mag==0) if(M.CercaMagDepositoPIENO(TRUE, 0, AbilitaControlloNodoDest, TRUE, lato_out_al_deposito, FALSE, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
        }
        //-----------------------------------------------------------------
        // INCOMPATIBILITA' DI ROTAZIONE PALLET DA PRELIEVO A DEPOSITO
        // Se manca il magazzino di deposito, ma il magazzino di prelievo
        // e FIFO (e quindi il pallet viene ruotato) verifico se prelevando
        // dal lato di deposito(quindi mantenendo la rotazione originale)
        // esiste il magazzino di deposito
        //-----------------------------------------------------------------
        if(M.m[mag_prel]->Fifo==true && cambia_rot==true && lato_ing_al_prelivo==false){
           if(num_mag==0 || (num_mag>0 && num_mag<=TOTMAG && M.m[num_mag]->PerPalRuotati==2)){
              cambia_rot=false;
              pos_spost=pin_spost=0;
              lato_ing_al_prelivo=true;
              mag_prel=pos_prel=pin_prel=0;
              pallet_non_raggiungibile=false;
              tipo_prov_box=mac_prov_box=bai_prov_box=mag_prov_box=0;
              M.CercaMagDaSvuotarePIENO(TRUE, lato_ing_al_prelivo, FALSE, tipo_mag, gruppo, num_agv, typ_presa, &mag_prel, &pos_prel, &pin_prel, &pos_spost, &pin_spost, &cambia_rot);
              if(mag_prel==0) M.CercaMagDaSvuotarePIENO(FALSE, lato_ing_al_prelivo, FALSE, tipo_mag, gruppo, num_agv, typ_presa, &mag_prel, &pos_prel, &pin_prel, &pos_spost, &pin_spost, &cambia_rot);
              if(mag_prel==0 && tipo_mag!=MG_TEMPORANEO){
                 //--------------------------------------------------------------
                 // Verifico se esiste una magazzino da svuotare con un pallet
                 // non raggiungibile
                 //--------------------------------------------------------------
                 M.CercaMagDaSvuotarePIENO(FALSE, lato_ing_al_prelivo, TRUE, NN, gruppo, num_agv, typ_presa, &mag_prel, &pos_prel, &pin_prel, &pos_spost, &pin_spost, &cambia_rot);
                 //--------------------------------------------------------------
                 // Setto memoria di pallet non raggiungibile
                 //--------------------------------------------------------------
                 if(mag_prel!=0) pallet_non_raggiungibile=true;
              }
              //--------------------------------------------------------------
              // GESTIONE SPOSTA E RITORNA:
              // In caso di questa gestione in cui il pallet dopo essere stato
              // spostato deve anche ritornare al magazzino di origine, è
              // necessario prelevare i pallet bloccanti dal lato di deposito
              //--------------------------------------------------------------
              if(mag_prel==0) continue;
              //--------------------------------------------------------------
              // MAGAZZINI TEMPORANEI RECUPERO MAGAZZINO DI ORIGINE
              // Trattandosi di un magazzino temporaneo recupero dalla
              // locazione di provenienza il magazzino di destinazione
              //--------------------------------------------------------------
              if(M.m[mag_prel]->Tipo==MG_TEMPORANEO){
                 //------------------------------------------------
                 // Decodifica provenieza
                 //------------------------------------------------
                 P.TestProvenienzaBox(M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].ProvBox, &tipo_prov_box, &mac_prov_box, &bai_prov_box, &mag_prov_box);
                 if(tipo_prov_box!=PROV_MAGAZZINO) continue;
                 if(mag_prov_box<1               ) continue;
                 if(mag_prov_box>TOTMAG          ) continue;
                 if(mag_prov_box==mag_prel       ) continue;
              }
              //-----------------------------------------------------------------
              // Inizializzazione della missione
              //-----------------------------------------------------------------
              PInt=0;
              Dest=0;
              Punto=0;
              PosDest=0;
              num_mag=0;
              num_pin=0;
              num_pos=0;
              chiamata=0;
              altro_agv=0;
              limitrofo=0;
              typ_presa=N_FORCA_AB;
              memset(&presa[0][0], false, sizeof(presa));
              provenienza = (short int)((10000*PROV_MAGAZZINO)+mag_prel);
              //-----------------------------------------------------------------
              // PARAMETRI SENSIBILI PER RICERCA MAGAZZINO DI DEPOSITO
              // Selezione tipo box richiesto/espulso dalla macchina
              //-----------------------------------------------------------------
              tipo_carico = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].StatoLav;
              tipo_box    = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].TipoBox;
              car_wash    = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].CarWash;
              codice      = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Codice;
              lotto       = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Lotto;
              basso       = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Basso;
              sovrap      = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Sovrap;
              grp_dest    = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].GruppoDest;
              rotaz       = M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Ruotato180;
              if(cambia_rot==true) rotaz=!rotaz;
              memcpy(&presa[0][0], &M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Presa[0][0], sizeof(presa));
              //-----------------------------------------------------------------
              // AL-20/06/2013 LE PRESE SONO TUTTE SOVRAPPONIBILI
              // tranne quelle con il flag di Car-Wash.
              // Il bit di "sovrapposizione" è importante per il
              // controllo dello stato della presa su cui sovraporre
              // che ovviamente deve essere ON
              //-----------------------------------------------------------------
              presa_sovrapponibile=sovrap;
              if(SovrapCarWash==0 && car_wash!=0) presa_sovrapponibile=false;
              //-----------------------------------------------------------------
              // Recupero il nodo del magazzino
              //-----------------------------------------------------------------
              Punto=M.m[mag_prel]->PuntoOut;
              if(lato_ing_al_prelivo==true) Punto=M.m[mag_prel]->PuntoIng;
              //-----------------------------------------------------------------
              // Composizione chiamata
              //-----------------------------------------------------------------
              if(tgv_libero==true){
                 TestNumChiamata(&chiamata, Punto, mag_prel);
                 if(chiamata==0) return 0;     // chiamata sconosciuta
                 //-----------------------------------------------------------------
                 // ORA CHE CONOSCO IL MAGAZZINO CERCO IL TGV PIU' VICINO
                 // 1 - CERCO UN TGV GIA' ABBINATO ALLA CHIAMATA
                 // 2 - CERCO UN TGV COMPLETAMENTE LIBERO (devono esserci entrambe
                 //     le presenze contemporaneamente)
                 //-----------------------------------------------------------------
                 if(CercaAgvLibero(FALSE, Punto, chiamata, CH_ESPULSIONE, ZONA_0_ALL, &num_agv, all_mess)) return 0;
                 if(num_agv==0){
                    if(CercaAgvLibero(TRUE, Punto, chiamata, CH_ESPULSIONE, ZONA_0_ALL, &num_agv, all_mess)) return 0;
                 }
              }
              //-----------------------------------------------------------------
              // Nessun TGV trovato
              //-----------------------------------------------------------------
              if(num_agv==0) continue;
              //-----------------------------------------------------------------
              // SE IL TGV E' COMPLETAMENTE LIBERO
              // Verifico la disponibilità del magazzino, per poi inizializzare
              // la missione di carico dalla baia
              //-----------------------------------------------------------------
              if(AGV[num_agv]->mission.PresCEST_A==LIBERA && AGV[num_agv]->mission.PresCEST_B==LIBERA){
                 //-----------------------------------------------------------------
                 // Selezione del TIPO DI PRESA
                 //
                 // GestioneDoppiaForcaTGV=0: forca singola carico singolo
                 //
                 // GestioneDoppiaForcaTGV=1: doppia forca con singolo carico su
                 //                           ogni forca
                 // GestioneDoppiaForcaTGV=2: forca singola con doppio carico
                 //-----------------------------------------------------------------
                 typ_presa=N_NO_FORCHE;
                 if(GestioneDoppiaForcaTGV==0 && AGV[num_agv]->stato.s.bit.carB==false) typ_presa=N_FORCA_B;
                 if(GestioneDoppiaForcaTGV==1){
                    if(AGV[num_agv]->stato.s.bit.carB==false && AGV[num_agv]->stato.s.bit.carA==true ) typ_presa=N_FORCA_B;
                    if(AGV[num_agv]->stato.s.bit.carB==false && AGV[num_agv]->stato.s.bit.carA==false) typ_presa=N_FORCA_A;
                 }
                 //-----------------------------------------------------------------
                 // GestioneDoppiaForcaTGV=2: forca singola con doppio carico
                 // TEMPORANEAMENTE NON AMMESSA IN ATTESA DI IMPIANTO SU CUI
                 // IMPLEMENTARLA
                 //-----------------------------------------------------------------
                 if(GestioneDoppiaForcaTGV==2) continue;
              }
              //----------------------------------------------------------------
              // Manca tipo presa --> skip!
              //----------------------------------------------------------------
              if(typ_presa==N_NO_FORCHE   ) continue;
              //-----------------------------------------------------------------
              // GESTIONE UDC
              // Come dati prodotto vengono usati quelli del primo UDC in presa
              // bassa, perchè comunque tutti i pallet di una presa deveono
              // essere uguali
              //-----------------------------------------------------------------
              udc_trovato=false;
              memset(&Udc, 0, sizeof(DATI_PRD));
              if(GestioneUdc!=0){
                 for(i=0; i<POSINPRESA; i++){
                    for(j=0; j<PIANIINPRESA; j++){
                       if(M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Presa[i][j]==false) continue;
                       //-------------------------------------------
                       // Verifico se UDC duplicato
                       //-------------------------------------------
                       k=i+(j*POSINPRESA);
                       memcpy(&Udc, &M.m[mag_prel]->Pos[pos_prel].Box[pin_prel].Prd[k], sizeof(DATI_PRD));
                       udc_trovato=true;
                       break;
                    }
                    if(udc_trovato==true) break;
                 }
              }
              //-----------------------------------------------------------------
              // GESTIONE RICERCA BAIA DI DEPOSITO
              // Verifica disponibilità di almeno un magazzino completamente
              // vuoto con possibilità di deposito a terra
              //
              // NOTA: In caso si prevista la sovrapposizione pallet sulla
              //       rulliera, il magazzino da cercare deve essere
              //       predisposto alla sovrapposizione
              //-----------------------------------------------------------------
              lato_out_al_deposito=false;   // tentativo di scarico sempre l'ingresso magazzino
              //-----------------------------------------------------------------
              // Pallet da spostare raggiungibile cerco nel GRUPPO PREVISTO
              //-----------------------------------------------------------------
              if(pallet_non_raggiungibile==false){
                 if(M.CercaMagRiempirePIENO(FALSE, FALSE, lato_out_al_deposito, FALSE, mag_prov_box, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) continue;
                 if(num_mag==0) if( M.CercaMagRiempirePIENO(TRUE, FALSE, lato_out_al_deposito, FALSE, mag_prov_box, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) continue;
                 //-----------------------------------------------------------------
                 // MAGAZZINO DI DEPOSITO TROVATO
                 //-----------------------------------------------------------------
                 if(num_mag>0 && num_mag<=TOTMAG){
                    //-----------------------------------------------------------------
                    // Ricalcolo della chiamata in base al magazzino di destinazione
                    // per assicurarsi che nessun altro TGV lo utilizzi
                    //-----------------------------------------------------------------
                    chiamata=0;
                    TestNumChiamata(&chiamata, M.m[num_mag]->PuntoIng, num_mag);
                    if(chiamata==0) return 0;     // chiamata sconosciuta
                    //-----------------------------------------------------------------
                    // verifica se la chiamata è già asservita da un AGV e l'altro AGV
                    // è in missione attiva.
                    //-----------------------------------------------------------------
                    altro_agv = (short int)(test_chiamata_asservita(chiamata));
                    if( altro_agv!=0 && altro_agv!=num_agv) continue;
                 }
              }
              //-----------------------------------------------------------------
              // Per lo spostamento di pallet davanti a quelli non
              // raggiungibili viene spostato il pallet in file con la
              // logica di provenienza dalle baie
              //-----------------------------------------------------------------
              if(pallet_non_raggiungibile==true){
                 //-----------------------------------------------------------------
                 // SPOSTAMENTO CON RITORNO - AbilitaSvuotamentoManualeMag==3
                 //
                 // Ricerca magazzino temporaneo in attesa di riportare il pallet
                 // bloccanti al magazzino sorgente
                 //-----------------------------------------------------------------
                 // 1. GESTIONE MAGAZZINI TEMPORANEI INCOMPLETI
                 if(M.CercaMagTempDeposito(FALSE, 0, AbilitaControlloNodoDest, lato_out_al_deposito, FALSE, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
                 //-----------------------------------------------------------------
                 // 2. GESTIONE MAGAZZINI ACCOPPIATI
                 // se trovo un magazzino incompleto verifico se è accoppiato ad
                 // un altro e in caso che lo sia destino il TGV al magazzino che
                 // dei due ha meno posizioni a terra impegnate
                 //-----------------------------------------------------------------
                 if(num_mag!=0){
                    //-------------------------------------------------
                    // Cerco magazzino accoppiato in struttura
                    //-------------------------------------------------
                    num_mag_accoppiato=0;
                    for(i=0;;i++){
                       if(AccoppiaMagaz[i][0]==0 && AccoppiaMagaz[i][1]==0            ) break;
                       if(AccoppiaMagaz[i][0]!=num_mag && AccoppiaMagaz[i][1]!=num_mag) continue;
                       if(AccoppiaMagaz[i][0]==num_mag){num_mag_accoppiato=AccoppiaMagaz[i][1]; break;}
                       if(AccoppiaMagaz[i][1]==num_mag){num_mag_accoppiato=AccoppiaMagaz[i][0]; break;}
                    }
                    //-------------------------------------------------
                    // Verifico se magazzino accoppiato è disponibile
                    //-------------------------------------------------
                    if(num_mag_accoppiato>0 && num_mag_accoppiato<=TOTMAG){
                       num_mag1=num_pin1=num_pos1=0;
                       if(M.CercaMagTempDeposito(FALSE, num_mag_accoppiato, AbilitaControlloNodoDest, lato_out_al_deposito, FALSE, grp_dest, &num_mag1, &num_pos1, &num_pin1, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
                       //-----------------------------------------------
                       // Se magazzino accoppiato è disponibile scelgo
                       // tra i due in coppia quello con meno postazioni
                       // a terra impegnate
                       //-----------------------------------------------
                       num_mag_accoppiato=0;
                       if(num_mag1!=0){
                          if(num_pos1>num_pos || (num_pos1==num_pos && num_pin1>num_pin)){
                             num_mag=num_mag1;
                             num_pos=num_pos1;
                             num_pin=num_pin1;
                             num_mag_accoppiato=num_mag1;
                          }
                       }
                    }
                 }
                 // 2. GESTIONE MAGAZZINI TEMPORANEI LIBERI
                 if(num_mag==0) if(M.CercaMagTempDeposito(TRUE, 0, AbilitaControlloNodoDest, lato_out_al_deposito, FALSE, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
                 // 3. GESTIONE MAGAZZINI LIBERI
                 if(num_mag==0) if(M.CercaMagDepositoPIENO(TRUE, 0, AbilitaControlloNodoDest, TRUE, lato_out_al_deposito, FALSE, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
              }
           }
        }
        //-----------------------------------------------------------------
        // manca magazzino di deposito
        //-----------------------------------------------------------------
        if(num_mag==0) continue;
        //-----------------------------------------------------------------
        // Ora che conosco il Magazzino di Deposito cerco il Tgv più vicino
        // al Mag Deposito in modo tale da intercettare l'eventuale Tgv in
        // uscita dopo il deposito precedente.
        // E' più importante il TGV vicino al Mag. Deposito che quello vicino
        // al Mag Prelievo.
        // 1 - CERCO UN TGV GIA' ABBINATO ALLA CHIAMATA
        // 2 - CERCO UN TGV COMPLETAMENTE LIBERO (devono esserci entrambe
        //     le presenze contemporaneamente)
        //
        // NOTA: solo se magazzino occupato per dare priortà al TGV che
        //       ha fatto l'ultimo spostamento
        //-----------------------------------------------------------------
        err = test_dest_occupata(M.m[num_mag]->PuntoIng, &altro_agv);
        if(err!=0 && altro_agv!=0 && num_agv!=altro_agv){
           new_agv=altro_agv;
           if(AGV[altro_agv]->test_mission_on()            ) new_agv=0;
           if(AGV[altro_agv]->test_agv_escluso()           ) new_agv=0;
           if(AGV[altro_agv]->mission.SemiAutomatico==true ) new_agv=0;
           if(AGV[altro_agv]->stato.s.bit.ris              ) new_agv=0;
           //----------------------------------
           // Nuovo TGV trovato
           //----------------------------------
           if(new_agv!=0) num_agv = new_agv;
        }
        //-----------------------------------------------------------------
        // Procedo a prelevare
        //-----------------------------------------------------------------
        PInt    = 0;
        Dest    = Punto;
        PosDest = pos_prel;
        //-----------------------------------------------------------------
        // Verifico se la destinazione è impegnata da un altro TGV
        //-----------------------------------------------------------------
        if(P.test_dest_limitrofe(Dest, &altro_agv, &limitrofo, num_agv) && altro_agv!=0 && altro_agv!=num_agv) return 0;
        if(test_dest_riservata(Dest, &altro_agv, num_agv) && altro_agv!=0 && altro_agv!=num_agv) return 0;
        err = test_dest_occupata(Dest, &altro_agv);
        if((err!=0) && (num_agv!=altro_agv)) return 0;
        //-----------------------------------------------------------------
        // Inizializzazione missione verso linea
        //-----------------------------------------------------------------
        if(new_init_mission(FALSE, CALCOLA_TIPO_PATH, num_agv, chiamata, Dest, PosDest, PInt, MISS_CARICO_DA_MAGAZZINO, typ_presa, &Ok, all_mess)) return 1;
     }
  }
  return 0;
}

// --------------------------------------------------------
//   AgvConCaricoDestMagazzino()
// --------------------------------------------------------
//   Ritorna 1 se c'è errore
//
int process::AgvConCaricoDestMagazzino(char *all_mess)
/********************************************/
{
  int  Pos;
  bool Ok=false;
  bool trovato;
  bool last_pallet;
  bool udc_trovato;
  bool lato_uscita;
  bool tgv_in_uscita;
  bool disp_alla_sovrapp;
  bool presa_sovrapponibile;
  bool altro_tgv_con_stessa_prov;
  bool deposito_nel_piu_vicino = (bool)(DEPOSITO_NEL_PIU_VICINO);
  int  provenienza, chiamata;
  int lotto, lottoA, lottoB;
  short int num_mag_accoppiato;
  short int tipo_prov, mac_prov, bai_prov, mag_prov;
  short int tipo_prov_box, mac_prov_box, bai_prov_box, mag_prov_box;
  short int tipo_carico, tipo_caricoA, tipo_caricoB;
  short int tipo_box, tipo_boxA, tipo_boxB;
  short int prov_box, prov_boxA, prov_boxB;
  short int codice, codiceA, codiceB;
  short int altro_num_mag_sorg, altro_num_pos_sorg;
  short int altro_num_mag_dest, altro_num_pos_dest;
  short int car_wash, car_washA, car_washB;
  short int grp_dest, grp_destA, grp_destB;
  bool sposta_pallet;
  bool riempi_mag_ch;
  bool basso, bassoA, bassoB;
  bool rotaz, rotazA, rotazB;
  bool presa[POSINPRESA+1][PIANIINPRESA+1];
  bool presaA[POSINPRESA+1][PIANIINPRESA+1];
  bool presaB[POSINPRESA+1][PIANIINPRESA+1];
  short int Dest, PInt;
  short int num_agv, altro_agv;
  short int bai_ch, mac_ch, mag_ch, pos_ch;
  short int num_mag, num_pos, num_pin;
  short int num_mag1, num_pos1, num_pin1;
  short int typ_presa, limitrofo;
  short int i, j, k, z;
  struct MAG mag;
  struct DATI_PRD Udc;
  struct DATI_PRD Udc0[MAXUDCBOX+1];
  struct DATI_PRD UdcA[MAXUDCBOX+1];
  struct DATI_PRD UdcB[MAXUDCBOX+1];

  //----------------------------------------------------------
  // Recupero dati Box indispensabili per individuare il MAG
  //----------------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     num_agv=0;
     if(AGV[i]->test_mission_on()           ) continue;
     if(AGV[i]->mission.SemiAutomatico==true) continue;
     if(AGV[i]->mission.Esclusione==true    ) continue;
     //----------------------------------------------------------
     // Almeno una forca impegnata
     //----------------------------------------------------------
     if((AGV[i]->mission.PresCEST_A==LIBERA && !AGV[i]->stato.s.bit.carA) &&
        (AGV[i]->mission.PresCEST_B==LIBERA && !AGV[i]->stato.s.bit.carB)) continue;
     //----------------------------------------------------------
     // Recupero i dati principali per la ricerca del magazzino
     //----------------------------------------------------------
     num_agv = i;
     lato_uscita=false;
     sposta_pallet=false;
     riempi_mag_ch=false;
     num_mag=num_pin=0;
     limitrofo=altro_agv=0;
     memset(&Udc0[0], 0, sizeof(Udc0));
     memset(&UdcA[0], 0, sizeof(UdcA));
     memset(&UdcB[0], 0, sizeof(UdcB));
     provenienza  = AGV[i]->mission.Provenienza;
     tipo_caricoA = AGV[i]->mission.PresCEST_A;
     tipo_caricoB = AGV[i]->mission.PresCEST_B;
     prov_boxA    = AGV[i]->mission.BoxA.ProvBox;
     prov_boxB    = AGV[i]->mission.BoxB.ProvBox;
     tipo_boxA    = AGV[i]->mission.BoxA.TipoBox;
     tipo_boxB    = AGV[i]->mission.BoxB.TipoBox;
     codiceA      = AGV[i]->mission.BoxA.Codice;
     codiceB      = AGV[i]->mission.BoxB.Codice;
     lottoA       = AGV[i]->mission.BoxA.Lotto;
     lottoB       = AGV[i]->mission.BoxB.Lotto;
     car_washA    = AGV[i]->mission.BoxA.CarWash;
     car_washB    = AGV[i]->mission.BoxB.CarWash;
     grp_destA    = AGV[i]->mission.BoxA.GruppoDest;
     grp_destB    = AGV[i]->mission.BoxB.GruppoDest;
     bassoA       = AGV[i]->mission.BoxA.Basso;
     bassoB       = AGV[i]->mission.BoxB.Basso;
     rotazA       = AGV[i]->mission.BoxA.Ruotato180;
     rotazB       = AGV[i]->mission.BoxB.Ruotato180;
     memcpy(&UdcA[0], &AGV[i]->mission.BoxA.Prd[0], sizeof(UdcA));
     memcpy(&UdcB[0], &AGV[i]->mission.BoxB.Prd[0], sizeof(UdcB));
     memcpy(&presaA[0][0], &AGV[i]->mission.BoxA.Presa[0][0], sizeof(presaA));
     memcpy(&presaB[0][0], &AGV[i]->mission.BoxB.Presa[0][0], sizeof(presaB));
     //----------------------------------------------------------
     // 05/01/2012 Solo se si trova sotto magazzino
     //----------------------------------------------------------
     Pos=AGV[i]->stato.pos;
     if( Pos<=TOTPUNTI ){
        if(N.punti_notevoli[Pos]==false) altro_tgv_con_stessa_prov=false;
     }
     if(altro_tgv_con_stessa_prov==true) continue;
     //----------------------------------------------------------
     // Seleziono il tipo di presa
     //----------------------------------------------------------
     tipo_carico = LIBERA;
     typ_presa   = N_NO_FORCHE;
     prov_box    = NN;
     tipo_box    = 0;
     codice      = 0;
     lotto       = 0;
     car_wash    = 0;
     basso       = 0;
     rotaz       = 0;
     grp_dest    = NN;
     memset(&presa[0][0], false, sizeof(presa));
     // ENTRAMBE LE FORCHE + STESSO TIPO (o GestioneDoppiaForcaTGV=2 => carico singolo reale)
     if(tipo_caricoA!=LIBERA && tipo_caricoB!=LIBERA && (tipo_caricoA==tipo_caricoB || GestioneDoppiaForcaTGV==2)){
        //-------------------------------------------------------
        // GESTIONE DOPPIA FORCA
        // 1 - doppia forca + doppio carico: scarico A+B solo
        //     se stesso tipo di supporto
        // 2 - forca singola + carico doppio: scarico A+B sempre
        //-------------------------------------------------------
        if(GestioneDoppiaForcaTGV==2 ) typ_presa = N_FORCA_AB;
        else{
           if(tipo_boxA==tipo_boxB) typ_presa = N_FORCA_AB;
           else                     typ_presa = N_FORCA_B;
        }
        //-------------------------------------------------------
        // Dati per deposito
        //-------------------------------------------------------
        tipo_carico = tipo_caricoB;
        prov_box    = prov_boxB;
        tipo_box    = tipo_boxB;
        codice      = codiceB;
        lotto       = lottoB;
        car_wash    = car_washB;
        grp_dest    = grp_destB;
        basso       = bassoB;
        rotaz       = rotazB;
        memcpy(&Udc0[0], &UdcB[0], sizeof(Udc0));
        memcpy(&presa[0][0], &presaB[0][0], sizeof(presa));
     }
     // ENTRAMBE LE FORCHE + TIPO DIVERSO
     if(tipo_caricoA!=LIBERA && tipo_caricoB!=LIBERA && tipo_caricoA!=tipo_caricoB){
        tipo_carico = tipo_caricoB;
        typ_presa   = N_FORCA_B;
        prov_box    = prov_boxB;
        tipo_box    = tipo_boxB;
        codice      = codiceB;
        lotto       = lottoB;
        car_wash    = car_washB;
        grp_dest    = grp_destB;
        basso       = bassoB;
        rotaz       = rotazB;
        memcpy(&Udc0[0], &UdcB[0], sizeof(Udc0));
        memcpy(&presa[0][0], &presaB[0][0], sizeof(presa));
     }
     // SOLO FORCA "A"
     if(tipo_caricoA!=LIBERA && tipo_caricoB==LIBERA){
        typ_presa   = N_FORCA_A;
        tipo_carico = tipo_caricoA;
        prov_box    = prov_boxA;
        tipo_box    = tipo_boxA;
        codice      = codiceA;
        lotto       = lottoA;
        car_wash    = car_washA;
        grp_dest    = grp_destA;
        basso       = bassoA;
        rotaz       = rotazA;
        memcpy(&Udc0[0], &UdcA[0], sizeof(Udc0));
        memcpy(&presa[0][0], &presaA[0][0], sizeof(presa));
     }
     // SOLO FORCA "B"
     if(tipo_caricoA==LIBERA && tipo_caricoB!=LIBERA){
        tipo_carico = tipo_caricoB;
        typ_presa   = N_FORCA_B;
        prov_box    = prov_boxB;
        tipo_box    = tipo_boxB;
        codice      = codiceB;
        lotto       = lottoB;
        car_wash    = car_washB;
        grp_dest    = grp_destB;
        basso       = bassoB;
        rotaz       = rotazB;
        memcpy(&Udc0[0], &UdcB[0], sizeof(Udc0));
        memcpy(&presa[0][0], &presaB[0][0], sizeof(presa));
     }
     //----------------------------------------------------------
     // Decodifica provenienza
     //----------------------------------------------------------
     tipo_prov=mac_prov=bai_prov=mag_prov=0;
     P.TestProvenienzaBox(provenienza, &tipo_prov, &mac_prov, &bai_prov, &mag_prov);
     //----------------------------------------------------------
     // SPOSTAMENTI: ammesse chiamata da magazzino
     //----------------------------------------------------------
     if(AGV[i]->mission.Chiamata!=0 && mag_prov>0 && mag_prov<=TOTMAG){
        //-------------------------------------------------------
        // Ammesse solo Chiamate di MAGAZZINI
        //-------------------------------------------------------
        bai_ch=mac_ch=mag_ch=pos_ch=0;
        P.TestChiamataNum(AGV[i]->mission.Chiamata, &mag_ch, &bai_ch, &mac_ch, &pos_ch);
        if(mag_ch==0) continue;
        //-------------------------------------------------------
        // MAGAZZINO CHIAMANTE == MAGAZZINO PROVENIENZA
        // si tratta di spostare un pallet bloccante
        //-------------------------------------------------------
        if(mag_prov==mag_ch && grp_dest==M.m[mag_prov]->Gruppo) sposta_pallet=true;
        //-------------------------------------------------------
        // MAGAZZINO CHIAMANTE <> MAGAZZINO PROVENIENZA
        // il magazzino chiamante rappresenta la destinazione
        //-------------------------------------------------------
        if(mag_prov!=mag_ch) riempi_mag_ch=true;
     }
     //--------------------------------------------------------
     // Se il TGV ha già a bordo un carico doppio il magazzino
     // cercato deve essere obbligatoriamente abilitato alla
     // sovrapposizione
     //--------------------------------------------------------
     disp_alla_sovrapp=false;
     if(typ_presa==N_FORCA_AB) disp_alla_sovrapp=true;
     //--------------------------------------------------------
     // AL-20/06/2013 LE PRESE SONO TUTTE SOVRAPPONIBILI
     // tranne quelle con il flag di Car-Wash.
     // Il bit di "sovrapposizione" è importante per il
     // controllo dello stato della presa su cui sovraporre
     // che ovviamente deve essere ON
     //--------------------------------------------------------
     presa_sovrapponibile=true;
     if(car_wash==true) presa_sovrapponibile=false;
     //-----------------------------------------------------------------
     // GESTIONE UDC
     // Come dati prodotto vengono usati quelli del primo UDC in presa
     // bassa, perchè comunque tutti i pallet di una presa deveono
     // essere uguali
     //-----------------------------------------------------------------
     udc_trovato=false;
     memset(&Udc, 0, sizeof(DATI_PRD));
     if(GestioneUdc!=0){
        for(z=0; z<POSINPRESA; z++){
           for(j=0; j<PIANIINPRESA; j++){
              if(presa[z][j]==false) continue;
              //-------------------------------------------
              // Verifico se UDC duplicato
              //-------------------------------------------
              k=z+(j*POSINPRESA);
              memcpy(&Udc, &Udc0[k], sizeof(DATI_PRD));
              udc_trovato=true;
              break;
           }
           if(udc_trovato==true) break;
        }
     }
     //--------------------------------------------------------
     // VERIFICA DISPONIBILITA' MAGAZZINO
     // deposito nei magazzini abbinati
     //--------------------------------------------------------
     lato_uscita=false;
     // 1. CERCA MAG DEPOSITO DA BAIA
     if(sposta_pallet==false && riempi_mag_ch==false){
        //-----------------------------------------------------------------
        // GESTIONE RICERCA MAGAZZINO DI DEPOSITO
        // 1. MAGAZZINI INCOMPLETI
        // 2. MAGAZZINI ACCOPPIATI
        // 3. MAGAZZINI LIBERI
        // NOTA: In caso si prevista la sovrapposizione pallet sulla
        //       rulliera, il magazzino da cercare deve essere
        //       predisposto alla sovrapposizione
        //-----------------------------------------------------------------
        lato_uscita=false;   // tentativo di scarico sempre l'ingresso magazzino
        // 1. GESTIONE MAGAZZINI INCOMPLETI
        if(M.CercaMagDepositoPIENO(FALSE, 0, FALSE, deposito_nel_piu_vicino, lato_uscita, disp_alla_sovrapp, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
        //-----------------------------------------------------------------
        // 2. GESTIONE MAGAZZINI ACCOPPIATI
        // se trovo un magazzino incompleto verifico se è accoppiato ad
        // un altro e in caso che lo sia destino il TGV al magazzino che
        // dei due ha meno posizioni a terra impegnate
        //-----------------------------------------------------------------
        if(num_mag!=0){
           //-------------------------------------------------
           // Cerco magazzino accoppiato in struttura
           //-------------------------------------------------
           num_mag_accoppiato=0;
           for(j=0;;j++){
              if(AccoppiaMagaz[j][0]==0 && AccoppiaMagaz[j][1]==0            ) break;
              if(AccoppiaMagaz[j][0]!=num_mag && AccoppiaMagaz[j][1]!=num_mag) continue;
              if(AccoppiaMagaz[j][0]==num_mag){num_mag_accoppiato=AccoppiaMagaz[j][1]; break;}
              if(AccoppiaMagaz[j][1]==num_mag){num_mag_accoppiato=AccoppiaMagaz[j][0]; break;}
           }
           //-------------------------------------------------
           // Verifico se magazzino accoppiato è disponibile
           //-------------------------------------------------
           if(num_mag_accoppiato>0 && num_mag_accoppiato<=TOTMAG){
              num_mag1=num_pin1=num_pos1=0;
              if(M.CercaMagDepositoPIENO(FALSE, num_mag_accoppiato, FALSE, deposito_nel_piu_vicino, lato_uscita, disp_alla_sovrapp, grp_dest, &num_mag1, &num_pos1, &num_pin1, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
              //-----------------------------------------------
              // Se magazzino accoppiato è disponibile scelgo
              // tra i due in coppia quello con meno postazioni
              // a terra impegnate
              //-----------------------------------------------
              num_mag_accoppiato=0;
              if(num_mag1!=0){
                 if(num_pos1>num_pos || (num_pos1==num_pos && num_pin1>num_pin)){
                    num_mag=num_mag1;
                    num_pos=num_pos1;
                    num_pin=num_pin1;
                    num_mag_accoppiato=num_mag1;
                 }
              }
           }
        }
        // 2. GESTIONE MAGAZZINI LIBERI
        if(num_mag==0) M.CercaMagDepositoPIENO(TRUE, 0, FALSE, deposito_nel_piu_vicino, lato_uscita, disp_alla_sovrapp, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess);
     }
     // 2. CERCA MAG DEPOSITO PER RIEMPIMENTO MANUALE
     if(sposta_pallet==false && riempi_mag_ch==true){
        tipo_prov_box=mac_prov_box=bai_prov_box=mag_prov_box=0;
        if(mag_prov>0 && mag_prov<=TOTMAG && prov_box!=NN){
           //---------------------------------------------------------
           // MAGAZZINI TEMPORANEI (AbilitaSvuotamentoManualeMag==2)
           // Se non è previsto il ritorno del pallet al magazzino
           // di origine, e il TGV è in fase di spostamento di un
           // pallet alla destinazione finale...libero tutti i
           // magazzini temporanei con pallet provenienti da quel
           // mag di provenienza
           //---------------------------------------------------------
           if(provenienza==prov_box && AbilitaSvuotamentoManualeMag==2 && DisattivazioneManualeMagTmp==false){
              if(grp_dest!=0 && M.m[mag_prov]->Gruppo!=grp_dest){
                 //-----------------------------------------------------
                 // Verifico se nel magazzino di provenienza ci sono
                 // altri pallet da spostare
                 //-----------------------------------------------------
                 last_pallet=true;
                 for(k=1; k<=MAXTERRAMAG; k++){
                    for(z=MAXPIANIMAG-1; z>=0; z--){
                       if(M.m[mag_prov]->Pos[k].Presenza[z]==false                      ) continue;
                       if(M.m[mag_prov]->Pos[k].Box[z].GruppoDest==M.m[mag_prov]->Gruppo) continue;
                       last_pallet=false;
                       break;
                    }
                    if(last_pallet==false) break;
                 }
                 //-----------------------------------------------------
                 // ULTIMO PALLET DA SPOSTARE
                 // Libero il magazzino temporaneo
                 //-----------------------------------------------------
                 if(last_pallet==true){
                    for(j=1; j<=TOTMAG; j++){
                       if(M.m[j]->Num<1              ) continue;
                       if(M.m[j]->Num>TOTMAG         ) continue;
                       if(M.m[j]->Num==mag_prov      ) continue;
                       if(M.m[j]->Tipo!=MG_TEMPORANEO) continue;
                       if(M.m[j]->NumCestoni==0      ) continue;
                       //-------------------------------------
                       // Verifico se i pallet provengono
                       // dal magazzino di orgine
                       //-------------------------------------
                       for(k=1; k<=MAXTERRAMAG; k++){
                          for(z=MAXPIANIMAG-1; z>=0; z--){
                             if(M.m[j]->Pos[k].Presenza[z]==false      ) continue;
                             if(M.m[j]->Pos[k].Box[z].ProvBox!=prov_box) continue;
                             M.m[j]->Tipo=MG_PROD_TUTTI;
                             break;
                          }
                          if(M.m[j]->Tipo==MG_PROD_TUTTI) break;
                       }
                       //-------------------------------------
                       // Salvataggio variazione tipo mag
                       //-------------------------------------
                       if(M.m[j]->Tipo==MG_PROD_TUTTI){
                          if(M.save_record_mag(M.m[j]->Num, all_mess)) break;
                       }
                    }
                 }
              }
           }
           //---------------------------------------------------------
           // MAGAZZINI TEMPORANEI
           // Riempimento magazzino di origine
           //--------------------------------------------------------
           if(provenienza!=prov_box && AbilitaSvuotamentoManualeMag==3){
              //------------------------------------------------
              // Decodifica provenieza
              //------------------------------------------------
              P.TestProvenienzaBox(prov_box, &tipo_prov_box, &mac_prov_box, &bai_prov_box, &mag_prov_box);
              if(tipo_prov_box!=PROV_MAGAZZINO) continue;
              if(mag_prov_box<1               ) continue;
              if(mag_prov_box>TOTMAG          ) continue;
              if(mag_prov_box==mag_prov       ) continue;
           }
        }
        if(M.CercaMagRiempirePIENO(FALSE, TRUE, lato_uscita, FALSE, mag_prov_box, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) continue;
        if(num_mag==0) if( M.CercaMagRiempirePIENO(TRUE, TRUE, lato_uscita, FALSE, mag_prov_box, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) continue;
     }
     // 3. CERCA MAG DEPOSITO PER DEPOSITO TEMPORANEO
     if(sposta_pallet==true && riempi_mag_ch==false){
        //-----------------------------------------------------------------
        // SPOSTAMENTO CON RITORNO - AbilitaSvuotamentoManualeMag==3
        //
        // Ricerca magazzino temporaneo in attesa di riportare il pallet
        // bloccanti al magazzino sorgente
        //-----------------------------------------------------------------
        // 1. GESTIONE MAGAZZINI TEMPORANEI INCOMPLETI
        if(M.CercaMagTempDeposito(FALSE, 0, TRUE, lato_uscita, FALSE, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
        //-----------------------------------------------------------------
        // 2. GESTIONE MAGAZZINI ACCOPPIATI
        // se trovo un magazzino incompleto verifico se è accoppiato ad
        // un altro e in caso che lo sia destino il TGV al magazzino che
        // dei due ha meno posizioni a terra impegnate
        //-----------------------------------------------------------------
        if(num_mag!=0){
           //-------------------------------------------------
           // Cerco magazzino accoppiato in struttura
           //-------------------------------------------------
           num_mag_accoppiato=0;
           for(j=0;;j++){
              if(AccoppiaMagaz[j][0]==0 && AccoppiaMagaz[j][1]==0            ) break;
              if(AccoppiaMagaz[j][0]!=num_mag && AccoppiaMagaz[j][1]!=num_mag) continue;
              if(AccoppiaMagaz[j][0]==num_mag){num_mag_accoppiato=AccoppiaMagaz[j][1]; break;}
              if(AccoppiaMagaz[j][1]==num_mag){num_mag_accoppiato=AccoppiaMagaz[j][0]; break;}
           }
           //-------------------------------------------------
           // Verifico se magazzino accoppiato è disponibile
           //-------------------------------------------------
           if(num_mag_accoppiato>0 && num_mag_accoppiato<=TOTMAG){
              num_mag1=num_pin1=num_pos1=0;
              if(M.CercaMagTempDeposito(FALSE, num_mag_accoppiato, TRUE, lato_uscita, FALSE, grp_dest, &num_mag1, &num_pos1, &num_pin1, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
              //-----------------------------------------------
              // Se magazzino accoppiato è disponibile scelgo
              // tra i due in coppia quello con meno postazioni
              // a terra impegnate
              //-----------------------------------------------
              num_mag_accoppiato=0;
              if(num_mag1!=0){
                 if(num_pos1>num_pos || (num_pos1==num_pos && num_pin1>num_pin)){
                    num_mag=num_mag1;
                    num_pos=num_pos1;
                    num_pin=num_pin1;
                    num_mag_accoppiato=num_mag1;
                 }
              }
           }
        }
        // 2. GESTIONE MAGAZZINI TEMPORANEI LIBERI
        if(num_mag==0) if(M.CercaMagTempDeposito(TRUE, 0, TRUE, lato_uscita, FALSE, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
        // 3. GESTIONE MAGAZZINI LIBERI
        if(num_mag==0) if(M.CercaMagDepositoPIENO(TRUE, 0, TRUE, TRUE, lato_uscita, FALSE, grp_dest, &num_mag, &num_pos, &num_pin, provenienza, num_agv, typ_presa, tipo_carico, tipo_box, presa, codice, lotto, car_wash, basso, rotaz, &Udc, presa_sovrapponibile, all_mess)) return 0;
     }
     //--------------------------------------------------------
     // non è stato trovato ---> Skip!!!
     //--------------------------------------------------------
     if(num_mag==0) continue;
     //--------------------------------------------------------
     // Se il magazzino trovato coincide con quello chiamante
     // devo ripassare la chiamata al TGV
     //--------------------------------------------------------
     chiamata=0;
     if((mag_ch!=0 && num_mag==mag_ch) || AGV[num_agv]->mission.Chiamata==provenienza){
        //----------------------------------------------------------------
        // Se prelevo da Magazzino, salvo in chiamata il Mag. Sorgente
        // per evitare che altri TGV eseguano la medesima operazione
        //----------------------------------------------------------------
        chiamata=provenienza;
     }
     //--------------------------------------------------------
     // DESTINAZIONE MAGAZZINO
     // Recupero dati magazzini selezionato
     //--------------------------------------------------------
     if(num_mag!=0){
        memset(&mag, 0, sizeof(MAG));
        if(M.read_mag(num_mag, &mag, all_mess)) continue;
        //---------------------------------------------------------------
        // Dati di missione
        //---------------------------------------------------------------
        PInt=0;
        if(lato_uscita==false) Dest=mag.PuntoIng;
        if(lato_uscita==true ) Dest=mag.PuntoOut;
        //---------------------------------------------------------------
        // MAGAZZINO SENZA POSSIBILITA' DI SOVRAPPOSIZIONE
        // Se il magazzino non è abilitato alla sovrapposizione e ho la
        // forca doppia impegnata deposito un cestone alla volta partendo
        // dal basso
        //
        // NOTA: se la forca è singola, ma con doppio carico procedero
        //       con il deposito di entrambi i cestoni per poi ricaricare
        //       quello in alto
        //---------------------------------------------------------------
        if(GestioneDoppiaForcaTGV==1){
           if(typ_presa>=N_FORCA_AB && mag.Sovrapposizione==0) typ_presa = N_FORCA_B;
        }
        //-----------------------------------------------------------
        // Controllo se magazzino già impeganto da altro TGV
        //-----------------------------------------------------------
        if(P.test_dest_occupata(Dest, &altro_agv) && altro_agv!=0 && altro_agv!=num_agv){
           //-------------------------------------------------------------
           // Se l'altro TGV è in uscita dal magazzino, il magazzino non
           // è da considerare occupato.
           //-------------------------------------------------------------
           tgv_in_uscita=false;
           altro_num_mag_sorg=altro_num_pos_sorg=0;
           M.test_punto_presente_mag(all_mess, AGV[altro_agv]->mission.pstart, &altro_num_mag_sorg, &altro_num_pos_sorg);
           altro_num_mag_dest=altro_num_pos_dest=0;
           M.test_punto_presente_mag(all_mess, AGV[altro_agv]->mission.pend, &altro_num_mag_dest, &altro_num_pos_dest);
           //-------------------------------------------------------
           // ALTRO AGV con partenza magazzino destinazione del
           // TGV considerato, e destinazione differente
           //-------------------------------------------------------
           if(altro_num_mag_sorg==num_mag && altro_num_mag_dest!=num_mag) tgv_in_uscita=true;
           if( tgv_in_uscita==false ) continue;   //###
        }
        //-----------------------------------------------------------
        // Destinazione riservata da altro TGV
        //-----------------------------------------------------------
        if(P.test_dest_riservata(Dest, &altro_agv, num_agv) && altro_agv!=0 && altro_agv!=num_agv){
           //-------------------------------------------------------------
           // Se l'altro TGV è in uscita dal magazzino, il magazzino non
           // è da considerare occupato.
           //-------------------------------------------------------------
           tgv_in_uscita=false;
           altro_num_mag_sorg=altro_num_pos_sorg=0;
           M.test_punto_presente_mag(all_mess, AGV[altro_agv]->mission.pstart, &altro_num_mag_sorg, &altro_num_pos_sorg);
           altro_num_mag_dest=altro_num_pos_dest=0;
           M.test_punto_presente_mag(all_mess, AGV[altro_agv]->mission.pend, &altro_num_mag_dest, &altro_num_pos_dest);
           //-------------------------------------------------------
           // ALTRO AGV con partenza magazzino destinazione del
           // TGV considerato, e destinazione differente
           //-------------------------------------------------------
           if(altro_num_mag_sorg==num_mag && altro_num_mag_dest!=num_mag) tgv_in_uscita=true;
           if( tgv_in_uscita==false ) continue;   //###
        }
        //-----------------------------------------------------------
        // TGV su nodo limitrofo
        //-----------------------------------------------------------
        if(P.test_dest_limitrofe(Dest, &altro_agv, &limitrofo, num_agv) && altro_agv!=0 && altro_agv!=num_agv) continue;
        //---------------------------------------------------------------
        // Inizializzazione della missione
        //---------------------------------------------------------------
        if(new_init_mission(FALSE, CALCOLA_TIPO_PATH, num_agv, chiamata, Dest, num_pos, PInt, MISS_SCARICO_SU_MAGAZZINO, typ_presa, &Ok, all_mess)) continue;
     }
  }
  return 0;
}


// --------------------------------------------------------
//     CercaAgvLibero()
// --------------------------------------------------------
// cerca un AGV libero più vicino al "punt_dest". Se lo trova lo
// restituisce in "num_agv", altrimenti restituisce 0.
// In caso di errore ritorna 1.
//
int process::CercaAgvLibero(bool tutto, short int dest, int chiamata, short int tipo_chiamata, short int zona, short int *num_agv, char *all_mess)
/**************************************************************************/
{
  int err=0;
  short int i, j, k;
  short int num_rot, new_dist, old_dist;
  short int gruppo_baia, num_baia, num_mac, num_mag, num_stz;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  bool trovato[MAXAGV+1];
  bool asservimento_ok;
  bool CercaAgvVicino;
  long int OldTimeInitAttesa;
  BYTE OldPercCaricaBatt;

  *num_agv=0;
  old_dist=9999;
  num_rot=new_dist=0;
  OldPercCaricaBatt=0;
  OldTimeInitAttesa=time(NULL)+63244800L;          // Forza al valore di oggi + 2ANNO  memset(&elenco_mag, 0, sizeof(elenco_mag));
  memset(&nodi_perc, 0, sizeof(nodi_perc));
  memset(&dati_perc, 0, sizeof(dati_perc));
  //-----------------------------------------------------------------
  // Scompongo la chiamata per verificare l'asservimento
  //-----------------------------------------------------------------
  num_baia=num_mac=num_mag=num_stz=0;
  if(chiamata>MAXGRUPPIIMP) P.TestChiamataNum(chiamata, &num_mag, &num_baia, &num_mac, &num_stz);
  //-----------------------------------------------------------------
  // Se ci sono le condizioni per liberare i TGV dalle associazione
  // alle zone ripeto i controlli
  //-----------------------------------------------------------------
  memset(&trovato, false, sizeof(trovato));
  for(i=1; i<=MAXAGV; i++){
     trovato[i]=false;
     if(AGV[i]->test_agv_escluso()                            ) continue;
     if(AGV[i]->mission.SemiAutomatico==true                  ) continue;
     if(AGV[i]->test_mission_on()                             ) continue;
     if(AGV[i]->stato.s.bit.ris && AGV[i]->mission.Chiamata==0) continue;
     //----------------------------------------------------------------
     // Controllo se il TGV si trova sulla zona richiesta
     //----------------------------------------------------------------
     if(zona!=ZONA_0_ALL && AGV[i]->mission.Zona!=zona        ) continue;
     //----------------------------------------------------------------
     // AL-29/11/2019 scarto navette in errore comunicazione
     //----------------------------------------------------------------
     if(AGV[i]->allarme_interno==ALL_INT_ERR_COMUNIC          ) continue;
     //----------------------------------------------------------------
     // (1) - TUTTO LIBERO    = entrambe le forche disponibili
     // (2) - ALMENO UN FORCA = la forca alta non è utilizzabile se
     //                         la bassa è già impegnata
     //                         Nb: IL TGV DEVE GIA' ESSERE ABBINATO
     //                             ALLA CHIAMATA
     //----------------------------------------------------------------
     // - (1) -
     if(tutto==true){
        if(AGV[i]->mission.Chiamata!=0 && AGV[i]->mission.Chiamata!=chiamata) continue;
        if((AGV[i]->mission.PresCEST_A!=LIBERA && AGV[i]->stato.s.bit.carA) ||
           (AGV[i]->mission.PresCEST_B!=LIBERA && AGV[i]->stato.s.bit.carB) ) continue;
     }
     // - (2) -
     else{
        if(AGV[i]->mission.Chiamata!=chiamata                               ) continue;
        if(AGV[i]->mission.PresCEST_B!=LIBERA && AGV[i]->stato.s.bit.carB   ) continue;
     }
     //----------------------------------------------------------------
     // Verifico asservimenti TGV
     //----------------------------------------------------------------
     asservimento_ok=false;
     if(chiamata!=0 && AGV[i]->mission.Chiamata==0){
        // CHIAMATA DA MACCHINA
        if(num_mac>0 && num_mac<=TOTCHIAMATE){
           for(j=1; j<=TOTCHIAMATE; j++){
              if(AGV[i]->mission.AsservMac[j]!=num_mac       ) continue;
              if(AGV[i]->mission.TipoAssMac[j]!=ASS_MAC_TUTTE){
                 if(AGV[i]->mission.TipoAssMac[j]==ASS_MAC_SOLO_ING && tipo_chiamata!=CH_RICHIESTA ) continue;
                 if(AGV[i]->mission.TipoAssMac[j]==ASS_MAC_SOLO_OUT && tipo_chiamata!=CH_ESPULSIONE) continue;
              }
              asservimento_ok=true;
              break;
           }
        }
        // CHIAMATA DA BAIA
        if(num_baia>0 && num_baia<=TOTBAIE){
           gruppo_baia = ba[num_baia]->Gruppo;
           if(gruppo_baia<1           ) continue; // nessun gruppo abbinato alla baia
           if(gruppo_baia>MAXGRUPPIIMP) continue; // nessun gruppo abbinato alla baia
           for(j=0; j<MAXGRUPPIIMP; j++){
              if(AGV[i]->mission.AsservBaie[j]!=gruppo_baia) continue;
              asservimento_ok=true;
              break;
           }
        }
        // CHIAMATA DA MAGAZZINO
        if(num_mag>0 && num_mag<=TOTMAG){
           gruppo_baia = M.m[num_mag]->Gruppo;
           if(gruppo_baia<1           ) continue; // nessun gruppo abbinato alla baia
           if(gruppo_baia>MAXGRUPPIIMP) continue; // nessun gruppo abbinato alla baia
           for(j=0; j<MAXGRUPPIIMP; j++){
              if(AGV[i]->mission.AsservBaie[j]!=gruppo_baia) continue;
              asservimento_ok=true;
              break;
           }
        }
        // CHIAMATA COINCIDE CON I GRUPPO
        if(num_mag==0 && num_mac==0 && num_baia==0){
           gruppo_baia = chiamata;
           if(gruppo_baia<1           ) continue; // nessun gruppo abbinato alla baia
           if(gruppo_baia>MAXGRUPPIIMP) continue; // nessun gruppo abbinato alla baia
           for(j=0; j<MAXGRUPPIIMP; j++){
              if(AGV[i]->mission.AsservBaie[j]!=gruppo_baia) continue;
              asservimento_ok=true;
              break;
           }
        }
        if(asservimento_ok==false) continue;
     }
     //----------------------------------------------------------------
     // CHIAMATA==0 | CHIAMATA=CHIAMATA AGV --> asserivmento sempre OK
     //----------------------------------------------------------------
     else{
        asservimento_ok=true;
     }
     //----------------------------------------------------------------
     // Asservimento non compatibile --> skip!
     //----------------------------------------------------------------
     if(asservimento_ok==false) continue;
     //----------------------------------------------------------------
     // TGV trovato!
     //----------------------------------------------------------------
     trovato[i]=true;
  }

  //------------------------------------------------
  // Ricerca del TGV più vicino NON IN RICARICA
  //------------------------------------------------
  if(PrioritaCercaAgvLiberoNONRicarica==true){
     new_dist=0;
     old_dist=9999;
     for(i=1; i<=MAXAGV; i++){
        //---------------------------------------------------------
        // Se il TGV è ha riposo skip!
        //---------------------------------------------------------
        if(AGV[i]->mission.Chiamata==0 && AGV[i]->stato.pos==AGV[i]->mission.PosRicarica && N.punti_notevoli[AGV[i]->mission.PosRicarica]==true) continue;
        //---------------------------------------------------------
        // verifica AGV senza missione e non escluso
        //---------------------------------------------------------
        if(trovato[i] && (!AGV[i]->test_mission_on()) && (!AGV[i]->test_agv_escluso())){
           //------------------------------------------------------
           // se il TGV ha già la chiamata riservata alla macchina,
           // allora ha precedenza sugli altri.
           //------------------------------------------------------
           if(chiamata>MAXGRUPPIIMP && AGV[i]->mission.Chiamata==chiamata){
              *num_agv = i;
              return 0;
           }
           //------------------------------------------------------
           // Cerco l'AGV più vicino sono se DESTINAZIONE VALIDA
           //------------------------------------------------------
           if(dest<1 || dest>TOTPUNTI){*num_agv=i; return 0;}
           else{
              memset(&nodi_perc, 0, sizeof(nodi_perc));
              memset(&dati_perc, 0, sizeof(dati_perc));
              err = AGV[i]->distanza(FAST_PATH, 0, dest, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
              if(new_dist<MAXPERCORSI && (new_dist < old_dist) && !err){
                 old_dist = new_dist;
                 *num_agv = i;
              }
           }

        }
     }
  }
  //------------------------------------------------
  // Ricerca del TGV più vicino OVUNQUE
  //------------------------------------------------
  new_dist=0;
  old_dist=9999;
  CercaAgvVicino=true;
  if(*num_agv==0){           // TGV GIA' TROVATO
     for(i=1; i<=MAXAGV; i++){
        //---------------------------------------------------------
        // verifica AGV senza missione e non escluso
        //---------------------------------------------------------
        if(trovato[i] && (!AGV[i]->test_mission_on()) && (!AGV[i]->test_agv_escluso())){
           //------------------------------------------------------
           // se il TGV ha già la chiamata riservata alla macchina,
           // allora ha precedenza sugli altri.
           //------------------------------------------------------
           if(chiamata>MAXGRUPPIIMP && AGV[i]->mission.Chiamata==chiamata){
              *num_agv = i;
              return 0;
           }
           //------------------------------------------------------
           // Cerco l'AGV più vicino sono se DESTINAZIONE VALIDA
           //------------------------------------------------------
           if(dest<1 || dest>TOTPUNTI){*num_agv=i; return 0;}
           //------------------------------------------------------
           // Partenza da posizioni di riposo (SPECIALIZZATA)
           //------------------------------------------------------
           if(AGV[i]->stato.pos<51){
              //----------------------------------------------------------------
              // (1) RICERCA TGV IN RICARICA DA PIU' TEMPO
              //----------------------------------------------------------------
              if(CercaAgvLiberoInRicarica==1 && AGV[i]->mission.TimeInitAttesa!=0){
                 CercaAgvVicino=false;
                 //---------------------------------------------
                 // trova la distanza pi— breve se pi— di 1 AGV
                 //---------------------------------------------
                 if(AGV[i]->mission.TimeInitAttesa<OldTimeInitAttesa){
                    *num_agv = i;
                    OldTimeInitAttesa=AGV[i]->mission.TimeInitAttesa;
                 }
              }
              //----------------------------------------------------------------
              // (2) RICERCA TGV IN RICARICA CON PERCENTUALE BATTERIA MAGGIORE
              //----------------------------------------------------------------
              if(CercaAgvLiberoInRicarica==2 && (AGV[i]->mission.PercCaricaBatt!=0 || AGV[i]->mission.TimeInitAttesa!=0)){
                 CercaAgvVicino=false;
                 //---------------------------------------------
                 // trova la distanza pi— breve se pi— di 1 AGV
                 //---------------------------------------------
                 if(AGV[i]->mission.PercCaricaBatt!=0){
                    if(AGV[i]->mission.PercCaricaBatt>OldPercCaricaBatt){
                       *num_agv = i;
                       OldPercCaricaBatt=AGV[i]->mission.PercCaricaBatt;
                    }
                 }
                 if(AGV[i]->mission.PercCaricaBatt==0 && OldPercCaricaBatt==0){
                    if(AGV[i]->mission.TimeInitAttesa<OldTimeInitAttesa){
                       *num_agv = i;
                       OldTimeInitAttesa=AGV[i]->mission.TimeInitAttesa;
                    }
                 }
              }
           }
           //------------------------------------------------------
           // RICERCA TGV IN RICARICA PIU' VICINA
           //------------------------------------------------------
           if(CercaAgvVicino==true){
              //---------------------------------------------
              // trova la distanza pi— breve se pi— di 1 AGV
              //---------------------------------------------
              memset(&nodi_perc, 0, sizeof(nodi_perc));
              memset(&dati_perc, 0, sizeof(dati_perc));
              err = AGV[i]->distanza(FAST_PATH, 0, dest, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
              if(new_dist<MAXPERCORSI && (new_dist < old_dist) && !err){
                 old_dist = new_dist;
                 *num_agv = i;
              }
           }
        }
     }
  }
  return 0;
}

// --------------------------------------------------------
//     CercaAgvLiberoPerReset()
// -------------------------------------------------------- */
// cerca un AGV libero più vicino al "punt_dest". Se lo trova lo
// restituisce in "num_agv", altrimenti restituisce 0.
// In caso di errore ritorna 1.
//
int process::CercaAgvLiberoPerReset(bool tutto, short int dest, int chiamata, short int tipo_chiamata, short int zona, short int *num_agv, char *all_mess)
/**************************************************************************/
{
  int err=0;
  int rot1, rot2;
  bool asservimento_ok;
  short int i, j, gruppo_baia;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  short int num_baia, num_mac, num_mag, num_stz;
  short int new_dist, num_rot, altro_agv, old_dist, oldagv;
  bool pint_non_valido;
  bool vicino_occupato;
  bool trovato[MAXAGV+1];
  short int punto_arrivo;
  short int MinPercToStart, MinPercorsi;
  int Dist, Vicino, xv, yv, x1, y1;
  double CalcolaRad, CatetoX, CatetoY;

  *num_agv=0;
  old_dist=9999;
  MinPercorsi=0;
  gruppo_baia=NN;
  new_dist=num_rot=altro_agv=0;
  memset(&trovato,   0, sizeof(trovato));
  memset(&nodi_perc, 0, sizeof(nodi_perc));
  memset(&dati_perc, 0, sizeof(dati_perc));
  //-------------------------------------------------
  // RESET MISSIONI AL VOLO
  //-------------------------------------------------
  if(AbilitaResetMissioniAlVolo==0) return 0;
  //-------------------------------------------------
  // Solo per destinazioni valide
  //-------------------------------------------------
  if(dest<1 || dest>TOTPUNTI      ) return 0;
  //-------------------------------------------------
  // Scompongo chiamata per verificare l'asservimento
  //-------------------------------------------------
  num_baia=num_mac=num_mag=num_stz=0;
  if(chiamata>MAXGRUPPIIMP) P.TestChiamataNum(chiamata, &num_mag, &num_baia, &num_mac, &num_stz);
  //------------------------------------
  // verifica se chiamata già asservita
  //------------------------------------
  oldagv = P.test_chiamata_asservita(chiamata);
  if( oldagv >0 ){
     //------------------------------------------
     // In caso di abilitazione avvicinamento
     // a destinazione, è ammesso il cambio
     // missione al volo se l'altro AGV ha
     // ancora il PIntForzato
     //------------------------------------------
     if(AbilitaAvvicinamentoADest!=0){
        if(AGV[oldagv]->mission.PIntForzato==0) return 0;
     }
     else return 0;
  }

  //-------------------------------------------------
  // Ricerca di un TGV Libero disponibile
  //-------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     trovato[i]=false;
     if(AGV[i]->mission.SemiAutomatico==true                    ) continue;
     if(AGV[i]->stato.s.bit.ris                                 ) continue;
     if(AGV[i]->stato.s.bit.allarme || AGV[i]->allarme_interno  ) continue;
     if(AGV[i]->test_agv_escluso()                              ) continue;
     if(!AGV[i]->test_mission_on()                              ) continue; // solo per TGV in missione
     //----------------------------------------------------------------
     // TGV GIA' ABBINATO A CHIAMATA --> skip!
     //----------------------------------------------------------------
     if(AGV[i]->mission.Chiamata!=0                             ) continue;
     //----------------------------------------------------------------
     // TGV con RESET AL VOLO già programmato
     //----------------------------------------------------------------
     if(AGV[i]->mission.ResetAlVolo==true                       ) continue;
     if(AGV[i]->mission.PIntForzato!=0                          ) continue;
     //----------------------------------------------------------------
     // AL-29/11/2019 scarto navette in errore comunicazione
     //----------------------------------------------------------------
     if(AGV[i]->allarme_interno==ALL_INT_ERR_COMUNIC            ) continue;
     //----------------------------------------------------------------
     // Controllo se il TGV si trova sulla zona richiesta
     //----------------------------------------------------------------
     if(zona!=ZONA_0_ALL && AGV[i]->mission.Zona!=zona          ) continue;
     //----------------------------------------------------------------
     // (1) - TUTTO LIBERO    = entrambe le forche disponibili
     // (2) - ALMENO UN FORCA = la forca alta non è utilizzabile se
     //                         la bassa è già impegnata
     //                         Nb: IL TGV DEVE GIA' ESSERE ABBINATO
     //                             ALLA CHIAMATA
     //----------------------------------------------------------------
     // - (1) -
     if(tutto==true){
        if(AGV[i]->mission.Chiamata!=0 && AGV[i]->mission.Chiamata!=chiamata) continue;
        if((AGV[i]->mission.PresCEST_A!=LIBERA && AGV[i]->stato.s.bit.carA) ||
           (AGV[i]->mission.PresCEST_B!=LIBERA && AGV[i]->stato.s.bit.carB) ) continue;
     }
     // - (2) -
     else{
        if(AGV[i]->mission.Chiamata!=chiamata                               ) continue;
        if(AGV[i]->mission.PresCEST_B!=LIBERA && AGV[i]->stato.s.bit.carB   ) continue;
     }
     //----------------------------------------------------------------
     // Verifico asservimenti TGV
     //----------------------------------------------------------------
     asservimento_ok=false;
     if(chiamata!=0 && AGV[i]->mission.Chiamata==0){
        // CHIAMATA DA MACCHINA
        if(num_mac>0 && num_mac<=TOTCHIAMATE){
           for(j=1; j<=TOTCHIAMATE; j++){
              if(AGV[i]->mission.AsservMac[j]!=num_mac       ) continue;
              if(AGV[i]->mission.TipoAssMac[j]!=ASS_MAC_TUTTE){
                 if(AGV[i]->mission.TipoAssMac[j]==ASS_MAC_SOLO_ING && tipo_chiamata!=CH_RICHIESTA ) continue;
                 if(AGV[i]->mission.TipoAssMac[j]==ASS_MAC_SOLO_OUT && tipo_chiamata!=CH_ESPULSIONE) continue;
              }
              asservimento_ok=true;
              break;
           }
        }
        // CHIAMATA DA BAIA
        if(num_baia>0 && num_baia<=TOTBAIE){
           gruppo_baia = ba[num_baia]->Gruppo;
           if(gruppo_baia<1           ) continue; // nessun gruppo abbinato alla baia
           if(gruppo_baia>MAXGRUPPIIMP) continue; // nessun gruppo abbinato alla baia
           for(j=0; j<MAXGRUPPIIMP; j++){
              if(AGV[i]->mission.AsservBaie[j]!=gruppo_baia) continue;
              asservimento_ok=true;
              break;
           }
        }
        // CHIAMATA DA BAIA
        if(num_mag>0 && num_mag<=TOTMAG){
           gruppo_baia = M.m[num_mag]->Gruppo;
           if(gruppo_baia<1           ) continue; // nessun gruppo abbinato alla baia
           if(gruppo_baia>MAXGRUPPIIMP) continue; // nessun gruppo abbinato alla baia
           for(j=0; j<MAXGRUPPIIMP; j++){
              if(AGV[i]->mission.AsservBaie[j]!=gruppo_baia) continue;
              asservimento_ok=true;
              break;
           }
        }
        // CHIAMATA COINCIDE CON I GRUPPO
        if(num_mag==0 && num_mac==0 && num_baia==0){
           gruppo_baia = chiamata;
           if(gruppo_baia<1           ) continue; // nessun gruppo abbinato alla baia
           if(gruppo_baia>MAXGRUPPIIMP) continue; // nessun gruppo abbinato alla baia
           for(j=0; j<MAXGRUPPIIMP; j++){
              if(AGV[i]->mission.AsservBaie[j]!=gruppo_baia) continue;
              asservimento_ok=true;
              break;
           }
        }
        if(asservimento_ok==false) continue;
     }
     //----------------------------------------------------------------
     // CHIAMATA==0 | CHIAMATA=CHIAMATA AGV --> asserivmento sempre OK
     //----------------------------------------------------------------
     else{
        asservimento_ok=true;
     }
     //----------------------------------------------------------------
     // Asservimento non compatibile --> skip!
     //----------------------------------------------------------------
     if(asservimento_ok==false) continue;
     //------------------------------------------------------------------
     // Non ammessi PINT su punti notevoli
     //------------------------------------------------------------------
     if(N.punti_notevoli[AGV[i]->mission.pintend]==true ) continue;
     //------------------------------------------------------------------
     // Solo se non ho già occupato fino a dest
     //------------------------------------------------------------------
     if(AGV[i]->mission.pintend==AGV[i]->mission.pend   ) continue;
     //------------------------------------------------------------------
     // IMPORTANTE: non ammesso il reset missione al volo di un
     //             TGV in attesa per precedenza (SICUREZZA)
     //------------------------------------------------------------------
     if(AGV[i]->mission.pintend==AGV[i]->stato.pos      ) continue;
     //------------------------------------------------------------------
     // NOTA: Se la prossima missione prevede proseguio in rettilineo
     //       sono ammessi anche i nodi di blocco rotazione a 180°
     //------------------------------------------------------------------
     rot1=99;
     for(j=0; j<MAXPERCORSI; j++){
        if(AGV[i]->mission.punto[j+1]==0) break;
        if(AGV[i]->mission.punto[j]!=AGV[i]->mission.pintend) continue;
        rot1 = AGV[i]->rot_standard(j);
        break;
     }
     //------------------------------------------------------------------
     // Verifico rotaz. su PRIMO NODO NUOVA MISSIONE
     //------------------------------------------------------------------
     rot2=99;
     memset(&nodi_perc, 0, sizeof(nodi_perc));
     memset(&dati_perc, 0, sizeof(dati_perc));
     err = AGV[i]->distanza(FAST_PATH, AGV[i]->mission.pintend, dest, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
     if(!err && new_dist<=MAXPERCORSI && (new_dist < old_dist)){
        rot2 = dati_perc[0].rot;
     }
     //------------------------------------------------------------------
     // Rotazione non ammessa su nodo
     //------------------------------------------------------------------
     if(rot1==99) continue;
     if(rot2==99) continue;
     //------------------------------------------------------------------
     // RESET MISSIONE
     //
     // AbilitaResetMissioniAlVolo==1
     // Ammesso anche punti in blocco_rot_180, ma non verrà fatto il
     // reset chiamata al momento dell'annullamento per essere sicuri
     // che il TGV prosegua nella direzione del calcolata
     //
     // AbilitaResetMissioniAlVolo==2
     // Ammesso solo in punto senza blocco_rot_180, e verrà fatto anche
     // reset chiamata al momento dell'annullamento missione per essere
     // sicuri per consentire al tgv di scegliere totalmente una nuova
     // missione
     //------------------------------------------------------------------
     if(AbilitaResetMissioniAlVolo==2 && N.n[AGV[i]->mission.pintend]->blocco_rot==true) continue;
     //------------------------------------------------------------------
     // Variazione di rotazione su primo nodo di
     // RESET MISSIONE AL VOLO
     //------------------------------------------------------------------
     if(rot1!=rot2){
        //---------------------------------------------
        // Solo TGV con PINT non su blocco rotazione
        //---------------------------------------------
        if(N.n[AGV[i]->mission.pintend]->blocco_rot==true) continue;
        //---------------------------------------------
        // !!! IMPORTANTE !!!
        // Se sul primo nodo è prevista una rotazione
        // non ci devono essere nodi occupati in una
        // DIST_MIN_ROT_DOPPIA_MAX
        //---------------------------------------------
        vicino_occupato=false;
        x1 = Punti[ AGV[i]->mission.pintend ][0];
        y1 = Punti[ AGV[i]->mission.pintend ][1];
        for(Vicino=51; Vicino<=TOTPUNTI; Vicino++){
           if(N.n[ Vicino ]->num<1             ) continue;
           if(N.n[ Vicino ]->num>TOTPUNTI      ) continue;
           if(N.nodo_busy[Vicino]==0           ) continue;
           if(N.nodo_busy[Vicino]==i           ) continue;
           if(AGV[i]->mission.pintend==Vicino  ) continue;
           if(AGV[i]->stato.pos!=Vicino        ) continue;
           if(N.punti_notevoli[Vicino]==true &&
              N.pt_in_ingombro[Vicino]==false  ) continue;
           //---------------------------------------------
           // Estrai le coordinate del nodo VICINO e se
           // DISTANZA > distanza minima --> CONTINUA
           //---------------------------------------------
           xv = Punti[ Vicino ][0];
           yv = Punti[ Vicino ][1];
           CatetoX = (int) (x1-xv);
           CatetoY = (int) (y1-yv);
           if(CatetoX>30000 || CatetoY>30000) continue;
           CalcolaRad = (CatetoX*CatetoX) + (CatetoY*CatetoY);
           Dist = (int) sqrt( CalcolaRad );
           if( Dist>=DIST_MIN_ROT_DOPPIA_MAX ) continue;
           vicino_occupato=true;
           break;
        }
        if(vicino_occupato==true) continue;
     }
     //------------------------------------------------------------------
     // Non ammessi PINT su punti inizio/fine deviazione o curva
     //------------------------------------------------------------------
     if(NoPIntSuStartDeviazione==true || NoPIntSuStartCurva==true || CurveDiBezier==0){
        pint_non_valido=false;
        for(j=1; j<MAXPERCORSI; j++){
           if(pint_non_valido==true                              ) break;
           if(AGV[i]->mission.punto[j]==0                        ) break;
           if(AGV[i]->mission.punto[j]==AGV[i]->mission.pend     ) break;
           if(AGV[i]->mission.punto[j]!=AGV[i]->mission.pintend  ) continue;
           // DEVIAZIONE = ROT>49 && ROT<70
           if(NoPIntSuStartDeviazione==true){
              if(AGV[i]->mission.dati_perc[j  ].rot>49 && AGV[i]->mission.dati_perc[j  ].rot<70) pint_non_valido=true;
              if(AGV[i]->mission.dati_perc[j-1].rot>49 && AGV[i]->mission.dati_perc[j-1].rot<70) pint_non_valido=true;
           }
           // CURVA = ROT>3 && ROT<40
           if(NoPIntSuStartCurva==true || CurveDiBezier==0){
              if(AGV[i]->mission.dati_perc[j  ].rot>3 && AGV[i]->mission.dati_perc[j  ].rot<40 ) pint_non_valido=true;
              if(AGV[i]->mission.dati_perc[j-1].rot>3 && AGV[i]->mission.dati_perc[j-1].rot<40 ) pint_non_valido=true;
           }
           break;  // solo non intermedio
        }
        if(pint_non_valido==true) continue;
     }
     //------------------------------------------------------------------
     // TGV TROVATO!!!
     //------------------------------------------------------------------
     trovato[i]=true;
  }
  //------------------------------------------------
  // Ricerca del TGV più vicino e con percorso
  // disponibile
  //------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     //---------------------------------------------
     // verifica AGV senza missione e non escluso
     //---------------------------------------------
     if(!trovato[i]) continue;
     //---------------------------------------------
     // trova la distanza più breve se più di 1 AGV
     //---------------------------------------------
     err = AGV[i]->distanza(FAST_PATH, AGV[i]->mission.pintend, dest, &nodi_perc[0], &dati_perc[0], all_mess, &new_dist, &num_rot);
     if(!err && new_dist<=MAXPERCORSI && (new_dist < old_dist)){
        //---------------------------------------------
        // Valuto se esiste il percorso disponibile
        //---------------------------------------------
        AGV[i]->test_percorso_disponibile(&nodi_perc[0], &dati_perc[0], &altro_agv, &MinPercorsi, &MinPercToStart, &punto_arrivo, all_mess);
        if(MinPercorsi>=MinPercToStart){
           old_dist = new_dist;
           *num_agv = i;
        }
     }
  }
  //------------------------------------------------
  // TGV TROVATO
  //
  // Setto già il PINT FORZATO E ABBINO LA CHIAMATA
  //------------------------------------------------
  if(*num_agv>0 && *num_agv<=MAXAGV){
     AGV[*num_agv]->mission.PIntForzato = AGV[*num_agv]->mission.pintend;
     AGV[*num_agv]->mission.Chiamata    = chiamata;
     AGV[*num_agv]->mission.ResetAlVolo = true;
     //------------------------------------------------------------------
     // file traccia
     //------------------------------------------------------------------
     sprintf(all_mess, "(1) - TGV%02d PROGRAMMA RESET SU PINT:%04d CH:%03d", *num_agv, AGV[*num_agv]->mission.PIntForzato, chiamata);
     file_cronologico_generico("LOGRESET.TXT", all_mess, 50000L);
  }
  return 0;
}

// --------------------------------------------------------
//     CercaAgvConCaricoPIENO()
// --------------------------------------------------------
// cerca un AGV libero pi— vicino al "punt_dest". Se lo trova lo
// restituisce in "num_agv", altrimenti restituisce 0.
// In caso di errore ritorna 1.
//
int process::CercaAgvConCaricoPIENO(bool tutto, short int dest, int chiamata, short int tipo_prov_box, short int gruppo_dest_box, short int *num_agv, char *all_mess)
/**************************************************************************/
{
  int err=0;
  short int i, j;
  int prov_boxTGV;
  short int gruppo_baia, gruppo_dest_boxTGV;
  short int tipo_prov_tgv, mac_prov_tgv, bai_prov_tgv, mag_prov_tgv;
  short int num_baia, num_mag, num_mac, num_stz;
  short int num_rot, new_dist, old_dist;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  bool trovato[MAXAGV+1];
  bool asservimento_ok;

  *num_agv=0;
  old_dist=9999;
  num_rot=new_dist=0;
  memset(&trovato, false, sizeof(trovato));
  memset(&nodi_perc, 0, sizeof(nodi_perc));
  memset(&dati_perc, 0, sizeof(dati_perc));
  //-----------------------------------------------------------------
  // Scompongo la chiamata per verificare l'asservimento
  //-----------------------------------------------------------------
  num_baia=num_mag=num_mac=num_stz=0;
  if(chiamata!=0) P.TestChiamataNum(chiamata, &num_mag, &num_baia, &num_mac, &num_stz);
  //-----------------------------------------------------------------
  // Se ci sono le condizioni per liberare i TGV dalle associazione
  // alle zone ripeto i controlli
  //-----------------------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(AGV[i]->test_agv_escluso()                            ) continue;
     if(AGV[i]->mission.SemiAutomatico==true                  ) continue;
     if(AGV[i]->test_mission_on()                             ) continue;
     if((!AGV[i]->stato.s.bit.carA && !AGV[i]->mission.PresCEST_A) &&
        (!AGV[i]->stato.s.bit.carB && !AGV[i]->mission.PresCEST_B)) continue;
     //----------------------------------------------------------------
     // Verifico asservimenti TGV
     // SOLO PER RICERCHE SENZA ID BOX SPECIFICO
     //----------------------------------------------------------------
     if(chiamata!=0 && AGV[i]->mission.Chiamata==0){
        // CHIAMATA DA BAIA
        if(num_baia>0 && num_baia<=TOTBAIE){
           gruppo_baia = ba[num_baia]->Gruppo;
           if(gruppo_baia<1           ) continue; // nessun gruppo abbinato alla baia
           if(gruppo_baia>MAXGRUPPIIMP) continue; // nessun gruppo abbinato alla baia
           for(j=0; j<MAXGRUPPIIMP; j++){
              if(AGV[i]->mission.AsservBaie[j]!=gruppo_baia) continue;
              asservimento_ok=true;
              break;
           }
        }
        if(asservimento_ok==false) continue;
     }
     //----------------------------------------------------------------
     // NELLA RICERCA DEI TGV CON CARICO DEVO ESSERCI ABBINAMENO ALLA
     // CHIAMATA
     //----------------------------------------------------------------
     if(chiamata!=0 && AGV[i]->mission.Chiamata!=0 && AGV[i]->mission.Chiamata!=chiamata) continue;
     //----------------------------------------------------------------
     // CERCO TGV COMPLETO
     // Entrambe le forche devono avere il tipo di cestone richiesto
     //----------------------------------------------------------------
     if(tutto==true){
        //--------------------------------------------------
        // (0) GESTIONE CONTROLLO SOLO FORCA "B"
        // (1) GESTIONE FORCA DOPPIA  CON CARICO DOPPIO
        // (2) GESTIONE FORCA SINGOLA CON CARICO DOPPIO
        //--------------------------------------------------
        if(GestioneDoppiaForcaTGV!=1){
           if(AGV[i]->mission.PresCEST_B==LIBERA     ) continue;
           if(AGV[i]->mission.PresCEST_B==PRES_VUOTO ) continue;
        }
        else{
           if(AGV[i]->mission.PresCEST_A==LIBERA     || AGV[i]->mission.PresCEST_B==LIBERA    ) continue;
           if(AGV[i]->mission.PresCEST_A==PRES_VUOTO || AGV[i]->mission.PresCEST_B==PRES_VUOTO) continue;
           if(AGV[i]->mission.PresCEST_A!=AGV[i]->mission.PresCEST_B                          ) continue;
        }
     }
     //----------------------------------------------------------------
     // CERCO TGV INCOMPLETO
     // (non sono ammessi i TGV completi del tipo cassone richiesto)
     // Basta che una delle abbia tipo di cestone richiesto
     // (ovviamente se si tratta della forca "A" la "B" deve essere
     //  libera)
     //----------------------------------------------------------------
     else{
        if(AGV[i]->mission.PresCEST_A!=LIBERA && AGV[i]->mission.PresCEST_B!=LIBERA        ) continue;
        if(AGV[i]->mission.PresCEST_B!=LIBERA && AGV[i]->mission.PresCEST_B==PRES_VUOTO    ) continue;
        if(AGV[i]->mission.PresCEST_B==LIBERA){
           if(AGV[i]->mission.PresCEST_A!=LIBERA && AGV[i]->mission.PresCEST_A==PRES_VUOTO ) continue;
        }
     }
     //----------------------------------------------------------------
     // CONTROLLO DATI
     // è necessario il controllo dei dati solo per le macchine che
     // richiedono un crudo, per i vuoti e i cotti lasciamo solo il
     // controllo del tipo box.
     //----------------------------------------------------------------
     if(AGV[i]->mission.PresCEST_B!=LIBERA){
        prov_boxTGV        = AGV[i]->mission.BoxB.ProvBox;
        gruppo_dest_boxTGV = AGV[i]->mission.BoxB.GruppoDest;
     }
     else{
        prov_boxTGV        = AGV[i]->mission.BoxA.ProvBox;
        gruppo_dest_boxTGV = AGV[i]->mission.BoxA.GruppoDest;
     }
     //----------------------------------------------------------------
     // "PIENI" controllo solo che il GRUPPO DI DESTINAZIONE sia
     // quello richiesto
     //----------------------------------------------------------------
     if(gruppo_dest_boxTGV!=gruppo_dest_box) continue;
     //----------------------------------------------------------------
     // Controllo tipo provenienza (se specificato)
     //----------------------------------------------------------------
     if(tipo_prov_box!=NN){
        tipo_prov_tgv=mac_prov_tgv=bai_prov_tgv=mag_prov_tgv=0;
        P.TestProvenienzaBox(prov_boxTGV, &tipo_prov_tgv, &mac_prov_tgv, &bai_prov_tgv, &mag_prov_tgv);
        if(tipo_prov_tgv!=tipo_prov_box) continue;
     }
     //----------------------------------------------------------------
     // TGV trovato!
     //----------------------------------------------------------------
     trovato[i]=true;
  }
  //------------------------------------------------
  // Ricerca del TGV più vicino OVUNQUE
  //
  // NOTA: superfluo per le ricerche tramite IDBOX
  //       ma utile per le ricerche senza id_box
  //       specifico.
  //------------------------------------------------
  new_dist=0;
  old_dist=9999;
  for(i=1; i<=MAXAGV; i++){
     //---------------------------------------------------------
     // verifica AGV senza missione e non escluso
     //---------------------------------------------------------
     if(trovato[i] && (!AGV[i]->test_mission_on()) && (!AGV[i]->test_agv_escluso())){
        //------------------------------------------------------
        // se il TGV ha già la chiamata riservata alla macchina,
        // allora ha precedenza sugli altri.
        //------------------------------------------------------
        if(AGV[i]->mission.Chiamata==chiamata && chiamata!=0){
           *num_agv = i;
           return 0;
        }
        //------------------------------------------------------
        // Se il TGV è in riserva non può prendere nuove
        // chiamate.
        //------------------------------------------------------
        if(!AGV[i]->stato.s.bit.ris){
           //---------------------------------------------
           // trova la distanza pi— breve se pi— di 1 AGV
           //---------------------------------------------
           err = AGV[i]->distanza(FAST_PATH, 0, dest, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
           if(new_dist<MAXPERCORSI && (new_dist < old_dist) && !err){
              old_dist = new_dist;
              *num_agv = i;
           }
        }
     }
  }
  return 0;
}

// --------------------------------------------------------
//     CercaAgvConCaricoVUOTO()
// --------------------------------------------------------
// cerca un AGV libero pi— vicino al "punt_dest". Se lo trova lo
// restituisce in "num_agv", altrimenti restituisce 0.
// In caso di errore ritorna 1.
//
int process::CercaAgvConCaricoVUOTO(bool tutto, short int dest, int chiamata, short int tipo_prov_box, short int tipo_carico, short int tipo_box, short int *num_agv, char *all_mess)
/**************************************************************************/
{
  int err=0;
  short int i, j;
  short int tipo_boxTGV;
  short int prov_boxTGV;
  short int gruppo_baia;
  short int num_baia, num_mac, num_mag, num_stz;
  short int num_rot, new_dist, old_dist;
  short int tipo_prov_tgv, mac_prov_tgv, bai_prov_tgv, mag_prov_tgv;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  bool trovato[MAXAGV+1];
  bool asservimento_ok;

  *num_agv=0;
  old_dist=9999;
  num_rot=new_dist=0;
  memset(&trovato, false, sizeof(trovato));
  memset(&nodi_perc, 0, sizeof(nodi_perc));
  memset(&dati_perc, 0, sizeof(dati_perc));
  //-----------------------------------------------------------------
  // Funzione che gestisce solo la movimentazione vuoti
  //-----------------------------------------------------------------
  if(tipo_carico!=PRES_VUOTO) return 0;
  //-----------------------------------------------------------------
  // Scompongo la chiamata per verificare l'asservimento
  //-----------------------------------------------------------------
  num_baia=num_mag=num_mac=num_stz=0;
  if(chiamata!=0) P.TestChiamataNum(chiamata, &num_mag, &num_baia, &num_mac, &num_stz);
  //-----------------------------------------------------------------
  // Se ci sono le condizioni per liberare i TGV dalle associazione
  // alle zone ripeto i controlli
  //-----------------------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(AGV[i]->test_agv_escluso()                            ) continue;
     if(AGV[i]->mission.SemiAutomatico==true                  ) continue;
     if(AGV[i]->test_mission_on()                             ) continue;
     if((!AGV[i]->stato.s.bit.carA && !AGV[i]->mission.PresCEST_A) &&
        (!AGV[i]->stato.s.bit.carB && !AGV[i]->mission.PresCEST_B)) continue;
     //----------------------------------------------------------------
     // Verifico asservimenti TGV
     //----------------------------------------------------------------
     if(chiamata!=0 && AGV[i]->mission.Chiamata==0){
        asservimento_ok=false;
        // CHIAMATA DA MACCHINA
        if(num_mac>0 && num_mac<=TOTCHIAMATE){
           for(j=1; j<=TOTCHIAMATE; j++){
              if(AGV[i]->mission.AsservMac[j]!=num_mac          ) continue;
              if(AGV[i]->mission.TipoAssMac[j]!=ASS_MAC_TUTTE &&
                 AGV[i]->mission.TipoAssMac[j]!=ASS_MAC_SOLO_ING) continue;
              asservimento_ok=true;
              break;
           }
        }
        // CHIAMATA DA BAIA
        if(num_baia>0 && num_baia<=TOTBAIE){
           gruppo_baia = ba[num_baia]->Gruppo;
           if(gruppo_baia<1           ) continue; // nessun gruppo abbinato alla baia
           if(gruppo_baia>MAXGRUPPIIMP) continue; // nessun gruppo abbinato alla baia
           for(j=0; j<MAXGRUPPIIMP; j++){
              if(AGV[i]->mission.AsservBaie[j]!=gruppo_baia) continue;
              asservimento_ok=true;
              break;
           }
        }
        if(asservimento_ok==false) continue;
     }
     //----------------------------------------------------------------
     // NELLA RICERCA DEI TGV CON CARICO DEVO ESSERCI ABBINAMENO ALLA
     // CHIAMATA
     //----------------------------------------------------------------
     if(chiamata!=0 && AGV[i]->mission.Chiamata!=0 && AGV[i]->mission.Chiamata!=chiamata) continue;
     //----------------------------------------------------------------
     // CERCO TGV COMPLETO
     // Entrambe le forche devono avere il tipo di cestone richiesto
     //----------------------------------------------------------------
     if(tutto==true){
        //--------------------------------------------------
        // (0) GESTIONE CONTROLLO SOLO FORCA "B"
        // (1) GESTIONE FORCA DOPPIA  CON CARICO DOPPIO
        // (2) GESTIONE FORCA SINGOLA CON CARICO DOPPIO
        //--------------------------------------------------
        if(GestioneDoppiaForcaTGV!=1){
           if(AGV[i]->mission.PresCEST_B==LIBERA     ) continue;
           if(AGV[i]->mission.PresCEST_B!=tipo_carico) continue;
           tipo_boxTGV = AGV[i]->mission.BoxB.TipoBox;
           prov_boxTGV = AGV[i]->mission.BoxB.ProvBox;
        }
        else{
           if(AGV[i]->mission.PresCEST_A==LIBERA || AGV[i]->mission.PresCEST_B==LIBERA) continue;
           if(AGV[i]->mission.PresCEST_A!=AGV[i]->mission.PresCEST_B                  ) continue;
           if(AGV[i]->mission.PresCEST_B!=tipo_carico                                 ) continue;
        }
     }
     //----------------------------------------------------------------
     // CERCO TGV INCOMPLETO
     // (non sono ammessi i TGV completi del tipo cassone richiesto)
     // Basta che una delle abbia tipo di cestone richiesto
     // (ovviamente se si tratta della forca "A" la "B" deve essere
     //  libera)
     //----------------------------------------------------------------
     else{
        if(AGV[i]->mission.PresCEST_A==tipo_carico && AGV[i]->mission.PresCEST_B==tipo_carico  ) continue;
        if(AGV[i]->mission.PresCEST_B!=LIBERA && AGV[i]->mission.PresCEST_B!=tipo_carico       ) continue;
        if(AGV[i]->mission.PresCEST_B==LIBERA){
           if(AGV[i]->mission.PresCEST_A!=LIBERA && AGV[i]->mission.PresCEST_A!=tipo_carico    ) continue;
        }
     }
     //----------------------------------------------------------------
     // CONTROLLO DATI
     // è necessario il controllo dei dati solo per le macchine che
     // richiedono un crudo, per i vuoti e i cotti lasciamo solo il
     // controllo del tipo box.
     //----------------------------------------------------------------
     if(AGV[i]->mission.PresCEST_B!=LIBERA){
        tipo_boxTGV = AGV[i]->mission.BoxB.TipoBox;
        prov_boxTGV = AGV[i]->mission.BoxB.ProvBox;
     }
     else{
        tipo_boxTGV = AGV[i]->mission.BoxA.TipoBox;
        prov_boxTGV = AGV[i]->mission.BoxA.ProvBox;
     }
     //----------------------------------------------------------------
     // "VUOTI" controllo solo il tipo di supporto e provenienza
     //----------------------------------------------------------------
     if(tipo_box!=SEARCH_ALL_TIPOBOX && tipo_boxTGV!=tipo_box   ) continue;
     //----------------------------------------------------------------
     // controllo tipo provenienza del box
     //----------------------------------------------------------------
     if(tipo_prov_box!=NN){
        tipo_prov_tgv=mac_prov_tgv=bai_prov_tgv=mag_prov_tgv=0;
        P.TestProvenienzaBox(prov_boxTGV, &tipo_prov_tgv, &mac_prov_tgv, &bai_prov_tgv, &mag_prov_tgv);
        if(tipo_prov_tgv!=tipo_prov_box) continue;
     }
     //----------------------------------------------------------------
     // TGV trovato!
     //----------------------------------------------------------------
     trovato[i]=true;
  }
  //------------------------------------------------
  // Ricerca del TGV più vicino OVUNQUE
  //------------------------------------------------
  new_dist=0;
  old_dist=9999;
  for(i=1; i<=MAXAGV; i++){
     //---------------------------------------------------------
     // verifica AGV senza missione e non escluso
     //---------------------------------------------------------
     if(trovato[i] && (!AGV[i]->test_mission_on()) && (!AGV[i]->test_agv_escluso())){
        //------------------------------------------------------
        // se il TGV ha già la chiamata riservata alla macchina,
        // allora ha precedenza sugli altri.
        //------------------------------------------------------
        if(AGV[i]->mission.Chiamata==chiamata && chiamata!=0){
           *num_agv = i;
           return 0;
        }
        //------------------------------------------------------
        // Se il TGV è in riserva non può prendere nuove
        // chiamate.
        //------------------------------------------------------
        if(!AGV[i]->stato.s.bit.ris){
           //---------------------------------------------
           // trova la distanza pi— breve se pi— di 1 AGV
           //---------------------------------------------
           err = AGV[i]->distanza(FAST_PATH, 0, dest, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
           if(new_dist<MAXPERCORSI && (new_dist < old_dist) && !err){
              old_dist = new_dist;
              *num_agv = i;
           }
        }
     }
  }
  return 0;
}



// -----------------------------
//     PosizioneRiposoAgv()
// -----------------------------
// Verifica che non ci siano missioni ON per l'AGV. Se non esistono
// forza il parcheggio dell'AGV
//
int process::PosizioneRiposoAgv(short int NumAgv, char *all_mess)
/*************************************************************/
{
  int err=0;
  bool Ok=false;
  bool AttesaON;
  bool AbilitaAttesa;
  bool tgv_fuori_ingombro;
  short int AltroAgv;
  short int i, j, OkProg;
  short int old_dist, new_dist, num_rot;
  short int nodi_perc[MAXPERCORSI];
  short int Pos, PosAttesa, PosRicarica;
  short int Dest, PInt;
  long  int DiffTempo;
  long  int TimeAttuale;
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  short int PosizioniRicaricaFlottante[MAXAGV+2];
//static int AttesaCiclo[MAXAGV+1];

  //------------------------------------------------------------------
  // IMPIANTI SPECIALI
  // Se il TGV non ha il bit di riserva cerca come posizione di
  // riposo il posizionamento in quota in un magazzino di conformi
  // o libero
  //------------------------------------------------------------------
  if(!PosizioneRiposo_IMPIANTI(NumAgv, all_mess)    ) return 0;
  //------------------------------------------------------------------
  // No gestione riposo SPECIALE allora faccio la gestione STANDARD
  //------------------------------------------------------------------
  err=0;
  AltroAgv=0;
  OkProg = (short int) AGV[NumAgv]->stato.s.bit.okprog;                 // AGV pronto aricevere nuovo prg.
  if(!OkProg) return 0;
  if(!AGV[NumAgv]->stato.s.bit.funz                ) return 0;          // Test se AGV in Automatico
  if(AGV[NumAgv]->test_mission_on()                ) return 0;
  if(AGV[NumAgv]->mission.SemiAutomatico           ) return 0;          // Test se AGV in Semiautomatico
  if(AGV[NumAgv]->stato.start                      ) return 0;
  if(AGV[NumAgv]->mission.Esclusione               ) return 0;          // Test se escluso
  if(AGV[NumAgv]->stato.pos==0                     ) return 0;
  if(AGV[NumAgv]->mission.Chiamata                 ) return 0;
  if(AGV[NumAgv]->mission.PresCEST_A!=LIBERA       ) return 0;
  if(AGV[NumAgv]->mission.PresCEST_B!=LIBERA       ) return 0;
  if(AGV[NumAgv]->stato.s.bit.carA==true           ) return 0;
  if(AGV[NumAgv]->stato.s.bit.carB==true           ) return 0;
  if(AGV[NumAgv]->mission.NoRiposoAuto             ) return 0;
  //------------------------------------------------------------------
  // Recupera la posizione del TGV e la sua
  // prevista posizione di riposo.
  //------------------------------------------------------------------
  Pos         = AGV[NumAgv]->stato.pos;
  PosRicarica = AGV[NumAgv]->mission.PosRicarica;
  PosAttesa   = AGV[NumAgv]->mission.PosAttesa;
  //------------------------------------------------------------------
  // TGV già in ricarica
  //------------------------------------------------------------------
  if(AGV[NumAgv]->stato.pos>0 && AGV[NumAgv]->stato.pos<21){
     // 17/03/2014 Se TGV NON è nella sua posizione di ricarica --> Sposta
     //return 0;
     if(AbilitaRiposoFlottante==true        ) return 0;
     if(AGV[NumAgv]->stato.pos==PosRicarica ) return 0;
  }
  //------------------------------------------------------------------
  // Memorizzo le posizioni di ricarica assegnate a ogni TGV
  //------------------------------------------------------------------
  memset(&PosizioniRicaricaFlottante[0], 0, sizeof(PosizioniRicaricaFlottante));
  for(i=1; i<=MAXAGV; i++) PosizioniRicaricaFlottante[i]=AGV[i]->mission.PosRicarica;
  //------------------------------------------------------------------
  // Memorizzo il percorso standard fino alla POSIZIONE DI RICARICA
  //------------------------------------------------------------------
  new_dist=num_rot=0;
  memset(&nodi_perc, 0, sizeof(nodi_perc));
  memset(&dati_perc, 0, sizeof(dati_perc));
  err = AGV[NumAgv]->distanza(STD_PATH, 0, PosRicarica, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
  if(new_dist>MAXPERCORSI || err) return 0;

  //------------------------------------------------------------------
  // Se il TGV è già in posizione fuori ingombro --> skip!
  //------------------------------------------------------------------
  tgv_fuori_ingombro=false;
  for(i=0; i<MAXNODIATTESA; i++){
     if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]<1         ) continue;
     if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]>MAXPUNTI  ) continue;
     if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]!=Pos      ) continue;
     tgv_fuori_ingombro=true;
     break;
  }

  //-------------------------------------------
  // 27/11/2012 Gestione del Riposo Flottante
  //-------------------------------------------
  if(AbilitaRiposoFlottante==true){
     //----------------------------------------------------------
     // Scatena missione ( AGV -> riposo ) nel punto di ricarica
     // più vicino alla posizione attuale dell'AGV.
     //----------------------------------------------------------
     num_rot =0;
     new_dist=0;
     old_dist=9999;
     //Ricarica;
     for(i=1; i<=MAXAGV; i++){
        if(PosizioniRicaricaFlottante[i]<0        ) continue;
        if(PosizioniRicaricaFlottante[i]>MAXPUNTI ) continue;
        //-------------------------------------
        // TGV già in ricarica --> skip!
        //-------------------------------------
        if(AGV[NumAgv]->stato.pos==PosizioniRicaricaFlottante[i]) break;
        //-------------------------------------
        // Nodo già occupato --> skip!
        //-------------------------------------
        if(test_dest_riservata (PosizioniRicaricaFlottante[i], &AltroAgv, NumAgv)) continue;
        err= test_dest_occupata(PosizioniRicaricaFlottante[i], &AltroAgv);
        if((err!=0) && (NumAgv!=AltroAgv)) continue;
        err = AGV[NumAgv]->distanza(STD_PATH, 0, PosizioniRicaricaFlottante[i], nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
        //-------------------------------------
        // Assegno come posizione di ricarica
        // la più vicina tra quelle papabili
        //-------------------------------------
        if(new_dist<MAXPERCORSI && (new_dist<old_dist) && !err && new_dist>0){
           old_dist=new_dist;
           PosRicarica=PosizioniRicaricaFlottante[i];
        }
     }
  }

  //------------------------------------------------------------------
  // SICUREZZA
  // Attesa minima 2 cicli di scansione prima di mandare un
  // TGV a riposo
  //------------------------------------------------------------------
/*
  if(AttesaCiclo[NumAgv]<2){
     AttesaCiclo[NumAgv]++;
     return 0;
  }
  AttesaCiclo[NumAgv]=0;
*/
  //------------------------------------------------------------------
  // Se il TGV si trova su un punto terminale viene inizializzato
  // un tempo di attesa prima del ritorno a riposo
  //------------------------------------------------------------------
  AttesaON=false;
  TimeAttuale=time(NULL);
  if(tgv_fuori_ingombro==true || (PosAttesa!=0 && Pos==PosAttesa)){
     if(AGV[NumAgv]->mission.TimeInitAttesa==0){
        AGV[NumAgv]->mission.TimeInitAttesa=TimeAttuale;
        return 0;
     }
     else{
        DiffTempo = TimeAttuale - AGV[NumAgv]->mission.TimeInitAttesa;
        if(TempoPerMissioneRicarica>(DiffTempo/60)) AttesaON=true;
     }
     //--------------------------------------------------------------
     // Se il TGV passa in riserva interrompo l'attesa
     //--------------------------------------------------------------
     if(AGV[NumAgv]->stato.s.bit.ris) AttesaON=false;
  }
  //--------------------------------------------------------------------
  // Se è attiva l'attesa --> skip!
  //--------------------------------------------------------------------
  if(AttesaON==true) return 0;
  //--------------------------------------------------------------------
  // Recuper destinazione missione
  //--------------------------------------------------------------------
  PInt = 0;
  Dest = PosRicarica;
  //------------------------------------------------------------------
  // GESTIONE NODO DI ATTESA SOLO SU PERCORSO VERSO RICARICA
  // Verifico se il nodo di attesa si trova sul tragitto per il
  // ritorno in ricarica
  //------------------------------------------------------------------
  AbilitaAttesa=true;
  if(NodoDiAttesaVersoRicarica==true){
     //------------------------------------------------------------
     // Verifico se il nodo impostato come attesa  compare nel
     // percorso fino alla ricarica. Escludo dalla ricerca i
     // nodi di start e di end
     //------------------------------------------------------------
     AbilitaAttesa=false;
     for(i=1; i<MAXPERCORSI; i++){
        if(nodi_perc[i+1]==0       ) break;
        if(nodi_perc[i]!=PosAttesa ) continue;
        AbilitaAttesa=true;
        break;
     }
  }
  //--------------------------------------------------------------------
  // Se è stato impostato un nodo di attesa
  //--------------------------------------------------------------------
  if(PosAttesa>0 && PosAttesa<=MAXPUNTI && Pos!=PosAttesa && !AGV[NumAgv]->stato.s.bit.ris && AbilitaAttesa) Dest = PosAttesa;
  //--------------------------------------------------------
  // Se la destinazione è la ricarica inizializzo
  // una prima missione FUORI INGOMBRO
  //--------------------------------------------------------
  if(Dest==PosRicarica && !AGV[NumAgv]->stato.s.bit.ris && tgv_fuori_ingombro==false && PosSuFuoriIngombroPreRicarica==true){
     //-------------------------------------------------------------------------
     // Scorro il percorso a ritroso fino a incontrare un PUNTO FUORI INGOMBRO
     // (non a inizio missione)
     //-------------------------------------------------------------------------
     for(i=MAXPERCORSI-1; i>2; i--){
        if(nodi_perc[i]==0) continue;
        //------------------------------------------------------------------
        // NO PINT SU INIZIO DEVIAZIONE
        // Scarto tutti i punti su posizione di rotazione in ingresso
        // uscita curva
        //------------------------------------------------------------------
        if(NoPIntSuStartDeviazione==true && (dati_perc[i].rot>3 || dati_perc[i-1].rot>3)) continue;
        //------------------------------------------------------------------
        // Se il TGV è già in posizione fuori ingombro --> skip!
        //------------------------------------------------------------------
        for(j=0; j<MAXNODIATTESA; j++){
           if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]<1              ) continue;
           if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]>MAXPUNTI       ) continue;
           if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]!=nodi_perc[i]  ) continue;
           PInt=nodi_perc[i];
           break;
        }
        if(PInt!=0) break;
     }
  }
  //--------------------------------------------------------
  // TGV già su posizione di attesa calcolata
  //--------------------------------------------------------
  if(Pos==Dest) return 0;
  //--------------------------------------------------------
  // Ogni TGV ha come destinazione una ricarica che ha come
  // nodo il numero del TGV stesso.
  //--------------------------------------------------------
  if(N.punti_notevoli[Dest]==true){
     if(test_dest_riservata(Dest, &AltroAgv, NumAgv) && AltroAgv!=0 && AltroAgv!=NumAgv){
        //---------------------------------------------------
        // Se la destinazione è la ricarica ed è occupata
        // alzo l'allarme interno
        //---------------------------------------------------
        if(Dest==PosRicarica) AGV[NumAgv]->allarme_interno = ALL_INT_RICARICA_OCC;
        return 0;
     }
     if(test_dest_occupata(Dest, &AltroAgv) && AltroAgv!=0 && AltroAgv!=NumAgv){
        //---------------------------------------------------
        // Se la destinazione è la ricarica ed è occupata
        // alzo l'allarme interno
        //---------------------------------------------------
        if(Dest==PosRicarica) AGV[NumAgv]->allarme_interno = ALL_INT_RICARICA_OCC;
        return 0;
     }
  }        
  //--------------------------------------------------------
  // Inizializzazione missione
  //--------------------------------------------------------
  if(new_init_mission(FALSE, CALCOLA_TIPO_PATH, NumAgv, 0, Dest, 0, PInt, MISS_SOLO_POSIZIONAMENTO, 0, &Ok, all_mess)) return 1;

  return err;
}

// ----------------------------------
//   FuoriInGombroPortePerIncendio()
// ----------------------------------
// Gestione missioni fuori ingombro porte
//
int process::FuoriInGombroPortePerIncendio(short int NumAgv, char *all_mess)
/*************************************************************/
{
  int err;
  bool Ok;
  int chiamata;
  short int pos;
  short int i, dest, nodo;
  short int old_dist, new_dist, num_rot;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];

  err     = 0;
  dest    = 0;
  Ok      = false;
  pos     = AGV[NumAgv]->stato.pos;
  chiamata= AGV[NumAgv]->mission.Chiamata;
  //------------------------------------------------------------------
  // Sposto in Ingombro alle porte
  //------------------------------------------------------------------
  if(IncendioInCorso==false                    ) return 0;
  if(N.nodi_in_ingombro_per_porte[pos]==false  ) return 0;
  if(!AGV[NumAgv]->stato.s.bit.funz            ) return 0;          // Test se AGV in Automatico
  if(!AGV[NumAgv]->stato.s.bit.okprog          ) return 0;          // Test se AGV in Automatico
  if(AGV[NumAgv]->test_mission_on()            ) return 0;
//if(AGV[NumAgv]->mission.SemiAutomatico       ) return 0;          // Test se AGV in Semiautomatico
  if(AGV[NumAgv]->stato.start                  ) return 0;
  if(AGV[NumAgv]->mission.Esclusione           ) return 0;          // Test se escluso
  //------------------------------------------------------------------
  // Cerco il nodo fuori ingombro più vicino
  //------------------------------------------------------------------
  new_dist=0;
  old_dist=9999;
  for(i=0; i<MAXNODIATTESA; i++){
     if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]<1         ) continue;
     if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]>MAXPUNTI  ) continue;
     nodo = N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i];
     //---------------------------------------------
     // Nodo trovato anch'esso in ingobro porta
     //---------------------------------------------
     if(N.nodi_in_ingombro_per_porte[nodo]==true) continue;
     //---------------------------------------------
     // Scarto il nodo sul quale ho già il TGV
     //---------------------------------------------
     if(AGV[NumAgv]->stato.pos==nodo) continue;
     memset(&nodi_perc, 0, sizeof(nodi_perc));
     memset(&dati_perc, 0, sizeof(dati_perc));
     err = AGV[NumAgv]->distanza(STD_PATH, 0, nodo, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
     if(new_dist<MAXPERCORSI && new_dist<old_dist && !err){
        dest = nodo;
        old_dist = new_dist;
     }
  }
  //--------------------------------------------------------
  // Nessuna destinazione trovata
  //--------------------------------------------------------
  if(dest==0) return 0;
  //--------------------------------------------------------
  // Inizializzazione missione
  //--------------------------------------------------------
  if(new_init_mission(TRUE, CALCOLA_TIPO_PATH, NumAgv, chiamata, dest, 0, 0, MISS_SOLO_POSIZIONAMENTO, 0, &Ok, all_mess)) return 1;
  return 0;
}







