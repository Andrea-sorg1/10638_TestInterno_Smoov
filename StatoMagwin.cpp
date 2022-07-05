//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "StatoMagwin.h"
#include "Listagen.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormStatoMagaz *FormStatoMagaz;
//---------------------------------------------------------------------------
__fastcall TFormStatoMagaz::TFormStatoMagaz(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormStatoMagaz::FormActivate(TObject *Sender)
{
  TimerRefresh->Enabled=true;
  //------------------
  // Messaggi
  //------------------
  Messaggi();
  //------------------------------------------
  // Larghezza colonne tabella MAgazzini
  //------------------------------------------
  LVMagaz->Column[0 ]->Width = 80;
  LVMagaz->Column[1 ]->Width = 150;
  LVMagaz->Column[2 ]->Width = 100;
  LVMagaz->Column[3 ]->Width = 100;
  LVMagaz->Column[4 ]->Width = 100;
  LVMagaz->Column[5 ]->Width = 80;
  //------------------------------------------
  // Larghezza colonne Tabella codici
  //------------------------------------------
  LVCodici->Column[0 ]->Width = 100;
  LVCodici->Column[1 ]->Width =  80;
  LVCodici->Column[2 ]->Width = 440;
  
  //------------------------------------------
  // Ripulisco le tabelle
  //------------------------------------------
  LVMagaz->Items->Clear();
  LVCodici->Items->Clear();
  //--------------------------------
  // Refresh
  //--------------------------------
  Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TFormStatoMagaz::Messaggi(){
    FormStatoMagaz->Caption     = MESS[278];
    lblTitolo->Caption          = MESS[278];

    LVMagaz->Column[0]->Caption = MESS[524];
    LVMagaz->Column[1]->Caption = MESS[721];
    LVMagaz->Column[2]->Caption = MESS[684];   
    LVMagaz->Column[3]->Caption = MESS[683];
    LVMagaz->Column[4]->Caption = MESS[ 25];
    LVMagaz->Column[5]->Caption = MESS[724];

    LVCodici->Column[0]->Caption = MESS[459];
    LVCodici->Column[1]->Caption = MESS[725];
    LVCodici->Column[2]->Caption = MESS[ 24];
}
//---------------------------------------------------------------------------
void __fastcall TFormStatoMagaz::Refresh(){
  int   i=0, j=0, k=0, x=0, z=0, indice=0, TotBoxMagaz=0, TotBoxPresa=0;
  char  buffer[101]="";
  char  buf_mag[51]="";
  short Codici[MAXCODICI];
  char  MagazziniCodici[MAXCODICI][501];
  bool  MagInserito=false;

  //-------------------------------------------------------
  //Azzero la struttura dei codici presenti nell'impianto
  //-------------------------------------------------------
  memset(Codici,0,sizeof(Codici));
  //-------------------------------------------------------
  //Azzero la struttura dei codici presenti nell'impianto
  //-------------------------------------------------------
  memset(MagazziniCodici,0,sizeof(MagazziniCodici));
  //------------------------------------------
  // Ripulisco la tabella
  //------------------------------------------
  LVMagaz->Items->Clear();
  //-----------------------------------------------------------
  // Scorro tutti i MAGAZZINI per compilare la 1° tabella
  //-----------------------------------------------------------
  indice=0;
//for(i=0;i<TOTMAG;i++){     /§§§
  for(i=1;i<=TOTMAG;i++){
     MagInserito=false;
     if(M.m[i]->Num<1     ) continue;
     if(M.m[i]->Num>TOTMAG ) continue;
     LVMagaz->Items->Add();
     //---------------------------------------------
     // Prima Colonna = Numero Magazzino
     //---------------------------------------------
     sprintf(buffer, "%d", M.m[i]->Num);
     LVMagaz->Items->Item[indice]->Caption = buffer;      // 1° Colonna
     //---------------------------------------------
     // Seconda Colonna = Assegnazione
     //---------------------------------------------
     switch(M.m[i]->Assegnazione){
        case  ASS_DINAMICA:  strcpy(buffer,  MESS[518]);
                             break;
        case  PRES_LAV1:     if(AbbinaCodiceStatoBox==true) sprintf(buffer, MESS[529], ASS_COD1);
                             else                           strcpy (buffer, ASS_LAV1);
                             break;
        case  PRES_LAV2:     if(AbbinaCodiceStatoBox==true) sprintf(buffer, MESS[529], ASS_COD2);
                             else                           strcpy (buffer, ASS_LAV2);
                             break;
        case  PRES_LAV3:     if(AbbinaCodiceStatoBox==true) sprintf(buffer, MESS[529], ASS_COD3);
                             else                           strcpy (buffer, ASS_LAV3);
                             break;
        case  PRES_LAV4:     if(AbbinaCodiceStatoBox==true) sprintf(buffer, MESS[529], ASS_COD4);
                             else                           strcpy (buffer, ASS_LAV4);
                             break;
        case  PRES_LAV5:     if(AbbinaCodiceStatoBox==true) sprintf(buffer, MESS[529], ASS_COD5);
                             else                           strcpy (buffer, ASS_LAV5);
                             break;
        case  PRES_LAV6:     if(AbbinaCodiceStatoBox==true) sprintf(buffer, MESS[529], ASS_COD6);
                             else                           strcpy (buffer, ASS_LAV6);
                             break;
        case  PRES_LAV7:     if(AbbinaCodiceStatoBox==true) sprintf(buffer, MESS[529], ASS_COD7);
                             else                           strcpy (buffer, ASS_LAV7);
                             break;
        case  PRES_LAV8:     if(AbbinaCodiceStatoBox==true) sprintf(buffer, MESS[529], ASS_COD8);
                             else                           strcpy (buffer, ASS_LAV8);
                             break;
        case  PRES_VUOTO:    sprintf(buffer, MESS[530]);
                             break;
     }
     LVMagaz->Items->Item[indice]->SubItems->Add(buffer); // 2° Colonna
     //---------------------------------------------
     // Terza Colonna =  Tipo Magazzino
     //---------------------------------------------
     switch(M.m[i]->Tipo){
        case  0:             strcpy(buffer, MESS[294]);    // Disattivato
                             break;
        case  1:             strcpy(buffer, MESS[295]);    // Pieni
                             break;
        case  2:             strcpy(buffer, MESS[296]);    // Pieni Bassi
                             break;
        case  3:             strcpy(buffer, MESS[297]);    // Pieni Alti
                             break;
        case  4:             strcpy(buffer, MESS[298]);    // Vuoti
                             break;

     }
     LVMagaz->Items->Item[indice]->SubItems->Add(buffer);      // 3° Colonna
     //---------------------------------------------
     // Quarta Colonna = Stato Magazzino
     //---------------------------------------------
     switch(M.m[i]->Stato){
        case  0:             strcpy(buffer, MESS[316]);       // 0 - Libero
                             break;
        case  1:             strcpy(buffer, MESS[317]);       // 1 - Pieni
                             break;
        case  2:             strcpy(buffer, MESS[318]);       // 2 - Vuoti
                             break;
     }
     LVMagaz->Items->Item[indice]->SubItems->Add(buffer);      // 4° Colonna
     //---------------------------------------------
     // Quinta Colonna = Tipo Box
     //---------------------------------------------
     sprintf(buffer, "%s", M.TipiBox[M.m[i]->TipoBox].Nome);
     LVMagaz->Items->Item[indice]->SubItems->Add(buffer); // 5° Colonna
     //---------------------------------------------
     // Sesta Colonna = Totale Box
     //---------------------------------------------
     TotBoxMagaz=0;
     //-------------------------------------------
     // Scorro i posti a terra per ogni magazzino
     //-------------------------------------------
   //for(j=0;j<MAXTERRAMAG;j++){    //§§§
     for(j=1;j<=MAXTERRAMAG;j++){
        //-------------------------------------------
        // Scorro i piani di ogni posto a terra
        //-------------------------------------------
        for(k=0;k<MAXPIANIMAG;k++){
           //------------------------------------------------------
           // Scorro la matrice delle prese per ogni piano dei Box
           //------------------------------------------------------
           TotBoxPresa=0;
           for(x=0;x<POSINPRESA;x++){
              for(z=0;z<PIANIINPRESA;z++){
                 if(M.m[i]->Pos[j].Box[k].Presa[x][z]==true){
                      TotBoxMagaz++;
                      TotBoxPresa++;
                 }
              }
           }
           //------------------------------------------------------
           // Se ci sono dei Box nella posizione del Magazzino
           // ed è la prima volta che incontro il magazzino memorizzo
           // il magazzino nell'elenco dei Mag. associati al Codice
           //------------------------------------------------------
           //if(TotBoxPresa>0 && Codici[M.m[i]->Pos[j].Box[k].StatoLav]==0){
           if(TotBoxPresa>0 && MagInserito==false){
              sprintf(buf_mag, "%d, ", M.m[i]->Num);
              strcat(MagazziniCodici[M.m[i]->Pos[j].Box[k].StatoLav], buf_mag);
              MagInserito=true;
           }
           //---------------------------------------------
           // Incremento il codice del Box
           // del numero di Box presenti nella postazione
           //---------------------------------------------
           Codici[M.m[i]->Pos[j].Box[k].StatoLav]=Codici[M.m[i]->Pos[j].Box[k].StatoLav]+TotBoxPresa++;
        }
     }
     sprintf(buffer, "%d", TotBoxMagaz);
     LVMagaz->Items->Item[indice]->SubItems->Add(buffer); // 6° Colonna
     //----------------------------------
     // Incremento indice righe tabella
     //----------------------------------
     indice++;
  }
  //------------------------------------------
  // Ripulisco la tabella
  //------------------------------------------
  LVCodici->Items->Clear();
  //-----------------------------------------------------------
  // Scorro il vettore dove mi sono memorizzato i codici
  // incontrati per compilare la 2° tabella
  //-----------------------------------------------------------
  indice=0;
  for(i=0;i<MAXCODICI;i++){
     if(Codici[i]<=0)  continue;
     LVCodici->Items->Add();
     //---------------------------------------------
     // Prima Colonna = Codice
     //---------------------------------------------
     switch(i){
        case  0:             sprintf(buffer, MESS[518]);
                             break;
        case  1:             sprintf(buffer, MESS[529], ASS_COD1);
                             break;
        case  2:             sprintf(buffer, MESS[529], ASS_COD2);
                             break;
        case  3:             sprintf(buffer, MESS[529], ASS_COD3);
                             break;
        case  4:             sprintf(buffer, MESS[529], ASS_COD4);
                             break;
        case  5:             sprintf(buffer, MESS[529], ASS_COD5);
                             break;
        case  6:             sprintf(buffer, MESS[529], ASS_COD6);
                             break;
        case  7:             sprintf(buffer, MESS[529], ASS_COD7);
                             break;
        case  8:             sprintf(buffer, MESS[529], ASS_COD8);
                             break;
        case  9:             sprintf(buffer, MESS[530]);
                             break;
     }
     LVCodici->Items->Item[indice]->Caption = buffer;      // 1° Colonna
     //---------------------------------------------
     // Seconda Colonna = Num Box per Codice
     //---------------------------------------------
     sprintf(buffer, "%d", Codici[i]);
     LVCodici->Items->Item[indice]->SubItems->Add(buffer); // 2° Colonna
     //---------------------------------------------
     // Terza Colonna = Magazzini per Codice
     //---------------------------------------------
     sprintf(buffer, "%s", MagazziniCodici[i]);
     LVCodici->Items->Item[indice]->SubItems->Add(buffer); // 2° Colonna
     indice++;
  }
}
//---------------------------------------------------------------------------
// Stampa il contenuto della finestra
//---------------------------------------------------------------------------
void __fastcall TFormStatoMagaz::BtnStampaClick(TObject *Sender)
{
  bool OkStampa;
  int  i, j, Y1, Y, X, inizio;
  char d[11], o[11];
  char buffer[101]="";
  char Dati[11][51];
  int  Xd[11];
  int  fine, DiffH, DiffW;
  int SpaziRimastiInUltimaPagina=0;
  div_t tot_pag;
  TPrinter *Prntr = Printer();

  OraData(o, d);
  //--------------------------------------------------------------------
  // Selezione stampante
  //--------------------------------------------------------------------
  OkStampa = PrintDialog->Execute();
  if(OkStampa==false) return;
  //--------------------------------------------------------------------
  // La stampa è verticale
  //--------------------------------------------------------------------
  Prntr->Orientation = poPortrait;
  //--------------------------------------------------------------------
  // Calcolo delle pagine da stampare
  //--------------------------------------------------------------------
  tot_pag = div(LVMagaz->Items->Count, 63);
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
     strcpy(Dati[ 1], "Assegnazione");
     strcpy(Dati[ 2], "Tipo");
     strcpy(Dati[ 3], "Stato");
     strcpy(Dati[ 4], "Box");
     strcpy(Dati[ 5], "Tot Box");
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
     Prntr->Canvas->TextOut(400, 75, "STORES LIST");
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
     for(i=0; i<5; i++){
        switch(i){
           case  0: X=X+300;   break;
           case  1: X=X+700;   break;
           case  2: X=X+400;   break;
           case  3: X=X+400;   break;
           case  4: X=X+700;   break;
           case  5: X=X+DiffW; break;
           default: X=X+DiffW; break;
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
     if(fine>LVMagaz->Items->Count){
        SpaziRimastiInUltimaPagina=fine-LVMagaz->Items->Count - 8;
        fine = LVMagaz->Items->Count;
     }
     for(i=(64*j); i<fine; i++){
        strcpy(Dati[ 0], LVMagaz->Items->Item[i]->Caption.c_str());
        strcpy(Dati[ 1], LVMagaz->Items->Item[i]->SubItems->Strings[ 0].c_str());
        strcpy(Dati[ 2], LVMagaz->Items->Item[i]->SubItems->Strings[ 1].c_str());
        strcpy(Dati[ 3], LVMagaz->Items->Item[i]->SubItems->Strings[ 2].c_str());
        strcpy(Dati[ 4], LVMagaz->Items->Item[i]->SubItems->Strings[ 3].c_str());
        strcpy(Dati[ 5], LVMagaz->Items->Item[i]->SubItems->Strings[ 4].c_str());
        Prntr->Canvas->TextOut(Xd[ 0]+30, Y+20, Dati[ 0]);
        Prntr->Canvas->TextOut(Xd[ 1]+30, Y+20, Dati[ 1]);
        Prntr->Canvas->TextOut(Xd[ 2]+30, Y+20, Dati[ 2]);
        Prntr->Canvas->TextOut(Xd[ 3]+30, Y+20, Dati[ 3]);
        Prntr->Canvas->TextOut(Xd[ 4]+30, Y+20, Dati[ 4]);
        Prntr->Canvas->TextOut(Xd[ 5]+30, Y+20, Dati[ 5]);
        Y=Y+DiffH;
     }
     j++;
     if(j<tot_pag.quot) Prntr->NewPage();
  }while(j<tot_pag.quot);
  if(LVCodici->Items->Count<SpaziRimastiInUltimaPagina) tot_pag = div(1,1);
  else{
     tot_pag = div(LVCodici->Items->Count - SpaziRimastiInUltimaPagina, 63);
     if(tot_pag.rem) tot_pag.quot++;
  }
  j=0;
  do{
     X=0;
     if(j>0){
       Y=0;
       Prntr->Canvas->Rectangle(0, 0, Prntr->PageWidth, Prntr->PageHeight);
     }
     if(j==0){
        DiffH = Prntr->PageHeight/70;
        DiffW = Prntr->PageWidth/10;
        Y=Y+DiffH;
     }
     Y1=Y;
     Prntr->Canvas->MoveTo(0,                Y);
     Prntr->Canvas->LineTo(Prntr->PageWidth, Y);
     DiffH = Prntr->PageHeight/20;
     DiffW = Prntr->PageWidth/10;
     Y=Y+DiffH;
     Prntr->Canvas->MoveTo(0,                Y);
     Prntr->Canvas->LineTo(Prntr->PageWidth, Y);
     strcpy(Dati[ 0], "Codice");
     strcpy(Dati[ 1], "Tot Box");
     strcpy(Dati[ 2], "Magazzini");
     //--------------------------------------------------------------------
     // Scelta del Font
     //--------------------------------------------------------------------
     Prntr->Canvas->Font->Name = "Arial";
     Prntr->Canvas->Font->Size = 18;
     Prntr->Canvas->TextOut(400, Y1+70, "CODES LIST");
     //--------------------------------------------------------------------
     // intestazione delle caselle di dati
     //--------------------------------------------------------------------
     DiffH = Prntr->PageHeight/40;
     Y1 = Y;
     Y=Y+DiffH;
     Prntr->Canvas->MoveTo(0,                Y);
     Prntr->Canvas->LineTo(Prntr->PageWidth, Y);
     Prntr->Canvas->MoveTo(0,                Y);
     Prntr->Canvas->LineTo(Prntr->PageWidth, Y);
     Prntr->Canvas->Font->Size = 10;
     Prntr->Canvas->TextOut(30, Y1+35, Dati[0]);
     Xd[0] = 30;
     for(i=0; i<2; i++){
        switch(i){
           case  0: X=X+700;   break;
           case  1: X=X+350;   break;
           case  2: X=X+DiffW; break;
           default: X=X+DiffW; break;
        }
        Xd[i+1] = X;
        Prntr->Canvas->MoveTo(X, Y1);
        Prntr->Canvas->LineTo(X, Y );
        Prntr->Canvas->TextOut(X+30, Y1+35, Dati[i+1]);
     }
     //--------------------------------------------------------------------
     // Scrittura dei dati impostati nelle casella della lista
     //--------------------------------------------------------------------
     DiffH = Prntr->PageHeight/70;
     Prntr->Canvas->Font->Size = 8;
     if(j==0)    fine=SpaziRimastiInUltimaPagina;
     else        fine = ((j*64)+SpaziRimastiInUltimaPagina);
     if(fine>LVCodici->Items->Count){
        fine = LVCodici->Items->Count;
     }
     if(j==0) inizio=0;
     else     inizio=SpaziRimastiInUltimaPagina+(64*(j-1));
     for(i=inizio; i<fine; i++){
        strcpy(Dati[ 0], LVCodici->Items->Item[i]->Caption.c_str());
        strcpy(Dati[ 1], LVCodici->Items->Item[i]->SubItems->Strings[ 0].c_str());
        strcpy(Dati[ 2], LVCodici->Items->Item[i]->SubItems->Strings[ 1].c_str());
        Prntr->Canvas->TextOut(Xd[ 0]+30, Y+20, Dati[ 0]);
        Prntr->Canvas->TextOut(Xd[ 1]+30, Y+20, Dati[ 1]);
        Prntr->Canvas->TextOut(Xd[ 2]+30, Y+20, Dati[ 2]);
        Y=Y+DiffH;
     }
     j++;
     if(j<=tot_pag.quot) Prntr->NewPage();
  }while(j<=tot_pag.quot);
  Prntr->EndDoc();
  return;
}
//------------------------------------------------------------------------------
// Criteri di ordinamento per ogni colonna della Tabella dei Magazzini
// Richiamati dalla funzione che gestisce il click sulle colonne della tabella
//------------------------------------------------------------------------------
void __fastcall TFormStatoMagaz::LVMagazCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
  switch (ColonnaMag){
     case 0:
        // --- criterio di comparazione per la 1° colonna ---
        Compare = atoi(Item1->Caption.c_str()) - atoi(Item2->Caption.c_str());
        if (Up[ColonnaMag]) Compare=-Compare;     // inverte l'ordine
        break;
     case 1:
        // --- criterio di comparazione per la 2° colonna ---
        Compare = Item1->SubItems->Strings[0].AnsiCompare(Item2->SubItems->Strings[0]);
        if (Up[ColonnaMag]) Compare=-Compare;     // inverte l'ordine
        break;
     case 2:
        // --- criterio di comparazione per la 3° colonna ---
        Compare = Item1->SubItems->Strings[1].AnsiCompare(Item2->SubItems->Strings[1]);
        if (Up[ColonnaMag]) Compare=-Compare;     // inverte l'ordine
        break;
     case 3:
        // --- criterio di comparazione per la 4° colonna ---
        Compare = Item1->SubItems->Strings[2].AnsiCompare(Item2->SubItems->Strings[2]);
        if (Up[ColonnaMag]) Compare=-Compare;     // inverte l'ordine
        break;
     case 4:
        // --- criterio di comparazione per la 5° colonna ---
        Compare = Item1->SubItems->Strings[3].AnsiCompare(Item2->SubItems->Strings[3]);
        if (Up[ColonnaMag]) Compare=-Compare;     // inverte l'ordine
        break;
     case 5:
        // --- criterio di comparazione per la 5° colonna ---
        Compare = atoi(Item1->SubItems->Strings[4].c_str()) - atoi(Item2->SubItems->Strings[4].c_str());
        if (Up[ColonnaMag]) Compare=-Compare;     // inverte l'ordine
        break;
     default: break;
  }
}
//------------------------------------------------------------------------------
// Criteri di ordinamento per ogni colonna della Tabella dei Codici
// Richiamati dalla funzione che gestisce il click sulle colonne della tabella
//------------------------------------------------------------------------------
void __fastcall TFormStatoMagaz::LVCodiciCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
  switch (ColonnaCod){
     case 0:
        // --- criterio di comparazione per la 1° colonna ---
        Compare = Item1->Caption.AnsiCompare(Item2->Caption.c_str());
        if (Up[ColonnaCod]) Compare=-Compare;     // inverte l'ordine
        break;
     case 1:
        // --- criterio di comparazione per la 2° colonna ---
        Compare =atoi(Item1->SubItems->Strings[0].c_str()) - atoi(Item2->SubItems->Strings[0].c_str());
        if (Up[ColonnaCod]) Compare=-Compare;     // inverte l'ordine
        break;
     case 2:
        // --- criterio di comparazione per la 3° colonna ---
        Compare = Item1->SubItems->Strings[1].AnsiCompare(Item2->SubItems->Strings[1]);
        if (Up[ColonnaCod]) Compare=-Compare;     // inverte l'ordine
        break;
     default: break;
  }
}
//---------------------------------------------------------------------------
// Gestione del Click su Colonna della Tabella dei Magazzini
//---------------------------------------------------------------------------
void __fastcall TFormStatoMagaz::LVMagazColumnClick(TObject *Sender,
      TListColumn *Column)
{
  // --- recupera il num.della colonna cliccata ---
  ColonnaMag=Column->Index;

  // --- richiama la funzione di ordinamento, NB il criterio di comparazione ---
  // --- viene impostato nella funzione ListViewCompare richiamata sempre da ---
  // --- AlphaSort;                                                        ---
  // --- flip-flop per criterio di ordinamento ---
  Up[ColonnaMag]=!Up[ColonnaMag];
  LVMagaz->AlphaSort();
}
//---------------------------------------------------------------------------
// Gestione del Click su Colonna della Tabella dei Codici
//---------------------------------------------------------------------------
void __fastcall TFormStatoMagaz::LVCodiciColumnClick(TObject *Sender,
      TListColumn *Column)
{
  // --- recupera il num.della colonna cliccata ---
  ColonnaCod=Column->Index;

  // --- richiama la funzione di ordinamento, NB il criterio di comparazione ---
  // --- viene impostato nella funzione ListViewCompare richiamata sempre da ---
  // --- AlphaSort;                                                        ---
  // --- flip-flop per criterio di ordinamento ---
  Up[ColonnaCod]=!Up[ColonnaCod];
  LVCodici->AlphaSort();

}
//---------------------------------------------------------------------------------
// Timer: Ogni 10 secondi aggiorno le tabelle e dopo 60 secondi chiudo la finestra
//---------------------------------------------------------------------------------
void __fastcall TFormStatoMagaz::TimerRefreshTimer(TObject *Sender)
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
  Up[ColonnaMag]=Up[ColonnaMag];
  LVMagaz->AlphaSort();
  // --- flip-flop per criterio di ordinamento ---
  Up[ColonnaMag]=Up[ColonnaMag];
  // --- richiama la funzione di ordinamento, NB il criterio di comparazione ---
  // --- viene impostato nella funzione ListViewCompare richiamata sempre da ---
  // --- AlphaSort;
  // --- flip-flop per criterio di ordinamento ---
  Up[ColonnaCod]=Up[ColonnaCod];
  LVCodici->AlphaSort();
  // --- flip-flop per criterio di ordinamento ---
  Up[ColonnaCod]=Up[ColonnaCod];
  TimerRefresh->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormStatoMagaz::FormClose(TObject *Sender, TCloseAction &Action)
{
  TimerRefresh->Enabled=false;  
}
//---------------------------------------------------------------------------
void __fastcall TFormStatoMagaz::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Shift.Contains(ssAlt) && Key==VK_F4 ) return;
  switch (Key) {
     case VK_ESCAPE: Close();    break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStatoMagaz::LVMagazCustomDrawItem(TCustomListView *Sender, TListItem *Item, TCustomDrawState State, bool &DefaultDraw)
{
  FormListagen->AlternateRowColor(Sender, Item);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatoMagaz::LVMagazCustomDrawSubItem(TCustomListView *Sender, TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw)
{
  FormListagen->AlternateRowColor(Sender, Item);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatoMagaz::LVCodiciCustomDrawItem(TCustomListView *Sender, TListItem *Item, TCustomDrawState State, bool &DefaultDraw)
{
  FormListagen->AlternateRowColor(Sender, Item);
}
//---------------------------------------------------------------------------
void __fastcall TFormStatoMagaz::LVCodiciCustomDrawSubItem(TCustomListView *Sender, TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw)
{
  FormListagen->AlternateRowColor(Sender, Item);
}
//---------------------------------------------------------------------------

