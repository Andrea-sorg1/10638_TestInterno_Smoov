//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "TecnoFer.h"
#include "PorteImp.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormPorteImp *FormPorteImp;
//---------------------------------------------------------------------------
__fastcall TFormPorteImp::TFormPorteImp(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormPorteImp::FormActivate(TObject *Sender)
{
  //----------------------------------------------
  // Inizializzazione di un'array di Editor
  //----------------------------------------------
  EditN[ 1] = EditNodo1;
  EditN[ 2] = EditNodo2;
  EditN[ 3] = EditNodo3;
  EditN[ 4] = EditNodo4;
  EditN[ 5] = EditNodo5;
  EditN[ 6] = EditNodo6;
  EditN[ 7] = EditNodo7;
  EditN[ 8] = EditNodo8;
  EditN[ 9] = EditNodo9;
  EditN[10] = EditNodo10;

  EditNS[ 1] = EditNsem1;
  EditNS[ 2] = EditNsem2;
  EditNS[ 3] = EditNsem3;
  EditNS[ 4] = EditNsem4;
  EditNS[ 5] = EditNsem5;
  EditNS[ 6] = EditNsem6;
  EditNS[ 7] = EditNsem7;
  EditNS[ 8] = EditNsem8;
  EditNS[ 9] = EditNsem9;
  EditNS[10] = EditNsem10;


  //----------------------------------------------
  // Scroll porte
  //----------------------------------------------
  UpDwPort->Position = NumeroPortaImp;
  UpDwPort->Min      = 1;
  UpDwPort->Max      = TOTPORTE;

  //----------------------------------------------
  // Messaggeria
  //----------------------------------------------
  Messaggi();

  //----------------------------------------------
  // Refresh della Form
  //----------------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormPorteImp::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if(Key==VK_ESCAPE) Close();
}
//---------------------------------------------------------------------------
//-----------------------
// Messaggi()
//-----------------------
void __fastcall TFormPorteImp::Messaggi()
{
  FormPorteImp->Caption   = MESS[286];
  BtnOk->Caption          = MESS[103];

  gbNodi->Caption         = MESS[382];
  CBEsclusa->Caption      = MESS[742];
  lblAnticipoRich->Caption= MESS[743];
  lblInterbloc->Caption   = MESS[744];

  gbConsensi->Caption     = MESS[383];
  CBRich->Caption         = MESS[385];
  CBOk->Caption           = MESS[386];
  CBOkPar->Caption        = MESS[388];
  CBIng->Caption          = MESS[387];
  CBCarico->Caption       = MESS[748];
  CBBlocco->Caption       = MESS[745];
  CBTgvInArrivo->Caption  = MESS[746];

  gbDirezione->Caption    = MESS[384];
  CBUp->Caption           = MESS[221];
  CBDown->Caption         = MESS[222];
  CBLeft->Caption         = MESS[223];
  CBRight->Caption        = MESS[224];

  gbSemafori->Caption     = MESS[747];

}
//---------------------------------------------------------------------------
//-----------------------
// GetCaselle
//-----------------------
void __fastcall TFormPorteImp::GetCaselle()
{
  int i;

  //--------------------------------------
  // Nodi in ingombro alle prote
  //--------------------------------------
  for(i=0; i<10; i++){
     PorteImp[NumeroPortaImp].Punto[i] = (short int)(atoi(EditN[i+1]->Text.c_str()));
  }
  //--------------------------------------
  // Nodi per gestione semaforo
  //--------------------------------------
  for(i=0; i<10; i++){
     PorteImp[NumeroPortaImp].PuntoSemaforo[i] = (short int)(atoi(EditNS[i+1]->Text.c_str()));
  }
  //--------------------------------------
  // Scrittura consensi con comunicazione
  // OFF
  //--------------------------------------
  if(break_Com[COM2]==true){
     PorteImp[NumeroPortaImp].RichIngresso     = CBRich->Checked;
     PorteImp[NumeroPortaImp].OkIngresso       = CBOk->Checked;
     PorteImp[NumeroPortaImp].TgvInIngombro    = CBIng->Checked;
     PorteImp[NumeroPortaImp].AperturaParziale = CBOkPar->Checked;
     PorteImp[NumeroPortaImp].TgvConCarico     = CBCarico->Checked;
     PorteImp[NumeroPortaImp].PortaInBlocco    = CBBlocco->Checked;
     PorteImp[NumeroPortaImp].TgvInArrivo      = CBTgvInArrivo->Checked;
  }
  //--------------------------------------
  // Direzione di movimento
  //--------------------------------------
  PorteImp[NumeroPortaImp].DirUp    = CBUp->Checked;
  PorteImp[NumeroPortaImp].DirDown  = CBDown->Checked;
  PorteImp[NumeroPortaImp].DirLeft  = CBLeft->Checked;
  PorteImp[NumeroPortaImp].DirRight = CBRight->Checked;
  //--------------------------------------
  // Flag di esclusione PORTA
  //--------------------------------------
  PorteImp[NumeroPortaImp].EsclusionePLC = CBEsclusa->Checked;
  //--------------------------------------
  // Anticipo richiesta ingresso
  //--------------------------------------
  PorteImp[NumeroPortaImp].AnticipoRichiesta = (short int)(atoi(EdAnticipo->Text.c_str()));
  //--------------------------------------
  // Anticipo richiesta ingresso
  //--------------------------------------
  PorteImp[NumeroPortaImp].PortaInterbloccata = (short int)(atoi(EdInterblocco->Text.c_str()));
}
//---------------------------------------------------------------------------
//-----------------------
// SetCaselle
//-----------------------
void __fastcall TFormPorteImp::SetCaselle()
{
  int  i;
  char buffer[31]="";

  //--------------------------------------
  // Numero porta
  //--------------------------------------
  sprintf(buffer, "%s nr.%02d", MESS[486], NumeroPortaImp);
  FormPorteImp->Caption = buffer;
  //--------------------------------------
  // Abilito consensi a inserimento
  // manuale con comunicazione esclusa
  //--------------------------------------
  if(break_Com[COM2]==true){
     gbConsensi->Enabled = true;
  }
  else{
     gbConsensi->Enabled = false;
  }

  //--------------------------------------
  // Nodi in ingombro alle prote
  //--------------------------------------
  for(i=0; i<10; i++){
     sprintf(buffer, "%04d", PorteImp[NumeroPortaImp].Punto[i]);
     EditN[i+1]->Text = buffer;
  }
  //--------------------------------------
  // Nodi in ingombro alle prote
  //--------------------------------------
  for(i=0; i<10; i++){
     sprintf(buffer, "%04d", PorteImp[NumeroPortaImp].PuntoSemaforo[i]);
     EditNS[i+1]->Text = buffer;
  }
  //--------------------------------------
  // Direzione di movimento
  //--------------------------------------
  CBUp->Checked    = PorteImp[NumeroPortaImp].DirUp;
  CBDown->Checked  = PorteImp[NumeroPortaImp].DirDown;
  CBLeft->Checked  = PorteImp[NumeroPortaImp].DirLeft;
  CBRight->Checked = PorteImp[NumeroPortaImp].DirRight;
  //--------------------------------------
  // Stato consensi porte
  //--------------------------------------
  CBRich->Checked         = PorteImp[NumeroPortaImp].RichIngresso;
  CBOk->Checked           = PorteImp[NumeroPortaImp].OkIngresso;
  CBIng->Checked          = PorteImp[NumeroPortaImp].TgvInIngombro;
  CBOkPar->Checked        = PorteImp[NumeroPortaImp].AperturaParziale;
  CBCarico->Checked       = PorteImp[NumeroPortaImp].TgvConCarico;
  CBBlocco->Checked       = PorteImp[NumeroPortaImp].PortaInBlocco;
  CBTgvInArrivo->Checked  = PorteImp[NumeroPortaImp].TgvInArrivo;
  //--------------------------------------
  // Flag di esclusione PORTA
  //--------------------------------------
  CBEsclusa->Checked = PorteImp[NumeroPortaImp].EsclusionePLC;
  //--------------------------------------
  // Anticipo richiesta ingresso
  //--------------------------------------
  sprintf(buffer, "%02d", PorteImp[NumeroPortaImp].AnticipoRichiesta);
  EdAnticipo->Text = buffer;
  //--------------------------------------
  // Porta Interbloccata
  //--------------------------------------
  sprintf(buffer, "%02d", PorteImp[NumeroPortaImp].PortaInterbloccata);
  EdInterblocco->Text = buffer;
}
//---------------------------------------------------------------------------
void __fastcall TFormPorteImp::BtnOkClick(TObject *Sender)
{
  int  err;
  char all_mess[101]="";

  //---------------------------
  // rinfresco della struttura
  //---------------------------
  GetCaselle();
  //---------------------------
  // salvataggio su file
  //---------------------------
  err = save_file_porteimp(all_mess);
  if(err) Application->MessageBox(all_mess, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //----------------------------------------------------------------
  // Traccia su LOG OPERATORE
  //----------------------------------------------------------------
  if(PorteImp[NumeroPortaImp].EsclusionePLC==true){
     sprintf(all_mess, "(EDITPOR) - (Por:%d) Salva editor porta disabled", NumeroPortaImp);
  }
  else{
     sprintf(all_mess, "(EDITPOR) - (Por:%d) Salva editor porta enabled", NumeroPortaImp);
  }
  FileLogOperatore(all_mess);
  //---------------------------
  // rinfresco della finestra
  //---------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormPorteImp::UpDwPortClick(TObject *Sender, TUDBtnType Button)
{
  //-------------------------------------
  // Recupero numero nuova porta
  //-------------------------------------
  NumeroPortaImp = (short int)(UpDwPort->Position);
  SetCaselle();
}
//---------------------------------------------------------------------------

