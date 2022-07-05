//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Agvwin.h"
#include "Mapping.h"
#include "OmronETH.h"
#include "Asservi.h"
#include "SQLDB.h"
#include "Remoto.h"
#include "Laser.h"
#include "BoxWin.h"
#include "Stato.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormTgv *FormTgv;
//---------------------------------------------------------------------------
__fastcall TFormTgv::TFormTgv(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFormTgv::FormActivate(TObject *Sender)
{
  // --- Inizializzazione delle variabili globali ---
  NewPos = 0;
  OldPos = 0;
  NewPresCEST_A = 0;
  OldPresCEST_A = 0;
  NewPresCEST_B = 0;
  OldPresCEST_B = 0;
  memset(&EdBoxA, 0, sizeof(BOX));
  memset(&EdBoxB, 0, sizeof(BOX));

  NumTgv = NumeroTgv;

  // --- Flag di form aperta ---
  EditorAgvAttivo = NumTgv;

  // --- Gestione laser
  BtnMapping->Visible=true;
  if(!AGV[NumTgv]->mission.GestioneLaser || IdPC>1) BtnMapping->Visible=false;

  // --- imposta i messaggi degli oggetti ---
  Messaggi();

  // --- Lettura struttura da file  ---
  LeggiDati();

  //------------------------------------------
  // TGV A FORCA SINGOLA
  //------------------------------------------
  if(GestioneDoppiaForcaTGV==0) GBBox_A->Visible=false;

  //------------------------------------------
  // Abilitazione del Timer della Form
  //------------------------------------------
  TimerClose->Enabled=true;

  //------------------------------------------
  // Gestione della Password LIVELLO 1
  //------------------------------------------
  BtnAsservimento->Enabled = true;
  if(LivelloPassword<1){
     BtnAsservimento->Enabled = false;
  }
  //------------------------------------------
  // Gestione della Password LIVELLO 3
  //------------------------------------------
  EditTgv2->Enabled      = true;               // Posizione di riposo del Tgv
  GroupBoxTgv2->Enabled  = true;               // Stato della missione
  gbTGV->Enabled         = true;               // Bits di stato TGV
  gbASRV->Enabled        = true;               // Bits di stato ASRV
  //---------------------------------------
  // Attivazione Editor per RAP_ENCODER
  //---------------------------------------
  EditRap->Enabled  = true;
  EditRap->Color    = clWindow;
  if(LivelloPassword<3){
     EditTgv2->Enabled      = false;          // Posizione di riposo del Tgv
     GroupBoxTgv2->Enabled  = false;          // Stato della missione
     gbTGV->Enabled         = false;          // Bits di stato
     gbASRV->Enabled        = false;          // Bits di stato ASRV
     //---------------------------------------
     // Disattivazione Editor per RAP_ENCODER
     //---------------------------------------
     EditRap->Enabled = false;
     EditRap->Color   = clBtnFace;
  }

  //------------------------------------------
  // Recupero dati di apertura pagina
  //------------------------------------------
  memcpy(&EdBoxA, &AGV[NumTgv]->mission.BoxA, sizeof(BOX));
  memcpy(&EdBoxB, &AGV[NumTgv]->mission.BoxB, sizeof(BOX));
  //------------------------------------------
  // Rinfresco della Form
  //------------------------------------------
  SetCaselle();
  tgv_color->Brush->Color=AGV[NumTgv]->mission.Col;
}
//---------------------------------------------------------------------------
void __fastcall TFormTgv::AzzeraTimerClick(TObject *Sender)
{
  //------------------------------------------------------
  // Disabilita/Abilita Timer per riprendere il conteggio
  // del tempo
  //------------------------------------------------------
  TimerClose->Enabled = false;
  TimerClose->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormTgv::AzzeraTimerMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
  //------------------------------------------------------
  // Disabilita/Abilita Timer per riprendere il conteggio
  // del tempo
  //------------------------------------------------------
  TimerClose->Enabled = false;
  TimerClose->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormTgv::FormClose(TObject *Sender, TCloseAction &Action)
{
  // --- Flag di form chiusa ---
  EditorAgvAttivo = 0;
  //----------------------------------------------
  // Diabilitazione del Timer di chiusura
  //----------------------------------------------
  TimerClose->Enabled=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormTgv::TimerCloseTimer(TObject *Sender)
{
  FormTgv->Close();
}
//---------------------------------------------------------------------------
// +--------------------------------------------------+
// | Intercettazione dei Pulsanti premuti da Tastiera |
// +--------------------------------------------------+
void __fastcall TFormTgv::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Shift.Contains(ssAlt) && Key==VK_F4 ) return;

  switch (Key){ case VK_ESCAPE: Close(); break; }
}
//---------------------------------------------------------------------------

// +------------------------------------------+
// | Imposta i messaggi abbinati agli oggetti |
// +------------------------------------------+
void __fastcall TFormTgv::Messaggi()
{
  int  codice;
  char buffer[101]="";

  // --- Intestazione Form ---
  sprintf(buffer, "Editor TGV: %02d", NumTgv);
  FormTgv->Caption = buffer;

  GroupBoxTgv1->Caption = MESS[480];
  TextTgv1->Caption     = MESS[481];
  TextTgv2->Caption     = MESS[482];
  TextTgv3->Caption     = MESS[483];
  TextTgv4->Caption     = MESS[484];
  TextTgv5->Caption     = MESS[485];
  TextTgv6->Caption     = MESS[486];
  TextTgv7->Caption     = MESS[487];

  GroupBoxTgv2->Caption = MESS[488];
  GroupBoxTgv2->Items->Strings[ 0] = MESS[489];
  GroupBoxTgv2->Items->Strings[ 1] = MESS[490];
  GroupBoxTgv2->Items->Strings[ 2] = MESS[491];

  GroupBoxTgv3->Caption      = MESS[492];
  BoxEsclusioneTgv1->Caption = MESS[493];
  BoxEsclusioneTgv2->Caption = MESS[494];
  BoxEsclusioneTgv3->Caption = MESS[495];
  BoxEsclusioneTgv4->Caption = MESS[496];

  //-------------------------------------
  // TGV
  //-------------------------------------
  gbTGV->Caption             = MESS[500];
  BoxStatoTgv1->Caption      = MESS[501];
  BoxStatoTgv2->Caption      = MESS[502];
  BoxStatoTgv3->Caption      = MESS[503];
  BoxStatoTgv4->Caption      = MESS[504];
  BoxStatoTgv5->Caption      = MESS[505];
  BoxStatoTgv6->Caption      = MESS[506];
  BoxStatoTgv7->Caption      = MESS[507];
  BoxStatoTgv8->Caption      = MESS[508];
  BoxStatoTgv9->Caption      = MESS[509];
  BoxStatoTgv10->Caption     = MESS[510];
  BoxStatoTgv11->Caption     = MESS[511];
  BoxStatoTgv12->Caption     = MESS[512];
  BoxStatoTgv13->Caption     = MESS[513];
  sprintf(buffer, MESS[514], 'B');
  BoxStatoTgv14->Caption     = buffer;
  sprintf(buffer, MESS[514], 'A');
  BoxStatoTgv15->Caption     = buffer;
  BoxStatoTgv16->Caption     = MESS[515];
  sprintf(buffer, MESS[516], 'B');
  BoxStatoTgv17->Caption     = buffer;
  sprintf(buffer, MESS[516], 'A');
  BoxStatoTgv18->Caption     = buffer;
  sprintf(buffer, MESS[517], 'B');
  BoxStatoTgv19->Caption     = buffer;
  sprintf(buffer, MESS[517], 'A');
  BoxStatoTgv20->Caption     = buffer;

  //-------------------------------------
  // GESTIONE_ASRV
  //-------------------------------------
  gbASRV->Caption            = MESS[500];




  GroupBoxTgv5->Caption            = MESS[519];
  GroupBoxTgv5->Items->Strings[ 0] = MESS[520];
  GroupBoxTgv5->Items->Strings[ 1] = MESS[521];
  GroupBoxTgv5->Items->Strings[ 2] = MESS[522];
  GroupBoxTgv5->Items->Strings[ 3] = MESS[523];

  GroupBoxTgv6->Caption  = MESS[525];
  GBBox_A->Caption       = MESS[526];
  GBBox_B->Caption       = MESS[527];
  BtnBoxA->Caption       = MESS[528];
  BtnBoxB->Caption       = MESS[528];

  // FORCA "A"
  CBPresA->Clear();
  CBPresA->Items->Add(MESS[518]);        // NESSUNO
  //--------------------------------------------
  // ABBINA_CODICE_A_STATO_BOX
  //--------------------------------------------
  if(AbbinaCodiceStatoBox==true){
     codice=ASS_COD1;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD1);
     else          strcpy(buffer, "");
     CBPresA->Items->Add(buffer);
     codice=ASS_COD2;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD2);
     else          strcpy(buffer, "");
     CBPresA->Items->Add(buffer);
     codice=ASS_COD3;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD3);
     else          strcpy(buffer, "");
     CBPresA->Items->Add(buffer);
     codice=ASS_COD4;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD4);
     else          strcpy(buffer, "");
     CBPresA->Items->Add(buffer);
     codice=ASS_COD5;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD5);
     else          strcpy(buffer, "");
     CBPresA->Items->Add(buffer);
     codice=ASS_COD6;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD6);
     else          strcpy(buffer, "");
     CBPresA->Items->Add(buffer);
     codice=ASS_COD7;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD7);
     else          strcpy(buffer, "");
     CBPresA->Items->Add(buffer);
     codice=ASS_COD8;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD8);
     else          strcpy(buffer, "");
     CBPresA->Items->Add(buffer);
  }
  else{
     strcpy(buffer, ASS_LAV1); CBPresA->Items->Add(buffer);
     strcpy(buffer, ASS_LAV2); CBPresA->Items->Add(buffer);
     strcpy(buffer, ASS_LAV3); CBPresA->Items->Add(buffer);
     strcpy(buffer, ASS_LAV4); CBPresA->Items->Add(buffer);
     strcpy(buffer, ASS_LAV5); CBPresA->Items->Add(buffer);
     strcpy(buffer, ASS_LAV6); CBPresA->Items->Add(buffer);
     strcpy(buffer, ASS_LAV7); CBPresA->Items->Add(buffer);
     strcpy(buffer, ASS_LAV8); CBPresA->Items->Add(buffer);
  }
  CBPresA->Items->Add(MESS[530]);       // VUOTO

  // FORCA "B"
  CBPresB->Clear();
  CBPresB->Items->Add(MESS[518]);        // NESSUNO
  //--------------------------------------------
  // ABBINA_CODICE_A_STATO_BOX
  //--------------------------------------------
  if(AbbinaCodiceStatoBox==true){
     codice=ASS_COD1;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD1);
     else          strcpy(buffer, "");
     CBPresB->Items->Add(buffer);
     codice=ASS_COD2;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD2);
     else          strcpy(buffer, "");
     CBPresB->Items->Add(buffer);
     codice=ASS_COD3;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD3);
     else          strcpy(buffer, "");
     CBPresB->Items->Add(buffer);
     codice=ASS_COD4;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD4);
     else          strcpy(buffer, "");
     CBPresB->Items->Add(buffer);
     codice=ASS_COD5;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD5);
     else          strcpy(buffer, "");
     CBPresB->Items->Add(buffer);
     codice=ASS_COD6;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD6);
     else          strcpy(buffer, "");
     CBPresB->Items->Add(buffer);
     codice=ASS_COD7;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD7);
     else          strcpy(buffer, "");
     CBPresB->Items->Add(buffer);
     codice=ASS_COD8;
     if(codice!=0) sprintf(buffer, MESS[529], ASS_COD8);
     else          strcpy(buffer, "");
     CBPresB->Items->Add(buffer);
  }
  else{
     strcpy(buffer, ASS_LAV1); CBPresB->Items->Add(buffer);
     strcpy(buffer, ASS_LAV2); CBPresB->Items->Add(buffer);
     strcpy(buffer, ASS_LAV3); CBPresB->Items->Add(buffer);
     strcpy(buffer, ASS_LAV4); CBPresB->Items->Add(buffer);
     strcpy(buffer, ASS_LAV5); CBPresB->Items->Add(buffer);
     strcpy(buffer, ASS_LAV6); CBPresB->Items->Add(buffer);
     strcpy(buffer, ASS_LAV7); CBPresB->Items->Add(buffer);
     strcpy(buffer, ASS_LAV8); CBPresB->Items->Add(buffer);
  }
  CBPresB->Items->Add(MESS[530]);       // VUOTO

  BtnSalva->Caption        = MESS[533];
  BtnCarica->Caption       = MESS[534];
  BtnAsservimento->Caption = MESS[535];
  BtnReset->Caption        = MESS[536];
  BtnEsci->Caption         = MESS[171];
}

//---------------------------------------------------------------------------

// +------------------------+
// | Legge i Dati dai file  |
// +------------------------+
void __fastcall TFormTgv::LeggiDati()
{
  int  err=0;
  char buffer[201]="";
  WORD dm[20];

  sprintf(buffer, "Ok");
  //-------------------------------
  // lettura stato AGV
  //-------------------------------
  if(!AGV[NumTgv]->mission.EsclPLC && !break_Com[COM1]){
     err = AGV[NumTgv]->load_stato(buffer);
     if(err){
        Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
        if(AGV[NumTgv]->load_mission( buffer )) AGV[NumTgv]->delete_mission( buffer );
     }
  }
}
//---------------------------------------------------------------------------

// ---------------------------
//    Salva()
// ---------------------------
// Funzione che salva il record sul file
//
void __fastcall TFormTgv::Salva()
{
  WORD dm[3];
  bool anomalia;
  bool manca_presenza_presa;
  char buffer[301]="";
  char Posizione[51]="";
  char NomeFile[100]="";
  int  i, j, result, err=0;
  short int altro_agv;
  short int tipo, num_prov;

  GetCaselle();
  //---------------------------------------------------------
  // Inizializzazione
  //---------------------------------------------------------
  memset(&dm[0], 0, sizeof(dm));
  //---------------------------------------------------------
  // Compilazione DM risevati navette LASER
  //---------------------------------------------------------
  if(break_Com[COM1]==false && AGV[NumTgv]->mission.EsclPLC==false && AGV[NumTgv]->mission.GestioneLaser){
     //---------------------------------------------------------
     // Informo il TGV di che tipo di funzionamento selezionato
     // DM1:
     // 0 - Controllo da PC
     // 1 - Guida a magneti (15/09/2011 - non utilizzato)
     // 2 - Guida a laser
     //---------------------------------------------------------
     dm[0] = 2;
     err = OM.wr_canale(dm, (WORD)(NumTgv), "WD", 1, 1, buffer);
     if(err) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     //---------------------------------------------------------
     // TRASFERIMENTO RAPENCODER*1000 SU DM200 NEI TGV LASER
     //---------------------------------------------------------
     dm[0] = WORD(AGV[NumTgv]->mission.RapEncoder*1000);
     err = LaserInvioRapportoEncoder(NumTgv, dm[0], buffer);
     if(err) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  //---------------------------------------------------------
  // Ammesse solo posizioni di impianto
  //---------------------------------------------------------
  if(NewPos<0 || NewPos>MAXPUNTI){
     sprintf(buffer, "new position %d, not admitted!\n\n(range from 0 to %d)", NewPos, MAXPUNTI);
     ShowMessage(buffer);
  }
  //---------------------------------------------------------
  // Se pos AGV è stata modificata trasmetti nuova posizione
  //---------------------------------------------------------
  if( OldPos!=NewPos ){
     AGV[NumTgv]->stato.pos=NewPos;
     err = AGV[NumTgv]->trasmetti_new_pos_AGV(AGV[NumTgv]->stato.pos, buffer);
     if(err) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     //-----------------------------------------
     //  Trasmetti comando al SERVER
     //-----------------------------------------
     if( IdPC>1 ){
        if(!err){
           err = RemSincronizzaTimeFile( AGV[NumTgv]->mission_name_sorg, AGV[NumTgv]->mission_name_dest, buffer);
           if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
           sprintf( NomeFile, "PosTgv%02d", NumTgv );
           sprintf( Posizione,"%d", AGV[NumTgv]->stato.pos );
           err = RemCreaFileComando ( NomeFile, (BYTE *) Posizione, sizeof( Posizione ), buffer );
           if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
        }
     }
  }

  //----------------------------------------------------
  // GESTIONE FORCA SINGOLA CON DOPPIO CARICO
  // Il bit di carico della navetta viene simulato
  // in base allo stato del bit di FORCA_B e lo
  // stato della presenza dati
  //----------------------------------------------------
  anomalia=false;
  if(GestioneDoppiaForcaTGV!=2){
     if((bool)(NewPresCEST_A)!=(bool)(AGV[NumTgv]->stato.s.bit.carA)) anomalia=true;
     if((bool)(NewPresCEST_B)!=(bool)(AGV[NumTgv]->stato.s.bit.carB)) anomalia=true;
  }
  else{
     if(AGV[NumTgv]->stato.s.bit.carB==true  && (NewPresCEST_A==LIBERA && NewPresCEST_B==LIBERA)) anomalia=true;
     if(AGV[NumTgv]->stato.s.bit.carB==false && (NewPresCEST_A!=LIBERA || NewPresCEST_B!=LIBERA)) anomalia=true;
  }
  if(anomalia){
     strcpy(buffer, MESS[369]);
     Application->MessageBox( buffer, MESS[99], MB_OK | MB_ICONINFORMATION);
     //---------------------------------------------------
     // Dopo la segnalazione riaggiorno anche l'editor
     //---------------------------------------------------
     CBPresA->ItemIndex = AGV[NumTgv]->mission.PresCEST_A;
     CBPresB->ItemIndex = AGV[NumTgv]->mission.PresCEST_B;
     return;
  }

  //------------------------------------------------
  // CONTROLLI AUTOMATICI SI DATI A BORDO
  //------------------------------------------------
  // FORCHE "A"
  if(NewPresCEST_A==LIBERA && !AGV[NumTgv]->stato.s.bit.carA){
     memset(&EdBoxA, 0, sizeof(BOX));
  }
  // FORCHE "B"
  if(NewPresCEST_B==LIBERA && !AGV[NumTgv]->stato.s.bit.carB){
     memset(&EdBoxB, 0, sizeof(BOX));
  }

  //------------------------------------------------
  // TGV A FORCA SINGOLA
  // Forzo a "0" i dati legati alla forca alta
  //------------------------------------------------
  if(GestioneDoppiaForcaTGV==0){
     memset(&EdBoxA, 0, sizeof(BOX));
  }
  // FORCA BASSA
  if(NewPresCEST_B!=LIBERA){
     //-------------------------------------------------------------
     // Se la presenza della posizione è alta e non c'è nessuna
     // presenza in PRESA setto tutte le presenze in presa
     //-------------------------------------------------------------
     manca_presenza_presa=true;
     for(i=0; i<POSINPRESA; i++){
        if(EdBoxB.Presa[i][0]==false) continue;
        manca_presenza_presa=false;
        break;
     }
     //-------------------------------------------------------------
     // Se mancano le presenze, obbligo di ingresso nell'editor
     // box a darle
     //-------------------------------------------------------------
     if(manca_presenza_presa==true){
      //for(i=0; i<POSINPRESA; i++) EdBoxB.Presa[i][0]=true;
        memcpy(&B.EditBox, &EdBoxB, sizeof(BOX));
        FormBox->Tgv=NumTgv;
        FormBox->Forc=N_FORCA_B;
        FormBox->Autoclose=true;
        FormBox->ShowModal();
        if(FormBox->ModalResult==mrOk){
           memcpy(&EdBoxB, &B.EditBox, sizeof(BOX));
           //----------------------------------
           // Variazione tipo box a bordo
           //----------------------------------
           switch(EdBoxB.StatoLav){
              case  ST_BOX_LAV1: CBPresB->ItemIndex=PRES_LAV1;   break;
              case  ST_BOX_LAV2: CBPresB->ItemIndex=PRES_LAV2;   break;
              case  ST_BOX_LAV3: CBPresB->ItemIndex=PRES_LAV3;   break;
              case  ST_BOX_LAV4: CBPresB->ItemIndex=PRES_LAV4;   break;
              case  ST_BOX_LAV5: CBPresB->ItemIndex=PRES_LAV5;   break;
              case  ST_BOX_LAV6: CBPresB->ItemIndex=PRES_LAV6;   break;
              case  ST_BOX_LAV7: CBPresB->ItemIndex=PRES_LAV7;   break;
              case  ST_BOX_LAV8: CBPresB->ItemIndex=PRES_LAV8;   break;
              case  ST_BOX_VUOTI:CBPresB->ItemIndex=PRES_VUOTO;  break;
              default:           CBPresB->ItemIndex=LIBERA;      break;
           }
        }
        return;
     }
     //-------------------------------------------------------------
     // Nuova posizione del box
     //-------------------------------------------------------------
     EdBoxB.Pos = NumTgv;
     switch(NewPresCEST_B){
        case  PRES_LAV1: EdBoxB.StatoLav=ST_BOX_LAV1;
                         if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD1;
                         break;
        case  PRES_LAV2: EdBoxB.StatoLav=ST_BOX_LAV2;
                         if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD2;
                         break;
        case  PRES_LAV3: EdBoxB.StatoLav=ST_BOX_LAV3;
                         if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD3;
                         break;
        case  PRES_LAV4: EdBoxB.StatoLav=ST_BOX_LAV4;
                         if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD4;
                         break;
        case  PRES_LAV5: EdBoxB.StatoLav=ST_BOX_LAV5;
                         if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD5;
                         break;
        case  PRES_LAV6: EdBoxB.StatoLav=ST_BOX_LAV6;
                         if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD6;
                         break;
        case  PRES_LAV7: EdBoxB.StatoLav=ST_BOX_LAV7;
                         if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD7;
                         break;
        case  PRES_LAV8: EdBoxB.StatoLav=ST_BOX_LAV8;
                         if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD8;
                         break;
        case  PRES_VUOTO:EdBoxB.StatoLav=ST_BOX_VUOTI;
                         EdBoxB.Codice=ASS_VUOTO;
                         EdBoxB.Altezza=0;
                         EdBoxB.Basso=false;
                         EdBoxB.Sovrap=false;
                         EdBoxB.CarWash=0;
                         break;
     }
  }
  // FORCA ALTA
  if(NewPresCEST_A!=LIBERA){
     //-------------------------------------------------------------
     // Se la presenza della posizione è alta e non c'è nessuna
     // presenza in PRESA setto tutte le presenze in presa
     //-------------------------------------------------------------
     manca_presenza_presa=true;
     for(i=0; i<POSINPRESA; i++){
        if(EdBoxA.Presa[i][0]==false) continue;
        manca_presenza_presa=false;
        break;
     }
     //-------------------------------------------------------------
     // Se mancano le presenze, obbligo di ingresso nell'editor
     // box a darle
     //-------------------------------------------------------------
     if(manca_presenza_presa==true){
       //for(i=0; i<POSINPRESA; i++) EdBoxA.Presa[i][0]=true;
        memcpy(&B.EditBox, &EdBoxA, sizeof(BOX));
        FormBox->Tgv=NumTgv;
        FormBox->Forc=N_FORCA_A;
        FormBox->Autoclose=true;
        FormBox->ShowModal();
        if(FormBox->ModalResult==mrOk){
           memcpy(&EdBoxA, &B.EditBox, sizeof(BOX));
           //----------------------------------
           // Variazione tipo box a bordo
           //----------------------------------
           switch(EdBoxA.StatoLav){
              case  ST_BOX_LAV1: CBPresA->ItemIndex=PRES_LAV1;   break;
              case  ST_BOX_LAV2: CBPresA->ItemIndex=PRES_LAV2;   break;
              case  ST_BOX_LAV3: CBPresA->ItemIndex=PRES_LAV3;   break;
              case  ST_BOX_LAV4: CBPresA->ItemIndex=PRES_LAV4;   break;
              case  ST_BOX_LAV5: CBPresA->ItemIndex=PRES_LAV5;   break;
              case  ST_BOX_LAV6: CBPresA->ItemIndex=PRES_LAV6;   break;
              case  ST_BOX_LAV7: CBPresA->ItemIndex=PRES_LAV7;   break;
              case  ST_BOX_LAV8: CBPresA->ItemIndex=PRES_LAV8;   break;
              case  ST_BOX_VUOTI:CBPresA->ItemIndex=PRES_VUOTO;  break;
              default:           CBPresA->ItemIndex=LIBERA;      break;
           }
        }
        return;
     }
     //-------------------------------------------------------------
     // Nuova posizione del box
     //-------------------------------------------------------------
     EdBoxA.Pos = NumTgv;
     switch(NewPresCEST_A){
        case  PRES_LAV1: EdBoxA.StatoLav=ST_BOX_LAV1;
                         if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD1;
                         break;
        case  PRES_LAV2: EdBoxA.StatoLav=ST_BOX_LAV2;
                         if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD2;
                         break;
        case  PRES_LAV3: EdBoxA.StatoLav=ST_BOX_LAV3;
                         if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD3;
                         break;
        case  PRES_LAV4: EdBoxA.StatoLav=ST_BOX_LAV4;
                         if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD4;
                         break;
        case  PRES_LAV5: EdBoxA.StatoLav=ST_BOX_LAV5;
                         if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD5;
                         break;
        case  PRES_LAV6: EdBoxA.StatoLav=ST_BOX_LAV6;
                         if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD6;
                         break;
        case  PRES_LAV7: EdBoxA.StatoLav=ST_BOX_LAV7;
                         if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD7;
                         break;
        case  PRES_LAV8: EdBoxA.StatoLav=ST_BOX_LAV8;
                         if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD8;
                         break;
        case  PRES_VUOTO:EdBoxA.StatoLav=ST_BOX_VUOTI;
                         EdBoxA.Codice=ASS_VUOTO;
                         EdBoxA.Altezza=0;
                         EdBoxA.Basso=false;
                         EdBoxA.Sovrap=false;
                         EdBoxA.CarWash=0;
                         break;
     }
  }

  //------------------------------------------------
  // CONTROLLO PROVENIENZA
  //------------------------------------------------
  tipo     = (short int)(AGV[NumTgv]->mission.Provenienza/10000);
  num_prov = (short int)(AGV[NumTgv]->mission.Provenienza - (tipo*10000));
  if((NewPresCEST_A!=LIBERA || NewPresCEST_B!=LIBERA) && (tipo==NN || num_prov==NN)){
     ShowMessage(MESS[650]);
     SetCaselle();
     return;
  }
  // PROVENIENZA AZZERATA SOLO SU NUOVA MISSIONE PER MOTIVI DI OTTIMIZZAZIONE MAGAZZINO
//if(NewPresCEST_A==LIBERA && NewPresCEST_B==LIBERA) AGV[NumTgv]->mission.Provenienza=NN;
  //---------------------------------------------
  // Dopo tutti i contrlli necessari
  // aggiorno la PresMP anche al TGV
  //---------------------------------------------
  AGV[NumTgv]->mission.PresCEST_A = NewPresCEST_A;
  AGV[NumTgv]->mission.PresCEST_B = NewPresCEST_B;

  memcpy(&AGV[NumTgv]->mission.BoxA, &EdBoxA, sizeof(BOX));
  memcpy(&AGV[NumTgv]->mission.BoxB, &EdBoxB, sizeof(BOX));

  //---------------------------------------------
  // Controlo che la posizione di riposo sia
  // un carica batteria
  //---------------------------------------------
  if(AGV[NumTgv]->mission.PosRicarica<1 || AGV[NumTgv]->mission.PosRicarica>TOTPUNTI) AGV[NumTgv]->mission.PosRicarica=NumTgv;
  if(AGV[NumTgv]->mission.PosAttesa<1 || AGV[NumTgv]->mission.PosAttesa>TOTPUNTI    ) AGV[NumTgv]->mission.PosAttesa=0;

  //---------------------------------------------
  // Controllo che non sia stata assegnata a un
  // TGV una posizione già impegnata da altra
  // navetta
  //---------------------------------------------
  for(altro_agv=1; altro_agv<=MAXAGV; altro_agv++){
     if(altro_agv==NumTgv                                ) continue;
     if(AGV[NumTgv]->stato.pos<1                         ) continue;
     if(AGV[NumTgv]->stato.pos>TOTPUNTI                  ) continue;
     if(AGV[NumTgv]->stato.pos!=AGV[altro_agv]->stato.pos) continue;
     sprintf(buffer, MESS[641], altro_agv);
     Application->MessageBox( buffer, MESS[99], MB_OK | MB_ICONINFORMATION);
     return;
  }

  //----------------------------------------------
  // Salvataggio su file "mission.dat"
  //----------------------------------------------
  AGV[NumTgv]->save_mission(buffer);

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
        err = B.StructToUdcTmp(CMD_UDC_REFRESH, NumTgv, 0, 0, 0, UDC_ALL_STZ, UDC_ALL_POS, UDC_ALL_LEV, N_FORCA_AB, buffer);
        if(GestioneUdc==2 && err!=0) err=0;
        if(err) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
     // -- RESET LOCAZIONI POSIZIONI VUOTE
     if(!err){
        err = B.StructToUdcTmp(CMD_UDC_RST_LOC, NumTgv, 0, 0, 0, UDC_ALL_STZ, UDC_ALL_POS, UDC_ALL_LEV, N_FORCA_AB, buffer);
        if(GestioneUdc==2 && err!=0) err=0;
        if(err) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
  }

  //----------------------------------------------
  // Traccia su log operatore
  //----------------------------------------------
  sprintf(buffer, "(EDITTGV) - Modifica dati TGV%d", NumTgv);
  FileLogOperatore(buffer);

  //----------------------------------------------
  //  Trasmetti comando al SERVER
  //----------------------------------------------
  if( IdPC>1 ){
     if(!err){
        err = RemSincronizzaTimeFile( AGV[NumTgv]->mission_name_sorg, AGV[NumTgv]->mission_name_dest, buffer);
        if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
        sprintf( NomeFile, "MissTgv%02d", NumTgv );
        err = RemCreaFileComando ( NomeFile, (BYTE *) &AGV[NumTgv]->mission, sizeof( MISSION ), buffer );
        if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
     //----------------------------------------------
     // Aggiornamento comando di STOP MANUALE
     //----------------------------------------------
     if(!err){
        sprintf( NomeFile, "StopM%02d", NumTgv );
        err = RemCreaFileComando ( NomeFile, (BYTE *) &AGV[NumTgv]->mission.StopManuale, sizeof( AGV[NumTgv]->mission.StopManuale ), buffer );
        if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
  }
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Reset()
// ---------------------------
// Funzione che azzera gli allarmi TGV
//
void __fastcall TFormTgv::Reset()
{
  char buffer[201]="";
  int err=0;

  GetCaselle();

  if(LivelloPassword<1) return;
  err = AGV[NumTgv]->AzzeraAllarmi(buffer);
  if(err) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);

  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    Carica()
// ---------------------------
// Funzione che carica i dati direttamente del PLC.
//
void __fastcall TFormTgv::Carica()
{
  int  err;
  char buffer[201];

  //---------------------------------------
  // Rilettura stato TGV
  //---------------------------------------
  err = AGV[NumTgv]->load_stato(buffer);
  if(err) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  if(err) return;

  //---------------------------------------
  // Refresh della finestra
  //---------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------

// ---------------------------
//    GetCaselle()
// ---------------------------
// Funzione che legge dalle caselle di dialogo (TEdit) di WINDOWS
// e scarica su le variabili stringa di MAGAZZINO
//
void __fastcall TFormTgv::GetCaselle()
{
  int err;
  short int tipo, num_prov;
  short int tipo_presa;

  //------------------------------------------------
  // Box di "Dati Missione"
  //------------------------------------------------
  NewPos = (short int) atoi( EditTgv1->Text.c_str() );          // Posizione
  AGV[NumTgv]->mission.PosRicarica = (short int) atoi(EditTgv2->Text.c_str());     // Posizione riposo
  AGV[NumTgv]->mission.PosAttesa   = (short int) atoi(EditTgv3->Text.c_str());     // Posizione riposo

  //------------------------------------------------
  // RAP ENCODER
  //------------------------------------------------
  try{
     AGV[NumTgv]->mission.RapEncoder = StrToFloat(EditRap->Text);                  // Rapporto encoder
  }
  catch(...){
     ShowMessage("Error in RAP_ENCODER...");
  }

  //------------------------------------------------
  // Box di "Stato Missione"
  //------------------------------------------------
  AGV[NumTgv]->stato.start = (short int) GroupBoxTgv2->ItemIndex;                  // Stato missione

  //------------------------------------------------
  // Box di "Esclusioni Tgv"
  //------------------------------------------------
  if(BoxEsclusioneTgv1->Checked==true) AGV[NumTgv]->mission.Esclusione=true;       // Esclusione
  else                                 AGV[NumTgv]->mission.Esclusione=false;
  if(BoxEsclusioneTgv2->Checked==true) AGV[NumTgv]->mission.NoRiposoAuto=true;     // Esclusione riposo
  else                                 AGV[NumTgv]->mission.NoRiposoAuto=false;
  if(BoxEsclusioneTgv3->Checked==true) AGV[NumTgv]->mission.EsclPLC=true;          // Esclusione comunicazione
  else                                 AGV[NumTgv]->mission.EsclPLC=false;
  if(BoxEsclusioneTgv4->Checked==true) AGV[NumTgv]->mission.SemiAutomatico=true;   // Semiautomatico TGV
  else                                 AGV[NumTgv]->mission.SemiAutomatico=false;

  //------------------------------------------------
  // Box di "Bit di Stato Tgv"
  //------------------------------------------------
  if( BoxStatoTgv1->Checked==true  ) SetBit((char *) AGV[NumTgv]->sbit,  0);
  else                               ClrBit((char *) AGV[NumTgv]->sbit,  0);
  if( BoxStatoTgv2->Checked==true  ) SetBit((char *) AGV[NumTgv]->sbit,  1);
  else                               ClrBit((char *) AGV[NumTgv]->sbit,  1);
  if( BoxStatoTgv3->Checked==true  ) SetBit((char *) AGV[NumTgv]->sbit,  2);
  else                               ClrBit((char *) AGV[NumTgv]->sbit,  2);
  if( BoxStatoTgv4->Checked==true  ) SetBit((char *) AGV[NumTgv]->sbit,  3);
  else                               ClrBit((char *) AGV[NumTgv]->sbit,  3);
  if( BoxStatoTgv5->Checked==true  ) SetBit((char *) AGV[NumTgv]->sbit,  4);
  else                               ClrBit((char *) AGV[NumTgv]->sbit,  4);
  if( BoxStatoTgv6->Checked==true  ) SetBit((char *) AGV[NumTgv]->sbit,  5);
  else                               ClrBit((char *) AGV[NumTgv]->sbit,  5);
  if( BoxStatoTgv7->Checked==true  ) SetBit((char *) AGV[NumTgv]->sbit,  6);
  else                               ClrBit((char *) AGV[NumTgv]->sbit,  6);
  if( BoxStatoTgv8->Checked==true  ) SetBit((char *) AGV[NumTgv]->sbit,  7);
  else                               ClrBit((char *) AGV[NumTgv]->sbit,  7);
  if( BoxStatoTgv9->Checked==true  ) SetBit((char *) AGV[NumTgv]->sbit,  8);
  else                               ClrBit((char *) AGV[NumTgv]->sbit,  8);
  if( BoxStatoTgv10->Checked==true ) SetBit((char *) AGV[NumTgv]->sbit,  9);
  else                               ClrBit((char *) AGV[NumTgv]->sbit,  9);
  if( BoxStatoTgv11->Checked==true ) SetBit((char *) AGV[NumTgv]->sbit, 10);
  else                               ClrBit((char *) AGV[NumTgv]->sbit, 10);
  if( BoxStatoTgv12->Checked==true ) SetBit((char *) AGV[NumTgv]->sbit, 11);
  else                               ClrBit((char *) AGV[NumTgv]->sbit, 11);
  if( BoxStatoTgv13->Checked==true ) SetBit((char *) AGV[NumTgv]->sbit, 12);
  else                               ClrBit((char *) AGV[NumTgv]->sbit, 12);
  if( BoxStatoTgv14->Checked==true ) SetBit((char *) AGV[NumTgv]->sbit, 13);
  else                               ClrBit((char *) AGV[NumTgv]->sbit, 13);
  if( BoxStatoTgv15->Checked==true ) SetBit((char *) AGV[NumTgv]->sbit, 14);
  else                               ClrBit((char *) AGV[NumTgv]->sbit, 14);
  if( BoxStatoTgv16->Checked==true ) SetBit((char *) AGV[NumTgv]->sbit, 15);
  else                               ClrBit((char *) AGV[NumTgv]->sbit, 15);
  if( BoxStatoTgv17->Checked==true ) SetBit((char *) AGV[NumTgv]->sbit, 16);
  else                               ClrBit((char *) AGV[NumTgv]->sbit, 16);
  if( BoxStatoTgv18->Checked==true ) SetBit((char *) AGV[NumTgv]->sbit, 17);
  else                               ClrBit((char *) AGV[NumTgv]->sbit, 17);
  if( BoxStatoTgv19->Checked==true ) SetBit((char *) AGV[NumTgv]->sbit, 18);
  else                               ClrBit((char *) AGV[NumTgv]->sbit, 18);
  if( BoxStatoTgv20->Checked==true ) SetBit((char *) AGV[NumTgv]->sbit, 19);
  else                               ClrBit((char *) AGV[NumTgv]->sbit, 19);

  //------------------------------------------------
  // GESTIONE_ASRV (bit di stato)
  //------------------------------------------------
  AGV[NumTgv]->asrv_stato.sbit1.mov_X   = BoxStatoAsrv1->Checked;
  AGV[NumTgv]->asrv_stato.sbit1.mov_Y   = BoxStatoAsrv2->Checked;
  AGV[NumTgv]->asrv_stato.sbit1.mov_av  = BoxStatoAsrv3->Checked;
  AGV[NumTgv]->asrv_stato.sbit1.mov_ind = BoxStatoAsrv4->Checked;
  AGV[NumTgv]->asrv_stato.sbit1.allarme = BoxStatoAsrv5->Checked;
  AGV[NumTgv]->asrv_stato.sbit1.ok_prog = BoxStatoAsrv6->Checked;
  AGV[NumTgv]->asrv_stato.sbit1.in_auto = BoxStatoAsrv7->Checked;
  AGV[NumTgv]->asrv_stato.sbit1.in_man  = BoxStatoAsrv8->Checked;
  AGV[NumTgv]->asrv_stato.sbit1.su_nod  = BoxStatoAsrv9->Checked;
  AGV[NumTgv]->asrv_stato.sbit1.carico  = BoxStatoAsrv10->Checked;
  AGV[NumTgv]->asrv_stato.sbit1.sollev0 = BoxStatoAsrv11->Checked;
  AGV[NumTgv]->asrv_stato.sbit1.sollev1 = BoxStatoAsrv12->Checked;
  AGV[NumTgv]->asrv_stato.sbit1.ric_rst = BoxStatoAsrv13->Checked;
  AGV[NumTgv]->asrv_stato.sbit1.aux13   = BoxStatoAsrv14->Checked;
  AGV[NumTgv]->asrv_stato.sbit1.aux14   = BoxStatoAsrv15->Checked;
  AGV[NumTgv]->asrv_stato.sbit1.aux15   = BoxStatoAsrv16->Checked;

  AGV[NumTgv]->asrv_stato.sbit2.stop_X  = BoxStatoAsrv17->Checked;
  AGV[NumTgv]->asrv_stato.sbit2.stop_Y  = BoxStatoAsrv18->Checked;
  AGV[NumTgv]->asrv_stato.sbit2.rot_X   = BoxStatoAsrv19->Checked;
  AGV[NumTgv]->asrv_stato.sbit2.rot_Y   = BoxStatoAsrv20->Checked;
  AGV[NumTgv]->asrv_stato.sbit2.ricarica= BoxStatoAsrv21->Checked;
  AGV[NumTgv]->asrv_stato.sbit2.cb_batt = BoxStatoAsrv22->Checked;
  AGV[NumTgv]->asrv_stato.sbit2.manutenz= BoxStatoAsrv23->Checked;
  AGV[NumTgv]->asrv_stato.sbit2.aux7    = BoxStatoAsrv24->Checked;
  AGV[NumTgv]->asrv_stato.sbit2.aux8    = BoxStatoAsrv25->Checked;
  AGV[NumTgv]->asrv_stato.sbit2.aux9    = BoxStatoAsrv26->Checked;
  AGV[NumTgv]->asrv_stato.sbit2.aux10   = BoxStatoAsrv27->Checked;
  AGV[NumTgv]->asrv_stato.sbit2.aux11   = BoxStatoAsrv28->Checked;

  //------------------------------------------------
  // Box di "Provenienza sul Carretto/Carrello"
  //------------------------------------------------
  tipo     = (short int) GroupBoxTgv5->ItemIndex; // provenienza
  num_prov = (short int) atoi(EdProv->Text.c_str());
  if(tipo==PROV_MACCHINE  && num_prov>0 && num_prov<=TOTCHIAMATE ) AGV[NumTgv]->mission.Provenienza = (int)((10000*PROV_MACCHINE) +num_prov);
  if(tipo==PROV_MAGAZZINO && num_prov>0 && num_prov<=TOTMAG      ) AGV[NumTgv]->mission.Provenienza = (int)((10000*PROV_MAGAZZINO)+num_prov);
  if(tipo==PROV_BAIE      && num_prov>0 && num_prov<=TOTBAIE     ) AGV[NumTgv]->mission.Provenienza = (int)((10000*PROV_BAIE)+num_prov);

  //------------------------------------------------
  // Box di "Informazioni sul Carretto/Carrello"
  //------------------------------------------------
  NewPresCEST_A = (short int)(CBPresA->ItemIndex);
  NewPresCEST_B = (short int)(CBPresB->ItemIndex);

  //------------------------------------------
  // TGV A SINGOLA FORCA
  // Forzo a "0" la presenza sulla forca alta
  //------------------------------------------
  if(GestioneDoppiaForcaTGV==0) NewPresCEST_A = LIBERA;
}
//---------------------------------------------------------------------------

// ---------------------------
//    SetCaselle()
// ---------------------------
// Funzione che forza i nuovi valori nelle caselle di dialogo (TEdit)
// di WINDOWS recuperandoli dalle variabili stringa di MAGAZZINO
//
void __fastcall TFormTgv::SetCaselle()
{
  int  i;
  char buffer[11]="";
  char Appoggio[101]="";
  short int tipo, num_baia, num_mac, num_mag;

  //------------------------------------------------
  // Box di "Dati Missione"
  //------------------------------------------------
  OldPos=AGV[NumTgv]->stato.pos;
  NewPos=AGV[NumTgv]->stato.pos;

  OldPresCEST_A=AGV[NumTgv]->mission.PresCEST_A;
  NewPresCEST_A=AGV[NumTgv]->mission.PresCEST_A;

  OldPresCEST_B=AGV[NumTgv]->mission.PresCEST_B;
  NewPresCEST_B=AGV[NumTgv]->mission.PresCEST_B;

  sprintf(Appoggio, "%04d", AGV[NumTgv]->stato.pos);
  EditTgv1->Text = Appoggio;                                                          // Posizione
  sprintf(Appoggio, "%04d", AGV[NumTgv]->mission.PosRicarica);
  EditTgv2->Text = Appoggio;                                                          // Posizione ricarica
  sprintf(Appoggio, "%04d", AGV[NumTgv]->mission.PosAttesa);
  EditTgv3->Text = Appoggio;                                                          // Posizione di attesa

  sprintf(Appoggio, MESS[484],  AGV[NumTgv]->stato.ptr_step);                         // Il Prg parte da DM1401
  TextTgv4->Caption = Appoggio;
  sprintf(Appoggio, MESS[485],  AGV[NumTgv]->stato.offset_da_nodo);                   // Il Prg parte da DM99
  TextTgv5->Caption = Appoggio;

  if(AGV[NumTgv]->mission.GestioneLaser==false){
     sprintf(buffer, "%d°", AGV[NumTgv]->stato.rot_calamite);                         // Gradi di rotazione con calamite
  }
  else{
     sprintf(buffer, "%d°", AGV[NumTgv]->stato.rot_laser);                            // Gradi di rotazione laser
  }

  TextGrd->Caption = buffer;
  //------------------------------------------------
  // Flag consensi
  //------------------------------------------------
  strcpy(Appoggio, MESS[486]);
  for(i=1; i<MAXPORTEIMP; i++){
     if(AGV[NumTgv]->mission.PorInIngombro[i]==false) continue;
     sprintf(buffer, "%d.", i);
     strcat(Appoggio, buffer);
  }
  TextTgv6->Caption = Appoggio;
  strcpy(Appoggio, "");
  if(AGV[NumTgv]->mission.MacInIngombro!=0 ) sprintf(Appoggio, "Mac:%d", AGV[NumTgv]->mission.MacInIngombro);
  if(AGV[NumTgv]->mission.BaiaInIngombro!=0) sprintf(Appoggio, "Bai:%d", AGV[NumTgv]->mission.BaiaInIngombro);
  TextTgv7->Caption = Appoggio;

  //-----------------------------------------------------
  // Visualizzazione destinazione bloccati
  //-----------------------------------------------------
  TextBloccati->Caption = "UNKNOWN";

  //-----------------------------------------------------
  // Forzatura dello stop del TGV su un punto intermedio
  //-----------------------------------------------------
  BoxStopTgv->Checked = AGV[NumTgv]->mission.StopManuale;           // Stop manuale TGV

  //------------------------------------------------
  // RAPPORTO ENCODER
  //------------------------------------------------
  EditRap->Text = AGV[NumTgv]->mission.RapEncoder;

  //------------------------------------------------
  // Box di "Stato Missione"
  //------------------------------------------------
  GroupBoxTgv2->ItemIndex = AGV[NumTgv]->stato.start;               // Stato missione

  //------------------------------------------------
  // Box di "Abilitazioni Tgv"
  //------------------------------------------------
  if(AGV[NumTgv]->mission.Esclusione==true)     BoxEsclusioneTgv1->Checked=true;      // Esclusione
  else                                          BoxEsclusioneTgv1->Checked=false;
  if(AGV[NumTgv]->mission.NoRiposoAuto==true)   BoxEsclusioneTgv2->Checked=true;      // Esclusione riposo
  else                                          BoxEsclusioneTgv2->Checked=false;
  if(AGV[NumTgv]->mission.EsclPLC==true)        BoxEsclusioneTgv3->Checked=true;      // Esclusione comunicazione
  else                                          BoxEsclusioneTgv3->Checked=false;
  if(AGV[NumTgv]->mission.SemiAutomatico==true) BoxEsclusioneTgv4->Checked=true;      // Semiautomatico TGV
  else                                          BoxEsclusioneTgv4->Checked=false;

  //------------------------------------------------
  // Box di "Bit di Stato Tgv"
  //------------------------------------------------
  if(TestBit((char *) AGV[NumTgv]->sbit,   0))  BoxStatoTgv1->Checked=true;
  else                                          BoxStatoTgv1->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,   1))  BoxStatoTgv2->Checked=true;
  else                                          BoxStatoTgv2->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,   2))  BoxStatoTgv3->Checked=true;
  else                                          BoxStatoTgv3->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,   3))  BoxStatoTgv4->Checked=true;
  else                                          BoxStatoTgv4->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,   4))  BoxStatoTgv5->Checked=true;
  else                                          BoxStatoTgv5->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,   5))  BoxStatoTgv6->Checked=true;
  else                                          BoxStatoTgv6->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,   6))  BoxStatoTgv7->Checked=true;
  else                                          BoxStatoTgv7->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,   7))  BoxStatoTgv8->Checked=true;
  else                                          BoxStatoTgv8->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,   8))  BoxStatoTgv9->Checked=true;
  else                                          BoxStatoTgv9->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,   9))  BoxStatoTgv10->Checked=true;
  else                                          BoxStatoTgv10->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,  10))  BoxStatoTgv11->Checked=true;
  else                                          BoxStatoTgv11->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,  11))  BoxStatoTgv12->Checked=true;
  else                                          BoxStatoTgv12->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,  12))  BoxStatoTgv13->Checked=true;
  else                                          BoxStatoTgv13->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,  13))  BoxStatoTgv14->Checked=true;
  else                                          BoxStatoTgv14->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,  14))  BoxStatoTgv15->Checked=true;
  else                                          BoxStatoTgv15->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,  15))  BoxStatoTgv16->Checked=true;
  else                                          BoxStatoTgv16->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,  16))  BoxStatoTgv17->Checked=true;
  else                                          BoxStatoTgv17->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,  17))  BoxStatoTgv18->Checked=true;
  else                                          BoxStatoTgv18->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,  18))  BoxStatoTgv19->Checked=true;
  else                                          BoxStatoTgv19->Checked=false;
  if(TestBit((char *) AGV[NumTgv]->sbit,  19))  BoxStatoTgv20->Checked=true;
  else                                          BoxStatoTgv20->Checked=false;

  //------------------------------------------------
  // GESTIONE_ASRV (bit di stato)
  //------------------------------------------------
  BoxStatoAsrv1->Checked  = AGV[NumTgv]->asrv_stato.sbit1.mov_X;
  BoxStatoAsrv2->Checked  = AGV[NumTgv]->asrv_stato.sbit1.mov_Y;
  BoxStatoAsrv3->Checked  = AGV[NumTgv]->asrv_stato.sbit1.mov_av;
  BoxStatoAsrv4->Checked  = AGV[NumTgv]->asrv_stato.sbit1.mov_ind;
  BoxStatoAsrv5->Checked  = AGV[NumTgv]->asrv_stato.sbit1.allarme;
  BoxStatoAsrv6->Checked  = AGV[NumTgv]->asrv_stato.sbit1.ok_prog;
  BoxStatoAsrv7->Checked  = AGV[NumTgv]->asrv_stato.sbit1.in_auto;
  BoxStatoAsrv8->Checked  = AGV[NumTgv]->asrv_stato.sbit1.in_man;
  BoxStatoAsrv9->Checked  = AGV[NumTgv]->asrv_stato.sbit1.su_nod;
  BoxStatoAsrv10->Checked = AGV[NumTgv]->asrv_stato.sbit1.carico;
  BoxStatoAsrv11->Checked = AGV[NumTgv]->asrv_stato.sbit1.sollev0;
  BoxStatoAsrv12->Checked = AGV[NumTgv]->asrv_stato.sbit1.sollev1;
  BoxStatoAsrv13->Checked = AGV[NumTgv]->asrv_stato.sbit1.ric_rst;
  BoxStatoAsrv14->Checked = AGV[NumTgv]->asrv_stato.sbit1.aux13;
  BoxStatoAsrv15->Checked = AGV[NumTgv]->asrv_stato.sbit1.aux14;
  BoxStatoAsrv16->Checked = AGV[NumTgv]->asrv_stato.sbit1.aux15;

  BoxStatoAsrv17->Checked = AGV[NumTgv]->asrv_stato.sbit2.stop_X;
  BoxStatoAsrv18->Checked = AGV[NumTgv]->asrv_stato.sbit2.stop_Y;
  BoxStatoAsrv19->Checked = AGV[NumTgv]->asrv_stato.sbit2.rot_X;
  BoxStatoAsrv20->Checked = AGV[NumTgv]->asrv_stato.sbit2.rot_Y;
  BoxStatoAsrv21->Checked = AGV[NumTgv]->asrv_stato.sbit2.ricarica;
  BoxStatoAsrv22->Checked = AGV[NumTgv]->asrv_stato.sbit2.cb_batt;
  BoxStatoAsrv23->Checked = AGV[NumTgv]->asrv_stato.sbit2.manutenz;
  BoxStatoAsrv24->Checked = AGV[NumTgv]->asrv_stato.sbit2.aux7;
  BoxStatoAsrv25->Checked = AGV[NumTgv]->asrv_stato.sbit2.aux8;
  BoxStatoAsrv26->Checked = AGV[NumTgv]->asrv_stato.sbit2.aux9;
  BoxStatoAsrv27->Checked = AGV[NumTgv]->asrv_stato.sbit2.aux10;
  BoxStatoAsrv28->Checked = AGV[NumTgv]->asrv_stato.sbit2.aux11;


  //-------------------------------------------
  // Decodifica provenienza
  //-------------------------------------------
  tipo=num_baia=num_mac=num_mag=0;
  P.TestProvenienzaBox(AGV[NumTgv]->mission.Provenienza, &tipo, &num_mac, &num_baia, &num_mag);

  GroupBoxTgv5->ItemIndex = tipo;                // provenienza
  if(tipo>NN){
     if(num_mac!=0 ) sprintf(Appoggio, "%04d", num_mac);
     if(num_mag!=0 ) sprintf(Appoggio, "%04d", num_mag);
     if(num_baia!=0) sprintf(Appoggio, "%04d", num_baia);
     EdProv->Text = Appoggio;
  }
  else EdProv->Text = "0000";

  //------------------------------------------------
  // Box di "Presenza Carretto/Carrello"
  //------------------------------------------------
  CBPresA->ItemIndex      = AGV[NumTgv]->mission.PresCEST_A;
  CBPresB->ItemIndex      = AGV[NumTgv]->mission.PresCEST_B;
  //-------------------------------------------
  // Abilitazione postazioni con presenza box
  //-------------------------------------------
  if(CBPresA->ItemIndex!=LIBERA) BtnBoxA->Enabled = true;
  else                           BtnBoxA->Enabled = false;
  if(CBPresB->ItemIndex!=LIBERA) BtnBoxB->Enabled = true;
  else                           BtnBoxB->Enabled = false;

  //------------------------------------------
  // Colorazione combo di PRESENZA
  //------------------------------------------
  // FORCA "B"
  switch(NewPresCEST_B){
     case  PRES_LAV1:    CBPresB->Color = (TColor)(COLOR_PRES_STAT1);  break;
     case  PRES_LAV2:    CBPresB->Color = (TColor)(COLOR_PRES_STAT2);  break;
     case  PRES_LAV3:    CBPresB->Color = (TColor)(COLOR_PRES_STAT3);  break;
     case  PRES_LAV4:    CBPresB->Color = (TColor)(COLOR_PRES_STAT4);  break;
     case  PRES_LAV5:    CBPresB->Color = (TColor)(COLOR_PRES_STAT5);  break;
     case  PRES_LAV6:    CBPresB->Color = (TColor)(COLOR_PRES_STAT6);  break;
     case  PRES_LAV7:    CBPresB->Color = (TColor)(COLOR_PRES_STAT7);  break;
     case  PRES_LAV8:    CBPresB->Color = (TColor)(COLOR_PRES_STAT8);  break;
     case  PRES_VUOTO:   CBPresB->Color = (TColor)(COLOR_PRES_VUOTI);  break;
     default:            CBPresB->Color = clSkyBlue;                   break;
  }
  // FORCA "A"
  switch(NewPresCEST_A){
     case  PRES_LAV1:    CBPresA->Color = (TColor)(COLOR_PRES_STAT1);  break;
     case  PRES_LAV2:    CBPresA->Color = (TColor)(COLOR_PRES_STAT2);  break;
     case  PRES_LAV3:    CBPresA->Color = (TColor)(COLOR_PRES_STAT3);  break;
     case  PRES_LAV4:    CBPresA->Color = (TColor)(COLOR_PRES_STAT4);  break;
     case  PRES_LAV5:    CBPresA->Color = (TColor)(COLOR_PRES_STAT5);  break;
     case  PRES_LAV6:    CBPresA->Color = (TColor)(COLOR_PRES_STAT6);  break;
     case  PRES_LAV7:    CBPresA->Color = (TColor)(COLOR_PRES_STAT7);  break;
     case  PRES_LAV8:    CBPresA->Color = (TColor)(COLOR_PRES_STAT8);  break;
     case  PRES_VUOTO:   CBPresA->Color = (TColor)(COLOR_PRES_VUOTI);  break;
     default:            CBPresA->Color = clSkyBlue;                   break;
  }
  //----------------------------------
  sprintf(buffer, "Tgv:%02d",NumTgv);
  txtTgv->Caption = buffer;
}
//---------------------------------------------------------------------------
void __fastcall TFormTgv::BtnBoxAClick(TObject *Sender)
{
  //------------------------------------------------
  // Apertura della finesta di assegnazione dati
  //------------------------------------------------
  switch(CBPresA->ItemIndex){
     case PRES_LAV1: EdBoxA.StatoLav=ST_BOX_LAV1;
                     if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD1;
                     break;
     case PRES_LAV2: EdBoxA.StatoLav=ST_BOX_LAV2;
                     if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD2;
                     break;
     case PRES_LAV3: EdBoxA.StatoLav=ST_BOX_LAV3;
                     if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD3;
                     break;
     case PRES_LAV4: EdBoxA.StatoLav=ST_BOX_LAV4;
                     if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD4;
                     break;
     case PRES_LAV5: EdBoxA.StatoLav=ST_BOX_LAV5;
                     if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD5;
                     break;
     case PRES_LAV6: EdBoxA.StatoLav=ST_BOX_LAV6;
                     if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD6;
                     break;
     case PRES_LAV7: EdBoxA.StatoLav=ST_BOX_LAV7;
                     if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD7;
                     break;
     case PRES_LAV8: EdBoxA.StatoLav=ST_BOX_LAV8;
                     if(AbbinaCodiceStatoBox==true) EdBoxA.Codice=ASS_COD8;
                     break;
     case PRES_VUOTO:EdBoxA.StatoLav=ST_BOX_VUOTI;
                     EdBoxA.Codice=ASS_VUOTO;
                     break;
  }
  // appoggio struttura editor box
  memcpy(&B.EditBox, &EdBoxA, sizeof(BOX));
  FormBox->Tgv=NumTgv;
  FormBox->Forc=N_FORCA_A;
  FormBox->Autoclose=true;
  FormBox->ShowModal();
  if(FormBox->ModalResult==mrOk){
     memcpy(&EdBoxA, &B.EditBox, sizeof(BOX));
     //----------------------------------
     // Variazione tipo box a bordo
     //----------------------------------
     switch(EdBoxA.StatoLav){
        case  ST_BOX_LAV1: CBPresA->ItemIndex=PRES_LAV1;   break;
        case  ST_BOX_LAV2: CBPresA->ItemIndex=PRES_LAV2;   break;
        case  ST_BOX_LAV3: CBPresA->ItemIndex=PRES_LAV3;   break;
        case  ST_BOX_LAV4: CBPresA->ItemIndex=PRES_LAV4;   break;
        case  ST_BOX_LAV5: CBPresA->ItemIndex=PRES_LAV5;   break;
        case  ST_BOX_LAV6: CBPresA->ItemIndex=PRES_LAV6;   break;
        case  ST_BOX_LAV7: CBPresA->ItemIndex=PRES_LAV7;   break;
        case  ST_BOX_LAV8: CBPresA->ItemIndex=PRES_LAV8;   break;
        case  ST_BOX_VUOTI:CBPresA->ItemIndex=PRES_VUOTO;  break;
        default:           CBPresA->ItemIndex=LIBERA;      break;
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormTgv::BtnBoxBClick(TObject *Sender)
{
  //------------------------------------------------
  // GESTIONE ARCHIVIO BOX
  //------------------------------------------------
  switch(CBPresB->ItemIndex){
     case PRES_LAV1: EdBoxB.StatoLav=ST_BOX_LAV1;
                     if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD1;
                     break;
     case PRES_LAV2: EdBoxB.StatoLav=ST_BOX_LAV2;
                     if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD2;
                     break;
     case PRES_LAV3: EdBoxB.StatoLav=ST_BOX_LAV3;
                     if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD3;
                     break;
     case PRES_LAV4: EdBoxB.StatoLav=ST_BOX_LAV4;
                     if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD4;
                     break;
     case PRES_LAV5: EdBoxB.StatoLav=ST_BOX_LAV5;
                     if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD5;
                     break;
     case PRES_LAV6: EdBoxB.StatoLav=ST_BOX_LAV6;
                     if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD6;
                     break;
     case PRES_LAV7: EdBoxB.StatoLav=ST_BOX_LAV7;
                     if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD7;
                     break;
     case PRES_LAV8: EdBoxB.StatoLav=ST_BOX_LAV8;
                     if(AbbinaCodiceStatoBox==true) EdBoxB.Codice=ASS_COD8;
                     break;
     case PRES_VUOTO:EdBoxB.StatoLav=ST_BOX_VUOTI;
                     EdBoxB.Codice=ASS_VUOTO;
                     break;
  }
  // appogggio struttura editor box
  memcpy(&B.EditBox, &EdBoxB, sizeof(BOX));
  FormBox->Tgv=NumTgv;
  FormBox->Forc=N_FORCA_B;
  FormBox->Autoclose=true;
  FormBox->ShowModal();
  if(FormBox->ModalResult==mrOk){
     memcpy(&EdBoxB, &B.EditBox, sizeof(BOX));
     //----------------------------------
     // Variazione tipo box a bordo
     //----------------------------------
     switch(EdBoxB.StatoLav){
        case  ST_BOX_LAV1: CBPresB->ItemIndex=PRES_LAV1;   break;
        case  ST_BOX_LAV2: CBPresB->ItemIndex=PRES_LAV2;   break;
        case  ST_BOX_LAV3: CBPresB->ItemIndex=PRES_LAV3;   break;
        case  ST_BOX_LAV4: CBPresB->ItemIndex=PRES_LAV4;   break;
        case  ST_BOX_LAV5: CBPresB->ItemIndex=PRES_LAV5;   break;
        case  ST_BOX_LAV6: CBPresB->ItemIndex=PRES_LAV6;   break;
        case  ST_BOX_LAV7: CBPresB->ItemIndex=PRES_LAV7;   break;
        case  ST_BOX_LAV8: CBPresB->ItemIndex=PRES_LAV8;   break;
        case  ST_BOX_VUOTI:CBPresB->ItemIndex=PRES_VUOTO;  break;
        default:           CBPresB->ItemIndex=LIBERA;      break;
     }
  }
}
//---------------------------------------------------------------------------
// +--------------------------------------------------------+
// | Azzeramento Allarmi del TGV                            |
// +--------------------------------------------------------+
void __fastcall TFormTgv::BtnResetClick(TObject *Sender)
{
  Reset();
}
//---------------------------------------------------------------------------
// +--------------------------------------------------------+
// | Salvataggio dati su file "mission.dat"                 |
// +--------------------------------------------------------+
void __fastcall TFormTgv::BtnSalvaClick(TObject *Sender)
{
  Salva();
}
//---------------------------------------------------------------------------

// +--------------------------------------------------------+
// | Recupero dati direttamente dal PLC                     |
// +--------------------------------------------------------+
void __fastcall TFormTgv::BtnCaricaClick(TObject *Sender)
{
  Carica();
}
//---------------------------------------------------------------------------
// +--------------------------------------------------------+
// | Impostazione Asservimenti TGV                          |
// +--------------------------------------------------------+
void __fastcall TFormTgv::BtnAsservimentoClick(TObject *Sender)
{
  if(LivelloPassword<2) return;
  //-------------------------------------------------
  // Apertura form di gestione Asservimento
  //-------------------------------------------------
  FormAsservi->NumTgv=NumTgv;
  FormAsservi->ShowModal();
}
//---------------------------------------------------------------------------
// +--------------------------------------------------------+
// | Impostazioni Aree di asservimento del TGV              |
// +--------------------------------------------------------+
void __fastcall TFormTgv::BoxStopTgvClick(TObject *Sender)
{
  int  i;
  char buffer[101]="";
  bool tgv_su_deviazione;

  //-------------------------------------------------
  // Cotrollo che il TGV non abbia già il percorso
  // occupato fino a destinazione
  //-------------------------------------------------
  if(AGV[NumTgv]->mission.pintend==AGV[NumTgv]->mission.pend) return;
  if(AGV[NumTgv]->stato.pos==AGV[NumTgv]->mission.pend      ) return;
  //-------------------------------------------------
  // Non sono ammessi STOP su start e end deviazioni
  // o curve
  //-------------------------------------------------
  tgv_su_deviazione=false;
  for(i=0; i<MAXPERCORSI; i++){
     if(AGV[NumeroTgv]->mission.punto[i]==0                               ) break;
     if(AGV[NumeroTgv]->mission.punto[i]!=AGV[NumeroTgv]->mission.pintend ) continue;
     if(AGV[NumeroTgv]->mission.dati_perc[i].rot<4                        ) continue;
     tgv_su_deviazione=true;
     break;
  }
  if(tgv_su_deviazione==true) return;
  //-------------------------------------------------
  // Modifica flag direttamente in struttura
  //-------------------------------------------------
  AGV[NumTgv]->mission.StopManuale = BoxStopTgv->Checked;
  //-------------------------------------------------
  // Registrazione su LOG OPERATORE dell'operazione
  // eseguita
  //-------------------------------------------------
  if( AGV[NumTgv]->mission.StopManuale) sprintf(buffer, "(STOP   ) - Set Stop Manuale TGV%d", NumTgv);
  if(!AGV[NumTgv]->mission.StopManuale) sprintf(buffer, "(STOP   ) - Reset Stop Manuale TGV%d", NumTgv);
  FileLogOperatore(buffer);
}
//---------------------------------------------------------------------------
void __fastcall TFormTgv::BtnMappingClick(TObject *Sender)
{
  //-----------------------------------
  // Se non password al livello 3
  //-----------------------------------
  if(LivelloPassword<3) return;
  //-----------------------------------
  // Apertura della Form di mappa
  //-----------------------------------
  FormMappa->AutoSend=false;
  FormMappa->NumTgv=NumeroTgv;
  FormMappa->ShowModal();
  //-----------------------------------
  // Rinfresco della form
  //-----------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormTgv::BtnColoreClick(TObject *Sender)
{
  if(LivelloPassword<2) return;
  //--------------------------------------------------
  // Recupero del Colore selezionato
  //--------------------------------------------------
  if(ColorDialog1->Execute()==mrOk){
     AGV[NumTgv]->mission.Col = ColorDialog1->Color;
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormTgv::TextGrdDblClick(TObject *Sender)
{
  int  new_rot;
  char buffer[101]="";
  AnsiString InputStringa;

  //-------------------------------------------
  // Solo con password di livello 3
  //-------------------------------------------
  if(LivelloPassword<3) return;
  //-------------------------------------------
  // Solo con comunicazioni disabilitate
  //-------------------------------------------
  if(!break_Com[COM1] && !AGV[NumTgv]->mission.EsclPLC) return;
  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  if(AGV[NumTgv]->mission.GestioneLaser==false) new_rot = AGV[NumTgv]->stato.rot_calamite;
  else                                          new_rot = AGV[NumTgv]->stato.rot_laser;
  //-----------------------------------------------
  // Inserimento nuova rotazione da InputBox
  //-----------------------------------------------
  InputStringa = InputBox("TGV's rotation", "Insert new rotation:", new_rot);
  if(InputStringa.IsEmpty() == true) return;
  //-----------------------------------------------
  // Verifico valore rotazione
  //-----------------------------------------------
  new_rot = StrToInt(InputStringa);
  if(new_rot<0 || new_rot>360) new_rot=0;
  //-----------------------------------------------
  // Aggiornamento stato rotazione
  //-----------------------------------------------
  if(AGV[NumTgv]->mission.GestioneLaser==false){
     AGV[NumTgv]->stato.rot_calamite = (short int)(new_rot);
     sprintf(buffer, "%d°", AGV[NumTgv]->stato.rot_calamite);          // Gradi di rotazione con calamite
  }
  else{
     AGV[NumTgv]->stato.rot_laser    = (short int)(new_rot);
     sprintf(buffer, "%d°", AGV[NumTgv]->stato.rot_laser);             // Gradi di rotazione laser
  }
  //-----------------------------------------------
  // Rinfresco dato visualizzato
  //-----------------------------------------------
  TextGrd->Caption = buffer;
}
//---------------------------------------------------------------------------

void __fastcall TFormTgv::OffDaNodoDblClick(TObject *Sender)
{
  int new_offset;
  char buffer[101]="";
  AnsiString InputStringa;

  //-------------------------------------------
  // Solo con password di livello 3
  //-------------------------------------------
  if(LivelloPassword<3) return;
  //-------------------------------------------
  // Solo con comunicazioni disabilitate
  //-------------------------------------------
  if(!break_Com[COM1] && !AGV[NumTgv]->mission.EsclPLC) return;
  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  InputStringa = InputBox("TGV's offset da nodo", "Insert new offset:", AGV[NumTgv]->stato.offset_da_nodo);
  if(InputStringa.IsEmpty() == true) return;
  //-----------------------------------------------
  // Verifico valore rotazione
  //-----------------------------------------------
  new_offset = StrToInt(InputStringa);
//if(new_rot<0 || new_rot>360) new_rot=0;
  AGV[NumTgv]->stato.offset_da_nodo = (short int)(new_offset);
  //-----------------------------------------------
  // Rinfresco dato visualizzato
  //-----------------------------------------------
  sprintf(buffer, MESS[485],  AGV[NumTgv]->stato.offset_da_nodo);                   // Il Prg parte da DM99
  TextTgv5->Caption = buffer;
}
//---------------------------------------------------------------------------

void __fastcall TFormTgv::CBPresBChange(TObject *Sender)
{
  char buffer[101]="";
  bool assegnazione_not_ok;

  //------------------------------------------------
  // Gestione Editor ID PALLET
  //------------------------------------------------
  if(CBPresB->ItemIndex!=LIBERA) BtnBoxB->Enabled = true;
  else                           BtnBoxB->Enabled = false;
  //------------------------------------------------
  // Gestione assegnazione
  //------------------------------------------------
  assegnazione_not_ok=false;
  switch(CBPresB->ItemIndex){
     case  PRES_LAV1:   CBPresB->Color = (TColor)(COLOR_PRES_STAT2);
                        strcpy(buffer, ASS_LAV1);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD1);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_LAV2:   CBPresB->Color = (TColor)(COLOR_PRES_STAT2);
                        strcpy(buffer, ASS_LAV2);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD2);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_LAV3:   CBPresB->Color = (TColor)(COLOR_PRES_STAT3);
                        strcpy(buffer, ASS_LAV3);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD3);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_LAV4:   CBPresB->Color = (TColor)(COLOR_PRES_STAT4);
                        strcpy(buffer, ASS_LAV4);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD4);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_LAV5:   CBPresB->Color = (TColor)(COLOR_PRES_STAT5);
                        strcpy(buffer, ASS_LAV5);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD5);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_LAV6:   CBPresB->Color = (TColor)(COLOR_PRES_STAT6);
                        strcpy(buffer, ASS_LAV6);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD6);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_LAV7:   CBPresB->Color = (TColor)(COLOR_PRES_STAT7);
                        strcpy(buffer, ASS_LAV7);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD7);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_LAV8:   CBPresB->Color = (TColor)(COLOR_PRES_STAT8);
                        strcpy(buffer, ASS_LAV8);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD8);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_VUOTO:  CBPresB->Color = (TColor)(COLOR_PRES_VUOTI); break;
     default:           CBPresB->Color = clSkyBlue;                  break;
  }
  //------------------------------------------------
  // Se l'assegnazione non è corretta assegno
  // quella iniziale
  //------------------------------------------------
  if(assegnazione_not_ok==true){
     strcpy(buffer, "Please put a correct code assignment!");
     Application->MessageBox(buffer, MESS[99], MB_OK | MB_ICONINFORMATION);
     CBPresB->ItemIndex  = NewPresCEST_B;
     switch(CBPresB->ItemIndex){
        case  PRES_LAV1:  CBPresB->Color = (TColor)(COLOR_PRES_STAT1);
                          break;
        case  PRES_LAV2:  CBPresB->Color = (TColor)(COLOR_PRES_STAT2);
                          break;
        case  PRES_LAV3:  CBPresB->Color = (TColor)(COLOR_PRES_STAT3);
                          break;
        case  PRES_LAV4:  CBPresB->Color = (TColor)(COLOR_PRES_STAT4);
                          break;
        case  PRES_LAV5:  CBPresB->Color = (TColor)(COLOR_PRES_STAT5);
                          break;
        case  PRES_LAV6:  CBPresB->Color = (TColor)(COLOR_PRES_STAT6);
                          break;
        case  PRES_LAV7:  CBPresB->Color = (TColor)(COLOR_PRES_STAT7);
                          break;
        case  PRES_LAV8:  CBPresB->Color = (TColor)(COLOR_PRES_STAT8);
                          break;
        case  PRES_VUOTO: CBPresB->Color = (TColor)(COLOR_PRES_VUOTI);
                          break;
               default:   CBPresB->Color = clSkyBlue;
                          break;
     }
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormTgv::CBPresAChange(TObject *Sender)
{
  char buffer[101]="";
  bool assegnazione_not_ok;

  //------------------------------------------------
  // Gestione Editor ID PALLET
  //------------------------------------------------
  if(CBPresA->ItemIndex!=LIBERA) BtnBoxA->Enabled = true;
  else                           BtnBoxA->Enabled = false;
  //------------------------------------------------
  // Gestione assegnazione
  //------------------------------------------------
  assegnazione_not_ok=false;
  switch(CBPresA->ItemIndex){
     case  PRES_LAV1:   CBPresA->Color = (TColor)(COLOR_PRES_STAT2);
                        strcpy(buffer, ASS_LAV1);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD1);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_LAV2:   CBPresA->Color = (TColor)(COLOR_PRES_STAT2);
                        strcpy(buffer, ASS_LAV2);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD2);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_LAV3:   CBPresA->Color = (TColor)(COLOR_PRES_STAT3);
                        strcpy(buffer, ASS_LAV3);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD3);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_LAV4:   CBPresA->Color = (TColor)(COLOR_PRES_STAT4);
                        strcpy(buffer, ASS_LAV4);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD4);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_LAV5:   CBPresA->Color = (TColor)(COLOR_PRES_STAT5);
                        strcpy(buffer, ASS_LAV5);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD5);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_LAV6:   CBPresA->Color = (TColor)(COLOR_PRES_STAT6);
                        strcpy(buffer, ASS_LAV6);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD6);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_LAV7:   CBPresA->Color = (TColor)(COLOR_PRES_STAT7);
                        strcpy(buffer, ASS_LAV7);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD7);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_LAV8:   CBPresA->Color = (TColor)(COLOR_PRES_STAT8);
                        strcpy(buffer, ASS_LAV8);
                        if(AbbinaCodiceStatoBox==true) sprintf(buffer, "%d", ASS_COD8);
                        if(atoi(buffer)==0) assegnazione_not_ok=true;
                        break;
     case  PRES_VUOTO:  CBPresA->Color = (TColor)(COLOR_PRES_VUOTI); break;
     default:           CBPresA->Color = clSkyBlue;                  break;
  }
  //------------------------------------------------
  // Se l'assegnazione non è corretta assegno
  // quella iniziale
  //------------------------------------------------
  if(assegnazione_not_ok==true){
     strcpy(buffer, "Please put a correct code assignment!");
     Application->MessageBox(buffer, MESS[99], MB_OK | MB_ICONINFORMATION);
     CBPresA->ItemIndex  = NewPresCEST_A;
     switch(CBPresA->ItemIndex){
        case  PRES_LAV1:  CBPresA->Color = (TColor)(COLOR_PRES_STAT1);
                          break;
        case  PRES_LAV2:  CBPresA->Color = (TColor)(COLOR_PRES_STAT2);
                          break;
        case  PRES_LAV3:  CBPresA->Color = (TColor)(COLOR_PRES_STAT3);
                          break;
        case  PRES_LAV4:  CBPresA->Color = (TColor)(COLOR_PRES_STAT4);
                          break;
        case  PRES_LAV5:  CBPresA->Color = (TColor)(COLOR_PRES_STAT5);
                          break;
        case  PRES_LAV6:  CBPresA->Color = (TColor)(COLOR_PRES_STAT6);
                          break;
        case  PRES_LAV7:  CBPresA->Color = (TColor)(COLOR_PRES_STAT7);
                          break;
        case  PRES_LAV8:  CBPresA->Color = (TColor)(COLOR_PRES_STAT8);
                          break;
        case  PRES_VUOTO: CBPresA->Color = (TColor)(COLOR_PRES_VUOTI);
                          break;
               default:   CBPresA->Color = clSkyBlue;
                          break;
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormTgv::btnTgvUpDnClick(TObject *Sender)
{
  int myTag;

  myTag = (((TBitBtn *)Sender)->Tag);

  myTgv = NumeroTgv;
  if(myTag==0) myTgv=NumeroTgv-1;
  if(myTag==1) myTgv=NumeroTgv+1;

  if(myTgv<1)       myTgv=MAXAGV;
  if(myTgv>MAXAGV)  myTgv=1;

  FormStato->PageControlTGV->ActivePage=FormStato->TabTGV[myTgv];
  FormStato->PageControlTGVChange(Sender);
  FormStato->Refresh();

  NumeroTgv=myTgv;
  Activate();
}
//---------------------------------------------------------------------------

