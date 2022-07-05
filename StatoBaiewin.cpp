//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "StatoBaiewin.h"
#include "Listagen.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormStatoBaie *FormStatoBaie;
//---------------------------------------------------------------------------
__fastcall TFormStatoBaie::TFormStatoBaie(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormStatoBaie::FormActivate(TObject *Sender)
{
  TimerRefresh->Enabled=true;
  //------------------
  // Messaggi
  //------------------
  Messaggi();
  //------------------------------------------
  // Larghezza colonne
  //------------------------------------------
  LVChiamBaie->Column[0 ]->Width = 80;
  LVChiamBaie->Column[1 ]->Width = 80;
  LVChiamBaie->Column[2 ]->Width = 150;
  LVChiamBaie->Column[3 ]->Width = 100;
  LVChiamBaie->Column[4 ]->Width = 80;
  LVChiamBaie->Column[5 ]->Width = 80;
  LVChiamBaie->Column[6 ]->Width = 80;
  LVChiamBaie->Column[7 ]->Width = 100;
  LVChiamBaie->Column[8 ]->Width = 100;
  LVChiamBaie->Column[9 ]->Width = 80;
  LVChiamBaie->Column[10]->Width = 80;
  LVChiamBaie->Column[11]->Width = 80;
  //------------------------------------------
  // Ripulisco la tabella
  //------------------------------------------
  LVChiamBaie->Items->Clear();
  //--------------------------------
  // Refresh
  //--------------------------------
  Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TFormStatoBaie::Messaggi(){
  FormStatoBaie->Caption  = MESS[328];
  lblTitolo->Caption      = MESS[328];

  LVChiamBaie->Column[ 0]->Caption = MESS[524];
  LVChiamBaie->Column[ 1]->Caption = MESS[523];
  LVChiamBaie->Column[ 2]->Caption = MESS[720];
  LVChiamBaie->Column[ 3]->Caption = MESS[684];
  LVChiamBaie->Column[ 4]->Caption = MESS[662];
  LVChiamBaie->Column[ 5]->Caption = MESS[497];
  LVChiamBaie->Column[ 6]->Caption = MESS[705];
  LVChiamBaie->Column[ 7]->Caption = MESS[474];
  LVChiamBaie->Column[ 8]->Caption = MESS[707];
  LVChiamBaie->Column[ 9]->Caption = MESS[459];
  LVChiamBaie->Column[10]->Caption = MESS[461];
  LVChiamBaie->Column[11]->Caption = MESS[708];
}
//---------------------------------------------------------------------------
void __fastcall TFormStatoBaie::Refresh(){
  int i=0, indice=0;
  char buffer[101]="";
  char Date[11]="";
  char Time[11]="";
  //------------------------------------------
  // Ripulisco la tabella
  //------------------------------------------
  LVChiamBaie->Items->Clear();
  //-----------------------------------------------------------
  // Scorro struttura delle chiamate e compilo la tabella
  //-----------------------------------------------------------
  indice=0;
  for(i=0;i<TOTBAIE;i++){
     if(P.ba[i]->Num<=0)        continue;
     if(P.ba[i]->Esclusione)    continue;
     if(!P.ba[i]->Chiamata)     continue;
     LVChiamBaie->Items->Add();
     //---------------------------------------------
     // Prima Colonna = Numero Baia
     //---------------------------------------------
     sprintf(buffer, "%d",P.ba[i]->Num);
     LVChiamBaie->Items->Item[indice]->Caption = buffer;      // 1° Colonna
     //---------------------------------------------
     // Seconda Colonna = Sigla
     //---------------------------------------------
     sprintf(buffer, "%s", P.ba[i]->Sigla);
     LVChiamBaie->Items->Item[indice]->SubItems->Add(buffer); // 2° Colonna
     //---------------------------------------------
     // Terza Colonna =  DataOra
     //---------------------------------------------
     timeint_to_timechar(&(P.ba[i]->TimeChiamata), &Date[0], &Time[0]);
     sprintf(buffer, "%s - %s", Date, Time);
     LVChiamBaie->Items->Item[indice]->SubItems->Add(buffer); // 3° Colonna
     //---------------------------------------------
     // Quarta Colonna = Tipo Baia
     //---------------------------------------------
     if(P.ba[i]->Tipo==0)    sprintf(buffer, MESS[466]);
     if(P.ba[i]->Tipo==1)    sprintf(buffer, MESS[467]);
     if(P.ba[i]->Tipo==2)    sprintf(buffer, MESS[468]);
     LVChiamBaie->Items->Item[indice]->SubItems->Add(buffer); // 4° Colonna
     //---------------------------------------------
     // Quinta Colonna = Priorità
     //---------------------------------------------
     sprintf(buffer, "%d", P.ba[i]->Priorita);
     LVChiamBaie->Items->Item[indice]->SubItems->Add(buffer); // 5° Colonna
     //---------------------------------------------
     // Sesta Colonna = Automatico
     //---------------------------------------------
     if(P.ba[i]->Automatico)    sprintf(buffer, "AUTO");
     else                       sprintf(buffer, "MAN");
     LVChiamBaie->Items->Item[indice]->SubItems->Add(buffer); // 6° Colonna
     //---------------------------------------------
     // Settima Colonna = Blocco
     //---------------------------------------------
     if(P.ba[i]->Blocco)        sprintf(buffer, "BLOCCO");
     else                       sprintf(buffer, "NO");
     LVChiamBaie->Items->Item[indice]->SubItems->Add(buffer); // 7° Colonna
     //---------------------------------------------
     // Ottava Colonna = Stato
     //---------------------------------------------
     if(P.ba[i]->TipoCod[0]==0)    sprintf(buffer, MESS[518]);
     if(P.ba[i]->TipoCod[0]==1)    sprintf(buffer, MESS[529], ASS_COD1);
     if(P.ba[i]->TipoCod[0]==2)    sprintf(buffer, MESS[529], ASS_COD2);
     if(P.ba[i]->TipoCod[0]==3)    sprintf(buffer, MESS[529], ASS_COD3);
     if(P.ba[i]->TipoCod[0]==4)    sprintf(buffer, MESS[529], ASS_COD4);
     if(P.ba[i]->TipoCod[0]==5)    sprintf(buffer, MESS[529], ASS_COD5);
     if(P.ba[i]->TipoCod[0]==6)    sprintf(buffer, MESS[529], ASS_COD6);
     if(P.ba[i]->TipoCod[0]==7)    sprintf(buffer, MESS[529], ASS_COD7);
     if(P.ba[i]->TipoCod[0]==8)    sprintf(buffer, MESS[529], ASS_COD8);
     if(P.ba[i]->TipoCod[0]==9)    sprintf(buffer, MESS[530]);
     LVChiamBaie->Items->Item[indice]->SubItems->Add(buffer); // 8° Colonna
     //---------------------------------------------
     // Nona Colonna = Pallet
     //---------------------------------------------
     sprintf(buffer, "%s", M.TipiBox[P.ba[i]->TipoBox[0]].Nome);
     LVChiamBaie->Items->Item[indice]->SubItems->Add(buffer); // 9° Colonna
     //---------------------------------------------
     // Decima Colonna = Codice
     //---------------------------------------------
     sprintf(buffer, "%d", P.ba[i]->Codice[0]);
     LVChiamBaie->Items->Item[indice]->SubItems->Add(buffer); // 10° Colonna
     //---------------------------------------------
     // Undicesima Colonna = Alezza
     //---------------------------------------------
     sprintf(buffer, "%d", P.ba[i]->Altezza[0]);
     LVChiamBaie->Items->Item[indice]->SubItems->Add(buffer); // 11° Colonna
     //---------------------------------------------
     // Dodicesima Colonna = CarWash
     //---------------------------------------------
     if(P.ba[i]->CarWash[0])        sprintf(buffer, "YES");
     else                           sprintf(buffer, "NO");
     LVChiamBaie->Items->Item[indice]->SubItems->Add(buffer); // 12° Colonna
     indice++;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStatoBaie::BtnStampaClick(TObject *Sender)
{
bool OkStampa;
  int  i, j, Y1, Y, X;
  char d[11], o[11];
  char buffer[101]="";
  char Dati[12][51];
  int  Xd[12];
  int  fine, DiffH, DiffW;
  div_t tot_pag;
  TPrinter *Prntr = Printer();

  OraData(o, d);
  //--------------------------------------------------------------------
  // La stampa è verticale
  //--------------------------------------------------------------------
  Prntr->Orientation = poPortrait;
  //--------------------------------------------------------------------
  // Selezione stampante
  //--------------------------------------------------------------------
  OkStampa = PrintDialog->Execute();
  if(OkStampa==false) return;
  //--------------------------------------------------------------------
  // Calcolo delle pagine da stampare
  //--------------------------------------------------------------------
  tot_pag = div(LVChiamBaie->Items->Count, 63);
  if(tot_pag.rem) tot_pag.quot++;
  j=0;
  Prntr->BeginDoc();
  do{
     //--------------------------------------------------------------------
     // Inizializzazione delle variabili di gestione pagina
     //--------------------------------------------------------------------
     X=Y=Y1=0;
     DiffH = Prntr->PageHeight/20;
     DiffW = Prntr->PageWidth/10;
     strcpy(Dati[ 0], "Num");
     strcpy(Dati[ 1], "Baia");
     strcpy(Dati[ 2], "Data/Ora");
     strcpy(Dati[ 3], "Tipo");
     strcpy(Dati[ 4], "Prior");
     strcpy(Dati[ 5], "Auto");
     strcpy(Dati[ 6], "Blocco");
     strcpy(Dati[ 7], "Stato");
     strcpy(Dati[ 8], "Pallet");
     strcpy(Dati[ 9], "Codice");
     strcpy(Dati[10], "Altezza");
     strcpy(Dati[11], "CarWash");
     //--------------------------------------------------------------------
     // Scelta del Font
     //--------------------------------------------------------------------
     Prntr->Canvas->Font->Name = "Arial";
     Prntr->Canvas->Font->Size = 18;
     //--------------------------------------------------------------------
     // Contorno e intestazione
     //--------------------------------------------------------------------
     Prntr->Canvas->Pen->Width = 7;
     Prntr->Canvas->Rectangle(0, 0, Prntr->PageWidth, Prntr->PageHeight);
     Y=Y+DiffH;
     Prntr->Canvas->MoveTo(0,                Y);
     Prntr->Canvas->LineTo(Prntr->PageWidth, Y);
     Prntr->Canvas->TextOut(400, 75, "BAIE LIST");
     Prntr->Canvas->Font->Size = 8;
     sprintf(buffer, "Date: %s - Time: %s", d, o);
     Prntr->Canvas->TextOut(3450, 150, buffer);
     //--------------------------------------------------------------------
     // intestazione delle caselle di dati
     //--------------------------------------------------------------------
     DiffH = Prntr->PageHeight/40;
     Y1 = Y;
   //X=X+DiffW;
     Y=Y+DiffH;
     Prntr->Canvas->MoveTo(0,                Y);
     Prntr->Canvas->LineTo(Prntr->PageWidth, Y);
     Prntr->Canvas->MoveTo(0,                Y);
     Prntr->Canvas->LineTo(Prntr->PageWidth, Y);
     Prntr->Canvas->Font->Size = 10;
     Prntr->Canvas->TextOut(30, Y1+35, Dati[0]);
     Xd[0] = 30;
     for(i=0; i<11; i++){
        switch(i){
           case  0: X=X+250;   break;
           case  1: X=X+300;   break;
           case  2: X=X+700;   break;
           case  3: X=X+400;   break;
           case  4: X=X+250;   break;
           case  5: X=X+300;   break;
           case  6: X=X+300;   break;
           case  7: X=X+400;   break;
           case  8: X=X+400;   break;
           case  9: X=X+700;   break;
           case 10: X=X+300;   break;
           case 11: X=X+DiffW; break;
        }
        Xd[i+1] = X;
        Prntr->Canvas->MoveTo(X, Y1);
        Prntr->Canvas->LineTo(X, Y );
        Prntr->Canvas->TextOut(X+30, Y1+35, Dati[i+1]);
      //X=X+DiffW;
     }
     //--------------------------------------------------------------------
     // Scrittura dei dati impostati nelle casella della lista
     //--------------------------------------------------------------------
     DiffH = Prntr->PageHeight/70;
     Prntr->Canvas->Font->Size = 8;
     fine = ((j+1)*64);
     if(fine>LVChiamBaie->Items->Count) fine = LVChiamBaie->Items->Count;
     for(i=(64*j); i<fine; i++){
        strcpy(Dati[ 0], LVChiamBaie->Items->Item[i]->Caption.c_str());
        strcpy(Dati[ 1], LVChiamBaie->Items->Item[i]->SubItems->Strings[ 0].c_str());
        strcpy(Dati[ 2], LVChiamBaie->Items->Item[i]->SubItems->Strings[ 1].c_str());
        strcpy(Dati[ 3], LVChiamBaie->Items->Item[i]->SubItems->Strings[ 2].c_str());
        strcpy(Dati[ 4], LVChiamBaie->Items->Item[i]->SubItems->Strings[ 3].c_str());
        strcpy(Dati[ 5], LVChiamBaie->Items->Item[i]->SubItems->Strings[ 4].c_str());
        strcpy(Dati[ 6], LVChiamBaie->Items->Item[i]->SubItems->Strings[ 5].c_str());
        strcpy(Dati[ 7], LVChiamBaie->Items->Item[i]->SubItems->Strings[ 6].c_str());
        strcpy(Dati[ 8], LVChiamBaie->Items->Item[i]->SubItems->Strings[ 7].c_str());
        strcpy(Dati[ 9], LVChiamBaie->Items->Item[i]->SubItems->Strings[ 8].c_str());
        strcpy(Dati[10], LVChiamBaie->Items->Item[i]->SubItems->Strings[ 9].c_str());
        strcpy(Dati[11], LVChiamBaie->Items->Item[i]->SubItems->Strings[ 10].c_str());
        Prntr->Canvas->TextOut(Xd[ 0]+30, Y+20, Dati[ 0]);
        Prntr->Canvas->TextOut(Xd[ 1]+30, Y+20, Dati[ 1]);
        Prntr->Canvas->TextOut(Xd[ 2]+30, Y+20, Dati[ 2]);
        Prntr->Canvas->TextOut(Xd[ 3]+30, Y+20, Dati[ 3]);
        Prntr->Canvas->TextOut(Xd[ 4]+30, Y+20, Dati[ 4]);
        Prntr->Canvas->TextOut(Xd[ 5]+30, Y+20, Dati[ 5]);
        Prntr->Canvas->TextOut(Xd[ 6]+30, Y+20, Dati[ 6]);
        Prntr->Canvas->TextOut(Xd[ 7]+30, Y+20, Dati[ 7]);
        Prntr->Canvas->TextOut(Xd[ 8]+30, Y+20, Dati[ 8]);
        Prntr->Canvas->TextOut(Xd[ 9]+30, Y+20, Dati[ 9]);
        Prntr->Canvas->TextOut(Xd[10]+30, Y+20, Dati[10]);
        Prntr->Canvas->TextOut(Xd[11]+30, Y+20, Dati[11]);
        Y=Y+DiffH;
     }
     j++;
     if(j<tot_pag.quot) Prntr->NewPage();
  }while(j<tot_pag.quot);
  Prntr->EndDoc();
  return;  
}
//---------------------------------------------------------------------------

void __fastcall TFormStatoBaie::LVChiamBaieColumnClick(TObject *Sender,
      TListColumn *Column)
{
  // --- recupera il num.della colonna cliccata ---
  Colonna=Column->Index;

  // --- richiama la funzione di ordinamento, NB il criterio di comparazione ---
  // --- viene impostato nella funzione ListViewCompare richiamata sempre da ---
  // --- AlphaSort;                                                        ---
  // --- flip-flop per criterio di ordinamento ---
  Up[Colonna]=!Up[Colonna];
  LVChiamBaie->AlphaSort();  
}
//---------------------------------------------------------------------------

void __fastcall TFormStatoBaie::LVChiamBaieCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
  switch (Colonna){
     case 0:
        // --- criterio di comparazione per la 1° colonna ---
        Compare = atoi(Item1->Caption.c_str()) - atoi(Item2->Caption.c_str());
        if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
        break;
     case 1:
        // --- criterio di comparazione per la 2° colonna ---
        Compare = Item1->SubItems->Strings[0].AnsiCompare(Item2->SubItems->Strings[0]);
        if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
        break;
     case 2:
        // --- criterio di comparazione per la 2° colonna ---
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
        Compare = atoi(Item1->SubItems->Strings[3].c_str()) - atoi(Item2->SubItems->Strings[3].c_str());
        if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
        break;
     case 5:
        // --- criterio di comparazione per la 5° colonna ---
        Compare = Item1->SubItems->Strings[4].AnsiCompare(Item2->SubItems->Strings[4]);
        if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
        break;
     case 6:
        // --- criterio di comparazione per la 5° colonna ---
        Compare = Item1->SubItems->Strings[5].AnsiCompare(Item2->SubItems->Strings[5]);
        if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
        break;
     case 7:
        // --- criterio di comparazione per la 5° colonna ---
        Compare = Item1->SubItems->Strings[6].AnsiCompare(Item2->SubItems->Strings[6]);
        if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
        break;
     case 8:
        // --- criterio di comparazione per la 5° colonna ---
        Compare = Item1->SubItems->Strings[7].AnsiCompare(Item2->SubItems->Strings[7]);
        if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
        break;
     case 9:
        // --- criterio di comparazione per la 5° colonna ---
        Compare = Item1->SubItems->Strings[8].AnsiCompare(Item2->SubItems->Strings[8]);
        if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
        break;
     case 10:
        // --- criterio di comparazione per la 5° colonna ---
        Compare = atoi(Item1->SubItems->Strings[9].c_str()) - atoi(Item2->SubItems->Strings[9].c_str());
        if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
        break;
     case 11:
        // --- criterio di comparazione per la 5° colonna ---
        Compare = Item1->SubItems->Strings[10].AnsiCompare(Item2->SubItems->Strings[10]);
        if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
        break;
     default: break;
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormStatoBaie::TimerRefreshTimer(TObject *Sender)
{
  TimerRefresh->Enabled=false;
  //----------------------------------------------
  // Aggiorno la tabella con le chiamate
  //----------------------------------------------
  Refresh();
  // --- richiama la funzione di ordinamento, NB il criterio di comparazione ---
  // --- viene impostato nella funzione ListViewCompare richiamata sempre da ---
  // --- AlphaSort;
  // --- flip-flop per criterio di ordinamento ---
  Up[Colonna]=Up[Colonna];                                                      
  LVChiamBaie->AlphaSort();
  // --- flip-flop per criterio di ordinamento ---
  Up[Colonna]=Up[Colonna];
  TimerRefresh->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormStatoBaie::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  TimerRefresh->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormStatoBaie::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  if( Shift.Contains(ssAlt) && Key==VK_F4 ) return;
  switch (Key) {
     case VK_ESCAPE: Close();    break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStatoBaie::LVChiamBaieCustomDrawItem(TCustomListView *Sender, TListItem *Item, TCustomDrawState State, bool &DefaultDraw)
{
  FormListagen->AlternateRowColor(Sender, Item);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatoBaie::LVChiamBaieCustomDrawSubItem(TCustomListView *Sender, TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw)
{
  FormListagen->AlternateRowColor(Sender, Item);
}
//---------------------------------------------------------------------------

