//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Boxwin.h"
#include "Remoto.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormBox *FormBox;
//---------------------------------------------------------------------------
__fastcall TFormBox::TFormBox(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormBox::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  switch (Key){case VK_ESCAPE: Close(); break;}
}
//---------------------------------------------------------------------------

void __fastcall TFormBox::FormActivate(TObject *Sender)
{
  int i, j, k;
  int TotPos = POSINPRESA;
  int TotPin = PIANIINPRESA;
  //-----------------------------
  Attivazione=true;
  Saved=false;

  //------------------------------------------
  // Gestione solo lettura
  //------------------------------------------
  if( SoloLettura==true ){
     GBDati->Enabled=false;
     prdPanel->Enabled=false;
     BtnOK->Visible=false;
  }
  else{
     GBDati->Enabled=true;
     prdPanel->Enabled=true;
     BtnOK->Visible=true;
  }

  //-----------------------------
  // GESTIONE UDC ATTIVA
  //-----------------------------
  MaxUDC = MAXUDCBOX;
  if(MaxUDC>0) FormBox->Height = 620;
  else         FormBox->Height = 375;

  //-----------------------------
  // Arry di oggetti
  //-----------------------------
  Gruppi();

  //-----------------------------
  // Messaggeria
  //-----------------------------
  Messaggi();

  //--------------------------------------------------------
  // Rendo invisibili di Check box di pallet non previsti
  // in presa
  //--------------------------------------------------------
  for(i=TotPos; i<3; i++){
     CBPresa[i][0]->Visible = false;
     for(j=0; j<2; j++){
        CBPresa[i][j]->Visible = false;
     }
  }

  for(i=0; i<3; i++){
     for(j=TotPin; j<2; j++){
        CBPresa[i][j]->Visible = false;
     }
  }
  //--------------------------------------------------------
  // Se il pallet è alto disattivo le presenze al 1° piano
  //
  // AL-09/08/2021 ADELAIDE: possibilità di visualizzare
  // anche spunte piani ALTI per visualizzare dati in
  // caso prese anomale
  //--------------------------------------------------------
//if(B.EditBox.Basso==true){     // PALLET BASSI ANCHE PIANI ALTI
     for(i=0; i<TotPos; i++){
        for(j=1; j<TotPin; j++){
           CBPresa[i][j]->Visible = true;
        }
     }
//}
//else{                         // PALLET ALTI SOLO PIANO TERRA
//   for(i=0; i<TotPos; i++){
//      for(j=1; j<TotPin; j++){
//         CBPresa[i][j]->Visible = false;
//      }
//   }
//}

  //--------------------------------------------------------
  // Modifica dicitura per presa singola
  //--------------------------------------------------------
  CBPresa[0][0]->Caption = "1 L";
  CBPresa[0][0]->Width   = 46;
  if(TotPos==1 && TotPin==1){
     CBPresa[0][0]->Caption = "Presence";
     CBPresa[0][0]->Width   = 120;
  }

  //------------------------------------------
  // Gruppo di appartenenza
  //------------------------------------------
  CBGruppoSorg->Clear();
  CBGruppoDest->Clear();
  for(i=0; i<MAXGRUPPIIMP; i++){
     if(strcmp(P.GruppiBaie[i].Nome, "")==0) continue;
     CBGruppoSorg->Items->Add(P.GruppiBaie[i].Nome);
     CBGruppoDest->Items->Add(P.GruppiBaie[i].Nome);
  }
  //-----------------------------
  // Disattivazioni finestra
  //-----------------------------
  if(LivelloPassword<2){
     BtnDel->Enabled = false;
     BtnAdd->Enabled = false;
  }
  //-----------------------------
  // Attivazione timer chiusura
  //-----------------------------
  TimerClose->Enabled=true;
  //-----------------------------
  Tab01->Caption="1L"; Tab01->TabVisible=false; Tab01->Tag=0;
  Tab02->Caption="2L"; Tab02->TabVisible=false; Tab02->Tag=0;
  Tab03->Caption="3L"; Tab03->TabVisible=false; Tab03->Tag=0;
  Tab04->Caption="1H"; Tab04->TabVisible=false; Tab04->Tag=0;
  Tab05->Caption="2H"; Tab05->TabVisible=false; Tab05->Tag=0;
  Tab06->Caption="3H"; Tab06->TabVisible=false; Tab06->Tag=0;
  //------------------------------------------------------------
  for(j=0,k=0; j<PIANIINPRESA; j++){
     for(i=0; i<POSINPRESA; i++){
        Tab[i][j]->Tag=k;
        k++;
     }
  }
  //-----------------------------
  idUDC=0;
  PgCUDC->ActivePage=Tab01;
  //-----------------------------
  // Refresh della Form
  //-----------------------------
  SetCaselle();
  //-----------------------------
  prdPanel->Visible=false;

  if(CBPos1B->Checked==true && CBPos1B->Visible==true) { Tab01->TabVisible=true;   prdPanel->Visible=true; }
  if(CBPos2B->Checked==true && CBPos2B->Visible==true) { Tab02->TabVisible=true;   prdPanel->Visible=true; }
  if(CBPos3B->Checked==true && CBPos3B->Visible==true) { Tab03->TabVisible=true;   prdPanel->Visible=true; }

  if(CBPos1A->Checked==true && CBPos1A->Visible==true) { Tab04->TabVisible=true;   prdPanel->Visible=true; }
  if(CBPos2A->Checked==true && CBPos2A->Visible==true) { Tab05->TabVisible=true;   prdPanel->Visible=true; }
  if(CBPos3A->Checked==true && CBPos3A->Visible==true) { Tab06->TabVisible=true;   prdPanel->Visible=true; }

//if(Tab01->TabVisible==false && Tab02->TabVisible==true){
  if(Tab01->TabVisible==false && Tab02->TabVisible==true && MAXUDCBOX>=1){      // Andrea: controllo su MAXUDCBOX
     idUDC=1;
     PgCUDC->ActivePage=Tab02;
     SetCaselle();
  }
  //-----------------------------
  Attivazione=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormBox::FormClose(TObject *Sender, TCloseAction &Action)
{
  EditorBoxAttivo=0;
  //----------------------------------------------
  // Reset parametri
  //----------------------------------------------
  Tgv=0;
  Mac=0;
  Mag=0;
  Bai=0;
  Pos=0;
  Stz=0;
  Pin=0;
  Forc=0;
  //----------------------------------------------
  // Diabilitazione del Timer di chiusura
  //----------------------------------------------
  TimerClose->Enabled=false;
  SoloLettura=false;

  if(Autoclose==false && Saved==true) ModalResult=mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TFormBox::TimerCloseTimer(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
// +------------------------------------------+
// | Definizione array/matrici di oggetti     |
// +------------------------------------------+
void __fastcall TFormBox::Gruppi()
{
  CBPresa[0][0] = CBPos1B;
  CBPresa[1][0] = CBPos2B;
  CBPresa[2][0] = CBPos3B;

  CBPresa[0][1] = CBPos1A;
  CBPresa[1][1] = CBPos2A;
  CBPresa[2][1] = CBPos3A;
  //--------------------------
  Tab[0][0] = Tab01;
  Tab[1][0] = Tab02;
  Tab[2][0] = Tab03;

  Tab[0][1] = Tab04;
  Tab[1][1] = Tab05;
  Tab[2][1] = Tab06;
}

// +------------------------------------------+
// | Imposta i messaggi abbinati agli oggetti |
// +------------------------------------------+
void __fastcall TFormBox::Messaggi()
{
  int  i, codice;
  char buffer[51]="";

  FormBox->Caption     = MESS[458];
  BtnOK->Caption       = MESS[103];
  BtnEsci->Caption     = MESS[104];

  lblCod->Caption      = MESS[459];
  lblLot->Caption      = MESS[460];
  lblAlt->Caption      = MESS[461];

  txtUDC->Caption      = MESS[450];
  txtLocation->Caption = MESS[452];
  txtCode->Caption     = MESS[459];
  txtDesc->Caption     = MESS[716];
  txtSize->Caption     = MESS[475];
  txtBatch->Caption    = MESS[460];
  txtState->Caption    = MESS[474];
  txtShift->Caption    = MESS[718];
  txtSource->Caption   = MESS[476];
  txtDest->Caption     = MESS[477];
  txtQuantity->Caption = MESS[478];
  txtHeight->Caption   = MESS[461];
  txtWeight->Caption   = MESS[717];

  txtDate->Caption     = MESS[616];
  txtTime->Caption     = MESS[617];


  CBStatLav->Clear();
  CBStatLav->Items->Add(MESS[518]);        // NESSUNO
  //--------------------------------------------
  // ABBINA_CODICE_A_STATO_BOX
  //--------------------------------------------
  if(AbbinaCodiceStatoBox==true){
     codice=ASS_COD1;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD1);
     else          strcpy(buffer, "");
     CBStatLav->Items->Add(buffer);
     codice=ASS_COD2;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD2);
     else          strcpy(buffer, "");
     CBStatLav->Items->Add(buffer);
     codice=ASS_COD3;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD3);
     else          strcpy(buffer, "");
     CBStatLav->Items->Add(buffer);
     codice=ASS_COD4;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD4);
     else          strcpy(buffer, "");
     CBStatLav->Items->Add(buffer);
     codice=ASS_COD5;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD5);
     else          strcpy(buffer, "");
     CBStatLav->Items->Add(buffer);
     codice=ASS_COD6;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD6);
     else          strcpy(buffer, "");
     CBStatLav->Items->Add(buffer);
     codice=ASS_COD7;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD7);
     else          strcpy(buffer, "");
     CBStatLav->Items->Add(buffer);
     codice=ASS_COD8;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD8);
     else          strcpy(buffer, "");
     CBStatLav->Items->Add(buffer);
  }
  else{
     strcpy(buffer, ASS_LAV1); CBStatLav->Items->Add(buffer);
     strcpy(buffer, ASS_LAV2); CBStatLav->Items->Add(buffer);
     strcpy(buffer, ASS_LAV3); CBStatLav->Items->Add(buffer);
     strcpy(buffer, ASS_LAV4); CBStatLav->Items->Add(buffer);
     strcpy(buffer, ASS_LAV5); CBStatLav->Items->Add(buffer);
     strcpy(buffer, ASS_LAV6); CBStatLav->Items->Add(buffer);
     strcpy(buffer, ASS_LAV7); CBStatLav->Items->Add(buffer);
     strcpy(buffer, ASS_LAV8); CBStatLav->Items->Add(buffer);
  }
  CBStatLav->Items->Add(MESS[530]);        // VUOTO
  //------------------------------------------
  // Compilazione lista gruppi/tipi box
  //------------------------------------------
  CBTipPal->Clear();
  for(i=0; i<MAXTIPOBOXIMP; i++){
     if(strcmp(M.TipiBox[i].Nome, "")==0) break;
     CBTipPal->Items->Add(M.TipiBox[i].Nome);
  }
  //------------------------------------------
  RGDati->Caption         = MESS[729];
  lblTipoPallet->Caption  = MESS[728];
  gbConfigPresa->Caption  = MESS[711];
  CBSovrapp->Caption      = MESS[726];
  CBCarW->Caption         = MESS[708];
  CBBas->Caption          = MESS[727];
  CBRot->Caption          = MESS[710];
}
//---------------------------------------------------------------------------
// +------------------------------------------+
// | Rinfresco della form                     |
// +------------------------------------------+
void __fastcall TFormBox::SetCaselle()
{
  char buffer[51]="";
  short int i, j;
  unsigned int location;
  int idPRD=0;
  char ora[11]="";
  char data[11]="";

  sprintf(buffer, "%04d", B.EditBox.ID);
  EdIdBox->Text = buffer;
  CBStatLav->ItemIndex =  B.EditBox.StatoLav;
  //------------------------------------------------
  // Dati pallet
  //------------------------------------------------
  CBTipPal->ItemIndex = B.EditBox.TipoBox;
  CBBas->Checked    = B.EditBox.Basso;
  CBSovrapp->Checked= B.EditBox.Sovrap;
  CBCarW->Checked   = (bool)(B.EditBox.CarWash);
  CBRot->Checked    = B.EditBox.Ruotato180;
  //------------------------------------------------
  // Visualizzazione data/ora
  //------------------------------------------------
  timeint_to_timechar(&B.EditBox.TimeC, &data[0], &ora[0]);
  sprintf(buffer, "%s - %s", data, ora); txtTime1->Caption = buffer;
  timeint_to_timechar(&B.EditBox.TimeS, &data[0], &ora[0]);
  sprintf(buffer, "%s - %s", data, ora); txtTime2->Caption = buffer;
  //------------------------------------------------
  // Aggiornamento configurazione presa
  //------------------------------------------------
  for(i=0; i<POSINPRESA; i++){
     for(j=0; j<PIANIINPRESA; j++){
        CBPresa[i][j]->Checked = B.EditBox.Presa[i][j];
     }
  }
  //------------------------------------------------
  switch(idUDC){
     case 0: idPRD=Tab01->Tag;  break;
     case 1: idPRD=Tab02->Tag;  break;
     case 2: idPRD=Tab03->Tag;  break;
     case 3: idPRD=Tab04->Tag;  break;
     case 4: idPRD=Tab05->Tag;  break;
     case 5: idPRD=Tab06->Tag;  break;
     default:idPRD=0;           break;
  }
  //------------------------------------------------
  // GESTIONE UDC
  //------------------------------------------------
  edUDC->Text = B.EditBox.Prd[idPRD].Udc;
  //------------------------------------------------
  // Locazione di provenienza
  //------------------------------------------------
  sprintf(buffer, "%09d", B.EditBox.Prd[idPRD].loc_prov);
  TxtLoc1->Caption = buffer;
  //------------------------------------------------
  // Locazione ATTUALE
  //------------------------------------------------
  location    = B.StructToLocation(Tgv, Mac, Mag, Bai, Stz, Pos, Pin, Forc, idPRD);
  if(location!=0) sprintf(buffer, "%09d", location);
  else            sprintf(buffer, "%09d", B.EditBox.Prd[idPRD].location);
  edLocation->Text = buffer;
  TxtLoc2->Caption = buffer;

  txtProv->Caption = B.EditBox.ProvBox;
  //------------------------------------------------
  // Dati Pallet
  //------------------------------------------------
  sprintf(buffer, "%02d", B.EditBox.Codice);   EdCod->Text     = buffer;
  sprintf(buffer, "%05d", B.EditBox.Lotto);    EdLot->Text     = buffer;
  sprintf(buffer, "%04d", B.EditBox.Altezza);  EdAltezza->Text = buffer;

  // Data e Ora inserimento
  timeint_to_timechar(&B.EditBox.Prd[idPRD].time, &data[0], &ora[0]);
  EdData->Text  = data;
  EdOra->Text   = ora;

  //------------------------------------------------
  // Dati Pallet Prd
  //------------------------------------------------
  sprintf(buffer, "%s", B.EditBox.Prd[idPRD].code);        edCode->Text   = buffer;
  sprintf(buffer, "%s", B.EditBox.Prd[idPRD].description);
  edDesc->Text   = buffer;
  sprintf(buffer, "%s", B.EditBox.Prd[idPRD].size);
  edSize->Text   = buffer;
  sprintf(buffer, "%s", B.EditBox.Prd[idPRD].batch);
  edBatch->Text  = buffer;
  sprintf(buffer, "%s", B.EditBox.Prd[idPRD].state);       edState->Text  = buffer;
  sprintf(buffer, "%s", B.EditBox.Prd[idPRD].shift);       edShift->Text  = buffer;
  sprintf(buffer, "%d", B.EditBox.Prd[idPRD].source);      edSource->Text = buffer;
  sprintf(buffer, "%d", B.EditBox.Prd[idPRD].destination); edDest->Text   = buffer;
  sprintf(buffer, "%d", B.EditBox.Prd[idPRD].quantity);    edQty->Text    = buffer;
  sprintf(buffer, "%d", B.EditBox.Prd[idPRD].height);      edHeight->Text = buffer;
  sprintf(buffer, "%d", B.EditBox.Prd[idPRD].weight);      edWeight->Text = buffer;
  //------------------------------------------------
  // COLORAZIONE COMBO STATO LAVORAZIONE
  //------------------------------------------------
  switch(B.EditBox.StatoLav){
     case  ST_BOX_LAV1:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT1);  break;
     case  ST_BOX_LAV2:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT2);  break;
     case  ST_BOX_LAV3:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT3);  break;
     case  ST_BOX_LAV4:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT4);  break;
     case  ST_BOX_LAV5:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT5);  break;
     case  ST_BOX_LAV6:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT6);  break;
     case  ST_BOX_LAV7:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT7);  break;
     case  ST_BOX_LAV8:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT8);  break;
     case  ST_BOX_VUOTI:  CBStatLav->Color = (TColor)(COLOR_PRES_VUOTI);  break;
     default:             CBStatLav->Color = clSkyBlue;                   break;
  }
  //------------------------------------------------
  // Gruppo sorgente e destinazione
  //------------------------------------------------
  CBGruppoSorg->ItemIndex = B.EditBox.GruppoProv;
  CBGruppoDest->ItemIndex = B.EditBox.GruppoDest;
}
//---------------------------------------------------------------------------
// +------------------------------------------+
// | Aggiornamento della struttura            |
// +------------------------------------------+
void __fastcall TFormBox::GetCaselle()
{
  short int i, j;
  int idPRD=0;
  char ora[11]="";
  char data[11]="";

  B.EditBox.ID          = (short int)(atoi(EdIdBox->Text.c_str()));
  B.EditBox.StatoLav    = (short int)(CBStatLav->ItemIndex);
  //------------------------------------------------
  // Dati pallet
  //------------------------------------------------
  B.EditBox.TipoBox     = CBTipPal->ItemIndex;
  B.EditBox.Codice      = (BYTE)(atoi(EdCod->Text.c_str()));
  B.EditBox.Lotto       = atoi(EdLot->Text.c_str());
  B.EditBox.Altezza     = (short int)(atoi(EdAltezza->Text.c_str()));
  B.EditBox.Basso       = CBBas->Checked;
  B.EditBox.Sovrap      = CBSovrapp->Checked;
  B.EditBox.CarWash     = (short int)(CBCarW->Checked);
  B.EditBox.Ruotato180  = CBRot->Checked;
  //------------------------------------------------
  // Dati Pallet Prd
  //------------------------------------------------
  switch(idUDC){
     case 0: idPRD=Tab01->Tag;  break;
     case 1: idPRD=Tab02->Tag;  break;
     case 2: idPRD=Tab03->Tag;  break;
     case 3: idPRD=Tab04->Tag;  break;
     case 4: idPRD=Tab05->Tag;  break;
     case 5: idPRD=Tab06->Tag;  break;
     default:idPRD=0;           break;
  }
  //------------------------------------------------
  strcpy(B.EditBox.Prd[idPRD].code,         edCode->Text.c_str());
  strcpy(B.EditBox.Prd[idPRD].description,  edDesc->Text.c_str());
  strcpy(B.EditBox.Prd[idPRD].size,         edSize->Text.c_str());
  strcpy(B.EditBox.Prd[idPRD].batch,        edBatch->Text.c_str());
  strcpy(B.EditBox.Prd[idPRD].state,        edState->Text.c_str());
  strcpy(B.EditBox.Prd[idPRD].shift,        edShift->Text.c_str());

  B.EditBox.Prd[idPRD].source      = (unsigned int) atoi(edSource->Text.c_str());
  B.EditBox.Prd[idPRD].destination = (unsigned int) atoi(edDest->Text.c_str());
  B.EditBox.Prd[idPRD].quantity    = (short int   ) atoi(edQty->Text.c_str());
  B.EditBox.Prd[idPRD].height      = (short int   ) atoi(edHeight->Text.c_str());
  B.EditBox.Prd[idPRD].weight      = (short int   ) atoi(edWeight->Text.c_str());
  //------------------------------------------------
  // Aggiornamento configurazione presa
  //------------------------------------------------
  for(i=0; i<POSINPRESA; i++){
     for(j=0; j<PIANIINPRESA; j++){
        B.EditBox.Presa[i][j] = CBPresa[i][j]->Checked;
     }
  }

  //------------------------------------------------
  // GESTIONE UDC
  //------------------------------------------------
  strcpy(B.EditBox.Prd[idPRD].Udc, edUDC->Text.c_str());
  B.EditBox.Prd[idPRD].location = (unsigned int)(atoi(edLocation->Text.c_str()));

  // Data e Ora inserimento
  strcpy(data, EdData->Text.c_str());
  strcpy(ora,  EdOra->Text.c_str());
  timechar_to_timeint(&B.EditBox.Prd[idPRD].time, &data[0], &ora[0]);

  //------------------------------------------------
  // Gruppo sorgente e destinazione
  //------------------------------------------------
  B.EditBox.GruppoProv = CBGruppoSorg->ItemIndex;
  B.EditBox.GruppoDest = CBGruppoDest->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TFormBox::AzzeraTimerClick(TObject *Sender)
{
  //------------------------------------------------------
  // Disabilita/Abilita Timer per riprendere il conteggio
  // del tempo
  //------------------------------------------------------
  TimerClose->Enabled = false;
  TimerClose->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TFormBox::BtnOKClick(TObject *Sender)
{
  Salva();
}
//---------------------------------------------------------------------------
// ---------------------------
//    Salva()
// ---------------------------
// Funzione che salva il record sul file
//
void __fastcall TFormBox::Salva()
{
  int err=0;
  char all_mess[101]="";
  int  i, j, k, result;
  unsigned int location;
  char NomeFile[100];
  short int DiffTempo;
  long int TimeAttuale;

  //-------------------------------------------------------------
  // Richiesta di conferma salvataggio
  //-------------------------------------------------------------
  result = Application->MessageBox(MESS[380], MESS[99], MB_YESNO | MB_ICONQUESTION);
  if(result != IDYES) return;

  //-------------------------------------------------------------
  //  Recupera dati dalle caselle di dialogo
  //-------------------------------------------------------------
  GetCaselle();
  //-------------------------------------------------------------
  // Modalità di chiusa form
  //-------------------------------------------------------------
  ModalResult=mrOk;
  if(Autoclose==false) ModalResult=mrNone;
  //-------------------------------------------------------------
  // CONTROLLI AUTOMATICI
  // Ammetto un valore di stato box !=0 solo se linea impostata
  //-------------------------------------------------------------
  if(B.EditBox.StatoLav!=ST_BOX_VUOTI && B.EditBox.Codice==0){
     Application->MessageBox("manca CODICE PALETTA...", MESS[99], MB_OK | MB_ICONINFORMATION);
     ModalResult=mrNone;
     return;
  }
  //-------------------------------------------------------------
  // Assegnazione automatica dello stato lav in base al codice
  //-------------------------------------------------------------
  if(AbbinaCodiceStatoBox==true){
     if(B.EditBox.Codice==ASS_VUOTO                      ) B.EditBox.StatoLav=ST_BOX_VUOTI;
     if(B.EditBox.Codice!=0 && B.EditBox.Codice==ASS_COD1) B.EditBox.StatoLav=ST_BOX_LAV1;
     if(B.EditBox.Codice!=0 && B.EditBox.Codice==ASS_COD2) B.EditBox.StatoLav=ST_BOX_LAV2;
     if(B.EditBox.Codice!=0 && B.EditBox.Codice==ASS_COD3) B.EditBox.StatoLav=ST_BOX_LAV3;
     if(B.EditBox.Codice!=0 && B.EditBox.Codice==ASS_COD4) B.EditBox.StatoLav=ST_BOX_LAV4;
     if(B.EditBox.Codice!=0 && B.EditBox.Codice==ASS_COD5) B.EditBox.StatoLav=ST_BOX_LAV5;
     if(B.EditBox.Codice!=0 && B.EditBox.Codice==ASS_COD6) B.EditBox.StatoLav=ST_BOX_LAV6;
     if(B.EditBox.Codice!=0 && B.EditBox.Codice==ASS_COD7) B.EditBox.StatoLav=ST_BOX_LAV7;
     if(B.EditBox.Codice!=0 && B.EditBox.Codice==ASS_COD8) B.EditBox.StatoLav=ST_BOX_LAV8;
  }
  //-------------------------------------------------------------
  // Controlli sulla configurazione della presa
  // 1 - se i pallet in presa sono ALTI non è possibile
  //     il pallet al secondo piano
  // 2 - sui pallet ai piani alti deve essere sempre
  //     presente il corrispondente a terra
  //-------------------------------------------------------------
  // (1) pallet alti MAI sovrapposti in presa
  if(B.EditBox.Basso==false){
     for(i=0; i<POSINPRESA; i++){
        for(j=1; j<PIANIINPRESA; j++){       // ciclo da 1° piano
           //------------------------------------------
           // metto a "0" le presenze ai piani alti se
           // manca il corrispondente al piano basso
           //------------------------------------------
           B.EditBox.Presa[i][j]=false;
           //------------------------------------------
           // Non sono ammessi UDC per posizioni non
           // presenti in presa
           //------------------------------------------
           k=i+(j*POSINPRESA);
           memset(&B.EditBox.Prd[k], 0, sizeof(DATI_PRD));
        }
     }
  }
  // (2) reset presenza piano superiore se inferiore non presente
  for(i=0; i<POSINPRESA; i++){
     for(j=1; j<PIANIINPRESA; j++){          // ciclo da 1° piano
        if(B.EditBox.Presa[i][j] == false) continue;
        //------------------------------------------
        // Se il corrispondente al piano precedente
        // è presente --> continue!
        //------------------------------------------
        if(B.EditBox.Presa[i][j-1]==true ) continue;
        //------------------------------------------
        // metto a "0" le presenze ai piani alti se
        // manca il corrispondente al piano basso
        //------------------------------------------
        B.EditBox.Presa[i][j]=false;
        //------------------------------------------
        // Non sono ammessi UDC per posizioni non
        // presenti in presa
        //------------------------------------------
        k=i+(j*POSINPRESA);
        memset(&B.EditBox.Prd[k], 0, sizeof(DATI_PRD));
     }
  }

  //----------------------------------------------------------------
  // VERIFICA UNIVOCITA' UDC
  //----------------------------------------------------------------
  if(GestioneUdc==1){
     for(i=0; i<POSINPRESA; i++){
        for(j=0; j<PIANIINPRESA; j++){
           if(B.EditBox.Presa[i][j]==false) continue;
           //-------------------------------------------
           // Verifico se UDC duplicato
           //-------------------------------------------
           k=i+(j*POSINPRESA);
           location = B.StructToLocation(Tgv, Mac, Mag, Bai, Stz, Pos, Pin, Forc, k);
           err = B.VerificaUDCBoxDuplicati(B.EditBox.Prd[k].Udc, location, all_mess);
           if(err){
              Application->MessageBox(all_mess, "Warning", MB_OK + MB_ICONWARNING + MB_TOPMOST);
              ModalResult=mrNone;
              SetCaselle();
              return;
           }
        }
     }
  }

  //----------------------------------------------------------------
  // Forzature AUTOMATICHE legate ai singoli impianti
  //----------------------------------------------------------------
  VerificaInputBOX_IMPIANTI(&B.EditBox);
  //-------------------------------------------------------------
  // Refresh della form
  //-------------------------------------------------------------
  SetCaselle();

  if(Autoclose==false) Saved=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormBox::CBBasClick(TObject *Sender)
{
  int i, j;
  //--------------------------------------------------------
  // Se il pallet è alto disattivo le presenze al 1° piano
  //--------------------------------------------------------
  if(CBBas->Checked==true){     // PALLET BASSI ANCHE PIANI ALTI
     for(i=0; i<POSINPRESA; i++){
        for(j=1; j<PIANIINPRESA; j++){
           CBPresa[i][j]->Visible = true;
        }
     }
  }
  else{                         // PALLET ALTI SOLO PIANO TERRA
     for(i=0; i<POSINPRESA; i++){
        for(j=1; j<PIANIINPRESA; j++){
           CBPresa[i][j]->Visible = false;
        }
     }
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormBox::CBStatLavChange(TObject *Sender)
{
  char buffer[101]="";
  bool assegnazione_not_ok;

  //------------------------------------------------
  // Gestione assegnazione
  //------------------------------------------------
  assegnazione_not_ok=false;
  switch(CBStatLav->ItemIndex){
     case  ST_BOX_LAV1:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT2);
                          strcpy(buffer, ASS_LAV1);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD1);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_LAV2:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT2);
                          strcpy(buffer, ASS_LAV2);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD2);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_LAV3:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT3);
                          strcpy(buffer, ASS_LAV3);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD3);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_LAV4:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT4);
                          strcpy(buffer, ASS_LAV4);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD4);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_LAV5:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT5);
                          strcpy(buffer, ASS_LAV5);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD5);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_LAV6:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT6);
                          strcpy(buffer, ASS_LAV6);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD6);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_LAV7:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT7);
                          strcpy(buffer, ASS_LAV7);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD7);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_LAV8:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT8);
                          strcpy(buffer, ASS_LAV8);
                          if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD8);
                          if(atoi(buffer)==0) assegnazione_not_ok=true;
                          break;
     case  ST_BOX_VUOTI:  CBStatLav->Color = (TColor)(COLOR_PRES_VUOTI); break;
     default:             CBStatLav->Color = clSkyBlue;                  break;
  }
  //------------------------------------------------
  // Se l'assegnazione non è corretta assegno
  // quella iniziale
  //------------------------------------------------
  if(assegnazione_not_ok==true){
     strcpy(buffer, "Please put a correct code assignment!");
     Application->MessageBox(buffer, MESS[99], MB_OK | MB_ICONINFORMATION);
     CBStatLav->ItemIndex  = B.EditBox.StatoLav;
     switch(B.EditBox.StatoLav){
        case  ST_BOX_LAV1:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT1);  break;
        case  ST_BOX_LAV2:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT2);  break;
        case  ST_BOX_LAV3:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT3);  break;
        case  ST_BOX_LAV4:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT4);  break;
        case  ST_BOX_LAV5:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT5);  break;
        case  ST_BOX_LAV6:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT6);  break;
        case  ST_BOX_LAV7:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT7);  break;
        case  ST_BOX_LAV8:   CBStatLav->Color = (TColor)(COLOR_PRES_STAT8);  break;
        case  ST_BOX_VUOTI:  CBStatLav->Color = (TColor)(COLOR_PRES_VUOTI);  break;
        default:             CBStatLav->Color = clSkyBlue;                   break;
     }
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormBox::PgCUDCChange(TObject *Sender)
{
  idUDC=PgCUDC->ActivePageIndex;
  if(idUDC<0        ) return;
  if(idUDC>MAXUDCBOX) return;

  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormBox::CBPosClick(TObject *Sender)
{
  int i,j;
//int TagIndex;
  bool AlmenoUno=false;

  if(Attivazione==true) return;

//TagIndex = (((TCheckBox *)Sender)->Tag);
  //--------------------------------------------------------------------------
//if(CBPos3A->Checked==true && CBPos3A->Visible==true){
  if(CBPos3A->Checked==true && CBPos3A->Visible==true && MAXUDCBOX>=5 ){     // Andrea: controllo su MAXUDCBOX
     Tab06->TabVisible=true;
     PgCUDC->ActivePage=Tab06;
     idUDC=5;
  }
  else Tab06->TabVisible=false;

  if(CBPos2A->Checked==true && CBPos2A->Visible==true && MAXUDCBOX>=4){     // Andrea: controllo su MAXUDCBOX
     Tab05->TabVisible=true;
     PgCUDC->ActivePage=Tab05;
     idUDC=4;
  }
  else Tab05->TabVisible=false;

  if(CBPos1A->Checked==true && CBPos1A->Visible==true && MAXUDCBOX>=3){     // Andrea: controllo su MAXUDCBOX
     Tab04->TabVisible=true;
     PgCUDC->ActivePage=Tab04;
     idUDC=3;
  }
  else Tab04->TabVisible=false;
  //--------------------------------------------------------------------------
  if(CBPos3B->Checked==true && CBPos3B->Visible==true && MAXUDCBOX>=2){     // Andrea: controllo su MAXUDCBOX
     Tab03->TabVisible=true;
     PgCUDC->ActivePage=Tab03;
     idUDC=2;
  }
  else Tab03->TabVisible=false;

  if(CBPos2B->Checked==true && CBPos2B->Visible==true && MAXUDCBOX>=1){     // Andrea: controllo su MAXUDCBOX
     Tab02->TabVisible=true;
     PgCUDC->ActivePage=Tab02;
     idUDC=1;
  }
  else Tab02->TabVisible=false;

  if(CBPos1B->Checked==true && CBPos1B->Visible==true && MAXUDCBOX>=0){     // Andrea: controllo su MAXUDCBOX
     Tab01->TabVisible=true;
     PgCUDC->ActivePage=Tab01;
     idUDC=0;
  }
  else Tab01->TabVisible=false;
  //------------------------------------------------
  // Aggiornamento configurazione presa
  //------------------------------------------------
  AlmenoUno=false;
  for(i=0; i<POSINPRESA; i++){
     for(j=0; j<PIANIINPRESA; j++){
        B.EditBox.Presa[i][j] = CBPresa[i][j]->Checked;
        if (CBPresa[i][j]->Checked==true) AlmenoUno=true;
     }
  }
  if(AlmenoUno==false) prdPanel->Visible=false;
  else prdPanel->Visible=true;


  //------------------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------


