//---------------------------------------------------------------------------

#include <vcl.h>
#include <time.h>
#include <systdate.h>

#pragma hdrstop

#include "Main.h"
#include "SQLDB.h"
#include "DBGridGen.h"
//---------------------------------------------------------------------------
// PLUGIN IMPIANTI
//---------------------------------------------------------------------------
#include PLANT_PLUGIN

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "XLSMini"
#pragma link "CGAUGES"
#pragma resource "*.dfm"
TFormSQLDB *FormSQLDB;
//---------------------------------------------------------------------------
__fastcall TFormSQLDB::TFormSQLDB(TComponent* Owner) : TForm(Owner)
{

}
//---------------------------------------------------------------------------
void TFormSQLDB::InitTabsSQL()
{
  if(InitTabOn==true) return;

  ADOTabTab  = new TADOTable(FormSQLDB);
  DSTab      = new TDataSource(FormSQLDB);
  //------------------------------------------------
  // Parametrizzazioni oggetti ADO
  //------------------------------------------------
  ADOTabTab->ConnectionString   = FormMain->ADOConnSQL->ConnectionString;
  ADOTabTab->TableName          = NomeTabA;
  DSTab->DataSet                = ADOTabTab;
  DBGridTab->DataSource         = DSTab;
  //-----------------------------------------
  // Inizializzazione Tabsheet Tabelle
  //-----------------------------------------
  TabTab->Caption=NomeTabA;
  //-----------------------------------------
  InitTabOn=true;
}
//---------------------------------------------------------------------------
void TFormSQLDB::DeleteTabsSQL()
{
  if(InitTabOn==false) return;
  try{
     //-----------------------------------------------------
     // Distruzione della Tabella
     //-----------------------------------------------------
     delete ADOTabTab;
     delete DSTab;
     //-----------------------------------------------------
     // Aggiornamento status bar
     //-----------------------------------------------------
     StatusBarSQL->Panels->Items[0]->Text = "";
     InitTabOn=false;
  }
  catch(...){
     StatusBarSQL->Panels->Items[0]->Text = "Error in destroy Tables.";
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormSQLDB::FormCreate(TObject *Sender)
{
  //-----------------------------------------
  // Inizializzazione delle varibili
  //-----------------------------------------
  time_err_sql     = time(NULL);
  index_refresh    = 0;
  ComandoSQL_ON    = false;
  InitTabOn        = false;
  pCanvasRefreshDB = FormSQLDB->Canvas;
  //-----------------------------------------
  Width  = 770;
  Height = 640;

  Constraints->MinHeight = Height;
  Constraints->MaxHeight = 1000;
  Constraints->MinWidth  = Width;
  Constraints->MaxWidth  = 1700;
  //-----------------------------------------
  // Inizializzazione del THREAD SQL
  //-----------------------------------------
  t_SQL = new TMyThread1();
  t_SQL->Suspend();           // now suspend the thread running
}
//---------------------------------------------------------------------------
void __fastcall TFormSQLDB::FormActivate(TObject *Sender)
{
  //-----------------------------------------
  // Inizializzazione variabili
  //-----------------------------------------
  TabIsOpened=false;
  ComandoSQL_ON=false;
  //-----------------------------------------
  // Inserimento ammesso solo da PC ALGO
  //-----------------------------------------
  BtnInsert->Visible=false;
  if(PcAlgo==true) BtnInsert->Visible=true;
  //-----------------------------------------
  // Compila combobox per DBGridGen
  //-----------------------------------------
  cbListaTabelle->Clear();
  if(NomeTabA.IsEmpty()==false) cbListaTabelle->Items->Add(NomeTabA.c_str());
  cbListaTabelle->ItemIndex=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormSQLDB::FormClose(TObject *Sender, TCloseAction &Action)
{
  //-----------------------------------------
  // Chiusura tabelle a chiusura della form
  //-----------------------------------------
  if(TabIsOpened==true){
     ADOTabTab->Close();
     //-----------------------------------------------------
     // Reset contatori
     //-----------------------------------------------------
     TxtNRecTab->Caption = "REC:\n0";
     //-----------------------------------------------------
     // Memorizzo tabelle chiuse
     //-----------------------------------------------------
     TabIsOpened = false;
     //-----------------------------------------------------
     // destroy della Tabella
     //-----------------------------------------------------
     DeleteTabsSQL();
     //-----------------------------------------------------
     // Aggiornamento status bar
     //-----------------------------------------------------
     StatusBarSQL->Panels->Items[0]->Text = "";
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormSQLDB::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  switch (Key){ case VK_ESCAPE: Close(); break;}
  if( Shift.Contains(ssCtrl) && Key==VK_DELETE) Key=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormSQLDB::TimerRefreshTimer(TObject *Sender)
{
  //------------------------------------------
  // Gestione grafica refresh grafico tabella
  //------------------------------------------
  if(ComandoSQL_ON){
     index_refresh++;
     if(index_refresh>21){
        //ComandoSQL_ON=false;
        index_refresh=0;
     }
     FormSQLDB->Paint();
  }
  else index_refresh=0;

  ImgLstDB->Draw(pCanvasRefreshDB, 2, 2, index_refresh);
}
//---------------------------------------------------------------------------
void __fastcall TFormSQLDB::BtnOpenTableClick(TObject *Sender)
{
  try{
     //-----------------------------------------------------
     // Inizializzazione della Tabella
     //-----------------------------------------------------
     InitTabsSQL();
     //-----------------------------------------------------
     // Apertura della Tabella
     //-----------------------------------------------------
     ADOTabTab->Open();
     //-----------------------------------------------------
     // Contatori RECORD
     //-----------------------------------------------------
     TxtNRecTab->Caption = "REC:\n" + IntToStr(ADOTabTab->RecordCount);
     //-----------------------------------------------------
     // Memorizzo che le tabelle sono state aperte
     //-----------------------------------------------------
     TabIsOpened=true;
     ComandoSQL_ON=true;
     //-----------------------------------------------------
     // Aggiornamento status bar
     //-----------------------------------------------------
     StatusBarSQL->Panels->Items[0]->Text = "";
  }
  catch(...){
     StatusBarSQL->Panels->Items[0]->Text = "Error in open Tables.";
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormSQLDB::BtnCloseTableClick(TObject *Sender)
{
  try{
     //-----------------------------------------------------
     // Apertura della Tabella
     //-----------------------------------------------------
     ADOTabTab->Close();
     //-----------------------------------------------------
     ComandoSQL_ON=false;
     //-----------------------------------------------------
     // Reset contatori
     //-----------------------------------------------------
     TxtNRecTab->Caption = "REC:\n0";
     //-----------------------------------------------------
     // Memorizzo tabelle chiuse
     //-----------------------------------------------------
     TabIsOpened = false;
     //-----------------------------------------------------
     // destroy della Tabella
     //-----------------------------------------------------
     DeleteTabsSQL();
     //-----------------------------------------------------
     // Aggiornamento status bar
     //-----------------------------------------------------
     StatusBarSQL->Panels->Items[0]->Text = "";
  }
  catch(...){
     StatusBarSQL->Panels->Items[0]->Text = "Error in close Tables.";
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormSQLDB::FormPaint(TObject *Sender)
{
  ImgLstDB->Draw(pCanvasRefreshDB, 2, 2, index_refresh);
}
//---------------------------------------------------------------------------
void __fastcall TFormSQLDB::BtnInsertClick(TObject *Sender)
{
  int  i;
  char StringaSQL[701];
  TADOQuery *ADOQueryTab;

  //-----------------------------------------------
  // Inizializzazione query di aggiornamento
  //-----------------------------------------------                 
  ADOQueryTab = new TADOQuery(FormSQLDB);
  ADOQueryTab->ConnectionString = FormMain->ADOConnSQL->ConnectionString;
  TabIsOpened = true;
  //-----------------------------------------------
  // Inserimento automatico record in tabella
  //-----------------------------------------------
  /************************************************
  for(i=1; i<910; i++){
     sprintf(StringaSQL, "INSERT INTO %s ([Baia], [UbicSPAL], [GruppoBaie], [GruppoUbicSpal], [TipoBaia], [StatoUbic], [Esclusione], [Asservimento], [GruppoDest], [GruppoDestSpal], [TipoCodice], [Codice1], [UDM1], [Codice2], [UDM2], [TipoPallet], [TecnoChange], [SpalChange]) VALUES (%d, '', 0, '', 0, 0, 0, 0, 0, '', '', '', '', '', '', 0, 0, 0)", NomeTabA, i);
     ADOQueryTab->SQL->Clear();
     ADOQueryTab->SQL->Add(StringaSQL);
     ADOQueryTab->ExecSQL();
  }
  ************************************************/
  //-----------------------------------------------
  // destroy query di inserimento
  //-----------------------------------------------
  TabIsOpened = false;
  ADOQueryTab->Close();
  delete ADOQueryTab;
}
//---------------------------------------------------------------------------
//=======================================================================
//
//              Funzioni di lettura scrittura dati su SQL
//
//=======================================================================

// -----------------------------------
//   delay_on_err_SQL()
// -----------------------------------
// Funzione di attesa su errore SQL (return TRUE su attesa da fare)
// DA RICHIAMARE PRIMA DEL LANCIO DI UN NUOVO COMANDO SU SQL
//
//
bool TFormSQLDB::delay_on_err_SQL()
{
  long int timeattuale;
  long int diff_time;

  timeattuale=time(NULL);
  //-----------------------------------------
  // Verifico se la comunicazione su SQL
  // è in errore
  //-----------------------------------------
  if(errore_comunicazione[COM3][DB_DBASE]==0) time_err_sql=time(NULL);
  //-----------------------------------------
  // Controllo quanto tempo è trascorso
  // sulla rilevazione dell'allarme
  //-----------------------------------------
  diff_time = timeattuale - time_err_sql;
  //-----------------------------------------
  // In condizione di allarme e tempo non
  // scaduto inizializzo l'attesa
  //-----------------------------------------
  if(diff_time<SecDelayOnErrSQL && errore_comunicazione[COM3][DB_DBASE]!=0){
     return TRUE;
  }
  //-----------------------------------------
  // No attesa --> Skip!!!
  //-----------------------------------------
  time_err_sql=time(NULL);
  return FALSE;
}
//---------------------------------------------------------------------------
// -----------------------------------
//   load_udc_from_SQL()
// -----------------------------------
// Funzione di lettura stato UDC
//
int TFormSQLDB::load_udc_from_SQL(int cmd, unsigned int locS, unsigned int locE, char *UdcS, char *UdcE, char *all_mess)
{
  int err=0;
  struct DATI_PRD UdcTmp[MAXUDCIMPIANTO];

  //--------------------------------------
  // Comunicazione disattivata
  //--------------------------------------
  if(break_Com[COM3]==true) return 0;
  //--------------------------------------
  // Reset struttura temporanea
  //--------------------------------------
  memset(&UdcTmp[0], 0, sizeof(UdcTmp));
  //--------------------------------------
  // Richiamo funzione specializzata
  // accesso al DATABASE
  //--------------------------------------
  err = load_udc_from_SQL_ICAM(cmd, &UdcTmp[0], locS, locE, UdcS, UdcE, all_mess);
  if(err) errore_comunicazione[COM3][DB_DBASE]=ERR_SQL_RD_TAB;
  else{
     errore_comunicazione[COM3][DB_DBASE]=0;
     //-------------------------------------------------
     // Aggiorno i dati di tutti gli UDC LETTI in base
     // alle strutture dove di si trovano
     //-------------------------------------------------
     err=B.UdcTmpToStruct(&UdcTmp[0], all_mess);
     //-----------------------------------------------
     // Dopo la lettura azzera il CustomerChange
     // per i comandi che lo leggono
     //-----------------------------------------------
     if(!err){
        err=write_udc_to_SQL_ICAM(CMD_WR_SQL_UDC_RST_CUCH, &UdcTmp[0], all_mess);
     }
  }
  return err;
}

//---------------------------------------------------------------------------
// -----------------------------------
//    write_udc_to_SQL()
// -----------------------------------
// aggiornamento del UDC
//
int TFormSQLDB::write_udc_to_SQL(int cmd, struct DATI_PRD *UdcTmp, char *all_mess)
{
  int err=0;

  //--------------------------------------
  // Comunicazione disattivata
  //--------------------------------------
  if(break_Com[COM3]==true) return 0;

  //--------------------------------------
  // Richiamo funzione specializzata
  // accesso al DATABASE
  //--------------------------------------
  err = write_udc_to_SQL_ICAM(cmd, &UdcTmp[0], all_mess);
  if(err) errore_comunicazione[COM3][DB_DBASE]=ERR_SQL_RD_TAB;
  else    errore_comunicazione[COM3][DB_DBASE]=0;

  return err;
}

//---------------------------------------------------------------------------

//----------------------------------------------------
//      Algo_TimeInt_To_DateTime()
// N.B. Non Funziona perchè c'è il problema del separatore
// N.B. Modificata da Albertino per il separatore 01/04/2016
//----------------------------------------------------
TDateTime TFormSQLDB::Algo_TimeInt_To_DateTime(long int TimeInt)
{
  TDateTime datatimebuf;
  SYSTEMTIME SystemTime;
  struct tm *t;

  if(TimeInt==0) return 0;

  t = localtime( &TimeInt );
  SystemTime.wYear           = t->tm_year+1900;
  SystemTime.wMonth          = t->tm_mon+1;
  SystemTime.wDay            = t->tm_mday;
  SystemTime.wHour           = t->tm_hour;
  SystemTime.wMinute         = t->tm_min;
  SystemTime.wSecond         = t->tm_sec;
  SystemTime.wDayOfWeek      = t->tm_wday;
  SystemTime.wMilliseconds   = 0;

  datatimebuf = SystemTimeToDateTime(SystemTime);
  datatimebuf.FormatString("dd/mm/yyyy hh:mm:ss");

  return datatimebuf;
}
//------------------------------------------------------------
//----------------------------------------------------
//      Algo_DateTime_To_TimeInt()
//----------------------------------------------------
long int TFormSQLDB::Algo_DateTime_To_TimeInt(TDateTime DateTimeTemp)
{
  SYSTEMTIME SystemTime;
  char Data[20], Ora[20];
  long int TimeInt;

  DateTimeToSystemTime( DateTimeTemp, SystemTime);
  sprintf( Data, "%02d/%02d/%04d", SystemTime.wDay,  SystemTime.wMonth,  SystemTime.wYear   );
  sprintf( Ora,  "%02d:%02d:%02d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond );
  timechar_to_timeint(&TimeInt, Data, Ora);

  return TimeInt;
}
//------------------------------------------------------------
//----------------------------------------------
//      Algo_TimeInt_To_DateTimeString()
// N.B. Il separatore è ":"
//----------------------------------------------
char* TFormSQLDB::Algo_TimeInt_To_DateTimeString(long int TimeInt, char* CharTimeSQL)
{
  struct tm *t;

  t = localtime( &TimeInt );
  sprintf( CharTimeSQL, "%02d/%02d/%04d %02d:%02d:%02d", t->tm_mday, t->tm_mon+1, t->tm_year+1900, t->tm_hour, t->tm_min, t->tm_sec);
//  sprintf( CharTimeSQL, "%02d/%02d/%04d %02d:%02d:%02d", t->tm_mon+1, t->tm_mday, t->tm_year+1900, t->tm_hour, t->tm_min, t->tm_sec);
  return CharTimeSQL;
}
//------------------------------------------------------------
//----------------------------------------------
//      TimeSeparatorConvert()
//  converte i separatori in una stringa datetime
//  '.' --> ':'
//  '-' --> '/'
//----------------------------------------------
void TFormSQLDB::TimeSeparatorConvert(char* CharDateTime)
{
  //for(int i=0;i<sizeof(*CharDateTime);i++){
  for(int i=0;;i++){
     if(CharDateTime[i]=='\0') break;
     if(CharDateTime[i]=='.') CharDateTime[i]=':';
     if(CharDateTime[i]=='-') CharDateTime[i]='/';
  }
}
//----------------------------------------------
void __fastcall TFormSQLDB::DBGridTabTitleClick(TColumn *Column)
{
  if(FlipFlop==true) ADOTabTab->Sort=Column->FieldName + " ASC";
  else               ADOTabTab->Sort=Column->FieldName + " DESC";

  FlipFlop=!FlipFlop;
}
//---------------------------------------------------------------------------
void __fastcall TFormSQLDB::SaveDialogFolderChange(TObject *Sender)
{
  AnsiString PathProject;

  PathProject = ExtractFilePath(ParamStr(0));
  SetCurrentDir(PathProject);
}
//---------------------------------------------------------------------------
void __fastcall TFormSQLDB::btnDBGridClick(TObject *Sender)
{
  if(FormDBGridGen->SQL_Table != cbListaTabelle->Text) FormDBGridGen->Executed=false;
  FormDBGridGen->SQL_Table = cbListaTabelle->Text;
  FormDBGridGen->ADOQuery->Connection=FormMain->ADOConnSQL;
  FormDBGridGen->ShowModal();
}
//---------------------------------------------------------------------------

