// ------------------------------------------------------------------------
//             file       MAG.CPP
// ------------------------------------------------------------------------
// Elenco funzioni del controllo della classe magazzini
//      magaz()
//
#include <stdio.h>
#include <io.h>
#include <mem.h>
#include <string.h>
#include <dir.h>
#include <conio.h>

#include <fcntl.h>
#include <errno.h>
#include <share.h>
#include <sys\stat.h>

#include "Tecnofer.h"
#include "Main.h"
#include "Remoto.h"
#include "Stato.h"
#include "Layout.h"
#include "SQLDB.h"

// ---------------------------
//      da GRAPHVAR.CPP
// ---------------------------
extern int BloccoRot[][3];

// --------------------------------
//        magaz()
// --------------------------------
//  inizializza tutte le variabili globali della classe PROCESS.
//
magaz::magaz()
/****************/
{
  //----------------------------------------------------------------
  // File gestione CLASSE MAGAZZINI
  //----------------------------------------------------------------
  strcpy( mag_name_sorg, "MAGAZZIN.DAT");
  strcpy( mag_name_dest, "MAGAZZIN.DAT");

  //----------------------------------------------------------------
  // File gestione BAIE IMPIANTO
  //----------------------------------------------------------------
  strcpy( tipibox_name_sorg,  "TIPIBOX.DAT");
  strcpy( tipibox_name_dest,  "TIPIBOX.DAT");
}


//=========== TIPIBOX


// ------------------------
//   load_file_tipibox()
// ------------------------
// Dal file TIPIBOX.DAT compila i record in memoria precedentemente allocati
//
int magaz::load_file_tipibox(char *all_mess)
/**********************************/
{
  int i, err=0;
  int len, handle;

  strcpy(all_mess, "OK");
  memset(&TipiBox, 0, sizeof(TipiBox ));
  //-------------------------------------------------
  // Assegnazione diciture di default
  //-------------------------------------------------
  for(i=0; i<MAXTIPOBOXIMP; i++){
     if(strcmp(DESCTIPOBOX[i], "")==0) break;
     strcpy(TipiBox[i].Nome, DESCTIPOBOX[i]);
     TipiBox[i].Lung                   = LUNG_CEST;
     TipiBox[i].Larg                   = LARG_CEST;
     TipiBox[i].offset_tgv_da_nodo_mag = 0;
     TipiBox[i].dist_box_in_magaz      = DIST_CEST_IN_MAG;
     TipiBox[i].eccesso_count_pos_mag  = 0.70;
  }
  //-------------------------------------------------
  // Controllo se il File pu• essere Letto aprendolo
  // in modalit… di Condivisione.
  //-------------------------------------------------
  handle = sopen( tipibox_name_sorg, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
      close( handle );
      sprintf(all_mess, MESS[112], tipibox_name_sorg);
      return 1;
  }
  //--------------------------------
  // Ciclo lettura file PORTEIMP.DAT
  //--------------------------------
  len = read( handle, &TipiBox, sizeof( TipiBox ) );
  if(len != sizeof(TipiBox)) err=1;
  if(err){
     memset(&TipiBox, 0, sizeof( TipiBox ));
     sprintf(all_mess, MESS[112], tipibox_name_sorg);
  }
  close( handle );
  return err;
}


// ------------------------
//   save_file_tipibox()
// ------------------------
// Salva sul file TIPIBOX.DAT i records delle macchine in memoria
//
int magaz::save_file_tipibox(char *all_mess)
/**********************************/
{
  int err=0;
  int len, handle;

  //------------------------------
  // Apertura file PORTEIMP.DAT
  //------------------------------
  handle = sopen( tipibox_name_dest, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
  if( handle<0 ){
     close( handle );
     sprintf(all_mess, MESS[113], tipibox_name_dest);
     return 1;
  }
  //----------------------------------
  // Ciclo Scrittura file PORTEIMP.DAT
  //----------------------------------
  len= write( handle, &TipiBox, sizeof( TipiBox ));
  if (len != sizeof( TipiBox )){
     err = 1;
     memset(&TipiBox, 0, sizeof( TipiBox ));
     sprintf(all_mess, MESS[113], tipibox_name_dest);
  }
  close( handle );
  return err;
}


// ==================== MAGAZZINI

// ----------------------------------------------
//       read_mag()
// ----------------------------------------------
// Legge il record "ch" in base a "num_mac" passato come parametro
//
int magaz::read_mag(int num_mag, struct MAG *mg, char *all_mess)
/***********************************************/
{
  int  stato=0;

  stato = cerca_record_mag(mg, num_mag);
  if(stato){
     //---------------------------------
     // record non trovato ! -> errore
     //---------------------------------
     sprintf(all_mess, "Error in read mag%d (%s)", num_mag, "read_mag()");
     return stato;
  }
  return 0;

}

// ----------------------------------------------
//       test_punto_presente_mag()
// ----------------------------------------------
// Verifica se  "punto" è + un nodo di una Macchina. Se si ritorna in
// "macchina" il numero della macchina trovato.
//
int magaz::test_punto_presente_mag(char *all_mess, short int punto, short int *num_mag, short int *num_pos)
/**********************************************************************/
{
  bool trovato;
  int  i, NumPos, n_rec, stato;

  strcpy( all_mess, "OK");
  //--------------------------------
  // Azzeramento variabili
  //--------------------------------
  *num_mag =0;
  *num_pos =0;
  //--------------------------------
  // Nodo non valido
  //--------------------------------
  if(punto<1 || punto>TOTPUNTI){
     sprintf(all_mess, "(0) Error node %d does not good for stores archive...", punto);
     return 1;
  }
  //--------------------------------
  // Cerca magazzino non nodo
  //--------------------------------
  for( n_rec=1; n_rec<=MAXMAG; n_rec++){
     if( m[ n_rec ]->Num <=0       ) continue;
     if( m[ n_rec ]->Num >TOTMAG   ) continue;
     if( n_rec>TOTMAG              ) break;
     //--------------------------------
     // MAGAZZINI MULTI NODO
     //--------------------------------
     NumPos=0;
     for(i=1; i<=m[ n_rec ]->Npt; i++){
        if(m[ n_rec ]->Pos[i].NodoIng<1        ) continue;
        if(m[ n_rec ]->Pos[i].NodoIng>TOTPUNTI ) continue;
        if(punto!=m[ n_rec ]->Pos[i].NodoIng   ) continue;
        NumPos = i;
        break;
     }
     if(NumPos==0){
        for(i=1; i<=m[ n_rec ]->Npt; i++){
           if(m[ n_rec ]->Pos[i].NodoOut<1        ) continue;
           if(m[ n_rec ]->Pos[i].NodoOut>TOTPUNTI ) continue;
           if(punto!=m[ n_rec ]->Pos[i].NodoOut   ) continue;
           NumPos = i;
           break;
        }
     }
     //--------------------------------
     // Nodo trovato
     //--------------------------------
     if(punto==m[ n_rec ]->PuntoIng || punto==m[ n_rec ]->PuntoOut || NumPos!=0){
        *num_mag = m[ n_rec ]->Num;
        *num_pos = NumPos;
        return 0;
     }
  }
  if(*num_mag==0){
     sprintf(all_mess, "Error search node %d in store archive...", punto);
     return 1;
  }
  return 0;
}

// ------------------------
//  load_file_mag()
// ------------------------
// Dal file BAIE.DAT compila i record in memoria precedentemente allocati
//
int magaz::load_file_mag(char *all_mess)
/**********************************/
{
  int  err=0;
  int  i, len;
  int  handle;

  strcpy( all_mess, "OK" );
  //------------------------------
  // Azzera records in memoria
  //------------------------------
  for( i=0; i<=MAXMAG; i++){
     memset( m[i], 0, sizeof( MAG ));
  }
  //------------------------------
  // Apertura file CHIAMATE.DAT
  //------------------------------
  //-------------------------------------------------
  // Controllo se il File pu• essere Letto aprendolo
  // in modalit… di Condivisione.
  //-------------------------------------------------
  handle = sopen( mag_name_sorg, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
      close( handle );
      sprintf(all_mess, MESS[112], mag_name_sorg);
      return 1;
  }
  //--------------------------------
  // Lettura file LOCAZIONE.DAT
  //--------------------------------
  for( i=0; i<=MAXMAG; i++){
     len = read( handle, m[i], sizeof( MAG ) );
     if (len != sizeof( MAG ))  err = 1;
     if(err){
        memset( m[i], 0, sizeof( MAG ));
        sprintf(all_mess, MESS[351], i, mag_name_sorg);
        break;
     }
  }
  close( handle );

  return err;
}


// ------------------------
//  save_record_mag()
// ------------------------
// Salva sul file BAIE.DAT i records di macchine in memoria
//
int magaz::save_record_mag(int numero, char *all_mess)
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
  handle = sopen( mag_name_dest, O_RDWR | O_BINARY , SH_DENYNO );
  if( handle<0 ){
     close( handle );
     sprintf(all_mess, MESS[112], mag_name_dest);
     return 1;
  }
  //------------------------------
  // Controllo lunghezza file
  //------------------------------
  length=0;
  if(handle >-1) length = filelength(handle);
  if( length < ((long)(numero)* sizeof( MAG ))){
     sprintf(all_mess, MESS[113], mag_name_dest);
     close(handle);
     return 1;
  }

  //--------------------------------------
  // Calcolo dell'OFFSET e posizionamento
  //--------------------------------------
  curpos = (long ) numero * sizeof( MAG );
  if( lseek( handle, curpos, SEEK_SET )<1){  // pos. il puntatore a inzio record
     sprintf(all_mess, "errore puntamento al record %d del file %s...", numero, mag_name_dest);
     close( handle );
     return 1;
  }
  //-------------------------------
  // Scrittura Record LOCAZION.DAT
  //-------------------------------
  len= write( handle, m[numero], sizeof( MAG ));
  if(len != sizeof( MAG )){
     err=1;
     sprintf(all_mess, "errore scrittura del record %d del file %s...", numero, mag_name_dest);
  }
  close(handle);

  return err;
}


// ------------------------
//    save_file_mag()
// ------------------------
// Salva sul file AUTOCLAV.DAT i records delle macchine in memoria
//
int magaz::save_file_mag(char *all_mess)
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
  handle = sopen( mag_name_dest, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
  if( handle<0 ){
     close( handle );
     sprintf(all_mess, "Errore scrittura file: %s", mag_name_dest);
     return 1;
  }
  //----------------------------------
  // Ciclo Scrittura file CHIAMATE.DAT
  //----------------------------------
  for( i=0; i<=MAXMAG; i++){
     len= write( handle, m[i], sizeof( MAG ));
     if (len != sizeof( MAG ))  err = 1;
     if(err){
         memset( m[i], 0, sizeof( MAG ));
         sprintf(all_mess, "errore scrittura del record %d del file %s...", i, mag_name_dest);
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
int magaz::num_record_mag()
/**********************************/
{
  int numero_rec=0;
  int i;

  for( i=0; i<MAXMAG; i++){
     if( m[i]->Num >0 ) numero_rec++;
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
int magaz::cerca_record_mag(struct MAG *mg, int numero)
/**********************************/
{
  if( numero > MAXMAG        ) return 1;
  if( m[ numero ]->Num ==0   ) return 1;

  memcpy( mg, m[ numero ], sizeof( MAG ) );

  return 0;
}



// -----------------------------------------
//       ConvertiQuotePLC()
// -----------------------------------------
//  Da quote in X, Y espresse in impulsi encoder e lette dal PLC TGV
//  esegue la conversione in posizione e piano di deposito
//
//  IMPORTANTE: se il paramentro "RapEncP=1" vengono gestiti direttamente i (cm)
//
//
int magaz::ConvertiQuotePLC(bool lato_uscita, short int num_mag, double RapEncP, short int tipo_box, WORD QuotaP, WORD QuotaH_A, WORD QuotaH_B, short int TypPresa, short int Posizione, short int Piano, short int *TypPresaPLC, short int *PosizionePLC, short int *PianoPLC, char *all_mess)
/*************************************************************************/
{
  WORD   QuotaH;
  double dec;
  double a, b;
  double PosizioneTGV;
  double PosizioneCalc;
  bool   deposito_su_out;
  short int lato_box;
  short int PosMag;
  short int NumPosATerra;

  strcpy(all_mess, "");
  //---------------------------------------------------------
  // Inizializzazione con valori di teorici
  //---------------------------------------------------------
  *TypPresaPLC  = TypPresa;
  *PosizionePLC = Posizione;
  *PianoPLC     = Piano;
  //---------------------------------------------------------
  // Determino il tipo di riempimento del magazzino
  //---------------------------------------------------------
  deposito_su_out = lato_uscita;
  if(m[num_mag]->Fifo==false) deposito_su_out=false;
  //---------------------------------------------------------
  // Verifica condizioni Generali
  //---------------------------------------------------------
  sprintf( all_mess, "Store %d NOT ok", num_mag );
  if( m[num_mag]->Num<1                ) return 1;
  if( m[num_mag]->Num>TOTMAG           ) return 1;
  if( m[num_mag]->Npt==0               ) return 1;
  if( m[num_mag]->PuntoIng==0          ) return 1;
  if( m[num_mag]->PuntoIng>MAXPUNTI    ) return 1;
  if( m[num_mag]->PuntoOut==0          ) return 1;
  if( m[num_mag]->PuntoOut>MAXPUNTI    ) return 1;
  //---------------------------------------------------------
  // GESTIONE_DINAMICA_MAG
  // Se questo #define è "0" significa che la gestione non
  // è attiva --> skip!
  //---------------------------------------------------------
  if(GestioneDinamicaMagazzini==0) return 0;
  //---------------------------------------------------------
  // Recupero il lato box su cui eseguire il calcolo
  //---------------------------------------------------------
  lato_box = M.TipiBox[tipo_box].Lung;
  //---------------------------------------------------------
  // Calcolo capacita magazzin presunta in base al tipo box
  //---------------------------------------------------------
//NumPosATerra  = (short int)(m[num_mag]->Profondita/(lato_box+DIST_CEST_IN_MAG))+1;
  a = (double)(m[num_mag]->Profondita)/double(lato_box+M.TipiBox[tipo_box].dist_box_in_magaz);
  b = (double)(a) - (int)(a);
  if(b>M.TipiBox[tipo_box].eccesso_count_pos_mag) a = (a - b) + 1;      // tolgo i decimali al valore calcolato e aggingo "1".
  NumPosATerra = (short int)(a)+1;     // calcolo effettivo palette a terra

  //---------------------------------------------------------
  // CALCOLO DELLA POSIZIONE IN PROFONDITA
  // In base all'offset da nodo e al lato di ingresso calcolo
  // la posizione ipotetica su cui si trova il TGV.
  //---------------------------------------------------------
  PosizioneTGV  = (double   )(QuotaP*RapEncP) - (double)(m[num_mag]->OffSetDaNodo/RapEncP) - (double)(TipiBox[tipo_box].offset_tgv_da_nodo_mag/RapEncP);
  dec           = (double   )(PosizioneTGV) - (int)(PosizioneTGV);
  if(dec>0.5) PosizioneTGV = (PosizioneTGV-dec) + 1; // arrotondo al cm successivo
  PosizioneCalc = (double   )(PosizioneTGV/(lato_box+TipiBox[tipo_box].dist_box_in_magaz))+1;
  PosMag        = (short int)(PosizioneCalc);
  dec           = (double   )(PosizioneCalc) - (int)(PosizioneCalc);
  //---------------------------------------------------------
  // Arrotondo alla posizione successiva se i decimali
  // superano ARROTONDA_PER_ECCESSO_SU_CONT_POS tranne
  // che per la prima posizione che serve anche per il
  // completo
  //
  // 21/06/2013 PER LA POSIZIONE "1" VIENE INCREMENTATO
  //            SOLO SE QUASI PRECISO
  //---------------------------------------------------------
  if(PosMag>1  && dec>M.TipiBox[tipo_box].eccesso_count_pos_mag) PosMag++;
  if(PosMag==1 && dec>0.9                                      ) PosMag++;
  //---------------------------------------------------------
  // Inverto la posizione se l'ingresso è dall'uscita
  //---------------------------------------------------------
  if(deposito_su_out==true) PosMag = NumPosATerra-(PosMag-1);
  //---------------------------------------------------------
  // Se il valore calcolato va oltre alla dimesione massima
  // del magazzino, ne forzo il valore
  //---------------------------------------------------------
  if(PosMag>NumPosATerra) PosMag = NumPosATerra;
  //---------------------------------------------------------
  // Aggiornamento valore restituito dalla funzione
  //---------------------------------------------------------
  *PosizionePLC = PosMag;
  //---------------------------------------------------------
  // CALCOLO DEL PIANO
  // Attualmente sono previsti solo 2 piani di deposito
  //
  // N.B. Quota di riferimento presa in base alla
  //      GestioneForcheDoppieTGV
  //---------------------------------------------------------
  QuotaH = QuotaH_B;
  if(GestioneDoppiaForcaTGV==1 && TypPresa==N_FORCA_A) QuotaH = QuotaH_A;
  //---------------------------------------------------------
  // Attualmente predisposizione solo ai 2 piani in magazzino
  //---------------------------------------------------------
  *PianoPLC=0;               // piano terra
  if(QuotaH>0) *PianoPLC=1;  // piano primo

  return 0;
}

// -----------------------------------------
//       VerificaTipoDeposito()
// -----------------------------------------
//  Restituisce  Posizione( dati magazzino ), Piano
//
//  Posizione :   0 -> Mag completo nessun deposito possibile
//                1 -> Deposito su 1° Posizione
//                2 -> Deposito su 2° Posizione
//                3 -> Deposito su 3° Posizione
//                4 -> Deposito su 4° Posizione
//                ...
//  Piano :       0 -> Piano Terra
//                1 -> 1° Piano
//                2 -> 2° Piano
//                ...
//
int magaz::VerificaTipoDeposito(bool lato_uscita, short int num_mag, short int num_pos, short int TypPresa, short int TipoBox, bool Presa[POSINPRESA+1][PIANIINPRESA+1], bool Sovrapponibile, bool Basso, bool Salta, short int *Posizione, short int *Piano, short int *PosizioneTgv, char *all_mess)
/*************************************************************************/
{
//int  stato=0;
  short int i,j;
//struct MAG mag;
  short int lato_box;
  short int AbilitaPianoMag;
  short int AbilitaPianoPosMag;
  short int last_piano_presa;
  int   PosizioneOccupata;
  int   PianoOccupato;
  int   PosizioneDest;
  int   PianoDest;
  int   NumPosATerra;
  bool  PosizioneOccupataSovrapponibile;
  bool  Completo;
  bool  deposito_su_out;
  bool  Trovato;
  bool  MagTrovato;
  bool  controllo_stato_box_a_terra;
  double a, b;

  *Posizione=0;
  *Piano=0;
  strcpy(all_mess, "");
//memset(&mag, 0, sizeof(MAG));
  //---------------------------------------------------------
  // Ricerca magazzino
  //---------------------------------------------------------
//stato = read_mag(num_mag, &mag, all_mess);
//if(stato) return 1;
  //---------------------------------------------------------
  // Verifica condizioni Generali
  //---------------------------------------------------------
  sprintf( all_mess, "Store %d NOT ok", num_mag );
  if( m[num_mag]->Num<1                                      ) return 1;
  if( m[num_mag]->Num>TOTMAG                                 ) return 1;
  if( m[num_mag]->Npt==0                                     ) return 1;
  if( m[num_mag]->PuntoIng==0                                ) return 1;
  if( m[num_mag]->PuntoIng>TOTPUNTI                          ) return 1;
  if( m[num_mag]->PuntoOut==0                                ) return 1;
  if( m[num_mag]->PuntoOut>TOTPUNTI                          ) return 1;
//if( TypPresa>=N_FORCA_AB && m[num_mag]->Sovrapposizione==0 ) return 1;
  //---------------------------------------------------------
  // Con questo #define si attiva il controllo di
  // una variabile di stato box (che potrebbe cambiare da
  // impianto a impianto) che informa il sistema della
  // possibiltà di sovrapporre altri box al box considerato.
  //
  // SENZA QUESTO #DEFINE OGNI BOX A TERRA IN MAG E'
  // SOVRAPPONIBILE CON ALTRI
  //
  // l'unico riferimento sono i piani abilitati in magazzino
  //---------------------------------------------------------
  controllo_stato_box_a_terra=false;
  #ifdef CONTROLLO_STATO_BOX_A_TERRA_IN_DEPOSITO
     controllo_stato_box_a_terra=true;
  #endif;
  //---------------------------------------------------------
  // Determino il tipo di riempimento del magazzino
  //---------------------------------------------------------
  deposito_su_out = lato_uscita;
  if(m[num_mag]->Fifo==false) deposito_su_out=false;
  //---------------------------------------------------------
  // Calcolo capacita magazzin presunta in base al tipo box
  //---------------------------------------------------------
  NumPosATerra = m[num_mag]->Npt;
  //--------------------------------------------------------------------
  // Massima sovrapposizione impostata a editor
  //--------------------------------------------------------------------
  AbilitaPianoMag = m[num_mag]->Sovrapposizione;
  //--------------------------------------------------------------------
  // Gestione fozatura legata a tipologia pallet trasportata
  //--------------------------------------------------------------------
  if(Basso==true  && MagSovrappMAX_BASSI!=99 && (MagSovrappMAX_BASSI-1)<AbilitaPianoMag) AbilitaPianoMag = (MagSovrappMAX_BASSI-1);
  if(Basso==false && MagSovrappMAX_ALTI!=99  && (MagSovrappMAX_ALTI-1)<AbilitaPianoMag ) AbilitaPianoMag = (MagSovrappMAX_ALTI-1);
  //--------------------------------------------------------------------
  // GESTIONE_DINAMICA_MAG
  // Sul primo inserimento anche la capaciatà viene forzata dal TGV
  //--------------------------------------------------------------------
  if(GestioneDinamicaMagazzini==1){
     //---------------------------------------------------------
     // Recupero il lato box su cui eseguire il calcolo
     //---------------------------------------------------------
     lato_box = M.TipiBox[TipoBox].Lung;
     //---------------------------------------------
     // Calcolo postazioni a terra
     //---------------------------------------------
   //NumPosATerra = (short int)(m[num_mag]->Profondita/(lato_box+DIST_CEST_IN_MAG))+1;
     a = (double)(m[num_mag]->Profondita)/(double)(lato_box+M.TipiBox[TipoBox].dist_box_in_magaz);
     b = (double)(a) - (int)(a);
     if(b>M.TipiBox[TipoBox].eccesso_count_pos_mag) a = (a - b) + 1;      // tolgo i decimali al valore calcolato e aggingo "1".
     NumPosATerra = (short int)(a)+1;     // calcolo effettivo palette a terra
  }
  //---------------------------------------------------------
  // Controlli di magazzino completo in base
  // alla posizione di deposito
  //
  // DEPOSITO DA INGRESSO: Completo quando la
  // prima posizione al piano ammesso è impegnata
  //
  // DEPOSITO SU USCITA: Completo quando l'ultima
  // posizione al piano ammesso è impegnata
  //---------------------------------------------------------
  Completo=false;
  if(deposito_su_out==false && m[num_mag]->Pos[1].Presenza[AbilitaPianoMag]==true            ) Completo=true;
  if(deposito_su_out==true  && m[num_mag]->Pos[NumPosATerra].Presenza[AbilitaPianoMag]==true ) Completo=true;
  if(Completo==true ) return 1;

  //---------------------------------------------------------
  // Se il tipo presa è doppia forca, ma il magazzino non
  // consente la sovrapposizione devo depositare a forca
  // singola
  //---------------------------------------------------------
  if( TypPresa>=N_FORCA_AB && AbilitaPianoMag==0 ){
     TypPresa=N_FORCA_B;
  }
  //---------------------------------------------------------
  // Verifica Prima o Ultima Posizione Occupata a seconda
  // del lato da cui si vuole depositare
  //---------------------------------------------------------
  MagTrovato=false;
  if(deposito_su_out==false){
     for(i=1; i<=MAXTERRAMAG; i++){
        for(j=MAXPIANIMAG-1; j>=0; j--){
           if( m[num_mag]->Pos[i].Presenza[j]  ) MagTrovato=true;
           if( MagTrovato                      ) break;
        }
        if( i>NumPosATerra ) MagTrovato=true;
        if( MagTrovato     ) break;
     }
  }
  else{
     for(i=MAXTERRAMAG; i>=0; i--){
        for(j=MAXPIANIMAG-1; j>=0; j--){
           if( m[num_mag]->Pos[i].Presenza[j]  ) MagTrovato=true;
           if( MagTrovato                      ) break;
        }
        if( i==0        ) MagTrovato=true;
        if( MagTrovato  ) break;
     }
  }
  //---------------------------------------------------------
  // Memorizzo ultimo piano occupato
  //---------------------------------------------------------
  PosizioneOccupataSovrapponibile=false;
  PosizioneOccupata = i;
  PianoOccupato     = j;
  //---------------------------------------------------------
  // Ultima posizione occupata
  //---------------------------------------------------------
  if( PosizioneOccupata>NumPosATerra    ) PianoOccupato=AbilitaPianoMag;
  //---------------------------------------------------------
  // CONTROLLO POSIZIONE A TERRA
  //---------------------------------------------------------
  if(controllo_stato_box_a_terra==false ) PosizioneOccupataSovrapponibile=true;
  else{
     //-------------------------------------------------
     // Sovrapponibiltà data da:
     // - bit di abilitazione alla sovrapposizione
     // - stesso tipo box tra pos a terra e box in dep
     //-------------------------------------------------
     if(m[num_mag]->Pos[PosizioneOccupata].Presenza[PianoOccupato]==true){
        // BIT DI SOVRAPPONIBILITA'
        PosizioneOccupataSovrapponibile = m[num_mag]->Pos[PosizioneOccupata].Box[PianoOccupato].Sovrap;
        // CONTROLLO TIPOBOX
        if(PosizioneOccupataSovrapponibile==true){
           if(m[num_mag]->Pos[PosizioneOccupata].Box[PianoOccupato].TipoBox!=TipoBox) PosizioneOccupataSovrapponibile=false;
           //-------------------------------------------------
           // Se i pallet in deposito e già a magazzino sono
           // di tipo diverso devo sovrapporre alla minima
           // altezza prevista tra i due
           //-------------------------------------------------
           if(Basso!=m[num_mag]->Pos[PosizioneOccupata].Box[PianoOccupato].Basso){
              if(MagSovrappMAX_BASSI!=99 && MagSovrappMAX_ALTI!=99){
                 if((MagSovrappMAX_BASSI-1)<AbilitaPianoMag) AbilitaPianoMag = (MagSovrappMAX_BASSI-1);
                 if((MagSovrappMAX_ALTI-1 )<AbilitaPianoMag) AbilitaPianoMag = (MagSovrappMAX_ALTI-1);
              }
           }
        }
        //-------------------------------------------------------------------
        // Cerco il piano di presa che contiene almeno un pallet nella PRESA
        // già a magazzino perchè la PRESA che depositerò appoggerà su quella
        //-------------------------------------------------------------------
        last_piano_presa=99;
        for(j=PIANIINPRESA-1; j>=0; j--){
           for(i=0; i<POSINPRESA; i++){
              if(m[num_mag]->Pos[PosizioneOccupata].Box[PianoOccupato].Presa[i][j]==false) continue;
              last_piano_presa=j;
              break;
           }
           if(last_piano_presa!=99) break;
        }
        // CONTROLLO STRUTTURA PRESA
        for(i=0; i<POSINPRESA; i++){
           if(m[num_mag]->Pos[PosizioneOccupata].Box[PianoOccupato].Presa[i][last_piano_presa]==false && Presa[i][0]==true){PosizioneOccupataSovrapponibile=false; break;}
        }
     }
  }
  //---------------------------------------------------------
  // FORZATURA DEPOSITO IN POSIZIONE
  // forzo come ultima posizione occuapata quella prima
  // di dove il TGV deve realmente depositare.
  //---------------------------------------------------------
  if(num_pos>0 && num_pos<NumPosATerra){
     if(deposito_su_out==false){
        if(num_pos<PosizioneOccupata){
           PosizioneOccupata=num_pos+1;
           PianoOccupato=MAXPIANIMAG;
        }
     }
     else{
        if(num_pos>PosizioneDest){
           PosizioneOccupata=num_pos-1;
           PianoOccupato=MAXPIANIMAG;
        }
     }
  }
  //---------------------------------------------------------
  // Verifica Posizione Deposito in base al tipo Presa
  //---------------------------------------------------------
  PosizioneDest=0;
  PianoDest    =0;
  if( MagTrovato ){
     //-----------------------------
     // Forca A
     //-----------------------------
     if( TypPresa==N_FORCA_A ){
        //------------------------------------------------------------
        // ABILITAZIONE DEPOSITO SOVRAPPOSTO SINGOLO
        //
        // AbilitaDepositoMagSovrapposto:
        // 0 - no possibilità di sovrapporre
        // 1 - sì possibilità di sovrapporre FORCA_A
        // 2 - sì possibilità di sovrapporre FORCA_B
        // 3 - sì possibilità di sovrapporre sia FORCA_A che FORCA_B
        //------------------------------------------------------------
        if((AbilitaDepositoMagSovrapposto==1 || AbilitaDepositoMagSovrapposto==3) && Sovrapponibile==true){
           //-----------------------------
           // Cambio Piano
           //-----------------------------
           AbilitaPianoPosMag=AbilitaPianoMag;
           if(m[num_mag]->Pos[PosizioneOccupata].MaxHPos!=0 && (m[num_mag]->Pos[PosizioneOccupata].MaxHPos-1)<AbilitaPianoMag) AbilitaPianoPosMag=(m[num_mag]->Pos[PosizioneOccupata].MaxHPos-1);
           if(AbilitaPianoPosMag!=0 && PianoOccupato<AbilitaPianoPosMag && PosizioneOccupataSovrapponibile==true){
              PosizioneDest = PosizioneOccupata;
              PianoDest     = PianoOccupato+1;
           }else{
              if(deposito_su_out==false) PosizioneDest= PosizioneOccupata-1;
              else                       PosizioneDest= PosizioneOccupata+1;
              PianoDest = 0; // piano terra
           }
        }
        else{
           if(deposito_su_out==false) PosizioneDest= PosizioneOccupata-1;
           else                       PosizioneDest= PosizioneOccupata+1;
           PianoDest = 0; // piano terra
        }
     }
     //-----------------------------
     // Forca B
     //-----------------------------
     if( TypPresa==N_FORCA_B ){
        //------------------------------------------------------------
        // ABILITAZIONE DEPOSITO SOVRAPPOSTO SINGOLO
        //
        // AbilitaDepositoMagSovrapposto:
        // 0 - no possibilità di sovrapporre
        // 1 - sì possibilità di sovrapporre FORCA_A
        // 2 - sì possibilità di sovrapporre FORCA_B
        // 3 - sì possibilità di sovrapporre sia FORCA_A che FORVA_B
        //------------------------------------------------------------
        if((AbilitaDepositoMagSovrapposto==2 || AbilitaDepositoMagSovrapposto==3) && Sovrapponibile==true){
           //-----------------------------
           // Cambio Piano
           //-----------------------------
           AbilitaPianoPosMag=AbilitaPianoMag;
           if(m[num_mag]->Pos[PosizioneOccupata].MaxHPos!=0 && (m[num_mag]->Pos[PosizioneOccupata].MaxHPos-1)<AbilitaPianoMag) AbilitaPianoPosMag=(m[num_mag]->Pos[PosizioneOccupata].MaxHPos-1);
           if(AbilitaPianoPosMag!=0 && PianoOccupato<AbilitaPianoPosMag && PosizioneOccupataSovrapponibile==true){
              PosizioneDest = PosizioneOccupata;
              PianoDest     = PianoOccupato+1;
           }else{
              if(deposito_su_out==false) PosizioneDest= PosizioneOccupata-1;
              else                       PosizioneDest= PosizioneOccupata+1;
              PianoDest = 0; // piano terra
           }
        }
        else{
           if(deposito_su_out==false) PosizioneDest= PosizioneOccupata-1;
           else                       PosizioneDest= PosizioneOccupata+1;
           PianoDest = 0; // piano terra
        }
     }
     //-----------------------------
     // Forca AB
     //
     // MAI SOVRAPPORRE!!!!!
     //
     //-----------------------------
     if( TypPresa==N_FORCA_AB ){
        if(deposito_su_out==false) PosizioneDest= PosizioneOccupata-1;
        else                       PosizioneDest= PosizioneOccupata+1;
        PianoDest = 0;
     }
  }

  //--------------------------------------------
  // Controlli di range ammessi
  //--------------------------------------------
  if( PosizioneDest<1            ) MagTrovato=0;
  if( PosizioneDest>NumPosATerra ) MagTrovato=0;
  if( PianoDest<0                ) MagTrovato=0;
  if( PianoDest>AbilitaPianoMag  ) MagTrovato=0;

  //--------------------------------------------
  // Magazzino NON trovato
  //--------------------------------------------
  if( MagTrovato==0 ){
     *Posizione     =0;
     *PosizioneTgv  =0;
     *Piano         =0;
     return 1;
  }

  //---------------------------------------------------------
  // Se è richiesto il SALTO POSISIZIONE incremento di "1"
  // la PosizioneOccupata
  //
  // IMPORTANTE: fattibile solo con magazzini multi-nodo
  //             e senza forzatura della posizione
  //---------------------------------------------------------
  if(Salta==true && num_pos==0 && MagazziniMultiNodo==true){
     //---------------------------------------
     // Se il deposito deve avvenire già a
     // terra, ottengo il "buco" con il
     // decremento di "1"
     //---------------------------------------
     if(PianoDest==0){
        if(deposito_su_out==false){PosizioneDest--; PianoDest=0;}
        else                      {PosizioneDest++; PianoDest=0;}
     }
     //---------------------------------------
     // Se il deposito previsto è al piano
     // rialzato, per avere il buco, devo
     // decrementare di "2"
     //---------------------------------------
     else{
        if(deposito_su_out==false){PosizioneDest=PosizioneDest-2; PianoDest=0;}
        else                      {PosizioneDest=PosizioneDest+2; PianoDest=0;}
     }
  }

  //--------------------------------------------
  // verifica se il salto pos. mi porta fuori Mag.
  // Controlli di range ammessi
  //--------------------------------------------
  if( PosizioneDest<1            ) MagTrovato=0;
  if( PosizioneDest>NumPosATerra ) MagTrovato=0;
  if( PianoDest<0                ) MagTrovato=0;
  if( PianoDest>AbilitaPianoMag  ) MagTrovato=0;

  //--------------------------------------------
  // Magazzino NON trovato
  //--------------------------------------------
  if( MagTrovato==0 ){
     *Posizione     =0;
     *PosizioneTgv  =0;
     *Piano         =0;
     return 1;
  }

  //--------------------------------------------
  // Magazzino TROVATO
  //--------------------------------------------
  *Piano        = PianoDest;
  *Posizione    = PosizioneDest;
  *PosizioneTgv = PosizioneDest;
  if(deposito_su_out==true) *PosizioneTgv = NumPosATerra-(PosizioneDest-1);

  return 0;
}


// -----------------------------------------
//       VerificaTipoPrelievo()
// -----------------------------------------
//  Restituisce  Posizione( dati magazzino ), Piano, Posizione TGV
//
//  Posizione :   0 -> Nessun prelievo possibile
//                1 -> Deposito su 1° Posizione
//                2 -> Deposito su 2° Posizione
//                3 -> Deposito su 3° Posizione
//                4 -> Deposito su 4° Posizione
//                ...
//  Piano :       0 -> Piano Terra
//                1 -> 1° Piano
//                2 -> 2° Piano
//                ...
//  PosizioneTgv: Posizione incontrata dal TGV rispetto al nodo di entrata: può essere speculare a "Posizione"
//                se l'ingresso è dalla parte opposta
//                0 -> Nessun prelievo possibile
//                1 -> Deposito su 1° Posizione
//                2 -> Deposito su 2° Posizione
//                3 -> Deposito su 3° Posizione
//                4 -> Deposito su 4° Posizione
//                ...
//
int magaz::VerificaTipoPrelievo(bool lato_ingresso, short int num_mag, short int TypPresa, short int *Posizione, short int *Piano, short int *PosizioneTgv, char *all_mess)
/*************************************************************************/
{
  short int i,j;
  int   PosizioneOccupata;
  int   PianoOccupato;
  int   PosizioneDest;
  int   PianoDest;
  bool  Trovato;
  bool  MagTrovato;
  bool  prelievo_da_ing;

  *PosizioneTgv=0;
  *Posizione=0;
  *Piano=0;
  strcpy(all_mess, "");
  //---------------------------------
  // Verifica condizioni Generali
  //---------------------------------
  sprintf( all_mess, "Store %d NOT ok", num_mag );
  if( m[num_mag]->Num<1             ) return 1;
  if( m[num_mag]->Num>TOTMAG        ) return 1;
  if( m[num_mag]->Npt==0            ) return 1;
  if( m[num_mag]->NumCestoni==0     ) return 1;
  if( m[num_mag]->PuntoIng==0       ) return 1;
  if( m[num_mag]->PuntoIng>TOTPUNTI ) return 1;
  if( m[num_mag]->PuntoOut==0       ) return 1;
  if( m[num_mag]->PuntoOut>TOTPUNTI ) return 1;
//  if( TypPresa>=N_FORCA_AB && m[num_mag]->Sovrapposizione==false ) return 1;

  prelievo_da_ing = !m[num_mag]->Fifo;
  if(lato_ingresso==true) prelievo_da_ing=true;
  //--------------------------------------------
  // Verifica Prima Posizione Occupata
  // Nel caso di MAG FiFO l'ingresso è opposto
  //--------------------------------------------
  MagTrovato=false;
  if( prelievo_da_ing==true ){
     for(i=1; i<=MAXTERRAMAG; i++){
        for(j=MAXPIANIMAG-1; j>=0; j--){
           if( m[num_mag]->Pos[i].Presenza[j]  ) MagTrovato=true;
           if( MagTrovato                      ) break;
        }
        if( MagTrovato ) break;
     }
  }else{
     for(i=MAXTERRAMAG; i>0; i--){
        for(j=MAXPIANIMAG-1; j>=0; j--){
           if( m[num_mag]->Pos[i].Presenza[j]  ) MagTrovato=true;
           if( MagTrovato                      ) break;
        }
        if( MagTrovato ) break;
     }
  }
  PosizioneOccupata = i;
  PianoOccupato=j;

  if( PosizioneOccupata>m[num_mag]->Npt )  MagTrovato=false;
  //-------------------------------------------------
  // Verifica Posizione Deposito in base al tipo Presa
  //-------------------------------------------------
  PosizioneDest=0;
  PianoDest    =0;
  if( MagTrovato ){
     //-----------------------------
     // Forca A
     //-----------------------------
     if( TypPresa==N_FORCA_A ){
        #ifdef PREL_TERRA_SOLO_FORCA_B
           if( PianoOccupato<1 ) MagTrovato=false; // Possibile solo prelievo alto
        #endif
        PosizioneDest  =PosizioneOccupata;
        PianoDest      =PianoOccupato;
     }
     //-----------------------------
     // Forca B
     //-----------------------------
     if( TypPresa==N_FORCA_B ){
        PosizioneDest = PosizioneOccupata;
        PianoDest      =PianoOccupato;
        //--------------------------------------------------
        // GESTIONE DOPPIA FORCA REALE
        // Forca bassa ammette prelievo solo a TERRA
        //--------------------------------------------------
        if(GestioneDoppiaForcaTGV==1){
           if( PianoOccupato>0 ) MagTrovato=false; // Possibile solo prelievo basso
           PianoDest     = 0;
        }
     }
     //-----------------------------
     // Forca AB
     //-----------------------------
     if( TypPresa==N_FORCA_AB ){
        if( PianoOccupato==0 ) MagTrovato=false; // Possibile solo prelievo basso
        PosizioneDest  =PosizioneOccupata;
        PianoDest      =0;
     }
  }
  //--------------------------------------------
  // Controlli di range ammessi
  //--------------------------------------------
  if( PosizioneDest<1                       ) MagTrovato=false;
  if( PosizioneDest>m[num_mag]->Npt         ) MagTrovato=false;
  if( PianoDest<0                           ) MagTrovato=false;
  if( PianoDest>m[num_mag]->Sovrapposizione ) MagTrovato=false;

  //--------------------------------------------
  // Magazzino NON trovato
  //--------------------------------------------
  if( MagTrovato==0 ){
     *Posizione     =0;
     *PosizioneTgv  =0;
     *Piano         =0;
     return 1;
  }

  //--------------------------------------------
  // Magazzino TROVATO
  //--------------------------------------------
  *Piano         = PianoDest;
  *Posizione     = PosizioneDest;
  *PosizioneTgv  = PosizioneDest;
  if(prelievo_da_ing==false) *PosizioneTgv  = (m[num_mag]->Npt-PosizioneDest)+1;

  return 0;
}

// --------------------------------------------------------
//   CercaIDInMag()
// --------------------------------------------------------
//   Verifica se un'IDBOX compare tra quelli depositati
//   in qualche magazzino impianto
//
bool magaz::CercaIDInMag(short int ID, short int *num_mag, short int *num_pos, short int *num_pin)
/********************************************/
{
  short int i, j, magazzino;

  *num_mag=0;
  *num_pos=0;
  *num_pin=0;
  //----------------------------------------------------
  // Verifica validità ID BOX
  //----------------------------------------------------
  if(ID<1              ) return FALSE;
  if(ID>MAXBOXIMPIANTO ) return FALSE;

  //----------------------------------------------------
  // Scroll di tutte le macchine per verificare se l'ID
  // compare tra quelli richiesti dalla macchina
  //----------------------------------------------------
  for(magazzino=1; magazzino<=MAXMAG; magazzino++){
     if(m[magazzino]->Num<1              ) continue;
     if(m[magazzino]->Num>TOTMAG         ) continue;
     if(m[magazzino]->PuntoIng<1         ) continue;
     if(m[magazzino]->PuntoIng>TOTPUNTI  ) continue;
     if(m[magazzino]->PuntoOut<1         ) continue;
     if(m[magazzino]->PuntoOut>TOTPUNTI  ) continue;
     for(i=1; i<=MAXTERRAMAG; i++){
        for(j=0; j<MAXPIANIMAG; j++){
           if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
           if(m[magazzino]->Pos[i].Box[j].ID!=ID     ) continue;
           *num_mag=magazzino;
           *num_pos=i;
           *num_pin=j;
           return TRUE;
        }
     }
  }
  return FALSE;
}

// --------------------------------------------
//   CercaMagDepositoPIENO()
// --------------------------------------------
//  Restituisce il numero del magazzino più vicino con la possibilità
//  di deposito in base al tipo di box da depositare
//  TipoDeposito :   0 -> Mag pieno nessun deposito possibile.
//                   1 -> deposito a terra.
//                   2 -> deposito alto al 1° piano.
//                   3 -> deposito alto al 2° piano.
//
int magaz::CercaMagDepositoPIENO(bool libero, short int singolo_mag, bool abilita_controllo_nodo, bool deposito_nel_piu_vicino, bool lato_uscita, bool disp_sovrap, short int gruppo_dest, short int *num_mag, short int *num_pos, short int *num_pin, int provenienza, short int num_agv, short int typ_presa, short int tipo_carico, short int tipo_box, bool presa[POSINPRESA+1][PIANIINPRESA+1], short int codice, int lotto, short int car_wash, bool basso, bool rotaz, struct DATI_PRD *Udc, bool presa_sovrapponibile, char *all_mess)
/*************************************************************************/
{
  int chiamata, tt, err=0;
  int gestione_cambio_lotto=GESTIONE_CAMBIO_LOTTO;
  int gestione_cambio_turno=GESTIONE_CAMBIO_TURNO;
  bool udc_trovato;
  bool cambio_lotto;
  bool cambio_turno;
  bool cambio_codice;
  bool salta_pos;
  bool deposito_in_out;
  bool tgv_in_uscita;
  bool almeno_un_pallet_da_spostare;
  short int count_pos;
  short int Punto, Pos, Pin, magazzino;
  short int i, j, k, z, altro_tgv, limitrofo;
  short int altro_tgv_blocco_rot;
  short int Posizione, PosizioneTgv, Piano;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  short int old_dist=9999, new_dist, num_rot;
  short int tipo, num_mac, num_baia, num_mag1;
  long int OldTime;
  struct DATI_PRD UdcMag;
  short int altro_num_mag_sorg, altro_num_pos_sorg;
  short int altro_num_mag_dest, altro_num_pos_dest;

  err       = 0;
  *num_mag  = 0;
  *num_pos  = 0;
  *num_pin  = 0;
  new_dist  = 0;
  num_rot   = 0;
  altro_tgv = 0;
  limitrofo = 0;
  OldTime   = 0;
  strcpy( all_mess, "OK");
  memset(&nodi_perc,  0, sizeof(nodi_perc));
  memset(&dati_perc,  0, sizeof(dati_perc));
  //-------------------------------------------
  // Funzione non abilitata per i VUOTI
  //-------------------------------------------
  if(tipo_carico==PRES_VUOTO) return 0;
  //-------------------------------------------
  // Decodifica provenienza
  //-------------------------------------------
  tipo=num_mac=num_baia=num_mag1=0;
  P.TestProvenienzaBox(provenienza, &tipo, &num_mac, &num_baia, &num_mag1);
  //-------------------------------------------
  // Se non viene passato il TGV non può
  // essere fatto il deposito nel più vicino
  //-------------------------------------------
  if(num_agv<1 || num_agv>MAXAGV) deposito_nel_piu_vicino=0;
  //-------------------------------------------
  // Ricerca tra tutti i magazzini in elenco
  //-------------------------------------------
  for(magazzino=1; magazzino<=MAXMAG; magazzino++){
     altro_tgv=0;
     //------------------------------------------
     // Non ammesso scarico in mag di provenienza
     //------------------------------------------
     if(num_mag1==magazzino) continue;
     //------------------------------------------
     // Se richiesto magazzino specifico,
     // scarto a priori gli altri
     //------------------------------------------
     if(singolo_mag!=0 && m[magazzino]->Num!=singolo_mag) continue;
     //------------------------------------------
     // Cerca Magazzino compatibile
     //------------------------------------------
     if(m[magazzino]->Num<1              ) continue;
     if(m[magazzino]->Num>TOTMAG         ) continue;
     if(m[magazzino]->Npt==0             ) continue;
     if(m[magazzino]->PuntoIng==0        ) continue;
     if(m[magazzino]->PuntoIng>TOTPUNTI  ) continue;
     if(m[magazzino]->PuntoOut==0        ) continue;
     if(m[magazzino]->PuntoOut>TOTPUNTI  ) continue;
     if(m[magazzino]->Completo==true     ) continue;
     //------------------------------------------
     // SOLO MAGAZZINI DEPOSITO PIENI
     //------------------------------------------
     if(m[magazzino]->Tipo!=MG_PROD_TUTTI &&
        m[magazzino]->Tipo!=MG_PROD_TIPO1 &&
        m[magazzino]->Tipo!=MG_PROD_TIPO2) continue;
     //------------------------------------------
     // DISPONIBILITA' ALLA SOVRAPPOSIZIONE
     //------------------------------------------
     if(disp_sovrap==true && m[magazzino]->Sovrapposizione==0) continue;
     //------------------------------------------
     // Controllo gruppo appartenenza MAGAZZINO
     //------------------------------------------
     if(gruppo_dest!=NN && gruppo_dest!=m[magazzino]->Gruppo) continue;
     //------------------------------------------
     // Magazzino è abilitato all'EMERGENZA
     //------------------------------------------
     if(m[magazzino]->PerEmergenza!=NN){
        if(m[magazzino]->PerEmergenza==1 && car_wash==0) continue;
        if(m[magazzino]->PerEmergenza==2 && car_wash!=0) continue;
     }
     //------------------------------------------
     // GESTIONE ROTAZIONE PALLET
     //------------------------------------------
     if(m[magazzino]->PerPalRuotati!=NN){
        if(m[magazzino]->PerPalRuotati==1 && rotaz==false) continue;
        if(m[magazzino]->PerPalRuotati==2 && rotaz==true) continue;
     }
     //------------------------------------------
     // OBBLIGO_ASSEGNAZIONE_MANUALE_MAG
     // Scarto tutti i magazzini impostati
     // sull'assegnazione auto
     //------------------------------------------
     #ifdef OBBLIGO_ASSEGNAZIONE_MANUALE_MAG
        if(m[magazzino]->Assegnazione==ASS_DINAMICA) continue;
     #endif;
     //------------------------------------------
     // GESTIONE_TIPOBOX
     // Controllo del tipo assegnato al magazzino
     //------------------------------------------
     #ifdef GESTIONE_TIPOBOX
        if(tipo_box!=SEARCH_ALL_TIPOBOX && tipo_box!=m[magazzino]->TipoBox) continue;
     #endif;
     //------------------------------------------
     // Magazzino completamente libero
     // NOTA: con il controllo del magazzino
     //       specifico, non viene consideraro
     //       se incompleto o completo
     //------------------------------------------
     if(singolo_mag==0){
        if(libero==true  && m[magazzino]->Stato!=MG_ST_FREE) continue;
        if(libero==false && m[magazzino]->Stato==MG_ST_FREE) continue;
     }
     //------------------------------------------
     // Verifico coerenza tipo cestone
     //------------------------------------------
     if(tipo_carico==PRES_VUOTO && m[magazzino]->Tipo!=MG_VUOTI) continue;
     if(tipo_carico!=PRES_VUOTO && m[magazzino]->Tipo==MG_VUOTI) continue;
     //------------------------------------------
     // Verifico abilitazione alti/bassi
     //------------------------------------------
     if(m[magazzino]->Tipo!=MG_PROD_TUTTI){
        if(m[magazzino]->Tipo==MG_PROD_TIPO1 && basso==false   ) continue;
        if(m[magazzino]->Tipo==MG_PROD_TIPO2 && basso==true    ) continue;
     }
     //------------------------------------------
     // VERIFICA TIPO MAGAZZINO (FIFO/LIFO)
     // In caso di ricerca prelievo da lato
     // ingresso forzo il mag. tipo LIFO
     //------------------------------------------
     deposito_in_out = lato_uscita;
     if(m[magazzino]->Fifo==false) deposito_in_out=false;
     //------------------------------------------
     // MAGAZZINO LIBERO CON TIPO ASSENGAZIONE
     //------------------------------------------
     if(m[magazzino]->Assegnazione!=ASS_DINAMICA){
        if(m[magazzino]->Assegnazione!=tipo_carico) continue;
     }
     //------------------------------------------
     // Magazzinio già impegnato
     //------------------------------------------
     cambio_lotto=false;
     cambio_turno=false;
     if(m[magazzino]->Stato!=MG_ST_FREE){
        if(tipo_carico==ST_BOX_VUOTI && m[magazzino]->Stato!=MG_ST_VUOTI) continue;
        if(tipo_carico!=ST_BOX_VUOTI && m[magazzino]->Stato==MG_ST_VUOTI) continue;
        //------------------------------------------
        // Deposito nel magazzino più vecchio per
        // garantire cronologia prodotto
        //------------------------------------------
        if(m[magazzino]->Time<OldTime) continue;
        //------------------------------------------
        // Cerco il primo cestone presente
        //------------------------------------------
        Pos=99;
        Pin=99;
        // LIFO
        if(deposito_in_out==false){
           for(i=1; i<=MAXTERRAMAG; i++){
              for(j=MAXPIANIMAG-1; j>=0; j--){
                 if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
                 Pos=i;
                 Pin=j;
                 break;
              }
              if(Pos!=99) break;
           }
        }
        // FIFO
        else{
           for(i=MAXTERRAMAG; i>0; i--){
              for(j=MAXPIANIMAG-1; j>=0; j--){
                 if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
                 Pos=i;
                 Pin=j;
                 break;
              }
              if(Pos!=99) break;
           }
        }
        if(Pos==99) continue;      // nessuna posizione impegnata
        //-----------------------------------------------------------------
        // Se il magazzino è LIFO e il primo BOX in prelievo è in fase di
        // spostamento, non è ammesso continuare il riempimento
        //-----------------------------------------------------------------
        almeno_un_pallet_da_spostare=false;
        for(i=MAXTERRAMAG; i>0; i--){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
              //--------------------------------------------------------
              // spostamento con gruppo destinazione
              //--------------------------------------------------------
              if(m[magazzino]->Pos[i].Box[j].GruppoDest==NN                  ) continue;
              if(m[magazzino]->Pos[i].Box[j].GruppoDest==m[magazzino]->Gruppo) continue;
              almeno_un_pallet_da_spostare=true;
              break;
           }
           if(almeno_un_pallet_da_spostare) break;
        }
        if(almeno_un_pallet_da_spostare==true) continue;
        //-----------------------------------------------------------------
        // GESTIONE UDC
        // Memorizzo i dati di uno degli Udc in presa che dovranno essere
        // tutti uguali
        //-----------------------------------------------------------------
        udc_trovato=false;
        memset(&UdcMag, 0, sizeof(DATI_PRD));
        if(GestioneUdc!=0){
           for(i=0; i<POSINPRESA; i++){
              for(j=0; j<PIANIINPRESA; j++){
                 if(m[magazzino]->Pos[Pos].Box[Pin].Presa[i][j]==false) continue;
                 //-------------------------------------------
                 // Verifico se UDC duplicato
                 //-------------------------------------------
                 k=i+(j*POSINPRESA);
                 memcpy(&UdcMag, &m[magazzino]->Pos[Pos].Box[Pin].Prd[k], sizeof(DATI_PRD));
                 udc_trovato=true;
                 break;
              }
              if(udc_trovato==true) break;
           }
        }
        //-----------------------------------------------------------------
        // GESTIONE_DINAMICA_MAG
        // Non controllo del tipo di supporto
        //-----------------------------------------------------------------
        if(GestioneDinamicaMagazzini==0){
           if(tipo_box!=SEARCH_ALL_TIPOBOX && tipo_box!=m[magazzino]->Pos[Pos].Box[Pin].TipoBox) continue;
        }
        //-----------------------------------------------------------------
        // CONTROLLI RISERVATI AI BOX PIENI CRUDI
        // 1 - Verifica dati postazione solo per i PIENI
        // 2 - Scarto tutti i magazzini anche incompleti al cui interno
        //     sia stato raggiunto l'Over Time su tutte le posizioni
        //-----------------------------------------------------------------
        if(m[magazzino]->Assegnazione==ASS_DINAMICA){
           if(tipo_carico!=PRES_VUOTO && m[magazzino]->Pos[Pos].Box[Pin].StatoLav!=tipo_carico) continue;
        }
        //-----------------------------------------------------------------
        // GESTIONE CAR-WASH
        // Se a magazzino ci sono già dei pallet con CAR-WASH verifico
        // che il valore sia lo stesso per gestire file con tipi di
        // CAR-WASH diversi
        //-----------------------------------------------------------------
        if(m[magazzino]->PerEmergenza==1){
           if(m[magazzino]->Pos[Pos].Box[Pin].CarWash!=car_wash) continue;
        }
        //-----------------------------------------------------------------
        // Salto posizione a terra con cambio lotto
        // 1. controllo cambio lotto con dati PLC.
        // 2. controllo anche il cambio lotto e codice a UDC
        //    SE GESTIONE ATTIVA E OBBLIGATA
        //-----------------------------------------------------------------
        cambio_lotto=false;
        cambio_turno=false;
        // 1. cambio lotto PLC
        if(m[magazzino]->Pos[Pos].Box[Pin].Lotto!=lotto) cambio_lotto=true;
        // 2. cambio lotto UDC
        if(GestioneUdc!=0){
           if(strcmp(Udc->code,  UdcMag.code)!=0 ) continue;
           if(strcmp(Udc->batch, UdcMag.batch)!=0) cambio_lotto=true;
           if(strcmp(Udc->shift, UdcMag.shift)!=0) cambio_turno=true;
        }
        //----------------------------------------------------------------------
        // Andrea 16/02/2018
        // Se Carwash allora considera solo il Codice ma non Lotto, Batch, Udc
        //----------------------------------------------------------------------
        if( cambio_lotto==true && car_wash==true ){
           cambio_lotto=false;
        }
        if( cambio_turno==true && car_wash==true ){     //### Andrea 14/9
           cambio_turno=false;
        }
        //-----------------------------------------------------------------
        // Verifico come gestire il cambio lotto
        // 0 - controllo disattivato
        // 1 - salto fila
        // 2 - salto posizione
        //-----------------------------------------------------------------
        if(gestione_cambio_lotto==0 && cambio_lotto==true) cambio_lotto=false;
        if(gestione_cambio_lotto==1 && cambio_lotto==true) continue;
        //-----------------------------------------------------------------
        // ### Andrea 14/9
        // Verifico come gestire il cambio turno
        // 0 - controllo disattivato
        // 1 - salto fila
        // 2 - salto posizione
        //-----------------------------------------------------------------
        if(gestione_cambio_turno==0 && cambio_turno==true) cambio_turno=false;
        if(gestione_cambio_turno==1 && cambio_turno==true) continue;
     }
     //------------------------------------------------------------
     // IN CASO DI DEPOSITO CONSIDERO IL PUNTO DI INGRESSO
     //------------------------------------------------------------
     if(deposito_in_out==false) Punto = m[magazzino]->PuntoIng;
     if(deposito_in_out==true ) Punto = m[magazzino]->PuntoOut;
     //------------------------------------------------------------
     // Controllo stato del nodo
     //------------------------------------------------------------
     new_dist=num_rot=0;
     if(num_agv>0 && num_agv<=MAXAGV){
        //-----------------------------------------------------------------
        // Composizione chiamata abbinata a magazzino
        //-----------------------------------------------------------------
        chiamata=0;
        P.TestNumChiamata(&chiamata, Punto, magazzino);
        if(chiamata==0) continue;     // chiamata sconosciuta
        //-----------------------------------------------------------------
        // verifica se la chiamata è già asservita da un AGV e l'altro AGV
        // è in missione attiva.
        //-----------------------------------------------------------------
        altro_tgv = (short int)(P.test_chiamata_asservita(chiamata));
        if( altro_tgv!=0 && altro_tgv!=num_agv ) continue;
        //--------------------------------------------------------------
        // Recupero la distanza dalla destinazione
        //--------------------------------------------------------------
        memset(&nodi_perc, 0, sizeof(nodi_perc));
        memset(&dati_perc, 0, sizeof(dati_perc));
        err = AGV[num_agv]->distanza(FAST_PATH, 0, Punto, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
        if(err || new_dist>MAXPERCORSI) continue;
        //-----------------------------------------------------------------
        // Controllo occupazione nodo
        //-----------------------------------------------------------------
        if(abilita_controllo_nodo==true){
           //------------------------------------------------------------
           // Gestione NODO CONTRAPPOSTO
           //------------------------------------------------------------
           if(N.nodo_contrapposto[Punto]!=0 && N.nodo_busy[N.nodo_contrapposto[Punto]] && N.nodo_busy[N.nodo_contrapposto[Punto]]!=num_agv) continue;
           //------------------------------------------------------------
           // GESTIONE BLOCCHI ROTAZIONE
           // Scarto i magazzini interbloccati da altri
           //------------------------------------------------------------
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
           if(altro_tgv_blocco_rot!=0) continue;
           //-----------------------------------------------------------
           // Controllo se magazzino già impeganto da altro TGV
           //-----------------------------------------------------------
           if(P.test_dest_occupata(Punto, &altro_tgv) && altro_tgv!=0 && altro_tgv!=num_agv){
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
              if(altro_num_mag_sorg==m[magazzino]->Num && altro_num_mag_dest!=m[magazzino]->Num) tgv_in_uscita=true;
              if( tgv_in_uscita==false ) continue;   //###
           }
           //-----------------------------------------------------------
           // Destinazione riservata da altro TGV
           //-----------------------------------------------------------
           if(P.test_dest_riservata(Punto, &altro_tgv, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv ){
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
              if(altro_num_mag_sorg==m[magazzino]->Num && altro_num_mag_dest!=m[magazzino]->Num) tgv_in_uscita=true;
              if( tgv_in_uscita==false ) continue;   //###
           }
           //-----------------------------------------------------------
           // TGV su nodo limitrofo
           //-----------------------------------------------------------
           if(P.test_dest_limitrofe(Punto, &altro_tgv, &limitrofo, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv ) continue;
        }
     }
     //-----------------------------------------------------------
     // Verifica del Tipo di Deposito possibile
     // ### Andrea 14/9 : aggiunto cambio_turno
     //-----------------------------------------------------------
     Posizione=PosizioneTgv=Piano=0;
     salta_pos = cambio_lotto | cambio_turno;
   //M.VerificaTipoDeposito(deposito_in_out, m[magazzino]->Num, 0, typ_presa, tipo_box, presa, presa_sovrapponibile, basso, cambio_lotto, &Posizione, &Piano, &PosizioneTgv, all_mess);
     M.VerificaTipoDeposito(deposito_in_out, m[magazzino]->Num, 0, typ_presa, tipo_box, presa, presa_sovrapponibile, basso, salta_pos, &Posizione, &Piano, &PosizioneTgv, all_mess);

     //-----------------------------------------------------------
     // Se TipoDeposito=0 nessun deposito possibile
     //-----------------------------------------------------------
     if(Posizione==0) continue;
     //---------------------------------------------------------------------
     // Andrea 02/05/2018
     // Se Cerco Mag.non libero allora cerca Mag più vecchio
     //---------------------------------------------------------------------
     if(libero==false){
        OldTime = m[magazzino]->Time;
        *num_mag = m[magazzino]->Num;
        *num_pos = Posizione;
        *num_pin = Piano;
     }
     else{
        //-----------------------------------
        // Cerca magazzino più vicino
        //-----------------------------------
        OldTime = m[magazzino]->Time;
        if(new_dist < old_dist){
           old_dist = new_dist;
           *num_mag = m[magazzino]->Num;
           *num_pos = Posizione;
           *num_pin = Piano;
           //---------------------------------------------
           // DEPOSITO_NEL_PIU_VICINO
           // Se è disattivato, il primo trovato va bene
           //---------------------------------------------
           if(deposito_nel_piu_vicino==0) return 0;
        }
     }
     //#############
  }
  return err;
}

// --------------------------------------------
//   CercaMagTempDeposito()
// --------------------------------------------
//  Restituisce il numero del magazzino più vicino con la possibilità
//  di deposito in base al tipo di box da depositare
//  TipoDeposito :   0 -> Mag pieno nessun deposito possibile.
//                   1 -> deposito a terra.
//                   2 -> deposito alto al 1° piano.
//                   3 -> deposito alto al 2° piano.
//
int magaz::CercaMagTempDeposito(bool libero, short int singolo_mag, bool abilita_controllo_nodo, bool lato_uscita, bool disp_sovrap, short int gruppo_dest, short int *num_mag, short int *num_pos, short int *num_pin, int provenienza, short int num_agv, short int typ_presa, short int tipo_carico, short int tipo_box, bool presa[POSINPRESA+1][PIANIINPRESA+1], short int codice, int lotto, short int car_wash, bool basso, bool rotaz, struct DATI_PRD *Udc, bool presa_sovrapponibile, char *all_mess)
/*************************************************************************/
{
  int chiamata, tt, err=0;
  int deposito_nel_piu_vicino=DEPOSITO_NEL_PIU_VICINO;
  bool udc_trovato;
  bool salta_pos;
  bool deposito_in_out;
  bool tgv_in_uscita;
  short int count_pos;
  short int Punto, Pos, Pin, magazzino;
  short int i, j, k, z, altro_tgv, limitrofo;
  short int altro_tgv_blocco_rot;
  short int Posizione, PosizioneTgv, Piano;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  short int old_dist=9999, new_dist, num_rot;
  short int tipo, num_mac, num_baia, num_mag1;
  long int OldTime;
  struct DATI_PRD UdcMag;
  int controlla_assegnazione_su_spost = CONTROLLO_ASSEGNAZIONE_SPOSTAMENTO;
  short int altro_num_mag_sorg, altro_num_pos_sorg;
  short int altro_num_mag_dest, altro_num_pos_dest;

  err       = 0;
  *num_mag  = 0;
  *num_pos  = 0;
  *num_pin  = 0;
  new_dist  = 0;
  num_rot   = 0;
  altro_tgv = 0;
  limitrofo = 0;
  strcpy( all_mess, "OK");
  OldTime = time(NULL) + 63244800L;          // Forza al valore di oggi + 2ANNO  memset(&elenco_mag, 0, sizeof(elenco_mag));
  memset(&nodi_perc,  0, sizeof(nodi_perc));
  memset(&dati_perc,  0, sizeof(dati_perc));
  //-------------------------------------------
  // Funzione non abilitata per i VUOTI
  //-------------------------------------------
  if(tipo_carico==PRES_VUOTO) return 0;
  //-------------------------------------------
  // Decodifica provenienza
  //-------------------------------------------
  tipo=num_mac=num_baia=num_mag1=0;
  P.TestProvenienzaBox(provenienza, &tipo, &num_mac, &num_baia, &num_mag1);
  //-------------------------------------------
  // Se non viene passato il TGV non può
  // essere fatto il deposito nel più vicino
  //-------------------------------------------
  if(num_agv<1 || num_agv>MAXAGV) deposito_nel_piu_vicino=0;
  //-------------------------------------------
  // Ricerca tra tutti i magazzini in elenco
  //-------------------------------------------
  for(magazzino=1; magazzino<=MAXMAG; magazzino++){
     altro_tgv=0;
     //------------------------------------------
     // Non ammesso scarico in mag di provenienza
     //------------------------------------------
     if(num_mag1==magazzino) continue;
     //------------------------------------------
     // Se richiesto magazzino specifico,
     // scarto a priori gli altri
     //------------------------------------------
     if(singolo_mag!=0 && m[magazzino]->Num!=singolo_mag) continue;
     //------------------------------------------
     // Cerca Magazzino compatibile
     //------------------------------------------
     if(m[magazzino]->Num<1              ) continue;
     if(m[magazzino]->Num>TOTMAG         ) continue;
     if(m[magazzino]->Npt==0             ) continue;
     if(m[magazzino]->PuntoIng==0        ) continue;
     if(m[magazzino]->PuntoIng>TOTPUNTI  ) continue;
     if(m[magazzino]->PuntoOut==0        ) continue;
     if(m[magazzino]->PuntoOut>TOTPUNTI  ) continue;
     if(m[magazzino]->Completo==true     ) continue;
     //------------------------------------------
     // SOLO MAGAZZINI TEMPORANEI
     //------------------------------------------
     if(m[magazzino]->Tipo!=MG_TEMPORANEO ) continue;
     //------------------------------------------
     // DISPONIBILITA' ALLA SOVRAPPOSIZIONE
     //------------------------------------------
     if(disp_sovrap==true && m[magazzino]->Sovrapposizione==0) continue;
     //------------------------------------------
     // Controllo gruppo appartenenza MAGAZZINO
     //------------------------------------------
     if(gruppo_dest!=NN && gruppo_dest!=m[magazzino]->Gruppo) continue;
     //------------------------------------------
     // GESTIONE ROTAZIONE PALLET
     //------------------------------------------
     if(m[magazzino]->PerPalRuotati!=NN){
        if(m[magazzino]->PerPalRuotati==1 && rotaz==false) continue;
        if(m[magazzino]->PerPalRuotati==2 && rotaz==true) continue;
     }
     //------------------------------------------
     // GESTIONE_TIPOBOX
     // Controllo del tipo assegnato al magazzino
     //------------------------------------------
     #ifdef GESTIONE_TIPOBOX
        if(tipo_box!=SEARCH_ALL_TIPOBOX && tipo_box!=m[magazzino]->TipoBox) continue;
     #endif;
     //------------------------------------------
     // Magazzino completamente libero
     // NOTA: con il controllo del magazzino
     //       specifico, non viene consideraro
     //       se incompleto o completo
     //------------------------------------------
     if(singolo_mag==0){
        if(libero==true  && m[magazzino]->Stato!=MG_ST_FREE) continue;
        if(libero==false && m[magazzino]->Stato==MG_ST_FREE) continue;
     }
     //------------------------------------------
     // VERIFICA TIPO MAGAZZINO (FIFO/LIFO)
     // In caso di ricerca prelievo da lato
     // ingresso forzo il mag. tipo LIFO
     //------------------------------------------
     deposito_in_out = lato_uscita;
     if(m[magazzino]->Fifo==false) deposito_in_out=false;
     //------------------------------------------
     // Controlla assegnazione su spostamento
     //------------------------------------------
     if( controlla_assegnazione_su_spost==1 ){
        if(tipo_carico!=NN && m[magazzino]->Assegnazione!=NN && tipo_carico!=m[magazzino]->Assegnazione ) continue;
     }
     //------------------------------------------
     // Magazzinio già impegnato
     //------------------------------------------
     if(m[magazzino]->Stato!=MG_ST_FREE){
        if(tipo_carico==ST_BOX_VUOTI && m[magazzino]->Stato!=MG_ST_VUOTI) continue;
        if(tipo_carico!=ST_BOX_VUOTI && m[magazzino]->Stato==MG_ST_VUOTI) continue;
        //------------------------------------------
        // Deposito nel magazzino più vecchio per
        // garantire cronologia prodotto
        //------------------------------------------
        if(m[magazzino]->Time>OldTime) continue;
        //------------------------------------------
        // Cerco il primo cestone presente
        //------------------------------------------
        Pos=99;
        Pin=99;
        // LIFO
        if(deposito_in_out==false){
           for(i=1; i<=MAXTERRAMAG; i++){
              for(j=MAXPIANIMAG-1; j>=0; j--){
                 if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
                 Pos=i;
                 Pin=j;
                 break;
              }
              if(Pos!=99) break;
           }
        }
        // FIFO
        else{
           for(i=MAXTERRAMAG; i>0; i--){
              for(j=MAXPIANIMAG-1; j>=0; j--){
                 if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
                 Pos=i;
                 Pin=j;
                 break;
              }
              if(Pos!=99) break;
           }
        }
        if(Pos==99) continue;      // nessuna posizione impegnata
        //-----------------------------------------------------------------
        // GESTIONE UDC
        // Memorizzo i dati di uno degli Udc in presa che dovranno essere
        // tutti uguali
        //-----------------------------------------------------------------
        udc_trovato=false;
        memset(&UdcMag, 0, sizeof(DATI_PRD));
        if(GestioneUdc!=0){
           for(i=0; i<POSINPRESA; i++){
              for(j=0; j<PIANIINPRESA; j++){
                 if(m[magazzino]->Pos[Pos].Box[Pin].Presa[i][j]==false) continue;
                 //-------------------------------------------
                 // Verifico se UDC duplicato
                 //-------------------------------------------
                 k=i+(j*POSINPRESA);
                 memcpy(&UdcMag, &m[magazzino]->Pos[Pos].Box[Pin].Prd[k], sizeof(DATI_PRD));
                 udc_trovato=true;
                 break;
              }
              if(udc_trovato==true) break;
           }
        }
        //-----------------------------------------------------------------
        // GESTIONE_DINAMICA_MAG
        // Non controllo del tipo di supporto
        //-----------------------------------------------------------------
        if(GestioneDinamicaMagazzini==0){
           if(tipo_box!=SEARCH_ALL_TIPOBOX && tipo_box!=m[magazzino]->Pos[Pos].Box[Pin].TipoBox) continue;
        }
        //-----------------------------------------------------------------
        // CONTROLLI RISERVATI AI BOX PIENI CRUDI
        // 1 - Verifica dati postazione solo per i PIENI
        // 2 - Scarto tutti i magazzini anche incompleti al cui interno
        //     sia stato raggiunto l'Over Time su tutte le posizioni
        //-----------------------------------------------------------------
        if(m[magazzino]->Assegnazione==ASS_DINAMICA){
           if(tipo_carico!=PRES_VUOTO && m[magazzino]->Pos[Pos].Box[Pin].StatoLav!=tipo_carico) continue;
        }
     }
     //------------------------------------------------------------
     // IN CASO DI DEPOSITO CONSIDERO IL PUNTO DI INGRESSO
     //------------------------------------------------------------
     if(deposito_in_out==false) Punto = m[magazzino]->PuntoIng;
     if(deposito_in_out==true ) Punto = m[magazzino]->PuntoOut;
     //------------------------------------------------------------
     // Controllo stato del nodo
     //------------------------------------------------------------
     new_dist=num_rot=0;
     if(num_agv>0 && num_agv<=MAXAGV){
        //-----------------------------------------------------------------
        // Composizione chiamata abbinata a magazzino
        //-----------------------------------------------------------------
        chiamata=0;
        P.TestNumChiamata(&chiamata, Punto, magazzino);
        if(chiamata==0) continue;     // chiamata sconosciuta
        //-----------------------------------------------------------------
        // verifica se la chiamata è già asservita da un AGV e l'altro AGV
        // è in missione attiva.
        //-----------------------------------------------------------------
        altro_tgv = (short int)(P.test_chiamata_asservita(chiamata));
        if( altro_tgv!=0 && altro_tgv!=num_agv ) continue;
        //-----------------------------------------------------------------
        // Controllo stato occupazione nodo
        //-----------------------------------------------------------------
        if(abilita_controllo_nodo==true){
           //------------------------------------------------------------
           // Gestione NODO CONTRAPPOSTO
           //------------------------------------------------------------
           if(N.nodo_contrapposto[Punto]!=0 && N.nodo_busy[N.nodo_contrapposto[Punto]] && N.nodo_busy[N.nodo_contrapposto[Punto]]!=num_agv) continue;
           //--------------------------------------------------------------
           // Recupero la distanza dalla destinazione
           //--------------------------------------------------------------
           memset(&nodi_perc, 0, sizeof(nodi_perc));
           memset(&dati_perc, 0, sizeof(dati_perc));
           err = AGV[num_agv]->distanza(FAST_PATH, 0, Punto, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
           if(err || new_dist>MAXPERCORSI) continue;
           //------------------------------------------------------------
           // GESTIONE BLOCCHI ROTAZIONE
           // Scarto i magazzini interbloccati da altri
           //------------------------------------------------------------
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
           if(altro_tgv_blocco_rot!=0) continue;
           //-----------------------------------------------------------
           // Controllo se magazzino già impeganto da altro TGV
           //-----------------------------------------------------------
           if(P.test_dest_occupata(Punto, &altro_tgv) && altro_tgv!=0 && altro_tgv!=num_agv){
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
              if(altro_num_mag_sorg==m[magazzino]->Num && altro_num_mag_dest!=m[magazzino]->Num) tgv_in_uscita=true;
              if( tgv_in_uscita==false ) continue;   //###
           }
           //-----------------------------------------------------------
           // Destinazione riservata da altro TGV
           //-----------------------------------------------------------
           if(P.test_dest_riservata(Punto, &altro_tgv, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv ){
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
              if(altro_num_mag_sorg==m[magazzino]->Num && altro_num_mag_dest!=m[magazzino]->Num) tgv_in_uscita=true;
              if( tgv_in_uscita==false ) continue;   //###
           }
           //-----------------------------------------------------------
           // TGV su nodo limitrofo
           //-----------------------------------------------------------
           if(P.test_dest_limitrofe(Punto, &altro_tgv, &limitrofo, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv ) continue;
        }
     }
     //-----------------------------------------------------------
     // Verifica del Tipo di Deposito possibile
     // ### Andrea 14/9 : aggiunto cambio_turno
     //-----------------------------------------------------------
     Posizione=PosizioneTgv=Piano=0;
     M.VerificaTipoDeposito(deposito_in_out, m[magazzino]->Num, 0, typ_presa, tipo_box, presa, presa_sovrapponibile, basso, FALSE, &Posizione, &Piano, &PosizioneTgv, all_mess);
     //-----------------------------------------------------------
     // Se TipoDeposito=0 nessun deposito possibile
     //-----------------------------------------------------------
     if(Posizione==0) continue;
     //---------------------------------------------------------------------
     // Andrea 02/05/2018
     // Se Cerco Mag.non libero allora cerca Mag più vecchio
     //---------------------------------------------------------------------
     if(libero==false){
        OldTime = m[magazzino]->Time;
        *num_mag = m[magazzino]->Num;
        *num_pos = Posizione;
        *num_pin = Piano;
     }
     else{
        //-----------------------------------
        // Cerca magazzino più vicino
        //-----------------------------------
        OldTime = m[magazzino]->Time;
        if(new_dist < old_dist){
           old_dist = new_dist;
           *num_mag = m[magazzino]->Num;
           *num_pos = Posizione;
           *num_pin = Piano;
           //---------------------------------------------
           // DEPOSITO_NEL_PIU_VICINO
           // Se è disattivato, il primo trovato va bene
           //---------------------------------------------
           if(deposito_nel_piu_vicino==0) return 0;
        }
     }
  }
  return err;
}

// --------------------------------------------
//   CercaMagPrelievoPIENO()
// --------------------------------------------
//  Restituisce il numero del magazzino più vicino con la possibilità
//  di deposito in base al tipo di box da depositare
//  TipoPrelievo :   0 -> Mag pieno nessun deposito possibile.
//                   1 -> deposito a terra.
//                   2 -> deposito alto al 1° piano.
//                   3 -> deposito alto al 2° piano.
//
int magaz::CercaMagPrelievoPIENO(bool incompleto, short int singolo_mag, bool abilita_controllo_nodo, bool lato_ingresso, short int gruppo_dest, short int *num_mag, short int *num_pos, short int *num_pin, short int num_agv, short int typ_presa, short int gruppo_baia, char *all_mess)
/*************************************************************************/
{
  int  chiamata, err;
  bool mag_pieno;
  bool tgv_in_uscita;
  bool prelievo_da_out;
  bool abilita_svuotamento;
  long int OldTime;
  short int Capienza_Mag;
  short int Punto, Pos, Pin, magazzino;
  short int i, j, k, z, altro_tgv, limitrofo;
  short int altro_tgv_blocco_rot;
  short int Posizione, PosizioneTgv, Piano;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  short int new_dist, num_rot;
  short int tipo, num_mac, num_baia, num_mag1;
  short int altro_num_mag_sorg, altro_num_pos_sorg;
  short int altro_num_mag_dest, altro_num_pos_dest;

  err        = 0;
  new_dist   = 0;
  *num_mag   = 0;
  *num_pos   = 0;
  *num_pin   = 0;
  altro_tgv  = 0;
  limitrofo  = 0;
  strcpy( all_mess, "OK");
  OldTime = time(NULL) + 63072000L;    // time attuale + 2 ANNI
  memset(&nodi_perc, 0, sizeof(nodi_perc));
  memset(&dati_perc, 0, sizeof(dati_perc));
  //-------------------------------------------
  // Ricerca tra tutti i magazzini in elenco
  //-------------------------------------------
  for(magazzino=1; magazzino<=MAXMAG; magazzino++){
     altro_tgv=0;
     //------------------------------------------
     // Cerca Magazzino compatibile
     //------------------------------------------
     if(m[magazzino]->Num<1               ) continue;
     if(m[magazzino]->Num>TOTMAG          ) continue;
     if(m[magazzino]->Npt==0              ) continue;
     if(m[magazzino]->NumCestoni==0       ) continue;
     if(m[magazzino]->PuntoIng==0         ) continue;
     if(m[magazzino]->PuntoIng>TOTPUNTI   ) continue;
     if(m[magazzino]->PuntoOut==0         ) continue;
     if(m[magazzino]->PuntoOut>TOTPUNTI   ) continue;
     if(m[magazzino]->Stato!=MG_ST_PIENI  ) continue;
     //------------------------------------------
     // SOLO MAGAZZINI DEPOSITO PIENI
     //------------------------------------------
     if(m[magazzino]->Tipo!=MG_PROD_TUTTI &&
        m[magazzino]->Tipo!=MG_PROD_TIPO1 &&
        m[magazzino]->Tipo!=MG_PROD_TIPO2) continue;
     //------------------------------------------
     // Se richiesto magazzino specifico,
     // scarto a priori gli altri
     //------------------------------------------
     if(singolo_mag!=0 && m[magazzino]->Num!=singolo_mag) continue;
     //--------------------------------------------------------------
     // GESTIONE INCOMPLETI
     // Andrea L.
     // In realtà il Mag. resta completo finchè non è vuoto percui
     // devo controllare l'effettiva quantità di pallet presenti
     //--------------------------------------------------------------
     // Determina se il Mag è realmente completo
     //--------------------------------------------------------------
     mag_pieno=false;
     Capienza_Mag = m[magazzino]->Npt*(m[magazzino]->Sovrapposizione+1);
     if(m[magazzino]->Npt!=0 && m[magazzino]->NumCestoni>=Capienza_Mag){
        mag_pieno=true;
     }
     if(singolo_mag==0 && incompleto==true && mag_pieno==true) continue;
     //------------------------------------------
     // Gestione gruppo di appartenenza
     //------------------------------------------
     if(gruppo_dest!=NN && gruppo_dest!=m[magazzino]->Gruppo) continue;
     //------------------------------------------
     // VERIFICA TIPO MAGAZZINO (FIFO/LIFO)
     // In caso di ricerca prelievo da lato
     // ingresso forzo il mag. tipo LIFO
     //------------------------------------------
     prelievo_da_out = m[magazzino]->Fifo;
     if(lato_ingresso==true) prelievo_da_out=false;
     //------------------------------------------
     // Cerco il primo cestone presente
     //------------------------------------------
     Pos=99;
     Pin=99;
     // LIFO
     if(prelievo_da_out==false){
        for(i=1; i<=MAXTERRAMAG; i++){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
              Pos=i;
              Pin=j;
              break;
           }
           if(Pos!=99) break;
        }
     }
     // FIFO
     else{
        for(i=MAXTERRAMAG; i>0; i--){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
              Pos=i;
              Pin=j;
              break;
           }
           if(Pos!=99) break;
        }
     }
     if(Pos==99) continue;      // nessuna posizione impegnata
     //-----------------------------------------------------------------
     // IN CASO DI PRELIEVO CONSIDERO IL PUNTO DI USCITA
     // (tranne che per magazzini Lifo)
     //-----------------------------------------------------------------
     if(prelievo_da_out==true) Punto = m[magazzino]->PuntoOut;
     else                      Punto = m[magazzino]->PuntoIng;
     //-----------------------------------------------------------------
     // Non ammessi vuoti
     //-----------------------------------------------------------------
     if(m[magazzino]->Pos[Pos].Box[Pin].StatoLav==ST_BOX_VUOTI            ) continue;
     //-----------------------------------------------------------------
     // Il gruppo di destinazione del box deve essere quello baia
     //-----------------------------------------------------------------
     if(m[magazzino]->Pos[Pos].Box[Pin].GruppoDest==NN                    ) continue;
     if(m[magazzino]->Pos[Pos].Box[Pin].GruppoDest==m[magazzino]->Gruppo  ) continue;
     if(m[magazzino]->Pos[Pos].Box[Pin].GruppoDest!=gruppo_baia           ) continue;
     //-----------------------------------------------------------------
     // CERCO IL MAGAZZINO CON PALLET DA CONSEGNARE PIU' VECCHIO
     //-----------------------------------------------------------------
     if(m[magazzino]->Pos[Pos].Box[Pin].TimeS>OldTime) continue;
     //-----------------------------------------------------------------
     // Memorizzo il magazzino più vecchio indipendentemente dal fatto
     // che sia raggiungibile, perchè l'anzianità è vincolante.
     //-----------------------------------------------------------------
     OldTime=m[magazzino]->Pos[Pos].Box[Pin].TimeS;
     //-----------------------------------------------------------------
     // Controlli nodo disponibile solo con TGV VALIDO
     //-----------------------------------------------------------------
     if(num_agv>0 && num_agv<=MAXAGV){
        //-----------------------------------------------------------------
        // Composizione chiamata abbinata a magazzino
        //-----------------------------------------------------------------
        chiamata=0;
        P.TestNumChiamata(&chiamata, Punto, magazzino);
        if(chiamata==0) continue;     // chiamata sconosciuta
        //-----------------------------------------------------------------
        // verifica se la chiamata è già asservita da un AGV e l'altro AGV
        // è in missione attiva.
        //-----------------------------------------------------------------
        altro_tgv = (short int)(P.test_chiamata_asservita(chiamata));
        if( altro_tgv!=0 && altro_tgv!=num_agv ) continue;
        //-----------------------------------------------------------------
        // Controllo occupazione nodo
        //-----------------------------------------------------------------
        if(abilita_controllo_nodo==true){
           //-----------------------------------------------------------------
           // Gestione NODO CONTRAPPOSTO
           //-----------------------------------------------------------------
           if(N.nodo_contrapposto[Punto]!=0 && N.nodo_busy[N.nodo_contrapposto[Punto]] && N.nodo_busy[N.nodo_contrapposto[Punto]]!=num_agv) continue;
           //-----------------------------------------------------------------
           // Recupero la distanza dalla destinazione
           //-----------------------------------------------------------------
           memset(&nodi_perc, 0, sizeof(nodi_perc));
           memset(&dati_perc, 0, sizeof(dati_perc));
           err = AGV[num_agv]->distanza(FAST_PATH, 0, Punto, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
           if(err || new_dist>MAXPERCORSI) continue;
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
           if(altro_tgv_blocco_rot!=0) continue;
           //-----------------------------------------------------------
           // Controllo se magazzino già impeganto da altro TGV
           //-----------------------------------------------------------
           if(P.test_dest_occupata(Punto, &altro_tgv) && altro_tgv!=0 && altro_tgv!=num_agv){
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
              if(altro_num_mag_sorg==m[magazzino]->Num && altro_num_mag_dest!=m[magazzino]->Num) tgv_in_uscita=true;
              if( tgv_in_uscita==false ) continue;   //###
           }
           //-----------------------------------------------------------
           // Destinazione riservata da altro TGV
           //-----------------------------------------------------------
           if(P.test_dest_riservata(Punto, &altro_tgv, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv){
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
              if(altro_num_mag_sorg==m[magazzino]->Num && altro_num_mag_dest!=m[magazzino]->Num) tgv_in_uscita=true;
              if( tgv_in_uscita==false ) continue;   //###
           }
           //-----------------------------------------------------------
           // TGV su nodo limitrofo
           //-----------------------------------------------------------
           if(P.test_dest_limitrofe(Punto, &altro_tgv, &limitrofo, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv ) continue;
        }
     }
     //-----------------------------------------------------------
     // Verifica del Tipo di Deposito possibile
     //-----------------------------------------------------------
     Posizione=PosizioneTgv=Piano=0;
     if(VerificaTipoPrelievo(lato_ingresso, m[magazzino]->Num, typ_presa, &Posizione, &Piano, &PosizioneTgv, all_mess)) continue;
     //-----------------------------------------------------------
     // Se TipoDeposito=0 nessun deposito possibile
     //-----------------------------------------------------------
     if(Posizione==0) continue;
     //-----------------------------------------------------------
     // Cerca magazzino più vicino
     //-----------------------------------------------------------
     *num_mag = m[magazzino]->Num;
     *num_pos = Posizione;
     *num_pin = Piano;
  }
  return err;
}

// --------------------------------------------
//   CercaMagPrelievoVUOTO()
// --------------------------------------------
//  Restituisce il numero del magazzino più vicino con la possibilità
//  di deposito in base al tipo di box da depositare
//  TipoPrelievo :   0 -> Mag pieno nessun deposito possibile.
//                   1 -> deposito a terra.
//                   2 -> deposito alto al 1° piano.
//                   3 -> deposito alto al 2° piano.
//
int magaz::CercaMagPrelievoVUOTO(bool incompleto, short int singolo_mag, bool abilita_controllo_nodo, bool lato_ingresso, short int gruppo_dest, short int *num_mag, short int *num_pos, short int *num_pin, short int num_agv, short int typ_presa, short int tipo_carico, short int tipo_box, char *all_mess)
/*************************************************************************/
{
  int  chiamata, err;
  bool tgv_in_uscita;
  bool prelievo_da_out;
  bool abilita_svuotamento;
  short int DiffBox, OldDiffBox, Capienza_Mag;
  short int count_pos_occup_a_terra;
  short int Punto, Pos, Pin, magazzino;
  short int i, j, k, z, altro_tgv, limitrofo;
  short int altro_tgv_blocco_rot;
  short int Posizione, PosizioneTgv, Piano;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  short int old_dist=9999, new_dist, num_rot;
  short int altro_num_mag_sorg, altro_num_pos_sorg;
  short int altro_num_mag_dest, altro_num_pos_dest;
  bool mag_pieno, controllo_distanza;


  err        = 0;
  *num_mag   = 0;
  *num_pos   = 0;
  *num_pin   = 0;
  new_dist   = 0;
  num_rot    = 0;
  altro_tgv  = 0;
  limitrofo  = 0;
  DiffBox    = 0;
  OldDiffBox = 99;
  strcpy( all_mess, "OK");
  memset(&nodi_perc, 0, sizeof(nodi_perc));
  memset(&dati_perc, 0, sizeof(dati_perc));
  //-------------------------------------------
  // Funzione ammessa solo per la ricerca
  // vuoti
  //-------------------------------------------
  if(tipo_carico!=PRES_VUOTO) return 0;
  //-------------------------------------------
  // Ricerca tra tutti i magazzini in elenco
  //-------------------------------------------
  for(magazzino=1; magazzino<=MAXMAG; magazzino++){
     altro_tgv=0;
     //-----------------------------------
     // Cerca Magazzino compatibile
     //-----------------------------------
     if(m[magazzino]->Num<1             ) continue;
     if(m[magazzino]->Num>TOTMAG        ) continue;
     if(m[magazzino]->Npt==0            ) continue;
     if(m[magazzino]->NumCestoni==0     ) continue;
     if(m[magazzino]->PuntoIng==0       ) continue;
     if(m[magazzino]->PuntoIng>TOTPUNTI ) continue;
     if(m[magazzino]->PuntoOut==0       ) continue;
     if(m[magazzino]->PuntoOut>TOTPUNTI ) continue;
     if(m[magazzino]->Stato==MG_ST_FREE ) continue;
     if(m[magazzino]->Tipo!=MG_VUOTI    ) continue;
     //------------------------------------------
     // Se richiesto magazzino specifico,
     // scarto a priori gli altri
     //------------------------------------------
     if(singolo_mag!=0 && m[magazzino]->Num!=singolo_mag) continue;
     //--------------------------------------------------------------
     // GESTIONE INCOMPLETI
     // Andrea L.
     // In realtà il Mag. resta completo finchè non è vuoto percui
     // devo controllare l'effettiva quantità di pallet presenti
     //--------------------------------------------------------------
     // Determina se il Mag è realmente completo
     //--------------------------------------------------------------
     mag_pieno=false;
     Capienza_Mag = m[magazzino]->Npt*(m[magazzino]->Sovrapposizione+1);
     if(m[magazzino]->Npt!=0 && m[magazzino]->NumCestoni>=Capienza_Mag){
        mag_pieno=true;
     }
   //if(incompleto==true && m[magazzino]->Completo==true           ) continue;
     if(singolo_mag==0 && incompleto==true && mag_pieno==true      ) continue;
     //------------------------------------------
     // Gestione gruppo di appartenenza
     //------------------------------------------
     if(gruppo_dest!=NN && gruppo_dest!=m[magazzino]->Gruppo       ) continue;
     //------------------------------------------
     // Verifico coerenza tipo cestone
     //------------------------------------------
     if(tipo_carico==PRES_VUOTO && m[magazzino]->Tipo!=MG_VUOTI    ) continue;
     if(tipo_carico==PRES_VUOTO && m[magazzino]->Stato!=MG_ST_VUOTI) continue;
     //------------------------------------------
     // GESTIONE MAGAZZINI ASSEGNATI
     //------------------------------------------
     if(m[magazzino]->Assegnazione!=PRES_VUOTO                     ) continue;
     //------------------------------------------
     // VERIFICA TIPO MAGAZZINO (FIFO/LIFO)
     // In caso di ricerca prelievo da lato
     // ingresso forzo il mag. tipo LIFO
     //------------------------------------------
     prelievo_da_out = m[magazzino]->Fifo;
     if(lato_ingresso==true) prelievo_da_out=false;
     //------------------------------------------
     // Cerco il primo cestone presente
     //------------------------------------------
     Pos=99;
     Pin=99;
     // LIFO
     if(prelievo_da_out==false){
        for(i=1; i<=MAXTERRAMAG; i++){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
              Pos=i;
              Pin=j;
              break;
           }
           if(Pos!=99) break;
        }
     }
     // FIFO
     else{
        for(i=MAXTERRAMAG; i>0; i--){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
              Pos=i;
              Pin=j;
              break;
           }
           if(Pos!=99) break;
        }
     }
     if(Pos==99) continue;      // nessuna posizione impegnata
     //-----------------------------------------------------------------
     // Calcolo differenza pallet a completamento
     //-----------------------------------------------------------------
     count_pos_occup_a_terra=0;
     for(i=1; i<=m[magazzino]->Npt; i++){
        if(m[magazzino]->Pos[i].Presenza[0]==false) continue;
        count_pos_occup_a_terra++;
     }
     //DiffBox = (short int)(count_pos_occup_a_terra);
     DiffBox = (short int)(m[magazzino]->NumCestoni);
     //-----------------------------------------------------------------
     // Verifico il tipo box in prelievo
     //-----------------------------------------------------------------
     if(tipo_box!=SEARCH_ALL_TIPOBOX && tipo_box!=m[magazzino]->Pos[Pos].Box[Pin].TipoBox) continue;
     //-----------------------------------------------------------------
     // IN CASO DI PRELIEVO CONSIDERO IL PUNTO DI USCITA
     // (tranne che per magazzini Lifo)
     //-----------------------------------------------------------------
     if(prelievo_da_out==true) Punto = m[magazzino]->PuntoOut;
     else                      Punto = m[magazzino]->PuntoIng;
     //-----------------------------------------------------------------
     // Controlli nodo disponibile solo con TGV VALIDO
     //-----------------------------------------------------------------
     if(num_agv>0 && num_agv<=MAXAGV){
        //-----------------------------------------------------------------
        // Composizione chiamata abbinata a magazzino
        //-----------------------------------------------------------------
        chiamata=0;
        P.TestNumChiamata(&chiamata, Punto, magazzino);
        if(chiamata==0) continue;     // chiamata sconosciuta
        //-----------------------------------------------------------------
        // verifica se la chiamata è già asservita da un AGV e l'altro AGV
        // è in missione attiva.
        //-----------------------------------------------------------------
        altro_tgv = (short int)(P.test_chiamata_asservita(chiamata));
        if( altro_tgv!=0 && altro_tgv!=num_agv ) continue;
        //-----------------------------------------------------------------
        // Recupero la distanza dalla destinazione
        //-----------------------------------------------------------------
        new_dist=num_rot=0;
        memset(&nodi_perc, 0, sizeof(nodi_perc));
        memset(&dati_perc, 0, sizeof(dati_perc));
        err = AGV[num_agv]->distanza(FAST_PATH, 0, Punto, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
        if(err || new_dist>MAXPERCORSI) continue;
        //-----------------------------------------------------------------
        // Controllo occupazione nodo
        //-----------------------------------------------------------------
        if(abilita_controllo_nodo==true){
           //-----------------------------------------------------------------
           // Gestione NODO CONTRAPPOSTO
           //-----------------------------------------------------------------
           if(N.nodo_contrapposto[Punto]!=0 && N.nodo_busy[N.nodo_contrapposto[Punto]] && N.nodo_busy[N.nodo_contrapposto[Punto]]!=num_agv) continue;
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
           if(altro_tgv_blocco_rot!=0) continue;
           //-----------------------------------------------------------
           // Controllo se magazzino già impeganto da altro TGV
           //-----------------------------------------------------------
           if(P.test_dest_occupata(Punto, &altro_tgv) && altro_tgv!=0 && altro_tgv!=num_agv){
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
              if(altro_num_mag_sorg==m[magazzino]->Num && altro_num_mag_dest!=m[magazzino]->Num) tgv_in_uscita=true;
              if( tgv_in_uscita==false ) continue;   //###
           }
           //-----------------------------------------------------------
           // Destinazione riservata da altro TGV
           //-----------------------------------------------------------
           if(P.test_dest_riservata(Punto, &altro_tgv, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv){
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
              if(altro_num_mag_sorg==m[magazzino]->Num && altro_num_mag_dest!=m[magazzino]->Num) tgv_in_uscita=true;
              if( tgv_in_uscita==false ) continue;   //###
           }
           //-----------------------------------------------------------
           // TGV su nodo limitrofo
           //-----------------------------------------------------------
           if(P.test_dest_limitrofe(Punto, &altro_tgv, &limitrofo, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv ) continue;
        }
     }
     //-----------------------------------------------------------
     // Verifica del Tipo di Deposito possibile
     //-----------------------------------------------------------
     Posizione=PosizioneTgv=Piano=0;
     if(VerificaTipoPrelievo(lato_ingresso, m[magazzino]->Num, typ_presa, &Posizione, &Piano, &PosizioneTgv, all_mess)) continue;
     //-----------------------------------------------------------
     // Se TipoDeposito=0 nessun deposito possibile
     //-----------------------------------------------------------
     if(Posizione==0) continue;
     //-----------------------------------------------------------
     // Per il prelievo oltra alla distanza è importante il
     // numero di box in magazzino per garantire lo svuotamento
     //-----------------------------------------------------------
     #ifdef SVUOTAMENTO_VUOTI_MENO_PRESENZE
        //---------------------------------
        // cerca Mag con meno pallets
        //---------------------------------
        controllo_distanza=true;
        if(incompleto==true){
           controllo_distanza=false;
           if(DiffBox<=OldDiffBox){      // accetto anche gli uguali per passare al controllo della distanza
              OldDiffBox = DiffBox;
              *num_mag = m[magazzino]->Num;
              *num_pin = Piano;
              *num_pos = Posizione;
           }
        }
        else{
           controllo_distanza=true;
        }
     #else
        controllo_distanza=true;
     #endif;
     if( controllo_distanza==true ){
        //-----------------------------------------------------------
        // Cerca magazzino più vicino
        //-----------------------------------------------------------
        if(new_dist < old_dist){
           old_dist = new_dist;
           *num_mag = m[magazzino]->Num;
           *num_pin = Piano;
           *num_pos = Posizione;
           //-----------------------------------------------
           // se non è valorizzato il TGV skip perchè il
           // controllo del più vicino non avrebbe senso
           //-----------------------------------------------
           if(num_agv<1 || num_agv>MAXAGV) return 0;
        }
     }
  }
  return err;
}

// --------------------------------------------
//   CercaMagDaSvuotarePIENO()
// --------------------------------------------
//  Restituisce il numero del magazzino più vicino con la possibilità
//  di deposito in base al tipo di box da depositare
//  TipoPrelievo :   0 -> Mag pieno nessun deposito possibile.
//                   1 -> deposito a terra.
//                   2 -> deposito alto al 1° piano.
//                   3 -> deposito alto al 2° piano.
//
int magaz::CercaMagDaSvuotarePIENO(bool incompleto, bool lato_ingresso, bool non_raggiungibile, short int tipo_mag, short int gruppo_dest, short int num_agv, short int typ_presa, short int *num_mag, short int *num_pos, short int *num_pin, short int *pos_spost, short int *pin_spost, bool *cambia_rot)
/*************************************************************************/
{
  int  chiamata, err;
  char all_mess[101]="";
  bool tgv_in_uscita;
  bool prelievo_da_out;
  bool abilita_svuotamento;
  bool almeno_un_pallet_da_spostare;
  short int DiffBox, OldDiffBox;
  short int count_pos_occup_a_terra;
  short int Punto, Pos, Pin, magazzino;
  short int i, j, k, z, altro_tgv, limitrofo;
  short int altro_tgv_blocco_rot;
  short int Posizione, PosizioneTgv, Piano;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  short int old_dist=9999, new_dist, num_rot;
  short int tipo, num_mac, num_baia, num_mag1;
  short int altro_num_mag_sorg, altro_num_pos_sorg;
  short int altro_num_mag_dest, altro_num_pos_dest;
  short int tipo_prov_box, mac_prov_box, bai_prov_box, mag_prov_box;

  err        = 0;
  *num_mag   = 0;
  *num_pos   = 0;
  *num_pin   = 0;
  *pos_spost = 0;
  *pin_spost = 0;
  new_dist   = 0;
  altro_tgv  = 0;
  limitrofo  = 0;
  DiffBox    = 0;
  OldDiffBox = 99;
  *cambia_rot = false;
  strcpy( all_mess, "OK");
  memset(&nodi_perc, 0, sizeof(nodi_perc));
  memset(&dati_perc, 0, sizeof(dati_perc));
  //-------------------------------------------
  // Funzione ammessa solo per la ricerca
  // vuoti
  //-------------------------------------------
  if(gruppo_dest==NN) return 0;
  //-------------------------------------------
  // Ricerca tra tutti i magazzini in elenco
  //-------------------------------------------
  for(magazzino=1; magazzino<=MAXMAG; magazzino++){
     altro_tgv=0;
     //------------------------------------------
     // Cerca Magazzino compatibile
     //------------------------------------------
     if(m[magazzino]->Num<1              ) continue;
     if(m[magazzino]->Num>TOTMAG         ) continue;
     if(m[magazzino]->Npt==0             ) continue;
     if(m[magazzino]->NumCestoni==0      ) continue;
     if(m[magazzino]->PuntoIng==0        ) continue;
     if(m[magazzino]->PuntoIng>TOTPUNTI  ) continue;
     if(m[magazzino]->PuntoOut==0        ) continue;
     if(m[magazzino]->PuntoOut>TOTPUNTI  ) continue;
     if(m[magazzino]->Stato==MG_ST_FREE  ) continue;
     //------------------------------------------
     // SOLO MAGAZZINI DEPOSITO PIENI
     //------------------------------------------
     if(tipo_mag==NN){
        if(m[magazzino]->Tipo!=MG_PROD_TUTTI &&
           m[magazzino]->Tipo!=MG_PROD_TIPO1 &&
           m[magazzino]->Tipo!=MG_PROD_TIPO2) continue;
     }
     //------------------------------------------
     // RICERCA DEL TIPO DI MAGAZZINO SPECIFICO
     //------------------------------------------
     else{
        if(m[magazzino]->Tipo!=tipo_mag) continue;
     }
     //------------------------------------------
     // Completi/incompleti
     //------------------------------------------
     if(incompleto==true && m[magazzino]->Completo==true  ) continue;
     //------------------------------------------
     // Obbligo di assegnazione GRUPPO
     //------------------------------------------
     if(m[magazzino]->Gruppo==NN         ) continue;
     //------------------------------------------
     // Verifico coerenza tipo cestone
     //------------------------------------------
     if(m[magazzino]->Stato==MG_ST_VUOTI ) continue;
     //------------------------------------------
     // VERIFICA TIPO MAGAZZINO (FIFO/LIFO)
     // In caso di ricerca prelievo da lato
     // ingresso forzo il mag. tipo LIFO
     //------------------------------------------
     prelievo_da_out = m[magazzino]->Fifo;
     if(lato_ingresso==true) prelievo_da_out=false;
     //------------------------------------------
     // Cerco il primo cestone presente
     //------------------------------------------
     Pos=99;
     Pin=99;
     // LIFO
     if(prelievo_da_out==false){
        for(i=1; i<=MAXTERRAMAG; i++){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
              Pos=i;
              Pin=j;
              break;
           }
           if(Pos!=99) break;
        }
     }
     // FIFO
     else{
        for(i=MAXTERRAMAG; i>0; i--){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
              Pos=i;
              Pin=j;
              break;
           }
           if(Pos!=99) break;
        }
     }
     if(Pos==99) continue;      // nessuna posizione impegnata
     //-----------------------------------------------------------------
     // Ricerca magazzino con primo pallet in prelievo DA SPOSTARE
     //-----------------------------------------------------------------
     if(non_raggiungibile==false){
        //--------------------------------------------------------
        // MAGAZZINI NON TEMPORANEI
        // spostamento con gruppo destinazione
        //--------------------------------------------------------
        if(m[magazzino]->Tipo!=MG_TEMPORANEO){
           //------------------------------------------------
           // Pallet da prelevare già a gruppo destinazione
           //------------------------------------------------
           if(m[magazzino]->Pos[Pos].Box[Pin].GruppoDest==NN) continue;
           if(m[magazzino]->Pos[Pos].Box[Pin].GruppoDest==m[magazzino]->Gruppo) continue;
           //-----------------------------------------------------------------
           // Verifico se il pallet è destinato al GRUPPO DESTINAZIONE
           //-----------------------------------------------------------------
           if(gruppo_dest!=m[magazzino]->Pos[Pos].Box[Pin].GruppoDest) continue;
        }
        //--------------------------------------------------------
        // MAGAZZINI TEMPORANEI
        // spostamento con numero magazzino di provenienza
        //--------------------------------------------------------
        else{
           //------------------------------------------------
           // Decodifica provenieza
           //------------------------------------------------
           tipo_prov_box=mac_prov_box=bai_prov_box=mag_prov_box=0;
           P.TestProvenienzaBox(m[magazzino]->Pos[Pos].Box[Pin].ProvBox, &tipo_prov_box, &mac_prov_box, &bai_prov_box, &mag_prov_box);
           if(tipo_prov_box!=PROV_MAGAZZINO    ) continue;
           if(mag_prov_box==m[magazzino]->Num  ) continue;
        }
        //--------------------------------------------------------
        // MEMORIZZO LA POSIZIONE DEL PALLET DA SPOSTARE
        //--------------------------------------------------------
        *pos_spost = Pos;
        *pin_spost = Pin;
     }
     //-----------------------------------------------------------------
     // Ricerca magazzino con un pallet non raggiungibile da spostare
     //-----------------------------------------------------------------
     else{
        almeno_un_pallet_da_spostare=false;
        for(i=MAXTERRAMAG; i>0; i--){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
              //--------------------------------------------------------
              // MAGAZZINI NON TEMPORANEI
              // spostamento con gruppo destinazione
              //--------------------------------------------------------
              if(m[magazzino]->Tipo!=MG_TEMPORANEO){
                 if(m[magazzino]->Pos[i].Box[j].GruppoDest==NN                  ) continue;
                 if(m[magazzino]->Pos[i].Box[j].GruppoDest==m[magazzino]->Gruppo) continue;
                 //-----------------------------------------------------------------
                 // Verifico se il pallet è destinato al GRUPPO DESTINAZIONE
                 //-----------------------------------------------------------------
                 if(gruppo_dest!=m[magazzino]->Pos[i].Box[j].GruppoDest) continue;
              }
              //--------------------------------------------------------
              // MAGAZZINI TEMPORANEI
              // spostamento con numero magazzino di provenienza
              //--------------------------------------------------------
              else{
                 //------------------------------------------------
                 // Decodifica provenieza
                 //------------------------------------------------
                 tipo_prov_box=mac_prov_box=bai_prov_box=mag_prov_box=0;
                 P.TestProvenienzaBox(m[magazzino]->Pos[i].Box[j].ProvBox, &tipo_prov_box, &mac_prov_box, &bai_prov_box, &mag_prov_box);
                 if(tipo_prov_box!=PROV_MAGAZZINO    ) continue;
                 if(mag_prov_box==m[magazzino]->Num  ) continue;
              }
              almeno_un_pallet_da_spostare=true;
              break;
           }
           if(almeno_un_pallet_da_spostare) break;
        }
        if(almeno_un_pallet_da_spostare==false) continue;
        //--------------------------------------------------------
        // MEMORIZZO LA POSIZIONE DEL PALLET DA SPOSTARE
        //--------------------------------------------------------
        else{
           *pos_spost = i;
           *pin_spost = j;
        }
     }
     //-----------------------------------------------------------------
     // Calcolo differenza postazioni a completamento
     //-----------------------------------------------------------------
     count_pos_occup_a_terra=0;
     for(i=1; i<=m[magazzino]->Npt; i++){
        if(m[magazzino]->Pos[i].Presenza[0]==false) continue;
        count_pos_occup_a_terra++;
     }
     DiffBox = (short int)(count_pos_occup_a_terra);
     //-----------------------------------------------------------------
     // IN CASO DI PRELIEVO CONSIDERO IL PUNTO DI USCITA
     // (tranne che per magazzini Lifo)
     //-----------------------------------------------------------------
     if(prelievo_da_out==true) Punto = m[magazzino]->PuntoOut;
     else                      Punto = m[magazzino]->PuntoIng;
     //-----------------------------------------------------------------
     // Controlli nodo disponibile solo con TGV VALIDO
     //-----------------------------------------------------------------
     if(num_agv>0 && num_agv<=MAXAGV){
        //-----------------------------------------------------------------
        // Composizione chiamata abbinata a magazzino
        //-----------------------------------------------------------------
        chiamata=0;
        P.TestNumChiamata(&chiamata, Punto, magazzino);
        if(chiamata==0) continue;     // chiamata sconosciuta
        //-----------------------------------------------------------------
        // verifica se la chiamata è già asservita da un AGV e l'altro AGV
        // è in missione attiva.
        //-----------------------------------------------------------------
        altro_tgv = (short int)(P.test_chiamata_asservita(chiamata));
        if( altro_tgv!=0 && altro_tgv!=num_agv ) continue;
        //-----------------------------------------------------------------
        // Gestione NODO CONTRAPPOSTO
        //-----------------------------------------------------------------
        if(N.nodo_contrapposto[Punto]!=0 && N.nodo_busy[N.nodo_contrapposto[Punto]] && N.nodo_busy[N.nodo_contrapposto[Punto]]!=num_agv) continue;
        //-----------------------------------------------------------------
        // Recupero la distanza dalla destinazione
        //-----------------------------------------------------------------
        memset(&nodi_perc, 0, sizeof(nodi_perc));
        memset(&dati_perc, 0, sizeof(dati_perc));
        err = AGV[num_agv]->distanza(FAST_PATH, 0, Punto, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
        if(err || new_dist>MAXPERCORSI) continue;
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
        if(altro_tgv_blocco_rot!=0) continue;
        //-----------------------------------------------------------
        // Controllo se magazzino già impeganto da altro TGV
        //-----------------------------------------------------------
        if(P.test_dest_occupata(Punto, &altro_tgv) && altro_tgv!=0 && altro_tgv!=num_agv) continue;
        //-----------------------------------------------------------
        // Destinazione riservata da altro TGV
        //-----------------------------------------------------------
        if(P.test_dest_riservata(Punto, &altro_tgv, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv) continue;
        //-----------------------------------------------------------
        // TGV su nodo limitrofo
        //-----------------------------------------------------------
        if(P.test_dest_limitrofe(Punto, &altro_tgv, &limitrofo, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv ) continue;
     }
     //-----------------------------------------------------------
     // Verifica del Tipo di Deposito possibile
     //-----------------------------------------------------------
     Posizione=PosizioneTgv=Piano=0;
     if(VerificaTipoPrelievo(lato_ingresso, m[magazzino]->Num, typ_presa, &Posizione, &Piano, &PosizioneTgv, all_mess)) continue;
     //-----------------------------------------------------------
     // Se TipoDeposito=0 nessun deposito possibile
     //-----------------------------------------------------------
     if(Posizione==0) continue;
     //-----------------------------------------------------------
     // Per il prelievo oltra alla distanza è importante il
     // numero di box in magazzino per garantire lo svuotamento
     //-----------------------------------------------------------
     abilita_svuotamento=true;
     if(incompleto==true){
        // FIFO (cerco quella con meno box dentro per svuotarla compeltamente)
        if(m[magazzino]->Fifo==true){
           if(DiffBox<=OldDiffBox){      // accetto anche gli uguali per passare al controllo della distanza
              abilita_svuotamento=true;
              OldDiffBox = DiffBox;
           }
        }
        // LIFO (il primo trovato in ordine di inserimento)
        else{
           abilita_svuotamento=true;
        }
     }
     if(abilita_svuotamento==false) continue;
     //-----------------------------------------------------------
     // Cerca magazzino più vicino
     //-----------------------------------------------------------
     if(new_dist < old_dist){
        old_dist = new_dist;
        *num_mag = m[magazzino]->Num;
        *num_pos = Posizione;
        *num_pin = Piano;
        //-----------------------------------------------
        // Verrà ruotato al prelievo
        //-----------------------------------------------
        if(prelievo_da_out==true) *cambia_rot=true;
        //-----------------------------------------------
        // se non è valorizzato il TGV skip perchè il
        // controllo del più vicino non avrebbe senso
        //-----------------------------------------------
        if(num_agv<1 || num_agv>MAXAGV) return 0;
     }
  }
  return err;
}

// --------------------------------------------
//   CercaMagRiempirePIENO()
// --------------------------------------------
//  Restituisce il numero del magazzino più vicino con la possibilità
//  di deposito in base al tipo di box da depositare
//  TipoDeposito :   0 -> Mag pieno nessun deposito possibile.
//                   1 -> deposito a terra.
//                   2 -> deposito alto al 1° piano.
//                   3 -> deposito alto al 2° piano.
//
int magaz::CercaMagRiempirePIENO(bool libero, bool abilita_controllo_nodo, bool lato_uscita, bool disp_sovrap, short int singolo_mag, short int gruppo_dest, short int *num_mag, short int *num_pos, short int *num_pin, int provenienza, short int num_agv, short int typ_presa, short int tipo_carico, short int tipo_box, bool presa[POSINPRESA+1][PIANIINPRESA+1], short int codice, int lotto, short int car_wash, bool basso, bool rotaz, struct DATI_PRD *Udc, bool presa_sovrapponibile, char *all_mess)
/*************************************************************************/
{
  int chiamata, err=0;
  int gestione_cambio_lotto=GESTIONE_CAMBIO_LOTTO;
  int gestione_cambio_turno=GESTIONE_CAMBIO_TURNO;      //### Andrea 14/9
  bool cambio_lotto;
  bool cambio_turno, salta_pos;      //### Andrea 14/9
  int deposito_nel_piu_vicino=DEPOSITO_NEL_PIU_VICINO;
  bool udc_trovato;
  bool deposito_in_out;
  bool tgv_in_uscita;
  bool almeno_un_pallet_da_spostare;
  short int count_pos;
  short int Punto, Pos, Pin, magazzino;
  short int i, j, k, z, altro_tgv, limitrofo;
  short int altro_tgv_blocco_rot;
  short int Posizione, PosizioneTgv, Piano;
  short int nodi_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati_perc[MAXPERCORSI];
  short int old_dist=9999, new_dist, num_rot;
  short int tipo, num_mac, num_baia, num_mag1;
  long int OldTime;
  struct DATI_PRD UdcMag;
  short int altro_num_mag_sorg, altro_num_pos_sorg;
  short int altro_num_mag_dest, altro_num_pos_dest;
  short int altro_agv;
  int controlla_assegnazione_su_spost = CONTROLLO_ASSEGNAZIONE_SPOSTAMENTO;

  err       = 0;
  *num_mag  = 0;
  *num_pos  = 0;
  *num_pin  = 0;
  new_dist  = 0;
  num_rot   = 0;
  altro_tgv = 0;
  limitrofo = 0;
  OldTime = time(NULL) + 63244800L;          // Forza al valore di oggi + 2ANNO  memset(&elenco_mag, 0, sizeof(elenco_mag));
  strcpy( all_mess, "OK");
  memset(&nodi_perc, 0, sizeof(nodi_perc));
  memset(&dati_perc, 0, sizeof(dati_perc));
  //-------------------------------------------
  // Funzione non abilitata per i VUOTI
  //-------------------------------------------
  if(tipo_carico==PRES_VUOTO) return 0;
  //-------------------------------------------
  // Decodifica provenienza
  //-------------------------------------------
  tipo=num_mac=num_baia=num_mag1=0;
  P.TestProvenienzaBox(provenienza, &tipo, &num_mac, &num_baia, &num_mag1);
  //-------------------------------------------
  // Se non viene passato il TGV non può
  // essere fatto il deposito nel più vicino
  //-------------------------------------------
  if(num_agv<1 || num_agv>MAXAGV) deposito_nel_piu_vicino=0;
  //-------------------------------------------
  // Ricerca tra tutti i magazzini in elenco
  //-------------------------------------------
  for(magazzino=1; magazzino<=MAXMAG; magazzino++){
     altro_tgv=0;
     //------------------------------------------
     // Cerca Magazzino compatibile
     //------------------------------------------
     if(m[magazzino]->Num<1              ) continue;
     if(m[magazzino]->Num>TOTMAG         ) continue;
     if(m[magazzino]->Npt==0             ) continue;
     if(m[magazzino]->PuntoIng==0        ) continue;
     if(m[magazzino]->PuntoIng>TOTPUNTI  ) continue;
     if(m[magazzino]->PuntoOut==0        ) continue;
     if(m[magazzino]->PuntoOut>TOTPUNTI  ) continue;
     if(m[magazzino]->Completo==true     ) continue;
     //------------------------------------------
     // RICERCA MAGAZZINO SPECIFICO
     //------------------------------------------
     if(singolo_mag>0 && singolo_mag<=TOTMAG){
        if(m[magazzino]->Num!=singolo_mag) continue;
     }
     //------------------------------------------
     // SOLO MAGAZZINI DEPOSITO PIENI
     //------------------------------------------
     if(m[magazzino]->Tipo!=MG_PROD_TUTTI &&
        m[magazzino]->Tipo!=MG_PROD_TIPO1 &&
        m[magazzino]->Tipo!=MG_PROD_TIPO2) continue;
     //------------------------------------------
     // DISPONIBILITA' ALLA SOVRAPPOSIZIONE
     //------------------------------------------
     if(disp_sovrap==true && m[magazzino]->Sovrapposizione==0  ) continue;
     //------------------------------------------
     // Controllo gruppo appartenenza MAGAZZINO
     //------------------------------------------
     if(gruppo_dest!=NN && gruppo_dest!=m[magazzino]->Gruppo   ) continue;
     //------------------------------------------
     // GESTIONE ROTAZIONE PALLET
     //------------------------------------------
     if(m[magazzino]->PerPalRuotati!=NN && AbilitaSvuotamentoManualeMag==3){
        if(m[magazzino]->PerPalRuotati==1 && rotaz==false) continue;
        if(m[magazzino]->PerPalRuotati==2 && rotaz==true) continue;
     }
     //------------------------------------------
     // Controlla assegnazione su spostamento
     //------------------------------------------
     if( controlla_assegnazione_su_spost==1 ){
        if(tipo_carico!=NN && m[magazzino]->Assegnazione!=NN && tipo_carico!=m[magazzino]->Assegnazione ) continue;
     }
     //------------------------------------------
     // GESTIONE_TIPOBOX
     // Controllo del tipo assegnato al magazzino
     //------------------------------------------
     #ifdef GESTIONE_TIPOBOX
        if(tipo_box!=SEARCH_ALL_TIPOBOX && tipo_box!=m[magazzino]->TipoBox) continue;
     #endif;
     //------------------------------------------
     // Magazzino completamente libero
     //------------------------------------------
     if(libero==true  && m[magazzino]->Stato!=MG_ST_FREE       ) continue;
     if(libero==false && m[magazzino]->Stato==MG_ST_FREE       ) continue;
     //------------------------------------------
     // Verifico coerenza tipo cestone
     //------------------------------------------
     if(tipo_carico==PRES_VUOTO && m[magazzino]->Tipo!=MG_VUOTI) continue;
     if(tipo_carico!=PRES_VUOTO && m[magazzino]->Tipo==MG_VUOTI) continue;
     //------------------------------------------
     // Verifico abilitazione alti/bassi
     //------------------------------------------
     if(m[magazzino]->Tipo!=MG_PROD_TUTTI){
        if(m[magazzino]->Tipo==MG_PROD_TIPO1 && basso==false   ) continue;
        if(m[magazzino]->Tipo==MG_PROD_TIPO2 && basso==true    ) continue;
     }
     //------------------------------------------
     // VERIFICA TIPO MAGAZZINO (FIFO/LIFO)
     // In caso di ricerca prelievo da lato
     // ingresso forzo il mag. tipo LIFO
     //------------------------------------------
     deposito_in_out = lato_uscita;
     if(m[magazzino]->Fifo==false) deposito_in_out=false;
     //------------------------------------------
     // Magazzinio già impegnato
     //------------------------------------------
     cambio_lotto=false;
     cambio_turno=false;
     if(m[magazzino]->Stato!=MG_ST_FREE){
        if(tipo_carico==ST_BOX_VUOTI && m[magazzino]->Stato!=MG_ST_VUOTI) continue;
        if(tipo_carico!=ST_BOX_VUOTI && m[magazzino]->Stato==MG_ST_VUOTI) continue;
        //------------------------------------------
        // Cerco il primo cestone presente
        //------------------------------------------
        Pos=99;
        Pin=99;
        // LIFO
        if(deposito_in_out==false){
           for(i=1; i<=MAXTERRAMAG; i++){
              for(j=MAXPIANIMAG-1; j>=0; j--){
                 if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
                 Pos=i;
                 Pin=j;
                 break;
              }
              if(Pos!=99) break;
           }
        }
        // FIFO
        else{
           for(i=MAXTERRAMAG; i>0; i--){
              for(j=MAXPIANIMAG-1; j>=0; j--){
                 if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
                 Pos=i;
                 Pin=j;
                 break;
              }
              if(Pos!=99) break;
           }
        }
        if(Pos==99) continue;      // nessuna posizione impegnata
        //-----------------------------------------------------------------
        // Se il magazzino è LIFO e il primo BOX in prelievo è in fase di
        // spostamento, non è ammesso continuare il riempimento
        //-----------------------------------------------------------------
        almeno_un_pallet_da_spostare=false;
        for(i=MAXTERRAMAG; i>0; i--){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(m[magazzino]->Pos[i].Presenza[j]==false) continue;
              //--------------------------------------------------------
              // spostamento con gruppo destinazione
              //--------------------------------------------------------
              if(m[magazzino]->Pos[i].Box[j].GruppoDest==NN                  ) continue;
              if(m[magazzino]->Pos[i].Box[j].GruppoDest==m[magazzino]->Gruppo) continue;
              almeno_un_pallet_da_spostare=true;
              break;
           }
           if(almeno_un_pallet_da_spostare) break;
        }
        if(almeno_un_pallet_da_spostare==true) continue;
        //-----------------------------------------------------------------
        // GESTIONE UDC
        // Memorizzo i dati di uno degli Udc in presa che dovranno essere
        // tutti uguali
        //-----------------------------------------------------------------
        udc_trovato=false;
        memset(&UdcMag, 0, sizeof(DATI_PRD));
        if(GestioneUdc!=0){
           for(i=0; i<POSINPRESA; i++){
              for(j=0; j<PIANIINPRESA; j++){
                 if(m[magazzino]->Pos[Pos].Box[Pin].Presa[i][j]==false) continue;
                 //-------------------------------------------
                 // Verifico se UDC duplicato
                 //-------------------------------------------
                 k=i+(j*POSINPRESA);
                 memcpy(&UdcMag, &m[magazzino]->Pos[Pos].Box[Pin].Prd[k], sizeof(DATI_PRD));
                 udc_trovato=true;
                 break;
              }
              if(udc_trovato==true) break;
           }
        }
        //-----------------------------------------------------------------
        // GESTIONE_DINAMICA_MAG
        // Non controllo del tipo di supporto
        //-----------------------------------------------------------------
        if(GestioneDinamicaMagazzini==0){
           if(tipo_box!=SEARCH_ALL_TIPOBOX && tipo_box!=m[magazzino]->Pos[Pos].Box[Pin].TipoBox) continue;
        }
        //-----------------------------------------------------------------
        // CONTROLLI RISERVATI AI BOX PIENI CRUDI
        // 1 - Verifica dati postazione solo per i PIENI
        // 2 - Scarto tutti i magazzini anche incompleti al cui interno
        //     sia stato raggiunto l'Over Time su tutte le posizioni
        //-----------------------------------------------------------------
        if(tipo_carico!=PRES_VUOTO){
           if(m[magazzino]->Pos[Pos].Box[Pin].StatoLav!=tipo_carico             ) continue;
           if(m[magazzino]->Pos[Pos].Box[Pin].GruppoDest!=m[magazzino]->Gruppo  ) continue;
           //-----------------------------------------------------------------
           // Deposito nel magazzino più vecchio per garantire la cronologia
           // del prodotto
           //-----------------------------------------------------------------
           if(m[magazzino]->Time>OldTime) continue;
           //-----------------------------------------------------------------
           // AbilitaSvuotamentoManualeMag==3
           // ATTIVO STESSO CONTROLLO DATI NEL DEPOSITO DA BAIA
           //-----------------------------------------------------------------
           if(AbilitaSvuotamentoManualeMag==3){
              //-----------------------------------------------------------------
              // GESTIONE CAR-WASH
              //-----------------------------------------------------------------
              if(m[magazzino]->PerEmergenza==1){
                 if(m[magazzino]->Pos[Pos].Box[Pin].CarWash!=car_wash) continue;
              }
              //-----------------------------------------------------------------
              // Salto posizione a terra con cambio lotto
              // 1. controllo cambio lotto con dati PLC.
              // 2. controllo anche il cambio lotto e codice a UDC
              //    SE GESTIONE ATTIVA E OBBLIGATA
              //-----------------------------------------------------------------
              cambio_lotto=false;
              cambio_turno=false;
              // 1. cambio lotto PLC
              if(m[magazzino]->Pos[Pos].Box[Pin].Lotto!=lotto) cambio_lotto=true;
              // 2. cambio lotto UDC
              if(GestioneUdc!=0){
                 if(strcmp(Udc->code,  UdcMag.code)!=0 ) continue;
                 if(strcmp(Udc->batch, UdcMag.batch)!=0) cambio_lotto=true;
                 if(strcmp(Udc->shift, UdcMag.shift)!=0) cambio_turno=true;
              }
              //----------------------------------------------------------------------
              // Andrea 16/02/2018
              // Se Carwash allora considera solo il Codice ma non Lotto, Batch, Udc
              //----------------------------------------------------------------------
              if( cambio_lotto==true && car_wash==true ){
                 cambio_lotto=false;
              }
              if( cambio_turno==true && car_wash==true ){     //### Andrea 14/9
                 cambio_turno=false;
              }
              //-----------------------------------------------------------------
              // Verifico come gestire il cambio lotto
              // 0 - controllo disattivato
              // 1 - salto fila
              // 2 - salto posizione
              //-----------------------------------------------------------------
              if(gestione_cambio_lotto==0 && cambio_lotto==true) cambio_lotto=false;
              if(gestione_cambio_lotto==1 && cambio_lotto==true) continue;
              //-----------------------------------------------------------------
              // ### Andrea 14/9
              // Verifico come gestire il cambio turno
              // 0 - controllo disattivato
              // 1 - salto fila
              // 2 - salto posizione
              //-----------------------------------------------------------------
              if(gestione_cambio_turno==0 && cambio_turno==true) cambio_turno=false;
              if(gestione_cambio_turno==1 && cambio_turno==true) continue;
           }
        }
     }
     //------------------------------------------------------------
     // IN CASO DI DEPOSITO CONSIDERO IL PUNTO DI INGRESSO
     //------------------------------------------------------------
     if(deposito_in_out==false) Punto = m[magazzino]->PuntoIng;
     if(deposito_in_out==true ) Punto = m[magazzino]->PuntoOut;
     //------------------------------------------------------------
     // Controllo stato del nodo
     //------------------------------------------------------------
     new_dist=num_rot=0;
     if(num_agv>0 && num_agv<=MAXAGV){
        //-----------------------------------------------------------------
        // Composizione chiamata abbinata a magazzino
        //-----------------------------------------------------------------
        chiamata=0;
        P.TestNumChiamata(&chiamata, Punto, magazzino);
        if(chiamata==0) continue;     // chiamata sconosciuta
        //-----------------------------------------------------------------
        // verifica se la chiamata è già asservita da un AGV e l'altro AGV
        // è in missione attiva.
        //-----------------------------------------------------------------
        altro_tgv = (short int)(P.test_chiamata_asservita(chiamata));
        if( altro_tgv!=0 && altro_tgv!=num_agv ) continue;
        //-----------------------------------------------------------------
        // Verifica occupazione nodo
        //-----------------------------------------------------------------
        if(abilita_controllo_nodo==true){
           //------------------------------------------------------------
           // Gestione NODO CONTRAPPOSTO
           //------------------------------------------------------------
           if(N.nodo_contrapposto[Punto]!=0 && N.nodo_busy[N.nodo_contrapposto[Punto]] && N.nodo_busy[N.nodo_contrapposto[Punto]]!=num_agv) continue;
           //--------------------------------------------------------------
           // Recupero la distanza dalla destinazione
           //--------------------------------------------------------------
           memset(&nodi_perc, 0, sizeof(nodi_perc));
           memset(&dati_perc, 0, sizeof(dati_perc));
           err = AGV[num_agv]->distanza(FAST_PATH, 0, Punto, nodi_perc, dati_perc, all_mess, &new_dist, &num_rot);
           if(err || new_dist>MAXPERCORSI) continue;
           //------------------------------------------------------------
           // GESTIONE BLOCCHI ROTAZIONE
           // Scarto i magazzini interbloccati da altri
           //------------------------------------------------------------
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
           if(altro_tgv_blocco_rot!=0) continue;
           //-----------------------------------------------------------
           // Controllo se magazzino già impeganto da altro TGV
           //-----------------------------------------------------------
           if(P.test_dest_occupata(Punto, &altro_tgv) && altro_tgv!=0 && altro_tgv!=num_agv) continue;
           //-----------------------------------------------------------
           // Destinazione riservata da altro TGV
           //-----------------------------------------------------------
           if(P.test_dest_riservata(Punto, &altro_tgv, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv ) continue;
           //-----------------------------------------------------------
           // TGV su nodo limitrofo
           //-----------------------------------------------------------
           if(P.test_dest_limitrofe(Punto, &altro_tgv, &limitrofo, num_agv) && altro_tgv!=0 && altro_tgv!=num_agv ) continue;
        }
     }
     //-----------------------------------------------------------
     // Verifica del Tipo di Deposito possibile
     //-----------------------------------------------------------
     Posizione=PosizioneTgv=Piano=0;
     M.VerificaTipoDeposito(deposito_in_out, m[magazzino]->Num, 0, typ_presa, tipo_box, presa, presa_sovrapponibile, basso, FALSE, &Posizione, &Piano, &PosizioneTgv, all_mess);
     //-----------------------------------------------------------
     // Se TipoDeposito=0 nessun deposito possibile
     //-----------------------------------------------------------
     if(Posizione==0) continue;
     //-----------------------------------------------------------
     // Memorizzo la data di tutti i magazzini trovati
     // per garantire la cronologia del prodotto
     //-----------------------------------------------------------
     OldTime = m[magazzino]->Time;
     //-----------------------------------
     // Cerca magazzino più vicino
     //-----------------------------------
     if(new_dist < old_dist){
        old_dist = new_dist;
        *num_mag = m[magazzino]->Num;
        *num_pos = Posizione;
        *num_pin = Piano;
        //---------------------------------------------
        // DEPOSITO_NEL_PIU_VICINO
        // Se è disattivato, il primo trovato va bene
        //---------------------------------------------
        if(deposito_nel_piu_vicino==0) return 0;
     }
  }
  return err;
}


