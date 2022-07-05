//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <io.h>
#include <mem.h>
#include <dos.h>

#pragma hdrstop

#include "Stampa.h"
#include "Tecnofer.h"
#include "DBSeq.h"
#include "Remoto.h"
#include "Main.h"
#include "Libreria.h"
#include "Variabil.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
Stampa::Stampa()
{
  Prntr = Printer();

  pag=0;
  Orientation = 0;
  Colonne = 10;
  SizeIntestazione = 18;
  SizeIntColonne = 10;
  LarghezzeCol[ 0]=200;
  LarghezzeCol[ 1]=300;
  LarghezzeCol[ 2]=476;
  LarghezzeCol[ 3]=925;
  LarghezzeCol[ 4]=476;
  LarghezzeCol[ 5]=476;
  LarghezzeCol[ 6]=476;
  LarghezzeCol[ 7]=476;
  LarghezzeCol[ 8]=476;
  LarghezzeCol[ 9]=476;

  strcpy(TipoFont, "Arial");
  strcpy(Intestazione, "Intestazione");

  for (int i=0; i<21; i++) {
     strcpy(IntCol[i], "");
     strcpy(Dati[i], "");
  }
  for(int g=0;g<21;g++) {
     Xd[g]=0;
  }
}
//---------------------------------------------------------------------------
void Stampa::StampaIntestazione()
{
  int  i, Y, Y1, X;
  char d[11], o[11];
  char buffer[101]="";
  int  DiffH;

  OraData(o, d);
  //--------------------------------------------------------------------
  // Se Orientation è 0 la stampa è verticale se 1 è orrizzontale
  //--------------------------------------------------------------------
  if (!Orientation) Prntr->Orientation = poPortrait;  // poPortrait=VERTICALE
  else              Prntr->Orientation = poLandscape; // poLandscape=ORRIZZONTALE

  Prntr->BeginDoc();
  //--------------------------------------------------------------------
  // Inizializzazione delle variabili di gestione pagina
  //--------------------------------------------------------------------
  X=Y=Y1=0;
  DiffH = SizeIntestazione*18;
  //--------------------------------------------------------------------
  // Scelta del Font
  //--------------------------------------------------------------------
  Prntr->Canvas->Font->Name = TipoFont;
  //--------------------------------------------------------------------
  // Contorno e intestazione
  //--------------------------------------------------------------------
  Prntr->Canvas->Pen->Width = 7;
  Prntr->Canvas->Rectangle(0, 0, Prntr->PageWidth, Prntr->PageHeight);
  Y=Y+DiffH;
  Prntr->Canvas->MoveTo(0,                Y);
  Prntr->Canvas->LineTo(Prntr->PageWidth, Y);
  Prntr->Canvas->Font->Size = SizeIntestazione;
  Prntr->Canvas->TextOut(400, SizeIntestazione*4, Intestazione);
  Prntr->Canvas->Font->Size = 8;
  sprintf(buffer, "Date: %s - Time: %s", d, o);
  Prntr->Canvas->TextOut(Prntr->PageWidth-1312,SizeIntestazione*2, buffer);
  //--------------------------------------------------------------------
  // intestazione delle caselle di dati
  //--------------------------------------------------------------------
  DiffH = SizeIntColonne*17;
  Y1 = Y;
  Y=Y+DiffH;
  Prntr->Canvas->MoveTo(0,                Y);
  Prntr->Canvas->LineTo(Prntr->PageWidth, Y);
  Prntr->Canvas->Font->Size = SizeIntColonne;
  Xd[0] = Y;
  Xd[1] = 30;
  for(i=0; i<Colonne; i++){
     if(i==0) {
        Prntr->Canvas->TextOut(30, Y1+SizeIntColonne*3, IntCol[0]);
        continue;
     }
     X=X+LarghezzeCol[i-1];
     Xd[i+1] = X;
     Prntr->Canvas->MoveTo(X, Y1);
     Prntr->Canvas->LineTo(X, Y );
     Prntr->Canvas->TextOut(X+30, Y1+SizeIntColonne*3, IntCol[i]);
  }
  return;
}
//---------------------------------------------------------------------------
void Stampa::StampaCampi()
{
  int  Y, i;

  if ( pag ) {
     FineStampa();
     StampaIntestazione();
     pag=0;
  }
  Y = Xd[0];
  //--------------------------------------------------------------------
  // Scrittura dei dati impostati nelle casella della lista
  //--------------------------------------------------------------------
  Prntr->Canvas->Font->Size = 8;
  for (i=0; i<Colonne; i++) {
     Prntr->Canvas->TextOut(Xd[i+1]+30, Y+20, Dati[i]);
  }
  Y = Y+97;
  Xd[0] = Y;
  if( Y>=Prntr->PageHeight - MARGINE_STAMPA_SOTTO )
     pag++;
  return;
}
//---------------------------------------------------------------------------
void Stampa::StampaTotali()
{
  int  Y;
  int  DiffH;

  if ( pag ) {
     FineStampa();
     StampaIntestazione();
     pag=0;
  }

  DiffH = SizeIntestazione*18;
  //--------------------------------------------------------------------
  // Scelta del Font
  //--------------------------------------------------------------------
  Prntr->Canvas->Font->Name = TipoFont;

  //--------------------------------------------------------------------
  // Linea conclusiva alla lista appena compilata
  //--------------------------------------------------------------------
  Y = Prntr->PageHeight-DiffH;
  Prntr->Canvas->MoveTo(0,                Y);
  Prntr->Canvas->LineTo(Prntr->PageWidth, Y);

  //--------------------------------------------------------------------
  // Scrittura Testo
  //--------------------------------------------------------------------
  Prntr->Canvas->Font->Size = SizeIntestazione;
  Prntr->Canvas->TextOut(400, Y+50, Totali);

  return;
}
//---------------------------------------------------------------------------
void Stampa::FineStampa()
{
  Prntr->EndDoc();
  return;
}
//---------------------------------------------------------------------------
void Stampa::StampaGenerica(int Tipo, char fileDAT[15])
{
  FILE *ptrfile1;
  char stringa1[141]="";

  char buffer[101]="";
  int i, err;

  switch( Tipo ){
     case 0: {      // STORICO MISSIONI
        Orientation = 0;        // 0: Verticale
                                // 1: Orrizzontale
        Colonne = 7;            // NUM colonne
        SizeIntestazione = 18;
        SizeIntColonne = 10;
        //--------------------------------------------------------------------
        // Compilo l'Arrey che contiene le grandezze delle colonne
        //--------------------------------------------------------------------
        // Se orientation=0 la somma delle LarghezzeCol deve risultare: 4762
        // Se orientation=1 la somma delle LarghezzeCol deve risultare: 6817
        LarghezzeCol[ 0]=600;
        LarghezzeCol[ 1]=600;
        LarghezzeCol[ 2]=500;
        LarghezzeCol[ 3]=900;
        LarghezzeCol[ 4]=500;
        LarghezzeCol[ 5]=500;
        LarghezzeCol[ 6]=500;
        //--------------------------------------------------------------------
        // Compilo la stringha dell'intestazione
        //--------------------------------------------------------------------
        strcpy(Intestazione, MESS[53]);
        //--------------------------------------------------------------------
        // Compilo le stringhe dell'intestazione delle colonne
        //--------------------------------------------------------------------
        strcpy(IntCol[ 0], MESS[151]);
        strcpy(IntCol[ 1], MESS[152]);
        strcpy(IntCol[ 2], MESS[153]);
        strcpy(IntCol[ 3], MESS[405]);
        strcpy(IntCol[ 4], "Typ");
        strcpy(IntCol[ 5], "Start");
        strcpy(IntCol[ 6], "End");
        break;
     }
     case 1: {      // CRONOLOGICO MACCHINE
        Orientation = 0;        // 0: Verticale
                                // 1: Orrizzontale
        Colonne = 7;            // NUM colonne
        SizeIntestazione = 18;
        SizeIntColonne = 10;
        //--------------------------------------------------------------------
        // Compilo l'Arrey che contiene le grandezze delle colonne
        //--------------------------------------------------------------------
        // Se orientation=0 la somma delle LarghezzeCol deve risultare: 4762
        // Se orientation=1 la somma delle LarghezzeCol deve risultare: 6817
        LarghezzeCol[ 0]=600;
        LarghezzeCol[ 1]=600;
        LarghezzeCol[ 2]=600;
        LarghezzeCol[ 3]=600;
        LarghezzeCol[ 4]=400;
        LarghezzeCol[ 5]=1500;
        LarghezzeCol[ 6]=400;
        //--------------------------------------------------------------------
        // Compilo la stringha dell'intestazione
        //--------------------------------------------------------------------
        strcpy(Intestazione, MESS[69]);
        //--------------------------------------------------------------------
        // Compilo le stringhe dell'intestazione delle colonne
        //--------------------------------------------------------------------
        strcpy(IntCol[ 0], "Call");
        strcpy(IntCol[ 1], "Start");
        strcpy(IntCol[ 2], MESS[522]);
        strcpy(IntCol[ 3], MESS[405]);
        strcpy(IntCol[ 4], MESS[153]);
        strcpy(IntCol[ 5], MESS[294]);
        strcpy(IntCol[ 6], MESS[578]);
        break;
     }
     case 2: {      // GENERICA
        Orientation = 0;        // 0: Verticale
                                // 1: Orrizzontale
        Colonne = 0;            // NUM colonne
        SizeIntestazione = 18;
        SizeIntColonne = 10;
        //--------------------------------------------------------------------
        // Compilo l'Arrey che contiene le grandezze delle colonne
        //--------------------------------------------------------------------
        // Se orientation=0 la somma delle LarghezzeCol deve risultare: 4762
        // Se orientation=1 la somma delle LarghezzeCol deve risultare: 6817
        LarghezzeCol[ 0]=476;
        //--------------------------------------------------------------------
        // Compilo la stringha dell'intestazione
        //--------------------------------------------------------------------
        strcpy(Intestazione, "");
        //--------------------------------------------------------------------
        // Compilo le stringhe dell'intestazione delle colonne
        //--------------------------------------------------------------------
        strcpy(IntCol[ 0], "");
        break;
     }
  }
  //--------------------------------------------------------------------
  // Compilo l'Arrey di Campi da stampare
  //--------------------------------------------------------------------
  //-----------------------------------------
  // Apertura del File originale in Lettura
  //-----------------------------------------
  ptrfile1 = fopen(fileDAT, "rb");
  if(ptrfile1==0){
     sprintf(buffer, MESS[98], fileDAT);
     Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     return;
  }
  //--------------------------------------------------------------------
  // Compilo i campi che devo stampare
  //--------------------------------------------------------------------
  StampaIntestazione();
  for( i=0; ; i++ ) {
     err = !Read_Line(ptrfile1, stringa1, sizeof(stringa1));
     if( err ) break;
     switch( Tipo ){
        case 0: {      // Storico Missioni Tgv
           Copy( stringa1,  1, 10, Dati[0]  );
           Copy( stringa1, 12,  8, Dati[1]  );
           Copy( stringa1, 21,  6, Dati[2]  );
           Copy( stringa1, 28,  8, Dati[3]  );
           Copy( stringa1, 38,  6, Dati[4]  );
           Copy( stringa1, 45,  8, Dati[5]  );
           Copy( stringa1, 54,  5, Dati[6]  );
           break;
        }
        case 1: {      // Visualizza Cronologico Macchine
           Copy( stringa1,  2, 10, Dati[0]  );
           Copy( stringa1, 13, 10, Dati[1]  );
           Copy( stringa1, 24, 10, Dati[2]  );
           Copy( stringa1, 35,  5, Dati[3]  );
           Copy( stringa1, 41,  5, Dati[4]  );
           Copy( stringa1, 47, 30, Dati[5]  );
           Copy( stringa1, 78,  3, Dati[6]  );
           if(Dati[0][2]!='/' && Dati[0][3]!='.'){
              Copy( stringa1,  1, 81, Dati[0]  );
              strcpy(Dati[1], "");
              strcpy(Dati[2], "");
              strcpy(Dati[3], "");
              strcpy(Dati[4], "");
              strcpy(Dati[5], "");
              strcpy(Dati[6], "");
           }
           break;
        }
        case 2:     // GENERICA
           strcpy(Dati[0], "");    break;
     }
     StampaCampi();
  }
  fclose( ptrfile1 );

  FineStampa();

  delete ptrfile1;
  return;
}

