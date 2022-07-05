//---------------------------------------------------------------------------
#include <vcl.h>
#include <dir.h>
#include <io.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <share.h>
#include <sys\stat.h>


#pragma hdrstop

#define MAINFILE 1

#include "Main.h"
#include "Stato.h"
#include "Layout.h"
#include "AgvWin.h"
#include "MacWin.h"
#include "BaiWin.h"
#include "MagWin.h"
#include "BoxWin.h"
#include "BatWin.h"
#include "NodoWin.h"
#include "VisFile.h"
#include "Listagen.h"
#include "Remoto.h"
#include "Password.h"
#include "Messagg.h"
#include "SemiAuto.h"
#include "PorteImp.h"
#include "Video.h"
#include "TestP.h"
#include "TestPlc1.h"
#include "TestPlc2.h"
#include "TestPlc3.h"
#include "ModBus.h"
#include "StatTgv.h"
#include "SQLDB.h"
#include "Input.h"
#include "ListaCar.h"
#include "UdpServer1.h"
#include "AlgoScambio.h"
#include "OmronEthTcp.h"
#include "OmronETH.h"
#include "OmronEthSer.h"
#include "AllenBViaVbOpc.h"
#include "Slc500.h"
#include "TipiBox.h"
#include "GruppiBa.h"
#include "ComunicaSiemens.h"
#include "AlgoNoDave.h"
#include "SrvTcpAndroid.h"
#include "StatoBaiewin.h"
#include "StatoMagwin.h"
#include "XYNodi.h"
#include "Impianti.h"

//---------------------------------------------------------------------------
// PLUGING IMPIANTI
//---------------------------------------------------------------------------
#include PLANT_PLUGIN

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "XLSMini"
#pragma link "WSocket"
#pragma resource "*.dfm"
TFormMain *FormMain;
int LastTimeMinutoCiclo=0;

//----------------------------------------------------
// Variabili definite in GRAPHVAR.CPP
//----------------------------------------------------
extern int   offset[][2];
extern int   offset1[][2];
extern int   OffSV1;
extern int   OffSO1;
extern float RidAll;
extern float RidMin;
extern float RidMax;
extern float Rap;

//  ---------------------
//   STRUTTURA NODI
//  ---------------------
//0,1 = x,y posizione centrale nodo
//  2 = numero nodo
//  3 = occupazione agv ( numero AGV oppure libero = 0 )
//-------------------------------------------------------
extern int Punti[][5];
extern int PercorsoCurvaBezier[][7];
extern int ConvLeggiFileGraphPuntiAscii( char *NomeFile, char* all_mess);
extern int ConvLeggiFileGraphPercorsiCurvaAscii( char *NomeFile, char* all_mess);
extern int ConvLeggiFileGraphPercorsiCurvaBezierAscii( char *NomeFile, char* all_mess);

//  ---------------------
//   STRUTTURE GRAFICHE
//  ---------------------
//
extern int Baie[][4];

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MEsciClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormResize(TObject *Sender)
{
  //---------------------------------------------------
  // Gestione posizionamento Form di gestione impianto
  //---------------------------------------------------
  if(!FormInClose){
     FormLayout->Left   = 0;
     FormLayout->Top    = 0;
     FormLayout->Height = FormMain->ClientHeight-83;
     FormStato->Height  = FormLayout->Height;
     if(ModificaCoordinateNodi==false){
        FormLayout->Width   = FormMain->ClientWidth-FormStato->Width-4;
        FormXYNodi->Height  = 0;
        FormStato->Left     = FormLayout->Width;
        FormStato->Top      = FormLayout->Top;
     }
     else{
        FormLayout->Width   = FormMain->ClientWidth-FormXYNodi->Width-FormStato->Width-4;
        FormXYNodi->Left    = FormLayout->Width;
        FormXYNodi->Top     = FormLayout->Top;
        FormXYNodi->Height  = FormLayout->Height;
        FormStato->Left     = FormLayout->Width+FormXYNodi->Width;
        FormStato->Top      = FormLayout->Top;
     }
  }
  //---------------------------------------------------
  // Riposizionamento dei pulsanti di gestione layout
  //---------------------------------------------------
  BtnNodo->Left        = SpeedBar->Width - 170;
  BtnLayout->Left      = SpeedBar->Width - 130;
  BtnZoomAll->Left     = SpeedBar->Width -  90;
  BtnZoomNumeri->Left  = SpeedBar->Width -  50;

  //---------------------------------------------------
  // Ridimensionamento della barra di stato
  //---------------------------------------------------
  // if(SBarImpianto->Width>0) SBarImpianto->Panels->Items[0]->Width = (int)(SBarImpianto->Width-695);
  if(SBarImpianto->Width>0) SBarImpianto->Panels->Items[0]->Width = (int)(SBarImpianto->Width-755);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SBarImpiantoDrawPanel(TStatusBar *StatusBar, TStatusPanel *Panel, const TRect &Rect)
{
  short int i, Stato;
  short int almeno_un_tgv;
  short int almeno_un_plc;
  bool errore_io_remoti;
  bool com_tgv_dis, com_tgv_err;

  TCanvas *pCanvas = SBarImpianto->Canvas;
  // --- Settoggio del Font ---
  pCanvas->Font->Name   = "Arial";
  pCanvas->Font->Size   = 8;
  //---------------------------------------------
  // Recupero lo stato dell'Impianto
  //---------------------------------------------
  Stato=0;
  if(Automatico==true && !FormLayout->TestPercorsiTecnoFerrari && !LogTgvAttivato) Stato=1;
  if(Automatico==false || FormLayout->TestPercorsiTecnoFerrari || LogTgvAttivato ) Stato=2;
  if(BackupInCorso==true           ) Stato=3;
  if(FormTgv->EditorAgvAttivo ||
     EditorBoxAttivo ||
     EditorMagazzinoAttivo ||
     EditorCBattAttivo ||
     EditorMacchineAttivo ||
     EditorBaieAttivo              ) Stato=4;
  if(ModificaCoordinateNodi==true  ) Stato=5;
  if(IncendioInCorso==true         ) Stato=6;
  if(AbilitaSimulazione==true      ) Stato=7;
  //---------------------------------------------
  // Verifico flag di riattivazione mapping
  //---------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(RiattivaMapping[i]==false) continue;
     Stato=8;
     break;
  }
  //---------------------------------------------
  // Cominicazione TGV1, ..., PLC, MACCHINE
  //---------------------------------------------
  if(Panel->Index==1){
     //-----------------------------------------
     // verifico se c'è almeno un TGV in comun.
     //-----------------------------------------
     com_tgv_dis=false;
     if(break_Com[COM1]==true) com_tgv_dis=true;
     almeno_un_tgv=false;
     for(i=1; i<=MAXAGV; i++){
        if(AGV[i]->mission.EsclPLC==true) continue;
        almeno_un_tgv=true;
        break;
     }
     if(almeno_un_tgv==false ) com_tgv_dis=true;
     //-----------------------------------------
     // verifico se c'è almeno un TGV in errore
     //-----------------------------------------
     com_tgv_err=false;
     almeno_un_tgv=false;
     for(i=1; i<=MAXAGV; i++){
        if(almeno_un_tgv) break;
        if(errore_comunicazione[COM1][i] && !AGV[i]->mission.ErrCom) almeno_un_tgv=true;
      //if(!errore_comunicazione[COM1][i] || AGV[i]->mission.ErrCom) col = clLime;
        if(AGV[i]->allarme_interno==ALL_INT_ERR_COMUNIC            ) almeno_un_tgv=true;  // errore com persistente.
     }
     if(almeno_un_tgv==true ) com_tgv_err=true;
     //-----------------------------------------
     // visualizzazione stato comunicazione
     //-----------------------------------------
     if(com_tgv_dis==true) pCanvas->Brush->Color = clBtnFace;
     else{
        if(com_tgv_err==false) pCanvas->Brush->Color = clLime;
        else                   pCanvas->Brush->Color = clRed;
     }
  }
  if(Panel->Index==2){
     pCanvas->Font->Color = clBlack;
     if( break_Com[COM2]) pCanvas->Brush->Color = clBtnFace;
     if(!break_Com[COM2]){
        almeno_un_plc=false;
        for(i=0; i<=MAXPLCCON; i++){
           if(almeno_un_plc                 ) break;
           if(!errore_comunicazione[COM2][i]) continue;
           almeno_un_plc=true;
           break;
        }
        if(almeno_un_plc==true){
           pCanvas->Brush->Color = clRed;
        }
        else{
           pCanvas->Brush->Color = clLime;
        }
     }
  }
  //--------------------------------------
  // Visualizzazione COMUNICAZIONE I/O
  //--------------------------------------
  if(Panel->Index==3){
     pCanvas->Font->Color = clBlack;
     if(break_Com[COM6]) pCanvas->Brush->Color = clBtnFace;
     else{
        errore_io_remoti=false;
        if( errore_comunicazione[COM6][PLC_IO_REMOTO_01] ) errore_io_remoti = true;
        if( errore_comunicazione[COM6][PLC_IO_REMOTO_02] ) errore_io_remoti = true;
        if( errore_comunicazione[COM6][PLC_IO_REMOTO_03] ) errore_io_remoti = true;
        if( errore_comunicazione[COM6][PLC_IO_REMOTO_04] ) errore_io_remoti = true;
        if( errore_comunicazione[COM6][PLC_IO_REMOTO_05] ) errore_io_remoti = true;
        if( errore_comunicazione[COM6][PLC_IO_REMOTO_06] ) errore_io_remoti = true;
        if( errore_comunicazione[COM6][PLC_IO_REMOTO_07] ) errore_io_remoti = true;
        if( errore_comunicazione[COM6][PLC_IO_REMOTO_08] ) errore_io_remoti = true;
        if( errore_comunicazione[COM6][PLC_IO_REMOTO_09] ) errore_io_remoti = true;
        if( errore_comunicazione[COM6][PLC_IO_REMOTO_10] ) errore_io_remoti = true;
        if( errore_io_remoti==true ) pCanvas->Brush->Color = clRed;
        else                         pCanvas->Brush->Color = clLime;
     }
  }
  //--------------------------------------
  // Visualizzazione COMUNICAZIONE SQL
  //--------------------------------------
  if(Panel->Index==4){
     pCanvas->Font->Color = clBlack;
     if( break_Com[COM3]) pCanvas->Brush->Color = clBtnFace;
     if(!break_Com[COM3]){
        if(errore_comunicazione[COM3][DB_DBASE]){
           pCanvas->Brush->Color = clRed;
        }
        else{
           pCanvas->Brush->Color = clLime;     // connessione on
        }
     }
  }
  //------------------------------------------
  // Gestione Password
  //------------------------------------------
  if(Panel->Index==8){
     pCanvas->Font->Color = clBlack;
     if(LivelloPassword==0) pCanvas->Brush->Color = clBtnFace;
     if(LivelloPassword==1) pCanvas->Brush->Color = clAqua;
     if(LivelloPassword==2) pCanvas->Brush->Color = clYellow;
     if(LivelloPassword==3) pCanvas->Brush->Color = clRed;
  }
  //------------------------------------------
  // Visualizzazione stato impianto
  //------------------------------------------
  if(Panel->Index==9){
     pCanvas->Font->Color  = clBlack;
     switch(Stato){
        case  1: pCanvas->Brush->Color = clLime;
                 break;
        case  2: pCanvas->Brush->Color = clRed;
                 break;
        case  3: pCanvas->Brush->Color = clYellow;
                 break;
        case  4: pCanvas->Brush->Color = clYellow;
                 break;
        case  5: pCanvas->Brush->Color = (TColor)(0x00FFFF80);
                 break;
        case  6: pCanvas->Brush->Color = clRed;
                 break;
        case  7: pCanvas->Brush->Color = (TColor)(0x004080FF);
                 break;
        case  8: pCanvas->Brush->Color = (TColor)(0x004080FF);
                 break;
        default: pCanvas->Brush->Color = clBtnFace;
                 break;
     }
  }
  //------------------------------------------
  // Abilitazione PC SERVER/CLIENT
  //------------------------------------------
  if(Panel->Index==10){
     pCanvas->Font->Color  = clRed;
     pCanvas->Brush->Color = clBtnFace;
  }
  //------------------------------------------
  // Refresh
  //------------------------------------------
  pCanvas->FillRect(Rect);
  pCanvas->TextOut(Rect.Left, Rect.Top, Panel->Text);
//pCanvas->Rectangle(Rect.Left, Rect.Top, Rect.Left+Panel->Width, Rect.Top+SBarIsm2000->Height);


}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  int i=0;
  WORD dm[3];
  char buffer[201]="";

  CanClose=0;
  //-------------------------------------------------------------------------
  // Salva i nodi occupati e le missioni prima di uscire dalla applicazione
  //-------------------------------------------------------------------------
  N.save_nodo_grd( buffer );
  N.save_nodo_busy( buffer );
  N.save_nodi_tgv_lanciati_su_rettilineo( buffer );   // GESTIONE PRECEDENZE PER TGV IN RETTILINEO
  for( i=1; i<=MAXAGV; i++){
     AGV[i]->save_mission( buffer );
     AGV[i]->save_zone_mapping(buffer);
  }
  //--------------------------------------------------------------
  // *** Memorizzazione delle impostazioni Form principale ***
  //--------------------------------------------------------------
  WriteIniFormMain();
  //--------------------------------------------------------------
  // *** Memorizzazione delle impostazioni di stistema ***
  //--------------------------------------------------------------
  WriteIniProject();
  AllenB_WriteIniProject();
  //--------------------------------------------------------------
  // Impostazioni di pogetto specializzate
  //--------------------------------------------------------------
  WriteIni_IMPIANTI();     // impostazioni di progetto specializzate
  //--------------------------------------------------------------
  // Conferma exit da software
  //--------------------------------------------------------------
  sprintf(buffer, MESS[  1], AGGIORNAMENTO, VERSIONE_IMPIANTO);
  switch(Application->MessageBox(MESS[172], buffer, MB_YESNO | MB_ICONQUESTION)) {
  case IDYES:
     CanClose=1;
     FormInClose=true;
     //-----------------------------------------------------------------
     // Chiusura e distruzione oggetti di comunicazione con il DM INCAS
     //-----------------------------------------------------------------
     ADOConnSQL->Close();
     //-----------------------------------------------------------------
     // Chiusura SERVER ALGODROID
     //-----------------------------------------------------------------
     FormSrvTcpAndroid->StopServerTCPAndroid(0);
     //-----------------------------------------------------------------
     // Attivazione "Timer" delle finestre principale
     //-----------------------------------------------------------------
     TimerLampeggio->Enabled          = false;
     TimerCiclo->Enabled              = false;    // Timer principale di gestione RunAutomatico
     TimerRefresh->Enabled            = false;
     //-----------------------------------------------------------------
     // Disattivazione "Timer" delle finestre secondarie
     //-----------------------------------------------------------------
     FormMappa->TimerMapping->Enabled = false;    // Timer di aggiornamento del Timer di Mapping
     //-----------------------------------------------------------------
     // Settaggio della DM302 che viene utilizzata dal PLC per segnalare
     // l'eventuale blocco della comunicazione con il PC.
     //-----------------------------------------------------------------
//     if(!break_Com[COM2]){
//        dm[0]=1;
//        if(OM.wr_canale(&dm[0], 0, "WD", 302, 1, buffer)) return;
//     }
     //-----------------------------------------------------------------
     // Registrazione allo spegnimento del software
     //-----------------------------------------------------------------
     if(IdPC<2) FileLogOperatore("(TGVSPEC) - Close del software");
     //-------------------------------------------------------------------------
     // Compattazione dei file (alla chiusura dell'applicazione)
     //-------------------------------------------------------------------------
     //if(PcAlgo==false && IdPC<2) RemSalvaAnomalie( TRUE );


  }
  return;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCreate(TObject *Sender)
{
  int  msec, err, Id;
  char buffer[101]="";
  int  TotMac  = TOTCHIAMATE;
  int  TotMag  = TOTMAG;
  int  TotBaie = TOTBAIE;
  int  TotPort = TOTPORTE;
  int  TotBatt = TOTBATT;

  SYSTEMTIME Time1;
  SYSTEMTIME Time2;

  err=0;
  Rid=RidAll;
  PcAlgo=true;               // PC ALGOSISTEMI
  FormInOpen=true;
  FormInClose=false;
  //----------------------------------------------------------
  // Inizializza il la variabile di ritardo per delay()
  //----------------------------------------------------------
  Calibrate_Delay();
  //----------------------------------------------------------
  // Selezione del tipo di PC      (SERVER/CLIENT)
  //----------------------------------------------------------
  Id=IdPC=0;                 // azzeramento identificativo
  FormInClose=false;         // flag di applicazione in chiusura
  RemPrimaScansione=true;    // prima scansione gestione remota
  strcpy(DriveServer, "S:"); // driver di rete
  abilita_delay_speciale=0;  // disattivazione del delay speciale
  //----------------------------------------------------------
  // Archivio la release del software su file
  //----------------------------------------------------------
  WriteIniRelease();
  //----------------------------------------------------------
  // Recupero il path del progetto
  //----------------------------------------------------------
  PathProject = ExtractFilePath(ParamStr(0));
  PathConfig  = PathProject + "Config\\";
  //----------------------------------------------------------
  // Lettura file identificativo PC
  //----------------------------------------------------------
  if(RemLoadIdentificativoPC("Identifi.dat", &Id, buffer)){
     FormInClose=true;
     Application->MessageBox( buffer, "ERRORE", MB_OK | MB_ICONEXCLAMATION);
     PostQuitMessage(0);
  }
  IdPC=Id; // assegnamento dell'id globale del PC
  //----------------------------------------------------------
  // Disattivazione "Timer" delle finestre principale
  //----------------------------------------------------------
  TimerLampeggio->Enabled = false;    // Timer di aggiornamento sinottico
  TimerCiclo->Enabled     = false;    // Timer principale di gestione RunAutomatico
  TimerRefresh->Enabled   = false;    // Timer di aggiornamento sinottico
  TimerMin->Enabled       = false;    // Timer con cadenza un minuto
  //---------------------------------------------------------
  // Barra di stato
  //---------------------------------------------------------
  SBarImpianto->Panels->Items[ 0]->Text = "";
  SBarImpianto->Panels->Items[ 1]->Text = "TGV";
  SBarImpianto->Panels->Items[ 2]->Text = "PLC";
  SBarImpianto->Panels->Items[ 3]->Text = "I/O";
  SBarImpianto->Panels->Items[ 4]->Text = "SQL";
  SBarImpianto->Panels->Items[ 5]->Text = "msec";
  SBarImpianto->Panels->Items[ 6]->Text = "msec";
  SBarImpianto->Panels->Items[ 7]->Text = "msec";
  SBarImpianto->Panels->Items[ 8]->Text = "Password: 1";
  SBarImpianto->Panels->Items[ 9]->Text = "AUTOMATIC ON";
  SBarImpianto->Panels->Items[10]->Text = "SERVER";
  //----------------------------------------------------------
  // Disattivazione menù per strutture inesistenti
  //----------------------------------------------------------
  if(TotMac<1 ) MenuChiamate->Visible  = false;
  if(TotMag<1 ) MenuMagazzini->Visible = false;
  if(TotPort<1) MenuPorte->Visible     = false;
  if(TotBaie<1) MenuBaie->Visible      = false;
  if(TotBatt<1) MenuCBatt->Visible     = false;
  //----------------------------------------------------------
  // Gestione tipo PC CLIENT/SERVER
  //----------------------------------------------------------
  if(IdPC==1){
     SBarImpianto->Panels->Items[10]->Text = "SERVER";
     MClientServer->Visible        = false;
     MSimulazione->Visible         = true;
     MLogTgv->Visible              = true;
     MCOM1->Visible                = true;
     MCOM2->Visible                = true;
     MCOM3->Visible                = true;    // sever SQL non presente in Seramiksan
     MCOM4->Visible                = false;
     MCOM5->Visible                = false;
     MCOM6->Visible                = true;    // I/O REMOTI
     MCOM7->Visible                = false;
     MCOM8->Visible                = false;
     MCOM9->Visible                = true;
     N2->Visible                   = true;
     N16->Visible                  = true;
     MEditorComunication->Visible  = true;
     MenuConfig->Visible           = true;
  }
  if(IdPC >1){
     sprintf(buffer, "CLIENT%d", IdPC);
     SBarImpianto->Panels->Items[10]->Text = buffer;
     MClientServer->Visible        = true;
     MSimulazione->Visible         = false;
     MLogTgv->Visible              = false;
     N16->Visible                  = false;
     MCOM1->Visible                = false;
     MCOM2->Visible                = false;
     MCOM3->Visible                = true;
     MCOM4->Visible                = false;
     MCOM5->Visible                = false;
     MCOM6->Visible                = false;
     MCOM7->Visible                = false;
     MCOM8->Visible                = false;
     MCOM9->Visible                = false;
     N2->Visible                   = true;
     N16->Visible                  = false;
     MEditorComunication->Visible  = false;
     MenuPorte->Visible            = false;    // Porte    MAI accessibili da CLIENT
     MenuConfig->Visible           = false;    // Config   MAI accessibili da CLIENT
  }

  //----------------------------------------------------------
  // Abilitazione
  //----------------------------------------------------------
  // Disabilitazione funzioni protette
  //----------------------------------------------------------
  if(access("C:\\TEST\\PROTETTO.DAT", 0)){
     // --- funzioni nascoste
     PcAlgo                  = false;       // non è un PC ALGOSISTEMI
     MSfondoGrafico->Visible = false;
     MSimulazione->Visible   = false;
     MTestPercorsi->Visible  = false;
     N15->Visible            = false;
  }

  //---------------------------------------------------------
  // GESTIONE CURVE LASER
  // Visualizzazione della voce di menù che permette il
  // controllo dei raggi
  //---------------------------------------------------------
  N14->Visible=false;
  MZoneMapping->Visible=false;
  MVisualizzaRiflettori->Visible=false;
  #ifdef GESTIONE_CURVE
     N14->Visible=true;
     MVisualizzaRiflettori->Visible=true;
     MZoneMapping->Visible=true;
  #endif;

  //----------------------------------------------------------
  // *** Inizializza tutte le variabili globali ***
  //----------------------------------------------------------
  InizializzaVariabiliGlobali();

  //----------------------------------------------------------
  // Rilettura dei file di inizializzazione progetto
  //----------------------------------------------------------
  ReadIniFormMain();      // aspetto della form
  ReadIniProject();       // impostazioni di progetto
  ReadIni_IMPIANTI();     // impostazioni di progetto specializzate
  AllenB_ReadIniProject();// Impostazioni Allen Bradley
  //----------------------------------------------------------
  // Imposta i messaggi abbinati agli oggetti
  //----------------------------------------------------------
  Messaggi();

  //---------------------------------------------------------
  // Caricamento strutture di config. NODI INTERBLOCCATI
  //---------------------------------------------------------
  err = ReadConfigInterbloccoNodi(buffer);
  if(err){
     Application->MessageBox(buffer, "ERROR", MB_OK | MB_ICONEXCLAMATION);
//   PostQuitMessage(0);
     Application->Terminate();
  }

  //---------------------------------------------------------
  // Caricamento strutture di config. ATTESE TGV
  //---------------------------------------------------------
  err = ReadConfigAtteseTGV(buffer);
  if(err){
     Application->MessageBox(buffer, "ERROR", MB_OK | MB_ICONEXCLAMATION);
//   PostQuitMessage(0);
     Application->Terminate();
  }

  //---------------------------------------------------------
  // Caricamento strutture di config. ATTESE TGV
  //---------------------------------------------------------
  err = ReadConfigPintObbligato(buffer);
  if(err){
     Application->MessageBox(buffer, "ERROR", MB_OK | MB_ICONEXCLAMATION);
//   PostQuitMessage(0);
     Application->Terminate();
  }

  //----------------------------------------------------------
  // *** Inizializza tutte le "Classi di Sistema" ***
  //----------------------------------------------------------
  InizializzaClassi();

  //----------------------------------------------------------
  // Lettura configurazioni di base
  //----------------------------------------------------------
  SettaConfigurazioneImpianto();

  //------------------------------------------------------
  // Registrazione dell'accensione del software
  //------------------------------------------------------
  if(IdPC<2) FileLogOperatore("(TGVSPEC) - Start del software");

  //------------------------------------------------------
  // Durata timer ciclo da IniProject
  //------------------------------------------------------
  TimerCiclo->Interval=MSecIntervalloTimerCiclo;

  //------------------------------------------------------
  // ATTESA PRIMA DI START TIMER
  //------------------------------------------------------
  msec=0;
  GetLocalTime( &Time1 );
  do{
     //-------------------------------------
     // Processo i messaggi di applicazione
     //-------------------------------------
     Application->ProcessMessages();
     //-------------------------------------
     // Attesa max di 1 sec
     //-------------------------------------
     GetLocalTime( &Time2 );
     msec = (int) (Time2.wSecond - Time1.wSecond);
     if( msec<0 ) msec+=60;
     msec = msec * 1000;
     msec = msec + (int) (Time2.wMilliseconds - Time1.wMilliseconds);
  }while(msec<500);
  //------------------------------------------------------
  // Attivazione "Timer" delle finestre principale
  //------------------------------------------------------
  TimerRefresh->Enabled   = true;
  TimerLampeggio->Enabled = true;
  TimerMin->Enabled       = true;            // Timer con cadenza un minuto
  //------------------------------------------------------
  // Attivazione dei timer di ciclo solo a fine attivaz.
  //------------------------------------------------------
  TimerCiclo->Enabled     = true;                // Timer principale di gestione RunAutomatico
}
//---------------------------------------------------------------------------
//-----------------------------------------------------
// Inizializzazione delle Variabili Globali
//-----------------------------------------------------
void TFormMain::InizializzaVariabiliGlobali()
{
  int i, pos;
  AnsiString Buff=PLANT_PLUGIN;


  pos = Buff.Pos(".CPP");
  //-----------------------------------------------------
  // Variabili globali di sistema
  //-----------------------------------------------------
  LastTimeMinutoCiclo=time(NULL);
  memset(&break_Com, true, sizeof(break_Com));
  memset(&errore_comunicazione, 0, sizeof(errore_comunicazione));
  NrOperatore                   = 0;                                // Numero dell'operatore (ssegnato in base alla Password)
  AbilitaSimulazione            = false;                            // Variabile di abilitazione del Ciclo di Simulazione
  TestInternoTecnoFerrari       = false;                            // Flag di abilitazioni TEST_INTERNO_TECNOFERRAI
  Lampeggio                     = false;                            // Variabile flip/flop
  PrimaScansione                = true;                             // Prima scansione software
  IncendioInCorso               = false;                            // Inizializza flag di INCENDIO IN CORSO
  LivelloPassword               = 1;                                // Livello della password impostata
  LivelloMinPassword            = 1;                                // Livello minimo di password consentita sul PC
  LivelloMaxPassword            = 3;                                // Livello minimo di password consentita sul PC
  Automatico                    = true;                             // Abilitazione "Ciclo Automatico"
  BackupInCorso                 = false;                            // Backup in corso
  LogTgvAttivato                = false;                            // Gestione log TGV attivato
  AttivaPassword3Veloce         = true;                             // Flag di attivazione Password speciale
  Minuto                        = false;                            // Flag di minuto trascorso
  ModificaCoordinateNodi        = false;                            // Abilitazione "Modifica coordinate nodi impianto"
  PlantName                     = Buff.SubString(1, pos-1);         // Plant name
  ComunicazioneDirettaPLC       = COMUNICAZIONE_DIRETTA;            // Comunicazione diretta con PLC
  strcpy(DriveServer,   "S:");                                      // Driver di rete per ricerca PC SERVER
  strcpy(LinguaSistema, "ITA");                                     // Gestione messaggeria di sistema
  Pass0 = "0";                                                      // Password a livello 0
  Pass1 = PASS_CLIENTE;                                             // Password a livello 1
  for(i=1; i<11; i++){                                              // Inizializzazione struttura Password
     Pass2[i] = PASS_TECNO;                                         // Password a livello 2
     Pass3[i] = PASS_LEVEL3;                                        // Password a livello 3
  }
  //-----------------------------------------------------
  // Gestione degli Editor aperti
  //-----------------------------------------------------
  EditorBoxAttivo       = 0;                                // Flag di editor BOX aperto
  EditorCBattAttivo     = 0;                                // Flag di editor MAGAZZINO aperto
  EditorMacchineAttivo  = 0;                                // Flag di editor MACCHINE aperto
  EditorMacchineAttivo  = 0;                                // Flag di editor MACCHINE aperto
  EditorBaieAttivo      = 0;                                // Flag di editor BAIE aperto
  //-----------------------------------------------------
  // Inizializzzione DataBaseName
  //-----------------------------------------------------
  NomeSRVSQL = "SQLSERVER2005\\SQLEXPRESS";
  NomeDBase  = "DBTecnoferrari";
  NomeTabA   = "TabellaA";
  DBasePsw   = "";
  ProviderADO= "SQLOLEDB.1";
  DBaseUser  = "Admin";
  //-----------------------------------------------------
  // ABBINA_CODICE_A_STATO_BOX
  // Da definire solo negli impianti in cui c'è una
  // corrispondenza univoca tra CODICE e STATOBOX
  // (possibile solo fino a MAX 8 CODICI)
  //-----------------------------------------------------
  AbbinaCodiceStatoBox=false;
  #ifdef ABBINA_CODICE_A_STATO_BOX
     AbbinaCodiceStatoBox=true;
  #endif;
  //-----------------------------------------------------
  // Gestione posizioni dinamiche dei magazzini
  //-----------------------------------------------------
  MagazziniMultiNodo=false;
  #ifdef ABILITA_MAGAZZINI_MULTI_NODO
     MagazziniMultiNodo=true;
  #endif;
  //-----------------------------------------------------
  // CURVE DI BEZIER
  //-----------------------------------------------------
  CurveDiBezier=CURVE_DI_BEZIER;
  //-----------------------------------------------------
  // Verifica se impianto impostato per doppia forca TGV
  //-----------------------------------------------------
  GestioneDoppiaForcaTGV=GESTIONE_DOPPIA_FORCA;
  //-----------------------------------------------------
  // Gestione posizioni dinamiche dei magazzini
  //-----------------------------------------------------
  GestioneDinamicaMagazzini=GESTIONE_DINAMICA_MAG;
  //-----------------------------------------------------
  // Verifica se impianto impostato per doppia forca TGV
  //-----------------------------------------------------
  GestioneUdc=ATTIVA_GESTIONE_UDC;
  //-----------------------------------------------------
  // SICUREZZA: La gestione ABILITA_MAGAZZINI_MULTI_NODO
  // esclude automaticamente anche la gestione
  // GESTIONE_DINAMICA_MAG
  //-----------------------------------------------------
  if(MagazziniMultiNodo==true) GestioneDinamicaMagazzini=0;
  //-----------------------------------------------------
  // TEST_INTERNO_TECNOFERRARI
  // Gestioni riservate
  //-----------------------------------------------------
  #ifdef TEST_INTERNO_TECNOFERRARI
     TestInternoTecnoFerrari = true;
  #endif;     
  //-----------------------------------------------------
  // Azzeramento delle strutture per la gestione
  // delle condizioni di DEAD LOCK.
  //-----------------------------------------------------
  CountCondizioneDiBlocco=0;
  memset(&AgvBloccati[0],     0, sizeof(AgvBloccati));
  memset(&BloccoInvariato[0], 0, sizeof(BloccoInvariato));
  //--------------------------------------------------------
  // Azzeramneto struttura di visualizzazione riflettori
  // per mappa
  //--------------------------------------------------------
  LaserMappaRiflettoriTgv=0;
  //--------------------------------------------------------
  // Azzeramento del flag di riattivazione mapping
  //--------------------------------------------------------
  memset(&RiattivaMapping, false, sizeof(RiattivaMapping));
  //-----------------------------------------------------
  // Variabili di impostazione progetto
  //-----------------------------------------------------
  CanaleCronoAllarmiTGV         = 6;                                // Canale per lettura allarmi da registrare
  DisattivaDelaySpeciale        = true;                             // Attivazione/Disattiva delay speciale
  ManualeConNodiOccupati        = true;                             // Mantiene i nodi occupati con il TGV in manuale
  AbilitaRiposoFlottante        = false;                            // Riposo flottante di default disattivato
  AbilitaRotazioneSuiRettilinei = true;                             // Abilitazione 180° dopo reset su rettilinei
  AbilitaPuntoIntermedioSuRotaz = false;                            // Forzatura del punto int sui punti di rotazione
  //-----------------------------------------------------
  // Inizializzazione delle variabili di utilità
  //-----------------------------------------------------
  CountMinStatTgv  = 0;                                             // Conteggio per archiviazione statistiche TGV
  CountMinPassword = 0;                                             // Conteggio per scadenza password
  CountCicliLogTgv = 0;                                             // Conteggio dei cicli di scansione per il salvataggi LOGTGV
  CountMinConnSQL  = 0;                                             // Conteggio minuti per tentativi connessione a SQL
}
//---------------------------------------------------------------------------
//-----------------------------------------------------
// Inizializzazione le classi di Sistema
//-----------------------------------------------------
void TFormMain::InizializzaClassi()
{
  int err=0;
  short int i;
  char buffer[201]="";
  int  TotMac  = TOTCHIAMATE;
  int  TotMag  = TOTMAG;
  int  TotBaie = TOTBAIE;
  int  TotCBat = TOTBATT;
  int  TotPort = TOTPORTE;

  //---------------------------------------------
  // Inizializzazione Classe TGV
  //---------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     AGV[i] = new agv(i);
  }
  //---------------------------------------------
  // Inizializzazione Classe NODI
  //---------------------------------------------
  for(i=0; i<=MAXPUNTI; i++){
     N.n[i] = new NOD;
     memset(N.n[i], 0x00, sizeof( NOD ));
  }
  //---------------------------------------------
  // Inizializzazione Classe PROCESS
  //---------------------------------------------
  for(i=0; i<=MAXCHIAMATE; i++){
     P.ch[i] = new CHIAMATE;
     memset(P.ch[i], 0x00, sizeof(CHIAMATE));
  }
  for(i=0; i<=MAXBAIE; i++){
     P.ba[i] = new BAIE;
     memset(P.ba[i], 0x00, sizeof(BAIE));
  }
  for(i=0; i<=MAXBATT; i++){
     P.cb[i] = new CAMBIOBAT;
     memset(P.ba[i], 0x00, sizeof(CAMBIOBAT));
  }
  //---------------------------------------------
  // Semiautomatico
  //---------------------------------------------
  P.IdMissioneSemiAuto=1;          // prima missione;
  P.AbilitaSemiAutoCiclico=false;
  memset(&P.SemiAuto[0], 0, sizeof(P.SemiAuto));

  //---------------------------------------------
  // Inizializzazione Classe MAGAZZINI
  //---------------------------------------------
  for(i=0; i<=MAXMAG; i++){
     M.m[i] = new MAG;
     memset(M.m[i], 0x00, sizeof(MAG));
  }

  //--------------------------------------------------------------------
  // Configurazione connessione
  //--------------------------------------------------------------------
  ADOConnSQL->ConnectionString  = "Provider="+ ProviderADO + "; Password=" + DBasePsw + "; Persist Security Info=True; User ID=" + DBaseUser + "; Initial Catalog=" + NomeDBase + "; Data Source=" + NomeSRVSQL + ";";
  ADOConnSQL->ConnectionTimeout = SecConnectioTimeOutSQL;
  ADOConnSQL->CommandTimeout    = SecComandoTimeOutSQL;

  //----------------------------------------------------------------
  // Lettura del file GRUPPIBAIE.DAT
  //----------------------------------------------------------------
  err = P.load_file_gruppibaie(buffer);
  if( err ){
     ShowMessage(buffer);
     // creazione file azzerato
     P.save_file_gruppibaie(buffer);
  }

  //----------------------------------------------------------------
  // Lettura del file TIPIBOX.DAT
  //----------------------------------------------------------------
  err = M.load_file_tipibox(buffer);
  if( err ) ShowMessage(buffer);

  //--------------------------------------------------------------------
  // Recupero da GRAPHVAR.DAT la nuova struttura Punti[][5]
  //--------------------------------------------------------------------
  ConvLeggiFileGraphPuntiAscii("GRAPHVAR.DAT", buffer);
  ConvLeggiFileGraphPercorsiCurvaAscii("GRAPHVAR.DAT", buffer);
  ConvLeggiFileGraphPercorsiCurvaBezierAscii("GRAPHVAR.DAT", buffer);
  //--------------------------------------------
  // Compila i dati relativi ai nodi
  //--------------------------------------------
  err = N.load_file_nodi( buffer );
  if( err ) ShowMessage(buffer);
  err = N.load_nodo_busy(buffer);
  if( err ) ShowMessage(buffer);
  err = N.load_nodo_grd(buffer);
  if( err ) ShowMessage(buffer);

  //--------------------------------------------
  // Ricalcolo delle distanze dei nodi
  //--------------------------------------------
  N.ricalcolo_distanze_nodi(buffer);

  //--------------------------------------------
  // GESTIONE PRECEDENZE PER TGV IN RETTILINEO
  //--------------------------------------------
  err = N.load_nodi_tgv_lanciati_su_rettilineo(buffer);
  if( err ) ShowMessage(buffer);

  //--------------------------------------------
  // CREA FILE CON STRUTTURE AZZERATE
  // solo per strutture inesisteni nell'impianto
  //--------------------------------------------
  if(TotMac<1){
     err = P.save_file_chiamate(buffer);
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  if(TotBaie<1){
     err = P.save_file_baie(buffer);
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  if(TotMag<1){
     err = M.save_file_mag(buffer);
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  if(TotPort<1){
     memset(&PorteImp, 0, sizeof(PorteImp ));
     err = save_file_porteimp(buffer);
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  if(TotCBat<1){
     err = P.save_file_cbat( buffer );
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }

  // MACCHINE
  err = P.load_file_chiamate( buffer );
  if( err ) ShowMessage(buffer);
  // BAIE
  err = P.load_file_baie( buffer );
  if( err ) ShowMessage(buffer);
  // MAGAZZINI
  err = M.load_file_mag( buffer );
  if( err ) ShowMessage(buffer);
  // CAMBIOBATTERIE
  err = P.load_file_cbat( buffer );
  if( err ) ShowMessage(buffer);

  //---------------------------------------
  // Compila l'array di punti notevoli
  // e dei punti circostanti
  //---------------------------------------
  N.compila_blocchi_manuali_percorso(buffer);
  N.compila_zone_fuori_ingombro_manuali(buffer);
  err = N.compila_punti_notevoli(buffer);
  if( err ) ShowMessage(buffer);
  err = N.compila_nodo_circostante(buffer);
  if( err ) ShowMessage(buffer);


  //--------------------------------------------
  // Richiamo funzioni gestite in GRAPHVAR.CPP
  //--------------------------------------------
  setta_pt_in_ingombro();
  setta_nodi_in_ingombro();
  setta_nodi_in_ingombro_per_porte();
  setta_nodi_contrapposti();
  setta_nodi_minpercorsi();

  //--------------------------------------------------------------------
  // Recupero da GRAPHVAR.DAT la nuova struttura Punti[][5]
  //--------------------------------------------------------------------
  ConvLeggiFileGraphPuntiAscii("GRAPHVAR.DAT", buffer);

}
//---------------------------------------------------------------------------
//-----------------------------------------------------
// Richiesta password di accensione del PC
//-----------------------------------------------------
void TFormMain::RicPasswordAccensione()
{
  bool psw_sconosciuta;

  //-----------------------------------
  // SERVER
  // Password di livello minimo = "1"
  //-----------------------------------
  if(IdPC>1){
     //-------------------------------------------------
     // Assegnazione livello minimo di password
     // consentita sul PC
     //-------------------------------------------------
     NrOperatore        = 0;
     LivelloMinPassword = 0;
     psw_sconosciuta    = true;
     //-------------------------------------------------
     // Apertura della form
     //-------------------------------------------------
     FormPassword->ShowModal();
     //-------------------------------------------------
     // Se è stato premuto il pulsante "OK" verifico
     // la validità della passoword
     //-------------------------------------------------
     if(FormPassword->ModalResult==mrOk){
        //----------------------------------------------
        // Attivazione Password al LIVELLO 0
        //----------------------------------------------
        if(FormPassword->Password==Pass0){
           NrOperatore        = 0;
           LivelloMinPassword = 0;
           psw_sconosciuta    = false;
        }
        //----------------------------------------------
        // Attivazione Password al LIVELLO 1
        //----------------------------------------------
        if(FormPassword->Password==Pass1){
           NrOperatore        = 0;
           LivelloMinPassword = 1;
           psw_sconosciuta    = false;
        }
        //----------------------------------------------
        // La password inserita non è corretta
        //----------------------------------------------
        if(psw_sconosciuta==true){
           Application->MessageBox(MESS[105], MESS[100], MB_OK|MB_ICONEXCLAMATION );
        }
     }
  }
}
//---------------------------------------------------------------------------
//-----------------------------------------------------
// Gestione della messaggeria di sistema
//-----------------------------------------------------
void TFormMain::StatoPassword()
{
  if(LivelloPassword<3) ModificaCoordinateNodi=false;
  //-----------------------------------
  // Disattivazione gestione SQL
  //-----------------------------------
  if(GestioneUdc==0) MCOM3->Enabled=false;
  //-----------------------------------
  // Menù disabilitati
  //-----------------------------------
  MenuSistema->Enabled   = true;
  MenuTgv->Enabled       = true;
  MenuNodi->Enabled      = true;
  MenuChiamate->Enabled  = true;
  MenuBaie->Enabled      = true;
  MenuMagazzini->Enabled = true;
  MenuPorte->Enabled     = true;
  MenuConfig->Enabled    = true;
  if(LivelloPassword<1){
     MenuSistema->Enabled   = false;
     MenuTgv->Enabled       = false;
     MenuNodi->Enabled      = false;
     MenuChiamate->Enabled  = false;
     MenuBaie->Enabled      = false;
     MenuMagazzini->Enabled = false;
     MenuPorte->Enabled     = false;
     MenuConfig->Enabled    = false;
  }
}
//---------------------------------------------------------------------------
// ------------------------------------------------
//         load_GRUPPI()
// ------------------------------------------------
int load_GRUPPI(char *filename)
/****************/
{
  FILE *ptrfile;
  char buffer[111];
  int   i=0;
  int   err;

  memset(&DESCGRUPPI[0],  0x00, sizeof(DESCGRUPPI));
  ptrfile = fopen(filename, "rb");
  if (ptrfile==0) return 1;
  do{
     err = Read_Line(ptrfile, buffer, 101);
     Copy( buffer, 1, 100, &DESCGRUPPI[i][0] );
     i++;
  }while(err && i< MAXGRUPPIIMP);
  fclose(ptrfile);
  return 0;
}
//---------------------------------------------------------------------------
// ------------------------------------------------
//         load_TIPOBOX()
// ------------------------------------------------
int load_TIPOBOX(char *filename)
/****************/
{
  FILE *ptrfile;
  char buffer[111];
  int   i=0;
  int   err;

  memset(&DESCTIPOBOX[0],  0x00, sizeof(DESCTIPOBOX));
  ptrfile = fopen(filename, "rb");
  if (ptrfile==0) return 1;
  do{
     err = Read_Line(ptrfile, buffer, 100);
     Copy( buffer, 1, 20, &DESCTIPOBOX[i][0] );
     i++;
  }while(err && i< MAXTIPOBOXIMP);
  fclose(ptrfile);
  return 0;
}
//---------------------------------------------------------------------------
// ------------------------------------------------
//         load_ALLMAC()
// ------------------------------------------------
int load_ALLMAC(char *filename)
/****************/
{
  int   i=0;
  int   err;
  char buffer[111];
  FILE *ptrfile;

  memset(&DESCALLMAC[0],  0x00, sizeof(DESCALLMAC));
  ptrfile = fopen(filename, "rb");
  if (ptrfile==0) return 1;
  do{
     err = Read_Line(ptrfile, buffer, 101);
     Copy( buffer, 1, 100, &DESCALLMAC[i][0] );
     i++;
  }while(err && i< MAX_ALLARMI_MAC);
  fclose(ptrfile);
  return 0;
}
//---------------------------------------------------------------------------
// ------------------------------------------------
//         load_ALLCBAT()
// ------------------------------------------------
int load_ALLCBAT(char *filename)
/****************/
{
  int  i=0;
  int  err;
  char buffer[111];
  FILE *ptrfile;

  memset(&DESCALLCBAT[0],  0x00, sizeof(DESCALLCBAT));
  ptrfile = fopen(filename, "rb");
  if (ptrfile==0) return 1;
  do{
     err = Read_Line(ptrfile, buffer, 101);
     Copy( buffer, 1, 100, &DESCALLCBAT[i][0] );
     i++;
  }while(err && i< MAX_ALLARMI_CBAT);
  fclose(ptrfile);
  return 0;
}
//-----------------------------------------------------
// Gestione della messaggeria di sistema
//-----------------------------------------------------
void TFormMain::Messaggi()
{
  int err;
  char buffer[101];
  char Titolo[201] ="";
  char file_all[101]="";
  char file_mess[101]="";
  char file_gruppi[101]="";
  char file_tipobox[101]="";

  //----------------------------------------------------------
  // Inizializzazione Messaggeria
  //----------------------------------------------------------
  strcpy(file_mess,    "MESSAGG.");  strcat(file_mess,    LinguaSistema);
  strcpy(file_all,     "ALLARM.");   strcat(file_all,     LinguaSistema);
  strcpy(file_gruppi,  "GRUPPI.");   strcat(file_gruppi,  LinguaSistema);
  strcpy(file_tipobox, "TIPOBOX.");  strcat(file_tipobox, LinguaSistema);

  //----------------------------------------------------------
  // MESS[]
  //----------------------------------------------------------
  err = load_MESSAGG(file_mess);
  if(err ){
     sprintf(buffer, "File di messaggeria inesistente: %s", file_mess);
     ShowMessage(buffer);
     Application->Terminate();
  }

  //----------------------------------------------------------
  // ALLARM[]
  //----------------------------------------------------------
  err = load_ALLARM(file_all);
  if(err ){
     sprintf(buffer, "File di allarmi inesistente: %s", file_all);
     ShowMessage(buffer);
     Application->Terminate();
  }

  //----------------------------------------------------------
  // DESCGRUPPI[]
  //----------------------------------------------------------
  err = load_GRUPPI(file_gruppi);
  if(err ){
     sprintf(buffer, "File di gruppi inesistente: %s", file_gruppi);
     ShowMessage(buffer);
     Application->Terminate();
  }

  //----------------------------------------------------------
  // DESCTIPOBOX[]
  //----------------------------------------------------------
  err = load_TIPOBOX(file_tipobox);
  if(err ){
     sprintf(buffer, "File di tipobox inesistente: %s", file_tipobox);
     ShowMessage(buffer);
     Application->Terminate();
  }
  //------------------------------------------------------------
  // Recupero dell'identificativo del PC
  //------------------------------------------------------------
  sprintf(Titolo, MESS[  1], AGGIORNAMENTO, VERSIONE_IMPIANTO);
  FormMain->Caption = "(" + IntToStr(COMM_ALGOSISTEMI) + ") - " + Titolo;

  // --- Menu Principale ---
  MenuSistema->Caption        = MESS[20];
  MenuTgv->Caption            = MESS[21];
  MenuNodi->Caption           = MESS[22];
  MenuChiamate->Caption       = MESS[23];
  MenuBaie->Caption           = MESS[28];
  MenuMagazzini->Caption      = MESS[24];
  MenuPorte->Caption          = MESS[26];
  MenuConfig->Caption         = MESS[27];

  // ---  Menu Sistema  ---
  MMessaggeria->Caption       = MESS[231];
  MPassword->Caption          = MESS[101];
  MComunicazioni->Caption     = MESS[232];
  MBackup->Caption            = MESS[233];
  MSfondoGrafico->Caption     = MESS[234];
  MSimulazione->Caption       = MESS[235];
  MZoneImpianto->Caption      = MESS[236];
  MLogTgv->Caption            = MESS[237];
  MTestPercorsiTecno->Caption = MESS[238];
  MTestPlc1->Caption          = MESS[239];
  MTestPlc2->Caption          = MESS[240];
  MErroriDiSistema->Caption   = MESS[241];
  MEsci->Caption              = MESS[171];

  // ---  Menu Tgv  ---
  MEditorTgv->Caption         = MESS[250];
  MPercorsoTgv->Caption       = MESS[251];
  MProgrammaTgv->Caption      = MESS[252];
  MProgrammaPlcTgv->Caption   = MESS[253];
  MAllarmiTgv->Caption        = MESS[254];
  MStoricoMissioni->Caption   = MESS[255];
  MStoricoAllarmi->Caption    = MESS[256];
  MStatistiche->Caption       = MESS[257];
  MResetMissione->Caption     = MESS[258];

  // ---  Menu Nodi  ---
  MCreaArchivioNodi->Caption       = MESS[270];
  MEditorNodi->Caption             = MESS[271];
  MLiberaNodi->Caption             = MESS[272];
  MRicalcoloDistanzeNodi->Caption  = MESS[273];
  MVisualizzaRiflettori->Caption   = MESS[269];
  MModificaCoordinateNodi->Caption = MESS[274];

  // ---  Menu Macchine  ---
  MCreaArchivioMacchine->Caption   = MESS[280];
  MEditorMacchine->Caption         = MESS[281];
  MStoricoChiamate->Caption        = MESS[282];

  // ---  Menu Baie  ---
  MCreaArchivioBaie->Caption       = MESS[326];
  MEditorBaie->Caption             = MESS[327];
  MStatoBaie->Caption              = MESS[328];

  // ---  Menu Magazzini  ---
  MCreaArchivioMagazzini->Caption  = MESS[276];
  MEditorMagazzini->Caption        = MESS[277];
  MStatoMagazzini->Caption         = MESS[278];

  // ---  Menu Porte  ---
  MCreaArchiviopPorte->Caption     = MESS[285];
  MEditorPorte->Caption            = MESS[286];
  MConsensi->Caption               = MESS[287];
  // ---  Menu Config  ---
  MConfigTipiBox->Caption          = MESS[731];
  MConfigGrpBa->Caption            = MESS[715];
}
//---------------------------------------------------------------------------
//-------------------------------
//   WriteIniRelease()
//-------------------------------
// Scrittura del file *.ini di sistema
//
void TFormMain::WriteIniRelease()
{
  TIniFile *IniFile;
  AnsiString FPath;
  AnsiString FIniFileName;
  char buffer[11]="";

  FPath        = ExtractFilePath(ParamStr(0));
  FIniFileName = FPath + "Release.ini";
  IniFile      = new TIniFile(FIniFileName);
  //-------------------------------------------------------
  // Scrittura su file della release del software
  //-------------------------------------------------------
  sprintf(buffer, "%02d.%02d", AGGIORNAMENTO, VERSIONE_IMPIANTO);
  IniFile->WriteString("TGVSPEC", "Release", buffer);
  IniFile->WriteInteger("TGVSPEC", "CommAlgo", COMM_ALGOSISTEMI);

  delete IniFile;
}
//---------------------------------------------------------------------------
//-------------------------------
//   ReadIniFormMain()
//-------------------------------
// Lettura del file *.ini di sistema
//
void TFormMain::ReadIniFormMain()
{
  TIniFile   *IniFile;
  AnsiString FIniFileName;
  int Stato;

  FIniFileName = PathProject + "IniForm.ini";
  IniFile      = new TIniFile(FIniFileName);
  //-------------------------------------------------------
  // Posizionamento della Form all'interno dello schermo
  //-------------------------------------------------------
  FormMain->Top  = IniFile->ReadInteger("Window", "Top",   FormMain->Top);
  FormMain->Left = IniFile->ReadInteger("Window", "Left",  FormMain->Left);
  Stato          = IniFile->ReadInteger("Window", "State", FormMain->WindowState);
  switch(Stato){
     case  0: FormMain->WindowState = wsNormal; break;
     case  1: FormMain->WindowState = wsMinimized; break;
     case  2: FormMain->WindowState = wsMaximized; break;
     default: FormMain->WindowState = wsNormal; break;
  }
  //-------------------------------------------------------
  // Posizionamento del layout all'interno della FormMain
  //-------------------------------------------------------
  offset[1][0] = IniFile->ReadInteger("Layout", "OffSetX",    OffSO1 );
  offset[1][1] = IniFile->ReadInteger("Layout", "OffSetY",    OffSV1 );
  Rid          = IniFile->ReadFloat(  "Layout", "RidAll",     Rid    );
  ModernHint   = IniFile->ReadBool(   "Layout", "ModernHint", false  );
  //-------------------------------------------------------
  // Gestione visualizzazione
  //-------------------------------------------------------
  delete IniFile;
}
//---------------------------------------------------------------------------
//-------------------------------
//   WriteIniFormMain()
//-------------------------------
// Scrittura del file *.ini di sistema
//
void TFormMain::WriteIniFormMain()
{
  TIniFile *IniFile;
  AnsiString FIniFileName;

  FIniFileName = PathProject + "IniForm.ini";
  IniFile      = new TIniFile(FIniFileName);
  //-------------------------------------------------------
  // Posizionamento della Form all'interno dello schermo
  //-------------------------------------------------------
  IniFile->WriteInteger("Window", "Top",    FormMain->Top);
  IniFile->WriteInteger("Window", "Left",   FormMain->Left);
  IniFile->WriteInteger("Window", "Width",  FormMain->Width);
  IniFile->WriteInteger("Window", "Height", FormMain->Height);
  IniFile->WriteInteger("Window", "State",  FormMain->WindowState);
  //-------------------------------------------------------
  // Posizionamento del layout all'interno della FormMain
  //-------------------------------------------------------
  IniFile->WriteInteger("Layout", "OffSetX",    offset[1][0]);
  IniFile->WriteInteger("Layout", "OffSetY",    offset[1][1]);
  IniFile->WriteFloat(  "Layout", "RidAll",     Rid);
  IniFile->WriteBool(   "Layout", "ModernHint", ModernHint);
  delete IniFile;
}
//---------------------------------------------------------------------------
// ------------------------
//   LoadLogTgv()
// ------------------------
// Lettura della struttura dati con la situazione dei TGV
// file binario LOGTGV.DAT
//
int TFormMain::LoadLogTgv(short int Numero, long int *TimeRecord, char *all_mess)
/**********************************/
{
  int  err=0;
  int  len;
  int  handle;
  long curpos;
  short int i, j, punto;
  long int plant_timezone;
  char filename[81]="LOGTGV.DAT";
  unsigned long length;
  struct LOGTGV LogTgv[MAXAGV];

  plant_timezone=0;
  strcpy( all_mess, "OK" );
  memset(&LogTgv, 0, sizeof(LogTgv));
  //-------------------------------------------------
  // Controllo se il File più essere Letto aprendolo
  // in modalit… di Condivisione.
  //-------------------------------------------------
  handle = sopen( filename, O_RDONLY | O_BINARY, SH_DENYNO, S_IREAD );
  if( handle<0 ){
   //close( handle );
     sprintf(all_mess, MESS[112], filename);
     return 1;
  }
  //---------------------------------------
  // Lettura file di FUSO ORARIO LOCALE
  //---------------------------------------
  if(RemLoadTimeZone(&plant_timezone, all_mess)) return 1;
  //--------------------------------------
  // Controllo lunghezza file
  //--------------------------------------
  length=0;
  if(handle >-1) length = filelength(handle);
  if(length < ((long)(Numero)* (sizeof(LOGTGV)*MAXAGV))){
     sprintf(all_mess, MESS[113], filename);
     close(handle);
     return 1;
  }

  //--------------------------------------
  // Calcolo dell'OFFSET e posizionamento
  //--------------------------------------
  curpos = (long ) Numero * (sizeof(LOGTGV)*MAXAGV);
  if( lseek( handle, curpos, SEEK_SET )<1){  // pos. il puntatore a inzio record
     sprintf(all_mess, MESS[110], Numero, filename);
     close( handle );
     return 1;
  }
  //--------------------------------------
  // Scrittura Record LOGTGV.DAT
  //--------------------------------------
  len = read( handle, &LogTgv, sizeof(LogTgv));
  if(len != sizeof(LogTgv)){
     err=1;
     sprintf(all_mess, MESS[111], Numero, filename);
  }
  close(handle);
  memset(&N.nodo_busy[0], 0, sizeof(N.nodo_busy));
  //---------------------------------------
  // Aggiornamento della struttura dei TGV
  //---------------------------------------
  *TimeRecord = LogTgv[0].Time - plant_timezone + _timezone;
  for(i=1; i<=MAXAGV; i++){
     AGV[i]->stato.pos                   = LogTgv[i-1].Pos;
     AGV[i]->stato.offset_da_nodo        = LogTgv[i-1].OffsetDaNodo;
     if(AGV[i]->mission.GestioneLaser==true) AGV[i]->stato.rot_laser    = LogTgv[i-1].Rot;
     else                                    AGV[i]->stato.rot_calamite = LogTgv[i-1].Rot;
     AGV[i]->stato.g_laser               = LogTgv[i-1].GLaser;
     AGV[i]->stato.start                 = LogTgv[i-1].Start;
     AGV[i]->mission.pstart              = LogTgv[i-1].PStart;
     AGV[i]->mission.pintstart           = LogTgv[i-1].PIntStart;
     AGV[i]->mission.pend                = LogTgv[i-1].PEnd;
     AGV[i]->mission.pintend             = LogTgv[i-1].PIntEnd;
     AGV[i]->mission.ResetAlVolo         = LogTgv[i-1].ResetAlVolo;
     AGV[i]->mission.PIntForzato         = LogTgv[i-1].PIntForzato;
     AGV[i]->mission.Chiamata            = LogTgv[i-1].Chiamata;
     AGV[i]->mission.NodIngMag           = LogTgv[i-1].NodIngMag;
     AGV[i]->mission.Typ                 = LogTgv[i-1].Typ;
     AGV[i]->mission.TypPath             = LogTgv[i-1].TypPath;  
     AGV[i]->mission.TypPresa            = LogTgv[i-1].TypPresa;
     AGV[i]->mission.KmPercorsi          = LogTgv[i-1].KmPercorsi;
     memcpy(&AGV[i]->mission.punto[0], &LogTgv[i-1].punto[0], sizeof(AGV[i]->mission.punto));
     memcpy(&AGV[i]->mission.punto_prenotato[0], &LogTgv[i-1].punto_prenotato[0], sizeof(AGV[i]->mission.punto_prenotato));
     AGV[i]->mission.NoRiposoAuto        = LogTgv[i-1].NoRiposoAuto;
     AGV[i]->mission.SemiAutomatico      = LogTgv[i-1].SemiAutomatico;
     AGV[i]->mission.Esclusione          = LogTgv[i-1].Esclusione;
     AGV[i]->mission.EsclPLC             = LogTgv[i-1].EsclPLC;
     AGV[i]->mission.StopManuale         = LogTgv[i-1].StopManuale;
     AGV[i]->mission.TGVInCambioMissione = LogTgv[i-1].TGVInCambioMissione;
     AGV[i]->mission.Provenienza         = LogTgv[i-1].Provenienza;
     AGV[i]->mission.MacInIngombro       = LogTgv[i-1].MacInIngombro;
     AGV[i]->mission.BaiaInIngombro      = LogTgv[i-1].BaiaInIngombro;
     memcpy(&AGV[i]->mission.PorInIngombro[0], &LogTgv[i-1].PorInIngombro[0], sizeof(AGV[i]->mission.PorInIngombro));
     //-------------------------------------------------
     // DATI CONTENITORE CARICATO
     //-------------------------------------------------
     AGV[i]->mission.PresCEST_A      = LogTgv[i-1].PresCEST_A;
     AGV[i]->mission.PresCEST_B      = LogTgv[i-1].PresCEST_B;
     AGV[i]->mission.BoxA.StatoLav   = LogTgv[i-1].PresCEST_B;
     AGV[i]->mission.BoxB.StatoLav   = LogTgv[i-1].PresCEST_B;
     memcpy(&AGV[i]->mission.BoxA.Presa[0][0], &LogTgv[i-1].PresaA[0][0], sizeof(AGV[i]->mission.BoxA.Presa));
     memcpy(&AGV[i]->mission.BoxB.Presa[0][0], &LogTgv[i-1].PresaB[0][0], sizeof(AGV[i]->mission.BoxB.Presa));
     AGV[i]->mission.BoxA.Codice     = LogTgv[i-1].CodA;
     AGV[i]->mission.BoxB.Codice     = LogTgv[i-1].CodB;
     AGV[i]->mission.BoxA.Lotto      = LogTgv[i-1].LottoA;
     AGV[i]->mission.BoxB.Lotto      = LogTgv[i-1].LottoB;
     AGV[i]->mission.BoxA.Basso      = LogTgv[i-1].BassoA;
     AGV[i]->mission.BoxB.Basso      = LogTgv[i-1].BassoB;
     AGV[i]->mission.BoxA.Sovrap     = LogTgv[i-1].SovrapA;
     AGV[i]->mission.BoxB.Sovrap     = LogTgv[i-1].SovrapB;
     AGV[i]->mission.BoxA.CarWash    = LogTgv[i-1].CarWashA;
     AGV[i]->mission.BoxB.CarWash    = LogTgv[i-1].CarWashB;
     AGV[i]->mission.BoxA.GruppoDest = LogTgv[i-1].GrpDestA;
     AGV[i]->mission.BoxB.GruppoDest = LogTgv[i-1].GrpDestB;
     //-------------------------------------------------
     // STATO NAVETTA
     //-------------------------------------------------
     AGV[i]->mission.VoltBatteria   = LogTgv[i-1].VoltBatteria;
     AGV[i]->mission.PercCaricaBatt = LogTgv[i-1].PercCaricaBatt;
     AGV[i]->stato.s.val            = LogTgv[i-1].Comandi;
     AGV[i]->allarme_interno        = LogTgv[i-1].allarme_interno;
     for(j=0; j<MAXLRALLARMI; j++){
        AGV[i]->allarme_esterno[j]  = LogTgv[i-1].lr_allarmi[j];
     }
     //---------------------------------------
     // Aggiorno i nodi occupati
     //---------------------------------------
     for(j=0; j<MAXPERCORSI; j++){
        if(LogTgv[i-1].punto[j]==0) continue;
        punto = LogTgv[i-1].punto[j];
        if(LogTgv[i-1].NodiOccupati[j]) N.nodo_busy[punto]=i;
     }
  }
  // --- salvataggio nodi occupati
  err=N.save_nodo_busy(all_mess);
  return err;
}
//---------------------------------------------------------------------------
// -------------------------------
//    AggiornaLogTgv()
// -------------------------------
// Scrittua di una stringa in un file ASCII contenete la
// posizione attuale dei TGV.
//
int TFormMain::AggiornaLogTgv()
/********************************************************/
{
  char all_mess[101]="";
  int  i, j, err, punto;
  long int TimeAttuale, length;
  int  handle, handle1, len, len1;
  char filename[201]="LOGTGV.DAT";
  char filename1[201]="LOGTGV.BAK";
  struct LOGTGV Dati[MAXAGV];
  struct LOGTGV LogTgv[MAXAGV];

  err=0;
  length=0;
  TimeAttuale = time(NULL);
  memset(&Dati,   0, sizeof(Dati));
  memset(&LogTgv, 0, sizeof(LogTgv));
  //-------------------------------------------------
  // Aggiornamento della struttura con la SITUAZIONE
  // del TGV.
  //-------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     LogTgv[i-1].Time                 = TimeAttuale;
     LogTgv[i-1].Pos                  = AGV[i]->stato.pos;
     LogTgv[i-1].OffsetDaNodo         = AGV[i]->stato.offset_da_nodo;
     if(AGV[i]->mission.GestioneLaser==true) LogTgv[i-1].Rot = AGV[i]->stato.rot_laser;
     else                                    LogTgv[i-1].Rot = AGV[i]->stato.rot_calamite;
     LogTgv[i-1].GLaser               = AGV[i]->stato.g_laser;
     LogTgv[i-1].Start                = AGV[i]->stato.start;
     LogTgv[i-1].PStart               = AGV[i]->mission.pstart;
     LogTgv[i-1].PIntStart            = AGV[i]->mission.pintstart;
     LogTgv[i-1].PEnd                 = AGV[i]->mission.pend;
     LogTgv[i-1].PIntEnd              = AGV[i]->mission.pintend;
     LogTgv[i-1].ResetAlVolo          = AGV[i]->mission.ResetAlVolo;
     LogTgv[i-1].PIntForzato          = AGV[i]->mission.PIntForzato;
     LogTgv[i-1].Chiamata             = AGV[i]->mission.Chiamata;
     LogTgv[i-1].NodIngMag            = AGV[i]->mission.NodIngMag;   
     LogTgv[i-1].Typ                  = AGV[i]->mission.Typ;
     LogTgv[i-1].TypPath              = AGV[i]->mission.TypPath;
     LogTgv[i-1].TypPresa             = AGV[i]->mission.TypPresa;
     LogTgv[i-1].KmPercorsi           = AGV[i]->mission.KmPercorsi;
     memcpy(&LogTgv[i-1].punto[0], &AGV[i]->mission.punto[0], sizeof(AGV[i]->mission.punto));
     memcpy(&LogTgv[i-1].punto_prenotato[0], &AGV[i]->mission.punto_prenotato[0], sizeof(AGV[i]->mission.punto_prenotato));
     LogTgv[i-1].NoRiposoAuto         = AGV[i]->mission.NoRiposoAuto;
     LogTgv[i-1].SemiAutomatico       = AGV[i]->mission.SemiAutomatico;
     LogTgv[i-1].Esclusione           = AGV[i]->mission.Esclusione;
     LogTgv[i-1].EsclPLC              = AGV[i]->mission.EsclPLC;
     LogTgv[i-1].StopManuale          = AGV[i]->mission.StopManuale;
     LogTgv[i-1].TGVInCambioMissione  = AGV[i]->mission.TGVInCambioMissione;
     LogTgv[i-1].Provenienza          = AGV[i]->mission.Provenienza;
     LogTgv[i-1].MacInIngombro        = AGV[i]->mission.MacInIngombro;
     LogTgv[i-1].BaiaInIngombro       = AGV[i]->mission.BaiaInIngombro;
     memcpy(&LogTgv[i-1].PorInIngombro[0], &AGV[i]->mission.PorInIngombro[0], sizeof(LogTgv[i-1].PorInIngombro));
     LogTgv[i-1].VoltBatteria         = AGV[i]->mission.VoltBatteria;
     LogTgv[i-1].PercCaricaBatt       = AGV[i]->mission.PercCaricaBatt;
     //-------------------------------------------------
     // DATI CONTENITORE CARICATO
     //-------------------------------------------------
     LogTgv[i-1].PresCEST_A   = AGV[i]->mission.PresCEST_A;
     LogTgv[i-1].PresCEST_B   = AGV[i]->mission.PresCEST_B;
     memcpy(&LogTgv[i-1].PresaA[0][0], &AGV[i]->mission.BoxA.Presa[0][0], sizeof(LogTgv[i-1].PresaA));
     memcpy(&LogTgv[i-1].PresaB[0][0], &AGV[i]->mission.BoxB.Presa[0][0], sizeof(LogTgv[i-1].PresaB));
     LogTgv[i-1].CodA         = AGV[i]->mission.BoxA.Codice;
     LogTgv[i-1].CodB         = AGV[i]->mission.BoxB.Codice;
     LogTgv[i-1].LottoA       = AGV[i]->mission.BoxA.Lotto;
     LogTgv[i-1].LottoB       = AGV[i]->mission.BoxB.Lotto;
     LogTgv[i-1].BassoA       = AGV[i]->mission.BoxA.Basso;
     LogTgv[i-1].BassoB       = AGV[i]->mission.BoxB.Basso;
     LogTgv[i-1].SovrapA      = AGV[i]->mission.BoxA.Sovrap;
     LogTgv[i-1].SovrapB      = AGV[i]->mission.BoxB.Sovrap;
     LogTgv[i-1].CarWashA     = AGV[i]->mission.BoxA.CarWash;
     LogTgv[i-1].CarWashB     = AGV[i]->mission.BoxB.CarWash;
     LogTgv[i-1].GrpDestB     = AGV[i]->mission.BoxB.GruppoDest;
     LogTgv[i-1].GrpDestA     = AGV[i]->mission.BoxA.GruppoDest;
     //-------------------------------------------------
     // Stato TGV
     //-------------------------------------------------
     LogTgv[i-1].Comandi         = AGV[i]->stato.s.val;
     LogTgv[i-1].allarme_interno = AGV[i]->allarme_interno;
     for(j=0; j<MAXLRALLARMI; j++){
        LogTgv[i-1].lr_allarmi[j] = AGV[i]->allarme_esterno[j];
     }
     //-------------------------------------------------
     // Aggiornamento dello stato dei NODI OCCUPATI
     //-------------------------------------------------
     memset(&LogTgv[i-1].NodiOccupati[0], false, sizeof(LogTgv[i-1].NodiOccupati));
     for(j=0; j<MAXPERCORSI; j++){
        if(AGV[i]->mission.punto[j]==0) continue;
        punto = AGV[i]->mission.punto[j];
        if(N.nodo_busy[punto]!=i) continue;
        LogTgv[i-1].NodiOccupati[j] = true;
     }
  }
  //-----------------------------------------------------------
  // APERTURA FILE IN LETTURA
  //-----------------------------------------------------------
  handle = sopen( filename, O_RDONLY | O_BINARY, SH_DENYNO, S_IREAD );
  //-----------------------------------------------------------
  // Controllo lunghezza file
  //-----------------------------------------------------------
  length=0;
  if(handle>-1) length = filelength(handle);
  //-----------------------------------------------------------
  // Verifico se l'archiviazione ha raggiunto le 4 ore
  // (con salvataggio circa ogni 5 sec.)
  //-----------------------------------------------------------
  if(length>(signed)(3000*sizeof(Dati))){
     //----------------------------------------------------------
     // Ogni 4 ore di archiviazione cancello la prima ora in modo
     // da avere i solo i dati di 3 ore.
     //----------------------------------------------------------
     handle1 = sopen( filename1, O_WRONLY | O_BINARY | O_APPEND | O_CREAT, SH_DENYNO, S_IWRITE );
     if( handle1<0 ){
        sprintf(all_mess, MESS[113], filename);
        close( handle );
      //close( handle1 );
        return 1;
     }
     //----------------------------------------------------------
     // Cancellazione dei record della prima mezz'ora
     //----------------------------------------------------------
     for(i=1; i<350; i++){
        len = read(handle, &Dati, sizeof(Dati));
        if(len!=(signed)(sizeof(Dati))){
           sprintf(all_mess, MESS[113], filename);
           close( handle );
           close( handle1 );
           return 1;
        }
     }
     //----------------------------------------------------------
     // Lettura dal primo file e scrittura su quello di appoggio
     //----------------------------------------------------------
     err=0;
     while(!err){
        len = read(handle, &Dati, sizeof(Dati));
        if(len==(signed)(sizeof(Dati))){
           len1 = write(handle1, &Dati, sizeof(Dati));
        }
        //----------------------------------------------------
        // In caso di errore in scrittura o lettura --> Skip!
        //----------------------------------------------------
        if(len!=(signed)(sizeof(Dati)) || len1!=(signed)(sizeof(Dati))) err=1;
     };
     close( handle  );
     close( handle1 );
     //----------------------------------------------------------
     // Rinomino i file di appoggio
     //----------------------------------------------------------
     unlink(filename);
     err = rename(filename1, filename);
     if(err){sprintf(all_mess, MESS[113], filename); return 1;}
  }
  else close( handle );
  //-----------------------------------------------------------
  // APERTURA FILE IN SCRITTURA
  //-----------------------------------------------------------
  handle = sopen( filename, O_WRONLY | O_BINARY | O_APPEND | O_CREAT, SH_DENYNO, S_IWRITE );
  if( handle<0 ){
     sprintf(all_mess, MESS[113], filename);
   //close( handle );
     return 1;
  }
  //-----------------------------------------------------------
  // Scrittura del nuovo record
  //-----------------------------------------------------------
  len = write(handle, &LogTgv, sizeof(LogTgv));
  close( handle );
  if(len != sizeof(LogTgv)) err=1;
  if(err) sprintf(all_mess, MESS[113], filename);
  return err;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BtnLayoutClick(TObject *Sender)
{
  if(FormLayout->WindowState==wsMaximized){
     FormLayout->WindowState = wsNormal;
     FormLayout->Left   = 0;
     FormLayout->Top    = 0;
     FormLayout->Height = FormMain->ClientHeight-83;
     FormLayout->Width  = FormMain->ClientWidth-268;
     FormStato->Left    = FormLayout->Width;
     FormStato->Top     = FormLayout->Top;
     FormStato->Height  = FormLayout->Height;
  }
  else{
     FormLayout->WindowState = wsMaximized;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BtnNodoClick(TObject *Sender)
{
  AnsiString InputStringa;
  char Chiave[5]="";
  int  Punto, CentroX, CentroY;

  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  InputStringa = InputBox(MESS[430], MESS[431], "");
  if(InputStringa.IsEmpty() == true) return;
  strcpy(Chiave, InputStringa.c_str());
  Punto = atoi(Chiave);
  //----------------------------------------------
  // Controllo della validità del valore inserito
  //----------------------------------------------
  if(Punto<1 || Punto>MAXPUNTI){
     Application->MessageBox(MESS[372], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }
  //-----------------------------------------------
  // Posizionamento su nodo richiesto
  //-----------------------------------------------
  CentroX = (int)(FormLayout->SfondoW/2);
  CentroY = (int)(FormLayout->SfondoH/2);
  //----------------------------------------------------
  // Se è stato punto un NODO centro il layout su quel
  // nodo
  //----------------------------------------------------
//Rid = RID_NUMERI;
//if(RidAll>RID_NUMERI) Rid = RidAll;
  if(Rid<RID_NUMERI) Rid = RID_NUMERI;
  offset[1][0] = Punti[Punto][0]-((CentroX*Rap)/Rid);
  offset[1][1] = Punti[Punto][1]-((CentroY*Rap)/Rid);
  FormLayout->PosizionamentoSuNodo=(short int)(Punto);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BtnPresentazioneClick(TObject *Sender)
{
  FormVideo->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BtnSemiautomaticoClick(TObject *Sender)
{
  if(LivelloPassword<1                   ) return;

  FormSemiAuto->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BtnPorteClick(TObject *Sender)
{
  AnsiString InputStringa;
  short int  NumPorta=0;
  char Chiave[5]="";

  //-----------------------------------------------
  // Impostazioni possibili solo dal SERVER
  //-----------------------------------------------
  if(IdPC>1){
     Application->MessageBox(MESS[95], MESS[99], MB_OK | MB_ICONINFORMATION);
     return;
  }
  //-----------------------------------------------
  // Livello password non ammesso
  //-----------------------------------------------
  if(LivelloPassword<3) return;
  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  InputStringa = InputBox(MESS[26], MESS[381], Chiave);
  if(InputStringa.IsEmpty() == true) return;
  strcpy(Chiave, InputStringa.c_str());
  NumPorta = (short int) atoi(Chiave);               // Recupero del Numero del TGV
  //----------------------------------------------
  // Controllo della validità del valore inserito
  //----------------------------------------------
  if(NumPorta<1 || NumPorta>TOTPORTE){
     Application->MessageBox( MESS[373], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }
  //----------------------------------------------
  // Apertura dell'editor del TGV selezionato
  //----------------------------------------------
  FormPorteImp->NumeroPortaImp = NumPorta;
  FormPorteImp->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BtnListBoxClick(TObject *Sender)
{
  FormStatoMagaz->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BtnDBSQLClick(TObject *Sender)
{
  //---------------------------------
  // Controllo livello di password
  //---------------------------------
  if(LivelloPassword<2) return;
  //---------------------------------
  // Apertura della form
  //---------------------------------
  FormSQLDB->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BtnZoomAllClick(TObject *Sender)
{
  //-----------------------------------------------
  // Valori per visualizzazione impianto intero
  //-----------------------------------------------
  Rid          = RidAll;
  offset[1][0] = OffSO1;
  offset[1][1] = OffSV1;
  //-----------------------------------------------
  // Azzeramento variabili
  //-----------------------------------------------
  FormLayout->PosizionamentoSuNodo=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BtnZoomNumeriClick(TObject *Sender)
{
  Rid = (double)(RID_NUMERI);
  if(Rid<RID_NUMCAR) Rid = (double)(RID_NUMCAR+0.01);
  offset[1][0] = OffSO1;
  offset[1][1] = OffSV1;
  //-----------------------------------------------
  // Azzeramento variabili
  //-----------------------------------------------
  FormLayout->PosizionamentoSuNodo=0;
}
//---------------------------------------------------------------------------

//-------------------------------
//   ReadIniProject()
//-------------------------------
// Lettura del file *.ini di sistema
//
void TFormMain::ReadIniProject()
{
  TIniFile   *IniFile;
  AnsiString FBuff;
  AnsiString FIniFileName;
  short int  DistSaltaNodoInCm = DIST_SALTO_NODI;

  FIniFileName = PathProject + "IniProject.ini";
  IniFile      = new TIniFile(FIniFileName);
  //-------------------------------------------------------
  // Posizionamento della Form all'interno dello schermo
  //-------------------------------------------------------
  FBuff = IniFile->ReadString("Remoto", "PathServer", DriveServer);
  strcpy(DriveServer, FBuff.c_str());
  //-------------------------------------------------------
  // Recupero della lingua di sistema
  //-------------------------------------------------------
  FBuff = IniFile->ReadString("Messaggeria", "Lingua", LinguaSistema);
  strcpy(LinguaSistema, FBuff.c_str());
  //-----------------------------------------------------
  // Gestione Password opertatore
  //-------------------------------------------------------
  AttivaPassword3Veloce = IniFile->ReadBool("Password", "AttivaPassword3Veloce", AttivaPassword3Veloce);
  Pass0     = IniFile->ReadString("Password", "Password0",    Pass0);
  Pass1     = IniFile->ReadString("Password", "Password1",    Pass1);
  Pass2[ 1] = IniFile->ReadString("Password", "Password2_01", Pass2[ 1]);
  Pass2[ 2] = IniFile->ReadString("Password", "Password2_02", Pass2[ 2]);
  Pass2[ 3] = IniFile->ReadString("Password", "Password2_03", Pass2[ 3]);
  Pass2[ 4] = IniFile->ReadString("Password", "Password2_04", Pass2[ 4]);
  Pass2[ 5] = IniFile->ReadString("Password", "Password2_05", Pass2[ 5]);
  Pass2[ 6] = IniFile->ReadString("Password", "Password2_06", Pass2[ 6]);
  Pass2[ 7] = IniFile->ReadString("Password", "Password2_07", Pass2[ 7]);
  Pass2[ 8] = IniFile->ReadString("Password", "Password2_08", Pass2[ 8]);
  Pass2[ 9] = IniFile->ReadString("Password", "Password2_09", Pass2[ 9]);
  Pass2[10] = IniFile->ReadString("Password", "Password2_10", Pass2[10]);
  Pass3[ 1] = IniFile->ReadString("Password", "Password3_01", Pass3[ 1]);
  Pass3[ 2] = IniFile->ReadString("Password", "Password3_02", Pass3[ 2]);
  Pass3[ 3] = IniFile->ReadString("Password", "Password3_03", Pass3[ 3]);
  Pass3[ 4] = IniFile->ReadString("Password", "Password3_04", Pass3[ 4]);
  Pass3[ 5] = IniFile->ReadString("Password", "Password3_05", Pass3[ 5]);
  Pass3[ 6] = IniFile->ReadString("Password", "Password3_06", Pass3[ 6]);
  Pass3[ 7] = IniFile->ReadString("Password", "Password3_07", Pass3[ 7]);
  Pass3[ 8] = IniFile->ReadString("Password", "Password3_08", Pass3[ 8]);
  Pass3[ 9] = IniFile->ReadString("Password", "Password3_09", Pass3[ 9]);
  Pass3[10] = IniFile->ReadString("Password", "Password3_10", Pass3[10]);
  LivelloMinPassword = (short)(IniFile->ReadInteger("Password", "LivelloMinPassword", 1));
  LivelloMaxPassword = (short)(IniFile->ReadInteger("Password", "LivelloMaxPassword", 3));
  LivelloPassword    = LivelloMinPassword;
  //-------------------------------------------------------
  // Gestione Percorsi
  //-------------------------------------------------------
  CurveDiBezier                           = (short) IniFile->ReadInteger("Percorsi", "CurveDiBezier",                           CurveDiBezier);
  MaxMetriOccupabili                      = (short)(IniFile->ReadInteger("Percorsi", "MaxMetriOccupabili",                      30));
  NumCicliPerSbloccoDeadLock              = (short)(IniFile->ReadInteger("Percorsi", "NumCicliPerSbloccoDeadLock",              15));
  ProseguiPerSbloccoDeadLock              = IniFile->ReadBool("Percorsi", "ProseguiPerSbloccoDeadLock",                         true);
  GrdMinPerAttivaDeviazioni               = (short) IniFile->ReadInteger("Percorsi", "GrdMinPerAttivaDeviazioni",               5);
  AbilitaAvvicinamentoADest               = (short) IniFile->ReadInteger("Percorsi", "AbilitaAvvicinamentoADest",               0);
  AbilitaGestioneIngombroCurvePerStopTGV  = (short) IniFile->ReadInteger("Percorsi", "AbilitaGestioneIngombroCurvePerStopTGV",  0);       // Abilita deposito vuoti in parcheggio
  AbilitaResetMissioniAlVolo              = (short) IniFile->ReadInteger("Percorsi", "AbilitaResetMissioniAlVolo",              0);
  ManualeConNodiOccupati                  = IniFile->ReadBool("Percorsi", "ManualeConNodiOccupati",                   ManualeConNodiOccupati);
  AbilitaRotazioneSuiRettilinei           = IniFile->ReadBool("Percorsi", "AbilitaRotazioneSuiRettilinei",            AbilitaRotazioneSuiRettilinei);
  AbilitaPuntoIntermedioSuRotaz           = IniFile->ReadBool("Percorsi", "AbilitaPuntoIntermedioSuRotaz",            false);
  AbilitaPTPerSpostaTgv                   = IniFile->ReadBool("Percorsi", "AbilitaPTPerSpostaTgv",                    true);
  AbilitaGestionePercorsiVeloci           = IniFile->ReadBool("Percorsi", "AbilitaGestionePercorsiVeloci",            true);
  OccupaTuttoSuPathVeloci                 = IniFile->ReadBool("Percorsi", "OccupaTuttoSuPathVeloci",                  true);
  NodoDiAttesaVersoRicarica               = IniFile->ReadBool("Percorsi", "NodoDiAttesaVersoRicarica",                true);
  PosSuFuoriIngombroPreRicarica           = IniFile->ReadBool("Percorsi", "PosSuFuoriIngombroPreRicarica",            false);
  AnticipoPintPerPtInIngombro             = IniFile->ReadBool("Percorsi", "AnticipoPintPerPtInIngombro",              true);
  ForzaPIntPerNavettaInAttesa             = IniFile->ReadBool("Percorsi", "ForzaPIntPerNavettaInAttesa",              true);
  AbilitaVerificaZonaFuoriIngombro        = IniFile->ReadBool("Percorsi", "AbilitaVerificaZonaFuoriIngombro",         false);
  NoPIntSuStartDeviazione                 = IniFile->ReadBool("Percorsi", "NoPIntSuStartDeviazione",                  true);
  NoPIntSuStartCurva                      = IniFile->ReadBool("Percorsi", "NoPIntSuStartCurva",                       true);
  AbilitaControlloRetteParalleleInclinate = IniFile->ReadBool("Percorsi", "AbilitaControlloRetteParalleleInclinate",  true);
  ZoneFuoriIngManConStopPrenotati         = IniFile->ReadBool("Percorsi", "ZoneFuoriIngManConStopPrenotati",          true);           //
  AbilitaPrenotatiConCompattaTGV          = IniFile->ReadBool("Percorsi", "AbilitaPrenotatiConCompattaTGV",           true);

  //-------------------------------------------------------
  // Gestione Impostazioni varie del sistema
  //-------------------------------------------------------
  PlantName                           = IniFile->ReadString("Sistema", "PlantName", PlantName);
  MSecIntervalloTimerCiclo            = (short) IniFile->ReadInteger("Sistema", "MSecIntervalloTimerCiclo", 300);
  DisattivaDelaySpeciale              = IniFile->ReadBool("Sistema", "DisattivaDelaySpeciale",              DisattivaDelaySpeciale);
  AbilitaRiposoFlottante              = IniFile->ReadBool("Sistema", "RiposoFlottante",                     AbilitaRiposoFlottante);
  AbilitaConfigurazioneImpianto       = IniFile->ReadBool("Sistema", "AbilitaConfigurazioneImpianto",       false);
  AbilitaLetturaMultiPLC              = IniFile->ReadBool("Sistema", "AbilitaLetturaMultiPLC",              true);
  AttesaTgvLiberoFuoriIngombro        = IniFile->ReadBool("Sistema", "AttesaTgvLiberoFuoriIngombro",        true);
  AbilitaPIntPerConsensiMacchina      = IniFile->ReadBool("Sistema", "AbilitaPIntPerConsensiMacchina",      true);
  AbilitaControlloPresaSuTGV          = IniFile->ReadBool("Sistema", "AbilitaControlloPresaSuTGV",          true);
  AbilitaControlloNodoDest            = IniFile->ReadBool("Sistema", "AbilitaControlloNodoDest",            CONTROLLO_NODO_DEPOSITO);
  DisattivazioneManualeMagTmp         = IniFile->ReadBool("Sistema", "DisattivazioneManualeMagTmp",         false);
  PrioritaCercaAgvLiberoNONRicarica   = IniFile->ReadBool("Sistema", "PrioritaCercaAgvLiberoNONRicarica",   true);
  CercaAgvLiberoInRicarica            = (short) IniFile->ReadInteger("Sistema", "CercaAgvLiberoInRicarica",          0);
  AbilitaSvuotamentoManualeMag        = (short) IniFile->ReadInteger("Sistema", "AbilitaSvuotamentoManualeMag",      0);
  AbilitaDepositoMagSovrapposto       = (short) IniFile->ReadInteger("Sistema", "AbilitaDepositoMagSovrapposto",     0);
  CanaleCronoAllarmiTGV               = (short) IniFile->ReadInteger("Sistema", "CanaleCronoAllarmiTGV",             CanaleCronoAllarmiTGV);
  TempoPerMissioneRicarica            = (short) IniFile->ReadInteger("Sistema", "TempoPerMissioneRicarica",          1);
  TimeAttesaSuPTInIngombroNoSblocco   = (short) IniFile->ReadInteger("Sistema", "TimeAttesaSuPTInIngombroNoSblocco", 5);
  MagSovrappMAX_BASSI                 = (short) IniFile->ReadInteger("Sistema", "MagSovrappMAX_BASSI",              99);
  MagSovrappMAX_ALTI                  = (short) IniFile->ReadInteger("Sistema", "MagSovrappMAX_ALTI",               99);
  RilascioApp                         = IniFile->ReadInteger("Sistema", "RilascioApp", 100);
  //-------------------------------------------------------
  // Comunicazioni
  //-------------------------------------------------------
  break_Com[COM1] = IniFile->ReadBool("Comunicazioni", "TGV",        break_Com[COM1]);
  break_Com[COM2] = IniFile->ReadBool("Comunicazioni", "PLC",        break_Com[COM2]);
  break_Com[COM3] = IniFile->ReadBool("Comunicazioni", "SQLSERVER",  break_Com[COM3]);
  break_Com[COM4] = IniFile->ReadBool("Comunicazioni", "COM4",       true);                              // Abilitazione comunicazione ausiliaria
  break_Com[COM5] = IniFile->ReadBool("Comunicazioni", "COM5",       true);                              // Abilitazione comunicazione ausiliaria
  break_Com[COM6] = IniFile->ReadBool("Comunicazioni", "IO_REMOTI",  true);                              // Abilitazione comunicazione ausiliaria
  break_Com[COM7] = IniFile->ReadBool("Comunicazioni", "COM7",       true);                              // Abilitazione comunicazione ausiliaria
  break_Com[COM8] = IniFile->ReadBool("Comunicazioni", "COM8",       true);                              // Abilitazione comunicazione ausiliaria
  break_Com[COM9] = IniFile->ReadBool("Comunicazioni", "SRVANDROID", true);                              // Abilitazione comunicazione ausiliaria
  MinErrConsecutiviTGV    = (short) IniFile->ReadInteger("Comunicazioni", "MinErrConsecutiviTGV",    10);

  //-------------------------------------------------------
  // Parametri di connessione a SQL
  //-------------------------------------------------------
  ProviderADO            = IniFile->ReadString("DBASE", "ProviderADO", ProviderADO);
  NomeSRVSQL             = IniFile->ReadString("DBASE", "NomeSRVSQL", NomeSRVSQL);
  NomeDBase              = IniFile->ReadString("DBASE", "NomeDBase",  NomeDBase);
  NomeTabA               = IniFile->ReadString("DBASE", "NomeTabA",   NomeTabA);
  DBaseUser              = IniFile->ReadString("DBASE", "DBaseUser",  DBaseUser);
  DBasePsw               = IniFile->ReadString("DBASE", "DBasePsw",   DBasePsw);
  MinRiconnessioneSQL    = (short) IniFile->ReadInteger("DBASE", "MinRiconnessioneSQL",     5);
  SecConnectioTimeOutSQL = (short) IniFile->ReadInteger("DBASE", "SecConnectioTimeOutSQL",  15);
  SecComandoTimeOutSQL   = (short) IniFile->ReadInteger("DBASE", "SecComandoTimeOutSQL",    30);
  SecDelayOnErrSQL       = (short) IniFile->ReadInteger("DBASE", "SecDelayOnErrSQL",        120);
  SecReloadOnThreadSQL   = (short) IniFile->ReadInteger("DBASE", "SecReloadOnThreadSQL",    3);
  NoLockQuery            = (bool)  IniFile->ReadBool("DBASE", "NoLockQuery",                false);
  AggiornaLogSQL         = (bool)  IniFile->ReadBool("DBASE", "AggiornaLogSQL",             true);
                                                                                                     // 0 - nessuno
  //----------------------------------------------------------
  // ESCLUSIONE COMUNICAZIONE SQL
  // se non abilitata gestione UDC
  //----------------------------------------------------------
  if(GestioneUdc==0) break_Com[COM3]=true;

  //----------------------------------------------------------
  // SALTO NODI
  // Incompatibile con:
  // - Pint su rotazione.
  //----------------------------------------------------------
  if(DistSaltaNodoInCm!=0){
     AbilitaPuntoIntermedioSuRotaz=false;
  }

  //----------------------------------------------------------
  // MAGAZZINI TEMPORANEI
  // in caso di pallet retrival con ritorno alla posizione
  // di origine "DisattivazioneManualeMagTmp=false" perchè
  // il magazzino TMP deve tornare attivo in automatico
  //----------------------------------------------------------
  if(AbilitaSvuotamentoManualeMag==3) DisattivazioneManualeMagTmp=false;

  //----------------------------------------------------------
  // GESTIONE CURVE (LASER)
  // Incompatibile con:
  // - Pint su rotazione.
  //----------------------------------------------------------
  #ifdef GESTIONE_CURVE
     AbilitaPuntoIntermedioSuRotaz=false;
  #endif;

  //-------------------------------------------------------
  // Gestione CLIENT
  //  - comunicazioni con TGV e PLC sempre OFF
  //  - comunicazione con SERVER SQL impostabile
  //-------------------------------------------------------
  if( IdPC>1 ){
     break_Com[COM1] = true;
     break_Com[COM2] = true;
     break_Com[COM4] = true;
     break_Com[COM5] = true;
     break_Com[COM6] = true;
     break_Com[COM7] = true;
     break_Com[COM8] = true;
     break_Com[COM9] = true;
  }
  delete IniFile;
}
//---------------------------------------------------------------------------
//-------------------------------
//   ReadConfigInterbloccoNodi()
//-------------------------------
// Lettura dei files XLS dell'interblocco nodi
//
int TFormMain::ReadConfigInterbloccoNodi(char *all_mess)
{
  int  Row, Col;
  AnsiString Buff;
  AnsiString Filename;

  //-----------------------------------------
  // Azzeramento struttura
  //-----------------------------------------
  memset(&N.nodi_interbloccati, 0, sizeof(N.nodi_interbloccati));
  //-----------------------------------------
  // Recupero il nome delle ricette per i
  // puntamenti
  //-----------------------------------------
  Filename = PathConfig+"ConfigInterbloccoNodi"+ ".xls";
  //-----------------------------------------
  // Nessun file di configurazione --> skip
  //-----------------------------------------
  if(access(Filename.c_str(), 0)){
     sprintf(all_mess, "the configuration file %s doesn't exist...", Filename.c_str());
     return 1;
  }
  //-----------------------------------------
  // Importazione file excel
  //-----------------------------------------
  XLS->Filename = Filename;
  XLS->Clear();              // azzeramento
  XLS->Read();               // rilettura
  //------------------------------------------------
  // Compilazione della struttura
  //------------------------------------------------
  try{
     for(Row=0; Row<50; Row++){
        for(Col=0; Col<50; Col++){
           //----------------------------------------------------------------
           // Non sono ammesse più di 255 parametri a ricetta
           //----------------------------------------------------------------
           if(Row>XLS->Sheets->Items[0]->LastRow) break;
           Buff = XLS->Sheets->Items[0]->AsString[Col][Row];
           N.nodi_interbloccati[Row][Col] = (short int)(atoi(Buff.c_str()));
        }
     }
  }
  catch(...)
  {
     ShowMessage("Configurazione attese TGV non possibile...");
     Application->Terminate();
  }
  return 0;
}
//---------------------------------------------------------------------------
//-------------------------------
//   ReadConfigPintObbligato()
//-------------------------------
// Lettura dei files XLS dell'interblocco nodi
//
int TFormMain::ReadConfigPintObbligato(char *all_mess)
{
  int  Row;
  AnsiString Buff;
  AnsiString Filename;

  //-----------------------------------------
  // Azzeramento struttura
  //-----------------------------------------
  memset(&N.pint_obbligato, 0, sizeof(N.pint_obbligato));
  //-----------------------------------------
  // Recupero il nome delle ricette per i
  // puntamenti
  //-----------------------------------------
  Filename = PathConfig+"ConfigPintObbligato"+ ".xls";
  //-----------------------------------------
  // Nessun file di configurazione --> skip
  //-----------------------------------------
  if(access(Filename.c_str(), 0)){
     sprintf(all_mess, "the configuration file %s doesn't exist...", Filename.c_str());
     return 1;
  }
  //-----------------------------------------
  // Importazione file excel
  //-----------------------------------------
  XLS->Filename = Filename;
  XLS->Clear();              // azzeramento
  XLS->Read();               // rilettura
  //------------------------------------------------
  // Compilazione della struttura
  //------------------------------------------------
  try{
     for(Row=1; Row<100; Row++){
        //----------------------------------------------------------------
        // Non sono ammesse più di 100 parametri a ricetta
        //----------------------------------------------------------------
        if(Row>XLS->Sheets->Items[0]->LastRow) break;
        Buff = XLS->Sheets->Items[0]->AsString[0][Row];
        N.pint_obbligato[Row-1].pintend = (short int)(atoi(Buff.c_str()));
        Buff = XLS->Sheets->Items[0]->AsString[1][Row];
        N.pint_obbligato[Row-1].prec = (short int)(atoi(Buff.c_str()));
        Buff = XLS->Sheets->Items[0]->AsString[2][Row];
        N.pint_obbligato[Row-1].succ = (short int)(atoi(Buff.c_str()));
     }
  }
  catch(...)
  {
     ShowMessage("Configurazione Pint obbligato TGV non possibile...");
     Application->Terminate();
  }
  return 0;
}
//---------------------------------------------------------------------------
//-------------------------------
//   ReadConfigAtteseTGV()
//-------------------------------
// Lettura dei files XLS dell'interblocco nodi
//
int TFormMain::ReadConfigAtteseTGV(char *all_mess)
{
  int  i, Row, Col;
  AnsiString Buff;
  AnsiString Filename;

  //-----------------------------------------
  // Azzeramento struttura
  //-----------------------------------------
  memset(&N.nodi_attesa, 0, sizeof(N.nodi_attesa));
  //-----------------------------------------
  // Recupero il nome delle ricette per i
  // puntamenti
  //-----------------------------------------
  Filename = PathConfig+"ConfigPosAttesa"+ ".xls";
  //-----------------------------------------
  // Nessun file di configurazione --> skip
  //-----------------------------------------
  if(access(Filename.c_str(), 0)){
     sprintf(all_mess, "the configuration file %s doesn't exist...", Filename.c_str());
     return 1;
  }
  //-----------------------------------------
  // Importazione file excel
  //-----------------------------------------
  XLS->Filename = Filename;
  XLS->Clear();              // azzeramento
  XLS->Read();               // rilettura
  //------------------------------------------------
  // Compilazione della struttura
  //------------------------------------------------
  try{
     for(Row=0; Row<MAXGRUPPIIMP; Row++){
        i=0;
        for(Col=1; Col<MAXNODIATTESA; Col++){
           //----------------------------------------------------------------
           // Non sono ammesse più di 255 parametri a ricetta
           //----------------------------------------------------------------
           if(Row>XLS->Sheets->Items[0]->LastRow) break;
           Buff = XLS->Sheets->Items[0]->AsString[Col][Row];
           N.nodi_attesa[Row][i] = (short int)(atoi(Buff.c_str()));
           i++;
        }
     }
  }
  catch(...)
  {
     ShowMessage("Configurazione attese TGV non possibile...");
     Application->Terminate();
  }
  return 0;
}
//---------------------------------------------------------------------------
//-------------------------------
//   WriteIniProject()
//-------------------------------
// Scrittura del file *.ini di sistema
//
void TFormMain::WriteIniProject()
{
  TIniFile *IniFile;
  AnsiString FPath;
  AnsiString FIniFileName;

  FPath        = ExtractFilePath(ParamStr(0));
  FIniFileName = FPath + "IniProject.ini";
  IniFile      = new TIniFile(FIniFileName);
  //-------------------------------------------------------
  // Posizionamento della Form all'interno dello schermo
  //-------------------------------------------------------
  IniFile->WriteString("Remoto", "PathServer", DriveServer);
  //-------------------------------------------------------
  // Messaggeria di sistema
  //-------------------------------------------------------
  IniFile->WriteString("Messaggeria", "Lingua", LinguaSistema);
  //-------------------------------------------------------
  // Gestione Password opertatore
  //-------------------------------------------------------
  IniFile->WriteBool("Password",   "AttivaPassword3Veloce", AttivaPassword3Veloce);
  IniFile->WriteString("Password", "Password0",    Pass0);
  IniFile->WriteString("Password", "Password1",    Pass1);
  IniFile->WriteString("Password", "Password2_01", Pass2[ 1]);
  IniFile->WriteString("Password", "Password2_02", Pass2[ 2]);
  IniFile->WriteString("Password", "Password2_03", Pass2[ 3]);
  IniFile->WriteString("Password", "Password2_04", Pass2[ 4]);
  IniFile->WriteString("Password", "Password2_05", Pass2[ 5]);
  IniFile->WriteString("Password", "Password2_06", Pass2[ 6]);
  IniFile->WriteString("Password", "Password2_07", Pass2[ 7]);
  IniFile->WriteString("Password", "Password2_08", Pass2[ 8]);
  IniFile->WriteString("Password", "Password2_09", Pass2[ 9]);
  IniFile->WriteString("Password", "Password2_10", Pass2[10]);
  IniFile->WriteString("Password", "Password3_01", Pass3[ 1]);
  IniFile->WriteString("Password", "Password3_02", Pass3[ 2]);
  IniFile->WriteString("Password", "Password3_03", Pass3[ 3]);
  IniFile->WriteString("Password", "Password3_04", Pass3[ 4]);
  IniFile->WriteString("Password", "Password3_05", Pass3[ 5]);
  IniFile->WriteString("Password", "Password3_06", Pass3[ 6]);
  IniFile->WriteString("Password", "Password3_07", Pass3[ 7]);
  IniFile->WriteString("Password", "Password3_08", Pass3[ 8]);
  IniFile->WriteString("Password", "Password3_09", Pass3[ 9]);
  IniFile->WriteString("Password", "Password3_10", Pass3[10]);
  IniFile->WriteInteger("Password", "LivelloMinPassword", LivelloMinPassword);
  IniFile->WriteInteger("Password", "LivelloMaxPassword", LivelloMaxPassword);

  //-------------------------------------------------------
  // Gestioni legate ai percorsi
  //-------------------------------------------------------
  IniFile->WriteInteger("Percorsi", "CurveDiBezier",                         CurveDiBezier);
  IniFile->WriteInteger("Percorsi", "MaxMetriOccupabili",                    MaxMetriOccupabili);
  IniFile->WriteInteger("Percorsi", "NumCicliPerSbloccoDeadLock",            NumCicliPerSbloccoDeadLock);
  IniFile->WriteBool("Percorsi", "ProseguiPerSbloccoDeadLock",               ProseguiPerSbloccoDeadLock);
  IniFile->WriteInteger("Percorsi", "GrdMinPerAttivaDeviazioni",             GrdMinPerAttivaDeviazioni);
  IniFile->WriteInteger("Percorsi", "AbilitaAvvicinamentoADest",             AbilitaAvvicinamentoADest);
  IniFile->WriteInteger("Percorsi", "AbilitaGestioneIngombroCurvePerStopTGV",AbilitaGestioneIngombroCurvePerStopTGV);
  IniFile->WriteInteger("Percorsi", "AbilitaResetMissioniAlVolo",            AbilitaResetMissioniAlVolo);
  IniFile->WriteBool("Percorsi", "ManualeConNodiOccupati",                   ManualeConNodiOccupati);
  IniFile->WriteBool("Percorsi", "AbilitaRotazioneSuiRettilinei",            AbilitaRotazioneSuiRettilinei);
  IniFile->WriteBool("Percorsi", "AbilitaPuntoIntermedioSuRotaz",            AbilitaPuntoIntermedioSuRotaz);
  IniFile->WriteBool("Percorsi", "AbilitaPTPerSpostaTgv",                    AbilitaPTPerSpostaTgv);
  IniFile->WriteBool("Percorsi", "AbilitaGestionePercorsiVeloci",            AbilitaGestionePercorsiVeloci);
  IniFile->WriteBool("Percorsi", "OccupaTuttoSuPathVeloci",                  OccupaTuttoSuPathVeloci);
  IniFile->WriteBool("Percorsi", "NodoDiAttesaVersoRicarica",                NodoDiAttesaVersoRicarica);
  IniFile->WriteBool("Percorsi", "AnticipoPintPerPtInIngombro",              AnticipoPintPerPtInIngombro);
  IniFile->WriteBool("Percorsi", "PosSuFuoriIngombroPreRicarica",            PosSuFuoriIngombroPreRicarica);
  IniFile->WriteBool("Percorsi", "ForzaPIntPerNavettaInAttesa",              ForzaPIntPerNavettaInAttesa);
  IniFile->WriteBool("Percorsi", "AbilitaVerificaZonaFuoriIngombro",         AbilitaVerificaZonaFuoriIngombro);
  IniFile->WriteBool("Percorsi", "NoPIntSuStartDeviazione",                  NoPIntSuStartDeviazione);
  IniFile->WriteBool("Percorsi", "NoPIntSuStartCurva",                       NoPIntSuStartCurva);
  IniFile->WriteBool("Percorsi", "AbilitaControlloRetteParalleleInclinate",  AbilitaControlloRetteParalleleInclinate);
  IniFile->WriteBool("Percorsi", "ZoneFuoriIngManConStopPrenotati",          ZoneFuoriIngManConStopPrenotati);            // Abilita deposito vuoti in parcheggio
  IniFile->WriteBool("Percorsi", "AbilitaPrenotatiConCompattaTGV",           AbilitaPrenotatiConCompattaTGV);

  //-------------------------------------------------------
  // Gestione Impostazioni varie del sistema
  //-------------------------------------------------------
  IniFile->WriteString("Sistema", "PlantName",                          PlantName);
  IniFile->WriteInteger("Sistema", "MSecIntervalloTimerCiclo",          MSecIntervalloTimerCiclo);
  IniFile->WriteBool("Sistema", "DisattivaDelaySpeciale",               DisattivaDelaySpeciale);
  IniFile->WriteBool("Sistema", "RiposoFlottante",                      AbilitaRiposoFlottante);
  IniFile->WriteBool("Sistema", "PrioritaCercaAgvLiberoNONRicarica",    PrioritaCercaAgvLiberoNONRicarica);
  IniFile->WriteBool("Sistema", "AbilitaConfigurazioneImpianto",        AbilitaConfigurazioneImpianto);
  IniFile->WriteBool("Sistema", "AbilitaLetturaMultiPLC",               AbilitaLetturaMultiPLC);
  IniFile->WriteBool("Sistema", "AttesaTgvLiberoFuoriIngombro",         AttesaTgvLiberoFuoriIngombro);
  IniFile->WriteBool("Sistema", "AbilitaPIntPerConsensiMacchina",       AbilitaPIntPerConsensiMacchina);
  IniFile->WriteBool("Sistema", "AbilitaControlloPresaSuTGV",           AbilitaControlloPresaSuTGV);
  IniFile->WriteBool("Sistema", "AbilitaControlloNodoDest",             AbilitaControlloNodoDest);
  IniFile->WriteBool("Sistema", "DisattivazioneManualeMagTmp",          DisattivazioneManualeMagTmp);
  IniFile->WriteInteger("Sistema", "CercaAgvLiberoInRicarica",          CercaAgvLiberoInRicarica);
  IniFile->WriteInteger("Sistema", "AbilitaSvuotamentoManualeMag",      AbilitaSvuotamentoManualeMag);
  IniFile->WriteInteger("Sistema", "AbilitaDepositoMagSovrapposto",     AbilitaDepositoMagSovrapposto);
  IniFile->WriteInteger("Sistema", "CanaleCronoAllarmiTGV",             CanaleCronoAllarmiTGV);
  IniFile->WriteInteger("Sistema", "TempoPerMissioneRicarica",          TempoPerMissioneRicarica);
  IniFile->WriteInteger("Sistema", "TimeAttesaSuPTInIngombroNoSblocco", TimeAttesaSuPTInIngombroNoSblocco);
  IniFile->WriteInteger("Sistema", "MagSovrappMAX_BASSI",               MagSovrappMAX_BASSI);
  IniFile->WriteInteger("Sistema", "MagSovrappMAX_ALTI",                MagSovrappMAX_ALTI);
  IniFile->WriteInteger("Sistema", "RilascioApp",                         RilascioApp);
  //-------------------------------------------------------
  // Gestione Comunicazioni
  //-------------------------------------------------------
  IniFile->WriteBool("Comunicazioni", "TGV",        break_Com[COM1]);
  IniFile->WriteBool("Comunicazioni", "PLC",        break_Com[COM2]);
  IniFile->WriteBool("Comunicazioni", "SQLSERVER",  break_Com[COM3]);
  IniFile->WriteBool("Comunicazioni", "COM4",       break_Com[COM4]);
  IniFile->WriteBool("Comunicazioni", "COM5",       break_Com[COM5]);
  IniFile->WriteBool("Comunicazioni", "IO_REMOTI",  break_Com[COM6]);
  IniFile->WriteBool("Comunicazioni", "COM7",       break_Com[COM7]);
  IniFile->WriteBool("Comunicazioni", "COM8",       break_Com[COM8]);
  IniFile->WriteBool("Comunicazioni", "SRVANDROID", break_Com[COM9]);
  IniFile->WriteInteger("Comunicazioni", "MinErrConsecutiviTGV",    MinErrConsecutiviTGV);

  //-------------------------------------------------------
  // Parametri di connessione a SQL
  //-------------------------------------------------------
  IniFile->WriteString("DBASE", "ProviderADO",             ProviderADO);
  IniFile->WriteString("DBASE", "NomeSRVSQL",              NomeSRVSQL);
  IniFile->WriteString("DBASE", "NomeDBase",               NomeDBase);
  IniFile->WriteString("DBASE", "NomeTabA",                NomeTabA);
  IniFile->WriteString("DBASE", "DBaseUser",               DBaseUser);
  IniFile->WriteString("DBASE", "DBasePsw",                DBasePsw);
  IniFile->WriteInteger("DBASE", "MinRiconnessioneSQL",    MinRiconnessioneSQL);
  IniFile->WriteInteger("DBASE", "SecConnectioTimeOutSQL", SecConnectioTimeOutSQL);
  IniFile->WriteInteger("DBASE", "SecComandoTimeOutSQL",   SecComandoTimeOutSQL);
  IniFile->WriteInteger("DBASE", "SecDelayOnErrSQL",       SecDelayOnErrSQL);
  IniFile->WriteInteger("DBASE", "SecReloadOnThreadSQL",   SecReloadOnThreadSQL);
  IniFile->WriteBool("DBASE", "NoLockQuery",               NoLockQuery);
  IniFile->WriteBool("DBASE", "AggiornaLogSQL",            AggiornaLogSQL);

  delete IniFile;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerCicloTimer(TObject *Sender)
{
  char all_mess[201]="";
  int TypCom, err=0;
  int TimeOutSeriale=TIMEOUTSERIALE;
  WORD Port, NumeroPlc;
  AnsiString  LocalIP;
  TIniFile   *IniFile;
  bool GiaInit[MAXPLCCON+1];
  bool SempreON=true;
  int NumMac, NumBai;
  char  IpAddress[300];
  int   MsecRisp;
  bool  io_remoto_trovato;

  TimerCiclo->Enabled = false;     // --- Abilitazione Timer Principale
  MinutoCiclo=Minuto;
  if( PrimaScansione==true ){
     //-----------------------------------------------------
     // SE NON E' UN PC ALGOSISTEMI NON ABILITO LA GESTIONE
     // DEI TGV LANCIATI IN RETTILINEO
     //-----------------------------------------------------
     if(PcAlgo==true || TestInternoTecnoFerrari==true){
        FormLayout->VisualizzaTgvLanciatiInRettilineo=true;
        LivelloPassword=3;
     }
     if(AbilitaSimulazione==true){
        LivelloPassword=3;
     }
     //-----------------------------------------------------
     // Cancellazione file TELECAMERINA alla riaccensione
     // con comunicazione attiva con TGV
     //-----------------------------------------------------
     if(break_Com[COM1]==false && PcAlgo==false){
        unlink("LOGTGV.DAT");
     }
     //-----------------------------------------------------
     // Richiesta password di accensione
     //-----------------------------------------------------
   //RicPasswordAccensione();
     //-----------------------------------------------------
     // Blocchi legati allo stato delle password
     //-----------------------------------------------------
     StatoPassword();
     //-----------------------------------------------------
     // Inizializza il la variabile di ritardo per delay()
     //-----------------------------------------------------
     Calibrate_Delay();
     //-----------------------------------------------------
     // Inizializzazione delle comunicazioni di sistema
     //-----------------------------------------------------
     if( SempreON ){
        //-----------------------------------------------------
        // 03/03/2008
        // Recupero il file "TecnoETH.ini" da "C:\\"
        // e li sposto in "C:\\WINDOWS\\ALGOSISTEMI" in
        // vista di applicazioni per VISTA
        //-----------------------------------------------------
        MkDir("C:\\WINDOWS");
        MkDir("C:\\WINDOWS\\ALGOSISTEMI");
        rename("C:\\TecnoETH.ini", "C:\\WINDOWS\\ALGOSISTEMI\\TecnoETH.ini");
        //---------------------------------
        // Recupera indirizzo dal FILE
        //---------------------------------
        IniFile = new TIniFile("C:\\WINDOWS\\ALGOSISTEMI\\TecnoETH.ini");
        LocalIP = IniFile->ReadString("IP Address", "LocalIP", "0.0.0.0");
        FormServerUDP->FServerAddr = WSocketResolveHost(LocalHostName());
        //---------------------------------------------
        // Se nessun dato memorizzato recupera da rete
        //---------------------------------------------
        if( LocalIP == "0.0.0.0" ){
           LocalIP = WSocket_inet_ntoa(FormServerUDP->FServerAddr);
           //------------------------------------------------------------------
           // Se Indirizzo IP è valido <> da 127.0.0.1 allora SALVA su FILE
           //------------------------------------------------------------------
           if( LocalIP != "127.0.0.1" ){
              IniFile->WriteString("IP Address", "LocalIP", LocalIP);
           }
        }
        //--------------------------------------
        // Imposta LocalIpAddr del ServerUDP
        //--------------------------------------
        FormServerUDP->LocalIPAddr=LocalIP;
        delete IniFile;
     }

     //------------------------------------------------------
     // Funzione di inzializzazione comunicazioni ETHERNET
     //------------------------------------------------------
     OmrInitComunicazioni();
     OmrEthSer_InitComunicazioni();
     OmrEthTcp_InitComunicazioni();

     //------------------------------------------------------
     // GESTIONE I/O REMOTATI
     // Configurazione comunicazione I/O remoti
     //------------------------------------------------------
     io_remoto_trovato=false;
     for(NumeroPlc=1; NumeroPlc<=MAXPLCCON; NumeroPlc++){  // scarto l'indice "0" perchè sicuramente PLC TERRA e
        //-------------------------------------------------------------
        // Modifica automaticamente la descrizione relativa ai PLC
        // impostati come I/O remoti
        //
        // ATTENZIONE: Non superare i 30ch nella dicitura
        //-------------------------------------------------------------
        if(NumeroPlc==PLC_IO_REMOTO_01){io_remoto_trovato=true; OmrPlc[NumeroPlc].Com = COM6; OmrPlc[NumeroPlc].DescrPlc = "REMOTE I/O (01)";}
        if(NumeroPlc==PLC_IO_REMOTO_02){io_remoto_trovato=true; OmrPlc[NumeroPlc].Com = COM6; OmrPlc[NumeroPlc].DescrPlc = "REMOTE I/O (02)";}
        if(NumeroPlc==PLC_IO_REMOTO_03){io_remoto_trovato=true; OmrPlc[NumeroPlc].Com = COM6; OmrPlc[NumeroPlc].DescrPlc = "REMOTE I/O (03)";}
        if(NumeroPlc==PLC_IO_REMOTO_04){io_remoto_trovato=true; OmrPlc[NumeroPlc].Com = COM6; OmrPlc[NumeroPlc].DescrPlc = "REMOTE I/O (04)";}
        if(NumeroPlc==PLC_IO_REMOTO_05){io_remoto_trovato=true; OmrPlc[NumeroPlc].Com = COM6; OmrPlc[NumeroPlc].DescrPlc = "REMOTE I/O (05)";}
        if(NumeroPlc==PLC_IO_REMOTO_06){io_remoto_trovato=true; OmrPlc[NumeroPlc].Com = COM6; OmrPlc[NumeroPlc].DescrPlc = "REMOTE I/O (06)";}
        if(NumeroPlc==PLC_IO_REMOTO_07){io_remoto_trovato=true; OmrPlc[NumeroPlc].Com = COM6; OmrPlc[NumeroPlc].DescrPlc = "REMOTE I/O (07)";}
        if(NumeroPlc==PLC_IO_REMOTO_08){io_remoto_trovato=true; OmrPlc[NumeroPlc].Com = COM6; OmrPlc[NumeroPlc].DescrPlc = "REMOTE I/O (08)";}
        if(NumeroPlc==PLC_IO_REMOTO_09){io_remoto_trovato=true; OmrPlc[NumeroPlc].Com = COM6; OmrPlc[NumeroPlc].DescrPlc = "REMOTE I/O (09)";}
        if(NumeroPlc==PLC_IO_REMOTO_10){io_remoto_trovato=true; OmrPlc[NumeroPlc].Com = COM6; OmrPlc[NumeroPlc].DescrPlc = "REMOTE I/O (10)";}
     }

     //------------------------------------------------------
     // Attivazione SOCKET SERVER per ANDROID
     //------------------------------------------------------
     if(break_Com[COM9]==false) FormSrvTcpAndroid->StartServerTCPAndroid();

     //-------------------------------------------------------
     // Disattivazione automatica se nessun I/O è previsto
     //-------------------------------------------------------
     memset(&errore_comunicazione[COM6][0], 0, sizeof(errore_comunicazione[COM6]));
     if(break_Com[COM6]==false && io_remoto_trovato==false) break_Com[COM6]=true;

     //------------------------------------------------------
     // Ciclo di Inizializzazione di tutte le COM
     //------------------------------------------------------
     memset(&GiaInit[0], false, sizeof(GiaInit));
     for(NumeroPlc=0; NumeroPlc<=MAXPLCCON; NumeroPlc++){
        Port   = (WORD)(OmrPlc[NumeroPlc].Com);
        TypCom = OmrPlc[NumeroPlc].TypCom;
        //-------------------------------------------------------
        // Non inizializza la seriale COM2
        //-------------------------------------------------------
        if(TypCom==0 && Port>COM1){
           GiaInit[Port]=true;
        }
        //-------------------------------------------------------
        // Solo per i PLC abilitati alla comunicazione HOST LINK
        //-------------------------------------------------------
        if(TypCom==0 && !GiaInit[Port]){
           err = OM.init_porta(Port, TimeOutSeriale, all_mess);
           if( err ){
              sprintf( all_mess, "Errore inizializzazione seriale COM%02d!", Port+1);
              ShowMessage(all_mess);
           }
           else err = OM.md(NumeroPlc, 2, all_mess);
           GiaInit[Port]=true;
        }
        //-------------------------------------------------------
        // Protocollo ALGO COMUNICATION
        //-------------------------------------------------------
        // --- inizializzazione
        Scambio_Inizializza();
        // --- load ultimo stato
        if(TypCom==2 && NumeroPlc==0){
           err = Scambio_SV_LetturaInizioProgramma(all_mess);
           if(err) Application->MessageBox(all_mess, MESS[100], MB_OK | MB_ICONEXCLAMATION);
        }
        //--------------------------------------------------
        // THREAD SQL
        //--------------------------------------------------
        if(SecReloadOnThreadSQL!=0){
           FormSQLDB->t_SQL->Priority = tpLower;   // set the priority lower than normal
           if(break_Com[COM3]==false){
              if(FormSQLDB->t_SQL->Suspended==true){
                 //-----------------------------------------------------------
                 // START DEL THREAD
                 //
                 // La priorità può essere impostata a di diversi livelli
                 // se impostata alta si rallenta l'interfaccia operatore
                 // se bassa si rallenta il ciclo automatico.
                 //
                 // t1->Priority = tpNormal;
                 // t1->Priority = tpLower;
                 //
                 // Eventualmente si può impostare una priorità alta quando
                 // non ci sono editor aperti e bassa in caso contrario.
                 //-----------------------------------------------------------
                 FormSQLDB->t_SQL->Resume();          // now start the thread running
              }
           }
           else{
              if(FormSQLDB->t_SQL->Suspended==false){
                 FormSQLDB->t_SQL->Suspend();
              }
           }
        }
     }
     //-----------------------------------------------------
     // Azzero il flag di prima scansione software attiva
     //-----------------------------------------------------
     //PrimaScansione=false;
     //-----------------------------------------------------
     // Inizializzazione variabili solo in Prima Scansione
     //-----------------------------------------------------
     P.RunAutomaticoEseguito=false;
     //-----------------------------------------------------
     // Creazione di alcune directory necessarie per la
     // gestione dei dati di Produzione e dei PC Remoti.
     //-----------------------------------------------------
     MkDir("PRODUZIO");
     MkDir("COMANDI");
     MkDir("STATO");
     MkDir("SORG");
     MkDir("OLD");
     MkDir("LASER");
     MkDir("SQL");
     MkDir("TX");
     //------------------------------------------------------
     // Visualizzo lo stato delle comunicazioni sul menù
     //------------------------------------------------------
     MCOM1->Checked                = !break_Com[COM1];
     MCOM2->Checked                = !break_Com[COM2];
     MCOM3->Checked                = !break_Com[COM3];
     MCOM4->Checked                = !break_Com[COM4];
     MCOM5->Checked                = !break_Com[COM5];
     MCOM6->Checked                = !break_Com[COM6];
     MCOM7->Checked                = !break_Com[COM7];
     MCOM8->Checked                = !break_Com[COM8];
     MCOM9->Checked                = !break_Com[COM9];
     //------------------------------------------------------
     // Gestione PC Server
     //------------------------------------------------------
     err = RemGestioneServer( all_mess );
     if( err ) P.error_trasmit( 0, all_mess);
     //------------------------------------------------------
     // Inizializza AllenBradley
     //------------------------------------------------------
     AllenB_Inizializza( all_mess);
  }

  //------------------------------------------------------------
  // INIZIALIZZAZIONE SOCKET SIEMENS ogni Minuto
  // 14/06/2012 GB
  //
  // -Connessione al 1° Ciclo di Scansione
  // -Tentativi sucessivi ogni 1 Minuti
  //  In questo modo si ripristinano le eventuali
  //  connessioni perse (es. Macchine spente)
  // -I tentativi di connessione sono subordinati a un Ping()
  //  in modo da non riallocare risorse.
  // -Ho verificato che  tentativi ripetuti di NoDave_Connect()
  //  allocano socket senza rilasciarli causando problemi alla
  //  chiusura e a lungo andare alle connessione sucessive.
  //
  //------------------------------------------------------------
  if( PrimaScansione || MinutoCiclo ){
     //------------------------------------------------------
     // INIZIALIZZAZIONE COMUNICAZIONI SIEMENS
     //------------------------------------------------------
     if(break_Com[COM_SIEMENS]==false){
        // MACCHINE
        memset(&GiaInit[0], false, sizeof(GiaInit));
        for(NumMac=1; NumMac<=TOTCHIAMATE; NumMac++){
           NumeroPlc=P.ch[NumMac]->Plc.Plc;
           if(GiaInit[NumeroPlc]==true) continue;
           //if(NumeroPlc==0          ) continue;    // indice "0" riservato al PLC DI TERRA
           GiaInit[NumeroPlc]=true;
           if(OmrPlc[ NumeroPlc ].RemotePort!=102) continue;  // Porta 102 PLC Siemens
           //------------------------------------------------------
           // Inizializzazione porta
           //------------------------------------------------------
           if( SIM_PlcConnesso[NumeroPlc]==0 ){
              strcpy( IpAddress, OmrPlc[ NumeroPlc ].RemoteHost.c_str());
              err=0;
              //---------------------------------------------------------------------------------------
              // Ping()
              // Inizialmente era stato inserito il PING per evitare di tentare la connessione
              // su un indirizzo non presente perchè dava problemi. In realtà richiamando la funzione
              // di connessione NON tutti i cicli (300 msec) ma 1 volta al minuto (o anche meno)
              // sembra che non si abbiamo effetti idesiderati......
              //---------------------------------------------------------------------------------------
              err=AlgoPing( IpAddress, 10, &MsecRisp, all_mess );
              if( err==0 ) err = init_porta_siemens(NumeroPlc, 0, all_mess);     // IndexPlc = slot = 2 per S300
              if( err ){
                 sprintf( all_mess, "Error in connection with PLC SIEMENS (disp %d): %s", NumeroPlc, all_mess);
                 P.error_trasmit(0, all_mess);
              }
           }
        }
        // BAIE
        for(NumBai=1; NumBai<=TOTBAIE; NumBai++){
           NumeroPlc=P.ba[NumBai]->Plc;
           if(GiaInit[NumeroPlc]==true) continue;
           //if(NumeroPlc==0          ) continue;    // indice "0" riservato al PLC DI TERRA
           GiaInit[NumeroPlc]=true;
           if(OmrPlc[ NumeroPlc ].RemotePort!=102) continue;  // Porta 102 PLC Siemens
           //------------------------------------------------------
           // Inizializzazione porta
           //------------------------------------------------------
           if( SIM_PlcConnesso[NumeroPlc]==0 ){
              strcpy( IpAddress, OmrPlc[ NumeroPlc ].RemoteHost.c_str());
              err=0;
              //---------------------------------------------------------------------------------------
              // Ping()
              // Inizialmente era stato inserito il PING per evitare di tentare la connessione
              // su un indirizzo non presente perchè dava problemi. In realtà richiamando la funzione
              // di connessione NON tutti i cicli (300 msec) ma 1 volta al minuto (o anche meno)
              // sembra che non si abbiamo effetti idesiderati......
              //---------------------------------------------------------------------------------------
              err=AlgoPing( IpAddress, 10, &MsecRisp, all_mess );
              if( err==0 ) err = init_porta_siemens(NumeroPlc, 0, all_mess);     // IndexPlc = slot = 2 per S300
              if( err ){
                 sprintf( all_mess, "Error in connection with PLC SIEMENS (disp %d): %s", NumeroPlc, all_mess);
                 P.error_trasmit(0, all_mess);
              }
           }
        }
     }
  }
  //---------------------------------------
  // Gestione SERVER
  //---------------------------------------
  if((IdPC==0 || IdPC==1) && FormInOpen==false){

     // --- Ciclo gestione SERVER ---
     err = RemGestioneServer( all_mess );
     if( err ) P.error_trasmit( 0, all_mess);

     // --- Ciclo Simulazione ---
     if(AbilitaSimulazione) P.CicloSimulazioneAGV();

     // --- Ciclo Semi-Automatico ---
     err = P.SemiAutomatico( all_mess );       // semi automatico
     if( err ) P.error_trasmit( 0, all_mess);


     // --- Ciclo Automatico ---
     err = P.RunAutomatico( all_mess );        // run automatico
     if( err ) P.error_trasmit( 0, all_mess);

     if(break_Com[COM1]==false && CountCicliLogTgv>5 && LogTgvAttivato==false){
        AggiornaLogTgv();                               // LOGTGV (TELECAMERINA)
        CountCicliLogTgv=0;
     }
     CountCicliLogTgv++;
  }

  //---------------------------------------
  // Gestione conteggi a tempo
  //---------------------------------------
  if(FormLayout->Active || FormStato->Active){
     if(MinutoCiclo && LivelloPassword>LivelloMinPassword) CountMinPassword++;
  }

  //---------------------------------------
  // Gestione CLIENT
  //---------------------------------------
  if( IdPC>1 ){
     err = RemGestioneClient( all_mess );
     if( err ) P.error_trasmit( 0, all_mess);
  }

  //---------------------------------------
  // Attivazioni a tempo
  //---------------------------------------
  // statistiche TGV
  if(MinutoCiclo) CountMinStatTgv++;
  if(CountMinStatTgv>2){
     TimTgvAggiornaDB();
     CountMinStatTgv=0;
  }
  // Password (scaedenza ogni 5 min.)
  if(PcAlgo==false && AbilitaSimulazione==false && TestInternoTecnoFerrari==false && CountMinPassword>5 && LivelloPassword>LivelloMinPassword){
     CountMinPassword=0;
     LivelloPassword=LivelloMinPassword;
     FormLayout->VisualizzaZoneMapping=false;
     FormLayout->VisualizzaZoneImpianto=false;
     MZoneMapping->Checked=false;
     MZoneImpianto->Checked=false;
     StatoPassword();
  }

  //---------------------------------------
  // Azzeramento conteggi
  //---------------------------------------
  if(CountMinStatTgv>99   ) CountMinStatTgv=0;
  if(CountMinPassword>99  ) CountMinPassword=0;
  if(CountCicliLogTgv>99  ) CountCicliLogTgv=0;
  if(CountMinConnSQL>99   ) CountMinConnSQL=0;

  //---------------------------------------
  // Azzeramento flag di gestione minuto
  //---------------------------------------
  if(MinutoCiclo){
     Minuto=false;
     MinutoCiclo=false;
  }

  //----------------------------------------------------------
  // START DEL THREAD
  //
  // La priorità può essere impostata a di diversi livelli
  // se impostata alta si rallenta l'interfaccia operatore
  // se bassa si rallenta il ciclo automatico.
  //
  // t1->Priority = tpNormal;
  // t1->Priority = tpLower;
  //
  // Eventualmente si può impostare una priorità alta quando
  // non ci sono editor aperti e bassa in caso contrario.
  //----------------------------------------------------------
//t1->Priority = tpLower;   // set the priority lower than normal
//if(t1->Suspended){
//   t1->Resume();          // now start the thread running
//}

  //---------------------------------------
  // Attivazione Timer
  //---------------------------------------
  TimerCiclo->Enabled = true;      // --- Disabilitazione Timer Principale

  //-----------------------------------------------------
  // Azzero il flag di prima scansione software attiva
  //-----------------------------------------------------
  PrimaScansione=false;

}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerMinTimer(TObject *Sender)
{
  Minuto=true;
  LastTimeMinutoCiclo=time(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerRefreshTimer(TObject *Sender)
{
  int  i, Stato;
  char buffer[101]="";
  short int almeno_un_tgv;
  short int almeno_un_plc;
  bool errore_io_remoti;
  bool com_tgv_dis, com_tgv_err;
  static int RitardoRefresh=0;
  static AnsiString Buff = "Loading Software ";

  // --- disattivazione timer per ricorsività ---
  TimerRefresh->Enabled  = false;
  TimerRefresh->Interval = 100;
  //------------------------------------------
  // Test percorsi TecnoFerrari
  //------------------------------------------
  if(FormLayout->TestPercorsiTecnoFerrari==true){
     TimerRefresh->Interval = 300;
  }
  //------------------------------------------
  // ritardo alla visualizzazione del layout
  //------------------------------------------
  if(RitardoRefresh<20){
     FormLayout->LabelMain->Visible = true;
     TimerRefresh->Interval = 250;
     Buff = Buff + ".";
     FormLayout->LabelMain->Caption = Buff;
     RitardoRefresh++;

     FormLayout->CGauge1->Progress=RitardoRefresh*5;
     FormLayout->RaceTGV->Left = (FormLayout->CGauge1->Width/100) * FormLayout->CGauge1->Progress + FormLayout->CGauge1->Left;
     FormLayout->RaceTGV->Top = FormLayout->CGauge1->Top;// - FormLayout->RaceTGV->Height;
  }
  else{
     //------------------------------------------
     // Nascondo la LABEL di caricamento software
     //------------------------------------------
     FormLayout->LabelMain->Visible = false;
     FormLayout->GaugePanel->Visible= false;
     //------------------------------------------
     // Consifero la pagina completamente aperta
     // solo una volta cominciato il refresh
     //------------------------------------------
     FormInOpen=false;
     //------------------------------------------
     // Aggiornamento intero sinottico Impianto
     //------------------------------------------
     FormLayout->AggiornaSinottico();
  }
  //------------------------------------------
  // Aggiornamento pagina di stato Impianto
  //------------------------------------------
  FormStato->VisualizzaStatoTGV();
  //-------------------------------------------
  // Aggiornamento barra di stato
  //-------------------------------------------
  Stato=0;
  if(Automatico==true && !FormLayout->TestPercorsiTecnoFerrari && !LogTgvAttivato) Stato=1;
  if(Automatico==false || FormLayout->TestPercorsiTecnoFerrari || LogTgvAttivato ) Stato=2;
  if(BackupInCorso                 ) Stato=3;
  if(FormTgv->EditorAgvAttivo ||
     EditorMagazzinoAttivo ||
     EditorMacchineAttivo ||
     EditorBaieAttivo              ) Stato=4;
  if(ModificaCoordinateNodi==true  ) Stato=5;
  if(IncendioInCorso               ) Stato=6;
  if(AbilitaSimulazione==true      ) Stato=7;
  //---------------------------------------------
  // Verifico flag di riattivazione mapping
  //---------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(RiattivaMapping[i]==false) continue;
     Stato=8;
     break;
  }

  // --- Password;
  sprintf(buffer, "Password:%d", LivelloPassword);
  FormMain->SBarImpianto->Panels->Items[8]->Text = buffer;
  // --- Stato sistema;
  switch(Stato){
     case  1: if(IdPC<2) FormMain->SBarImpianto->Panels->Items[9]->Text = "AUTOMATIC ON";
              else       FormMain->SBarImpianto->Panels->Items[9]->Text = "Server connection ON";
              break;
     case  2: if(IdPC<2) FormMain->SBarImpianto->Panels->Items[9]->Text = "AUTOMATIC OFF";
              else       FormMain->SBarImpianto->Panels->Items[9]->Text = "Server connection OFF";
              break;
     case  3: FormMain->SBarImpianto->Panels->Items[9]->Text = "Backup in process..."; break;
     case  4: FormMain->SBarImpianto->Panels->Items[9]->Text = "Editor is opened..."; break;
     case  5: FormMain->SBarImpianto->Panels->Items[9]->Text = "Modify X,Y Node..."; break;
     case  6: FormMain->SBarImpianto->Panels->Items[9]->Text = "   INCENDIO   "; break;
     case  7: FormMain->SBarImpianto->Panels->Items[9]->Text = "SIMULAZIONE ATTIVA"; break;
     case  8: FormMain->SBarImpianto->Panels->Items[9]->Text = "Mapping..."; break;
     default: FormMain->SBarImpianto->Panels->Items[9]->Text = "";
  }

  //-----------------------------------------
  // Refresh stato TGV
  //-----------------------------------------
  com_tgv_dis=false;
  if(break_Com[COM1]==true) com_tgv_dis=true;
  almeno_un_tgv=false;
  for(i=1; i<=MAXAGV; i++){
     if(AGV[i]->mission.EsclPLC==true) continue;
     almeno_un_tgv=true;
     break;
  }
  if(almeno_un_tgv==false ) com_tgv_dis=true;
  //-----------------------------------------
  // verifico se c'è almeno un TGV in errore
  //-----------------------------------------
  com_tgv_err=false;
  almeno_un_tgv=false;
  for(i=1; i<=MAXAGV; i++){
     if(almeno_un_tgv) break;
     if(errore_comunicazione[COM1][i] && !AGV[i]->mission.ErrCom) almeno_un_tgv=true;
   //if(!errore_comunicazione[COM1][i] || AGV[i]->mission.ErrCom) col = clLime;
     if(AGV[i]->allarme_interno==ALL_INT_ERR_COMUNIC            ) almeno_un_tgv=true;  // errore com persistente.
   }
   if(almeno_un_tgv==true ) com_tgv_err=true;
   //-----------------------------------------
   // visualizzazione dello stato TGV
   //-----------------------------------------
   if(com_tgv_dis) SBarImpianto->Panels->Items[1]->Text  = "OFF";
   else{
     if(com_tgv_err==false) SBarImpianto->Panels->Items[1]->Text  = "TGV";
     else                   SBarImpianto->Panels->Items[1]->Text  = "ERR1";
  }

  //-----------------------------------
  // Refresh stato PLC TERRA
  //-----------------------------------
  if( break_Com[COM2]) SBarImpianto->Panels->Items[2]->Text  = "OFF";
  if(!break_Com[COM2]){
     almeno_un_plc=false;
     for(i=0; i<=MAXPLCCON; i++){
        if(almeno_un_plc                 ) break;
        if(!errore_comunicazione[COM2][i]) continue;
        almeno_un_plc=true;
        break;
     }
     if(almeno_un_plc){
        SBarImpianto->Panels->Items[2]->Text  = "ERR2";
     }
     else{
        SBarImpianto->Panels->Items[2]->Text  = "PLC";
     }
  }
  //------------------------------------------
  // Stato COMUNICAZIONE I/O REMOTATI
  //------------------------------------------
  if(break_Com[COM6]) SBarImpianto->Panels->Items[3]->Text = "OFF";
  else{
     errore_io_remoti=false;
     if( errore_comunicazione[COM6][PLC_IO_REMOTO_01] ) errore_io_remoti = true;
     if( errore_comunicazione[COM6][PLC_IO_REMOTO_02] ) errore_io_remoti = true;
     if( errore_comunicazione[COM6][PLC_IO_REMOTO_03] ) errore_io_remoti = true;
     if( errore_comunicazione[COM6][PLC_IO_REMOTO_04] ) errore_io_remoti = true;
     if( errore_comunicazione[COM6][PLC_IO_REMOTO_05] ) errore_io_remoti = true;
     if( errore_comunicazione[COM6][PLC_IO_REMOTO_06] ) errore_io_remoti = true;
     if( errore_comunicazione[COM6][PLC_IO_REMOTO_07] ) errore_io_remoti = true;
     if( errore_comunicazione[COM6][PLC_IO_REMOTO_08] ) errore_io_remoti = true;
     if( errore_comunicazione[COM6][PLC_IO_REMOTO_09] ) errore_io_remoti = true;
     if( errore_comunicazione[COM6][PLC_IO_REMOTO_10] ) errore_io_remoti = true;
     if(errore_io_remoti==true        ) SBarImpianto->Panels->Items[3]->Text = "ERR";
     else                               SBarImpianto->Panels->Items[3]->Text = "  I/O";
  }
  //------------------------------------------
  // STATO CONNESSIONE SQL SERVER
  //------------------------------------------
  if( break_Com[COM3]) SBarImpianto->Panels->Items[4]->Text  = "OFF";
  if(!break_Com[COM3]){
     switch(errore_comunicazione[COM3][DB_DBASE]){
        case  ERR_SQL_NONE: // nessun errore
           SBarImpianto->Panels->Items[4]->Text  = "CONNECTION SQL ON";     // connessione on
           break;
        case  ERR_SQL_CONNECTION: // impossibile connession a SQL
           SBarImpianto->Panels->Items[4]->Text  = "ERR IN CONNECTION";
           break;
        case  ERR_SQL_RD_TAB:
           SBarImpianto->Panels->Items[4]->Text  = "err in load Tab";
           break;
        case  ERR_SQL_WR_TAB:
           SBarImpianto->Panels->Items[4]->Text  = "err in write Tab";
           break;
        default:
           SBarImpianto->Panels->Items[4]->Text  = "unknow err SQL";
           break;
     }
  }

  // --- attivazione timer per ricorsività ---
  TimerRefresh->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerLampeggioTimer(TObject *Sender)
{
  Lampeggio=!Lampeggio;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MMessaggeriaClick(TObject *Sender)
{
  FormMessaggi->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MPasswordClick(TObject *Sender)
{
  short int i;
  long int Tempo;
  char buffer[101]="";
  char Stringa[81]="";
  char Date[11]="";
  char Time[11]="";
  char g[3]="";
  char m[3]="";
  char a[7]="";
  bool psw_sconosciuta;

  //-------------------------------------------------
  // Apertura della form
  //-------------------------------------------------
  FormPassword->ShowModal();
  //-------------------------------------------------
  // Se è stato premuto il pulsante "OK" verifico
  // la validità della passoword
  //-------------------------------------------------
  if(FormPassword->ModalResult==mrOk){
     Tempo=time(NULL);
     strcpy(Stringa, PASS_LEVEL3);
     timeint_to_timechar(&Tempo, &Date[0], &Time[0]);
     memcpy(&g[0], &Date[0], 2);   // giorno
     g[2]=0;
     memcpy(&m[0], &Date[3], 2);   // mese
     m[2]=0;
     memcpy(&a[0], &Date[6], 4);   // anno
     a[4]=0;
     Tempo = (long int)(atoi(g) + atoi(m) + atoi(a));
     //-------------------------------------------------
     // Assegnazione livello minimo di password
     // consentita sul PC
     //-------------------------------------------------
     NrOperatore     = 0;
     LivelloPassword = LivelloMinPassword;
     psw_sconosciuta = true;
     //-------------------------------------------------
     // Se la PASS_LEVEL3 non è stata impostata viene
     // calcolata come somma di gg+mm+aaaa
     //-------------------------------------------------
     for(i=1; i<11; i++){
        strcpy(Stringa, Pass3[i].c_str());
        if(strcmp(Stringa, "")==0) sprintf(Stringa, "%ld", Tempo);
        //-------------------------------------------------
        // Attivazione Password al LIVELLO 0
        //-------------------------------------------------
        if(FormPassword->Password==Pass0){
           NrOperatore     = 0;
           LivelloPassword = 0;
           psw_sconosciuta = false;
           break;
        }
        //-------------------------------------------------
        // Attivazione Password al LIVELLO 1
        //-------------------------------------------------
        if(FormPassword->Password==Pass1){
           NrOperatore     = 0;
           LivelloPassword = 1;
           psw_sconosciuta = false;
           break;
        }
        //-------------------------------------------------
        // Attivazione Password al LIVELLO 2
        //-------------------------------------------------
        if(FormPassword->Password==Pass2[i]){
           NrOperatore     = i;
           LivelloPassword = 2;
           psw_sconosciuta = false;
           break;
        }
        //-------------------------------------------------
        // Attivazione Password al LIVELLO 3
        //-------------------------------------------------
        if(FormPassword->Password==Stringa){
           NrOperatore     = i;
           LivelloPassword = 3;
           psw_sconosciuta = false;
           break;
        }
     }

     //-------------------------------------------------
     // Livello non consentito sul PC
     //-------------------------------------------------
     if(LivelloPassword > LivelloMaxPassword ){
        sprintf(buffer, "Level password %d not ammitted on this PC...", LivelloPassword);
        Application->MessageBox(buffer, MESS[100], MB_OK|MB_ICONEXCLAMATION );
        LivelloPassword = LivelloMaxPassword;
     }

     //-------------------------------------------------
     // La password inserita non è corretta
     //-------------------------------------------------
     if(psw_sconosciuta==true){
        Application->MessageBox(MESS[105], MESS[100], MB_OK|MB_ICONEXCLAMATION );
     }
     //-------------------------------------------------
     // Attivazione blocchi
     //-------------------------------------------------
     StatoPassword();
     //-------------------------------------------------
     // Traccia su LOG OPERATORE
     //-------------------------------------------------
     sprintf(buffer, "(PASSWOR) - Modifica Password a livello %d", LivelloPassword);
     FileLogOperatore(buffer);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MTestPlc1Click(TObject *Sender)
{
  // --- Apertura della Form ---
  if(LivelloPassword<3) return;
  FormTestPlc->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MTestPlc2Click(TObject *Sender)
{
  // --- Apertura della Form ---
  if(LivelloPassword<3) return;
  FormTestPlc2->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MTestPlc3Click(TObject *Sender)
{
  // --- Apertura della Form ---
  if(LivelloPassword<1) return;
  FormTestPlc2->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MErroriDiSistemaClick(TObject *Sender)
{
  char NomeFile[100]="LogTrsErr.dat";
  char NomeFile2[100]="TRSERR.DAT";


  /**********************************************************************************
  // per ripristinare la vecchia gestione scommentare le rigue seguenti e commentare tutte le altre
  //-------------------------------------------------------------------------------------------------
  char NomeFile1[200];
  char NomeFile2[200];
  //-----------------------------------------
  //  Recupera file da SERVER
  //-----------------------------------------
  if( IdPC>1 ){
     sprintf( NomeFile1, "%s\\TRSERR.DAT", DriveServer );
     sprintf( NomeFile2, "TRSERR.DAT" );
     CopyFile( NomeFile1, NomeFile2, FALSE );
  }
  strcpy(FormListagen->NomeFileLista, "TRSERR.DAT");
  strcpy(FormListagen->CaptionFileLista, "Errori di sistema");
  FormListagen->TipoFileLista=0;
  //--------------------------------------------------
  // Se viene mosso il Mouse  all'interno della Form
  // azzero la variabile di Reset del software.
  //--------------------------------------------------
  FormListagen->ShowModal();
  **********************************************************************************/
  //-----------------------------------------
  //  Recupera file da SERVER
  //-----------------------------------------
  if( IdPC>1 ){
     sprintf( NomeFile2, "%s\\%s", DriveServer, NomeFile );
     CopyFile( NomeFile2, NomeFile, FALSE );
  }
  //------------------------------------------------------------
  // VISUALIZZAZIONE LISTGEN
  //------------------------------------------------------------
  //------------------------------------------------------------
  // se il file non esiste ERRORE
  //------------------------------------------------------------
  if(!FileExists(NomeFile)){
     Application->MessageBox("File non esistente", MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }
  //-----------------------------------------------------
  // Settaggio variabili per visualizzazione File
  //-----------------------------------------------------
  strcpy(FormListagen->NomeFileLista,  NomeFile);
  strcpy(FormListagen->NomeFileAzzera, NomeFile);
  strcpy(FormListagen->CaptionFileLista,"Errori di sistema");
  FormListagen->CarSeparatore=';'; // Carattere separatore campi nel file
  FormListagen->ColNum=4;          // numero di colonne totali
  //------------------------------------------------------------
  // Dimensioni ed Intestazioni delle colonne
  // sarebbe possibile impostarle a -1 per fare l'autosize,
  // ma poi la funzione di stampa non formatta bene il testo!
  //------------------------------------------------------------
  memset(&FormListagen->CaptionColumn,0,sizeof(FormListagen->CaptionColumn));
  FormListagen->ColSize[ 0]=130;   strcpy(FormListagen->CaptionColumn[ 0], "Date and Time");
  FormListagen->ColSize[ 1]= 50;   strcpy(FormListagen->CaptionColumn[ 1], "TGV");
  FormListagen->ColSize[ 2]= 50;   strcpy(FormListagen->CaptionColumn[ 2], "Pos");
  FormListagen->ColSize[ 3]=450;   strcpy(FormListagen->CaptionColumn[ 3], "Message");
  //------------------------------------------------------------
  FormListagen->TipoFileLista=5;
  FormListagen->btnExportExcel->Enabled  = true;
  FormListagen->btnSaveAsCSV->Enabled    = true;
  FormListagen->btnAzzera->Enabled       = false;

  FormListagen->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MCOMClick(TObject *Sender)
{
  int  i, err, Port=0;
  char all_mess[101]="";

  if(LivelloPassword<3 ) return;
  Port = (((TMenuItem *)Sender)->Tag);

  //--------------------------------------------------------
  // Aggiornamento grafico
  //--------------------------------------------------------
  switch(Port){
     case  COM1: break_Com[Port]=!break_Com[Port];
                 if(!break_Com[Port]){MCOM1->Checked=true;}
                 else                {MCOM1->Checked=false;}
                 break;
     case  COM2: break_Com[Port]=!break_Com[Port];
                 if(!break_Com[Port]){MCOM2->Checked=true;}
                 else                {MCOM2->Checked=false;}
                 break;
     case  COM3: if(GestioneUdc==0) MCOM3->Enabled=false;
                 else{
                    break_Com[Port]=!break_Com[Port];
                    if(!break_Com[Port]){MCOM3->Checked=true;}
                    else                {MCOM3->Checked=false;}
                    //--------------------------------------------------
                    // Gestione Thread SQL
                    //--------------------------------------------------
                    if(SecReloadOnThreadSQL!=0){
                       FormSQLDB->t_SQL->Priority = tpLower;   // set the priority lower than normal
                       if(break_Com[Port]==false){
                          if(FormSQLDB->t_SQL->Suspended==true){
                             //-----------------------------------------------------------
                             // START DEL THREAD
                             //
                             // La priorità può essere impostata a di diversi livelli
                             // se impostata alta si rallenta l'interfaccia operatore
                             // se bassa si rallenta il ciclo automatico.
                             //
                             // t1->Priority = tpNormal;
                             // t1->Priority = tpLower;
                             //
                             // Eventualmente si può impostare una priorità alta quando
                             // non ci sono editor aperti e bassa in caso contrario.
                             //-----------------------------------------------------------
                             FormSQLDB->t_SQL->Resume();          // now start the thread running
                          }
                       }
                       else{
                          if(FormSQLDB->t_SQL->Suspended==false){
                             FormSQLDB->t_SQL->Suspend();
                          }
                       }
                    }                       
                 }
                 break;
     case  COM4: break_Com[Port]=!break_Com[Port];
                 if(!break_Com[Port]){MCOM4->Checked=true;}
                 else                {MCOM4->Checked=false;}
                 break;
     case  COM5: break_Com[Port]=!break_Com[Port];
                 if(!break_Com[Port]){MCOM5->Checked=true;}
                 else                {MCOM5->Checked=false;}
                 break;
     case  COM6: break_Com[Port]=!break_Com[Port];
                 if(!break_Com[Port]){MCOM6->Checked=true;}
                 else                {MCOM6->Checked=false;}
                 break;
     case  COM7: break_Com[Port]=!break_Com[Port];
                 if(!break_Com[Port]){MCOM7->Checked=true;}
                 else                {MCOM7->Checked=false;}
                 break;
     case  COM8: break_Com[Port]=!break_Com[Port];
                 if(!break_Com[Port]){MCOM8->Checked=true;}
                 else                {MCOM8->Checked=false;}
                 break;
     case  COM9: //--------------------------------------------------
                 // Apertura form di interfaccia e parametrizzazione
                 //--------------------------------------------------
                 FormSrvTcpAndroid->ShowModal();
                 if(FormSrvTcpAndroid->ModalResult!=mrCancel){
                    if(FormSrvTcpAndroid->ModalResult==mrOk) {break_Com[Port]=false; FormSrvTcpAndroid->StartServerTCPAndroid();}
                    else                                     {break_Com[Port]=true;  FormSrvTcpAndroid->StopServerTCPAndroid(0);}
                 }
                 // visualizzazione stato comunicazione
                 if(!break_Com[Port]){MCOM9->Checked=true;}
                 else                {MCOM9->Checked=false;}
                 break;
     default:    break;
  }
  //----------------------------------------
  // Traccia su LOG OPERATORE
  //----------------------------------------
  sprintf(all_mess, "(COM%02d  ) - Abilita/Disabilita comunicazione", Port+1);
  FileLogOperatore(all_mess);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MClientServerClick(TObject *Sender)
{
  int  Id, pos;
  char buffer[101]="";
  char NomeFile[201]="";
  struct ffblk StrFile;
  AnsiString Path;

  //--------------------------------------------------------
  // Accesso ammesso solo ai tecnici TecnoFerrari
  //--------------------------------------------------------
  if(LivelloPassword<3) return;
  //--------------------------------------------------------
  // Dasattivazione del timer di sistema
  //--------------------------------------------------------
  TimerCiclo->Enabled=false;
  //--------------------------------------------------------
  // Parametrizzazione della Form di ricera Path
  //--------------------------------------------------------
  OpenDialogServer->Filter      = "Pc Server (Identifi.dat)|Identifi.dat";
  OpenDialogServer->FileName    = "";
  OpenDialogServer->FilterIndex = 1;
  OpenDialogServer->InitialDir  = DriveServer;
  if(OpenDialogServer->Execute()==false         ){TimerCiclo->Enabled=true; return;}
  if(OpenDialogServer->FileName.IsEmpty()==true ){TimerCiclo->Enabled=true; return;}
  //---------------------------------------------------------
  // Recupero del path di rete del PC Server
  //---------------------------------------------------------
  strcpy(NomeFile, OpenDialogServer->FileName.c_str());
  if(findfirst(NomeFile, &StrFile, 0)){TimerCiclo->Enabled=true; return;}
  //---------------------------------------------------------
  // Verifico che il PC selezionato sia un Pc Server
  //---------------------------------------------------------
  if(RemLoadIdentificativoPC(NomeFile, &Id, buffer)){
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     TimerCiclo->Enabled=true;
     return;
  }
  if(Id>1){
     Application->MessageBox("Connect to PC Client...", MESS[99], MB_OK | MB_ICONINFORMATION);
     TimerCiclo->Enabled=true;
     return; //pc client
  }
  //---------------------------------------------------------
  // Riporto tutto a caratteri maiuscoli
  //---------------------------------------------------------
  StrUpper(StrFile.ff_name);
  OpenDialogServer->FileName=AnsiUpperCase(OpenDialogServer->FileName);
  //---------------------------------------------------------
  // Assegnazione del path
  //---------------------------------------------------------
  pos = OpenDialogServer->FileName.Pos(StrFile.ff_name);
  if(pos<=0){
     Application->MessageBox("Error in PC Server file name...", MESS[99], MB_OK | MB_ICONINFORMATION);
     TimerCiclo->Enabled=true;
     return; //pc client
  }
  Path = OpenDialogServer->FileName.SubString(0, pos-2);
  strcpy(DriveServer, Path.c_str());
  //---------------------------------------------------------
  // Reinizializzazione dei nomi file condivisi
  //---------------------------------------------------------
  RemPrimaScansione=true;
  if(RemGestioneClient(buffer)) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  //---------------------------------------------------------
  // Traccia su LOG OPERATORE
  //---------------------------------------------------------
  FileLogOperatore("(TGVSPEC) - Modifica Path di rete del PC Server");
  ShowMessage("Lan connection with PC Server is ok...");
  TimerCiclo->Enabled=true;
  return;
}
void __fastcall TFormMain::OPCClientconnectionClick(TObject *Sender)
{
  int  Id, pos;
  char buffer[101]="";
  char NomeFile[201]="";
  struct ffblk StrFile;
  AnsiString Path;

  //--------------------------------------------------------
  // Accesso ammesso solo ai tecnici TecnoFerrari
  //--------------------------------------------------------
  if(LivelloPassword<3) return;
  //--------------------------------------------------------
  // Dasattivazione del timer di sistema
  //--------------------------------------------------------
  TimerCiclo->Enabled=false;
  //--------------------------------------------------------
  // Parametrizzazione della Form di ricera Path
  //--------------------------------------------------------
  //OpenDialogServer->Filter      = "Pc Server (Identifi.dat)|Identifi.dat";
  OpenDialogServer->Filter      = "Opc Client(OPC.CFG)|OpcClient.exe";
  OpenDialogServer->FileName    = "";
  OpenDialogServer->FilterIndex = 1;
  //  OpenDialogServer->InitialDir  = DriveServer;
  OpenDialogServer->InitialDir  = PathGateway;
  if(OpenDialogServer->Execute()==false         ){TimerCiclo->Enabled=true; return;}
  if(OpenDialogServer->FileName.IsEmpty()==true ){TimerCiclo->Enabled=true; return;}
  //---------------------------------------------------------
  // Recupero del path di rete del Opc Client
  //---------------------------------------------------------
  strcpy(NomeFile, OpenDialogServer->FileName.c_str());
  if(findfirst(NomeFile, &StrFile, 0)){TimerCiclo->Enabled=true; return;}
  //---------------------------------------------------------
  // Verifico che il PC selezionato sia un Pc Server
  //---------------------------------------------------------
  if(RemLoadIdentificativoPC(NomeFile, &Id, buffer)){
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     TimerCiclo->Enabled=true;
     return;
  }
  if(Id>1){
     Application->MessageBox("Connect to PC Client...", MESS[99], MB_OK | MB_ICONINFORMATION);
     TimerCiclo->Enabled=true;
     return; //pc client
  }
  //---------------------------------------------------------
  // Riporto tutto a caratteri maiuscoli
  //---------------------------------------------------------
  StrUpper(StrFile.ff_name);
  OpenDialogServer->FileName=AnsiUpperCase(OpenDialogServer->FileName);
  //---------------------------------------------------------
  // Assegnazione del path
  //---------------------------------------------------------
  pos = OpenDialogServer->FileName.Pos(StrFile.ff_name);
  if(pos<=0){
     Application->MessageBox("Error in Path Gateway ...", "ATTENTION", MB_OK | MB_ICONINFORMATION);
     return; //pc client
  }
  Path = OpenDialogServer->FileName.SubString(0, pos-2);
  //  strcpy(DriveServer, Path.c_str());
  PathGateway = Path;
  //---------------------------------------------------------
  // Traccia su LOG OPERATORE
  //---------------------------------------------------------
  FileLogOperatore("(TGVSPEC) - Modifica Path di rete dell'OPC Client");
  ShowMessage("Lan connection with OPC Client is ok...");

  //---------------------------------------------------------
  // Salvataggio del nuovo PATH GATEWAY
  //---------------------------------------------------------
  AllenB_WriteIniProject();
  AllenB_Percorso();

  TimerCiclo->Enabled=true;
  return;

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::OPCClientConfigurationClick(TObject *Sender)
{
  int result;
  char all_mess[300];

  //--------------------------------------------------------
  // Accesso ammesso solo ai tecnici TecnoFerrari
  //--------------------------------------------------------
  if(LivelloPassword<3) return;
  //--------------------------------------------------------
  // Chiedi conferma
  //--------------------------------------------------------
  result = Application->MessageBox( "Modifica files configurazione OPC Client ?", "ATTENZIONE", MB_YESNO | MB_ICONQUESTION);
  if(result != IDYES) return;

  //-------------------------------------------------------------------------------------------
  // Compilazione automatica dei files di configurazione iniaziale della applicazione
  // Opc Client VB6 ( Derivati da Allenb.ini )
  // Serve ricompilarli se Allenb.ini è stato modificato per es.: Nomi Plc, aree dati ecc.
  //-------------------------------------------------------------------------------------------
  AllenB_InizializzaFileConfig( all_mess );
  ShowMessage("Eventuali impostazioni saranno attive dopo avere riavviato l'applicazione OPC CLIENT...");
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TFormMain::MEditorComunicationClick(TObject *Sender)
{
  FormServerUDP->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MBackupClick(TObject *Sender)
{
  int  err=0;
  int  rslt=0;
  char newname[101]="";
  char buffer[1001]="";
  char NomeFile[151]="";
  short int i=0;
  AnsiString InputStringa;
  WORD dm[21];

  memset(&dm, 0, sizeof(dm));
  memset(&buffer[0], NULL, sizeof(buffer));
  //-----------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------
  FormInput->lblInputString->Caption = "Operator Backup";
  if(IdPC == 1) FormInput->Check1->Caption = "Production Backup";
  else FormInput->Check1->Caption = "";
  FormInput->txtEdit->Text="";

  FormInput->Tipo=4;
  rslt=FormInput->ShowModal();

  if(rslt==mrOk){
     if( IdPC==1 ){
        for(i=0; i<=900; i++){
           sprintf(NomeFile, "BAK%03d.zip", i);
           if(access(NomeFile, 0)!=0) break;
        }
     }
     else{
        sprintf(NomeFile,"bak da client %d",IdPC);
     }

     InputStringa = FormInput->txtEdit->Text;
     if(InputStringa.IsEmpty() == true){
        sprintf(buffer, "(BACKUP ) - %s: Operator Backup", NomeFile);
     }
     else{
        sprintf(buffer, "(BACKUP ) - %s: %s", NomeFile, InputStringa.c_str());
     }
     buffer[160]=NULL;  // sicurezza anti-sforamento
     FileLogOperatore(buffer);
     //-------------------------------------------------
     // Generazione file con FUSO ORARIO LOCALE
     //-------------------------------------------------
     RemSaveTimeZone( buffer );
     //-------------------------------------------------
     //  Trasmetti comando al SERVER
     //-------------------------------------------------
     if( IdPC>1 ){
        strcpy(newname, "Backup");
        err = RemCreaFileComando (newname, NULL, NULL, buffer );
        if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
        ShowMessage("Backup command transfered to the PC SERVER...");
        return;
     }

     //-------------------------------------------------------------------------
     // Creazione file di backup
     //-------------------------------------------------------------------------
     if(IdPC == 1 && FormInput->Check1->State == cbChecked ){
        RemSalvaAnomalie(true);    // true = salva la cartella produzio
     }
     else RemSalvaAnomalie(false); // false = NON salva la cartella produzio
     //-------------------------------------------------------------------------
     // Cancellazione dei file temporanei
     //-------------------------------------------------------------------------
     for(i=1; i<=MAXAGV; i++){
        sprintf(newname, "PROG%03d.DAT", i);
        unlink(newname);
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BtnLogOpClick(TObject *Sender)
{
  char NomeFile1[200];
  char NomeFile2[200];
  //----------------------------------------------
  // Accesso ammesso solo ai tecnici TecnoFerrari
  //----------------------------------------------
  //if(LivelloPassword<3) return;
  //----------------------------------------------
  //  Recupera file da SERVER
  //----------------------------------------------
  if( IdPC>1 ){
     sprintf( NomeFile1, "%s\\LOGOP.DAT", DriveServer );
     sprintf( NomeFile2, "LOGOP.DAT" );
     CopyFile( NomeFile1, NomeFile2, FALSE );
  }
  if(LivelloPassword<3){
     FormListagen->btnAzzera->Enabled=false;
     FormListagen->btnStampa->Enabled=false;
  }
  FormListagen->TipoFileLista=2;
  strcpy(FormListagen->NomeFileLista,    "LOGOP.DAT");
  strcpy(FormListagen->CaptionFileLista, "Log Operator");
  FormListagen->ShowModal();
  //---------------------------------------
  // riabilita pulsanti disabilitati
  //---------------------------------------
  FormListagen->btnAzzera->Enabled=true;
  FormListagen->btnStampa->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MSimulazioneClick(TObject *Sender)
{
  if(LivelloPassword<3) return;
  AbilitaSimulazione=!AbilitaSimulazione;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MTestPercorsiClick(TObject *Sender)
{
  if(LivelloPassword<3) return;
  FormTest->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MZoneImpiantoClick(TObject *Sender)
{
  if(LivelloPassword<3) return;
  FormLayout->VisualizzaZoneImpianto=!FormLayout->VisualizzaZoneImpianto;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MZoneMappingClick(TObject *Sender)
{
  if(LivelloPassword<3) return;
  FormLayout->VisualizzaZoneMapping=!FormLayout->VisualizzaZoneMapping;

  MZoneMapping->Checked=FormLayout->VisualizzaZoneMapping;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MLogTgvClick(TObject *Sender)
{
  int  err, i;
  char Ora[11]="";
  char Data[11]="";
  char buffer[101]="";
  long int TimeLog;

  //---------------------------------------------------
  // Da attivare solo con password 3 comunicazione OFF
  //---------------------------------------------------
  if(LivelloPassword<3 || !break_Com[COM1] || !break_Com[COM2] || !break_Com[COM3]) return;
  //---------------------------------------------------
  // Disattivazione dell'opzione visualizzazione della
  // situazione dei TGV.
  //---------------------------------------------------
  if(LogTgvAttivato==true){
     LogTgvAttivato=false;
     //---------------------------------------
     // Azzeramento grafico
     //---------------------------------------
     FormStato->UpDownScroll->Position=1;
     FormStato->PanelScroll->Visible=false;
     //---------------------------------------
     // Azzeramento delle missioni dei TGV
     //---------------------------------------
     for(i=1; i<=MAXAGV; i++){
        AGV[i]->reset_ram_mission();
        AGV[i]->reset_chiamata_AGV();
        AGV[i]->free_all_nodi(buffer);
     }
     //--------------------------------------------------
     // visualizzo sulla voce di menù la selezione
     //--------------------------------------------------
     MLogTgv->Checked = LogTgvAttivato;
     return;
  }
  //--------------------------------------------------
  // Password NON OK
  //--------------------------------------------------
  if(LivelloPassword<3){
     ShowMessage("Password not OK!!!");
     return;
  }
  //--------------------------------------------------
  // Verifica data del file di LOG TGV
  //--------------------------------------------------
  err = LoadLogTgv(1, &TimeLog, buffer);
  if(err ){
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }
  //--------------------------------------------------
  // Apertura della Form
  //--------------------------------------------------
  LogTgvAttivato        = true;
  AbilitaSimulazione    = false;     // disattivazione della simulazione
  FormStato->PanelScroll->Visible  = true;
  FormStato->LabelScroll2->Color   = clLime;
  FormStato->LabelScroll2->Caption = "Indice: 1 - IN CORSO...";
  timeint_to_timechar(&TimeLog, &Data[0], &Ora[0]);
  sprintf(buffer, "%s - %s", Data, Ora);
  FormStato->LabelScroll1->Caption = buffer;
  //--------------------------------------------------
  // visualizzo sulla voce di menù la selezione
  //--------------------------------------------------
  MLogTgv->Checked = LogTgvAttivato;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MSfondoGraficoClick(TObject *Sender)
{
// riattiva SfondoGrafico
/*
  char all_mess[101]="";
  AnsiString Percorso;
  struct IMPIANTO Dati;

  memset(&Dati, 0, sizeof(IMPIANTO));
  FormLayout->AbilitaSfondoGrafico=!FormLayout->AbilitaSfondoGrafico;
  if(FormLayout->AbilitaSfondoGrafico==true){
     Percorso = ExtractFilePath(ParamStr(0));
     strcpy(all_mess, Percorso.c_str());
     memcpy(&Dati.Percorso[0], &all_mess[0], 100);
     //---------------------------------------------------------------
     // Connessione all' ACTIVE_X di gestione dello SFONDO GRAFICO
     //---------------------------------------------------------------
     try{
        if(break_Com[COM1] || break_Com[COM2]){
           FormLayout->COMISfondo = CoTSfondoGrafico::Create();
           FormLayout->COMISfondo->DatiImpianto(&Dati);
        }
     }
     catch(...)
     {
        FormLayout->AbilitaSfondoGrafico=false;
        ShowMessage("(ACTIVE X) - Impossibile trovare l'applicazione desiderata...");
        Application->Terminate();
     }
  }
*/  
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MTestPercorsiTecnoClick(TObject *Sender)
{
  short int i;
  char buffer[101]="";

  if(LivelloPassword<3  ) return;
  if(!break_Com[COM1]   ) return;
  //---------------------------------------------------------
  // Il TGV non deve avere carrelli a bordo
  //---------------------------------------------------------
//if(AGV[1]->mission.PresCar!=0 || (AGV[1]->stato.s.bit.car1 || AGV[1]->stato.s.bit.car2 || AGV[1]->stato.s.bit.car3)){
//   ShowMessage("ATTENZIONE: Il TGV1 non deve avere la presenza di carro a bordo");
//}

  //---------------------------------------------------------
  // Gestione Test Percorsi TecnoFerrari
  //---------------------------------------------------------
  FormLayout->TestPercorsiTecnoFerrari=!FormLayout->TestPercorsiTecnoFerrari;
  FormMain->SBarImpianto->Panels->Items[0]->Text = "";
  //---------------------------------------------------------
  // Incaso di Test Percorsi TECNOFERRARI viene eseguito
  // un reset_mission per tutti i TGV.
  //---------------------------------------------------------
  if(FormLayout->TestPercorsiTecnoFerrari==true){
     P.error_trasmit(0, "Reset missioni per test percorsi TecnoFerrari");
     for(i=1; i<=MAXAGV; i++){
        AGV[i]->reset_mission(buffer);
        AGV[i]->reset_chiamata_AGV();
        AGV[i]->stato.start=0;
     }
  }
  //--------------------------------------------------
  // visualizzo sulla voce di menù la selezione
  //--------------------------------------------------
  MTestPercorsiTecno->Checked = FormLayout->TestPercorsiTecnoFerrari;

}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MEditorTgvClick(TObject *Sender)
{
  AnsiString InputStringa;
  int MaxAgv=MAXAGV;
  short int NumTgv=0;
  char Chiave[5]="";

  //-----------------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------------
  if(MaxAgv>1){
     InputStringa = InputBox(MESS[250], MESS[259], Chiave);
     if(InputStringa.IsEmpty() == true) return;
     strcpy(Chiave, InputStringa.c_str());
     NumTgv = (short int) atoi(Chiave);               // Recupero del Numero del TGV
     //-----------------------------------------------------
     // Controllo della validità del valore inserito
     //-----------------------------------------------------
     if(NumTgv<1 || NumTgv>MAXAGV){
        Application->MessageBox(MESS[374], MESS[100], MB_OK | MB_ICONEXCLAMATION);
        return;
     }
  }
  else NumTgv=(short int)(MaxAgv);
  //----------------------------------------------
  // Apertura dell'editor del TGV selezionato
  //----------------------------------------------
  FormTgv->NumeroTgv = NumTgv;
  FormTgv->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MPercorsoTgvClick(TObject *Sender)
{
  AnsiString InputStringa;
  char all_mess[201]="";
  int  err=0;
  int  MaxAgv=MAXAGV;
  short int NumTgv=0;
  char Chiave[5]="";

  //-----------------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------------
  if(MaxAgv>1){
     InputStringa = InputBox(MESS[251], MESS[259], Chiave);
     if(InputStringa.IsEmpty() == true) return;
     strcpy(Chiave, InputStringa.c_str());
     NumTgv = (short int) atoi(Chiave);               // Recupero del Numero del TGV
     //-----------------------------------------------------
     // Controllo della validità del valore inserito
     //-----------------------------------------------------
     if(NumTgv<1 || NumTgv>MAXAGV){
        Application->MessageBox(MESS[374], MESS[100], MB_OK | MB_ICONEXCLAMATION);
        return;
     }
  }
  else NumTgv=(short int)(MaxAgv);
  //-----------------------------------------------------
  // Settaggio variabili per visualizzazione File
  //-----------------------------------------------------
  if(AgvBloccati[NumTgv]==0) sprintf(all_mess, "Path TGV%d", NumTgv);
  else                       sprintf(all_mess, "Path TGV%d (TGV%d in obstruction)", NumTgv, AgvBloccati[NumTgv]);
  strcpy(FormVisualizzaFile->CaptionFileVisualizza, all_mess);
  err=AGV[NumTgv]->CompilaPercorsoGrafico( FormVisualizzaFile->IntestazioneFileVisualizza );
  if(err){
     sprintf(all_mess, MESS[113], FormVisualizzaFile->NomeFileVisualizza);
     Application->MessageBox(all_mess, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  else{
     FormVisualizzaFile->ShowModal();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MProgrammaTgvClick(TObject *Sender)
{
  AnsiString InputStringa;
  char all_mess[201]="";
  int  err=0;
  int  MaxAgv=MAXAGV;
  short int NumTgv=0;
  char Chiave[5]="";

  //-----------------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------------
  if(MaxAgv>1){
     InputStringa = InputBox(MESS[252], MESS[259], Chiave);
     if(InputStringa.IsEmpty() == true) return;
     strcpy(Chiave, InputStringa.c_str());
     NumTgv = (short int) atoi(Chiave);               // Recupero del Numero del TGV
     //-----------------------------------------------------
     // Controllo della validità del valore inserito
     //-----------------------------------------------------
     if(NumTgv<1 || NumTgv>MAXAGV){
        Application->MessageBox(MESS[374], MESS[100], MB_OK | MB_ICONEXCLAMATION);
        return;
     }
  }
  else NumTgv=(short int)(MaxAgv);
  //-----------------------------------------------------
  // Settaggio variabili per visualizzazione File
  //-----------------------------------------------------
  strcpy(FormVisualizzaFile->NomeFileVisualizza, "PROGTGV.DAT");
  strcpy(FormVisualizzaFile->IntestazioneFileVisualizza, "| STEP  |   ISTRUCTION                         |   CODING OF CONTROLS          |");
  sprintf(all_mess, "%s%d - (RAM)", MESS[252], NumTgv);
  strcpy(FormVisualizzaFile->CaptionFileVisualizza, all_mess);
  err=AGV[NumTgv]->CompilaProgrammaGrafico( FALSE );
  if(err){
     sprintf(all_mess, MESS[113], FormVisualizzaFile->NomeFileVisualizza);
     Application->MessageBox(all_mess, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  else{
     FormVisualizzaFile->ShowModal();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MProgrammaPlcTgvClick(TObject *Sender)
{
  AnsiString InputStringa;
  char all_mess[201]="";
  int  err=0;
  int  MaxAgv=MAXAGV;
  short int NumTgv=0;
  char Chiave[5]="";

  //-----------------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------------
  if(MaxAgv>1){
     InputStringa = InputBox(MESS[253], MESS[259], Chiave);
     if(InputStringa.IsEmpty() == true) return;
     strcpy(Chiave, InputStringa.c_str());
     NumTgv = (short int) atoi(Chiave);               // Recupero del Numero del TGV
     //-----------------------------------------------------
     // Controllo della validità del valore inserito
     //-----------------------------------------------------
     if(NumTgv<1 || NumTgv>MAXAGV){
        Application->MessageBox(MESS[374], MESS[100], MB_OK | MB_ICONEXCLAMATION);
        return;
     }
  }
  else NumTgv=(short int)(MaxAgv);
  //-----------------------------------------------------
  // Settaggio variabili per visualizzazione File
  //-----------------------------------------------------
  strcpy(FormVisualizzaFile->NomeFileVisualizza, "PROGTGV.DAT");
  strcpy(FormVisualizzaFile->IntestazioneFileVisualizza, "| PASSO |   ISTRUZIONE                         |   CODIFICA DEI COMANDI        |");
  sprintf(all_mess, "%s%d - (PLC)", MESS[253], NumTgv);
  strcpy(FormVisualizzaFile->CaptionFileVisualizza, all_mess);
  err=AGV[NumTgv]->CompilaProgrammaGrafico( TRUE );
  if(err){
     sprintf(all_mess, MESS[113], FormVisualizzaFile->NomeFileVisualizza);
     Application->MessageBox(all_mess, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  else{
     FormVisualizzaFile->ShowModal();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MAllarmiTgvClick(TObject *Sender)
{
  AnsiString InputStringa;
  char all_mess[201]="";
  int  MaxAgv=MAXAGV;
  short int NumTgv=0;
  char Chiave[5]="";
  char filename[51]="";
  char filehelp[51]="";

  //-----------------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------------
  if(MaxAgv>1){
     InputStringa = InputBox(MESS[254], MESS[259], Chiave);
     if(InputStringa.IsEmpty() == true) return;
     strcpy(Chiave, InputStringa.c_str());
     NumTgv = (short int) atoi(Chiave);               // Recupero del Numero del TGV
     //-----------------------------------------------------
     // Controllo della validità del valore inserito
     //-----------------------------------------------------
     if(NumTgv<1 || NumTgv>MAXAGV){
        Application->MessageBox(MESS[374], MESS[100], MB_OK | MB_ICONEXCLAMATION);
        return;
     }
  }
  else NumTgv=(short int)(MaxAgv);
  //-----------------------------------------------------
  // Compilazione file con Allarmi attuali TGV
  //-----------------------------------------------------
  AGV[NumTgv]->CompilaAllarmiAttuali();
  //-----------------------------------------------------
  // inizializzazione dei file da visualizzare
  //-----------------------------------------------------
  sprintf(filename, "ALLTGV%02d.DAT", NumTgv);
  strcpy(filehelp, "HELP\\TGVBOX.HLP");
  //-----------------------------------------------------
  // Settaggio variabili per visualizzazione File
  //-----------------------------------------------------
  strcpy(FormVisualizzaFile->NomeFileVisualizza, filename);
  strcpy(FormVisualizzaFile->CaptionFileVisualizza, MESS[254]);
  sprintf(all_mess, "%s%d", MESS[254], NumTgv);
  strcpy(FormVisualizzaFile->IntestazioneFileVisualizza, all_mess);
  if(!access(filehelp, 0)){
     FormVisualizzaFile->HelpFile    = filehelp;
     FormVisualizzaFile->HelpContext = 1;
  }
  FormVisualizzaFile->ShowModal();
  FormVisualizzaFile->HelpFile    = "";
  FormVisualizzaFile->HelpContext = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MStoricoMissioniClick(TObject *Sender)
{
  short int NumTgv=0;
  AnsiString InputStringa;
  int  MaxAgv=MAXAGV;
  char Chiave[5]="";
  char NomeFile1[200];
  char NomeFile2[200];

  //-----------------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------------
  if(MaxAgv>1){
     InputStringa = InputBox(MESS[255], MESS[259], Chiave);
     if(InputStringa.IsEmpty() == true) return;
     strcpy(Chiave, InputStringa.c_str());
     NumTgv = (short int) atoi(Chiave);               // Recupero del Numero del TGV
     //-----------------------------------------------------
     // Controllo della validità del valore inserito
     //-----------------------------------------------------
     if(NumTgv<1 || NumTgv>MAXAGV){
        Application->MessageBox(MESS[374], MESS[100], MB_OK | MB_ICONEXCLAMATION);
        return;
     }
  }
  else NumTgv=(short int)(MaxAgv);
  //-----------------------------------------------------
  //  Recupera file da SERVER
  //-----------------------------------------------------
  if( IdPC>1 ){
     if(NumTgv==0){
        sprintf( NomeFile1, "%s\\HISTORY.DAT", DriveServer );
        sprintf( NomeFile2, "HISTORY.DAT" );
     }
     else{
        sprintf( NomeFile1, "%s\\HIST%03d.DAT", DriveServer, NumTgv );
        sprintf( NomeFile2, "HIST%03d.DAT", NumTgv);
     }
     CopyFile( NomeFile1, NomeFile2, FALSE );
  }
  //-----------------------------------------
  //  Apertura dei files da SERVER
  //-----------------------------------------
  if(NumTgv==0) strcpy(FormVisualizzaFile->NomeFileVisualizza, "HISTORY.DAT");
  else sprintf(FormVisualizzaFile->NomeFileVisualizza, "HIST%03d.DAT", NumTgv);
  strcpy(FormVisualizzaFile->CaptionFileVisualizza, MESS[367]);
  strcpy(FormVisualizzaFile->IntestazioneFileVisualizza, MESS[368]);
  FormVisualizzaFile->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MStoricoAllarmiClick(TObject *Sender)
{
  int err=0;
  char NomeFile[100]="LogCronoAll.txt";
  char NomeFile1[100];
  char NomeFile2[100]="CRONOALL.DAT";
  char NomeFile3[100]="CronoAll.tmp";
  char buffer[501]="";
  char Stringa[10][151];
  AnsiString myBuffer="";
  FILE *ptrfileSRC;
  FILE *ptrfileDST;
  //-----------------------------------------
  //  Recupera file da SERVER
  //-----------------------------------------
  if( IdPC>1 ){
     sprintf( NomeFile1, "%s\\CRONOALL.DAT", DriveServer );
     sprintf( NomeFile2, "CRONOALL.DAT" );
     CopyFile( NomeFile1, NomeFile2, FALSE );
  }
  //-------------------------------------------------------------------------------------------------
  // per ripristinare la vecchia gestione scommentare le 4 rigue seguenti e commentare tutte le altre
  // strcpy(FormListagen->NomeFileLista, "CRONOALL.DAT");
  // strcpy(FormListagen->CaptionFileLista, MESS[256]);
  // FormListagen->TipoFileLista=3;
  //    FormListagen->ShowModal();
  //-------------------------------------------------------------------------------------------------
  unlink(NomeFile);
  unlink(NomeFile3);

  CopyFile(NomeFile2, NomeFile3, FALSE );
  //------------------------------------------------------------
  // Converte il file esistente in un CSV"
  //------------------------------------------------------------
  ptrfileSRC = fopen( NomeFile3,"rb");
  if(ptrfileSRC==0){
     Application->MessageBox("Cannot Read File", MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }
  ptrfileDST = fopen( NomeFile, "wb");
  if(ptrfileDST==0){
     Application->MessageBox("Cannot Create File", "Error",MB_OK + MB_ICONSTOP + MB_DEFBUTTON2 + MB_TOPMOST);
     fclose(ptrfileSRC);
     return;
  }
  //------------------------------------------------------------
  do{
     err=Read_Line(ptrfileSRC, buffer, 400);
     memset(&Stringa,0,sizeof(Stringa));
     if(err){
        Copy(buffer,  1, 20,Stringa[0]); Erase_White_Begin(Stringa[0]);   Erase_White_End(Stringa[0]);
        Copy(buffer, 21,  6,Stringa[1]); Erase_White_Begin(Stringa[1]);   Erase_White_End(Stringa[1]);
        Copy(buffer, 27,  7,Stringa[2]); Erase_White_Begin(Stringa[2]);   Erase_White_End(Stringa[2]);
        Copy(buffer, 34, 81,Stringa[3]); Erase_White_Begin(Stringa[3]);   Erase_White_End(Stringa[3]);
        Copy(buffer,119,  9,Stringa[4]); Erase_White_Begin(Stringa[4]);   Erase_White_End(Stringa[4]);
        Copy(buffer,128,400,Stringa[5]); Erase_White_Begin(Stringa[5]);   Erase_White_End(Stringa[5]);

        myBuffer=AnsiString(Stringa[0]) +";"+  // Data e Ora
                 AnsiString(Stringa[1]) +";"+  // TGV
                 AnsiString(Stringa[2]) +";"+  // Pos
                 AnsiString(Stringa[3]) +";"+  // Message
                 AnsiString(Stringa[4]) +";"+  // Total
                 AnsiString(Stringa[5]);       // Line

        Write_Line(ptrfileDST, myBuffer.c_str());
     }
  }while(err);

  fclose(ptrfileSRC);
  fclose(ptrfileDST);
  unlink(NomeFile3);
  //------------------------------------------------------------
  // VISUALIZZAZIONE LISTGEN
  //------------------------------------------------------------
  //------------------------------------------------------------
  // se il file non esiste ERRORE
  //------------------------------------------------------------
  if(!FileExists(NomeFile)){
     Application->MessageBox("File not found", MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }
  //-----------------------------------------------------
  // Settaggio variabili per visualizzazione File
  //-----------------------------------------------------
  strcpy(FormListagen->NomeFileLista, NomeFile);
  strcpy(FormListagen->NomeFileAzzera, NomeFile2);
  strcpy(FormListagen->CaptionFileLista, MESS[256]);
  FormListagen->CarSeparatore=';'; // Carattere separatore campi nel file
  FormListagen->ColNum=6;          // numero di colonne totali
  //------------------------------------------------------------
  // Dimensioni ed Intestazioni delle colonne
  // sarebbe possibile impostarle a -1 per fare l'autosize,
  // ma poi la funzione di stampa non formatta bene il testo!
  //------------------------------------------------------------
  memset(&FormListagen->CaptionColumn,0,sizeof(FormListagen->CaptionColumn));
  FormListagen->ColSize[ 0]=130;   strcpy(FormListagen->CaptionColumn[ 0], "Date and Time");
  FormListagen->ColSize[ 1]= 50;   strcpy(FormListagen->CaptionColumn[ 1], "TGV");
  FormListagen->ColSize[ 2]= 50;   strcpy(FormListagen->CaptionColumn[ 2], "Pos");
  FormListagen->ColSize[ 3]=450;   strcpy(FormListagen->CaptionColumn[ 3], "Message");
  FormListagen->ColSize[ 4]= 60;   strcpy(FormListagen->CaptionColumn[ 4], "Total");
  FormListagen->ColSize[ 5]= 40;   strcpy(FormListagen->CaptionColumn[ 5], "Line");
  //------------------------------------------------------------
  FormListagen->TipoFileLista=5;
  FormListagen->btnExportExcel->Enabled  =true;
  FormListagen->btnSaveAsCSV->Enabled    =true;
  FormListagen->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MStatisticheClick(TObject *Sender)
{
  /************************************************************************************
  FormInput->Tipo=1;
  FormInput->ShowModal();
  if(FormInput->ModalResult==mrOk){
     if(FormInput->Index<0 || FormInput->Index>MAXAGV){
        Application->MessageBox(MESS[374], MESS[100], MB_OK | MB_ICONEXCLAMATION);
        return;
     }
     FormStatTgv->NumStatTgv = (short int)(FormInput->Index);
     strcpy(FormStatTgv->DataStatTgv, FormInput->DataS);
     FormStatTgv->ShowModal();
  }
  FormStatTgv->ShowModal();
  ************************************************************************************/
  FormStatTgv->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MResetMissioneClick(TObject *Sender)
{
  int NumTgv=0;
  bool ResetMissione;
  bool ResetChiamata;
  int  result, err;
  int  MaxAgv=MAXAGV;
  AnsiString InputStringa;
  char Chiave[5]="";
  char buffer[201]="";
  char NomeFile[100];

  //-----------------------------------------------------
  // Gestione Livello password
  //-----------------------------------------------------
  if(LivelloPassword<1) return;
  //-----------------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------------
  if(MaxAgv>1){
     InputStringa = InputBox(MESS[258], MESS[259], Chiave);
     if(InputStringa.IsEmpty() == true) return;
     strcpy(Chiave, InputStringa.c_str());
     NumTgv = (short int) atoi(Chiave);               // Recupero del Numero del TGV
     //-----------------------------------------------------
     // Controllo della validità del valore inserito
     //-----------------------------------------------------
     if(NumTgv<1 || NumTgv>MAXAGV){
        Application->MessageBox(MESS[374], MESS[100], MB_OK | MB_ICONEXCLAMATION);
        return;
     }
  }
  else NumTgv=(short int)(MaxAgv);

  //------------------------------------------------
  // Richiesta di conferma annullamento missione
  //------------------------------------------------
  sprintf(buffer, MESS[341], NumTgv);
  result = Application->MessageBox(buffer, MESS[99], MB_YESNO | MB_ICONQUESTION);
  if(result != IDYES) return;
  //----------------------------------------------------------
  // Inizializzo reset missione
  //----------------------------------------------------------
  ResetMissione=true;
  ResetChiamata=true;
  //----------------------------------------------------------
  // Richiesta abilitazione reset chiamata
  //----------------------------------------------------------
  if(AGV[NumTgv]->mission.Chiamata!=0 && (AGV[NumTgv]->mission.PresCEST_A!=LIBERA || AGV[NumTgv]->mission.PresCEST_B!=LIBERA)){
     ResetChiamata=false;
     result = Application->MessageBox("Reset match the mission call?", MESS[99], MB_YESNO | MB_ICONQUESTION);
     if(result == IDYES) ResetChiamata=true;
  }
  //----------------------------------------------------------
  // Invio alla navette il comando di RESET MISSION (DM98=1)
  //----------------------------------------------------------
  if(ResetMissione==true && AGV[NumTgv]->trasmetti_reset_AGV(buffer)) ResetMissione=false;
  //----------------------------------------------------------
  // SICUREZZA
  // Invio due volte il comando di RESET MISSIONE TGV
  //----------------------------------------------------------
  if(ResetMissione==true && AGV[NumTgv]->trasmetti_reset_AGV(buffer)) ResetMissione=false;
  //----------------------------------------------------------
  // SICUREZZA 2
  // Invio 3 volte il comando di RESET MISSIONE TGV
  //----------------------------------------------------------
  if(ResetMissione==true && AGV[NumTgv]->trasmetti_reset_AGV(buffer)) ResetMissione=false;
  //----------------------------------------------------------
  // Segnalazione errore
  //----------------------------------------------------------
  if(ResetMissione==false){
     sprintf(buffer, "RESET NOT POSSIBLE: Comunication error with TGV%d!", NumTgv);
     ShowMessage(buffer);
     return;
  }
  //----------------------------------------------------------
  // Se sono verificate tutte le condizione necessarie per
  // il Reset delle Missioni al volo azzero anche la missione
  // in RAM.
  //----------------------------------------------------------
  if(ResetMissione==true){
     // --- Azzeramento dati missione ---
     AGV[NumTgv]->reset_mission(buffer);
     if(ResetChiamata==true) AGV[NumTgv]->reset_chiamata_AGV();
     AGV[NumTgv]->mission.TGVInCambioMissione=false;
     AGV[NumTgv]->mission.TimeInitAttesa=0;
  }

  //-----------------------------------------
  //  Trasmetti comando al SERVER
  //-----------------------------------------
  if( IdPC>1 ){
     sprintf( NomeFile, "ResetMissTgv%02d", NumTgv );
     err = RemCreaFileComando ( NomeFile, NULL, NULL, buffer );
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     //----------------------------------------------
     // Comando di reset chiamata
     //----------------------------------------------
     if(ResetChiamata==true){
        sprintf( NomeFile, "ResetChTgv%02d", NumTgv );
        err = RemCreaFileComando ( NomeFile, NULL, NULL, buffer );
        if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
  }

  //-----------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------
  sprintf(buffer, "(RES_MIS) - Reset missione Tgv%d", NumTgv);
  FileLogOperatore(buffer);
  if(ResetChiamata==true){
     sprintf(buffer, "(RES_CH ) - Reset chiamata Tgv%d", NumTgv);
     FileLogOperatore(buffer);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MCreaArchivioNodiClick(TObject *Sender)
{
  AnsiString InputStringa;
  int i, err, result;
  char buffer[201]="";

  if( LivelloPassword<3 ) return;
  //-------------------------------------------------------
  // Introduzione di un ulteriore Password di sicurezza
  //-------------------------------------------------------
  InputStringa = InputBox(MESS[270], MESS[102], "");
  if(InputStringa.IsEmpty() == true) return;
  if(strcmp(InputStringa.c_str(), "ALPHA")!=0) return;

  sprintf(buffer, MESS[342], N.file_name_sorg);
  result = Application->MessageBox(buffer, MESS[99], MB_OKCANCEL | MB_ICONQUESTION);
  if (result == IDOK){
     //------------------------------------------
     // Azzera le strutture in memoria
     //------------------------------------------
     for(i=0; i<=MAXPUNTI; i++){
        setmem (N.n[i], sizeof( NOD ), 0);       // azzera la struttura
     }
     err = N.save_file_nodi(buffer);
     if( err ) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MEditorNodiClick(TObject *Sender)
{
  short int i;
  int  result;
  char buffer[101]="";

  if( LivelloPassword<3 ) return;
  // --- Apertura della form ---
  FormNodi->NodoIniziale=1;
  FormNodi->ShowModal();
  //--------------------------------------------------
  // Richiesta di scaricamento mappa nodi (TGV LASER)
  //--------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     if(!RiattivaMapping[i]) continue;
     sprintf(buffer, MESS[343], i);
     result = Application->MessageBox(buffer, MESS[99], MB_YESNO | MB_ICONQUESTION);
     if(result != IDYES) continue;
     //--------------------------------------------
     // Apertura della finestra di gestione mappa
     //--------------------------------------------
     FormMappa->NumTgv=i;
     FormMappa->ShowModal();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MLiberaNodiClick(TObject *Sender)
{
  char NomeFile[101]="";
  char buffer[101]="";
  int  i, err;

  if( LivelloPassword<2 ) return;
  //--------------------------------------------------
  // Libera i nodi di tutti i TGV
  //--------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
   //if( AGV[i]->test_mission_on() && !AGV[i]->mission.Esclusione) continue;
     //-----------------------------------------------
     // Eseguo la procedura di liberazione nodi solo
     // per i TGV non in movimento
     // CONTROLLO CHE ABBIA LA MISSIONE IN CORSO E
     // NON CI SIA IL BIT DI ALLARME
     //-----------------------------------------------
     if(AGV[i]->mission.pstart && !AGV[i]->stato.s.bit.allarme) continue;
     //-------------------------------------------
     // Libera tutti i nodi occupati dai TGV
     // esclusi o senza missione in corso
     //-------------------------------------------
     if( AGV[i]->free_all_nodi(buffer) ){
        Application->MessageBox( MESS[445], MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
  }
  //--------------------------------------------------
  //  Trasmetti comando al SERVER
  //--------------------------------------------------
  if( IdPC>1 ){
     strcpy( NomeFile, "LiberaNodi");
     err = RemCreaFileComando ( NomeFile, NULL, NULL, buffer );
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  //-----------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------
  FileLogOperatore("(LIB_NOD) - Liberazione dei Nodi occupati");
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MRicalcoloDistanzeNodiClick(TObject *Sender)
{
  double dist;
  short int i, j, p1, p2;
  int x1, x2, y1, y2, distX, distY;
  int result, Id, Raggio, Circonferenza;
  char all_mess[101]="";

  if(LivelloPassword<3) return;

  result = Application->MessageBox( "ATTENTION: This operation modify the path of TGVs. Continue?", MESS[99], MB_YESNO | MB_ICONQUESTION);
  if(result != IDYES) return;

  //--------------------------------------------
  // Ciclo per tutti i punti dell'impianto
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
        //----------------------------------
        // Calcolo della distanza
        //----------------------------------
        dist = (double)(sqrt_algo((distX*distX)+(distY*distY)));
        //----------------------------------
        // Calcolo della distanza percorsa
        // su un arco di circonferneza.
        //----------------------------------
        //----------------------------------
        // Calcolo della distanza percorsa
        // su un arco di circonferneza.
        //----------------------------------
        Id=Raggio=Circonferenza=0;
        if(!N.CercaCurva(TRUE, p1, p2, &Raggio, &Circonferenza, &Id)){
           dist = (short int)(Circonferenza);
        }
        N.n[i]->rec[j].dist=(short int)(dist);
     }
  }
  //---------------------------------------
  // Salvataggio del file dei nodi
  //---------------------------------------
  if(N.save_file_nodi(all_mess)) ShowMessage(all_mess);
  else ShowMessage("Ok...");

  // --- Traccia su LOG OPERATORE
  FileLogOperatore("(DIST   ) - Ricalcolo di tutte le distanze");
  return;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MVisualizzaRiflettoriClick(TObject *Sender)
{
  AnsiString Buff1, Buff2, InputStringa;
  int pos, num_tgv, zona, i, err, result;

  if( LivelloPassword<3 ) return;
  //-------------------------------------------------------
  // Introduzione di un ulteriore Password di sicurezza
  //-------------------------------------------------------
  //InputStringa = InputBox("Visualizza Riflettori", "Seleziona TGV+zona (tt+zz):", "");
  InputStringa = InputBox(MESS[269], MESS[267], "");
  if(InputStringa.IsEmpty() == true) return;
  //-------------------------------------------------------
  // Scomposizine stringa
  //-------------------------------------------------------
  pos=num_tgv=zona=0;
  pos  = InputStringa.Pos("+");
  if(pos!=0){
     Buff1 = InputStringa.SubString(1,     pos-1);
     Buff2 = InputStringa.SubString(pos+1, Buff2.Length());
     num_tgv = (int)(atoi(Buff1.c_str()));   // Numero TGV
     zona    = (int)(atoi(Buff2.c_str()));   // Numero Zona
  }
  //-------------------------------------------------------
  // Controllo inserimento dati
  //-------------------------------------------------------
  if(pos==0 || num_tgv==0){
     //ShowMessage("ATTENZIONE: Inserimento dati in formato errato...");
     ShowMessage(MESS[649]);
     return;
  }
  //-------------------------------------------------------
  // Recupero zona selezionata
  //-------------------------------------------------------
  if((num_tgv>0 && num_tgv<=MAXAGV) && (zona==99 || (zona>=0 && zona<=MAXZONEMAPPING))){
     LaserMappaRiflettoriZona=zona;
     LaserMappaRiflettoriTgv =num_tgv;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MModificaCoordinateNodiClick(TObject *Sender)
{
  AnsiString InputStringa;

  if(ModificaCoordinateNodi==false && LivelloPassword<3) return;
  //-------------------------------------------------------
  // Introduzione di un ulteriore Password di sicurezza
  //-------------------------------------------------------
  if(ModificaCoordinateNodi==false){
     InputStringa = InputBox(MESS[101], MESS[102], "");
     if(InputStringa.IsEmpty() == true) return;
     if(strcmp(InputStringa.c_str(), "ALGOCAD")!=0) return;
  }
  //----------------------------------------------
  // Abilita/Disabilita opzione di modifica
  // delle coordinate di un gruppo di nodi
  //----------------------------------------------
  ModificaCoordinateNodi=!ModificaCoordinateNodi;
  MModificaCoordinateNodi->Checked = ModificaCoordinateNodi;
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
  else{
     // --- Traccia su LOG OPERATORE
     FileLogOperatore("(X_Y_NOD) - Modifica coordinate nodi dell'impianto");
     //----------------------------------------------------
     // Attivazione finestra XYNodi
     //----------------------------------------------------
     FormLayout->Width   = FormMain->ClientWidth-FormXYNodi->Width-FormStato->Width-4;
     FormXYNodi->Left    = FormLayout->Width;
     FormXYNodi->Top     = FormLayout->Top;
     FormXYNodi->Height  = FormLayout->Height;
     FormStato->Left     = FormLayout->Width+FormXYNodi->Width;
     FormStato->Top      = FormLayout->Top;
     //-----------------------------------------------------------
     // Refresh finestra
     //-----------------------------------------------------------
     FormXYNodi->Refresh(TRUE);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MRicaricaCoordinateClick(TObject *Sender)
{
  FormXYNodi->ReloadGRAPHVAR(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MCreaArchivioMacchineClick(TObject *Sender)
{
  AnsiString InputStringa;
  int i, err, result;
  char buffer[201]="";

  if( LivelloPassword<3 ) return;
  //-------------------------------------------------------
  // Introduzione di un ulteriore Password di sicurezza
  //-------------------------------------------------------
  InputStringa = InputBox(MESS[280], MESS[102], "");
  if(InputStringa.IsEmpty() == true) return;
  if(strcmp(InputStringa.c_str(), "ALPHA")!=0) return;

  sprintf(buffer, MESS[342], P.chiamate_name_sorg);
  result = Application->MessageBox(buffer, MESS[99], MB_OKCANCEL | MB_ICONQUESTION);
  if (result == IDOK){
     //---------------------------------------
     // Azzera le strutture in memoria
     //---------------------------------------
     for(i=0; i<=MAXCHIAMATE; i++){
        setmem (P.ch[i], sizeof( CHIAMATE ), 0);       // azzera la struttura
     }
     err = P.save_file_chiamate(buffer);
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     //-----------------------------
     // Traccia su LOG OPERATORE
     //-----------------------------
     FileLogOperatore("(CREAMAC) - Rigenerazione Archivio Mac");
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MEditorMacchineClick( TObject *Sender)
{
  FormMacchine->NumeroMacchina  = 1;     // Apertura Editor su primo record
  FormMacchine->NumeroStazione  = 0;     // Apertura Editor su primo record
  FormMacchine->NumeroPosizione = 0;     // Apertura Editor su primo record
  // --- Apertura della form ---
  FormMacchine->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MCreaArchivioBaieClick(TObject *Sender)
{
  AnsiString InputStringa;
  int i, err, result;
//int posti;
  char buffer[201]="";

  if( LivelloPassword<3 ) return;
  //-------------------------------------------------------
  // Introduzione di un ulteriore Password di sicurezza
  //-------------------------------------------------------
  InputStringa = InputBox(MESS[280], MESS[102], "");
  if(InputStringa.IsEmpty() == true) return;
  if(strcmp(InputStringa.c_str(), "ALPHA")!=0) return;

  sprintf(buffer, MESS[342], P.baie_name_sorg);
  result = Application->MessageBox(buffer, MESS[99], MB_OKCANCEL | MB_ICONQUESTION);
  if (result == IDOK){
     //---------------------------------------
     // Azzera le strutture in memoria
     //---------------------------------------
     for(i=0; i<=MAXBAIE; i++){
        setmem(P.ba[i], sizeof( BAIE ), 0);       // azzera la struttura
     }
     //---------------------------------------
     // Compilazione automatica di quelle
     // configurate in grafica
     //---------------------------------------
/*
     for(i=1; i<=TOTBAIE; i++){
        posti = Baie[i][1];          // Massimo numero cestoni
        if(posti<1) continue;
        //-----------------------------------
        // Inizializzazione campi principali
        //-----------------------------------
        P.ba[i]->Num         = i;
        P.ba[i]->Punto       = i;
        P.ba[i]->Esclusione  = true;
        strcpy(P.ba[i]->Sigla, "BAIA");
     }
*/
     //---------------------------------------
     // Salvataggio su file
     //---------------------------------------
     err = P.save_file_baie(buffer);
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     //---------------------------------------
     // Traccia su LOG OPERATORE
     //---------------------------------------
     FileLogOperatore("(CREABAI) - Rigenerazione Archivio Baie");
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MEditorBaieClick(TObject *Sender)
{
  short int n_rec;

  //----------------------------------------
  // Recupero numero della prima baia
  //----------------------------------------
  for( n_rec=1; n_rec<=MAXBAIE; n_rec++){
     if( P.ba[ n_rec ]->Num >0 ) break;
  }
  if(n_rec>MAXBAIE) n_rec=1;
  //----------------------------------------
  // Apertura della finestra
  //----------------------------------------
  FormBaie->NumeroBaia  = n_rec;     // Apertura Editor su primo record
  // --- Apertura della form ---
  FormBaie->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MStatoBaieClick(TObject *Sender)
{
  FormStatoBaie->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MStoricoChiamateClick(TObject *Sender)
{
  char NomeFile1[200];
  char NomeFile2[200];
  //-----------------------------------------
  //  Recupera file da SERVER
  //-----------------------------------------
  if( IdPC>1 ){
     sprintf( NomeFile1, "%s\\LOGCH.DAT", DriveServer );
     sprintf( NomeFile2, "LOGCH.DAT" );
     CopyFile( NomeFile1, NomeFile2, FALSE );
  }
  //-----------------------------------------
  //  Apertura dei files da SERVER
  //-----------------------------------------
  strcpy(FormVisualizzaFile->NomeFileVisualizza, "LOGCH.DAT");
  strcpy(FormVisualizzaFile->CaptionFileVisualizza, MESS[282]);
  strcpy(FormVisualizzaFile->IntestazioneFileVisualizza, MESS[646]);
  FormVisualizzaFile->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MCreaArchivioMagazziniClick(TObject *Sender)
{
  AnsiString InputStringa;
  int i, err, result;
  char buffer[201]="";

  if( LivelloPassword<3 ) return;
  //-------------------------------------------------------
  // Introduzione di un ulteriore Password di sicurezza
  //-------------------------------------------------------
  InputStringa = InputBox(MESS[276], MESS[102], "");
  if(InputStringa.IsEmpty() == true) return;
  if(strcmp(InputStringa.c_str(), "ALPHA")!=0) return;

  sprintf(buffer, MESS[342], M.mag_name_sorg);
  result = Application->MessageBox(buffer, MESS[99], MB_OKCANCEL | MB_ICONQUESTION);
  if (result == IDOK){
     //---------------------------------------
     // Azzera le strutture in memoria
     //---------------------------------------
     for(i=0; i<=MAXMAG; i++){
        setmem (M.m[i], sizeof( MAG ), 0);       // azzera la struttura
     }
     err = M.save_file_mag(buffer);
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     //-----------------------------
     // Traccia su LOG OPERATORE
     //-----------------------------
     FileLogOperatore("(CREAMAG) - Rigenerazione Archivio Mag");
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MEditorMagazziniClick(TObject *Sender)
{
  FormMagaz->NumeroMagazzino = 1;     // Apertura Editor su primo record
  FormMagaz->NumeroPosizione = 1;     // Apertura Editor su primo record
  // --- Apertura della form ---
  FormMagaz->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MStatoMagazziniClick(TObject *Sender)
{
  FormStatoMagaz->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MCreaArchivioPorteClick(TObject *Sender)
{
  AnsiString InputStringa;
  int  err, result;
  char buffer[201]="";

  if( LivelloPassword<3 ) return;
  //-------------------------------------------------------
  // Introduzione di un ulteriore Password di sicurezza
  //-------------------------------------------------------
  InputStringa = InputBox(MESS[285], MESS[102], "");
  if(InputStringa.IsEmpty() == true) return;
  if(strcmp(InputStringa.c_str(), "ALPHA")!=0) return;

  sprintf(buffer, MESS[342], porteimp_name_sorg);
  result = Application->MessageBox(buffer, MESS[99], MB_OKCANCEL | MB_ICONQUESTION);
  if (result == IDOK){
     //---------------------------------------
     // Azzera le strutture in memoria
     //---------------------------------------
     memset(&PorteImp, 0, sizeof(PorteImp));       // azzera la struttura
     //---------------------------------------
     // salvataggio su file della struttura
     //---------------------------------------
     err = save_file_porteimp(buffer);
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MEditorPorteClick(TObject *Sender)
{
  AnsiString InputStringa;
  int TotPorte = TOTPORTE;
  short int  NumPorta=1;
  char Chiave[5]="1";

  if(LivelloPassword<2) return;
  //-----------------------------------------------
  // Nessuna porta
  //-----------------------------------------------
  if(TotPorte<1) return;
  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  if(TotPorte>1){
     NumPorta=0;
     InputStringa = InputBox(MESS[26], MESS[381], Chiave);
     if(InputStringa.IsEmpty() == true) return;
     strcpy(Chiave, InputStringa.c_str());
     NumPorta = (short int) atoi(Chiave);               // Recupero del Numero del TGV
     //----------------------------------------------
     // Controllo della validità del valore inserito
     //----------------------------------------------
     if(NumPorta<1 || NumPorta>TOTPORTE){
        Application->MessageBox(MESS[373], MESS[100], MB_OK | MB_ICONEXCLAMATION);
        return;
     }
  }
  //----------------------------------------------
  // Apertura dell'editor del TGV selezionato
  //----------------------------------------------
  FormPorteImp->NumeroPortaImp = NumPorta;
  FormPorteImp->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MConsensiClick(TObject *Sender)
{
  AnsiString InputStringa;
  int TotPorte = TOTPORTE;
  short int  NumPorta=1;
  char Chiave[5]="";

  if(LivelloPassword<3) return;
  //-----------------------------------------------
  // Se comunicazione attiva --> skip!
  //-----------------------------------------------
  if(!break_Com[COM2] ) return;
  //-----------------------------------------------
  // Nessuna porta
  //-----------------------------------------------
  if(TotPorte<1) return;
  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  if(TotPorte>1){
     NumPorta=0;
     InputStringa = InputBox("Door's management", MESS[381], Chiave);
     if(InputStringa.IsEmpty() == true) return;
     strcpy(Chiave, InputStringa.c_str());
     NumPorta = (short int) atoi(Chiave);               // Recupero del Numero del TGV
     //----------------------------------------------
     // Controllo della validità del valore inserito
     //----------------------------------------------
     if(NumPorta<1 || NumPorta>TOTPORTE){
        Application->MessageBox(MESS[373], MESS[100], MB_OK | MB_ICONEXCLAMATION);
        return;
     }
  }
  //-----------------------------------------------
  // Sumulazione stato porte
  //-----------------------------------------------
  PorteImp[NumPorta].OkIngresso=!PorteImp[NumPorta].OkIngresso;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MConfigTipiBoxClick(TObject *Sender)
{
  //--------------------------------
  // Solo per tecnici esperti
  //--------------------------------
  if(LivelloPassword<3) return;
  //--------------------------------
  // Apertura della form
  //--------------------------------
  FormTipiBox->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MConfigGrpBaClick(TObject *Sender)
{
  //--------------------------------
  // Solo per tecnici esperti
  //--------------------------------
  if(LivelloPassword<3) return;
  //--------------------------------
  // Apertura della form
  //--------------------------------
  FormGruppiBa->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CreateCBatClick(TObject *Sender)
{
  AnsiString InputStringa;
  int i, err, result;
  char buffer[201]="";

  if( LivelloPassword<3 ) return;
  //-------------------------------------------------------
  // Introduzione di un ulteriore Password di sicurezza
  //-------------------------------------------------------
  InputStringa = InputBox(MESS[280], MESS[102], "");
  if(InputStringa.IsEmpty() == true) return;
  if(strcmp(InputStringa.c_str(), "ALPHA")!=0) return;

  sprintf(buffer, MESS[342], P.cbat_name_sorg);
  result = Application->MessageBox(buffer, MESS[99], MB_OKCANCEL | MB_ICONQUESTION);
  if (result == IDOK){
     //---------------------------------------
     // Azzera le strutture in memoria
     //---------------------------------------
     for(i=0; i<=MAXBATT; i++){
        setmem(P.cb[i], sizeof( CAMBIOBAT ), 0);       // azzera la struttura
     }
     //---------------------------------------
     // Salvataggio su file
     //---------------------------------------
     err = P.save_file_cbat(buffer);
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     //---------------------------------------
     // Traccia su LOG OPERATORE
     //---------------------------------------
     FileLogOperatore("(CREABATT) - Rigenerazione Archivio CambioBatterie");
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MEditorCBattClick(TObject *Sender)
{
  short int n_rec;
  //----------------------------------------
  // Recupero numero della prima baia
  //----------------------------------------
  for( n_rec=1; n_rec<=MAXBATT; n_rec++){
     if( P.cb[ n_rec ]->Num >0 ) break;
  }
  if(n_rec>MAXBATT) n_rec=1;
  //----------------------------------------
  // Apertura della finestra
  //----------------------------------------
  FormCambioBatteria->NumCBatt = n_rec;  // Apertura Editor su primo record
  FormCambioBatteria->ShowModal();       // Apertura della form
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MStatoCBattClick(TObject *Sender)
{
  MStatoCBatt->Enabled=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MLogMovimentiClick(TObject *Sender)
{
  FormStato->btnLogViewClick(FormStato->btnLogView);
}
//---------------------------------------------------------------------------

