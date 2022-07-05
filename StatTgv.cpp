//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <io.h>
#include <comctrls.hpp>
#include <vcl\Clipbrd.hpp>
#include <DateUtils.hpp>

#pragma hdrstop

#include "TecnoFer.h"
#include "Main.h"
#include "Remoto.h"
#include "VisFile.h"
#include "Listagen.h"
#include "Input.h"
#include "StatTgv.h"
#include "StatTgvEstesa.h"
#include "SQLDB.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "XLSMini"
#pragma resource "*.dfm"
TFormStatTgv *FormStatTgv;
//---------------------------------------------------------------------------
//-------------------------------
// Costruttore Classe
//-------------------------------
__fastcall TFormStatTgv::TFormStatTgv(TComponent* Owner) : TForm(Owner)
{
  int i,size=0;
  TListColumn  *NewColumn;
  int MaxAgv=MAXAGV;

  //--------------------------------------
  // Se il numero dei TGV in struttura
  // è infeririore a quello nel DB uso
  // riduco il ciclo
  //--------------------------------------
  if(MaxAgv>MAXTGV_TIMTGV) MaxAgv=MAXTGV_TIMTGV;

  Height = 450;
  Constraints->MaxHeight=Height;
  Constraints->MinHeight=Height;

  Width  = 600;
  Constraints->MinWidth=Width;
  Constraints->MaxWidth=1500;

  for(i=0; i<MaxAgv; i++){
     NewColumn = ListTGV->Columns->Add();
     NewColumn->Width=60;
     size += NewColumn->Width;
  }

  if(size>Constraints->MaxWidth) size=Constraints->MaxWidth;
  if(size<Constraints->MinWidth) size=Constraints->MinWidth;
  Width=size+NewColumn->Width;

  edOraStart->Text  = "00.00.00";
}
//---------------------------------------------------------------------------
//-----------------------
// Attivazione Finestra
//-----------------------
void __fastcall TFormStatTgv::FormActivate(TObject *Sender)
{
  int  err;
  char Date[11]="";
  char Time[11]="";
  char all_mess[101]="";
  long int TimAttuale;
  //--------------------------------------
  // Inizializzazione variabili
  //--------------------------------------
  Km=0;
  DataNONattuale=false;
  OraData(TimeAttuale, DateAttuale);
  strcpy(DataOggi,DateAttuale);
  memset(Ore, 0, sizeof(Ore));
  memset(Min, 0, sizeof(Min));
  memset(Sec, 0, sizeof(Sec));
  memset(&EditTimTgv, 0, sizeof(TIMTGV));
  NumRighe = MAXFIELD_TIMTGV;

  OraData(Time, Date);
  edDataStart->Text = Date;
  //edOraStart->Text  = "23.59.59";
  //edOraStart->Text  = "00.00.01";
  DateTimePickerStart->Date = Today();
  //--------------------------------------
  // Gruppi di oggetti
  //--------------------------------------
  Gruppi();

  //--------------------------------------
  // Messaggeria
  //--------------------------------------
  Messaggi();

  //---------------------------------------------------------
  // Recupero dei dati dal DB4 in base alla Data di ricerca
  //---------------------------------------------------------
  if(strcmp(DataStatTgv,"")==0){
     strcpy(DataStatTgv,Date);

  }
  err=TimTgvCercaRecord(&EditTimTgv, "IDX", DataStatTgv, 0, false, all_mess);

  if(err==1){
     memset(&EditTimTgv, 0, sizeof(TIMTGV));
     strcat(all_mess, " - STATTGV");
     P.error_trasmit(0, all_mess);
  }
  if(err==2){
     memset(&EditTimTgv, 0, sizeof(TIMTGV));
     Application->MessageBox("Record Not Found", "Error", MB_OK + MB_ICONSTOP + MB_DEFBUTTON2 + MB_TOPMOST);
     Close();
  }
  //--------------------------------------
  // Compilazione delle variabili grafiche
  //--------------------------------------
  CompilaStrutture();
  //--------------------------------------
  // Rinfresco Pagina
  //--------------------------------------
  SetCaselle();
  //--------------------------------------
  //btnCalendarOK->Enabled=false;
  ListInsertValues();

  //--------------------------------------
  // Set Focus su pulsante di Exit
  //--------------------------------------
  BtnExit->SetFocus();
}
//---------------------------------------------------------------------------
//-----------------------
// Messaggeria
//-----------------------
void __fastcall TFormStatTgv::Messaggi()
{
  //------------------------------------------
  // Intestazione
  //------------------------------------------
  FormStatTgv->Caption = MESS[620];

  //------------------------------------------
  // Testi in lingua
  //------------------------------------------
  Load_MSGStatTGV();
}
//---------------------------------------------------------------------------
//-----------------------
// Array di oggetti
//-----------------------
void __fastcall TFormStatTgv::Gruppi()
{

}
//---------------------------------------------------------------------------
//-----------------------
// CompilaStrutture()
//-----------------------
void __fastcall TFormStatTgv::CompilaStrutture()
{

}
//---------------------------------------------------------------------------
// ---------------------------
//    ScomponiTempi
// ---------------------------
// Funzione che traduce secondi in hh,mm,ss
//
void __fastcall TFormStatTgv::ScomponiTempi( long int sec, long int *hh, long int *mm, long int *ss  )
//*********************************************************************************************
{
    float aa,bb,cc;
    long int a,b,c;

    aa = (float)(sec);
    a  = (long int)(aa/3600);            // ore
    bb = (aa-(a*3600));
    b  = (long int)(bb/60);              // minuti
    cc = (aa -(a*3600) - (b*60));
    c  = (long int)(cc);                 // secondi

    *hh =a;
    *mm =b;
    *ss =c;
}
//---------------------------------------------------------------------------
//-----------------------
// SetCaselle
//-----------------------
void __fastcall TFormStatTgv::SetCaselle()
{

}
//---------------------------------------------------------------------------
//-------------------------------------
// Chiusura della Finestra da tastiera
//-------------------------------------
void __fastcall TFormStatTgv::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Key == VK_ESCAPE ) Close();
}
//---------------------------------------------------------------------------
//------------------------------------
// Azzeramento dati
//------------------------------------
//------------------------------------
// Stampa dati
//------------------------------------
void __fastcall TFormStatTgv::BtnStampaClick(TObject *Sender)
{
  bool OkStampa;

  OkStampa = PrintDialog->Execute();
  if(OkStampa) {
     FormStatTgv->Print();
  }
}
//---------------------------------------------------------------------------
//------------------------------------
// Storico NUM MISSIONI TGVs
//------------------------------------
void __fastcall TFormStatTgv::btnListaStatClick(TObject *Sender)
{
  int indice=1;
  char Stringa[10][31];
  char buffer [501]="";
  char NomeFile[51]="LogTGV_Statistic.txt";
  long  int hh;
  long  int mm;
  long  int ss;
  short int NumMiss,km;
  FILE *ptrfile;
  int MaxAgv=MAXAGV;

  //--------------------------------------
  // Se il numero dei TGV in struttura
  // è infeririore a quello nel DB uso
  // riduco il ciclo
  //--------------------------------------
  if(MaxAgv>MAXTGV_TIMTGV) MaxAgv=MAXTGV_TIMTGV;

  unlink(NomeFile);
  memset(Stringa,0,sizeof(Stringa));

  ptrfile = fopen( NomeFile, "wb");
  if (ptrfile==0){
     Application->MessageBox("Cannot Create File", "Error",MB_OK + MB_ICONSTOP + MB_DEFBUTTON2 + MB_TOPMOST);
     return;
  }
  Screen->Cursor=crHourGlass;
  for(indice=MaxAgv;indice>=1;indice--){
     sprintf(Stringa[0],"%02d",indice);

     ScomponiTempi(atol(EditTimTgv.Time[indice][3]), &hh, &mm, &ss);
     sprintf(Stringa[1],"%d:%02d:%02d",hh,mm,ss);

     ScomponiTempi(atol(EditTimTgv.Time[indice][4]), &hh, &mm, &ss);
     sprintf(Stringa[2],"%d:%02d:%02d",hh,mm,ss);

     ScomponiTempi(atol(EditTimTgv.Time[indice][5]), &hh, &mm, &ss);
     sprintf(Stringa[3],"%d:%02d:%02d",hh,mm,ss);

     ScomponiTempi(atol(EditTimTgv.Time[indice][6]), &hh, &mm, &ss);
     sprintf(Stringa[4],"%d:%02d:%02d",hh,mm,ss);

     ScomponiTempi(atol(EditTimTgv.Time[indice][7]), &hh, &mm, &ss);
     sprintf(Stringa[5],"%d:%02d:%02d",hh,mm,ss);

     ScomponiTempi(atol(EditTimTgv.Time[indice][8]), &hh, &mm, &ss);
     sprintf(Stringa[6],"%d:%02d:%02d",hh,mm,ss);

     km = AGV[indice]->mission.KmPercorsi;
     //---------------------------------------------------------
     // Data attuale missioni lette direttamente da TGV
     //---------------------------------------------------------
     NumMiss = (short int)(atoi(EditTimTgv.Time[indice][9]));
     if(DataNONattuale==false) NumMiss = AGV[indice]->mission.NumMissioni;

     sprintf(buffer,"%s;%s;%s;%s;%s;%s;%s;%d;%d",
        Stringa[0], // TGV
        Stringa[1], // TimeAutomatico
        Stringa[2], // TimeSemiAutomatico
        Stringa[3], // TimeRicarica
        Stringa[4], // TimeRiserva
        Stringa[5], // TimeAllarme
        Stringa[6], // TimeAttivo
        km,         // KmPercorsi
        NumMiss);   // NumMissioni

     Write_Line(ptrfile, buffer);
  }
  fclose(ptrfile);
  Screen->Cursor=crDefault;
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
  strcpy(FormListagen->NomeFileLista, NomeFile);
  strcpy(FormListagen->NomeFileAzzera, "");
  sprintf(buffer,"Statistiche TGV del %s", DataStatTgv);
  strcpy(FormListagen->CaptionFileLista,buffer);

  FormListagen->CarSeparatore=';'; // Carattere separatore campi nel file
  FormListagen->ColNum=9;          // numero di colonne totali
  //------------------------------------------------------------
  // Dimensioni ed Intestazione delle colonne
  // sarebbe possibile impostarle a -1 per fare l'autosize,
  // ma poi la funzione di stampa non formatta bene il testo!
  //------------------------------------------------------------
  memset(&FormListagen->CaptionColumn,0,sizeof(FormListagen->CaptionColumn));
  FormListagen->ColSize[ 0]=50; strcpy(FormListagen->CaptionColumn[ 0], "TGV");
  FormListagen->ColSize[ 1]=70; strcpy(FormListagen->CaptionColumn[ 1], "Automatico");
  FormListagen->ColSize[ 2]=70; strcpy(FormListagen->CaptionColumn[ 2], "SemiAutomatico");
  FormListagen->ColSize[ 3]=70; strcpy(FormListagen->CaptionColumn[ 3], "Ricarica");
  FormListagen->ColSize[ 4]=70; strcpy(FormListagen->CaptionColumn[ 4], "Riserva");
  FormListagen->ColSize[ 5]=70; strcpy(FormListagen->CaptionColumn[ 5], "Allarme");
  FormListagen->ColSize[ 6]=70; strcpy(FormListagen->CaptionColumn[ 6], "Attivo");
  FormListagen->ColSize[ 7]=70; strcpy(FormListagen->CaptionColumn[ 7], "Km Percorsi");
  FormListagen->ColSize[ 8]=70; strcpy(FormListagen->CaptionColumn[ 8], "Missioni");
  //------------------------------------------------------------
  FormListagen->TipoFileLista=5;
  FormListagen->btnExportExcel->Enabled  =true;
  FormListagen->btnSaveAsCSV->Enabled    =true;
  FormListagen->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TFormStatTgv::BtnListTotMissClick(TObject *Sender)
{
  char NomeFile[100];
  char NomeFile1[100];
  char NomeFile2[100]="NewTotMiss.dat";
  char buffer[501]="";
  char Stringa[10][51];
  AnsiString myBuffer="";
  FILE *ptrfileSRC;
  FILE *ptrfileDST;
  int MaxAgv=MAXAGV;

  //--------------------------------------
  // Se il numero dei TGV in struttura
  // è infeririore a quello nel DB uso
  // riduco il ciclo
  //--------------------------------------
  if(MaxAgv>MAXTGV_TIMTGV) MaxAgv=MAXTGV_TIMTGV;

  memset(Stringa,0,sizeof(Stringa));
  //-----------------------------------------
  //  Recupera file da SERVER
  //-----------------------------------------
  if( IdPC>1 ){
     sprintf( NomeFile1, "%s\\%s", DriveServer,NomeFile2 );
     CopyFile(NomeFile1, NomeFile2, FALSE );
  }
  //------------------------------------------------------------
  // VISUALIZZAZIONE LISTGEN
  //------------------------------------------------------------
  strcpy(NomeFile,NomeFile2);
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
  strcpy(FormListagen->NomeFileLista, NomeFile);
  strcpy(FormListagen->NomeFileAzzera, "");
  strcpy(FormListagen->CaptionFileLista,"Cronologic Mission TGV");
  FormListagen->CarSeparatore=';'; // Carattere separatore campi nel file
  FormListagen->ColNum=MaxAgv+1;   // numero di colonne totali

  //------------------------------------------------------------
  // Dimensioni ed Intestazioni delle colonne
  // sarebbe possibile impostarle a -1 per fare l'autosize,
  // ma poi la funzione di stampa non formatta bene il testo!
  //------------------------------------------------------------
  memset(&FormListagen->CaptionColumn,0,sizeof(FormListagen->CaptionColumn));
  FormListagen->ColSize[ 0]=150;   strcpy(FormListagen->CaptionColumn[ 0], "Date and Time");
  for(int i=1;i<=MaxAgv;i++){
     //--------------------------------------------------------------------------
     // Attualmente è posssibile visualizzare massimo 15 colonne con la ListGen
     // in questo caso significa che è possibile visualizzare 14 TGV + 1 DATA
     // se si vuole aumentare il numero di TGV/Colonne occorre aumentare il
     // Define MAXCOL1 (attualmente=15) presente in Listagen.h
     //--------------------------------------------------------------------------
     if(i>MAXCOL1) break;

     sprintf(buffer,"TGV%2d",i);
     FormListagen->ColSize[i]=50;
     strcpy(FormListagen->CaptionColumn[i], buffer);
  }
  //------------------------------------------------------------
  FormListagen->TipoFileLista=5;
  FormListagen->btnExportExcel->Enabled  =true;
  FormListagen->btnSaveAsCSV->Enabled    =true;
  FormListagen->btnAzzera->Enabled       =false;
  FormListagen->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgv::ListInsertValues()
{
  int i;
  int ColNum;
  AnsiString Txt="";
  int MaxAgv=MAXAGV;

  //--------------------------------------
  // Se il numero dei TGV in struttura
  // è infeririore a quello nel DB uso
  // riduco il ciclo
  //--------------------------------------
  if(MaxAgv>MAXTGV_TIMTGV) MaxAgv=MAXTGV_TIMTGV;

  ColNum=ListTGV->Columns->Count;

  CompilaLista();

  ListTGV->Clear();
  ListTGV->Column[0]->Caption = AnsiString(DataRegistrazDB);
  //----------------
  // TITOLI COLONNE
  //----------------
  for(i=1;i<ColNum;i++){
     if(i<=MaxAgv) ListTGV->Column[i]->Caption = Txt.sprintf("TGV %02d",i);
     else{
        ListTGV->Column[i]->Caption = "*";
        ListTGV->Column[i]->Width=10;
     }
  }
  //----------------
  // RIGHE
  //---------------
  for(IndiceRiga=0;IndiceRiga<=NumRighe;IndiceRiga++){
     ListTGV->Items->Add();
     ListTGV->Items->Item[IndiceRiga]->Caption = DescRigheStatTGV[IndiceRiga];
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgv::ListTGVInsert(TObject *Sender, TListItem *Item)
{
  int agv=0;
  int MaxAgv=MAXAGV;

  //--------------------------------------
  // Se il numero dei TGV in struttura
  // è infeririore a quello nel DB uso
  // riduco il ciclo
  //--------------------------------------
  if(MaxAgv>MAXTGV_TIMTGV) MaxAgv=MAXTGV_TIMTGV;

  for(agv=1;agv<=MaxAgv;agv++){
     Item->SubItems->Add(Stringa[agv][IndiceRiga]);  // TGV n
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgv::CompilaLista()
{
  int   err,tgv=0;
  int   riga=0;
  char  all_mess[101]="";
  char  Appoggio[101]="";
  int PercAuto;
  int PercWait;
  BYTE BatteriaV;
  BYTE BatteriaP;
  long int TotAuto;
  long int TotWait;
  long int DiffTempo;
  long int ListOre;
  long int ListMin;
  long int ListSec;
  struct TIMTGV tmpTimTgv;
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
  //-------------------------------
  // Data di partenza
  //-------------------------------
  strcpy(DataS, edDataStart->Text.c_str());
  strcpy(OraS,  edOraStart->Text.c_str());
  timechar_to_timeint(&Tempo, &DataS[0], &OraS[0]);
  //---------------------------------------------------------
  // Recupero dei dati dal DB4 in base alla Data di ricerca
  //---------------------------------------------------------
  if(strcmp(DataS,"")==0) return;
  err=TimTgvCercaRecord(&tmpTimTgv, "IDX", DataS, 0, false, all_mess);
  //---------------------------------------------------------
  if(err==1){
     memset(&tmpTimTgv, 0, sizeof(tmpTimTgv));
     strcat(all_mess, " - STATTGV");
     P.error_trasmit(0, all_mess);
  }
  if(err==2){
     Application->MessageBox("Record Not Found", "Error", MB_OK + MB_ICONSTOP + MB_DEFBUTTON2 + MB_TOPMOST);
     // memset(&tmpTimTgv, 0, sizeof(tmpTimTgv));
     //Close();
  }

  Tempo=atol(tmpTimTgv.Time[1][1]);

  timeint_to_timechar(&Tempo, &DateAttuale[0], &TimeAttuale[0]);
  sprintf(DataRegistrazDB,"%s [%s %s] ",tmpTimTgv.DataAttuale, DateAttuale,TimeAttuale );

  //------------------------------------------------------------------------
  // Se la data trovata non coincide con quella cercata azzera tutto
  // se si vuole invece visualizzare il dati della data più vicina a quella
  // cercata allora commentare le righe qua sotto
  //------------------------------------------------------------------------
  /********************************************
  if(strcmp(DataS,tmpTimTgv.DataAttuale)!=0){
     memset(&tmpTimTgv,0,sizeof(tmpTimTgv));
     strcpy(DataRegistrazDB,"not found");
  }
  ********************************************/
  //---------------------------------------
  // CICLO RECUPERO CAMPI
  //---------------------------------------
  TotAuto=0;
  TotWait=0;
  PercAuto=0;
  PercWait=0;
  for(tgv=1;tgv<=MaxAgv;tgv++){
     for(riga=0;riga<=NumRighe;riga++){
        if(riga==0){ //--- 00 - Percorrenza
           Km=0;
           if(tgv<=MaxAgv && tgv>0) Km = (unsigned short int)(AGV[tgv]->mission.KmPercorsi);
           sprintf(Appoggio, "Km %d", Km);
           Stringa[tgv][riga] = Appoggio;
           continue;
        }
        if(riga==1){ //--- 01 - Batteria ---
           BatteriaV=BatteriaP=0;
           if(tgv<=MaxAgv && tgv>0){BatteriaV = AGV[tgv]->mission.VoltBatteria; BatteriaP = AGV[tgv]->mission.PercCaricaBatt;}
           sprintf(Appoggio, "%dV - %d%", BatteriaV, BatteriaP);
           Stringa[tgv][riga] = Appoggio;
           continue;
        }
        if(riga==2){ //--- 02 - [FIELD_02] DA ULTIMO AZZERAMENTO ---
           DiffTempo=(atol(tmpTimTgv.Time[tgv][1])-atol(tmpTimTgv.Time[tgv][2]));
           ScomponiTempi(DiffTempo, &ListOre, &ListMin, &ListSec);
           sprintf(Appoggio, "%02ld.%02ld.%02ld", ListOre, ListMin, ListSec);
           Stringa[tgv][riga] = Appoggio;
           continue;
        }
        if(riga==9){ //--- 09 - [FIELD_09] NUMERO MISSIONI ---
           sprintf(Appoggio, "%03d", atoi(tmpTimTgv.Time[tgv][riga]));
           Stringa[tgv][riga]=Appoggio;
           continue;
        }
        ScomponiTempi(atol(tmpTimTgv.Time[tgv][riga]), &ListOre, &ListMin, &ListSec);
        sprintf(Appoggio, "%02ld.%02ld.%02ld", ListOre, ListMin, ListSec);
        Stringa[tgv][riga]=Appoggio;
        //--------------------------------------------------
        // Totale automatico
        //--------------------------------------------------
        if(riga==3) TotAuto=TotAuto+atol(tmpTimTgv.Time[tgv][riga]);
        if(riga==4) TotWait=TotWait+atol(tmpTimTgv.Time[tgv][riga]);
     }
  }
  //-----------------------------------------------------
  // RESA IMPIANTO NAVETTE
  //-----------------------------------------------------
  if(DiffTempo>0){
     PercAuto = (int)(((TotAuto/MaxAgv)*100)/DiffTempo);
     PercWait = (int)(((TotWait/MaxAgv)*100)/DiffTempo);
  }
  sprintf(Appoggio, "Efficiency SHUTTLES: %d%c   -   SHUTTLES in Waiting: %d%c", PercAuto, '%', PercWait,'%');
  PanResa->Caption=Appoggio;
}
//---------------------------------------------------------------------------
int __fastcall TFormStatTgv::Load_MSGStatTGV()
{
  int  i=0;
  int  err;
  char buffer[101];
  char filename[101]="MSGStatTGV.";
  FILE *ptrfile;

  memset(&DescRigheStatTGV, 0, sizeof(DescRigheStatTGV));

  strcat(filename, LinguaSistema);

  ptrfile = fopen(filename, "rb");
  if (ptrfile==0) return 1;
  do{
     err = Read_Line(ptrfile, buffer, 101);
     Copy( buffer, 1, 100, &DescRigheStatTGV[i][0] );
     i++;
  }while(err && i<=NumRighe);

  fclose(ptrfile);
  return 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgv::btnCalendarOKClick(TObject *Sender)
{
  // btnCalendarOK->Enabled=false;
  ListInsertValues();
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgv::ListTGVCustomDrawItem(TCustomListView *Sender, TListItem *Item, TCustomDrawState State, bool &DefaultDraw)
{
  AlternateRowColor(Sender, Item);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgv::ListTGVCustomDrawSubItem( TCustomListView *Sender, TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw)
{
  AlternateRowColor(Sender, Item);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgv::AlternateRowColor(TCustomListView *Sender, TListItem *Item)
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
void __fastcall TFormStatTgv::popSelAllClick(TObject *Sender)
{
  ListTGV->SelectAll();
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgv::popCopyClick(TObject *Sender)
{
  FormListagen->CopyClipboard(ListTGV, NULL);
}
//------------------------------------------------------------
void __fastcall TFormStatTgv::ListTGVKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if(Shift.Contains(ssCtrl)){
     // Select All nella listview con Control+A
     if(Key=='A' || Key=='a') ListTGV->SelectAll();
     // Copia nella clipboard i campi selezionati nella listview con Control+C
     if(Key=='C' || Key=='c') FormListagen->CopyClipboard(ListTGV, NULL);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgv::btnExportExcelClick(TObject *Sender)
{
  char all_mess[501]="";
  char buffer[101]="";
  char appoggio[101]="";
  int  i,j,tot;
  int  err=0;
  int  ColNum=0;
  bool OkSave=false;
  bool OK=false;
  AnsiString NomeFileDlg="";
  AnsiString NomeFile="";
  AnsiString txt="";
  int MaxAgv=MAXAGV;

  //--------------------------------------
  // Se il numero dei TGV in struttura
  // è infeririore a quello nel DB uso
  // riduco il ciclo
  //--------------------------------------
  if(MaxAgv>MAXTGV_TIMTGV) MaxAgv=MAXTGV_TIMTGV;
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
  strcpy(appoggio,FormStatTgv->Caption.c_str());
  for(i=0,j=0;i<sizeof(appoggio);i++){
     if(appoggio[i]=='\0') break;
     if((appoggio[i]>='0' && appoggio[i]<='9') || (appoggio[i]>='A' && appoggio[i]<='z') || appoggio[i]==' '){
        buffer[j]=appoggio[i];
        j++;
     }
     else{
        buffer[j]='_'; // Sostituisce i caratteri non stampabili con '_'
        j++;
     }
  }
  //----------------------------------------------
  SaveDialog->FileName=buffer;
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
  strcpy(appoggio,FormStatTgv->Caption.c_str());
  if(strlen(appoggio) >= 30 || strlen(appoggio) <= 0) strcpy(appoggio,"Report");
  XLSReport->Sheets->Items[0]->Name=appoggio;

  // TITOLI COLONNE
  ColNum=MaxAgv+1;
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
     sprintf(buffer," Export OK \n File: %s",NomeFileDlg);
     MessageDlg(buffer, mtCustom, TMsgDlgButtons() << mbOK, 0);
  }
  else{
     sprintf(buffer," Export Error \n File: %s",NomeFileDlg);
     MessageDlg(buffer, mtError, TMsgDlgButtons() << mbOK, 0);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgv::SaveDialogFolderChange(TObject *Sender)
{
  AnsiString PathProject;

  PathProject = ExtractFilePath(ParamStr(0));
  SetCurrentDir(PathProject);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgv::btnStatTgvEstesaClick(TObject *Sender)
{
  FormStatTgvEstesa->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgv::btnDayDOWNClick(TObject *Sender)
{
  //-------------------------------
  // Giorno -1
  //-------------------------------
  TDateTime dtDate;

  strcpy(DataS, edDataStart->Text.c_str());
  strcpy(OraS,  edOraStart->Text.c_str());
  //strcpy(OraS,  "23.59.59");
  timechar_to_timeint(&Tempo, &DataS[0], &OraS[0]);
  dtDate = FormSQLDB->Algo_TimeInt_To_DateTime(Tempo);

  dtDate--;

  Tempo = FormSQLDB->Algo_DateTime_To_TimeInt(dtDate);

  timeint_to_timechar(&Tempo, &DataS[0], &OraS[0]);

  edDataStart->Text= DataS;
  edOraStart->Text = OraS;

  DateTimePickerStart->Date= FormSQLDB->Algo_TimeInt_To_DateTime(Tempo);

  ListInsertValues();
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgv::btnDayUPClick(TObject *Sender)
{
  //-------------------------------
  // Giorno +1
  //-------------------------------
  TDateTime dtDate;

  strcpy(DataS, edDataStart->Text.c_str());
  strcpy(OraS,  edOraStart->Text.c_str());
  //strcpy(OraS,  "23.59.59");
  timechar_to_timeint(&Tempo, &DataS[0], &OraS[0]);
  dtDate = FormSQLDB->Algo_TimeInt_To_DateTime(Tempo);

  if(dtDate < Today()){
     dtDate++;
  }

  Tempo = FormSQLDB->Algo_DateTime_To_TimeInt(dtDate);

  timeint_to_timechar(&Tempo, &DataS[0], &OraS[0]);

  edDataStart->Text= DataS;
  edOraStart->Text = OraS;

  DateTimePickerStart->Date= FormSQLDB->Algo_TimeInt_To_DateTime(Tempo);

  ListInsertValues();
}
//---------------------------------------------------------------------------
void __fastcall TFormStatTgv::DateTimePickerStartChange(TObject *Sender)
{
  char Data[11]="";
  char Ora[11]="";
  long int tempo=0;

  tempo=FormSQLDB->Algo_DateTime_To_TimeInt(DateTimePickerStart->Date);
  timeint_to_timechar(&tempo, &Data[0], &Ora[0]);
  edDataStart->Text = Data;
}
//---------------------------------------------------------------------------

