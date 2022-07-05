//---------------------------------------------------------------------------
#include <vcl.h>
#include <math.h>
#pragma hdrstop

#include "Nodowin.h"
#include "Remoto.h"
#include "Laser.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormNodi *FormNodi;
//  ---------------------
//   STRUTTURA NODI
//  ---------------------
//  0,1 = x,y posizione centrale nodo
//  2 = numero nodo
//  3 = occupazione agv ( numero AGV oppure libero = 0 )
//-------------------------------------------------------
extern int Punti[][5];
extern int PercorsoCurva[][5];
extern int ConvScriviFileGraphPuntiAscii( char *NomeFile, char* all_mess);
//  ---------------------
//   STRUTTURA PERCORSO
//  ---------------------
//  0 = Numero nodo partenza
//  1 = Numero nodo arrivo
//-------------------------------------------------------
extern int Percorso[][2];

//  -------------------------------------------
//     STRUTTURA AREE SPECIALI
//     Raggruppamento di alcune Zone Primarie
//  -------------------------------------------
//  1  - Numero Area Speciale (es. Area 51 "area militare accesso negato !!")
//  2  - ZpX1
//  3  - ZpX2
//  4  - ZpX3
//  5  - ZpX4
//  6  - ZpX5
//  7  - ZpX6
//  8  - ZpX7
//  9  - ZpX8
// 10  - ZpX9
// 11  - ZpX10
//---------------------------------------------------------
extern int AreaSpeciale[][12];

//  ---------------------
//   GESTIONE CURVE
//  ---------------------
extern int CercaCurva( bool SoloCurveGrafiche, int P1, int P2, int* Raggio, int* Circonferenza );
//---------------------------------------------------------------------------
__fastcall TFormNodi::TFormNodi(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFormNodi::FormActivate(TObject *Sender)
{
  // --- Inizializzazione delle variabili globali ---
  SelPulsanti          = false;
  Errore               = false;
  Inserimento          = false;
  CoordinateModificate = false;
  Indice               = 0;
  NumRec               = 0;

  // --- Dimensionamento della griglia di percorso ---
  PercorsoGrid->DefaultRowHeight = 21;
  PercorsoGrid->DefaultColWidth  = 35;
  PercorsoGrid->ColCount = 11;
  PercorsoGrid->RowCount = MAXPUNTINODO;

  // --- Assegna gli indirizzi all'oggetto condiviso ---
  Gruppi();

  // --- imposta i messaggi degli oggetti ---
  Messaggi();

  // --- Lettura struttura da file  ---
  LeggiDati();
}
//---------------------------------------------------------------------------

// +--------------------------------------------------+
// | Intercettazione dei Pulsanti premuti da Tastiera |
// +--------------------------------------------------+
void __fastcall TFormNodi::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Shift.Contains(ssAlt) && Key==VK_F4 ) return;

  switch (Key){
     case VK_ESCAPE: Close();               break;
     case VK_NEXT:   if(SelPulsanti==true){Indietro();} break;
     case VK_PRIOR:  if(SelPulsanti==true){Avanti();}   break;
   //case 'P':       SommaRigaPercorso();   break;
   //case 'M':       SottraiRigaPercorso(); break;
  }
}
//---------------------------------------------------------------------------

// +-----------------------------------------------------------+
// | Assegna ad un unico oggetto gli indirizzi di vari oggetti |
// +-----------------------------------------------------------+
void __fastcall TFormNodi::Gruppi()
{
  //
  // --- Non utilizzata per questa Form ---
  //
}
//---------------------------------------------------------------------------

// +------------------------------------------+
// | Imposta i messaggi abbinati agli oggetti |
// +------------------------------------------+
void __fastcall TFormNodi::Messaggi()
{
  // --- Intestazione Form ---
  FormNodi->Caption = MESS[271];

  // --- Intestazioni Box ---
  GroupBoxNodi1->Caption = MESS[392];
  GroupBoxNodi2->Caption = MESS[393];
  GroupBoxNodi3->Caption = MESS[394];
  GroupBoxNodi4->Caption = MESS[395];
  GroupBoxNodi5->Caption = MESS[396];
  GroupBoxNodi6->Caption = MESS[389];

  // --- Box "Dati Generali" ---
  TextNodi1->Caption = MESS[397];
  TextNodi2->Caption = MESS[398];
  TextNodi3->Caption = MESS[399];
  TextNodi4->Caption = MESS[400];
  TextNodi5->Caption = MESS[390];
  TextNodi6->Caption = MESS[391];
  CBBloccoRot->Caption  = MESS[154];
  CBInvertiRot->Caption = MESS[155];

  // --- Box "Direzione per manovra "Carico/Scarico" ---
  GroupBoxNodi2->ItemIndex = 0;
  GroupBoxNodi2->Items->Strings[ 0] = "None";
  GroupBoxNodi2->Items->Strings[ 1] = MESS[402];
  GroupBoxNodi2->Items->Strings[ 2] = MESS[403];
  GroupBoxNodi2->ItemIndex = 0;

  // --- Box "Descrizione Nodo" ---
  GroupBoxNodi3->ItemIndex = 0;
  GroupBoxNodi3->Items->Strings[ 0] = MESS[404];
  GroupBoxNodi3->Items->Strings[ 1] = MESS[405];
  GroupBoxNodi3->Items->Strings[ 2] = MESS[406];
  GroupBoxNodi3->Items->Strings[ 3] = MESS[407];
  GroupBoxNodi3->ItemIndex = 0;

  // --- Box "Tipo di Gestione Nodo" ---
  GroupBoxNodi4->ItemIndex = 0;
  GroupBoxNodi4->Items->Strings[ 0] = MESS[408];
  GroupBoxNodi4->Items->Strings[ 1] = MESS[409];
  GroupBoxNodi4->Items->Strings[ 2] = MESS[410];
  GroupBoxNodi4->Items->Strings[ 3] = MESS[411];
  GroupBoxNodi4->ItemIndex = 0;

  // --- Box "Dati Percorso" ---
  BtnSommaPerc->Caption   = MESS[424];
  BtnSottrazPerc->Caption = MESS[425];

  // --- Gestione messaggi del "Pulsanti"---
  BtnIndietro->Caption    = MESS[333];
  BtnAvanti->Caption      = MESS[332];
  BtnCerca->Caption       = MESS[334];
  BtnCopia->Caption       = MESS[335];
  BtnSalva->Caption       = MESS[337];
  BtnStampa->Caption      = MESS[339];
  BtnCancella->Caption    = MESS[338];
  BtnInserimento->Caption = MESS[336];
  BtnEsci->Caption        = MESS[171];
}

//---------------------------------------------------------------------------

// +------------------------+
// | Legge i Dati dai file  |
// +------------------------+
void __fastcall TFormNodi::LeggiDati()
{
  char buffer[201]="";
  int  err;
  int  Rest=0;


  Inserimento=false;
  NumRec=0;

  N.rec_attuale=NodoIniziale;
  setmem (&nod, sizeof(NOD), 0);                  // azzera la struttura
  //-------------------------------------------------------------------------
  // 23/07/2004 Per snellire la comunicazione CLIENT/SERVER
  //            facciamo un controllo dell'ora invece che
  //            copiare sistematicamente il file
  //-------------------------------------------------------------------------
  if( IdPC>1 ){
     //-----------------------------------------
     // Rinfresco DATI
     //-----------------------------------------
     Rest = RemConfrontaDateFile( N.file_name_sorg, N.file_name_dest );
     if( Rest>=1 ){
        //-------------------------------------
        // Rilegge il file
        //-------------------------------------
        err = N.load_file_nodi( buffer );
        if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
  }else{
     //-------------------------------------
     // Rilegge il file
     //-------------------------------------
     err = N.load_file_nodi( buffer );
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }

  //-------------------------------------
  // Recupera il primo record
  //-------------------------------------
  memcpy( &nod, N.n[NodoIniziale], sizeof( NOD) );
  nod.busy = N.nodo_busy[NodoIniziale];
  NumRec = (short int) (N.num_record());
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Avanti()
// ---------------------------
// Funzione che avanza di un record
//
void __fastcall TFormNodi::Avanti()
{
  int err=0, n_rec;

  //--------------------------------
  //  cerca il prossimo record
  //--------------------------------
  for( n_rec=N.rec_attuale+1; n_rec<=MAXPUNTI; n_rec++){
     if( N.n[ n_rec ]->num >0 ) break;
  }
  if( n_rec>MAXPUNTI ){
     Application->MessageBox(MESS[428], MESS[99], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  //------------------------------------
  //  Recupera dati nella struttura mag
  //------------------------------------
  memset(&nod, 0, sizeof(NOD));
  err = N.cerca_record(&nod, n_rec );
  if( err==0 ) N.rec_attuale = (short int) n_rec;
  if(nod.num<=MAXPUNTI) nod.busy = N.nodo_busy[nod.num];
  Inserimento=false;
  Indice = 0;
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Indietro()
// ---------------------------
// Funzione che retrocede di un record
//
void __fastcall TFormNodi::Indietro()
{
  int err=0, n_rec;

  //--------------------------------
  //  cerca il precedente record
  //--------------------------------
  for( n_rec=N.rec_attuale-1; n_rec>0; n_rec--){
     if( N.n[ n_rec ]->num >0 ) break;
  }
  if( n_rec<1 ){
     Application->MessageBox(MESS[429], MESS[99], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  //------------------------------------
  //  Recupera dati nella struttura mag
  //------------------------------------
  memset(&nod, 0, sizeof(NOD));
  err = N.cerca_record(&nod, n_rec );
  if( err==0 ) N.rec_attuale = (short int) n_rec;
  if(nod.num<=MAXPUNTI) nod.busy = N.nodo_busy[nod.num];
  Inserimento=false;
  Indice = 0;
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Cerca()
// ---------------------------
// Funzione che cerca un record
//
void __fastcall TFormNodi::Cerca()
{
  AnsiString InputStringa;
  char Chiave[5]="";
  int n_rec=1, err=0;

  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  InputStringa = InputBox(MESS[430], MESS[431], "");
  if(InputStringa.IsEmpty() == true) return;
  strcpy(Chiave, InputStringa.c_str());
  //----------------------------------------------
  // Controllo della validità del valore inserito
  //----------------------------------------------
  if(atoi(Chiave)<1 || atoi(Chiave)>MAXPUNTI){
     Application->MessageBox(MESS[432], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }
  //------------------------------------
  //  Recupera record
  //------------------------------------
  n_rec = atoi(Chiave);
  memset(&nod, 0, sizeof(NOD));
  err = N.cerca_record(&nod, n_rec );
  if(err) {
     Application->MessageBox(MESS[432], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }
  N.rec_attuale = (short int) n_rec;
  if(nod.num <= MAXPUNTI) nod.busy = N.nodo_busy[nod.num];
  Inserimento=false;
  Indice = 0;
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Copia()
// ---------------------------
// Funzione che copia un record
//
void __fastcall TFormNodi::Copia()
{
  AnsiString InputStringa;
  char buffer[201]="";
  char Chiave[5]="";
  int result;
  int n_rec=1, err=0;


  if(Inserimento) return;
  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  InputStringa = InputBox(MESS[433], MESS[431], "");
  if(InputStringa.IsEmpty() == true) return;
  strcpy(Chiave, InputStringa.c_str());
  //----------------------------------------------
  // Controllo della validità del valore inserito
  //----------------------------------------------
  if(atoi(Chiave)<1 || atoi(Chiave)>MAXPUNTI){
     Application->MessageBox(MESS[432], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  //------------------------------------
  //  Verifica se record esistente
  //------------------------------------
  n_rec = atoi(Chiave);
  if( n_rec == N.rec_attuale ) return;
  if( N.n[ n_rec ]->num >0 ){
     result = Application->MessageBox(MESS[434], MESS[99], MB_YESNO | MB_ICONQUESTION);
     if(result != IDYES) return;
  }

  //-----------------
  //  Copia record
  //-----------------
  memcpy( N.n[ n_rec ], N.n[ N.rec_attuale ], sizeof( NOD ) );
  memcpy( &nod,       N.n[ N.rec_attuale ], sizeof( NOD ) );
  N.rec_attuale     = (short int) n_rec;
  N.n[ n_rec ]->num = (short int) n_rec;
  nod.num         = (short int) n_rec;
  Inserimento=false;
  Indice = 0;

  NumRec = (short int)(N.num_record());
  SetCaselle();
  //-----------------------------------------
  //  Salva record sul file
  //-----------------------------------------
  err = N.save_record_nodi( N.rec_attuale, buffer);
  if( err ) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);

  //$$$$$$$$$$
  //Salva();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Salva()
// ---------------------------
// Funzione che salva il record sul file
//
void __fastcall TFormNodi::Salva()
{
  char buffer[100]="";
//  char NomeFile[100];
  int err =0, result;

  result = Application->MessageBox( MESS[380], MESS[99], MB_YESNO | MB_ICONQUESTION);
  if(result != IDYES) return;
  //-----------------------------------------
  //  Recupera dati dalle caselle di dialogo
  //-----------------------------------------
  GetCaselle();
  if(Errore==true) return;
  if(VerificaInput()) return;
  if(nod.num > MAXPUNTI) return;

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4
/*
  int i=0;
  //-------------------------------------
  // Compilazione lunghi percorsi con
  // Zone specificate.
  //-------------------------------------
  for(i=0; i<=MAXPUNTINODO-1; i++){
     if(nod.rec[i].pt==0 && nod.rec[i].zp==0) break;
     if(i<=1){
        nod.rec[i].ps--;
     }
     else{
        nod.rec[i].ps--;
     }
  }
*/

/*
  //-------------------------------------
  // Compilazione lunghi percorsi con
  // PT a scalare.
  //-------------------------------------
  for(i=0; i<=MAXPUNTINODO-1; i++){
     if(nod.rec[i].pt==0 && nod.rec[i].zp==0) break;
     if(nod.rec[i].ps>600){
        nod.rec[i].ps = nod.rec[i].ps-1;
     }
  }
  for(i=0; i<=MAXPUNTINODO-1; i++){
     if(nod.rec[i].pt==0 && nod.rec[i].zp==0) break;
     if(nod.rec[i].ps<600){
        nod.rec[i+1].ps = nod.rec[i+1].pt;
        nod.rec[i].ps = nod.num+1;
        break;
     }
  }
*/
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4

  Inserimento=false;
  Indice = 0;
  N.nodo_busy[nod.num] = nod.busy;
  N.save_nodo_busy(buffer);
  if(nod.num <= MAXPUNTI) nod.busy = N.nodo_busy[nod.num];

  NumRec = (short int)(N.num_record());
  SetCaselle();
  //-----------------------------------------
  //  Salva sul file
  //-----------------------------------------
  memcpy( N.n[nod.num], &nod, sizeof( NOD) );
  N.rec_attuale = nod.num;
  err = N.save_record_nodi( N.rec_attuale, buffer);
  if( err ) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);

  //-------------------------------------------------------------
  // Traccia su LOG OPERATORE
  //-------------------------------------------------------------
  sprintf(buffer, "(EDITNOD) - (Nodo:%d) Salvataggio Editor Nodi", nod.num);
  FileLogOperatore(buffer);

  //-----------------------------------------
  // Aggiornamento del GRAPHVAR.DAT
  //-----------------------------------------
  ConvScriviFileGraphPuntiAscii("GRAPHVAR.DAT", buffer);

  //-----------------------------------------
  //  Trasmetti comando al SERVER
  //-----------------------------------------
/*
  if( IdPC>1 ){
      if(!err){
          err = RemSincronizzaTimeFile( N.file_name_sorg, N.file_name_dest, buffer );
          if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
          sprintf( NomeFile, "Nod%04d", N.rec_attuale );
          err = RemCreaFileComando ( NomeFile, (BYTE *) N.n[N.rec_attuale], sizeof( NOD ), buffer );
          if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
      }
  }
*/
}
//---------------------------------------------------------------------------

// ---------------------------
//    Inser()
// ---------------------------
// Funzione che abilita disabilita l'inserimento di un record sul file
//
void __fastcall TFormNodi::Inser()
{
  Inserimento = !Inserimento;
  if(Inserimento == true ) setmem( &nod, sizeof(NOD),0);
  if(Inserimento == false) memcpy( &nod, N.n[N.rec_attuale], sizeof( NOD ));

  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//         Cancel()
// ---------------------------
// Funzione che cancella un record
//
void __fastcall TFormNodi::Cancel()
{
  char buffer[100]="";
//  char NomeFile[100];
  int err =0;
  int result, n_rec;

  if( LivelloPassword<2 ) return;
  //----------------------------------------------------
  // Se il file non contiene alcun record allora return
  //----------------------------------------------------
  if(NumRec <= 0){
     Application->MessageBox(MESS[436], MESS[435], MB_OK | MB_ICONINFORMATION);
     return;
  }
  result = Application->MessageBox(MESS[437], MESS[435], MB_YESNO | MB_ICONQUESTION);
  if(result != IDYES) return;

  setmem (&nod, sizeof(NOD), 0);                  // azzera la struttura
  if(nod.num <= MAXPUNTI) nod.busy = N.nodo_busy[nod.num];
  //-----------------------------------------
  //  Salva record sul file
  //-----------------------------------------
  memcpy( N.n[N.rec_attuale], &nod, sizeof( NOD ) );
  err = N.save_record_nodi( N.rec_attuale, buffer);
  if( err ) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);

  //-----------------------------------------
  //  Trasmetti comando al SERVER
  //-----------------------------------------
/*
  if( IdPC>1 ){
      if(!err){
          err = RemSincronizzaTimeFile( N.file_name_sorg, N.file_name_dest, buffer );
          if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
          sprintf( NomeFile, "Nod%04d", N.rec_attuale );
          err = RemCreaFileComando ( NomeFile, (BYTE *) N.n[N.rec_attuale], sizeof( NOD ), buffer );
          if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
      }
  }
*/
  //--------------------------------
  //  cerca il prossimo record
  //--------------------------------
  for( n_rec=N.rec_attuale+1; n_rec<=MAXPUNTI; n_rec++){
     if( N.n[ n_rec ]->num >0 ) break;
  }

  //--------------------------------
  //  Se non trovato
  //  cerca il precedente record
  //--------------------------------
  if( n_rec>MAXPUNTI ){
     for( n_rec=N.rec_attuale-1; n_rec>0; n_rec--){
        if( N.n[ n_rec ]->num >0 ) break;
     }
  }

  //------------------------------------
  //  Recupera dati nella struttura mag
  //------------------------------------
  memset(&nod, 0, sizeof(NOD));
  err = N.cerca_record(&nod, n_rec );

  N.rec_attuale = (short int) n_rec;

  NumRec = (short int)(N.num_record());
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    SommaRigaPercorso()
// ---------------------------
// Funzione che aggiunge una riga nelle informazioni di Percorso
//
void __fastcall TFormNodi::SommaRigaPercorso()
{
  short int i;

  GetCaselle();
  if(Errore==true) return;
  //----------------------------------------------
  // Inserisci fino al limite dell'array
  //----------------------------------------------
  if(nod.rec[MAXPUNTINODO-1].ps==0){
     for(i=MAXPUNTINODO-1; i>Indice; i--){
        memcpy( &nod.rec[i], &nod.rec[i-1], sizeof( rec_linea ));
     }
     memset (&nod.rec[Indice] , 0, sizeof( rec_linea ));
  }
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//   SottraiRigaPercorso()
// ---------------------------
// Funzione che sottrae una riga dalle informazioni di Percorso
//
void __fastcall TFormNodi::SottraiRigaPercorso()
{
  short int i;

  GetCaselle();
  if(Errore==true) return;
  //----------------------------------------------
  // Cancella una riga di percorso
  //----------------------------------------------
  for(i=Indice; i<MAXPUNTINODO-1; i++){
     memcpy (&nod.rec[i], &nod.rec[i+1], sizeof( rec_linea ));
  }
  memset (&nod.rec[MAXPUNTINODO-1] , 0, sizeof( rec_linea ));
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    GetCaselle()
// ---------------------------
// Funzione che legge dalle caselle di dialogo (TEdit) di WINDOWS
//
void __fastcall TFormNodi::GetCaselle()
{
  char Colonna[11]="";
  char buffer[101]="";
  short int i=0;
  int x1, y1, Valore=0;

  Errore=false;
  //------------------------------------------------
  // Box di "Dati Generali"
  //------------------------------------------------
  nod.num  = (short int) atoi( EditNodi1->Text.c_str() );
  nod.zp   = (short int) atoi( EditNodi2->Text.c_str() );
  nod.zs   = (short int) atoi( EditNodi3->Text.c_str() );
  nod.busy = (short int) atoi( EditNodi4->Text.c_str() );
  x1         = atoi(EditNodi5->Text.c_str());
  y1         = atoi(EditNodi6->Text.c_str())*(-1);
  if(Inserimento==false){
     if(Punti[nod.num][0]!=x1) CoordinateModificate=true;
     if(Punti[nod.num][1]!=y1) CoordinateModificate=true;
     Punti[nod.num][0] = x1;
     Punti[nod.num][1] = y1;
  }
  nod.blocco_rot  = CBBloccoRot->Checked;
  nod.inverti_180 = CBInvertiRot->Checked;

  //------------------------------------------------
  // Assegnazione Zona Mapping
  //------------------------------------------------
  if(atoi( EditNodiZoneMap->Text.c_str())>255) EditNodiZoneMap->Text="255";
  nod.ZoneMap = (byte) atoi( EditNodiZoneMap->Text.c_str() );
  //------------------------------------------------
  // Assegnazione piano (non può essere "0")
  //------------------------------------------------
  nod.piano = (short int) atoi( EditNodi7->Text.c_str() );
  if(nod.piano==0) nod.piano=1;
  //------------------------------------------------
  // Gestione nodo Master (valido solo per i nodi
  // Slave)
  //------------------------------------------------
  nod.NodoMaster = (short int) atoi( EditNodi8->Text.c_str() );
  //------------------------------------------------
  // Box di "Direzione manovra di Carico/Scarico"
  //------------------------------------------------
  nod.direz = (short int) GroupBoxNodi2->ItemIndex;

  //------------------------------------------------
  // Appartenenza nodo
  //------------------------------------------------
  nod.corsia = (BYTE)(GroupBoxNodi3->ItemIndex);

  //------------------------------------------------
  // Tipo nodo
  //------------------------------------------------
  nod.typ = (BYTE)(GroupBoxNodi4->ItemIndex);

  //------------------------------------------------
  // Box di "Dati Percorso"
  //------------------------------------------------
  try{
     for(i=0; i<=MAXPUNTINODO-1; i++){
        if(Errore==true){
           Application->MessageBox(buffer, MESS[100], MB_OK|MB_ICONEXCLAMATION);
           return;
        }
        // --- Punto Terminale
        strcpy(Colonna, MESS[413]);
        Valore = atoi(PercorsoGrid->Cells[1][i+1].c_str());
        //if(Valore>=0 && Valore<=TOTPUNTI) nod.rec[i].pt = (short int)(Valore);
        if((Valore>=0 && Valore<=TOTPUNTI) || Valore==PERC_ALTERNATIVO_VELOCE) nod.rec[i].pt = (short int)(Valore);
        else{ sprintf(buffer, MESS[412], Colonna, i+1); Errore=true; continue;}
        // --- Zona Primaria
        strcpy(Colonna, MESS[414]);
        Valore = atoi(PercorsoGrid->Cells[2][i+1].c_str());
      //if(Valore>=0 && Valore<=MAXZP) nod.rec[i].zp = (short int)(Valore);
        if(Valore>=0 && Valore<=PERC_ALTERNATIVO_VELOCE) nod.rec[i].zp = (short int)(Valore);
        else{ sprintf(buffer, MESS[412], Colonna, i+1); Errore=true; continue;}
        // --- Zona Secondaria
        strcpy(Colonna, MESS[415]);
        Valore = atoi(PercorsoGrid->Cells[3][i+1].c_str());
        if(Valore>=0 && Valore<=MAXZS) nod.rec[i].zs    = (short int)(Valore);
        else{ sprintf(buffer, MESS[412], Colonna, i+1); Errore=true; continue;}
     //***// --- Rotazione Iniziale
        strcpy(Colonna, "Pprv");
        Valore = atoi(PercorsoGrid->Cells[4][i+1].c_str());
        if((Valore>=0 && Valore<=TOTPUNTI) || Valore==PT_JOLLY_PERC_ALTERN_PER_ROT){
           nod.rec[i].NodPrecL   = (byte)(Valore&0xff);
           nod.rec[i].NodPrecH   = (byte)(Valore>>8);
        }
        else{ sprintf(buffer, MESS[412], Colonna, i+1); Errore=true; continue;}
     //***
        // --- Punto Sucessivo
        strcpy(Colonna, MESS[416]);
        Valore = atoi(PercorsoGrid->Cells[5][i+1].c_str());
        if(Valore>=0 && Valore<=TOTPUNTI) nod.rec[i].ps = (short int)(Valore);
        else{ sprintf(buffer, MESS[412], Colonna, i+1); Errore=true; continue;}
        // --- Avanti
        strcpy(Colonna, MESS[417]);
        Valore = atoi(PercorsoGrid->Cells[6][i+1].c_str());
        if(Valore>=0 && Valore<=1) nod.rec[i].av        = (short int)(Valore);
        else{ sprintf(buffer, MESS[412], Colonna, i+1); Errore=true; continue;}
        // --- Indietro
        strcpy(Colonna, MESS[418]);
        Valore = atoi(PercorsoGrid->Cells[7][i+1].c_str());
        if(Valore>=0 && Valore<=1) nod.rec[i].ind       = (short int)(Valore);
        else{ sprintf(buffer, MESS[412], Colonna, i+1); Errore=true; continue;}
        // --- Rotazione
        strcpy(Colonna, MESS[419]);
        Valore = atoi(PercorsoGrid->Cells[8][i+1].c_str());
        if(Valore>=0 && Valore<=3) nod.rec[i].rot       = (short int)(Valore);
        else{ sprintf(buffer, MESS[412], Colonna, i+1); Errore=true; continue;}
        // --- Rallentamento
        strcpy(Colonna, MESS[420]);
        Valore = atoi(PercorsoGrid->Cells[9][i+1].c_str());
        if(Valore>=0 && Valore<20) nod.rec[i].ral       = (short int)(Valore);
        else{ sprintf(buffer, MESS[412], Colonna, i+1); Errore=true; continue;}
        // --- Distanza
      //  strcpy(Colonna, MESS[421]);
      //  Valore = atoi(PercorsoGrid->Cells[10][i+1].c_str());
      //  if(Valore>=0 && Valore<=9999) nod.rec[i].dist   = (short int)(Valore);
      //  else{ sprintf(buffer, MESS[412], Colonna, i+1); Errore=true; continue;}
     }
     //--------------------------------------------------
     // Recupero l'indice della riga di percorso attuale
     //--------------------------------------------------
     Indice = (short int)(PercorsoGrid->Row-1);
  }
  //----------------------------------------
  // Trattamento delle Eccezzioni
  //----------------------------------------
  catch(...){
     Errore==true;
     sprintf(buffer, MESS[412], Colonna, i+1);
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
}
//---------------------------------------------------------------------------

// ---------------------------
//    SetCaselle()
// ---------------------------
// Funzione che forza i nuovi valori nelle caselle di dialogo (TEdit)
//
void __fastcall TFormNodi::SetCaselle()
{
  short int i=0;
  char Appoggio[81]="";

  //------------------------------------------------
  // Box di "Dati Generali"
  //------------------------------------------------
  sprintf(Appoggio, "%04d", nod.num);
  EditNodi1->Text = Appoggio;
  sprintf(Appoggio, "%02d", nod.zp);
  EditNodi2->Text = Appoggio;
  sprintf(Appoggio, "%02d", nod.zs);
  EditNodi3->Text = Appoggio;
  sprintf(Appoggio, "%02d", nod.busy);
  EditNodi4->Text = Appoggio;
  EditNodi5->Text = Punti[nod.num][0];
  EditNodi6->Text = Punti[nod.num][1]*(-1);
  EditNodi7->Text = nod.piano;
  sprintf(Appoggio, "%04d", nod.NodoMaster);
  EditNodi8->Text = Appoggio;
  CBBloccoRot->Checked  = nod.blocco_rot;
  CBInvertiRot->Checked = nod.inverti_180;

  //------------------------------------------------
  // Assegnazione Zona Mapping
  //------------------------------------------------
  sprintf(Appoggio, "%03d", nod.ZoneMap);
  EditNodiZoneMap->Text = Appoggio;
  //------------------------------------------------
  // Disattivazione dei campi per le coordinate se
  // in nodo è invisibile
  //------------------------------------------------
  if(Punti[nod.num][4]!=0){
     EditNodi5->Enabled = true;
     EditNodi6->Enabled = true;
     UpDown5->Enabled   = true;
     UpDown6->Enabled   = true;
  }
  else{
     EditNodi5->Enabled = false;
     EditNodi6->Enabled = false;
     UpDown5->Enabled   = false;
     UpDown6->Enabled   = false;
  }

  //------------------------------------------------
  // Box di "Stato Missione"
  //------------------------------------------------
  GroupBoxNodi2->ItemIndex = nod.direz;

  //------------------------------------------------
  // Box di "appartenzana"
  //------------------------------------------------
  GroupBoxNodi3->ItemIndex = nod.corsia;

  //------------------------------------------------
  // Box di "Tipo di Gestione Nodo"
  //------------------------------------------------
  GroupBoxNodi4->ItemIndex = nod.typ;

  //------------------------------------------------
  // Box di "Dati Percorso"
  //------------------------------------------------
  for(i=0; i<=MAXPUNTINODO; i++){     //i=colonna
     for(int k=0, j=0; j<=10; j++){
        k=0;
        //Inserimento etichette tabella
        if(i==0 && j>0){         //Se colonna j >0
           if(j> 4){ k=-1;}
           if(j==4) strcpy(Appoggio, "Pprv");          //teso colonna 4 fisso
           if(j!=4) strcpy(Appoggio, MESS[412+j+k]);    //Testo contenente etichetta colonna j-esima
           PercorsoGrid->Cells[j][0] = Appoggio;        //Selezione della prima riga (indice 0, usata per intestazione colonne) colonna j-esima
           continue;
        }
        switch(j){
           case  0: PercorsoGrid->Cells[0][i] = IntToStr(i);       break;
           case  1: PercorsoGrid->Cells[1][i] = nod.rec[i-1].pt;   break;
           case  2: PercorsoGrid->Cells[2][i] = nod.rec[i-1].zp;   break;
           case  3: PercorsoGrid->Cells[3][i] = nod.rec[i-1].zs;   break;
           case  4: PercorsoGrid->Cells[4][i] =(nod.rec[i-1].NodPrecL + (nod.rec[i-1].NodPrecH<<8)); break;
           case  5: PercorsoGrid->Cells[5][i] = nod.rec[i-1].ps;   break;
           case  6: PercorsoGrid->Cells[6][i] = nod.rec[i-1].av;   break;
           case  7: PercorsoGrid->Cells[7][i] = nod.rec[i-1].ind;  break;
           case  8: PercorsoGrid->Cells[8][i] = nod.rec[i-1].rot;  break;
           case  9: PercorsoGrid->Cells[9][i] = nod.rec[i-1].ral;  break;
           case 10: PercorsoGrid->Cells[10][i] = nod.rec[i-1].dist; break;
           default: break;
        }
     }
  }
  //--------------------------------------------------
  // Gestione "Panel informativi"
  //--------------------------------------------------
  if(Inserimento==false){                         // Editazione/Inserimento
     EditPanel->Caption = MESS[439];
     EditPanel->Color   = clLime;
  }
  if(Inserimento==true){
     EditPanel->Caption = MESS[440];
     EditPanel->Color   = clYellow;
  }

  sprintf(Appoggio, MESS[441], NumRec);         // Numero record presenti in Archivio
  NRecordPanel->Caption = Appoggio;
  //-----------------------------------------------------------
  // 18/04/2014 Abilitazione Abbinamento Zona Mapping al Nodo
  //-----------------------------------------------------------
  if( Laser_AbilitaZonaMapSuNodo) GroupZoneMapping->Visible=true;
  if(!Laser_AbilitaZonaMapSuNodo) GroupZoneMapping->Visible=false;

}
//---------------------------------------------------------------------------

// ---------------------------
//    VerificaInput()
// ---------------------------
// Questa funzione fa il controllo di validit… dati introdotti nel nodo
// prima di efettuare il salvataggio sul file
//
int __fastcall TFormNodi::VerificaInput()
{
  char buffer[200];
  short int i,j;
  short int p1, p2;
  int distX, distY;
  int dx1_3, dy1_3, dx2_3, dy2_3;
  int r, d, x1, x2, y1, y2, x3, y3;
  double dist;
  bool uguali=false;
  bool area_speciale;
  int Id, Raggio, Circonferenza;
  bool BloccoDistanza = BLOCCO_MODIFICA_DISTANZA_DA_EDITOR_NODI;
  short int NodoPrec1, NodoPrec2;
  short int ps9, av9, ind9, rot9, ral9, dist9;
  

  //-------------------------------------------
  // sono ammesse solo delle ZP comprese tra
  // 0 e MAXZP o comprese nei percorsi speciali
  //-------------------------------------------
  for(i=0; i<MAXPUNTINODO; i++){
     if( nod.rec[i].zp>=0 && nod.rec[i].zp<=MAXZP ) continue;
     if( nod.rec[i].zp==PERC_ALTERNATIVO_VELOCE   ) continue;
     area_speciale=false;
     for(j=1; j<=20; j++){
        if( AreaSpeciale[j][0]==0             ) break;
        if( AreaSpeciale[j][0]!=nod.rec[i].zp ) continue;
        area_speciale=true;
        break;
     }
     //-----------------------------
     // ZP NON AMMESSA
     //-----------------------------
     if(area_speciale==false){
        sprintf(buffer, "ZP %02d non ammessa...", nod.rec[i].zp);
        Application->MessageBox( buffer , MESS[99], MB_OK | MB_ICONINFORMATION);
        return 1;
     }
  }

  //-------------------------------------------
  // Verifica punti terminali (pt) unici !
  //-------------------------------------------
  for(i=0; i<MAXPUNTINODO; i++){
     for(j=0; j<MAXPUNTINODO; j++){
        if(i!=j){
           if( nod.rec[i].pt>0 && nod.rec[i].pt==nod.rec[j].pt){
              if( nod.rec[i].pt!=PERC_ALTERNATIVO_VELOCE && nod.rec[j].pt!=PERC_ALTERNATIVO_VELOCE &&
                  nod.rec[i].zp!=PERC_ALTERNATIVO_VELOCE && nod.rec[j].zp!=PERC_ALTERNATIVO_VELOCE ){
                 NodoPrec1=(nod.rec[i].NodPrecL + (nod.rec[i].NodPrecH<<8));
                 NodoPrec2=(nod.rec[j].NodPrecL + (nod.rec[j].NodPrecH<<8));
                 if(NodoPrec1==NodoPrec2){
                    uguali=true;
                    sprintf(buffer, MESS[442], nod.rec[i].pt);
                    break;
                 }
              }
           }
           if(uguali==true)break;
        }
        if(uguali==true)break;
     }
  }

  //------------------------------------------------
  // Verifico se il nodo per il quale sono state
  // modificate le coordinate appartiene a una
  // curva.
  //------------------------------------------------
  if(CoordinateModificate){
     for(i=0; i<1001; i++){
        if(PercorsoCurva[i][0]==0 && PercorsoCurva[i][1]==0) continue;
        if(PercorsoCurva[i][0]!=nod.num && PercorsoCurva[i][1]!=nod.num) continue;
        r =PercorsoCurva[i][3];
        x1=Punti[PercorsoCurva[i][0]][0];
        y1=Punti[PercorsoCurva[i][0]][1];
        x2=Punti[PercorsoCurva[i][1]][0];
        y2=Punti[PercorsoCurva[i][1]][1];
        //---------------------------------------------------------
        // Cerco un punto equidistante da i due punti della curva
        // che abbia una coordinata in comune.
        //---------------------------------------------------------
        for(j=1; j<=MAXPUNTI; j++){
           if(Punti[j][2]==0) break;
           if(j==PercorsoCurva[i][0] || j==PercorsoCurva[i][1]) continue;
           x3=Punti[j][0];
           y3=Punti[j][1];
           //-------------------------------------
           // Calcolo le distanza delle coorinate
           // (tolleranza +/- 3cm.)
           //-------------------------------------
           dx1_3 = abs(x1-x3);
           dy1_3 = abs(y1-y3);
           dx2_3 = abs(x2-x3);
           dy2_3 = abs(y2-y3);
           if(dx1_3<4 && dy2_3<4){
            //sprintf(buffer, "P1:%d - P2:%d - P3:%d", PercorsoCurva[i][0], PercorsoCurva[i][1], j);
            //ShowMessage(buffer);
              d=abs(dy1_3-dx2_3);
              if(d<4){r=dy1_3; break;}
           }
           if(dy1_3<4 && dx2_3<4){
            //sprintf(buffer, "P1:%d - P2:%d - P3:%d", PercorsoCurva[i][0], PercorsoCurva[i][1], j);
            //ShowMessage(buffer);
              d=abs(dx1_3-dy2_3);
              if(d<4){r=dx1_3; break;}
           }
        }
        //--------------------------------
        // Modifica raggio della curva
        //--------------------------------
        PercorsoCurva[i][3] = r;
     }
  }
  //-------------------------------------------
  // Se la distanza è stata impostata nulla
  // viene ricalcolta automaticamente da
  // graphvar.cpp
  //-------------------------------------------
  p1 = nod.num;
  for(i=0; i<MAXPUNTINODO; i++){
     if(nod.rec[i].ps==0) break;
     dist = 0;
     p2 = nod.rec[i].ps;
     if(nod.rec[i].dist==0 || CoordinateModificate || BloccoDistanza==true){
        x1 = Punti[p1][0];
        y1 = Punti[p1][1];
        x2 = Punti[p2][0];
        y2 = Punti[p2][1];
        //----------------------------------
        // Se i nodi si trovano in retta la
        // distanza viene calcolata con il
        // teorema di pitagora.
        //----------------------------------
        distX=abs(x1-x2);
        distY=abs(y1-y2);
        if(distX>30000 || distY>30000) continue;
        dist = (double)(sqrt((distX*distX)+(distY*distY)));
        //----------------------------------
        // Calcolo della distanza percorsa
        // su un arco di circonferneza.
        //----------------------------------
        Id=Raggio=Circonferenza=0;
        if(!N.CercaCurva(TRUE, p1, p2, &Raggio, &Circonferenza, &Id)){
           dist = (short int)(Circonferenza);
        }
        nod.rec[i].dist=(short int)(dist);
     }
  }
  //-------------------------------------------
  // 14/03/97 Verifica le occorrenze dei punti
  // sucessivi e imposta lo stesso valore di
  // distanza .
  //-------------------------------------------
  for(i=0; i<MAXPUNTINODO; i++){
     if(nod.rec[i].ps==0) break;
     //-------------------------------------------
     // Se punto sucessivo Š lo stesso forza lo
     // stesso valore di distanza.
     //-------------------------------------------
     for(j=(short int)(i+1); j<MAXPUNTINODO; j++ ){
        if( nod.rec[i].ps == nod.rec[j].ps ){
           nod.rec[j].dist = nod.rec[i].dist;
        }
     }
  }
  //-----------------------------------------
  // Azzero il flag di Coordinare modificate
  //-----------------------------------------
  if(CoordinateModificate){
     CoordinateModificate=false;
     //-----------------------------------------------------
     // Se all'interno dell'impianto esiste almeno un TGV
     // dotato di gestione laser setto il flag per ricordare
     // l'attivazione del MAPPING
     //-----------------------------------------------------
     for(i=1; i<=MAXAGV; i++){
        if(!AGV[i]->mission.GestioneLaser) continue;
        RiattivaMapping[i]=true;
     }
  }
  //---------------------------------------
  //  fine controllo
  //---------------------------------------
  if(uguali){
     Application->MessageBox( buffer , MESS[99], MB_OK | MB_ICONINFORMATION);
     return 1;
  }
  return 0;
}
//---------------------------------------------------------------------------
// ---------------------------
//    CompilaNodoMaster()
// ---------------------------
// Funzione che compila un Nodo MASTER
//
void __fastcall TFormNodi::CompilaNodoMaster()
{
  #define MAXSLAVE 50

  AnsiString InputStringa;
  char Chiave[5]="";
  int result;
  int NodoMaster;
  int CtrSlave;
  int ArraySlave[ MAXSLAVE+2 ];
  int TotSlave;
  int i=0;
  bool FilaTipoFIFO=false;
  int  NodoUscita;
  int  prec, succ;
  int  NodDaRicercare, NodoDaEvitare, NodoSucessivo;
  bool FineRicerca=false;
  bool SmpreON=true;
  int  TotSlaveFifo=0;//, NodoUscitaFifo=0;
  int   NodoIngressoFifo=0;


  NodoMaster = N.rec_attuale;
  //------------------------------------------------------------
  // Verifica che sia stata Compilata la prima riga del record
  // con le indicazioni del nodo di uscita di default
  //------------------------------------------------------------
  if((nod.rec[0].pt!=0 || nod.rec[0].zp!=0 || nod.rec[0].zs!=0) && nod.rec[0].ps==0){
     Application->MessageBox("Mancano nella prima riga le indicazioni per il nodo di uscita!", MESS[99],  MB_OK);
     return;
  }
  NodoUscita = nod.rec[0].ps;
  //------------------------------------------------------------
  // Verifica se la cella è di tipo LIFO o NO
  //------------------------------------------------------------
  result = Application->MessageBox( "FILA da compilare di tipo LIFO ?", MESS[99], MB_YESNO | MB_ICONQUESTION);
  if(result != IDYES) FilaTipoFIFO=true;

  //-----------------------------------------------
  // Inserire Numero di SLAVE per cella tipo FIFO
  //-----------------------------------------------
  if( FilaTipoFIFO==true ){
     InputStringa = InputBox("FILA TIPO FIFO", "Specificare numero di posizioni nella FILA", "");
     //if(InputStringa.IsEmpty() == true) return;
     strcpy(Chiave, InputStringa.c_str());
     TotSlaveFifo = atoi( Chiave )-1;

     //----------------------------------------------
     // Controllo della validità del valore inserito
     //----------------------------------------------
     if( TotSlaveFifo<1 || TotSlaveFifo>MAXPUNTI ){
        Application->MessageBox("Numero di posizioni errata!", MESS[100], MB_OK | MB_ICONEXCLAMATION);
        return;
     }
  }

  //-----------------------------------------------
  // Ricerca del nodo di Ingresso alla cella FIFO
  //-----------------------------------------------
  if( FilaTipoFIFO==true ){
     NodDaRicercare =  NodoMaster;
     NodoDaEvitare  =  NodoUscita;
     for(i=0;;i++){
        prec      = Percorso[i][0];
        succ      = Percorso[i][1];
        //-----------------------
        // fine percorso -> esci
        //-----------------------
        if(prec==0 && succ==0){  break;     }
        //----------------------------------------------------------------------------
        // Trova indicazione di percorso per il primo NODO SLAVE
        // - Cerca nella struttura percorsi la prima corrispondenza con NODOMASTER
        // - Che non sia il NODO di USCITA ( ricerca come LIFO)
        //----------------------------------------------------------------------------
        if( prec!=NodDaRicercare && succ!=NodDaRicercare ) continue;
        if( prec==NodoDaEvitare  || succ==NodoDaEvitare  ) continue;
        if( prec==NodDaRicercare ) NodoIngressoFifo=succ;
        if( succ==NodDaRicercare ) NodoIngressoFifo=prec;
        break;
     }
  }


  //--------------------------------------------------
  // Compila Array Slave appartenenti a questo MASTER
  //--------------------------------------------------
  memset( ArraySlave, 0, sizeof(ArraySlave));
//  RicercaSlaveIndietro=false;
  TotSlave=0;
  if( SmpreON==true ){

     //-----------------------
     // Trova Indice iniziale
     //-----------------------
     NodDaRicercare=NodoMaster; NodoDaEvitare = NodoUscita;
     if( FilaTipoFIFO==true )   NodoDaEvitare = NodoIngressoFifo;

     FineRicerca=false;
     //--------------------------------------------------
     // Cerca SLAVE
     //--------------------------------------------------
     for( CtrSlave=1; CtrSlave<=MAXSLAVE; CtrSlave++){
        for(i=0;;i++){
           prec      = Percorso[i][0];
           succ      = Percorso[i][1];
           //-----------------------
           // fine percorso -> esci
           //-----------------------
           if(prec==0 && succ==0){  FineRicerca=true; break;     }
           //----------------------------------------------------------------------------
           // Trova indicazione di percorso per il primo NODO SLAVE
           // - Cerca nella struttura percorsi la prima corrispondenza con NODOMASTER
           // - Che non sia il NODO di USCITA ( ricerca come LIFO)
           //----------------------------------------------------------------------------
           if( prec!=NodDaRicercare && succ!=NodDaRicercare ) continue;
           if( prec==NodoDaEvitare  || succ==NodoDaEvitare  ) continue;
           if( prec==NodDaRicercare ) NodoSucessivo=succ;
           if( succ==NodDaRicercare ) NodoSucessivo=prec;

           //----------------------------------------------------------------
           // In caso di cella di tipo FIFO con uscita opposta all'ingresso
           // Memorizzo Nodo di uscita e tronco ricerca
           //----------------------------------------------------------------
           if( FilaTipoFIFO ){
              if( TotSlave >= TotSlaveFifo ) {
                 FineRicerca=true; break;
              }
           }

           NodoDaEvitare           =NodDaRicercare;
           NodDaRicercare          =NodoSucessivo;
           ArraySlave[ CtrSlave ]  =NodoSucessivo;

           TotSlave++;
           break;
        }

        //-----------------------------
        // Fine ricerca SLAVE
        //-----------------------------
        if( FineRicerca==true) break;
     }
     //-----------------------
     // Nessuno SLAVE trovato
     //-----------------------
     if( TotSlave==0 ){
        Application->MessageBox("Nessuno Slave Trovato!", MESS[99],  MB_OK);
        return;
     }
  }

  //--------------------------------------------------
  // Compila Record MASTER
  //--------------------------------------------------
  for( CtrSlave=1; CtrSlave<=TotSlave; CtrSlave++){
     if( CtrSlave >= (MAXPUNTINODO-1) ) break;
     nod.rec[CtrSlave].pt= (short int ) ArraySlave[ CtrSlave ];
     nod.rec[CtrSlave].ps= (short int ) ArraySlave[ 1 ];
  }

  return;
}
//---------------------------------------------------------------------------
// ---------------------------
//    CreaNodiSlave()
// ---------------------------
// Funzione che crea e compila un nodo SLAVE
//
void __fastcall TFormNodi::CreaNodiSlave()
{
  AnsiString InputStringa;
  char  buffer[201]="";
  int   result;
  int   err=0;
  int   NodoMaster;
  int   CtrSlave;
  int   ArraySlave[ 50];
  int   TotSlave;
  int   i=0;
  int   Slave;
  int   Linea;
  int   prec, succ;
  int   NodDaRicercare, NodoDaEvitare;
  bool  FilaTipoFIFO=false;
  int   NodoIngressoFifo=0;

  NodoMaster = N.rec_attuale;
  memset( ArraySlave, 0, sizeof(ArraySlave));

  //------------------------------------------------------------
  // Verifica se la cella è di tipo LIFO o NO
  //------------------------------------------------------------
  result = Application->MessageBox( "FILA da compilare di tipo LIFO ?", MESS[99], MB_YESNO | MB_ICONQUESTION);
  if(result != IDYES) FilaTipoFIFO=true;

  //--------------------------------------------------
  // Compila Array Slave appartenenti a questo MASTER
  //--------------------------------------------------
  CtrSlave=0;
  for(i=0; i<MAXPUNTINODO-1; i++){
     if( nod.rec[i].pt==PERC_ALTERNATIVO_VELOCE ) continue;
     if( nod.rec[i].pt ){
        ArraySlave[ CtrSlave+1 ]=nod.rec[i].pt;
        CtrSlave++;
     }
  }
  ArraySlave[ 0 ]=NodoMaster;
  TotSlave=CtrSlave;


  //-----------------------------------------------
  // Ricerca del nodo di Ingresso alla cella FIFO
  //-----------------------------------------------
  if( FilaTipoFIFO==true ){
     NodDaRicercare =  ArraySlave[ CtrSlave   ];
     NodoDaEvitare  =  ArraySlave[ CtrSlave-1 ];
     for(i=0;;i++){
        prec      = Percorso[i][0];
        succ      = Percorso[i][1];
        //-----------------------
        // fine percorso -> esci
        //-----------------------
        if(prec==0 && succ==0){  break;     }
        //----------------------------------------------------------------------------
        // Trova indicazione di percorso per il primo NODO SLAVE
        // - Cerca nella struttura percorsi la prima corrispondenza con NODOMASTER
        // - Che non sia il NODO di USCITA ( ricerca come LIFO)
        //----------------------------------------------------------------------------
        if( prec!=NodDaRicercare && succ!=NodDaRicercare ) continue;
        if( prec==NodoDaEvitare  || succ==NodoDaEvitare  ) continue;
        if( prec==NodDaRicercare ) NodoIngressoFifo=succ;
        if( succ==NodDaRicercare ) NodoIngressoFifo=prec;
        break;
     }
  }

  //--------------------------------------------------
  // Compila Record SLAVE
  //--------------------------------------------------
  for( CtrSlave=1; CtrSlave<=TotSlave; CtrSlave++){
     //------------------------------------
     //  Verifica se record esistente
     //------------------------------------
     Slave = ArraySlave[ CtrSlave ];
     if( N.n[ Slave ]->num >0 ){
        sprintf( buffer, "Il nodo %d esiste già. Sovrascrivere?", Slave);
        result = Application->MessageBox( buffer, MESS[99], MB_YESNO | MB_ICONQUESTION);
        if(result != IDYES) continue;
     }
     //------------------------------------
     //  Compila Record
     //------------------------------------
     memcpy( N.n[ Slave ], N.n[ N.rec_attuale ], sizeof( NOD ) );
     memset( N.n[ Slave ]->rec, 0, sizeof( N.n[0]->rec));
     N.n[ Slave ]->num = (short int) Slave;
     N.n[ Slave ]->NodoMaster= (WORD) NodoMaster;
     N.n[ Slave ]->rec[0].ps= (WORD) ArraySlave[ CtrSlave-1];
     //-----------------------------------------------
     // Se Cella di tipo FIFO compila PS in direzione
     // uscita FIFO
     //-----------------------------------------------
     if( FilaTipoFIFO==true ){
        N.n[ Slave ]->rec[0].ps= (WORD) ArraySlave[ CtrSlave+1];
        if( N.n[ Slave ]->rec[0].ps==0 ) N.n[ Slave ]->rec[0].ps = (WORD) NodoIngressoFifo;
     }

     Linea=1;
     for( i=0; i<=TotSlave; i++){
        //---------------------------------------
        // Con destinazione se stesso --> SKIP
        //---------------------------------------
        if( i==CtrSlave ) continue;

        N.n[ Slave ]->rec[ Linea ].pt = (WORD)  ArraySlave[ i ];
        //---------------------------------------
        // Con destinazione Precedente
        //---------------------------------------
        if( i<CtrSlave ) N.n[ Slave ]->rec[ Linea ].ps = (WORD)  ArraySlave[ CtrSlave-1];
        //---------------------------------------
        // Con destinazione Sucessiva
        //---------------------------------------
        if( i>CtrSlave ) N.n[ Slave ]->rec[ Linea ].ps = (WORD)  ArraySlave[ CtrSlave+1];
        Linea++;
     }


     //-----------------------------------------
     //  Salva record sul file
     //-----------------------------------------
     err = N.save_record_nodi( Slave, buffer);
     if( err ) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
}
//---------------------------------------------------------------------------
// +--------------------------------------------------------+
// | Gestione dei "Range" nei campi di "Edit" del           |
// +--------------------------------------------------------+
void __fastcall TFormNodi::Range(TObject *Sender)
{
  int Min,Max,Val;

  switch(((TMaskEdit *)Sender)->Tag) {
     case  1: Min= 0; Max=MAXPUNTI; break;       // Numero Nodo
     case  2: Min= 0; Max=MAXZP;    break;       // Zona Primaria   (Nodo)
     case  3: Min= 0; Max=MAXZS;    break;       // Zona Secondaria (Nodo)
     case  4: Min= 0; Max=MAXAGV;   break;       // Occupato (numero del TGV)
  }
  Val=atoi(((TMaskEdit *)Sender)->Text.c_str());

  if ( Val > Max ) Val=Max;
  if ( Val < Min ) Val=Min;
  ((TMaskEdit *)Sender)->Text=Val;
}
//---------------------------------------------------------------------------

void __fastcall TFormNodi::BtnCercaClick(TObject *Sender)
{
  SelPulsanti=true;
  Cerca();
}
//---------------------------------------------------------------------------

void __fastcall TFormNodi::BtnCopiaClick(TObject *Sender)
{
  SelPulsanti=true;
  Copia();
}
//---------------------------------------------------------------------------

void __fastcall TFormNodi::BtnSalvaClick(TObject *Sender)
{
  SelPulsanti=true;
  Salva();
}
//---------------------------------------------------------------------------
void __fastcall TFormNodi::BtnStampaClick(TObject *Sender)
{
  bool OkStampa;

  SelPulsanti = true;
  OkStampa = PrintDialog->Execute();
  if(OkStampa == false) return;
  FormNodi->Print();
}
//---------------------------------------------------------------------------
void __fastcall TFormNodi::BtnCancellaClick(TObject *Sender)
{
  SelPulsanti=true;
  Cancel();
}
//---------------------------------------------------------------------------
void __fastcall TFormNodi::BtnInserimentoClick(TObject *Sender)
{
  SelPulsanti=true;
  Inser();
}
//---------------------------------------------------------------------------
void __fastcall TFormNodi::BtnAvantiClick(TObject *Sender)
{
  SelPulsanti=true;
  Avanti();
}
//---------------------------------------------------------------------------
void __fastcall TFormNodi::BitCompilaMasterClick(TObject *Sender)
{
  SelPulsanti=true;
  CompilaNodoMaster();
  SetCaselle();
  //------------------------
  // Salvataggio
  //------------------------
  Salva();
}
//---------------------------------------------------------------------------
void __fastcall TFormNodi::BtnCreaSlaveClick(TObject *Sender)
{
  SelPulsanti=true;
  CreaNodiSlave();
}
//---------------------------------------------------------------------------
void __fastcall TFormNodi::BtnIndietroClick(TObject *Sender)
{
  SelPulsanti=true;
  Indietro();
}
//---------------------------------------------------------------------------
void __fastcall TFormNodi::BtnSommaPercClick(TObject *Sender)
{
  SommaRigaPercorso();
}
//---------------------------------------------------------------------------
void __fastcall TFormNodi::BtnSottrazPercClick(TObject *Sender)
{
  SottraiRigaPercorso();
}
//---------------------------------------------------------------------------
void __fastcall TFormNodi::PercorsoGridClick(TObject *Sender)
{
  SelPulsanti=false;
  //--------------------------------------------------
  // Recupero l'indice della riga di percorso attuale
  //--------------------------------------------------
  Indice = (short int)(PercorsoGrid->Row-1);
}
//---------------------------------------------------------------------------
void __fastcall TFormNodi::PercorsoGridGetEditMask(TObject *Sender,
      int ACol, int ARow, AnsiString &Value)
{
  Value = "!9999;1; ";
}
//---------------------------------------------------------------------------
void __fastcall TFormNodi::PercorsoGridSelectCell(TObject *Sender, int Col, int Row, bool &CanSelect)
{
  bool BloccoDistanza = BLOCCO_MODIFICA_DISTANZA_DA_EDITOR_NODI;

  //--------------------------------------------------
  // Blocco della colla di editazione distanza
  //--------------------------------------------------
  if(BloccoDistanza==true && Col==10){
     CanSelect=false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormNodi::PercorsoGridDrawCell(TObject *Sender, int ACol,
      int ARow, TRect &Rect, TGridDrawState State)
{
  short int NodoPrec1;
  TColor col = clWhite;
  //----------------------------------------------------
  // Riga non valida
  //----------------------------------------------------
  if(ARow<=0 || ARow>=MAXPUNTINODO) return;
  //----------------------------------------------------
  // Colonna 4 Pprv (Nodo precedente)
  //----------------------------------------------------
  if(ACol==4 ){
     //----------------------------------------------------
     // Colorazione del blocchetto
     //----------------------------------------------------
     Rect.Left=Rect.Right-5;
     col = clWhite;
     NodoPrec1=(nod.rec[ARow-1].NodPrecL + (nod.rec[ARow-1].NodPrecH<<8));
     if(NodoPrec1>=1 ) col = clFuchsia;
     PercorsoGrid->Canvas->Brush->Color = col;
     PercorsoGrid->Canvas->FillRect(Rect);
  }
}
//---------------------------------------------------------------------------

