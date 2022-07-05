//---------------------------------------------------------------------------
#include <vcl.h>
#include <vcl.h>
#pragma hdrstop

#include "Main.h"
#include "Remoto.h"
#include "OmronETH.h"
#include "SrvTcpAndroid.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

#pragma link "WSocket"
#pragma resource "*.dfm"

TFormSrvTcpAndroid *FormSrvTcpAndroid;
//---------------------------------------------------------------------------
__fastcall TFormSrvTcpAndroid::TFormSrvTcpAndroid(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormSrvTcpAndroid::FormCreate(TObject *Sender)
{
  OldMESSAPK             = 0;       // default LINGUA ITALIANA
  OldALLARMAPK           = 0;       // default LINGUA ITALIANA
  LinguaAndroid          = 2;       // default LINGUA ITALIANA
  ReleaseAPK             = 0;       // Release applicazione APK
  DispTecnoAPK           = 0;       // flag di dispositivo TECNOF  
  TypImpAPK              = ABILITA_OPZIONI_ANDROID;  // tipo impianto impostato su APK
  TcpComandoRicevutoTCP  = 0;
  TcpTrasmissioneInCorso = false;
  TcpComandoInEsecuzione = false;
  memset(&CodMag,       0, sizeof(CodMag));
  memset(&ConnData[0],  0, sizeof(ConnData));
  memset(&MESSAPK[0],   NULL, sizeof(MESSAPK));
  memset(&ALLARMAPK[0], NULL, sizeof(ALLARMAPK));
  //----------------------------------------------
  // Rilettura dati comunicazione ANDROID
  //----------------------------------------------
  ReadIniAndroid();
  //----------------------------------------------
  // dimensionamento grigli in base a num record
  //----------------------------------------------
  AndroidGrid->ColWidths[ 0] = 30;
  AndroidGrid->ColWidths[ 1] = 45;
  AndroidGrid->ColWidths[ 2] = 75;
  AndroidGrid->ColWidths[ 3] = 65;
  AndroidGrid->ColWidths[ 4] = 85;
  AndroidGrid->ColWidths[ 5] = 50;
  AndroidGrid->ColWidths[ 6] = 30;
  AndroidGrid->ColWidths[ 7] = 30;
  AndroidGrid->ColWidths[ 8] = 30;
  AndroidGrid->ColWidths[ 9] = 25;
  AndroidGrid->ColWidths[10] = 60;
  AndroidGrid->ColWidths[11] = 90;
  AndroidGrid->ColWidths[12] = 90;


  AndroidGrid->Cells[ 0][0]  = "";
  AndroidGrid->Cells[ 1][0]  = "PORT";
  AndroidGrid->Cells[ 2][0]  = "IP";
  AndroidGrid->Cells[ 3][0]  = "ACCESS";
  AndroidGrid->Cells[ 4][0]  = "TOT PERMIS";
  AndroidGrid->Cells[ 5][0]  = "MODE";
  AndroidGrid->Cells[ 6][0]  = "ID";
  AndroidGrid->Cells[ 7][0]  = "TYP";
  AndroidGrid->Cells[ 8][0]  = "APK";
  AndroidGrid->Cells[ 9][0]  = "d.";
  AndroidGrid->Cells[10][0]  = "TYPTGV";
  AndroidGrid->Cells[11][0]  = "StrRX";
  AndroidGrid->Cells[12][0]  = "StrTX";

  //----------------------------------------------
  // Assegnazione dinamica degli oggetti SOCKET
  //----------------------------------------------
  CliWSocket[ 1] = CliWSocket1;
  CliWSocket[ 2] = CliWSocket2;
  CliWSocket[ 3] = CliWSocket3;
  CliWSocket[ 4] = CliWSocket4;
  CliWSocket[ 5] = CliWSocket5;
  CliWSocket[ 6] = CliWSocket6;
  CliWSocket[ 7] = CliWSocket7;
  CliWSocket[ 8] = CliWSocket8;
  CliWSocket[ 9] = CliWSocket9;
  CliWSocket[10] = CliWSocket10;
  CliWSocket[11] = CliWSocket11;
  CliWSocket[12] = CliWSocket12;
  CliWSocket[13] = CliWSocket13;
  CliWSocket[14] = CliWSocket14;
  CliWSocket[15] = CliWSocket15;
  CliWSocket[16] = CliWSocket16;
  CliWSocket[17] = CliWSocket17;
  CliWSocket[18] = CliWSocket18;
  CliWSocket[19] = CliWSocket19;
  CliWSocket[20] = CliWSocket20;

  //---------------------------
  // Attivazione del Timer
  //---------------------------
  TimerStateConnection->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormSrvTcpAndroid::FormActivate(TObject *Sender)
{
  FormIsOpen=true;
  //---------------------------------------
  // DISATTIVAZIONE PULSANTI PER
  // GESTIONE CODICI ACCESSO
  //---------------------------------------
  if(PcAlgo==false){
     BtnKeygen->Visible=false;
     BtnTestKey->Visible=false;
  }
  //---------------------------------------
  // Parametri SERVER SOCKET
  //---------------------------------------
  EditTimeOutSrv->Text     = TcpTimeOutConnessClientRemoto;
  EditTcpPort->Text        = TcpPort;
  //---------------------------------------
  // Attivazione del Timer
  //---------------------------------------
  TimerRefresh->Enabled=true;
  //---------------------------------------
  // Refresh della FORM
  //---------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormSrvTcpAndroid::FormClose(TObject *Sender, TCloseAction &Action)
{
  FormIsOpen=false;
  //---------------------------
  // Disattivazione del Timer
  //---------------------------
  TimerRefresh->Enabled=false;
  //TimerStateConnection->Enabled=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormSrvTcpAndroid::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if(Key==VK_ESCAPE){ FormSrvTcpAndroid->ModalResult=mrCancel; Close(); }
}
//---------------------------------------------------------------------------
//-------------------------------------------------
// SetCaselle()
//-------------------------------------------------
void __fastcall TFormSrvTcpAndroid::SetCaselle()
{
  int i;

  //---------------------------------------
  // Aggiornamento griglia Client
  //---------------------------------------
  for(i=1; i<=MAXSOCKET_ANDROID; i++){
     // stato connessione
     if(CliWSocket[i]->State==wsConnected){
        AndroidGrid->Cells[0][i] = 1;
        AndroidGrid->Cells[1][i] = SrvWSocket->Port;
     }
     else{
        AndroidGrid->Cells[0][i] = 0;
        AndroidGrid->Cells[1][i] = "";
        AndroidGrid->Cells[3][i] = "";
        AndroidGrid->Cells[4][i] = "";
        AndroidGrid->Cells[5][i] = "";
     }
     // INDIRIZZO IP
     AndroidGrid->Cells[2][i] = CliWSocket[i]->GetPeerAddr();
  }
}
//---------------------------------------------------------------------------
//-------------------------------------------------
// GetCaselle()
//-------------------------------------------------
void __fastcall TFormSrvTcpAndroid::GetCaselle()
{
  TcpTimeOutConnessClientRemoto = atoi(EditTimeOutSrv->Text.c_str());
  TcpPort                       = atoi(EditTcpPort->Text.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TFormSrvTcpAndroid::AndroidGridDrawCell(TObject *Sender, int Col, int Row, TRect &Rect, TGridDrawState State)
{
  TColor col;

  if(Row<1 || Row>MAXSOCKET_ANDROID) return;
  //----------------------------------------------------
  // Se il colore selezionato è il nero --> skip!
  //----------------------------------------------------
  switch(Col){
     case  0: if(break_Com[COM9]==true) col = clYellow;         // COMUNICAZIONE DISABILITATA
              else{
                 if(AndroidGrid->Cells[0][Row]==1) col = clLime;  // CONNESSIONE ATTIVA
                 if(AndroidGrid->Cells[0][Row]==0) col = clRed;   // CONNESSIONE DISATTIVATA
              }
              break;
     default: return;
  }
  //----------------------------------------------------
  // Colorazione del blocchetto
  //----------------------------------------------------
  AndroidGrid->Canvas->Brush->Color = col;
  AndroidGrid->Canvas->FillRect(Rect);
}
//---------------------------------------------------------------------------
void __fastcall TFormSrvTcpAndroid::BtnSalvaClick(TObject *Sender)
{
  GetCaselle();
  //---------------------------------------
  // Parametri SERVER SOCKET
  //---------------------------------------
  EditTimeOutSrv->Text = TcpTimeOutConnessClientRemoto;
  EditTcpPort->Text    = TcpPort;
  //---------------------------------------
  // Salvataggio impostazioni
  //---------------------------------------
  WriteIniAndroid();
}
//---------------------------------------------------------------------------
void __fastcall TFormSrvTcpAndroid::BtnKeygenClick(TObject *Sender)
{
  char mm[3]="";
  char gg[3]="";
  char ss[3]="";
  char dd[3]="";
  char buffer[51]="";
  long int int_codice=0;
  char str_codice[11]="";
  SYSTEMTIME TimeAttuale;
  TDateTime DataAttuale;
  TDateTime DataInizioAnno;
  int GiornoDellaSettimana;
  int pos, DiffTime;
  div_t Settimane;
  AnsiString Buff;
  AnsiString InputStringa;
  int durata;

  //-----------------------------------------------
  // Richiesta DURATA CODICE
  //-----------------------------------------------
  InputStringa = InputBox("Durata", "Inserire durata (max: 60gg):", "");
  if(InputStringa.IsEmpty() == true) return;

  durata = atoi(InputStringa.c_str());
  if(durata<1 || durata>60){
     ShowMessage("ATTENZIONE: durata non valida!");
     return;
  }
  //--------------------------------------------
  // Recupero la settimana dell'anno
  //--------------------------------------------
  GetLocalTime(&TimeAttuale);
  DataAttuale    = TDateTime(TimeAttuale.wYear, TimeAttuale.wMonth, TimeAttuale.wDay);   // data attuale
  DataInizioAnno = TDateTime(TimeAttuale.wYear, 1, 1);                                   // data inizio anno
  DiffTime       = DataAttuale - DataInizioAnno;
  Settimane      = div(DiffTime, 7); Settimane.quot++;                                  // calcolo settimane e giorni di scarto
  GiornoDellaSettimana = DataAttuale.DayOfWeek();                                       // recupero il giorno della settimana
  //--------------------------------------------
  // Conversione in stringa
  //--------------------------------------------
  sprintf(gg, "%02d", TimeAttuale.wDay);
  sprintf(mm, "%02d", TimeAttuale.wMonth);
  sprintf(ss, "%02d", Settimane.quot);
  sprintf(dd, "%02d", durata);
  //--------------------------------------------
  // Composizione stringa mescoltata
  //
  // aabbaabb
  // MGSDSDMG
  //--------------------------------------------
  sprintf(str_codice, "%c%c%c%c%c%c%c%c", mm[0], gg[0], ss[1], dd[1], ss[0], dd[0], mm[1], gg[1]);
  int_codice   = (long)(atol(str_codice)*GiornoDellaSettimana);
  //--------------------------------------------
  // Compilo gli AnsiString per il calcolo del
  // codice finale
  //--------------------------------------------
  Buff         = IntToStr(GiornoDellaSettimana);
  InputStringa = IntToStr(int_codice);
  //--------------------------------------------
  // Recupero la posizione nella quale inserire
  // il moltiplicatore del giorno prendendolo
  // dalla prima cifra della cifra calcoltata
  //--------------------------------------------
  pos = (int)(InputStringa.Length())+1;
  InputStringa.Insert(Buff, pos);
  //--------------------------------------------
  // Inserimento CODICE BASE
  //--------------------------------------------
  int_codice   = (long)(StrToInt(InputStringa) + CODICE_BASE_ANDROID);
  //--------------------------------------------
  // Visualizzazione risultato
  //--------------------------------------------
  sprintf(buffer, "%ld", int_codice);
  ShowMessage(buffer);
}
//---------------------------------------------------------------------------

void __fastcall TFormSrvTcpAndroid::BtnTestKeyClick(TObject *Sender)
{
  char mm[3]="";
  char gg[3]="";
  char ss[3]="";
  char dd[3]="";
  char data[15]="";
  char buffer[101]="";
  long int int_codice=0;
  long int time_start;
  char str_codice[11]="";
  SYSTEMTIME TimeAttuale;
  TDateTime DataRecuperata;
  TDateTime DataInizioAnno;
  int GiornoDellaSettimana1, GiornoDellaSettimana2;
  int pos, DiffTime;
  div_t Settimane;
  AnsiString Buff;
  AnsiString InputStringa;
  int  a, b, c, e;
  float aa, bb, cc, ee;
  long int durata, time_trascorso;

  memset(&gg[0], NULL, sizeof(gg));
  memset(&mm[0], NULL, sizeof(mm));
  memset(&ss[0], NULL, sizeof(ss));
  memset(&dd[0], NULL, sizeof(dd));
  //-----------------------------------------------
  // Richiesta DURATA CODICE
  //-----------------------------------------------
  InputStringa = InputBox("Codice attivazione", "Inserire codice (max.9 cifre):", "");
  if(InputStringa.IsEmpty() == true) return;
  int_codice = atol(InputStringa.c_str());
  if(int_codice<0 || int_codice>999999999){
     ShowMessage("(1) - ATTENZIONE: codice non valido!");
     return;
  }
  if(InputStringa.Length()<7){
     ShowMessage("(2) - ATTENZIONE: codice non valido!");
     return;
  }
  //-----------------------------------------------
  // Solo per codici != CODICE_CLIENTE
  //-----------------------------------------------
  if(int_codice!=CODICE_CLIENTE){
     //---------------------------------------------
     // Eliminazione codice base
     //---------------------------------------------
     int_codice = int_codice - CODICE_BASE_ANDROID;
     //---------------------------------------------
     // Estrapolazione moltiplicatore:
     // Giorno della settimana
     //---------------------------------------------
     InputStringa          = IntToStr(int_codice);
     pos                   = InputStringa.Length();
     Buff                  = InputStringa.SubString(pos, 1);
     GiornoDellaSettimana1 = StrToInt(Buff);
     InputStringa          = InputStringa.Delete(pos, 1); // eliminazine giorno settimana dal codice
     //---------------------------------------------
     // Divisione del valore ottenuto per il giorno
     // della settimana
     //---------------------------------------------
     int_codice = StrToInt(InputStringa) / GiornoDellaSettimana1;
     //---------------------------------------------
     // Ottengo stringa mescoltata
     //
     // aabbaabb
     // MGSDSDMG
     //---------------------------------------------
     sprintf(str_codice, "%08ld", int_codice);
     mm[0] = str_codice[0];
     gg[0] = str_codice[1];
     ss[1] = str_codice[2];
     dd[1] = str_codice[3];
     ss[0] = str_codice[4];
     dd[0] = str_codice[5];
     mm[1] = str_codice[6];
     gg[1] = str_codice[7];

     //---------------------------------------------
     // Verifica se il giorno recuperato
     // è coerente con i dati di Settimana e
     // Giorno della settimana
     //---------------------------------------------
     try{
        GetLocalTime(&TimeAttuale);
        DataRecuperata = TDateTime(TimeAttuale.wYear, (WORD)(atoi(mm)), (WORD)(atoi(gg)));    // data attuale
        DataInizioAnno = TDateTime(TimeAttuale.wYear, 1, 1);                                  // data inizio anno
        DiffTime       = DataRecuperata - DataInizioAnno;
        Settimane      = div(DiffTime, 7); Settimane.quot++;                                  // calcolo settimane e giorni di scarto
        GiornoDellaSettimana2 = DataRecuperata.DayOfWeek();                                   // recupero il giorno della settimana
     }
     catch(...){
        ShowMessage("(3) - ATTENZIONE: codice non valido!");
        return;
     }
     //---------------------------------------------
     // Controllo coerenza settimana
     //---------------------------------------------
     if(GiornoDellaSettimana1!=GiornoDellaSettimana2){
        ShowMessage("(5) - ATTENZIONE: codice non valido!");
        return;
     }
     if(atoi(ss)!=Settimane.quot){
        ShowMessage("(6) - ATTENZIONE: codice non valido!");
        return;
     }
     //---------------------------------------------
     // Se il codice risulta corretto verifico se
     // è valido anche come durata in secondi
     // dal 1/01/1970
     //---------------------------------------------
     durata = (long int)(atoi(dd) * 24 * 60 * 60);
     sprintf(data, "%02d/%02d/%04d", (WORD)(atoi(gg)), (WORD)(atoi(mm)), TimeAttuale.wYear);
     timechar_to_timeint(&time_start, &data[0], "00.00.01");    // Data/Ora fine giornata
     //---------------------------------------------
     // Calcolo tempo trascorso da attivazione
     // codice
     //---------------------------------------------
     time_trascorso = (time(NULL) - time_start);
     //---------------------------------------------
     // Verifica per quanti giorni è ancora valido
     //---------------------------------------------
     if(time_trascorso>durata){
        ee = (float)(time_trascorso-durata);
        e  = (int)(ee/86400);           // giorni
        aa = (ee-(e*86400));
        a  = (int)(aa/3600);            // ore
        bb = (aa-(a*3600));
        b  = (int)(bb/60);              // minuti
        cc = (aa -(a*3600) - (b*60));
        c  = (int)(cc);                 // secondi
        sprintf(buffer, "codice scaduto da %02dg, %02do, %02dm, %02ds...", e, a, b, c);
     }
     else{
        ee = (float)(durata-time_trascorso);
        e  = (int)(ee/86400);           // giorni
        aa = (ee-(e*86400));
        a  = (int)(aa/3600);            // ore
        bb = (aa-(a*3600));
        b  = (int)(bb/60);              // minuti
        cc = (aa -(a*3600) - (b*60));
        c  = (int)(cc);                 // secondi
        sprintf(buffer, "codice valido ancora per %02dg, %02do, %02dm, %02ds...", e, a, b, c);
     }
     ShowMessage(buffer);
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormSrvTcpAndroid::AndroidGridDblClick(TObject *Sender)
{
  int indice;

  //-------------------------
  // Recupero socket in close
  //-------------------------
  indice = AndroidGrid->Row;
  //-------------------------
  // Chiusura connessione
  //-------------------------
  StopServerTCPAndroid(indice);
}
//---------------------------------------------------------------------------

//-------------------------------
//      load_MESSAPK()
//-------------------------------
//  Legge il file MESSAPK.LNG,e legge la lista
//  lista dei messaggi per l'applicazione
//  ANDROID in base alla lingua richiesta
//
// ------------------------------------------------
int TFormSrvTcpAndroid::load_MESSAPK()
/****************/
{
  FILE *ptrfile;
  AnsiString Buff;
  char file_lingua[21];
  char buffer[111];
  int   j, i;
  int   pos_sost, pos, err;

  err=0;
  //-------------------------------------------
  // Rilettura file solo se ligua cambiata
  // da ultima lettura
  //-------------------------------------------
  if(OldMESSAPK==LinguaAndroid) return 0;
  //-------------------------------------------
  // Inizializzazione variabili
  //-------------------------------------------
  memset(&MESSAPK[0],  NULL, sizeof(MESSAPK));
  strcpy(file_lingua, "MESSAPK.ITA");
  switch(LinguaAndroid){
     case  2: strcpy(file_lingua, "MESSAPK.ITA");
              break;
     case  3: strcpy(file_lingua, "MESSAPK.ENG");
              break;
     case  4: strcpy(file_lingua, "MESSAPK.SPA");
              break;
     case  5: strcpy(file_lingua, "MESSAPK.TED");
              break;
     case  6: strcpy(file_lingua, "MESSAPK.FRA");
              break;
     case  7: strcpy(file_lingua, "MESSAPK.CEC");
              break;
     case  8: strcpy(file_lingua, "MESSAPK.NL");
              break;
     case  9: strcpy(file_lingua, "MESSAPK.RUS");
              break;
     case 10: strcpy(file_lingua, "MESSAPK.POL");
              break;
     case 11: strcpy(file_lingua, "MESSAPK.POR");
              break;
     case 12: strcpy(file_lingua, "MESSAPK.ROM");
              break;
  }
  //-------------------------------------------
  // APERTURA FILE
  //-------------------------------------------
  i=1;
  ptrfile = fopen(file_lingua, "rb");
  if (ptrfile==0){
     //------------------------------------------
     // In caso di errore lettura file in lingua
     // vengono restituiti i messaggi in ENG
     //------------------------------------------
     strcpy(file_lingua, "MESSAPK.ENG");
     ptrfile = fopen(file_lingua, "rb");
     if(ptrfile==0) return 1;
  }
  //-------------------------------------------
  // LETTURA MESSAGGI
  //-------------------------------------------
  do{
     err = Read_Line(ptrfile, buffer, 101);
     Copy( buffer, 5, 100, &MESSAPK[i][0] );
     i++;
  }while(err && i< MAXMESSAG_ANDROID);
  //-------------------------------------------
  // CHIUSURA FILE
  //-------------------------------------------
  fclose(ptrfile);
  //-------------------------------------------
  // Sostituzione della '@' con un '\n' = LF
  //-------------------------------------------
  i=0;
  for(i=0; i<MAXMESSAG_ANDROID; i++){
     pos_sost=pos=0;
     Buff = MESSAPK[i];
     for(j=0; j<10; j++){                // ammesse max. 10 '@' a messaggio
        pos  = Buff.Pos("@");
        if(pos==0) break;
        pos_sost=pos_sost+pos;           // posizioni a cui si trovano '@'
        MESSAPK[i][pos_sost-1] = 0x0A;   // '\n' = LF
        Buff = Buff.SubString(pos+1, Buff.Length());
     }
  }
  //-------------------------------------------
  // SALVATAGGIO ULTIMA LINGUA LETTA
  //-------------------------------------------
  OldMESSAPK=LinguaAndroid;

  return 0;
}
//---------------------------------------------------------------------------

//-------------------------------
//      load_ALLARMAPK()
//-------------------------------
//  Legge il file ALLARM.LNG,e legge la lista
//  lista dei messaggi per l'applicazione
//  ANDROID in base alla lingua richiesta
//
// ------------------------------------------------
int TFormSrvTcpAndroid::load_ALLARMAPK(int num_agv)
/****************/
{
  FILE *ptrfile;
  char buffer[111];
  char file_all1[21]="";
  char file_all2[21]="";
  short int pollicino;
  int   i=0;
  int   err;

  //-------------------------------------------
  // Rilettura file solo se ligua cambiata
  // da ultima lettura
  //-------------------------------------------
  if(OldALLARMAPK==LinguaAndroid) return 0;
  //-----------------------------------------
  // Selezione nome file in base alla lingua
  // e alla versione della SCHEDA POLLICINO
  //-----------------------------------------
  err=0;
  memset(&ALLARMAPK[0], NULL, sizeof(ALLARMAPK));
  pollicino=AGV[num_agv]->dm1000_versione_allarmi;
  strcpy(file_all1, "ALLARM.ITA");
  strcpy(file_all2, "ALLARM.ITA");
  switch(LinguaAndroid){
     case  2: if(pollicino!=0) sprintf(file_all1, "ALLARM%d.ITA", pollicino);
              else             strcpy(file_all1,  "ALLARM.ITA");
              strcpy(file_all2, "ALLARM.ITA");
              break;
     case  3: if(pollicino!=0) sprintf(file_all1, "ALLARM%d.ENG", pollicino);
              else             strcpy(file_all1,  "ALLARM.ENG");
              strcpy(file_all2, "ALLARM.ENG");
              break;
     case  4: if(pollicino!=0) sprintf(file_all1, "ALLARM%d.SPA", pollicino);
              else             strcpy(file_all1,  "ALLARM.SPA");
              strcpy(file_all2, "ALLARM.SPA");
              break;
     case  5: if(pollicino!=0) sprintf(file_all1, "ALLARM%d.TED", pollicino);
              else             strcpy(file_all1,  "ALLARM.TED");
              strcpy(file_all2, "ALLARM.TED");
              break;
     case  6: if(pollicino!=0) sprintf(file_all1, "ALLARM%d.FRA", pollicino);
              else             strcpy(file_all1,  "ALLARM.FRA");
              strcpy(file_all2, "ALLARM.FRA");
              break;
     case  7: if(pollicino!=0) sprintf(file_all1, "ALLARM%d.CEC", pollicino);
              else             strcpy(file_all1,  "ALLARM.CEC");
              strcpy(file_all2, "ALLARM.CEC");
              break;
     case  8: if(pollicino!=0) sprintf(file_all1, "ALLARM%d.NL", pollicino);
              else             strcpy(file_all1,  "ALLARM.NL");
              strcpy(file_all2, "ALLARM.NL");
              break;
     case  9: if(pollicino!=0) sprintf(file_all1, "ALLARM%d.RUS", pollicino);
              else             strcpy(file_all1,  "ALLARM.RUS");
              strcpy(file_all2, "ALLARM.RUS");
              break;
     case 10: if(pollicino!=0) sprintf(file_all1, "ALLARM%d.POL", pollicino);
              else             strcpy(file_all1,  "ALLARM.POL");
              strcpy(file_all2, "ALLARM.POL");
              break;
     case 11: if(pollicino!=0) sprintf(file_all1, "ALLARM%d.POR", pollicino);
              else             strcpy(file_all1,  "ALLARM.POR");
              strcpy(file_all2, "ALLARM.POR");
              break;
     case 12: if(pollicino!=0) sprintf(file_all1, "ALLARM%d.ROM", pollicino);
              else             strcpy(file_all1,  "ALLARM.ROM");
              strcpy(file_all2, "ALLARM.ROM");
              break;
     default: break;
  }

  //-------------------------------------------
  // APERTURA FILE
  //-------------------------------------------
  ptrfile = fopen(file_all1, "rb");
  if (ptrfile==0){
     ptrfile = fopen(file_all2, "rb");
     if (ptrfile==0) return 1;
  }
  do{
     err = Read_Line(ptrfile, buffer, 101);
     Copy( buffer, 1, 100, &ALLARMAPK[i][0] );
     i++;
  }while(err && i< MAXALLARM_ANDROID);
  //-------------------------------------------
  // CHIUSURA FILE
  //-------------------------------------------
  fclose(ptrfile);
  //-------------------------------------------
  // SALVATAGGIO ULTIMA LINGUA LETTA
  //-------------------------------------------
  OldALLARMAPK=LinguaAndroid;
  return 0;
}
//---------------------------------------------------------------------------

//-------------------------------
//   ReadIniAndroid()
//-------------------------------
// Lettura del file *.ini di sistema
//
void TFormSrvTcpAndroid::ReadIniAndroid()
{
  TIniFile *IniFile;
  AnsiString FPath;
  AnsiString FIniFileName;

  FPath        = ExtractFilePath(ParamStr(0));
  FIniFileName = FPath + "SrvAndroid.ini";
  IniFile      = new TIniFile(FIniFileName);


  //-----------------------------------------------------
  // Parametrizzazione interfaccia TCP
  //-----------------------------------------------------
  TcpPort                       = IniFile->ReadInteger("SOCKET", "TcpPort",                       1000);
  TcpTimeOutConnessClientRemoto = IniFile->ReadInteger("SOCKET", "TcpTimeOutConnessClientRemoto", 60);


  delete IniFile;
}
//---------------------------------------------------------------------------
//-------------------------------
//   WriteIniAndroid()
//-------------------------------
// Scrittura del file *.ini di sistema
//
void TFormSrvTcpAndroid::WriteIniAndroid()
{
  TIniFile *IniFile;
  AnsiString FPath;
  AnsiString FIniFileName;

  FPath        = ExtractFilePath(ParamStr(0));
  FIniFileName = FPath + "SrvAndroid.ini";
  IniFile      = new TIniFile(FIniFileName);

  //-----------------------------------------------------
  // Parametrizzazione interfaccia TCP
  //-----------------------------------------------------
  IniFile->WriteInteger("SOCKET", "TcpPort", TcpPort);
  IniFile->WriteInteger("SOCKET", "TcpTimeOutConnessClientRemoto", TcpTimeOutConnessClientRemoto);


  delete IniFile;
}
//---------------------------------------------------------------------------

//===========================================================================
// SERVER TCP
// Funzioni Gstione RICEZIONE <-- messaggi da SUPERVISORE
//===========================================================================
//
//---------------------------------------------------------------------------
// 1 SrvWsocket sempre in ascolto
// 2 CliWsocket per colloquiare con un CLIENT
// 3 TmpWsocket Temporaneo solo per rispondere sessione già occupata
//---------------------------------------------------------------------------
//
//-------------------------------------------------
//   StartServerTCP()
//-------------------------------------------------
void TFormSrvTcpAndroid::StartServerTCPAndroid()
{
  char all_mess[101]="";
  
  //---------------------------------------------------------------------
  // Comunicazione socket disattivata
  //---------------------------------------------------------------------
  if(break_Com[COM9]==true) return;
  //---------------------------------------------------------------------
  // Socket già in ascolto --> skip! (evita il riavvio)
  //---------------------------------------------------------------------
  if(SrvWSocket->State==wsListening) return;
  //---------------------------------------------------------------------
  // Refresh delle strutture di scambio dati con i dispositivi ANDROID
  //---------------------------------------------------------------------
  RefreshServerTCPAndroid(0x01, all_mess);
  //---------------------------------------------------------------------
  // Sever TCP Ethernet
  //---------------------------------------------------------------------
  SrvWSocket->Port  = TcpPort;
  SrvWSocket->Proto = "tcp";
  SrvWSocket->Addr  = "0.0.0.0";
  try {
     SrvWSocket->Listen();
  }
  catch (const ESocketException &E) {
     // The socket is probably already in use
     if (strncmp(E.Message.c_str(), "Error 10048", 11)==0){
        if(FormIsOpen==true) TextRxTCP->Caption = "SOCKET ANDROID already running as server";
        else FormMain->SBarImpianto->Panels->Items[0]->Text = "SOCKET ANDROID already running as server";
     }else{
        throw;
     }
  }
}
//---------------------------------------------------------------------------
//-------------------------------------------------
//   StopServerTCP()
//-------------------------------------------------
void TFormSrvTcpAndroid::StopServerTCPAndroid(int num_socket)
{
  int i;

  //----------------------------
  // Chiusura singolo Socket
  //----------------------------
  if(num_socket>0 && num_socket<=MAXSOCKET_ANDROID){
     if(CliWSocket[num_socket]->State==wsClosed) return;
     //----------------------------
     // Abort della connessione
     //----------------------------
     CliWSocket[num_socket]->Abort();
     //----------------------------
     // Chiusura della connessione
     //----------------------------
     CliWSocket[num_socket]->Close();
     //----------------------------
     // Reset Codice accesso
     //----------------------------
     memset(&ConnData[num_socket], 0, sizeof(CONNECTION_DATA));
  }
  //----------------------------
  // Se l'indice comunicato
  // alla funzione non è valido
  // vegono chiusi tutti
  //----------------------------
  else{
     //----------------------------
     // STOP SERVER
     // solo in caso di comando di
     // chiusura di tutti i socket
     //----------------------------
     if(SrvWSocket->State!=wsClosed){
        //----------------------------
        // Abort della connessione
        //----------------------------
        SrvWSocket->Abort();
        //----------------------------
        // Chiusura della connessione
        //----------------------------
        SrvWSocket->Close();
     }
     //----------------------------
     // STOP DI TUTTI I CLIENTs
     // importante questa sequenza
     //----------------------------
     for(i=1; i<=MAXSOCKET_ANDROID; i++){
        if(CliWSocket[i]->State==wsClosed) continue;
        //----------------------------
        // Abort della connessione
        //----------------------------
        CliWSocket[i]->Abort();
        //----------------------------
        // Chiusura della connessione
        //----------------------------
        CliWSocket[i]->Close();
        //----------------------------
        // Reset Codice accesso
        //----------------------------
        memset(&ConnData[i], 0, sizeof(CONNECTION_DATA));
     }
  }
}
//---------------------------------------------------------------------------
//----------------------------------------------------------------------------
//   SrvWSocketSessionAvailable()
//
// Il SRVER accetta una nuova SESSIONE
// This event is triggered when we - as a server - have received a client
// connection request. We must accept the connection. Two cases: we are
// already busy with another client, or this is the first client connecting.
//----------------------------------------------------------------------------
void __fastcall TFormSrvTcpAndroid::SrvWSocketSessionAvailable(TObject *Sender, WORD Error)
{
  int Client;

  //---------------------------------------------------------------
  // Verifica che NON sia già connesso come CLIENT
  // Apre un socket temporaneo per rispondere che è già occupato
  //---------------------------------------------------------------
  try{
     //-------------------------------------------
     // Cerco un socket libero
     //-------------------------------------------
     for(Client=1; Client<=MAXSOCKET_ANDROID; Client++){
        //------------------------------------------
        // Appena trovo un client non connesso
        // --> skip!
        //------------------------------------------
        if(CliWSocket[Client]->State!=wsConnected) break;
     }
     //-------------------------------------------
     // Se i socket sono tutti bloccati --> skip!
     //-------------------------------------------
     if(CliWSocket[Client]->State==wsConnected){
        // We are already busy with a client. Use the TmpWSocket to send a
        // busy message to the second client. Display a message to notify
        // the user that someone is trying to contact him.
        TmpWSocket->HSocket = SrvWSocket->Accept();
        TmpWSocket->SendStr("Busy! Try later...");
        TmpWSocket->Close();
        return;
     }
     //---------------------------------------------------------
     // This is our first client trying to connect, we accept
     //
     // Accetta la connessione con Host e si appoggia
     // sul socket   CliWsocket   per colloquiare
     //---------------------------------------------------------
     CliWSocket[Client]->HSocket = SrvWSocket->Accept();
     CliWSocket[Client]->SendStr("Connection is OK...");
     //---------------------------------------------------------
     // Sull'accettazione di una nuova connessione azzero
     // per sicurezza il codice di accesso abbianato. Questo
     // verifica che venga fatto correttamente il controllo
     // sul nuovo codice di accesso ed evita di controllare
     // quello vecchio abbinato al medesimo socket client
     //---------------------------------------------------------
     memset(&ConnData[Client], 0, sizeof(CONNECTION_DATA));
     //---------------------------------------------------------
     // Refresh della form solo se aperta
     //---------------------------------------------------------
     if(FormIsOpen==true) TextConnection->Caption = "(" + IntToStr(Client) + ")" + " Connected with " + CliWSocket[Client]->GetPeerAddr();
  }
  catch(const ESocketException &E) {
     FormMain->SBarImpianto->Panels->Items[0]->Text = E.Message;
  };
}
//---------------------------------------------------------------------------
void __fastcall TFormSrvTcpAndroid::CliWSocketDataAvailable(TObject *Sender, WORD Error)
{
  int  a, b, c, e;
  float aa, bb, cc, ee;
  int  indice, err=0;
  int  LenMessaggioRx;
  char RcvBuf[2000];
  char all_mess[200];
  char buffer[501];
  AnsiString Buff;
  unsigned char  NumPacchetto, TipoMessaggio, TipoComando;

  TcpComandoInEsecuzione=true;
  TcpComandoRicevutoTCP++;
  if(TcpComandoRicevutoTCP>30000) TcpComandoRicevutoTCP=0;
  //--------------------------------------------------
  // Recupero l'indice del socket
  //--------------------------------------------------
  indice = ((TWSocket *)Sender)->Tag;
  //---------------------------------------------------------------------------
  // Comando intercettato
  //---------------------------------------------------------------------------
//FormMain->ComandoIntercettato=true;
//FormMain->ErroreGenericoServer=false;
  ConnData[indice].TcpCtrConnessClientScaduta=0; // -> Azzera il Ctr di controllo connessione scaduta
  //---------------------------------------------------------------------------
  // A) Ricezione dati
  //---------------------------------------------------------------------------
  LenMessaggioRx = CliWSocket[indice]->Receive(&RcvBuf[0], sizeof(RcvBuf) - 1);
  if(LenMessaggioRx >500 ){TcpComandoInEsecuzione=false; return;}
  if(LenMessaggioRx <= 0 ){TcpComandoInEsecuzione=false; return;}
  RcvBuf[LenMessaggioRx] = 0;

  //---------------------------------------------------------------------------
  // B) Se finestra aperta --> visualizza dati ricevuti
  //---------------------------------------------------------------------------
  err = TcpElaboraStringaRicevuta((unsigned char*) RcvBuf, &NumPacchetto, &TipoMessaggio, &TipoComando, all_mess);

  //-----------------------------------------------
  // Stampa Stringa (solo prima parte)
  //-----------------------------------------------
  if(!err){
     Buff = CliWSocket[indice]->GetPeerAddr();
     sprintf(buffer, "%s: %d - [%X][%X][%X][%X][%X][%X]", Buff.c_str(), TcpComandoRicevutoTCP, RcvBuf[0], RcvBuf[1], RcvBuf[2], RcvBuf[3], RcvBuf[4], RcvBuf[5]);
     //-----------------------------------------------
     // Archiviazione su file
     //-----------------------------------------------
     file_cronologico_generico( "ServerClientTcp.LOG", buffer, 50000L);
  }

  //---------------------------------------------------------------------------
  // Aggiornamento finestra
  //---------------------------------------------------------------------------
  if(FormIsOpen==true){
     TextRxTCP->Caption = buffer;
     CmdRxTCP->Caption  = "";
     if( err==0 ){
        sprintf(buffer, "IdxMess: [%X], TypMess: [%X]", NumPacchetto, TipoMessaggio);
        CmdRxTCP->Caption  = buffer;
        //--------------------------------------------------
        // GESTIONI MODALITA' ACCESSO
        // Manuali pari al numero dei TGV dell'impianto
        //--------------------------------------------------
        if(ConnData[indice].Mode==MODE_MANUALS_ANDROID || ConnData[indice].Mode==MODE_STATE_TGV_ANDROID){
           AndroidGrid->Cells[3][indice] = "STANDARD";
           AndroidGrid->Cells[4][indice] = MAXAGV;
           if(ConnData[indice].Mode==MODE_MANUALS_ANDROID  ) AndroidGrid->Cells[5][indice] = "MANUAL";
           if(ConnData[indice].Mode==MODE_STATE_TGV_ANDROID) AndroidGrid->Cells[5][indice] = "TGV";
        }
        //--------------------------------------------------
        // GESTIONI MODALITA' ACCESSO
        // In base al codice di accesso
        //--------------------------------------------------
        else{
           if(ConnData[indice].CodAccesso==CODICE_CLIENTE){
              AndroidGrid->Cells[3][indice]="CLIENTE";
              AndroidGrid->Cells[4][indice]="no limit";
           }
           else{
              AndroidGrid->Cells[3][indice]=ConnData[indice].CodAccesso;
              //-----------------------------------------------
              // Visualizzazione tempo rimasto
              //-----------------------------------------------
              ee = (float)(ConnData[indice].TimeRemain);
              e  = (int)(ee/86400);           // giorni
              aa = (ee-(e*86400));
              a  = (int)(aa/3600);            // ore
              bb = (aa-(a*3600));
              b  = (int)(bb/60);              // minuti
              cc = (aa -(a*3600) - (b*60));
              c  = (int)(cc);                 // secondi
              sprintf(buffer, "%02dgg %02d:%02d:%02d", e, a, b, c);
              AndroidGrid->Cells[4][indice] = buffer;
           }
           //--------------------------------------------------
           // MODALITA' DI ACCESSO
           //--------------------------------------------------
           AndroidGrid->Cells[5][indice]="PLANT";
        }
        AndroidGrid->Cells[ 6][indice]=TcpComandoRicevutoTCP;
        AndroidGrid->Cells[ 7][indice]=TipoMessaggio;
        sprintf(buffer, "[%X][%X][%X][%X][%X][%X]", RcvBuf[0], RcvBuf[1], RcvBuf[2], RcvBuf[3], RcvBuf[4], RcvBuf[5]);
        AndroidGrid->Cells[11][indice]=buffer;
        //--------------------------------------------------
        // Versione APK e tipo di dispositivio
        //--------------------------------------------------
        AndroidGrid->Cells[ 8][indice] = "V." + IntToStr((int)(ReleaseAPK));
        AndroidGrid->Cells[ 9][indice] = IntToStr((int)(DispTecnoAPK));
        //--------------------------------------------------
        // TIPO IMPIANTO APK
        //--------------------------------------------------
        switch(TypImpAPK){
           case  1: strcpy(buffer, "BOX2006"); break;
           case  2: strcpy(buffer, "BOX2000"); break;
           case  3: strcpy(buffer, "PAL2000"); break;
           case  4: strcpy(buffer, "ISM2000"); break;
           case  5: strcpy(buffer, "TGVFOOD"); break;
           case  6: strcpy(buffer, "TGVSPEC"); break;
           default: strcpy(buffer, "uncknow"); break;
        }
        AndroidGrid->Cells[10][indice] = buffer;
     }
     else{
        CmdRxTCP->Caption  = all_mess;
     }

  }

  //---------------------------------------------------------------------------
  // Se c'è una Trasmissione già in atto --> skip!
  //---------------------------------------------------------------------------
  if(TcpTrasmissioneInCorso==true){
     if(FormIsOpen==true) CmdRxTCP->Caption = "WAIT - Trasmission already on...";
     TcpComandoInEsecuzione=false;
     return;
  }

  //---------------------------------------------------------------------------
  // C) Risposta
  //---------------------------------------------------------------------------
  if( err==0 ) err = TcpDispenserComandi(indice, (unsigned char*) RcvBuf, LenMessaggioRx, all_mess );
  if( err ){
     file_cronologico_generico("TCPComunicationError.txt", all_mess, 50000L);
  }
  //---------------------------------------------------------------------------
  // D) Memorizzo un avvenuto errore
  //---------------------------------------------------------------------------
  if(!err) TcpComandoInEsecuzione=false;
  if( err){
//   FormMain->ComandoIntercettato=false;
//   FormMain->ErroreGenericoServer=true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormSrvTcpAndroid::CliWSocketSessionConnected(TObject *Sender, WORD Error)
{
  if(FormIsOpen==true){
     if( Error!=0 && Error==WSAECONNREFUSED) TextRxTCP->Caption = "No server available";
     if( Error!=0 && Error==WSAECONNREFUSED) TextRxTCP->Caption = "Can't connect, error #" + IntToStr(Error);
     if( Error==0                          ) TextRxTCP->Caption = "Connected";
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormSrvTcpAndroid::CliWSocketSessionClosed(TObject *Sender, WORD Error)
{
  int indice=0;

  //--------------------------------------------------
  // Recupero l'indice del socket in chiusura
  //--------------------------------------------------
  indice = ((TWSocket *)Sender)->Tag;
  //--------------------------------------------------
  // Visualizzazione del socket in chiusura
  //--------------------------------------------------
  if(FormIsOpen==true){
     TextRxTCP->Caption = "(" + IntToStr(indice) + ")" + " Disconnect with " + CliWSocket[indice]->GetPeerAddr();
  }
}
//---------------------------------------------------------------------------
//-----------------------------------------------------
//   TcpElaboraStringaRicevuta()
//
// Verifica il messaggio ricevuto controlla:
// - Recupera il numero di pacchetto
// - Recupera il tipo di messaggio
// - Recupera la tabella
// - Recupera il tipo comando
//
//-----------------------------------------------------
int TFormSrvTcpAndroid::TcpElaboraStringaRicevuta( unsigned char* rcv_string, unsigned char* NumPacchetto, unsigned char* TipoMess, unsigned char* TipoCmd, char* all_mess)
{
  WORD err=0;

  //-------------------------------------------
  // Inizializza varibili
  //-------------------------------------------
  strcpy( all_mess, "" );
  //-------------------------------------------
  // Controllo Stringa ricevuta
  //-------------------------------------------
  if( err==0 ){
     //--------------------------------------------------------
     // Recupero Numero pacchetto
     // Recupero Tipo Messaggio
     // Recupero Lunghezza calcolata
     //--------------------------------------------------------
     *NumPacchetto = rcv_string[ 0];
     *TipoMess     = rcv_string[ 1];
     *TipoCmd      = rcv_string[ 2];
  }

  return 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

int TFormSrvTcpAndroid::TcpSend(int num_socket, unsigned char *StringaTx,  unsigned int LenMessaggio, char* all_mess)
{
  char buffer[101]="";
  //-------------------------------------------
  // Inizializza varibili
  //-------------------------------------------
  strcpy( all_mess, "" );
  //--------------------------------------------------
  // Send Stringa
  //--------------------------------------------------
  //if (WSocketSend->State == wsConnected){
  if (CliWSocket[num_socket]->State == wsConnected){
     //WSocketSend->Send((char *) StringaTx, LenMessaggio);
     CliWSocket[num_socket]->Send((char *) StringaTx, LenMessaggio);
     //--------------------------------------------------
     // Finestra aperta
     //--------------------------------------------------
     if(FormIsOpen==true){
        sprintf(buffer, "[%X][%X][%X][%X][%X][%X]", StringaTx[0], StringaTx[1], StringaTx[2], StringaTx[3], StringaTx[4], StringaTx[5]);
        AndroidGrid->Cells[12][num_socket]=buffer;
     }
  }else{
     sprintf(all_mess, "SERVER ANDROID TCP: No Remote Server connected ");
     return 1;
  }
  return 0;
}
//---------------------------------------------------------------------------

//-----------------------------------------------------
//   DispenserComandi()
//-----------------------------------------------------
int  TFormSrvTcpAndroid::TcpDispenserComandi(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, char* all_mess)
{
  int err=0;

  if( break_Com[COM9]==true ) return 0;
  //------------------------------------------
  // Elaborazione stringa ricevuta
  //------------------------------------------
  err = TcpElaboraStringaRicevuta((unsigned char*) StringaRicevuta, &TcpNumeroPacchetto, &TcpTipoMessaggio, &TcpTipoComando, all_mess);

  switch ( TcpTipoMessaggio ) {
     case  CMD_02_PARAMETRI_IMP:      TcpTrasmissioneInCorso=true;
                                      //----------------------------------------------
                                      // Nel caso del CMD2 il "TcpTipoComando"
                                      // coincide con la lingua richiesta
                                      //----------------------------------------------
                                      LinguaAndroid = (int)(TcpTipoComando + 2);     // sommo "2" per uniformità con variabile "IndiceMessaggi"
                                                                                     // utilizzata dall'applicazione Box2006
                                      //----------------------------------------------
                                      // Grestione comando
                                      //----------------------------------------------
                                      TcpComandoWriteCmd2(num_socket, StringaRicevuta, LenMessaggioRx, TcpNumeroPacchetto, TcpTipoMessaggio, all_mess);
                                      TcpTrasmissioneInCorso=false;
                                      break;
     case  CMD_09_MESSAGGERIA:        TcpTrasmissioneInCorso=true;
                                      //----------------------------------------------
                                      // Grestione comando
                                      //----------------------------------------------
                                      TcpComandoWriteCmd9(num_socket, StringaRicevuta, LenMessaggioRx, TcpNumeroPacchetto, TcpTipoMessaggio, all_mess);
                                      TcpTrasmissioneInCorso=false;
                                      break;
     case  CMD_20_STATO_ALL_TGV:      TcpTrasmissioneInCorso=true;
                                      //----------------------------------------------
                                      // Grestione comando
                                      //----------------------------------------------
                                      TcpComandoWriteCmd20(num_socket, StringaRicevuta, LenMessaggioRx, TcpNumeroPacchetto, TcpTipoMessaggio, all_mess);
                                      TcpTrasmissioneInCorso=false;
                                      break;
     case  CMD_21_STATO_ONE_TGV:      TcpTrasmissioneInCorso=true;
                                      //----------------------------------------------
                                      // Grestione comando
                                      //----------------------------------------------
                                      TcpComandoWriteCmd21(num_socket, StringaRicevuta, LenMessaggioRx, TcpNumeroPacchetto, TcpTipoMessaggio, all_mess);
                                      TcpTrasmissioneInCorso=false;
                                      break;
     case  CMD_22_PARAMETRI_COM_TGV:  TcpTrasmissioneInCorso=true;
                                      //----------------------------------------------
                                      // Grestione comando
                                      //----------------------------------------------
                                      TcpComandoWriteCmd22(num_socket, StringaRicevuta, LenMessaggioRx, TcpNumeroPacchetto, TcpTipoMessaggio, all_mess);
                                      TcpTrasmissioneInCorso=false;
                                      break;
     case  CMD_30_STATO_ALL_MAC:      TcpTrasmissioneInCorso=true;
                                      //----------------------------------------------
                                      // Grestione comando
                                      //----------------------------------------------
                                      TcpComandoWriteCmd30(num_socket, StringaRicevuta, LenMessaggioRx, TcpNumeroPacchetto, TcpTipoMessaggio, all_mess);
                                      TcpTrasmissioneInCorso=false;
                                      break;
     case  CMD_31_STATO_ONE_MAC:      TcpTrasmissioneInCorso=true;
                                      //----------------------------------------------
                                      // Grestione comando
                                      //----------------------------------------------
                                      TcpComandoWriteCmd31(num_socket, StringaRicevuta, LenMessaggioRx, TcpNumeroPacchetto, TcpTipoMessaggio, all_mess);
                                      TcpTrasmissioneInCorso=false;
                                      break;
     case  CMD_40_STATO_ALL_MAG:      TcpTrasmissioneInCorso=true;
                                      //----------------------------------------------
                                      // Grestione comando
                                      //----------------------------------------------
                                      TcpComandoWriteCmd40(num_socket, StringaRicevuta, LenMessaggioRx, TcpNumeroPacchetto, TcpTipoMessaggio, all_mess);
                                      TcpTrasmissioneInCorso=false;
                                      break;
     case  CMD_50_STATO_ONE_BOX:      TcpTrasmissioneInCorso=true;
                                      //----------------------------------------------
                                      // Grestione comando
                                      //----------------------------------------------
                                      TcpComandoWriteCmd50(num_socket, StringaRicevuta, LenMessaggioRx, TcpNumeroPacchetto, TcpTipoMessaggio, all_mess);
                                      TcpTrasmissioneInCorso=false;
                                      break;
     case  CMD_60_STATO_ALL_COD:      TcpTrasmissioneInCorso=true;
                                      //----------------------------------------------
                                      // Grestione comando
                                      //----------------------------------------------
                                      TcpComandoWriteCmd60(num_socket, StringaRicevuta, LenMessaggioRx, TcpNumeroPacchetto, TcpTipoMessaggio, all_mess);
                                      TcpTrasmissioneInCorso=false;
                                      break;
     case  CMD_61_STATO_ONE_COD:      TcpTrasmissioneInCorso=true;
                                      //----------------------------------------------
                                      // Grestione comando
                                      //----------------------------------------------
                                      TcpComandoWriteCmd61(num_socket, StringaRicevuta, LenMessaggioRx, TcpNumeroPacchetto, TcpTipoMessaggio, all_mess);
                                      TcpTrasmissioneInCorso=false;
                                      break;
     case  CMD_62_STATO_ONE_COD:      TcpTrasmissioneInCorso=true;
                                      //----------------------------------------------
                                      // Grestione comando
                                      //----------------------------------------------
                                      TcpComandoWriteCmd62(num_socket, StringaRicevuta, LenMessaggioRx, TcpNumeroPacchetto, TcpTipoMessaggio, all_mess);
                                      TcpTrasmissioneInCorso=false;
                                      break;
     case  CMD_70_STATO_ALL_CEL:      TcpTrasmissioneInCorso=true;
                                      //----------------------------------------------
                                      // Grestione comando
                                      //----------------------------------------------
                                      TcpComandoWriteCmd70(num_socket, StringaRicevuta, LenMessaggioRx, TcpNumeroPacchetto, TcpTipoMessaggio, all_mess);
                                      TcpTrasmissioneInCorso=false;
                                      break;
     case  CMD_71_STATO_ONE_CEL:      TcpTrasmissioneInCorso=true;
                                      //----------------------------------------------
                                      // Grestione comando
                                      //----------------------------------------------
                                      TcpComandoWriteCmd71(num_socket, StringaRicevuta, LenMessaggioRx, TcpNumeroPacchetto, TcpTipoMessaggio, all_mess);
                                      TcpTrasmissioneInCorso=false;
                                      break;
     default :   err=1;
                 strcpy(all_mess, "SERVER DB TCP: Command unknow" );
                 break;
  }
  return err;
}
//---------------------------------------------------------------------------

void __fastcall TFormSrvTcpAndroid::TimerStateConnectionTimer(TObject *Sender)
{
  int i;

  TimerStateConnection->Enabled=false;

  for(i=1; i<=MAXSOCKET_ANDROID; i++){
     //---------------------------------------------------------------
     // Time Out CLIENT REMOTO
     // Se un CLIENT è già connesso ma NON si riceve nulla da troppo
     // tempo la connessione è scaduta --> CLOSE
     // P.S. Se impostato un Time-Out==0 --> Non viene gestito
     //---------------------------------------------------------------
     if(CliWSocket[i]->State==wsConnected) {
        ConnData[i].TcpCtrConnessClientScaduta++;
        if( TcpTimeOutConnessClientRemoto!=0 ){
           if( ConnData[i].TcpCtrConnessClientScaduta >= TcpTimeOutConnessClientRemoto ){
              CliWSocket[i]->Abort();
              CliWSocket[i]->Close();
              memset(&ConnData[i], 0, sizeof(CONNECTION_DATA));
           }
        }
     }
  }
  TimerStateConnection->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormSrvTcpAndroid::TimerRefreshTimer(TObject *Sender)
{
  TimerRefresh->Enabled=false;

  SetCaselle();

  TimerRefresh->Enabled=true;
}
//---------------------------------------------------------------------------

//-----------------------------------------------------
//   AcceptedConnection()
//
// Funzione che gestisce i permessi di accesso
// al Server per le comunicazioni ANDORID
//
// 0 - ok
// 1 - connessine non ammessa
// 2 - codice non valido
// 3 - codice scaduto
//-----------------------------------------------------
int TFormSrvTcpAndroid::AcceptedConnection(int num_socket, BYTE mode, long int codice_accesso)
{
  int i, count;
  int MaxPermessi=0;
  char mm[3]="";
  char gg[3]="";
  char ss[3]="";
  char dd[3]="";
  char data[15]="";
  long int int_codice=0;
  long int time_start;
  char str_codice[11]="";
  SYSTEMTIME TimeAttuale;
  TDateTime DataRecuperata;
  TDateTime DataInizioAnno;
  int GiornoDellaSettimana1, GiornoDellaSettimana2;
  int pos, DiffTime;
  div_t Settimane;
  AnsiString Buff;
  AnsiString InputStringa;
  long int durata, time_trascorso;


  //------------------------------------------------------
  // VERIFICA MODALITA' DI ACCESSO
  // 0 = solo manuali (nessuna connessione PC prevista)
  //------------------------------------------------------
  if(mode==MODE_MANUALS_ANDROID) return STATO_CONN_NOT_AMMESSA_ANDROID;
  //------------------------------------------------------
  // VERIFICA MODALITA' DI ACCESSO
  // 1 = solo stato TGV
  //     (un numero di connessioni pari a MAXAGV)
  //------------------------------------------------------
  if(mode==MODE_STATE_TGV_ANDROID){
     MaxPermessi = MAXAGV;
     //------------------------------------------------------
     // Conteggio delle connessioni già attive con la stessa
     // modalità di accesso
     //------------------------------------------------------
     count=0;
     for(i=1; i<=MAXSOCKET_ANDROID; i++){
        //---------------------------------------------------------------
        // Scarto connessioni non attive
        //---------------------------------------------------------------
        if(CliWSocket[i]->State!=wsConnected) continue;
        //---------------------------------------------------------------
        // Scarto le connessioni che non sono abbinate al codice di
        // accesso che sto analizzando
        //---------------------------------------------------------------
        if(ConnData[i].Mode!=MODE_STATE_TGV_ANDROID) continue;
        //---------------------------------------------------------------
        // Conto quante connessioni sono attive con il codice cliente
        //---------------------------------------------------------------
        count++;
     }
     if(count>MaxPermessi) return STATO_CONN_NOT_AMMESSA_ANDROID;   // connessione non ammessa
     //-------------------------------------------------------
     // PERMESSO CONCESSO
     //-------------------------------------------------------
     ConnData[num_socket].Mode       = mode;
     ConnData[num_socket].CodAccesso = codice_accesso;
     ConnData[num_socket].TimeRemain = 0;
     return STATO_CONN_AMMESSA_ANDROID;
  }
  //------------------------------------------------------
  // VERIFICA MODALITA' DI ACCESSO
  // 2 = stato IMPIANTO
  //     (verifica codici di accesso)
  //------------------------------------------------------
  if(mode==MODE_STATE_PLANT_ANDROID){
     //------------------------------------------------------
     // Verifica permessi possibili in base al tipo di
     // codice
     //------------------------------------------------------
     MaxPermessi = MAX_PERMESSI_SPECIALI;
     if(codice_accesso==CODICE_CLIENTE) MaxPermessi = MAX_PERMESSI_CLIENTE;
     //------------------------------------------------------
     // Conteggio delle connessioni già attive solo se il
     // codice di accesso è "00000000" = CODICE CLIENTE
     //------------------------------------------------------
     count=0;
     for(i=1; i<=MAXSOCKET_ANDROID; i++){
        //---------------------------------------------------------------
        // Scarto connessioni non attive
        //---------------------------------------------------------------
        if(CliWSocket[i]->State!=wsConnected) continue;
        //---------------------------------------------------------------
        // Scarto le connessioni che non sono abbinate al codice di
        // accesso che sto analizzando
        //---------------------------------------------------------------
        if(codice_accesso==CODICE_CLIENTE && ConnData[i].CodAccesso!=CODICE_CLIENTE) continue;
        if(codice_accesso!=CODICE_CLIENTE && ConnData[i].CodAccesso==CODICE_CLIENTE) continue;
        //---------------------------------------------------------------
        // Conto quante connessioni sono attive con il codice cliente
        //---------------------------------------------------------------
        count++;
     }
     if(count>MaxPermessi) return STATO_CONN_NOT_AMMESSA_ANDROID;   // connessione non ammessa
     //------------------------------------------------------
     // Controlli successivi solo per codici speciali
     //------------------------------------------------------
     if(codice_accesso!=CODICE_CLIENTE){
        InputStringa = IntToStr(codice_accesso);
        memset(&gg[0], NULL, sizeof(gg));
        memset(&mm[0], NULL, sizeof(mm));
        memset(&ss[0], NULL, sizeof(ss));
        memset(&dd[0], NULL, sizeof(dd));
        //-----------------------------------------------
        // Richiesta DURATA CODICE
        //-----------------------------------------------
        if(codice_accesso<0 || codice_accesso>999999999 ) return STATO_COD_NON_VALIDO_ANDROID;
        //---------------------------------------------
        // Ammesse stringhe solo di più di 8 caratteri
        // altrimenti sono sicuramente indecifrabili
        //---------------------------------------------
        if(InputStringa.Length()<7) return STATO_COD_NON_VALIDO_ANDROID;
        //---------------------------------------------
        // Eliminazione codice base
        //---------------------------------------------
        int_codice = codice_accesso - CODICE_BASE_ANDROID;
        //---------------------------------------------
        // Conversione interto to stringa e recupero
        // lunghezza
        //---------------------------------------------
        InputStringa          = IntToStr(int_codice);
        pos                   = InputStringa.Length();
        //---------------------------------------------
        // Estrapolazione moltiplicatore:
        // Giorno della settimana
        //---------------------------------------------
        Buff                  = InputStringa.SubString(pos, 1);
        GiornoDellaSettimana1 = StrToInt(Buff);
        InputStringa          = InputStringa.Delete(pos, 1); // eliminazine giorno settimana dal codice
        //---------------------------------------------
        // Controllo validita giorno settimana
        //---------------------------------------------
        if(GiornoDellaSettimana1<1 ) return STATO_COD_NON_VALIDO_ANDROID;
        if(GiornoDellaSettimana1>7 ) return STATO_COD_NON_VALIDO_ANDROID;
        //---------------------------------------------
        // Divisione del valore ottenuto per il giorno
        // della settimana
        //---------------------------------------------
        int_codice = StrToInt(InputStringa) / GiornoDellaSettimana1;
        //---------------------------------------------
        // Ottengo stringa mescoltata
        //
        // aabbaabb
        // MGSDSDMG
        //---------------------------------------------
        sprintf(str_codice, "%08ld", int_codice);
        mm[0] = str_codice[0];
        gg[0] = str_codice[1];
        ss[1] = str_codice[2];
        dd[1] = str_codice[3];
        ss[0] = str_codice[4];
        dd[0] = str_codice[5];
        mm[1] = str_codice[6];
        gg[1] = str_codice[7];

        //---------------------------------------------
        // Verifica se il giorno recuperato
        // è coerente con i dati di Settimana e
        // Giorno della settimana
        //---------------------------------------------
        try{
           GetLocalTime(&TimeAttuale);
           DataRecuperata = TDateTime(TimeAttuale.wYear, (WORD)(atoi(mm)), (WORD)(atoi(gg)));    // data attuale
           DataInizioAnno = TDateTime(TimeAttuale.wYear, 1, 1);                                  // data inizio anno
           DiffTime       = DataRecuperata - DataInizioAnno;
           Settimane      = div(DiffTime, 7); Settimane.quot++;                                  // calcolo settimane e giorni di scarto
           GiornoDellaSettimana2 = DataRecuperata.DayOfWeek();                                   // recupero il giorno della settimana
        }
        catch(...){
           return STATO_COD_NON_VALIDO_ANDROID;
        }
        //---------------------------------------------
        // Controllo coerenza settimana
        //---------------------------------------------
        if(GiornoDellaSettimana1!=GiornoDellaSettimana2) return STATO_COD_NON_VALIDO_ANDROID;
        if(atoi(ss)!=Settimane.quot                    ) return STATO_COD_NON_VALIDO_ANDROID;
        //---------------------------------------------
        // Se il codice risulta corretto verifico se
        // è valido anche come durata in secondi
        // dal 1/01/1970
        //---------------------------------------------
        durata = (long int)(atoi(dd) * 24 * 60 * 60);
        sprintf(data, "%02d/%02d/%04d", (WORD)(atoi(gg)), (WORD)(atoi(mm)), TimeAttuale.wYear);
        timechar_to_timeint(&time_start, &data[0], "00.00.01");    // Data/Ora fine giornata
        //---------------------------------------------
        // Calcolo tempo trascorso da attivazione
        // codice
        //---------------------------------------------
        time_trascorso = (time(NULL) - time_start);
        //---------------------------------------------
        // Verifica per quanti giorni è ancora valido
        //---------------------------------------------
        if(time_trascorso>durata){
           //-------------------------------------------------------
           // PERMESSO NON CONCESSO PER CODICE SCADUTO
           //-------------------------------------------------------
           return STATO_COD_SCADUTO_ANDROID;
        }
        else{
           //-------------------------------------------------------
           // PERMESSO CONCESSO + CALCOLO VALIDITA'
           //-------------------------------------------------------
           ConnData[num_socket].Mode       = mode;
           ConnData[num_socket].CodAccesso = codice_accesso;
           ConnData[num_socket].TimeRemain = durata-time_trascorso;
           return STATO_CODTMP_ANDROID;
        }
     }
     //-------------------------------------------------------
     // PERMESSO CONCESSO
     //-------------------------------------------------------
     ConnData[num_socket].Mode       = mode;
     ConnData[num_socket].CodAccesso = codice_accesso;
     ConnData[num_socket].TimeRemain = 0;
     return STATO_CONN_AMMESSA_ANDROID;
  }
  //-------------------------------------------------------
  // default PERMESSO NON CONCESSO
  //-------------------------------------------------------
  return STATO_CONN_NOT_AMMESSA_ANDROID;
}

//---------------------------------------------------------------------------

//-----------------------------------------------------
//   RefreshServerTCPAndroid()
//
// Funzione di aggiornamento strutture globali
//-----------------------------------------------------
int  TFormSrvTcpAndroid::RefreshServerTCPAndroid(BYTE Typ, char *all_mess)
{
  int err=0;
/*
  int i, j, k, lav, err;
  bool lista_cod;
  short int okordine;
  char Codice[31]="";
  struct CODICIMAG BuffCod;

  //---------------------------------------------------
  // Comunicazione non attiva
  //---------------------------------------------------
  if( break_Com[COM9]==true ) return 0;

  //---------------------------------------------------
  // SELEZIONE STRUTTURE DA AGGIORNARE
  //---------------------------------------------------
  lista_cod  = (bool)(TestBit((char *)&Typ, 0));            // LISTA PRODOTTI IN MAGAZZINO

  //---------------------------------------------------
  // Inizializzazione variabili
  //---------------------------------------------------
  err=0;
  strcpy(all_mess, "OK");


  if(lista_cod==true){
     memset(&CodMag,  0, sizeof(CodMag));
     memset(&BuffCod, 0, sizeof(CODICIMAG));
     // MAX CODICI ALL'INTERNO DELL'IMPIANTO
     // --- TGV
     for(i=1; i<=MAXAGV; i++){
        memset(&Codice[0], NULL, sizeof(Codice));
        strcpy(Codice, B.b[AGV[i]->mission.IDBox]->Codice);
        Erase_White_End(Codice);
        if(strcmp(Codice, "")==0) continue;
        for(j=0; j<=TOTCODICIMAG; j++){
           //------------------------------------------------
           // Codice già presente nell'impianto
           //------------------------------------------------
           if(strcmp(Codice, CodMag[j].Cod)==0){
              //--------------------------------------
              // Verifico se COTTO o CRUDO
              // NOTA: serve per aggiungere eventuali
              //       stati di lavorazione
              //--------------------------------------
              switch(B.b[AGV[i]->mission.IDBox]->Lav[B.b[AGV[i]->mission.IDBox]->NLav]){
                 case  1:
                 case  2:
                 case  3:
                 case  4:
                 case  5:
                 case  6:
                 case 14:
                 case 15: lav=1;   // crudo
                          break;
                 default: lav=2;   // cotto
                          break;
              }
              if(lav==1 )  SetBit((char *) &CodMag[j].Lav, 0); // CRUDI
              if(lav==2 )  SetBit((char *) &CodMag[j].Lav, 1); // COTTI
              break;
           }
           //------------------------------------------------
           // Salto i codici già inseriti
           //------------------------------------------------
           if(strcmp(CodMag[j].Cod, "")!=0) continue;
           //------------------------------------------------
           // Codice da inserire nella struttura
           //------------------------------------------------
           strcpy(CodMag[j].Cod, B.b[AGV[i]->mission.IDBox]->Codice);
           strcpy(CodMag[j].Des, B.b[AGV[i]->mission.IDBox]->Descrizione);
           //--------------------------------------
           // Verifico se COTTO o CRUDO
           //--------------------------------------
           switch(B.b[AGV[i]->mission.IDBox]->Lav[B.b[AGV[i]->mission.IDBox]->NLav]){
              case  1:
              case  2:
              case  3:
              case  4:
              case  5:
              case  6:
              case 14:
              case 15: lav=1;   // crudo
                       break;
              default: lav=2;   // cotto
                       break;
           }
           if(lav==1 )  SetBit((char *) &CodMag[j].Lav, 0); // CRUDI
           if(lav==2 )  SetBit((char *) &CodMag[j].Lav, 1); // COTTI
           break;
        }
     }
     // --- MAGAZZINI
     for(i=1; i<=MAXMAG; i++){
        memset(&Codice[0], NULL, sizeof(Codice));
        strcpy(Codice, M.m[i]->Cod);
        Erase_White_End(Codice);
        if(strcmp(Codice, "")==0) continue;
        for(j=0; j<=TOTCODICIMAG; j++){
           //------------------------------------------------
           // Codice già presente nell'impianto
           //------------------------------------------------
           if(strcmp(Codice, CodMag[j].Cod)==0){
              //--------------------------------------
              // Verifico se COTTO o CRUDO
              // NOTA: serve per aggiungere eventuali
              //       stati di lavorazione
              //--------------------------------------
              switch(M.m[i]->Lav[M.m[i]->NLav]){
                 case  1:
                 case  2:
                 case  3:
                 case  4:
                 case  5:
                 case  6:
                 case 14:
                 case 15: lav=1;   // crudo
                          break;
                 default: lav=2;   // cotto
                         break;
              }
              if(lav==1 )  SetBit((char *) &CodMag[j].Lav, 0); // CRUDI
              if(lav==2 )  SetBit((char *) &CodMag[j].Lav, 1); // COTTI
              break;
           }
           //------------------------------------------------
           // Salto i codici già inseriti
           //------------------------------------------------
           if(strcmp(CodMag[j].Cod, "")!=0) continue;
           //------------------------------------------------
           // Codice da inserire nella struttura
           //------------------------------------------------
           strcpy(CodMag[j].Cod, M.m[i]->Cod);
           strcpy(CodMag[j].Des, M.m[i]->Des);
           //--------------------------------------
           // Verifico se COTTO o CRUDO
           //--------------------------------------
           switch(M.m[i]->Lav[M.m[i]->NLav]){
              case  1:
              case  2:
              case  3:
              case  4:
              case  5:
              case  6:
              case 14:
              case 15: lav=1;   // crudo
                       break;
              default: lav=2;   // cotto
                       break;
           }
           if(lav==1 )  SetBit((char *) &CodMag[j].Lav, 0); // CRUDI
           if(lav==2 )  SetBit((char *) &CodMag[j].Lav, 1); // COTTI
           break;
        }
     }
     // --- MACCHINE (SOLO BOX SOTTO ESSICCATOI)
     for(i=1; i<=MAXCHIAMATE; i++){
        if(P.p[i]->Num<1          ) continue;
        if(P.p[i]->Num>TOTCHIAMATE) continue;
        if(P.p[i]->Typ!=M_ES      ) continue;
        //-------------------------------------------------
        // Controllo tutti i box perchè potrebbero esserci
        // codici diversi
        //-------------------------------------------------
        for(j=1; j<=MAXBOXMAC; j++){
           if(P.p[i]->IDBox[j]<1                  ) continue;
           if(P.p[i]->IDBox[j]>MAXBOXIMPIANTO     ) continue;
           if(B.b[P.p[i]->IDBox[j]]->Pieno==false ) continue;
           //------------------------------------------------
           // Recupero id codici
           //------------------------------------------------
           memset(&Codice[0], NULL, sizeof(Codice));
           strcpy(Codice, B.b[P.p[i]->IDBox[j]]->Codice);
           Erase_White_End(Codice);
           if(strcmp(Codice, "")==0) continue;
           for(k=0; k<=TOTCODICIMAG; k++){
              //------------------------------------------------
              // Codice già presente nell'impianto
              //------------------------------------------------
              if(strcmp(Codice, CodMag[k].Cod)==0){
                 SetBit((char *) &CodMag[k].Lav, 0); // sotto l'ES sicuramente: CRUDI
                 break;
              }
              //------------------------------------------------
              // Salto i codici già inseriti
              //------------------------------------------------
              if(strcmp(CodMag[k].Cod, "")!=0) continue;
              //------------------------------------------------
              // Codice da inserire nella struttura
              //------------------------------------------------
              strcpy(CodMag[k].Cod, B.b[P.p[i]->IDBox[j]]->Codice);
              strcpy(CodMag[k].Des, B.b[P.p[i]->IDBox[j]]->Descrizione);
              SetBit((char *) &CodMag[k].Lav, 0); // sotto l'ES sicuramente: CRUDI
              break;
           }
        }
     }
     //------------------------------------------------
     // ORDINAMENTO ALFABETICO STRUTTURA CODICI
     //------------------------------------------------
     for(;;){
        okordine=0;
        for(i=0; i<TOTCODICIMAG; i++){
           memset(&BuffCod, 0, sizeof(CODICIMAG));
           if(i>0 && strcmp(CodMag[i].Cod, "")!=0 && strcmp(CodMag[i].Cod, CodMag[i-1].Cod)<0){
              memcpy(&BuffCod,     &CodMag[i-1], sizeof(CODICIMAG));
              memcpy(&CodMag[i-1], &CodMag[i],   sizeof(CODICIMAG));
              memcpy(&CodMag[i],   &BuffCod,     sizeof(CODICIMAG));
           }
           else okordine++;
        }
        if(okordine>TOTCODICIMAG-1) break;
     }
  }
*/
  return err;
}
//---------------------------------------------------------------------------

//-----------------------------------------------------
//   TcpComandoWriteCmd2()
//
// GESTIONE COMANDO STATO IMPIANTO
//-----------------------------------------------------
int  TFormSrvTcpAndroid::TcpComandoWriteCmd2(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess)
{
  WORD buff;
  WORD B1, B2;
  BYTE Mode, C1, C2, C3, C4, NoRisp;
  BYTE BitRichAPc;
  bool backup;
  int i, err;
  int stato_connection;
  long int codice_connection;
  short int LastBox, TotCod;
  unsigned int  LenRisposta;
  unsigned char StringaTrs[2051];   // Stringa da trasmettere
  struct CODICIMAG BuffCod;
  struct PARAMETRI_IMP{
     BYTE Typ;
     BYTE StatConn;
     BYTE bAux1;
     BYTE bAux2;
     BYTE bAux3;
     BYTE bAux4;
     BYTE bAux5;
     BYTE MaxTgv;
     BYTE MaxMac;
     BYTE MaxCod;
     WORD MaxMag;
     WORD MaxNod;
     WORD MaxBox;
     WORD wAux1;
     WORD wAux2;
     WORD wAux3;
     WORD wAux4;
     WORD wAux5;
  } Dati;


  err=0;
  strcpy(all_mess, "OK");
  memset(&BuffCod,       0, sizeof(CODICIMAG));
  memset(&Dati,          0, sizeof(PARAMETRI_IMP));
  memset(&StringaTrs, NULL, sizeof(StringaTrs));
  //-------------------------------------------------
  // Verifico che il comando sia quello corretto
  //-------------------------------------------------
  if(TypMess!=CMD_02_PARAMETRI_IMP ) return 0;
  if(LenMessaggioRx!=17            ) return 0;

  //------------------------------------------------------
  // Recupero i byte che compongono il codice di
  // attivazione e modaltà
  //------------------------------------------------------
  Mode          = (BYTE)(StringaRicevuta[ 3]);        // MODALITA' ACCESSO
  C1            = (BYTE)(StringaRicevuta[ 4]);
  C2            = (BYTE)(StringaRicevuta[ 5]);
  C3            = (BYTE)(StringaRicevuta[ 6]);
  C4            = (BYTE)(StringaRicevuta[ 7]);
  ReleaseAPK    = (BYTE)(StringaRicevuta[ 8]);
  DispTecnoAPK  = (BYTE)(StringaRicevuta[ 9]);
  TypImpAPK     = (BYTE)(StringaRicevuta[10]);
  BitRichAPc    = (BYTE)(StringaRicevuta[11]);
  NoRisp        = (BYTE)(StringaRicevuta[16]);
  codice_connection = (C1<<24) + (C2<<16) + (C3<<8) + C4;

  //-------------------------------------------------
  // SCOMPOSIZIONE BYTE DI RICHIESTE A PC
  // Ogni bit ha un significato differente di
  // richieste per il PC SERVER:
  // bit00 - richiesta BACKUP
  //-------------------------------------------------
  backup  = (bool)(TestBit((char *)&BitRichAPc, 0));   // RICHIESTA BACKUP MANUALE

  //-------------------------------------------------
  // Funzioni di permesso accesso
  // 00 - ok
  // 01 - connessine non ammessa
  // 02 - codice non valido
  // 03 - codice scaduto
  // ....
  // 49 - codice accesso abilitato temporaneo
  // 50 - attivazione dispositivo android
  // 51 - disattivazione dispositivo android
  //
  //-------------------------------------------------
  stato_connection = AcceptedConnection(num_socket, Mode, codice_connection);
  //-------------------------------------------------
  // Aggiornamento struttura dati richiesti
  //-------------------------------------------------
  // STATO CONNESSIONE
  if(stato_connection!=STATO_CONN_AMMESSA_ANDROID &&
     stato_connection!=STATO_CODTMP_ANDROID &&
     stato_connection!=STATO_ATTIVA_HW_ANDROID &&
     stato_connection!=STATO_DISATTIVA_HW_ANDROID){
     //-------------------------------------------------
     // Compilo solo il byte di stato connessione
     //-------------------------------------------------
     buff=stato_connection;
     Dati.StatConn = (BYTE)(buff);
  }
  else{
     // Abilita opzioni android
     Dati.Typ = ABILITA_OPZIONI_ANDROID;
     // Abilita connessione
     Dati.StatConn = stato_connection;
     //-------------------------------------------------
     // Compilazioni dati impianti solo se:
     //
     // 1. TIPO IMPIANTO CORRETTO
     // 2. DISPOSITIVO TECNOFERRARI
     //-------------------------------------------------
     if(TypImpAPK==ABILITA_OPZIONI_ANDROID && DispTecnoAPK==HARDWARE_TECNOFERRARI){
        // TGV
        buff = MAXAGV;
        Dati.MaxTgv = (BYTE)(buff);
        // MACCHINE
        buff = TOTCHIAMATE;
        Dati.MaxMac = (BYTE)(buff);
        // MAGAZZINI
        buff = TOTMAG;
        B1   = (WORD)(buff>>8);
        B2   = (WORD)(buff & 0x00ff);
        buff = (WORD)((B2<<8) + B1);
        Dati.MaxMag = buff;
        // NODI
        buff = TOTPUNTI;
        B1   = (WORD)(buff>>8);
        B2   = (WORD)(buff & 0x00ff);
        buff = (WORD)((B2<<8) + B1);
        Dati.MaxNod = buff;
        // BOX
        LastBox = 0;
/* non esiste la struttura box nel PAL2000
        for(i=MAXBOXIMPIANTO; i>0; i--){
           if(B.b[i]->Num<1              ) continue;
           if(B.b[i]->Num>MAXBOXIMPIANTO ) continue;
           LastBox = (short int)(i);
           break;
        }
*/
        buff = (WORD)(LastBox);
        B1   = (WORD)(buff>>8);
        B2   = (WORD)(buff & 0x00ff);
        buff = (WORD)((B2<<8) + B1);
        Dati.MaxBox = buff;
        // CODICI IMPIANTO
        TotCod=0;
        for(i=0; i<=TOTCODICIMAG; i++){
           //------------------------------------------------
           // Struttura non compilata --> skip!
           //------------------------------------------------
           if(strcmp(CodMag[i].Cod, "")==0) break;
           TotCod++;
        }
        // aggiornamento struttura ANDROID
        buff = (WORD)(TotCod);
        Dati.MaxCod = (BYTE)(buff);
     }
  }
  //-------------------------------------------------
  // ELABORAZIONI BIT DI RICHIESTA A PC
  //-------------------------------------------------
  if(backup==true){
     err = RemCreaFileComando ("Backup", NULL, NULL, all_mess );
     if(!err) FileLogOperatore("(BACKUP ) - Backup da dispositivo ALGODROID");
  }

  //-------------------------------------------------
  // compilazione della stringa di risp
  //-------------------------------------------------
  if(!err && NoRisp==0){
     StringaTrs[ 0 ]=IdxMess;
     StringaTrs[ 1 ]=TypMess;
     memcpy(&StringaTrs[2], (BYTE *)(&Dati), sizeof(PARAMETRI_IMP));
     LenRisposta = (unsigned short int)(sizeof(PARAMETRI_IMP));
     //--------------------------------------------
     // Trasmissione via Socket
     //--------------------------------------------
     err = TcpSend(num_socket, StringaTrs, LenRisposta, all_mess);
  }

  return err;
}
//---------------------------------------------------------------------------


//-----------------------------------------------------
//   TcpComandoWriteCmd9()
//
// GESTIONE TRASMISSIONE LISTA MESSAGGI
//
//-----------------------------------------------------
int  TFormSrvTcpAndroid::TcpComandoWriteCmd9(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess)
{
  int  i, err, errore_file;
  short int num_mess;
  short int NrMess_Star, NrMess_Fine;
  unsigned int  LenRisposta;
  unsigned char StringaTrs[2051];   // Stringa da trasmettere
  struct LISTA_MESS{
     BYTE Num;
     BYTE Messaggio[79];
  } Dati[MAXMESSAG_ANDROID+1];


  err=0;
  strcpy(all_mess, "OK");
  memset(&Dati,       NULL, sizeof(Dati));
  memset(&StringaTrs, NULL, sizeof(StringaTrs));
  //-------------------------------------------------
  // Verifico che il comando sia quello corretto
  //-------------------------------------------------
  if(TypMess!=CMD_09_MESSAGGERIA   ) return 0;
  if(LenMessaggioRx!=4             ) return 0;
  //-------------------------------------------------
  // Variabili di puntamento ai messaggi richiesti
  //-------------------------------------------------
  NrMess_Star = (BYTE)(StringaRicevuta[ 2 ]);
  NrMess_Fine = (BYTE)(StringaRicevuta[ 3 ]);
  if(NrMess_Star>MAXMESSAG_ANDROID-1) return 0;    // numero MESS non ammesso
  if(NrMess_Fine>MAXMESSAG_ANDROID-1) return 0;    // numero MESS non ammesso
  //----------------------------------------------
  // Rilettura messaggi solo sulla variazione
  // di lingua
  //----------------------------------------------
  OldMESSAPK  = 0;                  // garantisce la rilettura di tutti i messaggi
                                    // su richiesta del dispositivo
  errore_file = load_MESSAPK();
  //-------------------------------------------------
  // Aggiornamento struttura dati richiesti
  //-------------------------------------------------
  if(errore_file==0){
     num_mess=0;
     for(i=NrMess_Star; i<=NrMess_Fine; i++){
        Dati[num_mess].Num = (BYTE)(i);
        memcpy(&Dati[num_mess].Messaggio[0], &MESSAPK[i][0], 79); //Dati[num_mess].Messaggio[78]=NULL;
        num_mess++;
     }
  }
  //-------------------------------------------------
  // compilazione della stringa di risp
  //-------------------------------------------------
  if(!err){
     StringaTrs[ 0 ]=IdxMess;
     StringaTrs[ 1 ]=TypMess;
     memcpy(&StringaTrs[2], (BYTE *)(&Dati), (int)(sizeof(LISTA_MESS)*num_mess));
     LenRisposta = (unsigned short int)(2+(sizeof(LISTA_MESS)*num_mess));
     //--------------------------------------------
     // Trasmissione via Socket
     //--------------------------------------------
     err = TcpSend(num_socket, StringaTrs, LenRisposta, all_mess);
  }
  return err;
}
//---------------------------------------------------------------------------


//-----------------------------------------------------
//   TcpComandoWriteCmd20()
//
// GESTIONE COMANDADO STATO TUTTI I TGV
//-----------------------------------------------------
int  TFormSrvTcpAndroid::TcpComandoWriteCmd20(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess)
{
  int  i, err;
  char buffstr[51]="";
  short int num_agv;
  short int chiamata;
  short int num_baia, num_mac, num_mag, num_stz;
  WORD buff;
  WORD B1, B2;
  BYTE NrTGV_Star=0;
  BYTE NrTGV_Fine=0;
  unsigned int  LenRisposta;
  unsigned char StringaTrs[2051];   // Stringa da trasmettere
  struct STATO_TGV{
     BYTE Num;
     BYTE Start;
     WORD Pos;
     BYTE Chiamata[10];
     WORD BitAll;
     WORD BitStato;
     WORD Aux1;
  } Dati[MAXAGV+1];


  err=0;
  strcpy(all_mess, "OK");
  memset(&Dati,          0, sizeof(Dati));
  memset(&StringaTrs, NULL, sizeof(StringaTrs));
  //-------------------------------------------------
  // Verifico che il comando sia quello corretto
  //-------------------------------------------------
  if(TypMess!=CMD_20_STATO_ALL_TGV ) return 0;
  if(LenMessaggioRx!=4             ) return 0;
  //-------------------------------------------------
  // Recupero dalla stringa in ingresso il numero
  // Iniziale e il totale TGV da ritrasmettere
  //-------------------------------------------------
  NrTGV_Star = (BYTE)(StringaRicevuta[ 2 ]);
  NrTGV_Fine = (BYTE)(StringaRicevuta[ 3 ]);
  if(NrTGV_Star<1 || NrTGV_Star>MAXAGV) return 0;    // numero TGV non ammesso
  if(NrTGV_Fine<1 || NrTGV_Fine>MAXAGV) return 0;    // numero TGV non ammesso
  //-------------------------------------------------
  // Aggiornamento struttura dati richiesti
  //-------------------------------------------------
  num_agv=0;
  for(i=NrTGV_Star; i<=NrTGV_Fine; i++){
     // TGV
     Dati[num_agv].Num    = (BYTE)(i);
     // DM91
     Dati[num_agv].Start  = (BYTE)(AGV[i]->stato.start);
     // POSIZIONE
     buff = AGV[i]->stato.pos;
     B1   = (WORD)(buff>>8);
     B2   = (WORD)(buff & 0x00ff);
     buff = (WORD)((B2<<8) + B1);
     Dati[num_agv].Pos = buff;
     // CHIAMATA
     chiamata = AGV[i]->mission.Chiamata;
     memset(&buffstr[0], NULL, sizeof(buffstr));
     if(chiamata>0){
        num_baia=num_mac=num_mag=num_stz=0;
        //-----------------------------------------
        // Scomposizione della chiamata
        //-----------------------------------------
        P.TestChiamataNum(chiamata, &num_mag, &num_baia, &num_mac, &num_stz);
        if(num_mac>0 && num_mac<=TOTCHIAMATE){
           sprintf(buffstr, "%s (%d)", P.ch[num_mac]->Sigla, num_stz);
        }
        if(num_baia>0 && num_baia<=TOTBAIE){
           sprintf(buffstr, "%s (%d)", P.ba[num_baia]->Sigla, num_baia);
        }
        if(num_mag>0 && num_mag<=TOTMAG){
           sprintf(buffstr, "STORE (%d)", num_mag);
        }
     }
     memcpy(&Dati[num_agv].Chiamata[0], &buffstr[0], 10); //Dati[num_agv].Chiamata[9]=NULL;
     // ALLARME
     buff=0;
     if(AGV[i]->stato.s.bit.allarme) SetBit((char *) &buff, 0);      // tgv in allarme da PLC
     if(AGV[i]->allarme_interno!=0 ) SetBit((char *) &buff, 1);      // tgv in allarme da PC
     B1   = (WORD)(buff>>8);
     B2   = (WORD)(buff & 0x00ff);
     buff = (WORD)((B2<<8) + B1);
     Dati[num_agv].BitAll = buff;
     // BIT DI STATO
     buff=0;
     if(!AGV[i]->stato.s.bit.funz                          ) SetBit((char *) &buff, 0);   // tgv in manuale
     if(AGV[i]->mission.SemiAutomatico                     ) SetBit((char *) &buff, 1);   // tgv in semiautomatico
     if(AGV[i]->mission.Esclusione                         ) SetBit((char *) &buff, 2);   // tgv escluso
     if(AGV[i]->stato.s.bit.carA | AGV[i]->stato.s.bit.carB) SetBit((char *) &buff, 3);   // tgv con carico a bordo
     B1   = (WORD)(buff>>8);
     B2   = (WORD)(buff & 0x00ff);
     buff = (WORD)((B2<<8) + B1);
     Dati[num_agv].BitStato = buff;

     num_agv++;
  }
  //-------------------------------------------------
  // compilazione della stringa di risp
  //-------------------------------------------------
  if(!err){
     StringaTrs[ 0 ]=IdxMess;
     StringaTrs[ 1 ]=TypMess;
     memcpy(&StringaTrs[2], (BYTE *)(&Dati), (int)(sizeof(STATO_TGV)*num_agv));
     LenRisposta = (unsigned short int)(2+(sizeof(STATO_TGV)*num_agv));
     //--------------------------------------------
     // Trasmissione via Socket
     //--------------------------------------------
     err = TcpSend(num_socket, StringaTrs, LenRisposta, all_mess);
  }
  return err;
}
//---------------------------------------------------------------------------

//-----------------------------------------------------
//   TcpComandoWriteCmd21()
//
// GESTIONE COMANDADO STATO TGV SINGOLO
//-----------------------------------------------------
int  TFormSrvTcpAndroid::TcpComandoWriteCmd21(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess)
{
  char buffer[101]="";
  char buffA[101]="";
  char buffB[101]="";
  char buffstr[101]="";
  int  chiamata;
  int  i, j, k, n, err, errore_file1, errore_file2;
  short int tot_all, num_baia, num_mac, num_mag, num_stz;
  WORD buff;
  WORD B1, B2;
  BYTE NrTGV_Star=0;
  unsigned int  LenRisposta;
  unsigned char StringaTrs[2051];   // Stringa da trasmettere
  struct STATO_TGV{
     BYTE Num;
     BYTE Start;
     WORD BitStato1;
     WORD BitStato2;
     WORD MissStato;
     WORD Pos;
     WORD PInt;
     BYTE PStart[10];
     BYTE PEnd[10];
     BYTE TypMiss[10];
     BYTE Chiamata[10];
     WORD Box;
     BYTE Codice[50];
     BYTE AllPC[50];
     BYTE AllPLC[5][50];
  } Dati;

  err=0;
  strcpy(all_mess, "OK");
  memset(&Dati,          0, sizeof(Dati));
  memset(&buffer,     NULL, sizeof(buffer));
  memset(&buffstr[0], NULL, sizeof(buffstr));
  memset(&StringaTrs, NULL, sizeof(StringaTrs));
  //-------------------------------------------------
  // Verifico che il comando sia quello corretto
  //-------------------------------------------------
  if(TypMess!=CMD_21_STATO_ONE_TGV ) return 0;
  if(LenMessaggioRx!=4             ) return 0;
  //-------------------------------------------------
  // Recupero dalla stringa in ingresso il numero
  // Iniziale e il totale TGV da ritrasmettere
  //-------------------------------------------------
  NrTGV_Star = (BYTE)(StringaRicevuta[ 2 ]);
  if(NrTGV_Star<1 || NrTGV_Star>MAXAGV) return 0;    // esiste solo Tgv start (singolo)
  //----------------------------------------------
  // Rilettura messaggi solo sulla variazione
  // di lingua
  //----------------------------------------------
  errore_file1 = load_MESSAPK();
  //-------------------------------------------------
  // Aggiornamento struttura dati richiesti
  //-------------------------------------------------
  if(errore_file1==0){
     i = NrTGV_Star;
     // TGV
     Dati.Num    = (BYTE)(i);
     // DM91 - START MISSIONE
     Dati.Start  = (BYTE)(AGV[i]->stato.start);
     // DM92 - BIT DI STATO
     buff = (WORD)(AGV[i]->stato.s.val);
     B1   = (WORD)(buff>>8);
     B2   = (WORD)(buff & 0x00ff);
     buff = (WORD)((B2<<8) + B1);
     Dati.BitStato1 = buff;
     // DM94 - BIT DI STATO
     buff = (WORD)(AGV[i]->stato.s.val>>16);
     B1   = (WORD)(buff>>8);
     B2   = (WORD)(buff & 0x00ff);
     buff = (WORD)((B2<<8) + B1);
     Dati.BitStato2 = buff;
     // STATO MISSIONE  (MAX 8)
     buff=0;
     if(AGV[i]->mission.SemiAutomatico) SetBit((char *) &buff, 0); // tgv in semiautomatico
     if(AGV[i]->mission.Esclusione    ) SetBit((char *) &buff, 1); // tgv escluso
     if(AGV[i]->mission.NoRiposoAuto  ) SetBit((char *) &buff, 2); // No riposo automatico
     if(AGV[i]->mission.EsclPLC       ) SetBit((char *) &buff, 3); // Esclusione comunicazione
     B1   = (WORD)(buff>>8);
     B2   = (WORD)(buff & 0x00ff);
     buff = (WORD)((B2<<8) + B1);
     Dati.MissStato = buff;
     // POSIZIONE
     buff = AGV[i]->stato.pos;
     B1   = (WORD)(buff>>8);
     B2   = (WORD)(buff & 0x00ff);
     buff = (WORD)((B2<<8) + B1);
     Dati.Pos = buff;
     // PINT
     buff = AGV[i]->mission.pintend;
     B1   = (WORD)(buff>>8);
     B2   = (WORD)(buff & 0x00ff);
     buff = (WORD)((B2<<8) + B1);
     Dati.PInt = buff;
     // DATI MISSIONE IN CORSO
     if(AGV[i]->test_mission_on() && AGV[i]->stato.s.bit.funz){
        // STRINGA NODO DI PARTENZA
        memset(&buffstr[0], NULL, sizeof(buffstr));
        sprintf(buffstr, MESSAPK[501], AGV[i]->mission.pstart);
        num_mac=num_mag=num_stz=0;
        P.test_punto_presente_mac(all_mess, AGV[i]->mission.pstart, &num_mac, &num_stz);
        P.test_punto_presente_baia(all_mess, AGV[i]->mission.pstart, &num_baia);
        M.test_punto_presente_mag(all_mess, AGV[i]->mission.pstart, &num_mag, &num_stz);
        if(num_mac!=0 ) strcpy(buffstr,  P.ch[num_mac]->Sigla);
        if(num_baia!=0) strcpy(buffstr,  P.ba[num_baia]->Sigla);
        if(num_mag!=0 ) sprintf(buffstr, MESSAPK[502], num_mag);
        // PSTART
        memcpy(&Dati.PStart[0], &buffstr[0], 10); //Dati.PStart[9]=NULL;
        // STRINGA NODO DI ARRIVO
        memset(&buffstr[0], NULL, sizeof(buffstr));
        sprintf(buffstr, MESSAPK[501], AGV[i]->mission.pend);
        num_mac=num_mag=num_stz=0;
        P.test_punto_presente_mac(all_mess, AGV[i]->mission.pend, &num_mac, &num_stz);
        P.test_punto_presente_baia(all_mess, AGV[i]->mission.pend, &num_baia);
        M.test_punto_presente_mag(all_mess, AGV[i]->mission.pend, &num_mag, &num_stz);
        if(num_mac!=0 ) strcpy(buffstr,  P.ch[num_mac]->Sigla);
        if(num_baia!=0) strcpy(buffstr,  P.ba[num_baia]->Sigla);
        if(num_mag!=0 ) sprintf(buffstr, MESSAPK[502], num_mag);
        // PEND
        memcpy(&Dati.PEnd[0], &buffstr[0], 10); //Dati.PEnd[9]=NULL;
        // STRINGA TIPO MISSIONE
        memset(&buffstr[0], NULL, sizeof(buffstr));
        switch(AGV[i]->mission.Typ){
           case  MISS_SOLO_POSIZIONAMENTO:  strcpy(buffstr, MESSAPK[503]);
                                            break;
           case  MISS_CARICO_DA_MACCHINA:
           case  MISS_CARICO_DA_MAGAZZINO:  strcpy(buffstr, MESSAPK[504]);
                                            break;
           case  MISS_SCARICO_SU_MACCHINA:
           case  MISS_SCARICO_SU_MAGAZZINO: strcpy(buffstr, MESSAPK[505]);
                                            break;
           default:                         strcpy(buffstr, "UNKNOW");
                                            break;
        }
        // TYP MISS
        memcpy(&Dati.TypMiss[0], &buffstr[0], 10); //Dati.TypMiss[9]=NULL;
     }
     else{
        memset(&buffstr[0], NULL, sizeof(buffstr));
        strcpy(buffstr, MESSAPK[507]);
        // TYP MISS
        memcpy(&Dati.TypMiss[0], &buffstr[0], 10); //Dati.TypMiss[9]=NULL;
     }
     // CHIAMATA
     memset(&buffstr[0], NULL, sizeof(buffstr));
     chiamata = AGV[i]->mission.Chiamata;
     if(chiamata>0){
        num_baia=num_mac=num_mag=num_stz=0;
        //-----------------------------------------
        // Scomposizione della chiamata
        //-----------------------------------------
        P.TestChiamataNum(chiamata, &num_mag, &num_baia, &num_mac, &num_stz);
        if(num_mac>0 && num_mac<=TOTCHIAMATE){
           sprintf(buffstr, "%s (%d)", P.ch[num_mac]->Sigla, num_stz);
        }
        if(num_baia>0 && num_baia<=TOTBAIE){
           sprintf(buffstr, "%s (%d)", P.ba[num_baia]->Sigla, num_baia);
        }
        if(num_mag>0 && num_mag<=TOTMAG){
           sprintf(buffstr, "STORE (%d)", num_mag);
        }
     }
     memcpy(&Dati.Chiamata[0], &buffstr[0], 10); //Dati.Chiamata[9]=NULL;
     // IDBOX
     buff = AGV[i]->mission.BoxB.ID;
     B1   = (WORD)(buff>>8);
     B2   = (WORD)(buff & 0x00ff);
     buff = (WORD)((B2<<8) + B1);
     Dati.Box = buff;
     // CODICE: (VUOTO o DESCRIZIONE)
     memset(&buffstr[0], NULL, sizeof(buffstr));
     if(AGV[i]->mission.PresCEST_B!=LIBERA || AGV[i]->mission.PresCEST_A!=LIBERA){
        memset(&buffA, NULL, sizeof(buffA));
        memset(&buffB, NULL, sizeof(buffB));
        switch(AGV[i]->mission.PresCEST_A){
           case PRES_LAV1:     if(AbbinaCodiceStatoBox==true) sprintf(buffA, "<A>: %s%02d", MESSAPK[520], ASS_COD1);
                               else                           sprintf(buffA, "<A>: %s",     ASS_LAV1);
                               break;
           case PRES_LAV2:     if(AbbinaCodiceStatoBox==true) sprintf(buffA, "<A>: %s%02d", MESSAPK[520], ASS_COD2);
                               else                           sprintf(buffA, "<A>: %s",     ASS_LAV2);
                               break;
           case PRES_LAV3:     if(AbbinaCodiceStatoBox==true) sprintf(buffA, "<A>: %s%02d", MESSAPK[520], ASS_COD3);
                               else                           sprintf(buffA, "<A>: %s",     ASS_LAV3);
                               break;
           case PRES_LAV4:     if(AbbinaCodiceStatoBox==true) sprintf(buffA, "<A>: %s%02d", MESSAPK[520], ASS_COD4);
                               else                           sprintf(buffA, "<A>: %s",     ASS_LAV4);
                               break;
           case PRES_LAV5:     if(AbbinaCodiceStatoBox==true) sprintf(buffA, "<A>: %s%02d", MESSAPK[520], ASS_COD5);
                               else                           sprintf(buffA, "<A>: %s",     ASS_LAV5);
                               break;
           case PRES_LAV6:     if(AbbinaCodiceStatoBox==true) sprintf(buffA, "<A>: %s%02d", MESSAPK[520], ASS_COD6);
                               else                           sprintf(buffA, "<A>: %s",     ASS_LAV6);
                               break;
           case PRES_LAV7:     if(AbbinaCodiceStatoBox==true) sprintf(buffA, "<A>: %s%02d", MESSAPK[520], ASS_COD7);
                               else                           sprintf(buffA, "<A>: %s",     ASS_LAV7);
                               break;
           case PRES_LAV8:     if(AbbinaCodiceStatoBox==true) sprintf(buffA, "<A>: %s%02d", MESSAPK[520], ASS_COD8);
                               else                           sprintf(buffA, "<A>: %s",     ASS_LAV8);
                               break;
           case PRES_VUOTO:    sprintf(buffA, "<A>: %s",      MESSAPK[521]);
                               break;
           default:            strcpy(buffA, "<A>: UNKNOW");
                               break;
        }
        switch(AGV[i]->mission.PresCEST_B){
           case PRES_LAV1:     if(AbbinaCodiceStatoBox==true) sprintf(buffB, "<B>: %s%02d", MESSAPK[520], ASS_COD1);
                               else                           sprintf(buffB, "<B>: %s",     ASS_LAV1);
                               break;
           case PRES_LAV2:     if(AbbinaCodiceStatoBox==true) sprintf(buffB, "<B>: %s%02d", MESSAPK[520], ASS_COD2);
                               else                           sprintf(buffB, "<B>: %s",     ASS_LAV2);
                               break;
           case PRES_LAV3:     if(AbbinaCodiceStatoBox==true) sprintf(buffB, "<B>: %s%02d", MESSAPK[520], ASS_COD3);
                               else                           sprintf(buffB, "<B>: %s",     ASS_LAV3);
                               break;
           case PRES_LAV4:     if(AbbinaCodiceStatoBox==true) sprintf(buffB, "<B>: %s%02d", MESSAPK[520], ASS_COD4);
                               else                           sprintf(buffB, "<B>: %s",     ASS_LAV4);
                               break;
           case PRES_LAV5:     if(AbbinaCodiceStatoBox==true) sprintf(buffB, "<B>: %s%02d", MESSAPK[520], ASS_COD5);
                               else                           sprintf(buffB, "<B>: %s",     ASS_LAV5);
                               break;
           case PRES_LAV6:     if(AbbinaCodiceStatoBox==true) sprintf(buffB, "<B>: %s%02d", MESSAPK[520], ASS_COD6);
                               else                           sprintf(buffB, "<B>: %s",     ASS_LAV6);
                               break;
           case PRES_LAV7:     if(AbbinaCodiceStatoBox==true) sprintf(buffB, "<B>: %s%02d", MESSAPK[520], ASS_COD7);
                               else                           sprintf(buffB, "<B>: %s",     ASS_LAV7);
                               break;
           case PRES_LAV8:     if(AbbinaCodiceStatoBox==true) sprintf(buffB, "<B>: %s%02d", MESSAPK[520], ASS_COD8);
                               else                           sprintf(buffB, "<B>: %s",     ASS_LAV8);
                               break;
           case PRES_VUOTO:    sprintf(buffB, "<B>: %s",      MESSAPK[521]);
                               break;
           default:            strcpy(buffB, "<B>: UNKNOW");
                               break;
        }
        strcpy(buffstr, buffB);
        strcat(buffstr, "\n"); // spazio di divisioria
        strcat(buffstr, buffA);
        memcpy(&Dati.Codice[0], &buffstr[0], 50); //Dati.Codice[49]=NULL;
     }
     // ALLARMI INTERNO PC
     memset(&buffstr[0], NULL, sizeof(buffstr));
     switch(AGV[i]->allarme_interno){
        case ALL_INT_RESET_DATI:          strcpy(buffstr, MESSAPK[508]); break;
        case ALL_INT_SCAR_MANUALE:        strcpy(buffstr, MESSAPK[509]); break;
        case ALL_INT_POS_NOT_OK:          strcpy(buffstr, MESSAPK[510]); break;
        case ALL_INT_STOP_TGV:            strcpy(buffstr, MESSAPK[514]); break;
        case ALL_INT_DATI_ANOMALI:        strcpy(buffstr, MESSAPK[515]); break;
        case ALL_INT_ERR_COMUNIC:         strcpy(buffstr, MESSAPK[516]); break;
        case ALL_INT_RICARICA_OCC:        strcpy(buffstr, MESSAPK[518]); break;
        case ALL_INT_START_DA_OCC:        strcpy(buffstr, MESSAPK[519]); break;
        case ALL_INT_ID_NOT_OK:           strcpy(buffstr, MESSAPK[517]); break;
     }
     memcpy(&Dati.AllPC[0], &buffstr[0], 50); //Dati.AllPC[49]=NULL;
     // ALLARMI PLC
     memset(&buffstr[0], NULL, sizeof(buffstr));
     // per test
   //AGV[i]->allarme_esterno[1]=3;
     //-------------------------------------------------------------
     // Rilettura del file allarmi in base alla versione della
     // Pollicino
     //-------------------------------------------------------------
     errore_file2 = load_ALLARMAPK(i);
     //-------------------------------------------------------------
     // Visualizzazione allarmi presenti
     //-------------------------------------------------------------
     if(errore_file2==0){
        n=0;
        tot_all=0;
        for( j=0;j<MAXLRALLARMI;j++){
           for( k=0;k<16;k++){
              if(((AGV[i]->allarme_esterno[j]>>k)&1) == 1 ){
                 //---------------------------------------------------
                 // Verifica il bit dell'allarme
                 //---------------------------------------------------
                 Copy(ALLARMAPK[n], 5,   5, buffer);
                 if(strcmp(buffer, "     ")==0) continue;
                 //---------------------------------------------------
                 // Registrazione della stringa
                 //---------------------------------------------------
                 strcpy(buffstr, ALLARMAPK[n]);
                 memcpy(&Dati.AllPLC[tot_all][0], &buffstr[0], 50); //Dati.AllPLC[tot_all][49]=NULL;
                 tot_all++;
              }
              n++;
              //-----------------------------------
              // incremento allarmi visualizzati
              //-----------------------------------
              if(tot_all>5) break;    // max 5 allarmi
           }
           if(tot_all>5) break;       // max 5 allarmi
        }
     }
  }
  //-------------------------------------------------
  // compilazione della stringa di risp
  //-------------------------------------------------
  if(!err){
     StringaTrs[ 0 ]=IdxMess;
     StringaTrs[ 1 ]=TypMess;
     memcpy(&StringaTrs[2], (BYTE *)(&Dati), sizeof(Dati));
     LenRisposta = (unsigned short int)(2 + sizeof(Dati));
     //--------------------------------------------
     // Trasmissione via Socket
     //--------------------------------------------
     err = TcpSend(num_socket, StringaTrs, LenRisposta, all_mess);
  }
  return err;
}
//---------------------------------------------------------------------------

//-----------------------------------------------------
//   TcpComandoWriteCmd22()
//
// GESTIONE COMANDADO PARAMETRI COMUNICAZIONE TGV
//-----------------------------------------------------
int  TFormSrvTcpAndroid::TcpComandoWriteCmd22(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess)
{
  char buffer[101]="";
  char buffstr[101]="";
  int  i, err;
  WORD buff;
  WORD B1, B2;
  BYTE NrTGV_Star=0;
  unsigned int  LenRisposta;
  unsigned char StringaTrs[2051];   // Stringa da trasmettere
  struct STATO_TGV{
     BYTE Num;
     BYTE IP[15];
     WORD Port;
     BYTE TypTGV;
  } Dati;

  err=0;
  strcpy(all_mess, "OK");
  memset(&Dati,          0, sizeof(Dati));
  memset(&buffer,     NULL, sizeof(buffer));
  memset(&buffstr[0], NULL, sizeof(buffstr));
  memset(&StringaTrs, NULL, sizeof(StringaTrs));
  //-------------------------------------------------
  // Verifico che il comando sia quello corretto
  //-------------------------------------------------
  if(TypMess!=CMD_22_PARAMETRI_COM_TGV) return 0;
  if(LenMessaggioRx!=4                ) return 0;
  //-------------------------------------------------
  // Recupero dalla stringa in ingresso il numero
  // Iniziale e il totale TGV da ritrasmettere
  //-------------------------------------------------
  NrTGV_Star = (BYTE)(StringaRicevuta[ 2 ]);
  if(NrTGV_Star<1 || NrTGV_Star>MAXAGV) return 0;    // esiste solo Tgv start (singolo)
  //-------------------------------------------------
  // Aggiornamento struttura dati richiesti
  //-------------------------------------------------
  i = NrTGV_Star;
  // TGV
  Dati.Num = (BYTE)(i);
  // INDIRIZZO IP
  strcpy(buffstr, OmrPlc[i].RemoteHost.c_str());
  memcpy(&Dati.IP[0], &buffstr[0], 15); //Dati.IP[14]=NULL;
  // PORTA DI COMUNICAZIONE
  buff = (WORD)(OmrPlc[i].RemotePort);
  B1   = (WORD)(buff>>8);
  B2   = (WORD)(buff & 0x00ff);
  buff = (WORD)((B2<<8) + B1);
  Dati.Port = buff;
  // TIPO TGV
  // di default coincide con il tipo impianto, ma può essere
  // utilizzato in maniera specifica per gestire gli impianti
  // Multi-Tgv
  Dati.TypTGV = ABILITA_OPZIONI_ANDROID;
  //-------------------------------------------------
  // compilazione della stringa di risp
  //-------------------------------------------------
  if(!err){
     StringaTrs[ 0 ]=IdxMess;
     StringaTrs[ 1 ]=TypMess;
     memcpy(&StringaTrs[2], (BYTE *)(&Dati), sizeof(Dati));
     LenRisposta = (unsigned short int)(2 + sizeof(Dati));
     //--------------------------------------------
     // Trasmissione via Socket
     //--------------------------------------------
     err = TcpSend(num_socket, StringaTrs, LenRisposta, all_mess);
  }
  return err;
}
//---------------------------------------------------------------------------

//-----------------------------------------------------
//   TcpComandoWriteCmd30()
//
// GESTIONE COMANDADO STATO TUTTE LE MACCHINE
//-----------------------------------------------------
int  TFormSrvTcpAndroid::TcpComandoWriteCmd30(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess)
{
  return 0;
}
//---------------------------------------------------------------------------

//-----------------------------------------------------
//   TcpComandoWriteCmd31()
//
// GESTIONE COMANDADO STATO SINGOLA MACCHINA
//-----------------------------------------------------
int  TFormSrvTcpAndroid::TcpComandoWriteCmd31(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess)
{
  return 0;
}
//---------------------------------------------------------------------------

//-----------------------------------------------------
//   TcpComandoWriteCmd40()
//
// GESTIONE COMANDADO STATO TUTTI I MAGAZZINI
//-----------------------------------------------------
int  TFormSrvTcpAndroid::TcpComandoWriteCmd40(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess)
{
  return 0;
}
//---------------------------------------------------------------------------

//-----------------------------------------------------
//   TcpComandoWriteCmd50()
//
// GESTIONE STATO TUTTI I BOX
//-----------------------------------------------------
int  TFormSrvTcpAndroid::TcpComandoWriteCmd50(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess)
{
  return 0;
}
//---------------------------------------------------------------------------

//-----------------------------------------------------
//   TcpComandoWriteCmd60()
//
// GESTIONE COMANDADO STATO TUTTI I CODICI
//-----------------------------------------------------
int  TFormSrvTcpAndroid::TcpComandoWriteCmd60(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess)
{
  int  i, err=0;
  char buffstr[101]="";
  short int num_cod;
  WORD buff;
  WORD NrCod_Star=0;
  WORD NrCod_Fine=0;
  unsigned int  LenRisposta;
  unsigned char StringaTrs[2051];   // Stringa da trasmettere
  struct STATO_COD{
     BYTE Num;
     BYTE Codice[50];
     BYTE StatLav;
  } Dati[TOTCODICIMAG+1];


  strcpy(all_mess, "OK");
  memset(&Dati,          0, sizeof(Dati));
  memset(&buffstr[0], NULL, sizeof(buffstr));
  memset(&StringaTrs, NULL, sizeof(StringaTrs));
  //-------------------------------------------------
  // Verifico che il comando sia quello corretto
  //-------------------------------------------------
  if(TypMess!=CMD_60_STATO_ALL_COD ) return 0;
  if(LenMessaggioRx!=4             ) return 0;
  //-------------------------------------------------
  // Recupero dalla stringa in ingresso il numero
  // Iniziale e il totale TGV da ritrasmettere
  //-------------------------------------------------
  NrCod_Star = (BYTE)(StringaRicevuta[ 2 ]);
  NrCod_Fine = (BYTE)(StringaRicevuta[ 3 ]);
//if(NrCod_Star<0 || NrCod_Star>TOTCODICIMAG-1) return 0;    // numero COD non ammesso
//if(NrCod_Fine<0 || NrCod_Fine>TOTCODICIMAG-1) return 0;    // numero COD non ammesso
  if(NrCod_Star>TOTCODICIMAG-1) return 0;    // numero COD non ammesso
  if(NrCod_Fine>TOTCODICIMAG-1) return 0;    // numero COD non ammesso
  //-------------------------------------------------
  // Aggiornamento struttura dati richiesti
  //-------------------------------------------------
  num_cod=0;
  for(i=NrCod_Star; i<NrCod_Fine; i++){
     // NR.CODICE
     buff = (WORD)(i);
   //B1   = (WORD)(buff>>8);
   //B2   = (WORD)(buff & 0x00ff);
   //buff = (WORD)((B2<<8) + B1);
     Dati[num_cod].Num = (BYTE)(buff);
     // CODICE
     memset(&buffstr[0], NULL, sizeof(buffstr));
     strcpy(buffstr, CodMag[i].Cod);
     strcat(buffstr, "\n"); // inserisco il carattere "a capo"
     strcat(buffstr, CodMag[i].Des);
     memcpy(&Dati[num_cod].Codice[0], &buffstr[0], 50); //Dati[num_box].Codice[49]=NULL;
     // STATO LAVORAZIONI
     Dati[num_cod].StatLav = (BYTE)(CodMag[i].Lav);
     num_cod++;
  }
  //-------------------------------------------------
  // compilazione della stringa di risp
  //-------------------------------------------------
  if(!err){
     StringaTrs[ 0 ]=IdxMess;
     StringaTrs[ 1 ]=TypMess;
     memcpy(&StringaTrs[2], (BYTE *)(&Dati), (int)(sizeof(STATO_COD)*num_cod));
     LenRisposta = (unsigned short int)(2+(sizeof(STATO_COD)*num_cod));
     //--------------------------------------------
     // Trasmissione via Socket
     //--------------------------------------------
     err = TcpSend(num_socket, StringaTrs, LenRisposta, all_mess);
  }
  return err;
}
//---------------------------------------------------------------------------

//-----------------------------------------------------
//   TcpComandoWriteCmd61()
//
// GESTIONE COMANDADO STATO SINGOLO CODICE
//-----------------------------------------------------
int  TFormSrvTcpAndroid::TcpComandoWriteCmd61(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess)
{
  return 0;
}
//---------------------------------------------------------------------------

//-----------------------------------------------------
//   TcpComandoWriteCmd62()
//
// GESTIONE COMANDO STATO SINGOLO CODICE (FOOD)
//-----------------------------------------------------
int  TFormSrvTcpAndroid::TcpComandoWriteCmd62(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess)
{
  return 0;
}
//---------------------------------------------------------------------------

//-----------------------------------------------------
//   TcpComandoWriteCmd70()
//
// GESTIONE COMANDO STATO TUTTE LE CELLE
//-----------------------------------------------------
int  TFormSrvTcpAndroid::TcpComandoWriteCmd70(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess)
{
  return 0;
}
//---------------------------------------------------------------------------

//-----------------------------------------------------
//   TcpComandoWriteCmd71()
//
// GESTIONE COMANDO STATO SINGOLA CELLA
//-----------------------------------------------------
int  TFormSrvTcpAndroid::TcpComandoWriteCmd71(int num_socket, unsigned char* StringaRicevuta, unsigned int LenMessaggioRx, unsigned char IdxMess, unsigned char TypMess, char* all_mess)
{
  return 0;
}
//---------------------------------------------------------------------------

