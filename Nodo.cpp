// ------------------------------------------------------------------------
//             file       NODO.CPP
// ------------------------------------------------------------------------
//
// nodo()                            Costruttore
// void nod_char_to_int()            Converti in stringhe la struttura NOD
// void nod_int_to_char()            Converti la struttura NOD in stringhe
// int  free_nodi()                  Libera tutti i nodi occupati
// int  load_nodo_busy()             Legge da file la situazione nodi occupati
// int  save_nodo_busy()             Salva su file l'array di "int" di nodi occupati
// int  compila_punti_notevoli()     Compila la matrice di punti notevoli (terminali)
// int  compila_nodo_circostante()   Compila la matrice di nodi circostanti
// int  load_file_nodi()             Legge il file dei nodi
// int  save_record_nodi()           Salva un record nel file dei nodi
// int  save_file_nodi()             Salva l'intero file dei nodi
// long num_record()                 Restituisce il numero di records
// int  cerca_record()               Cerca un record e aggiona la struct nod
// int  print_punto()                Stampa l'elenco nodi e le zone di appartenenza
// int  stamp_elenco_nod()           Stampa elenco nodi e relativi dati
//
#include <stdio.h>
#include <mem.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <math.h>

#include <fcntl.h>
#include <errno.h>
#include <share.h>
#include <sys\stat.h>


#include "tecnofer.h"

extern float Rap;
extern int   offset[][2];
//  ---------------------
//   STRUTTURA NODI
//  ---------------------
//  0,1 = x,y posizione centrale nodo
//  2 = numero nodo
//  3 = occupazione agv ( numero AGV oppure libero = 0 )
//-------------------------------------------------------
extern int Punti[][5];

//  ---------------------
//   STRUTTURE GRAFICHE
//  ---------------------
//-------------------------------------------------------
extern int Percorso[][2];
extern int PercorsoCurva[][5];
extern int PercorsoCurvaBezier[][7];
extern int GrafScaffali[][16];

//-------------------------------------------------------
// Funzioni di occupazione nodi
//-------------------------------------------------------
extern int VerificaNodoOccupato(bool RioccupaNodo, int NumAgv, int NodoConsiderato,
                                int & NodoOccupabile, int & AgvAdiacente, int & satellite_occupato, int & NodoInterruzionePercorso, int ScartaAgv, int SoloAgv );

// ---------------------------
//       nodo()
// ---------------------------
//  Questa funzione ha lo scopo di creare la classe nodo ( avendo lo stesso
//  nome ). Tale funzione pu• non esistere, e comunque se esiste Š automati-
//  camente richiamata nella creazione (new). E' usata normalmente per
//  inzializzare variabili globali e utili alla classe.
//
nodo::nodo()
/**********/
{
  int i;

  //--------------------------------------------------------------
  // inizializzazioni strutture
  // - NODI OCCUPATI
  // - GRADI DI ROTAZIONE DELLE NAVETTE SUI NODI
  // - NODI PER GESTIONE PRECEDENZE PER TGV IN RETTILINEO
  //--------------------------------------------------------------
  memset(&nodo_grd[0],              0, sizeof(nodo_grd));
  memset(&nodo_busy[0],             0, sizeof(nodo_busy));
  memset(&nodo_con_tgv_lanciato[0], 0, sizeof(nodo_con_tgv_lanciato));
    //--------------------------------------------------------------
  // GESTIONE CURVE
  // Raggio DWG e raggio PC inizializzati uguali
  //--------------------------------------------------------------
  memset(&raggi_curve_laser[0], 0, sizeof(raggi_curve_laser));
  for(i=0; i<=MAXPUNTI; i++){
     if( PercorsoCurva[i][0]==0 && PercorsoCurva[i][1]==0 && PercorsoCurva[i][3]==0) break;
     raggi_curve_laser[i][0] = (short int)(PercorsoCurva[i][0]);  //P1
     raggi_curve_laser[i][1] = (short int)(PercorsoCurva[i][1]);  //P2
     raggi_curve_laser[i][2] = (short int)(PercorsoCurva[i][3]);  //Raggio da DWG
     raggi_curve_laser[i][3] = (short int)(PercorsoCurva[i][3]);  //Raggio calcolato da PC
     raggi_curve_laser[i][4] = 1;                                 //Curva con ing/out tangente
  }
  //--------------------------------------------------------------
  // Compilazione nomi file
  //--------------------------------------------------------------
  strcpy(file_name_sorg, "NODO.DAT");
  strcpy(file_name_dest, "NODO.DAT");

  strcpy(FileNameNodoBusySorg, "NODOBUSY.DAT");
  strcpy(FileNameNodoBusyDest, "NODOBUSY.DAT");

  strcpy( FileNameNodoGrdSorg, "NODOGRD.DAT");
  strcpy( FileNameNodoGrdDest, "NODOGRD.DAT");
}


// ---------------------------------------------------
//      free_nodi()
//  Libera tutti i nodi occupati in precedenza dagli
//  AGV   ritorna 1 in caso di errore.
// ---------------------------------------------------
// Libera tutti i Nodi occupati dagli AGV. Torna 1 in caso di errore.
//
int nodo::free_nodi(char *all_mess)
/*********************************/
{
  //Modifica del 18/01/94
  memset(&nodo_busy, 0, sizeof(nodo_busy));
  if(save_nodo_busy(all_mess)) return 1;
  return 0;
}/*** free_nodi() ***/



// ------------------------
//    load_nodo_busy()
// ------------------------
//   load file nodi occupati
//
int nodo::load_nodo_busy(char *all_mess)
/***********************************/
{
  int  i,
       err =0;
  int  handle;

  memset(&nodo_busy , 0, sizeof(nodo_busy));
  handle = sopen( FileNameNodoBusySorg, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
    //close( handle );
      sprintf(all_mess, MESS[112], FileNameNodoBusySorg);
      return 1;
  }
  i = read( handle, &nodo_busy[0], sizeof( nodo_busy ) );
  close( handle );

  if (i != sizeof(nodo_busy))  err = 1;
  if(err) sprintf(all_mess, MESS[112], FileNameNodoBusySorg);

  return err;
}


// ------------------------
//     save_nodo_busy()
// ------------------------
// Salva sul file NODOBUSY.DAT i nodi occupati da AGV
//
int nodo::save_nodo_busy(char *all_mess)
/***********************************/
{
  int  i, err = 0;
  int  handle;

  handle = sopen( FileNameNodoBusyDest, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
  if( handle<0 ){
     sprintf(all_mess, MESS[113], FileNameNodoBusyDest);
     //close( handle );
     return 1;
  }
  i =  write( handle, &nodo_busy[0], sizeof( nodo_busy ) );
  close( handle );

  if (i != sizeof(nodo_busy))  err = 1;
  if(err) sprintf(all_mess, MESS[113], FileNameNodoBusyDest);
  return err;
}

// ------------------------
//    load_nodo_grd()
// ------------------------
//   load file NODOGRD.DAT con i gradi di rotazione sui nodi
//
int nodo::load_nodo_grd(char *all_mess)
/***********************************/
{
  int  i,
       err =0;
  int  handle;

  memset(&nodo_grd , 0, sizeof(nodo_grd));
  handle = sopen( FileNameNodoGrdSorg, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
    //close( handle );
      sprintf(all_mess, MESS[112], FileNameNodoGrdSorg);
      return 1;
  }
  i = read( handle, &nodo_grd, sizeof( nodo_grd ) );
  close( handle );

  if (i != sizeof(nodo_grd))  err = 1;
  if(err) sprintf(all_mess, MESS[112], FileNameNodoGrdSorg);

  return err;
}


// ------------------------
//     save_nodo_grd()
// ------------------------
// Salva sul file NODOGRD.DAT i nodi occupati da AGV
//
int nodo::save_nodo_grd(char *all_mess)
/***********************************/
{
  int  i, err = 0;
  int  handle;

  handle = sopen( FileNameNodoGrdDest, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
  if( handle<0 ){
     sprintf(all_mess, MESS[113], FileNameNodoGrdDest);
     //close( handle );
     return 1;
  }
  i =  write( handle, &nodo_grd, sizeof( nodo_grd ) );
  if (i != sizeof(nodo_grd))  err = 1;
  if(err) sprintf(all_mess, MESS[112], FileNameNodoGrdDest);

  close( handle );

  return err;
}

// ------------------------------
//    compila_punti_notevoli()
// ------------------------------
// Compila un array di punti_notevoli[MAXPUNTI+1] con un valore 1
// se il punto apprtiene a 1)Parcheggio 2)Macchina 3)Magazzino
// N.B. da eseguire dopo la lettura del file MACCHINE.
//
int nodo::compila_punti_notevoli(char *all_mess)
/**********************************************/
{
  int i, n_rec, err, punto;
  struct MAG mg;
  struct BAIE bb;
  struct CHIAMATE mc;

  strcpy( all_mess, "OK");
  memset(punti_notevoli, false, sizeof(punti_notevoli));
  //---------------------------------
  // Posizioni di ricarica
  //---------------------------------
  for(i=1; i<51; i++){
     if(Punti[i][4]==0) continue;
     punti_notevoli[i] = true;
  }
  //---------------------------------
  // Punti appartenenti a macchine
  //---------------------------------
  // --- MACCHINE
  for( n_rec=1; n_rec<=MAXCHIAMATE; n_rec++){
     if( P.ch[ n_rec ]->Num >0 ){
        memset(&mc, 0, sizeof(CHIAMATE));
        err = P.cerca_record_chiamate(&mc, n_rec );
        if( err==0 ){
           for(i=0; i<MAXSTAZMAC; i++){
              if(mc.Staz[i].PuntoIng>0 && mc.Staz[i].PuntoIng<=TOTPUNTI) punti_notevoli[mc.Staz[i].PuntoIng] = true;
              if(mc.Staz[i].PuntoOut>0 && mc.Staz[i].PuntoOut<=TOTPUNTI) punti_notevoli[mc.Staz[i].PuntoOut] = true;
           }
        }
     }
  }
  // --- BAIE
  for( n_rec=1; n_rec<=MAXBAIE; n_rec++){
     if( P.ba[ n_rec ]->Num >0 ){
        memset(&bb, 0, sizeof(BAIE));
        err = P.cerca_record_baie(&bb, n_rec );
        if( err==0 ){
           if(bb.Punto>0 && bb.Punto<=TOTPUNTI) punti_notevoli[bb.Punto] = true;
           for(i=0; i<5; i++){
              if(bb.PuntoAux[i]>0 && bb.PuntoAux[i]<=TOTPUNTI) punti_notevoli[bb.PuntoAux[i]] = true;
           }
        }
     }
  }
  // --- MAGAZZINI
  for( n_rec=1; n_rec<=MAXMAG; n_rec++){
     if( M.m[ n_rec ]->Num >0 ){
        memset(&mg, 0, sizeof(MAG));
        err = M.cerca_record_mag(&mg, n_rec );
        if( err==0 ){
           if(mg.PuntoIng>0 && mg.PuntoIng<=TOTPUNTI) punti_notevoli[mg.PuntoIng] = true;
           if(mg.PuntoOut>0 && mg.PuntoOut<=TOTPUNTI) punti_notevoli[mg.PuntoOut] = true;
           //---------------------------------------------
           // MAGAZZINI MULTI NODO
           // tutti i nodi del magazzino sono da
           // considerarsi punti notevoli
           //---------------------------------------------
           for(i=1; i<=MAXTERRAMAG; i++){
              if(mg.Pos[i].NodoIng<1        ) continue;
              if(mg.Pos[i].NodoIng>TOTPUNTI ) continue;
              punti_notevoli[mg.Pos[i].NodoIng] = true;
           }
           for(i=1; i<=MAXTERRAMAG; i++){
              if(mg.Pos[i].NodoOut<1        ) continue;
              if(mg.Pos[i].NodoOut>TOTPUNTI ) continue;
              punti_notevoli[mg.Pos[i].NodoOut] = true;
           }
        }
     }
  }
  return 0;
}


// ----------------------------
//    compila_nodo_circostante
// ----------------------------
// Dal file Nodo, compila la matrice nodo_circostante , dove:
//      nodo_circostante[num.nodo][0] = Nodo contiguo in direzione AVANTI-0ø
//      nodo_circostante[num.nodo][1] = Nodo contiguo in direzione INDIETRO-0ø
//      nodo_circostante[num.nodo][2] = Nodo contiguo in direzione AVANTI-90ø
//      nodo_circostante[num.nodo][3] = Nodo contiguo in direzione indietro-0ø
//
int nodo::compila_nodo_circostante(char *all_mess)
/**********************************/
{
  int n_rec;
//int pt;
  int av, ind, rot, ps, i, dist;
  float MINDISTANZA;          // minima distanza per fuori ingombro tra AGVs
        MINDISTANZA = 9999;   // 3200 mm.
  struct NOD nod;

  //---------------------------------------------------------
  // Provvisorio per verifica eliminato
  //---------------------------------------------------------
  FILE * ptrfile;
  ptrfile = fopen("CIRCOST.DAT", "wb");
  if(ptrfile==0) {
     sprintf(all_mess, MESS[113],"CIRCOST.DAT" );
     return 1;
  }

  memset(&nod, 0, sizeof(NOD));
  memset(&nodo_circostante, 0, sizeof(nodo_circostante));
  //---------------------------------------------------------------------
  // Valuta se ci sono punti circostanti al nodo in questione nelle 4
  // direzioni polari, questi nodi se occupati da un primo AGV non
  // potranno essere occupati da un secondo perche avremmo il rischio di
  // una collisione .
  // N.B. Non vengono considerati i punti contigui che sono punti terminali
  // appartenenti a macchine, magazzini o parcheggi perchŠ di solito
  // fuori ingombro dai normali percorsi.
  // N.B. Nella compilazione della struttura di appoggio "nodo_circostante_appoggio"
  // vengono considerati anche i punti cotigui che sono punti terminali appartenenti
  // a macchine o magazzini o parcheggi, perchŠ questa struttura Š utilizzata per la
  // ricerca di un PERCORSO ALTERNATIVO.
  //---------------------------------------------------------------------
  for( n_rec=0; n_rec<=MAXPUNTI; n_rec++){
     memcpy( &nod, n[ n_rec ], sizeof( NOD) );
     if( nod.num ==0 ) continue;
     for( i=0;i<MAXPUNTINODO;i++){
        av  = nod.rec[i].av;      // Direzione avanti
        ind = nod.rec[i].ind;     // Direzione indietro
        rot = nod.rec[i].rot;     // 0==0ø 1==90ø
        ps  = nod.rec[i].ps;      // Punto sucessivo
      //pt  = punti_notevoli[ps]; // Punto sucessivo appartenente a
                                  // macchina magazzino o parcheggio
        dist= nod.rec[i].dist;    // distanza al prossimo nodo
        //---------------------------------------------------------
        // Cerca il punto circostante in direzione AVANTI e a 0ø
        //---------------------------------------------------------
        if(av && ( dist<MINDISTANZA ) && (rot == 0) && ps && (nod.num <= MAXPUNTI)){
           nodo_circostante[nod.num][0] = (short int) ps;
        }
        //---------------------------------------------------------
        // Cerca il punto circostante in direzione INDIETRO e a 0ø
        //---------------------------------------------------------
        if(ind && ( dist<MINDISTANZA ) && (rot == 0) && ps && (nod.num <= MAXPUNTI)){
           nodo_circostante[nod.num][1] = (short int) ps;
        }
        //---------------------------------------------------------
        // Cerca il punto circostante in direzione AVANTI e a 90ø
        //---------------------------------------------------------
        if( av && ( dist<MINDISTANZA ) && (rot == 1) && ps && (nod.num <= MAXPUNTI)){
           nodo_circostante[nod.num][2] = (short int) ps;
        }
        //---------------------------------------------------------
        // Cerca il punto circostante in direzione INDIETRO e a 90ø
        //---------------------------------------------------------
        if(ind && ( dist<MINDISTANZA ) && (rot == 1) && ps && (nod.num <= MAXPUNTI)){
           nodo_circostante[nod.num][3] = (short int) ps;
        }

        //----------------------------------------------------------------------
        // 22/07/2004 Cerca il punto circostante in direzione INDIETRO e a 180°
        //----------------------------------------------------------------------
        if(ind && ( dist<MINDISTANZA ) && (rot == 2) && ps && (nod.num <= MAXPUNTI)){
           nodo_circostante[nod.num][0] = (short int) ps;
        }
        //----------------------------------------------------------------------
        // 22/07/2004 Cerca il punto circostante in direzione AVANTI e a 180°
        //----------------------------------------------------------------------
        if(av && ( dist<MINDISTANZA ) && (rot == 2) && ps && (nod.num <= MAXPUNTI)){
           nodo_circostante[nod.num][1] = (short int) ps;
        }
        //----------------------------------------------------------------------
        // 22/07/2004 Cerca il punto circostante in direzione INDIETRO e a 270°
        //----------------------------------------------------------------------
        if( ind && ( dist<MINDISTANZA ) && (rot == 3) && ps && (nod.num <= MAXPUNTI)){
           nodo_circostante[nod.num][2] = (short int) ps;
        }
        //----------------------------------------------------------------------
        // 22/07/2004 Cerca il punto circostante in direzione AVANTI e a 270°
        //----------------------------------------------------------------------
        if(av && ( dist<MINDISTANZA ) && (rot == 3) && ps && (nod.num <= MAXPUNTI)){
           nodo_circostante[nod.num][3] = (short int) ps;
        }
     }

     fprintf(ptrfile, "N: %d = %d %d %d %d \n", nod.num,
                                                nodo_circostante[nod.num][0],
                                                nodo_circostante[nod.num][1],
                                                nodo_circostante[nod.num][2],
                                                nodo_circostante[nod.num][3]);
    }
    fclose(ptrfile);
    return 0;
}

// -------------------------------------
//    compila_gestione_blocchi_manuali
// -------------------------------------
// Compilazione della struttura dei blocchi manuali alla gestione STANDARD
// dei percorsi TGV.
//
int nodo::compila_blocchi_manuali_percorso(char *all_mess)
/**********************************/
{
  int i, stop, err;
  DBSeq *CSVTable = new DBSeq;

  i=stop=err=0;
  //-----------------------------------------------------------
  // Azzermento della struttura globale
  //-----------------------------------------------------------
  memset(&blocco_man, 0, sizeof(blocco_man));
  //-----------------------------------------------------------
  // CONVERSIONE DEL VECCHIO FORMATO "GestioneBlocchiManuali"
  // NELLA NUOVA VERSIONE "GestioneBlocchiManuali1"
  //
  // Inizializzazione della tabella CSV
  //-----------------------------------------------------------
  strcpy(CSVTable->NomeDBase,      "GestioneBlocchiManuali");
  strcpy(CSVTable->SottoDirectory, "");
  CSVTable->CarattereSeparatore    = ';';
  CSVTable->TipoFile               = DBSEQ_TIPO_FILE_STANDARD;
  //-----------------------------------------------------------
  // Apertura della tabella CSV
  //-----------------------------------------------------------
  err = CSVTable->Open( all_mess );         // APRI TABELLA
  if(err==0){
     //-----------------------------------------------------------
     // Posizionamento sul primo record
     //-----------------------------------------------------------
     err=CSVTable->ReadFirstRecord(all_mess);
     //-----------------------------------------------------------
     // Salto la riga dell'intestazione
     //-----------------------------------------------------------
     if(err==0) err=CSVTable->ReadNextRecord(all_mess);
     //-----------------------------------------------------------
     // Recupero dati in sequenza
     //-----------------------------------------------------------
     if(err==0){
        do{
           //-----------------------------------------------------------
           // Compilazione della struttura dati
           //-----------------------------------------------------------
           stop=CSVTable->GetTuttiCampiRecord( 11, all_mess );
           blocco_man[i].pintend = (short int)(atoi(CSVTable->StrCampi[ 1]));
           blocco_man[i].succ1   = (short int)(atoi(CSVTable->StrCampi[ 2]));
           blocco_man[i].succ2   = (short int)(atoi(CSVTable->StrCampi[ 3]));
           blocco_man[i].occu1   = (short int)(atoi(CSVTable->StrCampi[ 4]));
           blocco_man[i].occu2   = (short int)(atoi(CSVTable->StrCampi[ 5]));
           blocco_man[i].pren1   = (short int)(atoi(CSVTable->StrCampi[ 6]));
           blocco_man[i].pren2   = (short int)(atoi(CSVTable->StrCampi[ 7]));
           blocco_man[i].aux1    = (short int)(atoi(CSVTable->StrCampi[ 8]));
           blocco_man[i].aux2    = (short int)(atoi(CSVTable->StrCampi[ 9]));
           blocco_man[i].aux3    = (short int)(atoi(CSVTable->StrCampi[10]));
           blocco_man[i].aux4    = (short int)(atoi(CSVTable->StrCampi[11]));
           i++;
           stop=CSVTable->ReadNextRecord(all_mess);
        }while(!stop && i<100);
     }
     //-----------------------------------------------------------
     // Chiusura della tabella CSV
     //-----------------------------------------------------------
     CSVTable->Close();         // CHIUDI TABELLA
     unlink("GestioneBlocchiManuali.CSV");                       // cancellazione del file perchè OBSOLETO
     //-----------------------------------------------------------
     // Parametrizzazione della nuova tabella CSV
     //-----------------------------------------------------------
     i=0;
     unlink("GestioneBlocchiManuali1.CSV");                      // cancellazione del file prima di RICREARLO
     strcpy(CSVTable->NomeDBase,      "GestioneBlocchiManuali1");
     strcpy(CSVTable->SottoDirectory, "");
     CSVTable->CarattereSeparatore    = ';';
     CSVTable->TipoFile               = DBSEQ_TIPO_FILE_STANDARD;
     if(err==0) err = CSVTable->OpenAppend( all_mess );          // APRI TABELLA  (se non esiste la crea)
     if(err==0){
        do{
           //-----------------------------------------------------------
           // Scrittura intestazione
           //-----------------------------------------------------------
           if(i==0){
              strcpy(CSVTable->StrCampi[ 1], "PIntermedio");
              strcpy(CSVTable->StrCampi[ 2], "Successivo1");
              strcpy(CSVTable->StrCampi[ 3], "Successivo2");
              strcpy(CSVTable->StrCampi[ 4], "Successivo3");
              strcpy(CSVTable->StrCampi[ 5], "Successivo4");
              strcpy(CSVTable->StrCampi[ 6], "Aux1");
              strcpy(CSVTable->StrCampi[ 7], "Aux2");
              strcpy(CSVTable->StrCampi[ 8], "Aux3");
              strcpy(CSVTable->StrCampi[ 9], "Aux4");
              strcpy(CSVTable->StrCampi[10], "Aux5");
              strcpy(CSVTable->StrCampi[11], "Aux6");
              strcpy(CSVTable->StrCampi[12], "Aux7");
              strcpy(CSVTable->StrCampi[13], "Aux8");
              strcpy(CSVTable->StrCampi[14], "Aux9");
              strcpy(CSVTable->StrCampi[15], "Aux10");
              strcpy(CSVTable->StrCampi[16], "Occupato1");
              strcpy(CSVTable->StrCampi[17], "Occupato2");
              strcpy(CSVTable->StrCampi[18], "Prenotato1");
              strcpy(CSVTable->StrCampi[19], "Prenotato2");
              err = CSVTable->SetTuttiCampiRecord( 30, all_mess );
              if(!err) err = CSVTable->InsertRecord( all_mess );                 // Insert RECORD alla posizione di END OF FILE
           }
           //-----------------------------------------------------------
           // Compilazione della struttura dati
           //-----------------------------------------------------------
           sprintf(CSVTable->StrCampi[ 1], "%d", blocco_man[i].pintend);
           sprintf(CSVTable->StrCampi[ 2], "%d", blocco_man[i].succ1);
           sprintf(CSVTable->StrCampi[ 3], "%d", blocco_man[i].succ2);
           sprintf(CSVTable->StrCampi[ 4], "%d", 0);
           sprintf(CSVTable->StrCampi[ 5], "%d", 0);
           sprintf(CSVTable->StrCampi[ 6], "%d", 0);
           sprintf(CSVTable->StrCampi[ 7], "%d", 0);
           sprintf(CSVTable->StrCampi[ 8], "%d", 0);
           sprintf(CSVTable->StrCampi[ 9], "%d", 0);
           sprintf(CSVTable->StrCampi[10], "%d", 0);
           sprintf(CSVTable->StrCampi[11], "%d", 0);
           sprintf(CSVTable->StrCampi[12], "%d", 0);
           sprintf(CSVTable->StrCampi[13], "%d", 0);
           sprintf(CSVTable->StrCampi[14], "%d", 0);
           sprintf(CSVTable->StrCampi[15], "%d", 0);
           sprintf(CSVTable->StrCampi[16], "%d", blocco_man[i].occu1);
           sprintf(CSVTable->StrCampi[17], "%d", blocco_man[i].occu2);
           sprintf(CSVTable->StrCampi[18], "%d", blocco_man[i].pren1);
           sprintf(CSVTable->StrCampi[19], "%d", blocco_man[i].pren2);
           err = CSVTable->SetTuttiCampiRecord( 30, all_mess );
           if(!err) err = CSVTable->InsertRecord( all_mess );                 // Insert RECORD alla posizione di END OF FILE
           i++;
        }while(blocco_man[i].pintend!=0 && !err && i<100);
     }
     CSVTable->Close();         // CHIUDI TABELLA
     delete CSVTable;
     return err;                // Conversione terminata
  }
  //-----------------------------------------------------------
  // Se non è stata eseguita la conversione del vecchio
  // formato di tabella eseguo una lettura.
  //-----------------------------------------------------------
  // Inizializzazione della tabella CSV
  //-----------------------------------------------------------
  strcpy(CSVTable->NomeDBase,      "GestioneBlocchiManuali1");
  strcpy(CSVTable->SottoDirectory, "");
  CSVTable->CarattereSeparatore    = ';';
  CSVTable->TipoFile               = DBSEQ_TIPO_FILE_STANDARD;
  //-----------------------------------------------------------
  // Apertura della tabella CSV
  //
  // CONVERSIONE DEL VECCHIO FORMATO "GestioneBlocchiManuali"
  // NELLA NUOVA VERSIONE "GestioneBlocchiManuali1"
  //-----------------------------------------------------------
  err = CSVTable->Open( all_mess );         // APRI TABELLA
  if(err==0){                               // Eseguo la lettura solo se non è stata fatta una conversione
     //-----------------------------------------------------------
     // Posizionamento sul primo record
     //-----------------------------------------------------------
     err=CSVTable->ReadFirstRecord(all_mess);
     //-----------------------------------------------------------
     // Salto la riga dell'intestazione
     //-----------------------------------------------------------
     if(err==0) err=CSVTable->ReadNextRecord(all_mess);
     //-----------------------------------------------------------
     // Recupero dati in sequenza
     //-----------------------------------------------------------
     if(err==0){
        do{
           //-----------------------------------------------------------
           // Compilazione della struttura dati
           //-----------------------------------------------------------
           stop=CSVTable->GetTuttiCampiRecord( 30, all_mess );
           blocco_man[i].pintend = (short int)(atoi(CSVTable->StrCampi[ 1]));
           blocco_man[i].succ1   = (short int)(atoi(CSVTable->StrCampi[ 2]));
           blocco_man[i].succ2   = (short int)(atoi(CSVTable->StrCampi[ 3]));
           blocco_man[i].succ3   = (short int)(atoi(CSVTable->StrCampi[ 4]));
           blocco_man[i].succ4   = (short int)(atoi(CSVTable->StrCampi[ 5]));
           blocco_man[i].succ5   = (short int)(atoi(CSVTable->StrCampi[ 6]));
           blocco_man[i].aux1    = (short int)(atoi(CSVTable->StrCampi[ 7]));
           blocco_man[i].aux2    = (short int)(atoi(CSVTable->StrCampi[ 8]));
           blocco_man[i].aux3    = (short int)(atoi(CSVTable->StrCampi[ 9]));
           blocco_man[i].aux4    = (short int)(atoi(CSVTable->StrCampi[10]));
           blocco_man[i].aux5    = (short int)(atoi(CSVTable->StrCampi[11]));
           blocco_man[i].aux6    = (short int)(atoi(CSVTable->StrCampi[12]));
           blocco_man[i].aux7    = (short int)(atoi(CSVTable->StrCampi[13]));
           blocco_man[i].aux8    = (short int)(atoi(CSVTable->StrCampi[14]));
           blocco_man[i].aux9    = (short int)(atoi(CSVTable->StrCampi[15]));
           blocco_man[i].occu1   = (short int)(atoi(CSVTable->StrCampi[16]));
           blocco_man[i].occu2   = (short int)(atoi(CSVTable->StrCampi[17]));
           blocco_man[i].pren1   = (short int)(atoi(CSVTable->StrCampi[18]));
           blocco_man[i].pren2   = (short int)(atoi(CSVTable->StrCampi[19]));
           blocco_man[i].pren3   = (short int)(atoi(CSVTable->StrCampi[20]));
           blocco_man[i].pren4   = (short int)(atoi(CSVTable->StrCampi[21]));
           blocco_man[i].pren5   = (short int)(atoi(CSVTable->StrCampi[22]));
           i++;
           stop=CSVTable->ReadNextRecord(all_mess);
        }while(!stop && i<200);
     }
  }
  //-----------------------------------------------------------
  // Chiusura della tabella CSV
  //-----------------------------------------------------------
  CSVTable->Close();         // CHIUDI TABELLA
  delete CSVTable;

  return err;
}

// ----------------------------------------
//    compila_zone_fuori_ingombro_manuali
// ----------------------------------------
// Compilazione della struttura delle ZONE FUORI INGOMBRO MANUALI
// alla gestione STANDARD dei percorsi TGV.
//
int nodo::compila_zone_fuori_ingombro_manuali(char *all_mess)
/**********************************/
{
  int i, stop, err;
  DBSeq *CSVTable = new DBSeq;

  i=stop=err=0;
  //-----------------------------------------------------------
  // Azzermento della struttura globale
  //-----------------------------------------------------------
  memset(&zone_f_ing_man, 0, sizeof(zone_f_ing_man));
  //-----------------------------------------------------------
  // Se non è stata eseguita la conversione del vecchio
  // formato di tabella eseguo una lettura.
  //-----------------------------------------------------------
  // Inizializzazione della tabella CSV
  //-----------------------------------------------------------
  strcpy(CSVTable->NomeDBase,      "GestioneZFuoriIngManuali");
  strcpy(CSVTable->SottoDirectory, "");
  CSVTable->CarattereSeparatore    = ';';
  CSVTable->TipoFile               = DBSEQ_TIPO_FILE_STANDARD;
  //-----------------------------------------------------------
  // Apertura della tabella CSV
  //
  // CONVERSIONE DEL VECCHIO FORMATO "GestioneBlocchiManuali"
  // NELLA NUOVA VERSIONE "GestioneBlocchiManuali1"
  //-----------------------------------------------------------
  err = CSVTable->Open( all_mess );         // APRI TABELLA
  if(err==0){                               // Eseguo la lettura solo se non è stata fatta una conversione
     //-----------------------------------------------------------
     // Posizionamento sul primo record
     //-----------------------------------------------------------
     err=CSVTable->ReadFirstRecord(all_mess);
     //-----------------------------------------------------------
     // Salto la riga dell'intestazione
     //-----------------------------------------------------------
     if(err==0) err=CSVTable->ReadNextRecord(all_mess);
     //-----------------------------------------------------------
     // Recupero dati in sequenza
     //-----------------------------------------------------------
     if(err==0){
        do{
           //-----------------------------------------------------------
           // Compilazione della struttura dati
           //-----------------------------------------------------------
           stop=CSVTable->GetTuttiCampiRecord( 30, all_mess );
           zone_f_ing_man[i].pintend = (short int)(atoi(CSVTable->StrCampi[ 1]));
           zone_f_ing_man[i].succ1   = (short int)(atoi(CSVTable->StrCampi[ 2]));
           zone_f_ing_man[i].succ2   = (short int)(atoi(CSVTable->StrCampi[ 3]));
           zone_f_ing_man[i].succ3   = (short int)(atoi(CSVTable->StrCampi[ 4]));
           zone_f_ing_man[i].succ4   = (short int)(atoi(CSVTable->StrCampi[ 5]));
           zone_f_ing_man[i].succ5   = (short int)(atoi(CSVTable->StrCampi[ 6]));
           zone_f_ing_man[i].aux1    = (short int)(atoi(CSVTable->StrCampi[ 7]));
           zone_f_ing_man[i].aux2    = (short int)(atoi(CSVTable->StrCampi[ 8]));
           zone_f_ing_man[i].aux3    = (short int)(atoi(CSVTable->StrCampi[ 9]));
           zone_f_ing_man[i].aux4    = (short int)(atoi(CSVTable->StrCampi[10]));
           zone_f_ing_man[i].aux5    = (short int)(atoi(CSVTable->StrCampi[11]));
           zone_f_ing_man[i].aux6    = (short int)(atoi(CSVTable->StrCampi[12]));
           zone_f_ing_man[i].aux7    = (short int)(atoi(CSVTable->StrCampi[13]));
           zone_f_ing_man[i].aux8    = (short int)(atoi(CSVTable->StrCampi[14]));
           zone_f_ing_man[i].aux9    = (short int)(atoi(CSVTable->StrCampi[15]));
           zone_f_ing_man[i].occu1   = (short int)(atoi(CSVTable->StrCampi[16]));
           zone_f_ing_man[i].occu2   = (short int)(atoi(CSVTable->StrCampi[17]));
           zone_f_ing_man[i].pren1   = (short int)(atoi(CSVTable->StrCampi[18]));
           zone_f_ing_man[i].pren2   = (short int)(atoi(CSVTable->StrCampi[19]));
           zone_f_ing_man[i].pren3   = (short int)(atoi(CSVTable->StrCampi[20]));
           zone_f_ing_man[i].pren4   = (short int)(atoi(CSVTable->StrCampi[21]));
           zone_f_ing_man[i].pren5   = (short int)(atoi(CSVTable->StrCampi[22]));
           i++;
           stop=CSVTable->ReadNextRecord(all_mess);
        }while(!stop && i<200);
     }
  }
  //-----------------------------------------------------------
  // Chiusura della tabella CSV
  //-----------------------------------------------------------
  CSVTable->Close();         // CHIUDI TABELLA
  delete CSVTable;

  return err;
}

// -------------------------------------
//    verifica_blocco_manuale
// -------------------------------------
// Verifica se per un dato nodo sono previsti blocchi di percorso.
//
// ritorna TRUE se almeno un blocco e stato individuato
//
bool nodo::verifica_blocco_manuale(short int num_agv, short int punto, short int *agv_bloccante)
/**********************************/
{
  int i, j, ptr_percorso;
  bool blocco_manuale;


  *agv_bloccante=0;
  //--------------------------------------------------------------
  // Individuo l'indice di percorso a cui è arrivato il TGV
  //--------------------------------------------------------------
  for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
     if(AGV[num_agv]->mission.punto[ptr_percorso] == punto) break;
     if(AGV[num_agv]->mission.punto[ptr_percorso] == 0) break;
  }
  if(AGV[num_agv]->mission.punto[ptr_percorso]!=punto) return FALSE;
  //--------------------------------------------------------------
  // Se il sucessivo al nodo considerato è già occupato dal TGV
  // allora "punto" non può più essere forzato come "pintend"
  //--------------------------------------------------------------
  if(AGV[num_agv]->mission.punto[ptr_percorso+1]!=0 && N.nodo_busy[AGV[num_agv]->mission.punto[ptr_percorso+1]]==num_agv) return FALSE;
  //--------------------------------------------------------------
  // BLOCCO MANUALE OCCUPAZIONE NODI
  // Gestione forzatura dei PINTEND solo in alcune condizioni
  // variabili a seconda dell'impianto
  //--------------------------------------------------------------
  blocco_manuale=false;
  for(i=0; i<200; i++){
     if(blocco_manuale==true) break;                                     // blocco individuato
     if(N.blocco_man[i].pintend!=punto) continue;
     //---------------------------------------------------------------
     // 1) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1==0 &&
        N.blocco_man[i].succ2==0 &&
        N.blocco_man[i].succ3==0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1==0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1==0 &&
        N.blocco_man[i].pren2==0 &&
        N.blocco_man[i].pren3==0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && AGV[num_agv]->stato.pos!=punto) blocco_manuale=true;
     }
     //---------------------------------------------------------------
     // 2) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  OCCU1
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1==0 &&
        N.blocco_man[i].succ2==0 &&
        N.blocco_man[i].succ3==0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1==0 &&
        N.blocco_man[i].pren2==0 &&
        N.blocco_man[i].pren3==0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv){
           blocco_manuale=true;
           *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
        }
     }
     //---------------------------------------------------------------
     // 3) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  OCCU1
     //  -  OCCU2
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1==0 &&
        N.blocco_man[i].succ2==0 &&
        N.blocco_man[i].succ3==0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2!=0 &&
        N.blocco_man[i].pren1==0 &&
        N.blocco_man[i].pren2==0 &&
        N.blocco_man[i].pren3==0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             N.nodo_busy[N.blocco_man[i].occu2]!=0 && N.nodo_busy[N.blocco_man[i].occu2]!=num_agv &&
                                             N.nodo_busy[N.blocco_man[i].occu1]==N.nodo_busy[N.blocco_man[i].occu2]){
           blocco_manuale=true;
           *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
        }
     }
     //---------------------------------------------------------------
     // 4) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  OCCU1
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2==0 &&
        N.blocco_man[i].succ3==0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1==0 &&
        N.blocco_man[i].pren2==0 &&
        N.blocco_man[i].pren3==0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1){
           blocco_manuale=true;
           *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
        }
     }
     //---------------------------------------------------------------
     // 5) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  OCCU1
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3==0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1==0 &&
        N.blocco_man[i].pren2==0 &&
        N.blocco_man[i].pren3==0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2){
           blocco_manuale=true;
           *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
        }
     }
     //---------------------------------------------------------------
     // 6) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  OCCU1
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3!=0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1==0 &&
        N.blocco_man[i].pren2==0 &&
        N.blocco_man[i].pren3==0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2 &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+3]==N.blocco_man[i].succ3){
           blocco_manuale=true;
           *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
        }
     }
     //---------------------------------------------------------------
     // 7) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  OCCU1
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3!=0 &&
        N.blocco_man[i].succ4!=0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1==0 &&
        N.blocco_man[i].pren2==0 &&
        N.blocco_man[i].pren3==0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2 &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+3]==N.blocco_man[i].succ3 &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+4]==N.blocco_man[i].succ4){
           blocco_manuale=true;
           *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
        }
     }
     //---------------------------------------------------------------
     // 8) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  OCCU1
     //  -  PREN1
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3==0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2==0 &&
        N.blocco_man[i].pren3==0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 9) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3==0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3==0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 10) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3==0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3!=0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren3][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 11) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3==0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3!=0 &&
        N.blocco_man[i].pren4!=0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren3][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren4][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 12) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  PREN5
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3==0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3!=0 &&
        N.blocco_man[i].pren4!=0 &&
        N.blocco_man[i].pren5!=0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren3][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren4][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren5][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 13) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3!=0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3==0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.blocco_man[i].succ3){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 14) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3!=0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3!=0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.blocco_man[i].succ3){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren3][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 15) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3!=0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3!=0 &&
        N.blocco_man[i].pren4!=0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.blocco_man[i].succ3){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren3][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren4][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 16) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  PREN5
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3!=0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3!=0 &&
        N.blocco_man[i].pren4!=0 &&
        N.blocco_man[i].pren5!=0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.blocco_man[i].succ3){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren3][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren4][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren5][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 17) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3!=0 &&
        N.blocco_man[i].succ4!=0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3==0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.blocco_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.blocco_man[i].succ4){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 18) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3!=0 &&
        N.blocco_man[i].succ4!=0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3!=0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.blocco_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.blocco_man[i].succ4){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren3][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 18) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3!=0 &&
        N.blocco_man[i].succ4!=0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3!=0 &&
        N.blocco_man[i].pren4!=0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.blocco_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.blocco_man[i].succ4){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren3][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren4][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 19) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  PREN5
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3!=0 &&
        N.blocco_man[i].succ4!=0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3!=0 &&
        N.blocco_man[i].pren4!=0 &&
        N.blocco_man[i].pren5!=0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.blocco_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.blocco_man[i].succ4){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren3][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren4][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren5][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 12) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  SUCC5
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3!=0 &&
        N.blocco_man[i].succ4!=0 &&
        N.blocco_man[i].succ5!=0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3==0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.blocco_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.blocco_man[i].succ4 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+5]==N.blocco_man[i].succ5){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 13) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  SUCC5
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3!=0 &&
        N.blocco_man[i].succ4!=0 &&
        N.blocco_man[i].succ5!=0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3!=0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.blocco_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.blocco_man[i].succ4 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+5]==N.blocco_man[i].succ5){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren3][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 14) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  SUCC5
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3!=0 &&
        N.blocco_man[i].succ4!=0 &&
        N.blocco_man[i].succ5!=0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3!=0 &&
        N.blocco_man[i].pren4!=0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.blocco_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.blocco_man[i].succ4 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+5]==N.blocco_man[i].succ5){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren3][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren4][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 15) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  SUCC5
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  PREN5
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3!=0 &&
        N.blocco_man[i].succ4!=0 &&
        N.blocco_man[i].succ5!=0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1!=0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3!=0 &&
        N.blocco_man[i].pren4!=0 &&
        N.blocco_man[i].pren5!=0){
        if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.blocco_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.blocco_man[i].succ4 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+5]==N.blocco_man[i].succ5){
                                             //-----------------------------------------------------------------
                                             // Verifico se altro_agv ha dei nodi prenotati
                                             //-----------------------------------------------------------------
                                             if(N.nodo_busy[N.blocco_man[i].occu1]>0 && N.nodo_busy[N.blocco_man[i].occu1]<=MAXAGV){
                                               if(nodi_prenotati[N.blocco_man[i].pren1][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren2][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren3][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren4][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1] &&
                                                  nodi_prenotati[N.blocco_man[i].pren5][N.nodo_busy[N.blocco_man[i].occu1]-1]==N.nodo_busy[N.blocco_man[i].occu1]){
                                                  blocco_manuale=true;
                                                  *agv_bloccante=N.nodo_busy[N.blocco_man[i].occu1];
                                               }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 20) struttura compilata nel seguente modo:
     //     14/03/2017 GB Controllo Solo Nodi prenotati
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1!=0 &&
        N.blocco_man[i].succ2!=0 &&
        N.blocco_man[i].succ3==0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1==0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1!=0 &&
        N.blocco_man[i].pren2!=0 &&
        N.blocco_man[i].pren3!=0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        //if(punto==N.blocco_man[i].pintend && N.nodo_busy[N.blocco_man[i].occu1]!=0 && N.nodo_busy[N.blocco_man[i].occu1]!=num_agv &&
        if(punto==N.blocco_man[i].pintend && AGV[num_agv]->mission.punto[ptr_percorso+1]==N.blocco_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.blocco_man[i].succ2){
            //-----------------------------------------------------------------
            // Verifico se altro_agv ha dei nodi prenotati
            //-----------------------------------------------------------------
            for(j=1; j<=MAXAGV; j++){
              if(j==num_agv) continue;
              if(nodi_prenotati[N.blocco_man[i].pren1][j-1]==j &&
                 nodi_prenotati[N.blocco_man[i].pren2][j-1]==j &&
                 nodi_prenotati[N.blocco_man[i].pren3][j-1]==j){
                 blocco_manuale=true;
                 *agv_bloccante=j;
              }
            }
        }
     }
  }
  return blocco_manuale;
}

// -------------------------------------
//    verifica_pint_forzato
// -------------------------------------
// Verifica se per un dato nodo sono previsti blocchi di percorso.
//
// ritorna TRUE se almeno un blocco e stato individuato
//
bool nodo::verifica_pint_forzato(short int punto)
/**********************************/
{
  int  i;
  bool pint_forzato;

  //--------------------------------------------------------------
  // BLOCCO MANUALE OCCUPAZIONE NODI
  // Gestione forzatura dei PINTEND solo in alcune condizioni
  // variabili a seconda dell'impianto
  //--------------------------------------------------------------
  pint_forzato=false;
  for(i=0; i<200; i++){
     if(pint_forzato==true) break;                                     // blocco individuato
     if(N.blocco_man[i].pintend!=punto) continue;
     //---------------------------------------------------------------
     // 1) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.blocco_man[i].pintend!=0 &&
        N.blocco_man[i].succ1==0 &&
        N.blocco_man[i].succ2==0 &&
        N.blocco_man[i].succ3==0 &&
        N.blocco_man[i].succ4==0 &&
        N.blocco_man[i].succ5==0 &&
        N.blocco_man[i].aux1==0 &&
        N.blocco_man[i].aux2==0 &&
        N.blocco_man[i].aux3==0 &&
        N.blocco_man[i].aux4==0 &&
        N.blocco_man[i].aux5==0 &&
        N.blocco_man[i].aux6==0 &&
        N.blocco_man[i].aux7==0 &&
        N.blocco_man[i].aux8==0 &&
        N.blocco_man[i].aux9==0 &&
        N.blocco_man[i].occu1==0 &&
        N.blocco_man[i].occu2==0 &&
        N.blocco_man[i].pren1==0 &&
        N.blocco_man[i].pren2==0 &&
        N.blocco_man[i].pren3==0 &&
        N.blocco_man[i].pren4==0 &&
        N.blocco_man[i].pren5==0){
        if(punto==N.blocco_man[i].pintend) pint_forzato=true;
     }
  }
  return pint_forzato;
}

// -----------------------------------------
//    verifica_zona_fuori_ingombro_manuale
// -----------------------------------------
// Verifica se per un dato nodo sono previsti condizioni per il settaggio di una ZONA FUORI INGOMBRO MANUALE.
//
// ritorna TRUE se almeno una condizione di ZONA FUORI INGOMBRO MANUALE ? stata trovata
//
bool nodo::verifica_zona_fuori_ingombro_manuale(short int num_agv, short int punto, short int agv_bloccante)
/**********************************/
{
  int i, j, ptr_percorso;
  bool sequenza_ok;
  short int altro_agv;
  short int ScartoAgv;
  bool nodo_fuori_ingombro;
  bool prenotato_gia_occupato;
  bool zona_fuori_ingombro_manuale;
  int NodoOccupabile, AgvPerInterruzionePercorso, satellite_occupato, NodoInterruzionePercorso;

  nodo_fuori_ingombro=false;
  zona_fuori_ingombro_manuale=false;
  //--------------------------------------------------------------
  // Ammessi solo TGV validi o "0"
  //--------------------------------------------------------------
  if(agv_bloccante<0       ) agv_bloccante=0;
  if(agv_bloccante>MAXAGV  ) agv_bloccante=0;
  if(agv_bloccante==num_agv) agv_bloccante=0;
  //--------------------------------------------------------------
  // Individuo l'indice di percorso a cui ? arrivato il TGV
  //--------------------------------------------------------------
  for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
     if(AGV[num_agv]->mission.punto[ptr_percorso] == punto) break;
     if(AGV[num_agv]->mission.punto[ptr_percorso] == 0) break;
  }
  if(AGV[num_agv]->mission.punto[ptr_percorso]!=punto) return FALSE;
  //--------------------------------------------------------------
  // Se il sucessivo al nodo considerato ? gi? occupato dal TGV
  // allora "punto" non pu? pi? essere forzato come "pintend"
  //--------------------------------------------------------------
  if(AGV[num_agv]->mission.punto[ptr_percorso+1]!=0 && N.nodo_busy[AGV[num_agv]->mission.punto[ptr_percorso+1]]==num_agv) return FALSE;
  //--------------------------------------------------------------
  // ZONA FUORI INGOMBRO MANUALE
  // Se ? capitato un PINT al TGV in un dato punto, con determinate
  // condizioni della navetta bloccante ? comunque possibile
  // continuare.
  //--------------------------------------------------------------
  for(i=0; i<200; i++){
     if(zona_fuori_ingombro_manuale==true ) break;                                     // zona individuato
     if(N.zone_f_ing_man[i].pintend!=punto) continue;
     //---------------------------------------------------------------
     // 1) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1==0 &&
        N.zone_f_ing_man[i].succ2==0 &&
        N.zone_f_ing_man[i].succ3==0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1==0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1==0 &&
        N.zone_f_ing_man[i].pren2==0 &&
        N.zone_f_ing_man[i].pren3==0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend){zona_fuori_ingombro_manuale=true; nodo_fuori_ingombro=true;}
     }
     //---------------------------------------------------------------
     // 2) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  OCCU1
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1==0 &&
        N.zone_f_ing_man[i].succ2==0 &&
        N.zone_f_ing_man[i].succ3==0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1==0 &&
        N.zone_f_ing_man[i].pren2==0 &&
        N.zone_f_ing_man[i].pren3==0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante))){
           //------------------------------------------------------
           // Se viene controllato solo OCCU1 significa che il
           // tgv occupante deve essere fermo sul nodo o in
           // missione, ma senza altri nodi occupati
           //------------------------------------------------------
           altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
           if(altro_agv>0 && altro_agv<=MAXAGV){
              //--------------------------------------------------
              // Altro agv con missione in corso PINT==OCCU1
              //--------------------------------------------------
              if(AGV[altro_agv]->test_mission_on()){
                 if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) zona_fuori_ingombro_manuale=true;
              }
              //--------------------------------------------------
              // Altro agv no missione in corso POS==OCCU1
              //--------------------------------------------------
              else{
                 if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) zona_fuori_ingombro_manuale=true;
              }
           }
        }
     }
     //---------------------------------------------------------------
     // 3) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  OCCU1
     //  -  OCCU2
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1==0 &&
        N.zone_f_ing_man[i].succ2==0 &&
        N.zone_f_ing_man[i].succ3==0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2!=0 &&
        N.zone_f_ing_man[i].pren1==0 &&
        N.zone_f_ing_man[i].pren2==0 &&
        N.zone_f_ing_man[i].pren3==0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv && N.nodo_busy[N.zone_f_ing_man[i].occu2]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu2]!=num_agv) ||
                                                  (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante && N.nodo_busy[N.zone_f_ing_man[i].occu2]==agv_bloccante))){
           //--------------------------------------------------------------
           // La sequenza OCCU1, OCCU2 deve essere quella di occupazione,
           // non basta che siano entrambi occupati
           //--------------------------------------------------------------
           sequenza_ok=false;
           altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
           for(j=0; j<MAXPERCORSI; j++){
              if(AGV[altro_agv]->mission.punto[j]==0) break;
              if(AGV[altro_agv]->mission.punto[j]!=N.zone_f_ing_man[i].occu1       ) continue;
              if(j>0 && N.nodo_busy[AGV[altro_agv]->mission.punto[j-1]]==altro_agv ) continue; // !!! IMPORTANTE !!!
                                                                                               // il precedente al nodo considerato non deve essere occupato
                                                                                               // si devono valutare solo OCCU1 e OCCU2
              //-----------------------------------------------
              // OCCU1 = CONSIDERATO e OCCU2 = SUCCESSIVO
              //-----------------------------------------------
              if(AGV[altro_agv]->mission.punto[j+1]!=0 &&  AGV[altro_agv]->mission.punto[j+1]==N.zone_f_ing_man[i].occu2){
                 sequenza_ok=true;
                 break;
              }
           }
        }
        if(sequenza_ok==true) zona_fuori_ingombro_manuale=true;
     }
     //---------------------------------------------------------------
     // 4) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  OCCU1
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2==0 &&
        N.zone_f_ing_man[i].succ3==0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1==0 &&
        N.zone_f_ing_man[i].pren2==0 &&
        N.zone_f_ing_man[i].pren3==0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                 AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1){
           //------------------------------------------------------
           // Se viene controllato solo OCCU1 significa che il
           // tgv occupante deve essere fermo sul nodo o in
           // missione, ma senza altri nodi occupati
           //------------------------------------------------------
           altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
           if(altro_agv>0 && altro_agv<=MAXAGV){
              //--------------------------------------------------
              // Altro agv con missione in corso PINT==OCCU1
              //--------------------------------------------------
              if(AGV[altro_agv]->test_mission_on()){
                 if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) zona_fuori_ingombro_manuale=true;
              }
              //--------------------------------------------------
              // Altro agv no missione in corso POS==OCCU1
              //--------------------------------------------------
              else{
                 if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) zona_fuori_ingombro_manuale=true;
              }
           }
        }
     }
     //---------------------------------------------------------------
     // 5) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  OCCU1
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3==0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1==0 &&
        N.zone_f_ing_man[i].pren2==0 &&
        N.zone_f_ing_man[i].pren3==0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2){
           //------------------------------------------------------
           // Se viene controllato solo OCCU1 significa che il
           // tgv occupante deve essere fermo sul nodo o in
           // missione, ma senza altri nodi occupati
           //------------------------------------------------------
           altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
           if(altro_agv>0 && altro_agv<=MAXAGV){
              //--------------------------------------------------
              // Altro agv con missione in corso PINT==OCCU1
              //--------------------------------------------------
              if(AGV[altro_agv]->test_mission_on()){
                 if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) zona_fuori_ingombro_manuale=true;
              }
              //--------------------------------------------------
              // Altro agv no missione in corso POS==OCCU1
              //--------------------------------------------------
              else{
                 if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) zona_fuori_ingombro_manuale=true;
              }
           }
        }
     }
     //---------------------------------------------------------------
     // 6) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  OCCU1
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3!=0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1==0 &&
        N.zone_f_ing_man[i].pren2==0 &&
        N.zone_f_ing_man[i].pren3==0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2 &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+3]==N.zone_f_ing_man[i].succ3){
           //------------------------------------------------------
           // Se viene controllato solo OCCU1 significa che il
           // tgv occupante deve essere fermo sul nodo o in
           // missione, ma senza altri nodi occupati
           //------------------------------------------------------
           altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
           if(altro_agv>0 && altro_agv<=MAXAGV){
              //--------------------------------------------------
              // Altro agv con missione in corso PINT==OCCU1
              //--------------------------------------------------
              if(AGV[altro_agv]->test_mission_on()){
                 if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) zona_fuori_ingombro_manuale=true;
              }
              //--------------------------------------------------
              // Altro agv no missione in corso POS==OCCU1
              //--------------------------------------------------
              else{
                 if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) zona_fuori_ingombro_manuale=true;
              }
           }
        }
     }
     //---------------------------------------------------------------
     // 7) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  OCCU1
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3!=0 &&
        N.zone_f_ing_man[i].succ4!=0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1==0 &&
        N.zone_f_ing_man[i].pren2==0 &&
        N.zone_f_ing_man[i].pren3==0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2 &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+3]==N.zone_f_ing_man[i].succ3 &&
                                            AGV[num_agv]->mission.punto[ptr_percorso+4]==N.zone_f_ing_man[i].succ4){
           //------------------------------------------------------
           // Se viene controllato solo OCCU1 significa che il
           // tgv occupante deve essere fermo sul nodo o in
           // missione, ma senza altri nodi occupati
           //------------------------------------------------------
           altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
           if(altro_agv>0 && altro_agv<=MAXAGV){
              //--------------------------------------------------
              // Altro agv con missione in corso PINT==OCCU1
              //--------------------------------------------------
              if(AGV[altro_agv]->test_mission_on()){
                 if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) zona_fuori_ingombro_manuale=true;
              }
              //--------------------------------------------------
              // Altro agv no missione in corso POS==OCCU1
              //--------------------------------------------------
              else{
                 if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) zona_fuori_ingombro_manuale=true;
              }
           }
        }
     }
     //---------------------------------------------------------------
     // 8) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  OCCU1
     //  -  PREN1
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3==0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2==0 &&
        N.zone_f_ing_man[i].pren3==0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                   if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 9) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3==0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3==0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 10) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3==0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3!=0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren3][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 11) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3==0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3!=0 &&
        N.zone_f_ing_man[i].pren4!=0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren3][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren4][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 12) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  PREN5
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3==0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3==0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren3][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren4][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren5][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 13) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3!=0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3==0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.zone_f_ing_man[i].succ3){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 14) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3!=0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3!=0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.zone_f_ing_man[i].succ3){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren3][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 15) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3!=0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3!=0 &&
        N.zone_f_ing_man[i].pren4!=0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.zone_f_ing_man[i].succ3){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren3][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren4][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 16) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  PREN5
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3!=0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3!=0 &&
        N.zone_f_ing_man[i].pren4!=0 &&
        N.zone_f_ing_man[i].pren5!=0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.zone_f_ing_man[i].succ3){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren3][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren4][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren5][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 17) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3!=0 &&
        N.zone_f_ing_man[i].succ4!=0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3==0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.zone_f_ing_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.zone_f_ing_man[i].succ4){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 18) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3!=0 &&
        N.zone_f_ing_man[i].succ4!=0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3!=0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.zone_f_ing_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.zone_f_ing_man[i].succ4){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren3][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 19) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3!=0 &&
        N.zone_f_ing_man[i].succ4!=0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3!=0 &&
        N.zone_f_ing_man[i].pren4!=0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.zone_f_ing_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.zone_f_ing_man[i].succ4){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren3][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren4][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 20) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  PREN5
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3!=0 &&
        N.zone_f_ing_man[i].succ4!=0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3!=0 &&
        N.zone_f_ing_man[i].pren4!=0 &&
        N.zone_f_ing_man[i].pren5!=0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.zone_f_ing_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.zone_f_ing_man[i].succ4){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren3][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren4][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren5][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 21) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  SUCC5
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3!=0 &&
        N.zone_f_ing_man[i].succ4!=0 &&
        N.zone_f_ing_man[i].succ5!=0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3==0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.zone_f_ing_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.zone_f_ing_man[i].succ4 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+5]==N.zone_f_ing_man[i].succ5){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 22) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  SUCC5
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3!=0 &&
        N.zone_f_ing_man[i].succ4!=0 &&
        N.zone_f_ing_man[i].succ5!=0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3!=0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.zone_f_ing_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.zone_f_ing_man[i].succ4 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+5]==N.zone_f_ing_man[i].succ5){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren3][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 23) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  SUCC5
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3!=0 &&
        N.zone_f_ing_man[i].succ4!=0 &&
        N.zone_f_ing_man[i].succ5!=0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3!=0 &&
        N.zone_f_ing_man[i].pren4!=0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.zone_f_ing_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.zone_f_ing_man[i].succ4 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+5]==N.zone_f_ing_man[i].succ5){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren3][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren4][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
     //---------------------------------------------------------------
     // 24) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  SUCC1
     //  -  SUCC2
     //  -  SUCC3
     //  -  SUCC4
     //  -  SUCC5
     //  -  OCCU1
     //  -  PREN1
     //  -  PREN2
     //  -  PREN3
     //  -  PREN4
     //  -  PREN5
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1!=0 &&
        N.zone_f_ing_man[i].succ2!=0 &&
        N.zone_f_ing_man[i].succ3!=0 &&
        N.zone_f_ing_man[i].succ4!=0 &&
        N.zone_f_ing_man[i].succ5!=0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1!=0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1!=0 &&
        N.zone_f_ing_man[i].pren2!=0 &&
        N.zone_f_ing_man[i].pren3!=0 &&
        N.zone_f_ing_man[i].pren4!=0 &&
        N.zone_f_ing_man[i].pren5!=0){
        if(punto==N.zone_f_ing_man[i].pintend && ((agv_bloccante==0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]!=num_agv) || (agv_bloccante!=0 && N.nodo_busy[N.zone_f_ing_man[i].occu1]==agv_bloccante)) &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+1]==N.zone_f_ing_man[i].succ1 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+2]==N.zone_f_ing_man[i].succ2 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+3]==N.zone_f_ing_man[i].succ3 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+4]==N.zone_f_ing_man[i].succ4 &&
                                             AGV[num_agv]->mission.punto[ptr_percorso+5]==N.zone_f_ing_man[i].succ5){
                                             //------------------------------------------------------
                                             // Controllo prenotati altra navetta
                                             //------------------------------------------------------
                                             prenotato_gia_occupato=false;
                                             altro_agv=N.nodo_busy[N.zone_f_ing_man[i].occu1];
                                             if(altro_agv>0 && altro_agv<=MAXAGV){
                                                //-----------------------------------------------------------------
                                                // AL-03/07/2019 - il TGV deve aver già raggiunto il nodo occupato
                                                //-----------------------------------------------------------------
                                                if(ZoneFuoriIngManConStopPrenotati==true || N.zone_f_ing_man[i].aux9==1) prenotato_gia_occupato=true;
                                                //--------------------------------------------------
                                                // Altro agv con missione in corso PINT==OCCU1
                                                //--------------------------------------------------
                                                if(AGV[altro_agv]->test_mission_on()){
                                                   if(AGV[altro_agv]->mission.pintend==N.zone_f_ing_man[i].occu1 && AGV[altro_agv]->stato.pos==AGV[altro_agv]->mission.pintend) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Altro agv no missione in corso POS==OCCU1
                                                //--------------------------------------------------
                                                else{
                                                   if(AGV[altro_agv]->stato.pos==N.zone_f_ing_man[i].occu1) prenotato_gia_occupato=false;
                                                }
                                                //--------------------------------------------------
                                                // Verifico nodi prenotati da stesso altro agv
                                                //--------------------------------------------------
                                                if(prenotato_gia_occupato==false){
                                                  if(nodi_prenotati[N.zone_f_ing_man[i].pren1][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren2][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren3][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren4][altro_agv-1]==altro_agv &&
                                                     nodi_prenotati[N.zone_f_ing_man[i].pren5][altro_agv-1]==altro_agv) zona_fuori_ingombro_manuale=true;
                                                }
                                             }
        }
     }
  }
  //---------------------------------------------------------------
  // Recuperata una ZONA FUORI INGOMBRO
  // se il controllo è per uno specifico TGV, non ci devono
  // essere altri nodi occupati da altre navette in un
  // intorno di
  //---------------------------------------------------------------
  if(agv_bloccante>0 && agv_bloccante<=MAXAGV && zona_fuori_ingombro_manuale==true && nodo_fuori_ingombro==false){
     ScartoAgv=agv_bloccante;
     if(ZoneFuoriIngManConStopPrenotati==false || AbilitaGestioneIngombroCurvePerStopTGV>1) ScartoAgv=0;
     NodoOccupabile=AgvPerInterruzionePercorso=satellite_occupato=NodoInterruzionePercorso=0;
     VerificaNodoOccupato(FALSE, num_agv, punto, NodoOccupabile, AgvPerInterruzionePercorso, satellite_occupato, NodoInterruzionePercorso, ScartoAgv, 0);
     if(NodoOccupabile==0 && N.verifica_nodo_fuori_ingombro(satellite_occupato)==false) zona_fuori_ingombro_manuale=false;
  }
  return zona_fuori_ingombro_manuale;
}

// -------------------------------------
//    verifica_nodo_fuori_ingombro
// -------------------------------------
// Verifica se un dato nodo è impostato nelle ZONE FUORI INGOMBRO MANUALI come nodo sempre occupabile.
// viene ritenuto nodo fuori ingombro
//
bool nodo::verifica_nodo_fuori_ingombro(int punto)
/**********************************/
{
  int i;
  bool nodo_fuori_ingombro_manuale;

  nodo_fuori_ingombro_manuale=false;
  //--------------------------------------------------------------
  // ZONA FUORI INGOMBRO MANUALE
  // Se ? capitato un PINT al TGV in un dato punto, con determinate
  // condizioni della navetta bloccante ? comunque possibile
  // continuare.
  //--------------------------------------------------------------
  for(i=0; i<200; i++){
     if(nodo_fuori_ingombro_manuale==true ) break;                                     // zona individuato
     if(N.zone_f_ing_man[i].pintend!=punto) continue;
     //---------------------------------------------------------------
     // 1) struttura compilata nel seguente modo:
     //  -  PINTEND
     //  -  tutti gli altri campi "0"
     //---------------------------------------------------------------
     if(N.zone_f_ing_man[i].pintend!=0 &&
        N.zone_f_ing_man[i].succ1==0 &&
        N.zone_f_ing_man[i].succ2==0 &&
        N.zone_f_ing_man[i].succ3==0 &&
        N.zone_f_ing_man[i].succ4==0 &&
        N.zone_f_ing_man[i].succ5==0 &&
        N.zone_f_ing_man[i].aux1==0 &&
        N.zone_f_ing_man[i].aux2==0 &&
        N.zone_f_ing_man[i].aux3==0 &&
        N.zone_f_ing_man[i].aux4==0 &&
        N.zone_f_ing_man[i].aux5==0 &&
        N.zone_f_ing_man[i].aux6==0 &&
        N.zone_f_ing_man[i].aux7==0 &&
        N.zone_f_ing_man[i].aux8==0 &&
        N.zone_f_ing_man[i].aux9==0 &&
        N.zone_f_ing_man[i].occu1==0 &&
        N.zone_f_ing_man[i].occu2==0 &&
        N.zone_f_ing_man[i].pren1==0 &&
        N.zone_f_ing_man[i].pren2==0 &&
        N.zone_f_ing_man[i].pren3==0 &&
        N.zone_f_ing_man[i].pren4==0 &&
        N.zone_f_ing_man[i].pren5==0){
        if(punto==N.zone_f_ing_man[i].pintend){nodo_fuori_ingombro_manuale=true; break;}
     }
  }
  //--------------------------------------------------------------
  // IMPIANTI SPECIALI: stuttura apposita
  //--------------------------------------------------------------
  if(nodo_fuori_ingombro_manuale==false){
     for(i=0; i<MAXNODIATTESA; i++){
        if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]<1       ) continue;
        if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]>TOTPUNTI) continue;
        if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][i]!=punto  ) continue;
        nodo_fuori_ingombro_manuale=true;
        break;
     }
  }
  return nodo_fuori_ingombro_manuale;
}

// ------------------------
//    load_file_nodi()
// ------------------------
// Dal file NODO.DAT compila i record in memoria precedentemente allocati
//
int nodo::load_file_nodi(char *all_mess)
/**************************************/
{
  int err=0;
  int i, len;
  int  handle;
  struct NOD nod1;

  strcpy( all_mess, "OK" );
  //------------------------------
  // Azzera records in memoria
  //------------------------------
  for( i=0; i<=MAXPUNTI; i++){
      memset( n[i], 0, sizeof( NOD ));
  }
  //------------------------------
  // Apertura file NODO.DAT
  //------------------------------
  handle = sopen( file_name_sorg, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
    //close( handle );
      sprintf(all_mess, MESS[112], file_name_sorg);
      return 1;
  }
  //------------------------------
  // Ciclo lettura file NODO.DAT
  //------------------------------
  for( i=0; i<=MAXPUNTI; i++){
      len = read( handle, n[i], sizeof( NOD ) );
      if (len != sizeof( NOD ))  err = 1;
      if(err){
          memset( n[i], 0, sizeof( NOD ));
          //sprintf(all_mess, MESS[112], file_name_sorg);
          sprintf(all_mess, MESS[344], i);
          break;
      }
  }
  close( handle );

  return err;
}


// ------------------------
//    save_record_nodi()
// ------------------------
// Salva sul file NODO.DAT i records di nodi in memoria
//
int nodo::save_record_nodi(int numero, char *all_mess)
/**********************************/
{
/*
  FILE *ptrfile;
  int err=0;
  long    curpos;
  int len,  pt;
  long length;


  strcpy( all_mess, "OK" );
  //------------------------------
  // Controllo lunghezza file
  //------------------------------
  length=0;
  pt = open(file_name, 0);
  if(pt >-1) length = filelength(pt);
  close(pt);
  if( length < (long)((long) numero * sizeof( NOD))){
     sprintf(all_mess, MESS[113], file_name);
     return 1;
  }
  //------------------------------
  // Apertura file NODO.DAT
  //------------------------------
  ptrfile = fopen( file_name, "rb+");
  if(ptrfile==0) {
     sprintf(all_mess, MESS[113], file_name);
     return 1;
  }
  //--------------------------------------
  // Calcolo dell'OFFSET e posizionamento
  //--------------------------------------
  curpos = (long ) numero * sizeof( NOD);
  err = fseek( ptrfile, curpos, SEEK_SET );    // pos. inizio record
  if( err!=0 ){
     sprintf(all_mess, MESS[127], numero, file_name);
     fclose(ptrfile);
     return 1;
  }
  //-------------------------------
  // Scrittura Record NODO.DAT
  //-------------------------------
  len= fwrite( n[numero], 1, sizeof( NOD ), ptrfile);
  if (len != sizeof( NOD )){
     err=1;
     sprintf(all_mess, MESS[113], file_name);
  }
  fclose(ptrfile);
  return err;
*/
  int err=0;
  long    curpos;
  int len;
  unsigned long length;
  int handle;

  strcpy( all_mess, "OK" );
  //------------------------------
  // Apertura file NODO.DAT
  //------------------------------
  handle = sopen( file_name_dest, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
    //close( handle );
      sprintf(all_mess, MESS[113], file_name_dest);
      return 1;
  }
  //------------------------------
  // Controllo lunghezza file
  //------------------------------
  length=0;
  if(handle >-1) length = filelength(handle);
  if( length < ((long) numero * sizeof( NOD))){
      close(handle);
      sprintf(all_mess, MESS[113], file_name_dest);
      return 1;
  }

  //--------------------------------------
  // Calcolo dell'OFFSET e posizionamento
  //--------------------------------------
  curpos = (long ) numero * sizeof( NOD);
  if( lseek( handle, curpos, SEEK_SET )<1){  // pos. il puntatore a inzio record
      sprintf(all_mess, MESS[110], numero, file_name_dest);
      close( handle );
      return 1;
  }
  //-------------------------------
  // Scrittura Record NODO.DAT
  //-------------------------------
  len= write( handle, n[numero], sizeof( NOD ));
  if (len != sizeof( NOD )){
      err=1;
       sprintf(all_mess, MESS[111], numero, file_name_dest);
  }
  close(handle);

  return err;
}


// ------------------------
//    save_file_nodi()
// ------------------------
// Salva sul file NODO.DAT i records di nodi in memoria
//
int nodo::save_file_nodi(char *all_mess)
/**********************************/
{
/*
  FILE *ptrfile;
  int err=0;
  int i, len;

  strcpy( all_mess, "OK" );

  //------------------------------
  // Apertura file NODO.DAT
  //------------------------------
  ptrfile = fopen( file_name, "wb");
  if (ptrfile==0) {
     sprintf(all_mess, MESS[113], file_name);
     return 1;
  }
  //-------------------------------
  // Ciclo Scrittura file NODO.DAT
  //-------------------------------
  for( i=0; i<=MAXPUNTI; i++){
     len= fwrite( n[i], 1, sizeof( NOD ), ptrfile);
     if (len != sizeof( NOD ))  err = 1;
     if(err){
        memset( n[i], 0, sizeof( NOD ));
        sprintf(all_mess, MESS[128], i, file_name );
        break;
     }
  }
  fclose(ptrfile);
  return err;
*/
  int err=0;
  int i, len;
  int handle;

  strcpy( all_mess, "OK" );

/*
  //---------------------------------------
  // Scommentare e richiamare la funzione
  // save_file_nodi (es:da un bottone)
  // per salvare TUTTI i rallentamenti a 9
  //---------------------------------------
  int j;

  for( i=0; i<=MAXPUNTI; i++){
     for( j=0; j<=MAXPUNTINODO; j++){
        if(n[i]->rec[j].ps!=0) {
           n[i]->rec[j].ral=9;
        }
     }
  }
*/

  //------------------------------
  // Apertura file NODO.DAT
  //------------------------------
  handle = sopen( file_name_dest, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
  if( handle<0 ){
    //close( handle );
      sprintf(all_mess, MESS[113], file_name_dest);
      return 1;
  }
  //-------------------------------
  // Ciclo Scrittura file NODO.DAT
  //-------------------------------
  for( i=0; i<=MAXPUNTI; i++){
      len= write( handle, n[i], sizeof( NOD ));
      if (len != sizeof( NOD ))  err = 1;
      if(err){
          memset( n[i], 0, sizeof( NOD ));
          sprintf(all_mess, MESS[111], i, file_name_dest);
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
// sul numero del nodo (se diverso da zero)
//
int nodo::num_record()
/**********************/
{
  short int numero_rec=0;
  short int i;

  for( i=0; i<MAXPUNTI; i++){
     if( n[i]->num >0 ) numero_rec++;
  }
  return numero_rec;
}


// ------------------------
//    cerca_record()
// ------------------------
// Cerca il record memorizzato in memoria e lo copia nella struttura
// nod, restituisce errore in caso di superamento dei limiti o
// record non inizializzato
//
int nodo::cerca_record(struct NOD *nod, int numero)
/**********************************/
{
  if( numero > MAXPUNTI ) return 1;
  if( n[ numero ]->num ==0 ) return 1;

  memcpy( nod, n[ numero ], sizeof( NOD) );
  return 0;
}

// ----------------------------------------------
//         VisualizzaCompilazioneNodi( )
// ----------------------------------------------
// Compilazione di un File "IMFNODO.DAT" con
// visualizzate le impostazioni di Tutti i nodi o
// di uno solo.
//
//
int nodo::VerificaDistanzaPunti( int Punto1, int Punto2, int *DistX, int *DistY, double *Dist)
/***************************/
{
  int x1, x2, y1, y2;
  int dX, dY;
  double d;

  //----------------------------------
  // Azzeramento variabili
  //----------------------------------
  d=0;
  dX=dY=0;
  *DistX = 0;
  *DistY = 0;
  *Dist  = 0;
  //----------------------------------
  // Entrambi i punti devono validi
  //----------------------------------
  if(Punto1<1 || Punto1>TOTPUNTI) return 1;
  if(Punto2<1 || Punto2>TOTPUNTI) return 1;
  //----------------------------------
  // Recupero coordinate punti
  //----------------------------------
  x1 = Punti[Punto1][0];
  y1 = Punti[Punto1][1];
  x2 = Punti[Punto2][0];
  y2 = Punti[Punto2][1];
  //----------------------------------
  // Se i nodi si trovano in retta la
  // distanza viene calcolata con il
  // teorema di pitagora.
  //----------------------------------
  dX=abs(x1-x2);
  dY=abs(y1-y2);
  if(dX>30000 || dY>30000) return 1;
  d = (double)(sqrt_algo((dX*dX)+(dY*dY)));
  //----------------------------------
  // Assegnazione varibiali
  //----------------------------------
  *DistX = dX;
  *DistY = dY;
  *Dist  = d;
  return 0;
}

// ----------------------------------------------
//         VisualizzaCompilazioneNodi( )
// ----------------------------------------------
// Compilazione di un File "IMFNODO.DAT" con
// visualizzate le impostazioni di Tutti i nodi o
// di uno solo.
//
//
int nodo::VisualizzaCompilazioneNodi( int Nodo )
/***************************/
{
/*
  FILE   *ptrfile;
  short int i=1;
  int n_rec=0;
  struct NOD nod;
  char appoggio[51]="";
  char stringa[101]="";
  char filename[21]="INFNODO.DAT";

  //---------------------------------------
  // Crea il file di visualizzazione
  //---------------------------------------
  ptrfile = fopen(filename, "wb");
  if (ptrfile==0) return 1;

  //-------------------------------------------------------
  // Stampa delle informazioni relative a tutto l'archivio
  // dei nodi.
  //-------------------------------------------------------
  if(Nodo==0){
     //--------------------------------
     //  cerca il primo record
     //--------------------------------
     for( n_rec=1; n_rec<=MAXPUNTI; n_rec++){
        if( n[ n_rec ]->num >0 ) break;
     }
     if( n_rec>=MAXPUNTI ){
        fclose(ptrfile);
        return 0;
     }
  }
  //-------------------------------------------------------
  // Stampa delle informazioni relative a uno solo dei
  // nodi dell'archivio.
  //-------------------------------------------------------
  else n_rec=Nodo;
  //--------------------------------
  //  Stampa  nodo
  //--------------------------------
  do{
     memset(&nod, 0, sizeof(NOD));
     if( cerca_record(&nod, n_rec ) ){ fclose(ptrfile); return 1; }
     sprintf(stringa, MESS[167], nod.num);
     Write_Line(ptrfile, stringa);

     if(nod.nom[0]=='A') strcpy(appoggio, MESS[404]);
     if(nod.nom[0]=='B') strcpy(appoggio, MESS[405]);
     if(nod.nom[0]=='C') strcpy(appoggio, MESS[406]);
     if(nod.nom[0]=='D') strcpy(appoggio, MESS[407]);
     sprintf(stringa, MESS[166], appoggio);
     Write_Line(ptrfile, stringa);

     sprintf(stringa, MESS[165], nod.zp);
     Write_Line(ptrfile, stringa);

     sprintf(stringa, MESS[164], nod.zs);
     Write_Line(ptrfile, stringa);

     sprintf(stringa, MESS[163], nod.busy);
     Write_Line(ptrfile, stringa);

     sprintf(stringa, MESS[162], nod.direz);
     Write_Line(ptrfile, stringa);

     strcpy(stringa,  MESS[159]);
     Write_Line(ptrfile, stringa);

     for(i=0; i<(MAXPUNTINODO-1); i++){
        if(!nod.rec[i].pt && !nod.rec[i].zs &&  !nod.rec[i].zp) break;
        else{
           sprintf(stringa, MESS[161],
                                      i,
                                      nod.rec[i].pt,
                                      nod.rec[i].zs,
                                      nod.rec[i].zp,
                                      nod.rec[i].ps,
                                      nod.rec[i].av,
                                      nod.rec[i].ind,
                                      nod.rec[i].rot,
                                      nod.rec[i].ral,
                                      nod.rec[i].dist);
           Write_Line(ptrfile, stringa);
        }
    }
    //--------------------------------
    //  cerca il prossimo record
    //--------------------------------
    for( n_rec=n_rec+1; n_rec<=MAXPUNTI; n_rec++){
        if( n[ n_rec ]->num >0 ) break;
    }
    if(Nodo!=0) n_rec=MAXPUNTI+1;    // chiudo il file se voglio visualizzare un solo Nodo

    Write_Line(ptrfile, "--------------------------------------------------------------------------------");
    Write_Line(ptrfile, "                                                                                ");
  }while(n_rec <= MAXPUNTI);
  fclose(ptrfile);
*/
  return 0;
}

//==============================================================================
//                  GESTIONE CURVE LASER
//==============================================================================


//-----------------------------------------------------
// CercaCurvaBezier()
// Fuzione scorre la struttura grafica PercorsoCurvaBezier[][] alla ricerca
// di una curva che interessi i due punti P1 e P2 e del "grado" richiesto.
//
// N.b. Se il grado richiesto alla funzione è "1" significa che va bene
//      qualunque grado di curva
//
// Restituisce l'array dei vertici di controllo e il grado
// Ritorna 0 se tutto OK 1 se punti NON VALIDI
//-----------------------------------------------------
int nodo::CercaCurvaBezier(bool SoloCurveGrafiche, int P1, int P2, int *grado, TPoint *p)
{
  int i;
  int NoCurva;

  //---------------------------------------------------------
  // CURVE DI BEZIER
  // Verifico se il tratto appartiene alla stuttura
  // grafica delle curve
  //---------------------------------------------------------
  NoCurva=1;
  for(i=0;;i++){
     //---------------------------------------------------------
     // Fine struttura --> skip!
     //---------------------------------------------------------
     if(PercorsoCurvaBezier[i][1]==0 && PercorsoCurvaBezier[i][2]==0) break;
     //---------------------------------------------------------
     // Tipo di curve in visualizzazione
     // CurveDiBezier==1 --> TUTTE
     // CurveDiBezier==2 --> SOLO QUADRATICE
     // CurveDiBezier==3 --> SOLO CUBICHE
     //---------------------------------------------------------
     if(*grado!=0 && PercorsoCurvaBezier[i][0]!=*grado) continue;
     //---------------------------------------------------------
     // Curva esistente, ma disattivata
     //---------------------------------------------------------
     if(PercorsoCurvaBezier[i][0]==0 && (PercorsoCurvaBezier[i][1]!=0 || PercorsoCurvaBezier[i][2]!=0)) continue;
     //---------------------------------------------------------
     // Recupero il "grado" della curva di Bezier
     //---------------------------------------------------------
     if((P1==PercorsoCurvaBezier[i][1] && P2==PercorsoCurvaBezier[i][2]) || (P1==PercorsoCurvaBezier[i][2] && P2==PercorsoCurvaBezier[i][1])){
        //-----------------------------------------------
        // Assegno il grado recuperato dalla struttura
        //-----------------------------------------------
        *grado = PercorsoCurvaBezier[i][0];
        //-----------------------------------------------
        // Compilazione array di punti nell'ordine
        // di visualizzazione in base al grado della
        // curva
        //
        // (1) QUADRATICA - GRADO 2
        //     Pstart + Pcontrol1 + PEnd
        //
        // (2) CUBICA     - GRADO 3
        //     Pstart + Pcontrol1 + Pcontrol2 + PEnd
        //
        //-----------------------------------------------
        // (1)
        if(*grado==2){
           //-----------------------------------------------
           // COORDINATE GRAFICHE DI VISUALIZZAZIONE
           //-----------------------------------------------
           if(SoloCurveGrafiche==true){
              //-----------------------------------------------------------------
              // Coordinate ingresso e uscita recuperate da struttura Punti[][]
              //-----------------------------------------------------------------
              p[0] = Point(((Punti[P1][0]-offset[1][0])/Rap)*Rid, ((Punti[P1][1]-offset[1][1])/Rap)*Rid);
              p[2] = Point(((Punti[P2][0]-offset[1][0])/Rap)*Rid, ((Punti[P2][1]-offset[1][1])/Rap)*Rid);
              //-----------------------------------------------------------------
              // Coordinate vertici di controllo espresse direttamente in X, Y
              //-----------------------------------------------------------------
              p[1] = Point(((PercorsoCurvaBezier[i][3]-offset[1][0])/Rap)*Rid, ((PercorsoCurvaBezier[i][4]-offset[1][1])/Rap)*Rid);
           }
           //-----------------------------------------------
           // COORDINATE REALI DI LAYOUT PER GUIDA
           //-----------------------------------------------
           else{
              //-----------------------------------------------------------------
              // Coordinate ingresso e uscita recuperate da struttura Punti[][]
              //-----------------------------------------------------------------
              p[0] = Point(Punti[P1][0], Punti[P1][1]);
              p[2] = Point(Punti[P2][0], Punti[P2][1]);
              //-----------------------------------------------------------------
              // Coordinate vertici di controllo espresse direttamente in X, Y
              //-----------------------------------------------------------------
              p[1] = Point(PercorsoCurvaBezier[i][3], PercorsoCurvaBezier[i][4]);
           }
           //-----------------------------------------------------------------
           // Curva grafica trovata --> skip!
           //-----------------------------------------------------------------
           NoCurva=0;
           break;
        }
        // (2)
        if(*grado==3){
           //-----------------------------------------------
           // COORDINATE GRAFICHE DI VISUALIZZAZIONE
           //-----------------------------------------------
           if(SoloCurveGrafiche==true){
              //-----------------------------------------------------------------
              // Coordinate ingresso e uscita recuperate da struttura Punti[][]
              //-----------------------------------------------------------------
              p[0] = Point(((Punti[P1][0]-offset[1][0])/Rap)*Rid, ((Punti[P1][1]-offset[1][1])/Rap)*Rid);
              p[3] = Point(((Punti[P2][0]-offset[1][0])/Rap)*Rid, ((Punti[P2][1]-offset[1][1])/Rap)*Rid);
              //-----------------------------------------------------------------
              // Coordinate vertici di controllo espresse direttamente in X, Y
              //-----------------------------------------------------------------
              p[1] = Point(((PercorsoCurvaBezier[i][3]-offset[1][0])/Rap)*Rid, ((PercorsoCurvaBezier[i][4]-offset[1][1])/Rap)*Rid);
              p[2] = Point(((PercorsoCurvaBezier[i][5]-offset[1][0])/Rap)*Rid, ((PercorsoCurvaBezier[i][6]-offset[1][1])/Rap)*Rid);
           }
           //-----------------------------------------------
           // COORDINATE REALI DI LAYOUT PER GUIDA
           //-----------------------------------------------
           else{
              //-----------------------------------------------------------------
              // Coordinate ingresso e uscita recuperate da struttura Punti[][]
              //-----------------------------------------------------------------
              p[0] = Point(Punti[P1][0], Punti[P1][1]);
              p[3] = Point(Punti[P2][0], Punti[P2][1]);
              //-----------------------------------------------------------------
              // Coordinate vertici di controllo espresse direttamente in X, Y
              //-----------------------------------------------------------------
              p[1] = Point(PercorsoCurvaBezier[i][3], PercorsoCurvaBezier[i][4]);
              p[2] = Point(PercorsoCurvaBezier[i][5], PercorsoCurvaBezier[i][6]);
           }
           //-----------------------------------------------------------------
           // Curva grafica trovata --> skip!
           //-----------------------------------------------------------------
           NoCurva=0;
           break;
        }
     }
  }

  //---------------------------------------------------------
  // Se attivata solo ricerca grafica --> skip!
  //---------------------------------------------------------
  if(SoloCurveGrafiche==true) return NoCurva;

  //---------------------------------------------------------
  // Calcolo vertici di controllo
  //---------------------------------------------------------
  //...
  //...
  //...

  return NoCurva;
}

//-----------------------------------------------------
// CercaCurva()
// Fuzione scorre la struttura grafica PercorsoCurva[][] alla ricerca di una
// curva che interessi i due punti P1 e P2 e se trovata restituisce raggio
// e circonferenza altrimenti fa un calcolo automatico
// Ritorna 0 se tutto OK 1 se punti NON VALIDI
//-----------------------------------------------------
int nodo::CercaCurva(bool SoloCurveGrafiche, int P1, int P2, int* Raggio, int* Circonferenza, int* IdCurva)
{
  int x1, y1, x2, y2;
  int i;
  long double distX, distY;
  long double dist;
  long double rd2, sn2;
  long double raggioDouble=0;    // raggio con precisione maggiore
  long double circonferenzaDouble =0;

  x1 = Punti[ P1 ][0];
  y1 = Punti[ P1 ][1];
  x2 = Punti[ P2 ][0];
  y2 = Punti[ P2 ][1];
  //----------------------------------
  // CALCOLO CORDA
  // Se i nodi si trovano in retta la
  // distanza viene calcolata con il
  // teorema di pitagora.
  // Se 0 --> SKIP
  //----------------------------------
  distX=abs(x1-x2);
  distY=abs(y1-y2);
  if(distX>30000 || distY>30000) return 1;
  dist = (long double)(sqrt_algo((distX*distX)+(distY*distY)));
  if( dist==0 ) return 1;

  *IdCurva=0;
  //-----------------------------------------------
  // RAGGIO
  // Recupera eventualmente raggio da graphvar.cpp
  //-----------------------------------------------
  for(i=0; ; i++){
     if( PercorsoCurva[i][0]==0 && PercorsoCurva[i][1]==0 && PercorsoCurva[i][3]==0) break;
      if(( P1 == PercorsoCurva[i][0] ) && ( P2 == PercorsoCurva[i][1] ) ||
        ( P1 == PercorsoCurva[i][1] ) && ( P2 == PercorsoCurva[i][0] )){
        raggioDouble = (long double) PercorsoCurva[i][3];
        *IdCurva=(int)(i+1);
        break;
     }
  }
  if( SoloCurveGrafiche && *IdCurva==0 ) return 1;

  //-------------------------------------------------------------
  // !!! SICUREZZA !!!
  // Il raggio è l'ipotenusa del triangolo che per cateto ha
  // metà della corda, quindi non può/ quindi se minore di
  // questo valore viene passata la corda come raggio
  //-------------------------------------------------------------
  if( raggioDouble<= dist/2 ) raggioDouble = dist;

  //-------------------------------------------------------------
  // Calcolo di rd2 vedi documento 19/02/2003
  // angolo composto dall'arco di circonferenza
  // 180° = PI   =  3.14159265358979
  //  90  = PI/2 =  1.57079632679489
  // Se la corda è + grande del raggio*(RADICE di 2) allora
  // vuol dire che l'angolo è maggiore di 90° cioè
  // 180° - arcosin ( sn2)
  //-------------------------------------------------------------
  sn2 = dist/2/raggioDouble;
  rd2 =  (long double) asin(sn2)*2;
  if( dist> (raggioDouble*1.414)){
     rd2= M_PI - rd2;
  }
  if( x1==x2 || y1==y2 ) rd2 = (long double) M_PI;

  circonferenzaDouble = raggioDouble*rd2;
  *Raggio             = (int) raggioDouble;
  *Circonferenza      = (int) circonferenzaDouble;

  return 0;
}

// -----------------------------------------------------
//      CalcoloRaggioArcoTangente()
// -----------------------------------------------------
//  Dato un percorso che transita su delle curve (laser) determina il
//  raggio dinamicamente
//
bool nodo::CalcoloRaggioArcoTangente(short int P0, short int P1, short int P2, short int P3, int *raggio, int *arco)
/*********************************************************/
{
  bool Ok=false;
  int nP0, nP1, nP2, nP3;
  int P1_x, P1_y, P2_x, P2_y, P3_x, P3_y,P4_x, P4_y;
  int Arco_approssimato, Raggio_approssimato;
  double P1_P2, P2_P3;
  double Centro_P3, CentroX, CentroY, diff_raggi;
  double ang_P1_P2, ang_P2_P3;
  double Raggio, RaggioABS, alpha, alpha2, beta, beta2, gamma, omega;
  double a, b;


  return FALSE;     // AL-12/09/13 Disattivato perchè dava qualche problema in CABRIONI curva 1203 - 1332
  //-------------------------------------------------------------
  // La funzione lavora correttamente solo se i punti sono
  // passati in senso orario in modo che P0 risulti quello
  // con "X" inferiore quindi se non sono nell'ordine giusto
  // li inverto
  //-------------------------------------------------------------
  nP0=P0; nP1=P1; nP2=P2; nP3=P3;
  if(Punti[P3][0]<Punti[P0][0]){
     nP0=P3; nP1=P2; nP2=P1; nP3=P0;
  }
  //-------------------------------------------------------------
  // Recupero le coordinate dei punti
  //-------------------------------------------------------------
  P1_x = Punti[nP0][0];
  P1_y = Punti[nP0][1];
  P2_x = Punti[nP1][0];
  P2_y = Punti[nP1][1];
  P3_x = Punti[nP2][0];
  P3_y = Punti[nP2][1];
  P4_x = Punti[nP3][0];
  P4_y = Punti[nP3][1];

  //-------------------------------------------------------------
  // Mi Ricavo dalla Trigonometria i Parametri per Calcolarmi il
  // Raggio teorico dell'Arco
  //-------------------------------------------------------------
  //MedioX = (P2_x + P3_x)/2;
  //MedioY = (P2_y + P3_y)/2;
  //---------------------------
  // Lunghezza di P1_P2
  //---------------------------
  P1_P2 = sqrt_algo(abs((P2_x-P1_x)*(P2_x-P1_x)) + abs((P2_y-P1_y)*(P2_y-P1_y)));
  //----------------------------------------
  // Angolo di P1_P2 rispetto all'asse X
  //----------------------------------------
  if(P2_x-P1_x==0){
     ang_P1_P2 = acos( (P2_x-P1_x)/(P1_P2) );
  }
  else{
     ang_P1_P2 = atan (((double)(P1_y-P2_y))/((double)(P2_x-P1_x)));
  }
  //------------------------------------------------------
  // Lunghezza del Tratto P2_P3
  // che dobbiamo sostitire con un arco di circonferenza
  //------------------------------------------------------
  P2_P3       = sqrt_algo(abs((P3_x-P2_x)*(P3_x-P2_x)) + abs((P3_y-P2_y)*(P3_y-P2_y)));
  //--------------------------------------
  // Angolo di P2_P3 rispetto all'asse X
  //--------------------------------------
  ang_P2_P3   = atan (((double)(P2_y-P3_y))/((double)(P3_x-P2_x)));
  //----------------------------------------------------------------------
  // Angolo fra gli ipotetici tratti che congiungono il Centro dell'Arco
  // con il Punto P2 ed il Centro dell'Arco con il punto Medio di P2_P3
  //----------------------------------------------------------------------
  alpha       = ang_P2_P3 - ang_P1_P2;
  //-------------------------------
  // Lunghezza Raggio Arco
  //-------------------------------
  Raggio    = (double)((P2_P3/2)/sin(alpha));
  RaggioABS = (double)(abs(Raggio));
  //---------------------------------------------------------
  // Angoli del triangolo formato dal Raggio che collega
  // il Centro dell'Arco con P2 e le proiezioni del Centro
  // sull'asse X e Y rispetto al punto P2
  //---------------------------------------------------------
  beta  = (M_PI/2) - ang_P1_P2;
  gamma = (M_PI/2) - beta;
  //---------------------------------
  // Coordinate centro arco
  //---------------------------------
  CentroY = (double)(P2_y - (Raggio*sin(beta)));
  CentroX = (double)(P2_x - (Raggio*sin(gamma)));
  //-------------------------------------------
  // Verifica Tangenza Arco al segmento P3_P4_
  //    (Controllo che ci siano 90 gradi
  //     tra il raggio CentroArco_P3 e P3_P4)
  //-------------------------------------------
  //-------------------------------------------
  // Calcolo l'angolo dall'arcotangente solo
  // se il divisore è "0" altrimenti significa
  // che l'angolo è di 90° cioè con tangente
  // infinita
  //-------------------------------------------
  a = (double)(CentroY-P3_y);
  b = (double)(P3_x-CentroX);
  alpha2 = M_PI/2;
  if(b!=0) alpha2 = atan (a/b);
  //-------------------------------------------
  // Calcolo l'angolo dall'arcotangente solo
  // se il divisore è "0" altrimenti significa
  // che l'angolo è di 90° cioè con tangente
  // infinita
  //-------------------------------------------
  a = (double)(P4_y-P3_y);
  b = (double)(P4_x-P3_x);
  beta2 = M_PI/2;
  if(b!=0) beta2 = atan (a/b);
  //-------------------------------------------
  // Calcolo angolo omega
  //-------------------------------------------
  if(P4_y<P3_y){
     omega = (double)(M_PI-alpha2+beta2);
  }
  else{
     omega = (double)(M_PI-alpha2-beta2);
  }
  // --------------------------------------
  // Se l'angolo è tra 92 e 88 gradi Ok
  // --------------------------------------
  Ok=false;
  if(omega<1.61 && omega>1.54               ) Ok = true;
  if(omega<(1.61+M_PI) && omega>(1.54+M_PI) ) Ok = true;
  //----------------------------------------------------------------------
  // Calcolo la lunghezza di Centro_Arco_P3 per verificare che sia uguale
  //    al raggio dell'arco
  //----------------------------------------------------------------------
  Centro_P3   = sqrt_algo(abs((P3_x-CentroX)*(P3_x-CentroX)) + abs((P3_y-CentroY)*(P3_y-CentroY)));
  diff_raggi  = (double)(RaggioABS - Centro_P3);
  if ( (diff_raggi<-5) || (diff_raggi>5) ){
     Ok=false;
  }
  //----------------------------------------------------------------------
  // Calcolo una media tra i due raggi ricavati per minimizzare l'errore
  //----------------------------------------------------------------------
  Raggio_approssimato = (int)((RaggioABS + Centro_P3)/2);
  //Arco_approssimato   = (int)(abs(Raggio_approssimato*(alpha*2)));
  Arco_approssimato   = (int)(abs(Raggio_approssimato*(acos((P2_P3/2)/Raggio_approssimato)*2)));
  if(Ok==true){
     *raggio = Raggio_approssimato;
     *arco   = Arco_approssimato;
  }
  return !Ok;
}

// -----------------------------------------------------
//      calcola_raggi_curva()
// -----------------------------------------------------
//  Dato un percorso che transita su delle curve (laser) determina il
//  raggio dinamicamente
//
int nodo::calcola_raggi_curva(short int *nodi_perc)
/*********************************************************/
{
  int i;
  bool no_tangente;
  bool ing_in_rettaX;
  bool ing_in_rettaY;
  bool out_in_rettaX;
  bool out_in_rettaY;
  bool NoGestioneCurve;
  int IdCurva, IdTmp;
  int ArcoDwg, ArcoTmp;
  int RaggioDwg, RaggioTmp;
  short int P0, P1, P2, P3;
  int x0, y0, x1, y1, x2, y2, x3, y3;
  int raggio_int, arco_int;
  long double distX, distY;
  long double corda;
  long double raggio;
//long double arco;
  long double tn1, rd1, rd2, sn2;

  //------------------------------------
  // Variabil.h -> Disattivazione curve
  //------------------------------------
  NoGestioneCurve=false;
  #ifndef GESTIONE_CURVE
     NoGestioneCurve=true;
  #endif;

  for(i=2; i<MAXPERCORSI-1; i++){
     if(nodi_perc[i+1]==0) break;
     if(NoGestioneCurve  ) break;
     ing_in_rettaX=false;
     ing_in_rettaY=false;
     out_in_rettaX=false;
     out_in_rettaY=false;
     //-------------------------------------------------
     // P0 = punto di arrivo sulla curva
     // P1 = punto inizio curva
     // P2 = punto uscita curva
     // P3 = punto successivo alla curva
     //-------------------------------------------------
   //arco=0;
     raggio=0;
     IdCurva=IdTmp=0;
     ArcoDwg=ArcoTmp=0;
     RaggioDwg=RaggioTmp=0;
     P0 = nodi_perc[i-2];
     P1 = nodi_perc[i-1];
     P2 = nodi_perc[i  ];
     P3 = nodi_perc[i+1];
     //-------------------------------------------------
     // Verifico effettivamente se P1 e P2
     // appartengono a una curva
     //-------------------------------------------------
     if(N.CercaCurva(TRUE, P1, P2, &RaggioDwg, &ArcoDwg, &IdCurva)) continue;
     //-------------------------------------------------
     //                !!! VINCOLO !!!
     // Se P1 e P2 appartengono a una curva è necessario
     // che:
     //             P0 e P1 siano in retta
     //                o
     //             P2 e P3 siano in retta
     //-------------------------------------------------
     if(!N.CercaCurva(TRUE, P0, P1, &RaggioTmp, &ArcoTmp, &IdTmp)) continue;
     if(!N.CercaCurva(TRUE, P2, P3, &RaggioTmp, &ArcoTmp, &IdTmp)) continue;
     //-------------------------------------------------
     // GESTIONE CALCOLO RAGGIO GENERICO DI ARCO CON
     // INGRESSO/USCITA TANGENTE
     //-------------------------------------------------
     raggio_int=0;
     no_tangente = N.CalcoloRaggioArcoTangente(P0, P1, P2, P3, &raggio_int, &arco_int);
     if(no_tangente==false) raggio = raggio_int;
   //arco   = arco_int;
     //-------------------------------------------------
     // GESTIONE STANDARD CALCOLO RAGGIO QUARTO CRF
     //
     // n.b. non necessario in quanto anche
     //      CalcoloRaggioArcoTangente, ma lo lasciamo
     //      per compatibilità con i vecchi impianti
     //-------------------------------------------------
     if(raggio==0){
        //-------------------------------------------------
        // Recupero le coordinate dei punti interessati
        //-------------------------------------------------
        x0 = Punti[ P0 ][0];
        y0 = Punti[ P0 ][1];
        x1 = Punti[ P1 ][0];
        y1 = Punti[ P1 ][1];
        x2 = Punti[ P2 ][0];
        y2 = Punti[ P2 ][1];
        x3 = Punti[ P3 ][0];
        y3 = Punti[ P3 ][1];
        //-------------------------------------------------
        // Verifica ingresso in curva
        //-------------------------------------------------
        distX=abs(x1-x0);
        distY=abs(y1-y0);
        if(distX>30000 || distY>30000) continue;
        if(distX<=5) ing_in_rettaY=true;
        if(distY<=5) ing_in_rettaX=true;
        //-------------------------------------------------
        // Verifica uscita da curva
        //-------------------------------------------------
        distX=abs(x3-x2);
        distY=abs(y3-y2);
        if(distX>30000 || distY>30000) continue;
        if(distX<=5) out_in_rettaY=true;
        if(distY<=5) out_in_rettaX=true;
        //-------------------------------------------------
        // Se manca uscita o ingresso tangente a un retta
        // in X o Y --> skip!
        //-------------------------------------------------
        if(ing_in_rettaX==false &&
           ing_in_rettaY==false &&
           out_in_rettaX==false &&
           out_in_rettaY==false) continue;
        //-------------------------------------------------
        // Calcolo corda sottesa dall'arco di circonfernza
        //-------------------------------------------------
        distX=abs(x2-x1);
        distY=abs(y2-y1);
        if(distX>30000 || distY>30000) continue;
        if(distX<=5    || distY<=5   ) continue;
        corda = (long double)(sqrt_algo((distX*distX)+(distY*distY)));
        if( corda==0 ) continue;
        //-------------------------------------------------
        // Calcolo dell'angolo ALPHA racchiuso tra la corda
        // che unisce i 2 punti e:
        // - le ascisse se il TGV entra in X
        // - le ordinate se il TGV entra in Y
        //-------------------------------------------------
        if(ing_in_rettaY || out_in_rettaY){
           tn1  = (long double)(distY)/(long double)(distX);
        }
        if(ing_in_rettaX || out_in_rettaX){
           tn1  = (long double)(distX)/(long double)(distY);
        }
        rd1  = (long double)(atanl(tn1));
        //-------------------------------------------------
        // RAGGIO
        //-------------------------------------------------
        rd2 = (M_PI/2) - rd1;
        sn2 = (long double)(sinl(rd2));
        if(sn2!=0) raggio = (double)((corda/2)/sn2);
        else raggio = corda;
        //-------------------------------------------------
        // CALCOLO ARCO DI CIRCONFERENZA
        // Calcolo di rd2 vedi documento 19/02/2003
        // angolo composto dall'arco di circonferenza
        // 180° = PI   =  3.14159265358979
        //  90  = PI/2 =  1.57079632679489
        // Se la corda è + grande del raggio*(RADICE di 2)
        // allora vuol dire che l'angolo è maggiore di 90°
        // cioè 180° - arcosin ( sn2)
        //-------------------------------------------------
        sn2 = corda/2/raggio;
        rd2 = (long double) asin(sn2)*2;
        if( corda> (raggio*1.414)){
           rd2= M_PI - rd2;
        }
        if( x1==x2 || y1==y2 ) rd2 = (long double) M_PI;
      //arco = raggio*rd2;
     }
     //-------------------------------------------------
     // Aggiornamento struttura globale raggi curva
     //-------------------------------------------------
     raggi_curve_laser[IdCurva-1][2] = (short int)(RaggioDwg);    //Raggio calcolato da DWG
     raggi_curve_laser[IdCurva-1][3] = (short int)(raggio);       //Raggio calcolato da PC
     raggi_curve_laser[IdCurva-1][4] = (short int)(!no_tangente); //Arco tangente alle tracce di ing/out
  }

  return 0;
}

// -----------------------------------------------------
//      CalcolaGrdRotazioneSuStartCurva()
// -----------------------------------------------------
//  Calcola i gradi di rotazione che deve compiere una navetta
//  laser sul comando di stard
//
//  !!! ATTENZIONE !!!
//
//  CALCOLO ATTENDIBILE SOLO PER ROTAZIONI SU NODO DI START CURVA
//  CON TRATTI ORTOGONALI
//
int nodo::CalcolaGrdRotazioneSuStartTGV(short int num_agv, short int ptr_path, short int *rot_succ, bool *antioraria)
/*********************************************************/
{
  short int nodo, succ;
  short int dir1, dir2;
  short int rot1, rot2;
  short int grd1, grd2;
  int grd_rot_su_start;
  int x1, y1, x2, y2;
  double CatetoX, CatetoY;
  long double radB, grdB;
  struct NOD nod;
  struct rec_linea dati_nod;
  short int rot_standard;
  int trovato;
  short int NodoPrec=0, RotFinale=99;
  //-------------------------------------------------
  // Inizializazzione variabili
  //-------------------------------------------------
  *rot_succ=0;
  x1=y1=x2=y2=0;
  CatetoX=CatetoY=0;
  grd_rot_su_start=grd1=grd2=0;
  //-------------------------------------------------
  // Solo per navette LASER
  //-------------------------------------------------
  if(AGV[num_agv]->mission.GestioneLaser==false) return 0;
  //-------------------------------------------------
  // Il TGV deve avere la missione compilata
  //-------------------------------------------------
  if(AGV[num_agv]->mission.punto[ptr_path]==0                         ) return 0;
  if(AGV[num_agv]->mission.punto[ptr_path]==AGV[num_agv]->mission.pend) return 0;
  //-------------------------------------------------
  // Recupero i gradi effettivi a cui si trova il TGV
  //-------------------------------------------------
  grd1 = AGV[num_agv]->stato.rot_laser;    // ???? da utilizzare per calocolo gradi dal laser
  if((360-grd1)<OFFSET_MIN_COORD_TRATTO_INCLINATO ) grd1=0;
  //-------------------------------------------------
  // Memorizzo poszione attuale TGV + nodo success.
  //-------------------------------------------------
  nodo = AGV[num_agv]->mission.punto[ptr_path  ];
  if(nodo<1 || nodo>TOTPUNTI) return 0;
  succ = AGV[num_agv]->mission.punto[ptr_path+1];
  if(succ<1 || succ>TOTPUNTI) return 0;
  //-------------------------------------------------
  // Con partenza dei PUNTI TERMINALI non calcolo
  // eventuali rotazioni perchè non possibili
  //-------------------------------------------------
  if(ptr_path==0 && N.punti_notevoli[nodo]==true) return 0;

  //-------------------------------------------------
  // Recupero lo stato del bit di rotazione a cui
  // si trova il TGV
  //-------------------------------------------------
  if(ptr_path==0){
     if(AGV[num_agv]->stato.s.bit.grd0  ) rot1=0;
     if(AGV[num_agv]->stato.s.bit.grd90 ) rot1=1;
     if(AGV[num_agv]->stato.s.bit.grd180) rot1=2;
     if(AGV[num_agv]->stato.s.bit.grd270) rot1=3;
  }
  else{
     rot1 = AGV[num_agv]->mission.dati_perc[ptr_path-1].rot;
     dir1 = AGV[num_agv]->mission.dati_perc[ptr_path-1].dir;
     //----------------------------------------------------
     // GESTIONE DEVIAZIONI + GESTIONE CURVE
     // Recupero la rotazione standard abbinata al nodo
     // (non viene richiamata la funzione rot_standard()
     //  perchè  elabora la rotazione in base al
     //  #define CAMBIO_ROTAZIONE_SU_START_CURVA)
     //----------------------------------------------------
     switch(rot1){
        case  0: rot_standard = 0;
                 break;
        case  1: rot_standard = 1;
                 break;
        case  2: rot_standard = 2;
                 break;
        case  3: rot_standard = 3;
                 break;
        case  4: if(!dir1) rot_standard=3;
                 if(dir1 ) rot_standard=1;
                 break;
        case  5: if(!dir1) rot_standard=2;
                 if(dir1 ) rot_standard=0;
                 break;
        case  6: if(!dir1) rot_standard=0;
                 if(dir1 ) rot_standard=2;
                 break;
        case  7: if(!dir1) rot_standard=3;
                 if(dir1 ) rot_standard=1;
                 break;
        case  8: if(!dir1) rot_standard=2;
                 if(dir1 ) rot_standard=0;
                 break;
        case  9: if(!dir1) rot_standard=1;
                 if(dir1 ) rot_standard=3;
                 break;
        case 10: if(!dir1) rot_standard=1;
                 if(dir1 ) rot_standard=3;
                 break;
        case 11: if(!dir1) rot_standard=0;
                 if(dir1 ) rot_standard=2;
                 break;
        case 12: if(!dir1) rot_standard=2;
                 if(dir1 ) rot_standard=0;
                 break;
        case 13: if(!dir1) rot_standard=3;
                 if(dir1 ) rot_standard=1;
                 break;
        case 14: if(!dir1) rot_standard=3;
                 if(dir1 ) rot_standard=1;
                 break;
        case 15: if(!dir1) rot_standard=0;
                 if(dir1 ) rot_standard=2;
                 break;
        case 16: if(!dir1) rot_standard=1;
                 if(dir1 ) rot_standard=3;
                 break;
        case 17: if(!dir1) rot_standard=2;
                 if(dir1 ) rot_standard=0;
                 break;
        case 18: if(!dir1) rot_standard=0;
                 if(dir1 ) rot_standard=2;
                 break;
        case 19: if(!dir1) rot_standard=1;
                 if(dir1 ) rot_standard=3;
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
     //-----------------------------------
     // Associo la ROTAZIONE STANDARD
     // abbinata al nodo
     //-----------------------------------
     rot1 = rot_standard;
  }
  //-------------------------------------------------
  // !!! IMPORTANTE !!!
  // Verifico con che rotazione il TGV deve
  // andare al primo punto previsto in missione
  //-------------------------------------------------
  memset(&nod, 0, sizeof(NOD));
  memset(&dati_nod, 0, sizeof(rec_linea));
  //-------------------------------------------------
  // Errore nodo inesistente
  //-------------------------------------------------
  if(N.cerca_record(&nod, nodo )) return 0;
  //----------------------------------------------------------
  //  14/02/2018B GB Valuta rotazione precedente 0-1-2-3
  //----------------------------------------------------------
  //trovato = cerca_path_into_nodo(AGV[num_agv]->mission.TypPath, 0, nodo, AGV[num_agv]->mission.pend, &nod, &dati_nod);
  RotFinale=99;
  NodoPrec=0;
  if(ptr_path==0) NodoPrec=nod.num;    // punto attuale
  if(ptr_path>=1) NodoPrec= AGV[num_agv]->mission.punto[ptr_path-1]; // punto precedente
  if(ptr_path>=1) {  // Ricaviamo i dati dalla missione in quanto tra un richiamo e non sono più corrispondenti al nodo precedente
      dati_nod.rot= AGV[num_agv]->mission.dati_perc[ptr_path-1].rot;
      dati_nod.ps = AGV[num_agv]->mission.dati_perc[ptr_path-1].ps;
      dati_nod.av = AGV[num_agv]->mission.dati_perc[ptr_path-1].dir;
      dati_nod.ind=!dati_nod.av;
  }
  CalcolaRotUscita(num_agv, ptr_path, NodoPrec, dati_nod, &RotFinale); // dati_nod deve contenere i dati del nodo precedente
  trovato = cerca_path_into_nodo(AGV[num_agv]->mission.TypPath, 0, nodo, AGV[num_agv]->mission.pend, NodoPrec, RotFinale, &nod, &dati_nod);
  //-------------------------------------------------
  // Errore percorso inesistente
  //-------------------------------------------------
  if(trovato<0) return 0;
  //---------------------------------------------------
  // Rotazione per punto successivo
  //---------------------------------------------------
  rot2 = dati_nod.rot;
  dir2 = dati_nod.ind;
  //---------------------------------------------------
  // Restituisco la rotazione a bit
  // (parametro che serve all'esterno)
  //---------------------------------------------------
  *rot_succ=rot2;
  //---------------------------------------------------
  // Verifico se la partenza è su un tratto non
  // ortogonale
  //---------------------------------------------------
  if(ptr_path==0){
     x1 = Punti[ nodo ][0];
     y1 = Punti[ nodo ][1];
     x2 = Punti[ succ ][0];
     y2 = Punti[ succ ][1];
     CatetoX = (double)(x2-x1);
     CatetoY = (double)(y2-y1);
     //------------------------------------
     // TGV su tratto inclinato
     // ROT>=50 && ROT<=70 => DEVIAZIONE
     //------------------------------------
     if(CatetoX>5 && CatetoY>5 && AGV[num_agv]->mission.dati_perc[ptr_path].rot>=50 && AGV[num_agv]->mission.dati_perc[ptr_path].rot<=70){
        radB = (long double)(atanl( CatetoY / CatetoX  ));
        grdB = RadToDeg(radB);
        grd2 = (short int)(grd2 - grdB);
     }
     //------------------------------------
     // GRADI DA ROTAZIONE IN MISSIONE
     //------------------------------------
     else{
        switch(rot2){
           case  0: grd2=  0; break;
           case  1: grd2= 90; break;
           case  2: grd2=180; break;
           case  3: grd2=270; break;
        }
     }
  }
  //---------------------------------------------------
  // !!! DA VERIFICARE !!!
  // Calcolo GRADI DA LASER (solo su primo nodo e
  // con tolleranze di +/- 5°)
  //---------------------------------------------------
  if(ptr_path==0 && grd1!=grd2 && abs(grd1-grd2)>OFFSET_MIN_COORD_TRATTO_INCLINATO){
     *antioraria=true;
     grd_rot_su_start = grd2-grd1;
   //if(grd_rot_su_start<0 || grd_rot_su_start>180){
     if((grd_rot_su_start<0 && grd_rot_su_start>=-180) || grd_rot_su_start>180){
        *antioraria=false;
        if(grd_rot_su_start<0) grd_rot_su_start=grd_rot_su_start*(-1);
        else{
           if(grd_rot_su_start>180) grd_rot_su_start=360-grd_rot_su_start;
        }
     }
     if(grd_rot_su_start<-180){
        grd_rot_su_start=360+grd_rot_su_start;
     }
  }
  //-------------------------------------------------
  // Se c'è differenza tra lo stato delle rotazioni
  // a bit verifico se la variazione è oraria o
  // antioraria
  //
  // NOTE:
  // 1. VALIDO SOLO PER CURVE DI BEZIER!!!!
  // 2. IN QUESTA VERSIONE IL CALCOLO VIENE FATTO
  //    DAI BIT e NON DAL LASER
  // 3. SE NN CI SONO CAMBI DIREZIONE
  //-------------------------------------------------
  if(ptr_path>0 && rot1!=rot2 && dir1==dir2 && CurveDiBezier!=0){
     //---------------------------------------------------
     // Gradi dai BIT
     //---------------------------------------------------
     if(rot1==1 && rot2==0){*antioraria=false; grd_rot_su_start=90;}
     if(rot1==2 && rot2==0){*antioraria=true;  grd_rot_su_start=180;}  //180° scegliamo le rotazioni anti-orarie
     if(rot1==3 && rot2==0){*antioraria=true;  grd_rot_su_start=90;}
     if(rot1==0 && rot2==1){*antioraria=true;  grd_rot_su_start=90;}
     if(rot1==2 && rot2==1){*antioraria=false; grd_rot_su_start=90;}
     if(rot1==3 && rot2==1){*antioraria=true;  grd_rot_su_start=180;}  //180° scegliamo le rotazioni anti-orarie
     if(rot1==0 && rot2==2){*antioraria=true;  grd_rot_su_start=180;}  //180° scegliamo le rotazioni anti-orarie
     if(rot1==1 && rot2==2){*antioraria=true;  grd_rot_su_start=90;}
     if(rot1==3 && rot2==2){*antioraria=false; grd_rot_su_start=90;}
     if(rot1==0 && rot2==3){*antioraria=false; grd_rot_su_start=90;}
     if(rot1==1 && rot2==3){*antioraria=true;  grd_rot_su_start=180;}  //180° scegliamo le rotazioni anti-orarie
     if(rot1==2 && rot2==3){*antioraria=true;  grd_rot_su_start=90;}
  }
  return grd_rot_su_start;
}

// -----------------------------------------------------
//      CalcolaRotUscita()
// -----------------------------------------------------
// Calcola la rotazione uscita (quando riesce) per permettere la scelta automatica
// del percorso in base alla rotazione di uscita
//
// Viene fatto il controllo sulla uscita della curva :
// Lavora solo con Curve di Bezier 90°
//
// N.B. Purtroppo questa fuzione è richiamata in distanza()  come cerca_path_into_nodo()
//      perciò deve essere abbastanza snella per evitare rallentamenti
// N.B. NON possiamo inserire calcoli troppo sofisticati a meno di cambiare il modo
//      di richiamare anche cerca_path_into_nodo()
//
//
int nodo::CalcolaRotUscita(short int num_agv, short int IndicePath, int NodoInizio, struct rec_linea dati_path, short int *RotFinaleArrivo)
/*********************************************************/
{
  short int attu;
  short int succ;
  short int rot_attu;
  //short int rot_succ;
  short int dir_attu;
  int  x1, y1, x2, y2;
  bool NoGestioneCurve;
  int  typ_curva;
  TPoint points[5];

  *RotFinaleArrivo=99;
  //----------------------------------------
  // Controllo Num AGV
  //----------------------------------------
  if(num_agv<1 || num_agv>MAXAGV) return 0;
  //--------------------------------------------------
  // (1) START Missione valutiamo solo posizione AGV
  //--------------------------------------------------
  if(IndicePath==0){
     if(AGV[num_agv]->stato.pos==NodoInizio){
        //-----------------------------------------
        // Recupero rotazione attuale da stato TGV
        //-----------------------------------------
        if(AGV[num_agv]->stato.s.bit.grd0==true  ) *RotFinaleArrivo=0;
        if(AGV[num_agv]->stato.s.bit.grd90==true ) *RotFinaleArrivo=1;
        if(AGV[num_agv]->stato.s.bit.grd180==true) *RotFinaleArrivo=2;
        if(AGV[num_agv]->stato.s.bit.grd270==true) *RotFinaleArrivo=3;
     }
  }
  //--------------------------------------------------
  // STEP valuta CURVE e ROTAZIONI
  //--------------------------------------------------
  if(IndicePath>0 && dati_path.ps>0){
     attu=0;
     succ=0;
     rot_attu=0;
     //rot_succ=0;
     dir_attu=0;
     NoGestioneCurve=false;
     //--------------------------------------------------
     // Individuo il tipo di curva
     //--------------------------------------------------
     attu = NodoInizio;
     succ = dati_path.ps;
     rot_attu = dati_path.rot;  // Rotazione ingresso curva
     dir_attu = dati_path.av;   // 0= Indietro 1=Avanti
     //--------------------------------------------------
     // Variabil.h -> Disattivazione curve
     //--------------------------------------------------
     #ifndef GESTIONE_CURVE
        NoGestioneCurve=true;
     #endif;
     //--------------------------------------------------
     // Coordinate dei punti
     //--------------------------------------------------
     x1 = Punti[attu][0];
     y1 = Punti[attu][1];
     x2 = Punti[succ][0];
     y2 = Punti[succ][1];
     //--------------------------------------------------
     // CURVE DI BEZIER
     // Selezione tipo di curva in base a impostazione
     // variabil.h e stato di rotazione di uscita
     // N.B. Valutiamo solo curve BEZIER (NON le ALTRE)
     // CurveDiBezier = 1 --> TUTTE
     // CurveDiBezier = 2 --> SOLO QUADRATICHE
     // CurveDiBezier = 3 --> SOLO CUBICHE
     //--------------------------------------------------
     if(CurveDiBezier>0){
        typ_curva = CurveDiBezier;   // la funzione ricerca tra tutti i tipi di curve
        if(N.CercaCurvaBezier(FALSE, attu, succ, &typ_curva, &points[0])) NoGestioneCurve=true;
     }else{
        NoGestioneCurve=true;
     }
     //--------------------------------------------------
     // Se non c'è variazione di entrambe le coordinate
     // dei punti --> Skip!!!
     //--------------------------------------------------
     if(x1==x2 || y1==y2) NoGestioneCurve=true;
     //--------------------------------------------------
     // (2) Trovata CURVA valutiamo Ritazione finale
     //--------------------------------------------------
     if(NoGestioneCurve==false){
        //--------------------------------------------------
        // 1) Start 0=0° Discesa Indietro (Curva di 90°)
        //--------------------------------------------------
        if(rot_attu==0 && dir_attu==0){           // Start Rot   0°  Dir Indietro
           if(x1>x2 && y1<y2) *RotFinaleArrivo=3; // SX
           if(x1<x2 && y1<y2) *RotFinaleArrivo=1; // DX
        }
        //--------------------------------------------------
        // 2) Start 2=180° Discesa Avanti (Curva di 90°)
        //--------------------------------------------------
        if(rot_attu==2 && dir_attu==1){           // Start Rot  180°  Dir Avanti
           if(x1<x2 && y1<y2) *RotFinaleArrivo=1; // SX
           if(x1<x2 && y1<y2) *RotFinaleArrivo=3; // DX
        }
        //--------------------------------------------------
        // 3) Start 0=0° Salita  Avanti (Curva di 90°)
        //--------------------------------------------------
        if(rot_attu==0 && dir_attu==1){           // Start Rot   0°  Dir Avanti
           if(x1>x2 && y1>y2) *RotFinaleArrivo=1; // SX
           if(x1<x2 && y1>y2) *RotFinaleArrivo=3; // DX
        }
        //--------------------------------------------------
        // 4) Start 2=180° Salita Indietro (Curva di 90°)
        //--------------------------------------------------
        if(rot_attu==2 && dir_attu==0){           // Start Rot  180°  Dir Indietro
           if(x1>x2 && y1>y2) *RotFinaleArrivo=3; // SX
           if(x1<x2 && y1>y2) *RotFinaleArrivo=1; // DX
        }
        //--------------------------------------------------
        // 5) Start 1=90° Destra Indietro (Curva di 90°)
        //--------------------------------------------------
        if(rot_attu==1 && dir_attu==0){           // Start Rot   90°  Dir Indietro
           if(x1<x2 && y1>y2) *RotFinaleArrivo=2; // Salita
           if(x1<x2 && y1<y2) *RotFinaleArrivo=0; // Discesa
        }
        //--------------------------------------------------
        // 6) Start 3=270° Destra Avanti (Curva di 90°)
        //--------------------------------------------------
        if(rot_attu==3 && dir_attu==1){           // Start Rot  270°  Dir Avanti
           if(x1<x2 && y1>y2) *RotFinaleArrivo=0; // Salita
           if(x1<x2 && y1<y2) *RotFinaleArrivo=2; // Discesa
        }
        //--------------------------------------------------
        // 7) Start 1=90° Sinistra Avanti (Curva di 90°)
        //--------------------------------------------------
        if(rot_attu==1 && dir_attu==1){           // Start Rot   90°  Dir Avanti
           if(x1>x2 && y1>y2) *RotFinaleArrivo=0; // Salita
           if(x1>x2 && y1<y2) *RotFinaleArrivo=2; // Discesa
        }
        //--------------------------------------------------
        // 8) Start 3=270° Sinistra Indietro (Curva di 90°)
        //--------------------------------------------------
        if(rot_attu==3 && dir_attu==0){           // Start Rot  270°  Dir Indietro
           if(x1>x2 && y1>y2) *RotFinaleArrivo=2; // Salita
           if(x1>x2 && y1<y2) *RotFinaleArrivo=0; // Discesa
        }
     }
     //------------------------------------------------------------------------------------
     // (3) Se NON è una curva la ROTAZIONE uscita segmento è quella impostata nei percorsi
     //------------------------------------------------------------------------------------
     if(NoGestioneCurve==true){
        *RotFinaleArrivo=dati_path.rot;
     }
  }
  return 0;
}


//==============================================================================
//                     GESTIONE PERCORSI DA AGV.CPP
//==============================================================================

// ---------------------------------------------------
//      cerca_path_into_nodo1()
// ---------------------------------------------------
// Cerca nella struttura NODO il percorso (linea) relativo a una destinazione
// data (target). La funzione restituisce in "dati" la linea trovata e il
// numero di linea come ritorno. Se non esiste, invece, restituisce -1.
//
int nodo::cerca_path_into_nodo1(short int tipo_percorso, int percorso_speciale, short int source, short int target, short int NodoPrec, short int RotFinaleArrivo, struct NOD *nod, struct rec_linea *dati)
/***************************************************************/
{
  int i, trovato;
  int NodoMasterTarget=0;
  int AreaSpecialePerPercorsoAlternativo;
  short int NodoPrecEdit;

  i=0;
  trovato = -1;
  setmem(dati, sizeof(rec_linea), 0);           /* azzera la struttura */
  //===========================================================
  // GESTIONE DEI PERCORSI VELOCI
  //
  // 14/02/2018 GB Gestione Percorso in base a Provenienza
  //            NodoPrecEdit<=   0 --> Gestione standard
  //            NodoPrecEdit>=   1 --> Gestione che valuta se coincide con nodo precedente
  //            NodoPrecEdit==1999 --> Valore JOLLY che valuta la ROTAZIONE con cui arriva
  //                                   il TGV valutando se coincide con il prossimo movimento
  //===========================================================
  if(AbilitaGestionePercorsiVeloci==true && tipo_percorso==FAST_PATH){
     //-----------------------------------------------------------
     // (1A) Solo punti terminali (VELOCE)
     //-----------------------------------------------------------
     for(i=0; i<MAXPUNTINODO; i++){
        if(nod->rec[i].zp!=PERC_ALTERNATIVO_VELOCE) continue;
        if(target == nod->rec[i].pt){
           //-----------------------------------------------------------------------------
           // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
           //-----------------------------------------------------------------------------
           NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
           if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
              if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
           }else{                                          // Controllo nodo precedente
              if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
              if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
           }
           //--------------------------------------------------------------
           trovato=i+PERC_ALTERNATIVO_VELOCE;
           memcpy(dati, &nod->rec[i], sizeof(rec_linea));
           return trovato;
        }
     };
     //---------------------------------------------------------------
     // (1A_Master) Solo punti terminali (VELOCE)
     //             Gestione del NODO MASTER di ingresso alle Celle
     //             con più NODI SLAVE
     //---------------------------------------------------------------
     NodoMasterTarget = N.n[target]->NodoMaster;
     for(i=0; i<MAXPUNTINODO ; i++){
        if(nod->rec[i].zp!=PERC_ALTERNATIVO_VELOCE) continue;
        if( NodoMasterTarget>0 ){
           if( NodoMasterTarget == nod->rec[i].pt){
              //-----------------------------------------------------------------------------
              // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
              //-----------------------------------------------------------------------------
              NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
              if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
                 if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
              }else{                                          // Controllo nodo precedente
                 if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
                 if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
              }
              //--------------------------------------------------------------
              trovato=i+PERC_ALTERNATIVO_VELOCE;
              memcpy(dati, &nod->rec[i], sizeof(rec_linea));
              return trovato;
           }
        }
     }

     //-----------------------------------------------------------
     // (1B) Solo punti terminali (STANDARD)
     //-----------------------------------------------------------
     for(i=0; i<MAXPUNTINODO; i++){
        if(nod->rec[i].zp==PERC_ALTERNATIVO_VELOCE) continue;
        if(target == nod->rec[i].pt){
           //-----------------------------------------------------------------------------
           // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
           //-----------------------------------------------------------------------------
           NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
           if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
              if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
           }else{                                          // Controllo nodo precedente
              if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
              if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
           }
           //--------------------------------------------------------------
           trovato=i;
           memcpy(dati, &nod->rec[i], sizeof(rec_linea));
           return trovato;
        }
     };

     //-----------------------------------------------------------
     // (1B_Master) Solo punti terminali (STANDARD)
     //             Gestione del NODO MASTER di ingresso alle Celle
     //             con più NODI SLAVE
     //-----------------------------------------------------------
     NodoMasterTarget = N.n[target]->NodoMaster;
     for(i=0; i<MAXPUNTINODO; i++){
        if(nod->rec[i].zp==PERC_ALTERNATIVO_VELOCE) continue;
        if( NodoMasterTarget>0 ){
           if( NodoMasterTarget == nod->rec[i].pt){
              //-----------------------------------------------------------------------------
              // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
              //-----------------------------------------------------------------------------
              NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
              if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
                 if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
              }else{                                          // Controllo nodo precedente
                 if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
                 if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
              }
              //--------------------------------------------------------------
              trovato=i;
              memcpy(dati, &nod->rec[i], sizeof(rec_linea));
              return trovato;
           }
        }
     };

     //-----------------------------------------------------------
     // (2A) PECORSI SPECIALI (VELOCE)
     //      Controlla se c'è in corso un "BLOCCO PERCORSO"
     //      e se la destinazione fa parte di una zona speciale
     //-----------------------------------------------------------
     if( percorso_speciale>0 ){
        //------------------------------------------------------------------------------------
        // Verifica se
        // a) E' attivo il "BloccoPercorsi" 1..3
        // b) Se nella riga c'è come ZP l'area speciale (51 per blocco1 52 per blocco2...)
        //    di cui il Nodo target fa parte
        //------------------------------------------------------------------------------------
        for( i=0; i<MAXPUNTINODO; i++ ){
           if(nod->rec[i].pt!=PERC_ALTERNATIVO_VELOCE) continue;
           AreaSpecialePerPercorsoAlternativo=0;
           if( nod->rec[i].zp>100){
              AreaSpecialePerPercorsoAlternativo = SettaZonaSpeciale( percorso_speciale, (int)(nod->rec[i].zp-(100*percorso_speciale)), target);
              if( nod->rec[i].zp==AreaSpecialePerPercorsoAlternativo ){
                 //-----------------------------------------------------------------------------
                 // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
                 //-----------------------------------------------------------------------------
                 NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
                 if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
                    if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
                 }else{                                          // Controllo nodo precedente
                    if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
                    if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
                 }
                 //--------------------------------------------------------------
                 trovato=i+PERC_ALTERNATIVO_VELOCE;
                 memcpy(dati, &nod->rec[i], sizeof(rec_linea));
                 return trovato;
              }
           }
        }
     }
     //-----------------------------------------------------------
     // (2B) PECORSI SPECIALI (STANDARD)
     //      Controlla se c'è in corso un "BLOCCO PERCORSO"
     //      e se la destinazione fa parte di una zona speciale
     //-----------------------------------------------------------
     if( percorso_speciale>0 ){
        //------------------------------------------------------------------------------------
        // Verifica se
        // a) E' attivo il "BloccoPercorsi" 1..3
        // b) Se nella riga c'è come ZP l'area speciale (51 per blocco1 52 per blocco2...)
        //    di cui il Nodo target fa parte
        //------------------------------------------------------------------------------------
        for( i=0; i<MAXPUNTINODO; i++ ){
           AreaSpecialePerPercorsoAlternativo=0;
           if( nod->rec[i].pt==PERC_ALTERNATIVO_VELOCE ) continue;
           if( nod->rec[i].zp>100){
              AreaSpecialePerPercorsoAlternativo = SettaZonaSpeciale( percorso_speciale, (int)(nod->rec[i].zp-(100*percorso_speciale)), target);
              if( nod->rec[i].zp==AreaSpecialePerPercorsoAlternativo ){
                 //-----------------------------------------------------------------------------
                 // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
                 //-----------------------------------------------------------------------------
                 NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
                 if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
                    if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
                 }else{                                          // Controllo nodo precedente
                    if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
                    if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
                 }
                 //--------------------------------------------------------------
                 trovato=i;
                 memcpy(dati, &nod->rec[i], sizeof(rec_linea));
                 return trovato;
              }
           }
        }
     }
     //-----------------------------------------------------------
     // (3A) Se target non è punto terminale, allora cerca
     //      indicazioni su ZP e ZS (VELOCE)
     //-----------------------------------------------------------
     for(i=0; i<MAXPUNTINODO ; i++){
        if(nod->rec[i].pt!=PERC_ALTERNATIVO_VELOCE) continue;
        if(((N.n[target]->zs == nod->rec[i].zs) && (N.n[target]->zs !=0)) &&
           ((N.n[target]->zp == nod->rec[i].zp) && (N.n[target]->zp !=0))){
           //-----------------------------------------------------------------------------
           // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
           //-----------------------------------------------------------------------------
           NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
           if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
              if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
           }else{                                          // Controllo nodo precedente
              if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
              if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
           }
           //--------------------------------------------------------------
           trovato=i+PERC_ALTERNATIVO_VELOCE;
           memcpy(dati, &nod->rec[i], sizeof(rec_linea));
           return trovato;
        }
     }
     //-----------------------------------------------------------
     // (3B) Se target non è punto terminale, allora cerca
     //      indicazioni su ZP e ZS (STANDARD)
     //-----------------------------------------------------------
     for(i=0; i<MAXPUNTINODO ; i++){
        if(nod->rec[i].pt==PERC_ALTERNATIVO_VELOCE) continue;
        if(((N.n[target]->zs == nod->rec[i].zs) && (N.n[target]->zs !=0)) &&
           ((N.n[target]->zp == nod->rec[i].zp) && (N.n[target]->zp !=0))){
           //-----------------------------------------------------------------------------
           // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
           //-----------------------------------------------------------------------------
           NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
           if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
              if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
           }else{                                          // Controllo nodo precedente
              if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
              if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
           }
           //--------------------------------------------------------------
           trovato=i;
           memcpy(dati, &nod->rec[i], sizeof(rec_linea));
           return trovato;
        }
     }
     //-------------------------------------------------------
     // (4A) Cerca allora indicazioni solo per Zona Primaria
     //      (VELOCE)
     //-------------------------------------------------------
     for(i=0; i<MAXPUNTINODO ; i++){
        if(nod->rec[i].pt!=PERC_ALTERNATIVO_VELOCE) continue;
        if(nod->rec[i].zs!=0                      ) continue;
        if((N.n[target]->zp == nod->rec[i].zp) && (N.n[target]->zp !=0)){
           //-----------------------------------------------------------------------------
           // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
           //-----------------------------------------------------------------------------
           NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
           if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
              if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
           }else{                                          // Controllo nodo precedente
              if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
              if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
           }
           //--------------------------------------------------------------
           trovato=i+PERC_ALTERNATIVO_VELOCE;
           memcpy(dati, &nod->rec[i], sizeof(rec_linea));
           return trovato;
        }
     };
     //-------------------------------------------------------
     // (4B) Cerca allora indicazioni solo per Zona Primaria
     //      (VELOCE)
     //-------------------------------------------------------
     for(i=0; i<MAXPUNTINODO ; i++){
        if(nod->rec[i].pt==PERC_ALTERNATIVO_VELOCE) continue;
        if(nod->rec[i].zs!=0                      ) continue;
        if((N.n[target]->zp == nod->rec[i].zp) && (N.n[target]->zp !=0)){
           //-----------------------------------------------------------------------------
           // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
           //-----------------------------------------------------------------------------
           NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
           if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
              if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
           }else{                                          // Controllo nodo precedente
              if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
              if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
           }
           //--------------------------------------------------------------
           trovato=i;
           memcpy(dati, &nod->rec[i], sizeof(rec_linea));
           return trovato;
        }
     };
     //-------------------------------------------------------
     //
     // (5) (VELOCE/STANDARD)
     //     Direzione preferenziale: se MANCA ZP, ZS, PT ma
     //     c'è un PS questo diventa il percorso di DEFAULT
     //
     //-------------------------------------------------------
     for(i=0; i<MAXPUNTINODO ; i++){
        if( nod->rec[i].zp!=0 ) continue;
        if( nod->rec[i].zs!=0 ) continue;
        if( nod->rec[i].pt!=0 ) continue;
        if( nod->rec[i].ps==0 ) break;
        //-----------------------------------------------------------------------------
        // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
        //-----------------------------------------------------------------------------
        NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
        if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
           if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
        }else{                                          // Controllo nodo precedente
           if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
           if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
        }
        //--------------------------------------------------------------
        trovato=i;
        memcpy(dati, &nod->rec[i], sizeof(rec_linea));
        return trovato;
     }
  }
  //===========================================================
  // GESTIONE DEI PERCORSI STANDARD
  //
  // 14/02/2018 GB Gestione Percorso in base a Provenienza
  //            NodoPrecEdit<=   0 --> Gestione standard
  //            NodoPrecEdit>=   1 --> Gestione che valuta se coincide con nodo precedente
  //            NodoPrecEdit==1999 --> Valore JOLLY che valuta la ROTAZIONE con cui arriva
  //                                   il TGV valutando se coincide con il prossimo movimento
  //===========================================================
  else{
     //-----------------------------------------------------------
     // (1) Solo punti terminali
     //-----------------------------------------------------------
     for(i=0; i<MAXPUNTINODO; i++){
        if(nod->rec[i].zp==PERC_ALTERNATIVO_VELOCE) continue;
        if(target == nod->rec[i].pt){
           //-----------------------------------------------------------------------------
           // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
           //-----------------------------------------------------------------------------
           NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
           if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
              if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
           }else{                                          // Controllo nodo precedente
              if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
              if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
           }
           //--------------------------------------------------------------
           trovato=i;
           memcpy(dati, &nod->rec[i], sizeof(rec_linea));
           return trovato;
        }
     };
     //-----------------------------------------------------------
     // (1_Master) Solo punti terminali
     //            Gestione del NODO MASTER di ingresso alle Celle
     //            con più NODI SLAVE
     //-----------------------------------------------------------
     NodoMasterTarget = N.n[target]->NodoMaster;
     for(i=0; i<MAXPUNTINODO; i++){
        if(nod->rec[i].zp==PERC_ALTERNATIVO_VELOCE) continue;
        if( NodoMasterTarget>0 ){
           if( NodoMasterTarget == nod->rec[i].pt){
              //-----------------------------------------------------------------------------
              // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
              //-----------------------------------------------------------------------------
              NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
              if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
                 if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
              }else{                                          // Controllo nodo precedente
                 if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
                 if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
              }
              //--------------------------------------------------------------
              trovato=i;
              memcpy(dati, &nod->rec[i], sizeof(rec_linea));
              return trovato;
           }
        }
     };
     //-----------------------------------------------------------
     // (2) 08/06/2011
     //      Controlla se c'è in corso un "BLOCCO PERCORSO"
     //      e se la destinazione fa parte di una zona speciale
     //-----------------------------------------------------------
     if( percorso_speciale>0 ){
        //------------------------------------------------------------------------------------
        // Verifica se
        // a) E' attivo il "BloccoPercorsi" 1..3
        // b) Se nella riga c'è come ZP l'area speciale (51 per blocco1 52 per blocco2...)
        //    di cui il Nodo target fa parte
        //------------------------------------------------------------------------------------
        for( i=0; i<MAXPUNTINODO; i++ ){
           AreaSpecialePerPercorsoAlternativo=0;
           if( nod->rec[i].pt==PERC_ALTERNATIVO_VELOCE ) continue;
           if( nod->rec[i].zp>100){
              //-----------------------------------------------------------------------------
              // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
              //-----------------------------------------------------------------------------
              NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
              if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
                 if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
              }else{                                          // Controllo nodo precedente
                 if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
                 if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
              }
              //--------------------------------------------------------------
              AreaSpecialePerPercorsoAlternativo = SettaZonaSpeciale( percorso_speciale, (int)(nod->rec[i].zp-(100*percorso_speciale)), target);
              if( nod->rec[i].zp==AreaSpecialePerPercorsoAlternativo ){
                 trovato=i;
                 memcpy(dati, &nod->rec[i], sizeof(rec_linea));
                 return trovato;
              }
           }
        }
     }
     //-----------------------------------------------------------
     // (3) Se target non è punto terminale, allora cerca
     //     indicazioni su Zona Primaria e Zona Secondaria
     //-----------------------------------------------------------
     for(i=0; i<MAXPUNTINODO ; i++){
        if(nod->rec[i].pt==PERC_ALTERNATIVO_VELOCE) continue;
        if(((N.n[target]->zs == nod->rec[i].zs) && (N.n[target]->zs !=0)) &&
           ((N.n[target]->zp == nod->rec[i].zp) && (N.n[target]->zp !=0))){
           //-----------------------------------------------------------------------------
           // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
           //-----------------------------------------------------------------------------
           NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
           if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
              if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
           }else{                                          // Controllo nodo precedente
              if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
              if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
           }
           //--------------------------------------------------------------
           trovato=i;
           memcpy(dati, &nod->rec[i], sizeof(rec_linea));
           return trovato;
        }
     }
     //-------------------------------------------------------
     // (4) Cerca allora indicazioni solo per Zona Primaria
     //-------------------------------------------------------
     for(i=0; i<MAXPUNTINODO ; i++){
        if(nod->rec[i].pt==PERC_ALTERNATIVO_VELOCE) continue;
        if(nod->rec[i].zs!=0                      ) continue;
        if((N.n[target]->zp == nod->rec[i].zp) && (N.n[target]->zp !=0)){
           //-----------------------------------------------------------------------------
           // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
           //-----------------------------------------------------------------------------
           NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
           if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
              if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
           }else{                                          // Controllo nodo precedente
              if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
              if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
           }
           //--------------------------------------------------------------
           trovato=i;
           memcpy(dati, &nod->rec[i], sizeof(rec_linea));
           return trovato;
        }
     };
     //-------------------------------------------------------
     //
     // (5) 18/06/2008
     //     Direzione preferenziale: se MANCA ZP, ZS, PT ma
     //     c'è un PS questo diventa il percorso di DEFAULT
     //
     //-------------------------------------------------------
     for(i=0; i<MAXPUNTINODO ; i++){
        if( nod->rec[i].zp!=0 ) continue;
        if( nod->rec[i].zs!=0 ) continue;
        if( nod->rec[i].pt!=0 ) continue;
        if( nod->rec[i].ps==0 ) break;
        //-----------------------------------------------------------------------------
        // 14/02/2018 GB Se NodoPrec==0 le righe dove il campo è compilato -> IGNORATE
        //-----------------------------------------------------------------------------
        NodoPrecEdit=(nod->rec[i].NodPrecL + (nod->rec[i].NodPrecH<<8));
        if(NodoPrec>=1 && NodoPrecEdit==PT_JOLLY_PERC_ALTERN_PER_ROT){ // Controllo automatico Rotazione
           if(RotFinaleArrivo!=nod->rec[i].rot                          ) continue;
        }else{                                          // Controllo nodo precedente
           if(NodoPrec <1 &&  NodoPrecEdit>0                            ) continue;
           if(NodoPrec>=1 && (NodoPrecEdit<=1 || NodoPrecEdit!=NodoPrec)) continue;
        }
        //--------------------------------------------------------------
        trovato=i;
        memcpy(dati, &nod->rec[i], sizeof(rec_linea));
        return trovato;
     }
  }
  return trovato;

}



// ---------------------------------------------------
//      cerca_path_into_nodo()
// ---------------------------------------------------
// Cerca nella struttura NODO il percorso (linea) relativo a una destinazione
// data (target). La funzione restituisce in "dati" la linea trovata e il
// numero di linea come ritorno. Se non esiste, invece, restituisce -1.
//
int nodo::cerca_path_into_nodo(short int tipo_percorso, int percorso_speciale, short int source, short int target, short int NodoPrec, short int RotFinaleArrivo, struct NOD *nod, struct rec_linea *dati)
/***************************************************************/
{
  int trovato=0;

  //----------------------------------------------------------------------------------------
  // (1) Verifica se ci sono percorsi previsti per un Nodo Precedente predefinito
  //     NodoPrecEdit = Verifica se il valore impostato coincide con il nodo di provenienza
  //     NodoPrecEdit = 1999 valore JOLLY valuta invece la rotazione finale di arrivo:
  //                    0,1,2,3 --> (Rotazione AGV 0,90,180,270) verifica la rotazione di
  //                    arrivo se coincidente con quella impostatata per Punto sucessivo
  //----------------------------------------------------------------------------------------
  #ifdef ABILITA_PERC_ALTERN_PER_PRECEDENTE
     trovato=cerca_path_into_nodo1(tipo_percorso, percorso_speciale, source, target, NodoPrec, RotFinaleArrivo, nod, dati);
  #endif
  //-------------------------------------------------------------------------
  // (2) Verifica percorsi standard
  //-------------------------------------------------------------------------
  if(trovato<=0){
     NodoPrec=0;
     trovato=cerca_path_into_nodo1(tipo_percorso, percorso_speciale, source, target, NodoPrec, RotFinaleArrivo, nod, dati);
  }
  return trovato;
}


// ---------------------------------------------------
//      cerca_path()
// ---------------------------------------------------
// - Se il flag "percorso_alternativo" Š BASSO Costruisce un percorso
//   da SOURCE a DEST.
// - Se il flag "percorso_alternativo" Š ALTO compila una struttura
//   ausiliaria con quello che potrebbe essere un possibile percorso
//   da SOURCE a DEST.
//
int nodo::cerca_path(short int num_agv, short int tipo_percorso, short int sorgente, short int destinazione, short int *nodi_perc, struct DATI_PERCORSO *dati_perc, char *all_mess)
/********************************************************/
{
  int stat=0, trovato=0;
  int  x1, x2, y1, y2, offx, offy;
  bool avanzamento_in_rettaX, avanzamento_in_rettaY;
  short int count_rot, prec=0, i, j, source, dest;
  short int old_rotazione=0, old_av=0, old_ind=0, old_ral=0, NodoPrec=0, RotFinale=0;
  struct NOD nod;
  struct rec_linea dati_path;
  bool   GestioneCurve;

  i=0;
  count_rot=0;
  source = sorgente;
  dest   = destinazione;
  memset(&nod, 0, sizeof(NOD));
  memset(&dati_path, 0, sizeof(rec_linea));
  //----------------------------------------------
  // Attiva/Disattiva gestione controlli laser
  //----------------------------------------------
  GestioneCurve=false;
  #ifdef GESTIONE_CURVE
     GestioneCurve=true;
  #endif;
  //----------------------------------------------
  // Ciclo compilazione percorso da Source a Dest
  //----------------------------------------------
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
        sprintf(all_mess, "Error Cerca_Path(): ( nodo=%d, dest.=%d, prec.=%d )" , source, dest, prec);  // 19/06/2008
        return 1;
     }
     //------------------------------------------------------------------
     //  14/02/2018B GB Compilazione di una rotazione precedente  0-1-2-3
     //  Simile a old_rotazione ma valuta anche la posizione TGV
     //------------------------------------------------------------------
     //trovato = cerca_path_into_nodo(tipo_percorso, 0, sorgente, dest, &nod, &dati_path);
     RotFinale=99;
     if(i==0)      NodoPrec=nod.num;    // punto attuale
     if(i>=1)      NodoPrec=prec;       // punto precedente
     CalcolaRotUscita(num_agv, i, NodoPrec, dati_path, &RotFinale);
     trovato = cerca_path_into_nodo(tipo_percorso, 0, sorgente, dest, NodoPrec, RotFinale, &nod, &dati_path);
     //----------------------------------------------
     // RICERCA PERCORSO STANDARD
     // Verifico se il percorso trovato
     //----------------------------------------------
     if(tipo_percorso==STD_PATH && (source != dest) && (trovato<0 || trovato>PERC_ALTERNATIVO_VELOCE)){
        sprintf(all_mess, "Ricerca interr.in %d manca percorso (dest.=%d, prec.=%d)", source, dest, prec);
        return 2;
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
     // RICERCA PERCORSO VELOCE
     // Verifico se il percorso trovato
     //----------------------------------------------
     if(tipo_percorso==FAST_PATH && (source != dest) && trovato<0){
        sprintf(all_mess, "Ricerca interr.in %d manca percorso (dest.=%d, prec.=%d)", source, dest, prec);
        return 3;
     }
     //----------------------------------------------
     //  Errore indicazioni target non presenti
     //  nŠ come PS, ZP o ZS
     //----------------------------------------------
  // if((trovato <0) && (source != dest)){
  //    sprintf(all_mess, MESS[117], agv_num, source , target, prec );
  //    return 4;
  // };
     //----------------------------------------------
     // Errore punto successivo = 0
     //----------------------------------------------
     if((dati_path.ps == 0) && (source != dest)){
        sprintf(all_mess, "Ricerca interr.in %d con prox.%d (dest.=%d, prec.=%d)", source, dati_path.ps, dest, prec );
        return 5;
     };
     //--------------------------------------------------------
     // Errore indicazioni punto sucessivo == posizione attuale
     //--------------------------------------------------------
     if((dati_path.av == dati_path.ind) && (source != dest)){
        sprintf(all_mess, "Errore in %d, dest=%d, prec=%d, indicazione direz. av/ind.", source, dest, prec);
        return 6;
     };
     //-----------------------------------------------------
     // Errore direzione opposta a punto precedente 10/07/95
     //-----------------------------------------------------
     if(GestioneCurve==false){
        if((dati_path.rot == old_rotazione) &&
           (dati_path.av  != old_av ) &&
           (dati_path.ind != old_ind) &&
           (i>0) &&
//         (source != dest)){
           (source != dest) && dati_path.ps==prec){  // necessario per salto nodi ma causa mancata
                                                     // segnalazione problemi rotazione
              sprintf(all_mess, "Errore in nodo %d dest=%d direz. opposta a nodo prec.%d", source, dest, prec);
              return 7;
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
              sprintf(all_mess, "Errore in nodo %d dest= %d direz. opposta a nodo prec.%d", source, dest, prec);
              return 8;
        }
     }
     //-----------------------------------------------------
     // Errore Rallentare prima di cambio rotazione 11/07/95
     //-----------------------------------------------------
     if((dati_path.rot != old_rotazione) && (old_ral == 0) && (i>0)){
        sprintf(all_mess, "Errore in nodo %d dest=%d cambio rotaz.: rall.nodo prec.%d", source, dest, prec);
        return 9;
     }
     //-----------------------------------------------------
     // Errore distanza inferiore a 30cm
     //-----------------------------------------------------
     if(dati_path.dist<DIST_MIN_ING_CURVA && (source != dest)){
        sprintf(all_mess, "distanza %dcm tra nodo %d e succ %d inferiore a %dcm...", dati_path.dist, source, dati_path.ps, DIST_MIN_ING_CURVA);
        return 10;
     }
     //-----------------------------------------------------
     // Se un tratto risulta in retta verifica se le
     // rotazioni sono coerenti:
     // Retta X ==> 90° o 270°.
     // Retta Y ==>  0° o 180°.
     //-----------------------------------------------------
     if((avanzamento_in_rettaX==true && dati_path.rot!=1 && dati_path.rot!=3) ||
        (avanzamento_in_rettaY==true && dati_path.rot!=0 && dati_path.rot!=2)){
        if(avanzamento_in_rettaX==true) sprintf(all_mess, "errore in nodo %d succ %d (dest=%d) retta X con rot %d", nod.num, dati_path.ps, dest, dati_path.rot);
        if(avanzamento_in_rettaY==true) sprintf(all_mess, "errore in nodo %d succ %d (dest=%d) retta Y con rot %d", nod.num, dati_path.ps, dest, dati_path.rot);
        return 11;
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
           sprintf(all_mess, "nodo %d con prox.%d duplicato (dest.=%d, prec.=%d)", source, dati_path.ps, dest, prec );
           return 12;
        }
     }
     //----------------------------------------------
     // Errore percorso troppo lungo
     //----------------------------------------------
     if(i>MAXPERCORSI){
        sprintf(all_mess, "distanza() interrotta al %d percorso lungo: %d (dest=%d)", source, i, dest);
        return 13;
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
     dati_perc[i].ps           = dati_path.ps;
     dati_perc[i].dir          = dati_path.ind;
     dati_perc[i].rot          = dati_path.rot;
     dati_perc[i].ral          = dati_path.ral;
     dati_perc[i].dist         = dati_path.dist;
     dati_perc[i].salta_a_ps   = 0;  // salta a nodo per distanza bassa
     dati_perc[i].salta_a_dist = 0;  // salta a distanza
     i++;                // incrementa conteggio Nodi
  }while(source != dest);
  return 0;
}

//==============================================================================
//                  GESTIONE PRECEDENZE PER TGV IN RETTILINEO
//==============================================================================


// --------------------------------------------
//    load_nodi_tgv_lanciati_su_rettilineo()
// --------------------------------------------
//   load struttura nodi riservati per TGV lanciato in rettilineo
//
int nodo::load_nodi_tgv_lanciati_su_rettilineo(char *all_mess)
/***********************************/
{
  int  i,
       err =0;
  int  handle;

  memset(&nodo_con_tgv_lanciato, 0, sizeof(nodo_con_tgv_lanciato));
  //-------------------------------------
  // Apertura file
  //-------------------------------------
  handle = sopen( "NODORETT.DAT", O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
    //close( handle );
      sprintf(all_mess, MESS[112], "NODORETT.DAT");
      return 1;
  }
  //-------------------------------------
  // Lettura file
  //-------------------------------------
  i = read( handle, &nodo_con_tgv_lanciato, sizeof( nodo_con_tgv_lanciato ) );
  close( handle );

  if (i != sizeof(nodo_con_tgv_lanciato))  err = 1;
  if(err) sprintf(all_mess, MESS[112], "NODORETT.DAT");

  return err;
}


// --------------------------------------------
//     save_nodi_tgv_lanciati_su_rettilineo()
// --------------------------------------------
// Salva struttura nodi riservati per TGV lanciato in rettilineo
//
int nodo::save_nodi_tgv_lanciati_su_rettilineo(char *all_mess)
/***********************************/
{
  int  i, err = 0;
  int  handle;

  //-------------------------------------
  // Apertura file
  //-------------------------------------
  handle = sopen( "NODORETT.DAT", O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
  if( handle<0 ){
     sprintf(all_mess, MESS[113], "NODORETT.DAT");
     //close( handle );
     return 1;
  }
  //-------------------------------------
  // Scrittura file
  //-------------------------------------
  i =  write( handle, &nodo_con_tgv_lanciato, sizeof( nodo_con_tgv_lanciato ) );
  close( handle );

  if (i != sizeof(nodo_con_tgv_lanciato))  err = 1;
  if(err) sprintf(all_mess, MESS[113], "NODORETT.DAT");
  return err;
}

// -----------------------------------------
//    verifica_tgv_lanciato_su_rettilineo
// -----------------------------------------
// Eseguo un comtrollo per ogni TGV e verifico se si trova su un
// rettilineo lanciato.
// Se è verificata questa condizione compilo una struttura dei
// nodi che verranno impegnati e che si trovano all'interno di
// ritorna TRUE se almeno un blocco e stato individuato
//
void nodo::verifica_tgv_lanciato_su_rettilineo()
/**********************************/
{
  double CalcolaRad;
  bool nodo_porta;
  bool tgv_lanciato_in_rett;
  int i, j, k, z, ptr_percorso;
  int p1, p2, Dist, DistX, DistY;
  int x1, x2, y1, y2;
  int rot_prec1, rot_attu, rot_succ, rot_succ1;

  //--------------------------------------------------------------
  // Ciclo per tutte le navette
  //--------------------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     //--------------------------------------------------------
     // Azzeramento perventivo dei nodi della struttura
     // relativi al TGV considerato
     //--------------------------------------------------------
     for(j=0; j<=MAXPUNTI; j++){
        if(N.nodo_con_tgv_lanciato[j]!=i) continue;
        N.nodo_con_tgv_lanciato[j]=0;
     }
     //--------------------------------------------------------
     // Esclusione dei TGV non in missione automatica
     //--------------------------------------------------------
     if(!AGV[i]->test_mission_on()       ) continue;  // scarto le navette senza missione in corso
     if(AGV[i]->stato.s.bit.allarme      ) continue;  // scarto le navette in allarme
     if(AGV[i]->mission.Esclusione       ) continue;  // scarto le navette escluse
     if(AGV[i]->mission.SemiAutomatico   ) continue;  // scarto le navette in semiautomatico
     if(AGV[i]->mission.StopManuale!=0   ) continue;  // ...
     //--------------------------------------------------------
     // Scorro la struttura della missione del TGV per
     // determinare se si trova in rettilineo lanciato.
     // Condizioni necessarie per essere lanciato
     // 1. NON DEVE ESSERE IN ACCELERAZIONE
     //    il nodo precedente alla posizione attuale deve
     //    essere in retta con il nodo attuale.
     // 2. NON DEVE ESSERE IN DECELERAZIONE
     //    il nodo successivo alla posizione attuale deve
     //    essere in retta con il nodo attuale.
     // 3. Tutti i nodi già occupati dal TGV devono essere
     //    in rettilineo
     //
     // controllo due nodi e non uno precedente e successivo
     // per essere sicuro di aver terminato l'accelerazione
     // ed essere lontano e non aver iniziato la decelerazione
     //--------------------------------------------------------
     ptr_percorso=0;
     for(j=0; j<MAXPERCORSI; j++){
        if(AGV[i]->mission.punto[j]==0                ) break;
        if(AGV[i]->mission.punto[j]!=AGV[i]->stato.pos) continue;
        ptr_percorso = j;
        break;
     }
     //--------------------------------------------------------
     // Non vengono considerati i primi nodi della missione
     // perchè se appena partino il TGV non sarà certamente
     // lanciato
     //--------------------------------------------------------
     if(ptr_percorso<2) continue;
     //--------------------------------------------------------
     // TGV in prossimità del punto intermedio -> continue
     //--------------------------------------------------------
     if(AGV[i]->stato.pos==AGV[i]->mission.pintend                                                  ) continue;
     if(ptr_percorso<MAXPERCORSI-1 && AGV[i]->mission.punto[ptr_percorso+1]==AGV[i]->mission.pintend) continue;
     if(ptr_percorso<MAXPERCORSI-2 && AGV[i]->mission.punto[ptr_percorso+2]==AGV[i]->mission.pintend) continue;
     //--------------------------------------------------------
     // Verifico lo stato di rotazione della navetta nei punti
     // che interessanno per dererminare se il TGV è laciato
     // in rettilineo
     //--------------------------------------------------------
     rot_prec1 = AGV[i]->mission.dati_perc[ptr_percorso-1].rot;
     rot_attu  = AGV[i]->mission.dati_perc[ptr_percorso  ].rot;
     rot_succ1 = AGV[i]->mission.dati_perc[ptr_percorso+1].rot;
     //--------------------------------------------------------
     // Condizioni per navetta lanciata (punti 1 e 2)
     //--------------------------------------------------------
     tgv_lanciato_in_rett=false;
     if(rot_prec1==rot_attu && rot_attu==rot_succ1){
        //---------------------------------------------------
        // Verificati i punti 1 e 2 controllo che la navetta
        // abbia solo nodi occupati in retta
        //---------------------------------------------------
        rot_succ = rot_attu;
        for(j=ptr_percorso+1; j<MAXPERCORSI; j++){
           if(AGV[i]->mission.punto[j]==0            ) break;
           if(nodo_busy[AGV[i]->mission.punto[j]]!=i ) break; // al primo nodo non occupato dal TGV -> skip!
           rot_succ = AGV[i]->mission.dati_perc[j].rot;
           if(rot_attu!=rot_succ) break;                      // al primo nodo occupato con variazione di rotaz -> skip!
        }
        //---------------------------------------------------
        // Memorizzo che il TGV è lanciato in rettilineo
        //---------------------------------------------------
        if(rot_attu==rot_succ) tgv_lanciato_in_rett=true;
     }
     if(tgv_lanciato_in_rett==false) continue;
     //--------------------------------------------------------
     // Dimostrato che il TGV è lanciato su un rettilineo
     // verifico se oltre l'ultimo nodo occupato il rettilineo
     // continua.
     //--------------------------------------------------------
     Dist=0;
     for(j=ptr_percorso+1; j<MAXPERCORSI; j++){ // non considero la posizione del TGV
        if(AGV[i]->mission.punto[j]==0) break;
        p1 = AGV[i]->mission.punto[j-1];   //punto precedente
        p2 = AGV[i]->mission.punto[j  ];   //punto considerato
        //-----------------------------------------------------
        // Il punto considerato non deve essere in ingombro
        // a una porta --> skip!
        //-----------------------------------------------------
        nodo_porta=0;
        for(k=1; k<=MAXPORTEIMP; k++){
           for(z=0; z<11; z++){
              if(PorteImp[k].Punto[z]<1        ) continue;
              if(PorteImp[k].Punto[z]>TOTPUNTI ) continue;
              nodo_porta=true;
              break;
           }
           if(nodo_porta==true) break;
        }
        //-----------------------------------------------------
        // interrompo il controllo dei nodi al prima porta
        // tanto a quel punto il tgv sicuramente deve 
        // fermarsi o rallentare per lo scambio consensi
        //-----------------------------------------------------
        if(nodo_porta==true) break;
        //-----------------------------------------------------
        // Dei punti prenotati scarto quelli già occupati
        // 1. se il punto è occupato dal tgv considerato
        //    --> continue.
        // 2. se il punto è occupato da un altro TGV --> skip.
        //-----------------------------------------------------
        if(nodo_busy[p2]!=0 && nodo_busy[p2]==i) continue;
        if(nodo_busy[p2]!=0 && nodo_busy[p2]!=i) break;
        //-----------------------------------------------------
        // Mi assicuro che la navetta prosegua in retta anche
        // oltre i nodi già occupati
        // GUARDO SOLO IL NODO SUCCESSIVO A QUELLO CONSIDERATO
        // PERCHE' CMQ LE NAVETTE COMINCIANO IL RALLENTAMENTO
        // IL NODO PRIMA DELLA ROTAZIONE
        //-----------------------------------------------------
        rot_succ = AGV[i]->mission.dati_perc[j].rot;       // successivo
        if(rot_attu!=rot_succ) break;
        //-----------------------------------------------------
        // Verifico se il punto prenotato e non occupato è
        // all'interno dell'AREA di lancio del TGV
        //-----------------------------------------------------
        x1 = Punti[ p1 ][0];
        y1 = Punti[ p1 ][1];
        x2 = Punti[ p2 ][0];
        y2 = Punti[ p2 ][1];
        DistX = abs(x1-x2);
        DistY = abs(y1-y2);
        if(DistX>30000 || DistY>30000) continue;
        CalcolaRad = (DistX*DistX) + (DistY*DistY);
        Dist = (int)(Dist + (int)(sqrt_algo( CalcolaRad )/100));  // espressa in metri
        //-----------------------------------------------------
        // se i punti prenotati, ma non occupati rimangono in
        // rettilineo e sono all'interno di un'area predefinita
        // (AREA_PER_TGV_LANCIATI_IN_RETTILINEO) li memorizzo
        // tra quelli riservati per i TGV lanciati
        //-----------------------------------------------------
        if(Dist<=AREA_PER_TGV_LANCIATI_IN_RETTILINEO && nodo_con_tgv_lanciato[p2]==0){
           //---------------------------------------------------------------
           // Prenoto il nodo solo se non lo ha già prenotato un altro TGV
           //---------------------------------------------------------------
           nodo_con_tgv_lanciato[p2] = (short int)(i);
        }
     }
  }
}

// -----------------------------------------
//   blocco_per_tgv_lanciato_su_rettilineo
// -----------------------------------------
// Verifica se forzare il punto intermedi a un dato TGV
// per una condizione di altra navetta lanciata su rettilineo
//
bool nodo::blocco_per_tgv_lanciato_su_rettilineo(short int num_agv, short int punto, short int *altro_agv)
/**********************************/
{
  double CalcolaRad;
  int i, ptr_percorso;
  bool blocco_per_tgv_lanciato;
  int rot_attu, rot_succ;
  int p1, p2, DistTot, Dist, DistX, DistY;
  int x1, x2, y1, y2;
  int count_nodi;
  short int agv_adiacente;
  int DistRotazioneSingola = DIST_MIN_ROT_SINGOLA;

  count_nodi=0;
  *altro_agv=0;
  blocco_per_tgv_lanciato=false;
  //--------------------------------------------------------------
  // Individuo l'indice di percorso a cui è arrivato il TGV
  //--------------------------------------------------------------
  for(ptr_percorso=0; ptr_percorso<MAXPERCORSI; ptr_percorso++){
     if(AGV[num_agv]->mission.punto[ptr_percorso] == punto) break;
     if(AGV[num_agv]->mission.punto[ptr_percorso] == 0) break;
  }
  if(AGV[num_agv]->mission.punto[ptr_percorso]!=punto) return FALSE;
  //--------------------------------------------------------------
  // Se il sucessivo al nodo considerato è già occupato dal TGV
  // allora "punto" non può più essere forzato come "pintend"
  //--------------------------------------------------------------
  if(AGV[num_agv]->mission.punto[ptr_percorso+1]!=0 && N.nodo_busy[AGV[num_agv]->mission.punto[ptr_percorso+1]]==num_agv) return FALSE;
  //--------------------------------------------------------------
  // Verifico se incontro un nodo con queste caratteristiche:
  // 1. prenotato da un altro tgv lanciato in rettilineo
  // 2. punto di rotazione del tgv considerato
  // 3. deve essere il punto in cui il tgv fa una rotazione
  //    rispetto al punto considerato (questo garantisce
  //    che comunque le navette non stiano troppo lontane)
  // 4. a una distanza sufficiente per potermi fermare sul
  //    nodo considerato senza collisioni
  //--------------------------------------------------------------
  DistTot=0;
  rot_attu = AGV[num_agv]->mission.dati_perc[ptr_percorso].rot;
  for(i=ptr_percorso+1; i<MAXPERCORSI; i++){
     if(AGV[num_agv]->mission.punto[i]==0) break;
     //-----------------------------------------------------
     // Memorizzo i nodi considerati
     //-----------------------------------------------------
     p1 = AGV[num_agv]->mission.punto[i-1];
     p2 = AGV[num_agv]->mission.punto[i  ];
     //-----------------------------------------------------
     // Se il nodo considerato è occupato -> skip
     //-----------------------------------------------------
     if(nodo_busy[p2]!=0 && nodo_busy[p2]!=num_agv) break;
     //-----------------------------------------------------
     // !!! ATTENZIONE !!!
     // Se ho già analizzato più di 3 nodi avanti -> skip
     // per evitare di fermare navette troppo lontane tra
     // loro (come numero di nodi)
     //-----------------------------------------------------
     if(count_nodi>2) break;
     //-----------------------------------------------------
     // !!! ATTENZIONE !!!
     // se nei cicli precedenti ho comunque raggiunto una
     // distanza superiore a quella di rotazione doppia
     // -> skip
     // per evitare di fermare navette troppo lontane tra
     // loro (come distanza in cm)
     //-----------------------------------------------------
     if(DistTot>DIST_MIN_ROT_DOPPIA_MAX) break;
     //-----------------------------------------------------
     // incremento il conteggio dei nodi che sto analizzando
     //-----------------------------------------------------
     count_nodi++;
     //-----------------------------------------------------
     // Memorizzo la distanza tra i 2 punti che sto
     // considerando
     //-----------------------------------------------------
     x1 = Punti[ p1 ][0];
     y1 = Punti[ p1 ][1];
     x2 = Punti[ p2 ][0];
     y2 = Punti[ p2 ][1];
     DistX = abs(x1-x2);
     DistY = abs(y1-y2);
     if(DistX>30000 || DistY>30000) continue;
     CalcolaRad = (DistX*DistX) + (DistY*DistY);
     Dist    = (int)(sqrt_algo( CalcolaRad ));
     DistTot = DistTot+Dist;
     //-----------------------------------------------------
     // verifico se la navetta deve compiere una rotazione
     //-----------------------------------------------------
     rot_succ = AGV[num_agv]->mission.dati_perc[i].rot;
     if(rot_attu==rot_succ) continue;
     //-----------------------------------------------------
     // Se il TGV deve compiere una rotazione verifico anche
     // se il nodo è prenotato da un altro TGV lanciato
     //-----------------------------------------------------
     if(nodo_con_tgv_lanciato[p2]!=0 && nodo_con_tgv_lanciato[p2]!=num_agv){
        agv_adiacente = nodo_con_tgv_lanciato[p2];
        //-----------------------------------------------------
        // Se recupero la ditanza necessaria per lasciare
        // passare la navetta lanciata
        //-----------------------------------------------------
        DistRotazioneSingola = DIST_MIN_ROT_SINGOLA;
      //if(AGV[agv_adiacente]->stato.s.bit.car) DistRotazioneSingola = DIST_MIN_ROT_SINGOLA + INGOMBRO_BOX_A_BORDO;
        //-----------------------------------------------------
        // Controllo che la distanza si superiore
        //-----------------------------------------------------
        if(DistTot>=DistRotazioneSingola){
           *altro_agv = agv_adiacente;
           blocco_per_tgv_lanciato=true;
        }
     }
     break;
  }
  return blocco_per_tgv_lanciato;
}

// -----------------------------------------
//    ricalcolo_distanze_nodi
// -----------------------------------------
// Ricalcolo distanze nodi da GRAFICA
//
int nodo::ricalcolo_distanze_nodi(char *all_mess)
/**********************************/
{
  double dist;
  short int i, j, p1, p2;
  int x1, x2, y1, y2, distX, distY;
  int Id, Raggio, Circonferenza;

  //--------------------------------------------
  // Ciclo per tutti i punti dell'impianto
  //--------------------------------------------
  for(i=0; i<MAXPUNTI; i++){
     if(N.n[i]->num<1        ) continue;
     if(N.n[i]->num>MAXPUNTI ) continue;
     p1 = N.n[i]->num;
     //--------------------------------------------
     // scorrimento di tutte le righe di percorso
     //--------------------------------------------
     for(j=0; j<MAXPUNTINODO; j++){
        if(N.n[i]->rec[j].ps==0) continue;
        dist = 0;
        p2 = N.n[i]->rec[j].ps;
        //----------------------------------
        // Recupero coordinate
        //----------------------------------
        x1 = Punti[p1][0];
        y1 = Punti[p1][1];
        x2 = Punti[p2][0];
        y2 = Punti[p2][1];
        //----------------------------------
        // Se i nodi si trovano in retta la
        // distanza viene calcolata con il
        // teorema di pitagora.
        //----------------------------------
        distX = abs(x1-x2);
        distY = abs(y1-y2);
        if(distX>30000 || distY>30000) continue;
        dist  = (double)(sqrt_algo((distX*distX)+(distY*distY)));
        //----------------------------------
        // Calcolo della distanza percorsa
        // su un arco di circonferneza.
        //----------------------------------
        Id=Raggio=Circonferenza=0;
        if(!N.CercaCurva(TRUE, p1, p2, &Raggio, &Circonferenza, &Id)){
           dist = (short int)(Circonferenza);
        }
        N.n[i]->rec[j].dist=(short int)(dist);
     }
  }
  //---------------------------------------
  // Salvataggio del file dei nodi
  //---------------------------------------
  if(N.save_file_nodi(all_mess)) return 1;

  return 0;
}



