//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ListaCar.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormListaCar *FormListaCar;
//---------------------------------------------------------------------------
__fastcall TFormListaCar::TFormListaCar(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormListaCar::FormClose(TObject *Sender, TCloseAction &Action)
{
  //-----------------------------------------
  // Sbianco le liste
  //-----------------------------------------
  ListView->Items->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormListaCar::FormActivate(TObject *Sender)
{
/*
  int  i, j, g, h, m;
  bool trovato;
  char time1[11]="";
  char data1[11]="";
  char aa[5]="";
  char mm[3]="";
  char gg[3]="";
  char buffer[101]="";
  short int num_mac, num_mag, num_stz;

  indice=0;
  //-----------------------------------------
  // Azzeramento lista tipi materiale
  // nell'impianto
  //-----------------------------------------
  memset(&TypM, 0, sizeof(TypM));
  //-----------------------------------------
  // Messaggeria Form
  //-----------------------------------------
  FormListaCar->Caption          = "Box's list";
  Label2->Caption                = MESS[466];
  ListView->Column[0]->Caption   = MESS[459];
  ListView->Column[1]->Caption   = "";
  ListView->Column[2]->Caption   = MESS[465];
  ListView->Column[3]->Caption   = MESS[466];
  ListView->Column[4]->Caption   = MESS[467];
  ListView->Column[5]->Caption   = "Time (1)";
  ListView->Column[6]->Caption   = MESS[682];
  ListView->Column[7]->Caption   = "Over time";
  ListView->Column[8]->Caption   = "Time (2)";
  ListView->Column[9]->Caption   = "Position";
  //-----------------------------------------
  // Inizializzazione
  //-----------------------------------------
  memset(&Stringa ,  0, sizeof(Stringa));
  memset(&Stringa1,  0, sizeof(Stringa1));
  memset(&Stringa2,  0, sizeof(Stringa2));
  memset(&Stringa3,  0, sizeof(Stringa3));
  memset(&Stringa4,  0, sizeof(Stringa4));
  memset(&Stringa5,  0, sizeof(Stringa5));
  memset(&Stringa6,  0, sizeof(Stringa6));
  memset(&Stringa7,  0, sizeof(Stringa7));
  memset(&Stringa8,  0, sizeof(Stringa8));
  memset(&Stringa9,  0, sizeof(Stringa9));
  //-----------------------------------------
  // Compilazione della lista dei carrelli
  //-----------------------------------------
  for(i=1; i<=MAXBOXIMPIANTO; i++){
     if(B.b[i]->ID<1              ) continue;
     if(B.b[i]->ID>MAXBOXIMPIANTO ) continue;
     if(B.b[i]->Pos<1             ) continue;
     if(B.b[i]->Pos>TOTPUNTI      ) continue;
     //---------------------------------------
     // Visualizzazione dati
     //---------------------------------------
     sprintf(Stringa[indice], "%03d", B.b[i]->ID);
     //---------------------------------------
     // Stato lavorazione
     //---------------------------------------
     switch(B.b[i]->StatoLav){
        case  ST_BOX_VUOTO:  strcpy(Stringa1[indice], MESS[462]);
                             break;
        case  ST_BOX_CRUDO:  strcpy(Stringa1[indice], MESS[463]);
                             break;
        case  ST_BOX_ESSICC: strcpy(Stringa1[indice], MESS[464]);
                             break;
        default:             strcpy(Stringa1[indice], "UNKNOWN");
                             break;
     }
     //---------------------------------------
     // Dati materiale
     //---------------------------------------
     sprintf(Stringa2[indice], "%04d", B.b[i]->IdEssiccazione);
     sprintf(Stringa3[indice], "%04d", B.b[i]->TipoMateriale);
     switch(B.b[i]->StatoBox){
        case  0:  strcpy(Stringa4[indice], "00pz.");
                  break;
        case  4:  strcpy(Stringa4[indice], "08pz.");
                  break;
        case  5:  strcpy(Stringa4[indice], "12pz.");
                  break;
        default:  strcpy(Stringa4[indice], "??pz.");
                  break;
     }
     //---------------------------------------
     // Elenco dei TIPI MATERIALE in impianto
     //---------------------------------------
     trovato=false;
     for(j=0; j<1000; j++){
        if(TypM[j]==0                    ) break;
        if(TypM[j]!=B.b[i]->TipoMateriale) continue;
        trovato=true;
        break;
     }
     if(!trovato){TypM[j]=B.b[i]->TipoMateriale;}
     //---------------------------------------
     // Data/Ora di CARICO
     //---------------------------------------
     timeint_to_timechar(&B.b[i]->TimeCR, &data1[0], &time1[0]);
     memcpy(&gg[0], &data1[0], 2);   // giorno
     gg[2]=0;
     memcpy(&mm[0], &data1[3], 2);   // mese
     mm[2]=0;
     memcpy(&aa[0], &data1[6], 4);   // anno
     aa[4]=0;
     sprintf(Stringa5[indice], "%s/%s/%s - %s", aa, mm, gg, time1);
     //---------------------------------------
     // Tempo di permanenza in magazzino
     // + OverTime
     //---------------------------------------
     sprintf(Stringa6[indice], "%d", B.b[i]->TempoInMag);
     // visualizzazione OVER-TIME in formato gg.hh.mm TDateTime
     g = B.b[i]->OverTime/1440;
     h = (B.b[i]->OverTime-(g*1440))/60;
     m = (B.b[i]->OverTime-(g*1440)-(h*60));
     sprintf(Stringa7[indice], "%02dg %02dh %02dm", g, h, m);
     //---------------------------------------
     // Data/Ora di ESSICCAZIONE
     //---------------------------------------
     timeint_to_timechar(&B.b[i]->TimeES, &data1[0], &time1[0]);
     memcpy(&gg[0], &data1[0], 2);   // giorno
     gg[2]=0;
     memcpy(&mm[0], &data1[3], 2);   // mese
     mm[2]=0;
     memcpy(&aa[0], &data1[6], 4);   // anno
     aa[4]=0;
     sprintf(Stringa8[indice], "%s/%s/%s - %s", aa, mm, gg, time1);
     //---------------------------------------
     // POSIZIONE
     //---------------------------------------
     num_mac=0; num_mag=0; num_stz=0;
     strcpy(Stringa9[indice], "UNKNOWN");
     P.test_punto_presente_mac(buffer, B.b[i]->Pos, 0, 0, &num_mac, &num_stz);
     M.test_punto_presente_mag(buffer, B.b[i]->Pos, 0, 0, &num_mag, &num_stz);
     if(num_mac>0 && num_mac<=TOTCHIAMATE) strcpy(Stringa9[indice],  P.ch[num_mac]->Sigla);
     if(num_mag>0 && num_mag<=TOTMAG     ) sprintf(Stringa9[indice], "Store %02d", num_mag);
     //---------------------------------------
     // compilaizione stringhe lista
     //---------------------------------------
     indice++;
  }
  //-------------------------------------------
  // Compilazione dei Combo Box
  //-------------------------------------------
  ComboTypM->Items->Clear();
  ComboTypM->Items->Add("");
  for(i=0; i<1000; i++){
     if(TypM[i]==0) break;
     ComboTypM->Items->Add(TypM[i]);
  }
  //------------------------------------------
  // Visualizzazione dei dati in lista
  //------------------------------------------
  for(indice=0; indice<=MAXBOXIMPIANTO; indice++){
     if(!strcmp(Stringa[indice], "")) break;
     ListView->Items->Add();
     ListView->Items->Item[indice]->Caption=Stringa[indice];
  }
  //-------------------------------------------
  // Totale carrelli
  //-------------------------------------------
  sprintf(buffer, MESS[441], indice);
  TextTot->Caption = buffer;
*/
}
//---------------------------------------------------------------------------
void __fastcall TFormListaCar::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  switch (Key){case VK_ESCAPE: Close(); break;}
}
//---------------------------------------------------------------------------
void __fastcall TFormListaCar::ListViewInsert(TObject *Sender, TListItem *Item)
{
  // --- aggiunge i valori nelle altre colonne ---
  Item->SubItems->Add(Stringa1[indice]); // 2° Colonna
  Item->SubItems->Add(Stringa2[indice]); // 3° Colonna
  Item->SubItems->Add(Stringa3[indice]); // 4° Colonna
  Item->SubItems->Add(Stringa4[indice]); // 5° Colonna
  Item->SubItems->Add(Stringa5[indice]); // 6° Colonna
  Item->SubItems->Add(Stringa6[indice]); // 7° Colonna
  Item->SubItems->Add(Stringa7[indice]); // 8° Colonna
  Item->SubItems->Add(Stringa8[indice]); // 9° Colonna
  Item->SubItems->Add(Stringa9[indice]); // 10° Colonna
}
//---------------------------------------------------------------------------
void __fastcall TFormListaCar::ListViewColumnClick(TObject *Sender, TListColumn *Column)
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
void __fastcall TFormListaCar::ListViewCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
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
     case 7:
        // --- criterio di comparazione per la 8° colonna ---
        Compare = Item1->SubItems->Strings[6].AnsiCompare(Item2->SubItems->Strings[6]);
        if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
        break;
     case 8:
        // --- criterio di comparazione per la 9° colonna ---
        Compare = Item1->SubItems->Strings[7].AnsiCompare(Item2->SubItems->Strings[7]);
        if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
        break;
     case 9:
        // --- criterio di comparazione per la 10° colonna ---
        Compare = Item1->SubItems->Strings[8].AnsiCompare(Item2->SubItems->Strings[8]);
        if (Up[Colonna]) Compare=-Compare;     // inverte l'ordine
        break;
     default: break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormListaCar::BtnEsciClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormListaCar::BtnStampa1Click(TObject *Sender)
{
  bool OkStampa;
  int  i, j, Y1, Y, X;
  char d[11], o[11];
  char buffer[101]="";
  char Dati[11][51];
  int  Xd[11];
  int  fine, DiffH, DiffW;
  div_t tot_pag;
  TPrinter *Prntr = Printer();

  OraData(o, d);
  //--------------------------------------------------------------------
  // Selezione stampante
  //--------------------------------------------------------------------
  OkStampa = PrintDialog1->Execute();
  if(OkStampa==false) return;
  //--------------------------------------------------------------------
  // Data la mole di dati la stampa è orrizzontale
  //--------------------------------------------------------------------
  Prntr->Orientation = poPortrait;
  //--------------------------------------------------------------------
  // Calcolo delle pagine da stampare
  //--------------------------------------------------------------------
  tot_pag = div(ListView->Items->Count, 63);
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
     strcpy(Dati[ 0], "ID");
     strcpy(Dati[ 1], "");
     strcpy(Dati[ 2], "Essic");
     strcpy(Dati[ 3], "Type");
     strcpy(Dati[ 4], "State");
     strcpy(Dati[ 5], "Time (1)");
     strcpy(Dati[ 6], "Pause");
     strcpy(Dati[ 7], "Over time");
     strcpy(Dati[ 8], "Time (2)");
     strcpy(Dati[ 9], "Position");
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
     Prntr->Canvas->TextOut(400, 75, "BOX'S LIST");
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
     for(i=0; i<9; i++){
        switch(i){
           case  0: X=X+200;   break;
           case  1: X=X+700;   break;
           case  2: X=X+300;   break;
           case  3: X=X+300;   break;
           case  4: X=X+300;   break;
           case  5: X=X+750;   break;
           case  6: X=X+300;   break;
           case  7: X=X+600;   break;
           case  8: X=X+750;   break;
           case  9: X=X+DiffW; break;
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
     if(fine>ListView->Items->Count) fine = ListView->Items->Count;
     for(i=(64*j); i<fine; i++){
        strcpy(Dati[ 0], ListView->Items->Item[i]->Caption.c_str());
        strcpy(Dati[ 1], ListView->Items->Item[i]->SubItems->Strings[ 0].c_str());
        strcpy(Dati[ 2], ListView->Items->Item[i]->SubItems->Strings[ 1].c_str());
        strcpy(Dati[ 3], ListView->Items->Item[i]->SubItems->Strings[ 2].c_str());
        strcpy(Dati[ 4], ListView->Items->Item[i]->SubItems->Strings[ 3].c_str());
        strcpy(Dati[ 5], ListView->Items->Item[i]->SubItems->Strings[ 4].c_str());
        strcpy(Dati[ 6], ListView->Items->Item[i]->SubItems->Strings[ 5].c_str());
        strcpy(Dati[ 7], ListView->Items->Item[i]->SubItems->Strings[ 6].c_str());
        strcpy(Dati[ 8], ListView->Items->Item[i]->SubItems->Strings[ 7].c_str());
        strcpy(Dati[ 9], ListView->Items->Item[i]->SubItems->Strings[ 8].c_str());
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
        Y=Y+DiffH;
     }
     j++;
     if(j<tot_pag.quot) Prntr->NewPage();
  }while(j<tot_pag.quot);
  Prntr->EndDoc();
  return;
}
//---------------------------------------------------------------------------


void __fastcall TFormListaCar::BtnRefreshClick(TObject *Sender)
{
/*
  int  i, g, h, m;
  char time1[11]="";
  char data1[11]="";
  char aa[5]="";
  char mm[3]="";
  char gg[3]="";
  char buffer[101]="";
  short int SelTypM;
  short int num_mac, num_mag, num_stz;

  indice=0;
  //-----------------------------------------
  // Inizializzazione
  //-----------------------------------------
  memset(&Stringa ,  0, sizeof(Stringa));
  memset(&Stringa1,  0, sizeof(Stringa1));
  memset(&Stringa2,  0, sizeof(Stringa2));
  memset(&Stringa3,  0, sizeof(Stringa3));
  memset(&Stringa4,  0, sizeof(Stringa4));
  memset(&Stringa5,  0, sizeof(Stringa5));
  memset(&Stringa6,  0, sizeof(Stringa6));
  memset(&Stringa7,  0, sizeof(Stringa7));
  memset(&Stringa8,  0, sizeof(Stringa8));
  memset(&Stringa9,  0, sizeof(Stringa9));
  //-----------------------------------------
  // Recupero variabili di FILTRO
  //-----------------------------------------
  SelTypM = atoi(ComboTypM->Text.c_str());
  //-----------------------------------------
  // Compilazione della lista dei carrelli
  //-----------------------------------------
  for(i=1; i<=MAXBOXIMPIANTO; i++){
     if(B.b[i]->ID<1              ) continue;
     if(B.b[i]->ID>MAXBOXIMPIANTO ) continue;
     if(B.b[i]->Pos<1             ) continue;
     if(B.b[i]->Pos>TOTPUNTI      ) continue;
     //---------------------------------------
     // Scarto tutti i box che non hanno
     // il Tipo di materiale richiesto
     //---------------------------------------
     if(SelTypM!=0 && SelTypM!=B.b[i]->TipoMateriale) continue;
     //---------------------------------------
     // Visualizzazione dati
     //---------------------------------------
     sprintf(Stringa[indice], "%03d", B.b[i]->ID);
     //---------------------------------------
     // Stato lavorazione
     //---------------------------------------
     switch(B.b[i]->StatoLav){
        case  ST_BOX_VUOTO:  strcpy(Stringa1[indice], MESS[462]);
                             break;
        case  ST_BOX_CRUDO:  strcpy(Stringa1[indice], MESS[463]);
                             break;
        case  ST_BOX_ESSICC: strcpy(Stringa1[indice], MESS[464]);
                             break;
        default:             strcpy(Stringa1[indice], "UNKNOWN");
                             break;
     }
     //---------------------------------------
     // Dati materiale
     //---------------------------------------
     sprintf(Stringa2[indice], "%04d", B.b[i]->IdEssiccazione);
     sprintf(Stringa3[indice], "%04d", B.b[i]->TipoMateriale);
     switch(B.b[i]->StatoBox){
        case  0:  strcpy(Stringa4[indice], "00pz.");
                  break;
        case  4:  strcpy(Stringa4[indice], "08pz.");
                  break;
        case  5:  strcpy(Stringa4[indice], "12pz.");
                  break;
        default:  strcpy(Stringa4[indice], "??pz.");
                  break;
     }
     //---------------------------------------
     // Data/Ora di CARICO
     //---------------------------------------
     timeint_to_timechar(&B.b[i]->TimeCR, &data1[0], &time1[0]);
     memcpy(&gg[0], &data1[0], 2);   // giorno
     gg[2]=0;
     memcpy(&mm[0], &data1[3], 2);   // mese
     mm[2]=0;
     memcpy(&aa[0], &data1[6], 4);   // anno
     aa[4]=0;
     sprintf(Stringa5[indice], "%s/%s/%s - %s", aa, mm, gg, time1);
     //---------------------------------------
     // Tempo di permanenza in magazzino
     // + OverTime
     //---------------------------------------
     sprintf(Stringa6[indice], "%d", B.b[i]->TempoInMag);
     // visualizzazione OVER-TIME in formato gg.hh.mm TDateTime
     g = B.b[i]->OverTime/1440;
     h = (B.b[i]->OverTime-(g*1440))/60;
     m = (B.b[i]->OverTime-(g*1440)-(h*60));
     sprintf(Stringa7[indice], "%02dg %02dh %02dm", g, h, m);
     //---------------------------------------
     // Data/Ora di ESSICCAZIONE
     //---------------------------------------
     timeint_to_timechar(&B.b[i]->TimeES, &data1[0], &time1[0]);
     memcpy(&gg[0], &data1[0], 2);   // giorno
     gg[2]=0;
     memcpy(&mm[0], &data1[3], 2);   // mese
     mm[2]=0;
     memcpy(&aa[0], &data1[6], 4);   // anno
     aa[4]=0;
     sprintf(Stringa8[indice], "%s/%s/%s - %s", aa, mm, gg, time1);
     //---------------------------------------
     // POSIZIONE
     //---------------------------------------
     num_mac=0; num_mag=0; num_stz=0;
     strcpy(Stringa9[indice], "UNKNOWN");
     P.test_punto_presente_mac(buffer, B.b[i]->Pos, 0, 0, &num_mac, &num_stz);
     M.test_punto_presente_mag(buffer, B.b[i]->Pos, 0, 0, &num_mag, &num_stz);
     if(num_mac>0 && num_mac<=TOTCHIAMATE) strcpy(Stringa9[indice],  P.ch[num_mac]->Sigla);
     if(num_mag>0 && num_mag<=TOTMAG     ) sprintf(Stringa9[indice], "Store %02d", num_mag);
     //---------------------------------------
     // Incremento indice
     //---------------------------------------
     indice++;
  }

  //------------------------------------------
  // Visualizzazione dei dati in lista
  //------------------------------------------
  ListView->Items->Clear();
  for(indice=0; indice<=MAXBOXIMPIANTO; indice++){
     if(!strcmp(Stringa[indice], "")) break;
     ListView->Items->Add();
     ListView->Items->Item[indice]->Caption=Stringa[indice];
  }
  //-------------------------------------------
  // Totale carrelli
  //-------------------------------------------
  sprintf(buffer, MESS[441], indice);
  TextTot->Caption = buffer;
*/
}
//---------------------------------------------------------------------------




