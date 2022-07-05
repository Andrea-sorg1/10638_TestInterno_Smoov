//---------------------------------------------------------------------------
#include <vcl.h>
#include <vcl\Clipbrd.hpp>

#pragma hdrstop

#include "DBGridGen.h"
#include "Main.h"
#include "Input.h"
#include "SQLDB.h"
#include "visfile.h"
#include "ListaGen.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CGAUGES"
#pragma link "XLSMini"
#pragma resource "*.dfm"
TFormDBGridGen *FormDBGridGen;
//---------------------------------------------------------------------------
__fastcall TFormDBGridGen::TFormDBGridGen(TComponent* Owner) : TForm(Owner)
{
  if(Width <650) Width  = 650;
  if(Height<600) Height = 600;
  //-----------------------------------
  Constraints->MinHeight = Height;
  Constraints->MaxHeight = 1000;
  Constraints->MinWidth  = Width;
  Constraints->MaxWidth  = 1800;

  dbCB1  = new TComboBox*    [MAX_COMBO+1];
  dbCB2  = new TComboBox*    [MAX_COMBO+1];
  dbLabel= new TLabel*       [MAX_COMBO+1];
  myList = new TStringList*  [MAX_COMBO+1];

  if(PcAlgo)  btnMaxRecord->Visible=true;
  else        btnMaxRecord->Visible=false;
  myMaxRecord="100";
  ADOQuery->MaxRecords=StrToIntDef(myMaxRecord,200)*1000;
  ADOQuery->CommandTimeout = SecComandoTimeOutSQL;

  NumCombo=MAX_COMBO;
  GestioneCreateComboBox();
  Executed=false;
  SqlQueryExt=false;
  EnableTableCB=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::FormActivate(TObject *Sender)
{
  long int TimeDiff;
  long int TimeAttuale;
  char  all_mess[101]="";
  TimeAttuale=time(NULL);
  AnsiString NoLock="";
  //---------------------------------------
  Reload=false;
  btnSpec01->Visible = false;
  //---------------------------------------
  // Azzero errore di comunicazione prima
  // di procedere con una nuova
  //---------------------------------------
  // FormSQLDB->ComandoSQL_ON=true;
  errore_comunicazione[COM3][DB_EXTERN]=0;
  //---------------------------------------
  if(NoLockQuery==true) NoLock = " WITH (nolock) ";

  if(SqlQueryExt==false || SQL_Query.IsEmpty()==true){
     SQL_Query="";
     SQL_Query = "set dateformat dmy; ";   // mySQL non accetta il dateformat
     SQL_Query += "SELECT * FROM " + SQL_Table + NoLock;
  }
  lblSQL->Caption=SQL_Query;

  try{
     //----------------------------
     DBGrid->Visible      = false;
     pnlCombo->Visible    = false;
     pnlBottoni->Visible  = false;
     ADOQuery->DisableControls();
     //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     ADOQuery->Close();
     //ADOQuery->ExecuteOptions<<eoAsyncFetch;
     ADOQuery->SQL->Clear();
     ADOQuery->SQL->Add(SQL_Query);
     Gauge->Progress=0;
     ADOQuery->Open();
     //----------------------------
     Caption=SQL_Table + " Record Count = " + IntToStr(ADOQuery->RecordCount);
     if(ADOQuery->RecordCount >=  ADOQuery->MaxRecords){
        Caption = Caption + " (MAX RECORDS VIEW REACHED)";
     }
     //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     DBGrid->Visible      = true;
     pnlCombo->Visible    = true;
     pnlBottoni->Visible  = true;
     //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     //----------------------------
     NumCombo=DataSource->DataSet->FieldCount;

     if(Executed==false){
        GestioneComboBox();
     }
     ADOQuery->EnableControls();
  }
  //--------------------------------------------------
  catch(Exception &exception){
     Application->ShowException(&exception);
     errore_comunicazione[COM3][DB_EXTERN]=ERR_SQL_RD_TAB;
     //---------------------------------------------------------------------------------
     char messaggio[901]="";
     char FileLogExceptionSQL[101]="";
     sprintf(FileLogExceptionSQL, "%sSQL\\LogExceptionSQL.txt", PathProject.c_str());
     sprintf(messaggio,"DBGridGen::FormActivate;%s;%s", all_mess, exception.Message.c_str() );
     messaggio[498]=NULL;    // per garantire il corretto scroll
     file_cronologico_generico(FileLogExceptionSQL, messaggio, 50000L);
     strcpy(messaggio, SQL_Query.c_str());
     messaggio[498]=NULL;    // per garantire il corretto scroll
     file_cronologico_generico(FileLogExceptionSQL, messaggio, 50000L);
     lbl_Message->Caption=exception.Message;
     //---------------------------------------------------------------------------------
  }

  TimeDiff = (time(NULL) - TimeAttuale);
  char buffer[51]="";
  sprintf( buffer, "Tempo trascorso %d (sec)", TimeDiff);
  lbl_Message->Caption=buffer;
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::FormClose(TObject *Sender, TCloseAction &Action)
{
  //Run=false;
  SqlQueryExt=false;
  EnableTableCB=false;
  ADOQuery->Close();
}
//---------------------------------------------------------------------------
void TFormDBGridGen::GestioneComboBox()
{
  int  i,j;
  int  app;
  int  max_cb_line;
  char str_tmp[501];
  AnsiString Field;

  memset(&ColSize,0,sizeof(ColSize));

  max_cb_line = MAX_COMBOLINE;
  if(Executed==true) max_cb_line = MAX_COMBOLINE*2;
  Executed=true;

  try{
     for(i=0; i<MAX_COMBO; i++){
        if(i<NumCombo){
           myFieldType[i] = DataSource->DataSet->Fields->Fields[i]->DataType;
           dbCB1[i]->Visible   = true;
           dbLabel[i]->Visible = true;

           if(myFieldType[i]==ftDateTime){
              dbLabel[i]->Font->Color=(TColor)0x00FF0080;
              dbCB2[i]->Visible = true;
           }
           else{
              dbLabel[i]->Font->Color=clWindowText;
              dbCB2[i]->Visible = false;
           }
           j=i;
        }
        else{
           dbCB1[i]->Visible    = false;
           dbCB2[i]->Visible    = false;
           dbLabel[i]->Visible  = false;
        }
     }
     Height = dbCB1[j]->Top + dbCB1[j]->Height + pnlBottoni->Height + 80;
     //--------------------------------------------------
     Gauge->MaxValue = ADOQuery->RecordCount;
     Gauge->ForeColor=(TColor) 0x007280FA;
     Gauge->Progress=0;
     Gauge->Visible       = true;
     DBGrid->Visible      = false;
     pnlBottoni->Visible  = false;
     pnlCombo->Visible    = false;
     //--------------------------------------------------
     for(i=0;i<NumCombo;i++){
        if(i>=MAX_COMBO) break;
        myList[i]->Clear();
        dbLabel[i]->Caption = DataSource->DataSet->Fields->Fields[i]->FieldName;
     }

     DataSource->DataSet->First();
     KeyStop=false;
     do{
        Gauge->Progress++;
        if((Gauge->Progress%RilascioApp)==0){
           Application->ProcessMessages();
        }
        //--------------------------------------------------------------------
        EscStatus=GetAsyncKeyState(VK_ESCAPE);
        if(KeyStop==true || EscStatus!=0){
           MessageDlg("Stopped",  mtError, TMsgDlgButtons() << mbOK, 0);
           Executed=false;
           break;
        }
        //--------------------------------------------------------------------
        for(i=0;i<NumCombo;i++){
           if(i>=MAX_COMBO) break;
           if(dbCB1[i]->Visible==false) break;

           if(myList[i]->Count<max_cb_line){
              strcpy(str_tmp, Erase_White_End(DataSource->DataSet->Fields->Fields[i]->Text.c_str()));
              myList[i]->Add( str_tmp );
              app=strlen(str_tmp);
              if(app>ColSize[i])ColSize[i]=app;
           }
           else continue;
        }
        DataSource->DataSet->Next();
     }while(!DataSource->DataSet->Eof);

     DataSource->DataSet->First();

     for(i=0;i<NumCombo;i++){
        if(i>=MAX_COMBO) break;
        if(dbCB1[i]->Visible){
           dbCB1[i]->Clear();
           dbCB1[i]->Items->AddStrings(myList[i]);
           if(dbCB2[i]->Visible){
              dbCB2[i]->Clear();
              dbCB2[i]->Items->AddStrings(myList[i]);
           }
        }
        else break;
     }
  }
  //--------------------------------------------------
  catch(Exception &exception){
     Application->ShowException(&exception);
     lbl_Message->Caption=exception.Message;
  }
  //--------------------------------------------------
  Gauge->Visible       = false;
  DBGrid->Visible      = true;
  pnlBottoni->Visible  = true;
  pnlCombo->Visible    = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::DBGridTitleClick(TColumn *Column)
{
  if(FlipFlop==true)     ADOQuery->Sort=Column->FieldName + " ASC";
  else                   ADOQuery->Sort=Column->FieldName + " DESC";

  FlipFlop=!FlipFlop;
}
//---------------------------------------------------------------------------
void TFormDBGridGen::GestioneCreateComboBox()
{
  dbCB1[0]    = ComboBoxDef1;
  dbCB2[0]    = ComboBoxDef2;
  dbLabel[0]  = lblCB1;

  try{
     myList[0] = new TStringList();
     myList[0]->Clear();
     myList[0]->Sorted=true;
     myList[0]->Duplicates=dupIgnore;

     for(int i=1; i<NumCombo; i++){
        if(i>=MAX_COMBO) break;
        dbCB1[i] = new TComboBox(this);

        dbCB1[i]->OnChange = ComboBoxDef1->OnChange;

        dbCB1[i]->Anchors= ComboBoxDef1->Anchors;
        dbCB1[i]->Height = ComboBoxDef1->Height;
        dbCB1[i]->Width  = ComboBoxDef1->Width;
        dbCB1[i]->Left   = ComboBoxDef1->Left;
        dbCB1[i]->Top    = dbCB1[i-1]->Top + dbCB1[i-1]->Height+5;

        dbCB1[i]->Parent =ComboBoxDef1->Parent;
        dbCB1[i]->Visible=false;
        dbCB1[i]->Tag=i;
        //------------------------------------------------------------
        dbCB2[i] = new TComboBox(this);
        dbCB2[i]->Anchors= ComboBoxDef2->Anchors;
        dbCB2[i]->Height = ComboBoxDef2->Height;
        dbCB2[i]->Width  = ComboBoxDef2->Width;
        dbCB2[i]->Left   = ComboBoxDef2->Left;
        dbCB2[i]->Top    = dbCB1[i]->Top;

        dbCB2[i]->Parent =ComboBoxDef2->Parent;
        dbCB2[i]->Visible=false;
        //------------------------------------------------------------
        dbLabel[i]= new TLabel(this);
        dbLabel[i]->Alignment = lblCB1->Alignment;
        dbLabel[i]->Height    = lblCB1->Height;
        dbLabel[i]->Width     = lblCB1->Width;
        dbLabel[i]->Left      = lblCB1->Left;
        dbLabel[i]->Top       = dbCB1[i]->Top;

        dbLabel[i]->Parent    = lblCB1->Parent;
        dbLabel[i]->Visible   = false;

        dbLabel[i]->Caption   = "";
        //------------------------------------------------------------
        myList[i] = new TStringList();
        myList[i]->Clear();
        myList[i]->Sorted=true;
        myList[i]->Duplicates=dupIgnore;
     }
  }
  //------------------------------
  catch(Exception &exception){
     Application->ShowException(&exception);
     lbl_Message->Caption=exception.Message;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::btnFilterClick(TObject *Sender)
{
  int  i;
  int  err=0;
  bool primo=false;
  char all_mess[101]="";
  long int TimeDiff;
  long int TimeAttuale;
  AnsiString Testo1="";
  AnsiString Testo2="";
  AnsiString Separator="";
  AnsiString NoLock="";
  AnsiString StringaSQL;
  TimeAttuale=time(NULL);
  TFieldType myFieldType;
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  DBGrid->Visible      = false;
  pnlCombo->Visible    = false;
  pnlBottoni->Visible  = false;
  ADOQuery->DisableControls();
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //---------------------------------------
  // Azzero errore di comunicazione prima
  // di procedere con una nuova
  //---------------------------------------
  // FormSQLDB->ComandoSQL_ON=true;
  errore_comunicazione[COM3][DB_EXTERN]=0;
  //---------------------------------------
  if(NoLockQuery==true) NoLock = " WITH (nolock) ";
  StringaSQL="";
  StringaSQL = "set dateformat dmy; ";   // mySQL non accetta il dateformat
  StringaSQL += "SELECT * FROM " + SQL_Table + NoLock;;
  try{
     for(i=0; i<NumCombo; i++){
        if(i>=MAX_COMBO) break;
        //------------------------------------------------------------------------------
        if(dbCB2[i]->Visible==true){
           if(dbCB1[i]->Text=="" && dbCB2[i]->Text!="") dbCB1[i]->Text = dbCB2[i]->Text;
           if(dbCB2[i]->Text=="" && dbCB1[i]->Text!="") dbCB2[i]->Text = dbCB1[i]->Text;
           //------------------------------------------------------------------------------
           if(dbCB1[i]->Text!="") {
              myFieldType = DataSource->DataSet->Fields->Fields[i]->DataType;
              if(myFieldType==ftTime || myFieldType == ftDateTime){
                 if(err==0){
                    err=ParseDateTime(dbCB1[i]->Text.c_str());
                    if(err) Application->MessageBox("Time Filter 1 Error", "error", MB_OK + MB_ICONSTOP + MB_TOPMOST);
                 }
              }
           }
           if(err==0){
              if(dbCB2[i]->Text!=""){
                 myFieldType = DataSource->DataSet->Fields->Fields[i]->DataType;
                 if(myFieldType==ftTime || myFieldType == ftDateTime){
                    err=ParseDateTime(dbCB2[i]->Text.c_str());
                    if(err) Application->MessageBox("Time Filter 2 Error", "error", MB_OK + MB_ICONSTOP + MB_TOPMOST);
                 }
              }
           }
        }
        //------------------------------------------------------------------------------
        Testo1=dbCB1[i]->Text;
        Testo2=dbCB2[i]->Text;
        Separator="";
        //--------------------------------------------------------------------------------------------------
        if(Testo1 !=""){
           myFieldType = DataSource->DataSet->Fields->Fields[i]->DataType;
           if(myFieldType==ftString || myFieldType==ftWideString){
              Separator="'";                                                                   // String
           }
           if(myFieldType==ftBCD || myFieldType==ftFloat){
              Testo1 = StringReplace(dbCB1[i]->Text,",", ".",TReplaceFlags() << rfReplaceAll); // Numeric
              Testo2 = StringReplace(dbCB2[i]->Text,",", ".",TReplaceFlags() << rfReplaceAll); // Numeric
           }
           if(myFieldType==ftTime || myFieldType == ftDateTime){
              Testo1 = StringReplace(dbCB1[i]->Text,".", ":",TReplaceFlags() << rfReplaceAll); // Time
              Testo2 = StringReplace(dbCB2[i]->Text,".", ":",TReplaceFlags() << rfReplaceAll); // Time
           }
        }
        //--------------------------------------------------------------------------------------------------
        if(Testo1 !=""){
           if(primo==false){ primo=true; StringaSQL += " WHERE "; }
           else{                         StringaSQL += " AND ";   }
           // Se è visibile il 2o combo allora è una data
           if(Testo2 !=""){
              if(dbCB2[i]->Visible==false || ( Testo2==Testo1 && (myFieldType != ftTime && myFieldType != ftDateTime))){
                 StringaSQL += dbLabel[i]->Caption + "=" + Separator + Testo1 + Separator;
              }
              else{
                 switch(myFieldType){
                    case  ftTime:
                    case  ftDateTime: StringaSQL += dbLabel[i]->Caption   + " BETWEEN '" + Testo1 + ".000' AND '" + Testo2 + ".999'";    break;
                    case  ftString:   StringaSQL += dbLabel[i]->Caption   + " BETWEEN '" + Testo1 + "' AND '" + Testo2 + "'";            break;
                    default:          StringaSQL += dbLabel[i]->Caption   + " BETWEEN "  + Testo1 + "  AND "  + Testo2;
                 }
              }
           }
        }
     }
     //---------------------------------------------------------------------------
     if(err==0){
        lblSQL->Caption=StringaSQL;
        ADOQuery->Close();
        ADOQuery->SQL->Clear();
        ADOQuery->SQL->Add(StringaSQL);
        //ADOQuery->ExecuteOptions<<eoAsyncFetch;
        ADOQuery->Prepared = true;
        Gauge->Progress=0;
        ADOQuery->Open();
        //------------------------------
        Caption=SQL_Table + " Record Count = " + IntToStr(ADOQuery->RecordCount);
        //------------------------------
        NumCombo=DataSource->DataSet->FieldCount;
     }
  }
  //------------------------------
  catch(Exception &exception){
     errore_comunicazione[COM3][DB_EXTERN]=ERR_SQL_RD_TAB;
     Application->ShowException(&exception);
         //---------------------------------------------------------------------------------
     char messaggio[901]="";
     char FileLogExceptionSQL[101]="";
     sprintf(FileLogExceptionSQL, "%sSQL\\LogExceptionSQL.txt", PathProject.c_str());
     sprintf(messaggio,"DBGridGen::btnFilterClick;%s;%s", all_mess, exception.Message.c_str() );
     messaggio[498]=NULL;    // per garantire il corretto scroll
     file_cronologico_generico(FileLogExceptionSQL, messaggio, 50000L);
     strcpy(messaggio, StringaSQL.c_str());
     messaggio[498]=NULL;    // per garantire il corretto scroll
     file_cronologico_generico(FileLogExceptionSQL, messaggio, 50000L);
     lbl_Message->Caption=exception.Message;
     //---------------------------------------------------------------------------------
  }
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  DBGrid->Visible      = true;
  pnlCombo->Visible    = true;
  pnlBottoni->Visible  = true;
  ADOQuery->EnableControls();
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  TimeDiff = (time(NULL) - TimeAttuale);
  char buffer[51]="";
  sprintf( buffer, "Tempo trascorso %d (sec)", TimeDiff);
  lbl_Message->Caption=buffer;
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Shift.Contains(ssCtrl) && Key==VK_DELETE ) Key=0;

  if(Gauge->Visible==true && Key == VK_ESCAPE){
     KeyStop=true;
     return;
  }
  if (Key == VK_ESCAPE){
     myClose();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::BtnExitClick(TObject *Sender)
{
  myClose();
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::btnExcelClick(TObject *Sender)
{
  char all_mess[501]="";
  char buffer[101]="";
  char appoggio[101]="";
  int  i,j,tot;
  int  col=0;
  int  NumCol=0;
  int  err=0;
  bool OkSave=false;
  bool OK=false;
  bool exit=false;
  AnsiString NomeFileDlg="";
  AnsiString NomeFile="";
  AnsiString txt="";
  AnsiString myText;
  //---------------------------------------------------------
  // SAVE Dialog
  //---------------------------------------------------------
  SaveDialog->Options << ofOverwritePrompt; // Chiede se sovrascrivere
  SaveDialog->Options << ofNoChangeDir;     // non cambia la directory di lavoro

  SaveDialog->DefaultExt="xls";
  SaveDialog->Filter="Excel files (*.xls)|*.xls";
  SaveDialog->InitialDir=GetCurrentDir()+"\\Export"; // Directory di default
   //----------------------------------------------
  SaveDialog->FileName="DBase Report";
  OkSave = SaveDialog->Execute();

  if(!OkSave) return;
  NomeFileDlg=SaveDialog->FileName;
  //--------------------------------------------------
  // Leggo un modello di file excel con le colorazioni
  // e i bordi delle caselle adatti
  //--------------------------------------------------
  XLSReport->Filename="DBxBase.xls";
  try{
     XLSReport->Read();
  }
  catch(...){
     sprintf(all_mess, "Impossible to open DBxBase.xls");
     Application->MessageBox(all_mess, "ERROR", MB_OK | MB_ICONEXCLAMATION);
     err=1;
     return;
  }
  //---------------------------------------------------------
  NomeFile="tmp.xls";
  XLSReport->Filename=NomeFile;

  // TITOLO SHEET
  sprintf(appoggio,"Report Pag.%02d",col);
  XLSReport->Sheets->Items[col]->Name=appoggio;
  NumCol=DataSource->DataSet->FieldCount;

  // TITOLI COLONNE
  for(i=0; i<NumCol; i++){
     XLSReport->Sheets->Items[col]->AsString[i][0] = DataSource->DataSet->Fields->Fields[i]->FieldName;
  }
  // Valore dei campi;
  j=1;
  DataSource->DataSet->First();
  ADOQuery->DisableControls();
  Gauge->MaxValue      = ADOQuery->RecordCount;
  Gauge->ForeColor     = (TColor) 0x007280FA;
  Gauge->Progress      = 0;
  Gauge->Visible       = true;
  DBGrid->Visible      = false;
  pnlBottoni->Visible  = false;
  pnlCombo->Visible    = false;
  exit=false;

  try{
     do{
        Gauge->Progress++;
        if((Gauge->Progress%RilascioApp)==0){
        	Application->ProcessMessages();
        }
        for(i=0; i<NumCol; i++){
           // CAMPI
           XLSReport->Sheets->Items[col]->AsString[i][j] = DataSource->DataSet->Fields->Fields[i]->Text.c_str();
        }
        j++;
        if(j>65000){
           j=1;
           col++;
           if(col>=9){
              Application->MessageBox("Maximum Sheet Number Reached", "Error", MB_OK + MB_ICONSTOP + MB_TOPMOST);
              exit=true;
              break;
           }
           // TITOLO SHEET
           sprintf(appoggio,"Report Pag.%02d",col);
           XLSReport->Sheets->Items[col]->Name = appoggio;
           // RISCRIVE TITOLI COLONNE NELL'ALTRA PAGINA
           for(i=0; i<NumCol; i++){
              XLSReport->Sheets->Items[col]->AsString[i][0] = DataSource->DataSet->Fields->Fields[i]->FieldName;
           }
        }
        if(exit==true) break;
        DataSource->DataSet->Next();
     }while(!DataSource->DataSet->Eof);

     DataSource->DataSet->First();

     ADOQuery->EnableControls();
     Gauge->Visible = false;
     DBGrid->Visible      = true;
     pnlBottoni->Visible  = true;
     pnlCombo->Visible    = true;
     //---------------------------------------------------------
     XLSReport->Write();
  }
  catch(...){
     sprintf(all_mess, "Impossible to save file %s", NomeFile);
     Application->MessageBox(all_mess, "ERROR", MB_OK | MB_ICONEXCLAMATION);
     err=2;
  }
  //---------------------------------------------------------
  // Esportazione avvenuta
  //---------------------------------------------------------
  OK=false;
  if(err==0){
     OK=CopyFile(NomeFile.c_str(),NomeFileDlg.c_str(),false);
     if(OK) OK=DeleteFile(NomeFile);
  }

  if(OK){
     sprintf(buffer," Export OK \n File: %s",NomeFileDlg);
     Application->MessageBox(buffer, "Excel Export Result", MB_OK + MB_ICONINFORMATION + MB_TOPMOST);
  }
  else{
     sprintf(buffer," Export Error \n File: %s",NomeFileDlg);
     Application->MessageBox(buffer, "Error", MB_OK + MB_ICONSTOP + MB_TOPMOST);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::btnMaxRecordClick(TObject *Sender)
{
  myMaxRecord=InputBox("TF", "Max Records (x1000)", myMaxRecord);
  ADOQuery->Close();
  ADOQuery->MaxRecords=StrToIntDef(myMaxRecord,100)*1000;
  btnClearClick(btnClear);
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::btnStampaClick(TObject *Sender)
{
  bool OkStampa;

  Printer()->Orientation = poLandscape;
  OkStampa = PrintDialog->Execute();
  if(OkStampa){
     FormDBGridGen->Print();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::btnClearClick(TObject *Sender)
{
  FormActivate(FormDBGridGen);
  for(int i=0; i<NumCombo; i++){
     if(i>=MAX_COMBO) break;
     dbCB1[i]->Text="";
     dbCB2[i]->Text="";

     myFieldType[i] = DataSource->DataSet->Fields->Fields[i]->DataType;
     if(myFieldType[i]==ftDateTime){
        dbLabel[i]->Font->Color=(TColor)0x00FF0080;
        dbCB2[i]->Visible = true;
     }
     else{
        dbLabel[i]->Font->Color=clWindowText;
        dbCB2[i]->Visible = false;
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::btnAutoFitColumnClick(TObject *Sender)
{
  for(int i=0;i<NumCombo;i++){
     if(i>=MAX_COMBO) break;
     DBGrid->Columns->Items[i]->Width=ColSize[i]*7;
     if(DBGrid->Columns->Items[i]->Width<20) DBGrid->Columns->Items[i]->Width=20;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::DBGridDblClick(TObject *Sender)
{
  int i,cnt;
  char HU[21]="";
  AnsiString val="";

  i=DBGrid->SelectedIndex;
  val=DBGrid->Fields[i]->Text;

  if(DataSource->DataSet->Fields->Fields[i]->FieldName=="HUinterna"){
     strcpy(HU,val.c_str());
     if(HU[0]=='0' && HU[1]=='2'){
        ShowMessHU(HU);
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::popCopyClick(TObject *Sender)
{
  CopyCell();
}
//---------------------------------------------------------------------------
void TFormDBGridGen::CopyCell()
{
  AnsiString val="";
  int i;

  i=DBGrid->SelectedIndex;
  val=DBGrid->Fields[i]->Text;

  Clipboard()->Clear();
  Clipboard()->SetTextBuf(Erase_White_End(val.c_str()));
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::BitBtn1Click(TObject *Sender)
{
  GestioneComboBox();
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::SaveDialogFolderChange(TObject *Sender)
{
  AnsiString PathProject;

  PathProject = ExtractFilePath(ParamStr(0));
  SetCurrentDir(PathProject);  
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::DBGridCellClick(TColumn *Column)
{
  CopyCell();
}
//---------------------------------------------------------------------------
void TFormDBGridGen::ShowMessHU(char* HU)
{
  //-------------------------------
  // PER IVC
  //-------------------------------
  #ifdef IVC_MEXICO
     int myCount;
     char buffer[101]="";
     AnsiString Testo[21];
     AnsiString TitoloMSG;
     AnsiString TestoMSG;
     AnsiString StringaSQL="";
     AnsiString NoLock="";
     TADOQuery  *ADOQueryTab;
     //---------------------------------------
     try{
        ADOQueryTab = new TADOQuery(FormSQLDB);
        ADOQueryTab->Connection = FormMain->ADOConnSQL;
        ADOQueryTab->CommandTimeout = SecComandoTimeOutSQL;
        //---------------------------------------
        // Azzero errore di comunicazione prima
        // di procedere con una nuova
        //---------------------------------------
        // FormSQLDB->ComandoSQL_ON=true;
        errore_comunicazione[COM3][DB_EXTERN]=0;
        //---------------------------------------
        if(NoLockQuery==true) NoLock = " WITH (nolock) ";
        StringaSQL ="";
        if(StringConnADO.IsEmpty()==true) StringaSQL = "set dateformat dmy; ";  // mySQL non accetta il dateformat
        sprintf(buffer, "SELECT * FROM %s %s WHERE HUinterna='%s'", NomeTabB, NoLock.c_str(),HU);
        StringaSQL += buffer;
        //---------------------------------------
        // Attivo la query
        //---------------------------------------
        ADOQueryTab->SQL->Clear();
        ADOQueryTab->SQL->Add(StringaSQL);
        ADOQueryTab->Open();
        //------------------------------------------------------------
        myCount = ADOQueryTab->RecordCount;
        if(myCount<=0){
           Application->MessageBox("not found", "Warning", MB_OK + MB_ICONWARNING +MB_TOPMOST);
           return;
        }
        //------------------------------------------------------------
        // recupera i campi
        //------------------------------------------------------------
        Testo[ 0]= "HUinterna\t "        + ADOQueryTab->FieldByName("HUinterna"           )->AsString + "\n";
        Testo[ 1]= "HUexterna\t "        + ADOQueryTab->FieldByName("HUexterna"           )->AsString + "\n";
        Testo[ 2]= "Lote\t\t "           + ADOQueryTab->FieldByName("Lote"                )->AsString + "\n";
        Testo[ 3]= "Codigo Producto\t "  + ADOQueryTab->FieldByName("CodigoProducto"      )->AsString + "\n";
        Testo[ 4]= "TipoEmpaque\t "      + ADOQueryTab->FieldByName("TipoEmpaque"         )->AsString + "\n";
        Testo[ 5]= "ProductionOrderID\t "+ ADOQueryTab->FieldByName("ProductionOrderID"   )->AsString + "\n";
        Testo[ 6]= "CodigoTarima\t "     + ADOQueryTab->FieldByName("CodigoTarima"        )->AsString + "\n";
        Testo[ 7]= "SAPStatus\t "        + ADOQueryTab->FieldByName("SAPStatus"           )->AsString + "\n";
        Testo[ 8]= "Ubication\t\t "      + ADOQueryTab->FieldByName("Ubication"           )->AsString + "\n";
        Testo[ 9]= "Altura\t\t "         + ADOQueryTab->FieldByName("Altura"              )->AsString + "\n";
        Testo[10]= "Linea\t\t "          + ADOQueryTab->FieldByName("Unknown"             )->AsString + "\n";
        Testo[11]= "Status\t\t "         + ADOQueryTab->FieldByName("Status"              )->AsString + "\n";
        Testo[12]= "Fecha\t\t "          + ADOQueryTab->FieldByName("Fecha"               )->AsString + "\n";
        Testo[13]= "Descripcion\t "      + ADOQueryTab->FieldByName("DescripcionProducto" )->AsString + "\n";
        //------------------------------------------------------------
        TitoloMSG=HU;
        TestoMSG="";
        for(int i=0; i<=13; i++) TestoMSG += Testo[i];
        Application->MessageBox(TestoMSG.c_str(), TitoloMSG.c_str(), MB_OK + MB_ICONINFORMATION + MB_TOPMOST);
        //------------------------------------------------------------
        ADOQueryTab->Close();   // chiusura data set
        delete ADOQueryTab;
        //------------------------------------------------------------
     }
     catch(Exception &exception){
        Application->ShowException(&exception);
        ADOQueryTab->Close();   // chiusura data set
        lbl_Message->Caption=exception.Message;
        delete ADOQueryTab;
        return;
     }
  #endif
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::rbOrarioClick(TObject *Sender)
{
  int TagIndex=0,idx=0;
  AnsiString StartTxt;
  AnsiString EndTxt;
  //----------------------------------------------------------
  TagIndex=(((TRadioButton *)Sender)->Tag);
  //----------------------------------------------------------
  if(TagIndex==0){
     StartTxt = Today().DateString() + " 05:00:00";
     EndTxt   = Today().DateString() + " 10:59:59";
  }
  //----------------------------------------------------------
  if(TagIndex==1){
     StartTxt = Today().DateString() + " 11:00:00";
     EndTxt   = Today().DateString() + " 16:59:59";
  }
  //----------------------------------------------------------
  if(TagIndex==2){
     StartTxt = Today().DateString() + " 17:00:00";
     EndTxt   = Today().DateString() + " 22:59:59";
  }
  //----------------------------------------------------------
  if(TagIndex==3){
     StartTxt = Yesterday().DateString() + " 23:00:00";
     EndTxt   = Today().DateString()     + " 04:59:59";
  }
  //----------------------------------------------------------
  if(SQL_Table=="Movements")              idx=0;
  if(SQL_Table=="HU_Report_StatusChange") idx=5;

  dbCB1[idx]->Text = StartTxt;
  dbCB2[idx]->Text = EndTxt;
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::btnSpec01Click(TObject *Sender)
{
  //-----------------------------------------------------------
  // Funzione Specializzata (magari da spostare in impianti!)
  //-----------------------------------------------------------
  int  i;
  int  NumCol;
  int  FieldTGV, FieldKM;
  int  NumTgv;
  int  KM;
  char buffer[501];
  char NomeFile[101]="DBGridKM.txt";
  struct{ int maxKM; int minKM; int ActualKM; } myTGV[MAXAGV+1];
  //---------------------------------
  if(SQL_Table!="Movements") return;
  //---------------------------------
  memset(&myTGV,0,sizeof(myTGV));

  FieldTGV=FieldKM=0;

  DataSource->DataSet->First();
  NumCol=DataSource->DataSet->FieldCount;
  //--------------------------------------
  // RICERCA COLONNE TGV E KM
  //--------------------------------------
  for(i=0; i<NumCol; i++){
     if(DataSource->DataSet->Fields->Fields[i]->FieldName=="TGV") FieldTGV = i;
     if(DataSource->DataSet->Fields->Fields[i]->FieldName== "KM") FieldKM  = i;
     if(FieldKM!=0 && FieldTGV!=0) break;
  }
  //--------------------------------------
  if(FieldKM==0 || FieldTGV==0){
     MessageBox(Handle, "TGV or KM error", "Error", MB_OK + MB_ICONSTOP + MB_TOPMOST);
     return;
  }
  //--------------------------------------
  ADOQuery->DisableControls();
  Gauge->MaxValue = DataSource->DataSet->RecordCount;
  Gauge->Visible=true;
  Gauge->Progress=0;
  try{
     do{
        Gauge->Progress++;
        if((Gauge->Progress%RilascioApp)==0){
        	Application->ProcessMessages();
        }
        // CAMPI
        NumTgv = StrToIntDef(DataSource->DataSet->Fields->Fields[FieldTGV]->Text,-1);
        KM     = StrToIntDef(DataSource->DataSet->Fields->Fields[FieldKM]->Text,0);

        if(NumTgv>=1 && NumTgv <=MAXAGV){
           myTGV[NumTgv].ActualKM = KM;
           if(myTGV[NumTgv].maxKM<KM) myTGV[NumTgv].maxKM=KM;
           if(myTGV[NumTgv].minKM>KM) myTGV[NumTgv].minKM=KM;
           if(myTGV[NumTgv].minKM<=0) myTGV[NumTgv].minKM=KM;
        }
        DataSource->DataSet->Next();
     }while(!DataSource->DataSet->Eof);
     DataSource->DataSet->First();
  }
  catch(Exception &exception){
     Application->ShowException(&exception);
     lbl_Message->Caption=exception.Message;
  }
  Gauge->Visible=false;
  ADOQuery->EnableControls();
  //---------------------------------
  unlink(NomeFile);
  for(i=1; i<=MAXAGV; i++){
     sprintf(buffer,";%02d;%d;%d;%d;%d", i, myTGV[i].minKM, myTGV[i].maxKM, (myTGV[i].maxKM-myTGV[i].minKM), myTGV[i].ActualKM);
     file_cronologico_generico(NomeFile, buffer, 10000L);
  }
  //-----------------------------------------------------
  // Settaggio variabili per visualizzazione File
  //-----------------------------------------------------
  strcpy(FormListagen->NomeFileLista,  NomeFile);
  strcpy(FormListagen->CaptionFileLista,"Somario TGV KM");
  FormListagen->CarSeparatore=';'; // Carattere separatore campi nel file
  FormListagen->ColNum=6;          // numero di colonne totali
  //------------------------------------------------------------
  // Dimensioni ed Intestazioni delle colonne
  // sarebbe possibile impostarle a -1 per fare l'autosize,
  // ma poi la funzione di stampa non formatta bene il testo!
  //------------------------------------------------------------
  memset(&FormListagen->CaptionColumn,0,sizeof(FormListagen->CaptionColumn));
  FormListagen->ColSize[ 0]=  0;   strcpy(FormListagen->CaptionColumn[ 0], "");
  FormListagen->ColSize[ 1]=150;   strcpy(FormListagen->CaptionColumn[ 1], "TGV");
  FormListagen->ColSize[ 2]=100;   strcpy(FormListagen->CaptionColumn[ 2], "Start KM");
  FormListagen->ColSize[ 3]=100;   strcpy(FormListagen->CaptionColumn[ 3], "End KM");
  FormListagen->ColSize[ 4]=100;   strcpy(FormListagen->CaptionColumn[ 4], "Dif");
  FormListagen->ColSize[ 5]=100;   strcpy(FormListagen->CaptionColumn[ 5], "Ultimo");
  //------------------------------------------------------------
  FormListagen->TipoFileLista=5;
  FormListagen->btnExportExcel->Enabled  = true;
  FormListagen->btnSaveAsCSV->Enabled    = true;
  FormListagen->btnAzzera->Enabled       = false;
  FormListagen->reverseFile              = false;

  FormListagen->ShowModal();

}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::DBGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Shift.Contains(ssCtrl) && Key==VK_DELETE ) Key=0;
}
//---------------------------------------------------------------------------
int TFormDBGridGen::ParseDateTime(char *strDateTime)
{
  int Lungh=0;
  char buffer[10]="";
  bool valid=false;
  Word AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond=0;

  Lungh=strlen(strDateTime);
  if(Lungh!=19) return 1;

  Copy(strDateTime,  1, 2,buffer);    // GG
  ADay   = (Word) StrToIntDef(buffer,-1);

  Copy(strDateTime,  4, 2, buffer);   // MM
  AMonth = (Word) StrToIntDef(buffer,-1);

  Copy(strDateTime,  7, 4, buffer);   // AAAA
  AYear = (Word) StrToIntDef(buffer,-1);

  Copy(strDateTime,  12, 2, buffer);  // hh
  AHour = (Word) StrToIntDef(buffer,-1);

  Copy(strDateTime,  15, 2, buffer);  // mm
  AMinute = (Word) StrToIntDef(buffer,-1);

  Copy(strDateTime,  18, 2, buffer);  // ss
  ASecond = (Word) StrToIntDef(buffer,-1);

  valid = IsValidDateTime(AYear,  AMonth,  ADay,  AHour,  AMinute,  ASecond,   AMilliSecond);
  if(valid==false) return 1;

  return 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::ComboBoxDefChange(TObject *Sender)
{
  int i,TagIndex=0;
  //--------------
  TagIndex = (((TSpeedButton *)Sender)->Tag);
  i=TagIndex;
  if(dbCB1[i]->Visible==true){
     if(dbCB1[i]->Text != ""){
        dbCB2[i]->Clear();
        dbCB2[i]->Items->AddStrings(myList[i]);
        dbCB2[i]->ItemIndex = dbCB1[i]->ItemIndex;
        dbCB2[i]->Visible = true;
     }
     else{
        dbCB2[i]->Clear();
        dbCB2[i]->Visible = false;
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormDBGridGen::myClose()
{
  char lpString[101]="";
  HWND hWnd;
  //----------------------
  Close();
  //---------------------------------------------------------------
  // Manda il messaggio di chiusura solo se l'handle della finestra
  // corrisponde effettivamente a quella della form
  hWnd=GetForegroundWindow();
  GetWindowText(hWnd, lpString,100);
  if(strcmp(lpString, Caption.c_str())==0){
     //PostMessage(hwnd, WM_CLOSE, 0, 0);
     SendMessageA(hWnd, WM_CLOSE, 0, 0);
  }
}
//---------------------------------------------------------------------------

