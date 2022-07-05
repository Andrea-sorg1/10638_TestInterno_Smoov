//---------------------------------------------------------------------------
#include <vcl.h>
#include <vcl/system.hpp>

#pragma hdrstop

#include "Remoto.h"           
#include "SQLDB.h"

#include "Batwin.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TFormCambioBatteria *FormCambioBatteria;
// --------------------------
//   STRUTTURE GRAFICHE
// --------------------------
//
// extern int Baie[][5];
//---------------------------------------------------------------------------
__fastcall TFormCambioBatteria::TFormCambioBatteria(TComponent* Owner) : TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::FormActivate(TObject *Sender)
{
  short int i;

  //--------------------------------------------------
  // Abilitazione del Timer della Form
  //--------------------------------------------------
  TimerClose->Enabled=true;
  //--------------------------------------------------
  FormCambioBatteria->Width=680;
  FormCambioBatteria->Height=360;
  //--------------------------------------------------
  // Editor macchine aperti
  //--------------------------------------------------
  // --- Inizializzazione delle variabili globali ---
  Inserimento = false;
  NumRec      = 0;
  //--------------------------------------------------
  // Recupero l'indice della riga di percorso attuale
  //--------------------------------------------------
  IndiceCBatt       = NumCBatt;
  EditorCBattAttivo = NumCBatt;

  //------------------------------------------
  // Gruppo di appartenenza
  //------------------------------------------
  cbGruppo->Clear();
  for(i=0; i<MAXGRUPPIIMP; i++){
     if(strcmp(P.GruppiBaie[i].Nome, "")==0) continue;
     cbGruppo->Items->Add(P.GruppiBaie[i].Nome);
  }

  // Assegna gli indirizzi all'oggetto condiviso
  Gruppi();

  // imposta i messaggi degli oggetti
  Messaggi();

  // Lettura struttura da file
  LeggiDati();

  // Compila Listview Allarmi
  CompilaAllarmi();

  //--------------------------------------------
  // LIVELLO "0" TUTTO IL PANEL PRICIPALE
  // DISATTIVATO
  //--------------------------------------------
  if(LivelloPassword<1){
     PanelCBatt->Enabled=false;
     //---------------------------------------
     // Dal GroupBox delle esclusioni che non
     // non appartiene al Panel Principale
     // lasci attivava solo l'esclusione
     //---------------------------------------
     CBEsclCom->Enabled=false;
  }
  else{
     PanelCBatt->Enabled=true;
     CBEsclCom->Enabled=true;
  }
  //--------------------------------------------------
  // Disattivazioni finestra con Password inferiore
  // a Livello 3.
  //--------------------------------------------------
  if(LivelloPassword<3 || AbilitaConfigurazioneImpianto==false){
     BtnCopia->Enabled       = false;
     BtnCancella->Enabled    = false;
     BtnInserimento->Enabled = false;
     edSigla->ReadOnly       = false;
     cbGruppo->Enabled       = false;
  }
  else{
     BtnCopia->Enabled       = true;
     BtnCancella->Enabled    = true;
     BtnInserimento->Enabled = true;
     edSigla->ReadOnly       = false;
     cbGruppo->Enabled       = true;
  }
  SetCaselle();
  BtnEsci->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::AzzeraTimerClick(TObject *Sender)
{
  //----------------------------------------------------------------
  // Disabilita/Abilita Timer per riprendere il conteggio del tempo
  //----------------------------------------------------------------
  TimerClose->Enabled = false;
  TimerClose->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::AzzeraTimerMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
  //------------------------------------------------------
  // Disabilita/Abilita Timer per riprendere il conteggio
  // del tempo
  //------------------------------------------------------
  TimerClose->Enabled = false;
  TimerClose->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::TimerCloseTimer(TObject *Sender)
{
  FormCambioBatteria->Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::FormClose(TObject *Sender, TCloseAction &Action)
{
  //----------------------------------------------
  // Disabilitazione del Timer di chiusura
  //----------------------------------------------
  TimerClose->Enabled=false;
  //----------------------------------------------
  // Editor macchine chiuso
  //----------------------------------------------
  NumCBatt=0;
  EditorCBattAttivo=0;
}
//---------------------------------------------------------------------------
// --------------------------------------------------
//  Intercettazione dei Pulsanti premuti da Tastiera
// --------------------------------------------------
void __fastcall TFormCambioBatteria::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Shift.Contains(ssAlt) && Key==VK_F4 ) return;
  switch (Key) {
     case VK_ESCAPE: Close();    break;
     case VK_NEXT:   Indietro(); break;
     case VK_PRIOR:  Avanti();   break;
  }
}
//---------------------------------------------------------------------------
// -----------------------------------------------------------
//  Assegna ad un unico oggetto gli indirizzi di vari oggetti
//  Gruppi()
// -----------------------------------------------------------
void __fastcall TFormCambioBatteria::Gruppi()
{

}
//---------------------------------------------------------------------------
// ------------------------------------------
//  Imposta i messaggi abbinati agli oggetti
//  Messaggi()
// ------------------------------------------
void __fastcall TFormCambioBatteria::Messaggi()
{
  //------------------------------------------
  // Caption della form
  //------------------------------------------
  FormCambioBatteria->Caption= MESS[327];

  GBStatoB->Caption          = MESS[463];
  CBEsclCom->Caption         = MESS[470];
  CBEsclM->Caption           = MESS[471];
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
  txtlNum->Caption           = MESS[300];
  txtNodo->Caption           = MESS[673];
  GBPlc->Caption             = MESS[703];
  txtIDPLC->Caption          = MESS[704];
  CBAutomatico->Caption      = MESS[497];
  CBBlocco->Caption          = MESS[705];
  CBRicIng->Caption          = MESS[385];
  CBOkIng->Caption           = MESS[386];
  CBTgvIng->Caption          = MESS[387];
}
//---------------------------------------------------------------------------
// ------------------------------------------------------------
//  Funzione che forza i nuovi valori nelle caselle di dialogo
//  (TEdit) di WINDOWS recuperandoli dalle variabili stringa
//  SetCaselle()
// ------------------------------------------------------------
void __fastcall TFormCambioBatteria::SetCaselle()
{
  int  i, j;
  char buffer[101]="";
  //------------------------------------------------
  // Memorizzo il numero della locazione aperta
  //------------------------------------------------
  EditorCBattAttivo = IndiceCBatt;
  //--------------------------------------------------
  // Editor non modificabile se Comunicazione con
  // PLC TERRA o I/O REMOTI è attiva
  //--------------------------------------------------
  if(break_Com[COM2]==true || P.EditCambioBatt.EsclComunic==true){
     GBPlc->Enabled = true;
  }
  else{
     GBPlc->Enabled = false;
  }
  //------------------------------------------------
  // Consensi CambioBatteria
  //------------------------------------------------
  sprintf(buffer, "%02d", P.EditCambioBatt.Plc);
  EdPlc->Text             = buffer;
  CBAutomatico->Checked   = P.EditCambioBatt.Automatico;
  CBBlocco->Checked       = P.EditCambioBatt.Blocco;
  CBRicIng->Checked       = P.EditCambioBatt.RichIng;
  CBOkIng->Checked        = P.EditCambioBatt.OkIng;
  CBTgvIng->Checked       = P.EditCambioBatt.TgvIng;

  //------------------------------------------------
  // Gruppo di appartenenza scaffale
  //------------------------------------------------
  cbGruppo->ItemIndex    = P.EditCambioBatt.Gruppo;
  //------------------------------------------------
  // Dati CambioBatteria
  //------------------------------------------------
  sprintf(buffer, "%04d", P.EditCambioBatt.Num);
  EdNum->Text             = buffer;
  edSigla->Text           = P.EditCambioBatt.Sigla;
  sprintf(buffer, "%04d", P.EditCambioBatt.Punto);
  EdNod->Text             = buffer;
  //------------------------------------------------
  // Abilitazioni/Disabilitazioni
  //------------------------------------------------
  CBEsclM->Checked     = P.EditCambioBatt.Esclusione;
  CBEsclCom->Checked   = P.EditCambioBatt.EsclComunic;
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

  // Compila Listview Allarmi
  CompilaAllarmi();
}
//---------------------------------------------------------------------------
// -----------------------------------------------------
//  Funzione che legge dalle caselle di dialogo (TEdit)
//  di WINDOWS e scarica su le variabili stringa
//  GetCaselle()
// -----------------------------------------------------
void __fastcall TFormCambioBatteria::GetCaselle()
{
  //------------------------------------------------
  // Dati CambioBatteria
  //------------------------------------------------
  P.EditCambioBatt.Num          = (short int)(atoi(EdNum->Text.c_str()));
  strcpy(P.EditCambioBatt.Sigla, edSigla->Text.c_str());
  P.EditCambioBatt.Punto        = (short int)(atoi(EdNod->Text.c_str()));
  //------------------------------------------------
  // Consensi con PLC
  //
  // SICUREZZA:
  // Lo stato dei consensi viene aggiornato in
  // struttura solo con comunicazione disattivata
  //------------------------------------------------
  P.EditCambioBatt.Plc = (short int)(atoi(EdPlc->Text.c_str()));
  if(break_Com[COM2]==true || P.EditCambioBatt.EsclComunic==true){
     P.EditCambioBatt.Automatico   = CBAutomatico->Checked;
     P.EditCambioBatt.Blocco       = CBBlocco->Checked;
     P.EditCambioBatt.RichIng      = CBRicIng->Checked;
     P.EditCambioBatt.OkIng        = CBOkIng->Checked;
     P.EditCambioBatt.TgvIng       = CBTgvIng->Checked;
  }
  //------------------------------------------------
  // Abilitazioni/Disabilitazioni
  //------------------------------------------------
  P.EditCambioBatt.Esclusione   = CBEsclM->Checked;
  P.EditCambioBatt.EsclComunic  = CBEsclCom->Checked;
  //----------------------------------------------
  // Gruppo di appartenenza macchina
  //----------------------------------------------
  P.EditCambioBatt.Gruppo       = (short int)(cbGruppo->ItemIndex);


  // Compila Listview Allarmi
  CompilaAllarmi();
}
//---------------------------------------------------------------------------
// ------------------------
//  Legge i Dati dai file
//  LeggiDati()
// ------------------------
void __fastcall TFormCambioBatteria::LeggiDati()
{
  int  posti, err;
  char all_mess[300];

  P.rec_attuale = IndiceCBatt;

  setmem(&P.EditCambioBatt, sizeof(CAMBIOBAT), 0);             // azzera la struttura
  //-------------------------------------
  // Recupera il primo record
  //-------------------------------------
  memcpy( &P.EditCambioBatt, P.cb[IndiceCBatt], sizeof( CAMBIOBAT ) );
  NumRec = P.num_record_cbatt();
}
//---------------------------------------------------------------------------
// -----------------------------------
//  Funzione che avanza di un record
//  Avanti()
// -----------------------------------
// Funzione che avanza di un record
//
void __fastcall TFormCambioBatteria::Avanti()
{
  int err=0;
  int n_rec;
  struct CAMBIOBAT cbb;
  //--------------------------------
  //  cerca il prossimo record
  //--------------------------------
  for( n_rec=P.rec_attuale+1; n_rec<=MAXBATT; n_rec++){
     if( P.cb[ n_rec ]->Num >0 ) break;
  }
  if( n_rec>MAXBATT ){
     Application->MessageBox(MESS[428], MESS[99], MB_OK | MB_ICONINFORMATION);
     return;
  }
  //------------------------------------
  //  Recupera dati nella struttura ccb
  //------------------------------------
  memset(&cbb, 0, sizeof(CAMBIOBAT));
  err = P.cerca_record_cbat(&cbb, n_rec );
  if( err==0 ){
     P.rec_attuale = (short int) n_rec;
     IndiceCBatt   = (short int) n_rec;

     LeggiDati();
  }
  Inserimento=false;

  SetCaselle();
}
//---------------------------------------------------------------------------
// --------------------------------------
//  Funzione che retrocede di un record
//  Indietro()
// --------------------------------------
void __fastcall TFormCambioBatteria::Indietro()
{
  int err=0;
  int n_rec;
  struct CAMBIOBAT cbb;

  //--------------------------------
  //  cerca il precedente record
  //--------------------------------
  for( n_rec=P.rec_attuale-1; n_rec>0; n_rec--){
     if( P.cb[ n_rec ]->Num >0 ) break;
  }
  if( n_rec<1 ){
     Application->MessageBox(MESS[429], MESS[99], MB_OK | MB_ICONINFORMATION);
     return;
  }

  //------------------------------------
  //  Recupera dati nella struttura ch
  //------------------------------------
  memset(&cbb, 0, sizeof(CAMBIOBAT));
  err = P.cerca_record_cbat(&cbb, n_rec );
  if(err==0){
     P.rec_attuale  = (short int) n_rec;
     IndiceCBatt    = (short int) n_rec;
     LeggiDati();
  }
  Inserimento=false;

  SetCaselle();
}
//---------------------------------------------------------------------------
// -------------------------------
//  Funzione che cerca un record
//  Cerca()
// -------------------------------
void __fastcall TFormCambioBatteria::Cerca()
{
  AnsiString InputStringa;
  char Chiave[5]="";
  int n_rec=1, err=0;
  struct CAMBIOBAT cbb;

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
  if(n_rec<0 || n_rec>MAXBATT){
     Application->MessageBox(MESS[375], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }
  //------------------------------------
  //  Recupera record
  //------------------------------------
  memset(&cbb, 0, sizeof(CAMBIOBAT));
  err = P.cerca_record_cbat(&cbb, n_rec );
  if(err) {
     Application->MessageBox(MESS[375], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }
  P.rec_attuale = (short int) n_rec;
  IndiceCBatt   = (short int) n_rec;
  memcpy( &P.EditCambioBatt, &cbb, sizeof(CAMBIOBAT));
  Inserimento=false;

  SetCaselle();
}
//---------------------------------------------------------------------------
// -------------------------------
//  Funzione che copia un record
//  Copia()
// -------------------------------
void __fastcall TFormCambioBatteria::Copia()
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
  if(n_rec<0 || n_rec>TOTBATT){
     Application->MessageBox(MESS[375], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }
  //------------------------------------
  //  Verifica se record esistente
  //------------------------------------
  if( n_rec == P.rec_attuale ) return;
  if( P.cb[ n_rec ]->Num >0  ){
     result = Application->MessageBox(MESS[376], MESS[99], MB_YESNO | MB_ICONQUESTION);
     if(result != IDYES) return;
  }
  //-----------------
  //  Copia record
  //-----------------
  memcpy(P.cb[n_rec],       P.cb[P.rec_attuale], sizeof(CAMBIOBAT));
  memcpy(&P.EditCambioBatt, P.cb[P.rec_attuale], sizeof(CAMBIOBAT));
  P.rec_attuale        = (short int) n_rec;
  IndiceCBatt          = (short int) n_rec;
  P.cb[ n_rec ]->Num   = (short int) n_rec;
  P.EditCambioBatt.Num = (short int) n_rec;

  Inserimento=false;

  NumRec = P.num_record_cbatt();
  //-----------------------------------------
  // Ricompilazione dei Punti notevoli
  //-----------------------------------------
  err = N.compila_punti_notevoli(buffer);
  if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //-----------------------------------------
  //  Salva record sul file
  //-----------------------------------------
  err = P.save_record_cbat( P.rec_attuale, buffer);
  if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //-----------------------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------------------
  sprintf(buffer, "(EDITBATT) - (CBatt:%d) Copia Editor Battery", IndiceCBatt);
  FileLogOperatore(buffer);

  SetCaselle();
}
//---------------------------------------------------------------------------
// ----------------------------------------
//  Funzione che salva il record sul file
//  Salva()
// ----------------------------------------
void __fastcall TFormCambioBatteria::Salva()
{
  int  err=0;
  int  i, j, w, result;
  int  num_loc;
  int  LocS, LocE;
  char buffer[120]="";
  char NomeFile[100]="";
  //struct CAMBIOBAT cbb_old;

  //----------------------------------------------------------------
  // Richiesta di conferma salvataggio
  //----------------------------------------------------------------
  result = Application->MessageBox(MESS[380], MESS[99], MB_YESNO | MB_ICONQUESTION);
  if(result != IDYES) return;
  //----------------------------------------------------------------
  //  Recupera dati dalle caselle di dialogo
  //----------------------------------------------------------------
  GetCaselle();
  if(P.EditCambioBatt.Num>TOTBATT){
     ShowMessage("ATTENTION: Battery number not ok!");
     return;
  }
  //--------------------------------------------------------------------
  // Se la macchina è di tipo sconosciuto viene automaticamente esclusa
  // --- per ora disabilitato, magari in fututo ---
  //--------------------------------------------------------------------
  //if(P.EditCambioBatt.Tipo==NN ) P.EditCambioBatt.Esclusione=true;

  //----------------------------------------------------------------
  // Ricompilazione dei Punti notevoli
  //----------------------------------------------------------------
  if(Inserimento==true){
     err = N.compila_punti_notevoli(buffer);
     if(err) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  //----------------------------------------------------------------
  // Salvataggio valori prima della rilettura da DB
  //----------------------------------------------------------------
  // memcpy(&cbb_old,  P.cb[P.EditCambioBatt.Num], sizeof(CAMBIOBAT));
  memcpy(P.cb[P.EditCambioBatt.Num], &P.EditCambioBatt, sizeof(CAMBIOBAT));
  //----------------------------------------------------------------
  //  Salva sul file
  //----------------------------------------------------------------
  Inserimento = false;
  memcpy( P.cb[P.EditCambioBatt.Num], &P.EditCambioBatt, sizeof(CAMBIOBAT));
  P.rec_attuale = P.EditCambioBatt.Num;
  IndiceCBatt   = P.EditCambioBatt.Num;
  NumRec        = P.num_record_cbatt();
  err = P.save_record_cbat( P.rec_attuale, buffer);
  if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //----------------------------------------------------------------
  // Traccia su LOG OPERATORE
  //----------------------------------------------------------------
  sprintf(buffer, "(EDITBAT) - (CBatt:%d) Salva editor Battery", IndiceCBatt);
  FileLogOperatore(buffer);
  //----------------------------------------------------------------
  //  Trasmetti comando al SERVER
  //----------------------------------------------------------------
  if( IdPC>1 ){
     err = RemSincronizzaTimeFile( P.cbat_name_sorg, P.cbat_name_dest, buffer );
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     sprintf( NomeFile, "CBatt%04d", P.rec_attuale );
     err = RemCreaFileComando ( NomeFile, (BYTE *) P.cb[ P.rec_attuale ], sizeof(CAMBIOBAT), buffer );
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  //----------------------------------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
// -------------------------------------
//  Funzione che abilita/disabilita
//  l'inserimento di un record sul file
//  Inser()
// -------------------------------------
void __fastcall TFormCambioBatteria::Inser()
{
  char buffer[151]="";

  if(LivelloPassword<2) return;
  Inserimento = !Inserimento;
  if(Inserimento == true ){ setmem( &P.EditCambioBatt, sizeof(CAMBIOBAT), 0); P.EditCambioBatt.Tipo=NN;}
  if(Inserimento == false){ memcpy( &P.EditCambioBatt, P.cb[P.rec_attuale], sizeof(CAMBIOBAT));}
  //-----------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------
  sprintf(buffer, "(EDITCBATT) - (CBatt:%d) Inserimento Editor CBattery", IndiceCBatt);
  FileLogOperatore(buffer);
  SetCaselle();
}
//---------------------------------------------------------------------------
// ----------------------------------
//  Funzione che cancella un record
//  Cancel()
// ----------------------------------
void __fastcall TFormCambioBatteria::Cancel()
{
  char buffer[100] ;
  char NomeFile[100];
  int err =0;
  int result, n_rec;
  struct CAMBIOBAT cbb;

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

  setmem (&P.EditCambioBatt, sizeof(CAMBIOBAT), 0);                  // azzera la struttura
  //-----------------------------------------
  //  Salva record sul file
  //-----------------------------------------
  memcpy( P.cb[P.rec_attuale], &P.EditCambioBatt, sizeof(CAMBIOBAT));
  err = P.save_record_cbat( P.rec_attuale, buffer);
  if( err ) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //-----------------------------------------
  //  Trasmetti comando al SERVER
  //-----------------------------------------
  if( IdPC>1 ){
      err = RemSincronizzaTimeFile( P.chiamate_name_sorg, P.chiamate_name_dest, buffer );
      if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
      sprintf( NomeFile, "Bai%04d", P.rec_attuale );
      err = RemCreaFileComando ( NomeFile, (BYTE *) P.cb[ P.rec_attuale ], sizeof( BAIE ), buffer );
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
     if( P.cb[ n_rec ]->Num >0 ) break;
  }

  //--------------------------------
  //  Se non trovato
  //  cerca il precedente record
  //--------------------------------
  if( n_rec>MAXBAIE ){
     for( n_rec=P.rec_attuale-1; n_rec>0; n_rec--){
        if( P.cb[ n_rec ]->Num >0 ) break;
     }
  }
  //-----------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------
  sprintf(buffer, "(EDITBATT) - (CBatt:%d) Cancellazione Editor Battery", IndiceCBatt);
  FileLogOperatore(buffer);
  //------------------------------------
  //  Recupera dati nella struttura ch
  //------------------------------------
  memset(&cbb, 0, sizeof(CAMBIOBAT));
  err = P.cerca_record_cbat(&cbb, n_rec );
  P.rec_attuale  = (short int) n_rec;
  IndiceCBatt = (short int) n_rec;
  memcpy( &P.EditCambioBatt, &cbb, sizeof(CAMBIOBAT));
  NumRec = P.num_record_baie();
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::BtnAvantiClick(TObject *Sender)
{
  Avanti();
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::BtnIndietroClick(TObject *Sender)
{
  Indietro();
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::BtnCercaClick(TObject *Sender)
{
  Cerca();
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::BtnCopiaClick(TObject *Sender)
{
  Copia();
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::BtnSalvaClick(TObject *Sender)
{
  Salva();
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::BtnInserimentoClick(TObject *Sender)
{
  Inser();
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::BtnCancellaClick(TObject *Sender)
{
  Cancel();
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::BtnStampaClick(TObject *Sender)
{
  bool OkStampa=false;

  Printer()->Orientation = poLandscape;
  OkStampa = PrintDialog->Execute();

  if(OkStampa) {
     Print();
     MessageDlg("\nPrint Job Done",  mtInformation, TMsgDlgButtons() << mbOK, 0);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::TxtPuntiAuxClick(TObject *Sender)
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
           P.EditCambioBatt.PuntoAux[i] = StrToIntDef(Buff.SubString(1, pos-1),0);
           Buff = Buff.SubString(pos+1, Buff.Length());
           i++;
        }
        else{
           P.EditCambioBatt.PuntoAux[i] = StrToIntDef(Buff,0);
        }
     }while(pos);
  }
  catch(...){
     ShowMessage("Error in input string format...");
  }
  return;
}
//---------------------------------------------------------------------------
// --------------------------------------------------------
// Compila gli eventuali allarmi presenti sulla macchina
// --------------------------------------------------------
void __fastcall TFormCambioBatteria::CompilaAllarmi()
{
  int  i=0;
  int  indice=0;
  char buffer1[301]="";
  char buffer2[301]="";
  char Stringa[300];
  //------------------------------------------------------
  // Compilazione struttura locale dei messaggi d'allarme
  //------------------------------------------------------
  ListView->Visible=true;
  if(load_ALLCBAT("ALLCBAT.ITA")){
     ListView->Visible=false;
     return;
  }
  //----------------------------------------------
  // Mostro la griglia degli allarmi se presenti
  //----------------------------------------------
  // ListView->Visible=false;
  // if( P.EditCambioBatt.Blocco)        ListView->Visible=true;
  // if( P.EditCambioBatt.BitAllarmi[0]) ListView->Visible=true;
  // if( P.EditCambioBatt.BitAllarmi[1]) ListView->Visible=true;
  // if( P.EditCambioBatt.BitAllarmi[2]) ListView->Visible=true;
  //------------------------------------------
  // Inizializzazione colonne
  //------------------------------------------
  ListView->Items->Clear();
  ListView->Column[0]->Width = 100;
  ListView->Column[1]->Width = 310;
  ListView->Column[0]->Caption = "NUM";
  ListView->Column[1]->Caption = MESS[630];
  //------------------------------------------------------------
  // Popolo la griglia con le stringhe degli allarmi presenti
  //------------------------------------------------------------
  indice=0;
  for(i=0; i<MAX_ALLARMI_CBAT; i++){
     if(TestBit((char *)&P.EditCambioBatt.BitAllarmi[0], i)){
        Copy(&DESCALLCBAT[i][0],  1, 12, buffer1);
        Copy(&DESCALLCBAT[i][0], 15, 55, buffer2);
        sprintf( Stringa, "%s %s", buffer1, buffer2);
        ListView->Items->Add();
        ListView->Items->Item[indice]->Caption=buffer1;
        ListView->Items->Item[indice]->SubItems->Add(buffer2); // 2° Colonna
        indice++;
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::ListViewCustomDrawItem(TCustomListView *Sender, TListItem *Item, TCustomDrawState State, bool &DefaultDraw)
{
  AlternateRowColor(Sender, Item);
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::ListViewCustomDrawSubItem(TCustomListView *Sender, TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw)
{
  AlternateRowColor(Sender, Item);
}
//---------------------------------------------------------------------------
void __fastcall TFormCambioBatteria::AlternateRowColor(TCustomListView *Sender, TListItem *Item)
{
  if(Item->Index % 2){
     Sender->Canvas->Font->Color  = clBlack;
     Sender->Canvas->Brush->Color = clWhite;
  }
  else{
     Sender->Canvas->Font->Color  = clBlack;
     Sender->Canvas->Brush->Color = (TColor) 0x00CCF2FF;
  }
}
//---------------------------------------------------------------------------


