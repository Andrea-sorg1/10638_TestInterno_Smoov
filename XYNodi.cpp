//---------------------------------------------------------------------------
#include <vcl.h>
#include <io.h>
#include <stdio.h>
#include <dir.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <share.h>
#include <errno.h>
#include <math.h>

#pragma hdrstop

#include "Tecnofer.h"
#include "XYNodi.h"
#include "Main.h"
#include "Stato.h"
#include "Layout.h"
#include "Laser.h"

extern int offset[][2];
//  ---------------------
//   STRUTTURA NODI
//  ---------------------
//  0,1 = x,y posizione centrale nodo
//  2 = numero nodo
//  3 = occupazione agv ( numero AGV oppure libero = 0 )
//-------------------------------------------------------
extern int Punti[][5];

//  ---------------------
//   STRUTTURE GRAFICHE
//  ---------------------
//-------------------------------------------------------
extern int Percorso[][2];
extern int PercorsoCurva[][5];
extern int PercorsoCurvaBezier[][7];
extern int Porte[][6];

//  ---------------------
//   FUNZIONI GRAFICHE
//  ---------------------
//-------------------------------------------------------
extern int ConvLeggiFileGraphPuntiAscii( char *NomeFile, char* all_mess);
extern int ConvLeggiFileGraphPercorsiCurvaAscii( char *NomeFile, char* all_mess);
extern int ConvLeggiFileGraphPercorsiCurvaBezierAscii( char *NomeFile, char* all_mess);
       int ConvLeggiFileGraphPercorsiRetta( char *NomeFile, char* all_mess);


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormXYNodi *FormXYNodi;
//  ---------------------
//   STRUTTURA NODI
//  ---------------------
//  0,1 = x,y posizione centrale nodo
//  2 = numero nodo
//  3 = occupazione agv ( numero AGV oppure libero = 0 )
//-------------------------------------------------------
extern int Punti[][5];
extern int ConvScriviFileGraphPuntiAscii( char *NomeFile, char* all_mess);

//---------------------------------------------------------------------------
__fastcall TFormXYNodi::TFormXYNodi(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormXYNodi::FormCreate(TObject *Sender)
{
  int i=0;
  
  TimeGraphvar=0;
  traslazione_vc=false;
  CurvaSelezionata=9999;
  memset(&ElencoNodi[0], 0, sizeof(ElencoNodi));

  gbIngombroAgvGrafico->Visible=false;

  for(i=0;i<=MAXAGV;i++){
     SegmentoPerIngombro[i]=999999;
     CurvaPerIngombro[i]=999999;
     DirezionePerIngombro[i]=0;
     PuntoPerIngombro[i]=999999;
  }

  //----------------------------------------------------------
  // Abilitazione
  //----------------------------------------------------------
  // Disabilitazione funzioni protette
  //----------------------------------------------------------
  #ifdef DISABILITA_ALGO_CAD
     if(access("C:\\TEST\\PROTETTO.DAT", 0)){
        //GBXYNodi->Enabled = false;
        EditX->Enabled = false;
        EditY->Enabled = false;
        EditXV->Enabled= false;
        EditYV->Enabled= false;
        udNX->Enabled  = false;
        udNY->Enabled  = false;
        udVX->Enabled  = false;
        udVY->Enabled  = false;
        udNX1->Enabled = false;
        udNY1->Enabled = false;
        udVX1->Enabled = false;
        udVY1->Enabled = false;
     }
  #endif

  #ifdef ABILITA_GESTIONE_INGOMBRO_AGV_GRAFICO
     gbIngombroAgvGrafico->Visible=true;
  #endif


}
//---------------------------------------------------------------------------
void __fastcall TFormXYNodi::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if(Key!=VK_ESCAPE) return;
  //----------------------------------------------
  // Annullamento modifiche
  //----------------------------------------------
  Abort();
  CurvaSelezionata=9999;
  memset(&ElencoNodi[0], 0, sizeof(ElencoNodi));
  //----------------------------------------------
  // Refresfh della finestra
  //----------------------------------------------
  Refresh(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TFormXYNodi::Refresh(bool Open)
{
  int i;
  bool NoGestioneCurve;
  char buffer[1001]="";

  NoGestioneCurve=true;
  #ifdef GESTIONE_CURVE
     NoGestioneCurve=false;
  #endif
  if(Open==true){
     //-----------------------------------------------------------
     // 18/04/2014 Abilitazione Abbinamento Zona Mapping al Nodo
     //-----------------------------------------------------------
     if(NoGestioneCurve==false){
        if( Laser_AbilitaZonaMapSuNodo) GroupZoneMapping->Visible=true;
        if(!Laser_AbilitaZonaMapSuNodo) GroupZoneMapping->Visible=false;
     }
     //----------------------------------------------------
     // Azzeramento strutture di aggiustamento layout
     //----------------------------------------------------
     udCurva->Min=0;
     udCurva->Max=MAX_TRACCE_CURVA_DWG;
     memset(&XY_Punti[0],               0, sizeof(XY_Punti));
     memset(&XY_PercorsoCurvaBezier[0], 0, sizeof(XY_PercorsoCurvaBezier));
     //----------------------------------------------------
     // Memorizzo strutture grafiche prima delle modifiche
     //----------------------------------------------------
     for(i=1; i<MAXPUNTI; i++){
        if(Punti[i][2]==0) break;
        memcpy(&XY_Punti[i][0], &Punti[i][0], sizeof(Punti[i]));
     }
     for(i=0; i<MAX_TRACCE_CURVA_DWG; i++){
        if(PercorsoCurvaBezier[i][0]==0) break;
        memcpy(&XY_PercorsoCurvaBezier[i][0], &PercorsoCurvaBezier[i][0], sizeof(PercorsoCurvaBezier[i]));
     }
     //-----------------------------------------------
     // Legenza assegnazione zone impianto
     //-----------------------------------------------
     sprintf(buffer, "999  - no change mapping zone\n000  - mapping zone will be calculated\n255  - mapping zone nr.0\n1...N - mapping zone number nr.1...N");
     TxtLegenda->Caption = buffer;
     //-----------------------------------------------
     // Default offset coordinate = "0"
     //-----------------------------------------------
     CurvaSelezionata= 9999;
     EditX->Text     = 0;
     EditY->Text     = 0;
     EditZ->Text     = 999;
     EditXV->Text    = 0;
     EditYV->Text    = 0;
     udNX->Position  = 0;
     udNX1->Position = 0;
     udNY->Position  = 0;
     udNY1->Position = 0;
     udVX->Position  = 0;
     udVX1->Position = 0;
     udVY->Position  = 0;
     udVY1->Position = 0;
  }
  //-----------------------------------------------
  // Compilazione della LISTA dei nodi selezionati
  //-----------------------------------------------
  CLBNodi->Items->Clear();
  for(i=0; i<MAXPUNTI; i++){
     if(ElencoNodi[i][0]==0) break;
     CLBNodi->Items->Add(ElencoNodi[i][0]);
     CLBNodi->Checked[i]=(bool)(ElencoNodi[i][1]);
  }
  //-----------------------------------------------
  // Visualizzazione del totale nodi selezionati
  //-----------------------------------------------
  PanelTotNodi->Caption = i;
  //-----------------------------------------------
  // Aggiornamento dati curva selezionata
  //-----------------------------------------------
  if(CurvaSelezionata!=9999){
     sprintf(buffer, "Curva%d\nNs:%4d, Ne:%4d, Xv:%d, Yv:%d", CurvaSelezionata+1, PercorsoCurvaBezier[CurvaSelezionata][1], PercorsoCurvaBezier[CurvaSelezionata][2], PercorsoCurvaBezier[CurvaSelezionata][3], PercorsoCurvaBezier[CurvaSelezionata][4]);
     stCurva->Caption  = buffer;
     EditXV->Text      = PercorsoCurvaBezier[CurvaSelezionata][3];
     EditYV->Text      = PercorsoCurvaBezier[CurvaSelezionata][4];
     udVX->Position    = PercorsoCurvaBezier[CurvaSelezionata][3];
     udVY->Position    = PercorsoCurvaBezier[CurvaSelezionata][4];
     udVX1->Position   = PercorsoCurvaBezier[CurvaSelezionata][3];
     udVY1->Position   = PercorsoCurvaBezier[CurvaSelezionata][4];
     udCurva->Position = CurvaSelezionata;
    }
  else{
     udVX->Position  = 0;
     udVX1->Position = 0;
     udVY->Position  = 0;
     udVY1->Position = 0;
     EditXV->Text    = 0;
     EditYV->Text    = 0;
     stCurva->Caption= "";
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormXYNodi::Aggiorna()
{
  int i, X, Y;
  int TotCurve;
  char buffer[101]="";
  bool almeno_un_nodo;

  //--------------------------------------------
  // Conteggio curve impianto
  //--------------------------------------------
  TotCurve=0;
  for(i=0;;i++){
     if(PercorsoCurvaBezier[i][0]==0) break;
     TotCurve++;
  }
  //
  // GESTIONE NODI
  //
  //--------------------------------------------
  // Nessun nodo selezionato
  //--------------------------------------------
  almeno_un_nodo=false;
  for(i=0; i<MAXPUNTI; i++){
     if(ElencoNodi[i][0]==0) continue;
     almeno_un_nodo=true;
     break;
  }
  if(almeno_un_nodo==false){
     udNX->Position  = 0;
     udNY->Position  = 0;
     udNX1->Position = 0;
     udNY1->Position = 0;
     EditX->Text     = 0;
     EditY->Text     = 0;
  }
  else{
     //--------------------------------------------
     // Recupero le impostazione OFFSET da EDITOR
     //--------------------------------------------
     X = StrToIntDef(EditX->Text, 0);
     Y = StrToIntDef(EditY->Text, 0);
     //--------------------------------------------
     // Somma dell'offset alle coordinate di tutti
     // i nodi che sono rimasti selezionati
     //--------------------------------------------
     for(i=0; i<MAXPUNTI; i++){
        if(ElencoNodi[i][0]==0) continue;
        //---------------------------------------------
        // Nodo selezionato aggiornamento coordinate
        //---------------------------------------------
        if(ElencoNodi[i][1]==1){
           Punti[ElencoNodi[i][0]][0] = XY_Punti[ElencoNodi[i][0]][0]+X;
           Punti[ElencoNodi[i][0]][1] = XY_Punti[ElencoNodi[i][0]][1]+(Y*(-1));
        }
        //---------------------------------------------
        // Nodo NON selezionato aggiornamento
        // ripristino ultime coordinate salvate
        //---------------------------------------------
        else{
           Punti[ElencoNodi[i][0]][0] = XY_Punti[ElencoNodi[i][0]][0];
           Punti[ElencoNodi[i][0]][1] = XY_Punti[ElencoNodi[i][0]][1];
        }
     }
     //--------------------------------------------
     // Traslazione delle Curve di Bezier
     //--------------------------------------------
     if(traslazione_vc==false) TraslazioneCurveGrafica(X, Y);
     //--------------------------------------------
     // Aggiorno posizionamento UpDown
     //--------------------------------------------
     udNX->Position  = X;
     udNY->Position  = Y;
     udNX1->Position = X;
     udNY1->Position = Y;
  }
  //
  // GESTIONE CURVE
  //
  //--------------------------------------------
  // Aggiorna curva selezionata
  //--------------------------------------------
  if(TotCurve==0 || CurvaSelezionata>=TotCurve ) CurvaSelezionata=9999;
  if(CurvaSelezionata==9999){
     udVX->Position  = 0;
     udVX1->Position = 0;
     udVY->Position  = 0;
     udVY1->Position = 0;
     EditXV->Text    = 0;
     EditYV->Text    = 0;
     stCurva->Caption= "";
  }
  else{
     sprintf(buffer, "Curva%d\nNs:%4d, Ne:%4d, Xv:%d, Yv:%d", CurvaSelezionata, PercorsoCurvaBezier[CurvaSelezionata][1], PercorsoCurvaBezier[CurvaSelezionata][2], PercorsoCurvaBezier[CurvaSelezionata][3], PercorsoCurvaBezier[CurvaSelezionata][4]);
     stCurva->Caption= buffer;
     EditXV->Text    = PercorsoCurvaBezier[CurvaSelezionata][3];
     EditYV->Text    = PercorsoCurvaBezier[CurvaSelezionata][4];
     udVX->Position  = PercorsoCurvaBezier[CurvaSelezionata][3];
     udVY->Position  = PercorsoCurvaBezier[CurvaSelezionata][4];
     udVX1->Position = PercorsoCurvaBezier[CurvaSelezionata][3];
     udVY1->Position = PercorsoCurvaBezier[CurvaSelezionata][4];
     traslazione_vc  = false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormXYNodi::Abort()
{
  int i, err;
  char buffer[101]="";

  //----------------------------------------------------
  // Reset offset di spostamento
  //----------------------------------------------------
  EditX->Text     = 0;
  EditY->Text     = 0;
  EditZ->Text     = 999;
  udNX->Position  = 0;
  udNX1->Position = 0;
  udNY->Position  = 0;
  udNY1->Position = 0;
  //-------------------------------------------------------
  // Rilettura struttura Curve Bezier
  //-------------------------------------------------------
  ReloadGRAPHVAR(FALSE);
  //-------------------------------------------------------
  // Refresch campi gestione vertice di controllo
  //-------------------------------------------------------
  if(CurvaSelezionata==9999){
     EditXV->Text    = 0;
     EditYV->Text    = 0;
     udVX->Position  = 0;
     udVX1->Position = 0;
     udVY->Position  = 0;
     udVY1->Position = 0;
  }
  else{
     sprintf(buffer, "Curva%d\nNs:%4d, Ne:%4d, Xv:%d, Yv:%d", CurvaSelezionata, PercorsoCurvaBezier[CurvaSelezionata][1], PercorsoCurvaBezier[CurvaSelezionata][2], PercorsoCurvaBezier[CurvaSelezionata][3], PercorsoCurvaBezier[CurvaSelezionata][4]);
     stCurva->Caption= buffer;
     EditXV->Text    = PercorsoCurvaBezier[CurvaSelezionata][3];
     EditYV->Text    = PercorsoCurvaBezier[CurvaSelezionata][4];
     udVX->Position  = PercorsoCurvaBezier[CurvaSelezionata][3];
     udVY->Position  = PercorsoCurvaBezier[CurvaSelezionata][4];
     udVX1->Position = PercorsoCurvaBezier[CurvaSelezionata][3];
     udVY1->Position = PercorsoCurvaBezier[CurvaSelezionata][4];
}
}
//---------------------------------------------------------------------------
void __fastcall TFormXYNodi::InsertNodi(short int nodo)
{
  int i;

  for(i=0; i<MAXPUNTI; i++){
     if(ElencoNodi[i][0]==nodo  ){ElencoNodi[i][1] = 1; break;}
     if(ElencoNodi[i][0]!=0     ) continue;
     ElencoNodi[i][0] = nodo;
     ElencoNodi[i][1] = 1;
     break;
  }
  //------------------------------
  // Refresh finestra
  //------------------------------
  Aggiorna();
}
//---------------------------------------------------------------------------
void __fastcall TFormXYNodi::DeleteNodi(short int nodo)
{
  int i;

  for(i=0; i<MAXPUNTI; i++){
     if(ElencoNodi[i][0]==0     ) continue;
     if(ElencoNodi[i][0]!=nodo  ) continue;
     ElencoNodi[i][1] = 0;
     break;
  }
  //------------------------------
  // Refresh finestra
  //------------------------------
  Aggiorna();
}
//---------------------------------------------------------------------------
short int __fastcall TFormXYNodi::CercaNodo(short int nodo)
{
  int i;
  short int index;

  index=0;
  for(i=0; i<MAXPUNTI; i++){
     if(ElencoNodi[i][0]==0     ) continue;
     if(ElencoNodi[i][0]!=nodo  ) continue;
     //---------------------------------
     // Ritorno indice lista nodi
     // selezionati
     //---------------------------------
     index=i+1;
     //---------------------------------
     // Se il nodo è in lista ma non
     // selezionato --> return 9999
     //---------------------------------
     if(ElencoNodi[i][1]==0) index=MAXPUNTI+1;
     break;
  }
  return index;
}
//---------------------------------------------------------------------------
void __fastcall TFormXYNodi::CLBNodiClickCheck(TObject *Sender)
{
  int i, index, nodo;

  index = CLBNodi->ItemIndex;

  //-----------------------------------------
  // Recupero il nodo a cui fa riferimento
  //-----------------------------------------
  nodo = atoi(CLBNodi->Items->Strings[index].c_str());

//ElencoNodi[nodo] = CLBNodi->Checked[index];
  if(CLBNodi->Checked[index]==false){
     for(i=0; i<MAXPUNTI; i++){
        if(ElencoNodi[i][0]!=nodo) continue;
        ElencoNodi[i][1]=0;
        break;
     }
  }
  else{
     for(i=0; i<MAXPUNTI; i++){
        if(ElencoNodi[i][0]!=nodo) continue;
        ElencoNodi[i][1]=1;
        break;
     }
  }
  //-----------------------------------------
  // Richiamo funzione Aggiorna
  //-----------------------------------------
  Aggiorna();
}
//---------------------------------------------------------------------------

void __fastcall TFormXYNodi::BtnEsciClick(TObject *Sender)
{
  char buffer[101];
  int  i=0;

  ModificaCoordinateNodi=false;
  FormMain->MModificaCoordinateNodi->Checked = ModificaCoordinateNodi;
  //----------------------------------------------
  // Annullamento modifiche
  //----------------------------------------------
  Abort();
  FormMappa->AutoSend=false;
  CurvaSelezionata=9999;
  memset(&ElencoNodi[0], 0, sizeof(ElencoNodi));
  for(i=1;i<=MAXAGV;i++){
     IngombroAgvGrafico[i]->AzzeraIngombroAgv();
  }
  //----------------------------------------------
  // Attivazione visualizzazione pag coordinate
  //----------------------------------------------
  FormLayout->WindowState = wsNormal;
  FormLayout->Left   = 0;
  FormLayout->Top    = 0;
  FormLayout->Height = FormMain->ClientHeight-83;
  FormStato->Height  = FormLayout->Height;
  FormXYNodi->Height = FormLayout->Height;
  if(ModificaCoordinateNodi==false){
   //FormLayout->Width   = FormMain->ClientWidth-FormStato->Width-3;
     FormLayout->Width   = FormMain->ClientWidth-FormStato->Width-4;
     FormXYNodi->Height  = 0;
     FormStato->Left     = FormLayout->Width;
     FormStato->Top      = FormLayout->Top;
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormXYNodi::BtnAbortClick(TObject *Sender)
{
  Abort();
}
//---------------------------------------------------------------------------
void __fastcall TFormXYNodi::BtnOkClick(TObject *Sender)
{
  char buffer[201]="";
  int Indice, result;
  int X, Y, Z, i, err;
  bool almeno_un_nodo;
  bool almeno_una_dist, almeno_un_zona;

  err=0;
  almeno_una_dist=almeno_un_zona=false;
  result = Application->MessageBox( MESS[380], MESS[99], MB_YESNO | MB_ICONQUESTION);
  if(result != IDYES) return;
  //--------------------------------------------
  // Recupero le impostazione OFFSET da EDITOR
  //--------------------------------------------
  X = StrToIntDef(EditX->Text, 0);
  Y = StrToIntDef(EditY->Text, 0)*(-1);
  Z = StrToIntDef(EditZ->Text, 999);
  //--------------------------------------------
  // Nessun nodo selezionato
  //--------------------------------------------
  almeno_un_nodo=false;
  for(i=0; i<MAXPUNTI; i++){
     if(ElencoNodi[i][0]==0) continue;
     if(ElencoNodi[i][1]==0) continue;
     almeno_un_nodo=true;
     break;
  }
  if(almeno_un_nodo==false){
     ShowMessage("ATTENTION: Nodes list is empty!!!");
     return;
  }
  //--------------------------------------------
  // Recupero l'indice del pulsante premuto
  //--------------------------------------------
  Indice = ((TSpeedButton *)Sender)->Tag;
  //===============================================
  // MODIFICA COORDINATE NODI
  //===============================================
  if(Indice==1){
     //--------------------------------------------
     // Aggiornamento del GRAPHVAR.DAT
     //--------------------------------------------
     ConvScriviFileGraphPuntiAscii("GRAPHVAR.DAT", buffer);
     //-------------------------------------------------------
     // Ricalcolo distanze nodi da grafica appena ricaricata
     //-------------------------------------------------------
     err = N.ricalcolo_distanze_nodi(buffer);
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     if(X!=0 || Y!=0){
        //-----------------------------------------------------
        // 1. COORDINATE NODI
        // Ricalcolo distanze nodi interessate dalla modifica
        // di coordinate
        //-----------------------------------------------------
        if(X!=0 || Y!=0){
           //-----------------------------------------------------
           // Almeno una distanza ricalcolata
           //-----------------------------------------------------
           almeno_una_dist=true;
           //-----------------------------------------------------
           // Se all'interno dell'impianto esiste almeno un TGV
           // dotato di gestione laser setto il flag per ricordare
           // l'attivazione del MAPPING
           //-----------------------------------------------------
           for(i=1; i<=MAXAGV; i++){
              if(!AGV[i]->mission.GestioneLaser) continue;
              RiattivaMapping[i]=true;
           }

           for(i=1; i<=MAXAGV; i++){
              if(!AGV[i]->mission.GestioneLaser) continue;
              //----------------------------------------------
              // Aggiornamento diretto mappe nodi a TGV
              //----------------------------------------------
              if(break_Com[COM1]               ) continue;
              if(AGV[i]->mission.EsclPLC==true ) continue;
              //----------------------------------------------
              // Richiesta conferma
              //----------------------------------------------
              sprintf(buffer, "Attivare scaricamento mappa coordinate per TGV%d?", i);
              result = Application->MessageBox(buffer, MESS[99], MB_YESNOCANCEL | MB_ICONQUESTION);

              if(result == IDCANCEL)  break;
              if(result != IDYES)     continue;
              //if(result != IDYES){RiattivaMapping[i]=false; continue;}

              //--------------------------------------------
              // Apertura della finestra di gestione mappa
              //--------------------------------------------
              FormMappa->NumTgv=(short int)(i);
              FormMappa->AutoSend=chkbSendAllTGV->Checked;
              FormMappa->ShowModal();
           }
        }
        //-----------------------------------------------------
        // 1. ZONE MAPPING
        // Riassegnazione zone mapping
        //-----------------------------------------------------
        if(Z!=999){
           //-----------------------------------------------------
           // Ricalcolo delle distanze nodi
           //-----------------------------------------------------
           for(i=0; i<MAXPUNTI; i++){
              if(N.n[i]->num<1        ) continue;
              if(N.n[i]->num>MAXPUNTI ) continue;
              //--------------------------------------------
              // Verifico se il nodo considerato è stato
              // selezionato
              //--------------------------------------------
              if(CercaNodo(N.n[i]->num)<1 || CercaNodo(N.n[i]->num)>MAXPUNTI) continue;
              //--------------------------------------------
              // Azzegnazione nuova zona (max. 255 zone)
              //--------------------------------------------
              if(Z<=255) N.n[i]->ZoneMap = Z;
              else       N.n[i]->ZoneMap = 255;
              //----------------------------------
              // Almeno una zona riassegnata
              //----------------------------------
              almeno_un_zona=true;
           }
        }
     }
     //---------------------------------------
     // Riazzeramento pagina
     //---------------------------------------
     Refresh(TRUE);
  }
  //===============================================
  // ASSEGNAZIONE ZONE MAPPING
  //===============================================
  if(Indice==2){
     if(Z!=999){
        //-----------------------------------------------------
        // 1. ZONE MAPPING
        // Riassegnazione zone mapping
        //-----------------------------------------------------
        if(Z!=999){
           //-----------------------------------------------------
           // Ricalcolo delle distanze nodi
           //-----------------------------------------------------
           for(i=0; i<MAXPUNTI; i++){
              if(N.n[i]->num<1        ) continue;
              if(N.n[i]->num>MAXPUNTI ) continue;
              //--------------------------------------------
              // Verifico se il nodo considerato è stato
              // selezionato
              //--------------------------------------------
              if(CercaNodo(N.n[i]->num)<1 || CercaNodo(N.n[i]->num)>MAXPUNTI) continue;
              //--------------------------------------------
              // Azzegnazione nuova zona (max. 255 zone)
              //--------------------------------------------
              if(Z<=255) N.n[i]->ZoneMap = Z;
              else       N.n[i]->ZoneMap = 255;
              //----------------------------------
              // Almeno una zona riassegnata
              //----------------------------------
              almeno_un_zona=true;
           }
        }
     }
     //---------------------------------------
     // Salvataggio del file dei nodi
     //---------------------------------------
     if(almeno_un_zona==true){
        err = N.save_file_nodi(buffer);
        if(err ) ShowMessage(buffer);
     }        
  }
  //---------------------------------------
  // Conferma modifiche avvenute
  //---------------------------------------
  strcpy(buffer, "!!! The modifications were successful !!!\n\n");
  if(almeno_una_dist==true) strcat(buffer, "- graphic coordinates recalculated.\n- distances recalculated.");
  if(almeno_un_zona==true ) strcat(buffer, "- new mapping zone is assigned.");
  ShowMessage(buffer);
}
//---------------------------------------------------------------------------
//-------------------------------
//   ReloadGRAPHVAR()
//-------------------------------
// Rilettura GRAPHVAR.DAT
//
void TFormXYNodi::ReloadGRAPHVAR(bool messagebox)
{
  int  err=0;
  char buffer[300]="";
  char filename[21]="GRAPHVAR.DAT";
  struct ffblk BlkGRAPHVAR;
  long int TimeFile;

  //-----------------------------------------------
  // Form di visualizzazione stato tabelle aperta
  //-----------------------------------------------
  err = findfirst( filename, &BlkGRAPHVAR, 0);
  if( err<0 ) return;
  //--------------------------------------------------------------------
  // Recupero data/ora file attuale
  //--------------------------------------------------------------------
  TimeFile = (long) BlkGRAPHVAR.ff_fdate;
  TimeFile = (long) (TimeFile<<16);
  TimeFile = (long) (TimeFile | (long) (BlkGRAPHVAR.ff_ftime));

  //--------------------------------------------------------------------
  // Nessuna variazione rilevata --> skip!
  //--------------------------------------------------------------------
  if(TimeFile==TimeGraphvar) return;

  //--------------------------------------------------------------------
  // Recupero da GRAPHVAR.DAT la nuova struttura Punti[][5]
  //--------------------------------------------------------------------
  err = ConvLeggiFileGraphPuntiAscii(filename, buffer);
  if(!err){
     err = ConvLeggiFileGraphPercorsiCurvaAscii(filename, buffer);
  }
  if(!err){
     err = ConvLeggiFileGraphPercorsiCurvaBezierAscii(filename, buffer);
  }
  if(!err){
     err = ConvLeggiFileGraphPercorsiRetta(filename, buffer);
  }

  //-------------------------------------------------------
  // Ricalcolo distanze nodi da grafica appena ricaricata
  //-------------------------------------------------------
  if(!err) err = N.ricalcolo_distanze_nodi(buffer);
  if(!err) sprintf(buffer, "Reload of %s successfully!!!", filename);
  if(err && messagebox==true) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  else                        FormMain->SBarImpianto->Panels->Items[ 0]->Text = buffer;
  //-------------------------------------------------------
  // Se non ci sono stati errori memorizzo la nuova
  // data/ora di importazione
  //-------------------------------------------------------
  TimeGraphvar=TimeFile;
}
//---------------------------------------------------------------------------
void __fastcall TFormXYNodi::udCurvaClick(TObject *Sender, TUDBtnType Button)
{
  udVX->Position  = 0;
  udVX1->Position = 0;
  udVY->Position  = 0;
  udVY1->Position = 0;
  EditXV->Text    = 0;
  EditYV->Text    = 0;
  //-------------------------------------
  // Selezione curva
  //-------------------------------------
  CurvaSelezionata=udCurva->Position;
  Aggiorna();
}
//---------------------------------------------------------------------------

void __fastcall TFormXYNodi::EditXChange(TObject *Sender)
{
  Aggiorna();
}
//---------------------------------------------------------------------------

void __fastcall TFormXYNodi::EditYChange(TObject *Sender)
{
  Aggiorna();
}
//---------------------------------------------------------------------------

void __fastcall TFormXYNodi::udNXClick(TObject *Sender, TUDBtnType Button)
{
  Aggiorna();
}
//---------------------------------------------------------------------------

void __fastcall TFormXYNodi::udNYClick(TObject *Sender, TUDBtnType Button)
{
  Aggiorna();
}
//---------------------------------------------------------------------------

void __fastcall TFormXYNodi::udNX1Click(TObject *Sender, TUDBtnType Button)
{
  //--------------------------------------------
  // Recupero le impostazione OFFSET da EDITOR
  //--------------------------------------------
  if(Button==btNext) udNX->Position+=10;
  if(Button==btPrev) udNX->Position-=10;
  EditX->Text = udNX->Position;
  Aggiorna();
}
//---------------------------------------------------------------------------

void __fastcall TFormXYNodi::udNY1Click(TObject *Sender, TUDBtnType Button)
{
  //--------------------------------------------
  // Recupero le impostazione OFFSET da EDITOR
  //--------------------------------------------
  if(Button==btNext) udNY->Position+=10;
  if(Button==btPrev) udNY->Position-=10;
  EditY->Text = udNY->Position;
  Aggiorna();
}
//---------------------------------------------------------------------------

void __fastcall TFormXYNodi::EditYVChange(TObject *Sender)
{
//  Aggiorna();
}
//---------------------------------------------------------------------------

void __fastcall TFormXYNodi::EditXVChange(TObject *Sender)
{
//  Aggiorna();
}
//---------------------------------------------------------------------------

void __fastcall TFormXYNodi::udVXClick(TObject *Sender, TUDBtnType Button)
{
  if(CurvaSelezionata==9999) return;
  traslazione_vc=true;
  PercorsoCurvaBezier[CurvaSelezionata][3]=udVX->Position;
  Aggiorna();
}
//---------------------------------------------------------------------------

void __fastcall TFormXYNodi::udVYClick(TObject *Sender, TUDBtnType Button)
{
  if(CurvaSelezionata==9999) return;
  traslazione_vc=true;
  PercorsoCurvaBezier[CurvaSelezionata][4]=udVY->Position;
  Aggiorna();
}
//---------------------------------------------------------------------------
void __fastcall TFormXYNodi::udVX1Click(TObject *Sender, TUDBtnType Button)
{
  if(CurvaSelezionata==9999) return;
  traslazione_vc=true;
  if(Button==btNext) udVX->Position+=10;
  if(Button==btPrev) udVX->Position-=10;
  PercorsoCurvaBezier[CurvaSelezionata][3]=udVX->Position;
  Aggiorna();
}
//---------------------------------------------------------------------------

void __fastcall TFormXYNodi::udVY1Click(TObject *Sender, TUDBtnType Button)
{
  if(CurvaSelezionata==9999) return;
  traslazione_vc=true;
  if(Button==btNext) udVY->Position+=10;
  if(Button==btPrev) udVY->Position-=10;
  PercorsoCurvaBezier[CurvaSelezionata][4]=udVY->Position;
  Aggiorna();
}
//---------------------------------------------------------------------------

//============================================================================
//  FUNZIONI DI TRASLAZIONE LAYOUT
//============================================================================
//---------------------------------------------------------------------------
// Traslazione delle coordinate grafiche della quantità specifica in X e Y
//
//
int  TFormXYNodi::TraslazioneGrafica( int X, int Y, char* all_mess)
{
  int  i;
  static bool TraslazioneEseguita=false;

  //-----------------------------------------
  // Permetti una sola traslazione a sessione
  //-----------------------------------------
  if(TraslazioneEseguita==true){
     return 0;
  }
  //-----------------------------------------
  // Traslazione coordinate NODI
  //-----------------------------------------
  if( X!=0 || Y!=0){
     //--------------------------------
     // Traslazione NODI
     //--------------------------------
     for( i=1; i<=MAXPUNTI; i++){
        if( Punti[ i ][2]==0 ) break;     // Stop struttura
        if( Punti[ i ][4]==0 ) continue;  // Leggi il nodo solo se esistente

        //--------------------------------
        // Traslazione in X
        //--------------------------------
        if(Punti[ i ][0]!=0){
           Punti[ i ][0]+=X;  // X
        }
        //--------------------------------
        // Traslazione in Y
        //--------------------------------
        if(Punti[ i ][1]!=0){
           Punti[ i ][1]+=Y;  // Y
        }
     }
     //--------------------------------
     // Traslazione CURVE BEZIER
     //--------------------------------
     //  0 = grado curva
     //  1 = Numero nodo ingresso curva (primo nodo controllo)
     //  2 = Numero nodo uscita curva   (ultimo nodo controllo)
     //  3 = Coordinata X del 2° nodo controllo
     //  4 = Coordinata Y del 2° nodo controllo
     //  5 = Coordinata X del 3° nodo controllo  (solo per CUBICHE)
     //  6 = Coordinata Y del 3° nodo controllo  (solo per CUBICHE)
     //-------------------------------------------------------
     for( i=0; ; i++){
        if( PercorsoCurvaBezier[ i ][0]==0 ) break;     // Stop struttura
        //------------------------------------------
        // Traslazione in X e Y
        // 3° nodo di controllo solo se impostato
        //------------------------------------------
        PercorsoCurvaBezier[ i ][3]+=X;
        PercorsoCurvaBezier[ i ][4]+=Y;
        if(PercorsoCurvaBezier[ i ][5]!=0) PercorsoCurvaBezier[ i ][5]+=X;
        if(PercorsoCurvaBezier[ i ][6]!=0) PercorsoCurvaBezier[ i ][6]+=Y;
     }
     //--------------------------------
     // Traslazione PORTE
     //--------------------------------
     //  1  - Ascissa  (X) per la sigla della porta
     //  2  - Ordinata (Y) per la sigla della porta
     //  3  - Ascissa  (X1) per la visualizzazione della porta
     //  4  - Ordinata (Y1) per la visualizzazione della porta
     //  5  - Ascissa  (X2) per la visualizzazione della porta
     //  6  - Ordinata (Y2) per la visualizzazione della porta
     //-------------------------------------------------------
     for( i=0; ; i++){
        if( i>0 && Porte[ i ][0]==0 ) break;
        Porte[ i ][0]+=X;
        Porte[ i ][1]+=Y;
        Porte[ i ][2]+=X;
        Porte[ i ][3]+=Y;
        Porte[ i ][4]+=X;
        Porte[ i ][5]+=Y;
     }
     //--------------------------------
     // Traslazione SFONDO GRAFICO
     //--------------------------------
     for(i=1; i<=MAXBLOCCHIGRAFICI; i++){
        if(!FormLayout->BloccoG[i].X && !FormLayout->BloccoG[i].Y) break;
        //----------------------------------------------
        // Coordinate e rotazione
        //----------------------------------------------
        FormLayout->BloccoG[i].X+=X;
        FormLayout->BloccoG[i].Y+=Y;
     }
  }
  //-----------------------------------------
  // Aggiornamento dello SFONDO
  //-----------------------------------------
  WriteSfondoGrafico();

  TraslazioneEseguita=true;
  return 0;
}
//---------------------------------------------------------------------------

//-------------------------------------------------------
//      WriteSfondoGrafico()
//-------------------------------------------------------
// Questa funzione salva di nuovo sui files le parti di disegno statico (sfondo)
// (eventualmente modificate in seguito a una traslazione)
//
//
void TFormXYNodi::WriteSfondoGrafico()
{

  int err=0;
  int i, len;
  int handle;
  char filename[301]="SFONDO.DAT";
  bool SempreON=true;

  //--------------------------------------------------------
  // Se il file con lo stato dello Sfondo Grafico è stato
  // aggiornato lo rileggo.
  //--------------------------------------------------------
  if(SempreON==true){
     //--------------------------------------------------------
     // Compilazione nome file degli SFONDO GRAFICO
     //--------------------------------------------------------
     strcpy(filename, "GRAFICA\\SFONDO.DAT");
     //--------------------------------------------------------
     // Apertura file SFONDO.DAT
     //--------------------------------------------------------
     handle = _rtl_open(filename, O_RDWR);
     if(handle<0){
        _rtl_close( handle );
        return;
     }
     //--------------------------------------------------------
     // Scrittura file SFONDO.DAT
     //--------------------------------------------------------
     //len = _rtl_read( handle, &BloccoG, sizeof(BloccoG));
     len = _rtl_write( handle, &FormLayout->BloccoG, sizeof(FormLayout->BloccoG));
     _rtl_close( handle );
     if(len != sizeof(FormLayout->BloccoG)) err=1;
     if(err) {
        return;
     }
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Traslazione delle coordinate grafiche della quantità specifica in X e Y
//
//
void TFormXYNodi::TraslazioneCurveGrafica(int X, int Y)
{
  int  i, j;
  bool NodoPresente1, NodoPresente2;
  bool NoTraslazionX, NoTraslazionY;
  bool AlmenoUnaModifica;
  int  IndiceCurva;
  int  NodoCurva1, NodoCurva2;
  int  XNodo1, YNodo1, XNodo2, YNodo2, XNodoControllo, YNodoControllo;
  int distX, distY;

  if(CurveDiBezier==0) return;
  AlmenoUnaModifica=false;
  //-----------------------------------------
  // Traslazione CURVE BEZIER
  //-----------------------------------------
  //  0 = grado curva
  //  1 = Numero nodo ingresso curva (primo nodo controllo)
  //  2 = Numero nodo uscita curva   (ultimo nodo controllo)
  //  3 = Coordinata X del 2° nodo controllo
  //  4 = Coordinata Y del 2° nodo controllo
  //  5 = Coordinata X del 3° nodo controllo  (solo per CUBICHE)
  //  6 = Coordinata Y del 3° nodo controllo  (solo per CUBICHE)
  //-------------------------------------------------------
  for( i=0; ; i++){
     IndiceCurva=i;
     if( PercorsoCurvaBezier[ i ][0]==0 ) break;     // Stop struttura
     NodoCurva1= XY_PercorsoCurvaBezier[ i ][1];
     NodoCurva2= XY_PercorsoCurvaBezier[ i ][2];
     NodoPresente1=NodoPresente2=false;
     NoTraslazionX=NoTraslazionY=false;
     for(j=0; j<=MAXPUNTI; j++){
        if(NodoPresente1==true && NodoPresente2==true) break;
        //if(ElencoNodi[j][0]==0) break; Occorre controllarli tutti
        if(ElencoNodi[j][1]==1 && ElencoNodi[j][0]==NodoCurva1 && NodoCurva1>0) NodoPresente1=true;
        if(ElencoNodi[j][1]==1 && ElencoNodi[j][0]==NodoCurva2 && NodoCurva2>0) NodoPresente2=true;
     }
     if(NodoPresente1==false && NodoPresente2==false) continue;
     //--------------------------------
     // Recupera coordinate Vertici
     //--------------------------------
     XNodo1=XY_Punti[NodoCurva1][0];  // X
     YNodo1=XY_Punti[NodoCurva1][1];  // Y
     XNodo2=XY_Punti[NodoCurva2][0];  // X
     YNodo2=XY_Punti[NodoCurva2][1];  // Y
     XNodoControllo=XY_PercorsoCurvaBezier[ i ][3];
     YNodoControllo=XY_PercorsoCurvaBezier[ i ][4];

     //---------------------------------------------------
     // Spostamento solo vertice 1
     //---------------------------------------------------
     IndiceCurva=IndiceCurva;
     if(NodoPresente1==true && NodoPresente2==false){
        //--------------------------------
        // Recupera coordinate precedenti
        //--------------------------------
        XNodo1=XY_Punti[NodoCurva1][0]+X;         // X
        YNodo1=XY_Punti[NodoCurva1][1]+(Y*(-1));  // Y
        distX = abs(XNodo1-XNodoControllo);
        distY = abs(YNodo1-YNodoControllo);
        //--------------------------------------------------------
        // Se spostamento in X e vertice appoggiato su Y --> SKIP
        //--------------------------------------------------------
        if(distY<50) NoTraslazionX=true;
        //--------------------------------------------------------
        // Se spostamento in Y e vertice appoggiato su X --> SKIP
        //--------------------------------------------------------
        if(distX<50) NoTraslazionY=true;
     }
     //---------------------------------------------------
     // Spostamento solo vertice 2
     //---------------------------------------------------
     IndiceCurva=IndiceCurva;
     if(NodoPresente1==false && NodoPresente2==true){
        //--------------------------------
        // Recupera coordinate precedenti
        //--------------------------------
        XNodo2=XY_Punti[NodoCurva2][0]+X;         // X
        YNodo2=XY_Punti[NodoCurva2][1]+(Y*(-1));  // Y
        distX = abs(XNodo2-XNodoControllo);
        distY = abs(YNodo2-YNodoControllo);
        //--------------------------------------------------------
        // Se spostamento in X e vertice appoggiato su Y --> SKIP
        //--------------------------------------------------------
        if(distY<50 ) NoTraslazionX=true;
        //--------------------------------------------------------
        // Se spostamento in Y e vertice appoggiato su X --> SKIP
        //--------------------------------------------------------
        if(distX<50 ) NoTraslazionY=true;
     }

     //------------------------------------------
     // Traslazione in X e Y
     // 3° nodo di controllo solo se impostato
     //------------------------------------------
     IndiceCurva=IndiceCurva;
     if(NoTraslazionX==false) AlmenoUnaModifica=true;
     if(NoTraslazionY==false) AlmenoUnaModifica=true;
     if(AlmenoUnaModifica){
        if(NoTraslazionX==false){
           PercorsoCurvaBezier[ i ][3]=XY_PercorsoCurvaBezier[ i ][3]+X;
           if(XY_PercorsoCurvaBezier[ i ][5]!=0) PercorsoCurvaBezier[ i ][5]=XY_PercorsoCurvaBezier[ i ][5]+X;
        }
        if(NoTraslazionY==false){
           PercorsoCurvaBezier[ i ][4]=XY_PercorsoCurvaBezier[ i ][4]+(Y*(-1));
           if(XY_PercorsoCurvaBezier[ i ][6]!=0) PercorsoCurvaBezier[ i ][6]=XY_PercorsoCurvaBezier[ i ][6]+(Y*(-1));
        }
     }
  }
  return;
}
//---------------------------------------------------------------------------

//============================================================================
//  FUNZIONE DI LETTURA FILE GRAHVAR.DAT DI GRAFICA
//============================================================================
// --------------------------
//  STRUTTURA NODI  Grafici
// --------------------------
//  0, 1 = x,y posizione centrale nodo
//  2 = numero nodo
//  3 = occupazione agv ( numero AGV oppure libero = 0 )
//  4 = PAGINA 1-2-3... (Numero pagina grafica di cui il nodo fa parte)
//--------------------------------------------------------------------
int PuntiGraf[MAXPUNTI+10][5];
// --------------------------
//  STRUTTURA PercorsoCurva
// --------------------------
//  0 = Numero nodo partenza
//  1 = Numero nodo arrivo        _         _
//  2 = Quadrante:             1 =  |   2 = |    3 = |_   4 = _|
//  3 = Raggio   :    espresso in cm come i nodi ( SE 0 se la calcola la funzione
//                    presupponendo che la curva parta tangente a X o Y )
//  4 = PAGINA 1-2-3... (Numero pagina grafica di cui la curva è da visualizzare)
//--------------------------------------------------------------------
int PercorsoCurvaGraf[MAX_TRACCE_CURVA_DWG+10][5];
// --------------------------
//  STRUTTURA Curva BEZIER
// --------------------------
//  0 = grado curva
//      0 - disattivata
//      1 - curva lineare (no curva = linea)
//      2 - quadratica ( 3 vertici di controllo)
//      3 - cubica     ( 4 vertici di controllo)
//  1 = Numero nodo ingresso curva (primo nodo controllo)
//  2 = Numero nodo uscita curva   (ultimo nodo controllo)
//  3 = Coordinata X del 2° nodo controllo
//  4 = Coordinata Y del 2° nodo controllo
//  5 = Coordinata X del 3° nodo controllo  (solo per CUBICHE)
//  6 = Coordinata Y del 3° nodo controllo  (solo per CUBICHE)
//--------------------------------------------------------------------
int PercorsoCurvaBezierGraf[MAX_TRACCE_CURVA_DWG+10][7];

// -----------------------
//  STRUTTURA PERCORSO
// -----------------------
//  0 = Numero nodo partenza
//  1 = Numero nodo arrivo
////-------------------------------------------------------
int PercorsoRettaGraf[MAXPUNTI][2];

//---------------------------------------------------------------------------
// Rileggi un file ascii in linguaggio "c" con le informazioni
// grafiche della struttura Punti[][]
// se la conversione va a buon fine compila la struttura in memoria
//
//
int  ConvLeggiFileGraphPuntiAscii( char *NomeFile, char* all_mess)
// **************************************************************** //
{
  FILE *ptrfile;
//  char StringaComando[120];
  int  StartPos, EndPos;
  int  ErroreConversione;
  int  IntestazioneTrovata;
  int  i, j, x;
  int  err;
  char buffer [300];
  char buffer1[300];

  //-------------------------------------------------------
  // Apri il file   GraphPunti.dat
  //-------------------------------------------------------
  IntestazioneTrovata=0;
  memset( &PuntiGraf, 0, sizeof( PuntiGraf ));
  //sprintf( StringaComando, "%s%s", PercorsoDwg, NomeFile );
  ptrfile = fopen( NomeFile, "rb");
  if (ptrfile<=0){
     sprintf( all_mess, "Errore apertura file %s", NomeFile );
     return 1;
  }else{
     //-------------------------------------------------------
     // STRUTTURA PUNTI
     // Recupera tutte  le stringhe e ricerca
     //-------------------------------------------------------
     err=0;
     ErroreConversione=0;
     for( i=1; ; i++){
        //-----------------------------------------------------------------------
        // Recupero Intestazione struttura  PuntiGraf[][]
        //-----------------------------------------------------------------------
        err=!Read_Line( ptrfile, buffer, 100);   if(err) break;
        Erase_White_Begin( buffer );
        if( strcmp( buffer, "int Punti[][5]={")==0 ){
           //------------------------------------------------------------
           // Recupero le riga dei record  struttura  PuntiGraf[][]
           //------------------------------------------------------------
           IntestazioneTrovata=1;
           for( j=0; j<MAXPUNTI; j++){
              err=!Read_Line( ptrfile, buffer, 100); if(err) break;
              Erase_White_Begin( buffer );
              if(strlen( buffer)==0) {j--; continue;}
              if( buffer[0]=='/' &&  buffer[1]=='/') {j--; continue;}
              if( buffer[0] =='}')  break;
              if( buffer[0] !='{') {
                 sprintf(all_mess, "Errore generico nella conversione PuntiGraf[] Indice%d", j);
                 ErroreConversione=1;
                 break;
              }
              StartPos=2;
              //------------------------------------------------------------
              // Recupero i campi della struttura  PuntiGraf[][]
              //------------------------------------------------------------
              for( x=0; x<=4; x++){
                 EndPos  = Pos( ',', buffer);
                 Copy( buffer, StartPos, EndPos, buffer1 );
                 buffer1[15]=0;
                 if( EndPos>90 ) {
                    sprintf(all_mess, "Errore generico nella conversione PuntiGraf[] Indice%d", j);
                    ErroreConversione=1;
                    break;
                 }
                 Erase_White(buffer1);
                 PuntiGraf[ j ][x] = atoi( buffer1 );
                 if( EndPos>0 ) buffer[ EndPos-1 ] =' ';
                 StartPos=EndPos+1;
              }
              if(ErroreConversione) break;
              if( PuntiGraf[ j ][ 2 ] != j ) {
                 if(( PuntiGraf[ j ][ 2 ] != 0 ) && ( PuntiGraf[ j ][ 4 ] != 0 )) {
                    sprintf(all_mess, "Indice campo %d della struttura PuntiGraf[] non corretta", j);
                    //ShowMessage(all_mess);
                    ErroreConversione=1;
                    break;
                 }
              }
           }
           if(IntestazioneTrovata==0){
              sprintf(all_mess, "Intestazione dalla struttura PuntiGraf[] non trovata");
              //ShowMessage(all_mess);
              ErroreConversione=1;
           }
           if(err) break;
           if(ErroreConversione) break;
        }
        if(err) break;
        if(ErroreConversione) break;
     }
     //--------------------------------------
     // Aggiornamento struttura globale
     //--------------------------------------
     if(!ErroreConversione && IntestazioneTrovata==1){
        for( i=1; i<=MAXPUNTI; i++){
           if( Punti[ i ][2]==0 ) break;     // Stop struttura
           //if( Punti[ i ][4]==0 ) continue;  // Leggi il nodo solo se esistente
           if( Punti[ i ][4]==0 && PuntiGraf[ i ][4]==0) continue;  // 19/05/2017 Leggi il nodo anche se nella struttura non Esisteva
           Punti[ i ][0] = PuntiGraf[ i ][0];
           Punti[ i ][1] = PuntiGraf[ i ][1];
           Punti[ i ][2] = PuntiGraf[ i ][2];
           Punti[ i ][3] = PuntiGraf[ i ][3];
           Punti[ i ][4] = PuntiGraf[ i ][4];
        }
     }
  }
  fclose(ptrfile);
  return ErroreConversione;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Scrive un file ascii in linguaggio "c" con le informazioni
// grafiche della struttura Punti[][]
// se la conversione va a buon fine compila il file con la struttura in
// memoria
//
//
int  ConvScriviFileGraphPuntiAscii( char *NomeFile, char* all_mess)
// **************************************************************** //
{
  int  i;
  FILE *ptrfile;
  char buffer [900];
  char buffer1[200];
  int  NumeroNodoDwg;
  int  EscuzioneNotOK =0;



  //-------------------------------------------------------------------
  // --(8)--
  //
  // Compilazione della struttura PuntiGraf[][]
  // E' una struttura uguale a quella utilizzata nei programmi  TGV
  // compilata in modo coerente, la lunghezza della struttura è
  // dimensionata in modo fisso a 2000 per ovvi motivi di portabilità
  //-------------------------------------------------------------------
  if( !EscuzioneNotOK ){
     //----------------------------------------------------------
     // --(8.1)-- Struttura Punti
     //
     // Esegue le operazioni di conversione
     //----------------------------------------------------------
     memset( &PuntiGraf, 0, sizeof( PuntiGraf ));
     for( i=1; i<=MAXPUNTI; i++){
        if( Punti[i][0]==0 && Punti[i][1]==0  && Punti[i][2]==0 ) break;  // Stop struttura
        NumeroNodoDwg = Punti[ i ][2];
        if(NumeroNodoDwg<MAXPUNTI){
           if( PuntiGraf[ NumeroNodoDwg ][2]!=0 ){
              sprintf( all_mess, "         ATTENZIONE: il nodo %d è duplicato (2° NON preso in considerazione)", NumeroNodoDwg );
              ShowMessage(all_mess);
              //EscuzioneNotOK=1;
              //break;
           }
           PuntiGraf[ NumeroNodoDwg ][0] = Punti[ i ][0]; //X
           PuntiGraf[ NumeroNodoDwg ][1] = Punti[ i ][1]; //Y
           PuntiGraf[ NumeroNodoDwg ][2] = NumeroNodoDwg; //Nodo
           PuntiGraf[ NumeroNodoDwg ][3] = Punti[ i ][3]; //Nodo sovrapposto
           PuntiGraf[ NumeroNodoDwg ][4] = Punti[ i ][4]; //Pagina
        }
     }
     //-------------------------------------------------------
     // --(8.2)--
     // Compila un file di testo  Con i dati così ottenuti
     //
     //    Passo_8 :  GraphPunti.dat
     //
     //-------------------------------------------------------
     if( !EscuzioneNotOK ){
        //-------------------------------------------------------
        // Apri il file   Passo_8    GraphVar.dat
        //-------------------------------------------------------
        ptrfile = fopen( NomeFile, "wb");
        if (ptrfile<=0){
           sprintf( all_mess, "Errore apertura file %s", NomeFile);
           //ShowMessage(all_mess);
           EscuzioneNotOK=1;
        }
        if(!EscuzioneNotOK){
           //-------------------------------------------------------------------
           // Compilazione struttura "offset"
           // La struttura viene PRECOMPILATA con le coordinate della finestra
           // in alto a SX per ridurre la prima visualizzazione nei
           // limiti dello schermo
           //-------------------------------------------------------------------
           strcpy( buffer, "// ----------------------------------------------" );                      Write_Line(ptrfile, buffer);
           strcpy( buffer, "//  Sfasamento Visualizzazione coordinate layout " );                      Write_Line(ptrfile, buffer);
           strcpy( buffer, "// ----------------------------------------------" );                      Write_Line(ptrfile, buffer);
           strcpy( buffer, "");                                                                        Write_Line(ptrfile, buffer);
           sprintf( buffer, "#define    OFFSO1  %d", offset[1][0]);                                    Write_Line(ptrfile, buffer);
           sprintf( buffer, "#define    OFFSV1  %d", offset[1][1]);                                    Write_Line(ptrfile, buffer);
           strcpy( buffer, "");                                                                        Write_Line(ptrfile, buffer);
           strcpy( buffer, "int   OffSV1 = OFFSV1;");                                                  Write_Line(ptrfile, buffer);
           strcpy( buffer, "int   OffSO1 = OFFSO1;");                                                  Write_Line(ptrfile, buffer);
           strcpy( buffer, "");                                                                        Write_Line(ptrfile, buffer);
           strcpy( buffer, "");                                                                        Write_Line(ptrfile, buffer);
           strcpy( buffer, "// -----------------------" );                                             Write_Line(ptrfile, buffer);
           strcpy( buffer, "//  STRUTTURA OFFSET      " );                                             Write_Line(ptrfile, buffer);
           strcpy( buffer, "// -----------------------" );                                             Write_Line(ptrfile, buffer);
           strcpy( buffer, "//  Riporta la finestra visualizzata nei limiti dello schermo");           Write_Line(ptrfile, buffer);
           strcpy( buffer, "//--------------------------------------------------------------------");  Write_Line(ptrfile, buffer);
           strcpy( buffer, "int offset[][2] = { {      0,          0 },");                             Write_Line(ptrfile, buffer);
           strcpy( buffer, "                    { OFFSO1,     OFFSV1 },     // variabile");            Write_Line(ptrfile, buffer);
           strcpy( buffer, "};");                                                                      Write_Line(ptrfile, buffer);
           strcpy( buffer, "");                                                                        Write_Line(ptrfile, buffer);
           strcpy( buffer, "");                                                                        Write_Line(ptrfile, buffer);

           //-------------------------------------------------------
           // Compilazione struttura NODI  Passo_8
           //-------------------------------------------------------
           strcpy( buffer, "// -----------------------" );                                             Write_Line(ptrfile, buffer);
           strcpy( buffer, "//  STRUTTURA NODI");                                                      Write_Line(ptrfile, buffer);
           strcpy( buffer, "// -----------------------" );                                             Write_Line(ptrfile, buffer);
           strcpy( buffer, "//  0, 1 = x,y posizione centrale nodo");                                  Write_Line(ptrfile, buffer);
           strcpy( buffer, "//  2 = numero nodo");                                                     Write_Line(ptrfile, buffer);
           strcpy( buffer, "//  3 = occupazione agv ( numero AGV oppure libero = 0 )");                Write_Line(ptrfile, buffer);
           strcpy( buffer, "//  4 = Visualizzazione 0-Mai 1-Sempre 2-Solo a un certo valore di Zoom"); Write_Line(ptrfile, buffer);
           strcpy( buffer, "//--------------------------------------------------------------------");  Write_Line(ptrfile, buffer);
           strcpy( buffer, "int Punti[][5]={");                                                        Write_Line(ptrfile, buffer);
           //-------------------------------------------------------------------
           // Compilazione struttura dei nodi grafici
           //-------------------------------------------------------------------
           for( i=0; i<MAXPUNTI; i++){
              if( PuntiGraf[i][0]==0 && PuntiGraf[i][1]==0  && PuntiGraf[i][2]==0 ) PuntiGraf[ i ][2]=i;
              sprintf( buffer, "{% 11d, %11d ,%4d,  %d,  %d},",
                       PuntiGraf[ i ][0],
                       PuntiGraf[ i ][1],
                       PuntiGraf[ i ][2],
                       PuntiGraf[ i ][3],
                       PuntiGraf[ i ][4] );

              sprintf( buffer1, " //%03d  Nodo", i );
              if( i<21 )           sprintf( buffer1, " //%03d  riposo AGV %d", i, i);
              if( i>100 && i<201)  sprintf( buffer1, " //%d  Macchine", i);
              if( i>200 && i<500)  sprintf( buffer1, " //%d  Magazzini", i);
              strcat( buffer, buffer1 );
              Write_Line(ptrfile, buffer);
           }
           strcpy( buffer, "{          0,           0 ,   0,   0, 0}, //0");                 Write_Line(ptrfile, buffer);
           strcpy( buffer, "};");                                                            Write_Line(ptrfile, buffer);
           strcpy( buffer, "");                                                              Write_Line(ptrfile, buffer);
           strcpy( buffer, "");                                                              Write_Line(ptrfile, buffer);
        }


        //-------------------------------------------------------
        // --(9.4)--
        // Compila un file di testo  Con i dati così ottenuti
        //
        //    Passo_9_4 :  GraphPercorsi[][]
        //
        //-------------------------------------------------------
        if( EscuzioneNotOK==0){
           //-------------------------------------------------------
           // Compilazione file   Passo_9 GraphPercorsi.dat
           //-------------------------------------------------------
           sprintf( buffer, "// -----------------------" );                                             Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  STRUTTURA PERCORSO");                                                  Write_Line(ptrfile, buffer);
           sprintf( buffer, "// -----------------------" );                                             Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  0 = Numero nodo partenza");                                            Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  1 = Numero nodo arrivo");                                              Write_Line(ptrfile, buffer);
           sprintf( buffer, "//");                                                                      Write_Line(ptrfile, buffer);
           sprintf( buffer, "//-------------------------------------------------------");             Write_Line(ptrfile, buffer);
           sprintf( buffer, "int Percorso[][2]={");                                                     Write_Line(ptrfile, buffer);

           for( i=0; ; i++){
              //if( Percorso[ i ][0]==0 ) break;
              sprintf( buffer, "{  %7d, %5d },  // %d",
                       Percorso[ i ][0],
                       Percorso[ i ][1],
                       i );
              Write_Line(ptrfile, buffer);
              if( Percorso[ i ][0]==0 ) break;
           }
           sprintf( buffer, "};");                             Write_Line(ptrfile, buffer);
           sprintf( buffer, "");                               Write_Line(ptrfile, buffer);
           sprintf( buffer, "");                               Write_Line(ptrfile, buffer);
        }

        //-------------------------------------------------------------------
        // --(9.5)--
        // Compila un file di testo con le curve Con i dati così ottenuti
        //
        //    Passo_9_5 :  PercorsoCurva[][]
        //
        //-------------------------------------------------------------------
        if( EscuzioneNotOK==0){
           //------------------------------------------------------------------------------
           // Campo Array TracceCurvaDwg
           // 0   Tipo del Blocco/Quadrante 1..4
           // 1   Riga del file DXF dove inizia intestazione Blocco  ecc.
           // 2   Coordinata Pc(X) del centro
           // 3   Coordinata Pc(Y) del centro
           // 4   Raggio
           // 5   Angolo Iniziale in gradi
           // 6   Angolo finale in gradi
           // 7   Coordinata P1(X) del punto iniziale (calcolato)
           // 8   Coordinata P1(Y) del punto iniziale (calcolato)
           // 9   Coordinata P2(X) del punto finale   (calcolato)
           //10   Coordinata P2(Y) del punto finale   (calcolato)
           //11   Nodo di partenza
           //12   Nodo di arrivo
           //
           //------------------------------------------------------------------------------
           //int TracceCurvaDwg[MAX_TRACCE_CURVA_DWG+10][14];

           //-------------------------------------------------------
           // Compilazione file   Passo_9 GraphPercorsi.dat
           //-------------------------------------------------------
           sprintf( buffer, "// ---------------------------" );                                                              Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  STRUTTURA PercorsoCurva");                                                                  Write_Line(ptrfile, buffer);
           sprintf( buffer, "// ---------------------------" );                                                              Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  0 = Numero nodo partenza");                                                                 Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  1 = Numero nodo arrivo         _         _                  ");                             Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  2 = Quadrante:             1 =  |   2 = |    3 = |_   4 = _|");                             Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  3 = Raggio   :    espresso in cm come i nodi ( SE 0 se la calcola la funzione");            Write_Line(ptrfile, buffer);
           sprintf( buffer, "//                    presupponendo che la curva parta tangente a X o Y");                      Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  4 = Tipo Visualizzazione: 0 = Curva da NON VISUALIZZARE (INESISTENTE!!)");                  Write_Line(ptrfile, buffer);
           sprintf( buffer, "//                            1 = Curva da Visualizzare SEMPRE");                               Write_Line(ptrfile, buffer);
           sprintf( buffer, "//                            2 = Curva da Visualizzare SOLO se INTERESSATA DA MISSIONE ...");  Write_Line(ptrfile, buffer);
           sprintf( buffer, "//                            3 = ......");                                                     Write_Line(ptrfile, buffer);
           sprintf( buffer, "//-------------------------------------------------------");                                    Write_Line(ptrfile, buffer);
           sprintf( buffer, "int PercorsoCurva[][5]={");                                                                     Write_Line(ptrfile, buffer);

           for( i=0; ; i++){

              sprintf( buffer, "{  %7d, %5d, %5d, %5d, %5d },  // %d",
                       PercorsoCurva[ i ][ 0],
                       PercorsoCurva[ i ][ 1],
                       PercorsoCurva[ i ][ 2],
                       PercorsoCurva[ i ][ 3],
                       PercorsoCurva[ i ][ 4],
                       i+1 );
              Write_Line(ptrfile, buffer);
              if( PercorsoCurva[ i ][0]==0 ) break;
           }
           sprintf( buffer, "};");                             Write_Line(ptrfile, buffer);
           sprintf( buffer, "");                               Write_Line(ptrfile, buffer);
           sprintf( buffer, "");                               Write_Line(ptrfile, buffer);
        }

        //-------------------------------------------------------------------
        // --(9.6)-- CURVE BEZIER
        // Compila un file di testo con le curve Con i dati così ottenuti
        //
        //    Passo_9_6 :  PercorsoCurvaBezierGraf[][]
        //
        //-------------------------------------------------------------------
        if( EscuzioneNotOK==0){
           //------------------------------------------------------------------------------
           // Campo Array TracceCurvaBezierDwg
           // 0   Tipo Curva: 2 = Quadratica, 3 = Cubica (NON ANCORA IMPLEMENTATE)
           // 1   Riga del file DXF dove inizia intestazione Blocco  ecc.
           // 2   Coordinata P1(X) del punto iniziale
           // 3   Coordinata P1(Y) del punto iniziale
           // 4   Coordinata P2(X) del vertice controllo 1
           // 5   Coordinata P2(Y) del vertice controllo 1
           // 6   Coordinata P3(X) del vertice controllo 2 (solo per cubiche)
           // 7   Coordinata P3(Y) del vertice controllo 2 (solo per cubiche)
           // 8   Coordinata P4(X) del punto finale
           // 9   Coordinata P4(Y) del punto finale
           //10   Nodo di partenza
           //11   Nodo di arrivo
           //
           //------------------------------------------------------------------------------
           //int TracceCurvaBezierDwg[MAX_TRACCE_CURVA_DWG+10][13];


           //-------------------------------------------------------
           // Compilazione file   Passo_9 GraphPercorsi.dat
           //-------------------------------------------------------
           sprintf( buffer, "// ---------------------------" );                                                              Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  STRUTTURA Curva BEZIER");                                                                   Write_Line(ptrfile, buffer);
           sprintf( buffer, "// ---------------------------" );                                                              Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  0 = grado curva");                                                                          Write_Line(ptrfile, buffer);
           sprintf( buffer, "//      0 - disattivata");                                                                      Write_Line(ptrfile, buffer);
           sprintf( buffer, "//      1 - curva lineare (no curva = linea)");                                                 Write_Line(ptrfile, buffer);
           sprintf( buffer, "//      2 - quadratica ( 3 vertici di controllo)");                                             Write_Line(ptrfile, buffer);
           sprintf( buffer, "//      3 - cubica     ( 4 vertici di controllo)");                                             Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  1 = Numero nodo ingresso curva (primo nodo controllo)");                                    Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  2 = Numero nodo uscita curva   (ultimo nodo controllo)");                                   Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  3 = Coordinata X del 2° nodo controllo");                                                   Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  4 = Coordinata Y del 2° nodo controllo");                                                   Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  5 = Coordinata X del 3° nodo controllo  (solo per CUBICHE)");                               Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  6 = Coordinata Y del 3° nodo controllo  (solo per CUBICHE)");                               Write_Line(ptrfile, buffer);
           sprintf( buffer, "//-------------------------------------------------------");                                    Write_Line(ptrfile, buffer);
           sprintf( buffer, "int PercorsoCurvaBezier[][7]={");                                                               Write_Line(ptrfile, buffer);

           for( i=0; ; i++){
              sprintf( buffer, "{  %5d, %5d, %5d, %7d, %7d, %7d, %7d },  // %d",
                       PercorsoCurvaBezier[ i ][ 0],
                       PercorsoCurvaBezier[ i ][ 1],
                       PercorsoCurvaBezier[ i ][ 2],
                       PercorsoCurvaBezier[ i ][ 3],
                       PercorsoCurvaBezier[ i ][ 4],
                       PercorsoCurvaBezier[ i ][ 5],
                       PercorsoCurvaBezier[ i ][ 6],
                       i+1);
              Write_Line(ptrfile, buffer);
              if( PercorsoCurvaBezier[ i ][0]==0 ) break;
           }
           sprintf( buffer, "};");                             Write_Line(ptrfile, buffer);
           sprintf( buffer, "");                               Write_Line(ptrfile, buffer);
           sprintf( buffer, "");                               Write_Line(ptrfile, buffer);
        }
        //-------------------------------------------------------------------
        // --(9.6)-- PORTE
        // Compila un file di testo con le curve Con i dati così ottenuti
        //
        //    Passo_9_6 :  PercorsoCurvaBezierGraf[][]
        //
        //-------------------------------------------------------------------
        if( EscuzioneNotOK==0){
           //-------------------------------------------------------
           // Compilazione file   Porte
           //-------------------------------------------------------
           sprintf( buffer, "// ---------------------------" );                                                              Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  STRUTTURA PORTE");                                                                   Write_Line(ptrfile, buffer);
           sprintf( buffer, "// ---------------------------" );                                                              Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  1  - Ascissa  (X) per la sigla della porta");                                                                 Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  2  - Ordinata (Y) per la sigla della porta");                                                                Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  3  - Ascissa  (X1) per la visualizzazione della porta");                                                 Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  4  - Ordinata (Y1) per la visualizzazione della porta");                                          Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  5  - Ascissa  (X2) per la visualizzazione della porta");                                            Write_Line(ptrfile, buffer);
           sprintf( buffer, "//  6  - Ordinata (Y2) per la visualizzazione della porta");                                   Write_Line(ptrfile, buffer);
           sprintf( buffer, "//-------------------------------------------------------");                                    Write_Line(ptrfile, buffer);
           sprintf( buffer, "int Porte[][6]={");                                                               Write_Line(ptrfile, buffer);

           for( i=0; ; i++){
              sprintf( buffer, "{  %5d, %5d, %5d, %7d, %7d, %7d },  // %d",
                       Porte[ i ][ 0],
                       Porte[ i ][ 1],
                       Porte[ i ][ 2],
                       Porte[ i ][ 3],
                       Porte[ i ][ 4],
                       Porte[ i ][ 5],
                       i );
              Write_Line(ptrfile, buffer);
              if( i>0 && Porte[ i ][0]==0 ) break;
           }
           sprintf( buffer, "};");                             Write_Line(ptrfile, buffer);
           sprintf( buffer, "");                               Write_Line(ptrfile, buffer);
           sprintf( buffer, "");                               Write_Line(ptrfile, buffer);
        }
     }
  }
  fclose(ptrfile);
  if( EscuzioneNotOK ) return 1;
  return 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Rileggi un file ascii in linguaggio "c" con le informazioni
// grafiche della struttura PercorsoCurva[][]
// se la conversione va a buon fine compila la struttura in memoria
//
//
int  ConvLeggiFileGraphPercorsiCurvaAscii( char *NomeFile, char* all_mess)
/*****************************************************************/
{
  int  StartPos, EndPos;
  int  ErroreConversione;
  int  IntestazioneTrovata;
  int  i, j, x;
  int  err;

  FILE *ptrfile;
  char buffer [300];
  char buffer1[300];


  //-------------------------------------------------------
  // Apri il file   GraphPercorsi.dat
  //-------------------------------------------------------
  IntestazioneTrovata=0;
  memset( &PercorsoCurvaGraf, 0, sizeof( PercorsoCurvaGraf ));
  ptrfile = fopen( NomeFile, "rb");
  if (ptrfile<=0){
     sprintf( all_mess, "Errore apertura file %s", NomeFile );
     //ShowMessage(all_mess);
     return 1;
  }else{
     //-------------------------------------------------------
     // Recupera tutte  le stringhe e ricerca
     //-------------------------------------------------------
     err=0;
     ErroreConversione=0;
     for( i=1; ; i++){
        //-----------------------------------------------------------------------
        // Recupero Intestazione struttura  PercorsoCurvaGraf[][]
        //-----------------------------------------------------------------------
        err=!Read_Line( ptrfile, buffer, 100);   if(err) break;
        Erase_White_Begin( buffer );
        if( strcmp( buffer, "int PercorsoCurva[][5]={")==0 ){
           //------------------------------------------------------------
           // Recupero le riga dei record  struttura  PercorsoCurvaGraf[][]
           //------------------------------------------------------------
           IntestazioneTrovata=1;
           for( j=0; j<MAX_TRACCE_CURVA_DWG; j++){
              err=!Read_Line( ptrfile, buffer, 100);   if(err) break;
              Erase_White_Begin( buffer );
              if(strlen( buffer)==0) {j--; continue;}
              if( buffer[0]=='/' &&  buffer[1]=='/') {j--; continue;}
              if( buffer[0] =='}')  break;
              if( buffer[0] !='{') {
                 sprintf(all_mess, "Errore generico nella conversione PercorsoCurvaGraf[] Indice%d", i);
                 //ShowMessage(all_mess);
                 ErroreConversione=1;
                 break;
              }
              StartPos=2;
              //------------------------------------------------------------
              // Recupero i campi della struttura  PercorsoCurvaGraf[][]
              //------------------------------------------------------------
              for( x=0; x<=4; x++){
                 EndPos  = Pos( ',', buffer);
                 Copy( buffer, StartPos, EndPos, buffer1 );
                 buffer1[15]=0;
                 if( EndPos>90 ) {
                    sprintf(all_mess, "Errore generico nella conversione PercorsoCurvaGraf[] Indice%d", i);
                    //ShowMessage(all_mess);
                    ErroreConversione=1;
                    break;
                 }
                 PercorsoCurvaGraf[ j ][x] = atoi( buffer1 );
                 if( EndPos>0 ) buffer[ EndPos-1 ] =' ';
                 StartPos=EndPos+1;
              }

              if(ErroreConversione) break;
              if((( PercorsoCurvaGraf[ j ][ 0 ] != 0 ) && ( PercorsoCurvaGraf[ j ][ 1 ] == 0 ))||
                 (( PercorsoCurvaGraf[ j ][ 0 ] == 0 ) && ( PercorsoCurvaGraf[ j ][ 1 ] != 0 ))
              ) {
                 sprintf(all_mess, "Indice campo %d della struttura PerocorsoCurvaGraf[] non corretta", j);
                 //ShowMessage(all_mess);
                 //ErroreConversione=1;
                 //break;
                 continue;
              }
           }
           if(err) break;
           if(ErroreConversione) break;
           if(IntestazioneTrovata==1) break;
        }
        if(err) break;
        if(ErroreConversione) break;
        if(IntestazioneTrovata==1) break;
     }
     if(  IntestazioneTrovata==0){
        sprintf(all_mess, "Intestazione dalla struttura PerocorsoCurvaGraf[] non trovata");
        //ShowMessage(all_mess);
        ErroreConversione=1;
     }

     //-------------------------------------------------------
     // Aggiorno la struttura grafica per la visualizzazione
     //-------------------------------------------------------
     if(! ErroreConversione) {
        for( i=0; i<=MAX_TRACCE_CURVA_DWG; i++){
           PercorsoCurva[ i ][0] = PercorsoCurvaGraf[ i ][0];
           PercorsoCurva[ i ][1] = PercorsoCurvaGraf[ i ][1];
           PercorsoCurva[ i ][2] = PercorsoCurvaGraf[ i ][2];
           PercorsoCurva[ i ][3] = PercorsoCurvaGraf[ i ][3];
           PercorsoCurva[ i ][4] = PercorsoCurvaGraf[ i ][4];
           if( PercorsoCurvaGraf[ i ][0]==0 && PercorsoCurvaGraf[ i ][1]==0 && PercorsoCurvaGraf[ i ][2]==0) break;
        }
     }
  }
  fclose(ptrfile);
  return ErroreConversione;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Rileggi un file ascii in linguaggio "c" con le informazioni
// grafiche della struttura Percorso[][2]
// se la conversione va a buon fine compila la struttura in memoria
//
//
int  ConvLeggiFileGraphPercorsiRetta( char *NomeFile, char* all_mess)
/*****************************************************************/
{
  int  StartPos, EndPos;
  int  ErroreConversione;
  int  IntestazioneTrovata;
  int  i, j, x;
  int  err;

  FILE *ptrfile;
  char buffer [300];
  char buffer1[300];

  //-------------------------------------------------------
  // Apri il file   GraphPercorsi.dat
  //-------------------------------------------------------
  IntestazioneTrovata=0;
  memset( &PercorsoRettaGraf, 0, sizeof( PercorsoRettaGraf ));
  ptrfile = fopen( NomeFile, "rb");
  if (ptrfile<=0){
     sprintf( all_mess, "Errore apertura file %s", NomeFile );
     //ShowMessage(all_mess);
     return 1;
  }else{
     //-------------------------------------------------------
     // Recupera tutte  le stringhe e ricerca
     //-------------------------------------------------------
     err=0;
     ErroreConversione=0;
     for( i=1; ; i++){
        //-----------------------------------------------------------------------
        // Recupero Intestazione struttura  PercorsoRettaGraf[][]
        //-----------------------------------------------------------------------
        err=!Read_Line( ptrfile, buffer, 100);   if(err) break;
        Erase_White_Begin( buffer );
        if( strcmp( buffer, "int Percorso[][2]={")==0 ){
           //------------------------------------------------------------
           // Recupero le riga dei record  struttura  PercorsoRettaGraf[][]
           //------------------------------------------------------------
           IntestazioneTrovata=1;
           for( j=0; j<MAXPUNTI; j++){
              err=!Read_Line( ptrfile, buffer, 100);   if(err) break;
              Erase_White_Begin( buffer );
              if(strlen( buffer)==0) {j--; continue;}
              if( buffer[0]=='/' &&  buffer[1]=='/') {j--; continue;}
              if( buffer[0] =='}')  break;
              if( buffer[0] !='{') {
                 sprintf(all_mess, "Errore generico nella conversione PercorsoCurvaGraf[] Indice%d", i);
                 //ShowMessage(all_mess);
                 ErroreConversione=1;
                 break;
              }
              StartPos=2;
              //------------------------------------------------------------
              // Recupero i campi della struttura  PercorsoRettaGraf[][]
              //------------------------------------------------------------
              for( x=0; x<=1; x++){
                 EndPos  = Pos( ',', buffer);
                 Copy( buffer, StartPos, EndPos, buffer1 );
                 buffer1[15]=0;
                 if( EndPos>90 ) {
                    sprintf(all_mess, "Errore generico nella conversione PercorsoRettaGraf[] Indice%d", i);
                    //ShowMessage(all_mess);
                    ErroreConversione=1;
                    break;
                 }
                 PercorsoRettaGraf[ j ][x] = atoi( buffer1 );
                 if( EndPos>0 ) buffer[ EndPos-1 ] =' ';
                 StartPos=EndPos+1;
              }

              if(ErroreConversione) break;
              if((( PercorsoRettaGraf[ j ][ 0 ] != 0 ) && ( PercorsoRettaGraf[ j ][ 1 ] == 0 ))||
                 (( PercorsoRettaGraf[ j ][ 0 ] == 0 ) && ( PercorsoRettaGraf[ j ][ 1 ] != 0 ))) {
                 sprintf(all_mess, "Indice campo %d della struttura PerocorsoGraf[] non corretta", j);
                 //ShowMessage(all_mess);
                 //ErroreConversione=1;
                 //break;
                 continue;
              }
           }
           if(err) break;
           if(ErroreConversione) break;
           if(IntestazioneTrovata==1) break;
        }
        if(err) break;
        if(ErroreConversione) break;
        if(IntestazioneTrovata==1) break;
     }
     if(  IntestazioneTrovata==0){
        sprintf(all_mess, "Intestazione dalla struttura PercorsoRettaGraf[] non trovata");
        //ShowMessage(all_mess);
        ErroreConversione=1;
     }
     //-------------------------------------------------------
     // Aggiorno la struttura grafica per la visualizzazione
     //-------------------------------------------------------
     if(! ErroreConversione) {
        for( i=0; i<=MAXPUNTI; i++){
           Percorso[ i ][0] = PercorsoRettaGraf[ i ][0];
           Percorso[ i ][1] = PercorsoRettaGraf[ i ][1];
           if( PercorsoRettaGraf[ i ][0]==0 && PercorsoRettaGraf[ i ][1]==0) break;
        }
     }
  }

  fclose(ptrfile);
  return ErroreConversione;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Rileggi un file ascii in linguaggio "c" con le informazioni
// grafiche della struttura PercorsoCurvaBezier[][]
// se la conversione va a buon fine compila la struttura in memoria
//
//
int ConvLeggiFileGraphPercorsiCurvaBezierAscii( char *NomeFile, char* all_mess)
/*****************************************************************/
{
  int  StartPos, EndPos;
  int  ErroreConversione;
  int  IntestazioneTrovata;
  int  i, j=0, x;
  int  err;

  FILE *ptrfile;
  char buffer [300];
  char buffer1[300];

  //-------------------------------------------------------
  // Apri il file   GraphPercorsi.dat
  //-------------------------------------------------------
  IntestazioneTrovata=0;
  memset( &PercorsoCurvaBezierGraf, 0, sizeof( PercorsoCurvaBezierGraf ));
  ptrfile = fopen( NomeFile, "rb");
  if (ptrfile<=0){
     sprintf( all_mess, "Errore apertura file %s", NomeFile );
     //ShowMessage(all_mess);
     return 1;
  }else{
     //-------------------------------------------------------
     // Recupera tutte  le stringhe e ricerca
     //-------------------------------------------------------
     err=0;
     ErroreConversione=0;
     for( i=1; ; i++){
        //-----------------------------------------------------------------------
        // Recupero Intestazione struttura  PercorsoCurvaGraf[][]
        //-----------------------------------------------------------------------
        err=!Read_Line( ptrfile, buffer, 100);   if(err) break;
        Erase_White_Begin( buffer );
        if( strcmp( buffer, "int PercorsoCurvaBezier[][7]={")==0 ){
           //------------------------------------------------------------
           // Recupero le riga dei record  struttura  PercorsoCurvaGraf[][]
           //------------------------------------------------------------
           IntestazioneTrovata=1;
           for( j=0; j<MAX_TRACCE_CURVA_DWG; j++){
              err=!Read_Line( ptrfile, buffer, 100); if(err) break;
              Erase_White_Begin( buffer );
              if(strlen( buffer)==0) {j--; continue;}
              if( buffer[0]=='/' &&  buffer[1]=='/') {j--; continue;}
              if( buffer[0] =='}')  break;

              if( buffer[0] !='{') {
                 sprintf(all_mess, "Errore generico nella conversione PercorsoCurvaBezierGraf[] Indice%d", i);
                 //ShowMessage(all_mess);
                 ErroreConversione=1;
                 break;
              }
              StartPos=2;
              //------------------------------------------------------------
              // Recupero i campi della struttura  PerocorsoCurvaBezierGraf[][]
              //------------------------------------------------------------
              for( x=0; x<=6; x++){
                 EndPos  = Pos( ',', buffer);
                 Copy( buffer, StartPos, EndPos, buffer1 );
                 buffer1[15]=0;
                 if( EndPos>90 ) {
                    sprintf(all_mess, "Errore generico nella conversione PercorsoCurvaBezierGraf[] Indice%d", i);
                    //ShowMessage(all_mess);
                    ErroreConversione=1;
                    break;
                 }
                 PercorsoCurvaBezierGraf[ j ][x] = atoi( buffer1 );
                 if( EndPos>0 ) buffer[ EndPos-1 ] =' ';
                 StartPos=EndPos+1;
              }

              if(ErroreConversione) break;
              if((( PercorsoCurvaBezierGraf[ j ][ 1 ] != 0 ) && ( PercorsoCurvaBezierGraf[ j ][ 2 ] == 0 ))||
                 (( PercorsoCurvaBezierGraf[ j ][ 1 ] == 0 ) && ( PercorsoCurvaBezierGraf[ j ][ 2 ] != 0 ))
              ) {
                 sprintf(all_mess, "Indice campo %d della struttura PercorsoCurvaBezierGraf[] non corretta", j);
                 //ShowMessage(all_mess);
                 //ErroreConversione=1;
                 //break;
                 continue;
              }
           }
           if(err) break;
           if(ErroreConversione) break;
           if(IntestazioneTrovata==1) break;
        }
        if(err) break;
        if(ErroreConversione) break;
        if(IntestazioneTrovata==1) break;
     }
     if(  IntestazioneTrovata==0){
        sprintf(all_mess, "Intestazione dalla struttura PercorsoCurvaBezierGraf[] non trovata");
        //ShowMessage(all_mess);
        ErroreConversione=1;
     }
     //-------------------------------------------------------
     // Aggiorno la struttura grafica per la visualizzazione
     //-------------------------------------------------------

     if(! ErroreConversione) {
        for( i=0; i<=MAX_TRACCE_CURVA_DWG; i++){
           PercorsoCurvaBezier[ i ][0] = PercorsoCurvaBezierGraf[ i ][0];
           PercorsoCurvaBezier[ i ][1] = PercorsoCurvaBezierGraf[ i ][1];
           PercorsoCurvaBezier[ i ][2] = PercorsoCurvaBezierGraf[ i ][2];
           PercorsoCurvaBezier[ i ][3] = PercorsoCurvaBezierGraf[ i ][3];
           PercorsoCurvaBezier[ i ][4] = PercorsoCurvaBezierGraf[ i ][4];
           PercorsoCurvaBezier[ i ][5] = PercorsoCurvaBezierGraf[ i ][5];
           PercorsoCurvaBezier[ i ][6] = PercorsoCurvaBezierGraf[ i ][6];
           if( PercorsoCurvaBezierGraf[ i ][0]==0 && PercorsoCurvaBezierGraf[ i ][1]==0 && PercorsoCurvaBezierGraf[ i ][2]==0) break;
        }
     }

  }
  fclose(ptrfile);
  return ErroreConversione;
}
//---------------------------------------------------------------------------
void __fastcall TFormXYNodi::stCurvaDblClick(TObject *Sender)
{
  int nrCurva;
  AnsiString InputStringa;

  InputStringa = InputBox("Seleazione CURVA", "Introdurre il numero della curva:", CurvaSelezionata);
  if(InputStringa.IsEmpty() == true) return;
  nrCurva = StrToIntDef(InputStringa, 0);
  if(nrCurva<1                     ) return;
  if(nrCurva>MAX_TRACCE_CURVA_DWG  ) return;
  //-------------------------------------
  // Puntamento alla curva inserita
  //-------------------------------------
  udVX->Position  = 0;
  udVX1->Position = 0;
  udVY->Position  = 0;
  udVY1->Position = 0;
  EditXV->Text    = 0;
  EditYV->Text    = 0;
  //-------------------------------------
  // Selezione curva
  //-------------------------------------
  CurvaSelezionata  = nrCurva-1;
  udCurva->Position = CurvaSelezionata;
  Aggiorna();
}
//---------------------------------------------------------------------------




