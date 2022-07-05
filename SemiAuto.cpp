//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Remoto.h"
#include "SemiAuto.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormSemiAuto *FormSemiAuto;
// --------------------------
//   da GRAPHVAR.CPP
// --------------------------
//
extern int GrafScaffali[][11];
//---------------------------------------------------------------------------
__fastcall TFormSemiAuto::TFormSemiAuto(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormSemiAuto::FormCreate(TObject *Sender)
{
  memset(&Dati[0], 0, sizeof(Dati));
}
//---------------------------------------------------------------------------
void __fastcall TFormSemiAuto::FormActivate(TObject *Sender)
{
  int  i;
  int  TotMac  = TOTCHIAMATE;
  int  TotMag  = TOTMAG;
  int  TotBaie = TOTBAIE;
  char all_mess[101]="";

  // --- Inizializzazione delle variabili globali e di Editor ---
  Indice=1;
  EditTgv->SetFocus();
  GroupStato2->ItemIndex = 0;
  RBPos->Checked = true;
  //-----------------------------------------------
  // Abilitazione ciclo continuo solo in passw=3
  //-----------------------------------------------
  CBAttivaCicloContinuo->Enabled=false;
  if(LivelloPassword>2) CBAttivaCicloContinuo->Enabled=true;

  //-----------------------------------------------
  // Azzeramento delle variabili di classe
  //-----------------------------------------------
  memcpy(&Dati[0], &P.SemiAuto[0], sizeof(Dati));
  // --- Assegna gli indirizzi all'oggetto condiviso ---
  Gruppi();

  // --- imposta i messaggi degli oggetti ---
  Messaggi();

  //------------------------------------------
  // Visualizzazione selezione forca
  //------------------------------------------
  RBForcheA->Visible  = false;
  RBForcheB->Visible  = false;
  RBForcheAB->Visible = false;
  if(GestioneDoppiaForcaTGV==1){
     RBForcheA->Visible  = true;
     RBForcheB->Visible  = true;
     RBForcheAB->Visible = true;
  }

  //------------------------------------------
  // Disabilito la possibilità di modificare
  // la missione.
  //------------------------------------------
  RBPos->Checked          = true;
  EditNodo->Color         = clCream;
  EditNodo->Text          = "0000";
  EditNodo->Enabled       = true;
  //-- BAIE
  EditBaia->Color         = clLtGray;
  EditBaia->Enabled       = false;
  EditBaia->Text          = "0000";
  //-- MACCHINE
  EditComboMac->Color     = clLtGray;
  EditComboMac->ItemIndex = 0;
  EditComboMac->Enabled   = false;
  //-- MAGAZZINI
  EditMag->Color          = clLtGray;
  EditMag->Text           = "00";
  EditMag->Enabled        = false;
  RGLatoMag->ItemIndex    = 0;
  RGLatoMag->Enabled      = false;
  //-- SCAFFALI
  EditComboMac->Color     = clLtGray;
  EditComboMac->ItemIndex = 0;
  EditComboMac->Enabled   = false;
  //------------------------------------------
  // Disattivo groupbox per strutture
  // inesistenti
  //------------------------------------------
  if(TotMac<1 ){GBMac->Visible=false;  RBMac->Visible=false;}
  if(TotMag<1 ){GBMag->Visible=false;  RBMag->Visible=false;}
  if(TotBaie<1){GBBaia->Visible=false; RBBaia->Visible=false;}
  //------------------------------------------
  // Compilazione lista macchine
  //------------------------------------------
  EditComboMac->Items->Clear();
  EditComboMac->Items->Add(MESS[518]);
  for(i=1; i<=TOTCHIAMATE; i++){
     // Macchine inserite
     if(P.ch[i]->Num>0 && P.ch[i]->Num<=TOTCHIAMATE){
        sprintf(all_mess, "%02d - %s", P.ch[i]->Num, P.ch[i]->Sigla);
        EditComboMac->Items->Add(all_mess);
     }
     // Macchine future
     else{
        sprintf(all_mess, "%02d - unknow", i);
        EditComboMac->Items->Add(all_mess);
     }
  }

  // --- Rinfresco della pagina ---
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormSemiAuto::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  switch (Key){ case VK_ESCAPE: Close(); break;}
}
//---------------------------------------------------------------------------
// +-----------------------------------------------------------+
// | Assegna ad un unico oggetto gli indirizzi di vari oggetti |
// +-----------------------------------------------------------+
void __fastcall TFormSemiAuto::Gruppi()
{
  //
  // --- NON UTILIZZATO per questa particolare Form ---
  //
}
//---------------------------------------------------------------------------
// +------------------------------------------+
// | Imposta i messaggi abbinati agli oggetti |
// +------------------------------------------+
void __fastcall TFormSemiAuto::Messaggi()
{
  FormSemiAuto->Caption = MESS[565];

  GroupStato2->Caption  = MESS[570];
  GroupStato1->Caption  = MESS[571];

  Label1->Caption       = MESS[572];
  RBPos->Caption        = MESS[573];
  RBMac->Caption        = MESS[574];
  RBMag->Caption        = MESS[575];
  RBBaia->Caption       = MESS[566];
  // posizionamento semplice
  GBPos->Caption        = MESS[576];
  Label2->Caption       = MESS[577];
  // Carico/Scarico da macchina
  GBMac->Caption        = MESS[578];
  Label4->Caption       = MESS[579];
  // Carico/Scarico da baia
  GBBaia->Caption       = MESS[566];
  Label3->Caption       = MESS[567];
  // Carico/Scarico da magazzino
  GBMag->Caption                = MESS[580];
  Label6->Caption               = MESS[581];
  RGLatoMag->Items->Strings[ 0] = MESS[582];
  RGLatoMag->Items->Strings[ 1] = MESS[583];
  RGLatoMag->Items->Strings[ 2] = MESS[584];
  //----------------------------------------
  CBAttivaCicloContinuo->Caption = MESS[601];
  gbTipoOperazione->Caption      = MESS[602];
  BtnConferma->Caption           = MESS[103];
  BtnReset->Caption              = MESS[170];
  BtnEsci->Caption               = MESS[171];
}
//---------------------------------------------------------------------------
// +--------------+
// | GetCaselle() |
// +--------------+
void __fastcall TFormSemiAuto::GetCaselle()
{
  memset(&Dati[Indice], 0, sizeof(SEMIAUTOMATICO));

  //------------------------------------------
  // Visualizza stato semiautomatico continuo
  //------------------------------------------
  P.AbilitaSemiAutoCiclico = CBAttivaCicloContinuo->Checked;
  
  //------------------------------------------------------
  // Recupero del Numero di TGV da muovere e Tipo Presa
  //------------------------------------------------------
  Dati[Indice].Tgv      = (short int)(atoi(EditTgv->Text.c_str()));
  Dati[Indice].TypPresa = 0;
  if( RBForcheA-> Checked==true ) Dati[Indice].TypPresa=N_FORCA_A;
  if( RBForcheB-> Checked==true ) Dati[Indice].TypPresa=N_FORCA_B;
  if( RBForcheAB->Checked==true ) Dati[Indice].TypPresa=N_FORCA_AB;

  //-----------------------------------------
  // Recupero Tipo della 1° Missione
  //-----------------------------------------
  Dati[Indice].TypMiss=0;
  if( RBPos->Checked==true  ) Dati[Indice].TypMiss=MISS_SEMIAUTO_SOLO_POSIZIONAMENTO; // Pos. Semplice
  if( RBMac->Checked==true  ) Dati[Indice].TypMiss=MISS_SEMIAUTO_MACCHINA;            // Magazzino
  if( RBMag->Checked==true  ) Dati[Indice].TypMiss=MISS_SEMIAUTO_MAGAZZINO;           // Macchina
  if( RBBaia->Checked==true ) Dati[Indice].TypMiss=MISS_SEMIAUTO_BAIA;                // Baia

  //-----------------------------------------
  // Recupero dei dati delle missioni
  //-----------------------------------------
  Dati[Indice].Dest    = (short int)(atoi(EditNodo->Text.c_str()));
  Dati[Indice].Mac     = (short int)(EditComboMac->ItemIndex);
  Dati[Indice].Mag     = (short int)(atoi(EditMag->Text.c_str()));
  Dati[Indice].Baia    = (short int)(atoi(EditBaia->Text.c_str()));
  Dati[Indice].LatoMag = (short int)(RGLatoMag->ItemIndex);
}

//---------------------------------------------------------------------------
// +--------------+
// | SetCaselle() |
// +--------------+
void __fastcall TFormSemiAuto::SetCaselle()
{
  short int i;
  short int NumBaiaScaf;
  char Appoggio[101]="";
  char all_mess[201]="";

  //------------------------------------------
  // Visualizza stato semiautomatico continuo
  //------------------------------------------
  CBAttivaCicloContinuo->Checked = P.AbilitaSemiAutoCiclico;

  //------------------------------------------
  // Rinfresco di tutti i Campi di Editor
  //------------------------------------------
  sprintf(Appoggio, "%02d", Dati[Indice].Tgv);
  EditTgv->Text=Appoggio;
  RBForcheA->Checked=0; RBForcheB->Checked=0; RBForcheAB->Checked=0;
  if( Dati[Indice].TypPresa==N_FORCA_A ) RBForcheA ->Checked=true;
  if( Dati[Indice].TypPresa==N_FORCA_B ) RBForcheB ->Checked=true;
  if( Dati[Indice].TypPresa==N_FORCA_AB) RBForcheAB->Checked=true;

  // NODO
  sprintf(Appoggio, "%04d", Dati[Indice].Dest);               // Prima Missione
  EditNodo->Text=Appoggio;
  // BAIA
  sprintf(Appoggio, "%04d", Dati[Indice].Baia);
  EditBaia->Text=Appoggio;
  // MACCHINA
  EditComboMac->ItemIndex  = Dati[Indice].Mac;
  // MAGAZZINO
  sprintf(Appoggio, "%02d", Dati[Indice].Mag);
  EditMag->Text=Appoggio;
  RGLatoMag->ItemIndex = Dati[Indice].LatoMag;

  //----------------------------------------
  // Rinfresco di tutti i RadioButton
  //----------------------------------------
  if(Dati[Indice].TypMiss==MISS_SEMIAUTO_SOLO_POSIZIONAMENTO  ) RBPos->Checked =true; // Prima Missione
  if(Dati[Indice].TypMiss==MISS_SEMIAUTO_MACCHINA             ) RBMac->Checked =true;
  if(Dati[Indice].TypMiss==MISS_SEMIAUTO_MAGAZZINO            ) RBMag->Checked =true;
  if(Dati[Indice].TypMiss==MISS_SEMIAUTO_BAIA                 ) RBBaia->Checked =true;

  //----------------------------------------
  // Scrittura dell'elenco delle missioni
  //----------------------------------------
  for(i=1; i<11; i++){
     if(Dati[i].Tgv==0) sprintf(all_mess, "nr.%02d - free", i);
     else{
        switch(Dati[i].TypMiss){
           case  MISS_SEMIAUTO_SOLO_POSIZIONAMENTO:
              sprintf(all_mess, MESS[586], i, Dati[i].Tgv, Dati[i].Dest);
           break;
           case  MISS_SEMIAUTO_MACCHINA:
              sprintf(all_mess, MESS[587], i, Dati[i].Tgv, Dati[i].Mac);
           break;
           case  MISS_SEMIAUTO_MAGAZZINO:
              sprintf(all_mess, MESS[588], i, Dati[i].Tgv, Dati[i].Mag);
           break;
           case  MISS_SEMIAUTO_BAIA:
              sprintf(all_mess, MESS[569], i, Dati[i].Tgv, Dati[i].Baia);
           break;
           default: sprintf(all_mess, MESS[585], i);
                    break;
        }
     }
     GroupStato2->Items->Strings[i-1] = all_mess;
  };
}
//---------------------------------------------------------------------------
// +--------------+
// |  Conferma()  |
// +--------------+
int __fastcall TFormSemiAuto::Conferma()
{
  short int i;
  short int NumBaiaScaf;
  bool OkPos=false;

  GetCaselle();
  //-----------------------------------------------------
  // Verifica dei dati delle varie missioni impostate
  //-----------------------------------------------------
  for(i=1; i<11; i++){
     if(!Dati[i].Tgv                                                  ) continue;
     if(!Dati[i].Dest && !Dati[i].Mac && !Dati[i].Mag && !Dati[i].Baia) continue;
     //-----------------------------------------------------
     // Verifica del numero di AGV
     //-----------------------------------------------------
     if(Dati[i].Tgv<1 || Dati[i].Tgv>MAXAGV){
        Application->MessageBox(MESS[589], MESS[100], MB_OK | MB_ICONEXCLAMATION);
        memset(&Dati[i], 0, sizeof(SEMIAUTOMATICO));
        SetCaselle();
        return 1;
     }
     //-----------------------------------------------------
     // Verifica se il TGV in Semiautomatico
     //-----------------------------------------------------
     if(AGV[Dati[i].Tgv]->mission.SemiAutomatico==false){
        Application->MessageBox(MESS[590], MESS[100], MB_OK | MB_ICONEXCLAMATION);
        memset(&Dati[i], 0, sizeof(SEMIAUTOMATICO));
        SetCaselle();
        return 1;
     }
     //-----------------------------------------------------
     // Verifica dei dati introdotti per la missione
     //-----------------------------------------------------
     if(Dati[i].TypMiss<MISS_SEMIAUTO_SOLO_POSIZIONAMENTO){
        Application->MessageBox(MESS[591], MESS[100], MB_OK | MB_ICONEXCLAMATION);
        memset(&Dati[i], 0, sizeof(SEMIAUTOMATICO));
        SetCaselle();
        return 1;
     }
     //-----------------------------------------------------
     // Missione: Posizionamento Semplice
     //-----------------------------------------------------
     if(Dati[i].TypMiss==MISS_SEMIAUTO_SOLO_POSIZIONAMENTO){
        if((Dati[i].Dest<=0) || (Dati[i].Dest>MAXPUNTI)){
           Application->MessageBox(MESS[592], MESS[100], MB_OK | MB_ICONEXCLAMATION);
           memset(&Dati[i], 0, sizeof(SEMIAUTOMATICO));
           SetCaselle();
           return 1;
        }
     }
     //-----------------------------------------------------
     // Missione: Carico da macchina
     //-----------------------------------------------------
     if(Dati[i].TypMiss==MISS_SEMIAUTO_MACCHINA){
        if((Dati[i].Mac<1) || (Dati[i].Mac>TOTCHIAMATE)){
           Application->MessageBox(MESS[593], MESS[100], MB_OK | MB_ICONEXCLAMATION);
           memset(&Dati[i], 0, sizeof(SEMIAUTOMATICO));
           SetCaselle();
           return 1;
        }
     }
     //-----------------------------------------------------
     // Missione: Carico/Scarico da MAGAZZINO
     //-----------------------------------------------------
     if(Dati[i].TypMiss==MISS_SEMIAUTO_MAGAZZINO){
        if((Dati[i].Mag<1) || (Dati[i].Mag>TOTMAG)){
           Application->MessageBox(MESS[594], MESS[100], MB_OK | MB_ICONEXCLAMATION);
           memset(&Dati[i], 0, sizeof(SEMIAUTOMATICO));
           SetCaselle();
           return 1;
        }
     }
     //-----------------------------------------------------
     // Missione: Carico/Scarico da MAGAZZINO
     //-----------------------------------------------------
     if(Dati[i].TypMiss==MISS_SEMIAUTO_BAIA){
        if((Dati[i].Baia<1) || (Dati[i].Baia>TOTBAIE)){
           Application->MessageBox(MESS[597], MESS[100], MB_OK | MB_ICONEXCLAMATION);
           memset(&Dati[i], 0, sizeof(SEMIAUTOMATICO));
           SetCaselle();
           return 1;
        }
     }
     //-----------------------------------------------------
     // Missione: Posizionamento Semplice
     //-----------------------------------------------------
     if(Dati[i].TypMiss!=MISS_SOLO_POSIZIONAMENTO && GestioneDoppiaForcaTGV==1){
        if((Dati[i].TypPresa<=N_NO_FORCHE) || (Dati[i].TypPresa>N_FORCA_AB)){
           Application->MessageBox(MESS[595], MESS[100], MB_OK | MB_ICONEXCLAMATION);
           memset(&Dati[i], 0, sizeof(SEMIAUTOMATICO));
           SetCaselle();
           return 1;
        }
     }
     //-----------------------------------------------------
     // test AGV non in pos ( 0ø o 90ø o 180ø o 270ø )
     //-----------------------------------------------------
   //if(TestBit((char *) AGV[Dati[i].Tgv]->sbit, 0)) OkPos=true;  //  0ø
   //if(TestBit((char *) AGV[Dati[i].Tgv]->sbit, 1)) OkPos=true;  //  90ø
   //if(TestBit((char *) AGV[Dati[i].Tgv]->sbit, 7)) OkPos=true;  //  180ø
   //if(TestBit((char *) AGV[Dati[i].Tgv]->sbit, 8)) OkPos=true;  //  270ø
     OkPos=false;
     if(Dati[i].Tgv>0 && Dati[i].Tgv<=MAXAGV){
        if(AGV[Dati[i].Tgv]->stato.s.bit.grd0  ) OkPos=true;
        if(AGV[Dati[i].Tgv]->stato.s.bit.grd90 ) OkPos=true;
        if(AGV[Dati[i].Tgv]->stato.s.bit.grd180) OkPos=true;
        if(AGV[Dati[i].Tgv]->stato.s.bit.grd270) OkPos=true;
     }
     if(!OkPos){
        Application->MessageBox(MESS[596], MESS[100], MB_OK | MB_ICONEXCLAMATION);
        memset(&Dati[i], 0, sizeof(SEMIAUTOMATICO));
        SetCaselle();
        return 1;
     }
  }
  return 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormSemiAuto::RBTypMissClick(TObject *Sender)
{
  int TypMiss;

  if(RBPos ->Checked==true ) TypMiss=MISS_SEMIAUTO_SOLO_POSIZIONAMENTO;
  if(RBMac ->Checked==true ) TypMiss=MISS_SEMIAUTO_MACCHINA;
  if(RBMag ->Checked==true ) TypMiss=MISS_SEMIAUTO_MAGAZZINO;
  if(RBBaia->Checked==true ) TypMiss=MISS_SEMIAUTO_BAIA;

  //--
  EditNodo->Color         = clLtGray;
  EditNodo->Enabled       = false;
  //--
  EditBaia->Color         = clLtGray;
  EditBaia->Enabled       = false;
  //--
  EditComboMac->Color     = clLtGray;
  EditComboMac->Enabled   = false;
  //--
  EditMag->Color          = clLtGray;
  EditMag->Enabled        = false;
  RGLatoMag->Enabled      = false;

  //------------------------------------------
  // Abilitazioni in base al tipo di missione
  //------------------------------------------
  switch(TypMiss){
     case  MISS_SEMIAUTO_SOLO_POSIZIONAMENTO:
              EditNodo->Color            = clCream;
              EditNodo->Enabled          = true;
              EditComboMac->ItemIndex    = 0;
              EditBaia->Text             = "0000";
              EditMag->Text              = "00";
              break;
     case  MISS_SEMIAUTO_BAIA:
              EditBaia->Color            = clCream;
              EditBaia->Enabled          = true;
              EditComboMac->ItemIndex    = 0;
              EditNodo->Text             = "0000";
              EditMag->Text              = "00";
              break;
     case  MISS_SEMIAUTO_MACCHINA:
              EditComboMac->Color        = clCream;
              EditComboMac->Enabled      = true;
              EditNodo->Text             = "0000";
              EditBaia->Text             = "0000";
              EditMag->Text              = "00";
              break;
     case  MISS_SEMIAUTO_MAGAZZINO:
              EditMag->Color             = clCream;
              EditMag->Enabled           = true;
              RGLatoMag->Enabled         = true;
              RGLatoMag->ItemIndex       = 0;
              EditNodo->Text             = "0000";
              EditBaia->Text             = "0000";
              EditComboMac->ItemIndex    = 0;
              break;
     default: break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormSemiAuto::GroupStato2Click(TObject *Sender)
{
  //----------------------------------------------
  // Memorizzo i dati della missione precedente
  //----------------------------------------------
  Conferma();
  //----------------------------------------------
  // Recupero l'indice della nuova missione
  //----------------------------------------------
  Indice = (short int)(GroupStato2->ItemIndex+1);
  //----------------------------------------------
  //Rinfresco la form di visualizzazione
  //----------------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormSemiAuto::BtnConfermaClick(TObject *Sender)
{
  int err;
  char buffer[101]="";
  char NomeFile[101]="";

  //----------------------------------------------
  // Memorizzo i dati della missione precedente
  //----------------------------------------------
  if(Conferma()){
     FormSemiAuto->ModalResult = mrNone;
  }
  //----------------------------------------------
  // Compilazione della struttura globale con le
  // missioni di SEMIAUTOMATICO...
  //----------------------------------------------
  if(IdPC<2) memcpy(&P.SemiAuto[0], &Dati[0], sizeof(Dati));
  //----------------------------------------------
  //  Trasmetti comando al SERVER
  //----------------------------------------------
  else{
     //-------------------------------------------
     // Selezione del nome del file
     //-------------------------------------------
     strcpy(NomeFile, "Semiautomatico");
     err = RemCreaFileComando(NomeFile, (BYTE *)Dati, sizeof(Dati), buffer);
     if(err){
        Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
  }
  // --- Traccia su LOG OPERATORE
  FileLogOperatore("(SEMIAUT) - Missioni in semiautomatico");
  //----------------------------------------------
  //Rinfresco la form di visualizzazione
  //----------------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormSemiAuto::BtnResetClick(TObject *Sender)
{
  short int i;

  //----------------------------------------------
  // Caqncellazione della riga selezionata
  //----------------------------------------------
  for(i=Indice; i<10; i++){
     memcpy(&Dati[i], &Dati[i+1], sizeof(SEMIAUTOMATICO));  // shift della struttura
     memset(&Dati[i+1],        0, sizeof(SEMIAUTOMATICO));  // cancellazione della riga
  }
  //----------------------------------------------
  // Rinfresco della Form
  //----------------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------

void __fastcall TFormSemiAuto::EditTgvChange(TObject *Sender)
{
  int num_agv;

  num_agv = atoi(EditTgv->Text.c_str());
  //--------------------------------------------------
  // GESTIONE FORCA DOPPIA
  // Default set forca in base stato TGV
  //--------------------------------------------------
  if(num_agv>0 && num_agv<=MAXAGV && GestioneDoppiaForcaTGV==1){
     RBForcheA->Checked=false; RBForcheB->Checked=false; RBForcheAB->Checked=false;
     if(AGV[num_agv]->mission.PresCEST_A!=LIBERA && AGV[num_agv]->mission.PresCEST_B==LIBERA) RBForcheA->Checked=true;
     if(AGV[num_agv]->mission.PresCEST_A==LIBERA && AGV[num_agv]->mission.PresCEST_B!=LIBERA) RBForcheB->Checked=true;
     if(AGV[num_agv]->mission.PresCEST_A!=LIBERA && AGV[num_agv]->mission.PresCEST_B!=LIBERA) RBForcheAB->Checked=true;
  }
  //--------------------------------------------------
  // GESTIONE FORCA SINGOLA
  // Se non è abilitata la doppia forca e il TGV
  // compie tutte le missioni con la FORCA BASSA
  //--------------------------------------------------
  if(num_agv>0 && num_agv<=MAXAGV && GestioneDoppiaForcaTGV!=1) RBForcheB->Checked=true;
}
//---------------------------------------------------------------------------

