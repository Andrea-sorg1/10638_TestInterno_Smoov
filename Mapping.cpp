//---------------------------------------------------------------------------
#include <vcl.h>
#include <math.h>
#include <stdio.h>

#include <time.h>

#pragma hdrstop

//-------------------------------------------------------
//  0,1 = x,y posizione centrale nodo
//  2 = numero nodo
//  3 = occupazione agv ( numero AGV oppure libero = 0 )
//-------------------------------------------------------
extern int Punti[][5];

#include "Main.h"
#include "Layout.h"
#include "Tecnofer.h"
#include "Mapping.h"
#include "TestPlc1.h"
#include "Laser.h"
#include "Visfile.h"
#include "DisRiflettori.h"
#include "Stato.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMappa *FormMappa;
//---------------------------------------------------------------------------
__fastcall TFormMappa::TFormMappa(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormMappa::FormCreate(TObject *Sender)
{
  TextStato->Caption="";
  AutoSend=false;
  //--------------------------------------------
  // Intestazioni della griglia COORDINATE
  //--------------------------------------------
  GridCoordinate->Cells[0][0] = "Nodo";
  GridCoordinate->Cells[1][0] = "Layout X";
  GridCoordinate->Cells[2][0] = "Layout Y";
  GridCoordinate->Cells[3][0] = "Laser X";
  GridCoordinate->Cells[4][0] = "Laser Y";
  //--------------------------------------------
  // Intestazioni della griglia ZONE
  //--------------------------------------------
  GridZone->Cells[0][0] = "Zona";
  GridZone->Cells[1][0] = "X";
  GridZone->Cells[2][0] = "Y";
  GridZone->Cells[3][0] = "Rot";
  GridZone->Cells[4][0] = "R (Zona)";
  GridZone->Cells[5][0] = "r (Riflet.)";
  GridZone->Cells[6][0] = "Min.";
  GridZone->Cells[7][0] = "R. Trovati";
  GridZone->Cells[8][0] = "Z.Abilitata";
  //--------------------------------------------
  // Inizializzazione parametri LASER
  //--------------------------------------------
  ReadIniParametriLaser();
}
//---------------------------------------------------------------------------
void __fastcall TFormMappa::FormActivate(TObject *Sender)
{
  int err;
  char all_mess[200];
  char buffer[101];

  sprintf(buffer,"Gestione mapping LASER TGV %d",NumTgv);
  FormMappa->Caption=buffer;
  EditRot->SetFocus();

  //--------------------------------------------
  // Inizializzazione delle variabili di classe
  //--------------------------------------------
  err = AGV[NumTgv]->load_zone_mapping(all_mess);
  memcpy(&DatiZona, &AGV[NumTgv]->zone_mapping, sizeof(DatiZona));

  NumZona=0;
  LaserComandoInCorso=0;
  LaserInterrompiComando=0;

  DatiZona.GRD_offset = OFFSET_ROTAZIONE_STANDARD ; // Eventuale valore per la ROTOTRASLAZIONE
                                                    // Nell'eventualità che i riferimenti
  DatiZona.GRD_offset = 0;                          // tra LAYOUT e LASER siano ruotati

  GridCoordinate->RowCount=TOTPUNTI+1;
  //--------------------------------------------
  // Leggi ultima impostazione su PLC specchi
  //--------------------------------------------
  //MinRiflettori=DatiZona.Z[0].Minr; // Memorizzo il numero minimo di riflettori
  MinRiflettori=3;
  NumScanMediaMap=20;
  G_MinimoMediaMap=80;       // Valore di default G=80
  RaggioRiflettori=50;       // Valore di default 50 riflettori rotondi
  //Laser_RotationMode=1;    // Rotazione di Default

  err = LaserLeggiNumeroSpecchi( NumTgv, &MinRiflettori, all_mess);
  if( err ) ShowMessage(all_mess);
  //--------------------------------------------
  // Selezione visualizzazione trasparente
  //--------------------------------------------
  CBVisualizzaTrasparente->Checked = FormLayout->VisualizzaTgvTrasparente[NumTgv];
  //--------------------------------------------
  // Rilettura dei dati da PLC
  //--------------------------------------------
   LoadDati();
  //--------------------------------------------
  // 03/01/2011
  // Rilettura Scostamento Punto Logico da PLC
  //--------------------------------------------
  if( LaserLeggiScostamentoPuntoLogico( NumTgv, &DatiZona.ScostamentoPuntoLogico, all_mess)){
     ShowMessage( all_mess );
  }

  //--------------------------------------------
  // Rinfresco grafico della Form
  //--------------------------------------------
  SetCaselle();

  tgv_color->Brush->Color=AGV[NumTgv]->mission.Col;
  //--------------------------------------------
  // Attivazione del Timer
  //--------------------------------------------
  TimerMapping->Enabled = true;    // Timer di refresh della form di mapping
  //--------------------------------------------
  if(AutoSend==true){
     ScaricaMappaSuPLC();
     HWND hwnd=GetForegroundWindow();
     PostMessage(hwnd, WM_CLOSE, 0, 0);
  }
  //--------------------------------------------
}
//---------------------------------------------------------------------------
void __fastcall TFormMappa::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  char buffer[301]="";
  char buffer1[301]="";
  int  err, err1;

  //--------------------------------------------
  // Disabilitazione del Timer
  //--------------------------------------------
  TimerMapping->Enabled = false;    // Timer di refresh della form di mapping
  LaserComandoInCorso=0;
  LaserInterrompiComando=1;

  //--------------------------------------------
  // Passa al PLC le ultime impostazioni
  //--------------------------------------------
  err=err1=0;
  GetCaselle();
  if(err==0){
     //---------------------------------------------------------
     // Trasmetti su DM139 numero massimo di zone mappabili
     //---------------------------------------------------------
     if(!err) if( AGV[NumTgv]->zone_mapping.MaxZoneMappate ) err = LaserInvioNumeroMaxZone( NumTgv, AGV[NumTgv]->zone_mapping.MaxZoneMappate-1, buffer);
     //---------------------------------------------------------
     // Trasmetti su DM130 numero di specchi da utilizzare
     //---------------------------------------------------------
     // GB-12/04/2012 Trasmetti di default quelli della zona 0 se NumZona==1000
     if( NumZona<MAXZONEMAPPING && DatiZona.Z[NumZona].Minr>=3 && DatiZona.Z[NumZona].Minr<=20 ){
        if(!err) err = LaserInvioNumeroSpecchi ( NumTgv, DatiZona.Z[NumZona].Minr, buffer);
        if(!err) err = LaserTrsComando_RS      ( NumTgv, NumZona, DatiZona.Z[NumZona].Minr, buffer);
     }else{
        if(!err) err = LaserInvioNumeroSpecchi ( NumTgv, DatiZona.Z[0].Minr, buffer);
        if(!err) err = LaserTrsComando_RS      ( NumTgv, NumZona, DatiZona.Z[0].Minr, buffer);
     }
     //---------------------------------------------------------
     // Azzera su DM0 Procedura di calibrazione
     //---------------------------------------------------------
     if(!err) err = LaserProceduraCalibratura( NumTgv, 0, buffer);
  }

  //-------------------------------------------------------------
  // Restituisci comunicazione con Testa LASER da PLC
  //-------------------------------------------------------------
  if(!err1) err1 = LaserAbilitaTipoComunicazione( NumTgv, 2, buffer1);

  //-------------------------------------------------------------
  // Scrittura file parametri laser (se non esiste)
  //-------------------------------------------------------------
  WriteIniParametriLaser();

  //-------------------------------------------------------------
  // Memorizza su struttura TGV
  //-------------------------------------------------------------
  //memcpy(&AGV[NumTgv]->zone_mapping, &DatiZona, sizeof(DatiZona));
  //if(AGV[NumTgv]->save_zone_mapping(buffer)) ShowMessage(buffer);
}
//---------------------------------------------------------------------------
void __fastcall TFormMappa::FormKeyDown(TObject *Sender, WORD &Key,  TShiftState Shift)
{
  if(Key==VK_ESCAPE){
     LaserInterrompiComando=1;
     Close();
  }
}

int RefresAereaDm=0;
//---------------------------------------------------------------------------
void __fastcall TFormMappa::TimerMappingTimer(TObject *Sender)
{
  double Xly, Yly;
  double Grdly;
  char all_mess[101];
  char buffer[101]="";
  short int dm[500];
  short int nodo, rot;
  int err;

  TimerMapping->Enabled = false;    // Timer di refresh della form di mapping


  //------------------------------------------------
  // Refresh dei dati di navigazione
  // Zona, G, Riflettori in uso
  //------------------------------------------------
  if( RefresAereaDm>4 ){
     RefresAereaDm=0;
     err = LaserLeggiAreaDmLaser( NumTgv, dm, all_mess);
     if( err==0 ){
        sprintf(buffer, "%d",   dm[140]);
        EditZona->Text = buffer;
        sprintf(buffer, "%d",   dm[129]);
        EditRiflInUso->Text = buffer;
        sprintf(buffer, "%x",   dm[132]);
        EditG->Text = buffer;
     }
  }
  RefresAereaDm++;
  //------------------------------------------------
  // Refresh della posizione del TGV
  //------------------------------------------------
  sprintf(all_mess, " X (mm.):   %d",   AGV[NumTgv]->stato.x_laser);
  TextXls->Caption = all_mess;
  sprintf(all_mess, " Y (mm.):   %d",   AGV[NumTgv]->stato.y_laser);
  TextYls->Caption = all_mess;
  sprintf(all_mess, " Angolo : %d",     AGV[NumTgv]->stato.rot_laser);
  TextRotls->Caption = all_mess;

  //-----------------------------------------------------------------------
  // 25/10/2002  &&&&&&&&&
  // Recupero il nodo dalla posizione AGV
  //-----------------------------------------------------------------------
  nodo = AGV[NumTgv]->stato.pos;
  TextNodo->Caption = nodo;
  if(AGV[NumTgv]->stato.s.bit.grd0  ) rot=0;
  if(AGV[NumTgv]->stato.s.bit.grd90 ) rot=1;
  if(AGV[NumTgv]->stato.s.bit.grd180) rot=2;
  if(AGV[NumTgv]->stato.s.bit.grd270) rot=3;
  TextRotN->Caption = rot;

  //-----------------------------------------------------------------------
  // N.B. Le quote ricevute dal Laser sono in mm. --> quindi divido per 10
  // e converto
  //-----------------------------------------------------------------------
  //AGV[NumTgv]->convertiXY_laser_to_layout(AGV[NumTgv]->stato.x_laser/10, AGV[NumTgv]->stato.y_laser/10, &Xly, &Yly);
  AGV[NumTgv]->convertiXY_laser_to_layout(AGV[NumTgv]->stato.x_laser, AGV[NumTgv]->stato.y_laser, &Xly, &Yly);
  sprintf(all_mess, " X (mm.):  %4.0f", Xly);
  TextXly->Caption = all_mess;
  sprintf(all_mess, " Y (mm.):  %4.0f", Yly);
  TextYly->Caption = all_mess;
  AGV[NumTgv]->convertiGRD_laser_to_layout(AGV[NumTgv]->stato.rot_laser, &Grdly);
  sprintf(all_mess, " Angolo : %3.0f", Grdly);
  TextRotly->Caption = all_mess;
  //------------------------------------------------
  // Azzeramento del conteggio del timer principale
  //------------------------------------------------
  TimerMapping->Enabled = true;    // Timer di refresh della form di mapping
}
//---------------------------------------------------------------------------
void __fastcall TFormMappa::BtnWriteClick(TObject *Sender)
{
  ScaricaMappaSuPLC();
}
//---------------------------------------------------------------------------
void __fastcall TFormMappa::BtnLoadClick(TObject *Sender)
{
  TimerMapping->Enabled = false;    // Timer di refresh della form di mapping
  //--------------------------------------------
  // Rileattura dei dati da PLC
  //--------------------------------------------
  LoadDati();
  TimerMapping->Enabled = true;     // Timer di refresh della form di mapping
  //--------------------------------------------
  // Rinfresco grafico della Form
  //--------------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormMappa::BtnTestClick(TObject *Sender)
{
  FormTestPlc->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TFormMappa::BtnMappingClick(TObject *Sender)
{
  AnsiString InputStringa;
  int  Num;
  char buffer[301]="";
  char buffer1[301]="";
  int i, err, err1, Nr;
  int X,Y,A,G;

  double ComponenteX, ComponenteY, ScostamentoPuntoLogico;
  long double grd, rad, sinA, cosA;

  bool MappingOnlyNewReflector=false;
  int MediaScan=3;
  int NumNewReflectors=0;
  int OldNumReflectors=0;
  int ReflectorIni=0;
  int ReflectorEnd=0;
  int XRef[MAXRIFLETTORI+10];
  int YRef[MAXRIFLETTORI+10];

  memset( XRef, 0, sizeof( XRef ));
  memset( YRef, 0, sizeof( YRef ));
  if( LaserComandoInCorso ) return;
  //--------------------------------------------------
  // (A) Introduzione DATI per il MAPPING
  //--------------------------------------------------
  err=0;
  if( err==0 ){
     //--------------------------------------------------
     // Introduzione NUMERO ZONA
     //--------------------------------------------------
     InputStringa = InputBox("Mapping della zona LASER", "Introdurre il numero della zona:", "");
     if(InputStringa.IsEmpty() == true) return;
     Num = StrToIntDef(InputStringa,0);
     //----------------------------------------------
     // Controllo della validità del valore inserito
     //----------------------------------------------
     if(Num<0 || Num>=MAXZONEMAPPING){
        ShowMessage("Zona non valida... Il valore deve essere compreso tra 0..40!");
        return;
     }
     NumZona = Num;
     //if(err==0) memset(&DatiZona.Z[NumZona], 0, sizeof(LAYER));  //GB-10/03/2012 NON azzerare
     //-------------------------------------------------------------
     // Recupero i dati generali di default per la zona 0
     //-------------------------------------------------------------
     GetCaselle();
     //GeneraleLayer.MinimoNumRifl= DatiZona.Z[NumZona].Minr;

  }
  //-------------------------------------------------------------
  OldNumReflectors=DatiZona.Z[NumZona].Nr;
  LaserComandoInCorso=1;
  //---------------------------------------------------------------------------------------
  // (A0) Recupero scostamento PUNTO LOGICO e TESTA LASER DM210
  //---------------------------------------------------------------------------------------
  if( LaserLeggiScostamentoPuntoLogico( NumTgv, &DatiZona.ScostamentoPuntoLogico, buffer)){
     ShowMessage( buffer );
     LaserComandoInCorso=0;
     return;
  }
  //-------------------------------------------------------------
  // (A1) CONTROLLO COMUNICAZIONE DA PC
  // Abilita comunicazione con Testa LASER tramite PLC "bridge"
  // 0 = Controllo da PC
  // 1 = Guida con magneti
  // 2 = Guida Laser
  //-------------------------------------------------------------
  TextStato->Caption = "Abilita comunicazione Bridge con Laser";
  err = LaserAbilitaTipoComunicazione( NumTgv, 0, buffer);
  //delay( 1000 );
  LaserProcessaWindowsMessage( 1000 );
  //------------------------------------------------------
  // GB-03/04/2012 Abilita rilevo SOLO nuovi Riflettori
  //------------------------------------------------------
  MappingOnlyNewReflector=false;
  // 24/07/2013 Modifica necessario se si imposta MinRiflettori==0
  //if( DatiZona.Z[NumZona].Xl!=0 && DatiZona.Z[NumZona].Yl!=0 && DatiZona.Z[NumZona].Minr>0 && DatiZona.Z[NumZona].ZAbiltata ){
  if( DatiZona.Z[NumZona].Xl!=0 && DatiZona.Z[NumZona].Yl!=0 && DatiZona.Z[NumZona].ZAbiltata ){
     if( MessageDlg("Yes Attenzione Mapping Standard! (No=Mapping SOLO dei nuovi riflettori)", mtInformation,  TMsgDlgButtons()  << mbNo << mbYes, 0) == mrNo) {
        MappingOnlyNewReflector=true;
     }
  }
  //---------------------------------------------------
  // GB-15/03/2012 Attesa manuale abilitazione Bridge
  //---------------------------------------------------
  if( MappingOnlyNewReflector==false ){
     //sprintf( buffer1, "Attesa Abilita Bridge");
     //Application->MessageBox( buffer1, "ATTENZIONE", MB_OK | MB_ICONEXCLAMATION);
  }
  //---------------------------------------------------------------------------------------
  // (B0) LETTURA POSIZIONE  LASER
  // Solo Se Zona 0 SETTA la posizione del LASER X,Y,A con le impostazioni di LAYOUT
  //---------------------------------------------------------------------------------------
  if( NumZona==0 && MappingOnlyNewReflector==false ){   //GB-03/04/2012
     DatiZona.Z[0].Xl  = DatiZona.X_0_laser*10;
     DatiZona.Z[0].Yl  = DatiZona.Y_0_laser*10;
     DatiZona.Z[0].Al  = DatiZona.GRD_laser*16384/90;
     DatiZona.Z[0].Rl  = 30;
     // GB 22/07/2013 Abilita anche il valore 0 --> Riflettori piatti
     //DatiZona.Z[0].Rr  = 50;
     DatiZona.Z[0].Rr  = RaggioRiflettori;
     DatiZona.Z[0].Minr= MinRiflettori;

     //-----------------------------------------------------------------------
     // Calcolo dello scostamento tra "PUNTO LOGICO" e testa laser
     //-----------------------------------------------------------------------
     //ScostamentoPuntoLogico=1160;
     ScostamentoPuntoLogico = DatiZona.ScostamentoPuntoLogico;
   //grd = (long double)(DatiZona.Z[0].Al);
     grd = (long double)(DatiZona.GRD_laser);
     rad = DegToRad(grd);
     SinCos(rad, sinA, cosA);
     ComponenteX = ScostamentoPuntoLogico*sinA;
     ComponenteY = ScostamentoPuntoLogico*cosA;
     DatiZona.Z[0].Xl = DatiZona.Z[0].Xl+ (int) ComponenteX;
     DatiZona.Z[0].Yl = DatiZona.Z[0].Yl- (int) ComponenteY;

  }
  //GB-10/03/2012 (3)
  X = DatiZona.Z[NumZona].Xl;
  Y = DatiZona.Z[NumZona].Yl;
  A = DatiZona.Z[NumZona].Al;

  //-----------------------------------------------------------
  // GB-06/06/2013 Forzatura Rotazione Testa LASER
  //-----------------------------------------------------------
  //----------------------------------------
  // Modo SA (Standby) necessario
  // altrimenti restituisce errore
  //----------------------------------------
  if(!err){
     TextStato->Caption = "SA Command" ;
     err = LaserTrsComando_SA( NumTgv, buffer);
     LaserProcessaWindowsMessage( 50 );
     delay( 100 ); // Per visualizzazione !!
  }
  //-------------------------------------------
  // Modo SU (Enter Scanner Direction Rotation) necessario
  //-------------------------------------------
  if(!err){
     TextStato->Caption = "SU Command" ;
     err = LaserTrsComando_SU( NumTgv, Laser_RotationMode, buffer);
     LaserProcessaWindowsMessage( 50 );
     delay( 100 ); // Per visualizzazione !!
  }
  //-----------------------------------------------------------
  // (B1) LETTURA POSIZIONE DA LASER
  // Solo Se Zona diversa da 0 Procedura PP per leggere
  // la posizione della testa direttamente dal LASER
  //-----------------------------------------------------------
  if( NumZona || MappingOnlyNewReflector==true ){   //GB-03/04/2012
     //----------------------------------------
     // Modo SA (Standby) necessario
     // altrimenti restituisce errore
     //----------------------------------------
     if(!err){
        TextStato->Caption = "SA Command" ;
        err = LaserTrsComando_SA( NumTgv, buffer);
        LaserProcessaWindowsMessage( 50 );
        delay( 100 ); // Per visualizzazione !!
     }
     //----------------------------------------
     // Modo PA
     //----------------------------------------
     if(!err){
        TextStato->Caption = "PA Command" ;
        LaserProcessaWindowsMessage( 50 );
        delay( 100 ); // Per visualizzazione !!
        err = LaserTrsComando_PA( NumTgv, buffer);
     }
     //------------------------------------------
     // Comando PC  invio Nr. di catarifrangenti
     //------------------------------------------
     if(!err){
        TextStato->Caption = "PC Command" ;
        LaserProcessaWindowsMessage( 50 );
        delay( 100 ); // Per visualizzazione !!
        err = LaserTrsComando_PC( NumTgv, MinRiflettori, buffer);
     }
     //------------------------------------------
     // Lettura posizione media aritmetica
     //------------------------------------------
     if(!err){
        err = LaserTrsComando_PP_Media( NumTgv, &X, &Y, &A, &G, &Nr, buffer);
        if( err==0 ){
           sprintf( buffer1, "Procedura PP Command Conclusa correttamente X=%d, Y=%d, A=%d, G=%d, Nr=%d", X, Y, A*90/16384, G, Nr);
           TextStato->Caption = buffer1;
           LaserProcessaWindowsMessage( 20 );
           delay(1000);
        }
     }
     //----------------------------------------------
     // Memorizzo i dati di zona
     //----------------------------------------------
     if( err==0 && MappingOnlyNewReflector==false ){  //GB-03/04/2012 In caso di Ricerca nuovi riflettori le origini rimangono quelle iniziali
        DatiZona.Z[NumZona].Xl  = X;
        DatiZona.Z[NumZona].Yl  = Y;
        DatiZona.Z[NumZona].Al  = A ;
        DatiZona.Z[NumZona].Rl  = DatiZona.Z[0].Rl;
        DatiZona.Z[NumZona].Rr  = DatiZona.Z[0].Rr;
        DatiZona.Z[NumZona].Minr= MinRiflettori;
     }
  }
  //----------------------------------------
  // Modo SA (Standby) necessario
  // altrimenti restituisce errore
  //----------------------------------------
  if(!err){
     TextStato->Caption = "SA Command" ;
     err = LaserTrsComando_SA( NumTgv, buffer);
     LaserProcessaWindowsMessage( 50 );
     delay( 100 ); // Per visualizzazione !!
  }
  if(!err){
     TextStato->Caption = "MA Command";
     LaserProcessaWindowsMessage( 50 );
     delay( 100 ); // Per visualizzazione !!
     err= LaserTrsComando_MA( NumTgv, buffer);
  }
  //---------------------------------------------------------------------------------------
  // (C0) START MAPPING MS/MM/MN
  // Rileva i riflettori
  //---------------------------------------------------------------------------------------
  if(!err){
     NumNewReflectors=0;
     //---------------------------------------------------
     // MS/MM Start Mapping Standard
     //    Esegue lo scan di tutti i riflettori presenti
     //---------------------------------------------------
     if( MappingOnlyNewReflector==false ){
        if( NumScanMediaMap<=1 ){  // GB-04/04/2012 Se valore è 1 --> usa il comando MS standard
           TextStato->Caption = "MS Command";  // N.B. Può durare 30 sec.
           LaserProcessaWindowsMessage( 50 );
           TextStato->Caption = "MS Command";  // N.B. Può durare 30 sec.
           //delay( 200 ); // Per visualizzazione !!
           //GB-10/03/2012 (3)
           //err = LaserTrsComando_MS( NumTgv, NumZona, DatiZona.Z[NumZona].Xl, DatiZona.Z[NumZona].Yl,
           //      DatiZona.Z[NumZona].Al, DatiZona.Z[NumZona].Rr, &DatiZona.Z[NumZona].Nr, buffer);
           err = LaserTrsComando_MS( NumTgv, NumZona, X, Y, A, DatiZona.Z[NumZona].Rr, &DatiZona.Z[NumZona].Nr, buffer);
        }
        if( NumScanMediaMap>=2 ){  // GB-04/04/2012 Se valore è >1 --> usa il comando MM (media delle letture)
           TextStato->Caption = "MM Command";  // N.B. Può durare 30 sec.
           LaserProcessaWindowsMessage( 50 );
           TextStato->Caption = "MM Command";  // N.B. Può durare 30 sec.
           //delay( 200 ); // Per visualizzazione !!
           //GB-10/03/2012 (3)
           //err = LaserTrsComando_MM( NumTgv, NumZona, NumScanMediaMap, DatiZona.Z[NumZona].Xl, DatiZona.Z[NumZona].Yl,
           //      DatiZona.Z[NumZona].Al, DatiZona.Z[NumZona].Rr, &DatiZona.Z[NumZona].Nr, buffer);
           err = LaserTrsComando_MM( NumTgv, NumZona, NumScanMediaMap, X, Y, A, DatiZona.Z[NumZona].Rr, &DatiZona.Z[NumZona].Nr, buffer);
        }
     }
     //---------------------------------------------------
     // GB-03/04/2012
     // MN Start Negative Mapping
     //    Trova solo i nuovi riflettori
     //---------------------------------------------------
     if( MappingOnlyNewReflector==true ){
        MediaScan=NumScanMediaMap; NumNewReflectors=0;
        TextStato->Caption = "MN Command";  // N.B. Può durare 30 sec.
        LaserProcessaWindowsMessage( 50 );
        TextStato->Caption = "MN Command";  // N.B. Può durare 30 sec.
        //GB-10/03/2012 (3)
        //err = LaserTrsComando_MN( NumTgv, NumZona, MediaScan, DatiZona.Z[NumZona].Xl, DatiZona.Z[NumZona].Yl,
        //      DatiZona.Z[NumZona].Al, DatiZona.Z[NumZona].Rr, &NumNewReflectors, buffer);
        err = LaserTrsComando_MN( NumTgv, NumZona, MediaScan, X, Y, A, DatiZona.Z[NumZona].Rr, &NumNewReflectors, buffer);
        if(err==0){
           sprintf( buffer, "Riflettori nuovi trovati: %d",  NumNewReflectors);
           ShowMessage(buffer);
        }
     }
  }
  //---------------------------------------------------------------------------------------
  // (C2) Leggi Comando MR
  // Scarica posizione riflettori rilevati dal LASER
  //---------------------------------------------------------------------------------------
  if(!err){
     //---------------------------------------------------------------------
     // GB-03/04/2012
     // Nel caso di Negative Mapping aggiungo i riflettori aggiuntivi
     //---------------------------------------------------------------------
     i=0;
     ReflectorIni=0;
     ReflectorEnd=DatiZona.Z[NumZona].Nr;
     memset( XRef, 0, sizeof( XRef ));
     memset( YRef, 0, sizeof( YRef ));

     if( MappingOnlyNewReflector==true  ){
        ReflectorIni   =  0;
        ReflectorEnd   =  NumNewReflectors;
     }
     //--------------------------------------------------------------
     // (MR) Request for mappping position
     // Recupera la posizione di tutti i riflettori trovati
     //--------------------------------------------------------------
     for( i=ReflectorIni; i<ReflectorEnd; i++ ){
        if(i==0){
           sprintf( buffer1, "MR Command Riflettore Nr.%d          ", i);
           TextStato->Caption = buffer1;
           LaserProcessaWindowsMessage( 20 );
        }
        //------------------------------------------------------------------------
        // GB-11/03/2012  Scarica SOLO i riflettori NUOVI da 0..NumNewReflectors
        // N.B. in caso di Mapping "MN" i nuvi riflettori ripartono sempre da 0..
        //------------------------------------------------------------------------
        if( MappingOnlyNewReflector==false  ){
           err = LaserTrsComando_MR( NumTgv, NumZona, i, &Nr, &DatiZona.Z[NumZona].Xr[i], &DatiZona.Z[NumZona].Yr[i], buffer);
        }else{
           err = LaserTrsComando_MR( NumTgv, NumZona, i, &Nr, &XRef[i], &YRef[i], buffer);
        }
        if( err==0){
           sprintf( buffer1, "MR Command Riflettore Richiesto:%d Restituito:%d", i, Nr);
           TextStato->Caption = buffer1;
           LaserProcessaWindowsMessage( 20 );
           delay( 200 ); // Per visualizzazione !!
        }
        if( i>MAXRIFLETTORI ) break;
        if( Nr<0 ) break;
        if( err  ) break;
     }
     //--------------------------------------------------------------
     // GB-10/03/2012  Azzera i restanti non rilevati
     // Recupera la posizione di tutti i riflettori trovati
     //--------------------------------------------------------------
     for( i=DatiZona.Z[NumZona].Nr; i<MAXRIFLETTORI; i++ ){
        DatiZona.Z[NumZona].Xr[i]=0; DatiZona.Z[NumZona].Yr[i]=0;
        if( i>MAXRIFLETTORI ) break;
     }
     //--------------------------------------------------------------
     // GB-11/03/2012  Aggiungi SOLO i riflettori NUOVI
     //--------------------------------------------------------------
     if( MappingOnlyNewReflector==true && err==0 ){
        for( i=ReflectorIni; i<ReflectorEnd; i++ ){
           if( DatiZona.Z[NumZona].Nr+i>MAXRIFLETTORI ) break;
           DatiZona.Z[NumZona].Xr[DatiZona.Z[NumZona].Nr+i]=XRef[i];
           DatiZona.Z[NumZona].Yr[DatiZona.Z[NumZona].Nr+i]=YRef[i];
        }
     }
     if( MappingOnlyNewReflector==true && err==0 ) DatiZona.Z[NumZona].Nr=DatiZona.Z[NumZona].Nr+NumNewReflectors;
     if( DatiZona.Z[NumZona].Nr>MAXRIFLETTORI    ) DatiZona.Z[NumZona].Nr=MAXRIFLETTORI;
  }
  //---------------------------------------------------------------------------------------
  // (C3) Salvataggio file e aggiorno MaxZoneMappate
  //---------------------------------------------------------------------------------------
  if(!err){
     if( NumZona >= DatiZona.MaxZoneMappate ) DatiZona.MaxZoneMappate= NumZona+1;
     //------------------------------------------------------
     // GB-11/04/2012 (4) Chiedi conferma nuovi riflettori
     //------------------------------------------------------
     if( MappingOnlyNewReflector==true ){
        sprintf( buffer, "Attenzione: confermi i %d nuovi riflettori trovati ?", NumNewReflectors );
        if( MessageDlg( buffer, mtInformation,  TMsgDlgButtons()  << mbYes << mbNo , 0)==mrNo) {
           DatiZona.Z[NumZona].Nr=OldNumReflectors;
        }
     }
     //----------------------------------------------
     // Memorizzazione della zona di mapping
     //----------------------------------------------
     DatiZona.Z[NumZona].ZAbiltata  = true;
     memcpy( &AGV[NumTgv]->zone_mapping, &DatiZona, sizeof(DatiZona));
     if(AGV[NumTgv]->save_zone_mapping(buffer)) ShowMessage(buffer);
  }

  //-------------------------------------------------------
  // Informa il PLC dell'ultima zona di mapping e MAX ZONE
  //-------------------------------------------------------
  if(!err){
     if( DatiZona.MaxZoneMappate ) err = LaserInvioNumeroMaxZone( NumTgv, DatiZona.MaxZoneMappate-1, buffer);
     if(!err) err = LaserInvioUltimaZonaMapping  ( NumTgv, NumZona, buffer);
  }
  //---------------------------------------------------------------------------------------
  // (D) DOWNLOAD su Eprom del LASER della Mappatura
  //---------------------------------------------------------------------------------------
  //----------------------------------------
  // Modo SA necessario
  // altrimenti restituisce errore
  //----------------------------------------
  if(!err){
     TextStato->Caption = "SA Command" ;
     LaserProcessaWindowsMessage( 50 );
     delay( 100 ); // Per visualizzazione !!
     err = LaserTrsComando_SA( NumTgv, buffer);
  }
  //------------------------------------------
  // (D0) DA Salvataggio su eprom-->Download
  //------------------------------------------
  if(!err){
     TextStato->Caption = "DA Command";
     LaserProcessaWindowsMessage( 50 );
     delay( 100 ); // Per visualizzazione !!
     err = LaserTrsComando_DA( NumTgv, buffer);
  }
  //------------------------------------
  // (D1) RS Set Raggio dei riflettori
  //------------------------------------
  if(!err){
     TextStato->Caption = "RS Command";
     LaserProcessaWindowsMessage( 50 );
     delay( 100 ); // Per visualizzazione !!
     err = LaserTrsComando_RS( NumTgv, NumZona, DatiZona.Z[NumZona].Rr, buffer);
  }

  //------------------------------------
  // (D2) DownLoad dei riflettori
  // Salvataggio sul NAV200
  //------------------------------------
  if(!err){
     //---------------------------------------------------------------------
     // GB-03/04/2012
     // Nel caso di Negative Mapping aggiungo i riflettori aggiuntivi
     //---------------------------------------------------------------------
     ReflectorIni=0;
     ReflectorEnd=DatiZona.Z[NumZona].Nr;

     for( i=ReflectorIni; i<=ReflectorEnd; i++ ){
        if( DatiZona.Z[NumZona].Nr<0 ) break;
        if(i==0){
           sprintf( buffer1, "DR Command Riflettore Nr.%d          ", i);
           TextStato->Caption = buffer1;
           LaserProcessaWindowsMessage( 20 );
           delay( 100 ); // Per visualizzazione !!
        }
        if( i!=DatiZona.Z[NumZona].Nr ) err = LaserTrsComando_DR( NumTgv, NumZona, i, DatiZona.Z[NumZona].Xr[i], DatiZona.Z[NumZona].Yr[i], &Nr, buffer);
        if( i==DatiZona.Z[NumZona].Nr ) err = LaserTrsComando_DR( NumTgv, NumZona,-1, DatiZona.Z[NumZona].Xr[i], DatiZona.Z[NumZona].Yr[i], &Nr, buffer);
        if( err==0){
           sprintf( buffer1, "DR Command Riflettore Richiesto:%d Restituito:%d", i, Nr);
           TextStato->Caption = buffer1;
           LaserProcessaWindowsMessage( 20 );
           delay( 100 ); // Per visualizzazione !!
        }
        if( i>MAXRIFLETTORI ) break;
        if( Nr<0 ) break;
        if( err ) break;
     }
  }
  //----------------------------------------
  // Modo SA (Standby) necessario
  // altrimenti restituisce errore
  //----------------------------------------
  if(!err){
     TextStato->Caption = "SA Command" ;
     LaserProcessaWindowsMessage( 50 );
     delay( 100 ); // Per visualizzazione !!
     err = LaserTrsComando_SA( NumTgv, buffer);
  }
  //-------------------------------------------------------------
  // Restituisci comunicazione con Testa LASER da PLC
  //-------------------------------------------------------------
  err1 = LaserAbilitaTipoComunicazione( NumTgv, 2, buffer1);
  if( err ) ShowMessage(buffer);
  if( err1) ShowMessage(buffer1);

  //-------------------------------------------------------------
  // Trasmissione avvenuta correttamente
  //-------------------------------------------------------------
  if( !err & ! err1 ) TextStato->Caption = "Procedura di mapping terminata corretamente ";

  //----------------------------------------------
  // Refresh Form
  //----------------------------------------------
  SetCaselle();

  LaserComandoInCorso=0;
}
//---------------------------------------------------------------------------
// ---------------------------
//    LoadDati()
// ---------------------------
// Rilettura da PLC della navetta della mappatura dei nodi
// Area disponibile DM 10000.... DM 19999
//
void __fastcall TFormMappa::LoadDati()
{
  int  i, err=0;
  int  TotPunti=TOTPUNTI;
  char all_mess[101]="";
  char buffer[201]="";
  bool AbilitaCompattazioneDM=false;
  int  NumDmStep, NumNodiStep;
  WORD DmMappaAux[(MAXPUNTI*3)+11];

  memset(&DmMappa[0]   , 0, sizeof(DmMappa));
  memset(&DmMappaAux[0], 0, sizeof(DmMappaAux));
  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]             )  return;
  if(AGV[NumTgv]->mission.EsclPLC)  return;
  /*
  //---------------------------------------------------------
  // Rilettura delle Coordinate di "0" del rif. LASER
  //---------------------------------------------------------
  err = OM.ld_canale(&DmMappa[0], (WORD)(NumTgv), "RD", 10000, 3, all_mess);
  if(err) sprintf(buffer, "01 LoadDati() - %s", all_mess);

  //---------------------------------------------------------
  // Ciclo di lettura di mappa dei nodi
  //---------------------------------------------------------
  if(!err){
     for(i=0; i<13; i++){
        //----------------------------------------------------------
        // Leggo i DM a gruppi di 500 fino a un massimo di TOTPUNTI
        //----------------------------------------------------------
        if(i== 1 && TotPunti< 167) break;
        if(i== 2 && TotPunti< 333) break;
        if(i== 3 && TotPunti< 499) break;
        if(i== 4 && TotPunti< 665) break;
        if(i== 5 && TotPunti< 831) break;
        if(i== 6 && TotPunti< 997) break;
        if(i== 7 && TotPunti<1163) break;
        if(i== 8 && TotPunti<1329) break;
        if(i== 9 && TotPunti<1495) break;
        if(i==10 && TotPunti<1661) break;
        if(i==11 && TotPunti<1827) break;
        if(i==12 && TotPunti<1993) break;
        //---------------------------------------------------------
        // Rilettura da PLC delle coordinate del riferimento LASER
        //---------------------------------------------------------
        //
        // 12/11/2010 CERDOMUS
        // Sostituila la lettura di 498 DM consecutivi con letture da 497 perchè in comunicazione TCP dava errore
        // funziona solo in FINS
        //
      //if(i<12) err = OM.ld_canale(&DmMappa[3+(i*498)], (WORD)(NumTgv), "RD", (WORD)(10003+(i*498)), 498, all_mess);
      //else     err = OM.ld_canale(&DmMappa[3+(i*498)], (WORD)(NumTgv), "RD", (WORD)(10003+(i*498)), 21, all_mess);
      //if(err){sprintf(buffer, "%02d LoadDati() - %s", i+2, all_mess); break;}
        if(i<12) err = OM.ld_canale(&DmMappa[3+(i*497)], (WORD)(NumTgv), "RD", (WORD)(10003+(i*497)), 497, all_mess);
        else     err = OM.ld_canale(&DmMappa[3+(i*497)], (WORD)(NumTgv), "RD", (WORD)(10003+(i*497)), 32, all_mess);
        if(err){sprintf(buffer, "%02d LoadDati() - %s", i+2, all_mess); break;}
     }
  }
  */
  //---------------------------------------------------------
  // 04/02/2015 Lettura Mappa dei nodi
  //---------------------------------------------------------
  #ifdef AREA_DM_COORDINATE_COMPATTATA
     AbilitaCompattazioneDM=true;
  #endif
  //---------------------------------------------------------
  // Rilettura delle Coordinate di "0" del rif. LASER
  // DM1000, 10001, 10002
  //---------------------------------------------------------
  err = OM.ld_canale(&DmMappaAux[0], (WORD)(NumTgv), "RD", 10000, 3, all_mess);
  if(err) sprintf(buffer, "01 LoadDati() - %s", all_mess);

  //---------------------------------------------------------
  // 04/02/2015  Ciclo di Lettura mappa dei nodi
  //---------------------------------------------------------
  if(!err){
     for(i=0; i<=19; i++){
        //-------------------------------------------------------------
        // 04/02/2015  Intremento a step di 495 DM
        // Scrittue su PLC delle coordinate del riferimento LASER
        //
        // Il trasferimento è step di 495 più in sicurezza di CERDOMUS
        //
        // Il limite massimo ora è 20x495+3=9903 (DM10000..19903)
        //-------------------------------------------------------------
        NumDmStep=495;
        if(AbilitaCompattazioneDM==false) NumNodiStep=NumDmStep/3;
        if(AbilitaCompattazioneDM==true ) NumNodiStep=NumDmStep/2;
        if(i>1 && (TotPunti+10)<(i*NumNodiStep)) break;   // Stop in base ai Nodi Impianto
        if(i>19) break;                              // Stop comunque dopo 20 cicli --> 19900 DM
        err = OM.ld_canale(&DmMappaAux[3+(i*NumDmStep)], (WORD)(NumTgv), "RD", (WORD)(10003+(i*NumDmStep)), NumDmStep, all_mess);
        if(err){sprintf(buffer, "%02d LoadDati() - %s", i+2, all_mess); break;}
     }
  }
  //--------------------------------------------------------------------
  // 04/02/2015  Compattazione DM (area 10000..20000)
  //             Negli impianti molto grandi le 3 DM per nodo sforano
  //             Perciò compattiamo trasmettendo solo X e Y :
  //
  //             DM1=Nodo DM2=X DM3=Y   --> DM1=X DM2=Y
  //--------------------------------------------------------------------
  if(err==0){
     if(AbilitaCompattazioneDM){
        DmMappa[0]=DmMappaAux[0];
        DmMappa[1]=DmMappaAux[1];
        DmMappa[2]=DmMappaAux[2];
        for(i=1; i<=TotPunti; i++){
           DmMappa[0+i*3]=i;
           DmMappa[1+i*3]=DmMappaAux[1+i*2];
           DmMappa[2+i*3]=DmMappaAux[2+i*2];
        }
     }else{
        memcpy(&DmMappa[0], &DmMappaAux[0], sizeof(DmMappa));
     }
  }
  //----------------------------------------
  // Se si è verificato un errore --> Skip!
  //----------------------------------------
  if(err){ShowMessage(buffer);}
}
//---------------------------------------------------------------------------
// ---------------------------
//    SaveDati()
// ---------------------------
// Scrittura su PLC della navetta della mappatura dei nodi
// Area disponibile DM 10000.... DM 19999
//
void __fastcall TFormMappa::SaveDati()
{
  int  i, err=0;
  int  TotPunti=TOTPUNTI;
  char all_mess[101]="";
  char buffer[201]="";
  bool AbilitaCompattazioneDM=false;
  int  NumDmStep, NumNodiStep;
  WORD DmMappaAux[(MAXPUNTI*3)+11];

  //---------------------------------------------------------
  // verifica comunicazione OK con AGV e se PLC AGV è escluso
  //---------------------------------------------------------
  if(break_Com[COM1]             ) return;
  if(AGV[NumTgv]->mission.EsclPLC) return;

/*
  //---------------------------------------------------------
  // Scrittura  delle Coordinate di "0" del rif. LASER
  //---------------------------------------------------------
  err = OM.wr_canale(&DmMappa[0], (WORD)(NumTgv), "WD", 10000, 3, all_mess);
  if(err) sprintf(buffer, "01 WriteDati() - %s", all_mess);

  //---------------------------------------------------------
  // Ciclo di scrittura di mappa dei nodi
  //---------------------------------------------------------
  if(!err){
     for(i=0; i<13; i++){
        //----------------------------------------------------------
        // Leggo i DM a gruppi di 500 fino a un massimo di TOTPUNTI
        //----------------------------------------------------------
        if(i== 1 && TotPunti< 167) break;
        if(i== 2 && TotPunti< 333) break;
        if(i== 3 && TotPunti< 499) break;
        if(i== 4 && TotPunti< 665) break;
        if(i== 5 && TotPunti< 831) break;
        if(i== 6 && TotPunti< 997) break;
        if(i== 7 && TotPunti<1163) break;
        if(i== 8 && TotPunti<1329) break;
        if(i== 9 && TotPunti<1495) break;
        if(i==10 && TotPunti<1661) break;
        if(i==11 && TotPunti<1827) break;
        if(i==12 && TotPunti<1993) break;
        //---------------------------------------------------------
        // Rilettura da PLC delle coordinate del riferimento LASER
        //---------------------------------------------------------
        //
        // 12/11/2010 CERDOMUS
        // Sostituila la scrittura di 498 DM consecutivi con letture da 497 perchè in comunicazione TCP dava errore
        // funziona solo in FINS
        //
      //if(i<12) err = OM.wr_canale(&DmMappa[3+(i*498)], (WORD)(NumTgv), "WD", (WORD)(10003+(i*498)), 498, all_mess);
      //else     err = OM.wr_canale(&DmMappa[3+(i*498)], (WORD)(NumTgv), "WD", (WORD)(10003+(i*498)), 21,  all_mess);
        if(i<12) err = OM.wr_canale(&DmMappa[3+(i*497)], (WORD)(NumTgv), "WD", (WORD)(10003+(i*497)), 497, all_mess);
        else     err = OM.wr_canale(&DmMappa[3+(i*497)], (WORD)(NumTgv), "WD", (WORD)(10003+(i*497)), 32,  all_mess);
        if(err){
           sprintf(buffer, "%02d WriteDati() - %s", i+2, all_mess);
           break;
        }
     }
  }
*/
  #ifdef AREA_DM_COORDINATE_COMPATTATA
     AbilitaCompattazioneDM=true;
  #endif
  //--------------------------------------------------------------------
  // 04/02/2015  Compattazione DM (area 10000..20000)
  //             Negli impianti molto grandi le 3 DM per nodo sforano
  //             Perciò compattiamo trasmettendo solo X e Y :
  //
  //             DM1=Nodo DM2=X DM3=Y   --> DM1=X DM2=Y
  //--------------------------------------------------------------------
  memset(&DmMappaAux[0], 0, sizeof(DmMappaAux));
  if(err==0){
     if(AbilitaCompattazioneDM){
        DmMappaAux[0]=DmMappa[0];
        DmMappaAux[1]=DmMappa[1];
        DmMappaAux[2]=DmMappa[2];
        for(i=1; i<=TotPunti; i++){
           DmMappaAux[1+i*2]=DmMappa[1+i*3];
           DmMappaAux[2+i*2]=DmMappa[2+i*3];
        }
     }else{
        //memcpy(&DmMappaAux[0], &DmMappa[0], sizeof(DmMappaAux));
        DmMappaAux[0]=DmMappa[0];
        DmMappaAux[1]=DmMappa[1];
        DmMappaAux[2]=DmMappa[2];
        for(i=1; i<=TotPunti; i++){
           DmMappaAux[0+i*3]=DmMappa[0+i*3];
           DmMappaAux[1+i*3]=DmMappa[1+i*3];
           DmMappaAux[2+i*3]=DmMappa[2+i*3];
        }
     }
  }
  //---------------------------------------------------------
  // Scrittura delle Coordinate di "0" del rif. LASER
  // DM1000, 10001, 10002
  //---------------------------------------------------------
  err = OM.wr_canale(&DmMappaAux[0], (WORD)(NumTgv), "WD", 10000, 3, all_mess);
  if(err) sprintf(buffer, "01 WriteDati() - %s", all_mess);

  //---------------------------------------------------------
  // 04/02/2015  Ciclo di scrittura mappa dei nodi
  //---------------------------------------------------------
  if(!err){
     for(i=0; i<=19; i++){
        //-------------------------------------------------------------
        // 04/02/2015  Intremento a step di 495 DM
        // Scrittue su PLC delle coordinate del riferimento LASER
        //
        // Il trasferimento è step di 495 più in sicurezza di CERDOMUS
        //
        // Il limite massimo ora è 20x495+3=9903 (DM10000..19903)
        //-------------------------------------------------------------
        NumDmStep=495;
        if(AbilitaCompattazioneDM==false) NumNodiStep=NumDmStep/3;
        if(AbilitaCompattazioneDM==true ) NumNodiStep=NumDmStep/2;
        if(i>1 && (TotPunti+10)<(i*NumNodiStep)) break;   // Stop in base ai Nodi Impianto
        if(i>19) break;                              // Stop comunque dopo 20 cicli --> 19900 DM
        err = OM.wr_canale(&DmMappaAux[3+(i*NumDmStep)], (WORD)(NumTgv), "WD", (WORD)(10003+(i*NumDmStep)), NumDmStep, all_mess);
        if(err){ sprintf(buffer, "%02d WriteDati() - %s", i+2, all_mess); break; }
     }
  }

  //----------------------------------------
  // Se si è verificato un errore --> Skip!
  //----------------------------------------
  if( err   ){ShowMessage(buffer);}
  if( err==0 && AutoSend==false ){
     sprintf(buffer, "Scarico dati effettuato correttamente");
     ShowMessage(buffer);
  }
}
//---------------------------------------------------------------------------
// ---------------------------
//    SetCaselle()
// ---------------------------
// Funzione che forza i nuovi valori nelle caselle di dialogo (TEdit)
//
void __fastcall TFormMappa::SetCaselle()
{
  int xlay, ylay, xlas, ylas;
  char buffer[101]="";
  short int i, j;

  //------------------------------------------------
  // Aggiornamento dei campi di editor
  //------------------------------------------------
  EditX0->Text   = DatiZona.X_0_laser*10;  // Gianfranco 19/03/2003 in cm
  EditY0->Text   = DatiZona.Y_0_laser*10;
  EditRot->Text  = DatiZona.GRD_laser;
  TextGRDOffset->Caption  = DatiZona.GRD_offset;
  EditScostPLogico->Text  = DatiZona.ScostamentoPuntoLogico;

  EdLaserMappaRiflettoriTGV->Text  = LaserMappaRiflettoriTgv;
  EdLaserMappaRiflettoriZona->Text = LaserMappaRiflettoriZona;

  sprintf(buffer, "Tgv:%02d",NumTgv);
  txtTgv->Caption = buffer;
  //------------------------------------------------
  // Box di coordinate NODO su LAYOUT e TGV
  //------------------------------------------------
  BtnLed->Enabled = false; // VERDE
  for(i=1; i<=TOTPUNTI; i++){
     for(j=0; j<5; j++){
        switch(j){
           case  0: GridCoordinate->Cells[0][i] = i; break;
           case  1: GridCoordinate->Cells[1][i] = Punti[i][0]*10; break;
           case  2: GridCoordinate->Cells[2][i] = Punti[i][1]*10; break;
           case  3: GridCoordinate->Cells[3][i] = (short)(DmMappa[4+((i-1)*3)])*10; break;
           case  4: GridCoordinate->Cells[4][i] = (short)(DmMappa[5+((i-1)*3)])*10; break;
           default: break;
        }
     }
     //-------------------------------------------
     // Visualizzazione differenza tra coordinate
     // laser e coordinate layout
     //-------------------------------------------
     xlay = abs(atoi(GridCoordinate->Cells[1][i].c_str()));
     ylay = abs(atoi(GridCoordinate->Cells[2][i].c_str()));
     xlas = abs(atoi(GridCoordinate->Cells[3][i].c_str()));
     ylas = abs(atoi(GridCoordinate->Cells[4][i].c_str()));
     if(xlay!=xlas || ylay!=ylas){
        BtnLed->Enabled = true; // ROSSO
        GridCoordinate->Cells[0][i] = "* " + IntToStr(i);
     }
  }
  //------------------------------------------------
  // Visualizzazione delle coordinate del centro
  //------------------------------------------------
  EditMinR->Text              = MinRiflettori;
  EditMaxZone->Text           = DatiZona.MaxZoneMappate;

  EditNumScanMediaMap->Text   = NumScanMediaMap;
  EditGMediaMap->Text         = G_MinimoMediaMap;
  EditRaggioRif->Text         = RaggioRiflettori;
  CheckBoxRotTesta->Checked   = Laser_RotationMode;
  //---------------------------------------------------
  // Visualizzazione dati di MAPPING trasmessi al LASER
  //---------------------------------------------------
  for(i=1; i<=MAXZONEMAPPING; i++){
     for(j=0; j<=8; j++){
        switch(j){
           case  0: GridZone->Cells[0][i] = i-1; break;
           case  1: GridZone->Cells[1][i] = DatiZona.Z[i-1].Xl;           break;
           case  2: GridZone->Cells[2][i] = DatiZona.Z[i-1].Yl;           break;
           case  3: GridZone->Cells[3][i] = DatiZona.Z[i-1].Al*90/16384;  break;
           case  4: GridZone->Cells[4][i] = DatiZona.Z[i-1].Rl;           break;
           case  5: GridZone->Cells[5][i] = DatiZona.Z[i-1].Rr;           break;
           case  6: GridZone->Cells[6][i] = DatiZona.Z[i-1].Minr;         break;
           case  7: GridZone->Cells[7][i] = DatiZona.Z[i-1].Nr;           break;
           case  8: GridZone->Cells[8][i] = DatiZona.Z[i-1].ZAbiltata;    break;
           default: break;
        }
     }
  }
}
//---------------------------------------------------------------------------
// ---------------------------
//    GetCaselle()
// ---------------------------
// Funzione che legge dalle caselle di dialogo (TEdit) di WINDOWS
//
void __fastcall TFormMappa::GetCaselle()
{
  int i, j;//, nodo;
  int X0las, Y0las;
  int GrdLaser;//, GrdLayoutNodoMapping;
  double X, Y;
  double XLaser, YLaser;
  long double sinA, cosA;
  long double grd, rad;
//  int  NumLayer;
  int  Xintero, Yintero;
  char buffer[200];
  div_t Divisione;
//  int  OldAngolo;
  int  NewAngolo;

  XLaser=YLaser=0;

  //---------------------------------------------------
  // Visualizzazione Riflettori
  //---------------------------------------------------
  LaserMappaRiflettoriTgv  = atoi(EdLaserMappaRiflettoriTGV->Text.c_str());
  LaserMappaRiflettoriZona = atoi(EdLaserMappaRiflettoriZona->Text.c_str());
  //--------------------------------------------------
  // Recupero origine del riferimento LASER
  // NON PIU rispetto al numero impostato ma
  // al nodo relativo alla zona 0
  //--------------------------------------------------
  //nodo = StrToIntDef(EditNodo->Text,0);
  //NumLayer=0;
  //nodo = GeneraleLayer.Nodo[ NumLayer ];
  //if(nodo<1 || nodo>MAXPUNTI     ){ShowMessage("Errore: Il numero del nodo non è corretto (0...2000)!"); return;}

  //---------------------------------------------------
  // Origine riferimento LASER rispetto al LASER -->cm
  //---------------------------------------------------
  X0las = StrToIntDef(EditX0->Text,0)/10;
  Y0las = StrToIntDef(EditY0->Text,0)/10;
  //--------------------------------------------------
  // Rotazione del TGV rispetto al riferimento LAYOUT
  //--------------------------------------------------
  GrdLaser             = StrToIntDef(EditRot->Text,0);
  DatiZona.GRD_laser   = GrdLaser;
  //GrdLayoutNodoMapping = StrToIntDef(EditRotTGVNodo->Text,0);
  //------------------------------------------------
  // Aggiornamento dei campi di editor
  //------------------------------------------------
  DmMappa[0] = (WORD)(X0las);
  DmMappa[1] = (WORD)(Y0las);
  DmMappa[2] = (WORD)(GrdLaser);
  //------------------------------------------------
  // Memorizzo i dati di conversione del riferimento
  //------------------------------------------------
  //DatiZona.X_0_layout = Punti[nodo][0];
  //DatiZona.Y_0_layout = Punti[nodo][1];
  DatiZona.X_0_laser  = X0las;
  DatiZona.Y_0_laser  = Y0las;

  DatiZona.ScostamentoPuntoLogico = StrToIntDef(EditScostPLogico->Text,0 );
  //------------------------------------------------
  // Salvataggio anche sulla struttura globale
  //------------------------------------------------
  MinRiflettori       = StrToIntDef(EditMinR->Text,0);
  if(MinRiflettori!=0 && MinRiflettori<3) MinRiflettori=3;
  if(                    MinRiflettori>9) MinRiflettori=9;
  // GB 22/07/2013 Abilita anche il valore 0 --> Riflettori piatti
  RaggioRiflettori    =  StrToIntDef(EditRaggioRif->Text,0);

  DatiZona.MaxZoneMappate = StrToIntDef(EditMaxZone->Text,0);
  if( DatiZona.MaxZoneMappate>=MAXZONEMAPPING ) DatiZona.MaxZoneMappate=MAXZONEMAPPING-1;

  NumScanMediaMap = StrToIntDef( EditNumScanMediaMap->Text,0);
  if( NumScanMediaMap<1  ) NumScanMediaMap=1;
  if( NumScanMediaMap>127) NumScanMediaMap=127;

  G_MinimoMediaMap = StrToIntDef( EditGMediaMap->Text,0);
  if( G_MinimoMediaMap< 10 ) G_MinimoMediaMap= 10;
  if( G_MinimoMediaMap>100 ) G_MinimoMediaMap=100;

  Laser_RotationMode = CheckBoxRotTesta->Checked;
  //----------------------------------------------------------
  // Angolo di offset tra rif.LAYOUT e rif.LASER
  // per la ROTOTRASLAZIONE inversa da Ly->Las è 360°-Angolo
  //----------------------------------------------------------
  grd = DatiZona.GRD_offset;
  rad = DegToRad(grd);
  SinCos(rad, sinA, cosA);
  if( grd==0 ){  sinA=0; cosA=1; } // Ottimizzazzione per aumentare precisione
  //------------------------------------------------
  // Box di "Dati Percorso"
  // Cal
  //------------------------------------------------
  for(i=1; i<=MAXPUNTI; i++){
     //------------------------------------------------------------------------
     // Annullamento di una eventuale traslazione della origine del
     // sistema LASER (considera ch le Y supervisore sono di segno opposto).
     // es: se X0las e Y0Las sono impostate uguali alle coordinate del nodo di
     //     mapping le due origini sarebbero già sovrapposte.
     //------------------------------------------------------------------------
     X =  Punti[i][0] ; // Se ci sarà poi un Offset in X occorrerà tenerne conto
     Y =-(Punti[i][1]); // Se ci sarà poi un Offset in X occorrerà tenerne conto

     // Il *10  e /10 serve per aumentare la precisione e evitare approssimazioni
     XLaser = ((X*cosA*10)-(Y*sinA*10))/10;     // ROTOTRASLAZIONE inversa 360°-angolo
     YLaser = ((X*sinA*10)+(Y*cosA*10))/10;     // ROTOTRASLAZIONE inversa 360°-angolo

     sprintf( buffer, "%5.0f",  XLaser );
     Xintero= atoi( buffer);
     sprintf( buffer, "%5.0f",  YLaser );
     Yintero= atoi( buffer);

     DmMappa[3+((i-1)*3)]=(WORD)(i);
     DmMappa[4+((i-1)*3)]=(WORD)(Xintero);
     DmMappa[5+((i-1)*3)]=(WORD)(Yintero);

     //---------------------------------------------
     // Se al nodo non è stata associata una pagina
     // scrivo "0" nei DM relativi.
     //
     // AL-28/02 SCARICO AI TGV TUTTI I NODI ANCHE
     //          QUELLI NASCOSTI
     //---------------------------------------------
/*
     if(Punti[i][4]==0){
        DmMappa[3+((i-1)*3)]=0;
        DmMappa[4+((i-1)*3)]=0;
        DmMappa[5+((i-1)*3)]=0;
     }
*/
  }
  //----------------------------------------------------
  // Recupero delle variazioni nelle impostazioni laser
  //----------------------------------------------------
  for(i=1; i<=MAXZONEMAPPING; i++){
     for(j=1; j<=8; j++){
        switch(j){
           case  1: DatiZona.Z[i-1].Xl           = StrToIntDef(GridZone->Cells[j][i],0);  // Quota X centro Zona
                    break;
           case  2: DatiZona.Z[i-1].Yl           = StrToIntDef(GridZone->Cells[j][i],0);  // Quota Y centro Zona
                    break;
           case  3: NewAngolo                    = StrToIntDef(GridZone->Cells[j][i],0);  // Quota Angolo centro Zona
                    Divisione = div( NewAngolo*16384, 90 );
                    if( NewAngolo >=0 && Divisione.rem>50 ){ // Approsimazione per eccesso (comunque serve solo per visualizzazione)
                       NewAngolo++;
                    }
                    if( NewAngolo <0 && Divisione.rem<-50 ){
                       NewAngolo--;
                    }
                    DatiZona.Z[i-1].Al = NewAngolo*16384/90; // Approsimazione per eccesso (comunque serve solo per visualizzazione)
                    break;
           case  4: DatiZona.Z[i-1].Rl           = StrToIntDef(GridZone->Cells[j][i],0);  // Raggio Zona
                    //if(DatiZona.Z[i-1].Rl<10 && DatiZona.Z[i-1].Xl!=0 && DatiZona.Z[i-1].Yl!=0) DatiZona.Z[i-1].Rl=10;
                    if(DatiZona.Z[i-1].Xl!=0 )  DatiZona.Z[i-1].Rl=30; // SEMPRE FISSO a 30 ANNOVI 29/03
                    break;
           case  5: DatiZona.Z[i-1].Rr           = StrToIntDef(GridZone->Cells[j][i],0);  // Raggio Riflettori 50mm (catarifrangente)
                    // GB 22/07/2013 Abilita anche il valore 0 --> Riflettori piatti
                    //if(DatiZona.Z[i-1].Rr<10 && DatiZona.Z[i-1].Xl!=0 && DatiZona.Z[i-1].Yl!=0) DatiZona.Z[i-1].Rr=10;
                    if(DatiZona.Z[i-1].Rr<0 ) DatiZona.Z[i-1].Rr=0;
                    break;
           case  6: DatiZona.Z[i-1].Minr         = StrToIntDef(GridZone->Cells[j][i],0);  // Min Riflettori
                    // GB 22/07/2013 Abilita anche il valore 0 --> Tutti quelli possibile
                    //if( DatiZona.Z[i-1].Minr< 3 && DatiZona.Z[i-1].Xl!=0 ) DatiZona.Z[i-1].Minr=3;
                    if( DatiZona.Z[i-1].Minr< 0 ) DatiZona.Z[i-1].Minr=0;
                    if( DatiZona.Z[i-1].Minr>10 ) DatiZona.Z[i-1].Minr=9;
                    break;
           //case  7: DatiZona.Z[i-1].Nr          = StrToIntDef(GridZone->Cells[j][i],0);  // Riflettori trovati
           //         break;
           case  8: DatiZona.Z[i-1].ZAbiltata    = StrToIntDef(GridZone->Cells[j][i],0);
                    //----------------------------------------------------------
                    // 03/01/2011
                    // Su richiesta di Annovi...aggiorno la variabile
                    // MaxZoneMappate impostando il numero della zona
                    // di peso maggiore e non viceversa.
                    //----------------------------------------------------------
                    //if( i-1 >=  DatiZona.MaxZoneMappate ) DatiZona.Z[i-1].ZAbiltata =0;
                    if( DatiZona.Z[i-1].ZAbiltata>1     ) DatiZona.Z[i-1].ZAbiltata =1;
                    if( DatiZona.Z[i-1].ZAbiltata==1    ) DatiZona.MaxZoneMappate   =i;
                    break;
           default: break;
        }
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMappa::BtnUploadClick(TObject *Sender)
{
  char buffer[301]="";
  char buffer1[301]="";
  int i, err, err1, Nr;
  int Num;
  int ZonaIniziale;
  int ZonaFinale;
  AnsiString InputStringa;

  if( LaserComandoInCorso              ) return ;
  //--------------------------------------------------
  // Introduci numero ZONA
  //--------------------------------------------------
  InputStringa = InputBox("Upload e salvataggio dati zona su file Mappaxx.dat", "Introdurre il numero della zona (1000=tutte): ", "");
  if(InputStringa.IsEmpty() == true) return;
  Num = StrToIntDef(InputStringa,0);
  //----------------------------------------------
  // Controllo della validità del valore inserito
  //----------------------------------------------
  if(Num!=1000){
     if(Num<0 || Num>MAXZONEMAPPING){
        ShowMessage("Zona non valida... Il valore deve essere compreso tra 0..40!");
        return;
     }
  }
  NumZona= Num;
  LaserComandoInCorso =1;
  //-------------------------------------------------------------
  // Abilita comunicazione con Testa LASER tramite PLC "bridge"
  // 0 = Controllo da PC
  // 1 = Guida con magneti
  // 2 = Guida Laser
  //-------------------------------------------------------------
  TextStato->Caption = "Abilita comunicazione Bridge con Laser";
  err = LaserAbilitaTipoComunicazione( NumTgv, 0, buffer);
  delay( 1500 );
  //---------------------------------------------------
  // GB-15/03/2012 Attesa manuale abilitazione Bridge
  //---------------------------------------------------
  //sprintf( buffer1, "Attesa Abilita Bridge");
  //Application->MessageBox( buffer1, "ATTENZIONE", MB_OK | MB_ICONEXCLAMATION);
  //----------------------------------------
  // Ciclo FOR per tutte le Zone (1000)
  //----------------------------------------
  if( NumZona==1000 ){
     ZonaIniziale =0;
     ZonaFinale   =0;
     if( DatiZona.MaxZoneMappate ) ZonaFinale   =DatiZona.MaxZoneMappate-1;
  }else{
     ZonaIniziale=NumZona;
     ZonaFinale  =NumZona;
  }
  for( NumZona=ZonaIniziale; NumZona<=ZonaFinale; NumZona++){
     //----------------------------------------
     // Modo SA necessario
     // altrimenti restituisce errore
     //----------------------------------------
     if(!err){
        TextStato->Caption = "SA Command" ;
        err = LaserTrsComando_SA( NumTgv, buffer);
     }
     //------------------------------------
     // Lettura da eprom-->Upload
     //------------------------------------
     if(!err){
        TextStato->Caption = "UA Command";
        err = LaserTrsComando_UA( NumTgv, buffer);
     }
     if(!err){
        i=0;
        //------------------------------------
        // Lettura di ogni singolo riflettore
        //------------------------------------
        for( ;;){
           if(i==0){
              sprintf( buffer1, "UR Command Riflettore Nr.%d          ", i);
              TextStato->Caption = buffer1;
           }
           //err = LaserTrsComando_UR( NumTgv, NumZona, &Nr, &Dati.Xr[i], &Dati.Yr[i], buffer);
           err = LaserTrsComando_UR( NumTgv, NumZona, &Nr, &DatiZona.Z[NumZona].Xr[i], &DatiZona.Z[NumZona].Yr[i], buffer);
           if( err==0){
              sprintf( buffer1, "UR Command Riflettore Richiesto:%d Restituito:%d", i, Nr);
              TextStato->Caption = buffer1;
              delay( 200 ); // Per visualizzazione !!
           }
           if( i>MAXRIFLETTORI ) break;
           if( err  ) break;
           if( Nr<0 ) break;
           i++;
        }
        if( err==0 ) DatiZona.Z[NumZona].Nr = i;
     }
     //--------------------------------------------------------------
     // GB-10/03/2012  Azzera i restanti non rilevati
     // Recupera la posizione di tutti i riflettori trovati
     //--------------------------------------------------------------
     for( i=DatiZona.Z[NumZona].Nr; i<MAXRIFLETTORI; i++ ){
        DatiZona.Z[NumZona].Xr[i]=0; DatiZona.Z[NumZona].Yr[i]=0;
        if( i>MAXRIFLETTORI ) break;
     }
     //----------------------------------------------
     // Memorizzazione della zona di mapping
     //----------------------------------------------
     if( err==0 ){
        //----------------------------------------------
        // Memorizzazione della zona di mapping
        //----------------------------------------------
        //WriteIniLaser(NumTgv, NumZona, &DatiZona.Z[NumZona]);
        memcpy( &AGV[NumTgv]->zone_mapping, &DatiZona, sizeof(DatiZona));
        if( AGV[NumTgv]->save_zone_mapping(buffer)) ShowMessage(buffer);
     }
     if( err ) break;
  }
  //-------------------------------------------------------------
  //-------------------------------------------------------------
  // Restituisci comunicazione con Testa LASER da PLC
  //-------------------------------------------------------------
  if( err )  ShowMessage(buffer);
  err1 = LaserAbilitaTipoComunicazione( NumTgv, 2, buffer1);
  if( err1)  ShowMessage(buffer1);
  if( !err & ! err1 ) TextStato->Caption = "Procedura Upload terminata corretamente";

  LaserComandoInCorso=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMappa::BtnDownloadClick(TObject *Sender)
{
  int Num;
  AnsiString InputStringa;
  char buffer[301]="";
  char buffer1[301]="";
  int i, err, err1, Nr;
  int ZonaIniziale;
  int ZonaFinale;

  if( LaserComandoInCorso              ) return ;
  //--------------------------------------------------
  // Introduci numero ZONA
  //--------------------------------------------------
  InputStringa = InputBox("Rilettura dati zona da file Mappaxx.dat e DOWNLOAD", "Introdurre il numero della zona (1000=tutte): ", "");
  if(InputStringa.IsEmpty() == true) return;
  Num = StrToIntDef(InputStringa,0);
  //----------------------------------------------
  // Controllo della validità del valore inserito
  //----------------------------------------------
  if(Num!=1000){
     if(Num<0 || Num>MAXZONEMAPPING){
        ShowMessage("Zona non valida... Il valore deve essere compreso tra 0..40!");
        return;
     }
  }
  NumZona=Num;

  LaserComandoInCorso=1;
  //-------------------------------------------------------------
  // Abilita comunicazione con Testa LASER tramite PLC "bridge"
  // 0 = Controllo da PC
  // 1 = Guida con magneti
  // 2 = Guida Laser
  //-------------------------------------------------------------
  TextStato->Caption = "Abilita comunicazione Bridge con Laser";
  err = LaserAbilitaTipoComunicazione( NumTgv, 0, buffer);
  delay( 1000 );
  //---------------------------------------------------
  // GB-15/03/2012 Attesa manuale abilitazione Bridge
  //---------------------------------------------------
  //sprintf( buffer1, "Attesa Abilita Bridge");
  //Application->MessageBox( buffer1, "ATTENZIONE", MB_OK | MB_ICONEXCLAMATION);
  //----------------------------------------
  // Ciclo FOR per tutte le Zone (1000)
  //----------------------------------------
  if( NumZona==1000 ){
     ZonaIniziale =0;
     ZonaFinale   =0;
     if( DatiZona.MaxZoneMappate ) ZonaFinale = DatiZona.MaxZoneMappate-1;
  }else{
     ZonaIniziale=NumZona;
     ZonaFinale  =NumZona;
  }
  //---------------------------------------------------
  // Rilettura da file Mappaxx.dat dei dati della zona
  //---------------------------------------------------
  if(!err){
     err = AGV[NumTgv]->load_zone_mapping(buffer);
     memcpy(&DatiZona, &AGV[NumTgv]->zone_mapping, sizeof(DatiZona));
  }
  for( NumZona=ZonaIniziale; NumZona<=ZonaFinale; NumZona++){
     if( err ) break;
     //----------------------------------------
     // Modo SA necessario
     // altrimenti restituisce errore
     //----------------------------------------
     if(!err){
        TextStato->Caption = "SA Command" ;
        err = LaserTrsComando_SA( NumTgv, buffer);
     }
     //------------------------------------
     // Modalita DA -->Download
     //------------------------------------
     if(!err){
        TextStato->Caption = "DA Command";
        err = LaserTrsComando_DA( NumTgv, buffer);
     }
     //------------------------------------
     // Set Raggio dei riflettori
     //------------------------------------
     if(!err){
        TextStato->Caption = "RS Command";
        err = LaserTrsComando_RS( NumTgv, NumZona, DatiZona.Z[NumZona].Rr, buffer);
     }
     //------------------------------------------------
     // Salvataggio su eprom dei rifletori -->Download
     //------------------------------------------------
     if(!err){
        for( i=0; i<=DatiZona.Z[NumZona].Nr; i++ ){
           if( DatiZona.Z[NumZona].Nr<0 ) break;
           if(i==0){
              sprintf( buffer1, "DR Command Riflettore Nr.%d          ", i);
              TextStato->Caption = buffer1;
           }
           if( i!=DatiZona.Z[NumZona].Nr ) err = LaserTrsComando_DR( NumTgv, NumZona, i, DatiZona.Z[NumZona].Xr[i], DatiZona.Z[NumZona].Yr[i], &Nr, buffer);
           if( i==DatiZona.Z[NumZona].Nr ) err = LaserTrsComando_DR( NumTgv, NumZona,-1, DatiZona.Z[NumZona].Xr[i], DatiZona.Z[NumZona].Yr[i], &Nr, buffer);
           if( err==0){
              sprintf( buffer1, "DR Command Riflettore Richiesto:%d Restituito:%d", i, Nr);
              TextStato->Caption = buffer1;
              delay( 200 ); // Per visualizzazione !!
           }
           if( i>MAXRIFLETTORI ) break;
           if( Nr<0 ) break;
           if( err ) break;
        }
     }
     if( err ) break;
  }

  //----------------------------------------------
  // Refresh Form
  //----------------------------------------------
  SetCaselle();

  //-------------------------------------------------------------
  // Restituisci comunicazione con Testa LASER da PLC
  //-------------------------------------------------------------
  err1 = LaserAbilitaTipoComunicazione( NumTgv, 2, buffer1);
  if( err )  ShowMessage(buffer);
  if( err1)  ShowMessage(buffer1);

  if( !err & ! err1 ) TextStato->Caption = "Procedura di Dowload terminata corretamente ";

  LaserComandoInCorso=0;
}
//---------------------------------------------------------------------------

void __fastcall TFormMappa::BtnCalEncoderClick(TObject *Sender)
{
  AnsiString InputStringa;
  char buffer[301]="";
  char buffer1[301]="";
  int err, err1, Nr;

  bool Ok;
  int X,Y,A,G;
  int NodoP0;
  int NodoP1;
  int xP0, yP0;
  int xP1, yP1;
  int Distanza;
  float  Sviluppo;
  double Appoggio;
  int    RapPlc;
  time_t TempoInizio;
  time_t TempoFine;
  time_t TempoDurata;
  time_t OverTimeProcedura;

  Ok=false;
  //--------------------------------------------------
  // (A) Introduzione DATI CALIBRAZIONE
  //--------------------------------------------------
  if( break_Com[COM1]              ) return;
  if( AGV[NumTgv]->mission.EsclPLC ) return;
  if( LaserComandoInCorso          ) return;
  err=0;
  if( err==0 ){
     //--------------------------------------------------
     // Introduzione NUMERO NODO P1 e controllo validità
     //--------------------------------------------------
     InputStringa = InputBox("Introdurre Nodo su cui posizionarsi", "Introdurre il numero del nodo P1:", "");
     if(InputStringa.IsEmpty() == true) return;
     NodoP0 = AGV[NumTgv]->stato.pos;
     NodoP1 = StrToIntDef(InputStringa,0);
     if(NodoP1<1 || NodoP1>MAXPUNTI || NodoP1 == NodoP0 ){
        ShowMessage("Nodo P1 non valido");
        return;
     }
     //-------------------------------------------------------------
     // Verifica Distanza nonecessiva
     //-------------------------------------------------------------
     if( err==0){
        xP0= Punti[NodoP0][0];
        yP0= Punti[NodoP0][1];
        xP1= Punti[NodoP1][0];
        yP1= Punti[NodoP1][1];

        Appoggio = (double)(abs((xP1 - xP0)*(xP1 - xP0)) + abs((yP1- yP0)*(yP1- yP0)));
        Distanza = (int) sqrt_algo( Appoggio );
        if( Distanza>1000 ){
           ShowMessage("Distanza da nodo P1 superiore ai 10 mt");
           return;
        }
     }
     //-------------------------------------------------------------
     // Verifica se TGV pronto e in semiautomatico
     //-------------------------------------------------------------
     if( err==0){
        if( AGV[NumTgv]->test_mission_on() != false){
           ShowMessage("Il TGV NON deve essere in missione");
           return;
        }
     }
     if( err==0){
        if( AGV[NumTgv]->mission.SemiAutomatico== false){
           ShowMessage("Il TGV deve essere in semiautomatico");
           return;
        }
     }
  }
  //-------------------------------------------------------------

  LaserComandoInCorso=1;
  //-------------------------------------------------------------
  // (A1) CONTROLLO COMUNICAZIONE DA PC
  // Abilita comunicazione con Testa LASER tramite PLC "bridge"
  // 0 = Controllo da PC
  // 1 = Guida con magneti
  // 2 = Guida Laser
  //-------------------------------------------------------------
  if( err==0 ){
     TextStato->Caption = "Abilita comunicazione Bridge con Laser";
     err = LaserAbilitaTipoComunicazione( NumTgv, 0, buffer);
     delay( 1000 );
  }
  //-------------------------------------------------------------
  // (A2) ATTIVA PROCEDURA CALIBRAZIONE
  //-------------------------------------------------------------
  if( err==0 ){
     TextStato->Caption = "Abilita Procedura CALIBRAZIONE ENCODER";
     if(err==0) err = LaserProceduraCalibratura( NumTgv, 1, buffer);
  }
  //-----------------------------------------------------------
  // (B) LETTURA POSIZIONE DA LASER P0
  // Solo Se Zona diversa da 0 Procedura PP per leggere
  // le posizioni direttamente dal LASER
  //-----------------------------------------------------------
  if( err==0 ){
     //----------------------------------------
     // Modo PA
     //----------------------------------------
     if(!err){
        TextStato->Caption = "PA Command" ;
        err = LaserTrsComando_PA( NumTgv, buffer);
     }
     //------------------------------------------
     // Lettura posizione media aritmetica
     //------------------------------------------
     if(!err){
        err = LaserTrsComando_PP_Media( NumTgv, &X, &Y, &A, &G, &Nr, buffer);
        if( err==0 ){
           //A= A*90/16384;  // Conversione BDEG --> GRADI
           //sprintf( buffer1, "Procedura PP Command Conclusa correttamente X=%d, Y=%d, A=%d, G=%d, Nr=%d", X, Y, A, G, Nr);
           sprintf( buffer1, "Procedura PP Command Conclusa correttamente X=%d, Y=%d, A=%d, G=%d, Nr=%d", X, Y, A*90/16384, G, Nr);
           TextStato->Caption = buffer1;
           xP0 = X;
           yP0 = Y;
           delay(1000);
        }
     }
  }
  //---------------------------------------------------------------

  //-----------------------------------------------------------
  // (C) SEMIAUTOMATICO SU P1
  //-----------------------------------------------------------
  if( err==0 ){
     //-------------------------------------------------------------
     // Abilita controllo da PLC
     //-------------------------------------------------------------
     if( err==0){
        TextStato->Caption = "Abilita comunicazione da PLC";
        err = LaserAbilitaTipoComunicazione( NumTgv, 2, buffer);
        delay( 1000 );
     }
     //-------------------------------------------------------------
     // Invio missione semiautomatico
     //-------------------------------------------------------------
     if( err==0){
        err = P.PosizionamentoManualeAGV( (short int) NumTgv, (short int) NodoP1, N_NO_FORCHE, &Ok, buffer);
     }
     //-------------------------------------------------------------
     // Attesa conclusione missione
     //-------------------------------------------------------------
     OverTimeProcedura = 120;  //sec
     TempoInizio = time(NULL);
     do{
        if( err ) break;
        LaserProcessaWindowsMessage( 1000 );

        TempoFine   = time(NULL);
        TempoDurata = TempoFine - TempoInizio;
        if( TempoDurata>=OverTimeProcedura ){ err=1; sprintf( buffer, "OVER-TIME" );  break;  }
        if( LaserInterrompiComando ){  err=1;  sprintf( buffer, "Comando interrotto da utente" ); break;  }

        if(( AGV[NumTgv]->stato.pos == NodoP1) && ( AGV[NumTgv]->test_mission_on() ==0 )) break;

        sprintf( buffer, "Time:%d Numero impulsi Encoder: %d", OverTimeProcedura-TempoDurata,  AGV[NumTgv]->stato.offset_da_nodo );
        TextStato->Caption = buffer;
     }while (err==0);
  }

  //-----------------------------------------------------------
  // (D) LETTURA POSIZIONE DA LASER P1
  // Solo Se Zona diversa da 0 Procedura PP per leggere
  // le posizioni direttamente dal LASER
  //-----------------------------------------------------------
  if( err==0 ){
     //-------------------------------------------------------------
     // Abilita controllo da PC
     //-------------------------------------------------------------
     if(!err){
        TextStato->Caption = "Abilita comunicazione da PC";
        err = LaserAbilitaTipoComunicazione( NumTgv, 0, buffer);
        delay( 1000 );
     }
     //----------------------------------------
     // Modo PA
     //----------------------------------------
     if(!err){
        TextStato->Caption = "PA Command" ;
        err = LaserTrsComando_PA( NumTgv, buffer);
     }
     //------------------------------------------
     // Lettura posizione media aritmetica
     //------------------------------------------
     if(!err){
        err = LaserTrsComando_PP_Media( NumTgv, &X, &Y, &A, &G, &Nr, buffer);
        if( err==0 ){
           //A= A*90/16384;  // Conversione BDEG --> GRADI
           //sprintf( buffer1, "Procedura PP Command Conclusa correttamente X=%d, Y=%d, A=%d, G=%d, Nr=%d", X, Y, A, G, Nr);
           sprintf( buffer1, "Procedura PP Command Conclusa correttamente X=%d, Y=%d, A=%d, G=%d, Nr=%d", X, Y, A*90/16384, G, Nr);
           TextStato->Caption = buffer1;
           xP1 = X;
           yP1 = Y;
           delay(1000);
        }
     }
  }
  //---------------------------------------------------------------


  //-------------------------------------------------------------
  // Procedura terminata correttamente calcolo svilupppo in cm
  //-------------------------------------------------------------
  if( err==0 ){
     Appoggio = (double)(abs((xP1 - xP0)*(xP1 - xP0)) + abs((yP1- yP0)*(yP1- yP0)));
     Distanza = (int) (sqrt_algo( Appoggio )/10);

     Sviluppo =0;
     if( AGV[NumTgv]->stato.offset_da_nodo>0 ) Sviluppo = (float) Distanza/ AGV[NumTgv]->stato.offset_da_nodo;

     sprintf( buffer, "Numero impulsi Encoder: %d, Distanza lineare %d, Sviluppo %05.3f SALVARE DATO ?",
        AGV[NumTgv]->stato.offset_da_nodo,  Distanza,  Sviluppo);
     //ShowMessage(buffer);
     if (MessageDlg( buffer, mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0) == mrYes){
        if( Sviluppo > 0 ){
           AGV[NumTgv]->mission.RapEncoder = Sviluppo;           // Rapporti Encoder dei Tgv
           RapPlc = (int) (Sviluppo*1000);
           err = LaserInvioRapportoEncoder( NumTgv, RapPlc, buffer); //
        }
     }
     TextStato->Caption = buffer;


  }
  //----------------------------------------------
  // Refresh Form
  //----------------------------------------------
  SetCaselle();

  //-------------------------------------------------------------
  // Fine PROCEDURA
  // Restituisci comunicazione con Testa LASER da PLC
  // e termina procedura
  //-------------------------------------------------------------
  err1 = LaserAbilitaTipoComunicazione( NumTgv, 2, buffer1);
  if( err )  ShowMessage(buffer);
  if( err1)  ShowMessage(buffer1);
  err1 = LaserProceduraCalibratura( NumTgv, 0, buffer1);
  if( err1 )  ShowMessage(buffer1);

  if( !err & ! err1 ) TextStato->Caption = "Procedura di calibrazione encoder terminata corretamente ";
  else TextStato->Caption = "Procedura terminata con errore";

  LaserComandoInCorso=0;
}

//---------------------------------------------------------------------------

void __fastcall TFormMappa::BtnCalSterzoClick(TObject *Sender)
{
  AnsiString InputStringa;
  char buffer[301]="";
  char buffer1[301]="";
  int err, err1, Nr;

  bool Ok;
  int TEST;
  int X,Y,A,G;
  int NodoP0;
  int NodoP1;
  int NodoP2;
  long double xP0, yP0;
  long double xP1, yP1;
  long double xP2, yP2;
  int Distanza;
  double Appoggio;
  time_t TempoInizio;
  time_t TempoFine;
  time_t TempoDurata;
  time_t OverTimeProcedura = 30;
  long double M1;
  long double M2;
  long double alpha;
  long double beta;
  long double AngoloErroreGrad;
  long double k;
  int  OldCentroSterzo;
  int  CentroSterzo;
  int  TgvAvanti;
  char Direzione[200];
  long double AlphaGrad;
  long double BetaGrad;

  TEST=0;
  Ok=false;
  //--------------------------------------------------
  // (A) Introduzione DATI CALIBRAZIONE
  //--------------------------------------------------
  if( break_Com[COM1]              )return ;
  if( AGV[NumTgv]->mission.EsclPLC ) return ;
  if( LaserComandoInCorso          ) return ;

  err=0;
  if( err==0 ){
     //--------------------------------------------------
     // Introduzione NUMERO NODO P1 e controllo validità
     //--------------------------------------------------
     InputStringa = InputBox("Introdurre Nodo su cui posizionarsi", "Introdurre il numero del nodo P1:", "");
     if(InputStringa.IsEmpty() == true) return;
     NodoP0 = AGV[NumTgv]->stato.pos;
     NodoP1 = StrToIntDef(InputStringa,0);
     if(NodoP1<1 || NodoP1>MAXPUNTI || NodoP1 == NodoP0 ){
        ShowMessage("Nodo P1 non valido");
        return;
     }
     //--------------------------------------------------
     // Introduzione NUMERO NODO P2 e controllo validità
     //--------------------------------------------------
     InputStringa = InputBox("Introdurre Nodo su cui posizionarsi", "Introdurre il numero del nodo P2:", "");
     if(InputStringa.IsEmpty() == true) return;
     NodoP2 = StrToIntDef(InputStringa,0);
     if(NodoP2<1 || NodoP2>MAXPUNTI || NodoP1 == NodoP0 || NodoP1 == NodoP2){
        ShowMessage("Nodo P2 non valido");
        return;
     }
     //-------------------------------------------------------------
     // Verifica Distanza non ecessiva
     //-------------------------------------------------------------
     if( err==0){
        xP0= Punti[NodoP0][0];
        yP0= Punti[NodoP0][1];
        xP1= Punti[NodoP1][0];
        yP1= Punti[NodoP1][1];
        xP2= Punti[NodoP2][0];
        yP2= Punti[NodoP2][1];

        Appoggio = (double)(abs((xP1 - xP0)*(xP1 - xP0)) + abs((yP1- yP0)*(yP1- yP0)));
        Distanza = (int) sqrt_algo( Appoggio );
        if( Distanza>1000 ){
           ShowMessage("Distanza da nodo P1 superiore ai 10 mt");
           return;
        }
        Appoggio = (double)(abs((xP2 - xP1)*(xP2 - xP1)) + abs((yP2- yP1)*(yP2- yP1)));
        Distanza = (int) sqrt_algo( Appoggio );
        if( Distanza>1000 ){
           ShowMessage("Distanza da nodo P2 superiore ai 10 mt");
           return;
        }
     }
     //-------------------------------------------------------------
     // Verifica se TGV pronto e in semiautomatico
     //-------------------------------------------------------------
     if( err==0){
        if( AGV[NumTgv]->test_mission_on() != false){
           ShowMessage("Il TGV NON deve essere in missione");
           return;
        }
     }
     if( err==0){
        if( AGV[NumTgv]->mission.SemiAutomatico== false){
           ShowMessage("Il TGV deve essere in semiautomatico");
           return;
        }
     }
  }
  //-------------------------------------------------------------

  //-------------------------------------------------------------
  // (A1) CONTROLLO COMUNICAZIONE DA PC
  // Abilita comunicazione con Testa LASER tramite PLC "bridge"
  // 0 = Controllo da PC
  // 1 = Guida con magneti
  // 2 = Guida Laser
  //-------------------------------------------------------------
  LaserComandoInCorso =1;
  TextStato->Caption = "Abilita comunicazione Bridge con Laser";
  err = LaserAbilitaTipoComunicazione( NumTgv, 0, buffer);
  delay( 1000 );

  //-------------------------------------------------------------
  // (A2) ATTIVA PROCEDURA CALIBRAZIONE
  //-------------------------------------------------------------
  TextStato->Caption = "Abilita Procedura CALIBRATURA STERZO";
  if(err==0) err = LaserProceduraCalibratura( NumTgv, 2, buffer);

  //-----------------------------------------------------------
  // (B) LETTURA POSIZIONE DA LASER P0
  // Solo Se Zona diversa da 0 Procedura PP per leggere
  // le posizioni direttamente dal LASER
  //-----------------------------------------------------------
if(!TEST){
  if( err==0 ){
     //----------------------------------------
     // Modo PA
     //----------------------------------------
     if(!err){
        TextStato->Caption = "PA Command" ;
        err = LaserTrsComando_PA( NumTgv, buffer);
     }
     //------------------------------------------
     // Lettura posizione media aritmetica
     //------------------------------------------
     if(!err){
        err = LaserTrsComando_PP_Media( NumTgv, &X, &Y, &A, &G, &Nr, buffer);
        if( err==0 ){
           //A= A*90/16384;  // Conversione BDEG --> GRADI
           //sprintf( buffer1, "Procedura PP Command Conclusa correttamente X=%d, Y=%d, A=%d, G=%d, Nr=%d", X, Y, A, G, Nr);
           sprintf( buffer1, "Procedura PP Command Conclusa correttamente X=%d, Y=%d, A=%d, G=%d, Nr=%d", X, Y, A*90/16384, G, Nr);
           TextStato->Caption = buffer1;
           xP0 = X;
           yP0 = Y;
           delay(1000);
        }
     }
  }
  //---------------------------------------------------------------
}

  //-----------------------------------------------------------
  // (C) SEMIAUTOMATICO SU P1
  //-----------------------------------------------------------
  if( err==0 ){
     //-------------------------------------------------------------
     // Abilita controllo da PLC
     //-------------------------------------------------------------
     if( err==0){
        TextStato->Caption = "Abilita comunicazione da PLC";
        err = LaserAbilitaTipoComunicazione( NumTgv, 2, buffer);
        delay( 1000 );
     }
     //-------------------------------------------------------------
     // Invio missione semiautomatico
     //-------------------------------------------------------------
     if( err==0){
        err = P.PosizionamentoManualeAGV( (short int) NumTgv, (short int) NodoP1, N_NO_FORCHE, &Ok, buffer);
     }
     //-------------------------------------------------------------
     // Attesa conclusione missione
     //-------------------------------------------------------------
     OverTimeProcedura = 120;  //sec
     TempoInizio = time(NULL);
     do{
        if( err ) break;
        LaserProcessaWindowsMessage( 1000 );
        //        AGV[NumTgv]->load_stato( buffer );

        TempoFine   = time(NULL);
        TempoDurata = TempoFine - TempoInizio;
        if( TempoDurata>=OverTimeProcedura ){ err=1; sprintf( buffer, "OVER-TIME" );  break;  }
        if( LaserInterrompiComando ){  err=1;  sprintf( buffer, "Comando interrotto da utente" ); break;  }

        if(( AGV[NumTgv]->stato.pos == NodoP1) &&
           ( AGV[NumTgv]->test_mission_on() ==0 ) &&
           ( AGV[NumTgv]->stato.s.bit.okprog ) ) break;

        sprintf( buffer, "Time:%d Numero impulsi Encoder: %d", OverTimeProcedura-TempoDurata,  AGV[NumTgv]->stato.offset_da_nodo );
        TextStato->Caption = buffer;
     }while (err==0);
  }

  //-----------------------------------------------------------
  // (D) LETTURA POSIZIONE DA LASER P1
  // Solo Se Zona diversa da 0 Procedura PP per leggere
  // le posizioni direttamente dal LASER
  //-----------------------------------------------------------
if(!TEST){
  if( err==0 ){
     //-------------------------------------------------------------
     // Abilita controllo da PC
     //-------------------------------------------------------------
     if(!err){
        TextStato->Caption = "Abilita comunicazione da PC";
        err = LaserAbilitaTipoComunicazione( NumTgv, 0, buffer);
        delay( 1000 );
     }
     //----------------------------------------
     // Modo PA
     //----------------------------------------
     if(!err){
        TextStato->Caption = "PA Command" ;
        err = LaserTrsComando_PA( NumTgv, buffer);
     }
     //------------------------------------------
     // Lettura posizione media aritmetica
     //------------------------------------------
     if(!err){
        err = LaserTrsComando_PP_Media( NumTgv, &X, &Y, &A, &G, &Nr, buffer);
        if( err==0 ){
           //A= A*90/16384;  // Conversione BDEG --> GRADI
           //sprintf( buffer1, "Procedura PP Command Conclusa correttamente X=%d, Y=%d, A=%d, G=%d, Nr=%d", X, Y, A, G, Nr);
           sprintf( buffer1, "Procedura PP Command Conclusa correttamente X=%d, Y=%d, A=%d, G=%d, Nr=%d", X, Y, A*90/16384, G, Nr);
           TextStato->Caption = buffer1;
           xP1 = X;
           yP1 = Y;
           delay(1000);
        }
     }
  }
  //---------------------------------------------------------------
}

  //-----------------------------------------------------------
  // (E) SEMIAUTOMATICO SU P2
  //-----------------------------------------------------------
  if( err==0 ){
     //-------------------------------------------------------------
     // Abilita controllo da PLC
     //-------------------------------------------------------------
     if( err==0){
        TextStato->Caption = "Abilita comunicazione da PLC";
        err = LaserAbilitaTipoComunicazione( NumTgv, 2, buffer);
        delay( 1000 );
     }
     //-------------------------------------------------------------
     // Invio missione semiautomatico
     //-------------------------------------------------------------
     if( err==0){
        err = AGV[NumTgv]->load_stato( buffer );
     }
     if( err==0){
        LaserProcessaWindowsMessage( 2000 );
        err = P.PosizionamentoManualeAGV( (short int) NumTgv, (short int) NodoP2, N_NO_FORCHE, &Ok, buffer);
     }
     //-------------------------------------------------------------
     // Attesa conclusione missione
     //-------------------------------------------------------------
     OverTimeProcedura = 120;  //sec
     TempoInizio = time(NULL);
     do{
        if( err ) break;
        LaserProcessaWindowsMessage( 1000 );
        //AGV[NumTgv]->load_stato( buffer );

        TempoFine   = time(NULL);
        TempoDurata = TempoFine - TempoInizio;
        if( TempoDurata>=OverTimeProcedura ){ err=1; sprintf( buffer, "OVER-TIME" );  break;  }
        if( LaserInterrompiComando ){  err=1;  sprintf( buffer, "Comando interrotto da utente" ); break;  }

        if(( AGV[NumTgv]->stato.pos == NodoP2) && ( AGV[NumTgv]->test_mission_on() ==0 ))  break;
        sprintf( buffer, "Time:%d Numero impulsi Encoder: %d", OverTimeProcedura-TempoDurata,  AGV[NumTgv]->stato.offset_da_nodo );
        TextStato->Caption = buffer;
     }while (err==0);
  }

  //-----------------------------------------------------------
  // (F) LETTURA POSIZIONE DA LASER P2
  // Solo Se Zona diversa da 0 Procedura PP per leggere
  // le posizioni direttamente dal LASER
  //-----------------------------------------------------------
if(!TEST){
  if( err==0 ){
     //-------------------------------------------------------------
     // Abilita controllo da PC
     //-------------------------------------------------------------
     if(!err){
        TextStato->Caption = "Abilita comunicazione da PC";
        err = LaserAbilitaTipoComunicazione( NumTgv, 0, buffer);
        delay( 1000 );
     }
     //----------------------------------------
     // Modo PA
     //----------------------------------------
     if(!err){
        TextStato->Caption = "PA Command" ;
        err = LaserTrsComando_PA( NumTgv, buffer);
     }
     //------------------------------------------
     // Lettura posizione media aritmetica
     //------------------------------------------
     if(!err){
        err = LaserTrsComando_PP_Media( NumTgv, &X, &Y, &A, &G, &Nr, buffer);
        if( err==0 ){
           //A= A*90/16384;  // Conversione BDEG --> GRADI
           //sprintf( buffer1, "Procedura PP Command Conclusa correttamente X=%d, Y=%d, A=%d, G=%d, Nr=%d", X, Y, A, G, Nr);
           sprintf( buffer1, "Procedura PP Command Conclusa correttamente X=%d, Y=%d, A=%d, G=%d, Nr=%d", X, Y, A*90/16384, G, Nr);
           TextStato->Caption = buffer1;
           xP2 = X;
           yP2 = Y;
           delay(1000);
        }
     }
  }
  //---------------------------------------------------------------
}




  //-------------------------------------------------------------
  // Procedura terminata correttamente calcolo svilupppo in cm
  //-------------------------------------------------------------
  if( err==0 ){
     //---------------------------------
     // Calcolo Angolo di errore
     //---------------------------------

     TgvAvanti = AGV[NumTgv]->stato.s.bit.av;

     if( xP1== xP0 ) xP0= (long double) (xP0+0.0001);
     if( xP1== xP2 ) xP2= (long double) (xP2+0.0001);
     M1 = M2 = 0; //0.001 iniziale
     if((xP1- xP0)!=0)  M1 = (long double) ( (yP1- yP0)/(xP1- xP0));
     if((xP2- xP1)!=0)  M2 = (long double) ( (yP2- yP1)/(xP2- xP1));

     alpha = (long double)(atanl(M1));
     beta  = (long double)(atanl(M2));

     AlphaGrad        = (long double)( alpha*57.295779513082320876798154814105);
     BetaGrad         = (long double)( beta *57.295779513082320876798154814105);

     //----------------------------------------------------
     // Valutare i 4 quadranti
     //----------------------------------------------------
     if( (xP1- xP0)<0 && (yP1- yP0)>0 ) AlphaGrad =(long double)(+180+AlphaGrad);
     if( (xP1- xP0)<0 && (yP1- yP0)<0 ) AlphaGrad =(long double)(+180+AlphaGrad);
     if( (xP1- xP0)>0 && (yP1- yP0)<0 ) AlphaGrad =(long double)(+360+AlphaGrad);

     if( (xP2- xP1)<0 && (yP2- yP1)>0 ) BetaGrad =(long double)(+180+BetaGrad);
     if( (xP2- xP1)<0 && (yP2- yP1)<0 ) BetaGrad =(long double)(+180+BetaGrad);
     if( (xP2- xP1)>0 && (yP2- yP1)<0 ) BetaGrad =(long double)(+360+BetaGrad);


     //------------------------------------------------------------
     // Se errore supera i 180° aggiustare +- secondo il quadrante
     //------------------------------------------------------------
     AngoloErroreGrad  = (long double) (AlphaGrad-BetaGrad);
     if( AngoloErroreGrad> 180 ) AngoloErroreGrad= (long double) (AngoloErroreGrad-360);
     if( AngoloErroreGrad<-180 ) AngoloErroreGrad= (long double) (AngoloErroreGrad+360);


     //-------------------------------------------
     // Recupero DM128 Centro sterzo e correzione
     // con il fattottore K (somma se TGV indietro
     // sottrazione se TGV avanti )
     //-------------------------------------------
     k = (long double) (AngoloErroreGrad*2000/90/2);
     err = LaserLeggiCentroSterzo( NumTgv, &OldCentroSterzo, buffer);
     if( err==0 ){
        if( TgvAvanti==0 ) {CentroSterzo = (int) (OldCentroSterzo + k); strcpy( Direzione, "Indietro");}
        if( TgvAvanti!=0 ) {CentroSterzo = (int) (OldCentroSterzo - k); strcpy( Direzione, "Avanti");}

        sprintf( buffer1, "Alpha %01.3f, Beta %01.3f, Angolo di errore %01.3f, Direzione %s, K=%01.3f,\n vecchio valore CS=%d, nuovo valore CS=%d \n  SALVARE DATO ?",
           (float) AlphaGrad,
           (float) BetaGrad,
           (float) AngoloErroreGrad,
           Direzione,
           (float) k,
           OldCentroSterzo,
           CentroSterzo);

        //ShowMessage(buffer);
        if (MessageDlg( buffer1, mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0) == mrYes){
           err = LaserInvioCentroSterzo( NumTgv, CentroSterzo, buffer);
        }
     }
     TextStato->Caption = buffer1;
  }
  //----------------------------------------------
  // Refresh Form
  //----------------------------------------------
  SetCaselle();

  //-------------------------------------------------------------
  // Fine PROCEDURA
  // Restituisci comunicazione con Testa LASER da PLC
  // e termina procedura
  //-------------------------------------------------------------
  err1 = LaserAbilitaTipoComunicazione( NumTgv, 2, buffer1);
  if( err )  ShowMessage(buffer);
  if( err1)  ShowMessage(buffer1);
  err1 = LaserProceduraCalibratura( NumTgv, 0, buffer1);
  if( err1 )  ShowMessage(buffer1);

  if( !err & ! err1 ) TextStato->Caption = "Procedura di calibrazione STERZO terminata corretamente ";
  else TextStato->Caption = "Procedura terminata con errore";

  LaserComandoInCorso=0;
}
//---------------------------------------------------------------------------

void __fastcall TFormMappa::BtnCancellaClick(TObject *Sender)
{
  AnsiString InputStringa;
  int  Num;
  char buffer[301]="";
  int err;

  //----------------------------------------------------
  // (A) Introduzione DATI per il CANCELLAMENTO MAPPING
  //----------------------------------------------------
  err=0;
  if( err==0 ){
     //--------------------------------------------------
     // Introduzione NUMERO ZONA
     //--------------------------------------------------
     InputStringa = InputBox("CANCELLAZIONE del mapping zona LASER", "Introdurre il numero della zona:", "");
     if(InputStringa.IsEmpty() == true) return;
     Num = StrToIntDef(InputStringa,0);
     //----------------------------------------------
     // Controllo della validità del valore inserito
     //----------------------------------------------
     if(Num<0 || Num>MAXZONEMAPPING){
        ShowMessage("Zona non valida... Il valore deve essere compreso tra 0..40!");
        return;
     }
     NumZona = Num;
     //--------------------------------------------------
     // Chiedi CONFERMA CANCELLAZIONE
     //--------------------------------------------------
     if (MessageDlg( "SEI VERAMENTE SICURO DI VOLERE CANCELLARE LA ZONA", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0) != mrYes){
        return;
     }
     //-------------------------------------------------------------
     // Recupero i dati generali di default per la zona 0
     //-------------------------------------------------------------
     GetCaselle();

     memset(&DatiZona.Z[NumZona], 0, sizeof(LAYER));

     //--------------------------------------------
     // Salvataggio file e aggiorno MaxZoneMappate
     //--------------------------------------------
     if( DatiZona.MaxZoneMappate>0 && NumZona==(DatiZona.MaxZoneMappate-1) ) DatiZona.MaxZoneMappate--;
     //----------------------------------------------
     // Memorizzazione della zona di mapping
     //----------------------------------------------
     //WriteIniLaser(NumTgv, NumZona, &DatiZona.Z[NumZona]);
     memcpy( &AGV[NumTgv]->zone_mapping, &DatiZona, sizeof(DatiZona));
     if(AGV[NumTgv]->save_zone_mapping(buffer)) ShowMessage(buffer);
  }

  if( !err ) TextStato->Caption = "Procedura di CANCELLAZIONE terminata corretamente ";

  //----------------------------------------------
  // Refresh Form
  //----------------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------

void __fastcall TFormMappa::BtnSaveClick(TObject *Sender)
{
  char buffer[301]="";
  int err=0;
  char all_mess[300];
  char FileName[300];

  //----------------------------------------------
  // Memorizzazione della zona di mapping
  //----------------------------------------------
  GetCaselle();
  memcpy( &AGV[NumTgv]->zone_mapping, &DatiZona, sizeof(DatiZona));
  if(AGV[NumTgv]->save_zone_mapping(buffer)) ShowMessage(buffer);

  //--------------------------------------------------------------------------------------------------------------------
  // GB-12/04/2012 Trasmetti di default quelli della zona 0 se NumZona==1000
  //--------------------------------------------------------------------------------------------------------------------
  if(AGV[NumTgv]->zone_mapping.MaxZoneMappate) LaserInvioNumeroMaxZone( NumTgv, AGV[NumTgv]->zone_mapping.MaxZoneMappate-1, buffer);
  if( NumZona<MAXZONEMAPPING && DatiZona.Z[NumZona].Minr>=3 && DatiZona.Z[NumZona].Minr<=20 ){
     if(!err) err = LaserInvioNumeroSpecchi ( NumTgv, DatiZona.Z[NumZona].Minr, buffer);
  }else{
     if(!err) err = LaserInvioNumeroSpecchi ( NumTgv, DatiZona.Z[0].Minr, buffer);
  }
  //----------------------------------------------
  // Compila Dati file TXT
  //----------------------------------------------
  LaserWriteTgvMappingTxt( NumTgv, FileName, all_mess);

  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormMappa::GridCoordinateDrawCell(TObject *Sender, int Col, int Row, TRect &Rect, TGridDrawState State)
{
  char buffer[21]="";
  TColor col = clBtnFace;
  //----------------------------------------------------
  // Riga non valida
  //----------------------------------------------------
  if(Row<=0 || Row>MAXPUNTI+1) return;
  //----------------------------------------------------
  // Gestione colori nella griglia
  //----------------------------------------------------
  switch(Col){
     case  0: Rect.Left  = Rect.Left+40;
              Rect.Right = Rect.Left+20;
              strcpy(buffer, GridCoordinate->Cells[Col][Row].c_str());
              if(buffer[0]=='*') col = clRed;
              break;
     default: return;
  }
  //----------------------------------------------------
  // Colorazione del blocchetto
  //----------------------------------------------------
  GridCoordinate->Canvas->Brush->Color = col;
  GridCoordinate->Canvas->FillRect(Rect);
}
//---------------------------------------------------------------------------

void __fastcall TFormMappa::BtnVisualizzaClick(TObject *Sender)
{
  AnsiString InputStringa;
  char all_mess[300];
  char buffer[300];
  char FileName[300];
  //int  err=0;
  //int  Num;

  //----------------------------------------------
  // Compila Dati
  //----------------------------------------------
  LaserWriteTgvMappingTxt( NumTgv, FileName, all_mess);

  //----------------------------------------------
  // Visulaizza File
  //----------------------------------------------
  strcpy(FormVisualizzaFile->NomeFileVisualizza, FileName);
  strcpy(FormVisualizzaFile->CaptionFileVisualizza,      "Visualizzazione Dati Mapping");
  sprintf( buffer, "Dati TGV%d", NumTgv );
  strcpy(FormVisualizzaFile->IntestazioneFileVisualizza, buffer);
  FormVisualizzaFile->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TFormMappa::BtnChangeReflectorClick(TObject *Sender)
{
  int Num;
  AnsiString InputStringa;
  char buffer[301]="";
  char buffer1[301]="";
  int  i, err, err1, Nr;
  int  ZonaIniziale;
  int  ZonaFinale;
  bool SempreON=true;
  int  result;

  bool NoDownloadRiflettori=true;

  err=err1=0;
  //--------------------------------------------------
  // Introduci numero ZONA
  //--------------------------------------------------
  InputStringa = InputBox("Rilettura dati zona da file Mappaxx.dat e DOWNLOAD", "Introdurre il numero della zona: ", "");
  if(InputStringa.IsEmpty() == true) return;
  Num = StrToIntDef(InputStringa,0);
  //----------------------------------------------
  // Controllo della validità del valore inserito
  //----------------------------------------------
  if(Num<0 || Num>MAXZONEMAPPING){
     ShowMessage("Zona non valida... Il valore deve essere compreso tra 0..40!");
     return;
  }
  NumZona=Num;
  //---------------------------------------------------
  // Rilettura da file Mappaxx.dat dei dati della zona
  //---------------------------------------------------
  if(!err){
     err = AGV[NumTgv]->load_zone_mapping(buffer);
     memcpy(&DatiZona, &AGV[NumTgv]->zone_mapping, sizeof(DatiZona));
  }
  if(err) return;
  //----------------------------------------------------------------
  // Cancellazione Riflettori inutili
  //----------------------------------------------------------------
  if(SempreON==true){
     //----------------------------------------------------------------
     // Apri pagina per la cancellazione dei riflettori
     //----------------------------------------------------------------
     memset(&FormDisRiflettori->ElencoSpecchi[0], 0, sizeof(FormDisRiflettori->ElencoSpecchi));
     memcpy(&FormDisRiflettori->DatiZona, &AGV[NumTgv]->zone_mapping, sizeof(DatiZona));
     FormDisRiflettori->NumSpecchiIniziale=DatiZona.Z[NumZona].Nr;
     FormDisRiflettori->NumZona           =NumZona;
     FormDisRiflettori->ZonaModificata    =false;
     for( i=0; i<DatiZona.Z[NumZona].Nr; i++ ){
        if( i>MAXRIFLETTORI ) break;
        FormDisRiflettori->ElencoSpecchi[i]=(short int) i;
     }
     FormDisRiflettori->ShowModal();
     //----------------------------------------------------------------
     // Conferma cancellazione
     //----------------------------------------------------------------
     if(FormDisRiflettori->ZonaModificata==true){
        sprintf( buffer1, "Riflettori eliminati %d --> %d Confermi", FormDisRiflettori->NumSpecchiIniziale, FormDisRiflettori->NumSpecchiModificato );
        result = Application->MessageBox( buffer1, "ATTENZIONE", MB_OK | MB_YESNO);
        if(result != IDYES){
           FormDisRiflettori->ZonaModificata=false;
        }
     }
     //----------------------------------------------------------------
     // Salvataggio Zona Mapping
     //----------------------------------------------------------------
     if(FormDisRiflettori->ZonaModificata==true){
        //----------------------------------------------
        // Memorizzazione della zona di mapping
        //----------------------------------------------
        memcpy(&DatiZona,                  &FormDisRiflettori->DatiZona, sizeof(DatiZona));
        memcpy(&AGV[NumTgv]->zone_mapping, &FormDisRiflettori->DatiZona, sizeof(DatiZona));
        if( AGV[NumTgv]->save_zone_mapping(buffer)) ShowMessage(buffer);
        //----------------------------------------------
        // Refresh Form
        //----------------------------------------------
        SetCaselle();
     }
  }
  if(FormDisRiflettori->ZonaModificata==false) return;

  ShowMessage("La zona modificatra è salvata solo su file locale \nSe le modifiche apportate sono corrette scaricare riflettori su NAV \nAltrimenti ricaricare riflettori da NAV" );
  //--------------------------------------------------------------------
  // Se si preferisce trasferimeto a Nav da pulsante dedicato -->SKIP
  //
  // In questo modo si ha la possibilità di vedere graficamente
  // e poi scaricare o recuperare posizione centrale
  //--------------------------------------------------------------------
  if(NoDownloadRiflettori             ) return;
  if(LaserComandoInCorso              ) return ;

  LaserComandoInCorso=1;
  //-------------------------------------------------------------
  // Abilita comunicazione con Testa LASER tramite PLC "bridge"
  // 0 = Controllo da PC
  // 1 = Guida con magneti
  // 2 = Guida Laser
  //-------------------------------------------------------------
  TextStato->Caption = "Abilita comunicazione Bridge con Laser";
  err = LaserAbilitaTipoComunicazione( NumTgv, 0, buffer);
  delay( 1000 );
  //---------------------------------------------------
  // GB-15/03/2012 Attesa manuale abilitazione Bridge
  //---------------------------------------------------
  //sprintf( buffer1, "Attesa Abilita Bridge");
  //Application->MessageBox( buffer1, "ATTENZIONE", MB_OK | MB_ICONEXCLAMATION);
  //----------------------------------------
  // Ciclo FOR per SOLO una Zona
  //----------------------------------------
  ZonaIniziale=NumZona;
  ZonaFinale  =NumZona;
  for( NumZona=ZonaIniziale; NumZona<=ZonaFinale; NumZona++){
     if( err ) break;
     //----------------------------------------
     // Modo SA necessario
     // altrimenti restituisce errore
     //----------------------------------------
     if(!err){
        TextStato->Caption = "SA Command" ;
        err = LaserTrsComando_SA( NumTgv, buffer);
     }
     //------------------------------------
     // Modalita DA -->Download
     //------------------------------------
     if(!err){
        TextStato->Caption = "DA Command";
        err = LaserTrsComando_DA( NumTgv, buffer);
     }
     //------------------------------------
     // Set Raggio dei riflettori
     //------------------------------------
     if(!err){
        TextStato->Caption = "RS Command";
        err = LaserTrsComando_RS( NumTgv, NumZona, DatiZona.Z[NumZona].Rr, buffer);
     }
     //------------------------------------------------
     // Salvataggio su eprom dei rifletori -->Download
     //------------------------------------------------
     if(!err){
        for( i=0; i<=DatiZona.Z[NumZona].Nr; i++ ){
           if( DatiZona.Z[NumZona].Nr<0 ) break;
           if(i==0){
              sprintf( buffer1, "DR Command Riflettore Nr.%d          ", i);
              TextStato->Caption = buffer1;
           }
           if( i!=DatiZona.Z[NumZona].Nr ) err = LaserTrsComando_DR( NumTgv, NumZona, i, DatiZona.Z[NumZona].Xr[i], DatiZona.Z[NumZona].Yr[i], &Nr, buffer);
           if( i==DatiZona.Z[NumZona].Nr ) err = LaserTrsComando_DR( NumTgv, NumZona,-1, DatiZona.Z[NumZona].Xr[i], DatiZona.Z[NumZona].Yr[i], &Nr, buffer);
           if( err==0){
              sprintf( buffer1, "DR Command Riflettore Richiesto:%d Restituito:%d", i, Nr);
              TextStato->Caption = buffer1;
              delay( 200 ); // Per visualizzazione !!
           }
           if( i>MAXRIFLETTORI ) break;
           if( Nr<0 ) break;
           if( err ) break;
        }
     }
     if( err ) break;
  }

  //----------------------------------------------
  // Refresh Form
  //----------------------------------------------
  SetCaselle();

  //-------------------------------------------------------------
  // Restituisci comunicazione con Testa LASER da PLC
  //-------------------------------------------------------------
  err1 = LaserAbilitaTipoComunicazione( NumTgv, 2, buffer1);
  if( err )  ShowMessage(buffer);
  if( err1)  ShowMessage(buffer1);
  if( !err & ! err1 ) TextStato->Caption = "Procedura di Dowload terminata corretamente ";

  LaserComandoInCorso=0;
}
//---------------------------------------------------------------------------

void __fastcall TFormMappa::EdLaserMappaRiflettoriTGVChange(
      TObject *Sender)
{
  //---------------------------------------------------
  // Visualizzazione Riflettori
  //---------------------------------------------------
  LaserMappaRiflettoriTgv  = atoi(EdLaserMappaRiflettoriTGV->Text.c_str());
  LaserMappaRiflettoriZona = atoi(EdLaserMappaRiflettoriZona->Text.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TFormMappa::EdLaserMappaRiflettoriZonaChange(
      TObject *Sender)
{
  //---------------------------------------------------
  // Visualizzazione Riflettori
  //---------------------------------------------------
  LaserMappaRiflettoriTgv  = atoi(EdLaserMappaRiflettoriTGV->Text.c_str());
  LaserMappaRiflettoriZona = atoi(EdLaserMappaRiflettoriZona->Text.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TFormMappa::CBVisualizzaTrasparenteClick(TObject *Sender)
{
  FormLayout->VisualizzaTgvTrasparente[NumTgv] = CBVisualizzaTrasparente->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormMappa::btnTgvUpDnClick(TObject *Sender)
{
  int myTag,myTgv;

  myTag = (((TBitBtn *)Sender)->Tag);

  myTgv=NumTgv;

  if(myTag==0) myTgv=NumTgv-1;
  if(myTag==1) myTgv=NumTgv+1;

  if(myTgv<1)       myTgv=MAXAGV;
  if(myTgv>MAXAGV)  myTgv=1;

  NumTgv=myTgv;
  Activate();
}
//---------------------------------------------------------------------------
void __fastcall TFormMappa::ScaricaMappaSuPLC()
{
  double dist;
  short int i, j, p1, p2;
  int x1, x2, y1, y2, distX, distY;
  int Id, Raggio, Circonferenza;
  char buffer[101]="";

  //--------------------------------------------
  // Compilazione della struttura con le X, Y
  //--------------------------------------------
  GetCaselle();
  TimerMapping->Enabled = false;    // Timer di refresh della form di mapping
  //--------------------------------------------
  // Scrittura dei dati su PLC
  //--------------------------------------------
  SaveDati();
  LoadDati();
  TimerMapping->Enabled = true;     // Timer di refresh della form di mapping
  //--------------------------------------------
  // memorizzazione nella struttura globale
  // e salvataggio su File
  //--------------------------------------------
  //WriteIniLaser(NumTgv, NumZona, &DatiZona.Z[NumZona]);
  memcpy(&AGV[NumTgv]->zone_mapping, &DatiZona, sizeof(DatiZona));
  if(AGV[NumTgv]->save_zone_mapping(buffer)){
     ShowMessage(buffer);
  }
  
  RiattivaMapping[NumTgv]=false;
  //--------------------------------------------
  // Ricalcolo per sicurezza anche tutte le
  // distanze nodi
  //--------------------------------------------
  for(i=0; i<MAXPUNTI; i++){
     if(N.n[i]->num<1        ) continue;
     if(N.n[i]->num>MAXPUNTI ) continue;
     p1 = N.n[i]->num;
     //--------------------------------------------
     // scorrimento di tutte le righe di percorso
     //--------------------------------------------
     for(j=0; j<MAXPUNTINODO; j++){
        if(N.n[i]->rec[j].ps==0) continue;
        dist = 0;
        p2 = N.n[i]->rec[j].ps;
        //----------------------------------
        // Recupero coordinate
        //----------------------------------
        x1 = Punti[p1][0];
        y1 = Punti[p1][1];
        x2 = Punti[p2][0];
        y2 = Punti[p2][1];
        //----------------------------------
        // Se i nodi si trovano in retta la
        // distanza viene calcolata con il
        // teorema di pitagora.
        //----------------------------------
        distX = abs(x1-x2);
        distY = abs(y1-y2);
        if(distX>30000 || distY>30000) continue;
        dist  = (double)(sqrt_algo((distX*distX)+(distY*distY)));
        //----------------------------------
        // Calcolo della distanza percorsa
        // su un arco di circonferenza.
        //----------------------------------
        Id=Raggio=Circonferenza=0;
        if(!N.CercaCurva(TRUE, p1, p2, &Raggio, &Circonferenza, &Id)){
           dist = (short int)(Circonferenza);
        }
        N.n[i]->rec[j].dist=(short int)(dist);
     }
  }
  //--------------------------------------------
  // Salvataggio del file dei nodi
  //--------------------------------------------
  N.save_file_nodi(buffer);
  //--------------------------------------------
  // Rinfresco grafico della Form
  //--------------------------------------------
  SetCaselle();
}

