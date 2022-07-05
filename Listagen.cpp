//---------------------------------------------------------------------------
#include <vcl.h>
#include <io.h>
#include <time.h>
#include <vcl\Clipbrd.hpp>
#include "StrUtils.hpp"
#pragma hdrstop

#include "Main.h"
#include "Password.h"
#include "Listagen.h"
#include "TecnoFer.h"
#include "Stampa.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CGAUGES"
#pragma link "XLSMini"
#pragma resource "*.dfm"
TFormListagen *FormListagen;
//---------------------------------------------------------------------------
__fastcall TFormListagen::TFormListagen(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::FormCreate(TObject *Sender)
{
  if(Width <670) Width  = 670;
  if(Height<465) Height = 465;  // 520
  //-----------------------------------
  Constraints->MinHeight = Height;
  Constraints->MaxHeight = 1000;
  Constraints->MinWidth  = Width;
  Constraints->MaxWidth  = 1800;
  //-----------------------------------
  memset(&CaptionColumn, 0, sizeof(CaptionColumn));
  memset(&ColumnText,    0, sizeof(ColumnText));
  memset(&ColSize,       0, sizeof(ColSize));
  CarSeparatore=';';

  oldFormWidth=FormListagen->Width;
  oldListWidth=FormListagen->ListView->Width;

  FormStyle=fsNormal;
  reverseFile=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::FormActivate(TObject *Sender)
{
  int i;
  char Titolo[201];
  //------------------------------------------------------------
  FormListagen->Height=530;
  pnlFilter->Height   = 50;
  //------------------------------------------------------------
  ListView->Visible=true;
  bitON=false;
  btnFilterON->Glyph=btnLampOFF->Glyph;
  //------------------------------------------------------------
  // se il file non esiste ERRORE
  //------------------------------------------------------------
  if(!FileExists(NomeFileLista)){
     Application->MessageBox("File not exist", MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
  //------------------------------------------------------------
  sprintf(Titolo, "%s ", CaptionFileLista);
  FormListagen->Caption = Titolo;
  lblSelected->Caption="";
  //--------------------------------------------------
  // Abilitazione del Timer della Form
  //--------------------------------------------------
  TimerClose->Enabled=true;
  //--------------------------------------------------
  // --- Riposizionamento Pagina ---
  // --- inizializzazione variabili di classe ---
  Tipo = TipoFileLista;  // individuo il tipo lista da visualizzare
  strcpy(fileDAT, NomeFileLista);
  strcpy(fileTMP, "PROVA.TMP");
  StampaRichEdit->Clear();
  //--------------------------------------------------
  // Bottone di esportazione in excel/csv visibile solo
  // con tipo di visualizzazione >= 5
  //--------------------------------------------------
  FormListagen->Height = 465;
  pnlFilter->Enabled   = false;
  pnlFilter->Visible   = false;
  pnlListView->Color   = ListView->Color;
  //----------------------------------------------
  pnlFilter->Visible=false;
  if(Tipo<5){
     btnExportExcel->Enabled=false;
     btnSaveAsCSV->Enabled=false;
     btnAttivaFiltri->Enabled=false;
     CGauge->Visible=false;
//     pnlFilter->Visible=false;
  }
  else{
     btnAttivaFiltri->Enabled=true;
     FilterWindow=false;
     btnAttivaFiltri->Glyph=btnGliphDown->Glyph;
     CGauge->Visible=true;
//     pnlFilter->Visible=true;
  }
  //------------------------------------------
  // Inizializzazione colonne
  //------------------------------------------
  ListView->Columns->Clear();
  //----------------------------------------------
  if(Tipo<5){
     for (i=0;i<=6;i++){
         ListView->Columns->Add();
     }
     ListView->Column[0]->Width =  70;
     ListView->Column[1]->Width =  60;
     ListView->Column[2]->Width =  40;
     ListView->Column[3]->Width =  40;
     ListView->Column[4]->Width = 390;
     ListView->Column[5]->Width =   0;
     ListView->Column[6]->Width =   0;

     strcpy(NomeFileAzzera,"");
  }
  else{
     if(ColNum>MAXCOL1)ColNum=MAXCOL1;
     if(ColNum<1)      ColNum=1;

     ListView->Visible=false;

     // Calcola la dimensione della listview in base alla larghezza delle colonne
     ListView->Width=20; // offset iniziale
     for(i=0; i<FormListagen->ColNum; i++){
        ListView->Width+=FormListagen->ColSize[i];
     }
     if(ListView->Width<630) ListView->Width=630;
     FormListagen->Width=ListView->Width+40; // Margine della Form rispetto alla List

     pnlListView->Width=ListView->Width;
     //pnlListView->Height=ListView->Height;

     CGauge->Left  = pnlListView->Left;
     CGauge->Width = pnlListView->Width;
     CGauge->Progress=0;

     ListView->Items->BeginUpdate();
     for (i=0;i<ColNum;i++) ListView->Columns->Add();
     ListView->Items->EndUpdate();

     ListView->Visible=true;

     cbSelFilter->Text="";
     btnAND->Down=true;
  }
//----------------------------------------------
  // --- imposta i messaggi degli oggetti ---
  Messaggi();

  // --- rinfresca la lista degli Errori Trasmissione presenti ---
  Refresh1();

  sprintf(Titolo, "%s  -  Record Count=%d", CaptionFileLista, ListView->Items->Count);
  if(Tipo>=5 && ListView->Items->Count >= MAXLIN){
     sprintf(Titolo, "%s - MAX RECORD COUNT REACHED (%d)", CaptionFileLista, ListView->Items->Count);
  }
  FormListagen->Caption = Titolo;

  if(Tipo>=5 && ListView->Items->Count<1){
     btnAttivaFiltri->Enabled=false;
  }

  OneShot=true;
}
//---------------------------------------------------------------------------
// ------------------------------------------
//  Imposta i messaggi abbinati agli oggetti
// ------------------------------------------
void __fastcall TFormListagen::Messaggi()
{
  int i;

  switch( Tipo ){
     case 0:                                         // Lista legata al TGV e alla sua posizione
        ListView->Column[0]->Caption = "Date";
        ListView->Column[1]->Caption = "Time";
        ListView->Column[2]->Caption = "TGV";
        ListView->Column[3]->Caption = "Pos";
        ListView->Column[4]->Caption = "Message";
        break;
     case 1:                                         // Lista alla macchina
        ListView->Column[0]->Caption = "Date";
        ListView->Column[1]->Caption = "Time";
        ListView->Column[2]->Caption = "MAC";
        ListView->Column[3]->Caption = "Pos";
        ListView->Column[4]->Caption = "Message";
        break;
     case 2:                                         // Log operatore
        ListView->Column[0]->Caption = "Date";
        ListView->Column[1]->Caption = "Time";
        ListView->Column[2]->Caption = "PC";
        ListView->Column[3]->Caption = "Id.";
        ListView->Column[4]->Caption = "Window";
        ListView->Column[5]->Caption = "Message";

        ListView->Column[2]->Width   =  40;
        ListView->Column[3]->Width   =  30;
        ListView->Column[4]->Width   =  70;
        ListView->Column[5]->Width   = 330;
        break;
     case 3:                                         // Lista legata al TGV per CRONOLOGICO ALLARMI
        ListView->Column[0]->Caption = "Date";
        ListView->Column[1]->Caption = "Time";
        ListView->Column[2]->Caption = "TGV";
        ListView->Column[3]->Caption = "Pos";
        ListView->Column[4]->Caption = "Message";
        ListView->Column[5]->Caption = "Total";
        ListView->Column[6]->Caption = "Line";

        ListView->Column[4]->Width   = 300;
        ListView->Column[5]->Width   =  55;
        ListView->Column[6]->Width   =  35;
        break;
     case 4:
        ListView->Column[0]->Caption = "Locazione";
        ListView->Column[1]->Caption = "Posizione";
        ListView->Column[2]->Caption = "UDC";
        ListView->Column[3]->Caption = "CodiceArticolo";
        ListView->Column[4]->Caption = "LottoCQ";
        ListView->Column[5]->Caption = "Descrizione";

        ListView->Column[0]->Width   = 65;
        ListView->Column[1]->Width   = 145;
        ListView->Column[2]->Width   = 85;
        ListView->Column[3]->Width   = 85;
        ListView->Column[4]->Width   = 75;
        ListView->Column[5]->Width   = 200;
        break;
     case 5:
        cbFilter->Clear();
        for(i=0;i<ColNum;i++){

           // Compilazione titolo colonne
           ListView->Column[i]->Caption = CaptionColumn[i];

           // Compilazione lista filtro
           if(strcmp(CaptionColumn[i], "")==0){
              cbFilter->Items->Add("---");
           }
           else{
              cbFilter->Items->Add(CaptionColumn[i]);
           }
           cbFilter->ItemIndex=0;
        }
        break;
     default:
        ListView->Column[0]->Caption = "Date";
        ListView->Column[1]->Caption = "Time";
        ListView->Column[2]->Caption = "";
        ListView->Column[3]->Caption = "";
        ListView->Column[4]->Caption = "Message";
        break;
  }
  btnAzzera->Caption="Delete";
  btnStampa->Caption="Print";
}
//---------------------------------------------------------------------------
// --------------------------
//  Rinfresco Allarmi attivi
// --------------------------
void __fastcall TFormListagen::Refresh1()
{
  FILE *ptrfile;
  int err=0;
  int righe=0;
  int MaxRead_Line;
  short int i=0, j=0, k=0, w=0;
  char buffer[401]="";
  char strAppoggio[201]="";
  char all_mess[201]="";

  // --- inizializza i valori le variabili ---
  indice=0;
  memset(&Stringa0, 0, sizeof(Stringa0));
  memset(&Stringa1, 0, sizeof(Stringa1));
  memset(&Stringa2, 0, sizeof(Stringa2));
  memset(&Stringa3, 0, sizeof(Stringa3));
  memset(&Stringa4, 0, sizeof(Stringa4));
  memset(&Stringa5, 0, sizeof(Stringa5));
  memset(&Stringa6, 0, sizeof(Stringa6));

  memset(&ColumnText, 0, sizeof(ColumnText));

  ListView->Items->Clear();

  err=0;
  if(Tipo<5) MaxRead_Line = 2000;
  else       MaxRead_Line = MAXLIN;

  if(reverseFile){
     // --- Inverto la sequenza del file ---
     err = inverti_sequenza_file_cron(fileDAT, fileTMP, 0, 0, &righe);

     if(err){
        sprintf(all_mess, "Write Error File: %s...", fileDAT);
        Application->MessageBox(all_mess, MESS[100], MB_OK | MB_ICONEXCLAMATION);
        return;
     }

     ptrfile = fopen(fileTMP, "rb");
  }
  else{
     ptrfile = fopen(fileDAT, "rb");
  }
  if (ptrfile==0){
     sprintf(buffer, "Read Error File: %s...", fileDAT);
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  for(i=0; i<=MaxRead_Line; i++){
     // err = !Read_Line(ptrfile, buffer, 181);
     memset(buffer,0,sizeof(buffer));
     err = !Read_Line(ptrfile, buffer, sizeof(buffer)-1);
     if(err) break;
     switch(Tipo){
        case  2: Copy( buffer,  1, 10, Stringa0[i] );      // Log operatore
                 Copy( buffer, 12,  8, Stringa1[i] );
                 Copy( buffer, 21,  4, Stringa2[i] );
                 Copy( buffer, 26,  2, Stringa3[i] );
                 Copy( buffer, 31,  9, Stringa4[i] );
                 Copy( buffer, 43, 81, Stringa5[i] );
                 break;
        case  3: Copy( buffer,  1, 10, Stringa0[i] );
                 Copy( buffer, 12,  8, Stringa1[i] );
                 Copy( buffer, 23,  3, Stringa2[i] );
                 Copy( buffer, 29,  4, Stringa3[i] );
                 Copy( buffer, 35, 81, Stringa4[i] );
                 Copy( buffer,119,  8, Stringa5[i] );
                 Copy( buffer,128,  3, Stringa6[i] );
                 break;
        case  4: Copy( buffer, 21,  6, ColumnText[0][i] ); // Loc
                 Copy( buffer, 28, 25, ColumnText[1][i] ); // Pos
                 Copy( buffer, 54, 20, ColumnText[2][i] ); // UDC
                 Copy( buffer, 75, 15, ColumnText[3][i] ); // Articolo
                 Copy( buffer, 91, 15, ColumnText[4][i] ); // LottoCQ
                 Copy( buffer,107, 35, ColumnText[5][i] ); // Descrizione
                 break;
        case  5: // scansiona la stringa fino ad incontrare il separatore,
                 // quindi compila i campi delle colonne
                 w=0;k=0; // k=indice al carattere della stringa di appoggio, w=indice della colonna

                 memset(&strAppoggio,NULL,sizeof(strAppoggio));
                 for(j=0;j<(int)strlen(buffer)+1;j++,k++){          // Scansiona la stringa
                    if(buffer[j]==CarSeparatore || buffer[j]=='\0'){ // trovato separatore o arrivato a fine stringa
                       strAppoggio[k]='\0';       // Aggiunge il terminatore alla stringa appoggio
                       strcpy(ColumnText[w][i],strAppoggio);
                       memset(&strAppoggio,NULL,sizeof(strAppoggio));
                       k=-1;                      // Per farlo ripartire da 0
                       w++;                       // Passa alla colonna sucessiva
                       if(w>ColNum) break;        // Arrivato a ultima colonna
                       continue;
                    }
                    strAppoggio[k]=buffer[j];
                 }
                 break;
                 
        default: Copy( buffer,  1, 10, Stringa0[i] );
                 Copy( buffer, 12,  8, Stringa1[i] );
                 Copy( buffer, 23,  3, Stringa2[i] );
                 Copy( buffer, 29,  4, Stringa3[i] );
                 Copy( buffer, 35, 81, Stringa4[i] );
                 break;
     }
  }
  int NumLine=i;
  fclose(ptrfile);
  //--------------------------------------------------------------------
  // inserisce i valori nella 1° colonna, le altre vengono impostate
  // nella funzione ListViewInsert richiamata ad ogni inserimento Add()
  //--------------------------------------------------------------------
  if(Tipo<5){
     for(indice=0; indice<=2000; indice++){
        if(!strcmp(Stringa0[indice], "")) break;
        ListView->Items->Add();
        ListView->Items->Item[indice]->Caption=Stringa0[indice];
     }
  }                                      
  else{
     indice=0;
     KeyStop=false;

     ListView->Visible=false;

     ListView->Items->Clear();
     ListView->Items->BeginUpdate();
     Screen->Cursor=crHourGlass;
     CGauge->Visible=true;
     // per pulire il buffer della tastiera
     EscStatus=GetAsyncKeyState(VK_ESCAPE);
     //------------------------------------
     for(indice=0; indice<=MaxRead_Line; indice++){
        CGauge->Progress=indice*100/(NumLine+1);
        EscStatus=GetAsyncKeyState(VK_ESCAPE);
        if(KeyStop==true || EscStatus!=0 ){
           Application->MessageBox("Stopped", "Warning", MB_OK + MB_ICONWARNING + MB_TOPMOST);
           Application->MessageBox("Stopped", "Warning", MB_OK + MB_ICONWARNING + MB_TOPMOST);
           break;
        }
        if(!strcmp(ColumnText[0][indice], "")) break;

        if((indice%RilascioApp==0) && indice>0){
           Application->ProcessMessages();
        }
        ListView->Items->Add(); // 1° Colonna
        ListView->Items->Item[indice]->Caption=ColumnText[0][indice];
     }
     //---------------------------------------
     // definisce le dimensioni delle colonne
     //---------------------------------------
     for (i=0;i<ColNum;i++){
        if (ColSize[i]<=0 || ColSize[i]>500) ColSize[i]=-1;
        if(strcmp(CaptionColumn[i],"")!=0) ListView->Column[i]->Width=ColSize[i];
        else ListView->Column[i]->Width=0;
     }
     CGauge->Visible=false;
     Screen->Cursor=crDefault;
     ListView->Items->EndUpdate();

     ListView->Visible=true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::ListViewInsert(TObject *Sender, TListItem *Item)
{
  int idx;
  //---------------------------------------
  // aggiunge i valori nelle altre colonne
  //---------------------------------------
  if(Tipo<5){
     Item->SubItems->Add(Stringa1[indice]); // 2° Colonna
     Item->SubItems->Add(Stringa2[indice]); // 3° Colonna
     Item->SubItems->Add(Stringa3[indice]); // 4° Colonna
     Item->SubItems->Add(Stringa4[indice]); // 5° Colonna
     Item->SubItems->Add(Stringa5[indice]); // 6° Colonna
     Item->SubItems->Add(Stringa6[indice]); // 7° Colonna
  }
  else{
     Item->SubItems->Clear();

     ListView->Items->BeginUpdate();
     for (idx=1;idx<=ColNum;idx++){
        Item->SubItems->Add(ColumnText[idx][indice]); // n° Colonna
     }
     ListView->Items->EndUpdate();
  }
}

//---------------------------------------------------------------------------
void __fastcall TFormListagen::ListViewColumnClick(TObject *Sender, TListColumn *Column)
{
  // --- recupera il num.della colonna cliccata ---
  Colonna=Column->Index;

  // --- richiama la funzione di ordinamento, NB il criterio di comparazione ---
  // --- viene impostato nella funzione ListViewCompare richiamata sempre da ---
  // --- AlphaSort;                                                          ---
  ListView->AlphaSort();

  // --- flip-flop per criterio di ordinamento ---
  Up[Colonna]=!Up[Colonna];
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::ListViewCompare(TObject *Sender, TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
  char buffer1[51]  = "";
  char buffer2[11]  = "";
  AnsiString Fecha1 = "";
  AnsiString Fecha2 = "";

  if(Tipo<5){
     switch (Colonna){
        case 0:
           // --- criterio di comparazione per la 1° colonna ---
           Compare = Item1->Caption.AnsiCompare(Item2->Caption);
           if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
           break;
        case 1:
           // --- criterio di comparazione per la 2° colonna ---
           Compare = Item1->SubItems->Strings[0].AnsiCompare(Item2->SubItems->Strings[0]);
           if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
           break;
        case 2:
           // --- criterio di comparazione per la 3° colonna ---
           Compare = Item1->SubItems->Strings[1].AnsiCompare(Item2->SubItems->Strings[1]);
           if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
           break;
        case 3:
           // --- criterio di comparazione per la 4° colonna ---
           Compare = Item1->SubItems->Strings[2].AnsiCompare(Item2->SubItems->Strings[2]);
           if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
           break;
        case 4:
           // --- criterio di comparazione per la 5° colonna ---
           Compare = Item1->SubItems->Strings[3].AnsiCompare(Item2->SubItems->Strings[3]);
           if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
           break;
        case 5:
           // --- criterio di comparazione per la 6° colonna ---
           Compare = Item1->SubItems->Strings[4].AnsiCompare(Item2->SubItems->Strings[4]);
           if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
           break;
        case 6:
           // --- criterio di comparazione per la 7° colonna ---
           Compare = Item1->SubItems->Strings[5].AnsiCompare(Item2->SubItems->Strings[5]);
           if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
           break;

        default: break;
     }
  }
  if(Tipo>=5){
     // --- criterio di comparazione per la 1° colonna ---
     if(Colonna==0){
        strcpy(buffer1,ListView->Column[0]->Caption.c_str());
        Copy(buffer1,1,3,buffer2);
        // ORDINAMENTO COLONNA TIPO DATA-ORA
        if(strcmp(buffer2,"Dat")==0){
           strcpy(buffer1,Item1->Caption.c_str());
           Copy(buffer1,7,4,buffer2);  Fecha1 += buffer2; // anno
           Copy(buffer1,4,2,buffer2);  Fecha1 += buffer2; // mese
           Copy(buffer1,1,2,buffer2);  Fecha1 += buffer2; // giorno
           Copy(buffer1,11,8,buffer2); Fecha1 += buffer2; // ora

           strcpy(buffer1,Item2->Caption.c_str());
           Copy(buffer1,7,4,buffer2);  Fecha2 += buffer2; // anno
           Copy(buffer1,4,2,buffer2);  Fecha2 += buffer2; // mese
           Copy(buffer1,1,2,buffer2);  Fecha2 += buffer2; // giorno
           Copy(buffer1,11,8,buffer2); Fecha2 += buffer2; // ora

           Compare = Fecha1.AnsiCompare(Fecha2);
        }
        else Compare = Item1->Caption.AnsiCompare(Item2->Caption);

        if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
     }
     // --- criterio di comparazione per la n° colonna ---
     if(Colonna>0){
        Compare = Item1->SubItems->Strings[Colonna-1].AnsiCompare(Item2->SubItems->Strings[Colonna-1]);
        if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  char lpString[101]="";
  HWND hWnd;
  //--------------------------------------------
  if(CGauge->Visible==true && Key == VK_ESCAPE){
     KeyStop=true;
     return;
  }

  if (Key == VK_ESCAPE){
     Close();
     //---------------------------------------------------------------
     // Manda il messaggio di chiusura solo se l'handle della finestra
     // corrisponde effettivamente a quella della form
     hWnd=GetForegroundWindow();
     GetWindowText(hWnd, lpString,100);
     if(strcmp(lpString, Caption.c_str())==0){
        PostMessage(hWnd, WM_CLOSE, 0, 0);
     }
  }
}
//---------------------------------------------------------------------------
// --------------------------
//  Azzeramento File
// --------------------------
void __fastcall TFormListagen::btnAzzeraClick(TObject *Sender)
{
  int status1=0;
  int status2=0;
  char buffer[101]="";
  char Stringa[81]="";
  char Date[11]="";
  char Time[11]="";
  char g[3]="";
  char m[3]="";
  char a[7]="";
  bool psw_sconosciuta;
  short int i;
  long int Tempo;
  //-------------------------------------------------
  // Se livello password non ammesso re-inserisco
  //-------------------------------------------------
  if(LivelloPassword<2){
     //-------------------------------------------------
     // Apertura della form
     //-------------------------------------------------
     FormPassword->ShowModal();
     //-------------------------------------------------
     // Se è stato premuto il pulsante "OK" verifico
     // la validità della password
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
        LivelloPassword = 0;// LivelloMinPassword;
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
           if(FormPassword->Password!=PASS_CLIENTE && FormPassword->Password!=Pass2[i] && FormPassword->Password!=Stringa){
              NrOperatore     = 0;
              LivelloPassword = 0;
              psw_sconosciuta = false;
              break;
           }
           //-------------------------------------------------
           // Attivazione Password al LIVELLO 1
           //-------------------------------------------------
           if(FormPassword->Password==PASS_CLIENTE){
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
        // La password inserita non è corretta
        //-------------------------------------------------
        if(psw_sconosciuta==true){
           Application->MessageBox(MESS[105], MESS[100], MB_OK|MB_ICONEXCLAMATION );
        }
        //-------------------------------------------------
        // Attivazione blocchi
        //-------------------------------------------------
        FormMain->StatoPassword();
        //-------------------------------------------------
        // Traccia su LOG OPERATORE
        //-------------------------------------------------
        sprintf(buffer, "(PASSWORD) - Modifica Password a livello %d", LivelloPassword);
        FileLogOperatore(buffer);
     }
  }
  //----------------------------------------
  // Verifica se livello minimo di password
  // è stato raggiunto
  //----------------------------------------
  if(LivelloPassword<2){
     ShowMessage(MESS[ 45]);
     return;
  }
  //----------------------------------------
  // Cancellazione fisica del file
  //----------------------------------------
  unlink(fileDAT);
  unlink(fileTMP);
  if(strcmp(NomeFileAzzera,"")!=0){
     unlink(NomeFileAzzera);
     sprintf(buffer, "(LISTAGEN) - Azzerato File %s", NomeFileAzzera);
     FileLogOperatore(buffer);
  }
  //----------------------------------------
  // Test accesso al file appena cancellato
  //----------------------------------------
  status1 = access(fileDAT, 0);
  status2 = access(fileTMP, 0);

  if(status1==0 && status2==0){
     //sprintf(all_mess, MESS[112], fileDAT);
     sprintf(buffer, "Read Error File: %s...", fileDAT);
     Application->MessageBox(buffer, MESS[99], MB_OK | MB_ICONINFORMATION);
  }
  else{
     //sprintf(all_mess, MESS[112], fileDAT);
     sprintf(buffer, "Read Error File: %s...", fileDAT);
     Application->MessageBox(buffer, MESS[99], MB_OK | MB_ICONINFORMATION);
     //----------------------------------------
     // Chiudo la pagina
     //----------------------------------------
     Close();
  }
  return ;
}
//---------------------------------------------------------------------------
// --------------------------
//  Rinfresca Pagina
// --------------------------
void __fastcall TFormListagen::btnRinfrescaClick(TObject *Sender)
{
  char Titolo[501];
  int RecCount;

  RecCount=ListView->Items->Count;
  CGauge->Progress=0;
  Refresh1();
  sprintf(Titolo, "%s  -  Record Count=%d", CaptionFileLista, RecCount);
  FormListagen->Caption = Titolo;
}
//---------------------------------------------------------------------------
// --------------------------
//  Stampa
// --------------------------
void __fastcall TFormListagen::btnStampaClick(TObject *Sender)
{
  FILE *ptrfile1;
  FILE *ptrfile2;
  char buffer[101]="";
  char stringa1[101]="";
  char stringa2[101]="";
  short int i=0;
  int err=0;

  //--------------------------------------------------------
  // In caso di tipo>=5 utilizza l'altra modalità di stampa
  //--------------------------------------------------------
  if(Tipo>=5){
     StampaAvanzata();
     return;
  }

  //-----------------------------------------
  // Apertura del File originale in Lettura
  //-----------------------------------------
  ptrfile1 = fopen(fileDAT, "rb");
  if(ptrfile1==0){
     sprintf(buffer, MESS[98], fileDAT);
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  //-----------------------------------------
  // Apertura del File appoggio in Scrittura
  //-----------------------------------------
  ptrfile2 = fopen("STAMPA.TMP", "wb");
  if(ptrfile2==0){
     sprintf(buffer, MESS[98], fileDAT);
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }

  //-------------------------------------------------
  // Scrittura dell'intestazione nel File di Stampa.
  //-------------------------------------------------
  strcpy(stringa2, "--------------------------------------------------------------------------------");
  Write_Line(ptrfile2, stringa2);
  switch( Tipo ){
     case 0:
        Write_Line(ptrfile2, "|  DATA   |   ORA   | TGV | POS |        MESSAGGIO                             |");   // Lista legata al TGV e alla sua posizione
        break;
     case 1:
        Write_Line(ptrfile2, "|  DATA   |   ORA   | MAC | POS |        MESSAGGIO                             |");   // Macchina + Posizione
        break;
     case 2:
        Write_Line(ptrfile2, "|  DATE   |  TIME  | WINDOW   |        MESSAGE                                 |");   // Log operatore
        break;
     case 3:
        Write_Line(ptrfile2, "|  DATA   |   ORA   | TGV | POS |        MESSAGGIO                             |");   // Lista legata al TGV e alla sua posizione
        break;
     case 4:
        Write_Line(ptrfile2, "|  DATA   |   ORA   | TGV | POS |        MESSAGGIO                             |");   // Lista legata al TGV e alla sua posizione
        break;
     default:
        break;
  }

  Write_Line(ptrfile2, stringa2);
  strcpy(stringa2, "                                                                                ");
  Write_Line(ptrfile2, stringa2);
  //------------------------------------------------
  // Copiatura del File Origine nel File di Stampa.
  //------------------------------------------------
  for(i=0;;i++){
     err = !Read_Line(ptrfile1, stringa1, 151);
     stringa1[100]=NULL;
     if(!err) Write_Line(ptrfile2, stringa1);
     else break;
  }

  //------------------------------------------------
  // Chiusura del File originale e del File di Stampa.
  //------------------------------------------------
  fclose( ptrfile1 );
  fclose( ptrfile2 );

  //---------------------------------------
  // Dimensionamento caratteri
  //---------------------------------------
  StampaRichEdit->Font->Size  = 8;
  StampaRichEdit->Font->Color = clBlack;

  //---------------------------------------
  // Carico a monitor del File di Stampa
  //---------------------------------------
  try{
     StampaRichEdit->Lines->LoadFromFile("STAMPA.TMP");
  }
  catch (...){
     sprintf(buffer, MESS[98], fileDAT);
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }

  //---------------------------------------
  // Stampa del file completo
  //---------------------------------------
  try{
//     StampaRichEdit->Print(fileDAT);
  }
  catch (...){
     sprintf(buffer, MESS[98], fileDAT);
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  StampaRichEdit->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::TimerCloseTimer(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::AzzeraTimerMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
  //----------------------------------------------------------------
  // Disabilita/Abilita Timer per riprendere il conteggio del tempo
  //----------------------------------------------------------------
  TimerClose->Enabled = false;
  TimerClose->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::FormClose(TObject *Sender, TCloseAction &Action)
{
  int i;

  for(i=0; i<ComboCount; i++){
     delete FilterCombo[i];
     delete SelFilterCombo[i];
  }
  ComboCount=0;
  //----------------------------------------------
  // Disabilitazione del Timer di chiusura
  //----------------------------------------------
  TimerClose->Enabled=false;
  //---------------------------------------------------
  // Ripristino dei pulsanti modificabili dall'esterno
  //---------------------------------------------------
  btnAzzera->Enabled      = true;
  btnAzzera->Visible      = true;

  btnStampa->Enabled      = true;
  btnStampa->Visible      = true;

  btnRinfresca->Enabled   = true;
  btnRinfresca->Visible   = true;

  btnExportExcel->Enabled = false;
  btnSaveAsCSV->Enabled   = false;

  Visible=false;
  ListView->Items->Clear();
  ListView->Columns->Clear();

  // Ripristina le larghezze iniziali
  Width=oldFormWidth;
  ListView->Width=oldListWidth;

  ListView->Color= (TColor) 0x00F6EADE;
  reverseFile=true;

  OneShot=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::StampaAvanzata()
{
  int i, g;
  bool OkStampa;
  Stampa *Stm;
  //----------------------------------------------------
  // Questa funzione di stampa è valida solo per Tipo>=5
  //----------------------------------------------------
  if(Tipo<5) return;

  Stm = new Stampa();

  //---------------------------------------------------------
  // Compilo la stringha dell'intestazione
  //---------------------------------------------------------
  strcpy(Stm->Intestazione, CaptionFileLista);
  //---------------------------------------------------------
  // IMPOSTAZIONI
  //---------------------------------------------------------
  Stm->Orientation      = 1; // orrizzontale
  Stm->Colonne          = ColNum;
  Stm->SizeIntestazione = 18;
  Stm->SizeIntColonne   = 8;//10;
  //---------------------------------------------------------
  // Compilo l'Array che contiene le grandezze delle colonne
  // e le stringhe dell'intestazione delle colonne
  //---------------------------------------------------------
  for( g=0; g<Stm->Colonne; g++){
    Stm->LarghezzeCol[g]=ListView->Column[g]->Width * 6.5;    // Larghezza
    strcpy(Stm->IntCol[g], FormListagen->CaptionColumn[g]);   // Intestazione
  }

  OkStampa = PrintDialog->Execute();
  if(OkStampa) {
     Stm->StampaIntestazione();
     for(i=0; i<ListView->Items->Count; i++){
        for( g=0; g<Stm->Colonne; g++){
           //---------------------------------------------------
           // Se l'intestazione della colonna è nulla
           // omette la stampa dei campi di quella colonna
           //---------------------------------------------------
           if(Stm->IntCol[g][0]=='\0') continue;
           if(g==0) {
              strcpy(Stm->Dati[g], ListView->Items->Item[i]->Caption.c_str());
              continue;
           }
           strcpy(Stm->Dati[g], ListView->Items->Item[i]->SubItems->Strings[g-1].c_str());
        }
        Stm->StampaCampi();
     }
     Stm->FineStampa();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::btnExportExcelClick(TObject *Sender)
{
  char all_mess[501]="";
  char buffer[101]="";
  char appoggio[101]="";
  int  i,j,tot;
  int  err=0;
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
  
  if (!DirectoryExists(SaveDialog->InitialDir)){
     MkDir(SaveDialog->InitialDir);
  }
  //----------------------------------------------
  // elimina i caratteri non stampabili
  //----------------------------------------------
  strcpy(appoggio,FormListagen->CaptionFileLista);
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
  strcpy(appoggio,FormListagen->CaptionFileLista);
  if(strlen(appoggio) >= 30 || strlen(appoggio) <= 0) strcpy(appoggio,"Report");
  XLSReport->Sheets->Items[0]->Name=appoggio;

  // TITOLI COLONNE
  for(i=0;i<ColNum;i++){
     txt=ListView->Column[i]->Caption;
     if(txt=="") txt="Not Assigned";
     XLSReport->Sheets->Items[0]->AsString[i][0]=txt;
  }
  // Valore dei campi;
  for(i=0; i<ListView->Items->Count; i++){
     for( j=0; j<ColNum; j++){
        if(j==0){
           txt=ListView->Items->Item[i]->Caption;
           XLSReport->Sheets->Items[0]->AsString[j][i+1]=txt;
        }
        else{
           txt=ListView->Items->Item[i]->SubItems->Strings[j-1];
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
void __fastcall TFormListagen::btnSaveAsCSVClick(TObject *Sender)
{
  char buffer[101]="";
  char appoggio[101]="";
  int  i,j,tot;
  int  err=0;
  bool OkSave=false;
  bool OK=false;
  AnsiString NomeFileDlg="";
  AnsiString NomeFile="PROVA.TMP";
  AnsiString txt="";
  //---------------------------------------------------------
  // SAVE Dialog
  //---------------------------------------------------------
  SaveDialog->Options << ofOverwritePrompt; // Chiede se sovrascrivere
  SaveDialog->Options << ofNoChangeDir;     // non cambia la directory di lavoro

  SaveDialog->DefaultExt="csv";
  SaveDialog->Filter="Csv files (*.csv)|*.csv";
  SaveDialog->InitialDir=GetCurrentDir()+"\\Export"; // Directory di default
  //----------------------------------------------  
  // elimina i caratteri non stampabili
  strcpy(appoggio,FormListagen->CaptionFileLista);
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
  //---------------------------------------------------------
  // Esportazione avvenuta
  //---------------------------------------------------------
  OK=false;
  if(err==0){
     OK=CopyFile(NomeFile.c_str(),NomeFileDlg.c_str(),false);
     // if(OK) OK=DeleteFile(NomeFile);
  }

  if(OK){
     sprintf(buffer," Export OK \n File: %s",NomeFileDlg.c_str());
     MessageDlg(buffer, mtCustom, TMsgDlgButtons() << mbOK, 0);
  }
  else{
     sprintf(buffer," Export Error \n File: %s",NomeFileDlg.c_str());
     MessageDlg(buffer, mtError, TMsgDlgButtons() << mbOK, 0);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::Filter_ON()
{
  int idx=0;
  int filter[21];
  int MaxRead_Line;
  int Num=MAXLIN;
  int cccp=0;
  int i;
  char Intestazione[201];
  AnsiString tmp,tmp1,tmp2;
  AnsiString SelFilterTxt[10];
  AnsiString FilterComboTxt[10];
  //-----------------------------------
  memset (&filter,0,sizeof(filter));

  MaxRead_Line=MAXLIN;
  SelFilterTxt[0]=cbSelFilter->Text;
  filter[0]=cbFilter->ItemIndex;
  FilterComboTxt[0]=cbFilter->Text;    // testi del campo scelta

  for(i=0; i<ComboCount; i++){
     SelFilterTxt[i+1]=SelFilterCombo[i]->Text;   // testi del filtro
     filter[i+1]=FilterCombo[i]->ItemIndex;       // indice degli oggetto attivi nei filtri
     FilterComboTxt[i+1]=FilterCombo[i]->Text;    // testi del campo scelta
  }

  Screen->Cursor=crHourGlass;
  CGauge->Progress=0;
  CGauge->Visible            = true;
  pnlFilter->Enabled         = false; //***
  btnAttivaFiltri->Enabled   = false; //***

  ListView->Items->Clear();
  Refresh();
  ListView->Items->BeginUpdate();

  for(indice=0; indice<=MaxRead_Line; indice++){
     if(!strcmp(ColumnText[0][indice], "")) break;
  }
  Num=indice;

  for(indice=0; indice<=MaxRead_Line; indice++){
     if(!strcmp(ColumnText[0][indice], "")) break;
     CGauge->Progress=indice*100/(Num+1);
     //---------------------------------------------------------------------------
     for(i=0; i<=ComboCount; i++){
        if(SelFilterTxt[i]=="") continue;
        tmp1=ColumnText[filter[i]][indice];
        tmp2=SelFilterTxt[i];
        if(chkOnlyDate->Checked==true){
           tmp=Trim(LeftStr(FilterComboTxt[i],3));
           if(tmp=="Dat" || tmp=="Fecha"){
              tmp1=LeftStr(tmp1,10);
              tmp2=LeftStr(tmp2,10);
           }
        }
        //---------------------------------------------------------------------------
        if(btnAND->Down==true){   // Modalità Filtri AND
           cccp=0;
           if(tmp1 != tmp2){
              cccp=9999;
              break;
           }
        }
        //---------------------------------------------------------------------------
        if(btnAND->Down==false){   // Modalità Filtri OR
           cccp=9999;
           if(tmp1 == tmp2){
              cccp=0;
              break;
           }
        }
     }
     //---------------------------------------------------------------------------
     if(cccp>0) continue; // Con cccp=0 introduce il dato nella List

     ListView->Items->Add(); // 1° Colonna
     ListView->Items->Item[idx]->Caption=ColumnText[0][indice];
     idx++;
  }
  ListView->Items->EndUpdate();
  Screen->Cursor=crDefault;
  CGauge->Visible            = false;
  pnlFilter->Enabled         = true;  //***
  btnAttivaFiltri->Enabled   = true;  //***

  sprintf(Intestazione, "%s  -  Record Count=%d", CaptionFileLista, ListView->Items->Count);
  FormListagen->Caption = Intestazione;
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::ListViewChange(TObject *Sender, TListItem *Item, TItemChange Change)
{
  // if(Tipo>=5) AggiornaFiltro(cbFilter, cbSelFilter);
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::cbFilterChange(TObject *Sender)
{
  AggiornaSelFiltro(cbFilter, cbSelFilter);
  AggiornaFiltro(   cbFilter, cbSelFilter);
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::AggiornaFiltro(TComboBox * Filter, TComboBox * SelFilter)
{
  int col=0;
  int cnt=0;
  AnsiString txt="";

  if(FilterWindow==false ||  btnAttivaFiltri->Enabled==false || btnAttivaFiltri->Visible==false) return;

  col=Filter->ItemIndex;
  // imposto limiti min e max (solo per sicurezza)
  if(col<0)          col=0;
  if(col>(ColNum-1)) col=ColNum-1;

  if(ListView->Selected != NULL){
     if(col>0) txt=ListView->Selected->SubItems->Strings[col-1];
     else      txt=ListView->Selected->Caption;
  }
  else{
     cnt=ListView->Items->Count;
     if(cnt>0){
        if(col>0){
            txt=ListView->Items->Item[0]->SubItems->Strings[col-1];
        }
        else{
           txt=ListView->Items->Item[0]->Caption;
        }
     }
     // else Close();
  }
  SelFilter->Text=txt;
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::ListViewKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  // ------------------------------------------------------------------------
  // Select All nella listview con Control+A
  // ------------------------------------------------------------------------
  if(Shift.Contains(ssCtrl)){
     // Select All nella listview con Control+A
     if(Key=='A' || Key=='a') ListView->SelectAll();
     // Copia nella clipboard i campi selezionati nella listview con Control+C
     if(Key=='C' || Key=='c') CopyClipboard(ListView, CGauge);
  }
}
//---------------------------------------------------------------------------
//------------------------------------------------------------
// Select All nella listview
//------------------------------------------------------------
void __fastcall TFormListagen::menu_SelAllClick(TObject *Sender)
{
  ListView->SelectAll();
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::popSelAllClick(TObject *Sender)
{
  ListView->SelectAll();
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::menu_CopyClick(TObject *Sender)
{
  CopyClipboard(ListView, CGauge);
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::popCopyClick(TObject *Sender)
{
  CopyClipboard(ListView, CGauge);
}
//---------------------------------------------------------------------------
//------------------------------------------------------------
// Copia nella clipboard i campi selezionati nella listview
//------------------------------------------------------------
void __fastcall TFormListagen::CopyClipboard( TListView *myListView, TCGauge *myGauge)
{
  int col=0;
  int tmpColNum;
  bool DataOra=false;
  AnsiString txt="";

  Screen->Cursor=crHourGlass;
  if(myGauge!=NULL) myGauge->Visible=true;

  TItemStates selected = TItemStates() << isSelected;
  TListItem *Item = myListView->Selected;

  tmpColNum=myListView->Columns->Count;

  if(myListView->Column[0]->Caption=="Data" && myListView->Column[1]->Caption=="Ora")  DataOra=true;
  if(myListView->Column[0]->Caption=="Date" && myListView->Column[1]->Caption=="Time") DataOra=true;
  //-----------------------------------------------------------------------
  // La prima riga contiene le intestazioni delle colonne
  // nel caso in cui le prime 2 colonne siano Data e Ora le unisce insieme
  // (non mette il ';' ma solo uno spazio ' ' di separazione)
  //-----------------------------------------------------------------------
  for(col=0;col<tmpColNum;col++){
     if(myListView->Column[col]->Caption=="")  txt=txt+"-;";
     else{
        txt=txt+myListView->Column[col]->Caption;

        if(DataOra==true){
           if(col<(tmpColNum-1) && col>0) txt=txt+';';
           if(col==0) txt=txt+' ';
        }
        else{
           if(col<(tmpColNum-1)) txt=txt+';';
        }
     }
  }
  txt=txt+"\r\n";
  //-----------------------------------------------------------------------
  // Compila i dati di delle colonne recuperando le righe selezionate
  // nel caso in cui le prime 2 colonne siano Data e Ora le unisce insieme
  // (non mette il ';' ma solo uno spazio ' ' di separazione)
  //-----------------------------------------------------------------------
  // per pulire il buffer della tastiera
  EscStatus=GetAsyncKeyState(VK_ESCAPE);
  //------------------------------------
  while (Item){
     if(myGauge!=NULL) myGauge->Progress=Item->Index*100/(myListView->SelCount+1);

     EscStatus=GetAsyncKeyState(VK_ESCAPE);
     if(EscStatus!=0){
        ShowMessage(""); // Per pulire il buffer della tastiera
        MessageDlg("Stopped",  mtError, TMsgDlgButtons() << mbOK, 0);
        myListView->ClearSelection();
        Screen->Cursor=crDefault;
        if(myGauge!=NULL) myGauge->Visible=false;
        return;
     }
     for(col=0;col<tmpColNum;col++){
        if(col>0) txt=txt+Item->SubItems->Strings[col-1];
        else      txt=txt+Item->Caption;

        if(DataOra==true){
           if(col<(tmpColNum-1) && col>0) txt=txt+';';
           if(col==0) txt=txt+' ';
        }
        else{
           if(col<(tmpColNum-1)) txt=txt+';';
        }
     }
     txt=txt+"\r\n";
     Item = myListView->GetNextItem(Item, sdAll, selected);
  }
  myListView->ClearSelection();
  Clipboard()->Clear();
  Clipboard()->SetTextBuf(txt.c_str());

  Screen->Cursor=crDefault;
  if(myGauge!=NULL) myGauge->Visible=false;

}

//---------------------------------------------------------------------------
//-----------------------------------------------------------------------
// Compila i dati della combobox di selezione
//-----------------------------------------------------------------------
void __fastcall TFormListagen::AggiornaSelFiltro(TComboBox * Filter, TComboBox * SelFilter)
{
  int i,j,k,n=0,count;
  char buffer[9999][101];
  char testo[101]="";
  bool trovato;

  if(FilterWindow==false ||  btnAttivaFiltri->Enabled==false || btnAttivaFiltri->Visible==false) return;

  memset(buffer,0,sizeof(buffer));

  // count=ListView->Items->Count;
  count=indice;
  if(count<=0)     count = MAXLIN-1;
  if(count>=MAXLIN) count = MAXLIN-1;

  j=Filter->ItemIndex;
  SelFilter->Clear();

  for(i=0;i<count;i++){
     strcpy(testo,ColumnText[j][i]);
     trovato=false;
     for(k=0;k<i;k++){
        if(strcmp(testo,buffer[k])==0){
           trovato=true;
           break;
        }
     }
     if(trovato==false){
        strcpy(buffer[n],testo);
        n++;
        SelFilter->Items->Add(testo);
     }
  }
  SelFilter->ItemIndex=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::btnAttivaFiltriClick(TObject *Sender)
{
  FilterWindow=!FilterWindow;
  pnlFilter->Left=pnlButtons->Left;
  cbSelFilter->Text="";

  if(FilterWindow==true){
     //FormListagen->Height=520;
     FormListagen->Height +=55;
     btnAttivaFiltri->Glyph=btnGliphUp->Glyph;
     pnlFilter->Enabled=true;
     pnlFilter->Visible=true;
     cbFilterChange(Sender);
  }
  else{
     //FormListagen->Height=465;
     FormListagen->Height -=55;
     btnAttivaFiltri->Glyph=btnGliphDown->Glyph;
     pnlFilter->Enabled=false;
     pnlFilter->Visible=false;

     if(bitON==true){
        bitON=false;
        btnFilterON->Glyph=btnLampOFF->Glyph;
        Filter_ON();
     }

  }
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::menu_ExitClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::SaveDialogFolderChange(TObject *Sender)
{
  AnsiString PathProject;

  PathProject = ExtractFilePath(ParamStr(0));
  SetCurrentDir(PathProject);
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::ListViewCustomDrawItem(TCustomListView *Sender, TListItem *Item, TCustomDrawState State, bool &DefaultDraw)
{
  AlternateRowColor(Sender, Item);
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::ListViewCustomDrawSubItem(TCustomListView *Sender, TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw)
{
  AlternateRowColor(Sender, Item);
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::AlternateRowColor(TCustomListView *Sender, TListItem *Item)
{
  if(Item->Index % 2){
     Sender->Canvas->Font->Color  = clBlack;
     Sender->Canvas->Brush->Color = clWhite;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::btnFilterONClick(TObject *Sender)
{
  bitON=true;
  btnFilterON->Glyph=btnLampON->Glyph;
  Filter_ON();
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::btnFilterOFFClick(TObject *Sender)
{
  char Titolo[201]="";
  //  if(Shape_OFF->Visible==true) return;

  bitON=false;
  btnFilterON->Glyph=btnLampOFF->Glyph;
  // cbSelFilter->Text="";
  // Filter_ON();
  Refresh1();

  sprintf(Titolo, "%s  -  Record Count=%d", CaptionFileLista, ListView->Items->Count);
  if(Tipo>=5 && ListView->Items->Count >= MAXLIN){
     sprintf(Titolo, "%s - MAX RECORD COUNT REACHED (%d)", CaptionFileLista, ListView->Items->Count);
  }
  FormListagen->Caption = Titolo;
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::btnEsciClick(TObject *Sender)
{
  TimerClose->Enabled=false;
  KeyStop=true;
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::ListViewMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if(ListView->SelCount>1) lblSelected->Caption="Last selection " + IntToStr(ListView->SelCount);
  else lblSelected->Caption="";
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::btnFilterAddClick(TObject *Sender)
{
  int offset=20;

  if(ComboCount <= 0 ) ComboCount=0;
  if(ComboCount >= 10 ) return;

  FormListagen->Height += offset;
  pnlFilter->Height    += offset;
  // ---------------< Filter ComboBox >------------------
  FilterCombo[ComboCount]=new TComboBox(this);

  FilterCombo[ComboCount]->Parent=pnlFilter;

  FilterCombo[ComboCount]->Left    = cbFilter->Left;
  FilterCombo[ComboCount]->Top     = cbFilter->Top+offset*ComboCount+offset;
  FilterCombo[ComboCount]->Height  = cbFilter->Height;
  FilterCombo[ComboCount]->Width   = cbFilter->Width;

  FilterCombo[ComboCount]->Items      = cbFilter->Items;
  FilterCombo[ComboCount]->ItemIndex  = 0;

  FilterCombo[ComboCount]->Tag = ComboCount;

  // ---------------< SEL Filter ComboBox >------------------
  SelFilterCombo[ComboCount]=new TComboBox(this);

  SelFilterCombo[ComboCount]->Parent=pnlFilter;

  SelFilterCombo[ComboCount]->Left    = cbSelFilter->Left;
  SelFilterCombo[ComboCount]->Top     = cbSelFilter->Top+offset*ComboCount+offset;
  SelFilterCombo[ComboCount]->Height  = cbSelFilter->Height;
  SelFilterCombo[ComboCount]->Width   = cbSelFilter->Width;
  SelFilterCombo[ComboCount]->Sorted  = true;

  SelFilterCombo[ComboCount]->Items      = cbSelFilter->Items;
  SelFilterCombo[ComboCount]->ItemIndex  = 0;

  SelFilterCombo[ComboCount]->Tag = ComboCount;

  // ----------------------------------------------------
  FilterCombo[ComboCount]->OnChange = cbNewFilterChange;

  AggiornaSelFiltro(FilterCombo[ComboCount], SelFilterCombo[ComboCount]);
  AggiornaFiltro(   FilterCombo[ComboCount], SelFilterCombo[ComboCount]);

  ComboCount++;
}
//---------------------------------------------------------------------------
void __fastcall  TFormListagen::cbNewFilterChange(TObject *Sender)
{
  int indx=(((TComboBox *) Sender)->Tag);

   if(indx<0) indx=0;

  AggiornaSelFiltro(FilterCombo[indx], SelFilterCombo[indx]);
  AggiornaFiltro(   FilterCombo[indx], SelFilterCombo[indx]);
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::btnCloseClick(TObject *Sender)
{
  char lpString[101]="";
  HWND hWnd;
  //------------------------------
  Close();
  // Manda il messaggio di chiusura solo se l'handle della finestra
  // corrisponde effettivamente a quella della form
  hWnd=GetForegroundWindow();
  GetWindowText(hWnd, lpString,100);
  if(strcmp(lpString, Caption.c_str())==0){
     PostMessage(hWnd, WM_CLOSE, 0, 0);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormListagen::btnFilterSubClick(TObject *Sender)
{
  int indx=0;
  int offset=20;

  indx=ComboCount-1;
  if(indx<0) return;
  
  FormListagen->Height -= offset;
  pnlFilter->Height    -= offset;

  delete FilterCombo[indx];
  delete SelFilterCombo[indx];

  Refresh();
  ComboCount--;
}
//---------------------------------------------------------------------------

