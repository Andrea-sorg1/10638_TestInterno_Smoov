
//---------------------------------------------------------------------------
#include <vcl.h>
#include <vcl/system.hpp>

#pragma hdrstop

#include "Macwin.h"
#include "Remoto.h"
#include "SQLDB.h"                                     
#include "Remoto.h"
#include "AbbPLC.h"
#include "Boxwin.h"
#include "ElCar2.h"

//  ---------------------
//   STRUTTURA MACCHINE
//  ---------------------
//  0 = Direzione di sviluppo ( 0>y+ 1>y- 2>x+ 3>x- )   (fissa)
//
//---------------------------------------------
extern int Macch[][5];

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMacchine *FormMacchine;

//---------------------------------------------------------------------------
__fastcall TFormMacchine::TFormMacchine(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFormMacchine::FormActivate(TObject *Sender)
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
  //--------------------------------------------------
  // Recupero l'indice della riga di percorso attuale
  //--------------------------------------------------
  IndiceMacchina       = NumeroMacchina;
  IndiceStazione       = NumeroStazione;
  IndicePosizione      = NumeroPosizione;
  EditorMacchineAttivo = NumeroMacchina;

  // --- Assegna gli indirizzi all'oggetto condiviso ---
  Gruppi();

  // --- imposta i messaggi degli oggetti ---
  Messaggi();

  // --- Lettura struttura da file  ---
  LeggiDati();

  //--------------------------------------------------
  // Disattivazione delle stazioni non esistenti
  //--------------------------------------------------
  TabStaz1->TabVisible=false;
  TabStaz2->TabVisible=false;
  TabStaz3->TabVisible=false;
  TabStaz4->TabVisible=false;
  TabStaz5->TabVisible=false;
  TabStaz6->TabVisible=false;
  TabStaz7->TabVisible=false;
  TabStaz8->TabVisible=false;
  TabStaz9->TabVisible=false;
  TabStaz10->TabVisible=false;
  switch(Macch[IndiceMacchina][1]){
     case  1: TabStaz1->Visible=true;
              break;
     case  2: TabStaz1->TabVisible=true;
              TabStaz2->TabVisible=true;
              break;
     case  3: TabStaz1->TabVisible=true;
              TabStaz2->TabVisible=true;
              TabStaz3->TabVisible=true;
              break;
     case  4: TabStaz1->TabVisible=true;
              TabStaz2->TabVisible=true;
              TabStaz3->TabVisible=true;
              TabStaz4->TabVisible=true;
              break;
     case  5: TabStaz1->TabVisible=true;
              TabStaz2->TabVisible=true;
              TabStaz3->TabVisible=true;
              TabStaz4->TabVisible=true;
              TabStaz5->TabVisible=true;
              break;
     case  6: TabStaz1->TabVisible=true;
              TabStaz2->TabVisible=true;
              TabStaz3->TabVisible=true;
              TabStaz4->TabVisible=true;
              TabStaz5->TabVisible=true;
              TabStaz6->TabVisible=true;
              break;
     case  7: TabStaz1->TabVisible=true;
              TabStaz2->TabVisible=true;
              TabStaz3->TabVisible=true;
              TabStaz4->TabVisible=true;
              TabStaz5->TabVisible=true;
              TabStaz6->TabVisible=true;
              TabStaz7->TabVisible=true;
              break;
     case  8: TabStaz1->TabVisible=true;
              TabStaz2->TabVisible=true;
              TabStaz3->TabVisible=true;
              TabStaz4->TabVisible=true;
              TabStaz5->TabVisible=true;
              TabStaz6->TabVisible=true;
              TabStaz7->TabVisible=true;
              TabStaz8->TabVisible=true;
              break;
     case  9: TabStaz1->TabVisible=true;
              TabStaz2->TabVisible=true;
              TabStaz3->TabVisible=true;
              TabStaz4->TabVisible=true;
              TabStaz5->TabVisible=true;
              TabStaz6->TabVisible=true;
              TabStaz7->TabVisible=true;
              TabStaz8->TabVisible=true;
              TabStaz9->TabVisible=true;
              break;
     case 10: TabStaz1->TabVisible=true;
              TabStaz2->TabVisible=true;
              TabStaz3->TabVisible=true;
              TabStaz4->TabVisible=true;
              TabStaz5->TabVisible=true;
              TabStaz6->TabVisible=true;
              TabStaz7->TabVisible=true;
              TabStaz8->TabVisible=true;
              TabStaz9->TabVisible=true;
              TabStaz10->TabVisible=true;
              break;
     default: TabStaz1->TabVisible=true;
              TabStaz2->TabVisible=true;
              TabStaz3->TabVisible=true;
              TabStaz4->TabVisible=true;
              TabStaz5->TabVisible=true;
              TabStaz6->TabVisible=true;
              TabStaz7->TabVisible=true;
              TabStaz8->TabVisible=true;
              TabStaz9->TabVisible=true;
              TabStaz10->TabVisible=true;
              break;
  }
  //--------------------------------------------------
  // Visualizzazione dei dati stazione
  //--------------------------------------------------
  switch(IndiceStazione){
     case  0: PageControlMac->ActivePage=TabStaz1; break;
     case  1: PageControlMac->ActivePage=TabStaz2; break;
     case  2: PageControlMac->ActivePage=TabStaz3; break;
     case  3: PageControlMac->ActivePage=TabStaz4; break;
     case  4: PageControlMac->ActivePage=TabStaz5; break;
     case  5: PageControlMac->ActivePage=TabStaz6; break;
     case  6: PageControlMac->ActivePage=TabStaz7; break;
     case  7: PageControlMac->ActivePage=TabStaz8; break;
     case  8: PageControlMac->ActivePage=TabStaz9; break;
     case  9: PageControlMac->ActivePage=TabStaz10; break;
     default: PageControlMac->ActivePage=TabStaz1; break;
  }

  //--------------------------------------------
  // LIVELLO "0" TUTTO IL PANEL PRICIPALE
  // DISATTIVATO
  //--------------------------------------------
  if(LivelloPassword<1){
     PanMacch->Enabled=false;
  }
  else{
     PanMacch->Enabled=true;
  }

  //---------------------------------------------
  // Disattivazioni finestra con Password
  // inferiore a Livello 3.
  //---------------------------------------------
  if(LivelloPassword<3){
     BtnCopia->Enabled       = false;
     BtnCancella->Enabled    = false;
     BtnInserimento->Enabled = false;
  }
  else{
     BtnCopia->Enabled       = true;
     BtnCancella->Enabled    = true;
     BtnInserimento->Enabled = true;
  }

  //---------------------------------------------
  // Refresh della Form
  //---------------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormMacchine::AzzeraTimerClick(TObject *Sender)
{
  //------------------------------------------------------
  // Disabilita/Abilita Timer per riprendere il conteggio
  // del tempo
  //------------------------------------------------------
  TimerClose->Enabled = false;
  TimerClose->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMacchine::AzzeraTimerMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
  //------------------------------------------------------
  // Disabilita/Abilita Timer per riprendere il conteggio
  // del tempo
  //------------------------------------------------------
  TimerClose->Enabled = false;
  TimerClose->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMacchine::TimerCloseTimer(TObject *Sender)
{
  FormMacchine->Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMacchine::FormClose(TObject *Sender, TCloseAction &Action)
{
  //----------------------------------------------
  // Diabilitazione del Timer di chiusura
  //----------------------------------------------
  TimerClose->Enabled=false;
  //----------------------------------------------
  // Editor macchine chiuso
  //----------------------------------------------
  NumeroMacchina=0;
  EditorMacchineAttivo=0;
}
//---------------------------------------------------------------------------
// +--------------------------------------------------+
// | Intercettazione dei Pulsanti premuti da Tastiera |
// +--------------------------------------------------+
void __fastcall TFormMacchine::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
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
void __fastcall TFormMacchine::Gruppi()
{
  CBPres[ 0] = CBPres0;
  CBPres[ 1] = CBPres1;
  CBPres[ 2] = CBPres2;
  CBPres[ 3] = CBPres3;
  CBPres[ 4] = CBPres4;
  CBPres[ 5] = CBPres5;
  CBPres[ 6] = CBPres6;
  CBPres[ 7] = CBPres7;

  EDIdBox[ 0] = EDIdBox0;
  EDIdBox[ 1] = EDIdBox1;
  EDIdBox[ 2] = EDIdBox2;
  EDIdBox[ 3] = EDIdBox3;
  EDIdBox[ 4] = EDIdBox4;
  EDIdBox[ 5] = EDIdBox5;
  EDIdBox[ 6] = EDIdBox6;
  EDIdBox[ 7] = EDIdBox7;

  BtnBox[ 0] = BtnBox0;
  BtnBox[ 1] = BtnBox1;
  BtnBox[ 2] = BtnBox2;
  BtnBox[ 3] = BtnBox3;
  BtnBox[ 4] = BtnBox4;
  BtnBox[ 5] = BtnBox5;
  BtnBox[ 6] = BtnBox6;
  BtnBox[ 7] = BtnBox7;

  EdMagAbbP[ 0] = EdMagP1;
  EdMagAbbP[ 1] = EdMagP2;
  EdMagAbbP[ 2] = EdMagP3;
  EdMagAbbP[ 3] = EdMagP4;
  EdMagAbbP[ 4] = EdMagP5;
  EdMagAbbP[ 5] = EdMagP6;
  EdMagAbbP[ 6] = EdMagP7;
  EdMagAbbP[ 7] = EdMagP8;
  EdMagAbbP[ 8] = EdMagP9;
  EdMagAbbP[ 9] = EdMagP10;
  EdMagAbbP[10] = EdMagP11;
  EdMagAbbP[11] = EdMagP12;
  EdMagAbbP[12] = EdMagP13;
  EdMagAbbP[13] = EdMagP14;
  EdMagAbbP[14] = EdMagP15;
  EdMagAbbP[15] = EdMagP16;
  EdMagAbbP[16] = EdMagP17;
  EdMagAbbP[17] = EdMagP18;
  EdMagAbbP[18] = EdMagP19;
  EdMagAbbP[19] = EdMagP20;

  EdMagAbbV[ 0] = EdMagV1;
  EdMagAbbV[ 1] = EdMagV2;
  EdMagAbbV[ 2] = EdMagV3;
  EdMagAbbV[ 3] = EdMagV4;
  EdMagAbbV[ 4] = EdMagV5;
  EdMagAbbV[ 5] = EdMagV6;
  EdMagAbbV[ 6] = EdMagV7;
  EdMagAbbV[ 7] = EdMagV8;
  EdMagAbbV[ 8] = EdMagV9;
  EdMagAbbV[ 9] = EdMagV10;
  EdMagAbbV[10] = EdMagV11;
  EdMagAbbV[11] = EdMagV12;
  EdMagAbbV[12] = EdMagV13;
  EdMagAbbV[13] = EdMagV14;
  EdMagAbbV[14] = EdMagV15;
  EdMagAbbV[15] = EdMagV16;
  EdMagAbbV[16] = EdMagV17;
  EdMagAbbV[17] = EdMagV18;
  EdMagAbbV[18] = EdMagV19;
  EdMagAbbV[19] = EdMagV20;


}
//---------------------------------------------------------------------------

// +------------------------------------------+
// | Imposta i messaggi abbinati agli oggetti |
// +------------------------------------------+
void __fastcall TFormMacchine::Messaggi()
{
  int  i;
  char buffer[101]="";

  FormMacchine->Caption      = MESS[281];


  TXNum->Caption             = MESS[661];
  TXPrior->Caption           = MESS[662];
  CBComPLC->Caption          = MESS[663];
  GBPlcM->Caption            = MESS[664];
  GBTipo->Caption            = MESS[665];
  GBTipo->Items->Strings[ 0] = MESS[691];
  GBTipo->Items->Strings[ 1] = MESS[692];
  GBTipo->Items->Strings[ 2] = MESS[693];
  GBTipo->Items->Strings[ 3] = MESS[694];
  GBTipo->Items->Strings[ 4] = MESS[695];
  GBTipo->Items->Strings[ 5] = MESS[696];

  GBStatoM->Caption          = MESS[666];
  GBDatiS->Caption           = MESS[667];
  GBStatoS->Caption          = MESS[668];
  GBDatiPos->Caption         = MESS[669];
  GBPlcS->Caption            = MESS[670];
  GBConsS->Caption           = MESS[671];
  GBConsM->Caption           = MESS[671];
  GBMagAbb->Caption          = MESS[672];
  TXNod->Caption             = MESS[673];
  TXNpt->Caption             = MESS[674];
  TXUrg->Caption             = MESS[675];
  CBEsclS->Caption           = MESS[676];
  CBEsclM->Caption           = MESS[676];
  CBNoCod->Caption           = MESS[677];
  CBNoMag->Caption           = MESS[678];
  CBRic->Caption             = MESS[679];
  CBEsp->Caption             = MESS[680];
  TXCodS->Caption            = MESS[681];
  TXCodM->Caption            = MESS[681];
  TXBoxRic->Caption          = MESS[659];
  TXStatBS->Caption          = MESS[683];
  TXTipoBS->Caption          = MESS[684];
  CBRicS->Caption            = MESS[685];
  CBRicM->Caption            = MESS[685];
  CBOkS->Caption             = MESS[686];
  CBOkM->Caption             = MESS[686];
  CBIngS->Caption            = MESS[687];
  CBIngM->Caption            = MESS[687];
  CBEndS->Caption            = MESS[688];
  CBEndM->Caption            = MESS[688];
  CBOkForc->Caption          = MESS[689];
  for(i=0; i<8; i++){
     sprintf(buffer, MESS[690], i+1);
     BtnBox[i]->Caption = buffer;
  }
  CBEsclCom->Caption         = MESS[697];
  CBFineProd->Caption        = MESS[698];
  LbPieni->Caption           = MESS[699];
  LbVuoti->Caption           = MESS[700];
  BtnListaBoxMan->Caption    = MESS[701];

  //------------------------------------------
  // Tipi codice ammessi
  //------------------------------------------
  CBStato->Clear();
  CBStato->Items->Add(MESS[518]); // NESSUNA
  //--------------------------------------------
  // ABBINA_CODICE_A_STATO_BOX
  //--------------------------------------------
  if(AbbinaCodiceStatoBox==true){
     sprintf(buffer, MESS[529], ASS_COD1);
     CBStato->Items->Add(buffer);
     sprintf(buffer, MESS[529], ASS_COD2);
     CBStato->Items->Add(buffer);
     sprintf(buffer, MESS[529], ASS_COD3);
     CBStato->Items->Add(buffer);
     sprintf(buffer, MESS[529], ASS_COD4);
     CBStato->Items->Add(buffer);
     sprintf(buffer, MESS[529], ASS_COD5);
     CBStato->Items->Add(buffer);
     sprintf(buffer, MESS[529], ASS_COD6);
     CBStato->Items->Add(buffer);
     sprintf(buffer, MESS[529], ASS_COD7);
     CBStato->Items->Add(buffer);
     sprintf(buffer, MESS[529], ASS_COD8);
     CBStato->Items->Add(buffer);
  }
  else{
     strcpy(buffer, ASS_LAV1);
     if(strcmp(buffer, "")!=0) CBStato->Items->Add(buffer);
     strcpy(buffer, ASS_LAV2);
     if(strcmp(buffer, "")!=0) CBStato->Items->Add(buffer);
     strcpy(buffer, ASS_LAV3);
     if(strcmp(buffer, "")!=0) CBStato->Items->Add(buffer);
     strcpy(buffer, ASS_LAV4);
     if(strcmp(buffer, "")!=0) CBStato->Items->Add(buffer);
     strcpy(buffer, ASS_LAV5);
     if(strcmp(buffer, "")!=0) CBStato->Items->Add(buffer);
     strcpy(buffer, ASS_LAV6);
     if(strcmp(buffer, "")!=0) CBStato->Items->Add(buffer);
     strcpy(buffer, ASS_LAV7);
     if(strcmp(buffer, "")!=0) CBStato->Items->Add(buffer);
     strcpy(buffer, ASS_LAV8);
     if(strcmp(buffer, "")!=0) CBStato->Items->Add(buffer);
  }
  CBStato->Items->Add(MESS[530]);  // VUOTO

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
}
//---------------------------------------------------------------------------

// +------------------------+
// | Legge i Dati dai file  |
// +------------------------+
void __fastcall TFormMacchine::LeggiDati()
{
  int NumMaccLettura, err;
  char all_mess[300];

  P.rec_attuale = IndiceMacchina;

  setmem(&P.EditMac, sizeof(CHIAMATE), 0);             // azzera la struttura
  //-------------------------------------
  // Recupera il primo record
  //-------------------------------------
  memcpy( &P.EditMac, P.ch[IndiceMacchina], sizeof( CHIAMATE ) );
  NumRec = P.num_record_chiamate();

  NumMaccLettura = IndiceMacchina;
  err=P.load_chiamate_macchine( &NumMaccLettura, all_mess);
  if(err) ShowMessage( all_mess );
}
//---------------------------------------------------------------------------
// ---------------------------
//    Avanti()
// ---------------------------
// Funzione che avanza di un record
//
void __fastcall TFormMacchine::Avanti()
{
  int err=0;
  int n_rec;
  struct CHIAMATE mc;

  //--------------------------------
  //  cerca il prossimo record
  //--------------------------------
  for( n_rec=P.rec_attuale+1; n_rec<=MAXCHIAMATE; n_rec++){
     if( P.ch[ n_rec ]->Num >0 ) break;
  }

  if( n_rec>MAXCHIAMATE ){
     Application->MessageBox(MESS[428], MESS[99], MB_OK | MB_ICONINFORMATION);
     return;
  }

  //------------------------------------
  //  Recupera dati nella struttura ch
  //------------------------------------
  memset(&mc, 0, sizeof(CHIAMATE));
  err = P.cerca_record_chiamate(&mc, n_rec );
  if( err==0 ){
     P.rec_attuale  = (short int) n_rec;
     IndiceMacchina = (short int) n_rec;

     //memcpy( &P.EditMac, &mc, sizeof( CHIAMATE ) );
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
void __fastcall TFormMacchine::Indietro()
{
  int err=0;
  int n_rec;
  struct CHIAMATE mc;

  //--------------------------------
  //  cerca il precedente record
  //--------------------------------
  for( n_rec=P.rec_attuale-1; n_rec>0; n_rec--){
     if( P.ch[ n_rec ]->Num >0 ) break;
  }
  if( n_rec<1 ){
     Application->MessageBox(MESS[429], MESS[99], MB_OK | MB_ICONINFORMATION);
     return;
  }

  //------------------------------------
  //  Recupera dati nella struttura ch
  //------------------------------------
  memset(&mc, 0, sizeof(CHIAMATE));
  err = P.cerca_record_chiamate(&mc, n_rec );
  if( err==0 ){
     P.rec_attuale  = (short int) n_rec;
     IndiceMacchina = (short int) n_rec;
     //memcpy( &P.EditMac, &mc, sizeof( CHIAMATE ) );
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
void __fastcall TFormMacchine::Cerca()
{
  AnsiString InputStringa;
  char Chiave[5]="";
  int n_rec=1, err=0;
  struct CHIAMATE mc;

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
  if(n_rec<0 || n_rec>MAXCHIAMATE){
     Application->MessageBox(MESS[375], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  //------------------------------------
  //  Recupera record
  //------------------------------------
  memset(&mc, 0, sizeof(CHIAMATE));
  err = P.cerca_record_chiamate(&mc, n_rec );
  if(err) {
     Application->MessageBox(MESS[375], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  P.rec_attuale  = (short int) n_rec;
  IndiceMacchina = (short int) n_rec;
  memcpy( &P.EditMac, &mc, sizeof( CHIAMATE ));
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
void __fastcall TFormMacchine::Copia()
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
  if(n_rec<0 || n_rec>MAXCHIAMATE){
     Application->MessageBox(MESS[375], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  //------------------------------------
  //  Verifica se record esistente
  //------------------------------------
  if( n_rec == P.rec_attuale ) return;
  if( P.ch[ n_rec ]->Num >0 ){
     result = Application->MessageBox(MESS[376], MESS[99], MB_YESNO | MB_ICONQUESTION);
     if(result != IDYES) return;
  }

  //-----------------
  //  Copia record
  //-----------------
  memcpy( P.ch[ n_rec ], P.ch[ P.rec_attuale ], sizeof( CHIAMATE ) );
  memcpy( &P.EditMac,    P.ch[ P.rec_attuale ], sizeof( CHIAMATE ) );
  P.rec_attuale       = (short int) n_rec;
  IndiceMacchina      = (short int) n_rec;
  P.ch[ n_rec ]->Num  = (short int) n_rec;
  P.EditMac.Num       = (short int) n_rec;

  Inserimento=false;

  NumRec = P.num_record_chiamate();
  //-----------------------------------------
  // Ricompilazione dei Punti notevoli
  //-----------------------------------------
  err = N.compila_punti_notevoli(buffer);
  if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //-----------------------------------------
  //  Salva record sul file
  //-----------------------------------------
  err = P.save_record_chiamate( P.rec_attuale, buffer);
  if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //-----------------------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------------------
  sprintf(buffer, "(EDITMAC) - (Mac:%d) Copia Editor Macchina", IndiceMacchina);
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
void __fastcall TFormMacchine::Salva()
{
  int  err=0;
  char buffer[120]="";
  char NomeFile[100]="";
  int  i, j, result;
  bool nessuna_chiamata_attiva;
  short int count_pos_impegnate;

  //----------------------------------------------------------------
  // Richiesta di conferma salvataggio
  //----------------------------------------------------------------
  result = Application->MessageBox(MESS[380], MESS[99], MB_YESNO | MB_ICONQUESTION);
  if(result != IDYES) return;
  //----------------------------------------------------------------
  //  Recupera dati dalle caselle di dialogo
  //----------------------------------------------------------------
  GetCaselle();
  if(P.EditMac.Num>MAXCHIAMATE) return;
  //----------------------------------------------------------------
  // Se la macchina è di tipo sconosciuto viene automaticamente
  // esclusa
  //----------------------------------------------------------------
  if(P.EditMac.Tipo==NN  ) P.EditMac.Esclusione=true;
  
  //----------------------------------------------------------------
  // Azzeramento dei nodi sulle stazioni non previste dalla
  // macchina
  //----------------------------------------------------------------
  for(i=0; i<MAXSTAZMAC; i++){
     if(i<Macch[P.EditMac.Num][1]) continue;
     P.EditMac.Staz[i].PuntoIng=0;
     P.EditMac.Staz[i].PuntoOut=0;
     for(j=0; j<MAXPOSTISTAZ; j++){
        P.EditMac.Staz[i].Pos[j].PresCEST_B=LIBERA;
        P.EditMac.Staz[i].Pos[j].PresCEST_A=LIBERA;
        memset(&P.EditMac.Staz[i].Pos[j].BoxB, 0, sizeof(BOX));
        memset(&P.EditMac.Staz[i].Pos[j].BoxA, 0, sizeof(BOX));
     }
  }

  //----------------------------------------------------------------
  // Controllo sull'urgenza assegnata alle stazioni
  //----------------------------------------------------------------
  for(i=0; i<MAXSTAZMAC; i++){
     if(P.EditMac.Staz[i].Urgenza<0) P.EditMac.Staz[i].Urgenza=0;
     if(P.EditMac.Staz[i].Urgenza>9) P.EditMac.Staz[i].Urgenza=9;
  }

  //----------------------------------------------------------------
  // Conteggio postazioni impegnate
  //----------------------------------------------------------------
  for(i=0; i<MAXSTAZMAC; i++){
     if(P.EditMac.Staz[i].PuntoIng<1        ) continue;
     if(P.EditMac.Staz[i].PuntoIng>TOTPUNTI ) continue;
     if(P.EditMac.Staz[i].PuntoOut<1        ) continue;
     if(P.EditMac.Staz[i].PuntoOut>TOTPUNTI ) continue;
     // Conteggio postazioni impegnate
     count_pos_impegnate=0;
     for(j=0; j<P.EditMac.Staz[i].Npt; j++){
        if(P.EditMac.Staz[i].Pos[j].PresCEST_B==LIBERA) continue;
        count_pos_impegnate++;
     }
     // AGGIORNAMENTO POSIZIONE IMPEGNATE IN BASE A FLAG DI PRESENZA
     P.EditMac.Staz[i].NumCestoni = count_pos_impegnate;
  }

  //----------------------------------------------------------------
  // Azzeramento presenza su posizioni inesistenti
  //----------------------------------------------------------------
  for(i=0; i<MAXSTAZMAC; i++){
     for(j=P.EditMac.Staz[i].Npt; j<MAXPOSTISTAZ; j++){
        P.EditMac.Staz[i].Pos[j].PresCEST_B=LIBERA;
        P.EditMac.Staz[i].Pos[j].PresCEST_A=LIBERA;
        memset(&P.EditMac.Staz[i].Pos[j].BoxB, 0, sizeof(BOX));
        memset(&P.EditMac.Staz[i].Pos[j].BoxA, 0, sizeof(BOX));
     }
  }

  //----------------------------------------------------------------
  // Azzeramento dati su posizioni senza presenza
  //----------------------------------------------------------------
  for(i=0; i<MAXSTAZMAC; i++){
     for(j=0; j<P.EditMac.Staz[i].NumCestoni; j++){
        if(P.EditMac.Staz[i].Pos[j].PresCEST_B==LIBERA) continue;
        memset(&P.EditMac.Staz[i].Pos[j].BoxA, 0, sizeof(BOX));
        memset(&P.EditMac.Staz[i].Pos[j].BoxB, 0, sizeof(BOX));
     }
  }

  //----------------------------------------------------------------
  // GESTIONE ARCHIVO BOX (controllo posizione impegnata)
  //----------------------------------------------------------------
  #ifdef ABILITA_ARCHIVIO_BOX
     for(i=0; i<MAXSTAZMAC; i++){
        for(j=0; j<P.EditMac.Staz[i].NumCestoni; j++){
           // FORCA "B"
           if(P.EditMac.Staz[i].Pos[j].PresCEST_B==LIBERA) continue;
           //-------------------------------------------------------------
           // Numero ammesso
           //-------------------------------------------------------------
           if(B.read_box(P.EditMac.Staz[i].Pos[j].BoxB.ID, &P.EditMac.Staz[i].Pos[j].BoxB, buffer)){
              sprintf(buffer, "Box n.%d not exists...", P.EditMac.Staz[i].Pos[j].BoxB.ID);
              ShowMessage(buffer);
              return;
           }
           //-------------------------------------------------------------
           // Verifica se numero carrello univoco
           //-------------------------------------------------------------
           if(B.VerificaIDBoxDuplicati(P.EditMac.Staz[i].Pos[j].BoxB.ID, 0, 0, 0, P.EditMac.Num, i, j, buffer)){
              ShowMessage(buffer);
              return;
           }
           //-------------------------------------------------------------
           // Nuova posizione del box
           //-------------------------------------------------------------
           B.b[P.EditMac.Staz[i].Pos[j].BoxB.ID]->Pos = P.EditMac.Staz[i].PuntoIng;
           //-------------------------------------------------------------
           // Aggiorna file BOX.DAT
           //-------------------------------------------------------------
           err = B.save_record_box(P.EditMac.Staz[i].Pos[j].BoxB.ID, buffer);
           if(err){
              Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
              return;
           }
           // FORCA "A"
           if(P.EditMac.Staz[i].Pos[j].PresCEST_A==LIBERA) continue;
           //-------------------------------------------------------------
           // Numero ammesso
           //-------------------------------------------------------------
           if(B.read_box(P.EditMac.Staz[i].Pos[j].BoxA.ID, &P.EditMac.Staz[i].Pos[j].BoxA, buffer)){
              sprintf(buffer, "Box n.%d not exists...", P.EditMac.Staz[i].Pos[j].BoxA.ID);
              ShowMessage(buffer);
              return;
           }
           //-------------------------------------------------------------
           // Verifica se numero carrello univoco
           //-------------------------------------------------------------
           if(B.VerificaIDBoxDuplicati(P.EditMac.Staz[i].Pos[j].BoxA.ID, 0, 0, 0, P.EditMac.Num, i, j, buffer)){
              ShowMessage(buffer);
              return;
           }
           //-------------------------------------------------------------
           // Nuova posizione del box
           //-------------------------------------------------------------
           B.b[P.EditMac.Staz[i].Pos[j].BoxA.ID]->Pos = P.EditMac.Staz[i].PuntoIng;
           //-------------------------------------------------------------
           // Aggiorna file BOX.DAT
           //-------------------------------------------------------------
           err = B.save_record_box(P.EditMac.Staz[i].Pos[j].BoxA.ID, buffer);
           if(err){
              Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
              return;
           }
        }
     }
  //----------------------------------------------------------------
  // NO GESTIONE ARCHIVO BOX (controllo posizione impegnata)
  //----------------------------------------------------------------
  #else
/*
     for(i=0; i<MAXSTAZMAC; i++){
        for(j=0; j<P.EditMac.Staz[i].NumCestoni; j++){
           // FORCA "B"
           if(P.EditMac.Staz[i].Pos[j].PresCEST_B==LIBERA) continue;
           //-------------------------------------------------------------
           // Numero ammesso
           //-------------------------------------------------------------
           if(P.EditMac.Staz[i].Pos[j].BoxB.ID<1 || P.EditMac.Staz[i].Pos[j].BoxB.ID>MAXBOXIMPIANTO){
              sprintf(buffer, "Box n.%d not exists...", P.EditMac.Staz[i].Pos[j].BoxB.ID);
              ShowMessage(buffer);
              return;
           }
           //-------------------------------------------------------------
           // Verifica se numero carrello univoco
           //-------------------------------------------------------------
           if(B.VerificaIDBoxDuplicati(P.EditMac.Staz[i].Pos[j].BoxB.ID, 0, 0, 0, P.EditMac.Num, i, j, buffer)){
              ShowMessage(buffer);
              return;
           }
           //-------------------------------------------------------------
           // Nuova posizione del box
           //-------------------------------------------------------------
           P.EditMac.Staz[i].Pos[j].BoxB.Pos = P.EditMac.Staz[i].PuntoIng;
           // FORCA "A"
           if(P.EditMac.Staz[i].Pos[j].PresCEST_A==LIBERA) continue;
           //-------------------------------------------------------------
           // Numero ammesso
           //-------------------------------------------------------------
           if(P.EditMac.Staz[i].Pos[j].BoxA.ID<1 || P.EditMac.Staz[i].Pos[j].BoxA.ID>MAXBOXIMPIANTO){
              sprintf(buffer, "Box n.%d not exists...", P.EditMac.Staz[i].Pos[j].BoxA.ID);
              ShowMessage(buffer);
              return;
           }
           //-------------------------------------------------------------
           // Verifica se numero carrello univoco
           //-------------------------------------------------------------
           if(B.VerificaIDBoxDuplicati(P.EditMac.Staz[i].Pos[j].BoxA.ID, 0, 0, 0, P.EditMac.Num, i, j, buffer)){
              ShowMessage(buffer);
              return;
           }
           //-------------------------------------------------------------
           // Nuova posizione del box
           //-------------------------------------------------------------
           P.EditMac.Staz[i].Pos[j].BoxA.Pos = P.EditMac.Staz[i].PuntoIng;
        }
     }
*/
  #endif;

  //----------------------------------------------------------------
  // Abbassamento chiamate
  //----------------------------------------------------------------
  for(i=0; i<MAXSTAZMAC; i++){
     if(P.EditMac.Staz[i].Npt==0 || P.EditMac.Staz[i].Pos[0                     ].PresCEST_B!=LIBERA) P.EditMac.Staz[i].Richiesta=false;
     if(P.EditMac.Staz[i].Npt>0 && P.EditMac.Staz[i].Pos[P.EditMac.Staz[i].Npt-1].PresCEST_B==LIBERA) P.EditMac.Staz[i].Espulsione=false;
  }

  //----------------------------------------------------------------
  // Azzeramento dell'elenco degli ID BOX richiesti ogni
  // volta che la macchina è senza richiesta cestoni
  //----------------------------------------------------------------
  nessuna_chiamata_attiva=true;
  for(i=0; i<MAXSTAZMAC; i++){
     if(P.EditMac.Staz[i].PuntoIng<1        ) continue;
     if(P.EditMac.Staz[i].PuntoIng>TOTPUNTI ) continue;
     if(P.EditMac.Staz[i].PuntoOut<1        ) continue;
     if(P.EditMac.Staz[i].PuntoOut>TOTPUNTI ) continue;
     if(P.EditMac.Staz[i].Richiesta==false  ) continue;
     nessuna_chiamata_attiva=false;
     break;
  }
  if(nessuna_chiamata_attiva==true || P.EditMac.Esclusione==true){
     memset(&P.EditMac.IDRic[0], 0, sizeof(P.EditMac.IDRic));
  }

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
  memcpy( P.ch[P.EditMac.Num], &P.EditMac, sizeof( CHIAMATE ) );
  P.rec_attuale  = P.EditMac.Num;
  IndiceMacchina = P.EditMac.Num;
  NumRec = P.num_record_chiamate();
  err = P.save_record_chiamate( P.rec_attuale, buffer);
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
        err = B.StructToUdcTmp(CMD_UDC_REFRESH, 0, P.EditMac.Num, 0, 0, UDC_ALL_STZ, UDC_ALL_POS, UDC_ALL_LEV, N_FORCA_AB, buffer);
        if(GestioneUdc==2 && err!=0) err=0;
        if(err) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
     // -- RESET LOCAZIONI POSIZIONI VUOTE
     if(!err){
        err = B.StructToUdcTmp(CMD_UDC_RST_LOC, 0, P.EditMac.Num, 0, 0, UDC_ALL_STZ, UDC_ALL_POS, UDC_ALL_LEV, N_FORCA_AB, buffer);
        if(GestioneUdc==2 && err!=0) err=0;
        if(err) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
  }
  //----------------------------------------------------------------
  // Traccia su LOG OPERATORE
  //----------------------------------------------------------------
  sprintf(buffer, "(EDITMAC) - (Mac:%d) Salva editor", IndiceMacchina);
  FileLogOperatore(buffer);
  //----------------------------------------------------------------
  //  Trasmetti comando al SERVER
  //----------------------------------------------------------------
  if( IdPC>1 ){
     err = RemSincronizzaTimeFile( P.chiamate_name_sorg, P.chiamate_name_dest, buffer );
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     sprintf( NomeFile, "Mac%03d", P.rec_attuale );
     err = RemCreaFileComando ( NomeFile, (BYTE *) P.ch[ P.rec_attuale ], sizeof( CHIAMATE ), buffer );
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
void __fastcall TFormMacchine::Inser()
{
  char buffer[151]="";

  if(LivelloPassword<2) return;
  Inserimento = !Inserimento;
  if(Inserimento == true ){ setmem( &P.EditMac, sizeof(CHIAMATE), 0); P.EditMac.Tipo=NN;}
  if(Inserimento == false){ memcpy( &P.EditMac, P.ch[P.rec_attuale], sizeof( CHIAMATE ));}
  //-----------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------
  sprintf(buffer, "(EDITMAC) - (Mac:%d) Inserimento Editor Macchine", IndiceMacchina);
  FileLogOperatore(buffer);
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Cancel()
// ---------------------------
// Funzione che cancella un record MACCHINE
//
void __fastcall TFormMacchine::Cancel()
{
  char buffer[100] ;
  char NomeFile[100];
  int err =0;
  int result, n_rec;
  struct CHIAMATE mc;

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

  setmem (&P.EditMac, sizeof(CHIAMATE), 0);                  // azzera la struttura
  //-----------------------------------------
  //  Salva record sul file
  //-----------------------------------------
  memcpy( P.ch[P.rec_attuale], &P.EditMac, sizeof( CHIAMATE ));
  err = P.save_record_chiamate( P.rec_attuale, buffer);
  if( err ) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //-----------------------------------------
  //  Trasmetti comando al SERVER
  //-----------------------------------------
  if( IdPC>1 ){
      err = RemSincronizzaTimeFile( P.chiamate_name_sorg, P.chiamate_name_dest, buffer );
      if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
      sprintf( NomeFile, "Mac%03d", P.rec_attuale );
      err = RemCreaFileComando ( NomeFile, (BYTE *) P.ch[ P.rec_attuale ], sizeof( CHIAMATE ), buffer );
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
  for( n_rec=P.rec_attuale+1; n_rec<=MAXCHIAMATE; n_rec++){
     if( P.ch[ n_rec ]->Num >0 ) break;
  }

  //--------------------------------
  //  Se non trovato
  //  cerca il precedente record
  //--------------------------------
  if( n_rec>MAXCHIAMATE ){
     for( n_rec=P.rec_attuale-1; n_rec>0; n_rec--){
        if( P.ch[ n_rec ]->Num >0 ) break;
     }
  }
  //-----------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------
  sprintf(buffer, "(EDITMAC) - (Mac:%d) Cancellazione Editor Macchine", IndiceMacchina);
  FileLogOperatore(buffer);
  //------------------------------------
  //  Recupera dati nella struttura ch
  //------------------------------------
  memset(&mc, 0, sizeof(CHIAMATE));
  err = P.cerca_record_chiamate(&mc, n_rec );
  P.rec_attuale  = (short int) n_rec;
  IndiceMacchina = (short int) n_rec;
  memcpy( &P.EditMac, &mc, sizeof( CHIAMATE ) );
  NumRec = P.num_record_chiamate();
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    GetCaselle()
// ---------------------------
// Funzione che legge dalle caselle di dialogo (TEdit) di WINDOWS
// e scarica su le variabili stringa di MACCHINE
//
void __fastcall TFormMacchine::GetCaselle()
{
  int i;

  //------------------------------------------------
  // Dati fissi NON MODIFICABILI
  //------------------------------------------------
  P.EditMac.Num      = (short int)(atoi(EdNum->Text.c_str()));
  P.EditMac.Priorita = (short int)(atoi(EdPrior->Text.c_str()));
  strcpy(P.EditMac.Sigla, EdSig->Text.c_str());
  P.EditMac.Tipo     = (short int)(GBTipo->ItemIndex);

  //------------------------------------------------
  // Abilitazioni/Disabilitazioni
  //------------------------------------------------
  P.EditMac.Esclusione       = CBEsclM->Checked;
  P.EditMac.EsclComunic      = CBEsclCom->Checked;
  P.EditMac.FineProd         = CBFineProd->Checked;

  //------------------------------------------------
  // Stato segnali  (PLC - MACCHINA)
  // (abilita set manuale solo con comunic. esclusa)
  //------------------------------------------------
  if(break_Com[COM2]==true || P.EditMac.EsclComunic==true){
     //------------------------------------------------
     // Dati letti dal PLC e abbinati alla macchina
     //------------------------------------------------
     P.EditMac.Automatico    = CBAuto->Checked;
     P.EditMac.Blocco        = CBBloc->Checked;
     P.EditMac.Codice        = (short int)(atoi(EdCodM->Text.c_str()));
     P.EditMac.RichIngMac    = CBRicM->Checked;
     P.EditMac.OkIngMac      = CBOkM->Checked;
     P.EditMac.TgvIngMac     = CBIngM->Checked;
     P.EditMac.FineOpMac     = CBEndM->Checked;
  }
  //------------------------------------------------
  // Stato ESSICATOIO
  //------------------------------------------------
  //--------------------------------------------------
  // se break_plc allora SKIP !!!!
  //--------------------------------------------------
  if(break_Com[COM2] || P.EditMac.EsclComunic==true){
     CBStatoMac->Enabled = true;
     P.EditMac.StatoMac  = CBStatoMac->ItemIndex;
  }else{
     CBStatoMac->Enabled = false;
  }
  //------------------------------------------------
  // Dati stazione
  //------------------------------------------------
  P.EditMac.Staz[IndiceStazione].PuntoIng   = (short int)(atoi(EdNod1->Text.c_str()));
  P.EditMac.Staz[IndiceStazione].PuntoOut   = (short int)(atoi(EdNod2->Text.c_str()));
  P.EditMac.Staz[IndiceStazione].Npt        = (short int)(atoi(EdNpt->Text.c_str()));
  P.EditMac.Staz[IndiceStazione].NumCestoni = (short int)(atoi(EdNumC->Text.c_str()));
  P.EditMac.Staz[IndiceStazione].Urgenza    = (short int)(atoi(EdUrgS->Text.c_str()));
  P.EditMac.Staz[IndiceStazione].Esclusione = CBEsclS->Checked;
  P.EditMac.Staz[IndiceStazione].NoCodice   = CBNoCod->Checked;
  P.EditMac.Staz[IndiceStazione].NoMagaz    = CBNoMag->Checked;

  if(RGIngS->Checked==true) P.EditMac.Staz[IndiceStazione].Tipo=MC_STAZ_ING;
  if(RGOutS->Checked==true) P.EditMac.Staz[IndiceStazione].Tipo=MC_STAZ_OUT;

  //------------------------------------------------
  // Nascondo i pulsanti relativi a posizioni
  // inesistenti
  //------------------------------------------------
  for(i=P.EditMac.Staz[IndiceStazione].Npt; i<8; i++){
     CBPres[i]->Visible = false; EDIdBox[i]->Visible=false; BtnBox[i]->Visible=false;
  }

  //------------------------------------------------
  // Dati posizioni stazione
  //------------------------------------------------
  for(i=0; i<P.EditMac.Staz[IndiceStazione].Npt; i++){
     CBPres[i]->Visible = true; EDIdBox[i]->Visible=true; BtnBox[i]->Visible=true;
     //--------------------------------------------
     // A tutte le posizioni con la prenza assegno
     // la lavorazione della stazione
     //--------------------------------------------
     if(CBPres[i]->Checked==true){
        P.EditMac.Staz[IndiceStazione].Pos[i].PresCEST_B = (short int)(CBStato->ItemIndex);
        P.EditMac.Staz[IndiceStazione].Pos[i].BoxB.ID    = (short int)(atoi(EDIdBox[i]->Text.c_str()));
     }
     else{
        P.EditMac.Staz[IndiceStazione].Pos[i].PresCEST_B = LIBERA;
        P.EditMac.Staz[IndiceStazione].Pos[i].BoxB.ID    = 0;
     }
  }

  //------------------------------------------------
  // Stato segnali  (PLC - STAZIONE)
  // (abilita set manuale solo con comunic. esclusa)
  //------------------------------------------------
  if(break_Com[COM2]==true || P.EditMac.EsclComunic==true){
     P.EditMac.Staz[IndiceStazione].Richiesta         = CBRic->Checked;
     P.EditMac.Staz[IndiceStazione].Espulsione        = CBEsp->Checked;
     P.EditMac.Staz[IndiceStazione].Codice            = (short int)(atoi(EdCodS->Text.c_str()));
     P.EditMac.Staz[IndiceStazione].StatoLav          = (short int)(CBStato->ItemIndex);
     P.EditMac.Staz[IndiceStazione].TipoBox           = (short int)(CBTipPal->ItemIndex);
     // da PLC (consensi con TGV)
     P.EditMac.Staz[IndiceStazione].RicIngresso       = CBRicS->Checked;
     P.EditMac.Staz[IndiceStazione].OkIngresso        = CBOkS->Checked;
     P.EditMac.Staz[IndiceStazione].TgvInIngombro     = CBIngS->Checked;
     P.EditMac.Staz[IndiceStazione].FineOperazioneTGV = CBEndS->Checked;
     P.EditMac.Staz[IndiceStazione].OkForcheAlte      = CBOkForc->Checked;
  }


  //------------------------------------------------
  // Elenco magazzini abbinati
  //------------------------------------------------
  for(i=0; i<20; i++){
     P.EditMac.MagAbbPieni[i] = (short int)(atoi(EdMagAbbP[i]->Text.c_str()));
     P.EditMac.MagAbbVuoti[i] = (short int)(atoi(EdMagAbbV[i]->Text.c_str()));
  }
}
//---------------------------------------------------------------------------

// ---------------------------
//    SetCaselle()
// ---------------------------
// Funzione che forza i nuovi valori nelle caselle di dialogo (TEdit)
// di WINDOWS recuperandoli dalle variabili stringa delle MACCHINE
//
void __fastcall TFormMacchine::SetCaselle()
{
  int i;
  char buffer[101]="";

  //------------------------------------------------
  // Memorizzo il numero della locazione aperta
  //------------------------------------------------
  EditorMacchineAttivo = IndiceMacchina;
  //------------------------------------------------
  // Numero locazione nel Caption della form
  //------------------------------------------------
  sprintf(buffer, MESS[660], P.EditMac.Num);
  FormMacchine->Caption = buffer;
  //------------------------------------------------
  // Dati fissi NON MODIFICABILI
  //------------------------------------------------
  sprintf(buffer, "%02d", P.EditMac.Num);
  EdNum->Text             = buffer;
  EdSig->Text             = P.EditMac.Sigla;
  sprintf(buffer, "%d",   P.EditMac.Priorita);
  EdPrior->Text = buffer;
  GBTipo->ItemIndex       = P.EditMac.Tipo;

  //------------------------------------------------
  // Abilitazioni/Disabilitazioni
  //------------------------------------------------
  CBEsclM->Checked     = P.EditMac.Esclusione;
  CBEsclCom->Checked   = P.EditMac.EsclComunic;
  CBFineProd->Checked  = P.EditMac.FineProd;

  //------------------------------------------------
  // Stato segnali
  // (abilita set manuale solo con comunic. esclusa)
  //------------------------------------------------
  if(break_Com[COM2]==true || P.EditMac.EsclComunic==true){
     CBComPLC->Enabled = true;
     GBPlcM->Enabled   = true;
     GBPlcS->Enabled   = true;
  }
  else{
     CBComPLC->Enabled = false;
     GBPlcM->Enabled   = false;
     GBPlcS->Enabled   = false;
  }

  //------------------------------------------------
  // Dati letti dal PLC e abbinati alla macchina
  //------------------------------------------------
  CBAuto->Checked   = P.EditMac.Automatico;
  CBBloc->Checked   = P.EditMac.Blocco;
  sprintf(buffer, "%02d", P.EditMac.Codice);
  EdCodM->Text      = buffer;
  CBRicM->Checked   = P.EditMac.RichIngMac;
  CBOkM->Checked    = P.EditMac.OkIngMac;
  CBIngM->Checked   = P.EditMac.TgvIngMac;
  CBEndM->Checked   = P.EditMac.FineOpMac;

  //------------------------------------------------
  // Stato ESSICATOIO
  //------------------------------------------------
  CBStatoMac->ItemIndex =  P.EditMac.StatoMac;
/*
  //------------------------------------------------
  // Box richiesti dalla macchina
  //------------------------------------------------
  sprintf(buffer, "");
  for(i=0; i<MAXSTAZMAC*MAXPOSTISTAZ; i++){
     if(P.EditMac.IDRic[i]<1             ) continue;
     if(P.EditMac.IDRic[i]>MAXBOXIMPIANTO) continue;
     sprintf(buffer1, "%d,  ", P.EditMac.IDRic[i]);
     strcat(buffer, buffer1);
  }
  TxtIDRic->Caption = buffer;
*/
  //------------------------------------------------
  // Dati stazione
  //------------------------------------------------
  sprintf(buffer, "%d°", P.EditMac.Staz[IndiceStazione].RotLayout);
  TXGrd->Caption = buffer;
  sprintf(buffer, "%04d", P.EditMac.Staz[IndiceStazione].PuntoIng);
  EdNod1->Text = buffer;
  sprintf(buffer, "%04d", P.EditMac.Staz[IndiceStazione].PuntoOut);
  EdNod2->Text = buffer;
  sprintf(buffer, "%02d", P.EditMac.Staz[IndiceStazione].Npt);
  EdNpt->Text = buffer;
  sprintf(buffer, "%02d", P.EditMac.Staz[IndiceStazione].NumCestoni);
  EdNumC->Text = buffer;
  sprintf(buffer, "%d",   P.EditMac.Staz[IndiceStazione].Urgenza);
  EdUrgS->Text = buffer;
  CBEsclS->Checked = P.EditMac.Staz[IndiceStazione].Esclusione;
  CBNoCod->Checked = P.EditMac.Staz[IndiceStazione].NoCodice;
  CBNoMag->Checked = P.EditMac.Staz[IndiceStazione].NoMagaz;

  if(P.EditMac.Staz[IndiceStazione].Tipo==MC_STAZ_ING){RGOutS->Checked=false; RGIngS->Checked=true; }
  if(P.EditMac.Staz[IndiceStazione].Tipo==MC_STAZ_OUT){RGOutS->Checked=true;  RGIngS->Checked=false;}

  // da PLC
  CBRic->Checked        = P.EditMac.Staz[IndiceStazione].Richiesta;
  CBEsp->Checked        = P.EditMac.Staz[IndiceStazione].Espulsione;
  sprintf(buffer, "%02d", P.EditMac.Staz[IndiceStazione].Codice);
  EdCodS->Text = buffer;
  CBStato->ItemIndex    = P.EditMac.Staz[IndiceStazione].StatoLav;
  CBTipPal->ItemIndex   = P.EditMac.Staz[IndiceStazione].TipoBox;

  //------------------------------------------------
  // Assegnazione colori al Combobox di selezione
  //------------------------------------------------
  switch(CBStato->ItemIndex){
     case  LIBERA:        CBStato->Color = clCream;
                          break;
     case  ST_BOX_LAV1:   CBStato->Color = (TColor)(COLOR_PRES_STAT1);
                          break;
     case  ST_BOX_LAV2:   CBStato->Color = (TColor)(COLOR_PRES_STAT2);
                          break;
     case  ST_BOX_LAV3:   CBStato->Color = (TColor)(COLOR_PRES_STAT3);
                          break;
     case  ST_BOX_LAV4:   CBStato->Color = (TColor)(COLOR_PRES_STAT4);
                          break;
     case  ST_BOX_LAV5:   CBStato->Color = (TColor)(COLOR_PRES_STAT5);
                          break;
     case  ST_BOX_LAV6:   CBStato->Color = (TColor)(COLOR_PRES_STAT6);
                          break;
     case  ST_BOX_LAV7:   CBStato->Color = (TColor)(COLOR_PRES_STAT7);
                          break;
     case  ST_BOX_LAV8:   CBStato->Color = (TColor)(COLOR_PRES_STAT8);
                          break;
     case  ST_BOX_VUOTI:  CBStato->Color = (TColor)(COLOR_PRES_VUOTI);
                          break;
                 default: CBStato->Color = clCream;
                          break;
  }

  // da PLC (consensi con TGV)
  CBRicS->Checked   = P.EditMac.Staz[IndiceStazione].RicIngresso;
  CBOkS->Checked    = P.EditMac.Staz[IndiceStazione].OkIngresso;
  CBIngS->Checked   = P.EditMac.Staz[IndiceStazione].TgvInIngombro;
  CBEndS->Checked   = P.EditMac.Staz[IndiceStazione].FineOperazioneTGV;
  CBOkForc->Checked = P.EditMac.Staz[IndiceStazione].OkForcheAlte;

  //------------------------------------------------
  // Rendo invisibili tutte le posizioni
  //------------------------------------------------
  for(i=0; i<8; i++){
     CBPres[i]->Visible  = false;
     EDIdBox[i]->Visible = false;
     BtnBox[i]->Visible  = false;
  }

  //------------------------------------------------
  // Aggioramento stato posizioni
  //------------------------------------------------
  for(i=0; i<P.EditMac.Staz[IndiceStazione].Npt; i++){
     CBPres[ i]->Visible  = true;
     EDIdBox[ i]->Visible = true;
     BtnBox[ i]->Visible  = true;
     CBPres[ i]->Checked  = false;
     if(P.EditMac.Staz[IndiceStazione].Pos[i].PresCEST_B!=LIBERA) CBPres[ i]->Checked=true;
     EDIdBox[ i]->Enabled = CBPres[ i]->Checked;
     BtnBox[ i]->Enabled  = CBPres[ i]->Checked;
     //-------------------------------------------
     // Evidenzio solo la posizione selezionata
     //-------------------------------------------
     CBPres[ i]->Color = clBtnFace;
     //-------------------------------------------
     // Abilitazione postazioni con presenza box
     //-------------------------------------------
     if(CBPres[ i]->Checked==true){
        sprintf(buffer, "%04d", P.EditMac.Staz[IndiceStazione].Pos[i].BoxB.ID);
        EDIdBox[ i]->Text        = buffer;
        EDIdBox[ i]->Color       = clCream;
        EDIdBox[ i]->Font->Color = clNavy;
     }
     else{
        EDIdBox[ i]->Text        = "0000";
        EDIdBox[ i]->Color       = clBtnFace;
        EDIdBox[ i]->Font->Color = clGray;
     }
  }

  //------------------------------------------------
  // Posizione selezionata
  //------------------------------------------------
  CBPres[IndicePosizione]->Color = clYellow;
  //------------------------------------------------
  // Elenco magazzini abbinati
  //------------------------------------------------
  for(i=0; i<20; i++){
     sprintf(buffer, "%02d", P.EditMac.MagAbbPieni[i]);
     EdMagAbbP[i]->Text = buffer;
     sprintf(buffer, "%02d", P.EditMac.MagAbbVuoti[i]);
     EdMagAbbV[i]->Text = buffer;
  }

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
  if(P.EditMac.PlcErr==true){
     PlcErrPanel->Visible=true;
  }else{
     PlcErrPanel->Visible=false;
  }
  sprintf( buffer, MESS[441], NumRec );
  NRecordPanel->Caption = buffer;

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
void __fastcall TFormMacchine::BtnCercaClick(TObject *Sender)
{
  Cerca();
}
//---------------------------------------------------------------------------

void __fastcall TFormMacchine::BtnCopiaClick(TObject *Sender)
{
  Copia();
}
//---------------------------------------------------------------------------

void __fastcall TFormMacchine::BtnSalvaClick(TObject *Sender)
{
  Salva();
}
//---------------------------------------------------------------------------
void __fastcall TFormMacchine::BtnStampaClick(TObject *Sender)
{
  bool OkStampa;

  OkStampa = PrintDialog->Execute();
  if(OkStampa == false) return;
  FormMacchine->Print();
}
//---------------------------------------------------------------------------

void __fastcall TFormMacchine::BtnCancellaClick(TObject *Sender)
{
  Cancel();
}
//---------------------------------------------------------------------------

void __fastcall TFormMacchine::BtnInserimentoClick(TObject *Sender)
{
  Inser();
}
//---------------------------------------------------------------------------

void __fastcall TFormMacchine::BtnAvantiClick(TObject *Sender)
{
  Avanti();
}
//---------------------------------------------------------------------------

void __fastcall TFormMacchine::BtnIndietroClick(TObject *Sender)
{
  Indietro();
}
//---------------------------------------------------------------------------

void __fastcall TFormMacchine::BtnPLCClick(TObject *Sender)
{
  //-------------------------
  // Apertura della pagina
  //-------------------------
  memcpy(&FormAbbPlc->PlcData, &P.EditMac.Plc, sizeof(PARAMETRI_COM));
  FormAbbPlc->ShowModal();
  if(FormAbbPlc->ModalResult==mrOk){
     memcpy(&P.EditMac.Plc, &FormAbbPlc->PlcData, sizeof(PARAMETRI_COM));
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormMacchine::PageControlMacChange(TObject *Sender)
{
  int  i;
  char buffer[101]="";

  IndiceStazione=0;
  IndicePosizione=0;
  //----------------------------------------
  // Recupero il numero del TGV selezionato
  //----------------------------------------
  if(PageControlMac->ActivePage==TabStaz1 ) IndiceStazione = 0;
  if(PageControlMac->ActivePage==TabStaz2 ) IndiceStazione = 1;
  if(PageControlMac->ActivePage==TabStaz3 ) IndiceStazione = 2;
  if(PageControlMac->ActivePage==TabStaz4 ) IndiceStazione = 3;
  if(PageControlMac->ActivePage==TabStaz5 ) IndiceStazione = 4;
  if(PageControlMac->ActivePage==TabStaz6 ) IndiceStazione = 5;
  if(PageControlMac->ActivePage==TabStaz7 ) IndiceStazione = 6;
  if(PageControlMac->ActivePage==TabStaz8 ) IndiceStazione = 7;
  if(PageControlMac->ActivePage==TabStaz9 ) IndiceStazione = 8;
  if(PageControlMac->ActivePage==TabStaz10) IndiceStazione = 9;

  //------------------------------------------------
  // Dati stazione
  //------------------------------------------------
  sprintf(buffer, "%d°", P.EditMac.Staz[IndiceStazione].RotLayout);
  TXGrd->Caption = buffer;
  sprintf(buffer, "%04d", P.EditMac.Staz[IndiceStazione].PuntoIng);
  EdNod1->Text = buffer;
  sprintf(buffer, "%04d", P.EditMac.Staz[IndiceStazione].PuntoOut);
  EdNod2->Text = buffer;
  sprintf(buffer, "%02d", P.EditMac.Staz[IndiceStazione].Npt);
  EdNpt->Text = buffer;
  sprintf(buffer, "%02d", P.EditMac.Staz[IndiceStazione].NumCestoni);
  EdNumC->Text = buffer;
  sprintf(buffer, "%d",   P.EditMac.Staz[IndiceStazione].Urgenza);
  EdUrgS->Text = buffer;
  CBEsclS->Checked = P.EditMac.Staz[IndiceStazione].Esclusione;
  CBNoCod->Checked = P.EditMac.Staz[IndiceStazione].NoCodice;
  CBNoMag->Checked = P.EditMac.Staz[IndiceStazione].NoMagaz;

  // da PLC
  CBRic->Checked   = P.EditMac.Staz[IndiceStazione].Richiesta;
  CBEsp->Checked   = P.EditMac.Staz[IndiceStazione].Espulsione;
  sprintf(buffer, "%02d", P.EditMac.Staz[IndiceStazione].Codice);
  EdCodS->Text = buffer;
  CBStato->ItemIndex  = P.EditMac.Staz[IndiceStazione].StatoLav;
  CBTipPal->ItemIndex = P.EditMac.Staz[IndiceStazione].TipoBox;

  // da PLC (consensi con TGV)
  CBRicS->Checked   = P.EditMac.Staz[IndiceStazione].RicIngresso;
  CBOkS->Checked    = P.EditMac.Staz[IndiceStazione].OkIngresso;
  CBIngS->Checked   = P.EditMac.Staz[IndiceStazione].TgvInIngombro;
  CBEndS->Checked   = P.EditMac.Staz[IndiceStazione].FineOperazioneTGV;
  CBOkForc->Checked = P.EditMac.Staz[IndiceStazione].OkForcheAlte;
  //------------------------------------------------
  // Aggioramento stato posizioni
  //------------------------------------------------
  for(i=0; i<8; i++){
     if(P.EditMac.Staz[IndiceStazione].Pos[i].PresCEST_B!=0) CBPres[i]->Checked = true;
     else                                                    CBPres[i]->Checked = false;
     EDIdBox[i]->Enabled = CBPres[i]->Checked;
     BtnBox[i]->Enabled  = CBPres[i]->Checked;
     //-------------------------------------------
     // Evidenzio solo la posizione selezionata
     //-------------------------------------------
     CBPres[i]->Color = clBtnFace;
     //-------------------------------------------
     // Abilitazione postazioni con presenza box
     //-------------------------------------------
     if(CBPres[i]->Checked==true){
        sprintf(buffer, "%04d", P.EditMac.Staz[IndiceStazione].Pos[i].BoxB.ID);
        EDIdBox[i]->Text        = buffer;
        EDIdBox[i]->Color       = clCream;
        EDIdBox[i]->Font->Color = clNavy;
     }
     else{
        EDIdBox[ i]->Text       = "0000";
        EDIdBox[i]->Color       = clBtnFace;
        EDIdBox[i]->Font->Color = clGray;
     }
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormMacchine::BtnBoxClick(TObject *Sender)
{
  int  i;
  char buffer[11]="";

  //------------------------------------------------
  // Assegnazione della presenza
  //------------------------------------------------
  IndicePosizione = (((TBitBtn *)Sender)->Tag);
  //------------------------------------------------
  // Evidenzion la posizione selezionata
  //------------------------------------------------
  for(i=0; i<8; i++) CBPres[i]->Color=clBtnFace;
  CBPres[IndicePosizione]->Color = clYellow;
  //------------------------------------------------
  // Apertura della finesta di assegnazione dati
  //------------------------------------------------
  P.EditMac.Staz[IndiceStazione].Pos[IndicePosizione].BoxB.ID = (short int)(atoi(EDIdBox[IndicePosizione]->Text.c_str()));
  //------------------------------------------------
  // Accesso editor box
  //------------------------------------------------
  memcpy(&B.EditBox, &P.EditMac.Staz[IndiceStazione].Pos[IndicePosizione].BoxB, sizeof(BOX));
  FormBox->Mac=P.EditMac.Num;
  FormBox->Stz=IndiceStazione;
  FormBox->Pos=IndicePosizione;
  FormBox->Forc=N_FORCA_B;
  FormBox->Autoclose=true;
  FormBox->ShowModal();
  if(FormBox->ModalResult==mrOk){
     memcpy(&P.EditMac.Staz[IndiceStazione].Pos[IndicePosizione].BoxB, &B.EditBox, sizeof(BOX));
     sprintf(buffer, "%04d", P.EditMac.Staz[IndiceStazione].Pos[IndicePosizione].BoxB.ID);
     EDIdBox[IndicePosizione]->Text = buffer;
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormMacchine::BtnListaBoxManClick(TObject *Sender)
{
  memcpy(&FormElBox->ElencoBox[0], &P.EditMac.IDRicMan[0], sizeof(P.EditMac.IDRicMan));
  //---------------------------------------
  // Apertura della FORM
  //---------------------------------------
  FormElBox->ShowModal();
  if(FormElBox->ModalResult==mrOk){
     memcpy(&P.EditMac.IDRicMan[0], &FormElBox->ElencoBox[0], sizeof(P.EditMac.IDRicMan));
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormMacchine::BtnReloadClick(TObject *Sender)
{
  memset(&P.EditMac.IDRic[0], 0, sizeof(P.EditMac.IDRic));
  //----------------------------------------------
  // Salvataggio per rendere effettivo il reload
  //----------------------------------------------
  Salva();
  //----------------------------------------------
  // Refresh della Form
  //----------------------------------------------
  SetCaselle();
}

//---------------------------------------------------------------------------

void __fastcall TFormMacchine::CBStatoChange(TObject *Sender)
{
  //------------------------------------------------
  // Gestione assegnazione
  //------------------------------------------------
  switch(CBStato->ItemIndex){
     case  LIBERA:        CBStato->Color = clCream;
                          break;
     case  ST_BOX_LAV1:   CBStato->Color = (TColor)(COLOR_PRES_STAT1);
                          break;
     case  ST_BOX_LAV2:   CBStato->Color = (TColor)(COLOR_PRES_STAT2);
                          break;
     case  ST_BOX_LAV3:   CBStato->Color = (TColor)(COLOR_PRES_STAT3);
                          break;
     case  ST_BOX_LAV4:   CBStato->Color = (TColor)(COLOR_PRES_STAT4);
                          break;
     case  ST_BOX_LAV5:   CBStato->Color = (TColor)(COLOR_PRES_STAT5);
                          break;
     case  ST_BOX_LAV6:   CBStato->Color = (TColor)(COLOR_PRES_STAT6);
                          break;
     case  ST_BOX_LAV7:   CBStato->Color = (TColor)(COLOR_PRES_STAT7);
                          break;
     case  ST_BOX_LAV8:   CBStato->Color = (TColor)(COLOR_PRES_STAT8);
                          break;
     case  ST_BOX_VUOTI:  CBStato->Color = (TColor)(COLOR_PRES_VUOTI);
                          break;
                 default: CBStato->Color = clCream;
                          break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMacchine::TXGrdDblClick(TObject *Sender)
{
  int new_rot;
  char buffer[101]="";
  AnsiString InputStringa;

  //-------------------------------------------
  // Solo con password di livello 3
  //-------------------------------------------
  if(LivelloPassword<3) return;
  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  InputStringa = InputBox("STATION's rotation", "Insert new rotation:", P.EditMac.Staz[IndiceStazione].RotLayout);
  if(InputStringa.IsEmpty() == true) return;
  //-----------------------------------------------
  // Verifico valore rotazione
  //-----------------------------------------------
  new_rot = StrToInt(InputStringa);
  if(new_rot<0 || new_rot>360) new_rot=0;
  P.EditMac.Staz[IndiceStazione].RotLayout = (short int)(new_rot);
  //-----------------------------------------------
  // Rinfresco dato visualizzato
  //-----------------------------------------------
  sprintf(buffer, "%d°", P.EditMac.Staz[IndiceStazione].RotLayout);          // Gradi di rotazione con calamite
  TXGrd->Caption = buffer;
}
//---------------------------------------------------------------------------

