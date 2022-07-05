
//---------------------------------------------------------------------------
#include <vcl.h>
#include <vcl/system.hpp>

#pragma hdrstop

#include "Baiwin.h"
#include "Remoto.h"
#include "SQLDB.h"
#include "BoxWin.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormBaie *FormBaie;

// --------------------------
//   STRUTTURE GRAFICHE
// --------------------------
//
extern int Baie[][4];

//---------------------------------------------------------------------------
__fastcall TFormBaie::TFormBaie(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormBaie::FormActivate(TObject *Sender)
{
  //--------------------------------------------------
  // Abilitazione del Timer della Form
  //--------------------------------------------------
  TimerClose->Enabled=true;
  //--------------------------------------------------
  // Editor macchine aperti
  //--------------------------------------------------
  // --- Inizializzazione delle variabili globali ---
  Inserimento = false;
  NumRec      = 0;
  memset(&EdBoxA, 0, sizeof(BOX));
  memset(&EdBoxB, 0, sizeof(BOX));

  //--------------------------------------------------
  // Recupero l'indice della riga di percorso attuale
  //--------------------------------------------------
  IndicePosBaia         = 0;            // posizionamento su pos di prelievo
  PCDatiPal->ActivePage = TSPos1;
  IndiceBaia            = NumeroBaia;
  EditorBaieAttivo      = NumeroBaia;

  // --- Assegna gli indirizzi all'oggetto condiviso ---
  Gruppi();

  // --- imposta i messaggi degli oggetti ---
  Messaggi();

  // --- Lettura struttura da file  ---
  LeggiDati();
  //--------------------------------------------
  if(break_Com[2]==true){ CBForcA->Enabled=true ; CBForcB->Enabled=true;  }
  else                  { CBForcA->Enabled=false; CBForcB->Enabled=false; }
  //--------------------------------------------
  // LIVELLO "0" TUTTO IL PANEL PRICIPALE
  // DISATTIVATO
  //--------------------------------------------
  if(LivelloPassword<1){
     PanBaia->Enabled=false;
     //---------------------------------------
     // Dal GroupBox delle esclusioni che non
     // non appartiene al Panel Principale
     // lasci attivava solo l'esclusione
     //---------------------------------------
     CBBlSovrap->Enabled=false;
     CBEsclCom->Enabled=false;
  }
  else{
     PanBaia->Enabled=true;
     CBBlSovrap->Enabled=true;
     CBEsclCom->Enabled=true;
  }

  //--------------------------------------------------
  // Disattivazioni finestra con Password inferiore
  // a Livello 3.
  //--------------------------------------------------
  if(LivelloPassword<3){
     EdAltez->Enabled        = false;
     EdSollev->Enabled       = false;
     BtnCopia->Enabled       = false;
     BtnCancella->Enabled    = false;
     BtnInserimento->Enabled = false;
     //--------------------------------------------
     // Andrea 15/05/2018
     //--------------------------------------------
     EdNum->Enabled = false;
     EdNod->Enabled = false;
     EdPlc->Enabled = false;
     GBTipo->Enabled = false;
     CBGruppo->Enabled = false;
  }
  else{
     EdAltez->Enabled        = true;
     EdSollev->Enabled       = true;
     BtnCopia->Enabled       = true;
     BtnCancella->Enabled    = true;
     BtnInserimento->Enabled = true;
     //--------------------------------------------
     // Andrea 15/05/2018
     //--------------------------------------------
     EdNum->Enabled = true;
     EdNod->Enabled = true;
     EdPlc->Enabled = true;
     GBTipo->Enabled = true;
     CBGruppo->Enabled = true;
  }
  //------------------------------------------
  // Recupero dati di apertura pagina
  //------------------------------------------
  memcpy(&EdBoxA, &P.EditBaia.BoxA, sizeof(BOX));
  memcpy(&EdBoxB, &P.EditBaia.BoxB, sizeof(BOX));
  //---------------------------------------------
  // Refresh della Form
  //---------------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormBaie::AzzeraTimerClick(TObject *Sender)
{
  //------------------------------------------------------
  // Disabilita/Abilita Timer per riprendere il conteggio
  // del tempo
  //------------------------------------------------------
  TimerClose->Enabled = false;
  TimerClose->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormBaie::AzzeraTimerMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
  //------------------------------------------------------
  // Disabilita/Abilita Timer per riprendere il conteggio
  // del tempo
  //------------------------------------------------------
  TimerClose->Enabled = false;
  TimerClose->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormBaie::TimerCloseTimer(TObject *Sender)
{
  FormBaie->Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormBaie::FormClose(TObject *Sender, TCloseAction &Action)
{
  //----------------------------------------------
  // Diabilitazione del Timer di chiusura
  //----------------------------------------------
  TimerClose->Enabled=false;
  //----------------------------------------------
  // Editor macchine chiuso
  //----------------------------------------------
  NumeroBaia=0;
  EditorBaieAttivo=0;
}
//---------------------------------------------------------------------------
// +--------------------------------------------------+
// | Intercettazione dei Pulsanti premuti da Tastiera |
// +--------------------------------------------------+
void __fastcall TFormBaie::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Shift.Contains(ssAlt) && Key==VK_F4 ) return;
  switch (Key) {
     case VK_ESCAPE: Close();    break;
     case VK_NEXT:   Indietro(); break;
     case VK_PRIOR:  Avanti();   break;
  }
}
//---------------------------------------------------------------------------

// +-----------------------------------------------------------+
// | Assegna ad un unico oggetto gli indirizzi di vari oggetti |
// +-----------------------------------------------------------+
void __fastcall TFormBaie::Gruppi()
{
  CBPresa[0][0] = CBPos1B;
  CBPresa[1][0] = CBPos2B;
  CBPresa[2][0] = CBPos3B;
  CBPresa[0][1] = CBPos1A;
  CBPresa[1][1] = CBPos2A;
  CBPresa[2][1] = CBPos3A;
}
//---------------------------------------------------------------------------

// +------------------------------------------+
// | Imposta i messaggi abbinati agli oggetti |
// +------------------------------------------+
void __fastcall TFormBaie::Messaggi()
{
  int  i, codice;
  char buffer[51]="";

  //------------------------------------------
  // Caption della form
  //------------------------------------------
  FormBaie->Caption          = MESS[327];

  RGDati->Caption            = MESS[464];

  GBTipo->Caption            = MESS[465];
  GBTipo->Items->Strings[ 0] = MESS[466];
  GBTipo->Items->Strings[ 1] = MESS[467];
  GBTipo->Items->Strings[ 2] = MESS[468];

  CBChiamata->Caption   = MESS[469];
  CBEsclCom->Caption    = MESS[470];
  CBEsclM->Caption      = MESS[471];
  CBNoMagaz->Caption    = MESS[472];
  CBNoTgvDisp->Caption  = MESS[473];

  BtnBoxA->Caption      = MESS[528];
  BtnBoxB->Caption      = MESS[528];

  lblStato->Caption     = MESS[474];
  lblPriorita->Caption  = MESS[479];
  //------------------------------------------
  // Gruppo di appartenenza
  //------------------------------------------
  CBGruppo->Clear();
  for(i=0; i<MAXGRUPPIIMP; i++){
     if(strcmp(P.GruppiBaie[i].Nome, "")==0) continue;
     CBGruppo->Items->Add(P.GruppiBaie[i].Nome);
  }

  //------------------------------------------
  // Gruppo di destinazione
  //------------------------------------------
  CBGruppoDest->Clear();
  CBGruppoDest->Items->Add("AUTO-DESTINATION");
  for(i=1; i<MAXGRUPPIIMP; i++){
     if(strcmp(P.GruppiBaie[i].Nome, "")==0) continue;
     CBGruppoDest->Items->Add(P.GruppiBaie[i].Nome);
  }

  //------------------------------------------
  // Tipi codice ammessi
  //------------------------------------------
  CBStato->Clear();
  CBStato->Items->Add(MESS[518]); // NESSUNA
  //--------------------------------------------
  // ABBINA_CODICE_A_STATO_BOX
  //--------------------------------------------
  if(AbbinaCodiceStatoBox==true){
     codice=ASS_COD1;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD1);
     else          strcpy(buffer, "");
     CBStato->Items->Add(buffer);
     codice=ASS_COD2;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD2);
     else          strcpy(buffer, "");
     CBStato->Items->Add(buffer);
     codice=ASS_COD3;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD3);
     else          strcpy(buffer, "");
     CBStato->Items->Add(buffer);
     codice=ASS_COD4;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD4);
     else          strcpy(buffer, "");
     CBStato->Items->Add(buffer);
     codice=ASS_COD5;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD5);
     else          strcpy(buffer, "");
     CBStato->Items->Add(buffer);
     codice=ASS_COD6;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD6);
     else          strcpy(buffer, "");
     CBStato->Items->Add(buffer);
     codice=ASS_COD7;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD7);
     else          strcpy(buffer, "");
     CBStato->Items->Add(buffer);
     codice=ASS_COD8;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD8);
     else          strcpy(buffer, "");
     CBStato->Items->Add(buffer);
  }
  else{
     strcpy(buffer, ASS_LAV1); CBStato->Items->Add(buffer);
     strcpy(buffer, ASS_LAV2); CBStato->Items->Add(buffer);
     strcpy(buffer, ASS_LAV3); CBStato->Items->Add(buffer);
     strcpy(buffer, ASS_LAV4); CBStato->Items->Add(buffer);
     strcpy(buffer, ASS_LAV5); CBStato->Items->Add(buffer);
     strcpy(buffer, ASS_LAV6); CBStato->Items->Add(buffer);
     strcpy(buffer, ASS_LAV7); CBStato->Items->Add(buffer);
     strcpy(buffer, ASS_LAV8); CBStato->Items->Add(buffer);
  }
  CBStato->Items->Add(MESS[530]);        // VUOTO

  //------------------------------------------
  // Compilazione lista tipi box
  //------------------------------------------
  CBTipPal->Clear();
  for(i=0; i<MAXTIPOBOXIMP; i++){
     if(strcmp(M.TipiBox[i].Nome, "")==0) break;
     CBTipPal->Items->Add(M.TipiBox[i].Nome);
  }
  //------------------------------------------
  // Pulsanti standard
  //------------------------------------------
  BtnIndietro->Caption       = MESS[333];
  BtnAvanti->Caption         = MESS[332];
  BtnCerca->Caption          = MESS[334];
  BtnCopia->Caption          = MESS[335];
  BtnSalva->Caption          = MESS[337];
  BtnStampa->Caption         = MESS[339];
  BtnCancella->Caption       = MESS[338];
  BtnInserimento->Caption    = MESS[336];
  BtnEsci->Caption           = MESS[171];
  //------------------------------------------
  TXNum->Caption             = MESS[300];
  TXUrg->Caption             = MESS[675];
  TXNodo->Caption            = MESS[673];
  TXAltez->Caption           = MESS[712];
  lblGruppo->Caption         = MESS[714];
  lblSollev->Caption         = MESS[713];
  GBStatoBaia->Caption       = MESS[463];
  CBBlSovrap->Caption        = MESS[702];
  GBPlc->Caption             = MESS[703];
  lblIDPLC->Caption          = MESS[704];
  CBAutomatico->Caption      = MESS[497];
  CBBlocco->Caption          = MESS[705];
  CBTuttoPieno->Caption      = MESS[706];
  CBRicIng->Caption          = MESS[385];
  CBTgvIng->Caption          = MESS[387];
  CBFineOp->Caption          = MESS[688];
  CBOkIng->Caption           = MESS[686];
  CBOkForcAlte->Caption      = MESS[689];
  lblPallet->Caption         = MESS[707];
  lblCodice->Caption         = MESS[459];
  lblLotto->Caption          = MESS[460];
  lblAltezza->Caption        = MESS[461];
  CBSovrap->Caption          = MESS[462];
  CBCarW->Caption            = MESS[708];
  CBBasso->Caption           = MESS[709];
  CBRuotato->Caption         = MESS[710];
  gbPresa->Caption           = MESS[711];
}
//---------------------------------------------------------------------------

// +------------------------+
// | Legge i Dati dai file  |
// +------------------------+
void __fastcall TFormBaie::LeggiDati()
{
  int  posti, err;
  int  i, j, TotPiani;
  char all_mess[300];

  P.rec_attuale = IndiceBaia;

  setmem(&P.EditBaia, sizeof(BAIE), 0);             // azzera la struttura
  //-------------------------------------
  // FORZATURA LETTURA
  //-------------------------------------
  if(break_Com[COM2]==false){
     err = P.load_chiamate_baie(all_mess);
     if( err) Application->MessageBox(all_mess, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }

  //-------------------------------------
  // Recupera il primo record
  //-------------------------------------
  memcpy( &P.EditBaia, P.ba[IndiceBaia], sizeof( BAIE ) );
  NumRec = P.num_record_baie();
  //--------------------------------------------------
  // Annullamento visualizzazione TabSheet di
  // posizioni inesistenti
  //--------------------------------------------------
  posti = Baie[IndiceBaia][1];          // Massimo numero cestoni
  if(posti<1) posti=1;
  switch(posti){
     case  1: TSPos1->TabVisible=true;
              TSPos2->TabVisible=false;
              TSPos3->TabVisible=false;
              TSPos4->TabVisible=false;
              TSPos5->TabVisible=false;
              break;
     case  2: TSPos1->TabVisible=true;
              TSPos2->TabVisible=true;
              TSPos3->TabVisible=false;
              TSPos4->TabVisible=false;
              TSPos5->TabVisible=false;
              break;
     case  3: TSPos1->TabVisible=true;
              TSPos2->TabVisible=true;
              TSPos3->TabVisible=true;
              TSPos4->TabVisible=false;
              TSPos5->TabVisible=false;
              break;
     case  4: TSPos1->TabVisible=true;
              TSPos2->TabVisible=true;
              TSPos3->TabVisible=true;
              TSPos4->TabVisible=true;
              TSPos5->TabVisible=false;
              break;
     case  5: TSPos1->TabVisible=true;
              TSPos2->TabVisible=true;
              TSPos3->TabVisible=true;
              TSPos4->TabVisible=true;
              TSPos5->TabVisible=true;
              break;
     default: TSPos1->TabVisible=true;
              TSPos2->TabVisible=false;
              TSPos3->TabVisible=false;
              TSPos4->TabVisible=false;
              TSPos5->TabVisible=false;
              break;
  }
  //--------------------------------------------------
  // Abilita solo prese ammessa da baia
  //--------------------------------------------------
  TotPiani = PIANIINPRESA;
  for(i=0; i<Baie[IndiceBaia][2]; i++){
     for(j=0; j<TotPiani; j++){
        CBPresa[i][j]->Visible = true;
     }
     for(j=TotPiani; j<2; j++){
        CBPresa[i][j]->Visible = false;   // piani non attivi su posizione presenti
     }
  }
  for(i=Baie[IndiceBaia][2]; i<3; i++){
     for(j=0; j<2; j++){
        CBPresa[i][j]->Visible = false;
     }
  }
  //--------------------------------------------------
  // Cambio la dicitura solo per presenze sigole
  //--------------------------------------------------
  CBPresa[0][0]->Caption = "1 L";
  CBPresa[0][0]->Width   = 46;
  if(Baie[IndiceBaia][2]==1 && TotPiani==1){
     CBPresa[0][0]->Caption = "Presenza pallet";
     CBPresa[0][0]->Width   = 120;
  }
}
//---------------------------------------------------------------------------
// ---------------------------
//    Avanti()
// ---------------------------
// Funzione che avanza di un record
//
void __fastcall TFormBaie::Avanti()
{
  int err=0;
  int n_rec;
  struct BAIE bb;

  //--------------------------------
  //  cerca il prossimo record
  //--------------------------------
  for( n_rec=P.rec_attuale+1; n_rec<=MAXBAIE; n_rec++){
     if( P.ba[ n_rec ]->Num >0 ) break;
  }

  if( n_rec>MAXBAIE ){
     Application->MessageBox(MESS[428], MESS[99], MB_OK | MB_ICONINFORMATION);
     return;
  }

  //------------------------------------
  //  Recupera dati nella struttura ch
  //------------------------------------
  memset(&bb, 0, sizeof(BAIE));
  err = P.cerca_record_baie(&bb, n_rec );
  if( err==0 ){
     P.rec_attuale  = (short int) n_rec;
     IndiceBaia = (short int) n_rec;

     //memcpy( &P.EditBaia, &bb, sizeof( BAIE ) );
     LeggiDati();
  }
  Inserimento=false;
  // --- Refresh della Form


  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Indietro()
// ---------------------------
// Funzione che retrocede di un record
//
void __fastcall TFormBaie::Indietro()
{
  int err=0;
  int n_rec;
  struct BAIE bb;

  //--------------------------------
  //  cerca il precedente record
  //--------------------------------
  for( n_rec=P.rec_attuale-1; n_rec>0; n_rec--){
     if( P.ba[ n_rec ]->Num >0 ) break;
  }
  if( n_rec<1 ){
     Application->MessageBox(MESS[429], MESS[99], MB_OK | MB_ICONINFORMATION);
     return;
  }

  //------------------------------------
  //  Recupera dati nella struttura ch
  //------------------------------------
  memset(&bb, 0, sizeof(BAIE));
  err = P.cerca_record_baie(&bb, n_rec );
  if( err==0 ){
     P.rec_attuale  = (short int) n_rec;
     IndiceBaia = (short int) n_rec;
     //memcpy( &P.EditBaia, &bb, sizeof( BAIE ) );
     LeggiDati();
  }
  Inserimento=false;
  // --- Refresh della Form
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Cerca()
// ---------------------------
// Funzione che cerca un record
//
void __fastcall TFormBaie::Cerca()
{
  AnsiString InputStringa;
  char Chiave[5]="";
  int n_rec=1, err=0;
  struct BAIE bb;

  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  InputStringa = InputBox( MESS[446],MESS[447], "");
  if(InputStringa.IsEmpty() == true) return;
  strcpy(Chiave, InputStringa.c_str());
  n_rec = atoi(Chiave);
  //----------------------------------------------
  // Controllo della validità del valore inserito
  //----------------------------------------------
  if(n_rec<0 || n_rec>MAXBAIE){
     Application->MessageBox(MESS[375], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  //------------------------------------
  //  Recupera record
  //------------------------------------
  memset(&bb, 0, sizeof(BAIE));
  err = P.cerca_record_baie(&bb, n_rec );
  if(err) {
     Application->MessageBox(MESS[375], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  P.rec_attuale  = (short int) n_rec;
  IndiceBaia = (short int) n_rec;
  memcpy( &P.EditBaia, &bb, sizeof( BAIE ));
  Inserimento=false;
  // --- Refresh della Form
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Copia()
// ---------------------------
// Funzione che copia un record MACCHINE
//
void __fastcall TFormBaie::Copia()
{
  AnsiString InputStringa;
  char buffer[201]="";
  char Chiave[5]="";
  int result;
  int n_rec=1, err=0;

  if(Inserimento)         return;
  if(LivelloPassword<2  ) return;
  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  InputStringa = InputBox( MESS[448], MESS[447], "");
  if(InputStringa.IsEmpty() == true) return;
  strcpy(Chiave, InputStringa.c_str());

  n_rec = atoi(Chiave);
  //----------------------------------------------
  // Controllo della validità del valore inserito
  //----------------------------------------------
  if(n_rec<0 || n_rec>MAXBAIE){
     Application->MessageBox(MESS[375], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  //------------------------------------
  //  Verifica se record esistente
  //------------------------------------
  if( n_rec == P.rec_attuale ) return;
  if( P.ba[ n_rec ]->Num >0  ){
     result = Application->MessageBox(MESS[376], MESS[99], MB_YESNO | MB_ICONQUESTION);
     if(result != IDYES) return;
  }

  //-----------------
  //  Copia record
  //-----------------
  memcpy( P.ba[ n_rec ], P.ba[ P.rec_attuale ], sizeof( BAIE ) );
  memcpy( &P.EditBaia,   P.ba[ P.rec_attuale ], sizeof( BAIE ) );
  P.rec_attuale       = (short int) n_rec;
  IndiceBaia          = (short int) n_rec;
  P.ba[ n_rec ]->Num  = (short int) n_rec;
  P.EditBaia.Num      = (short int) n_rec;

  Inserimento=false;

  NumRec = P.num_record_baie();
  //-----------------------------------------
  // Ricompilazione dei Punti notevoli
  //-----------------------------------------
  err = N.compila_punti_notevoli(buffer);
  if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //-----------------------------------------
  //  Salva record sul file
  //-----------------------------------------
  err = P.save_record_baie( P.rec_attuale, buffer);
  if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //-----------------------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------------------
  sprintf(buffer, "(EDITMAC) - (Bai:%d) Copia Editor Baia", IndiceBaia);
  FileLogOperatore(buffer);
  //-----------------------------------------
  // Rinfresco della finestra
  //-----------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Salva()
// ---------------------------
// Funzione che salva il record sul file
//
void __fastcall TFormBaie::Salva()
{
  int  err=0;
  char buffer[120]="";
  char NomeFile[100]="";
  int  i, j, w, result;
  bool almeno_un_box_a_terra;
  bool nessuna_chiamata_attiva;
  short int count_in_presa;

  //----------------------------------------------------------------
  // Richiesta di conferma salvataggio
  //----------------------------------------------------------------
  result = Application->MessageBox(MESS[380], MESS[99], MB_YESNO | MB_ICONQUESTION);
  if(result != IDYES) return;
  //----------------------------------------------------------------
  //  Recupera dati dalle caselle di dialogo
  //----------------------------------------------------------------
  GetCaselle();
  if(P.EditBaia.Num>MAXBAIE) return;
  //----------------------------------------------------------------
  // Se la macchina è di tipo sconosciuto viene automaticamente
  // esclusa
  //----------------------------------------------------------------
  if(P.EditBaia.Tipo==NN ) P.EditBaia.Esclusione=true;

  //----------------------------------------------------------------
  // Verifica che il PLC impostato sia di tipo SIEMENS
  //----------------------------------------------------------------
//if(P.EditBaia.Plc!=0 && OmrPlc[P.EditBaia.Plc].Com!=COM_SIEMENS){
//   ShowMessage("ATTENTION: Plc not SIEMENS!");
//   P.EditBaia.Plc=0;
//   SetCaselle();
//   return;
//}

  //----------------------------------------------------------------
  // Controllo sull'urgenza assegnata alle stazioni
  //----------------------------------------------------------------
  if(P.EditBaia.Urgenza<0) P.EditBaia.Urgenza=0;
  if(P.EditBaia.Urgenza>9) P.EditBaia.Urgenza=9;

  //----------------------------------------------------------------
  // Lo stato della posizione "TipoCodice" viene messo !=0 solo
  // se la posizione relativa ha l'informazione di codice compilata
  //----------------------------------------------------------------
  if(AbbinaCodiceStatoBox==true){
     for(i=0; i<MAXPOSBAIA; i++){
        P.EditBaia.TipoCod[i]=LIBERA;
        if(P.EditBaia.Codice[i]!=0){
           if(P.EditBaia.Codice[i]==ASS_COD1   ) P.EditBaia.TipoCod[i]=ST_BOX_LAV1;
           if(P.EditBaia.Codice[i]==ASS_COD2   ) P.EditBaia.TipoCod[i]=ST_BOX_LAV2;
           if(P.EditBaia.Codice[i]==ASS_COD3   ) P.EditBaia.TipoCod[i]=ST_BOX_LAV3;
           if(P.EditBaia.Codice[i]==ASS_COD4   ) P.EditBaia.TipoCod[i]=ST_BOX_LAV4;
           if(P.EditBaia.Codice[i]==ASS_COD5   ) P.EditBaia.TipoCod[i]=ST_BOX_LAV5;
           if(P.EditBaia.Codice[i]==ASS_COD6   ) P.EditBaia.TipoCod[i]=ST_BOX_LAV6;
           if(P.EditBaia.Codice[i]==ASS_COD7   ) P.EditBaia.TipoCod[i]=ST_BOX_LAV7;
           if(P.EditBaia.Codice[i]==ASS_COD8   ) P.EditBaia.TipoCod[i]=ST_BOX_LAV8;
           if(P.EditBaia.Codice[i]==ASS_VUOTO  ) P.EditBaia.TipoCod[i]=ST_BOX_VUOTI;
        }
     }
  }

  //-------------------------------------------------------------
  // Controlli sulla configurazione della presa
  // 1 - se i pallet in presa sono ALTI non è possibile
  //     il pallet al secondo piano
  // 2 - sui pallet ai piani alti deve essere sempre
  //     presente il corrispondente a terra
  //-------------------------------------------------------------
  for(w=0; w<MAXPOSBAIA; w++){
     // (1) pallet alti MAI sovrapposti in presa
     if(P.EditBaia.Basso[w]==false){
        for(i=0; i<POSINPRESA; i++){
           for(j=1; j<PIANIINPRESA; j++){       // ciclo da 1° piano
              //------------------------------------------
              // metto a "0" le presenze ai piani alti se
              // manca il corrispondente al piano basso
              //------------------------------------------
              P.EditBaia.Presa[w][i][j]=false;
           }
        }
     }
     // (2) reset presenza piano superiore se inferiore non prensente
     for(i=0; i<POSINPRESA; i++){
        for(j=1; j<PIANIINPRESA; j++){          // ciclo da 1° piano
           if(P.EditBaia.Presa[w][i][j  ]==false) continue;
           //------------------------------------------
           // Se il corrispondente al piano precedente
           // è presente --> continue!
           //------------------------------------------
           if(P.EditBaia.Presa[w][i][j-1]==true ) continue;
           //------------------------------------------
           // metto a "0" le presenze ai piani alti se
           // manca il corrispondente al piano basso
           //------------------------------------------
           P.EditBaia.Presa[w][i][j]=false;
        }
     }
  }

  //----------------------------------------------------------------
  // Verifico se c'è almeno una presenza in presa bassa
  //----------------------------------------------------------------
  count_in_presa=0;
  almeno_un_box_a_terra=false;
  for(i=0; i<POSINPRESA; i++){
     for(j=0; j<PIANIINPRESA; j++){
        if(P.EditBaia.Presa[0][i][j]==false) continue;
        count_in_presa++;
        if(j==0) almeno_un_box_a_terra=true;
     }
  }

  //----------------------------------------------------------------
  // Aggiornamento presenze su baia
  //
  // SOLO BASSA (no predisposizione doppia forca) e con almeno una
  // peresenza in presa
  //----------------------------------------------------------------
  P.EditBaia.PresCEST_A = LIBERA;
  P.EditBaia.PresCEST_B = LIBERA;
  memset(&P.EditBaia.BoxA, 0, sizeof(BOX));
  memset(&P.EditBaia.BoxB, 0, sizeof(BOX));
  //----------------------------------------------------------------
  // Memorizzo il totale di pallet in presa a terra
  //----------------------------------------------------------------
  P.EditBaia.StatoPres = count_in_presa;
  if(almeno_un_box_a_terra==true){
     // BASSA
     if(P.EditBaia.TipoCod[0]!=LIBERA){
        switch(P.EditBaia.TipoCod[0]){
           case ST_BOX_LAV1: P.EditBaia.PresCEST_B = PRES_LAV1;  break;
           case ST_BOX_LAV2: P.EditBaia.PresCEST_B = PRES_LAV2;  break;
           case ST_BOX_LAV3: P.EditBaia.PresCEST_B = PRES_LAV3;  break;
           case ST_BOX_LAV4: P.EditBaia.PresCEST_B = PRES_LAV4;  break;
           case ST_BOX_LAV5: P.EditBaia.PresCEST_B = PRES_LAV5;  break;
           case ST_BOX_LAV6: P.EditBaia.PresCEST_B = PRES_LAV6;  break;
           case ST_BOX_LAV7: P.EditBaia.PresCEST_B = PRES_LAV7;  break;
           case ST_BOX_LAV8: P.EditBaia.PresCEST_B = PRES_LAV8;  break;
           case ST_BOX_VUOTI:P.EditBaia.PresCEST_B = PRES_VUOTO; break;
                    default: P.EditBaia.PresCEST_B = PRES_VUOTO; break;
        }
        P.EditBaia.BoxB.StatoLav    = P.EditBaia.TipoCod[0];
        P.EditBaia.BoxB.TipoBox     = P.EditBaia.TipoBox[0];
        P.EditBaia.BoxB.Codice      = P.EditBaia.Codice[0];
        P.EditBaia.BoxB.Altezza     = P.EditBaia.Altezza[0];
        P.EditBaia.BoxB.CarWash     = P.EditBaia.CarWash[0];
        P.EditBaia.BoxB.Basso       = P.EditBaia.Basso[0];
        P.EditBaia.BoxB.Sovrap      = P.EditBaia.Sovrap[0];
        P.EditBaia.BoxB.Ruotato180  = P.EditBaia.Ruotato180[0];
        P.EditBaia.BoxB.Lotto       = P.EditBaia.Lotto[0];            //§§§
        memcpy(&P.EditBaia.BoxB.Presa[0][0], &P.EditBaia.Presa[0][0][0], sizeof(P.EditBaia.BoxB.Presa));
        P.EditBaia.BoxB.GruppoProv  = P.EditBaia.Gruppo;           // non utilizzate per ALL GLASS
        P.EditBaia.BoxB.GruppoDest  = P.EditBaia.GruppoDest;       // non utilizzate per ALL GLASS
     }
     // ALTA  (solo se viene spuntata ed è presente la BASSA)
     if(P.EditBaia.TipoCod[0]!=LIBERA && CBForcA->Checked==true){
        switch(P.EditBaia.TipoCod[0]){
           case ST_BOX_LAV1: P.EditBaia.PresCEST_A = PRES_LAV1;  break;
           case ST_BOX_LAV2: P.EditBaia.PresCEST_A = PRES_LAV2;  break;
           case ST_BOX_LAV3: P.EditBaia.PresCEST_A = PRES_LAV3;  break;
           case ST_BOX_LAV4: P.EditBaia.PresCEST_A = PRES_LAV4;  break;
           case ST_BOX_LAV5: P.EditBaia.PresCEST_A = PRES_LAV5;  break;
           case ST_BOX_LAV6: P.EditBaia.PresCEST_A = PRES_LAV6;  break;
           case ST_BOX_LAV7: P.EditBaia.PresCEST_A = PRES_LAV7;  break;
           case ST_BOX_LAV8: P.EditBaia.PresCEST_A = PRES_LAV8;  break;
           case ST_BOX_VUOTI:P.EditBaia.PresCEST_A = PRES_VUOTO; break;
                    default: P.EditBaia.PresCEST_A = PRES_VUOTO; break;
        }
        P.EditBaia.BoxA.StatoLav    = P.EditBaia.TipoCod[0];
        P.EditBaia.BoxA.TipoBox     = P.EditBaia.TipoBox[0];
        P.EditBaia.BoxA.Codice      = P.EditBaia.Codice[0];
        P.EditBaia.BoxA.Altezza     = P.EditBaia.Altezza[0];
        P.EditBaia.BoxA.CarWash     = P.EditBaia.CarWash[0];
        P.EditBaia.BoxA.Basso       = P.EditBaia.Basso[0];
        P.EditBaia.BoxA.Sovrap      = P.EditBaia.Sovrap[0];
        P.EditBaia.BoxA.Ruotato180  = P.EditBaia.Ruotato180[0];
        P.EditBaia.BoxA.Lotto       = P.EditBaia.Lotto[0];            //§§§
        memcpy(&P.EditBaia.BoxA.Presa[0][0], &P.EditBaia.Presa[0][0][0], sizeof(P.EditBaia.BoxA.Presa));
        P.EditBaia.BoxA.GruppoProv  = P.EditBaia.Gruppo;           // non utilizzate per ALL GLASS
        P.EditBaia.BoxA.GruppoDest  = P.EditBaia.GruppoDest;       // non utilizzate per ALL GLASS
     }
  }
  //----------------------------------------------------------------
  // RECUPERO DATI UDC PRODOTTO DA STRUTTURA BOX TEMPORANEA
  //----------------------------------------------------------------
  memcpy(&P.EditBaia.BoxA.Prd, &EdBoxA.Prd, sizeof(P.EditBaia.BoxA.Prd));
  memcpy(&P.EditBaia.BoxB.Prd, &EdBoxB.Prd, sizeof(P.EditBaia.BoxB.Prd));

  //----------------------------------------------------------------
  // GESTIONE CHIAMATA
  // Viene abbassata automaticamente se baia sconosciuta
  //----------------------------------------------------------------
  if(P.EditBaia.Tipo==BA_PRELIEVO && almeno_un_box_a_terra==false) P.EditBaia.Chiamata=false;
  if(P.EditBaia.Tipo==BA_DEPOSITO && almeno_un_box_a_terra==true ) P.EditBaia.Chiamata=false;

  //----------------------------------------------------------------
  // Ricompilazione dei Punti notevoli
  //----------------------------------------------------------------
  if(Inserimento==true){
     err = N.compila_punti_notevoli(buffer);
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }

  //----------------------------------------------------------------
  //  Salva sul file
  //----------------------------------------------------------------
  Inserimento = false;
  memcpy( P.ba[P.EditBaia.Num], &P.EditBaia, sizeof( BAIE ) );
  P.rec_attuale = P.EditBaia.Num;
  IndiceBaia    = P.EditBaia.Num;
  NumRec        = P.num_record_baie();
  err = P.save_record_baie( P.rec_attuale, buffer);
  if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //----------------------------------------------------------------
  // GESTIONE UDC
  // da stuttura software ad array UDC per aggiornamento SQL
  //
  // !!! IMPORTANTE !!!
  // dopo aver aggiornato la struttura globale del software
  //----------------------------------------------------------------
  if(GestioneUdc>0){
     // -- REFRESH DATI UDC
     if(!err){
        err = B.StructToUdcTmp(CMD_UDC_REFRESH, 0, 0, 0, P.EditBaia.Num, UDC_ALL_STZ, UDC_ALL_POS, UDC_ALL_LEV, N_FORCA_AB, buffer);
        if(GestioneUdc==2 && err!=0) err=0;
        if(err) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
     // -- RESET LOCAZIONI POSIZIONI VUOTE
     if(!err){
        err = B.StructToUdcTmp(CMD_UDC_RST_LOC, 0, 0, 0, P.EditBaia.Num, UDC_ALL_STZ, UDC_ALL_POS, UDC_ALL_LEV, N_FORCA_AB, buffer);
        if(GestioneUdc==2 && err!=0) err=0;
        if(err) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
  }
  //----------------------------------------------------------------
  // Traccia su LOG OPERATORE
  //----------------------------------------------------------------
  sprintf(buffer, "(EDITBAI) - (Bai:%d) Salva editor", IndiceBaia);
  FileLogOperatore(buffer);
  //----------------------------------------------------------------
  //  Trasmetti comando al SERVER
  //----------------------------------------------------------------
  if( IdPC>1 ){
     err = RemSincronizzaTimeFile( P.baie_name_sorg, P.baie_name_dest, buffer );
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     sprintf( NomeFile, "Bai%04d", P.rec_attuale );
     err = RemCreaFileComando ( NomeFile, (BYTE *) P.ba[ P.rec_attuale ], sizeof( BAIE ), buffer );
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  //----------------------------------------------------------------
  // Refresh della Form principale
  //----------------------------------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Inser()
// ---------------------------
// Funzione che abilita disabilita l'inserimento di un record sul file
//
void __fastcall TFormBaie::Inser()
{
  char buffer[151]="";

  if(LivelloPassword<2) return;
  Inserimento = !Inserimento;
  if(Inserimento == true ){ setmem( &P.EditBaia, sizeof(BAIE), 0); P.EditBaia.Tipo=NN;}
  if(Inserimento == false){ memcpy( &P.EditBaia, P.ba[P.rec_attuale], sizeof( BAIE ));}
  //-----------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------
  sprintf(buffer, "(EDITBAI) - (Bai:%d) Inserimento Editor Baie", IndiceBaia);
  FileLogOperatore(buffer);
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Cancel()
// ---------------------------
// Funzione che cancella un record MACCHINE
//
void __fastcall TFormBaie::Cancel()
{
  char buffer[100] ;
  char NomeFile[100];
  int err =0;
  int result, n_rec;
  struct BAIE bb;

  if( LivelloPassword<2 ) return;
  //----------------------------------------------------
  // Se il file non contiene aacun record allora return
  //----------------------------------------------------
  if(NumRec <= 0){
     Application->MessageBox(MESS[449], MESS[443], MB_OK | MB_ICONINFORMATION);
     return;
  }
  result = Application->MessageBox(MESS[437], MESS[443], MB_YESNO | MB_ICONQUESTION);
  if(result != IDYES) return;

  setmem (&P.EditBaia, sizeof(BAIE), 0);                  // azzera la struttura
  //-----------------------------------------
  //  Salva record sul file
  //-----------------------------------------
  memcpy( P.ba[P.rec_attuale], &P.EditBaia, sizeof( BAIE ));
  err = P.save_record_baie( P.rec_attuale, buffer);
  if( err ) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //-----------------------------------------
  //  Trasmetti comando al SERVER
  //-----------------------------------------
  if( IdPC>1 ){
      err = RemSincronizzaTimeFile( P.baie_name_sorg, P.baie_name_dest, buffer );
      if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
      sprintf( NomeFile, "Bai%04d", P.rec_attuale );
      err = RemCreaFileComando ( NomeFile, (BYTE *) P.ba[ P.rec_attuale ], sizeof( BAIE ), buffer );
      if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  //-----------------------------------------
  // Ricompilazione dei Punti notevoli
  //-----------------------------------------
  if(!err){
     err = N.compila_punti_notevoli(buffer);
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  //--------------------------------
  //  cerca il prossimo record
  //--------------------------------
  for( n_rec=P.rec_attuale+1; n_rec<=MAXBAIE; n_rec++){
     if( P.ba[ n_rec ]->Num >0 ) break;
  }

  //--------------------------------
  //  Se non trovato
  //  cerca il precedente record
  //--------------------------------
  if( n_rec>MAXBAIE ){
     for( n_rec=P.rec_attuale-1; n_rec>0; n_rec--){
        if( P.ba[ n_rec ]->Num >0 ) break;
     }
  }
  //-----------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------
  sprintf(buffer, "(EDITBAI) - (Bai:%d) Cancellazione Editor Baia", IndiceBaia);
  FileLogOperatore(buffer);
  //------------------------------------
  //  Recupera dati nella struttura ch
  //------------------------------------
  memset(&bb, 0, sizeof(BAIE));
  err = P.cerca_record_baie(&bb, n_rec );
  P.rec_attuale  = (short int) n_rec;
  IndiceBaia = (short int) n_rec;
  memcpy( &P.EditBaia, &bb, sizeof( BAIE ) );
  NumRec = P.num_record_baie();
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    GetCaselle()
// ---------------------------
// Funzione che legge dalle caselle di dialogo (TEdit) di WINDOWS
// e scarica su le variabili stringa di MACCHINE
//
void __fastcall TFormBaie::GetCaselle()
{
  int i, j;
  //------------------------------------------------
  // Dati baia
  //------------------------------------------------
  P.EditBaia.Num          = (short int)(atoi(EdNum->Text.c_str()));
  strcpy(P.EditBaia.Sigla, EdSig->Text.c_str());
  P.EditBaia.Urgenza      = (short int)(atoi(EdUrg->Text.c_str()));
  P.EditBaia.Punto        = (short int)(atoi(EdNod->Text.c_str()));
  P.EditBaia.BaiaRialzata = (short int)(atoi(EdAltez->Text.c_str()));
  P.EditBaia.Sollevamento = (short int)(atoi(EdSollev->Text.c_str()));

  //------------------------------------------------
  // Tipo Baia
  //------------------------------------------------
  P.EditBaia.Tipo       = GBTipo->ItemIndex;
  P.EditBaia.Gruppo     = (short int)(CBGruppo->ItemIndex);
  P.EditBaia.GruppoDest = (short int)(CBGruppoDest->ItemIndex);


  //------------------------------------------------
  // Consensi con PLC
  //
  // SICUREZZA:
  // Lo stato dei consensi viene aggiornato in
  // struttura solo con comunicazione disattivata
  //------------------------------------------------
  P.EditBaia.Plc        = (short int)(atoi(EdPlc->Text.c_str()));
  if((break_Com[COM2]==true && break_Com[COM6]==true) || P.EditBaia.EsclComunic==true){
     P.EditBaia.Automatico   = CBAutomatico->Checked;
     P.EditBaia.Blocco       = CBBlocco->Checked;
     P.EditBaia.TuttoPieno   = CBTuttoPieno->Checked;
     P.EditBaia.Chiamata     = CBChiamata->Checked;
     P.EditBaia.RichIng      = CBRicIng->Checked;
     P.EditBaia.OkIng        = CBOkIng->Checked;
     P.EditBaia.OkForcheAlte = CBOkForcAlte->Checked;
     P.EditBaia.TgvIng       = CBTgvIng->Checked;
     P.EditBaia.FineOp       = CBFineOp->Checked;
  }
  //------------------------------------------------
  // Abilitazioni/Disabilitazioni
  //------------------------------------------------
  P.EditBaia.BloccoSovrap = CBBlSovrap->Checked;
  P.EditBaia.Esclusione   = CBEsclM->Checked;
  P.EditBaia.EsclComunic  = CBEsclCom->Checked;
  P.EditBaia.NoMagaz      = CBNoMagaz->Checked;
  P.EditBaia.NoTgvDisp    = CBNoTgvDisp->Checked;

  //------------------------------------------------
  // Dati postazione
  //------------------------------------------------
  P.EditBaia.TipoCod[IndicePosBaia] = CBStato->ItemIndex;
  P.EditBaia.TipoBox[IndicePosBaia] = CBTipPal->ItemIndex;
  P.EditBaia.Codice[IndicePosBaia]  = (BYTE)(atoi(EdCodPal->Text.c_str()));
  P.EditBaia.Lotto[IndicePosBaia]   = atoi(EdLotPal->Text.c_str());
  P.EditBaia.Altezza[IndicePosBaia] = (short int)(atoi(EdAltPal->Text.c_str()));
  P.EditBaia.CarWash[IndicePosBaia] = CBCarW->Checked;
  P.EditBaia.Basso[IndicePosBaia]   = CBBasso->Checked;
  P.EditBaia.Sovrap[IndicePosBaia]  = CBSovrap->Checked;
  P.EditBaia.Ruotato180[IndicePosBaia] = CBRuotato->Checked;
  //------------------------------------------------
  // Aggiornamento configurazione presa
  //------------------------------------------------
  for(i=0; i<POSINPRESA; i++){
     for(j=0; j<PIANIINPRESA; j++){
        P.EditBaia.Presa[IndicePosBaia][i][j] = CBPresa[i][j]->Checked;
     }
  }
}
//---------------------------------------------------------------------------

// ---------------------------
//    SetCaselle()
// ---------------------------
// Funzione che forza i nuovi valori nelle caselle di dialogo (TEdit)
// di WINDOWS recuperandoli dalle variabili stringa delle MACCHINE
//
void __fastcall TFormBaie::SetCaselle()
{
  int  i, j;
  char buffer[101]="";

  //------------------------------------------------
  // Memorizzo il numero della locazione aperta
  //------------------------------------------------
  EditorBaieAttivo = IndiceBaia;

  //--------------------------------------------------
  // Editor non modificabile se Comunicazione con
  // PLC TERRA o I/O REMOTI è attiva
  //--------------------------------------------------
  if((break_Com[COM2]==true && break_Com[COM6]==true) || P.EditBaia.EsclComunic==true){
     GBPlc->Enabled = true;
     RGDati->Enabled = true;
  }else{
     GBPlc->Enabled = false;
     RGDati->Enabled = false;
  }

  //--------------------------------------------------
  // Andrea 06/12/17
  // Estrapolato Tipo di Pallet da "RGDati" per
  // renderlo modificabile anche con com. PLC attiva
  //--------------------------------------------------
  if(LivelloPassword<1)  CBTipPal->Enabled=false;
  else                   CBTipPal->Enabled=true;

  //------------------------------------------------
  // Tipo Baia
  //------------------------------------------------
  GBTipo->ItemIndex       = P.EditBaia.Tipo;
  CBGruppo->ItemIndex     = P.EditBaia.Gruppo;
  CBGruppoDest->ItemIndex = P.EditBaia.GruppoDest;

  //------------------------------------------------
  // Consensi Baia
  //------------------------------------------------
  sprintf(buffer, "%02d", P.EditBaia.Plc);
  EdPlc->Text           = buffer;
  CBChiamata->Checked   = P.EditBaia.Chiamata;
  CBAutomatico->Checked = P.EditBaia.Automatico;
  CBBlocco->Checked     = P.EditBaia.Blocco;
  CBTuttoPieno->Checked = P.EditBaia.TuttoPieno;
  CBRicIng->Checked     = P.EditBaia.RichIng;
  CBOkIng->Checked      = P.EditBaia.OkIng;
  CBOkForcAlte->Checked = P.EditBaia.OkForcheAlte;
  CBTgvIng->Checked     = P.EditBaia.TgvIng;
  CBFineOp->Checked     = P.EditBaia.FineOp;

  //------------------------------------------------
  // Dati baia
  //------------------------------------------------
  sprintf(buffer, "%04d", P.EditBaia.Num);
  EdNum->Text             = buffer;
  EdSig->Text             = P.EditBaia.Sigla;
  sprintf(buffer, "%d",   P.EditBaia.Urgenza);
  EdUrg->Text             = buffer;
  sprintf(buffer, "%04d", P.EditBaia.Punto);
  EdNod->Text             = buffer;
  sprintf(buffer, "%04d", P.EditBaia.BaiaRialzata);
  EdAltez->Text           = buffer;
  sprintf(buffer, "%04d", P.EditBaia.Sollevamento);
  EdSollev->Text          = buffer;

  TextPriorita->Caption   = P.EditBaia.Priorita;

  //-----------------------------------------------
  // Aggiornamento editor baia
  //-----------------------------------------------
  TxtPuntiAux->Caption="";
  for(i=0; i<5; i++){
     TxtPuntiAux->Caption = TxtPuntiAux->Caption + IntToStr(P.EditBaia.PuntoAux[i]) + ",";
  }

  //------------------------------------------------
  // Abilito groupbox di gestione baie standard
  //------------------------------------------------
  GBPlc->Visible=true;
  GBStatoBaia->Visible=true;
  //------------------------------------------------
  // Abilitazioni/Disabilitazioni
  //------------------------------------------------
  CBBlSovrap->Checked   = P.EditBaia.BloccoSovrap;
  CBEsclM->Checked      = P.EditBaia.Esclusione;
  CBEsclCom->Checked    = P.EditBaia.EsclComunic;
  CBNoMagaz->Checked    = P.EditBaia.NoMagaz;
  CBNoTgvDisp->Checked  = P.EditBaia.NoTgvDisp;

  //------------------------------------------------
  // Presenza "ALTA" e "BASSA"
  //------------------------------------------------
  CBForcA->Checked = (bool)(P.EditBaia.PresCEST_A);
  CBForcB->Checked = (bool)(P.EditBaia.PresCEST_B);

  //------------------------------------------------
  // Dati postazione
  //------------------------------------------------
  CBStato->ItemIndex  = P.EditBaia.TipoCod[IndicePosBaia];
  CBTipPal->ItemIndex = P.EditBaia.TipoBox[IndicePosBaia];
  sprintf(buffer, "%02d", P.EditBaia.Codice[IndicePosBaia]);
  EdCodPal->Text      = buffer;
  sprintf(buffer, "%05d", P.EditBaia.Lotto[IndicePosBaia]);
  EdLotPal->Text      = buffer;
  sprintf(buffer, "%04d", P.EditBaia.Altezza[IndicePosBaia]);
  EdAltPal->Text      = buffer;
  CBCarW->Checked     = P.EditBaia.CarWash[IndicePosBaia];
  CBBasso->Checked    = P.EditBaia.Basso[IndicePosBaia];
  CBSovrap->Checked   = P.EditBaia.Sovrap[IndicePosBaia];
  CBRuotato->Checked  = P.EditBaia.Ruotato180[IndicePosBaia];
  //------------------------------------------------
  // Aggiornamento configurazione presa
  //------------------------------------------------
  for(i=0; i<POSINPRESA; i++){
     for(j=0; j<PIANIINPRESA; j++){
        CBPresa[i][j]->Checked = P.EditBaia.Presa[IndicePosBaia][i][j];
     }
  }
  lblPresCestA->Caption = IntToStr( P.EditBaia.PresCEST_A );
  lblPresCestB->Caption = IntToStr( P.EditBaia.PresCEST_B );
  //------------------------------------------------
  // Visualiazzione stato presenze
  //------------------------------------------------
  TxtStPresenze->Caption = P.EditBaia.StatoPres;

  //------------------------------------------------
  // Gestione "Panel informativi"
  //------------------------------------------------
  if(Inserimento==false){
     EditPanel->Caption = "EDIT";
     EditPanel->Color   = clLime;
  }
  if(Inserimento==true){
     EditPanel->Caption = "INSERT";
     EditPanel->Color   = clYellow;
  }

  sprintf( buffer, MESS[441], NumRec );
  NRecordPanel->Caption = buffer;
}
//---------------------------------------------------------------------------
void __fastcall TFormBaie::BtnCercaClick(TObject *Sender)
{
  Cerca();
}
//---------------------------------------------------------------------------

void __fastcall TFormBaie::BtnCopiaClick(TObject *Sender)
{
  Copia();
}
//---------------------------------------------------------------------------

void __fastcall TFormBaie::BtnSalvaClick(TObject *Sender)
{
  Salva();
}
//---------------------------------------------------------------------------

void __fastcall TFormBaie::BtnStampaClick(TObject *Sender)
{
  bool OkStampa;

  OkStampa = PrintDialog->Execute();
  if(OkStampa==false) return;
  FormBaie->Print();
}
//---------------------------------------------------------------------------

void __fastcall TFormBaie::BtnCancellaClick(TObject *Sender)
{
  Cancel();
}
//---------------------------------------------------------------------------

void __fastcall TFormBaie::BtnInserimentoClick(TObject *Sender)
{
  Inser();
}
//---------------------------------------------------------------------------

void __fastcall TFormBaie::BtnAvantiClick(TObject *Sender)
{
  Avanti();
}
//---------------------------------------------------------------------------

void __fastcall TFormBaie::BtnIndietroClick(TObject *Sender)
{
  Indietro();
}
//---------------------------------------------------------------------------

void __fastcall TFormBaie::PCDatiPalChange(TObject *Sender)
{
  int i, j;
  char buffer[11]="";

  IndicePosBaia=0;
  //------------------------------------------------
  // Recupero il numero del TGV selezionato
  //------------------------------------------------
  if(PCDatiPal->ActivePage==TSPos1) IndicePosBaia = 0;
  if(PCDatiPal->ActivePage==TSPos2) IndicePosBaia = 1;
  if(PCDatiPal->ActivePage==TSPos3) IndicePosBaia = 2;
  if(PCDatiPal->ActivePage==TSPos4) IndicePosBaia = 3;
  if(PCDatiPal->ActivePage==TSPos5) IndicePosBaia = 4;
  //------------------------------------------------
  // Visualizzazione forche "A" e "B" solo su prima
  // posizione
  //------------------------------------------------
  CBForcB->Visible=true; CBForcA->Visible=true; TxtStPresenze->Visible=true;
  if(IndicePosBaia!=0){CBForcB->Visible=false; CBForcA->Visible=false; TxtStPresenze->Visible=false;}
  //------------------------------------------------
  // Dati postazione
  //------------------------------------------------
  CBStato->ItemIndex  = P.EditBaia.TipoCod[IndicePosBaia];
  CBTipPal->ItemIndex = P.EditBaia.TipoBox[IndicePosBaia];
  sprintf(buffer, "%02d", P.EditBaia.Codice[IndicePosBaia]);
  EdCodPal->Text      = buffer;
  sprintf(buffer, "%04d", P.EditBaia.Altezza[IndicePosBaia]);
  EdAltPal->Text      = buffer;
  CBCarW->Checked     = P.EditBaia.CarWash[IndicePosBaia];
  CBBasso->Checked    = P.EditBaia.Basso[IndicePosBaia];
  CBSovrap->Checked   = P.EditBaia.Sovrap[IndicePosBaia];
  CBRuotato->Checked  = P.EditBaia.Ruotato180[IndicePosBaia];
  //------------------------------------------------
  // Aggiornamento configurazione presa
  //------------------------------------------------
  for(i=0; i<POSINPRESA; i++){
     for(j=0; j<PIANIINPRESA; j++){
        CBPresa[i][j]->Checked = P.EditBaia.Presa[IndicePosBaia][i][j];
     }
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormBaie::TxtPuntiAuxClick(TObject *Sender)
{
  int  i, pos;
  AnsiString Buff;
  AnsiString InputStringa;

  if(LivelloPassword<3) return;
  //-----------------------------------------------
  // Inserimento nuova rotazione da InputBox
  //-----------------------------------------------
  InputStringa = InputBox("Auxiliary node's", "node1,node2,...,node5:", InputStringa);
  if(InputStringa.IsEmpty() == true) return;
  //-----------------------------------------------
  // Verifico valore rotazione
  //-----------------------------------------------
  Buff = InputStringa;
  try{
     pos=i=0;
     do{
        pos = Buff.Pos(",");
        if(pos!=0){
           P.EditBaia.PuntoAux[i] = StrToInt(Buff.SubString(1, pos-1));
           Buff                   = Buff.SubString(pos+1, Buff.Length());
           i++;
        }
        else{
           P.EditBaia.PuntoAux[i] = StrToInt(Buff);
        }
     }while(pos);
  }
  catch(...){
     ShowMessage("Error in input string format...");
  }
  //-----------------------------------------------
  // Aggiornamento editor baia
  //-----------------------------------------------
  TxtPuntiAux->Caption="";
  for(i=0; i<5; i++){
     TxtPuntiAux->Caption = TxtPuntiAux->Caption + IntToStr(P.EditBaia.PuntoAux[i]) + ",";
  }

  return;
}
//---------------------------------------------------------------------------

void __fastcall TFormBaie::CBStatoChange(TObject *Sender)
{
  char buffer[101]="";
  bool assegnazione_not_ok;

  //------------------------------------------------
  // Gestione assegnazione
  //------------------------------------------------
  assegnazione_not_ok=false;
  switch(CBStato->ItemIndex){
     case  ST_BOX_LAV1:   CBStato->Color = (TColor)(COLOR_PRES_STAT2);
                          strcpy(buffer, ASS_LAV1);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD1);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_LAV2:   CBStato->Color = (TColor)(COLOR_PRES_STAT2);
                          strcpy(buffer, ASS_LAV2);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD2);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_LAV3:   CBStato->Color = (TColor)(COLOR_PRES_STAT3);
                          strcpy(buffer, ASS_LAV3);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD3);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_LAV4:   CBStato->Color = (TColor)(COLOR_PRES_STAT4);
                          strcpy(buffer, ASS_LAV4);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD4);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_LAV5:   CBStato->Color = (TColor)(COLOR_PRES_STAT5);
                          strcpy(buffer, ASS_LAV5);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD5);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_LAV6:   CBStato->Color = (TColor)(COLOR_PRES_STAT6);
                          strcpy(buffer, ASS_LAV6);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD6);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_LAV7:   CBStato->Color = (TColor)(COLOR_PRES_STAT7);
                          strcpy(buffer, ASS_LAV7);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD7);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_LAV8:   CBStato->Color = (TColor)(COLOR_PRES_STAT8);
                          strcpy(buffer, ASS_LAV8);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD8);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_VUOTI:  CBStato->Color = (TColor)(COLOR_PRES_VUOTI); break;
     default:             CBStato->Color = clSkyBlue;                  break;
  }
  //------------------------------------------------
  // Se l'assegnazione non è corretta assegno
  // quella iniziale
  //------------------------------------------------
  if(assegnazione_not_ok==true){
     strcpy(buffer, "Please put a correct code assignment!");
     Application->MessageBox(buffer, MESS[99], MB_OK | MB_ICONINFORMATION);
     CBStato->ItemIndex  = P.EditBaia.TipoCod[IndicePosBaia];
     switch(P.EditBaia.TipoCod[IndicePosBaia]){
        case  ST_BOX_LAV1:  CBStato->Color = (TColor)(COLOR_PRES_STAT1);  break;
        case  ST_BOX_LAV2:  CBStato->Color = (TColor)(COLOR_PRES_STAT2);  break;
        case  ST_BOX_LAV3:  CBStato->Color = (TColor)(COLOR_PRES_STAT3);  break;
        case  ST_BOX_LAV4:  CBStato->Color = (TColor)(COLOR_PRES_STAT4);  break;
        case  ST_BOX_LAV5:  CBStato->Color = (TColor)(COLOR_PRES_STAT5);  break;
        case  ST_BOX_LAV6:  CBStato->Color = (TColor)(COLOR_PRES_STAT6);  break;
        case  ST_BOX_LAV7:  CBStato->Color = (TColor)(COLOR_PRES_STAT7);  break;
        case  ST_BOX_LAV8:  CBStato->Color = (TColor)(COLOR_PRES_STAT8);  break;
        case  ST_BOX_VUOTI: CBStato->Color = (TColor)(COLOR_PRES_VUOTI);  break;
               default:     CBStato->Color = clSkyBlue;                   break;
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormBaie::BtnBoxClick(TObject *Sender)
{
  int TagIndex;
  bool BoxA=false;
  bool BoxB=false;
  //--------------
  TagIndex = (( TBitBtn *)Sender)->Tag;
  if(TagIndex==0) BoxB=true;
  if(TagIndex==1) BoxA=true;
  if(BoxA==false && BoxB==false) return;
  //------------------------------------------------
  // Apertura della finesta di assegnazione dati
  //------------------------------------------------
  // BOX A
  //------------------------------------------------
  if(BoxA==true){
     //§§§
     memcpy(&B.EditBox, &P.EditBaia.BoxA, sizeof( BOX ));
     //§§§

  }
  //------------------------------------------------
  // BOX B
  //------------------------------------------------
  if(BoxB==true){
     //§§§
     memcpy(&B.EditBox, &P.EditBaia.BoxB, sizeof( BOX ));
     //§§§
  }
  //--------------------------------------------------
  // APERTURA FORM BOX
  //--------------------------------------------------
  FormBox->Bai=NumeroBaia;
  FormBox->Pos=IndiceBaia;
  FormBox->Pin=0;
  FormBox->Autoclose=false;

  //--------------------------------------------------
  // Editor Box non modificabile se Comunicazione con
  // PLC TERRA o I/O REMOTI è attiva
  //--------------------------------------------------
  if((break_Com[COM2]==true && break_Com[COM6]==true) || P.EditBaia.EsclComunic==true){
     FormBox->SoloLettura=false;
  }
  else{
     FormBox->SoloLettura=true;
  }
  FormBox->ShowModal();
  //--------------------------------------------------
  if(FormBox->ModalResult==mrOk){
     //--------------------------------------------------
     // BOX A
     //--------------------------------------------------
     if(BoxA==true){
        memcpy(&EdBoxA, &B.EditBox, sizeof(BOX));
        //----------------------------------
        // Variazione tipo box a bordo
        //----------------------------------
        switch(EdBoxA.StatoLav){
           case  ST_BOX_LAV1: P.EditBaia.PresCEST_A=PRES_LAV1;   break;
           case  ST_BOX_LAV2: P.EditBaia.PresCEST_A=PRES_LAV2;   break;
           case  ST_BOX_LAV3: P.EditBaia.PresCEST_A=PRES_LAV3;   break;
           case  ST_BOX_LAV4: P.EditBaia.PresCEST_A=PRES_LAV4;   break;
           case  ST_BOX_LAV5: P.EditBaia.PresCEST_A=PRES_LAV5;   break;
           case  ST_BOX_LAV6: P.EditBaia.PresCEST_A=PRES_LAV6;   break;
           case  ST_BOX_LAV7: P.EditBaia.PresCEST_A=PRES_LAV7;   break;
           case  ST_BOX_LAV8: P.EditBaia.PresCEST_A=PRES_LAV8;   break;
           case  ST_BOX_VUOTI:P.EditBaia.PresCEST_A=PRES_VUOTO;  break;
           default:           P.EditBaia.PresCEST_A=LIBERA;      break;
        }
     }
     //--------------------------------------------------
     // BOX B
     //--------------------------------------------------
     if(BoxB==true){
        memcpy(&EdBoxB, &B.EditBox, sizeof(BOX));
        //----------------------------------
        // Variazione tipo box a bordo
        //----------------------------------
        switch(EdBoxB.StatoLav){
           case  ST_BOX_LAV1: P.EditBaia.PresCEST_B=PRES_LAV1;   break;
           case  ST_BOX_LAV2: P.EditBaia.PresCEST_B=PRES_LAV2;   break;
           case  ST_BOX_LAV3: P.EditBaia.PresCEST_B=PRES_LAV3;   break;
           case  ST_BOX_LAV4: P.EditBaia.PresCEST_B=PRES_LAV4;   break;
           case  ST_BOX_LAV5: P.EditBaia.PresCEST_B=PRES_LAV5;   break;
           case  ST_BOX_LAV6: P.EditBaia.PresCEST_B=PRES_LAV6;   break;
           case  ST_BOX_LAV7: P.EditBaia.PresCEST_B=PRES_LAV7;   break;
           case  ST_BOX_LAV8: P.EditBaia.PresCEST_B=PRES_LAV8;   break;
           case  ST_BOX_VUOTI:P.EditBaia.PresCEST_B=PRES_VUOTO;  break;
           default:           P.EditBaia.PresCEST_B=LIBERA;      break;
        }
     }
  }
  lblPresCestA->Caption = IntToStr( P.EditBaia.PresCEST_A );
  lblPresCestB->Caption = IntToStr( P.EditBaia.PresCEST_B );
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormBaie::CBForcClick(TObject *Sender)
{
  if(CBForcA->Checked==true) BtnBoxA->Visible=true;
  else BtnBoxA->Visible=false;

  if(CBForcB->Checked==true) BtnBoxB->Visible=true;
  else BtnBoxB->Visible=false;
}
//---------------------------------------------------------------------------

