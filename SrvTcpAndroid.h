//---------------------------------------------------------------------------
#ifndef SrvTcpAndroidH
#define SrvTcpAndroidH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "WSocket.hpp"
//---------------------------------------------------------------------------
#include "TecnoFer.h"
#include <Buttons.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------

//------------------------------------------------
// TIPO IMPIANTO
//
// 0 - sconosciuto
// 1 - Abilitazioni tipo BOX2006
// 2 - Abilitazioni tipo BOX2000
// 3 - Abilitazioni tipo PAL2000
// 4 - Abilitazioni tipo ISM2000
// 5 - Abilitazioni tipo FOOD
// 6 - Abilitazioni tipo SPECIALI 1
// 7 - Abilitazioni tipo SPECIALI 1
// ...
// ...
// 255 - APPLICAZIONE DI ATTIVAZIONE DISPOSITIVI
//------------------------------------------------
#define  ABILITA_OPZIONI_ANDROID     6

//------------------------------------------------
// Dimensionamento connessioni massime supportate
//------------------------------------------------
#define  MAXSOCKET_ANDROID            20
#define  MAXMESSAG_ANDROID            700
#define  MAXALLARM_ANDROID            100

//------------------------------------------------
// GESTIONE STATO CONNESSIONE
//------------------------------------------------
#define STATO_CONN_AMMESSA_ANDROID      0
#define STATO_CONN_NOT_AMMESSA_ANDROID  1
#define STATO_COD_NON_VALIDO_ANDROID    2
#define STATO_COD_SCADUTO_ANDROID       3
//...
#define STATO_CODTMP_ANDROID           49
#define STATO_ATTIVA_HW_ANDROID        50
#define STATO_DISATTIVA_HW_ANDROID     51

//------------------------------------------------
// HARDWARE TECNOFERRARI
//------------------------------------------------
#define HARDWARE_TECNOFERRARI           1

//------------------------------------------------
// MODALITA' ACCESSO SUPPORTATE
//------------------------------------------------
#define MODE_MANUALS_ANDROID          0
#define MODE_STATE_TGV_ANDROID        1
#define MODE_STATE_PLANT_ANDROID      2

//------------------------------------------------
// Elenco comandi gestiti in comunicazione
//------------------------------------------------
#define  CMD_02_PARAMETRI_IMP         2
#define  CMD_09_MESSAGGERIA           9
#define  CMD_20_STATO_ALL_TGV         20
#define  CMD_21_STATO_ONE_TGV         21
#define  CMD_22_PARAMETRI_COM_TGV     22
#define  CMD_30_STATO_ALL_MAC         30
#define  CMD_31_STATO_ONE_MAC         31
#define  CMD_40_STATO_ALL_MAG         40
#define  CMD_50_STATO_ONE_BOX         50
#define  CMD_60_STATO_ALL_COD         60
#define  CMD_61_STATO_ONE_COD         61
#define  CMD_62_STATO_ONE_COD         62
#define  CMD_70_STATO_ALL_CEL         70
#define  CMD_71_STATO_ONE_CEL         71

//------------------------------------------------
// Colori gestiti dall'applicazione
//------------------------------------------------
#define  COL_APK_GRAY                 0
#define  COL_APK_LIME                 1
#define  COL_APK_RED                  2
#define  COL_APK_BLUE                 3
#define  COL_APK_WHITE                4
#define  COL_APK_CYAN                 5
#define  COL_APK_YELLOW               6
#define  COL_APK_MAGENTA              7

//---------------------------------------------------------------------------

class TFormSrvTcpAndroid : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *GroupBox2;
  TLabel *TextRxTCP;
  TLabel *CmdRxTCP;
  TLabel *Label3;
  TEdit *EditTimeOutSrv;
  TTimer *TimerStateConnection;
  TLabel *TextConnection;
  TBevel *Bevel1;
  TTimer *TimerRefresh;
  TWSocket *SrvWSocket;
  TWSocket *TmpWSocket;
  TWSocket *CliWSocket1;
  TWSocket *CliWSocket2;
  TWSocket *CliWSocket3;
  TWSocket *CliWSocket4;
  TWSocket *CliWSocket5;
  TLabel *Label7;
  TEdit *EditTcpPort;
  TBitBtn *BtnSalva;
  TBitBtn *BtnAbort;
  TBitBtn *BtnClose;
  TStringGrid *AndroidGrid;
  TWSocket *CliWSocket6;
  TWSocket *CliWSocket7;
  TWSocket *CliWSocket8;
  TWSocket *CliWSocket9;
  TWSocket *CliWSocket10;
  TWSocket *CliWSocket11;
  TWSocket *CliWSocket12;
  TWSocket *CliWSocket13;
  TWSocket *CliWSocket14;
  TWSocket *CliWSocket15;
  TWSocket *CliWSocket16;
  TWSocket *CliWSocket17;
  TWSocket *CliWSocket18;
  TWSocket *CliWSocket19;
  TWSocket *CliWSocket20;
  TButton *BtnKeygen;
  TButton *BtnTestKey;
  void __fastcall FormActivate(TObject *Sender);

  void __fastcall SrvWSocketSessionAvailable(TObject *Sender, WORD Error);
  void __fastcall CliWSocketDataAvailable(TObject *Sender, WORD Error);
  void __fastcall CliWSocketSessionClosed(TObject *Sender, WORD Error);
  void __fastcall CliWSocketSessionConnected(TObject *Sender, WORD Error);
  void __fastcall BtnSalvaClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall TimerStateConnectionTimer(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  
  
  
  
  void __fastcall TimerRefreshTimer(TObject *Sender);

  void __fastcall AndroidGridDrawCell(TObject *Sender, int Col, int Row,
          TRect &Rect, TGridDrawState State);
  void __fastcall AndroidGridDblClick(TObject *Sender);
  
  
  void __fastcall BtnKeygenClick(TObject *Sender);
  void __fastcall BtnTestKeyClick(TObject *Sender);
  
  
  
private:	// User declarations

  bool FormIsOpen;
  void __fastcall SetCaselle();
  void __fastcall GetCaselle();

  char         MESSAPK[MAXMESSAG_ANDROID][111];          // Messaggeria di Sistema
                                                         // 001...300 solo per APK
                                                         // 301...500 per supervisore su invia ad APK
  char         ALLARMAPK[MAXALLARM_ANDROID][111];        // Copia della lista allarmi, compilata solo per APK
                                                         // in base alla lingua richiesta

  TWSocket    *CliWSocket[MAXSOCKET_ANDROID+1];
  TStaticText *TextSocket[MAXSOCKET_ANDROID+1];


  struct CONNECTION_DATA{
     long int     CodAccesso;
         BYTE     Mode;                                 // Modalità accesso
                                                        // 0 - solo manuali              (nessuna connessione a PC)
                                                        // 1 - stato tgv + manuali       (max connessioni = MAXAGV)
                                                        // 2 - monitoraggio impianto     (connessioni in base al CODICE ACCESSO)
     long int     TimeRemain;                           // Tempo residuo di connessione
          int     TcpCtrConnessClientScaduta;           // Controllo a Tempo sulla connessione del Client:
                                                        // Se NON si riceve niente per X tempo chiudiamo la connessione
                                                        // perchè potrebbe essere caduta la rete ( e NON si ripristinerebbe
                                                        // più la connessione TCP
  }ConnData[MAXSOCKET_ANDROID+1];


  int  TcpPort;                                         // Porta Locale su cui riceve i messaggi il SERVER TCP
  int  TcpTimeOutConnessClientRemoto;                   // Tempo di preset controllo connessione scaduta
  bool TcpTrasmissioneInCorso;                          // Flag di comando in corso
  int  TcpComandoRicevutoTCP;                           // Contatore pacchetti TCP ricevuti
  unsigned char TcpNumeroPacchetto;                     // Numero pacchetto
  unsigned char TcpTipoMessaggio;                       // Tipo di messaggio
  unsigned char TcpTipoComando;                         // Tipo di comando (in richiesta dati altrimenti = 0)

  struct CODICIMAG{
     WORD Lav;
     char Cod[21];
     char Des[31];
  }CodMag[TOTCODICIMAG+1];

  int OldMESSAPK;
  int OldALLARMAPK;
  BYTE ReleaseAPK;
  BYTE DispTecnoAPK;
  BYTE TypImpAPK;
  int LinguaAndroid;                                    // Indice lingua:
                                                        // 1 - Errore
                                                        // 2 - ITA
                                                        // 3 - ENG
                                                        // 4 - SPA

public:		// User declarations

  void ReadIniAndroid();
  void WriteIniAndroid();
  void StartServerTCPAndroid();
  void StopServerTCPAndroid(int num_socket);
  int  load_MESSAPK();
  int  load_ALLARMAPK(int num_agv);
  int  AcceptedConnection(int num_socket, BYTE mode, long int codice_accesso);
  int  RefreshServerTCPAndroid(BYTE Typ, char *all_mess);
  bool TcpComandoInEsecuzione;
  int  TcpSend(int num_socket, unsigned char *StringaTx,  unsigned int LenMessaggio, char* all_mess);
  int  TcpElaboraStringaRicevuta(unsigned char* rcv_string, unsigned char* NumPacchetto, unsigned char* TipoMess, unsigned char* TipoCmd, char* all_mess);
  int  TcpDispenserComandi(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, char* all_mess);

  //----------------------------------------------------
  // Funzione gestione compandi terminale ANDROID
  //----------------------------------------------------
  // -- stato impianto
  int  TcpComandoWriteCmd2(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess);
  // -- lista allarmi (PLC + PC)
  int  TcpComandoWriteCmd9(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess);
  // -- stato tutti TGV
  int  TcpComandoWriteCmd20(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess);
  // -- stato singolo TGV
  int  TcpComandoWriteCmd21(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess);
  // -- richiesta parametri cominicazione singolo TGV
  int  TcpComandoWriteCmd22(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess);
  // -- stato tutte le MAC
  int  TcpComandoWriteCmd30(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess);
  // -- stato singla MAC
  int  TcpComandoWriteCmd31(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess);
  // -- stato tutti i MAG
  int  TcpComandoWriteCmd40(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess);
  // -- stato tutti i BOX
  int  TcpComandoWriteCmd50(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess);
  // -- stato tutti i CODICI
  int  TcpComandoWriteCmd60(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess);
  // -- stato singolo CODICE (BOX)
  int  TcpComandoWriteCmd61(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess);
  // -- stato singolo CODICE (FOOD)
  int  TcpComandoWriteCmd62(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess);
  // -- stato tutte le CELLE
  int  TcpComandoWriteCmd70(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess);
  // -- stato singla CELLA
  int  TcpComandoWriteCmd71(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess);

  __fastcall TFormSrvTcpAndroid(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormSrvTcpAndroid *FormSrvTcpAndroid;
//---------------------------------------------------------------------------
#endif
