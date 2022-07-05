//---------------------------------------------------------------------------
#include <vcl.h>
#include <math.h>

#pragma hdrstop

#include "Tecnofer.h"
#include "DisRiflettori.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormDisRiflettori *FormDisRiflettori;

// --------------------------------
// Per TGV laser (ZONA LASER)
/*
struct LAYER{
  int Xl;                      // X Layer
  int Yl;                      // Y Layer
  int Al;                      // A Layer
  int Rl;                      // Raggio Layer (30 mt circa fisso perchè relativo alla testa)
  int Nr;                      // N° di riflettori trovati
  int Minr;                    // N° Minimo di riflettori
  int Rr;                      // Raggio del riflettore  50 mm (raggio del catarifrangente)
  int Xr[MAXRIFLETTORI+1];     // Coordinata X riflettore
  int Yr[MAXRIFLETTORI+1];     // Coordinata Y riflettore
  int ZAbiltata;               // Zona Abilitata alla navigazione
  int aux2;
};


// --------------------------------
// Per TGV
// Coordinate delle zone di mapping

struct ZONEMAPPING{
  int    X_0_layout;                  // offset X dell'origine del riferimento layout             (OBSOLETO e ININFLUENTE)
  int    Y_0_layout;                  // offset Y dell'origine del riferimento layout             (OBSOLETO e ININFLUENTE)
  int    X_0_laser;                   // offset X dell'origine del riferimento laser              (Da Editor x Zona 0)
  int    Y_0_laser;                   // offset Y dell'origine del riferimento laser              (Da Editor x Zona 0)
  double GRD_laser;                   // Angolo di rotazione Layout del TGV nel punto di mapping  (Da Editor x Zona 0)
  double GRD_offset;                  // Angolo di differenza tra rif. cartesiano e rif. laser    (Forzato a 0 ININFLUENTE)
  int    ScostamentoPuntoLogico;      // Scostamento in mm tra "Punto logico" TGV e testa laser   (Da PLC Dm210)

  int    MaxZoneMappate;              // Zone mappate: Ricalcolo se mapping o abilitata da Editor (perchè mappata con SICK)
  int    aux2;
  int    aux3;
  int    aux4;

  double aux5;
  double aux6;

  struct LAYER Z[MAXZONEMAPPING+1];   // Zona 0...39 per un Totale di 40
};
*/

//---------------------------------------------------------------------------
__fastcall TFormDisRiflettori::TFormDisRiflettori(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormDisRiflettori::FormActivate(TObject *Sender)
{
  char Buffer[300];
  int i, Specchio;

  ZonaModificata=false;
  //-----------------------------------------------
  // Compilazione della LISTA dei nodi selezionati
  //-----------------------------------------------
  CLBNodi->Items->Clear();
  for(i=0; i<NumSpecchiIniziale; i++){
     //if(ElencoSpecchi[i]==0) break;
     if(i>=MAXRIFLETTORI   ) break;
     CLBNodi->Items->Add(ElencoSpecchi[i]);
     CLBNodi->Checked[i]=true;
  }
  //--------------------------------------------
  // Recupero le coordinate riflettore
  //--------------------------------------------
  Specchio=0;
  sprintf( Buffer, "Offeset (mm) Riflettore: %2d",  Specchio);
  GroupBox1->Caption=Buffer;
  EditX->Text = DatiZona.Z[NumZona].Xr[Specchio];
  EditY->Text = DatiZona.Z[NumZona].Yr[Specchio];

  //-----------------------------------------------
  // Messaggeria dei pulsanti
  //-----------------------------------------------
  BtnOk->Caption    = MESS[103];
  BtnEsci->Caption  = MESS[171];
  //-----------------------------------------------
  // Visualizzazione del totale nodi selezionati
  //-----------------------------------------------
  PanelTotRiflettori->Caption = i;
  //-----------------------------------------------
  // Visualizzazione nome della zona nella caption
  //-----------------------------------------------
  sprintf(Buffer, "Disabilita Riflettori Zona %d", NumZona);
  FormDisRiflettori->Caption = Buffer;

}
//---------------------------------------------------------------------------
void __fastcall TFormDisRiflettori::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  switch (Key) { case VK_ESCAPE: Close(); break; }
}
//---------------------------------------------------------------------------
void __fastcall TFormDisRiflettori::BtnOkClick(TObject *Sender)
{
  struct    ZONEMAPPING DatiZonaOld;
  int i, j;

  //memset(&FormDisRiflettori->ElencoSpecchi[0], 0, sizeof(FormDisRiflettori->ElencoSpecchi));
  //------------------------------------------
  // Ricalcolo numero Riflettori
  //------------------------------------------
  NumSpecchiModificato=0;
  for(i=0; i<NumSpecchiIniziale; i++){
     if(ElencoSpecchi[i]!=i) continue;
     NumSpecchiModificato++;
  }

  //------------------------------------------
  // Numero Riflettori modificato
  //------------------------------------------
  if(NumSpecchiModificato<NumSpecchiIniziale){
     memcpy(&DatiZonaOld, &DatiZona, sizeof(DatiZona));
     //---------------------------------
     // Compatta riflettori modificati
     //---------------------------------
     j=0;
     for(i=0; i<NumSpecchiIniziale; i++){
        if(ElencoSpecchi[i]!=i) continue; // Salta quelli eliminati
        DatiZona.Z[NumZona].Xr[j]=DatiZonaOld.Z[NumZona].Xr[i];
        DatiZona.Z[NumZona].Yr[j]=DatiZonaOld.Z[NumZona].Yr[i];
        j++;
     }
     //---------------------------------
     // Azzera riflettori in eccesso
     //---------------------------------
     for(i=j; i<NumSpecchiIniziale; i++){
        DatiZona.Z[NumZona].Xr[i]=0;
        DatiZona.Z[NumZona].Yr[i]=0;
     }
     DatiZona.Z[NumZona].Nr=NumSpecchiModificato;

     ZonaModificata=true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormDisRiflettori::CLBNodiClickCheck(TObject *Sender)
{
  int index, Specchio;

  //-----------------------------------------
  // Recupero lo specchio a cui fa riferimento
  //-----------------------------------------
  index = CLBNodi->ItemIndex;
  Specchio = atoi(CLBNodi->Items->Strings[index].c_str());
  if(CLBNodi->Checked[index]==false) ElencoSpecchi[Specchio]=-1;
  if(CLBNodi->Checked[index]==true ) ElencoSpecchi[Specchio]=(short int) Specchio;

}
//---------------------------------------------------------------------------


void __fastcall TFormDisRiflettori::CLBNodiDblClick(TObject *Sender)
{
  int index, Specchio;
  char Buffer[300];

  index = CLBNodi->ItemIndex;

  //-----------------------------------------
  // Recupero il nodo a cui fa riferimento
  //-----------------------------------------
  Specchio = atoi(CLBNodi->Items->Strings[index].c_str());

  //--------------------------------------------
  // Recupero le coordinate riflettore
  //--------------------------------------------
  sprintf( Buffer, "Offeset (mm) Riflettore: %2d",  Specchio);
  GroupBox1->Caption=Buffer;
  EditX->Text = DatiZona.Z[NumZona].Xr[Specchio];
  EditY->Text = DatiZona.Z[NumZona].Yr[Specchio];

}
//---------------------------------------------------------------------------








