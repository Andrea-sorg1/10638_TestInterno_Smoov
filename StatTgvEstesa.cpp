//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <io.h>
#include <comctrls.hpp>
#include <vcl\Clipbrd.hpp>
#include <DateUtils.hpp>

#pragma hdrstop

#include "TecnoFer.h"
#include "StatTgvEstesa.h"
#include "StatTgv.h"
#include "Listagen.h"
#include "SQLDB.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CGAUGES"
#pragma link "XLSMini"
#pragma link "CSPIN"
#pragma resource "*.dfm"

TFormStatTgvEstesa *FormStatTgvEstesa;
//---------------------------------------------------------------------------
//-------------------------------
// Costruttore Classe
//-------------------------------
__fastcall TFormStatTgvEstesa::TFormStatTgvEstesa(TComponent* Owner) : TForm(Owner)
{
  int gg,size=0;
  TListColumn  *NewColumn;

  Giorni=3+1; // Bisogna tenere conto della 1a colonna coi testi

  FormStatTgvEstesa->Caption="ExtendedStats";

  Height = 450;
  Constraints->MaxHeight=Height;
  Constraints->MinHeight=Height;

  Width  = 600;
  Constraints->MinWidth=Width;
  Constraints->MaxWidth=1500;

  for(gg=0; gg<Giorni; gg++){
     NewColumn = ListTGV->Columns->Add();
     NewColumn->Width=80;
     NewColumn->Caption=gg;
     size += NewColumn->Width;
  }

  if(size>Constraints->MaxWidth) size=Constraints->MaxWidth;
  if(size<Constraints->MinWidth) size=Constraints->MinWidth;
  Width=size + NewColumn->Width;

  edOraStart->Text  = "00.00.00";
}
//---------------------------------------------------------------------------
//-----------------------
// Attivazione Finestra
//-----------------------
void __fastcall TFormStatTgvEstesa::FormActivate(TObject *Sender)
{
  char buffer[11]="";
  char Date[11]="";
  char Time[11]="";

  NumRighe = MAXFIELD_TIMTGV;

  sprintf(buffer,"%02d",Giorni-1);
  edGiorni->Text=buffer;
  edTgvNum->Text="01";

  OraData(Time, Date);
  edDataStart->Text = Date;
  //edOraStart->Text  = "23.59.59";
  //edOraStart->Text  = "00.00.01";
  DateTimePickerStart->Date = Today();

  ListInsertValues();
}
//---------------------------------------------------------------------------
//-------------------------------------
// Chiusura della Finestra da tastiera
//-------------------------------------
void __fastcall TFormStatTgvEstesa::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if(CGauge->Visible==true) return;
  if( Key == VK_ESCAPE ) Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::ListTGVCustomDrawItem(TCustomListView *Sender, TListItem *Item, TCustomDrawState State, bool &DefaultDraw)
{
  AlternateRowColor(Sender, Item);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::ListTGVCustomDrawSubItem(TCustomListView *Sender, TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw)
{
  AlternateRowColor(Sender, Item);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::AlternateRowColor(TCustomListView *Sender, TListItem *Item)
{
  if(Item->Index % 2){
     Sender->Canvas->Font->Color  = clBlack;
     Sender->Canvas->Brush->Color = clWhite;
  }
  if(Item->Index==0 || Item->Index==1){
     Sender->Canvas->Font->Color  = clBlack;
     Sender->Canvas->Brush->Color = (TColor)0x00B3E0C5;  // Verde x Batteria e KM
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::ListInsertValues()
{
  int gg;
  int ColNum;
  int EscStatus;
  int idx;
  AnsiString Txt="";

  ColNum=ListTGV->Columns->Count;

  ListTGV->Clear();
  ListTGV->Column[0]->Caption = "TGV " + edTgvNum->Text;
  //----------------
  // TITOLI COLONNE
  //----------------
  for(gg=1;gg<ColNum;gg++){
     if(gg<Giorni) ListTGV->Column[gg]->Caption = "Loading...";
     else{
        ListTGV->Column[gg]->Caption = "";
        ListTGV->Column[gg]->Width=0;
     }
  }
  //----------------
  // RIGHE
  //---------------
  CGauge->Visible=true;
  CGauge->ForeColor=(TColor) 0x00C47244; // BLU

  CompilaLista();

  ListTGV->Items->BeginUpdate();
  for(IndiceRiga=0;IndiceRiga<=NumRighe;IndiceRiga++){
     CGauge->Progress=IndiceRiga*100/(NumRighe+1);
     EscStatus=GetAsyncKeyState(VK_ESCAPE);
     if(EscStatus!=0){
        ShowMessage(""); // Per pulire il buffer della tastiera
        MessageDlg("Stopped",  mtError, TMsgDlgButtons() << mbOK, 0);
        break;
     }
     idx=IndiceRiga;
     if((idx%RilascioApp)==0 && idx>0) Application->ProcessMessages();
     EscStatus=GetAsyncKeyState(VK_ESCAPE);
     ListTGV->Items->Add();
     ListTGV->Items->Item[IndiceRiga]->Caption = DescRigheStatTGV[IndiceRiga]; // Titoli Righe
  }
  ListTGV->Items->EndUpdate();
  CGauge->Visible=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::ListTGVInsert(TObject *Sender, TListItem *Item)
{
  int  gg;
  long int Tempo;
  AnsiString myBuffer="";
  //-------------------------------
  for(gg=0; gg<Giorni; gg++){
     myBuffer=Stringa[gg][IndiceRiga];
     Item->SubItems->Add(myBuffer);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::CompilaLista()
{
  int   gg,err;
  int   riga=0;
  int   idx;
  char  all_mess[101]="";
  char  Appoggio[101]="";
  char  DateAttuale[11];
  char  TimeAttuale[11];
  char  buffer[51]="";
  long int DiffTempo;
  long int ListOre;
  long int ListMin;
  long int ListSec;
  struct TIMTGV tmpTimTgv;
  TDateTime dtDate;
  AnsiString myBuffer="";
  int MaxAgv=MAXAGV;

  //--------------------------------------
  // Se il numero dei TGV in struttura
  // è infeririore a quello nel DB uso
  // riduco il ciclo
  //--------------------------------------
  if(MaxAgv>MAXTGV_TIMTGV) MaxAgv=MAXTGV_TIMTGV;
  //---------------------------------------------------------
  memset(&Stringa,   0, sizeof(Stringa  ));
  memset(&tmpTimTgv, 0, sizeof(tmpTimTgv));
  //--- TGV Number ---
  tgv=StrToIntDef(edTgvNum->Text,0);
  if(tgv<1 || tgv>MaxAgv){
     tgv=1;
     edTgvNum->Text=IntToStr(tgv);
  }
  //-------------------------------
  // Data di partenza
  //-------------------------------
  for(gg=0; gg<Giorni; gg++){
     CGauge->ForeColor=(TColor) 0x007280FA; // Rosso
     CGauge->Progress=gg*100/(Giorni+1);
     idx=gg;
     if((idx%RilascioApp)==0 && idx>0) Application->ProcessMessages();
     //---------------------------------------------------------
     // giorni precedenti
     strcpy(DataS, edDataStart->Text.c_str());
     strcpy(OraS,  edOraStart->Text.c_str());
     //strcpy(OraS,  "23.59.59");
     timechar_to_timeint(&Tempo, &DataS[0], &OraS[0]);
     dtDate = FormSQLDB->Algo_TimeInt_To_DateTime(Tempo);
     dtDate=dtDate-gg;
     Tempo = FormSQLDB->Algo_DateTime_To_TimeInt(dtDate);
     timeint_to_timechar(&Tempo, &DataS[0], &OraS[0]);
     //---------------------------------------------------------
     // Recupero dei dati dal DB4 in base alla Data di ricerca
     //---------------------------------------------------------
     err=TimTgvCercaRecord(&tmpTimTgv, "IDX", DataS, 0, false, all_mess);
     //---------------------------------------------------------
     if(err==1){
        memset(&tmpTimTgv, 0, sizeof(tmpTimTgv));
        strcat(all_mess, " - STATTGV");
        P.error_trasmit(0, all_mess);
     }
     if(err==2){
        sprintf(buffer,"Record Not Found \n %s", DataS);
        //Application->MessageBox(buffer, "Error", MB_OK + MB_ICONSTOP + MB_DEFBUTTON2 + MB_TOPMOST);
        memset(&tmpTimTgv, 0, sizeof(tmpTimTgv));
        //Close();
     }

     sprintf(DataRegistrazDB,"%s",DataS);
     //------------------------------------------------------------------------
     // Se la data trovata non coincide con quella cercata azzera tutto
     // se si vuole invece visualizzare il dati della data più vicina a quella
     // cercata allora commentare le righe qua sotto
     //------------------------------------------------------------------------
     if(strcmp(DataS,tmpTimTgv.DataAttuale)!=0){
        memset(&tmpTimTgv,0,sizeof(tmpTimTgv));
        sprintf(buffer,"(NotFound) %s",DataRegistrazDB);
        strcpy(DataRegistrazDB,buffer);

     }
     ListTGV->Column[gg+1]->Caption=DataRegistrazDB;
     //---------------------------------------
     // CICLO RECUPERO CAMPI
     //---------------------------------------
     for(riga=0;riga<=NumRighe;riga++){
        if(riga==0){ //--- 00 - Percorrenza
           Km=0;
           if(tgv<=MaxAgv && tgv>0) Km = (unsigned short int)(AGV[tgv]->mission.KmPercorsi);
           sprintf(Appoggio, "Km %d", Km);
           Stringa[gg][riga] = Appoggio;
           continue;
        }
        if(riga==1){ //--- 01 - Batteria ---
           Batteria=0;
           if(tgv<=MaxAgv && tgv>0) Batteria = (short int)(AGV[tgv]->mission.VoltBatteria);
           sprintf(Appoggio, "%dV", Batteria);
           Stringa[gg][riga] = Appoggio;
           continue;
        }
        if(riga==2){ //--- 02 - [FIELD_02] DA ULTIMO AZZERAMENTO ---
           DiffTempo=(atol(tmpTimTgv.Time[tgv][1])-atol(tmpTimTgv.Time[tgv][2]));
           FormStatTgv->ScomponiTempi(DiffTempo, &ListOre, &ListMin, &ListSec);
           sprintf(Appoggio, "%02ld.%02ld.%02ld", ListOre, ListMin, ListSec);
           Stringa[gg][riga] = Appoggio;
           continue;
        }
        if(riga==9){ //--- 09 - [FIELD_09] NUMERO MISSIONI ---
           sprintf(Appoggio, "%03d", atoi(tmpTimTgv.Time[tgv][riga]));
           Stringa[gg][riga]=Appoggio;
           continue;
        }
        FormStatTgv->ScomponiTempi(atol(tmpTimTgv.Time[tgv][riga]), &ListOre, &ListMin, &ListSec);
        sprintf(Appoggio, "%02ld.%02ld.%02ld", ListOre, ListMin, ListSec);
        Stringa[gg][riga]=Appoggio;
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::btnCalendarOKClick(TObject *Sender)
{
  int i, gg, count, size=0;
  TListColumn  *NewColumn;

  FormStatTgvEstesa->Visible=false;

  // Cancellazione delle colonne
  count = ListTGV->Columns->Count;

  for(i=1; i<count; i++){
     delete ListTGV->Columns->Items[1];
  }

  // Giorni da visualizzare
  Giorni=StrToIntDef(edGiorni->Text,1)+1;
  if(Giorni>31){
     Giorni=31;
     edGiorni->Text=IntToStr(Giorni);
  }

  // Aggiunge le colonne
  for(gg=0; gg<Giorni; gg++){
     NewColumn = ListTGV->Columns->Add();
     NewColumn->Width=85;
     NewColumn->Caption=gg;
  }

  // imposta dimensioni delle colonne
  count = ListTGV->Columns->Count-1;
  for(i=0; i<count; i++){
     size += ListTGV->Column[i]->Width;
  }
  // imposta dimensioni della form
  if(size>Constraints->MaxWidth) size=Constraints->MaxWidth;
  if(size<Constraints->MinWidth) size=Constraints->MinWidth;
  Width=size + NewColumn->Width;
  FormStatTgvEstesa->Visible=true;

  // Richiama funzione di inserimento valori
  ListInsertValues();
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::btnExportExcelClick(TObject *Sender)
{
  char all_mess[501]="";
  char myBuffer[101]="";
  char appoggio[101]="";
  int  i,j,tot;
  int  err=0;
  int  ColNum=0;
  bool OkSave=false;
  bool OK=false;
  AnsiString NomeFileDlg="";
  AnsiString NomeFile="";
  AnsiString txt="";
  //---------------------------------------------------------
  // SAVE Dialog
  //---------------------------------------------------------
  SaveDialog->Options << ofOverwritePrompt; // Chiede se sovrascrivere
  SaveDialog->Options << ofNoChangeDir;     // non cambia la directory di lavoro

  SaveDialog->DefaultExt="xls";
  SaveDialog->Filter="Excel files (*.xls)|*.xls";
  SaveDialog->InitialDir=GetCurrentDir()+"\\Export"; // Directory di default
  //----------------------------------------------
  // elimina i caratteri non stampabili
  //----------------------------------------------
  strcpy(appoggio,FormStatTgvEstesa->Caption.c_str());
  for(i=0,j=0;i<sizeof(appoggio);i++){
     if(appoggio[i]=='\0') break;
     if((appoggio[i]>='0' && appoggio[i]<='9') || (appoggio[i]>='A' && appoggio[i]<='z') || appoggio[i]==' '){
        myBuffer[j]=appoggio[i];
        j++;
     }
     else{
        myBuffer[j]='_'; // Sostituisce i caratteri non stampabili con '_'
        j++;
     }
  }
  //----------------------------------------------
  SaveDialog->FileName=myBuffer;
  OkSave = SaveDialog->Execute();

  if(!OkSave) return;
  NomeFileDlg=SaveDialog->FileName;

  //--------------------------------------------------
  // Leggo un modello di file excel con le colorazioni
  // e i bordi delle caselle adatti
  //--------------------------------------------------
  XLSReport->Filename="xBase.xls";
  try{
     XLSReport->Read();
  }
  catch(...){
     sprintf(all_mess, "Impossible to open xBase.xls");
     Application->MessageBox(all_mess, "ERROR", MB_OK | MB_ICONEXCLAMATION);
     err=1;
  }
  //---------------------------------------------------------
  NomeFile="tmp.xls";
  XLSReport->Filename=NomeFile;

  // TITOLO SHEET
  strcpy(appoggio,FormStatTgvEstesa->Caption.c_str());
  if(strlen(appoggio) >= 30 || strlen(appoggio) <= 0) strcpy(appoggio,"Report");
  XLSReport->Sheets->Items[0]->Name=appoggio;

  // TITOLI COLONNE
  ColNum=ListTGV->Columns->Count-1;
  for(i=0;i<ColNum;i++){
     txt=ListTGV->Column[i]->Caption;
     if(txt=="") txt="Not Assigned";
     XLSReport->Sheets->Items[0]->AsString[i][0]=txt;
  }
  // Valore dei campi;
  for(i=0; i<ListTGV->Items->Count; i++){
     for( j=0; j<ColNum; j++){
        if(j==0){
           txt=ListTGV->Items->Item[i]->Caption;
           XLSReport->Sheets->Items[0]->AsString[j][i+1]=txt;
        }
        else{
           txt=ListTGV->Items->Item[i]->SubItems->Strings[j-1];
           XLSReport->Sheets->Items[0]->AsString[j][i+1]=txt;
        }
     }
  }

  sprintf(appoggio,"TGV Num.%d",tgv);
  XLSReport->Sheets->Items[0]->AsString[0][i+2] = appoggio;

  //---------------------------------------------------------
  try{
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
     sprintf(myBuffer," Export OK \n File: %s",NomeFileDlg);
     MessageDlg(myBuffer, mtCustom, TMsgDlgButtons() << mbOK, 0);
  }
  else{
     sprintf(myBuffer," Export Error \n File: %s",NomeFileDlg);
     MessageDlg(myBuffer, mtError, TMsgDlgButtons() << mbOK, 0);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::SaveDialogFolderChange(TObject *Sender)
{
  AnsiString PathProject;

  PathProject = ExtractFilePath(ParamStr(0));
  SetCurrentDir(PathProject);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::edTgvNumChange(TObject *Sender)
{
  int val=0;
  char buffer[11]="";
  int MaxAgv=MAXAGV;

  //--------------------------------------
  // Se il numero dei TGV in struttura
  // è infeririore a quello nel DB uso
  // riduco il ciclo
  //--------------------------------------
  if(MaxAgv>MAXTGV_TIMTGV) MaxAgv=MAXTGV_TIMTGV;

  val=StrToIntDef(edTgvNum->Text,0);
  if(val<1     ) val=MaxAgv;
  if(val>MaxAgv) val=1;

  sprintf(buffer,"%02d",val);
  edTgvNum->Text=buffer;
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::UpDownGiorniClick(TObject *Sender, TUDBtnType Button)
{
  int val=0;
  TEdit *EditAppoggio;

  EditAppoggio=edGiorni;

  val=StrToIntDef(EditAppoggio->Text,-1);
  if(val< 1) val=31;
  if(val>31) val= 1;
  EditAppoggio->Text=IntToStr(val);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::edGiorniKeyPress(TObject *Sender, char &Key)
{
  myEditMask(edGiorni, Key, 2);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::edTgvNumKeyPress(TObject *Sender, char &Key)
{
  myEditMask(edTgvNum, Key, 2);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::edGiorniKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
  int val;

  val=StrToIntDef(edGiorni->Text,-1);
  if(val<1 ) val= 1;
  if(val>31) val=31;
  edGiorni->Text=IntToStr(val);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::edTgvNumKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
  int val;
  int MaxAgv=MAXAGV;

  //--------------------------------------
  // Se il numero dei TGV in struttura
  // è infeririore a quello nel DB uso
  // riduco il ciclo
  //--------------------------------------
  if(MaxAgv>MAXTGV_TIMTGV) MaxAgv=MAXTGV_TIMTGV;

  val=StrToIntDef(edTgvNum->Text,-1);
  if(val<1      ) val= 1;
  if(val>MaxAgv ) val= MaxAgv;
  edTgvNum->Text=IntToStr(val);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::UpDownTGVClick(TObject *Sender, TUDBtnType Button)
{
  int val=1;
  TEdit *EditAppoggio;
  int MaxAgv=MAXAGV;

  //--------------------------------------
  // Se il numero dei TGV in struttura
  // è infeririore a quello nel DB uso
  // riduco il ciclo
  //--------------------------------------
  if(MaxAgv>MAXTGV_TIMTGV) MaxAgv=MAXTGV_TIMTGV;

  EditAppoggio=edTgvNum;

  val=StrToIntDef(EditAppoggio->Text,-1);
  if(val< 1     ) val=MaxAgv;
  if(val>MaxAgv ) val= 1;
  EditAppoggio->Text=IntToStr(val);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::myEditMask(TEdit *Edit, char &Key, int Digit)
{
  int val1,val2;
  AnsiString oldString="";

  if(Key == Char(VK_BACK  ) ||
     Key == Char(VK_DELETE) ||
     (Key <= 0x1F && Key >0x00) ) return;

  if( (Key >= '0') && (Key <= '9') ){
     oldString = Edit->Text;
     val1 = oldString.Length();
     val2 = Edit->SelLength;
     if(val1==val2){
        Edit->Text="";
        return;
     }
     if(val1>=Digit) Key=0;
     return;
  }
  Key=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::popSelAllClick(TObject *Sender)
{
  ListTGV->SelectAll();
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::popCopyClick(TObject *Sender)
{
  FormListagen->CopyClipboard(ListTGV, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::ListTGVKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if(Shift.Contains(ssCtrl)){
     // Select All nella listview con Control+A
     if(Key=='A' || Key=='a') ListTGV->SelectAll();
     // Copia nella clipboard i campi selezionati nella listview con Control+C
     if(Key=='C' || Key=='c') FormListagen->CopyClipboard(ListTGV, NULL);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::BtnStampaClick(TObject *Sender)
{
  bool OkStampa;

  OkStampa = PrintDialog->Execute();
  if(OkStampa) {
     FormStatTgvEstesa->Print();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgvEstesa::DateTimePickerStartChange(TObject *Sender)
{
  char Data[11]="";
  char Ora[11]="";
  long int tempo=0;

  tempo=FormSQLDB->Algo_DateTime_To_TimeInt(DateTimePickerStart->Date);
  timeint_to_timechar(&tempo, &Data[0], &Ora[0]);
  edDataStart->Text = Data;
}
//---------------------------------------------------------------------------

