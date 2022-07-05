//---------------------------------------------------------------------------
#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ToolWin.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <vcl\inifiles.hpp>
#include <Dialogs.hpp>
#include <DBTables.hpp>
#include <ADODB.hpp>
#include <DB.hpp>
#include "XLSMini.hpp"
#include "WSocket.hpp"

//---------------------------------------------------------------------------
#include "TecnoFer.h"
#include "Mapping.h"
#include "Laser.h"
#include "OmronETH.h"
#include "UdpServer1.h"
#include <NMEcho.hpp>
#include <Psock.hpp>
#include <NMFngr.hpp>

//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
  TMainMenu *MainMenu;
  TMenuItem *MenuSistema;
  TMenuItem *MEsci;
  TTimer *TimerCiclo;
  TStatusBar *SBarImpianto;
  TMenuItem *N1;
  TMenuItem *MComunicazioni;
  TMenuItem *MCOM1;
  TMenuItem *MCOM2;
  TMenuItem *N2;
  TMenuItem *MEditorComunication;
  TMenuItem *MPassword;
  TMenuItem *MMessaggeria;
  TMenuItem *N3;
  TMenuItem *MTestPlc1;
  TMenuItem *N4;
  TMenuItem *MSfondoGrafico;
  TTimer *TimerRefresh;
  TBevel *BevelStato;
  TPanel *SpeedBar;
  TSpeedButton *BtnLayout;
  TSpeedButton *BtnZoomAll;
  TSpeedButton *BtnZoomNumeri;
  TTimer *TimerLampeggio;
  TMenuItem *MenuTgv;
  TMenuItem *MEditorTgv;
  TMenuItem *N5;
  TMenuItem *MPercorsoTgv;
  TMenuItem *MProgrammaTgv;
  TMenuItem *MProgrammaPlcTgv;
  TMenuItem *N6;
  TMenuItem *MAllarmiTgv;
  TMenuItem *N7;
  TMenuItem *MStoricoMissioni;
  TMenuItem *MStoricoAllarmi;
  TMenuItem *N8;
  TMenuItem *MResetMissione;
  TMenuItem *MenuNodi;
  TMenuItem *MCreaArchivioNodi;
  TMenuItem *N10;
  TMenuItem *MEditorNodi;
  TMenuItem *N11;
  TMenuItem *MLiberaNodi;
  TMenuItem *MenuChiamate;
  TMenuItem *MSimulazione;
  TMenuItem *MErroriDiSistema;
  TSpeedButton *BtnSemiAuto;
  TSpeedButton *BtnPresentazione;
  TTimer *TimerMin;
  TMenuItem *MClientServer;
  TOpenDialog *OpenDialogServer;
  TMenuItem *N17;
  TMenuItem *MBackup;
  TMenuItem *MenuPorte;
  TMenuItem *MCreaArchiviopPorte;
  TMenuItem *N18;
  TMenuItem *MEditorPorte;
  TMenuItem *N19;
  TMenuItem *MStatistiche;
  TMenuItem *N16;
  TSpeedButton *BtnPorte;
  TMenuItem *N20;
  TMenuItem *N9;
  TMenuItem *MRicalcoloDistanzeNodi;
  TMenuItem *MCOM3;
  TMenuItem *N12;
  TMenuItem *MConsensi;
  TMenuItem *MModificaCoordinateNodi;
  TSpeedButton *BtnNodo;
  TMenuItem *MTestPlc2;
  TMenuItem *MTestPercorsiTecno;
  TMenuItem *MLogTgv;
  TMenuItem *MZoneImpianto;
  TMenuItem *N15;
  TSpeedButton *BtnPassword;
  TSpeedButton *BtnLogOp;
  TSpeedButton *BtnBakcup;
  TMenuItem *MTestPercorsi;
  TXLSMini *XLS;
  TSpeedButton *BtnRiorgMag;
  TADOConnection *ADOConnSQL;
  TSpeedButton *BtnDBSQL;
  TSpeedButton *BtnListBox;
  TMenuItem *MStoricoChiamate;
  TMenuItem *MTestPlc3;
  TMenuItem *N13;
  TMenuItem *MCreaArchivioMacchine;
  TMenuItem *MEditorMacchine;
  TMenuItem *MenuConfig;
  TMenuItem *MenuMagazzini;
  TMenuItem *MCreaArchivioMagazzini;
  TMenuItem *MEditorMagazzini;
  TMenuItem *N24;
  TMenuItem *OPCClientconnection;
  TMenuItem *OPCClientConfiguration;
  TWSocket *WSocket0;
  TWSocket *WSocket1;
  TWSocket *WSocket2;
  TWSocket *WSocket3;
  TWSocket *WSocket4;
  TWSocket *WSocket5;
  TWSocket *WSocket6;
  TWSocket *WSocket7;
  TWSocket *WSocket8;
  TWSocket *WSocket9;
  TWSocket *WSocket10;
  TWSocket *WSocket11;
  TWSocket *WSocket12;
  TWSocket *WSocket13;
  TWSocket *WSocket14;
  TWSocket *WSocket15;
  TWSocket *WSocket16;
  TWSocket *WSocket17;
  TWSocket *WSocket18;
  TWSocket *WSocket19;
  TWSocket *WSocket20;
  TWSocket *WSocket21;
  TWSocket *WSocket22;
  TWSocket *WSocket23;
  TWSocket *WSocket24;
  TWSocket *WSocket25;
  TWSocket *WSocket26;
  TWSocket *WSocket27;
  TWSocket *WSocket28;
  TWSocket *WSocket29;
  TWSocket *WSocket30;
  TWSocket *WSocket31;
  TWSocket *WSocket32;
  TWSocket *WSocket33;
  TWSocket *WSocket34;
  TWSocket *WSocket35;
  TWSocket *WSocket36;
  TWSocket *WSocket37;
  TWSocket *WSocket38;
  TWSocket *WSocket39;
  TWSocket *WSocket40;
  TWSocket *WSocket41;
  TWSocket *WSocket42;
  TWSocket *WSocket43;
  TWSocket *WSocket44;
  TWSocket *WSocket45;
  TWSocket *WSocket46;
  TWSocket *WSocket47;
  TWSocket *WSocket48;
  TWSocket *WSocket49;
  TWSocket *WSocket50;
  TWSocket *WSocket51;
  TMenuItem *N21;
  TMenuItem *MCOM4;
  TMenuItem *MCOM5;
  TMenuItem *MCOM6;
  TMenuItem *MCOM7;
  TMenuItem *MCOM8;
  TMenuItem *MCOM9;
  TMenuItem *MenuBaie;
  TMenuItem *MCreaArchivioBaie;
  TMenuItem *MEditorBaie;
  TMenuItem *N22;
  TMenuItem *MStatoBaie;
  TMenuItem *MConfigTipiBox;
  TMenuItem *N25;
  TMenuItem *MStatoMagazzini;
  TMenuItem *MenuCBatt;
  TMenuItem *CreateCBat;
  TMenuItem *MEditorCBatt;
  TMenuItem *N26;
  TMenuItem *MStatoCBatt;
  TMenuItem *MRicaricaCoordinate;
  TMenuItem *N14;
  TMenuItem *MVisualizzaRiflettori;
  TMenuItem *N23;
  TMenuItem *MConfigGrpBa;
  TADOQuery *ADOQuery1;
  TMenuItem *MLogMovimenti;
  TSpeedButton *BtnStatTgv;
  TMenuItem *MZoneMapping;
  void __fastcall MEsciClick(TObject *Sender);

  void __fastcall FormResize(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);

  void __fastcall BtnLayoutClick(TObject *Sender);

  void __fastcall MMessaggeriaClick(TObject *Sender);
  void __fastcall MPasswordClick(TObject *Sender);
  void __fastcall MTestPlc1Click(TObject *Sender);
  
  void __fastcall SBarImpiantoDrawPanel(TStatusBar *StatusBar,
          TStatusPanel *Panel, const TRect &Rect);
  
  void __fastcall MCOMClick(TObject *Sender);
  
  void __fastcall MEditorComunicationClick(TObject *Sender);
  void __fastcall MSfondoGraficoClick(TObject *Sender);
  void __fastcall TimerRefreshTimer(TObject *Sender);
  void __fastcall TimerCicloTimer(TObject *Sender);
  void __fastcall BtnZoomAllClick(TObject *Sender);
  void __fastcall BtnZoomNumeriClick(TObject *Sender);
  
  void __fastcall TimerLampeggioTimer(TObject *Sender);
  void __fastcall MEditorTgvClick(TObject *Sender);
  void __fastcall MPercorsoTgvClick(TObject *Sender);
  void __fastcall MProgrammaTgvClick(TObject *Sender);
  void __fastcall MProgrammaPlcTgvClick(TObject *Sender);
  void __fastcall MAllarmiTgvClick(TObject *Sender);
  void __fastcall MStoricoAllarmiClick(TObject *Sender);
  void __fastcall MStoricoMissioniClick(TObject *Sender);
  void __fastcall MResetMissioneClick(TObject *Sender);
  void __fastcall MCreaArchivioNodiClick(TObject *Sender);
  void __fastcall MEditorNodiClick(TObject *Sender);
  void __fastcall MLiberaNodiClick(TObject *Sender);
  void __fastcall MSimulazioneClick(TObject *Sender);
  void __fastcall MErroriDiSistemaClick(TObject *Sender);
  void __fastcall BtnSemiautomaticoClick(TObject *Sender);
  void __fastcall BtnPresentazioneClick(TObject *Sender);
  void __fastcall TimerMinTimer(TObject *Sender);
  void __fastcall MClientServerClick(TObject *Sender);
  void __fastcall MBackupClick(TObject *Sender);
  
  
  
  void __fastcall MCreaArchivioPorteClick(TObject *Sender);
  void __fastcall MEditorPorteClick(TObject *Sender);
  
  void __fastcall MStatisticheClick(TObject *Sender);
  void __fastcall BtnPorteClick(TObject *Sender);


  void __fastcall MRicalcoloDistanzeNodiClick(TObject *Sender);
  void __fastcall MConsensiClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);


  void __fastcall MModificaCoordinateNodiClick(TObject *Sender);
  

  void __fastcall BtnNodoClick(TObject *Sender);
  void __fastcall MTestPlc2Click(TObject *Sender);
  void __fastcall MTestPercorsiTecnoClick(TObject *Sender);
  void __fastcall MZoneImpiantoClick(TObject *Sender);
  void __fastcall MLogTgvClick(TObject *Sender);
  void __fastcall BtnLogOpClick(TObject *Sender);
  
  void __fastcall MTestPercorsiClick(TObject *Sender);
  void __fastcall BtnDBSQLClick(TObject *Sender);
  void __fastcall MStoricoChiamateClick(TObject *Sender);
  void __fastcall MTestPlc3Click(TObject *Sender);
  void __fastcall MCreaArchivioMacchineClick(TObject *Sender);
  void __fastcall MEditorMacchineClick(TObject *Sender);
  void __fastcall MCreaArchivioMagazziniClick(TObject *Sender);
  void __fastcall MEditorMagazziniClick(TObject *Sender);
  void __fastcall OPCClientconnectionClick(TObject *Sender);
  void __fastcall OPCClientConfigurationClick(TObject *Sender);
  void __fastcall BtnListBoxClick(TObject *Sender);
  void __fastcall MCreaArchivioBaieClick(TObject *Sender);
  void __fastcall MEditorBaieClick(TObject *Sender);
  void __fastcall MConfigTipiBoxClick(TObject *Sender);
  void __fastcall MStatoBaieClick(TObject *Sender);
  void __fastcall MStatoMagazziniClick(TObject *Sender);
  void __fastcall CreateCBatClick(TObject *Sender);
  void __fastcall MEditorCBattClick(TObject *Sender);
  void __fastcall MStatoCBattClick(TObject *Sender);
  void __fastcall MRicaricaCoordinateClick(TObject *Sender);
  void __fastcall MVisualizzaRiflettoriClick(TObject *Sender);
  void __fastcall MConfigGrpBaClick(TObject *Sender);
  void __fastcall MLogMovimentiClick(TObject *Sender);
  void __fastcall MZoneMappingClick(TObject *Sender);
private:	// User declarations

  int  CountMinPassword;
  int  CountMinStatTgv;
  int  CountCicliLogTgv;
  int  CountMinConnSQL;
  //-------------------------------------------------------
  // Funzioni di inizializzazione del Sistema
  //-------------------------------------------------------
  void InizializzaClassi();
  void InizializzaVariabiliGlobali();
  //-------------------------------------------------
  // Inizializzazione della Form principale
  //-------------------------------------------------
  void ReadIniFormMain();
  void WriteIniFormMain();
  //-------------------------------------------------
  // Release software
  //-------------------------------------------------
  void WriteIniRelease();

public:		// User declarations

  bool FormInOpen;
  bool FormInClose;
  bool Minuto;                                         // Flag di minuto trascorso
  bool MinutoCiclo;
  void Messaggi();

  //-------------------------------------------------
  // Inizializzazione del Progetto
  //-------------------------------------------------
  void ReadIniProject();
  void WriteIniProject();

  //-------------------------------------------------
  // Funzione di lettura file configurazione LOCAZ.
  //-------------------------------------------------
  int ReadConfigInterbloccoNodi(char *all_mess);
  int ReadConfigAtteseTGV(char *all_mess);
  int ReadConfigPintObbligato(char *all_mess);

  //-------------------------------------------------
  // Abilitazioni Password
  //-------------------------------------------------
  void RicPasswordAccensione();
  void StatoPassword();

  //-------------------------------------------------
  // Gestione TELECAMERINA
  //-------------------------------------------------
  int  LoadLogTgv(short int Record, long int *TimeRecord, char *all_mess);
  int  AggiornaLogTgv();

  __fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
