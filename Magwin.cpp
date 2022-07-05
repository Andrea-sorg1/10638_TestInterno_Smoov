//---------------------------------------------------------------------------
#include <vcl.h>
#include <vcl/system.hpp>

#pragma hdrstop

#include "Tecnofer.h"
#include "Magwin.h"
#include "Layout.h"
#include "Remoto.h"
#include "SQLDB.h"
#include "Boxwin.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMagaz *FormMagaz;

// --------------------------
//   STRUTTURE GRAFICHE
// --------------------------
//
extern int Magaz[][5];
extern int AccoppiaMagaz[][2];

//---------------------------------------------------------------------------
__fastcall TFormMagaz::TFormMagaz(TComponent* Owner) : TForm(Owner)
{
}
//--------------------------------------------------------------------------

void __fastcall TFormMagaz::FormActivate(TObject *Sender)
{
  int  i, codice;
  char buffer[51]="";

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
  NumMagAccoppiato=0;
  NumMagTemporaneo=0;
  DatiBoxModificati=0;

  //--------------------------------------------------
  // Recupero l'indice della riga di percorso attuale
  //--------------------------------------------------
  IndiceMagazzino       = NumeroMagazzino;
  IndicePosizione       = NumeroPosizione;
  EditorMagazzinoAttivo = NumeroMagazzino;

  // --- Compilazione elenco assegnazione magazzini ---
  CBAsseg->Clear();
  strcpy(buffer, MESS[518]);
  CBAsseg->Items->Add(buffer);
  //--------------------------------------------
  // ABBINA_CODICE_A_STATO_BOX
  //---------------------------------------------
  if(AbbinaCodiceStatoBox==true){
     codice=ASS_COD1;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD1);
     else          strcpy(buffer, "");
     CBAsseg->Items->Add(buffer);
     codice=ASS_COD2;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD2);
     else          strcpy(buffer, "");
     CBAsseg->Items->Add(buffer);
     codice=ASS_COD3;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD3);
     else          strcpy(buffer, "");
     CBAsseg->Items->Add(buffer);
     codice=ASS_COD4;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD4);
     else          strcpy(buffer, "");
     CBAsseg->Items->Add(buffer);
     codice=ASS_COD5;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD5);
     else          strcpy(buffer, "");
     CBAsseg->Items->Add(buffer);
     codice=ASS_COD6;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD6);
     else          strcpy(buffer, "");
     CBAsseg->Items->Add(buffer);
     codice=ASS_COD7;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD7);
     else          strcpy(buffer, "");
     CBAsseg->Items->Add(buffer);
     codice=ASS_COD8;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD8);
     else          strcpy(buffer, "");
     CBAsseg->Items->Add(buffer);
  }
  else{
     strcpy(buffer, ASS_LAV1); CBAsseg->Items->Add(buffer);
     strcpy(buffer, ASS_LAV2); CBAsseg->Items->Add(buffer);
     strcpy(buffer, ASS_LAV3); CBAsseg->Items->Add(buffer);
     strcpy(buffer, ASS_LAV4); CBAsseg->Items->Add(buffer);
     strcpy(buffer, ASS_LAV5); CBAsseg->Items->Add(buffer);
     strcpy(buffer, ASS_LAV6); CBAsseg->Items->Add(buffer);
     strcpy(buffer, ASS_LAV7); CBAsseg->Items->Add(buffer);
     strcpy(buffer, ASS_LAV8); CBAsseg->Items->Add(buffer);
  }
  CBAsseg->Items->Add(MESS[530]);       // VUOTO

  // --- Compilazione tipi box ammessi ---
  CBTipoBox->Items->Clear();
  for(i=0; i<MAXTIPOBOXIMP; i++){
     if(strcmp(M.TipiBox[i].Nome, "")==0) break;
     CBTipoBox->Items->Add(M.TipiBox[i].Nome);
  }

  // --- Assegna gli indirizzi all'oggetto condiviso ---
  Gruppi();

  // --- imposta i messaggi degli oggetti ---
  Messaggi();

  // --- Lettura struttura da file  ---
  LeggiDati();

  //----------------------------------------------
  // GESTIONE POSIZIONI DINAMICHE MAGAZZINI
  // La capacità viene recuperata dalla lunghezza
  // traccia e dalle dimansioni pallet
  // In caso contrario disattivo l'editor di
  // inserimento della profondità massima perchè
  // non utilizzato
  //----------------------------------------------
  if(GestioneDinamicaMagazzini!=0){
     EdNpt->Enabled=false;
     EdNpt->Color=clLtGray;
  }
  if(GestioneDinamicaMagazzini==0){
     lblProfondita->Visible=false;
     EdProfond->Visible=false;
  }
  //----------------------------------------------
  // MAGAZZINI MULTI NODO
  // Disattivo la visualizzazione del nodo su
  // ogni pos
  //----------------------------------------------
  if(MagazziniMultiNodo==false){
     EdNodP1->Visible=false;
     EdNodP2->Visible=false;
     TextNodP->Visible=false;
  }

  //--------------------------------------------
  // LIVELLO "0" TUTTO IL PANEL PRICIPALE
  // DISATTIVATO
  //--------------------------------------------
  if(LivelloPassword<1){
     PanMagaz->Enabled=false;
  }
  else{
     PanMagaz->Enabled=true;
  }
  //----------------------------------------------
  // Disattivazioni finestra con Password
  // inferiore a Livello 2.
  //----------------------------------------------
  if(LivelloPassword<2){
     CBTipoBox->Enabled=false;
  }
  else{
     CBTipoBox->Enabled=true;
  }
  //---------------------------------------------
  // Disattivazioni finestra con Password
  // inferiore a Livello 3.
  //---------------------------------------------
  if(LivelloPassword<3){
     BtnCopia->Enabled         = false;
     BtnCancella->Enabled      = false;
     BtnInserimento->Enabled   = false;
     GBConfigDeposito->Enabled = false;
     //--------------------------------------------
     // Andrea 15/05/2018
     //--------------------------------------------
     EdNum->Enabled = false;
     EdNodIng->Enabled = false;
     EdNodOut->Enabled = false;
  }
  else{
     BtnCopia->Enabled         = true;
     BtnCancella->Enabled      = true;
     BtnInserimento->Enabled   = true;
     GBConfigDeposito->Enabled = true;
     //--------------------------------------------
     // Andrea 15/05/2018
     //--------------------------------------------
     EdNum->Enabled = true;
     EdNodIng->Enabled = true;
     EdNodOut->Enabled = true;
  }
  //---------------------------------------------
  // Refresh della Form
  //---------------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormMagaz::AzzeraTimerClick(TObject *Sender)
{
  //------------------------------------------------------
  // Disabilita/Abilita Timer per riprendere il conteggio
  // del tempo
  //------------------------------------------------------
  TimerClose->Enabled = false;
  TimerClose->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMagaz::AzzeraTimerMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
  //------------------------------------------------------
  // Disabilita/Abilita Timer per riprendere il conteggio
  // del tempo
  //------------------------------------------------------
  TimerClose->Enabled = false;
  TimerClose->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMagaz::TimerCloseTimer(TObject *Sender)
{
   FormMagaz->Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMagaz::FormClose(TObject *Sender, TCloseAction &Action)
{
  //----------------------------------------
  // Se Dati BOX modificati Salva comunque
  // 1=Dati magazzino precedenti
  //----------------------------------------
  if(DatiBoxModificati){
     DatiBoxModificati=1;
     Salva();
  }
  //----------------------------------------------
  // Diabilitazione del Timer di chiusura
  //----------------------------------------------
  TimerClose->Enabled=false;
  //----------------------------------------------
  // Editor macchine chiuso
  //----------------------------------------------
  NumeroMagazzino=0;
  EditorMagazzinoAttivo=0;
}
//---------------------------------------------------------------------------
// +--------------------------------------------------+
// | Intercettazione dei Pulsanti premuti da Tastiera |
// +--------------------------------------------------+
void __fastcall TFormMagaz::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
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
void __fastcall TFormMagaz::Gruppi()
{
  CBPres[0] = CBPres0;
  CBPres[1] = CBPres1;
  CBPres[2] = CBPres2;
}
//---------------------------------------------------------------------------

// +------------------------------------------+
// | Imposta i messaggi abbinati agli oggetti |
// +------------------------------------------+
void __fastcall TFormMagaz::Messaggi()
{
  int  i;
  char buffer[101]="";

  FormMagaz->Caption          = MESS[277];

  TextNum->Caption            = MESS[300];
  TextNod1->Caption           = MESS[302];
  TextNod2->Caption           = MESS[303];
  TextNpt->Caption            = MESS[305];
  TextCest->Caption           = MESS[306];
  CBCompleto->Caption         = MESS[307];
  TextSovrap->Caption         = MESS[308];
  TextMaxPianoP->Caption      = MESS[311];
  TextDat->Caption            = MESS[309];
  TextOra->Caption            = MESS[310];

  //------------------------------------------
  // Gruppo di appartenenza
  //------------------------------------------
  CBGruppo->Clear();
  for(i=0; i<MAXGRUPPIIMP; i++){
     if(strcmp(P.GruppiBaie[i].Nome, "")==0) continue;
     CBGruppo->Items->Add(P.GruppiBaie[i].Nome);
  }

  RGCarWash->Caption             = MESS[304];
  RGCarWash->Items->Strings[ 0]  = MESS[294];
  RGCarWash->Items->Strings[ 1]  = MESS[311];
  RGCarWash->Items->Strings[ 2]  = MESS[312];

  RBRotTutti->Caption         = MESS[294];
  RBSoloRuotati->Caption      = MESS[313];
  RBNoRuotati->Caption        = MESS[314];
  TextMaxPianoP->Caption      = MESS[320];

  GBTipo->Caption             = MESS[293];
  GBTipo->Items->Strings[ 0]  = MESS[294];
  GBTipo->Items->Strings[ 1]  = MESS[295];
  GBTipo->Items->Strings[ 2]  = MESS[296];
  GBTipo->Items->Strings[ 3]  = MESS[297];
  GBTipo->Items->Strings[ 4]  = MESS[298];

  GBPosizioni->Caption        = MESS[319];
  GBStato->Caption            = MESS[315];
  GBStato->Items->Strings[ 0] = MESS[316];
  GBStato->Items->Strings[ 1] = MESS[317];
  GBStato->Items->Strings[ 2] = MESS[318];

  sprintf(buffer, MESS[321], 0);  GBPiano0->Caption = buffer;
  sprintf(buffer, MESS[321], 1);  GBPiano1->Caption = buffer;
  sprintf(buffer, MESS[321], 2);  GBPiano2->Caption = buffer;

  CBPres0->Caption        = MESS[325];
  CBPres1->Caption        = MESS[325];
  CBPres2->Caption        = MESS[325];

  BtnBox0->Caption        = MESS[528];
  BtnBox1->Caption        = MESS[528];
  BtnBox2->Caption        = MESS[528];

  BtnIndietro->Caption    = MESS[333];
  BtnAvanti->Caption      = MESS[332];
  BtnCerca->Caption       = MESS[334];
  BtnCopia->Caption       = MESS[335];
  BtnSalva->Caption       = MESS[337];
  BtnStampa->Caption      = MESS[339];
  BtnCancella->Caption    = MESS[338];
  BtnInserimento->Caption = MESS[336];
  BtnEsci->Caption        = MESS[171];
  //---------------------------------------
  CBFifo->Caption            = MESS[301];
  GBAssegnaz->Caption        = MESS[721];
  TextNodP->Caption          = MESS[673];
  GBConfigDeposito->Caption  = MESS[ 27];
  lblProfondita->Caption     = MESS[723];
  lblSollevam->Caption       = MESS[713];
  lblOffsetNode->Caption     = MESS[722];
  lblAltezza->Caption        = MESS[461];
}
//---------------------------------------------------------------------------

// +------------------------+
// | Legge i Dati dai file  |
// +------------------------+
void __fastcall TFormMagaz::LeggiDati()
{
  short int i, j, k;
  short int tipo_prov_box, mac_prov_box, bai_prov_box, mag_prov_box;

  M.rec_attuale = IndiceMagazzino;

  setmem(&M.EditMag, sizeof(MAG), 0);             // azzera la struttura
  //-------------------------------------
  // Recupera il primo record
  //-------------------------------------
  memcpy( &M.EditMag, M.m[IndiceMagazzino], sizeof( MAG ) );
  NumRec = M.num_record_mag();
  //-------------------------------------
  // GESTIONE MAGAZZINI ACCOPPIATI
  //-------------------------------------
  NumMagAccoppiato=0;
  for(i=0;;i++){
     if(AccoppiaMagaz[i][0]==0 && AccoppiaMagaz[i][1]==0) break;
     if(AccoppiaMagaz[i][0]!=M.EditMag.Num && AccoppiaMagaz[i][1]!=M.EditMag.Num) continue;
     if(AccoppiaMagaz[i][0]==M.EditMag.Num){NumMagAccoppiato=AccoppiaMagaz[i][1]; break;}
     if(AccoppiaMagaz[i][1]==M.EditMag.Num){NumMagAccoppiato=AccoppiaMagaz[i][0]; break;}
  }
  //-------------------------------------
  // GESTIONE MAGAZZINI TEMPORANEI
  //-------------------------------------
  NumMagTemporaneo=0;
  for(i=1; i<=TOTMAG; i++){
     if(M.m[i]->Num<1              ) continue;
     if(M.m[i]->Num>TOTMAG         ) continue;
     if(M.m[i]->Num==M.EditMag.Num ) continue;
     if(M.m[i]->Tipo!=MG_TEMPORANEO) continue;
     //------------------------------------------
     // Verifica se in un magazzino TMP esiste
     // almeno un pallet da riportare
     //------------------------------------------
     for(k=MAXTERRAMAG; k>0; k--){
        for(j=MAXPIANIMAG-1; j>=0; j--){
           if(M.m[i]->Pos[k].Presenza[j]==false) continue;
           //------------------------------------------------
           // Decodifica provenieza
           //------------------------------------------------
           tipo_prov_box=mac_prov_box=bai_prov_box=mag_prov_box=0;
           P.TestProvenienzaBox(M.m[i]->Pos[k].Box[j].ProvBox, &tipo_prov_box, &mac_prov_box, &bai_prov_box, &mag_prov_box);
           if(tipo_prov_box!=PROV_MAGAZZINO ) continue;
           if(mag_prov_box!=M.EditMag.Num   ) continue;
           NumMagTemporaneo = M.m[i]->Num;
           break;
        }
        if(NumMagTemporaneo!=0) break;
     }
     if(NumMagTemporaneo!=0) break;
  }
}
//---------------------------------------------------------------------------
// ---------------------------
//    Avanti()
// ---------------------------
// Funzione che avanza di un record
//
void __fastcall TFormMagaz::Avanti()
{
  int err=0;
  int i, n_rec;
  short int j, k;
  short int tipo_prov_box, mac_prov_box, bai_prov_box, mag_prov_box;
  struct MAG mg;

  //----------------------------------------
  // Se Dati BOX modificati Salva comunque
  // 1=Dati magazzino precedenti
  //----------------------------------------
  if(DatiBoxModificati){
     DatiBoxModificati=1;
     Salva();
  }

  //--------------------------------
  //  cerca il prossimo record
  //--------------------------------
  for( n_rec=M.rec_attuale+1; n_rec<=MAXMAG; n_rec++){
     if( M.m[ n_rec ]->Num >0 ) break;
  }

  if( n_rec>MAXMAG ){
     Application->MessageBox(MESS[428], MESS[99], MB_OK | MB_ICONINFORMATION);
     return;
  }

  //------------------------------------
  //  Recupera dati nella struttura ch
  //------------------------------------
  memset(&mg, 0, sizeof(MAG));
  err = M.cerca_record_mag(&mg, n_rec );
  if( err==0 ){
     M.rec_attuale       = (short int) n_rec;
     IndiceMagazzino     = (short int) n_rec;
     memcpy( &M.EditMag, &mg, sizeof( MAG ) );
  }
  Inserimento=false;

  // posizionamento automatico sulla prima posizione
  IndicePosizione=1;

  //-------------------------------------
  // GESTIONE MAGAZZINI ACCOPPIATI
  //-------------------------------------
  NumMagAccoppiato=0;
  for(i=0;;i++){
     if(AccoppiaMagaz[i][0]==0 && AccoppiaMagaz[i][1]==0) break;
     if(AccoppiaMagaz[i][0]!=M.EditMag.Num && AccoppiaMagaz[i][1]!=M.EditMag.Num) continue;
     if(AccoppiaMagaz[i][0]==M.EditMag.Num){NumMagAccoppiato=AccoppiaMagaz[i][1]; break;}
     if(AccoppiaMagaz[i][1]==M.EditMag.Num){NumMagAccoppiato=AccoppiaMagaz[i][0]; break;}
  }
  //-------------------------------------
  // GESTIONE MAGAZZINI TEMPORANEI
  //-------------------------------------
  NumMagTemporaneo=0;
  for(i=1; i<=TOTMAG; i++){
     if(M.m[i]->Num<1              ) continue;
     if(M.m[i]->Num>TOTMAG         ) continue;
     if(M.m[i]->Num==M.EditMag.Num ) continue;
     if(M.m[i]->Tipo!=MG_TEMPORANEO) continue;
     //------------------------------------------
     // Verifica se in un magazzino TMP esiste
     // almeno un pallet da riportare
     //------------------------------------------
     for(k=MAXTERRAMAG; k>0; k--){
        for(j=MAXPIANIMAG-1; j>=0; j--){
           if(M.m[i]->Pos[k].Presenza[j]==false) continue;
           //------------------------------------------------
           // Decodifica provenieza
           //------------------------------------------------
           tipo_prov_box=mac_prov_box=bai_prov_box=mag_prov_box=0;
           P.TestProvenienzaBox(M.m[i]->Pos[k].Box[j].ProvBox, &tipo_prov_box, &mac_prov_box, &bai_prov_box, &mag_prov_box);
           if(tipo_prov_box!=PROV_MAGAZZINO ) continue;
           if(mag_prov_box!=M.EditMag.Num   ) continue;
           NumMagTemporaneo = M.m[i]->Num;
           break;
        }
        if(NumMagTemporaneo!=0) break;
     }
     if(NumMagTemporaneo!=0) continue;
  }
  // --- Refresh della Form
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Indietro()
// ---------------------------
// Funzione che retrocede di un record
//
void __fastcall TFormMagaz::Indietro()
{
  int err=0;
  int i, j, k, n_rec;
  struct MAG mg;
  short int tipo_prov_box, mac_prov_box, bai_prov_box, mag_prov_box;

  //----------------------------------------
  // Se Dati BOX modificati Salva comunque
  // 1=Dati magazzino precedenti
  //----------------------------------------
  if(DatiBoxModificati){
     DatiBoxModificati=1;
     Salva();
  }
  //--------------------------------
  //  cerca il precedente record
  //--------------------------------
  for( n_rec=M.rec_attuale-1; n_rec>0; n_rec--){
     if( M.m[ n_rec ]->Num >0 ) break;
  }
  if( n_rec<1 ){
     Application->MessageBox(MESS[429], MESS[99], MB_OK | MB_ICONINFORMATION);
     return;
  }

  //------------------------------------
  //  Recupera dati nella struttura ch
  //------------------------------------
  memset(&mg, 0, sizeof(MAG));
  err = M.cerca_record_mag(&mg, n_rec );
  if( err==0 ){
     M.rec_attuale       = (short int) n_rec;
     IndiceMagazzino     = (short int) n_rec;
     memcpy( &M.EditMag, &mg, sizeof( MAG ) );
  }
  Inserimento=false;

  // posizionamento automatico sulla prima posizione
  IndicePosizione=1;

  //-------------------------------------
  // GESTIONE MAGAZZINI ACCOPPIATI
  //-------------------------------------
  NumMagAccoppiato=0;
  for(i=0;;i++){
     if(AccoppiaMagaz[i][0]==0 && AccoppiaMagaz[i][1]==0) break;
     if(AccoppiaMagaz[i][0]!=M.EditMag.Num && AccoppiaMagaz[i][1]!=M.EditMag.Num) continue;
     if(AccoppiaMagaz[i][0]==M.EditMag.Num){NumMagAccoppiato=AccoppiaMagaz[i][1]; break;}
     if(AccoppiaMagaz[i][1]==M.EditMag.Num){NumMagAccoppiato=AccoppiaMagaz[i][0]; break;}
  }

  //-------------------------------------
  // GESTIONE MAGAZZINI TEMPORANEI
  //-------------------------------------
  NumMagTemporaneo=0;
  for(i=1; i<=TOTMAG; i++){
     if(M.m[i]->Num<1              ) continue;
     if(M.m[i]->Num>TOTMAG         ) continue;
     if(M.m[i]->Num==M.EditMag.Num ) continue;
     if(M.m[i]->Tipo!=MG_TEMPORANEO) continue;
     //------------------------------------------
     // Verifica se in un magazzino TMP esiste
     // almeno un pallet da riportare
     //------------------------------------------
     for(k=MAXTERRAMAG; k>0; k--){
        for(j=MAXPIANIMAG-1; j>=0; j--){
           if(M.m[i]->Pos[k].Presenza[j]==false) continue;
           //------------------------------------------------
           // Decodifica provenieza
           //------------------------------------------------
           tipo_prov_box=mac_prov_box=bai_prov_box=mag_prov_box=0;
           P.TestProvenienzaBox(M.m[i]->Pos[k].Box[j].ProvBox, &tipo_prov_box, &mac_prov_box, &bai_prov_box, &mag_prov_box);
           if(tipo_prov_box!=PROV_MAGAZZINO ) continue;
           if(mag_prov_box!=M.EditMag.Num   ) continue;
           NumMagTemporaneo = M.m[i]->Num;
           break;
        }
        if(NumMagTemporaneo!=0) break;
     }
     if(NumMagTemporaneo!=0) continue;
  }
  // --- Refresh della Form
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Cerca()
// ---------------------------
// Funzione che cerca un record
//
void __fastcall TFormMagaz::Cerca()
{
  AnsiString InputStringa;
  char Chiave[5]="";
  int n_rec=1, err=0;
  struct MAG mg;

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
  if(n_rec<0 || n_rec>MAXMAG){
     Application->MessageBox(MESS[375], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  //------------------------------------
  //  Recupera record
  //------------------------------------
  memset(&mg, 0, sizeof(MAG));
  err = M.cerca_record_mag(&mg, n_rec );
  if(err) {
     Application->MessageBox(MESS[375], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  M.rec_attuale       = (short int) n_rec;
  IndiceMagazzino     = (short int) n_rec;
  memcpy( &M.EditMag, &mg, sizeof( MAG ));
  Inserimento=false;

  // posizionamento automatico sulla prima posizione
  IndicePosizione=1;

  // --- Refresh della Form
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Copia()
// ---------------------------
// Funzione che copia un record MACCHINE
//
void __fastcall TFormMagaz::Copia()
{
  AnsiString InputStringa;
  char buffer[201]="";
  char Chiave[5]="";
  int i, j, k, result;
  int n_rec=1, err=0;
  short int tipo_prov_box, mac_prov_box, bai_prov_box, mag_prov_box;

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
  if(n_rec<0 || n_rec>MAXMAG){
     Application->MessageBox(MESS[375], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  //------------------------------------
  //  Verifica se record esistente
  //------------------------------------
  if( n_rec == M.rec_attuale ) return;
  if( M.m[ n_rec ]->Num >0 ){
     result = Application->MessageBox(MESS[376], MESS[99], MB_YESNO | MB_ICONQUESTION);
     if(result != IDYES) return;
  }

  //-----------------
  //  Copia record
  //-----------------
  memcpy( M.m[ n_rec ], M.m[ M.rec_attuale ], sizeof( MAG ) );
  memcpy( &M.EditMag,  M.m[ M.rec_attuale ], sizeof( MAG ) );
  M.rec_attuale     = (short int) n_rec;
  IndiceMagazzino   = (short int) n_rec;
  M.m[ n_rec ]->Num = (short int) n_rec;
  M.EditMag.Num    = (short int) n_rec;

  //-----------------------------------------
  // In caso di COPIA azzero i nodi perchè
  // possano essere ricalcolati in auto
  //-----------------------------------------
  M.EditMag.PuntoIng=0;
  M.EditMag.PuntoOut=0;
  for(i=1; i<=M.EditMag.Npt; i++){
     M.EditMag.Pos[i].NodoIng=0;
     M.EditMag.Pos[i].NodoOut=0;
  }

  Inserimento=false;

  NumRec = M.num_record_mag();
  //-------------------------------------
  // GESTIONE MAGAZZINI ACCOPPIATI
  //-------------------------------------
  NumMagAccoppiato=0;
  for(i=0;;i++){
     if(AccoppiaMagaz[i][0]==0 && AccoppiaMagaz[i][1]==0) break;
     if(AccoppiaMagaz[i][0]!=M.EditMag.Num && AccoppiaMagaz[i][1]!=M.EditMag.Num) continue;
     if(AccoppiaMagaz[i][0]==M.EditMag.Num){NumMagAccoppiato=AccoppiaMagaz[i][1]; break;}
     if(AccoppiaMagaz[i][1]==M.EditMag.Num){NumMagAccoppiato=AccoppiaMagaz[i][0]; break;}
  }
  //-------------------------------------
  // GESTIONE MAGAZZINI TEMPORANEI
  //-------------------------------------
  NumMagTemporaneo=0;
  for(i=1; i<=TOTMAG; i++){
     if(M.m[i]->Num<1              ) continue;
     if(M.m[i]->Num>TOTMAG         ) continue;
     if(M.m[i]->Num==M.EditMag.Num ) continue;
     if(M.m[i]->Tipo!=MG_TEMPORANEO) continue;
     //------------------------------------------
     // Verifica se in un magazzino TMP esiste
     // almeno un pallet da riportare
     //------------------------------------------
     for(k=MAXTERRAMAG; k>0; k--){
        for(j=MAXPIANIMAG-1; j>=0; j--){
           if(M.m[i]->Pos[k].Presenza[j]==false) continue;
           //------------------------------------------------
           // Decodifica provenieza
           //------------------------------------------------
           tipo_prov_box=mac_prov_box=bai_prov_box=mag_prov_box=0;
           P.TestProvenienzaBox(M.m[i]->Pos[k].Box[j].ProvBox, &tipo_prov_box, &mac_prov_box, &bai_prov_box, &mag_prov_box);
           if(tipo_prov_box!=PROV_MAGAZZINO ) continue;
           if(mag_prov_box!=M.EditMag.Num   ) continue;
           NumMagTemporaneo = M.m[i]->Num;
           break;
        }
        if(NumMagTemporaneo!=0) break;
     }
     if(NumMagTemporaneo!=0) continue;
  }
  //-----------------------------------------
  // Ricompilazione dei Punti notevoli
  //-----------------------------------------
  err = N.compila_punti_notevoli(buffer);
  if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //-----------------------------------------
  //  Salva record sul file
  //-----------------------------------------
  err = M.save_record_mag( M.rec_attuale, buffer);
  if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //-----------------------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------------------
  sprintf(buffer, "(EDITMAG) - (Mag:%d) Copia Editor Magazziono", IndiceMagazzino);
  FileLogOperatore(buffer);

  // posizionamento automatico sulla prima posizione
  IndicePosizione=1;

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
void __fastcall TFormMagaz::Salva()
{
  bool   magazzino_libero;
  bool   magazzini_pieni;
  bool   box_con_statolav_diversi;
  bool   manca_presenza_presa;
  bool   almeno_un_pallet_da_spostare;
  int    SovrapCarWash = SOVRAPPOSIZIONE_CARWASH;
  char   buffer[120]="";
  char   NomeFile[100]="";
  double a, b;
  int  i, j, k, z, err, result;
  short int lato_box=LUNG_CEST;
  short int pos_trovata;
  short int count_pos_impegnate;
  short int AbilitaPianoMag;

  //----------------------------------------------------------------
  // Richiesta di conferma salvataggio
  //----------------------------------------------------------------
  err=0;
  if( DatiBoxModificati!=1 ){
     result = Application->MessageBox(MESS[380], MESS[99], MB_YESNO | MB_ICONQUESTION);
     if(result != IDYES) return;
  }
  //----------------------------------------------------------------
  //  Recupera dati dalle caselle di dialogo
  //----------------------------------------------------------------
  GetCaselle();
  if(M.EditMag.Num>MAXMAG) return;
  //----------------------------------------------------------------
  // Richiesta di conferma salvataggio
  //----------------------------------------------------------------
  if( DatiBoxModificati ){
     //----------------------------------------------------------------
     // E' il caso dove si vuole uscire senza modificare i dati VECCHI
     // Nel qual caso si rcuperano i dati precedenti e si sovrascrive
     // solo i dati BOX
     //----------------------------------------------------------------
     if( DatiBoxModificati==1 ){
        memcpy( &M.EditMag, M.m[M.EditMag.Num], sizeof( MAG ) );
     }
  }

  //----------------------------------------------------------------
  // Sovrapposizione: AMMESSO MAX UN PIANO
  //----------------------------------------------------------------
  if(M.EditMag.Sovrapposizione>MAXPIANIMAG-1) M.EditMag.Sovrapposizione=MAXPIANIMAG-1;

  //----------------------------------------------------------------
  // Abilitazione sovrapposizione palette CAR-WASH
  //----------------------------------------------------------------
  if(SovrapCarWash==0 && M.EditMag.PerEmergenza==1) M.EditMag.Sovrapposizione=0;

  //----------------------------------------------------------------
  // Controlli automatici
  //----------------------------------------------------------------
  // MAGAZZINO LIBERO
  magazzino_libero=true;
  count_pos_impegnate=0;
  for(i=1; i<=MAXTERRAMAG; i++){
     for(j=0; j<MAXPIANIMAG; j++){
        if(M.EditMag.Pos[i].Presenza[j]==false) continue;
      //if(M.EditMag.Pos[i].Box[j].StatoLav==0) continue;
        magazzino_libero=false;
        count_pos_impegnate++;
     }
     //Piani Max per Posizione
     if(M.EditMag.Pos[i].MaxHPos>M.EditMag.Sovrapposizione+1) M.EditMag.Pos[i].MaxHPos = M.EditMag.Sovrapposizione+1;
  }

  //----------------------------------------------------------------
  // AGGIORNAMENTO POSIZIONE IMPEGNATE IN BASE A FLAG DI PRESENZA
  //----------------------------------------------------------------
  M.EditMag.NumCestoni = count_pos_impegnate;

  //---------------------------------------------------------------
  // Azzera Box oltre piano massimo ammesso
  //---------------------------------------------------------------
  for(i=1; i<=MAXTERRAMAG; i++){
     for(j=M.EditMag.Sovrapposizione+1; j<MAXPIANIMAG; j++){
        M.EditMag.Pos[i].Presenza[j] = false;
        M.EditMag.Pos[i].QuotaH[j]   = 0;
        M.EditMag.Pos[i].QuotaP[j]   = 0;
        memset(&M.EditMag.Pos[i].Box[j], 0, sizeof(BOX));
     }
  }

  //----------------------------------------------------------------
  // Magazzini FIFO (quando nodo di deposito e di prelievo non
  // conicidono
  //----------------------------------------------------------------
  if(M.EditMag.PuntoIng!=0 && M.EditMag.PuntoOut!=0){
     M.EditMag.Fifo=false;
     if(M.EditMag.PuntoIng!=M.EditMag.PuntoOut) M.EditMag.Fifo=true;
  }
  //----------------------------------------------------------------
  // Azzeramento dati x MAGAZZINO LIBERO
  //----------------------------------------------------------------
  if(magazzino_libero==true || M.EditMag.NumCestoni==0){
     M.EditMag.Stato      = MG_ST_FREE;
     M.EditMag.NumCestoni = 0;
     M.EditMag.Completo   = false;
     magazzino_libero     = true;
     for(i=1; i<=MAXTERRAMAG; i++){
        for(j=0; j<MAXPIANIMAG; j++){
           M.EditMag.Pos[i].Presenza[j] = false;
           M.EditMag.Pos[i].QuotaH[j]   = 0;
           M.EditMag.Pos[i].QuotaP[j]   = 0;
           memset(&M.EditMag.Pos[i].Box[j], 0, sizeof(BOX));
        }
     }
     //-----------------------------------------------
     // GESTIONE_TIPOBOX
     // Se è abilitata non resetto il tipo box a
     // magazzino, ma lascio quello impostato
     // manualmente
     //-----------------------------------------------
     #ifndef GESTIONE_TIPOBOX
        M.EditMag.TipoBox = 0;
     #endif;
  }

  //----------------------------------------------------------------
  // SICUREZZA: tutte le postazioni ai piani superiori devono
  //            avere il corrispondente al piano precedente
  //
  // NOTA: Questo controllo è valido solo se non ho il la
  //       GESTIONE_DINAMICA_MAG. In quel caso è possibile
  //       che al TGV risulti di aver depositato su una postazione
  //       alta, metre a PC questa posizione non ha BOX a terra
  //----------------------------------------------------------------
  if(GestioneDinamicaMagazzini==0){
     for(i=1; i<=MAXTERRAMAG; i++){
        for(j=1; j<MAXPIANIMAG; j++){
           if(M.EditMag.Pos[i].Presenza[j]==false) continue;
           //-----------------------------------------
           // Controllo lo stato del piano precedente
           //-----------------------------------------
           if(M.EditMag.Pos[i].Presenza[j-1]==false){
              sprintf(buffer, MESS[322], i, j);
              ShowMessage(buffer);
              return;
           }
        }
     }
  }

  //----------------------------------------------------------------
  // SICUREZZA: sbiancamento dati per tutte le posizioni con
  //            "presenza" a false
  //----------------------------------------------------------------
  for(i=1; i<=MAXTERRAMAG; i++){
     for(j=0; j<MAXPIANIMAG; j++){
        if(M.EditMag.Pos[i].Presenza[j]==true) continue;
        M.EditMag.Pos[i].QuotaH[j] = 0;
        M.EditMag.Pos[i].QuotaP[j] = 0;
        memset(&M.EditMag.Pos[i].Box[j], 0, sizeof(BOX));
     }
  }

  //----------------------------------------------------------------
  // Assegnazione stato lavorazione ai box in magazzino
  // - NO GESTIONE ARCHIVIO BOX: salvo lo stato di lavorazione ai
  //                             box in base al tipo di magazzino
  // - OK GESTIONE ARCHIVIO BOX: segnalo anomalia se nel magazzino
  //                             ci sono box con stato di
  //                             lavorazione differente.
  //----------------------------------------------------------------
  box_con_statolav_diversi=false;
  //--------------------------------------------
  // Aggiornamento automatico stato magazzino
  //--------------------------------------------
  for(i=1; i<=MAXTERRAMAG; i++){
     for(j=0; j<MAXPIANIMAG; j++){
        if(M.EditMag.Pos[i].Presenza[j]==false) continue;
        //------------------------------------------
        // Assegnazione del TIPO BOX a tutte le pos
        //------------------------------------------
        M.EditMag.Pos[i].Box[j].TipoBox = M.EditMag.TipoBox;
        //------------------------------------------
        // Se la presenza della posizione è alta e
        // non c'è nessuna presenza in PRESA setto
        // tutte le presenze in presa
        //------------------------------------------
        manca_presenza_presa=true;
        for(k=0; k<Magaz[M.EditMag.Num][2]; k++){
           if(M.EditMag.Pos[i].Box[j].Presa[k][0]==false) continue;
           manca_presenza_presa=false;
           break;
        }
        if(manca_presenza_presa==true){
           for(k=0; k<Magaz[M.EditMag.Num][2]; k++) M.EditMag.Pos[i].Box[j].Presa[k][0]=true;
        }
        //------------------------------------------
        // Assegnazione automatica del codice
        // solo non è attiva l'assegnazione dinamica
        //------------------------------------------
        if(M.EditMag.Assegnazione!=ASS_DINAMICA){
           switch(M.EditMag.Assegnazione){
              case PRES_LAV1: M.EditMag.Pos[i].Box[j].StatoLav=ST_BOX_LAV1;
                              if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[i].Box[j].Codice=ASS_COD1;
                              break;
              case PRES_LAV2: M.EditMag.Pos[i].Box[j].StatoLav=ST_BOX_LAV2;
                              if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[i].Box[j].Codice=ASS_COD2;
                              break;
              case PRES_LAV3: M.EditMag.Pos[i].Box[j].StatoLav=ST_BOX_LAV3;
                              if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[i].Box[j].Codice=ASS_COD3;
                              break;
              case PRES_LAV4: M.EditMag.Pos[i].Box[j].StatoLav=ST_BOX_LAV4;
                              if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[i].Box[j].Codice=ASS_COD4;
                              break;
              case PRES_LAV5: M.EditMag.Pos[i].Box[j].StatoLav=ST_BOX_LAV5;
                              if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[i].Box[j].Codice=ASS_COD5;
                              break;
              case PRES_LAV6: M.EditMag.Pos[i].Box[j].StatoLav=ST_BOX_LAV6;
                              if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[i].Box[j].Codice=ASS_COD6;
                              break;
              case PRES_LAV7: M.EditMag.Pos[i].Box[j].StatoLav=ST_BOX_LAV7;
                              if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[i].Box[j].Codice=ASS_COD7;
                              break;
              case PRES_LAV8: M.EditMag.Pos[i].Box[j].StatoLav=ST_BOX_LAV8;
                              if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[i].Box[j].Codice=ASS_COD8;
                              break;
              case PRES_VUOTO:M.EditMag.Pos[i].Box[j].StatoLav=ST_BOX_VUOTI;     // in caso di vuoti azzero tutti i dati
                              M.EditMag.Pos[i].Box[j].Codice  =ASS_VUOTO;
                              M.EditMag.Pos[i].Box[j].Altezza =0;
                              M.EditMag.Pos[i].Box[j].Basso   =false;
                              M.EditMag.Pos[i].Box[j].Sovrap  =false;
                              M.EditMag.Pos[i].Box[j].CarWash =0;
                              break;
                      default:M.EditMag.Pos[i].Box[j].StatoLav=ST_BOX_VUOTI;
                              M.EditMag.Pos[i].Box[j].Codice  =ASS_VUOTO;
                              break;
           }
        }
     }
  }
  //----------------------------------------------------------------
  // GESTIONE MAGAZZINI ACCOPPIATI
  // Estendo alcuni controlli anche al magazzino accoppiato
  //----------------------------------------------------------------
  if(NumMagAccoppiato>0 && NumMagAccoppiato<=TOTMAG){
     M.m[NumMagAccoppiato]->Tipo         = M.EditMag.Tipo;
     M.m[NumMagAccoppiato]->TipoBox      = M.EditMag.TipoBox;
     M.m[NumMagAccoppiato]->Assegnazione = M.EditMag.Assegnazione;
     M.m[NumMagAccoppiato]->Sovrapposizione = M.EditMag.Sovrapposizione;  // Andrea 04/06/2018
     //--------------------------------------------
     // Aggiornamento automatico stato magazzino
     //--------------------------------------------
     for(i=1; i<=MAXTERRAMAG; i++){
        for(j=0; j<MAXPIANIMAG; j++){
           if(M.m[NumMagAccoppiato]->Pos[i].Presenza[j]==false) continue;
           //------------------------------------------
           // Assegnazione del TIPO BOX a tutte le pos
           //------------------------------------------
           M.m[NumMagAccoppiato]->Pos[i].Box[j].TipoBox = M.EditMag.TipoBox;
           //------------------------------------------
           // Se la presenza della posizione è alta e
           // non c'è nessuna presenza in PRESA setto
           // tutte le presenze in presa
           //------------------------------------------
           manca_presenza_presa=true;
           for(k=0; k<Magaz[M.m[NumMagAccoppiato]->Num][2]; k++){
              if(M.m[NumMagAccoppiato]->Pos[i].Box[j].Presa[k][0]==false) continue;
              manca_presenza_presa=false;
              break;
           }
           if(manca_presenza_presa==true){
              for(k=0; k<Magaz[M.m[NumMagAccoppiato]->Num][2]; k++) M.m[NumMagAccoppiato]->Pos[i].Box[j].Presa[k][0]=true;
           }
           //------------------------------------------
           // Assegnazione automatica del codice
           // solo non è attiva l'assegnazione dinamica
           //------------------------------------------
           if(M.EditMag.Assegnazione!=ASS_DINAMICA){
              switch(M.EditMag.Assegnazione){
                 case PRES_LAV1: M.m[NumMagAccoppiato]->Pos[i].Box[j].StatoLav=ST_BOX_LAV1;
                                 if(AbbinaCodiceStatoBox==true) M.m[NumMagAccoppiato]->Pos[i].Box[j].Codice=ASS_COD1;
                                 break;
                 case PRES_LAV2: M.m[NumMagAccoppiato]->Pos[i].Box[j].StatoLav=ST_BOX_LAV2;
                                 if(AbbinaCodiceStatoBox==true) M.m[NumMagAccoppiato]->Pos[i].Box[j].Codice=ASS_COD2;
                                 break;
                 case PRES_LAV3: M.m[NumMagAccoppiato]->Pos[i].Box[j].StatoLav=ST_BOX_LAV3;
                                 if(AbbinaCodiceStatoBox==true) M.m[NumMagAccoppiato]->Pos[i].Box[j].Codice=ASS_COD3;
                                 break;
                 case PRES_LAV4: M.m[NumMagAccoppiato]->Pos[i].Box[j].StatoLav=ST_BOX_LAV4;
                                 if(AbbinaCodiceStatoBox==true) M.m[NumMagAccoppiato]->Pos[i].Box[j].Codice=ASS_COD4;
                                 break;
                 case PRES_LAV5: M.m[NumMagAccoppiato]->Pos[i].Box[j].StatoLav=ST_BOX_LAV5;
                                 if(AbbinaCodiceStatoBox==true) M.m[NumMagAccoppiato]->Pos[i].Box[j].Codice=ASS_COD5;
                                 break;
                 case PRES_LAV6: M.m[NumMagAccoppiato]->Pos[i].Box[j].StatoLav=ST_BOX_LAV6;
                                 if(AbbinaCodiceStatoBox==true) M.m[NumMagAccoppiato]->Pos[i].Box[j].Codice=ASS_COD6;
                                 break;
                 case PRES_LAV7: M.m[NumMagAccoppiato]->Pos[i].Box[j].StatoLav=ST_BOX_LAV7;
                                 if(AbbinaCodiceStatoBox==true) M.m[NumMagAccoppiato]->Pos[i].Box[j].Codice=ASS_COD7;
                                 break;
                 case PRES_LAV8: M.m[NumMagAccoppiato]->Pos[i].Box[j].StatoLav=ST_BOX_LAV8;
                                 if(AbbinaCodiceStatoBox==true) M.m[NumMagAccoppiato]->Pos[i].Box[j].Codice=ASS_COD8;
                                 break;
                 case PRES_VUOTO:M.m[NumMagAccoppiato]->Pos[i].Box[j].StatoLav=ST_BOX_VUOTI;     // in caso di vuoti azzero tutti i dati
                                 M.m[NumMagAccoppiato]->Pos[i].Box[j].Codice  =ASS_VUOTO;
                                 M.m[NumMagAccoppiato]->Pos[i].Box[j].Altezza =0;
                                 M.m[NumMagAccoppiato]->Pos[i].Box[j].Basso   =false;
                                 M.m[NumMagAccoppiato]->Pos[i].Box[j].Sovrap  =false;
                                 M.m[NumMagAccoppiato]->Pos[i].Box[j].CarWash =0;
                                 break;
                         default:M.m[NumMagAccoppiato]->Pos[i].Box[j].StatoLav=ST_BOX_VUOTI;
                                 M.m[NumMagAccoppiato]->Pos[i].Box[j].Codice  =ASS_VUOTO;
                                 break;
              }
           }
        }
     }
  }
  //----------------------------------------------------------------
  // Segnalazione box incongruenti in magazzino
  //----------------------------------------------------------------
  if(box_con_statolav_diversi==true){
     strcpy(buffer, MESS[323]);
     Application->MessageBox(buffer, MESS[99], MB_OK | MB_ICONINFORMATION);
  }

  //----------------------------------------------------------------
  // !!! POSIZIONE IMPORTANTE !!!
  // ASSEGNO LO STATO AL MAGAZZINO IN BASE ALLO STATO DELLA
  // POSIZIONE DI PRELIEVO
  //----------------------------------------------------------------
  if(M.EditMag.NumCestoni>0){
     pos_trovata=99;
     if( M.EditMag.Fifo==false ){
        for(i=1; i<=MAXTERRAMAG; i++){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(M.EditMag.Pos[i].Presenza[j]==false) continue;
              pos_trovata=j;
              break;
           }
           if(pos_trovata!=99) break;
        }
     }
     else{
        for(i=MAXTERRAMAG; i>0; i--){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(M.EditMag.Pos[i].Presenza[j]==false) continue;
              pos_trovata=j;
              break;
           }
           if(pos_trovata!=99) break;
        }
     }
     if(pos_trovata!=99 && M.EditMag.Pos[i].Presenza[pos_trovata]==true){
        //------------------------------------------
        // Setto il tipo di magazzino in base
        // a l'unico box depositato
        //------------------------------------------
        M.EditMag.TipoBox=M.EditMag.Pos[i].Box[pos_trovata].TipoBox;
        if(M.EditMag.Pos[i].Box[pos_trovata].StatoLav==ST_BOX_VUOTI) M.EditMag.Stato=MG_ST_VUOTI;
        if(M.EditMag.Pos[i].Box[pos_trovata].StatoLav!=ST_BOX_VUOTI) M.EditMag.Stato=MG_ST_PIENI;
        //----------------------------------------------------------------
        // Se il magazzino è in assegnazione automatica, è importante
        // che l'ultima paletta in deposito abbia il codice assegnato
        //----------------------------------------------------------------
        if(M.EditMag.Assegnazione==ASS_DINAMICA && M.EditMag.Tipo!=MG_ESCLUSO && M.EditMag.Pos[i].Box[pos_trovata].Codice==0){
           sprintf(buffer, "Please put a correct code assignment (pos %d, level %d)\n...or the store can not be used!", i, pos_trovata);
           Application->MessageBox(buffer, MESS[99], MB_OK | MB_ICONINFORMATION);
           M.EditMag.Tipo=MG_ESCLUSO;
        }
     }
  }

  //----------------------------------------------------------------
  // Controllo che il Tipo di magazzino ammetta i box in esso
  // inseriti
  //----------------------------------------------------------------
  if(M.EditMag.Stato!=MG_ST_FREE && M.EditMag.Tipo!=MG_ESCLUSO){
     if((M.EditMag.Stato==MG_ST_VUOTI && M.EditMag.Tipo!=MG_VUOTI ) || (M.EditMag.Stato!=MG_ST_VUOTI && M.EditMag.Tipo==MG_VUOTI)){
        strcpy(buffer, MESS[324]);
        Application->MessageBox(buffer, MESS[99], MB_OK | MB_ICONINFORMATION);
        return;
     }
  }
  //----------------------------------------------------------------
  // GESTIONE MAGAZZINI ACCOPPIATI
  // Estendo alcuni controlli anche al magazzino accoppiato
  //----------------------------------------------------------------
  if(NumMagAccoppiato>0 && NumMagAccoppiato<=TOTMAG){
     if(M.m[NumMagAccoppiato]->Stato!=MG_ST_FREE && M.m[NumMagAccoppiato]->Tipo!=MG_ESCLUSO){
        if((M.m[NumMagAccoppiato]->Stato==MG_ST_VUOTI && M.m[NumMagAccoppiato]->Tipo!=MG_VUOTI ) || (M.m[NumMagAccoppiato]->Stato!=MG_ST_VUOTI && M.m[NumMagAccoppiato]->Tipo==MG_VUOTI)){
           strcpy(buffer, MESS[324]);
           Application->MessageBox(buffer, MESS[99], MB_OK | MB_ICONINFORMATION);
           return;
        }
     }
  }
  //----------------------------------------------------------------
  // GESTIONE DINAMICA POSIZIONI IN MAGAZZINO
  //
  // !!! Attenzione !!!
  // fino a questo controllo il valore di "Npt" non è attendibile
  //----------------------------------------------------------------
  if(GestioneDinamicaMagazzini==1){
     //---------------------------------------------------------
     // Recupero il lato box su cui eseguire il calcolo
     //---------------------------------------------------------
     lato_box = M.TipiBox[M.EditMag.TipoBox].Lung;
     //---------------------------------------------
     // Calcolo postazioni a terra
     //
     // Arrotondo per eccesso quando i decimali
     // superano 0.6 pallet
     //---------------------------------------------
     //M.EditMag.Npt = (short int)(M.EditMag.Profondita/(lato_box+DIST_CEST_IN_MAG))+1;
     a = (double)(M.EditMag.Profondita)/(double)(lato_box+M.TipiBox[M.EditMag.TipoBox].dist_box_in_magaz);
     b = (double)(a) - (int)(a);
     if(b>M.TipiBox[M.EditMag.TipoBox].eccesso_count_pos_mag) a = (a - b) + 1;      // tolgo i decimali al valore calcolato e aggingo "1".
     M.EditMag.Npt = (short int)(a)+1;     // calcolo effettivo palette a terra
  }

  //----------------------------------------------------------------
  // MAGAZZINI MULTI NODO
  // Controllo validità nodo inserito
  //----------------------------------------------------------------
/*
  if(MagazziniMultiNodo==true){
     for(i=1; i<=M.EditMag.Npt; i++){
        //------------------------------------------
        // Inserimento automatico nodi posizione
        //------------------------------------------
        if(M.EditMag.Pos[i].NodoIng==0 && M.EditMag.PuntoIng>0){
           M.EditMag.Pos[i].NodoIng=(short int)(M.EditMag.PuntoIng+(i-1));
        }
        if(M.EditMag.Pos[i].NodoOut==0 && M.EditMag.PuntoOut>0){
           M.EditMag.Pos[i].NodoOut=(short int)(M.EditMag.PuntoOut+(i-1));
        }
        continue;
     }
     //------------------------------------------
     // IMPORTANTE: azzeramento nodo solo se
     // magazzino a capacità "0" per evitare
     // di inserire tutte le volte il nodo se
     // delle posizioni sono disabilitate
     // temporaneamente
     //------------------------------------------
     if(M.EditMag.Npt==0){
        for(i=1; i<=MAXTERRAMAG; i++){
           M.EditMag.Pos[i].NodoIng=0;
           M.EditMag.Pos[i].NodoOut=0;
        }
     }
  }
*/
  //----------------------------------------------------------------
  // GESTIONE COMPLETO
  // Set del flag di completo con tutte le posizioni impegnate
  //
  // NOTA: solo nei magazzini contenenti i vuoti azzero
  //       automaticamente il flag di completo se non ho
  //       riempito tutte le posizioni a terra
  //----------------------------------------------------------------
  // Massima sovrapposizione impostata a editor
  //----------------------------------------------------------------
  AbilitaPianoMag = M.EditMag.Sovrapposizione;
  //---------------------------------------------------------------
  // Se Nr. pallet presenti maggiore della capacità setta completo
  //---------------------------------------------------------------
  if(M.EditMag.Npt!=0 && M.EditMag.NumCestoni>=(M.EditMag.Npt*(M.EditMag.Sovrapposizione+1))){
     M.EditMag.Completo=true;
  }
  else{
     //-----------------------------------------------------------
     // Se occupata ultima pos. utile di deposito setta completo
     //-----------------------------------------------------------
     if(M.EditMag.Pos[1].Presenza[AbilitaPianoMag]==true){
        M.EditMag.Completo=true;
     }
     else{
        if(M.EditMag.Stato==MG_ST_VUOTI) M.EditMag.Completo=false;
        if(M.EditMag.NumCestoni==0     ) M.EditMag.Completo=false;
     }
  }

  //----------------------------------------------------------------
  // PALLET DA SPOSTARE
  // se si trovano dentro a magazzini LIFO devono essere
  // marcati completi per sospendere il riempimento con
  // altri pallet
  //----------------------------------------------------------------
  almeno_un_pallet_da_spostare=false;
  for(i=MAXTERRAMAG; i>0; i--){
     for(j=MAXPIANIMAG-1; j>=0; j--){
        if(M.EditMag.Pos[i].Presenza[j]==false                 ) continue;
        if(M.EditMag.Pos[i].Box[j].GruppoDest==NN              ) continue;
        if(M.EditMag.Pos[i].Box[j].GruppoDest==M.EditMag.Gruppo) continue;
        almeno_un_pallet_da_spostare=true;
        break;
     }
     if(almeno_un_pallet_da_spostare) break;
  }
  if(almeno_un_pallet_da_spostare==true){
     //-------------------------------------------------
     // MAGAZZINI TEMPORANEI
     // Non possono avere pallet da spostare
     //-------------------------------------------------
     if(M.EditMag.Tipo==MG_TEMPORANEO){
        for(i=MAXTERRAMAG; i>0; i--){
           for(j=MAXPIANIMAG-1; j>=0; j--){
              if(M.EditMag.Pos[i].Presenza[j]==false                 ) continue;
              if(M.EditMag.Pos[i].Box[j].GruppoDest==NN              ) continue;
              if(M.EditMag.Pos[i].Box[j].GruppoDest==M.EditMag.Gruppo) continue;
              M.EditMag.Pos[i].Box[j].GruppoDest=M.EditMag.Gruppo;
           }
        }
     }
     //-------------------------------------------------
     // MAGAZZINI STANDARD
     // Con pallet da spostare vengono marcati completi
     // per evitare che continuino ad essere riempiti
     //-------------------------------------------------
     else M.EditMag.Completo==true;
  }

  //----------------------------------------------------------------
  // Data/Ora primo inserimento (SOLO PER I PIENI)
  //----------------------------------------------------------------
  if(M.EditMag.Stato==MG_ST_FREE || M.EditMag.Stato==MG_ST_VUOTI) M.EditMag.Time=0;     // Azzeramento data/ora primo inserimento
  else{
     if(M.EditMag.Time<=946684800) M.EditMag.Time=time(NULL);
  }


  //--------------------------------------------------------------------
  // SICUREZZA
  // Azzero i dati di tutte le postazioni oltre alla capacità magazzino
  //--------------------------------------------------------------------
  for(i=M.EditMag.Npt+1; i<=MAXTERRAMAG; i++){
     for(j=0; j<MAXPIANIMAG; j++){
        M.EditMag.Pos[i].Presenza[j] = false;
        M.EditMag.Pos[i].QuotaH[j]   = 0;
        M.EditMag.Pos[i].QuotaP[j]   = 0;
        memset(&M.EditMag.Pos[i].Box[j], 0, sizeof(BOX));
     }
  }

  //----------------------------------------------------------------
  // Forzature AUTOMATICHE legate ai singoli impianti
  //----------------------------------------------------------------
  VerificaInputMAG_IMPIANTI(&M.EditMag);

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
  memcpy( M.m[M.EditMag.Num], &M.EditMag, sizeof( MAG ) );
  M.rec_attuale   = M.EditMag.Num;
  IndiceMagazzino = M.EditMag.Num;
  NumRec = M.num_record_mag();
  err = M.save_record_mag( M.rec_attuale, buffer);
  if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //----------------------------------------------------------------
  // MAGAZZINI ACCOPPIATI
  //----------------------------------------------------------------
  if(NumMagAccoppiato>0 && NumMagAccoppiato<=TOTMAG){
     err = M.save_record_mag( NumMagAccoppiato, buffer);
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
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
        err = B.StructToUdcTmp(CMD_UDC_REFRESH, 0, 0, M.EditMag.Num, 0, UDC_ALL_STZ, UDC_ALL_POS, UDC_ALL_LEV, N_FORCA_AB, buffer);
        if(GestioneUdc==2 && err!=0) err=0;
        if(err) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
     // -- RESET LOCAZIONI POSIZIONI VUOTE
     if(!err){
        err = B.StructToUdcTmp(CMD_UDC_RST_LOC, 0, 0, M.EditMag.Num, 0, UDC_ALL_STZ, UDC_ALL_POS, UDC_ALL_LEV, N_FORCA_AB, buffer);
        if(GestioneUdc==2 && err!=0) err=0;
        if(err) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
  }
  //----------------------------------------------------------------
  // Traccia su LOG OPERATORE
  //----------------------------------------------------------------
  sprintf(buffer, "(EDITMAG) - (Mag:%d) Salva editor", IndiceMagazzino);
  FileLogOperatore(buffer);
  //----------------------------------------------------------------
  //  Trasmetti comando al SERVER
  //----------------------------------------------------------------
  if( IdPC>1 ){
     err = RemSincronizzaTimeFile( M.mag_name_sorg, M.mag_name_dest, buffer );
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     sprintf( NomeFile, "Mag%03d", M.rec_attuale );
     err = RemCreaFileComando ( NomeFile, (BYTE *) M.m[ M.rec_attuale ], sizeof( MAG ), buffer );
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     //----------------------------------------------------------------
     // MAGAZZINI ACCOPPIATI
     //----------------------------------------------------------------
     if(NumMagAccoppiato>0 && NumMagAccoppiato<=TOTMAG){
        sprintf( NomeFile, "Mag%03d", NumMagAccoppiato );
        err = RemCreaFileComando ( NomeFile, (BYTE *) M.m[ NumMagAccoppiato ], sizeof( MAG ), buffer );
        if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
  }
  //----------------------------------------------------------------
  // Refresh della Form principale
  //----------------------------------------------------------------
  DatiBoxModificati=0;
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Inser()
// ---------------------------
// Funzione che abilita disabilita l'inserimento di un record sul file
//
void __fastcall TFormMagaz::Inser()
{
  char buffer[151]="";

  if(LivelloPassword<2) return;
  Inserimento = !Inserimento;
  if(Inserimento == true ){ setmem( &M.EditMag, sizeof(MAG), 0); M.EditMag.Tipo=NN;}
  if(Inserimento == false){ memcpy( &M.EditMag, M.m[M.rec_attuale], sizeof( MAG ));}
  //-----------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------
  sprintf(buffer, "(EDITMAG) - (Mag:%d) Inserimento Editor Magazzino", IndiceMagazzino);
  FileLogOperatore(buffer);
  // posizionamento automatico sulla prima posizione
  IndicePosizione=1;
  //-----------------------------
  // Refresh della form
  //-----------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Cancel()
// ---------------------------
// Funzione che cancella un record MACCHINE
//
void __fastcall TFormMagaz::Cancel()
{
  char buffer[100] ;
  char NomeFile[100];
  int i, err =0;
  int result, n_rec;
  struct MAG mg;

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

  setmem (&M.EditMag, sizeof(MAG), 0);                  // azzera la struttura
  //-----------------------------------------
  //  Salva record sul file
  //-----------------------------------------
  memcpy( M.m[M.rec_attuale], &M.EditMag, sizeof( MAG ));
  err = M.save_record_mag( M.rec_attuale, buffer);
  if( err ) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //-----------------------------------------
  //  Trasmetti comando al SERVER
  //-----------------------------------------
  if( IdPC>1 ){
      err = RemSincronizzaTimeFile( M.mag_name_sorg, M.mag_name_dest, buffer );
      if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
      sprintf( NomeFile, "Mag%03d", M.rec_attuale );
      err = RemCreaFileComando ( NomeFile, (BYTE *) M.m[ M.rec_attuale ], sizeof( MAG ), buffer );
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
  for( n_rec=M.rec_attuale+1; n_rec<=MAXMAG; n_rec++){
     if( M.m[ n_rec ]->Num >0 ) break;
  }

  //--------------------------------
  //  Se non trovato
  //  cerca il precedente record
  //--------------------------------
  if( n_rec>MAXMAG ){
     for( n_rec=M.rec_attuale-1; n_rec>0; n_rec--){
        if( M.m[ n_rec ]->Num >0 ) break;
     }
  }
  //-----------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------
  sprintf(buffer, "(EDITMAG) - (Mag:%d) Cancellazione Editor Magazziono", IndiceMagazzino);
  FileLogOperatore(buffer);
  //------------------------------------
  //  Recupera dati nella struttura ch
  //------------------------------------
  memset(&mg, 0, sizeof(MAG));
  err = M.cerca_record_mag(&mg, n_rec );
  M.rec_attuale   = (short int) n_rec;
  IndiceMagazzino = (short int) n_rec;
  memcpy( &M.EditMag, &mg, sizeof( MAG ) );
  NumRec = M.num_record_mag();
  //-------------------------------------
  // GESTIONE MAGAZZINI ACCOPPIATI
  //-------------------------------------
  NumMagAccoppiato=0;
  for(i=0;;i++){
     if(AccoppiaMagaz[i][0]==0 && AccoppiaMagaz[i][1]==0) break;
     if(AccoppiaMagaz[i][0]!=M.EditMag.Num && AccoppiaMagaz[i][1]!=M.EditMag.Num) continue;
     if(AccoppiaMagaz[i][0]==M.EditMag.Num){NumMagAccoppiato=AccoppiaMagaz[i][1]; break;}
     if(AccoppiaMagaz[i][1]==M.EditMag.Num){NumMagAccoppiato=AccoppiaMagaz[i][0]; break;}
  }
  //-------------------------------------
  // Refresh della finestra
  //-------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    GetCaselle()
// ---------------------------
// Funzione che legge dalle caselle di dialogo (TEdit) di WINDOWS
// e scarica su le variabili stringa di MACCHINE
//
void __fastcall TFormMagaz::GetCaselle()
{
  char ora[11]="";
  char data[11]="";

  //------------------------------------------------
  // Dati fissi NON MODIFICABILI
  //------------------------------------------------
  M.EditMag.Num                = (short int)(atoi(EdNum->Text.c_str()));
  M.EditMag.Npt                = (short int)(atoi(EdNpt->Text.c_str()));
  M.EditMag.PuntoIng           = (short int)(atoi(EdNodIng->Text.c_str()));
  M.EditMag.PuntoOut           = (short int)(atoi(EdNodOut->Text.c_str()));
  M.EditMag.NumCestoni         = (short int)(atoi(EdNCest->Text.c_str()));
  M.EditMag.Sovrapposizione    = (short int)(atoi(EdSovrap->Text.c_str()));

  strcpy(data, EdData->Text.c_str());
  strcpy(ora,  EdOra->Text.c_str());
  timechar_to_timeint(&M.EditMag.Time, &data[0], &ora[0]);
  //------------------------------------------------
  // Abilitazioni/Disabilitazioni
  //------------------------------------------------
  M.EditMag.Fifo         = CBFifo->Checked;
  M.EditMag.Completo     = CBCompleto->Checked;
  M.EditMag.PerEmergenza = RGCarWash->ItemIndex;

  //------------------------------------------------
  // GESTIONE ROTAZIONE PALLET
  //------------------------------------------------
  if(RBRotTutti->Checked==true     ) M.EditMag.PerPalRuotati=0;
  if(RBSoloRuotati->Checked==true  ) M.EditMag.PerPalRuotati=1;
  if(RBNoRuotati->Checked==true    ) M.EditMag.PerPalRuotati=2;

  //------------------------------------------------
  // Tipo magazzino e Stato
  //------------------------------------------------
  M.EditMag.Tipo          = (BYTE) GBTipo->ItemIndex;
  M.EditMag.Gruppo        = (BYTE)(CBGruppo->ItemIndex);
  M.EditMag.Stato         = GBStato->ItemIndex;
  M.EditMag.TipoBox       = CBTipoBox->ItemIndex;

  //------------------------------------------------
  // Configurazione deposito
  //------------------------------------------------
  M.EditMag.Profondita    = (short int)(atoi(EdProfond->Text.c_str()));
  M.EditMag.BaiaRialzata  = (short int)(atoi(EdBaAlta->Text.c_str()));
  M.EditMag.OffSetDaNodo  = (short int)(atoi(EdOffNod->Text.c_str()));
  M.EditMag.Sollevamento  = (short int)(atoi(EdSoll->Text.c_str()));

  //------------------------------------------------
  // Gestione assegnazione
  //------------------------------------------------
  M.EditMag.Assegnazione = (short int)(CBAsseg->ItemIndex);

  //------------------------------------------------
  // Dati posizioni
  //------------------------------------------------
  // nodo
  M.EditMag.Pos[IndicePosizione].NodoIng  = (short int)(atoi(EdNodP1->Text.c_str()));
  M.EditMag.Pos[IndicePosizione].NodoOut  = (short int)(atoi(EdNodP2->Text.c_str()));
  //Piani Massimi per Nodo Magazzino
  M.EditMag.Pos[IndicePosizione].MaxHPos  = (short int)(atoi(edMaxPianiP->Text.c_str()));
  // piano terra
  M.EditMag.Pos[IndicePosizione].Presenza[0] = CBPres0->Checked;

  // piano primo
  M.EditMag.Pos[IndicePosizione].Presenza[1] = CBPres1->Checked;

  // piano 2
  M.EditMag.Pos[IndicePosizione].Presenza[2] = CBPres2->Checked;
}
//---------------------------------------------------------------------------

// ---------------------------
//    SetCaselle()
// ---------------------------
// Funzione che forza i nuovi valori nelle caselle di dialogo (TEdit)
// di WINDOWS recuperandoli dalle variabili stringa delle MACCHINE
//
void __fastcall TFormMagaz::SetCaselle()
{
  WORD quota;
  double a, b;
  double rap_imp;
  char ora[11]="";
  char data[11]="";
  char buffer[101]="";
  char all_mess[201]="";
  short int lato_box = LUNG_CEST;
  TColor ColC, ColS, ColNS;


  //------------------------------------------------
  // Memorizzo il numero della locazione aperta
  //------------------------------------------------
  EditorMagazzinoAttivo = IndiceMagazzino;
  //------------------------------------------------
  // Numero locazione nel Caption della form
  //------------------------------------------------
  sprintf(buffer, MESS[299], M.EditMag.Num);
  FormMagaz->Caption = buffer;
  //------------------------------------------------
  // Dati fissi NON MODIFICABILI
  //------------------------------------------------
  sprintf(buffer, "%03d", M.EditMag.Num);
  EdNum->Text = buffer;
  sprintf(buffer, "%04d", M.EditMag.PuntoIng);
  EdNodIng->Text = buffer;
  sprintf(buffer, "%04d", M.EditMag.PuntoOut);
  EdNodOut->Text = buffer;
  sprintf(buffer, "%02d", M.EditMag.Npt);
  EdNpt->Text = buffer;
  sprintf(buffer, "%02d", M.EditMag.NumCestoni);
  EdNCest->Text = buffer;
  sprintf(buffer, "%d",   M.EditMag.Sovrapposizione);
  EdSovrap->Text = buffer;
  timeint_to_timechar(&M.EditMag.Time, &data[0], &ora[0]);
  EdData->Text  = data;
  EdOra->Text   = ora;

  //--------------------------------------------------
  // GESTIONE MAGAZZINI TEMPORANEI
  // Se esisteno magazzini temporanei con pallet
  // che devono ancora toranre al magazzino di
  // origine non è ammesso modificare ASSEGNAZIONE
  //--------------------------------------------------
  if(NumMagTemporaneo==0){
     GBAssegnaz->Visible=true;
     TxtPalRetrival->Caption = "";
  }
  else{
     GBAssegnaz->Visible=false;
     sprintf(all_mess, "\n\nPALLET RETRIVAL FROM STORE %d\n...not possible to change assignament\nduring this operation!", NumMagTemporaneo);
     TxtPalRetrival->Caption = all_mess;
  }
  //--------------------------------------------------
  // Disattivazione delle stazioni non esistenti
  //--------------------------------------------------
  TabPos1->TabVisible=false;
  TabPos2->TabVisible=false;
  TabPos3->TabVisible=false;
  TabPos4->TabVisible=false;
  TabPos5->TabVisible=false;
  TabPos6->TabVisible=false;
  TabPos7->TabVisible=false;
  TabPos8->TabVisible=false;
  TabPos9->TabVisible=false;
  TabPos10->TabVisible=false;
  TabPos11->TabVisible=false;
  TabPos12->TabVisible=false;
  TabPos13->TabVisible=false;
  TabPos14->TabVisible=false;
  TabPos15->TabVisible=false;
  TabPos16->TabVisible=false;
  TabPos17->TabVisible=false;
  TabPos18->TabVisible=false;
  TabPos19->TabVisible=false;
  TabPos20->TabVisible=false;
  TabPos21->TabVisible=false;
  TabPos22->TabVisible=false;
  TabPos23->TabVisible=false;
  TabPos24->TabVisible=false;
  TabPos25->TabVisible=false;
  TabPos26->TabVisible=false;
  TabPos27->TabVisible=false;
  TabPos28->TabVisible=false;
  TabPos29->TabVisible=false;
  TabPos30->TabVisible=false;
  TabPos31->TabVisible=false;
  TabPos32->TabVisible=false;
  TabPos33->TabVisible=false;
  TabPos34->TabVisible=false;
  TabPos35->TabVisible=false;
  TabPos36->TabVisible=false;
  TabPos37->TabVisible=false;
  TabPos38->TabVisible=false;
  TabPos39->TabVisible=false;
  TabPos40->TabVisible=false;
  switch(M.EditMag.Npt){
     case  1: TabPos1->TabVisible=true;
              break;
     case  2: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              break;
     case  3: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              break;
     case  4: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              break;
     case  5: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              break;
     case  6: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              break;
     case  7: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              break;
     case  8: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              break;
     case  9: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              break;
     case 10: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
     case 11: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              break;
     case 12: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              break;
     case 13: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              break;
     case 14: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              break;
     case 15: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              break;
     case 16: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              break;
     case 17: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              break;
     case 18: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              break;
     case 19: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              break;
     case 20: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              break;
     case 21: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
               break;
     case 22: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              break;
     case 23: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              break;
     case 24: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              break;
     case 25: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              break;
     case 26: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              break;
     case 27: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              TabPos27->TabVisible=true;
              break;
     case 28: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              TabPos27->TabVisible=true;
              TabPos28->TabVisible=true;
              break;
     case 29: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              TabPos27->TabVisible=true;
              TabPos28->TabVisible=true;
              TabPos29->TabVisible=true;
              break;
     case 30: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              TabPos27->TabVisible=true;
              TabPos28->TabVisible=true;
              TabPos29->TabVisible=true;
              TabPos30->TabVisible=true;
              break;
     case 31: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              TabPos27->TabVisible=true;
              TabPos28->TabVisible=true;
              TabPos29->TabVisible=true;
              TabPos30->TabVisible=true;
              TabPos31->TabVisible=true;
              break;
     case 32: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              TabPos27->TabVisible=true;
              TabPos28->TabVisible=true;
              TabPos29->TabVisible=true;
              TabPos30->TabVisible=true;
              TabPos31->TabVisible=true;
              TabPos32->TabVisible=true;
              break;
     case 33: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              TabPos27->TabVisible=true;
              TabPos28->TabVisible=true;
              TabPos29->TabVisible=true;
              TabPos30->TabVisible=true;
              TabPos31->TabVisible=true;
              TabPos32->TabVisible=true;
              TabPos33->TabVisible=true;
              break;
     case 34: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              TabPos27->TabVisible=true;
              TabPos28->TabVisible=true;
              TabPos29->TabVisible=true;
              TabPos30->TabVisible=true;
              TabPos31->TabVisible=true;
              TabPos32->TabVisible=true;
              TabPos33->TabVisible=true;
              TabPos34->TabVisible=true;
              break;
     case 35: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              TabPos27->TabVisible=true;
              TabPos28->TabVisible=true;
              TabPos29->TabVisible=true;
              TabPos30->TabVisible=true;
              TabPos31->TabVisible=true;
              TabPos32->TabVisible=true;
              TabPos33->TabVisible=true;
              TabPos34->TabVisible=true;
              TabPos35->TabVisible=true;
              break;
     case 36: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              TabPos27->TabVisible=true;
              TabPos28->TabVisible=true;
              TabPos29->TabVisible=true;
              TabPos30->TabVisible=true;
              TabPos31->TabVisible=true;
              TabPos32->TabVisible=true;
              TabPos33->TabVisible=true;
              TabPos34->TabVisible=true;
              TabPos35->TabVisible=true;
              TabPos36->TabVisible=true;
              break;
     case 37: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              TabPos27->TabVisible=true;
              TabPos28->TabVisible=true;
              TabPos29->TabVisible=true;
              TabPos30->TabVisible=true;
              TabPos31->TabVisible=true;
              TabPos32->TabVisible=true;
              TabPos33->TabVisible=true;
              TabPos34->TabVisible=true;
              TabPos35->TabVisible=true;
              TabPos36->TabVisible=true;
              TabPos37->TabVisible=true;
              break;
     case 38: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              TabPos27->TabVisible=true;
              TabPos28->TabVisible=true;
              TabPos29->TabVisible=true;
              TabPos30->TabVisible=true;
              TabPos31->TabVisible=true;
              TabPos32->TabVisible=true;
              TabPos33->TabVisible=true;
              TabPos34->TabVisible=true;
              TabPos35->TabVisible=true;
              TabPos36->TabVisible=true;
              TabPos37->TabVisible=true;
              TabPos38->TabVisible=true;
              break;
     case 39: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              TabPos27->TabVisible=true;
              TabPos28->TabVisible=true;
              TabPos29->TabVisible=true;
              TabPos30->TabVisible=true;
              TabPos31->TabVisible=true;
              TabPos32->TabVisible=true;
              TabPos33->TabVisible=true;
              TabPos34->TabVisible=true;
              TabPos35->TabVisible=true;
              TabPos36->TabVisible=true;
              TabPos37->TabVisible=true;
              TabPos38->TabVisible=true;
              TabPos39->TabVisible=true;
              break;
     case 40: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              TabPos27->TabVisible=true;
              TabPos28->TabVisible=true;
              TabPos29->TabVisible=true;
              TabPos30->TabVisible=true;
              TabPos31->TabVisible=true;
              TabPos32->TabVisible=true;
              TabPos33->TabVisible=true;
              TabPos34->TabVisible=true;
              TabPos35->TabVisible=true;
              TabPos36->TabVisible=true;
              TabPos37->TabVisible=true;
              TabPos38->TabVisible=true;
              TabPos39->TabVisible=true;
              TabPos40->TabVisible=true;
              break;
     default: TabPos1->TabVisible=true;
              TabPos2->TabVisible=true;
              TabPos3->TabVisible=true;
              TabPos4->TabVisible=true;
              TabPos5->TabVisible=true;
              TabPos6->TabVisible=true;
              TabPos7->TabVisible=true;
              TabPos8->TabVisible=true;
              TabPos9->TabVisible=true;
              TabPos10->TabVisible=true;
              TabPos11->TabVisible=true;
              TabPos12->TabVisible=true;
              TabPos13->TabVisible=true;
              TabPos14->TabVisible=true;
              TabPos15->TabVisible=true;
              TabPos16->TabVisible=true;
              TabPos17->TabVisible=true;
              TabPos18->TabVisible=true;
              TabPos19->TabVisible=true;
              TabPos20->TabVisible=true;
              TabPos21->TabVisible=true;
              TabPos22->TabVisible=true;
              TabPos23->TabVisible=true;
              TabPos24->TabVisible=true;
              TabPos25->TabVisible=true;
              TabPos26->TabVisible=true;
              TabPos27->TabVisible=true;
              TabPos28->TabVisible=true;
              TabPos29->TabVisible=true;
              TabPos30->TabVisible=true;
              TabPos31->TabVisible=true;
              TabPos32->TabVisible=true;
              TabPos33->TabVisible=true;
              TabPos34->TabVisible=true;
              TabPos35->TabVisible=true;
              TabPos36->TabVisible=true;
              TabPos37->TabVisible=true;
              TabPos38->TabVisible=true;
              TabPos39->TabVisible=true;
              TabPos40->TabVisible=true;
              break;
  }
  //--------------------------------------------------
  // Visualizzazione dei dati stazione
  //--------------------------------------------------
  switch(IndicePosizione){
     case  1: PageControlMag->ActivePage=TabPos1;  break;
     case  2: PageControlMag->ActivePage=TabPos2;  break;
     case  3: PageControlMag->ActivePage=TabPos3;  break;
     case  4: PageControlMag->ActivePage=TabPos4;  break;
     case  5: PageControlMag->ActivePage=TabPos5;  break;
     case  6: PageControlMag->ActivePage=TabPos6;  break;
     case  7: PageControlMag->ActivePage=TabPos7;  break;
     case  8: PageControlMag->ActivePage=TabPos8;  break;
     case  9: PageControlMag->ActivePage=TabPos9;  break;
     case 10: PageControlMag->ActivePage=TabPos10; break;
     case 11: PageControlMag->ActivePage=TabPos11; break;
     case 12: PageControlMag->ActivePage=TabPos12; break;
     case 13: PageControlMag->ActivePage=TabPos13; break;
     case 14: PageControlMag->ActivePage=TabPos14; break;
     case 15: PageControlMag->ActivePage=TabPos15; break;
     case 16: PageControlMag->ActivePage=TabPos16; break;
     case 17: PageControlMag->ActivePage=TabPos17; break;
     case 18: PageControlMag->ActivePage=TabPos18; break;
     case 19: PageControlMag->ActivePage=TabPos19; break;
     case 20: PageControlMag->ActivePage=TabPos20; break;
     case 21: PageControlMag->ActivePage=TabPos21; break;
     case 22: PageControlMag->ActivePage=TabPos22; break;
     case 23: PageControlMag->ActivePage=TabPos23; break;
     case 24: PageControlMag->ActivePage=TabPos24; break;
     case 25: PageControlMag->ActivePage=TabPos25; break;
     case 26: PageControlMag->ActivePage=TabPos26; break;
     case 27: PageControlMag->ActivePage=TabPos27; break;
     case 28: PageControlMag->ActivePage=TabPos28; break;
     case 29: PageControlMag->ActivePage=TabPos29; break;
     case 30: PageControlMag->ActivePage=TabPos30; break;
     case 31: PageControlMag->ActivePage=TabPos31; break;
     case 32: PageControlMag->ActivePage=TabPos32; break;
     case 33: PageControlMag->ActivePage=TabPos33; break;
     case 34: PageControlMag->ActivePage=TabPos34; break;
     case 35: PageControlMag->ActivePage=TabPos35; break;
     case 36: PageControlMag->ActivePage=TabPos36; break;
     case 37: PageControlMag->ActivePage=TabPos37; break;
     case 38: PageControlMag->ActivePage=TabPos38; break;
     case 39: PageControlMag->ActivePage=TabPos39; break;
     case 40: PageControlMag->ActivePage=TabPos40; break;
     default: PageControlMag->ActivePage=TabPos1;  break;
  }

  //------------------------------------------------
  // Disattivazione presenza i posizione alta
  // per magazzini non abilitati alla sovrapp.
  //------------------------------------------------
  if(M.EditMag.Sovrapposizione>0){
     GBPiano1->Visible=true;
     CBPres1->Visible=true;
     //££££££££££££££££££££££££££££££££££££££££££££££
     //----------------------------------------
     // eventuale 2° piano
     //----------------------------------------
     if(M.EditMag.Sovrapposizione>1){
        GBPiano2->Visible=true;
        CBPres2->Visible=true;
     }
     else{
        GBPiano2->Visible=false;
        CBPres2->Visible=false;
     }
     //££££££££££££££££££££££££££££££££££££££££££££££
  }
  else{
     GBPiano1->Visible=false;
     CBPres1->Visible=false;
     GBPiano2->Visible=false;
     CBPres2->Visible=false;
  }
  pnlPres2->Visible=CBPres2->Visible;
  pnlPres1->Visible=CBPres1->Visible;
  pnlPres0->Visible=CBPres0->Visible;
  //------------------------------------------------
  // Configurazione deposito
  //------------------------------------------------
  sprintf(buffer, "%04d", M.EditMag.Sollevamento);
  EdSoll->Text = buffer;
  sprintf(buffer, "%04d", M.EditMag.BaiaRialzata);
  EdBaAlta->Text = buffer;
  sprintf(buffer, "%04d", M.EditMag.Profondita);
  EdProfond->Text = buffer;
  sprintf(buffer, "%02d", M.EditMag.OffSetDaNodo);
  EdOffNod->Text = buffer;
  //------------------------------------------------
  // Tipo magazzino e Stato
  //------------------------------------------------
  GBTipo->ItemIndex   = M.EditMag.Tipo;
  CBGruppo->ItemIndex = M.EditMag.Gruppo;
  GBStato->ItemIndex  = M.EditMag.Stato;
  CBTipoBox->ItemIndex= M.EditMag.TipoBox;
  //------------------------------------------------
  // Gestione assegnazione
  //------------------------------------------------
  CBAsseg->ItemIndex  = M.EditMag.Assegnazione;
  //------------------------------------------------
  // ABBINA_CODICE_A_STATO_BOX
  // Assegnazione colori al Combobox di selezione
  //------------------------------------------------
  switch(M.EditMag.Assegnazione){
     case  ASS_DINAMICA: CBAsseg->Color = clCream;                     break;
     case  PRES_LAV1:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT1);  break;
     case  PRES_LAV2:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT2);  break;
     case  PRES_LAV3:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT3);  break;
     case  PRES_LAV4:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT4);  break;
     case  PRES_LAV5:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT5);  break;
     case  PRES_LAV6:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT6);  break;
     case  PRES_LAV7:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT7);  break;
     case  PRES_LAV8:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT8);  break;
     case  PRES_VUOTO:   CBAsseg->Color = (TColor)(COLOR_PRES_VUOTI);  break;
            default:     CBAsseg->Color = clCream;                     break;
  }

  //---------------------------------------------------------
  // STATO: "Per EMERGENZA" o "Per ROTAZIONE"
  // modificabili solo a magazzino vuoto
  //---------------------------------------------------------
  if(M.EditMag.NumCestoni!=0 || (NumMagAccoppiato!=0 && M.m[NumMagAccoppiato]->NumCestoni!=0)){
     RGCarWash->Enabled=false;
     RBRotTutti->Enabled=false;
     RBSoloRuotati->Enabled=false;
     RBNoRuotati->Enabled=false;
  }
  else{
     RGCarWash->Enabled=true;
     RBRotTutti->Enabled=true;
     RBSoloRuotati->Enabled=true;
     RBNoRuotati->Enabled=true;
  }

  //---------------------------------------------------------
  // Abilitazioni/Disabilitazioni
  //---------------------------------------------------------
  CBFifo->Checked      = M.EditMag.Fifo;
  CBCompleto->Checked  = M.EditMag.Completo;
  RGCarWash->ItemIndex = M.EditMag.PerEmergenza;

  //---------------------------------------------------------
  // GESTIONE ROTAZIONE PALLET
  //---------------------------------------------------------
  if(M.EditMag.PerPalRuotati==0) RBRotTutti->Checked=true;
  if(M.EditMag.PerPalRuotati==1) RBSoloRuotati->Checked=true;
  if(M.EditMag.PerPalRuotati==2) RBNoRuotati->Checked=true;

  //---------------------------------------------------------
  // GESTIONE_DINAMICA_MAG
  // Calcolo posizioni a terra teoriche
  //---------------------------------------------------------
  quota=0;
  //---------------------------------------------------------
  // Prendo il TGV1 come riferimento per il calcolo delle
  // quote reali
  //---------------------------------------------------------
  rap_imp = AGV[1]->mission.RapEncoder;
  if(AGV[1]->mission.GestioneLaser==true) rap_imp=1;
  //---------------------------------------------------------
  // Recupero il lato box su cui eseguire il calcolo
  //---------------------------------------------------------
  lato_box = M.TipiBox[M.EditMag.TipoBox].Lung;
  a = (double)(M.EditMag.OffSetDaNodo/rap_imp)+(double)(M.TipiBox[M.EditMag.TipoBox].offset_tgv_da_nodo_mag/rap_imp)+(double)(((IndicePosizione-1)*(lato_box+M.TipiBox[M.EditMag.TipoBox].dist_box_in_magaz))/rap_imp);
  b = (double)(a) - (int)(a);
  if(b>0.5)  a = (a - b) + 1; // arrotondo all'impulso successivo
  //---------------------------------------------------------
  // Compilo mappa delle posizioni di deposito
  //---------------------------------------------------------
  quota = (WORD)(a);

  //---------------------------------------------------------
  // Dati posizioni
  //---------------------------------------------------------
  // nodo
  sprintf(buffer, "%04d", M.EditMag.Pos[IndicePosizione].NodoIng);
  EdNodP1->Text = buffer;
  sprintf(buffer, "%04d", M.EditMag.Pos[IndicePosizione].NodoOut);
  EdNodP2->Text = buffer;
  // Piani max per posizione
  sprintf(buffer, "%01d", M.EditMag.Pos[IndicePosizione].MaxHPos);
  edMaxPianiP->Text=buffer;
  // piano terra
  CBPres0->Checked  = M.EditMag.Pos[IndicePosizione].Presenza[0];
  GBPiano0->Enabled = CBPres0->Checked;
  BtnBox0->Enabled  = CBPres0->Checked;
  //---------------------------------------------------------
  // Assegnazione del colore alla presenza
  //---------------------------------------------------------
  CBPres0->Color = clBtnFace;
  if(CBPres0->Checked==true){
     switch(M.EditMag.Pos[IndicePosizione].Box[0].StatoLav){
        case ST_BOX_VUOTI:CBPres0->Color=(TColor)(COLOR_PRES_VUOTI); break;
        case ST_BOX_LAV1: CBPres0->Color=(TColor)(COLOR_PRES_STAT1); break;
        case ST_BOX_LAV2: CBPres0->Color=(TColor)(COLOR_PRES_STAT2); break;
        case ST_BOX_LAV3: CBPres0->Color=(TColor)(COLOR_PRES_STAT3); break;
        case ST_BOX_LAV4: CBPres0->Color=(TColor)(COLOR_PRES_STAT4); break;
        case ST_BOX_LAV5: CBPres0->Color=(TColor)(COLOR_PRES_STAT5); break;
        case ST_BOX_LAV6: CBPres0->Color=(TColor)(COLOR_PRES_STAT6); break;
        case ST_BOX_LAV7: CBPres0->Color=(TColor)(COLOR_PRES_STAT7); break;
        case ST_BOX_LAV8: CBPres0->Color=(TColor)(COLOR_PRES_STAT8); break;
                 default: CBPres0->Color=(TColor)(COLOR_PRES_VUOTI); break;
     }
  }
  TxQP0_1->Caption = quota;
  TxQP0_2->Caption = M.EditMag.Pos[IndicePosizione].QuotaP[0];
  //-----------------
  // piano alto
  //-----------------
  CBPres1->Checked  = M.EditMag.Pos[IndicePosizione].Presenza[1];
  GBPiano1->Enabled = CBPres1->Checked;
  BtnBox1->Enabled  = CBPres1->Checked;
  TxQH1->Caption    = M.EditMag.Pos[IndicePosizione].QuotaH[1];
  //------------------------------------------------
  // Assegnazione del colore alla presenza
  //------------------------------------------------
  CBPres1->Color = clBtnFace;
  if(CBPres1->Checked==true){
     switch(M.EditMag.Pos[IndicePosizione].Box[1].StatoLav){
        case ST_BOX_VUOTI:CBPres1->Color=(TColor)(COLOR_PRES_VUOTI); break;
        case ST_BOX_LAV1: CBPres1->Color=(TColor)(COLOR_PRES_STAT1); break;
        case ST_BOX_LAV2: CBPres1->Color=(TColor)(COLOR_PRES_STAT2); break;
        case ST_BOX_LAV3: CBPres1->Color=(TColor)(COLOR_PRES_STAT3); break;
        case ST_BOX_LAV4: CBPres1->Color=(TColor)(COLOR_PRES_STAT4); break;
        case ST_BOX_LAV5: CBPres1->Color=(TColor)(COLOR_PRES_STAT5); break;
        case ST_BOX_LAV6: CBPres1->Color=(TColor)(COLOR_PRES_STAT6); break;
        case ST_BOX_LAV7: CBPres1->Color=(TColor)(COLOR_PRES_STAT7); break;
        case ST_BOX_LAV8: CBPres1->Color=(TColor)(COLOR_PRES_STAT8); break;
                 default: CBPres1->Color=(TColor)(COLOR_PRES_VUOTI); break;
     }
  }
  //-----------------
  // piano 2
  //-----------------
  CBPres2->Checked  = M.EditMag.Pos[IndicePosizione].Presenza[2];
  GBPiano2->Enabled = CBPres2->Checked;
  BtnBox2->Enabled  = CBPres2->Checked;
  TxQH2->Caption    = M.EditMag.Pos[IndicePosizione].QuotaH[2];
  //------------------------------------------------
  // Assegnazione del colore alla presenza
  //------------------------------------------------
  CBPres2->Color = clBtnFace;
  if(CBPres2->Checked==true){
     switch(M.EditMag.Pos[IndicePosizione].Box[2].StatoLav){
        case ST_BOX_VUOTI:CBPres2->Color=(TColor)(COLOR_PRES_VUOTI); break;
        case ST_BOX_LAV1: CBPres2->Color=(TColor)(COLOR_PRES_STAT1); break;
        case ST_BOX_LAV2: CBPres2->Color=(TColor)(COLOR_PRES_STAT2); break;
        case ST_BOX_LAV3: CBPres2->Color=(TColor)(COLOR_PRES_STAT3); break;
        case ST_BOX_LAV4: CBPres2->Color=(TColor)(COLOR_PRES_STAT4); break;
        case ST_BOX_LAV5: CBPres2->Color=(TColor)(COLOR_PRES_STAT5); break;
        case ST_BOX_LAV6: CBPres2->Color=(TColor)(COLOR_PRES_STAT6); break;
        case ST_BOX_LAV7: CBPres2->Color=(TColor)(COLOR_PRES_STAT7); break;
        case ST_BOX_LAV8: CBPres2->Color=(TColor)(COLOR_PRES_STAT8); break;
                 default: CBPres2->Color=(TColor)(COLOR_PRES_VUOTI); break;
     }
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

  sprintf( all_mess, MESS[441], NumRec );
  NRecordPanel->Caption = all_mess;
}
//---------------------------------------------------------------------------
void __fastcall TFormMagaz::BtnBoxClick(TObject *Sender)
{
  int NumBox;
  //------------------------------------------------
  NumBox = ((TBitBtn *)Sender)->Tag;
  if(NumBox>2) return;
  //------------------------------------------------
  // GESTIONE ARCHIVIO BOX
  //------------------------------------------------
  M.EditMag.Pos[IndicePosizione].Box[NumBox].TipoBox = CBTipoBox->ItemIndex;
  if(M.EditMag.Assegnazione!=ASS_DINAMICA && M.EditMag.Pos[IndicePosizione].Box[NumBox].StatoLav==LIBERA){
     switch(M.EditMag.Assegnazione){
        case PRES_LAV1: M.EditMag.Pos[IndicePosizione].Box[NumBox].StatoLav=ST_BOX_LAV1;
                        if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[IndicePosizione].Box[NumBox].Codice=ASS_COD1;
                        break;
        case PRES_LAV2: M.EditMag.Pos[IndicePosizione].Box[NumBox].StatoLav=ST_BOX_LAV1;
                        if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[IndicePosizione].Box[NumBox].Codice=ASS_COD1;
                        break;
        case PRES_LAV3: M.EditMag.Pos[IndicePosizione].Box[NumBox].StatoLav=ST_BOX_LAV1;
                        if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[IndicePosizione].Box[NumBox].Codice=ASS_COD1;
                        break;
        case PRES_LAV4: M.EditMag.Pos[IndicePosizione].Box[NumBox].StatoLav=ST_BOX_LAV1;
                        if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[IndicePosizione].Box[NumBox].Codice=ASS_COD1;
                        break;
        case PRES_LAV5: M.EditMag.Pos[IndicePosizione].Box[NumBox].StatoLav=ST_BOX_LAV1;
                        if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[IndicePosizione].Box[NumBox].Codice=ASS_COD1;
                        break;
        case PRES_LAV6: M.EditMag.Pos[IndicePosizione].Box[NumBox].StatoLav=ST_BOX_LAV1;
                        if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[IndicePosizione].Box[NumBox].Codice=ASS_COD1;
                        break;
        case PRES_LAV7: M.EditMag.Pos[IndicePosizione].Box[NumBox].StatoLav=ST_BOX_LAV1;
                        if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[IndicePosizione].Box[NumBox].Codice=ASS_COD1;
                        break;
        case PRES_LAV8: M.EditMag.Pos[IndicePosizione].Box[NumBox].StatoLav=ST_BOX_LAV1;
                        if(AbbinaCodiceStatoBox==true) M.EditMag.Pos[IndicePosizione].Box[NumBox].Codice=ASS_COD1;
                        break;
        case PRES_VUOTO:M.EditMag.Pos[IndicePosizione].Box[NumBox].StatoLav=ST_BOX_VUOTI;
                        M.EditMag.Pos[IndicePosizione].Box[NumBox].Codice=ASS_VUOTO;
                        break;
              default:  M.EditMag.Pos[IndicePosizione].Box[NumBox].StatoLav=ST_BOX_VUOTI;
                        M.EditMag.Pos[IndicePosizione].Box[NumBox].Codice=ASS_VUOTO;
                        break;
     }
  }

  memcpy(&B.EditBox, &M.EditMag.Pos[IndicePosizione].Box[NumBox], sizeof(BOX));
  FormBox->Mag=M.EditMag.Num;
  FormBox->Pos=IndicePosizione;
  FormBox->Pin=NumBox;
  FormBox->Autoclose=false;

  TimerClose->Enabled=false;
  FormBox->ShowModal();
  TimerClose->Enabled=true;

  if(FormBox->ModalResult==mrOk){
     memcpy(&M.EditMag.Pos[IndicePosizione].Box[NumBox], &B.EditBox, sizeof(BOX));
     CBTipoBox->ItemIndex = M.EditMag.Pos[IndicePosizione].Box[NumBox].TipoBox;
     switch(M.EditMag.Pos[IndicePosizione].Box[NumBox].StatoLav){
        case ST_BOX_VUOTI:CBPres[NumBox]->Color=(TColor)(COLOR_PRES_VUOTI); break;
        case ST_BOX_LAV1: CBPres[NumBox]->Color=(TColor)(COLOR_PRES_STAT1); break;
        case ST_BOX_LAV2: CBPres[NumBox]->Color=(TColor)(COLOR_PRES_STAT2); break;
        case ST_BOX_LAV3: CBPres[NumBox]->Color=(TColor)(COLOR_PRES_STAT3); break;
        case ST_BOX_LAV4: CBPres[NumBox]->Color=(TColor)(COLOR_PRES_STAT4); break;
        case ST_BOX_LAV5: CBPres[NumBox]->Color=(TColor)(COLOR_PRES_STAT5); break;
        case ST_BOX_LAV6: CBPres[NumBox]->Color=(TColor)(COLOR_PRES_STAT6); break;
        case ST_BOX_LAV7: CBPres[NumBox]->Color=(TColor)(COLOR_PRES_STAT7); break;
        case ST_BOX_LAV8: CBPres[NumBox]->Color=(TColor)(COLOR_PRES_STAT8); break;
                 default: CBPres[NumBox]->Color=(TColor)(COLOR_PRES_VUOTI); break;
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMagaz::PageControlMagChange(TObject *Sender)
{
  int  i;
  WORD quota;
  double a, b;
  double rap_imp;
  short int lato_box=LUNG_CEST;
  char buffer[101]="";
  TColor ColC, ColS, ColNS;

  IndicePosizione=1;
  //----------------------------------------
  // Recupero il numero del TGV selezionato
  //----------------------------------------
  if(PageControlMag->ActivePage==TabPos1 ) IndicePosizione =  1;
  if(PageControlMag->ActivePage==TabPos2 ) IndicePosizione =  2;
  if(PageControlMag->ActivePage==TabPos3 ) IndicePosizione =  3;
  if(PageControlMag->ActivePage==TabPos4 ) IndicePosizione =  4;
  if(PageControlMag->ActivePage==TabPos5 ) IndicePosizione =  5;
  if(PageControlMag->ActivePage==TabPos6 ) IndicePosizione =  6;
  if(PageControlMag->ActivePage==TabPos7 ) IndicePosizione =  7;
  if(PageControlMag->ActivePage==TabPos8 ) IndicePosizione =  8;
  if(PageControlMag->ActivePage==TabPos9 ) IndicePosizione =  9;
  if(PageControlMag->ActivePage==TabPos10) IndicePosizione = 10;
  if(PageControlMag->ActivePage==TabPos11) IndicePosizione = 11;
  if(PageControlMag->ActivePage==TabPos12) IndicePosizione = 12;
  if(PageControlMag->ActivePage==TabPos13) IndicePosizione = 13;
  if(PageControlMag->ActivePage==TabPos14) IndicePosizione = 14;
  if(PageControlMag->ActivePage==TabPos15) IndicePosizione = 15;
  if(PageControlMag->ActivePage==TabPos16) IndicePosizione = 16;
  if(PageControlMag->ActivePage==TabPos17) IndicePosizione = 17;
  if(PageControlMag->ActivePage==TabPos18) IndicePosizione = 18;
  if(PageControlMag->ActivePage==TabPos19) IndicePosizione = 19;
  if(PageControlMag->ActivePage==TabPos20) IndicePosizione = 20;
  if(PageControlMag->ActivePage==TabPos21) IndicePosizione = 21;
  if(PageControlMag->ActivePage==TabPos22) IndicePosizione = 22;
  if(PageControlMag->ActivePage==TabPos23) IndicePosizione = 23;
  if(PageControlMag->ActivePage==TabPos24) IndicePosizione = 24;
  if(PageControlMag->ActivePage==TabPos25) IndicePosizione = 25;
  if(PageControlMag->ActivePage==TabPos26) IndicePosizione = 26;
  if(PageControlMag->ActivePage==TabPos27) IndicePosizione = 27;
  if(PageControlMag->ActivePage==TabPos28) IndicePosizione = 28;
  if(PageControlMag->ActivePage==TabPos29) IndicePosizione = 29;
  if(PageControlMag->ActivePage==TabPos30) IndicePosizione = 30;
  if(PageControlMag->ActivePage==TabPos31) IndicePosizione = 31;
  if(PageControlMag->ActivePage==TabPos32) IndicePosizione = 32;
  if(PageControlMag->ActivePage==TabPos33) IndicePosizione = 33;
  if(PageControlMag->ActivePage==TabPos34) IndicePosizione = 34;
  if(PageControlMag->ActivePage==TabPos35) IndicePosizione = 35;
  if(PageControlMag->ActivePage==TabPos36) IndicePosizione = 36;
  if(PageControlMag->ActivePage==TabPos37) IndicePosizione = 37;
  if(PageControlMag->ActivePage==TabPos38) IndicePosizione = 38;
  if(PageControlMag->ActivePage==TabPos39) IndicePosizione = 39;
  if(PageControlMag->ActivePage==TabPos40) IndicePosizione = 40;

  //----------------------------------------
  // GESTIONE_DINAMICA_MAG
  // Calcolo posizioni a terra teoriche
  //----------------------------------------
  quota=0;
  //---------------------------------------------------------
  // Prendo il TGV1 come riferimento per il calcolo delle
  // quote reali
  //---------------------------------------------------------
  rap_imp = AGV[1]->mission.RapEncoder;
  if(AGV[1]->mission.GestioneLaser==true) rap_imp=1;
  //---------------------------------------------------------
  // Recupero il lato box su cui eseguire il calcolo
  //---------------------------------------------------------
  lato_box = M.TipiBox[M.EditMag.TipoBox].Lung;
  a = (double)(M.EditMag.OffSetDaNodo/rap_imp)+(double)(M.TipiBox[M.EditMag.TipoBox].offset_tgv_da_nodo_mag/rap_imp)+(double)(((IndicePosizione-1)*(lato_box+M.TipiBox[M.EditMag.TipoBox].dist_box_in_magaz))/rap_imp);
  b = (double)(a) - (int)(a);
  if(b>0.5)  a = (a - b) + 1; // arrotondo all'impulso successivo
  //-------------------------------------------
  // Compilo mappa delle posizioni di deposito
  //-------------------------------------------
  quota = (WORD)(a);

  //----------------------------------------
  // Dati posizioni
  //----------------------------------------
  // nodo
  sprintf(buffer, "%04d", M.EditMag.Pos[IndicePosizione].NodoIng);
  EdNodP1->Text = buffer;
  sprintf(buffer, "%04d", M.EditMag.Pos[IndicePosizione].NodoOut);
  EdNodP2->Text = buffer;
  // Piani max per posizione
  sprintf(buffer, "%01d", M.EditMag.Pos[IndicePosizione].MaxHPos);
  edMaxPianiP->Text=buffer;
  // piano terra
  CBPres0->Checked  = M.EditMag.Pos[IndicePosizione].Presenza[0];
  GBPiano0->Enabled = CBPres0->Checked;
  BtnBox0->Enabled  = CBPres0->Checked;
  //---------------------------------------
  // Assegnazione del colore alla presenza
  //---------------------------------------
  CBPres0->Color    = clBtnFace;
  if(CBPres0->Checked==true){
     switch(M.EditMag.Pos[IndicePosizione].Box[0].StatoLav){
        case ST_BOX_VUOTI:CBPres0->Color=(TColor)(COLOR_PRES_VUOTI); break;
        case ST_BOX_LAV1: CBPres0->Color=(TColor)(COLOR_PRES_STAT1); break;
        case ST_BOX_LAV2: CBPres0->Color=(TColor)(COLOR_PRES_STAT2); break;
        case ST_BOX_LAV3: CBPres0->Color=(TColor)(COLOR_PRES_STAT3); break;
        case ST_BOX_LAV4: CBPres0->Color=(TColor)(COLOR_PRES_STAT4); break;
        case ST_BOX_LAV5: CBPres0->Color=(TColor)(COLOR_PRES_STAT5); break;
        case ST_BOX_LAV6: CBPres0->Color=(TColor)(COLOR_PRES_STAT6); break;
        case ST_BOX_LAV7: CBPres0->Color=(TColor)(COLOR_PRES_STAT7); break;
        case ST_BOX_LAV8: CBPres0->Color=(TColor)(COLOR_PRES_STAT8); break;
                 default: CBPres0->Color=(TColor)(COLOR_PRES_VUOTI); break;
     }
  }
  TxQP0_1->Caption = quota;
  TxQP0_2->Caption = M.EditMag.Pos[IndicePosizione].QuotaP[0];
  //------------
  // piano alto
  //------------
  CBPres1->Checked  = M.EditMag.Pos[IndicePosizione].Presenza[1];
  GBPiano1->Enabled = CBPres1->Checked;
  BtnBox1->Enabled  = CBPres1->Checked;
  //---------------------------------------
  // Assegnazione del colore alla presenza
  //---------------------------------------
  CBPres1->Color        = clBtnFace;
  if(CBPres1->Checked==true){
     switch(M.EditMag.Pos[IndicePosizione].Box[1].StatoLav){
        case ST_BOX_VUOTI:CBPres1->Color=(TColor)(COLOR_PRES_VUOTI); break;
        case ST_BOX_LAV1: CBPres1->Color=(TColor)(COLOR_PRES_STAT1); break;
        case ST_BOX_LAV2: CBPres1->Color=(TColor)(COLOR_PRES_STAT2); break;
        case ST_BOX_LAV3: CBPres1->Color=(TColor)(COLOR_PRES_STAT3); break;
        case ST_BOX_LAV4: CBPres1->Color=(TColor)(COLOR_PRES_STAT4); break;
        case ST_BOX_LAV5: CBPres1->Color=(TColor)(COLOR_PRES_STAT5); break;
        case ST_BOX_LAV6: CBPres1->Color=(TColor)(COLOR_PRES_STAT6); break;
        case ST_BOX_LAV7: CBPres1->Color=(TColor)(COLOR_PRES_STAT7); break;
        case ST_BOX_LAV8: CBPres1->Color=(TColor)(COLOR_PRES_STAT8); break;
                 default: CBPres1->Color=(TColor)(COLOR_PRES_VUOTI); break;
     }
  }

  //------------
  // piano 2
  //------------
  CBPres2->Checked  = M.EditMag.Pos[IndicePosizione].Presenza[2];
  GBPiano2->Enabled = CBPres2->Checked;
  BtnBox2->Enabled  = CBPres2->Checked;
  //---------------------------------------
  // Assegnazione del colore alla presenza
  //---------------------------------------
  CBPres2->Color        = clBtnFace;
  if(CBPres2->Checked==true){
     switch(M.EditMag.Pos[IndicePosizione].Box[2].StatoLav){
        case ST_BOX_VUOTI:CBPres2->Color=(TColor)(COLOR_PRES_VUOTI); break;
        case ST_BOX_LAV1: CBPres2->Color=(TColor)(COLOR_PRES_STAT1); break;
        case ST_BOX_LAV2: CBPres2->Color=(TColor)(COLOR_PRES_STAT2); break;
        case ST_BOX_LAV3: CBPres2->Color=(TColor)(COLOR_PRES_STAT3); break;
        case ST_BOX_LAV4: CBPres2->Color=(TColor)(COLOR_PRES_STAT4); break;
        case ST_BOX_LAV5: CBPres2->Color=(TColor)(COLOR_PRES_STAT5); break;
        case ST_BOX_LAV6: CBPres2->Color=(TColor)(COLOR_PRES_STAT6); break;
        case ST_BOX_LAV7: CBPres2->Color=(TColor)(COLOR_PRES_STAT7); break;
        case ST_BOX_LAV8: CBPres2->Color=(TColor)(COLOR_PRES_STAT8); break;
                 default: CBPres2->Color=(TColor)(COLOR_PRES_VUOTI); break;
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMagaz::CBPres0Click(TObject *Sender)
{
  //----------------------------------------
  // Dati posizioni
  //----------------------------------------
  // piano terra
  GBPiano0->Enabled = CBPres0->Checked;
  BtnBox0->Enabled  = CBPres0->Checked;
  //---------------------------------------
  // Assegnazione del colore alla presenza
  //---------------------------------------
  CBPres0->Color    = clBtnFace;
  if(CBPres0->Checked==true){
     switch(M.EditMag.Pos[IndicePosizione].Box[0].StatoLav){
        case ST_BOX_VUOTI:CBPres0->Color=(TColor)(COLOR_PRES_VUOTI); break;
        case ST_BOX_LAV1: CBPres0->Color=(TColor)(COLOR_PRES_STAT1); break;
        case ST_BOX_LAV2: CBPres0->Color=(TColor)(COLOR_PRES_STAT2); break;
        case ST_BOX_LAV3: CBPres0->Color=(TColor)(COLOR_PRES_STAT3); break;
        case ST_BOX_LAV4: CBPres0->Color=(TColor)(COLOR_PRES_STAT4); break;
        case ST_BOX_LAV5: CBPres0->Color=(TColor)(COLOR_PRES_STAT5); break;
        case ST_BOX_LAV6: CBPres0->Color=(TColor)(COLOR_PRES_STAT6); break;
        case ST_BOX_LAV7: CBPres0->Color=(TColor)(COLOR_PRES_STAT7); break;
        case ST_BOX_LAV8: CBPres0->Color=(TColor)(COLOR_PRES_STAT8); break;
                 default: CBPres0->Color=(TColor)(COLOR_PRES_VUOTI); break;
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMagaz::CBPres1Click(TObject *Sender)
{
  //----------------------------------------
  // Dati posizioni
  //----------------------------------------
  // piano alto
  GBPiano1->Enabled = CBPres1->Checked;
  BtnBox1->Enabled  = CBPres1->Checked;
  //---------------------------------------
  // Assegnazione del colore alla presenza
  //---------------------------------------
  CBPres1->Color        = clBtnFace;
  if(CBPres1->Checked==true){
     switch(M.EditMag.Pos[IndicePosizione].Box[1].StatoLav){
        case ST_BOX_VUOTI:CBPres1->Color=(TColor)(COLOR_PRES_VUOTI); break;
        case ST_BOX_LAV1: CBPres1->Color=(TColor)(COLOR_PRES_STAT1); break;
        case ST_BOX_LAV2: CBPres1->Color=(TColor)(COLOR_PRES_STAT2); break;
        case ST_BOX_LAV3: CBPres1->Color=(TColor)(COLOR_PRES_STAT3); break;
        case ST_BOX_LAV4: CBPres1->Color=(TColor)(COLOR_PRES_STAT4); break;
        case ST_BOX_LAV5: CBPres1->Color=(TColor)(COLOR_PRES_STAT5); break;
        case ST_BOX_LAV6: CBPres1->Color=(TColor)(COLOR_PRES_STAT6); break;
        case ST_BOX_LAV7: CBPres1->Color=(TColor)(COLOR_PRES_STAT7); break;
        case ST_BOX_LAV8: CBPres1->Color=(TColor)(COLOR_PRES_STAT8); break;
                 default: CBPres1->Color=(TColor)(COLOR_PRES_VUOTI); break;
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMagaz::CBPres2Click(TObject *Sender)
{
  //----------------------------------------
  // Dati posizioni
  //----------------------------------------
  // piano 2
  GBPiano2->Enabled = CBPres2->Checked;
  BtnBox2->Enabled  = CBPres2->Checked;
  //---------------------------------------
  // Assegnazione del colore alla presenza
  //---------------------------------------
  CBPres2->Color        = clBtnFace;
  if(CBPres2->Checked==true){
     switch(M.EditMag.Pos[IndicePosizione].Box[2].StatoLav){
        case ST_BOX_VUOTI:CBPres2->Color=(TColor)(COLOR_PRES_VUOTI); break;
        case ST_BOX_LAV1: CBPres2->Color=(TColor)(COLOR_PRES_STAT1); break;
        case ST_BOX_LAV2: CBPres2->Color=(TColor)(COLOR_PRES_STAT2); break;
        case ST_BOX_LAV3: CBPres2->Color=(TColor)(COLOR_PRES_STAT3); break;
        case ST_BOX_LAV4: CBPres2->Color=(TColor)(COLOR_PRES_STAT4); break;
        case ST_BOX_LAV5: CBPres2->Color=(TColor)(COLOR_PRES_STAT5); break;
        case ST_BOX_LAV6: CBPres2->Color=(TColor)(COLOR_PRES_STAT6); break;
        case ST_BOX_LAV7: CBPres2->Color=(TColor)(COLOR_PRES_STAT7); break;
        case ST_BOX_LAV8: CBPres2->Color=(TColor)(COLOR_PRES_STAT8); break;
                 default: CBPres2->Color=(TColor)(COLOR_PRES_VUOTI); break;
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMagaz::BtnCercaClick(TObject *Sender)
{
  //----------------------------------------
  // Se Dati BOX modificati Salva comunque
  // 1=Dati magazzino precedenti
  //----------------------------------------
  if(DatiBoxModificati){
     DatiBoxModificati=1;
     Salva();
  }
  Cerca();
}
//---------------------------------------------------------------------------

void __fastcall TFormMagaz::BtnCopiaClick(TObject *Sender)
{
  Copia();
}
//---------------------------------------------------------------------------

void __fastcall TFormMagaz::BtnSalvaClick(TObject *Sender)
{
  if(DatiBoxModificati) DatiBoxModificati=2;
  Salva();
}
//---------------------------------------------------------------------------

void __fastcall TFormMagaz::BtnStampaClick(TObject *Sender)
{
  bool OkStampa;

  OkStampa = PrintDialog->Execute();
  if(OkStampa == false) return;
  FormMagaz->Print();
}
//---------------------------------------------------------------------------

void __fastcall TFormMagaz::BtnCancellaClick(TObject *Sender)
{
  Cancel();
}
//---------------------------------------------------------------------------

void __fastcall TFormMagaz::BtnInserimentoClick(TObject *Sender)
{
  Inser();
}
//---------------------------------------------------------------------------

void __fastcall TFormMagaz::BtnAvantiClick(TObject *Sender)
{
  Avanti();
}
//---------------------------------------------------------------------------

void __fastcall TFormMagaz::BtnIndietroClick(TObject *Sender)
{
  Indietro();
}
//---------------------------------------------------------------------------

void __fastcall TFormMagaz::CBAssegChange(TObject *Sender)
{
  char buffer[101]="";
  bool assegnazione_not_ok;

  //------------------------------------------------
  // Gestione assegnazione
  //------------------------------------------------
  assegnazione_not_ok=false;
  switch(CBAsseg->ItemIndex){
     case  ASS_DINAMICA:  CBAsseg->Color = clCream;
                          break;
     case  PRES_LAV1:     CBAsseg->Color = (TColor)(COLOR_PRES_STAT1);
                          strcpy(buffer, ASS_LAV1);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD1);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  PRES_LAV2:     CBAsseg->Color = (TColor)(COLOR_PRES_STAT2);
                          strcpy(buffer, ASS_LAV2);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD2);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  PRES_LAV3:     CBAsseg->Color = (TColor)(COLOR_PRES_STAT3);
                          strcpy(buffer, ASS_LAV3);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD3);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  PRES_LAV4:     CBAsseg->Color = (TColor)(COLOR_PRES_STAT4);
                          strcpy(buffer, ASS_LAV4);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD4);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  PRES_LAV5:     CBAsseg->Color = (TColor)(COLOR_PRES_STAT5);
                          strcpy(buffer, ASS_LAV5);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD5);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  PRES_LAV6:     CBAsseg->Color = (TColor)(COLOR_PRES_STAT6);
                          strcpy(buffer, ASS_LAV6);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD6);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  PRES_LAV7:     CBAsseg->Color = (TColor)(COLOR_PRES_STAT7);
                          strcpy(buffer, ASS_LAV7);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD7);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  PRES_LAV8:     CBAsseg->Color = (TColor)(COLOR_PRES_STAT8);
                          strcpy(buffer, ASS_LAV8);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD8);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  PRES_VUOTO:    CBAsseg->Color = (TColor)(COLOR_PRES_VUOTI); break;
     default:             CBAsseg->Color = clCream;                    break;
  }
  //------------------------------------------------
  // Se l'assegnazione non è corretta assegno
  // quella iniziale
  //------------------------------------------------
  if(assegnazione_not_ok==true){
     strcpy(buffer, "Please put a correct code assignment!");
     Application->MessageBox(buffer, MESS[99], MB_OK | MB_ICONINFORMATION);
     CBAsseg->ItemIndex  = M.EditMag.Assegnazione;
     switch(M.EditMag.Assegnazione){
        case  ASS_DINAMICA: CBAsseg->Color = clCream;                    break;
        case  PRES_LAV1:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT1); break;
        case  PRES_LAV2:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT2); break;
        case  PRES_LAV3:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT3); break;
        case  PRES_LAV4:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT4); break;
        case  PRES_LAV5:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT5); break;
        case  PRES_LAV6:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT6); break;
        case  PRES_LAV7:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT7); break;
        case  PRES_LAV8:    CBAsseg->Color = (TColor)(COLOR_PRES_STAT8); break;
        case  PRES_VUOTO:   CBAsseg->Color = (TColor)(COLOR_PRES_VUOTI); break;
               default:     CBAsseg->Color = clCream;                    break;
     }
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormMagaz::TxQP0_2DblClick(TObject *Sender)
{
  int new_pos;
  AnsiString InputStringa;

  //-------------------------------------------
  // Solo con password di livello 3
  //-------------------------------------------
  if(LivelloPassword<3) return;
  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  InputStringa = InputBox("Pallet position", "Insert new value:", M.EditMag.Pos[IndicePosizione].QuotaP[0]);
  if(InputStringa.IsEmpty() == true) return;
  //-----------------------------------------------
  // Verifico valore rotazione
  //-----------------------------------------------
  new_pos = StrToInt(InputStringa);
  if(new_pos<0 || new_pos>M.EditMag.Profondita) new_pos=0;
  M.EditMag.Pos[IndicePosizione].QuotaP[0] = (short int)(new_pos);
  //-----------------------------------------------
  // Rinfresco dato visualizzato
  //-----------------------------------------------
  TxQP0_2->Caption = M.EditMag.Pos[IndicePosizione].QuotaP[0];
}
//---------------------------------------------------------------------------
void __fastcall TFormMagaz::TxQHClick(TObject *Sender)
{
  int piano, new_quota;
  AnsiString InputStringa;

  //--------------------------------------------------
  // Recupero l'indice del pulsante premuto
  //--------------------------------------------------
  piano = ((TStaticText *)Sender)->Tag;
  //-------------------------------------------
  // Solo con password di livello 3
  //-------------------------------------------
  if(LivelloPassword<3) return;
  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  InputStringa = InputBox("Pallet height", "Insert new value:", M.EditMag.Pos[IndicePosizione].QuotaH[piano]);
  if(InputStringa.IsEmpty() == true) return;
  //-----------------------------------------------
  // Verifico valore rotazione
  //-----------------------------------------------
  new_quota = StrToInt(InputStringa);
  if(new_quota<0) new_quota=0;
  M.EditMag.Pos[IndicePosizione].QuotaP[piano] = (short int)(new_quota);
  //-----------------------------------------------
  // Rinfresco dato visualizzato
  //-----------------------------------------------
  if(piano==1) TxQH1->Caption = M.EditMag.Pos[IndicePosizione].QuotaP[1];
  if(piano==2) TxQH2->Caption = M.EditMag.Pos[IndicePosizione].QuotaP[2];
}
//---------------------------------------------------------------------------

