//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Main.h"
#include "Tecnofer.h"
#include "Remoto.h"
#include "Input.h"
#include "Impianti.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//
//   Funzioni Generiche adattabili a qualsiasi Databasedi manipolazione
//   TimTgv
//
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// ----------------------------------
//  int TimTgvInizializzaTabella();
// ----------------------------------
// Inizializza i parametri della tabella
//
void TimTgvInizializzaTabella(char *Percorso, char *NomeTabella, TTable *Tabella)
{
    //-----------------------------------------------------
    // In "DatabaseName" si può impostare o un "BDE Alias"
    // oppure esplicitamente la directory in cui si vuole
    // creare la Tabella.
    // In "TableName" il nome della file.dbf (tabella)
    // TableType è il tipo DBASE, PARADOX, ACCESS ecc.
    // N.B. Non specificando un percorso viene preso quello
    //      corrente
    //-----------------------------------------------------
    Tabella->DatabaseName = Percorso;
    Tabella->TableType    = ttDBase;
    Tabella->TableName    = NomeTabella;
    return;
}

//---------------------------------------------------------------------------
// ----------------------------
//    int  TimTgvCreaTabella();
// ----------------------------
// Creazione Tabella Codici in DB4
//
int TimTgvCreaTabella()
{
    short int i, j;
    char NomeIdx[11]="";
    char NomeField[21]="";
    bool CreateIt;
    TIMTGV T;

    TTable *NewTable = new TTable(FormMain);
    memset(&T, 0, sizeof(T));
    //----------------------------------------
    // Istruzioni sotto test delle eccezzioni
    //----------------------------------------
    try{
        //-----------------------------------------------------
        // Richiama la funzione di inizializzazione dei campi
        // Percorso, Nome tabella ecc..
        //-----------------------------------------------------
        TimTgvInizializzaTabella(PercorsoProduzio, FileTabellaTimTgv, NewTable);
        NewTable->Active = false; // La tabella non deve essere attiva al momento della creazione

        //------------------------------------------------------
        // Definizione del RECORD :
        // Per il momento vengono introdotti solo campi Stringa
        //------------------------------------------------------
        NewTable->FieldDefs->Clear();

        strcpy(NomeField, "Date");
        NewTable->FieldDefs->Add(NomeField, ftString, sizeof(T.DataAttuale)-1, False);

        for(i=1; i<=MAXTGV_TIMTGV; i++){
           for(j=1; j<=MAXFIELD_TIMTGV; j++){
              sprintf(NomeField, "Field%02d_%02d", i, j);
              NewTable->FieldDefs->Add(NomeField,  ftString, sizeof(T.Time[i][j])-1,        False);
           }
        }
        //---------------------------------------------------------------------
        // Creazione INDICI
        // Creiamo Indici di tipo "Maintained" che riordina automaticamente
        // gli indice, ma permette di duplicarli.
        // N.B.: Per ora non utilizziamo l'opzione "Unique" inquanto ci crea
        //       dei problemi.
        //---------------------------------------------------------------------
        NewTable->IndexDefs->Clear();
        strcpy(NomeIdx, "IDX");
        strcpy(NomeField, "Date");
        NewTable->IndexDefs->Add(NomeIdx, NomeField, TIndexOptions());

        // Aggiunge il secondo indice Rick 05/10/2015
        strcpy(NomeIdx, "IDX2");
        strcpy(NomeField, "Field01_01");
        NewTable->IndexDefs->Add(NomeIdx, NomeField, TIndexOptions());
        //---------------------------------------------------------------------
        // Controllo che la Tabella NON esista già e se non esiste la crea
        //---------------------------------------------------------------------
        CreateIt = (!NewTable->Exists);
        if(!CreateIt) CreateIt=true;
        //--------------------------
        // Creazione della TABELLA
        //--------------------------
        if(CreateIt) NewTable->CreateTable();
    }
    //----------------------------------------
    // Trattamento delle Eccezzioni
    //----------------------------------------
    catch(...){
        P.error_trasmit(0, "ERRORE in DB4 TEMPI TGV: (errore creazione tabella)");
        delete NewTable;
        return 1;
    }
    delete NewTable;
    return 0;
}

//---------------------------------------------------------------------------
// ----------------------------------------
//  void TimTgvSetCampiTabella()
// ----------------------------------------
// Copia i campi della struttura Record sui campi omonimi della TTABLE
//
void TimTgvSetCampiTabella(struct TIMTGV *Record, TTable *Tabella)
{
  char NomeField[21]="";
  short int i, j;

  //------------------------------
  // Compila i campi della TTABLE
  //------------------------------
//Tabella->Edit();

  strcpy(NomeField, "Date");
  Tabella->FieldValues[NomeField] = Record->DataAttuale;

  for(i=1; i<=MAXTGV_TIMTGV; i++){
     for(j=1; j<=MAXFIELD_TIMTGV; j++){
        sprintf(NomeField, "Field%02d_%02d", i, j);
        Tabella->FieldValues[NomeField] = Record->Time[i][j];
     }
  }
  return;
}
//---------------------------------------------------------------------------
// ---------------------------------------
//  void TimTgvGetCampiTabella()
// ---------------------------------------
// Copia i campi della TTABLE  sui campi omonimi della struttura Record
//
void TimTgvGetCampiTabella(struct TIMTGV *Record, TTable *Tabella)
{
  short int i, j;
  char NomeField[21]="";

  strcpy(NomeField, "Date");
  strcpy(Record->DataAttuale, Tabella->FieldByName(NomeField)->AsString.c_str());

  for(i=1; i<=MAXTGV_TIMTGV; i++){
     for(j=1; j<=MAXFIELD_TIMTGV; j++){
        sprintf(NomeField, "Field%02d_%02d", i, j);
        strcpy(Record->Time[i][j], Tabella->FieldByName(NomeField)->AsString.c_str());
     }
  }
  return;
}

//---------------------------------------------------------------------------
// ---------------------------------
//  int TimTgvCercaPrimoRecord()
// ---------------------------------
// Cerca il Primo Record di una Tabella Codici in DB4
// Come parametro viene passato anche il tipo di indice:
// NUM = Codice Alfa.10 caratteri
// IDX = Indice numerico 4 numeri
//
int TimTgvCercaPrimoRecord(struct TIMTGV *Record, char *NomeIndice, char *all_mess )
{
    TTable *NewTable = new TTable(FormMain);
    //----------------------------------------
    // Istruzioni sotto test delle eccezzioni
    //----------------------------------------
    try{
        //-----------------------------------------------------
        // Richiama la funzione di inizializzazione dei campi
        // Percorso, Nome tabella ecc..
        //-----------------------------------------------------
        TimTgvInizializzaTabella(PercorsoProduzio, FileTabellaTimTgv, NewTable);
        NewTable->IndexName    = NomeIndice;
        NewTable->Active       = true;

        //-------------------------------
        // Apertura Tabella
        //-------------------------------
        NewTable->Open();

        //------------------------------------------------------
        // Mi posiziono sul Primo Record della Tabella
        //------------------------------------------------------
        NewTable->First();
        //------------------------------
        // Compila struttura del Record
        //------------------------------
        TimTgvGetCampiTabella( Record, NewTable);
        //-------------------------------
        // Chiusura Tabella
        //-------------------------------
        NewTable->Close();
    }
    //----------------------------------------
    // Trattamento delle Eccezzioni
    //----------------------------------------
    catch (EDatabaseError &e){
        strcpy( all_mess, "Errore posizionamento sul primo record");
        NewTable->Close();
        delete NewTable;
        Application->MessageBox(e.Message.c_str(), "ERROR", MB_OK + MB_ICONSTOP + MB_TOPMOST);
        return 1;
    }
    catch(...){
        strcpy( all_mess, "Errore posizionamento sul primo record");
        NewTable->Close();
        delete NewTable;
        return 1;
    }
    delete NewTable;
    return 0;
}

//---------------------------------------------------------------------------
// ---------------------------------
//  int TimTgvCercaUltimoRecord()
// ---------------------------------
// Cerca l'ultimo record di una Tabella Codici in DB4
//
int TimTgvCercaUltimoRecord(struct TIMTGV *Record, char *NomeIndice, char *all_mess )
{
    TTable *NewTable = new TTable(FormMain);
    //----------------------------------------
    // Istruzioni sotto test delle eccezzioni
    //----------------------------------------
    try{
        //-----------------------------------------------------
        // Richiama la funzione di inizializzazione dei campi
        // Percorso, Nome tabella ecc..
        //-----------------------------------------------------
        TimTgvInizializzaTabella(PercorsoProduzio, FileTabellaTimTgv, NewTable);
        NewTable->IndexName    = NomeIndice;
        NewTable->Active       = true;

        //-------------------------------
        // Apertura Tabella
        //-------------------------------
        NewTable->Open();

        //------------------------------------------------------
        // Mi posiziono su Ultimo Record della Tabella
        //------------------------------------------------------
        NewTable->Last();
        //------------------------------
        // Compila struttura del Record
        //------------------------------
        TimTgvGetCampiTabella( Record, NewTable);
        //-------------------------------
        // Chiusura Tabella
        //-------------------------------
        NewTable->Close();
    }
    //----------------------------------------
    // Trattamento delle Eccezzioni
    //----------------------------------------
    catch (EDatabaseError &e){
        strcpy( all_mess, "Errore posizionamento sull'ultimo record" );
        NewTable->Close();
        delete NewTable;
        Application->MessageBox(e.Message.c_str(), "ERROR", MB_OK + MB_ICONSTOP + MB_TOPMOST);
        return 1;
    }
    catch(...){
        strcpy( all_mess, "Errore posizionamento sull'ultimo record...");
        NewTable->Close();
        delete NewTable;
        return 1;
    }
    delete NewTable;
    return 0;
}
//---------------------------------------------------------------------------
// ---------------------------------
//  int TimTgvCercaRecord()
// ---------------------------------
// Cerca un record identificato da "chiave"
//
int TimTgvCercaRecord(struct TIMTGV *Record, char *NomeIndice, char *Chiave1, long int Chiave2, bool VisMsgBox, char *all_mess)
{
  int   i, err=0;
  char  myBuffer[51]="";
  bool  trovato=false;

  TTable *NewTable = new TTable(FormMain);
  //----------------------------------------
  // Istruzioni sotto test delle eccezzioni
  //----------------------------------------
  try{
     //-----------------------------------------------------
     // Richiama la funzione di inizializzazione dei campi
     // Percorso, Nome tabella ecc..
     //-----------------------------------------------------
     TimTgvInizializzaTabella(PercorsoProduzio, FileTabellaTimTgv, NewTable);
     NewTable->Active       = true;
     // NewTable->IndexName    = NomeIndice;
     //-------------------------------
     // Apertura Tabella
     //-------------------------------
     NewTable->Open();
     //------------------------------------------------------
     // verifica la presenza dell'indice 2 (IDX2)
     //------------------------------------------------------
     NewTable->IndexDefs->Update();
     for(i=0; i<NewTable->IndexDefs->Count; i++){
        if(NewTable->IndexDefs->Items[i]->Name == String("IDX2")){
           trovato=true;
           break;
        }
     }
     //-----------------------------------------------------------------------------
     // Mi posiziono sul Record della Tabella
     // Se è presente IDX2 utilizza questo altrimenti utilizza quello passato (IDX)
     // L'IDX2 è un timestamp quindi permette alla funzione GotoNearest
     // di posizionarsi correttamente su record più vicino a quello cercato
     //-----------------------------------------------------------------------------
     if(trovato && Chiave2>0){
        NewTable->IndexName = "IDX2";
        NewTable->SetKey();
        sprintf(myBuffer,"%012ld",Chiave2);
        NewTable->FieldByName("FIELD01_01")->AsString = myBuffer;
        NewTable->GotoNearest();
     }
     else{
        NewTable->IndexName = NomeIndice; //"IDX";
        NewTable->SetKey();
        NewTable->FieldByName("Date")->AsString = Chiave1;
        NewTable->GotoKey();
     }
     if(NewTable->RecordCount>0){
         TimTgvGetCampiTabella( Record, NewTable);
         // non trovato
         if(strcmp(Record->DataAttuale, Chiave1)!=0){
           if(VisMsgBox==true) Application->MessageBox("Record Not Found", "Warning", MB_OK + MB_ICONWARNING + MB_TOPMOST);
           err=2;
           // se non trovato si posiziona sul piu  vicino
           // con l'indice 2 (Timestamp)
           if(trovato){
              long int tempo=0;
              timechar_to_timeint(&Chiave2, Chiave1, "00.00.00");
              tempo=Chiave2;
              NewTable->IndexName = "IDX2";
              NewTable->SetKey();
              sprintf(myBuffer,"%012ld",tempo);
              NewTable->FieldByName("FIELD01_01")->AsString = myBuffer;
           }
           NewTable->GotoNearest();
           TimTgvGetCampiTabella( Record, NewTable);
         }
     }
     //------------------------------------------------------------------
     // Se si è posizionato in fondo segnalo che sono sull'ultimo record
     //------------------------------------------------------------------
     if(NewTable->RecNo==NewTable->RecordCount){
        // commentato perchè il messaggio rompe troppo le pa**e!
        // Application->MessageBox("Last Record Reached", "Warning", MB_OK + MB_ICONWARNING + MB_TOPMOST);
     }
     //-------------------------------------------------------------
     // Compila struttura del Record solo se la tabella ne contiene
     //-------------------------------------------------------------
     if(NewTable->RecordCount>0){
        memset(Record,0,sizeof(Record));
        TimTgvGetCampiTabella( Record, NewTable);
     }
  }
  //----------------------------------------
  // Trattamento delle Eccezzioni
  //----------------------------------------
  catch (EDatabaseError &e){
     strcpy( all_mess, MESS[371] );
     Application->MessageBox(e.Message.c_str(), "ERROR", MB_OK + MB_ICONSTOP + MB_TOPMOST);
     err=1;
  }
  catch(...){
     sprintf(all_mess, "(12) - %s", MESS[371]);
     err=1;
  }
  //-------------------------------
  // Chiusura Tabella
  //-------------------------------
  NewTable->Close();
  delete NewTable;
  return err;
}
//---------------------------------------------------------------------------
// ----------------------------------------
//  void TimTgvCompattaTabella()
// ----------------------------------------
// Copia i campi della struttura Record sui campi omonimi della TTABLE
// N.B. Tabella->TableName deve essere privo di estensione.
// N.B. Questa funzione è stata creata appositamente per il DBASE ma
//      può essere modificata facilmente anche per il PARADOX ecc.
//
void TimTgvCompattaTabella( TTable *Tabella )
{
   char buffer[400];
   char all_mess[451]="";
   DBIResult   rslt;
   CRTblDesc   TblDesc;
   hDBIDb hDb;


   hDb = Tabella->DBHandle;
   memset((void *) &TblDesc, 0, sizeof(CRTblDesc));
   sprintf( TblDesc.szTblName, "%s.dbf", Tabella->TableName.c_str());

   Tabella->Close();
   Tabella->Active=false;

   TblDesc.bPack = TRUE;
   rslt = DbiDoRestructure(hDb, 1, &TblDesc, NULL, NULL, NULL, FALSE);
   if(rslt != DBIERR_NONE){
      DbiGetErrorString( rslt, buffer);
      sprintf(all_mess, "Errore compattazione tabella...", buffer);
      P.error_trasmit(0, all_mess);
      ShowMessage(buffer);
   }

   return;
}
//---------------------------------------------------------------------------
// -----------------------------
//    TimTgvAggiornaDB()
// -----------------------------
// Aggiorna la tabella DB4 contenente tutti i dati dei TGV
//
void TimTgvAggiornaDB()
/*******************************/
{
  short int i, j;
  char buffer[201]="";
  char all_mess[201]="";
  char Date[11]="";
  char Time[11]="";
  char aa[5]="";
  char mm[3]="";
  char gg[3]="";
  char NomeIndice[11]="";
  char NomeField[21]="";
  bool AperturaOk;
  bool risultato, tgv_ok;
  bool Inserimento=false;
  bool Cancellazione, Continua;
  long int TimeAttuale;
  long int TimePrimoRecord;
  long int TimeFineGiornata;
  long int DifSecondi;
  long int DiffTempo;
  long int TimeApp;
  struct TIMTGV TimTgv;
  AnsiString txt=";";
  TTable *TimTgvTable;
  int MaxAgv=MAXAGV;
  SYSTEMTIME  TimeCompatta;


  //--------------------------------------
  // Solo il SERVER deve fare il refresh
  //--------------------------------------
  if(IdPC>1) return;
  //--------------------------------------
  // Se il numero dei TGV in struttura
  // è infeririore a quello nel DB uso
  // riduco il ciclo
  //--------------------------------------
  if(MaxAgv>MAXTGV_TIMTGV) MaxAgv=MAXTGV_TIMTGV;
  //--------------------------------------
  // Cadenza di aggiornamento
  //--------------------------------------
  TimeAttuale=time(NULL);
  if(TimeAttuale-LastRefreshTimTgv<SEC_AGGIORNA_TIMTGV) return;
  //--------------------------------------
  // Inizializzazione variabili
  //--------------------------------------
  TimeApp=0;
  DiffTempo=0;
  strcpy(NomeIndice, "IDX");
  strcpy(NomeField, "Date");
  GetLocalTime( &TimeCompatta );
  memset(&TimTgv, 0, sizeof(TIMTGV));
  timeint_to_timechar(&TimeAttuale, &Date[0], &Time[0]);
  timechar_to_timeint(&TimeFineGiornata, &Date[0], "23.59.59");    // Data/Ora fine giornata
  memcpy(&gg[0], &Date[0], 2);   // giorno
  gg[2]=0;
  memcpy(&mm[0], &Date[3], 2 );  // mese
  mm[2]=0;
  memcpy(&aa[0], &Date[6], 4);   // anno
  aa[4]=0;
  //--------------------------------------
  // Inizializza TTABLE LOCALE
  //--------------------------------------
  TimTgvTable = new TTable(FormMain); // Deve sempre essere legata ad una form!
  try{
     AperturaOk = false;
     Inserimento = false;
     //-----------------------------------------------------
     // Richiama la funzione di inizializzazione dei campi
     // Percorso, Nome tabella ecc..
     //-----------------------------------------------------
     TimTgvInizializzaTabella(PercorsoProduzio, FileTabellaTimTgv, TimTgvTable);
     TimTgvTable->IndexName    = NomeIndice;
     TimTgvTable->Active       = true;
     TimTgvTable->Open();                   // APRI TABELLA
     AperturaOk                = true;
     //------------------------------------------------------
     // Mi posiziono sul Record della Tabella
     //------------------------------------------------------
     TimTgvTable->SetKey();
     TimTgvTable->FieldValues[NomeField]=Date;
     risultato = TimTgvTable->GotoKey();
     if( risultato==true ){
        Inserimento=false;
        TimTgvGetCampiTabella(&TimTgv, TimTgvTable);
     }
     else{
        Inserimento=true;
     }
  }
  //-----------------------------------------------------------------
  // Trattamento delle Eccezzioni:
  // 1 - Errore Apertura Tabella |--> Traccia su file dell'errore.
  //                             |--> Creazione nuova tabella DB4.
  //                             |--> Riapertura della tabella DB4.
  //                             |--> Inserimento nuovo record.
  //
  // 2 - Errore Ricerca Record   |--> Inserimento nuovo record.
  //-----------------------------------------------------------------
  catch(...){
     Inserimento=true;
     if(AperturaOk == false){
        //---------------------------------------------
        // Creazione nuova tabella DB4
        //---------------------------------------------
        if(TimTgvCreaTabella()){
           delete TimTgvTable;
           return;
        }
        //-----------------------------------------------------
        // Richiama la funzione di inizializzazione dei campi
        // Percorso, Nome tabella ecc..
        //-----------------------------------------------------
        TimTgvInizializzaTabella(PercorsoProduzio, FileTabellaTimTgv, TimTgvTable);
        TimTgvTable->IndexName    = NomeIndice;
        TimTgvTable->Active       = true;
        TimTgvTable->Open();                   // APRI TABELLA
        TimTgvTable->SetKey();
        TimTgvTable->FieldValues[NomeField]=Date;
        P.error_trasmit(0, "ERRORE in DB4 TEMPI TGV: (errore apertura tabella)");
     }
     else{
        P.error_trasmit(0, "ERRORE in DB4 TEMPI TGV: (errore ricerca record)");
        //---------------------------------------------
        // Azzeramento struttura TEMPI TGV
        // e del numero di missioni del TGV
        //---------------------------------------------
        memset(&TimTgv, 0, sizeof(TIMTGV));
     }
  }
  //-----------------------------------------------------------
  // Compilazione della struttura dei TEMPI per tutti i TGV
  // (In fase di inserimento di un nuovo record inizializzo
  //  prima alcuni record).
  //-----------------------------------------------------------
  if(Inserimento==true){
     //-------------------------------------------------
     // - Data attuale (indice del DB4 dei TEMPI TGV).
     // - Ultimo azzeramento ha data attuale.
     // - Azzeramento del conteggio delle missioni TGV.
     //-------------------------------------------------
     strcpy(TimTgv.DataAttuale, Date);
     for(i=1; i<=MaxAgv; i++){
        sprintf(TimTgv.Time[i][1], "%012ld", TimeAttuale);  // Time Attuale            (SEMPRE PRESENTE)
        sprintf(TimTgv.Time[i][2], "%012ld", TimeAttuale);  // Time Ultimo azzeramento (SEMPRE PRESENTE)
        sprintf(buffer,"%d", AGV[i]->mission.NumMissioni);
        AGV[i]->mission.NumMissioni=0;
        AGV[i]->save_mission( all_mess );
        if(i<MaxAgv) txt = txt + buffer + ";";
        else         txt = txt + buffer;
     }
     file_cronologico_generico("NewTotMiss.dat", txt.c_str(), 30000L);
  }
  for(i=1; i<=MaxAgv; i++){
     DiffTempo=TimeAttuale-atol(TimTgv.Time[i][1]);
     //-----------------------------------------------------------
     // Aggiorno nuovo Time di aggiornamento DB4
     //-----------------------------------------------------------
     sprintf(TimTgv.Time[i][1], "%012ld", TimeAttuale);
     //-----------------------------------------------------------
     // AL-13/07/2018 solo in condizioni di pronto all'esecuzione
     //               missione perchè deve essere utilizzato per
     //               il calcolo di quanto tempo:
     //               IL TGV E' IN ATTESA SENZA NULLA DA FARE E
     //               PER IL CALCOLO DELL'EFFICENZA IMPIANTO
     //-----------------------------------------------------------
     tgv_ok=true;
     if(AGV[i]->stato.s.bit.funz==false     ) tgv_ok=false;
     if(AGV[i]->stato.s.bit.allarme==true   ) tgv_ok=false;
     if(AGV[i]->stato.s.bit.ris==true       ) tgv_ok=false;
     if(AGV[i]->allarme_interno!=0          ) tgv_ok=false;
     if(AGV[i]->mission.Esclusione==true    ) tgv_ok=false;
     if(AGV[i]->mission.SemiAutomatico==true) tgv_ok=false;
     //-----------------------------------------------------------
     // Incremento dei sec. con Tgv in AUTOMATICO
     //-----------------------------------------------------------
     if(AGV[i]->stato.s.bit.funz && tgv_ok==true){
        TimeApp=atol(TimTgv.Time[i][3]);
        TimeApp=TimeApp+DiffTempo;
        sprintf(TimTgv.Time[i][3], "%012ld", TimeApp);
     }
     //-----------------------------------------------------------
     // Incremento dei sec. con Tgv in SEMIAUTOMATICO
     //-----------------------------------------------------------
     if(AGV[i]->mission.SemiAutomatico){
        TimeApp=atol(TimTgv.Time[i][4]);
        TimeApp=TimeApp+DiffTempo;
        sprintf(TimTgv.Time[i][4], "%012ld", TimeApp);
     }
     //-----------------------------------------------------------
     // Incremento dei sec. con Tgv in RICARICA BATTERIA
     //-----------------------------------------------------------
     if(tgv_ok==true){
        if(AGV[i]->stato.start==0 && (AGV[i]->stato.pos==AGV[i]->mission.PosRicarica || (AGV[i]->stato.pos>0 && AGV[i]->stato.pos<51))){
           TimeApp=atol(TimTgv.Time[i][5]);
           TimeApp=TimeApp+DiffTempo;
           sprintf(TimTgv.Time[i][5], "%012ld", TimeApp);
        }
     }
     //-----------------------------------------------------------
     // Incremento dei sec. con Tgv in RISERVA
     //-----------------------------------------------------------
     if(AGV[i]->stato.s.bit.ris){
        TimeApp=atol(TimTgv.Time[i][6]);
        TimeApp=TimeApp+DiffTempo;
        sprintf(TimTgv.Time[i][6], "%012ld", TimeApp);
     }
     //-----------------------------------------------------------
     // Incremento dei sec. con Tgv in ALLARME
     //-----------------------------------------------------------
     if(AGV[i]->stato.s.bit.allarme && AGV[i]->stato.s.bit.funz){
        TimeApp=atol(TimTgv.Time[i][7]);
        TimeApp=TimeApp+DiffTempo;
        sprintf(TimTgv.Time[i][7], "%012ld", TimeApp);
     }
     //-----------------------------------------------------------
     // Incremento dei sec. con Tgv in MISSIONE ATTIVA
     //-----------------------------------------------------------
     if(AGV[i]->stato.start){
        TimeApp=atol(TimTgv.Time[i][8]);
        TimeApp=TimeApp+DiffTempo;
        sprintf(TimTgv.Time[i][8], "%012ld", TimeApp);
     }
     //-----------------------------------------------------------
     // Aggiornamento del numero delle MISSIONI GIORNALIERE
     //-----------------------------------------------------------
     ltoa(AGV[i]->mission.NumMissioni, TimTgv.Time[i][9], 10);
  }
  //-----------------------------------------------------------
  // Salvataggio del record nella Tabella DB4 dei TEMPI TGV
  //-----------------------------------------------------------
  try{
     if(Inserimento == false){
        TimTgvTable->Edit();                // MODIFICA RECORD GIA' ESISTENTE
     }
     if(Inserimento == true){
        TimTgvTable->Insert();              // INSERIMENTO NUOVO RECORD
     }
     TimTgvSetCampiTabella(&TimTgv, TimTgvTable);
     TimTgvTable->Post();
     LastRefreshTimTgv=TimeAttuale;
  }
  catch(...){
     P.error_trasmit(0, "ERRORE in DB4 TEMPI TGV: (errore gestione record)");
     //---------------------------------------------
     // Creazione nuova tabella DB4
     //---------------------------------------------
     TimTgvTable->Close();
     TimTgvCreaTabella();
     delete TimTgvTable;
     return;
  }
  //-----------------------------------------------------------
  // Eliminazione di tutti i Record più vecchi di un anno
  // ( a parità di giorno e di mese ).
  //-----------------------------------------------------------
  Cancellazione=false;
  try{
     TimTgvTable->First();                   // APRI TABELLA (Primo Record)
     do{
        TimTgvGetCampiTabella(&TimTgv, TimTgvTable);
        TimePrimoRecord = atol(TimTgv.Time[1][1]);
        //----------------------------------------------------
        // Differenza in secondi tra la Data/Ora Attuale e la
        // Data/Ora del primo record memorizzato in archivio.
        //----------------------------------------------------
        DifSecondi = TimeFineGiornata - TimePrimoRecord;
        Continua=false;
        //----------------------------------------------------
        // Se è trascorso un anno:
        // 30*24*60*60 = "2592000" = Secondi in un mese
        //----------------------------------------------------
        TimeApp = (long int)(24*60*60*GIORNI_TIMTGV);
        if(DifSecondi>=TimeApp){
           TimTgvTable->Delete();
           TimTgvTable->First();
           Cancellazione=true;
           Continua=true;
        }
     }while( Continua );
  }
  catch(...){
     P.error_trasmit(0, "ERRORE in DB4 TEMPI TGV: (errore gestione record)");
  }
  //---------------------------------------------
  // Chiusura Tabella e in caso di cancellazione
  // anche compattazione.
  //---------------------------------------------
  if(Cancellazione==false){
     TimTgvTable->Close();
  }
  else{
     //-----------------------------------------------------------
     // Chiusura e compattazione della Tabella
     //-----------------------------------------------------------
     TimTgvCompattaTabella(TimTgvTable);
  }
  delete TimTgvTable;
  return;
}
//---------------------------------------------------------------------------
